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
#include "tokens.h"

namespace emel {

const char *token_name(token tok)
{
    const char *ret = "undefined-token";

# define TOK_NAME(TOK) \
    case TOK: \
        ret = #TOK; \
        break;

    switch (tok) {
        TOK_NAME(comment)
        TOK_NAME(id)
        TOK_NAME(number)
        TOK_NAME(text)
        TOK_NAME(true_)
        TOK_NAME(false_)
        TOK_NAME(class_)
        TOK_NAME(end_class)
        TOK_NAME(while_)
        TOK_NAME(end_while)
        TOK_NAME(for_)
        TOK_NAME(end_for)
        TOK_NAME(if_)
        TOK_NAME(else_)
        TOK_NAME(end_if)
        TOK_NAME(switch_)
        TOK_NAME(case_)
        TOK_NAME(default_)
        TOK_NAME(end_switch)
        TOK_NAME(continue_)
        TOK_NAME(break_)
        TOK_NAME(return_)
        TOK_NAME(try_)
        TOK_NAME(catch_)
        TOK_NAME(byref)
        TOK_NAME(byval)
        TOK_NAME(as_external)
        TOK_NAME(ref_of)
        TOK_NAME(val_of)
        TOK_NAME(colon)
        TOK_NAME(comma)
        TOK_NAME(dot)
        TOK_NAME(semicolon)
        TOK_NAME(assign)
        TOK_NAME(q_sign)
        TOK_NAME(or_)
        TOK_NAME(xor_)
        TOK_NAME(and_)
        TOK_NAME(not_)
        TOK_NAME(eq)
        TOK_NAME(ne)
        TOK_NAME(lt)
        TOK_NAME(gt)
        TOK_NAME(lte)
        TOK_NAME(gte)
        TOK_NAME(plus)
        TOK_NAME(minus)
        TOK_NAME(mul)
        TOK_NAME(div)
        TOK_NAME(left_paren)
        TOK_NAME(right_paren)
        TOK_NAME(left_brace)
        TOK_NAME(right_brace)
        TOK_NAME(left_bracket)
        TOK_NAME(right_bracket)

        default:
            break;
    }
# undef TOK_NAME

    return ret;
}

} // namespace emel

