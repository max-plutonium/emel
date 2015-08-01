/*
 * Copyright (C) 2015 Max Plutonium <plutonium.max@gmail.com>
 *
 * This file is part of the test suite of the EMEL library.
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
#include <gmock/gmock.h>
#include "../emel/opcodes.h"

using namespace emel;

TEST(OpCodes, EncodeDecode)
{
    opcode op = opcode::debug_break;
    opcode op2 = opcode::push_const;
    auto encoded_op = emel_insn_encode(op, 123);
    auto encoded_op2 = emel_insn_encode(op2, 65535);
    opcode tmp;
    decltype(encoded_op) idx;
    emel_insn_decode(encoded_op, &tmp, &idx);
    EXPECT_EQ(op, tmp);
    EXPECT_EQ(idx, 123);
    emel_insn_decode(encoded_op2, &tmp, &idx);
    EXPECT_EQ(op2, tmp);
    EXPECT_EQ(idx, 65535);
}
