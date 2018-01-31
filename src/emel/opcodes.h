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

#include <cstdint>
#include <deque>
#include <utility>

#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_LIST_SIZE 30
#define BOOST_MPL_LIMIT_VECTOR_SIZE 30

#include <boost/variant.hpp>

namespace emel {

using insn_type = std::uint32_t;

enum class opcode : unsigned char {
    spec, ///< Special commands
    pop, ///< Pop value from the stack
    dup, ///< Duplicate value on top of the stack
    swap, ///< Exchange value on top of the stack and the value beneath it
    ret, ///< Return from function

    push, ///< Push integer on the stack
    push_const, ///< Push contant value from the pool on the stack
    push_local, ///< Push local variable on the stack
    load_local, ///< Load value from top of the stack to the local variable
    call_op, ///< Invoke builtin operator

    push_field, ///< Push object field on the stack
    load_field, ///< Load value from top of the stack to the object field
    brf, ///< Unconditional forward branch
    brb, ///< Unconditional backward branch
    brf_true, ///< Conditional forward branch, if top of the stack is true

    brf_false, ///< Conditional forward branch, if top of the stack is false
    brb_true, ///< Conditional backward branch, if top of the stack is true
    brb_false, ///< Conditional backward branch, if top of the stack is false
    br_table, ///< Make branch table
    amake, ///< Make array

    astore, ///< Store value to array
    aload, ///< Load value from array
    push_frame, // Создать новый кадр
    drop_frame, // Выйти из текущего кадра
    raise, // Бросить исключение

    try_, // Начало блока try
    end_try, // Конец блока try
    call, // Вызов функции по имени
    fcall, // Вызов функции по номеру
    max_opcode
};

using insn_array = std::deque<insn_type>;
extern struct empty_value_type {} empty_value;
using value_type = boost::variant<empty_value_type, std::string, double, bool>;

enum class op_kind {
    not_ = 101, neg,
    or_ = 201, xor_, and_, eq, ne, lt, gt, lte, gte, add, sub, mul, div
};

EMEL_EXPORT std::pair<opcode, std::uint32_t> insn_decode(insn_type insn);
EMEL_EXPORT insn_type insn_encode(opcode op, std::uint32_t idx = 0);
EMEL_EXPORT insn_type insn_encode(opcode op, op_kind k);

const char *opcode_name(opcode op);
const char *opkind_name(op_kind op);

std::string insn_to_string(insn_type insn);

EMEL_EXPORT std::ostream &operator <<(std::ostream &os, empty_value_type);
EMEL_EXPORT std::ostream &operator <<(std::ostream &os, const insn_array &);

} // namespace emel
