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

using testing::Eq;

TEST(Object, Creation)
{
    runtime::object obj1;
    runtime::object obj2(empty_value);
    runtime::object obj3("string");
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
    auto ptr = runtime::make_object("test");

    runtime::object obj1;
    runtime::object obj2(empty_value);
    runtime::object obj3("string");
    runtime::object obj4(1.23);
    runtime::object obj5(true);
    runtime::object obj6(false);
    runtime::object obj7(ptr);

    runtime::object obj8(obj1);
    runtime::object obj9(obj2);
    runtime::object obj10(obj3);
    runtime::object obj11(obj4);
    runtime::object obj12(obj5);
    runtime::object obj13(obj6);
    runtime::object obj14(obj7);

    EXPECT_EQ(runtime::object::is_empty, obj1.get_type());
    EXPECT_TRUE(obj1.empty());
    EXPECT_EQ(runtime::object::is_empty, obj2.get_type());
    EXPECT_TRUE(obj2.empty());

    EXPECT_EQ(runtime::object::is_string, obj3.get_type());
    EXPECT_FALSE(obj3.empty());
    EXPECT_EQ("string", static_cast<std::string>(obj3));

    EXPECT_EQ(runtime::object::is_number, obj4.get_type());
    EXPECT_FALSE(obj4.empty());
    EXPECT_EQ(1.23, static_cast<double>(obj4));

    EXPECT_EQ(runtime::object::is_boolean, obj5.get_type());
    EXPECT_FALSE(obj5.empty());
    EXPECT_TRUE(static_cast<bool>(obj5));

    EXPECT_EQ(runtime::object::is_boolean, obj6.get_type());
    EXPECT_FALSE(obj6.empty());
    EXPECT_FALSE(static_cast<bool>(obj6));

    EXPECT_EQ(runtime::object::is_ref, obj7.get_type());
    EXPECT_FALSE(obj7.empty());
    EXPECT_EQ(ptr, static_cast<runtime::reference>(obj7));


    EXPECT_EQ(runtime::object::is_empty, obj8.get_type());
    EXPECT_TRUE(obj8.empty());
    EXPECT_EQ(runtime::object::is_empty, obj9.get_type());
    EXPECT_TRUE(obj9.empty());

    EXPECT_EQ(runtime::object::is_string, obj10.get_type());
    EXPECT_FALSE(obj10.empty());
    EXPECT_EQ("string", static_cast<std::string>(obj10));

    EXPECT_EQ(runtime::object::is_number, obj11.get_type());
    EXPECT_FALSE(obj11.empty());
    EXPECT_EQ(1.23, static_cast<double>(obj11));

    EXPECT_EQ(runtime::object::is_boolean, obj12.get_type());
    EXPECT_FALSE(obj12.empty());
    EXPECT_TRUE(static_cast<bool>(obj12));

    EXPECT_EQ(runtime::object::is_boolean, obj13.get_type());
    EXPECT_FALSE(obj13.empty());
    EXPECT_FALSE(static_cast<bool>(obj13));

    EXPECT_EQ(runtime::object::is_ref, obj14.get_type());
    EXPECT_FALSE(obj14.empty());
    EXPECT_EQ(ptr, static_cast<runtime::reference>(obj14));
}

TEST(Object, CopyAssign)
{
    auto ptr = runtime::make_object("test");

    runtime::object obj1;
    runtime::object obj2(empty_value);
    runtime::object obj3("string");
    runtime::object obj4(1.23);
    runtime::object obj5(true);
    runtime::object obj6(false);
    runtime::object obj7(ptr);

    obj1 = obj2;
    EXPECT_EQ(runtime::object::is_empty, obj1.get_type());
    EXPECT_TRUE(obj1.empty());

    obj1 = obj3;
    EXPECT_EQ(runtime::object::is_string, obj1.get_type());
    EXPECT_FALSE(obj1.empty());
    EXPECT_EQ("string", static_cast<std::string>(obj1));

    obj1 = obj4;
    EXPECT_EQ(runtime::object::is_number, obj1.get_type());
    EXPECT_FALSE(obj1.empty());
    EXPECT_EQ(1.23, static_cast<double>(obj1));

    obj1 = obj5;
    EXPECT_EQ(runtime::object::is_boolean, obj1.get_type());
    EXPECT_FALSE(obj1.empty());
    EXPECT_TRUE(static_cast<bool>(obj1));

    obj1 = obj6;
    EXPECT_EQ(runtime::object::is_boolean, obj1.get_type());
    EXPECT_FALSE(obj1.empty());
    EXPECT_FALSE(static_cast<bool>(obj1));

    obj1 = obj7;
    EXPECT_EQ(runtime::object::is_ref, obj1.get_type());
    EXPECT_FALSE(obj1.empty());
    EXPECT_EQ(ptr, static_cast<runtime::reference>(obj1));


    EXPECT_EQ(runtime::object::is_empty, obj2.get_type());
    EXPECT_TRUE(obj2.empty());

    EXPECT_EQ(runtime::object::is_string, obj3.get_type());
    EXPECT_FALSE(obj3.empty());
    EXPECT_EQ("string", static_cast<std::string>(obj3));

    EXPECT_EQ(runtime::object::is_number, obj4.get_type());
    EXPECT_FALSE(obj4.empty());
    EXPECT_EQ(1.23, static_cast<double>(obj4));

    EXPECT_EQ(runtime::object::is_boolean, obj5.get_type());
    EXPECT_FALSE(obj5.empty());
    EXPECT_TRUE(static_cast<bool>(obj5));

    EXPECT_EQ(runtime::object::is_boolean, obj6.get_type());
    EXPECT_FALSE(obj6.empty());
    EXPECT_FALSE(static_cast<bool>(obj6));

    EXPECT_EQ(runtime::object::is_ref, obj7.get_type());
    EXPECT_FALSE(obj7.empty());
    EXPECT_EQ(ptr, static_cast<runtime::reference>(obj7));
}

