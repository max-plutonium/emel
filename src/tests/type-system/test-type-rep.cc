/*
 * Copyright (C) 2016 Max Plutonium <plutonium.max@gmail.com>
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

#include <emel/type-system/type.h>

using namespace emel;

using testing::IsEmpty;
using testing::SizeIs;
using testing::ElementsAreArray;

TEST(TypeRep, None)
{
	type::rep v;

	bool b = false;
	std::int64_t integer = 0L;
	double number = 0.0;
	std::string string;
	std::vector<type::rep> vector;
	memory::counted_ptr ptr;

	const type *t = v.get_type();
	EXPECT_EQ(type::none, t->get_kind());
	EXPECT_EQ("none", t->get_name());

	EXPECT_TRUE(t->is_system());
	EXPECT_FALSE(t->is_user());
	EXPECT_TRUE(t->is_primitive());
	EXPECT_FALSE(t->is_composite());
	EXPECT_TRUE(t->is_single());
	EXPECT_FALSE(t->is_array());
	EXPECT_TRUE(t->is_local());
	EXPECT_FALSE(t->is_counted());
	EXPECT_FALSE(t->is_managed());

	EXPECT_FALSE(v.get(b));
	EXPECT_FALSE(v.get(integer));
	EXPECT_FALSE(v.get(number));
	EXPECT_FALSE(v.get(string));
	EXPECT_FALSE(v.get(vector));
	EXPECT_FALSE(v.get(ptr));

	EXPECT_FALSE(b);
	EXPECT_EQ(0L, integer);
	EXPECT_EQ(0.0, number);
	EXPECT_EQ("", string);
	EXPECT_THAT(vector, IsEmpty());
	EXPECT_EQ(nullptr, ptr);
}

TEST(TypeRep, Bool)
{
	type::rep v(true);

	bool b = false;
	std::int64_t integer = 0L;
	double number = 0.0;
	std::string string;
	std::vector<type::rep> vector;
	memory::counted_ptr ptr;

	// bool true
	const type *t = v.get_type();
	EXPECT_EQ(type::bool_, t->get_kind());
	EXPECT_EQ("bool", t->get_name());

	EXPECT_TRUE(t->is_system());
	EXPECT_FALSE(t->is_user());
	EXPECT_TRUE(t->is_primitive());
	EXPECT_FALSE(t->is_composite());
	EXPECT_TRUE(t->is_single());
	EXPECT_FALSE(t->is_array());
	EXPECT_TRUE(t->is_local());
	EXPECT_FALSE(t->is_counted());
	EXPECT_FALSE(t->is_managed());

	EXPECT_TRUE(v.get(b));
	EXPECT_FALSE(v.get(integer));
	EXPECT_FALSE(v.get(number));
	EXPECT_FALSE(v.get(string));
	EXPECT_FALSE(v.get(vector));
	EXPECT_FALSE(v.get(ptr));

	EXPECT_TRUE(b);
	EXPECT_EQ(0L, integer);
	EXPECT_EQ(0.0, number);
	EXPECT_EQ("", string);
	EXPECT_THAT(vector, IsEmpty());
	EXPECT_EQ(nullptr, ptr);

	b = true;
	integer = 0L;
	number = 0.0;
	string = "";
	vector.clear();
	ptr = nullptr;

	// bool false
	v.set(false);
	EXPECT_EQ(type::bool_, v.get_type()->get_kind());

	EXPECT_TRUE(v.get(b));
	EXPECT_FALSE(v.get(integer));
	EXPECT_FALSE(v.get(number));
	EXPECT_FALSE(v.get(string));
	EXPECT_FALSE(v.get(vector));
	EXPECT_FALSE(v.get(ptr));

	EXPECT_FALSE(b);
	EXPECT_EQ(0L, integer);
	EXPECT_EQ(0.0, number);
	EXPECT_EQ("", string);
	EXPECT_THAT(vector, IsEmpty());
	EXPECT_EQ(nullptr, ptr);
}

TEST(TypeRep, DISABLED_Int)
{
	type::rep v(123L);

	bool b = false;
	std::int64_t integer = 0L;
	double number = 0.0;
	std::string string;
	std::vector<type::rep> vector;
	memory::counted_ptr ptr;

	const type *t = v.get_type();
	EXPECT_EQ(type::int_, t->get_kind());
	EXPECT_EQ("int", t->get_name());

	EXPECT_TRUE(t->is_system());
	EXPECT_FALSE(t->is_user());
	EXPECT_TRUE(t->is_primitive());
	EXPECT_FALSE(t->is_composite());
	EXPECT_TRUE(t->is_single());
	EXPECT_FALSE(t->is_array());
	EXPECT_TRUE(t->is_local());
	EXPECT_FALSE(t->is_counted());
	EXPECT_FALSE(t->is_managed());

	EXPECT_FALSE(v.get(b));
	EXPECT_TRUE(v.get(integer));
	EXPECT_FALSE(v.get(number));
	EXPECT_FALSE(v.get(string));
	EXPECT_FALSE(v.get(vector));
	EXPECT_FALSE(v.get(ptr));

	EXPECT_FALSE(b);
	EXPECT_EQ(123L, integer);
	EXPECT_EQ(0.0, number);
	EXPECT_EQ("", string);
	EXPECT_THAT(vector, IsEmpty());
	EXPECT_EQ(nullptr, ptr);

	b = false;
	integer = 0L;
	number = 0.0;
	string = "";
	vector.clear();
	ptr = nullptr;

	v.set(std::numeric_limits<decltype(integer)>::min() / 4);
	EXPECT_EQ(type::int_, v.get_type()->get_kind());

	EXPECT_FALSE(v.get(b));
	EXPECT_TRUE(v.get(integer));
	EXPECT_FALSE(v.get(number));
	EXPECT_FALSE(v.get(string));
	EXPECT_FALSE(v.get(vector));
	EXPECT_FALSE(v.get(ptr));

	EXPECT_FALSE(b);
	EXPECT_EQ(std::numeric_limits<decltype(integer)>::min() / 4, integer);
	EXPECT_EQ(0.0, number);
	EXPECT_EQ("", string);
	EXPECT_THAT(vector, IsEmpty());
	EXPECT_EQ(nullptr, ptr);
}

TEST(TypeRep, Num)
{
	type::rep v(123.0);

	bool b = false;
	std::int64_t integer = 0L;
	double number = 0.0;
	std::string string;
	std::vector<type::rep> vector;
	memory::counted_ptr ptr;

	const type *t = v.get_type();
	EXPECT_EQ(type::num, t->get_kind());
	EXPECT_EQ("num", t->get_name());

	EXPECT_TRUE(t->is_system());
	EXPECT_FALSE(t->is_user());
	EXPECT_TRUE(t->is_primitive());
	EXPECT_FALSE(t->is_composite());
	EXPECT_TRUE(t->is_single());
	EXPECT_FALSE(t->is_array());
	EXPECT_TRUE(t->is_local());
	EXPECT_FALSE(t->is_counted());
	EXPECT_FALSE(t->is_managed());

	EXPECT_FALSE(v.get(b));
	EXPECT_FALSE(v.get(integer));
	EXPECT_TRUE(v.get(number));
	EXPECT_FALSE(v.get(string));
	EXPECT_FALSE(v.get(vector));
	EXPECT_FALSE(v.get(ptr));

	EXPECT_FALSE(b);
	EXPECT_EQ(0L, integer);
	EXPECT_EQ(123.0, number);
	EXPECT_EQ("", string);
	EXPECT_THAT(vector, IsEmpty());
	EXPECT_EQ(nullptr, ptr);

	b = false;
	integer = 0L;
	number = 0.0;
	string = "";
	vector.clear();
	ptr = nullptr;

	v.set(-0.00256);
	EXPECT_EQ(type::num, v.get_type()->get_kind());

	EXPECT_FALSE(v.get(b));
	EXPECT_FALSE(v.get(integer));
	EXPECT_TRUE(v.get(number));
	EXPECT_FALSE(v.get(string));
	EXPECT_FALSE(v.get(vector));
	EXPECT_FALSE(v.get(ptr));

	EXPECT_FALSE(b);
	EXPECT_EQ(0L, integer);
	EXPECT_EQ(-0.00256, number);
	EXPECT_EQ("", string);
	EXPECT_THAT(vector, IsEmpty());
	EXPECT_EQ(nullptr, ptr);
}

TEST(TypeRep, Str)
{
	type::rep v("a1ёЫ");

	bool b = false;
	std::int64_t integer = 0L;
	double number = 0.0;
	std::string string;
	std::vector<type::rep> vector;
	memory::counted_ptr ptr;

	const type *t = v.get_type();
	EXPECT_EQ(type::str, t->get_kind());
	EXPECT_EQ("str", t->get_name());

	EXPECT_TRUE(t->is_system());
	EXPECT_FALSE(t->is_user());
	EXPECT_TRUE(t->is_primitive());
	EXPECT_FALSE(t->is_composite());
	EXPECT_FALSE(t->is_single());
	EXPECT_TRUE(t->is_array());
	EXPECT_TRUE(t->is_local());
	EXPECT_FALSE(t->is_counted());
	EXPECT_FALSE(t->is_managed());

	EXPECT_FALSE(v.get(b));
	EXPECT_FALSE(v.get(integer));
	EXPECT_FALSE(v.get(number));
	EXPECT_TRUE(v.get(string));
	EXPECT_FALSE(v.get(vector));
	EXPECT_FALSE(v.get(ptr));

	EXPECT_FALSE(b);
	EXPECT_EQ(0L, integer);
	EXPECT_EQ(0.0, number);
	EXPECT_EQ("a1ёЫ", string);
	EXPECT_THAT(vector, IsEmpty());
	EXPECT_EQ(nullptr, ptr);

	b = false;
	integer = 0L;
	number = 0.0;
	string = "";
	vector.clear();
	ptr = nullptr;

	v.set("это было в жаркий июльский день, когда болота горят");

	t = v.get_type();
	EXPECT_EQ(type::str, t->get_kind());
	EXPECT_EQ("str", t->get_name());

	EXPECT_TRUE(t->is_system());
	EXPECT_FALSE(t->is_user());
	EXPECT_TRUE(t->is_primitive());
	EXPECT_FALSE(t->is_composite());
	EXPECT_FALSE(t->is_single());
	EXPECT_TRUE(t->is_array());
	EXPECT_FALSE(t->is_local());
	EXPECT_TRUE(t->is_counted());
	EXPECT_FALSE(t->is_managed());

	EXPECT_FALSE(v.get(b));
	EXPECT_FALSE(v.get(integer));
	EXPECT_FALSE(v.get(number));
	EXPECT_TRUE(v.get(string));
	EXPECT_FALSE(v.get(vector));
	EXPECT_FALSE(v.get(ptr));

	EXPECT_FALSE(b);
	EXPECT_EQ(0L, integer);
	EXPECT_EQ(0.0, number);
	EXPECT_EQ("это было в жаркий июльский день, когда болота горят", string);
	EXPECT_THAT(vector, IsEmpty());
	EXPECT_EQ(nullptr, ptr);
}

TEST(TypeRep, DISABLED_Arr)
{
	type::rep v(std::vector<type::rep> { 1L, 2L, 3L });

	bool b = false;
	std::int64_t integer = 0L;
	double number = 0.0;
	std::string string;
	std::vector<type::rep> vector;
	memory::counted_ptr ptr;

	const type *t = v.get_type();
	EXPECT_EQ(type::arr, t->get_kind());
	EXPECT_EQ("arr", t->get_name());

	EXPECT_TRUE(t->is_system());
	EXPECT_FALSE(t->is_user());
	EXPECT_TRUE(t->is_primitive());
	EXPECT_FALSE(t->is_composite());
	EXPECT_FALSE(t->is_single());
	EXPECT_TRUE(t->is_array());
	EXPECT_FALSE(t->is_local());
	EXPECT_TRUE(t->is_counted());
	EXPECT_FALSE(t->is_managed());

	EXPECT_FALSE(v.get(b));
	EXPECT_FALSE(v.get(integer));
	EXPECT_FALSE(v.get(number));
	EXPECT_FALSE(v.get(string));
	EXPECT_TRUE(v.get(vector));
	EXPECT_FALSE(v.get(ptr));

	EXPECT_FALSE(b);
	EXPECT_EQ(0L, integer);
	EXPECT_EQ(0.0, number);
	EXPECT_EQ("", string);
	EXPECT_THAT(vector, SizeIs(3));
	EXPECT_EQ(nullptr, ptr);

	std::int64_t idx = 0;
	for(auto &r : vector) {
		std::int64_t val = 0;
		ASSERT_TRUE(r.get(val));
		EXPECT_EQ(++idx, val);
	}
}

TEST(TypeRep, DISABLED_Ptr)
{
	type::rep v(reinterpret_cast<void *>(0x10247890));

	bool b = false;
	std::int64_t integer = 0L;
	double number = 0.0;
	std::string string;
	std::vector<type::rep> vector;
	memory::counted_ptr ptr;

	EXPECT_EQ(type::ptr, v.get_type()->get_kind());

	EXPECT_FALSE(v.get(b));
	EXPECT_FALSE(v.get(integer));
	EXPECT_FALSE(v.get(number));
	EXPECT_FALSE(v.get(string));
	EXPECT_FALSE(v.get(vector));
	EXPECT_TRUE(v.get(ptr));

	EXPECT_FALSE(b);
	EXPECT_EQ(0L, integer);
	EXPECT_EQ(0.0, number);
	EXPECT_EQ("", string);
	EXPECT_THAT(vector, IsEmpty());
	EXPECT_EQ(reinterpret_cast<void *>(0x10247890), ptr);
}

TEST(TypeRep, Copying)
{
	type::rep v1(123L);
	type::rep v2(v1);
	type::rep v3("qwerty-qwerty-qwerty");
	type::rep v4;

	EXPECT_EQ(type::int_, v1.get_type()->get_kind());
	EXPECT_EQ(type::int_, v2.get_type()->get_kind());
	EXPECT_EQ(type::str, v3.get_type()->get_kind());
	EXPECT_EQ(type::none, v4.get_type()->get_kind());

	bool b = false;
	std::int64_t integer = 0L;
	double number = 0.0;
	std::string string;
	std::vector<type::rep> vector;
	memory::counted_ptr ptr;

	EXPECT_FALSE(v2.get(b));
	EXPECT_TRUE(v2.get(integer));
	EXPECT_FALSE(v2.get(number));
	EXPECT_FALSE(v2.get(string));
	EXPECT_FALSE(v2.get(vector));
	EXPECT_FALSE(v2.get(ptr));

	EXPECT_FALSE(b);
	EXPECT_EQ(123L, integer);
	EXPECT_EQ(0.0, number);
	EXPECT_EQ("", string);
	EXPECT_THAT(vector, IsEmpty());
	EXPECT_EQ(nullptr, ptr);

	v4 = v2;

	EXPECT_EQ(type::int_, v1.get_type()->get_kind());
	EXPECT_EQ(type::int_, v2.get_type()->get_kind());
	EXPECT_EQ(type::str, v3.get_type()->get_kind());
	EXPECT_EQ(type::int_, v4.get_type()->get_kind());

	b = false;
	integer = 0L;
	number = 0.0;
	string = "";
	vector.clear();
	ptr = nullptr;

	EXPECT_FALSE(v4.get(b));
	EXPECT_TRUE(v4.get(integer));
	EXPECT_FALSE(v4.get(number));
	EXPECT_FALSE(v4.get(string));
	EXPECT_FALSE(v4.get(vector));
	EXPECT_FALSE(v4.get(ptr));

	EXPECT_FALSE(b);
	EXPECT_EQ(123L, integer);
	EXPECT_EQ(0.0, number);
	EXPECT_EQ("", string);
	EXPECT_THAT(vector, IsEmpty());
	EXPECT_EQ(nullptr, ptr);

	v4 = v3;

	EXPECT_EQ(type::int_, v1.get_type()->get_kind());
	EXPECT_EQ(type::int_, v2.get_type()->get_kind());
	EXPECT_EQ(type::str, v3.get_type()->get_kind());
	EXPECT_EQ(type::str, v4.get_type()->get_kind());

	b = false;
	integer = 0L;
	number = 0.0;
	string = "";
	vector.clear();
	ptr = nullptr;

	EXPECT_FALSE(v4.get(b));
	EXPECT_FALSE(v4.get(integer));
	EXPECT_FALSE(v4.get(number));
	EXPECT_TRUE(v4.get(string));
	EXPECT_FALSE(v4.get(vector));
	EXPECT_FALSE(v4.get(ptr));

	EXPECT_FALSE(b);
	EXPECT_EQ(0L, integer);
	EXPECT_EQ(0.0, number);
	EXPECT_EQ("qwerty-qwerty-qwerty", string);
	EXPECT_THAT(vector, IsEmpty());
	EXPECT_EQ(nullptr, ptr);

	type::rep v5(v4);

	EXPECT_EQ(type::int_, v1.get_type()->get_kind());
	EXPECT_EQ(type::int_, v2.get_type()->get_kind());
	EXPECT_EQ(type::str, v3.get_type()->get_kind());
	EXPECT_EQ(type::str, v4.get_type()->get_kind());
	EXPECT_EQ(type::str, v5.get_type()->get_kind());

	b = false;
	integer = 0L;
	number = 0.0;
	string = "";
	vector.clear();
	ptr = nullptr;

	EXPECT_FALSE(v5.get(b));
	EXPECT_FALSE(v5.get(integer));
	EXPECT_FALSE(v5.get(number));
	EXPECT_TRUE(v5.get(string));
	EXPECT_FALSE(v5.get(vector));
	EXPECT_FALSE(v5.get(ptr));

	EXPECT_FALSE(b);
	EXPECT_EQ(0L, integer);
	EXPECT_EQ(0.0, number);
	EXPECT_EQ("qwerty-qwerty-qwerty", string);
	EXPECT_THAT(vector, IsEmpty());
	EXPECT_EQ(nullptr, ptr);
}

TEST(TypeRep, Moving)
{
	type::rep v1(123L);
	type::rep v2(std::move(v1));
	type::rep v3("qwerty-qwerty-qwerty");
	type::rep v4;

	EXPECT_EQ(type::none, v1.get_type()->get_kind());
	EXPECT_EQ(type::int_, v2.get_type()->get_kind());
	EXPECT_EQ(type::str, v3.get_type()->get_kind());
	EXPECT_EQ(type::none, v4.get_type()->get_kind());

	bool b = false;
	std::int64_t integer = 0L;
	double number = 0.0;
	std::string string;
	std::vector<type::rep> vector;
	memory::counted_ptr ptr;

	EXPECT_FALSE(v2.get(b));
	EXPECT_TRUE(v2.get(integer));
	EXPECT_FALSE(v2.get(number));
	EXPECT_FALSE(v2.get(string));
	EXPECT_FALSE(v2.get(vector));
	EXPECT_FALSE(v2.get(ptr));

	EXPECT_FALSE(b);
	EXPECT_EQ(123L, integer);
	EXPECT_EQ(0.0, number);
	EXPECT_EQ("", string);
	EXPECT_THAT(vector, IsEmpty());
	EXPECT_EQ(nullptr, ptr);

	v4 = std::move(v2);

	EXPECT_EQ(type::none, v1.get_type()->get_kind());
	EXPECT_EQ(type::none, v2.get_type()->get_kind());
	EXPECT_EQ(type::str, v3.get_type()->get_kind());
	EXPECT_EQ(type::int_, v4.get_type()->get_kind());

	b = false;
	integer = 0L;
	number = 0.0;
	string = "";
	vector.clear();
	ptr = nullptr;

	EXPECT_FALSE(v4.get(b));
	EXPECT_TRUE(v4.get(integer));
	EXPECT_FALSE(v4.get(number));
	EXPECT_FALSE(v4.get(string));
	EXPECT_FALSE(v4.get(vector));
	EXPECT_FALSE(v4.get(ptr));

	EXPECT_FALSE(b);
	EXPECT_EQ(123L, integer);
	EXPECT_EQ(0.0, number);
	EXPECT_EQ("", string);
	EXPECT_THAT(vector, IsEmpty());
	EXPECT_EQ(nullptr, ptr);

	v4 = std::move(v3);

	EXPECT_EQ(type::none, v1.get_type()->get_kind());
	EXPECT_EQ(type::none, v2.get_type()->get_kind());
	EXPECT_EQ(type::none, v3.get_type()->get_kind());
	EXPECT_EQ(type::str, v4.get_type()->get_kind());

	b = false;
	integer = 0L;
	number = 0.0;
	string = "";
	vector.clear();
	ptr = nullptr;

	EXPECT_FALSE(v4.get(b));
	EXPECT_FALSE(v4.get(integer));
	EXPECT_FALSE(v4.get(number));
	EXPECT_TRUE(v4.get(string));
	EXPECT_FALSE(v4.get(vector));
	EXPECT_FALSE(v4.get(ptr));

	EXPECT_FALSE(b);
	EXPECT_EQ(0L, integer);
	EXPECT_EQ(0.0, number);
	EXPECT_EQ("qwerty-qwerty-qwerty", string);
	EXPECT_THAT(vector, IsEmpty());
	EXPECT_EQ(nullptr, ptr);

	type::rep v5(std::move(v4));

	EXPECT_EQ(type::none, v1.get_type()->get_kind());
	EXPECT_EQ(type::none, v2.get_type()->get_kind());
	EXPECT_EQ(type::none, v3.get_type()->get_kind());
	EXPECT_EQ(type::none, v4.get_type()->get_kind());
	EXPECT_EQ(type::str, v5.get_type()->get_kind());

	b = false;
	integer = 0L;
	number = 0.0;
	string = "";
	vector.clear();
	ptr = nullptr;

	EXPECT_FALSE(v5.get(b));
	EXPECT_FALSE(v5.get(integer));
	EXPECT_FALSE(v5.get(number));
	EXPECT_TRUE(v5.get(string));
	EXPECT_FALSE(v5.get(vector));
	EXPECT_FALSE(v5.get(ptr));

	EXPECT_FALSE(b);
	EXPECT_EQ(0L, integer);
	EXPECT_EQ(0.0, number);
	EXPECT_EQ("qwerty-qwerty-qwerty", string);
	EXPECT_THAT(vector, IsEmpty());
	EXPECT_EQ(nullptr, ptr);
}
