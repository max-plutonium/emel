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

#include "../emel/runtime/object.h"

using namespace emel;
using namespace std::literals;

using testing::Eq;

TEST(Object, Creation)
{
    runtime::object obj1;
    runtime::object obj2(empty_value);
    runtime::object obj3("string"s);
    runtime::object obj4(1.23);
    runtime::object obj5(true);
    runtime::object obj6(false);
    runtime::object obj7(runtime::make_object("test"));

    EXPECT_EQ(runtime::object::is_empty, obj1.get_type());
    EXPECT_TRUE(obj1.empty());
    EXPECT_EQ(runtime::object::is_empty, obj2.get_type());
    EXPECT_TRUE(obj2.empty());
    EXPECT_EQ(runtime::object::is_string, obj3.get_type());
    EXPECT_FALSE(obj3.empty());
    EXPECT_EQ(runtime::object::is_number, obj4.get_type());
    EXPECT_FALSE(obj4.empty());
    EXPECT_EQ(runtime::object::is_boolean, obj5.get_type());
    EXPECT_FALSE(obj5.empty());
    EXPECT_EQ(runtime::object::is_boolean, obj6.get_type());
    EXPECT_FALSE(obj6.empty());
    EXPECT_EQ(runtime::object::is_ref, obj7.get_type());
    EXPECT_FALSE(obj7.empty());
}

TEST(Object, Copy)
{
    runtime::object obj1;
    runtime::object obj2(empty_value);
    runtime::object obj3("string"s);
    runtime::object obj4(1.23);
    runtime::object obj5(true);
    runtime::object obj6(false);
    runtime::object obj7(runtime::make_object("test"));

    runtime::object obj8(obj1);
    runtime::object obj9(obj2);
    runtime::object obj10(obj3);
    runtime::object obj11(obj4);
    runtime::object obj12(obj5);
    runtime::object obj13(obj6);
    runtime::object obj14(obj7);

    EXPECT_EQ(runtime::object::is_empty, obj8.get_type());
    EXPECT_TRUE(obj8.empty());
    EXPECT_EQ(runtime::object::is_empty, obj9.get_type());
    EXPECT_TRUE(obj9.empty());
    EXPECT_EQ(runtime::object::is_string, obj10.get_type());
    EXPECT_FALSE(obj10.empty());
    EXPECT_EQ(runtime::object::is_number, obj11.get_type());
    EXPECT_FALSE(obj11.empty());
    EXPECT_EQ(runtime::object::is_boolean, obj12.get_type());
    EXPECT_FALSE(obj12.empty());
    EXPECT_EQ(runtime::object::is_boolean, obj13.get_type());
    EXPECT_FALSE(obj13.empty());
    EXPECT_EQ(runtime::object::is_ref, obj14.get_type());
    EXPECT_FALSE(obj14.empty());
}

TEST(Object, CopyAssign)
{
    runtime::object obj1;
    runtime::object obj2(empty_value);
    runtime::object obj3("string"s);
    runtime::object obj4(1.23);
    runtime::object obj5(true);
    runtime::object obj6(false);
    runtime::object obj7(runtime::make_object("test"));

    obj1 = obj2;
    EXPECT_EQ(runtime::object::is_empty, obj1.get_type());
    EXPECT_TRUE(obj1.empty());

    obj1 = obj3;
    EXPECT_EQ(runtime::object::is_string, obj1.get_type());
    EXPECT_FALSE(obj1.empty());

    obj1 = obj4;
    EXPECT_EQ(runtime::object::is_number, obj1.get_type());
    EXPECT_FALSE(obj1.empty());

    obj1 = obj5;
    EXPECT_EQ(runtime::object::is_boolean, obj1.get_type());
    EXPECT_FALSE(obj1.empty());

    obj1 = obj6;
    EXPECT_EQ(runtime::object::is_boolean, obj1.get_type());
    EXPECT_FALSE(obj1.empty());

    obj1 = obj7;
    EXPECT_EQ(runtime::object::is_ref, obj1.get_type());
    EXPECT_FALSE(obj1.empty());
}
