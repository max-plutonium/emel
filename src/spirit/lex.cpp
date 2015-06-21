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
#include "lex.h"

#include <boost/phoenix/object/construct.hpp>
#include <boost/phoenix/bind/bind_function.hpp>

namespace emel {

using boost::phoenix::construct;

static double get_double(pos_iter f, pos_iter l) {
    return boost::lexical_cast<double>(std::string(f, l));
}

lexer::lexer()
    : ws("\\s+")
    , comment("(\\/\\/[^\n]*)|(\\/\\*[^*]*\\*+([^/*][^*]*\\*+)*\\/)|(\'[^\']*\')", token::comment)
    , id("[a-zA-Z_](\\w)*", token::id)
    , number("-?((\\d+)|(\\d*\\.\\d+)|(\\d+\\.\\d*))", token::number)
    , text("\\\"([^\\\"]|\\\\\\\")*\\\"", token::text)
    , true_("(?i:true)", token::true_)
    , false_("(?i:false)", token::false_)
    , class_("(?i:class)", token::class_)
    , end_class("(?i:end *class)", token::end_class)
    , while_("(?i:while)", token::while_)
    , end_while("(?i:end *while)", token::end_while)
    , for_("(?i:for)", token::for_)
    , end_for("(?i:end *for)", token::end_for)
    , if_("(?i:if)", token::if_)
    , else_("(?i:else)", token::else_)
    , end_if("(?i:end *if)", token::end_if)
    , switch_("(?i:switch)", token::switch_)
    , case_("(?i:case)", token::case_)
    , default_("(?i:default)", token::default_)
    , end_switch("(?i:end *switch)", token::end_switch)
    , continue_("(?i:continue)", token::continue_)
    , break_("(?i:break)", token::break_)
    , return_("(?i:return)", token::return_)
    , try_("(?i:try)", token::try_)
    , catch_("(?i:catch)", token::catch_)
    , byref("(?i:byref)", token::byref)
    , byval("(?i:byval)", token::byval)
    , as_external("(?i:asexternal)", token::as_external)
    , ref_of("(?i:refof)", token::ref_of)
    , val_of("(?i:valof)", token::val_of)
    , colon(":", token::colon)
    , comma(",", token::comma)
    , dot("\\.", token::dot)
    , semicolon(";", token::semicolon)
    , assign("=", token::assign)
    , q_sign("\\?", token::q_sign)
    , or_("\\|", token::or_)
    , xor_("\\^", token::xor_)
    , and_("&", token::and_)
    , not_("~", token::not_)
    , eq("==", token::eq)
    , ne("!=", token::ne)
    , lt("<", token::lt)
    , gt(">", token::gt)
    , lte("<=", token::lte)
    , gte(">=", token::gte)
    , plus("\\+", token::plus)
    , minus("\\-", token::minus)
    , mul("\\*", token::mul)
    , div("\\/", token::div)
    , left_paren("\\(", token::left_paren)
    , right_paren("\\)", token::right_paren)
    , left_brace("\\{", token::left_brace)
    , right_brace("\\}", token::right_brace)
    , left_bracket("\\[", token::left_bracket)
    , right_bracket("\\]", token::right_bracket)
{
    this->self =
            true_[ lex::_val = true ] | false_[ lex::_val = false ]
            | class_ | end_class
            | while_ | end_while
            | for_ | end_for
            | if_ | else_ | end_if
            | switch_ | case_ | default_ | end_switch
            | continue_ | break_ | return_
            | try_ | catch_
            | byref | byval | as_external | ref_of | val_of
            | colon | comma | dot | semicolon
            | assign | q_sign | or_ | xor_ | and_ | not_
            | eq | ne | lt | gt | lte | gte
            | plus | minus | mul | div
            | left_paren | right_paren
            | left_brace | right_brace
            | left_bracket | right_bracket
            | id[ lex::_val = construct<std::string>(lex::_start, lex::_end) ]
            | number[ lex::_val = boost::phoenix::bind(get_double, lex::_start, lex::_end) ]
            | text[ lex::_val = construct<std::string>(lex::_start, lex::_end) ]
            | ws[ lex::_pass = lex::pass_flags::pass_ignore ]
            | comment[ lex::_pass = lex::pass_flags::pass_ignore ];
}

} // namespace emel
