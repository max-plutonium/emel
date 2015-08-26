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

namespace emel {

std::pair<opcode, std::uint32_t> insn_decode(insn_type insn)
{
    return std::make_pair(static_cast<opcode>(insn & 0x1F), insn >> 5);
}

insn_type insn_encode(opcode op, std::uint32_t idx)
{
    return (idx << 5) + static_cast<std::uint32_t>(op);
}

} // namespace emel
