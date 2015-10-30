/*
 * Copyright (C) 2015 Max Plutonium <plutonium.max@gmail.com>
 *
 * This file is part of the EMEL library.
 *
 * The EMEL library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * The EMEL library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the EMEL library. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "plugins.h"

#include <boost/dll/import.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>

#include <set>
#include <shared_mutex>
#include <unordered_map>

namespace emel {

namespace bfs = boost::filesystem;
namespace dll = boost::dll;

using library_ptr = std::shared_ptr<dll::shared_library>;

struct version_compare : std::binary_function<plugin::version, plugin::version, bool>
{
    bool operator()(const plugin::version &lhs, const plugin::version &rhs) const
    {
        if(lhs.major < rhs.major || lhs.minor < rhs.minor || lhs.patch < rhs.patch)
            return true;
        return false;
    }
};

struct lib_entry {
    std::multimap<plugin::version, library_ptr, version_compare> instances;
    std::unordered_multimap<std::string, library_ptr> names_map;
};

static std::shared_timed_mutex g_libs_lock;
static std::unordered_map<std::string, lib_entry> g_libs_map;

static std::vector<std::string> get_paths(const std::string &dir_name)
{
    std::vector<std::string> paths;
    constexpr char sep =
#ifdef __linux__
        ':';
#else
        ';';
#endif

    if(dir_name.empty()) {
        paths.push_back(bfs::current_path().string());

        if(const char *env = std::getenv("LD_LIBRARY_PATH")) {
            const std::string lp(env);
            std::size_t idx = lp.find(sep), prev_idx = 0;
            do {
                if(std::string::npos == idx)
                    idx = lp.size();
                paths.push_back(lp.substr(prev_idx, idx - prev_idx));
                prev_idx = idx + 1;
                if(prev_idx > lp.length())
                    break;
                idx = lp.find(sep, prev_idx);
            } while(true);
        }
    } else
        paths.push_back(bfs::absolute(dir_name).string());

    return paths;
}

static std::vector<bfs::directory_entry> find_libs(const std::string &path)
{
    std::vector<bfs::directory_entry> files;
    bfs::directory_iterator dir_begin(path), dir_end;

    std::copy_if(dir_begin, dir_end, std::back_inserter(files),
        [](const bfs::directory_entry &entry) {
            const auto fname = entry.path().filename().string();
            return ((fname.find("libemel") != std::string::npos)
                && (fname.rfind(dll::shared_library::suffix().string()) != std::string::npos));
        });

    return files;
}

static plugin::version parse_version(const std::string &str)
{
    plugin::version ret;
    std::size_t idx = str.find('.'), prev_idx = 0;
    ret.major = boost::lexical_cast<std::size_t>(str.substr(prev_idx, idx - prev_idx));
    prev_idx = idx + 1; idx = str.find('.', prev_idx);
    ret.minor = boost::lexical_cast<std::size_t>(str.substr(prev_idx, idx - prev_idx));
    prev_idx = idx + 1; idx = str.find('.', prev_idx);
    ret.patch = boost::lexical_cast<std::size_t>(str.substr(prev_idx, idx - prev_idx));
    return ret;
}

plugin::plugin(const char *type_name, const std::string &default_dir)
    : type_name(type_name), default_dir(default_dir)
{
}

std::size_t plugin::load_dir(const std::string &dir_name)
{
    std::size_t ret = 0;
    std::vector<std::string> paths = get_paths(dir_name);

    for(const std::string &path : paths) {
        std::vector<bfs::directory_entry> files = find_libs(path);

        for(const bfs::directory_entry &file : files) {
            library_ptr lib = std::make_shared<dll::shared_library>(file.path());
            if (!lib->has("emel_plugin_type") && !lib->has("emel_plugin_name")
                    && !lib->has("emel_plugin_version") && !lib->has("emel_plugin_instance"))
                // no such symbol
                continue;

            auto type = lib->get_alias<const char *>("emel_plugin_type");
            auto name = lib->get_alias<const char *>("emel_plugin_name");
            auto version = lib->get_alias<const char *>("emel_plugin_version");

            std::unique_lock<decltype(g_libs_lock)> lk(g_libs_lock);
            auto it = g_libs_map.find(type);

            if(g_libs_map.end() == it) {
                lib_entry le;
                le.instances.emplace(parse_version(version), lib);
                le.names_map.emplace(name, std::move(lib));
                g_libs_map.emplace(type, std::move(le));

            } else {
                it->second.instances.emplace(parse_version(version), lib);
                it->second.names_map.emplace(name, std::move(lib));
            }

            ++ret;
        }
    }

    return ret;
}

std::pair<plugin::version, void *>
plugin::load_name(const std::string &name) const
{
    std::shared_lock<decltype(g_libs_lock)> lk(g_libs_lock);

    lib_entry &le = g_libs_map.at(type_name);
    auto it = le.names_map.find(name);

    if(le.names_map.end() == it)
        return std::make_pair<plugin::version>({0, 0, 0}, nullptr);

    auto &lib = it->second;
    auto version = lib->get_alias<const char *>("emel_plugin_version");
    auto instance = lib->get_alias<void *()>("emel_plugin_instance");
    return std::make_pair(parse_version(version), instance());
}

std::size_t plugin::names_count(const std::string &name) const
{
    std::shared_lock<decltype(g_libs_lock)> lk(g_libs_lock);
    return g_libs_map.at(type_name).names_map.count(name);
}

std::vector<std::string> plugin::names() const
{
    std::set<std::string> set;

    {
        std::shared_lock<decltype(g_libs_lock)> lk(g_libs_lock);
        lib_entry &le = g_libs_map.at(type_name);
        std::transform(le.names_map.cbegin(),
                le.names_map.cend(), std::inserter(set, set.begin()),
            [](const std::pair<std::string, library_ptr> &entry) {
                return entry.first;
            });
    }

    std::vector<std::string> ret;
    ret.reserve(set.size());
    std::copy(set.begin(), set.end(), std::back_inserter(ret));
    return ret;
}

std::pair<std::string, void *>
plugin::load_version(plugin::version ver) const
{
    std::shared_lock<decltype(g_libs_lock)> lk(g_libs_lock);

    lib_entry &le = g_libs_map.at(type_name);
    auto it = le.instances.find(ver);
    if(le.instances.end() == it)
        return std::make_pair<std::string>({ }, nullptr);

    auto &lib = it->second;
    auto name = lib->get_alias<const char *>("emel_plugin_name");
    auto instance = lib->get_alias<void *()>("emel_plugin_instance");
    return std::make_pair(name, instance());
}

std::size_t plugin::versions_count(plugin::version ver) const
{
    std::shared_lock<decltype(g_libs_lock)> lk(g_libs_lock);
    return g_libs_map.at(type_name).instances.count(ver);
}

std::vector<plugin::version> plugin::versions() const
{
    std::set<version, version_compare> set;

    {
        std::shared_lock<decltype(g_libs_lock)> lk(g_libs_lock);
        lib_entry &le = g_libs_map.at(type_name);
        std::transform(le.instances.cbegin(),
                le.instances.cend(), std::inserter(set, set.begin()),
            [](const std::pair<version, library_ptr> &entry) {
                return entry.first;
            });
    }

    std::vector<version> ret;
    ret.reserve(set.size());
    std::copy(set.begin(), set.end(), std::back_inserter(ret));
    return ret;
}

typed_plugin<parser> frontend("frontend");

} // namespace emel