TEST(Object, Move)
{
    auto ptr = runtime::make_object("test");

    runtime::object obj1;
    runtime::object obj2(empty_value);
    runtime::object obj3("string");
    runtime::object obj4(1.23);
    runtime::object obj5(true);
    runtime::object obj6(false);
    runtime::object obj7(ptr);

    runtime::object obj8(std::move(obj1));
    runtime::object obj9(std::move(obj2));
    runtime::object obj10(std::move(obj3));
    runtime::object obj11(std::move(obj4));
    runtime::object obj12(std::move(obj5));
    runtime::object obj13(std::move(obj6));
    runtime::object obj14(std::move(obj7));

    EXPECT_EQ(runtime::object::is_empty, obj1.get_type());
    EXPECT_TRUE(obj1.empty());
    EXPECT_EQ(runtime::object::is_empty, obj2.get_type());
    EXPECT_TRUE(obj2.empty());
    EXPECT_EQ(runtime::object::is_empty, obj3.get_type());
    EXPECT_TRUE(obj3.empty());
    EXPECT_EQ(runtime::object::is_empty, obj4.get_type());
    EXPECT_TRUE(obj4.empty());
    EXPECT_EQ(runtime::object::is_empty, obj5.get_type());
    EXPECT_TRUE(obj5.empty());
    EXPECT_EQ(runtime::object::is_empty, obj6.get_type());
    EXPECT_TRUE(obj6.empty());
    EXPECT_EQ(runtime::object::is_empty, obj7.get_type());
    EXPECT_TRUE(obj7.empty());


    EXPECT_EQ(runtime::object::is_empty, obj8.get_type());
    EXPECT_TRUE(obj8.empty());
    EXPECT_EQ(runtime::object::is_empty, obj9.get_type());
    EXPECT_TRUE(obj9.empty());

    EXPECT_EQ(runtime::object::is_string, obj10.get_type());
    EXPECT_FALSE(obj10.empty());
    EXPECT_EQ("string", static_cast<std::string>(obj10));

    EXPECT_EQ(runtime::object::is_number, obj11.get_type());
    EXPECT_FALSE(obj11.empty());
    EXPECT_EQ(1.23, static_cast<double>(obj11));

    EXPECT_EQ(runtime::object::is_boolean, obj12.get_type());
    EXPECT_FALSE(obj12.empty());
    EXPECT_TRUE(static_cast<bool>(obj12));

    EXPECT_EQ(runtime::object::is_boolean, obj13.get_type());
    EXPECT_FALSE(obj13.empty());
    EXPECT_FALSE(static_cast<bool>(obj13));

    EXPECT_EQ(runtime::object::is_ref, obj14.get_type());
    EXPECT_FALSE(obj14.empty());
    EXPECT_EQ(ptr, static_cast<runtime::reference>(obj14));
}

TEST(Object, MoveAssign)
{
    auto ptr = runtime::make_object("test");

    runtime::object obj1;
    runtime::object obj2(empty_value);
    runtime::object obj3("string");
    runtime::object obj4(1.23);
    runtime::object obj5(true);
    runtime::object obj6(false);
    runtime::object obj7(ptr);

    obj1 = std::move(obj2);
    EXPECT_EQ(runtime::object::is_empty, obj1.get_type());
    EXPECT_TRUE(obj1.empty());

    obj1 = std::move(obj3);
    EXPECT_EQ(runtime::object::is_string, obj1.get_type());
    EXPECT_FALSE(obj1.empty());
    EXPECT_EQ("string", static_cast<std::string>(obj1));

    obj1 = std::move(obj4);
    EXPECT_EQ(runtime::object::is_number, obj1.get_type());
    EXPECT_FALSE(obj1.empty());
    EXPECT_EQ(1.23, static_cast<double>(obj1));

    obj1 = std::move(obj5);
    EXPECT_EQ(runtime::object::is_boolean, obj1.get_type());
    EXPECT_FALSE(obj1.empty());
    EXPECT_TRUE(static_cast<bool>(obj1));

    obj1 = std::move(obj6);
    EXPECT_EQ(runtime::object::is_boolean, obj1.get_type());
    EXPECT_FALSE(obj1.empty());
    EXPECT_FALSE(static_cast<bool>(obj1));

    obj1 = std::move(obj7);
    EXPECT_EQ(runtime::object::is_ref, obj1.get_type());
    EXPECT_FALSE(obj1.empty());
    EXPECT_EQ(ptr, static_cast<runtime::reference>(obj1));


    EXPECT_EQ(runtime::object::is_empty, obj2.get_type());
    EXPECT_TRUE(obj2.empty());
    EXPECT_EQ(runtime::object::is_empty, obj3.get_type());
    EXPECT_TRUE(obj3.empty());
    EXPECT_EQ(runtime::object::is_empty, obj4.get_type());
    EXPECT_TRUE(obj4.empty());
    EXPECT_EQ(runtime::object::is_empty, obj5.get_type());
    EXPECT_TRUE(obj5.empty());
    EXPECT_EQ(runtime::object::is_empty, obj6.get_type());
    EXPECT_TRUE(obj6.empty());
    EXPECT_EQ(runtime::object::is_empty, obj7.get_type());
    EXPECT_TRUE(obj7.empty());
}

#include <boost/lexical_cast.hpp>

TEST(Object, CastToString)
{
    auto ptr = runtime::make_object("test");

    runtime::object obj1;
    runtime::object obj2(empty_value);
    runtime::object obj3("string");
    runtime::object obj4(1.23);
    runtime::object obj5(true);
    runtime::object obj6(false);
    runtime::object obj7(ptr);

    EXPECT_EQ("", static_cast<std::string>(obj1));
    EXPECT_EQ("", static_cast<std::string>(obj2));
    EXPECT_EQ("string", static_cast<std::string>(obj3));
    EXPECT_EQ("1.23", static_cast<std::string>(obj4));
    EXPECT_EQ("true", static_cast<std::string>(obj5));
    EXPECT_EQ("false", static_cast<std::string>(obj6));
    EXPECT_EQ("test", static_cast<std::string>(obj7));
}

TEST(Object, CastToDouble)
{
    auto ptr = runtime::make_object("2.34");

    runtime::object obj1;
    runtime::object obj2(empty_value);
    runtime::object obj3("string");
    runtime::object obj4(1.23);
    runtime::object obj5(true);
    runtime::object obj6(false);
    runtime::object obj7(ptr);

    EXPECT_EQ(0.0, static_cast<double>(obj1));
    EXPECT_EQ(0.0, static_cast<double>(obj2));
    EXPECT_TRUE(std::isnan(static_cast<double>(obj3)));
    EXPECT_EQ(1.23, static_cast<double>(obj4));
    EXPECT_EQ(1, static_cast<double>(obj5));
    EXPECT_EQ(0.0, static_cast<double>(obj6));
    EXPECT_EQ(2.34, static_cast<double>(obj7));
}

TEST(Object, CastToBoolean)
{
    auto ptr = runtime::make_object("");

    runtime::object obj1;
    runtime::object obj2(empty_value);
    runtime::object obj3("string");
    runtime::object obj4(1.23);
    runtime::object obj5(0.0);
    runtime::object obj6(true);
    runtime::object obj7(false);
    runtime::object obj8(ptr);
    runtime::object obj9("true");
    runtime::object obj10("false");
    runtime::object obj11(" \ntrue");
    runtime::object obj12("false \n ");
    runtime::object obj13(" \nTrue");
    runtime::object obj14(" FALSE \n ");
    runtime::object obj15(" \t \n ");

    EXPECT_FALSE(static_cast<bool>(obj1));
    EXPECT_FALSE(static_cast<bool>(obj2));
    EXPECT_TRUE(static_cast<bool>(obj3));
    EXPECT_TRUE(static_cast<bool>(obj4));
    EXPECT_FALSE(static_cast<bool>(obj5));
    EXPECT_TRUE(static_cast<bool>(obj6));
    EXPECT_FALSE(static_cast<bool>(obj7));
    EXPECT_FALSE(static_cast<bool>(obj8));
    EXPECT_TRUE(static_cast<bool>(obj9));
    EXPECT_FALSE(static_cast<bool>(obj10));
    EXPECT_TRUE(static_cast<bool>(obj11));
    EXPECT_FALSE(static_cast<bool>(obj12));
    EXPECT_TRUE(static_cast<bool>(obj13));
    EXPECT_FALSE(static_cast<bool>(obj14));
    EXPECT_FALSE(static_cast<bool>(obj15));
}

