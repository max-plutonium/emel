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
#include "grammar.h"

#include <boost/phoenix/phoenix.hpp>
#include <boost/fusion/adapted/struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(
        emel::ast::class_,
        (std::string, name)
        (std::string, base_name)
        (std::vector<emel::ast::node>, exprs)
        (std::vector<emel::ast::method>, methods)
    );

BOOST_FUSION_ADAPT_STRUCT(
        emel::ast::param,
        (std::string, name)
        (bool, by_ref)
    );

BOOST_FUSION_ADAPT_STRUCT(
        emel::ast::method,
        (std::string, name)
        (std::vector<emel::ast::param>, params)
        (std::vector<emel::ast::node>, exprs)
    );

BOOST_FUSION_ADAPT_STRUCT(
        emel::ast::try_,
        (std::string, var_name)
        (std::vector<emel::ast::node>, exprs)
    );

BOOST_FUSION_ADAPT_STRUCT(
        emel::ast::case_,
        (std::vector<emel::ast::node>, match_values)
        (std::vector<emel::ast::node>, exprs)
    );

BOOST_FUSION_ADAPT_STRUCT(
        emel::ast::switch_,
        (emel::ast::node, cond)
        (std::vector<emel::ast::node>, blocks)
    );

BOOST_FUSION_ADAPT_STRUCT(
        emel::ast::if_,
        (emel::ast::node, cond)
        (std::vector<emel::ast::node>, then_exprs)
        (std::vector<emel::ast::node>, else_exprs)
    );

BOOST_FUSION_ADAPT_STRUCT(
        emel::ast::for_,
        (emel::ast::node, init)
        (emel::ast::node, cond)
        (emel::ast::node, step)
        (std::vector<emel::ast::node>, exprs)
    );

BOOST_FUSION_ADAPT_STRUCT(
        emel::ast::while_,
        (emel::ast::node, cond)
        (std::vector<emel::ast::node>, exprs)
    );

BOOST_FUSION_ADAPT_STRUCT(
        emel::ast::return_,
        (emel::ast::node, e)
    );

namespace emel { namespace spirit_frontend {

grammar::grammar() : grammar::base_type(root, "root")
{
    using qi::_val;
    using qi::_1;
    using qi::_2;
    using qi::_3;
    using qi::_4;
    using qi::_a;
    using boost::phoenix::at_c;
    using boost::phoenix::push_back;
    using boost::phoenix::construct;

    // TODO несколько классов
    root = class_def[ _val = std::move(_1) ] /*| +class_def[ _val = std::move(_1) ]*/;

    class_def = class_ > id[ at_c<0>(_val) = std::move(_1) ]
        > -(colon > id[ at_c<1>(_val) = std::move(_1) ])
        >> *(block_expr[ push_back(at_c<2>(_val), std::move(_1)) ]
            | method_def[ push_back(at_c<3>(_val), std::move(_1)) ])
        > end_class;

    class_def.name("class def");
// TODO qi::on_error<qi::fail>(class_def, eh(_1, _3, _4));

    param_def = id[ at_c<0>(_val) = std::move(_1) ] >> -byref[ at_c<1>(_val) = true ];

    param_def.name("param def");
    qi::on_error<qi::fail>(param_def, eh(_1, _3, _4));

    method_def = id[ at_c<0>(_val) = std::move(_1) ]
        >> left_paren
        >> -(param_def % comma)[ at_c<1>(_val) = std::move(_1) ]
        > right_paren > left_brace
        >> -exprs[ at_c<2>(_val) = std::move(_1) ]
        > right_brace;

    method_def.name("method def");
    qi::on_error<qi::fail>(method_def, eh(_1, _3, _4));

    exprs %= +block_expr;

    exprs.name("expressions");
    qi::on_error<qi::fail>(exprs, eh(_1, _3, _4));

    block_expr %= try_block | switch_branch | if_branch
           | for_loop | while_loop | line_expr;

    block_expr.name("block of expression");
    qi::on_error<qi::fail>(block_expr, eh(_1, _3, _4));

    branch_expr %= (continue_branch | break_branch | return_branch) > semicolon;

    branch_expr.name("branch expression");
    qi::on_error<qi::fail>(branch_expr, eh(_1, _3, _4));

    // TODO нужно что-то делать с ;
    line_expr %= branch_expr | (expr >> semicolon) | semicolon;

    line_expr.name("line expression");
    qi::on_error<qi::fail>(line_expr, eh(_1, _3, _4));

    try_block = try_ > left_paren
        > id[ at_c<0>(_val) = std::move(_1) ]
        > right_paren
        >> -exprs[ at_c<1>(_val) = std::move(_1) ]
        > catch_;

    try_block.name("try block");
    qi::on_error<qi::fail>(try_block, eh(_1, _3, _4));

    case_branch = (+(case_ > value[ push_back(at_c<0>(_val), std::move(_1)) ] > colon)
           | (default_ > colon))
       >> -exprs[ at_c<1>(_val) = std::move(_1) ];

    case_branch.name("case branch");
    qi::on_error<qi::fail>(case_branch, eh(_1, _3, _4));

    switch_branch = switch_ > left_paren
        > expr[ at_c<0>(_val) = std::move(_1) ]
        > right_paren
        >> *case_branch[ push_back(at_c<1>(_val), std::move(_1)) ]
        > end_switch;

    switch_branch.name("switch branch");
    qi::on_error<qi::fail>(switch_branch, eh(_1, _3, _4));

    if_branch = if_ > left_paren
        > expr[ at_c<0>(_val) = std::move(_1) ]
        > right_paren
        >> -exprs[ at_c<1>(_val) = std::move(_1) ]
        >> -(else_ > -exprs[ at_c<2>(_val) = std::move(_1) ])
        > end_if;

    if_branch.name("if branch");
    qi::on_error<qi::fail>(if_branch, eh(_1, _3, _4));

    for_loop = for_ > left_paren
        >> -expr[ at_c<0>(_val) = std::move(_1) ] > semicolon
        >> -expr[ at_c<1>(_val) = std::move(_1) ] > semicolon
        >> -expr[ at_c<2>(_val) = std::move(_1) ]
        > right_paren
        >> -exprs[ at_c<3>(_val) = std::move(_1) ]
        > end_for;

    for_loop.name("for loop");
    qi::on_error<qi::fail>(for_loop, eh(_1, _3, _4));

    while_loop = while_ > left_paren
        > expr[ at_c<0>(_val) = std::move(_1) ]
        > right_paren
        >> -exprs[ at_c<1>(_val) = std::move(_1) ]
        > end_while;

    while_loop.name("while loop");
    qi::on_error<qi::fail>(while_loop, eh(_1, _3, _4));

    continue_branch = continue_;
    break_branch = break_;
    return_branch = return_ >> -expr[ at_c<0>(_val) = std::move(_1) ];

    continue_branch.name("continue branch");
    break_branch.name("break branch");
    return_branch.name("return branch");
}

} // namespace spirit_frontend

} // namespace emel

