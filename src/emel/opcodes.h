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
#ifndef OPCODES_H
#define OPCODES_H

#include <cstdint>

namespace emel EMEL_EXPORT {

enum class opcode : unsigned char {
    halt,
    push_const // Загрузить константу из пула в стек
};

} // namespace emel

extern "C" {

EMEL_EXPORT void
emel_insn_decode(std::uint32_t insn, emel::opcode *op, std::uint32_t *idx);

EMEL_EXPORT std::uint32_t
emel_insn_encode(emel::opcode op, std::uint32_t idx);

} // extern "C"

#endif // OPCODES_H
