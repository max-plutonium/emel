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
#pragma once

namespace emel {

enum token {
    comment = 70000,
    id, number, text, true_, false_,
    class_, end_class,
    while_, end_while,
    for_, end_for,
    if_, else_, end_if,
    switch_, case_, default_, end_switch,
    continue_, break_, return_,
    try_, catch_,
    byref, byval, as_external, ref_of, val_of,
    colon, comma, dot, semicolon,
    assign, q_sign, or_, xor_, and_, not_,
    eq, ne, lt, gt, lte, gte,
    plus, minus, mul, div,
    left_paren, right_paren,
    left_brace, right_brace,
    left_bracket, right_bracket
};

const char *token_name(token tok);

} // namespace emel
