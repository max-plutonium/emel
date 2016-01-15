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
#include "keywords.h"

#include <boost/spirit/repository/include/qi_distinct.hpp>

namespace emel { namespace spirit_frontend {

keywords::keywords()
{
    using namespace boost::spirit;
    using boost::spirit::repository::qi::distinct;
    using qi::alnum;
    using qi::no_case;
    using qi::lit;
    using qi::space;

    end = no_case[ lit("end") ];
    true_ = distinct(alnum)[ no_case[ lit("true") ] ];
    true_.name("true");
    false_ = distinct(alnum)[ no_case[ lit("false") ] ];
    false_.name("false");
    class_ = distinct(alnum)[ no_case[ lit("class") ] ];
    class_.name("class");
    end_class = end >> class_;
    end_class.name("end class");
    while_ = distinct(alnum)[ no_case[ lit("while") ] ];
    while_.name("while");
    end_while = end >> while_;
    end_while.name("end while");
    for_ = distinct(alnum)[ no_case[ lit("for") ] ];
    for_.name("for");
    end_for = end >> for_;
    end_for.name("end for");
    if_ = distinct(alnum)[ no_case[ lit("if") ] ];
    if_.name("if");
    else_ = distinct(alnum)[ no_case[ lit("else") ] ];
    else_.name("else");
    end_if = end >> if_;
    end_if.name("end if");
    switch_ = distinct(alnum)[ no_case[ lit("switch") ] ];
    switch_.name("switch");
    case_ = distinct(alnum)[ no_case[ lit("case") ] ];
    case_.name("case");
    default_ = distinct(alnum)[ no_case[ lit("default") ] ];
    default_.name("default");
    end_switch = end >> switch_;
    end_switch.name("end switch");
    continue_ = distinct(alnum)[ no_case[ lit("continue") ] ];
    continue_.name("continue");
    break_ = distinct(alnum)[ no_case[ lit("break") ] ];
    break_.name("break");
    return_ = distinct(alnum)[ no_case[ lit("return") ] ];
    return_.name("return");
    try_ = distinct(alnum)[ no_case[ lit("try") ] ];
    try_.name("try");
    catch_ = distinct(alnum)[ no_case[ lit("catch") ] ];
    catch_.name("catch");
    byref = distinct(alnum)[ no_case[ lit("byref") ] ];
    byref.name("byref");
    byval = distinct(alnum)[ no_case[ lit("byval") ] ];
    byval.name("byval");
    as_external = distinct(alnum)[ no_case[ lit("asexternal") ] ];
    as_external.name("as external");
    ref_of = distinct(alnum)[ no_case[ lit("refof") ] ];
    ref_of.name("ref of");
    val_of = distinct(alnum)[ no_case[ lit("valof") ] ];
    val_of.name("val of");
    colon = lit(':');
    colon.name(":");
    dbl_colon = lit("::");
    dbl_colon.name("::");
    comma = lit(',');
    comma.name(",");
    dot = lit('.');
    dot.name(".");
    semicolon = lit(';');
    semicolon.name(";");
    assign = lit('=');
    assign.name("=");
    q_sign = lit('?');
    q_sign.name("?");
    or_ = lit('|');
    or_.name("|");
    xor_ = lit('^');
    xor_.name("^");
    and_ = lit('&');
    and_.name("&");
    not_ = lit('~');
    not_.name("~");
    eq = lit("==");
    eq.name("==");
    ne = lit("!=");
    ne.name("!=");
    lt = lit('<');
    lt.name("<");
    gt = lit('>');
    gt.name(">");
    lte = lit("<=");
    lte.name("<=");
    gte = lit(">=");
    gte.name(">=");
    plus = lit('+');
    plus.name("+");
    minus = lit('-');
    minus.name("-");
    mul = lit('*');
    mul.name("*");
    div = lit('/');
    div.name("/");
    left_paren = lit('(');
    left_paren.name("(");
    right_paren = lit(')');
    right_paren.name(")");
    left_brace = lit('{');
    left_brace.name("{");
    right_brace = lit('}');
    right_brace.name("}");
    left_bracket = lit('[');
    left_bracket.name("[");
    right_bracket = lit(']');
    right_bracket.name("]");
}

} // namespace spirit_frontend

} // namespace emel
