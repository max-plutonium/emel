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
#ifndef AST_H
#define AST_H

#include "opcodes.h"

#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_LIST_SIZE 30
#define BOOST_MPL_LIMIT_VECTOR_SIZE 30

#include <memory>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/variant/get.hpp>

namespace emel EMEL_EXPORT {

namespace ast EMEL_EXPORT {

struct class_;
struct param;
struct method;
struct while_;
struct for_;
struct if_;
struct case_;
struct switch_;
struct continue_;
struct break_;
struct return_;
struct try_;
struct assign;
struct ternary;
struct bin_op;
struct variable;
struct un_op;
struct call;

using node = boost::make_recursive_variant<
        std::string, double, bool,
        boost::recursive_wrapper<class_>,
        boost::recursive_wrapper<method>,
        boost::recursive_wrapper<while_>,
        boost::recursive_wrapper<for_>,
        boost::recursive_wrapper<if_>,
        boost::recursive_wrapper<case_>,
        boost::recursive_wrapper<switch_>,
        boost::recursive_wrapper<continue_>,
        boost::recursive_wrapper<break_>,
        boost::recursive_wrapper<return_>,
        boost::recursive_wrapper<try_>,
        boost::recursive_wrapper<assign>,
        boost::recursive_wrapper<ternary>,
        boost::recursive_wrapper<bin_op>,
        boost::recursive_wrapper<variable>,
        boost::recursive_wrapper<un_op>,
        boost::recursive_wrapper<call>
    >::type;

struct class_
{
    std::string name, base_name;
    std::vector<node> exprs;
    std::vector<method> methods;
};

struct param
{
    std::string name;
    bool by_ref = false;
};

struct method
{
    std::string name;
    std::vector<param> params;
    std::vector<node> exprs;

    method() = default;

    method(std::string name, std::vector<param> params, std::vector<node> exprs)
        : name(name), params(std::move(params)), exprs(std::move(exprs))
    { }
};

struct while_
{
    node cond;
    std::vector<node> exprs;
};

struct for_
{
    node init, cond, step;
    std::vector<node> exprs;
};

struct if_
{
    node cond;
    std::vector<node> then_exprs, else_exprs;
};

struct case_
{
    std::vector<node> match_values;
    std::vector<node> exprs;
};

struct switch_
{
    node cond;
    std::vector<node> blocks;
};

struct continue_ { };
struct break_ { };

struct return_
{
    node e;
};

struct try_
{
    std::string var_name;
    std::vector<node> exprs;
};

struct assign
{
    std::string var_name;
    node rhs;
    bool as_external = false;
};

struct ternary
{
    node cond, first, second;

    ternary(node c, node f, node s)
        : cond(std::move(c)), first(std::move(f)), second(std::move(s))
    { }
};

struct bin_op
{
    op_kind k;
    node lhs, rhs;

    bin_op(op_kind ak, node l, node r)
        : k(ak), lhs(std::move(l)), rhs(std::move(r))
    { }
};

struct variable
{
    std::string name;
    bool ref_of = false;
    bool val_of = false;
};

struct un_op
{
    op_kind k;
    node rhs;
};

struct call
{
    std::vector<node> names, args;
    node chain_call;
};

std::ostream &operator <<(std::ostream &os, const class_ &arg);
std::ostream &operator <<(std::ostream &os, const param &arg);
std::ostream &operator <<(std::ostream &os, const std::vector<param> &arg);
std::ostream &operator <<(std::ostream &os, const method &arg);
std::ostream &operator <<(std::ostream &os, const std::vector<method> &arg);
std::ostream &operator <<(std::ostream &os, const while_ &arg);
std::ostream &operator <<(std::ostream &os, const for_ &arg);
std::ostream &operator <<(std::ostream &os, const if_ &arg);
std::ostream &operator <<(std::ostream &os, const case_ &arg);
std::ostream &operator <<(std::ostream &os, const switch_ &arg);
std::ostream &operator <<(std::ostream &os, const continue_ &arg);
std::ostream &operator <<(std::ostream &os, const break_ &arg);
std::ostream &operator <<(std::ostream &os, const return_ &arg);
std::ostream &operator <<(std::ostream &os, const try_ &arg);
std::ostream &operator <<(std::ostream &os, const assign &arg);
std::ostream &operator <<(std::ostream &os, const ternary &arg);
std::ostream &operator <<(std::ostream &os, const bin_op &arg);
std::ostream &operator <<(std::ostream &os, const variable &arg);
std::ostream &operator <<(std::ostream &os, const un_op &arg);
std::ostream &operator <<(std::ostream &os, const call &arg);
std::ostream &operator <<(std::ostream &os, const std::vector<ast::node> &arg);

} // namespace ast

} // namespace emel

#endif // AST_H