TEST(Object, EqualsEmpty)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_TRUE(empty1 == empty2);
    EXPECT_FALSE(empty1 != empty2);
    EXPECT_FALSE(empty1 == string1);
    EXPECT_FALSE(empty1 == string2);
    EXPECT_FALSE(empty1 == string3);
    EXPECT_FALSE(empty1 == string4);
    EXPECT_FALSE(empty1 == string_empty);
    EXPECT_FALSE(empty1 == string_true);
    EXPECT_FALSE(empty1 == string_false);
    EXPECT_FALSE(empty1 == number1);
    EXPECT_FALSE(empty1 == number2);
    EXPECT_FALSE(empty1 == number3);
    EXPECT_FALSE(empty1 == true_);
    EXPECT_FALSE(empty1 == false_);
}

TEST(Object, EqualsString)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_FALSE(string1 == empty1);
    EXPECT_FALSE(string1 == empty2);
    EXPECT_TRUE(string1 == string1);
    EXPECT_TRUE(string1 == string2);
    EXPECT_FALSE(string1 == string3);
    EXPECT_FALSE(string1 == string4);
    EXPECT_FALSE(string1 == string_empty);
    EXPECT_FALSE(string1 == string_true);
    EXPECT_FALSE(string1 == string_false);
    EXPECT_FALSE(string1 == number1);
    EXPECT_FALSE(string1 == number2);
    EXPECT_FALSE(string1 == number3);
    EXPECT_FALSE(string1 == true_);
    EXPECT_FALSE(string1 == false_);

    EXPECT_TRUE(string4 == number1);
    EXPECT_TRUE(string4 == number2);

    EXPECT_TRUE(string_true == true_);
    EXPECT_FALSE(string_true == false_);

    EXPECT_FALSE(string_false == true_);
    EXPECT_TRUE(string_false == false_);
}

TEST(Object, EqualsNumber)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_FALSE(number1 == empty1);
    EXPECT_FALSE(number1 == empty2);
    EXPECT_FALSE(number1 == string1);
    EXPECT_FALSE(number1 == string2);
    EXPECT_FALSE(number1 == string3);
    EXPECT_TRUE(number1 == string4);
    EXPECT_FALSE(number1 == string_empty);
    EXPECT_FALSE(number1 == string_true);
    EXPECT_FALSE(number1 == string_false);
    EXPECT_TRUE(number1 == number1);
    EXPECT_TRUE(number1 == number2);
    EXPECT_FALSE(number1 == number3);
    EXPECT_FALSE(number1 == true_);
    EXPECT_FALSE(number1 == false_);

    EXPECT_FALSE(number3 == true_);
    EXPECT_TRUE(number3 == false_);
}

TEST(Object, EqualsBoolean)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_FALSE(true_ == empty1);
    EXPECT_FALSE(true_ == empty2);
    EXPECT_TRUE(true_ == string1);
    EXPECT_TRUE(true_ == string2);
    EXPECT_TRUE(true_ == string3);
    EXPECT_TRUE(true_ == string4);
    EXPECT_FALSE(true_ == string_empty);
    EXPECT_TRUE(true_ == string_true);
    EXPECT_FALSE(true_ == string_false);
    EXPECT_TRUE(true_ == number1);
    EXPECT_TRUE(true_ == number2);
    EXPECT_FALSE(true_ == number3);
    EXPECT_TRUE(true_ == true_);
    EXPECT_FALSE(true_ == false_);

    EXPECT_TRUE(false_ == empty1);
    EXPECT_TRUE(false_ == empty2);
    EXPECT_FALSE(false_ == string1);
    EXPECT_FALSE(false_ == string2);
    EXPECT_FALSE(false_ == string3);
    EXPECT_FALSE(false_ == string4);
    EXPECT_TRUE(false_ == string_empty);
    EXPECT_FALSE(false_ == string_true);
    EXPECT_TRUE(false_ == string_false);
    EXPECT_FALSE(false_ == number1);
    EXPECT_FALSE(false_ == number2);
    EXPECT_TRUE(false_ == number3);
    EXPECT_FALSE(false_ == true_);
    EXPECT_TRUE(false_ == false_);
}

TEST(Object, LessThanEmpty)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_FALSE(empty1 < empty2);
    EXPECT_FALSE(empty1 < string1);
    EXPECT_FALSE(empty1 < string2);
    EXPECT_FALSE(empty1 < string3);
    EXPECT_FALSE(empty1 < string4);
    EXPECT_FALSE(empty1 < string_empty);
    EXPECT_FALSE(empty1 < string_true);
    EXPECT_FALSE(empty1 < string_false);
    EXPECT_FALSE(empty1 < number1);
    EXPECT_FALSE(empty1 < number2);
    EXPECT_FALSE(empty1 < number3);
    EXPECT_FALSE(empty1 < true_);
    EXPECT_FALSE(empty1 < false_);
}

TEST(Object, LessThanString)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_FALSE(string1 < empty1);
    EXPECT_FALSE(string1 < empty2);
    EXPECT_FALSE(string1 < string1);
    EXPECT_FALSE(string1 < string2);
    EXPECT_FALSE(string1 < string3);
    EXPECT_FALSE(string1 < string4);
    EXPECT_FALSE(string1 < string_empty);
    EXPECT_TRUE(string1 < string_true);
    EXPECT_FALSE(string1 < string_false);
    EXPECT_FALSE(string1 < number1);
    EXPECT_FALSE(string1 < number2);
    EXPECT_FALSE(string1 < number3);
    EXPECT_TRUE(string1 < true_);
    EXPECT_FALSE(string1 < false_);

    EXPECT_FALSE(string4 < number1);
    EXPECT_FALSE(string4 < number2);

    EXPECT_FALSE(string_true < true_);
    EXPECT_FALSE(string_true < false_);

    EXPECT_TRUE(string_false < true_);
    EXPECT_FALSE(string_false < false_);
}

TEST(Object, LessThanNumber)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_FALSE(number1 < empty1);
    EXPECT_FALSE(number1 < empty2);
    EXPECT_FALSE(number1 < string1);
    EXPECT_FALSE(number1 < string2);
    EXPECT_FALSE(number1 < string3);
    EXPECT_FALSE(number1 < string4);
    EXPECT_FALSE(number1 < string_empty);
    EXPECT_FALSE(number1 < string_true);
    EXPECT_FALSE(number1 < string_false);
    EXPECT_FALSE(number1 < number1);
    EXPECT_FALSE(number1 < number2);
    EXPECT_FALSE(number1 < number3);
    EXPECT_FALSE(number1 < true_);
    EXPECT_FALSE(number1 < false_);

    EXPECT_TRUE(number3 < true_);
    EXPECT_FALSE(number3 < false_);
}

