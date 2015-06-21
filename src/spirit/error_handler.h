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
#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include "emel_global.h"

#include <boost/spirit/home/support/info.hpp>

namespace emel {

class error_handler
{
public:
    using result = void;

    void operator ()(pos_iter pos1, pos_iter pos2, const boost::spirit::info &info) const;
};

} // namespace emel

#endif // ERROR_HANDLER_H
