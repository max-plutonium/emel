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

#include <set>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <dlfcn.h>

namespace emel EMEL_EXPORT {

namespace bfs = boost::filesystem;

void plugin::library_deleter::operator()(plugin::library *ptr)
{
    if(ptr && ptr->handle)
        ::dlclose(ptr->handle);
    delete ptr;
}

bool plugin::version_compare::operator()(
        const plugin::version &lhs, const plugin::version &rhs) const
{
    if(lhs.major < rhs.major || lhs.minor < rhs.minor || lhs.patch < rhs.patch)
        return true;
    return false;
}

static std::vector<std::string> get_paths(const std::string &dir_name)
{
    std::vector<std::string> paths;

    if(dir_name.empty()) {
        paths.push_back(bfs::current_path().string());

        std::string lp(std::getenv("LD_LIBRARY_PATH"));

        std::size_t idx = lp.find(':'), prev_idx = 0;
        while(idx != std::string::npos) {
            paths.push_back(lp.substr(prev_idx, idx - prev_idx));
            prev_idx = idx + 1;
            idx = lp.find(':', prev_idx);
        }
    } else
        paths.push_back(bfs::absolute(dir_name).string());

    return paths;
}

static std::vector<bfs::directory_entry> find_libs(
        const std::string &path, const char *ext = ".so")
{
    std::vector<bfs::directory_entry> files;
    bfs::directory_iterator dir_begin(path), dir_end;

    std::copy_if(dir_begin, dir_end, std::back_inserter(files),
        [ext](const bfs::directory_entry &entry) {
            return entry.path().filename().string().rfind(ext)
                != std::string::npos;
        });

    return files;
}

static plugin::library_ptr try_load(const bfs::directory_entry &file,
        const char *instance_sym, const char *name_sym, const char *version_sym)
{
    plugin::library_ptr ret(new plugin::library, plugin::library_deleter());

    ret->handle = ::dlopen(file.path().filename().c_str(), RTLD_LAZY);
    ret->instance_func = reinterpret_cast<decltype(ret->instance_func)>(
        ::dlsym(ret->handle, instance_sym));
    ret->name_func = reinterpret_cast<decltype(ret->name_func)>(
        ::dlsym(ret->handle, name_sym));
    ret->version_func = reinterpret_cast<decltype(ret->version_func)>(
        ::dlsym(ret->handle, version_sym));

    if(!ret->handle || !ret->instance_func
        || !ret->name_func || !ret->version_func)
        ret.reset();

    return ret;
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

plugin::plugin(const char *instance_sym, const char *name_sym,
               const char *version_sym, const std::string &default_dir)
    : instance_sym(instance_sym), name_sym(name_sym)
    , version_sym(version_sym), default_dir(default_dir)
{
}

std::size_t plugin::load_dir(const std::string &dir_name)
{
    std::size_t ret = 0;
    std::vector<std::string> paths = get_paths(dir_name);

    for(const std::string &path : paths) {
        std::vector<bfs::directory_entry> files = find_libs(path);

        for(const bfs::directory_entry &file : files) {
            plugin::library_ptr ptr = try_load(file, instance_sym, name_sym, version_sym);
            if(!ptr)
                continue;
            instances.emplace(parse_version(ptr->version_func()), ptr);
            names_map.emplace(ptr->name_func(), ptr);
            ++ret;
        }
    }

    return ret;
}

std::pair<plugin::version, void *>
plugin::load_name(const std::string &name) const
{
    auto iter = names_map.find(name);
    if(names_map.end() == iter)
        return std::make_pair<plugin::version>({0, 0, 0}, nullptr);

    return std::make_pair(parse_version(iter->second->version_func()),
                          iter->second->instance_func());
}

std::size_t plugin::names_count(const std::string &name) const
{
    return names_map.count(name);
}

std::vector<std::string> plugin::names() const
{
    std::set<std::string> set;

    std::transform(names_map.cbegin(), names_map.cend(),
                   std::inserter(set, set.begin()),
        [](const std::pair<std::string, library_ptr> &entry) {
            return entry.first;
        });

    std::vector<std::string> ret;
    ret.reserve(set.size());
    std::copy(set.begin(), set.end(), std::back_inserter(ret));
    return ret;
}

std::pair<std::string, void *>
plugin::load_version(plugin::version ver) const
{
    auto iter = instances.find(ver);
    if(instances.end() == iter)
        return std::make_pair<std::string>({}, nullptr);

    return std::make_pair(iter->second->name_func(),
                          iter->second->instance_func());
}

std::size_t plugin::versions_count(plugin::version ver) const
{
    return instances.count(ver);
}

std::vector<plugin::version> plugin::versions() const
{
    std::set<version, version_compare> set;

    std::transform(instances.cbegin(), instances.cend(),
                   std::inserter(set, set.begin()),
        [](const std::pair<version, library_ptr> &entry) {
            return entry.first;
        });

    std::vector<version> ret;
    ret.reserve(set.size());
    std::copy(set.begin(), set.end(), std::back_inserter(ret));
    return ret;
}

typed_plugin<parser> frontend("emel_frontend_instance",
    "emel_frontend_name", "emel_frontend_version");

} // namespace emel