TEST(Object, LessThanBoolean)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_FALSE(true_ < empty1);
    EXPECT_FALSE(true_ < empty2);
    EXPECT_FALSE(true_ < string1);
    EXPECT_FALSE(true_ < string2);
    EXPECT_FALSE(true_ < string3);
    EXPECT_FALSE(true_ < string4);
    EXPECT_FALSE(true_ < string_empty);
    EXPECT_FALSE(true_ < string_true);
    EXPECT_FALSE(true_ < string_false);
    EXPECT_FALSE(true_ < number1);
    EXPECT_FALSE(true_ < number2);
    EXPECT_FALSE(true_ < number3);
    EXPECT_FALSE(true_ < true_);
    EXPECT_FALSE(true_ < false_);

    EXPECT_FALSE(false_ < empty1);
    EXPECT_FALSE(false_ < empty2);
    EXPECT_FALSE(false_ < string1);
    EXPECT_FALSE(false_ < string2);
    EXPECT_FALSE(false_ < string3);
    EXPECT_FALSE(false_ < string4);
    EXPECT_FALSE(false_ < string_empty);
    EXPECT_FALSE(false_ < string_true);
    EXPECT_FALSE(false_ < string_false);
    EXPECT_FALSE(false_ < number1);
    EXPECT_FALSE(false_ < number2);
    EXPECT_FALSE(false_ < number3);
    EXPECT_FALSE(false_ < true_);
    EXPECT_FALSE(false_ < false_);
}

TEST(Object, GreaterThanEmpty)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_FALSE(empty1 > empty2);
    EXPECT_FALSE(empty1 > string1);
    EXPECT_FALSE(empty1 > string2);
    EXPECT_FALSE(empty1 > string3);
    EXPECT_FALSE(empty1 > string4);
    EXPECT_FALSE(empty1 > string_empty);
    EXPECT_FALSE(empty1 > string_true);
    EXPECT_FALSE(empty1 > string_false);
    EXPECT_FALSE(empty1 > number1);
    EXPECT_FALSE(empty1 > number2);
    EXPECT_FALSE(empty1 > number3);
    EXPECT_FALSE(empty1 > true_);
    EXPECT_FALSE(empty1 > false_);
}

TEST(Object, GreaterThanString)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_TRUE(string1 > empty1);
    EXPECT_TRUE(string1 > empty2);
    EXPECT_FALSE(string1 > string1);
    EXPECT_FALSE(string1 > string2);
    EXPECT_TRUE(string1 > string3);
    EXPECT_TRUE(string1 > string4);
    EXPECT_TRUE(string1 > string_empty);
    EXPECT_FALSE(string1 > string_true);
    EXPECT_TRUE(string1 > string_false);
    EXPECT_TRUE(string1 > number1);
    EXPECT_TRUE(string1 > number2);
    EXPECT_TRUE(string1 > number3);
    EXPECT_FALSE(string1 > true_);
    EXPECT_TRUE(string1 > false_);

    EXPECT_FALSE(string4 > number1);
    EXPECT_FALSE(string4 > number2);

    EXPECT_FALSE(string_true > true_);
    EXPECT_TRUE(string_true > false_);

    EXPECT_FALSE(string_false > true_);
    EXPECT_FALSE(string_false > false_);
}

TEST(Object, GreaterThanNumber)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_TRUE(number1 > empty1);
    EXPECT_TRUE(number1 > empty2);
    EXPECT_FALSE(number1 > string1);
    EXPECT_FALSE(number1 > string2);
    EXPECT_FALSE(number1 > string3);
    EXPECT_FALSE(number1 > string4);
    EXPECT_FALSE(number1 > string_empty);
    EXPECT_FALSE(number1 > string_true);
    EXPECT_FALSE(number1 > string_false);
    EXPECT_FALSE(number1 > number1);
    EXPECT_FALSE(number1 > number2);
    EXPECT_TRUE(number1 > number3);
    EXPECT_TRUE(number1 > true_);
    EXPECT_TRUE(number1 > false_);

    EXPECT_FALSE(number3 > true_);
    EXPECT_FALSE(number3 > false_);
}

TEST(Object, GreaterThanBoolean)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_FALSE(true_ > empty1);
    EXPECT_FALSE(true_ > empty2);
    EXPECT_FALSE(true_ > string1);
    EXPECT_FALSE(true_ > string2);
    EXPECT_FALSE(true_ > string3);
    EXPECT_FALSE(true_ > string4);
    EXPECT_FALSE(true_ > string_empty);
    EXPECT_FALSE(true_ > string_true);
    EXPECT_FALSE(true_ > string_false);
    EXPECT_FALSE(true_ > number1);
    EXPECT_FALSE(true_ > number2);
    EXPECT_FALSE(true_ > number3);
    EXPECT_FALSE(true_ > true_);
    EXPECT_FALSE(true_ > false_);

    EXPECT_FALSE(false_ > empty1);
    EXPECT_FALSE(false_ > empty2);
    EXPECT_FALSE(false_ > string1);
    EXPECT_FALSE(false_ > string2);
    EXPECT_FALSE(false_ > string3);
    EXPECT_FALSE(false_ > string4);
    EXPECT_FALSE(false_ > string_empty);
    EXPECT_FALSE(false_ > string_true);
    EXPECT_FALSE(false_ > string_false);
    EXPECT_FALSE(false_ > number1);
    EXPECT_FALSE(false_ > number2);
    EXPECT_FALSE(false_ > number3);
    EXPECT_FALSE(false_ > true_);
    EXPECT_FALSE(false_ > false_);
}

TEST(Object, LTEEmpty)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_FALSE(empty1 <= empty2);
    EXPECT_FALSE(empty1 <= string1);
    EXPECT_FALSE(empty1 <= string2);
    EXPECT_FALSE(empty1 <= string3);
    EXPECT_FALSE(empty1 <= string4);
    EXPECT_FALSE(empty1 <= string_empty);
    EXPECT_FALSE(empty1 <= string_true);
    EXPECT_FALSE(empty1 <= string_false);
    EXPECT_FALSE(empty1 <= number1);
    EXPECT_FALSE(empty1 <= number2);
    EXPECT_FALSE(empty1 <= number3);
    EXPECT_FALSE(empty1 <= true_);
    EXPECT_FALSE(empty1 <= false_);
}

TEST(Object, LTEString)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_FALSE(string1 <= empty1);
    EXPECT_FALSE(string1 <= empty2);
    EXPECT_TRUE(string1 <= string1);
    EXPECT_TRUE(string1 <= string2);
    EXPECT_FALSE(string1 <= string3);
    EXPECT_FALSE(string1 <= string4);
    EXPECT_FALSE(string1 <= string_empty);
    EXPECT_TRUE(string1 <= string_true);
    EXPECT_FALSE(string1 <= string_false);
    EXPECT_FALSE(string1 <= number1);
    EXPECT_FALSE(string1 <= number2);
    EXPECT_FALSE(string1 <= number3);
    EXPECT_TRUE(string1 <= true_);
    EXPECT_FALSE(string1 <= false_);

    EXPECT_TRUE(string4 <= number1);
    EXPECT_TRUE(string4 <= number2);

    EXPECT_TRUE(string_true <= true_);
    EXPECT_FALSE(string_true <= false_);

    EXPECT_TRUE(string_false <= true_);
    EXPECT_TRUE(string_false <= false_);
}

