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
#include "insns_manager.h"
#include "../opcodes.h"

namespace emel { namespace bytecode {

insn_array &insns_manager::get_code()
{
    return methods.back().code;
}

const insn_array &insns_manager::get_code() const
{
    return methods.back().code;
}

insns_manager::insns_manager(std::vector<method_info> &methods)
    : methods(methods)
{
}

std::size_t insns_manager::allocate_slot()
{
    insn_array &insns = get_code();
    opcode op; std::uint32_t num_of_slots;
    std::tie(op, num_of_slots) = insn_decode(insns[frame_idx_stack.top()]);
    ++num_of_slots;
    insns[frame_idx_stack.top()] = insn_encode(op, num_of_slots);
    return num_of_slots - 1;
}

std::size_t insns_manager::num_slots() const
{
    const insn_array &insns = get_code();
    return insn_decode(insns[frame_idx_stack.top()]).second;
}

std::size_t insns_manager::emit_insn(opcode op, std::uint32_t idx)
{
    auto &insns = get_code();
    insns.emplace_back(insn_encode(op, idx));
    return insns.size() - 1;
}

void insns_manager::edit_insn(std::size_t i, opcode op, std::uint32_t idx)
{
    get_code()[i] = insn_encode(op, idx);
}

std::size_t insns_manager::last_insn_index() const
{
    return get_code().size() - 1;
}

void insns_manager::start_frame()
{
    frame_idx_stack.push(get_code().size());
}

void insns_manager::fini_frame()
{
    frame_idx_stack.pop();
}

} // namespace bytecode

} // namespace emel
