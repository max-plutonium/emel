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
#include "error-handler.h"

#include <iostream>

namespace emel { namespace spirit_frontend {

void error_handler::operator ()(pos_iter pos1, pos_iter pos2,
                                const boost::spirit::info &info) const
{
    // TODO
    std::cout << "Error in " << pos2.get_position().file << ":"
              << pos2.get_position().line << ":" << pos2.get_position().column
              << " Expecting " << info << " here: \""
              << std::string(pos1, pos2) << "\"" << std::endl;
}

} // namespace spirit_frontend

} // namespace emel