TEST(Object, LTENumber)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_FALSE(number1 <= empty1);
    EXPECT_FALSE(number1 <= empty2);
    EXPECT_FALSE(number1 <= string1);
    EXPECT_FALSE(number1 <= string2);
    EXPECT_FALSE(number1 <= string3);
    EXPECT_TRUE(number1 <= string4);
    EXPECT_FALSE(number1 <= string_empty);
    EXPECT_FALSE(number1 <= string_true);
    EXPECT_FALSE(number1 <= string_false);
    EXPECT_TRUE(number1 <= number1);
    EXPECT_TRUE(number1 <= number2);
    EXPECT_FALSE(number1 <= number3);
    EXPECT_FALSE(number1 <= true_);
    EXPECT_FALSE(number1 <= false_);

    EXPECT_TRUE(number3 <= true_);
    EXPECT_TRUE(number3 <= false_);
}

TEST(Object, LTEBoolean)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_FALSE(true_ <= empty1);
    EXPECT_FALSE(true_ <= empty2);
    EXPECT_FALSE(true_ <= string1);
    EXPECT_FALSE(true_ <= string2);
    EXPECT_FALSE(true_ <= string3);
    EXPECT_FALSE(true_ <= string4);
    EXPECT_FALSE(true_ <= string_empty);
    EXPECT_FALSE(true_ <= string_true);
    EXPECT_FALSE(true_ <= string_false);
    EXPECT_FALSE(true_ <= number1);
    EXPECT_FALSE(true_ <= number2);
    EXPECT_FALSE(true_ <= number3);
    EXPECT_FALSE(true_ <= true_);
    EXPECT_FALSE(true_ <= false_);

    EXPECT_FALSE(false_ <= empty1);
    EXPECT_FALSE(false_ <= empty2);
    EXPECT_FALSE(false_ <= string1);
    EXPECT_FALSE(false_ <= string2);
    EXPECT_FALSE(false_ <= string3);
    EXPECT_FALSE(false_ <= string4);
    EXPECT_FALSE(false_ <= string_empty);
    EXPECT_FALSE(false_ <= string_true);
    EXPECT_FALSE(false_ <= string_false);
    EXPECT_FALSE(false_ <= number1);
    EXPECT_FALSE(false_ <= number2);
    EXPECT_FALSE(false_ <= number3);
    EXPECT_FALSE(false_ <= true_);
    EXPECT_FALSE(false_ <= false_);
}

TEST(Object, GTEEmpty)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_FALSE(empty1 >= empty2);
    EXPECT_FALSE(empty1 >= string1);
    EXPECT_FALSE(empty1 >= string2);
    EXPECT_FALSE(empty1 >= string3);
    EXPECT_FALSE(empty1 >= string4);
    EXPECT_FALSE(empty1 >= string_empty);
    EXPECT_FALSE(empty1 >= string_true);
    EXPECT_FALSE(empty1 >= string_false);
    EXPECT_FALSE(empty1 >= number1);
    EXPECT_FALSE(empty1 >= number2);
    EXPECT_FALSE(empty1 >= number3);
    EXPECT_FALSE(empty1 >= true_);
    EXPECT_FALSE(empty1 >= false_);
}

TEST(Object, GTEString)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_TRUE(string1 >= empty1);
    EXPECT_TRUE(string1 >= empty2);
    EXPECT_TRUE(string1 >= string1);
    EXPECT_TRUE(string1 >= string2);
    EXPECT_TRUE(string1 >= string3);
    EXPECT_TRUE(string1 >= string4);
    EXPECT_TRUE(string1 >= string_empty);
    EXPECT_FALSE(string1 >= string_true);
    EXPECT_TRUE(string1 >= string_false);
    EXPECT_TRUE(string1 >= number1);
    EXPECT_TRUE(string1 >= number2);
    EXPECT_TRUE(string1 >= number3);
    EXPECT_FALSE(string1 >= true_);
    EXPECT_TRUE(string1 >= false_);

    EXPECT_TRUE(string4 >= number1);
    EXPECT_TRUE(string4 >= number2);

    EXPECT_TRUE(string_true >= true_);
    EXPECT_TRUE(string_true >= false_);

    EXPECT_FALSE(string_false >= true_);
    EXPECT_TRUE(string_false >= false_);
}

TEST(Object, GTENumber)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_TRUE(number1 >= empty1);
    EXPECT_TRUE(number1 >= empty2);
    EXPECT_FALSE(number1 >= string1);
    EXPECT_FALSE(number1 >= string2);
    EXPECT_FALSE(number1 >= string3);
    EXPECT_TRUE(number1 >= string4);
    EXPECT_FALSE(number1 >= string_empty);
    EXPECT_FALSE(number1 >= string_true);
    EXPECT_FALSE(number1 >= string_false);
    EXPECT_TRUE(number1 >= number1);
    EXPECT_TRUE(number1 >= number2);
    EXPECT_TRUE(number1 >= number3);
    EXPECT_TRUE(number1 >= true_);
    EXPECT_TRUE(number1 >= false_);

    EXPECT_FALSE(number3 >= true_);
    EXPECT_TRUE(number3 >= false_);
}

TEST(Object, GTEBoolean)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_FALSE(true_ >= empty1);
    EXPECT_FALSE(true_ >= empty2);
    EXPECT_FALSE(true_ >= string1);
    EXPECT_FALSE(true_ >= string2);
    EXPECT_FALSE(true_ >= string3);
    EXPECT_FALSE(true_ >= string4);
    EXPECT_FALSE(true_ >= string_empty);
    EXPECT_FALSE(true_ >= string_true);
    EXPECT_FALSE(true_ >= string_false);
    EXPECT_FALSE(true_ >= number1);
    EXPECT_FALSE(true_ >= number2);
    EXPECT_FALSE(true_ >= number3);
    EXPECT_FALSE(true_ >= true_);
    EXPECT_FALSE(true_ >= false_);

    EXPECT_FALSE(false_ >= empty1);
    EXPECT_FALSE(false_ >= empty2);
    EXPECT_FALSE(false_ >= string1);
    EXPECT_FALSE(false_ >= string2);
    EXPECT_FALSE(false_ >= string3);
    EXPECT_FALSE(false_ >= string4);
    EXPECT_FALSE(false_ >= string_empty);
    EXPECT_FALSE(false_ >= string_true);
    EXPECT_FALSE(false_ >= string_false);
    EXPECT_FALSE(false_ >= number1);
    EXPECT_FALSE(false_ >= number2);
    EXPECT_FALSE(false_ >= number3);
    EXPECT_FALSE(false_ >= true_);
    EXPECT_FALSE(false_ >= false_);
}

TEST(Object, PlusEmpty)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_EQ(runtime::object(), empty1 + empty2);
    EXPECT_EQ(runtime::object(), empty1 + string1);
    EXPECT_EQ(runtime::object(), empty1 + string2);
    EXPECT_EQ(runtime::object(), empty1 + string3);
    EXPECT_EQ(runtime::object(), empty1 + string4);
    EXPECT_EQ(runtime::object(), empty1 + string_empty);
    EXPECT_EQ(runtime::object(), empty1 + string_true);
    EXPECT_EQ(runtime::object(), empty1 + string_false);
    EXPECT_EQ(runtime::object(), empty1 + number1);
    EXPECT_EQ(runtime::object(), empty1 + number2);
    EXPECT_EQ(runtime::object(), empty1 + number3);
    EXPECT_EQ(runtime::object(), empty1 + true_);
    EXPECT_EQ(runtime::object(), empty1 + false_);
}

