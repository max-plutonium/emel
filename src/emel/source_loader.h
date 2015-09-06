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
#ifndef SOURCE_LOADER_H
#define SOURCE_LOADER_H

#include <unordered_map>
#include <string>
#include <vector>

namespace emel {

class source_loader
{
protected:
    std::unordered_map<std::string, std::string> locations_map;

public:
    std::size_t scan_dir(const std::string &dir_name = std::string());
    std::string get_path_for(const std::string &class_name) const;
    std::vector<std::string> names() const;
};

} // namespace emel

#endif // SOURCE_LOADER_H
