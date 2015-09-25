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
#include "opcodes.h"

#include <sstream>

namespace emel {

empty_value_type empty_value;

std::pair<opcode, std::uint32_t> insn_decode(insn_type insn)
{
    return std::make_pair(static_cast<opcode>(insn & 0x1F), insn >> 5);
}

insn_type insn_encode(opcode op, std::uint32_t idx)
{
    return (idx << 5) + static_cast<std::uint32_t>(op);
}

const char *opcode_name(opcode op)
{
    const char *result = "undefined-opcode";

# define OPCODE_NAME(OP, STR) \
    case OP: result = STR; break;

    switch (op) {
        OPCODE_NAME(opcode::spec, "spec")
        OPCODE_NAME(opcode::pop, "pop")
        OPCODE_NAME(opcode::dup, "dup")
        OPCODE_NAME(opcode::swap, "swap")
        OPCODE_NAME(opcode::push_const, "push-const")
        OPCODE_NAME(opcode::push_local, "push-local")
        OPCODE_NAME(opcode::push_field, "push-field")
        OPCODE_NAME(opcode::push_static, "push-static")
        OPCODE_NAME(opcode::load_local, "load-local")
        OPCODE_NAME(opcode::load_field, "load-field")
        OPCODE_NAME(opcode::load_static, "load-static")
        OPCODE_NAME(opcode::push_frame, "push-frame")
        OPCODE_NAME(opcode::drop_frame, "drop-frame")
        OPCODE_NAME(opcode::throw_, "throw")
        OPCODE_NAME(opcode::try_, "try")
        OPCODE_NAME(opcode::end_try, "end-try")
        OPCODE_NAME(opcode::call_op, "call-op")
        OPCODE_NAME(opcode::call, "call")
        OPCODE_NAME(opcode::fcall, "fcall")
        OPCODE_NAME(opcode::call_static, "call-static")
        OPCODE_NAME(opcode::fcall_static, "fcall-static")
        OPCODE_NAME(opcode::brf, "brf")
        OPCODE_NAME(opcode::brb, "brb")
        OPCODE_NAME(opcode::brf_true, "brf-true")
        OPCODE_NAME(opcode::brf_false, "brf-false")
        OPCODE_NAME(opcode::brb_true, "brb-true")
        OPCODE_NAME(opcode::brb_false, "brb-false")
        OPCODE_NAME(opcode::ret, "ret")

        default:
            break;
    }

# undef OPCODE_NAME

    return result;
}

std::string insn_to_string(insn_type insn)
{
    auto pair = insn_decode(insn);
    std::ostringstream oss;
    oss << opcode_name(pair.first) << ' ' << pair.second;
    return oss.str();
}

std::ostream &operator <<(std::ostream &os, empty_value_type)
{
    return os << "empty-value";
}

} // namespace emel
