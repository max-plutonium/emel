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

#include <emel/runtime/interp.h>

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
    EXPECT_THAT(res.as_string().value(), Eq("test"));
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
    EXPECT_FALSE(res.as_bool().value());
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
    EXPECT_THAT(res.as_number().value(), Eq(-1.23));
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
    EXPECT_TRUE(res.as_bool().value());
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
    EXPECT_FALSE(res.as_bool().value());
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
    EXPECT_TRUE(res.as_bool().value());
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
    EXPECT_TRUE(res.as_bool().value());
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
    EXPECT_TRUE(res.as_bool().value());
}

TEST(Interp, BinaryGreaterThan)
{
    const std::vector<value_type> const_pool {
        empty_value, "test"s, 1.23, 1.231
    };

    const insn_array insns {
        insn_encode(opcode::push_const, 2),
        insn_encode(opcode::push_const, 3),
        insn_encode(opcode::call_op, op_kind::gt),
        insn_encode(opcode::ret, 1)
    };

    runtime::interp interp(const_pool,
        insns.begin(), insns.end(), 0, 2);

    auto res = interp.run();
    ASSERT_FALSE(res.empty());
    EXPECT_TRUE(res.as_bool().value());
}

TEST(Interp, BinaryLTE)
{
    const std::vector<value_type> const_pool {
        empty_value, "test"s, 1.23, 1.231, 1.23000, 2.34
    };

    const insn_array insns {
        insn_encode(opcode::push_const, 3),
        insn_encode(opcode::push_const, 2),
        insn_encode(opcode::call_op, op_kind::lte),
        insn_encode(opcode::push_const, 4),
        insn_encode(opcode::push_const, 2),
        insn_encode(opcode::call_op, op_kind::lte),
        insn_encode(opcode::call_op, op_kind::and_),
        insn_encode(opcode::push_const, 4),
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::call_op, op_kind::lte),
        insn_encode(opcode::call_op, op_kind::xor_),
        insn_encode(opcode::ret, 1)
    };

    runtime::interp interp(const_pool,
        insns.begin(), insns.end(), 0, 3);

    auto res = interp.run();
    ASSERT_FALSE(res.empty());
    EXPECT_TRUE(res.as_bool().value());
}

TEST(Interp, BinaryGTE)
{
    const std::vector<value_type> const_pool {
        empty_value, "test"s, 1.23, 1.231, 1.23000, 2.34
    };

    const insn_array insns {
        insn_encode(opcode::push_const, 2),
        insn_encode(opcode::push_const, 3),
        insn_encode(opcode::call_op, op_kind::gte),
        insn_encode(opcode::push_const, 4),
        insn_encode(opcode::push_const, 2),
        insn_encode(opcode::call_op, op_kind::gte),
        insn_encode(opcode::call_op, op_kind::and_),
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::push_const, 4),
        insn_encode(opcode::call_op, op_kind::gte),
        insn_encode(opcode::call_op, op_kind::xor_),
        insn_encode(opcode::ret, 1)
    };

    runtime::interp interp(const_pool,
        insns.begin(), insns.end(), 0, 3);

    auto res = interp.run();
    ASSERT_FALSE(res.empty());
    EXPECT_TRUE(res.as_bool().value());
}

TEST(Interp, BinaryAdd)
{
    const std::vector<value_type> const_pool {
        empty_value, "test"s, 1.23, 2.34
    };

    const insn_array insns {
        insn_encode(opcode::push_const, 2),
        insn_encode(opcode::push_const, 3),
        insn_encode(opcode::call_op, op_kind::add),
        insn_encode(opcode::ret, 1)
    };

    runtime::interp interp(const_pool,
        insns.begin(), insns.end(), 0, 2);

    auto res = interp.run();
    ASSERT_FALSE(res.empty());
    EXPECT_EQ(1.23 + 2.34, res.as_number().value());
}

TEST(Interp, BinarySub)
{
    const std::vector<value_type> const_pool {
        empty_value, "test"s, 1.23, 2.34
    };

    const insn_array insns {
        insn_encode(opcode::push_const, 3),
        insn_encode(opcode::push_const, 2),
        insn_encode(opcode::call_op, op_kind::sub),
        insn_encode(opcode::ret, 1)
    };

    runtime::interp interp(const_pool,
        insns.begin(), insns.end(), 0, 2);

    auto res = interp.run();
    ASSERT_FALSE(res.empty());
    EXPECT_EQ(1.23 - 2.34, res.as_number().value());
}

TEST(Interp, BinaryMul)
{
    const std::vector<value_type> const_pool {
        empty_value, "test"s, 1.23, 2.34
    };

    const insn_array insns {
        insn_encode(opcode::push_const, 3),
        insn_encode(opcode::push_const, 2),
        insn_encode(opcode::call_op, op_kind::mul),
        insn_encode(opcode::ret, 1)
    };

    runtime::interp interp(const_pool,
        insns.begin(), insns.end(), 0, 2);

    auto res = interp.run();
    ASSERT_FALSE(res.empty());
    EXPECT_EQ(1.23 * 2.34, res.as_number().value());
}

