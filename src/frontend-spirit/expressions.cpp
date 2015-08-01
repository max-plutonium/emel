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
#include "expressions.h"

#include <boost/phoenix/phoenix.hpp>
#include <boost/fusion/adapted/struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(
        emel::ast::assign,
        (std::string, var_name)
        (emel::ast::node, rhs)
        (bool, as_external)
    );

BOOST_FUSION_ADAPT_STRUCT(
        emel::ast::ternary,
        (emel::ast::node, cond)
        (emel::ast::node, first)
        (emel::ast::node, second)
    );

BOOST_FUSION_ADAPT_STRUCT(
        emel::ast::bin_op,
        (emel::op_kind, k)
        (emel::ast::node, lhs)
        (emel::ast::node, rhs)
    );

BOOST_FUSION_ADAPT_STRUCT(
        emel::ast::variable,
        (std::string, name)
        (bool, ref_of)
        (bool, val_of)
    );

BOOST_FUSION_ADAPT_STRUCT(
        emel::ast::un_op,
        (emel::op_kind, k)
        (emel::ast::node, rhs)
    );

BOOST_FUSION_ADAPT_STRUCT(
        emel::ast::call,
        (std::vector<emel::ast::node>, names)
        (std::vector<emel::ast::node>, args)
        (emel::ast::node, chain_call)
    );

namespace emel { namespace spirit_frontend {

expressions::expressions()
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

    expr %= assignment | ternary;

    assignment = id[ at_c<0>(_val) = std::move(_1) ]
        >> -(byval | as_external[ at_c<2>(_val) = true ])
        >> assign >> expr[ at_c<1>(_val) = std::move(_1) ];

    assignment.name("assignment");
    qi::on_error<qi::fail>(assignment, eh(_1, _3, _4));

    ternary = log_or[ _val = std::move(_1) ]
        >> *(q_sign > expr[ _a = std::move(_1) ] > colon > log_or
           [ _val = construct<ast::ternary>(std::move(_val), std::move(_a), std::move(_1)) ]);

    ternary.name("ternary");
    qi::on_error<qi::fail>(ternary, eh(_1, _3, _4));

    log_or = log_xor[ _val = std::move(_1) ] >> *(or_ > log_xor
        [ _val = construct<ast::bin_op>(op_kind::or_, std::move(_val), std::move(_1)) ]);

    log_xor = log_and[ _val = std::move(_1) ] >> *(xor_ > log_and
        [ _val = construct<ast::bin_op>(op_kind::xor_, std::move(_val), std::move(_1)) ]);

    log_and = rel_eq[ _val = std::move(_1) ] >> *(and_ > rel_eq
        [ _val = construct<ast::bin_op>(op_kind::and_, std::move(_val), std::move(_1)) ]);

    rel_eq = rel_order[ _val = std::move(_1) ]
        >> *((eq[ _a = op_kind::eq ] | ne[ _a = op_kind::ne ])
           > rel_order[ _val = construct<ast::bin_op>(_a, std::move(_val), std::move(_1)) ]);

    rel_order = addition[ _val = std::move(_1) ]
        >> *((lte[ _a = op_kind::lte ] | gte[ _a = op_kind::gte ]
              | lt[ _a = op_kind::lt ] | gt[ _a = op_kind::gt ])
           > addition[ _val = construct<ast::bin_op>(_a, std::move(_val), std::move(_1)) ]);

    addition = multiplication[ _val = std::move(_1) ]
        >> *((plus[ _a = op_kind::add ] | minus[ _a = op_kind::sub ])
           > multiplication[ _val = construct<ast::bin_op>(_a, std::move(_val), std::move(_1)) ]);

    multiplication = factor[ _val = std::move(_1) ]
        >> *((mul[ _a = op_kind::mul ] | div[ _a = op_kind::div ])
           > factor[ _val = construct<ast::bin_op>(_a, std::move(_val), std::move(_1)) ]);

    log_or.name("log or");
    qi::on_error<qi::fail>(log_or, eh(_1, _3, _4));
    log_xor.name("log xor");
    qi::on_error<qi::fail>(log_xor, eh(_1, _3, _4));
    log_and.name("log and");
    qi::on_error<qi::fail>(log_and, eh(_1, _3, _4));
    rel_eq.name("rel eq");
    qi::on_error<qi::fail>(rel_eq, eh(_1, _3, _4));
    rel_order.name("rel order");
    qi::on_error<qi::fail>(rel_order, eh(_1, _3, _4));
    addition.name("addition");
    qi::on_error<qi::fail>(addition, eh(_1, _3, _4));
    multiplication.name("multiplication");
    qi::on_error<qi::fail>(multiplication, eh(_1, _3, _4));

    factor %= value | call | var_ref | (left_paren > expr > right_paren)
        | unary | (plus > factor);

    factor.name("factor");
    qi::on_error<qi::fail>(factor, eh(_1, _3, _4));

    var_ref = -(ref_of[ at_c<1>(_val) = true ] | val_of[ at_c<2>(_val) = true ])
              >> id[ at_c<0>(_val) = std::move(_1) ];

    var_ref.name("var ref");
    qi::on_error<qi::fail>(var_ref, eh(_1, _3, _4));

    unary = (not_[ at_c<0>(_val) = op_kind::not_ ]
        | minus[ at_c<0>(_val) = op_kind::neg ])
            > factor[ at_c<1>(_val) = std::move(_1) ];

    unary.name("unary op");
    qi::on_error<qi::fail>(unary, eh(_1, _3, _4));

    // TODO разделить пути поиска идентификатора на переменная.ид и класс.ид
    // FIXME пустое выражение в качестве аргумента должно генерировать передачу
    // пустого значения
    call = (id[ push_back(at_c<0>(_val), std::move(_1)) ] % (dot | dbl_colon))
        >> left_paren
        >> -(expr[ push_back(at_c<1>(_val), std::move(_1)) ] % comma)
        >> right_paren >> *(dot > call[ at_c<2>(_val) = std::move(_1) ]);

    call.name("call");
    qi::on_error<qi::fail>(call, eh(_1, _3, _4));
}

} // namespace spirit_frontend

} // namespace emel
