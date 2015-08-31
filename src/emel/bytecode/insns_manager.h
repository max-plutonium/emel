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
#ifndef INSNS_MANAGER_H
#define INSNS_MANAGER_H

#include "bytecode.h"

#include <stack>
#include <vector>

namespace emel { namespace bytecode {

class insns_manager
{
    std::stack<std::size_t, std::vector<std::size_t>> frame_idx_stack;
    std::vector<method_info> &methods;

    insn_array &get_code();
    const insn_array &get_code() const;

public:
    explicit insns_manager(std::vector<method_info> &methods);
    std::size_t allocate_slot();
    std::size_t num_slots() const;
    std::size_t emit_insn(opcode op, std::uint32_t idx = 0);
    void edit_insn(std::size_t i, opcode op, std::uint32_t idx = 0);
    std::size_t last_insn_index() const;
    void start_frame();
    void fini_frame();
};

} // namespace bytecode

} // namespace emel

#endif // INSNS_MANAGER_H
