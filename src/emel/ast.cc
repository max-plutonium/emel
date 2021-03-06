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
#include "ast.h"

#include <mutex>
#include <unordered_map>

namespace emel { namespace ast {

static std::mutex s_pos_lock;
static std::unordered_map<std::string, std::shared_ptr<std::string>> s_pos_map;

position::position(const std::string &file, std::size_t line, std::size_t column)
    : file(), line(line), column(column)
{
    std::lock_guard<decltype(s_pos_lock)> lk(s_pos_lock);
    auto pair = s_pos_map.emplace(file, nullptr);
    if(pair.second)
        this->file = pair.first->second = std::make_shared<std::string>(file);
    else
        this->file = pair.first->second;
}

std::ostream &operator <<(std::ostream &os, const class_ &arg)
{
    os << "class " << arg.name;
    if(!arg.base_name.empty())
        os << " : " << arg.base_name;
    return os;
}

std::ostream &operator <<(std::ostream &os, const param &arg)
{
    os << "param " << arg.name;
    if(arg.by_ref)
        os << " by ref";
    return os;
}

std::ostream &operator <<(std::ostream &os, const std::vector<param> &arg)
{
    os << "params [";
    for(const param &p : arg)
        os << p;
    return os << "]";
}

std::ostream &operator <<(std::ostream &os, const method &arg)
{
    os << "method " << arg.name << "( ";
    for(const param &p : arg.params)
        os << p << ",";
    return os << ")";
}

std::ostream &operator <<(std::ostream &os, const std::vector<method> &arg)
{
    os << "methods [";
    for(const method &m : arg)
        os << m;
    return os << "]";
}

std::ostream &operator <<(std::ostream &os, const while_ &arg)
{
    return os << "while (" << arg.cond << ")";
}

std::ostream &operator <<(std::ostream &os, const for_ &arg)
{
    return os << "for (" << arg.init << ";" << arg.cond << ";" << arg.step << ")";
}

std::ostream &operator <<(std::ostream &os, const if_ &arg)
{
    return os << "if (" << arg.cond << ")";
}

std::ostream &operator <<(std::ostream &os, const case_ &arg)
{
    return os << "case " << arg.match_values;
}

std::ostream &operator <<(std::ostream &os, const switch_ &arg)
{
    return os << "witch (" << arg.cond << ") {\n" << arg.blocks << "\n}";
}

std::ostream &operator <<(std::ostream &os, const continue_ &)
{
    return os << "continue";
}

std::ostream &operator <<(std::ostream &os, const break_ &)
{
    return os << "break";
}

std::ostream &operator <<(std::ostream &os, const return_ &arg)
{
    return os << "return " << arg.e;
}

std::ostream &operator <<(std::ostream &os, const try_ &arg)
{
    return os << "try " << arg.exprs << " catch";
}

std::ostream &operator <<(std::ostream &os, const assign &arg)
{
    os << arg.var_name;
    if(arg.as_external)
        os << " as external ";
    return os << " = " << arg.rhs;
}

std::ostream &operator <<(std::ostream &os, const ternary &arg)
{
    return os << arg.cond << " ? " << arg.first << " : " << arg.second;
}

std::ostream &operator <<(std::ostream &os, const bin_op &arg)
{
    os << "(" << arg.lhs;
    switch(arg.k) {
        case op_kind::or_:
            os << " | ";
            break;
        case op_kind::xor_:
            os << " ^ ";
            break;
        case op_kind::and_:
            os << " & ";
            break;
        case op_kind::eq:
            os << " == ";
            break;
        case op_kind::ne:
            os << " != ";
            break;
        case op_kind::lt:
            os << " < ";
            break;
        case op_kind::gt:
            os << " > ";
            break;
        case op_kind::lte:
            os << " <= ";
            break;
        case op_kind::gte:
            os << " >= ";
            break;
        case op_kind::add:
            os << " + ";
            break;
        case op_kind::sub:
            os << " - ";
            break;
        case op_kind::mul:
            os << " * ";
            break;
        case op_kind::div:
            os << " / ";
            break;
        default: break;
    }

    return os << arg.rhs << ")";
}

std::ostream &operator <<(std::ostream &os, const variable &arg)
{
    if(arg.ref_of)
        os << "ref of ";
    else if(arg.val_of)
        os << "val of ";
    return os << arg.name;
}

std::ostream &operator <<(std::ostream &os, const un_op &arg)
{
    switch(arg.k) {
        case op_kind::not_:
            os << "~ ";
            break;
        case op_kind::neg:
            os << "- ";
            break;
        default: break;
    }

    return os << arg.rhs;
}

std::ostream &operator <<(std::ostream &os, const call &arg)
{
    for(auto &name : arg.names)
        os << name << ".";
    os << "(";
    for(auto &a : arg.args)
        os << a << ",";
    return os << ")" << arg.chain_call;
}

std::ostream &operator <<(std::ostream &os, const std::vector<node> &arg)
{
    for(const node &n : arg)
        os << n;
    return os;
}

} // namespace ast

} // namespace emel
