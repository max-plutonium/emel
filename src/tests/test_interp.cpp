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

#include "../emel/runtime/interp.h"

using namespace emel;
using namespace std::literals;

using testing::Eq;

TEST(Interp, ReturnEmpty)
{
    const std::vector<value_type> const_pool {
        empty_value, "test"s, "Object"s, ""s, "~init"s
    };

    const insn_array insns {
        insn_encode(opcode::push_const, 1),
        insn_encode(opcode::ret, 0)
    };

    runtime::interp interp(const_pool,
        insns.begin(), insns.end(), 0, 1);

    auto res = interp.run();
    EXPECT_TRUE(res.empty());
}

TEST(Interp, ReturnConstant)
{
    const std::vector<value_type> const_pool {
        empty_value, "test"s, "Object"s, ""s, "~init"s
    };

    const insn_array insns {
        insn_encode(opcode::push_const, 1),
        insn_encode(opcode::ret, 1)
    };

    runtime::interp interp(const_pool,
        insns.begin(), insns.end(), 0, 1);

    auto res = interp.run();
    ASSERT_FALSE(res.empty());
    EXPECT_THAT((std::string) res, Eq("test"));
}

TEST(Interp, UnaryNot)
{
    const std::vector<value_type> const_pool {
        empty_value, "test"s, 1.23
    };

    const insn_array insns {
        insn_encode(opcode::push_const, 2),
        insn_encode(opcode::call_op, op_kind::not_),
        insn_encode(opcode::ret, 1)
    };

    runtime::interp interp(const_pool,
        insns.begin(), insns.end(), 0, 1);

    auto res = interp.run();
    ASSERT_FALSE(res.empty());
    EXPECT_FALSE((bool) res);
}

TEST(Interp, UnaryNeg)
{
    const std::vector<value_type> const_pool {
        empty_value, "test"s, 1.23
    };

    const insn_array insns {
        insn_encode(opcode::push_const, 2),
        insn_encode(opcode::call_op, op_kind::neg),
        insn_encode(opcode::ret, 1)
    };

    runtime::interp interp(const_pool,
        insns.begin(), insns.end(), 0, 1);

    auto res = interp.run();
    ASSERT_FALSE(res.empty());
    EXPECT_THAT((double) res, Eq(-1.23));
}

TEST(Interp, BinaryOr)
{
    const std::vector<value_type> const_pool {
        empty_value, "test"s, 1.23, 0.0
    };

    const insn_array insns {
        insn_encode(opcode::push_const, 3),
        insn_encode(opcode::push_const, 2),
        insn_encode(opcode::call_op, op_kind::or_),
        insn_encode(opcode::ret, 1)
    };

    runtime::interp interp(const_pool,
        insns.begin(), insns.end(), 0, 2);

    auto res = interp.run();
    ASSERT_FALSE(res.empty());
    EXPECT_TRUE((bool) res);
}

TEST(Interp, BinaryXor)
{
    const std::vector<value_type> const_pool {
        empty_value, "test"s, 1.23, 1.0
    };

    const insn_array insns {
        insn_encode(opcode::push_const, 3),
        insn_encode(opcode::push_const, 2),
        insn_encode(opcode::call_op, op_kind::xor_),
        insn_encode(opcode::ret, 1)
    };

    runtime::interp interp(const_pool,
        insns.begin(), insns.end(), 0, 2);

    auto res = interp.run();
    ASSERT_FALSE(res.empty());
    EXPECT_FALSE((bool) res);
}

TEST(Interp, BinaryAnd)
{
    const std::vector<value_type> const_pool {
        empty_value, "test"s, 1.23, 0.00001
    };

    const insn_array insns {
        insn_encode(opcode::push_const, 3),
        insn_encode(opcode::push_const, 2),
        insn_encode(opcode::call_op, op_kind::and_),
        insn_encode(opcode::ret, 1)
    };

    runtime::interp interp(const_pool,
        insns.begin(), insns.end(), 0, 2);

    auto res = interp.run();
    ASSERT_FALSE(res.empty());
    EXPECT_TRUE((bool) res);
}

TEST(Interp, BinaryRelativeOps)
{
    const std::vector<value_type> const_pool {
        empty_value, "test"s, 1.23, 1.23000, 1.230001
    };

    const insn_array insns {
        insn_encode(opcode::push_const, 3),
        insn_encode(opcode::push_const, 4),
        insn_encode(opcode::call_op, op_kind::eq),
        insn_encode(opcode::call_op, op_kind::not_),
        insn_encode(opcode::push_const, 3),
        insn_encode(opcode::push_const, 4),
        insn_encode(opcode::call_op, op_kind::ne),
        insn_encode(opcode::call_op, op_kind::and_),
        insn_encode(opcode::ret, 1)
    };

    const insn_array insns2 {
        insn_encode(opcode::push_const, 3),
        insn_encode(opcode::push_const, 2),
        insn_encode(opcode::call_op, op_kind::eq),
        insn_encode(opcode::call_op, op_kind::and_),
        insn_encode(opcode::push_const, 3),
        insn_encode(opcode::push_const, 2),
        insn_encode(opcode::call_op, op_kind::ne),
        insn_encode(opcode::call_op, op_kind::xor_),
        insn_encode(opcode::ret, 1)
    };

    runtime::interp interp(const_pool,
        insns2.begin(), insns2.end(), 0, 3);

    interp.push_frame(const_pool,
        insns.begin(), insns.end(), 0, 3);

    auto res = interp.run();
    ASSERT_FALSE(res.empty());
    EXPECT_TRUE((bool) res);
}

TEST(Interp, BinaryLessThan)
{
    const std::vector<value_type> const_pool {
        empty_value, "test"s, 1.23, 1.231
    };

    const insn_array insns {
        insn_encode(opcode::push_const, 3),
        insn_encode(opcode::push_const, 2),
        insn_encode(opcode::call_op, op_kind::lt),
        insn_encode(opcode::ret, 1)
    };

    runtime::interp interp(const_pool,
        insns.begin(), insns.end(), 0, 2);

    auto res = interp.run();
    ASSERT_FALSE(res.empty());
    EXPECT_TRUE((bool) res);
}
