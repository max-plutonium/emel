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
#include "bytecode.h"

namespace emel { namespace bytecode {

/*struct value_input_visitor : public boost::static_visitor<>
{
    std::istream &is;

    value_input_visitor(std::istream &is) : is(is) { }

  template <typename Tp>
    void operator()(Tp value) const { is >> value; }
};

std::ostream &operator <<(std::ostream &os, const std::vector<value_type> &arg)
{
    os << arg.size();
    for(const auto &entry : arg)
        os << entry;
    return os;
}

std::istream &operator >>(std::istream &is, std::vector<value_type> &arg)
{
    std::size_t size;
    is >> size;
    arg.resize(size);
    value_input_visitor vi { is };
    for(auto &entry : arg)
        boost::apply_visitor(vi, entry);
    return is;
}

std::ostream &operator <<(std::ostream &os, const insn_array &arg)
{
    os << arg.size();
    for(const auto &entry : arg)
        os << entry;
    return os;
}

std::istream &operator >>(std::istream &is, insn_array &arg)
{
    std::size_t size;
    is >> size;
    arg.resize(size);
    for(auto &entry : arg)
        is >> entry;
    return is;
}

std::ostream &operator <<(std::ostream &os, const visibility &arg)
{
    return os << static_cast<unsigned>(arg);
}

std::istream &operator >>(std::istream &is, visibility &arg)
{
    unsigned tmp;
    is >> tmp;
    arg = static_cast<visibility>(tmp);
    return is;
}

std::ostream &operator <<(std::ostream &os, const field_info &arg)
{
    return os << arg.name << arg.vsb << arg.is_static << arg.is_const;
}

std::istream &operator >>(std::istream &is, field_info &arg)
{
    bool tmp;
    is >> arg.name >> arg.vsb >> tmp;
    arg.is_static = tmp;
    is >> tmp;
    arg.is_const = tmp;
    return is;
}

std::ostream &operator <<(std::ostream &os, const method_info &arg)
{
    return os << arg.name << arg.nr_args << arg.vsb
              << arg.is_native << arg.is_static << arg.is_virtual
              << arg.is_pure << arg.code;
}

std::istream &operator >>(std::istream &is, method_info &arg)
{
    bool tmp;
    is >> arg.name >> arg.nr_args >> arg.vsb;
    is >> tmp;
    arg.is_native = tmp;
    is >> tmp;
    arg.is_static = tmp;
    is >> tmp;
    arg.is_virtual = tmp;
    is >> tmp;
    arg.is_pure = tmp;
    is >> arg.code;
    return is;
}

std::ostream &operator <<(std::ostream &os, const std::vector<field_info> &arg)
{
    os << arg.size();
    for(const auto &entry : arg)
        os << entry;
    return os;
}

std::istream &operator >>(std::istream &is, std::vector<field_info> &arg)
{
    std::size_t size;
    is >> size;
    arg.resize(size);
    for(auto &entry : arg)
        is >> entry;
    return is;
}

std::ostream &operator <<(std::ostream &os, const std::vector<method_info> &arg)
{
    os << arg.size();
    for(const auto &entry : arg)
        os << entry;
    return os;
}

std::istream &operator >>(std::istream &is, std::vector<method_info> &arg)
{
    std::size_t size;
    is >> size;
    arg.resize(size);
    for(auto &entry : arg)
        is >> entry;
    return is;
}

std::ostream &operator <<(std::ostream &os, const class_info &arg)
{
    return os << arg.name << arg.base_name << arg.const_pool
              << arg.vsb << arg.is_abstract << arg.is_interface
              << arg.fields << arg.methods;
}

std::istream &operator >>(std::istream &is, class_info &arg)
{
    bool tmp;
    is >> arg.name >> arg.base_name >> arg.const_pool >> arg.vsb;
    is >> tmp;
    arg.is_abstract = tmp;
    is >> tmp;
    arg.is_interface = tmp;
    is >> arg.fields >> arg.methods;
    return is;
}*/

} // namespace bytecode

} // namespace emel
