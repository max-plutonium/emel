/*
 * Copyright (C) 2015, 2016 Max Plutonium <plutonium.max@gmail.com>
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
#include "source-loader.h"

#include <fstream>
#include <iostream>
#include <set>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/locale.hpp>

namespace emel {

namespace bfs = boost::filesystem;

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
        //paths.push_back(bfs::current_path().string());

        if(const char *env = std::getenv("EMEL_HOME")) {
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
        paths.push_back(bfs::absolute(dir_name, bfs::initial_path()).string());

    return paths;
}

static std::vector<std::pair<std::string, bfs::directory_entry>>
find_sources_recursive(const std::string &path, const std::string &subdirs, const char *ext)
{
    std::vector<std::pair<std::string, bfs::directory_entry>> ret;
    bfs::directory_iterator dir_begin(path), dir_end;

    std::for_each(dir_begin, dir_end,
        [&ret, &subdirs, &ext](const bfs::directory_entry &entry) {
            if(bfs::is_directory(entry)) {
                std::string nested_subdir = subdirs;
                if(!subdirs.empty())
                    nested_subdir.append(".");
                nested_subdir.append(entry.path().filename().string());

                auto res = find_sources_recursive(
                    entry.path().string(), nested_subdir, ext);
                ret.reserve(ret.size() + res.size());
                while(!res.empty()) {
                    ret.push_back(std::move(res.back()));
                    res.pop_back();
                }

            } else {
                if(ext == entry.path().extension().string())
                    ret.emplace_back(subdirs, entry);
            }
        });

    return ret;
}

std::size_t source_loader::scan_dir(const std::string &dir_name)
{
    std::size_t ret = 0;
    std::vector<std::string> paths = get_paths(dir_name);

    for(const std::string &path : paths) {
        auto entries = find_sources_recursive(path, "", ".emel");

        for(const auto &entry : entries) {
            std::string class_name = entry.first;
            if(!class_name.empty())
                class_name.append(".");

            const auto filename = entry.second.path().filename().string();
            const auto pos = filename.find_first_of('.');
            assert(std::string::npos != pos);
            class_name.append(filename.substr(0, pos));
            locations_map.emplace(std::move(class_name), bfs::absolute(entry.second.path()).string());
            ++ret;
        }
    }

    return ret;
}

std::string source_loader::get_path_for(const std::string &class_name) const
{
    auto iter = locations_map.find(class_name);
    if(locations_map.end() == iter)
        return std::string();
    return iter->second;
}

std::vector<std::string> source_loader::names() const
{
    std::set<std::string> set;

    std::transform(locations_map.cbegin(), locations_map.cend(),
                   std::inserter(set, set.begin()),
        [](const std::pair<std::string, std::string> &entry) {
            return entry.first;
        });

    std::vector<std::string> ret;
    ret.reserve(set.size());
    std::copy(set.begin(), set.end(), std::back_inserter(ret));
    return ret;
}

std::string source_loader::read_source(const std::string &class_name, const char *locale_name) const
{
    static boost::locale::generator generator;

    const auto path = get_path_for(class_name);
    if(path.empty()) {
        std::cerr << "Path for class " << class_name << " doesn't exists." << std::endl;
        return std::string();
    }

    std::ifstream is(path.c_str());

    if(!is.is_open()) {
        std::cerr << "Could not open file " << path << std::endl;
        return std::string();
    }

    is.unsetf(std::ios::skipws);

    std::string ret {
        std::istreambuf_iterator<char>(is.rdbuf()),
        std::istreambuf_iterator<char>() };

    is.close();

    if(std::strstr(locale_name, "utf-8") != nullptr)
        return ret;
    else
        return boost::locale::conv::to_utf<char>(ret, generator(locale_name));
}

} // namespace emel
