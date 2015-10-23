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

#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_LIST_SIZE 30
#define BOOST_MPL_LIMIT_VECTOR_SIZE 30

#include <vector>
#include <memory>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/variant/get.hpp>

namespace emel EMEL_EXPORT { namespace ast EMEL_EXPORT {

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

enum class op_kind {
    or_ = 201, xor_, and_, eq, ne, lt, gt, lte, gte, add, sub, mul, div,
    not_ = 101, neg
};

struct position {
    std::shared_ptr<std::string> file;
    std::size_t line, column;

    position() = default;
    position(const position &) = default;
    position &operator =(const position &) = default;
    position(position&&) = default;
    position &operator =(position&&) = default;

    position(const std::string &file, std::size_t line, std::size_t column = 0);
};

struct position_node {
    position first, last;

    void set_position(position first, position last) {
        this->first = std::move(first); this->last = std::move(last);
    }
};

using node = boost::make_recursive_variant<
        empty_value_type, std::string, double, bool,
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

struct class_ : position_node
{
    std::string name, base_name;
    std::vector<node> exprs;
    std::vector<method> methods;
};

struct param : position_node
{
    std::string name;
    bool by_ref = false;
};

struct method : position_node
{
    std::string name;
    std::vector<param> params;
    std::vector<node> exprs;

    method() = default;

    method(std::string name, std::vector<param> params, std::vector<node> exprs)
        : name(name), params(std::move(params)), exprs(std::move(exprs))
    { }
};

struct while_ : position_node
{
    node cond;
    std::vector<node> exprs;

    while_() = default;

    while_(node cond, std::vector<node> exprs)
        : cond(std::move(cond))
        , exprs(std::move(exprs))
    { }
};

struct for_ : position_node
{
    node init, cond, step;
    std::vector<node> exprs;

    for_() = default;

    for_(node init, node cond, node step, std::vector<node> exprs)
        : init(std::move(init))
        , cond(std::move(cond))
        , step(std::move(step))
        , exprs(std::move(exprs))
    { }
};

struct if_ : position_node
{
    node cond;
    std::vector<node> then_exprs, else_exprs;

    if_() = default;

    if_(node cond, std::vector<node> then_exprs, std::vector<node> else_exprs)
        : cond(std::move(cond))
        , then_exprs(std::move(then_exprs))
        , else_exprs(std::move(else_exprs))
    { }
};

struct case_ : position_node
{
    std::vector<node> match_values;
    std::vector<node> exprs;

    case_() = default;

    case_(std::vector<node> match_values, std::vector<node> exprs)
        : match_values(std::move(match_values))
        , exprs(std::move(exprs))
    { }
};

struct switch_ : position_node
{
    node cond;
    std::vector<node> blocks;

    switch_() = default;

    switch_(node cond, std::vector<node> blocks)
        : cond(std::move(cond))
        , blocks(std::move(blocks))
    { }
};

struct continue_ : position_node { };
struct break_ : position_node { };

struct return_ : position_node
{
    node e;
};

struct try_ : position_node
{
    std::string var_name;
    std::vector<node> exprs;
};

struct assign : position_node
{
    std::string var_name;
    node rhs;
    bool as_external = false;

    assign() = default;

    assign(const std::string &name, node rhs)
        : var_name(name), rhs(std::move(rhs)) { }
};

struct ternary : position_node
{
    node cond, first, second;

    ternary(node c, node f, node s)
        : cond(std::move(c)), first(std::move(f)), second(std::move(s))
    { }
};

struct bin_op : position_node
{
    op_kind k;
    node lhs, rhs;

    bin_op(op_kind ak, node l, node r)
        : k(ak), lhs(std::move(l)), rhs(std::move(r))
    { }
};

struct variable : position_node
{
    std::string name;
    bool ref_of = false;
    bool val_of = false;
};

struct un_op : position_node
{
    op_kind k;
    node rhs;

    un_op() = default;

    un_op(op_kind k, node rhs)
        : k(k), rhs(std::move(rhs)) { }
};

struct call : position_node
{
    std::vector<node> names, args;
    node chain_call;
};

EMEL_EXPORT std::ostream &operator <<(std::ostream &os, const class_ &arg);
EMEL_EXPORT std::ostream &operator <<(std::ostream &os, const param &arg);
EMEL_EXPORT std::ostream &operator <<(std::ostream &os, const std::vector<param> &arg);
EMEL_EXPORT std::ostream &operator <<(std::ostream &os, const method &arg);
EMEL_EXPORT std::ostream &operator <<(std::ostream &os, const std::vector<method> &arg);
EMEL_EXPORT std::ostream &operator <<(std::ostream &os, const while_ &arg);
EMEL_EXPORT std::ostream &operator <<(std::ostream &os, const for_ &arg);
EMEL_EXPORT std::ostream &operator <<(std::ostream &os, const if_ &arg);
EMEL_EXPORT std::ostream &operator <<(std::ostream &os, const case_ &arg);
EMEL_EXPORT std::ostream &operator <<(std::ostream &os, const switch_ &arg);
EMEL_EXPORT std::ostream &operator <<(std::ostream &os, const continue_ &arg);
EMEL_EXPORT std::ostream &operator <<(std::ostream &os, const break_ &arg);
EMEL_EXPORT std::ostream &operator <<(std::ostream &os, const return_ &arg);
EMEL_EXPORT std::ostream &operator <<(std::ostream &os, const try_ &arg);
EMEL_EXPORT std::ostream &operator <<(std::ostream &os, const assign &arg);
EMEL_EXPORT std::ostream &operator <<(std::ostream &os, const ternary &arg);
EMEL_EXPORT std::ostream &operator <<(std::ostream &os, const bin_op &arg);
EMEL_EXPORT std::ostream &operator <<(std::ostream &os, const variable &arg);
EMEL_EXPORT std::ostream &operator <<(std::ostream &os, const un_op &arg);
EMEL_EXPORT std::ostream &operator <<(std::ostream &os, const call &arg);
EMEL_EXPORT std::ostream &operator <<(std::ostream &os, const std::vector<ast::node> &arg);

} // namespace ast

} // namespace emel
