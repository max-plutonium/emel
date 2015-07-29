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
#ifndef VALUES_H
#define VALUES_H

#include "../ast.h"
#include "skipper.h"
#include "keywords.h"

namespace emel { namespace spirit_frontend {

class values : virtual public keywords
{
    using iterator_type = pos_iter;

protected:
    qi::rule<iterator_type, ast::node(), skipper> value;
    qi::rule<iterator_type, char()> escaped;
    qi::rule<iterator_type, std::string(), skipper> id;
    qi::rule<iterator_type, std::string()> text_content, text_value;
    qi::rule<iterator_type, double(), skipper> number_value;
    qi::rule<iterator_type, bool(), skipper> bool_value;

public:
    values();
};

} // namespace spirit_frontend

} // namespace emel

#endif // VALUES_H
