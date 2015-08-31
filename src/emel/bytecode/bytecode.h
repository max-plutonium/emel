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
#ifndef BYTECODE_H
#define BYTECODE_H

#include "../opcodes.h"

#include <boost/variant/variant.hpp>

namespace emel { namespace bytecode {

using value_type = boost::variant<std::string, double, bool>;
using insn_array = std::vector<insn_type>;

enum class visibility {
    public_, protected_, module, private_
};

struct field_info {
    std::size_t class_name_index, name_index;
    visibility vsb;
    std::uint32_t is_static : 1;
    std::uint32_t is_const : 1;
    std::uint32_t flags : 30;
};

struct method_info {
    std::size_t class_name_index, name_index;
    std::size_t nr_args;
    visibility vsb;
    std::uint32_t is_native : 1;
    std::uint32_t is_static : 1;
    std::uint32_t is_virtual : 1;
    std::uint32_t is_pure : 1;
    std::uint32_t flags : 28;
    insn_array code;
};

struct class_info
{
    std::vector<value_type> const_pool;
    std::size_t name_index, base_name_index;
    visibility vsb;
    std::uint32_t is_abstract : 1;
    std::uint32_t is_interface : 1;
    std::uint32_t flags : 30;
    std::vector<field_info> fields, static_fields;
    std::vector<method_info> methods, static_methods;
};

/*std::ostream &operator <<(std::ostream &os, const std::vector<value_type> &arg);
std::istream &operator >>(std::istream &is, std::vector<value_type> &arg);
std::ostream &operator <<(std::ostream &os, const insn_array &arg);
std::istream &operator >>(std::istream &is, insn_array &arg);
std::ostream &operator <<(std::ostream &os, const visibility &arg);
std::istream &operator >>(std::istream &is, visibility &arg);
std::ostream &operator <<(std::ostream &os, const field_info &arg);
std::istream &operator >>(std::istream &is, field_info &arg);
std::ostream &operator <<(std::ostream &os, const method_info &arg);
std::istream &operator >>(std::istream &is, method_info &arg);
std::ostream &operator <<(std::ostream &os, const std::vector<field_info> &arg);
std::istream &operator >>(std::istream &is, std::vector<field_info> &arg);
std::ostream &operator <<(std::ostream &os, const std::vector<method_info> &arg);
std::istream &operator >>(std::istream &is, std::vector<method_info> &arg);
std::ostream &operator <<(std::ostream &os, const class_info &arg);
std::istream &operator >>(std::istream &is, class_info &arg);*/

} // namespace bytecode

} // namespace emel

#endif // BYTECODE_H