TEST(Object, PlusString)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_EQ(runtime::object("string"), string1 + empty1);
    EXPECT_EQ(runtime::object("string"), string1 + empty2);
    EXPECT_EQ(runtime::object("stringstring"), string1 + string1);
    EXPECT_EQ(runtime::object("stringstring"), string1 + string2);
    EXPECT_EQ(runtime::object("stringString"), string1 + string3);
    EXPECT_EQ(runtime::object("string1.23"), string1 + string4);
    EXPECT_EQ(runtime::object("string"), string1 + string_empty);
    EXPECT_EQ(runtime::object("stringtrue"), string1 + string_true);
    EXPECT_EQ(runtime::object("stringfalse"), string1 + string_false);
    EXPECT_EQ(runtime::object("string1.23"), string1 + number1);
    EXPECT_EQ(runtime::object("string1.23"), string1 + number2);
    EXPECT_EQ(runtime::object("string0"), string1 + number3);
    EXPECT_EQ(runtime::object("stringtrue"), string1 + true_);
    EXPECT_EQ(runtime::object("stringfalse"), string1 + false_);

    EXPECT_EQ(runtime::object("1.231.23"), string4 + number1);
    EXPECT_EQ(runtime::object("1.231.23"), string4 + number2);

    EXPECT_EQ(runtime::object("truetrue"), string_true + true_);
    EXPECT_EQ(runtime::object("truefalse"), string_true + false_);

    EXPECT_EQ(runtime::object("falsetrue"), string_false + true_);
    EXPECT_EQ(runtime::object("falsefalse"), string_false + false_);
}

TEST(Object, PlusNumber)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_EQ(runtime::object(1.23), number1 + empty1);
    EXPECT_EQ(runtime::object(1.23), number1 + empty2);
    EXPECT_EQ(runtime::object("1.23string"), number1 + string1);
    EXPECT_EQ(runtime::object("1.23string"), number1 + string2);
    EXPECT_EQ(runtime::object("1.23String"), number1 + string3);
    EXPECT_EQ(runtime::object("1.231.23"), number1 + string4);
    EXPECT_EQ(runtime::object("1.23"), number1 + string_empty);
    EXPECT_EQ(runtime::object("1.23true"), number1 + string_true);
    EXPECT_EQ(runtime::object("1.23false"), number1 + string_false);
    EXPECT_EQ(runtime::object(2.46), number1 + number1);
    EXPECT_EQ(runtime::object(2.46), number1 + number2);
    EXPECT_EQ(runtime::object(1.23), number1 + number3);
    EXPECT_EQ(runtime::object(2.23), number1 + true_);
    EXPECT_EQ(runtime::object(1.23), number1 + false_);

    EXPECT_EQ(runtime::object(1.0), number3 + true_);
    EXPECT_EQ(runtime::object(0.0), number3 + false_);
}

TEST(Object, PlusBoolean)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_EQ(runtime::object(true), true_ + empty1);
    EXPECT_EQ(runtime::object(true), true_ + empty2);
    EXPECT_EQ(runtime::object(true), true_ + string1);
    EXPECT_EQ(runtime::object(true), true_ + string2);
    EXPECT_EQ(runtime::object(true), true_ + string3);
    EXPECT_EQ(runtime::object(true), true_ + string4);
    EXPECT_EQ(runtime::object(true), true_ + string_empty);
    EXPECT_EQ(runtime::object(true), true_ + string_true);
    EXPECT_EQ(runtime::object(true), true_ + string_false);
    EXPECT_EQ(runtime::object(true), true_ + number1);
    EXPECT_EQ(runtime::object(true), true_ + number2);
    EXPECT_EQ(runtime::object(true), true_ + number3);
    EXPECT_EQ(runtime::object(true), true_ + true_);
    EXPECT_EQ(runtime::object(true), true_ + false_);

    EXPECT_EQ(runtime::object(false), false_ + empty1);
    EXPECT_EQ(runtime::object(false), false_ + empty2);
    EXPECT_EQ(runtime::object(false), false_ + string1);
    EXPECT_EQ(runtime::object(false), false_ + string2);
    EXPECT_EQ(runtime::object(false), false_ + string3);
    EXPECT_EQ(runtime::object(false), false_ + string4);
    EXPECT_EQ(runtime::object(false), false_ + string_empty);
    EXPECT_EQ(runtime::object(false), false_ + string_true);
    EXPECT_EQ(runtime::object(false), false_ + string_false);
    EXPECT_EQ(runtime::object(false), false_ + number1);
    EXPECT_EQ(runtime::object(false), false_ + number2);
    EXPECT_EQ(runtime::object(false), false_ + number3);
    EXPECT_EQ(runtime::object(false), false_ + true_);
    EXPECT_EQ(runtime::object(false), false_ + false_);
}

TEST(Object, MinusEmpty)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_EQ(runtime::object(), empty1 - empty2);
    EXPECT_EQ(runtime::object(), empty1 - string1);
    EXPECT_EQ(runtime::object(), empty1 - string2);
    EXPECT_EQ(runtime::object(), empty1 - string3);
    EXPECT_EQ(runtime::object(), empty1 - string4);
    EXPECT_EQ(runtime::object(), empty1 - string_empty);
    EXPECT_EQ(runtime::object(), empty1 - string_true);
    EXPECT_EQ(runtime::object(), empty1 - string_false);
    EXPECT_EQ(runtime::object(), empty1 - number1);
    EXPECT_EQ(runtime::object(), empty1 - number2);
    EXPECT_EQ(runtime::object(), empty1 - number3);
    EXPECT_EQ(runtime::object(), empty1 - true_);
    EXPECT_EQ(runtime::object(), empty1 - false_);
}

TEST(Object, MinusString)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_EQ(runtime::object("string"), string1 - empty1);
    EXPECT_EQ(runtime::object("string"), string1 - empty2);
    EXPECT_EQ(runtime::object(""), string1 - string1);
    EXPECT_EQ(runtime::object(""), string1 - string2);
    EXPECT_EQ(runtime::object("string"), string1 - string3);
    EXPECT_EQ(runtime::object("string"), string1 - string4);
    EXPECT_EQ(runtime::object("string"), string1 - string_empty);
    EXPECT_EQ(runtime::object("string"), string1 - string_true);
    EXPECT_EQ(runtime::object("string"), string1 - string_false);
    EXPECT_EQ(runtime::object("string"), string1 - number1);
    EXPECT_EQ(runtime::object("string"), string1 - number2);
    EXPECT_EQ(runtime::object("string"), string1 - number3);
    EXPECT_EQ(runtime::object("string"), string1 - true_);
    EXPECT_EQ(runtime::object("string"), string1 - false_);

    EXPECT_EQ(runtime::object(""), string4 - number1);
    EXPECT_EQ(runtime::object(""), string4 - number2);

    EXPECT_EQ(runtime::object(""), string_true - true_);
    EXPECT_EQ(runtime::object("true"), string_true - false_);

    EXPECT_EQ(runtime::object("false"), string_false - true_);
    EXPECT_EQ(runtime::object(""), string_false - false_);
}