TEST(Interp, BinaryDiv)
{
    const std::vector<value_type> const_pool {
        empty_value, "test"s, 1.23, 2.34
    };

    const insn_array insns {
        insn_encode(opcode::push_const, 2),
        insn_encode(opcode::push_const, 3),
        insn_encode(opcode::call_op, op_kind::div),
        insn_encode(opcode::ret, 1)
    };

    runtime::interp interp(const_pool,
        insns.begin(), insns.end(), 0, 2);

    auto res = interp.run();
    ASSERT_FALSE(res.empty());
    EXPECT_EQ(2.34 / 1.23, res.as_number().value());
}

TEST(Interp, PushPopLocals)
{
    const std::vector<value_type> const_pool {
        empty_value, "test"s
    };

    const insn_array insns {
        insn_encode(opcode::push_const, 1),
        insn_encode(opcode::load_local, 0),
        insn_encode(opcode::push_local, 0),
        insn_encode(opcode::ret, 1)
    };

    runtime::interp interp(const_pool,
        insns.begin(), insns.end(), 1, 1);

    auto res = interp.run();
    ASSERT_FALSE(res.empty());
    EXPECT_EQ("test", res.as_string().value());
}

TEST(Interp, DupAndPop)
{
    const std::vector<value_type> const_pool {
        empty_value, "test"s, 3.0
    };

    const insn_array insns {
        insn_encode(opcode::push_const, 2),
        insn_encode(opcode::dup),
        insn_encode(opcode::dup, 1),
        insn_encode(opcode::pop),
        insn_encode(opcode::pop, 1),
        insn_encode(opcode::dup, 2),
        insn_encode(opcode::load_local, 0),
        insn_encode(opcode::push_const, 1),
        insn_encode(opcode::call_op, op_kind::mul),
        insn_encode(opcode::load_local, 0),
        insn_encode(opcode::dup, 4),
        insn_encode(opcode::pop, 2),
        insn_encode(opcode::call_op, op_kind::add),
        insn_encode(opcode::push_local, 0),
        insn_encode(opcode::call_op, op_kind::add),
        insn_encode(opcode::call_op, op_kind::add),
        insn_encode(opcode::ret, 1)
    };

    runtime::interp interp(const_pool,
        insns.begin(), insns.end(), 1, 1);

    auto res = interp.run();
    ASSERT_FALSE(res.empty());
    EXPECT_EQ("testtesttest6.03.0", res.as_string().value());
}

TEST(Interp, Swap)
{
    const std::vector<value_type> const_pool {
        empty_value, 2.0, 3.0
    };

    const insn_array insns {
        insn_encode(opcode::push_const, 1),
        insn_encode(opcode::push_const, 2),
        insn_encode(opcode::call_op, op_kind::sub),
        insn_encode(opcode::push_const, 2),
        insn_encode(opcode::push_const, 1),
        insn_encode(opcode::swap),
        insn_encode(opcode::call_op, op_kind::sub),
        insn_encode(opcode::call_op, op_kind::eq),
        insn_encode(opcode::ret, 1)
    };

    runtime::interp interp(const_pool,
        insns.begin(), insns.end(), 1, 1);

    auto res = interp.run();
    ASSERT_FALSE(res.empty());
    EXPECT_TRUE(res.as_bool().value());
}

TEST(Interp, Branches)
{
    const std::vector<value_type> const_pool {
        empty_value, 1.0, 2.0, 3.0
    };

    const insn_array insns {
        insn_encode(opcode::push_const, 2),
        insn_encode(opcode::push_const, 3),
        insn_encode(opcode::brf, 2),
        insn_encode(opcode::push_const, 1),
        insn_encode(opcode::call_op, op_kind::sub),
        insn_encode(opcode::dup, 2),
        insn_encode(opcode::brf_false, 2),
        insn_encode(opcode::brb_true, 4),

        insn_encode(opcode::pop),
        insn_encode(opcode::push_const, 1),
        insn_encode(opcode::push_const, 3),
        insn_encode(opcode::call_op, op_kind::sub),
        insn_encode(opcode::push_local, 0),
        insn_encode(opcode::brf_true, 6),
        insn_encode(opcode::push_const, 1),
        insn_encode(opcode::load_local, 0),
        insn_encode(opcode::push_const, 2),
        insn_encode(opcode::call_op, op_kind::eq),
        insn_encode(opcode::brb_true, 15),

        insn_encode(opcode::push_const, 0),
        insn_encode(opcode::load_local, 0),
        insn_encode(opcode::push_local, 0),
        insn_encode(opcode::brf_true, 6),
        insn_encode(opcode::push_const, 1),
        insn_encode(opcode::load_local, 0),
        insn_encode(opcode::brb, 4),

        insn_encode(opcode::call_op, op_kind::sub),
        insn_encode(opcode::ret, 1),
        insn_encode(opcode::push_local, 0),
        insn_encode(opcode::brb_false, 3),
        insn_encode(opcode::push_const, 0),
        insn_encode(opcode::load_local, 0),
        insn_encode(opcode::brb, 4)
    };

    runtime::interp interp(const_pool,
        insns.begin(), insns.end(), 1, 1);

    auto res = interp.run();
    ASSERT_FALSE(res.empty());
    EXPECT_EQ(2, res.as_number().value());
}