TEST(Object, MinusNumber)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_EQ(runtime::object(1.23), number1 - empty1);
    EXPECT_EQ(runtime::object(1.23), number1 - empty2);
    EXPECT_EQ(runtime::object(1.23), number1 - string1);
    EXPECT_EQ(runtime::object(1.23), number1 - string2);
    EXPECT_EQ(runtime::object(1.23), number1 - string3);
    EXPECT_EQ(runtime::object(0.0), number1 - string4);
    EXPECT_EQ(runtime::object(1.23), number1 - string_empty);
    EXPECT_EQ(runtime::object(1.23), number1 - string_true);
    EXPECT_EQ(runtime::object(1.23), number1 - string_false);
    EXPECT_EQ(runtime::object(0.0), number1 - number1);
    EXPECT_EQ(runtime::object(0.0), number1 - number2);
    EXPECT_EQ(runtime::object(1.23), number1 - number3);
    EXPECT_DOUBLE_EQ(runtime::object(0.23), number1 - true_);
    EXPECT_EQ(runtime::object(1.23), number1 - false_);

    EXPECT_EQ(runtime::object(-1.0), number3 - true_);
    EXPECT_EQ(runtime::object(0.0), number3 - false_);
}

TEST(Object, MinusBoolean)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_EQ(runtime::object(true), true_ - empty1);
    EXPECT_EQ(runtime::object(true), true_ - empty2);
    EXPECT_EQ(runtime::object(true), true_ - string1);
    EXPECT_EQ(runtime::object(true), true_ - string2);
    EXPECT_EQ(runtime::object(true), true_ - string3);
    EXPECT_EQ(runtime::object(true), true_ - string4);
    EXPECT_EQ(runtime::object(true), true_ - string_empty);
    EXPECT_EQ(runtime::object(true), true_ - string_true);
    EXPECT_EQ(runtime::object(true), true_ - string_false);
    EXPECT_EQ(runtime::object(true), true_ - number1);
    EXPECT_EQ(runtime::object(true), true_ - number2);
    EXPECT_EQ(runtime::object(true), true_ - number3);
    EXPECT_EQ(runtime::object(true), true_ - true_);
    EXPECT_EQ(runtime::object(true), true_ - false_);

    EXPECT_EQ(runtime::object(false), false_ - empty1);
    EXPECT_EQ(runtime::object(false), false_ - empty2);
    EXPECT_EQ(runtime::object(false), false_ - string1);
    EXPECT_EQ(runtime::object(false), false_ - string2);
    EXPECT_EQ(runtime::object(false), false_ - string3);
    EXPECT_EQ(runtime::object(false), false_ - string4);
    EXPECT_EQ(runtime::object(false), false_ - string_empty);
    EXPECT_EQ(runtime::object(false), false_ - string_true);
    EXPECT_EQ(runtime::object(false), false_ - string_false);
    EXPECT_EQ(runtime::object(false), false_ - number1);
    EXPECT_EQ(runtime::object(false), false_ - number2);
    EXPECT_EQ(runtime::object(false), false_ - number3);
    EXPECT_EQ(runtime::object(false), false_ - true_);
    EXPECT_EQ(runtime::object(false), false_ - false_);
}

TEST(Object, MultEmpty)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_EQ(runtime::object(), empty1 * empty2);
    EXPECT_EQ(runtime::object(), empty1 * string1);
    EXPECT_EQ(runtime::object(), empty1 * string2);
    EXPECT_EQ(runtime::object(), empty1 * string3);
    EXPECT_EQ(runtime::object(), empty1 * string4);
    EXPECT_EQ(runtime::object(), empty1 * string_empty);
    EXPECT_EQ(runtime::object(), empty1 * string_true);
    EXPECT_EQ(runtime::object(), empty1 * string_false);
    EXPECT_EQ(runtime::object(), empty1 * number1);
    EXPECT_EQ(runtime::object(), empty1 * number2);
    EXPECT_EQ(runtime::object(), empty1 * number3);
    EXPECT_EQ(runtime::object(), empty1 * true_);
    EXPECT_EQ(runtime::object(), empty1 * false_);
}

TEST(Object, MultString)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(3.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_EQ(runtime::object("string"), string1 * empty1);
    EXPECT_EQ(runtime::object("string"), string1 * empty2);
    EXPECT_EQ(runtime::object("string"), string1 * string1);
    EXPECT_EQ(runtime::object("string"), string1 * string2);
    EXPECT_EQ(runtime::object("string"), string1 * string3);
    EXPECT_EQ(runtime::object("string"), string1 * string4);
    EXPECT_EQ(runtime::object("string"), string1 * string_empty);
    EXPECT_EQ(runtime::object("string"), string1 * string_true);
    EXPECT_EQ(runtime::object("string"), string1 * string_false);
    EXPECT_EQ(runtime::object("string"), string1 * number1);
    EXPECT_EQ(runtime::object("stringstringstring"), string1 * number2);
    EXPECT_EQ(runtime::object(""), string1 * number3);
    EXPECT_EQ(runtime::object("string"), string1 * true_);
    EXPECT_EQ(runtime::object("string"), string1 * false_);

    EXPECT_EQ(runtime::object("1.23"), string4 * number1);
    EXPECT_EQ(runtime::object("1.231.231.23"), string4 * number2);

    EXPECT_EQ(runtime::object("true"), string_true * true_);
    EXPECT_EQ(runtime::object("true"), string_true * false_);

    EXPECT_EQ(runtime::object("false"), string_false * true_);
    EXPECT_EQ(runtime::object("false"), string_false * false_);
}

TEST(Object, MultNumber)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_EQ(runtime::object(0.0), number1 * empty1);
    EXPECT_EQ(runtime::object(0.0), number1 * empty2);
    EXPECT_EQ(runtime::object(1.23), number1 * string1);
    EXPECT_EQ(runtime::object(1.23), number1 * string2);
    EXPECT_EQ(runtime::object(1.23), number1 * string3);
    EXPECT_EQ(runtime::object(1.23 * 1.23), number1 * string4);
    EXPECT_EQ(runtime::object(1.23), number1 * string_empty);
    EXPECT_EQ(runtime::object(1.23), number1 * string_true);
    EXPECT_EQ(runtime::object(1.23), number1 * string_false);
    EXPECT_EQ(runtime::object(1.23 * 1.23), number1 * number1);
    EXPECT_EQ(runtime::object(1.23 * 1.23), number1 * number2);
    EXPECT_EQ(runtime::object(0.0), number1 * number3);
    EXPECT_EQ(runtime::object(1.23), number1 * true_);
    EXPECT_EQ(runtime::object(0.0), number1 * false_);

    EXPECT_EQ(runtime::object(0.0), number3 * true_);
    EXPECT_EQ(runtime::object(0.0), number3 * false_);
}

TEST(Object, MultBoolean)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_EQ(runtime::object(true), true_ * empty1);
    EXPECT_EQ(runtime::object(true), true_ * empty2);
    EXPECT_EQ(runtime::object(true), true_ * string1);
    EXPECT_EQ(runtime::object(true), true_ * string2);
    EXPECT_EQ(runtime::object(true), true_ * string3);
    EXPECT_EQ(runtime::object(true), true_ * string4);
    EXPECT_EQ(runtime::object(true), true_ * string_empty);
    EXPECT_EQ(runtime::object(true), true_ * string_true);
    EXPECT_EQ(runtime::object(true), true_ * string_false);
    EXPECT_EQ(runtime::object(true), true_ * number1);
    EXPECT_EQ(runtime::object(true), true_ * number2);
    EXPECT_EQ(runtime::object(true), true_ * number3);
    EXPECT_EQ(runtime::object(true), true_ * true_);
    EXPECT_EQ(runtime::object(true), true_ * false_);

    EXPECT_EQ(runtime::object(false), false_ * empty1);
    EXPECT_EQ(runtime::object(false), false_ * empty2);
    EXPECT_EQ(runtime::object(false), false_ * string1);
    EXPECT_EQ(runtime::object(false), false_ * string2);
    EXPECT_EQ(runtime::object(false), false_ * string3);
    EXPECT_EQ(runtime::object(false), false_ * string4);
    EXPECT_EQ(runtime::object(false), false_ * string_empty);
    EXPECT_EQ(runtime::object(false), false_ * string_true);
    EXPECT_EQ(runtime::object(false), false_ * string_false);
    EXPECT_EQ(runtime::object(false), false_ * number1);
    EXPECT_EQ(runtime::object(false), false_ * number2);
    EXPECT_EQ(runtime::object(false), false_ * number3);
    EXPECT_EQ(runtime::object(false), false_ * true_);
    EXPECT_EQ(runtime::object(false), false_ * false_);
}

TEST(Object, DivEmpty)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_EQ(runtime::object(), empty1 / empty2);
    EXPECT_EQ(runtime::object(), empty1 / string1);
    EXPECT_EQ(runtime::object(), empty1 / string2);
    EXPECT_EQ(runtime::object(), empty1 / string3);
    EXPECT_EQ(runtime::object(), empty1 / string4);
    EXPECT_EQ(runtime::object(), empty1 / string_empty);
    EXPECT_EQ(runtime::object(), empty1 / string_true);
    EXPECT_EQ(runtime::object(), empty1 / string_false);
    EXPECT_EQ(runtime::object(), empty1 / number1);
    EXPECT_EQ(runtime::object(), empty1 / number2);
    EXPECT_EQ(runtime::object(), empty1 / number3);
    EXPECT_EQ(runtime::object(), empty1 / true_);
    EXPECT_EQ(runtime::object(), empty1 / false_);
}

TEST(Object, DivString)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(3.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_EQ(runtime::object("string"), string1 / empty1);
    EXPECT_EQ(runtime::object("string"), string1 / empty2);
    EXPECT_EQ(runtime::object("string"), string1 / string1);
    EXPECT_EQ(runtime::object("string"), string1 / string2);
    EXPECT_EQ(runtime::object("string"), string1 / string3);
    EXPECT_EQ(runtime::object("string"), string1 / string4);
    EXPECT_EQ(runtime::object("string"), string1 / string_empty);
    EXPECT_EQ(runtime::object("string"), string1 / string_true);
    EXPECT_EQ(runtime::object("string"), string1 / string_false);
    EXPECT_EQ(runtime::object("string"), string1 / number1);
    EXPECT_EQ(runtime::object("string"), string1 / number2);
    EXPECT_EQ(runtime::object("string"), string1 / number3);
    EXPECT_EQ(runtime::object("string"), string1 / true_);
    EXPECT_EQ(runtime::object("string"), string1 / false_);

    EXPECT_EQ(runtime::object("1.23"), string4 / number1);
    EXPECT_EQ(runtime::object("1.23"), string4 / number2);

    EXPECT_EQ(runtime::object("true"), string_true / true_);
    EXPECT_EQ(runtime::object("true"), string_true / false_);

    EXPECT_EQ(runtime::object("false"), string_false / true_);
    EXPECT_EQ(runtime::object("false"), string_false / false_);
}

TEST(Object, DivNumber)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_EQ(runtime::object("inf"), number1 / empty1);
    EXPECT_EQ(runtime::object("inf"), number1 / empty2);
    EXPECT_EQ(runtime::object(1.23), number1 / string1);
    EXPECT_EQ(runtime::object(1.23), number1 / string2);
    EXPECT_EQ(runtime::object(1.23), number1 / string3);
    EXPECT_EQ(runtime::object(1.0), number1 / string4);
    EXPECT_EQ(runtime::object(1.23), number1 / string_empty);
    EXPECT_EQ(runtime::object(1.23), number1 / string_true);
    EXPECT_EQ(runtime::object(1.23), number1 / string_false);
    EXPECT_EQ(runtime::object(1.0), number1 / number1);
    EXPECT_EQ(runtime::object(1.0), number1 / number2);
    EXPECT_EQ(runtime::object("inf"), number1 / number3);
    EXPECT_EQ(runtime::object(1.23), number1 / true_);
    EXPECT_EQ(runtime::object("inf"), number1 / false_);

    EXPECT_EQ(runtime::object(0.0), number3 / true_);
    EXPECT_EQ(runtime::object("-nan"), number3 / false_);
}

TEST(Object, DivBoolean)
{
    runtime::object empty1;
    runtime::object empty2(empty_value);

    runtime::object string1("string");
    runtime::object string2("string");
    runtime::object string3("String");
    runtime::object string4("1.23");
    runtime::object string_empty("");
    runtime::object string_true("true");
    runtime::object string_false("false");

    runtime::object number1(1.23);
    runtime::object number2(1.2300);
    runtime::object number3(0.0);

    runtime::object true_(true);
    runtime::object false_(false);

    EXPECT_EQ(runtime::object(true), true_ / empty1);
    EXPECT_EQ(runtime::object(true), true_ / empty2);
    EXPECT_EQ(runtime::object(true), true_ / string1);
    EXPECT_EQ(runtime::object(true), true_ / string2);
    EXPECT_EQ(runtime::object(true), true_ / string3);
    EXPECT_EQ(runtime::object(true), true_ / string4);
    EXPECT_EQ(runtime::object(true), true_ / string_empty);
    EXPECT_EQ(runtime::object(true), true_ / string_true);
    EXPECT_EQ(runtime::object(true), true_ / string_false);
    EXPECT_EQ(runtime::object(true), true_ / number1);
    EXPECT_EQ(runtime::object(true), true_ / number2);
    EXPECT_EQ(runtime::object(true), true_ / number3);
    EXPECT_EQ(runtime::object(true), true_ / true_);
    EXPECT_EQ(runtime::object(true), true_ / false_);

    EXPECT_EQ(runtime::object(false), false_ / empty1);
    EXPECT_EQ(runtime::object(false), false_ / empty2);
    EXPECT_EQ(runtime::object(false), false_ / string1);
    EXPECT_EQ(runtime::object(false), false_ / string2);
    EXPECT_EQ(runtime::object(false), false_ / string3);
    EXPECT_EQ(runtime::object(false), false_ / string4);
    EXPECT_EQ(runtime::object(false), false_ / string_empty);
    EXPECT_EQ(runtime::object(false), false_ / string_true);
    EXPECT_EQ(runtime::object(false), false_ / string_false);
    EXPECT_EQ(runtime::object(false), false_ / number1);
    EXPECT_EQ(runtime::object(false), false_ / number2);
    EXPECT_EQ(runtime::object(false), false_ / number3);
    EXPECT_EQ(runtime::object(false), false_ / true_);
    EXPECT_EQ(runtime::object(false), false_ / false_);
}
