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

#include "../emel/runtime/string.h"

using namespace emel;

using testing::Eq;

TEST(String, CharacterConversion)
{
	runtime::character c1 = 'q';
	runtime::character c2 = 'w';
	runtime::character c3 = 'e';
	runtime::character c4 = '%';
	runtime::character c5 = '$';
	runtime::character c6 = '(';
	runtime::character c7 = u'я';
	runtime::character c8 = u'Ъ';
	runtime::character c9 = u'щ';
	runtime::character c10 = u'ё';
	runtime::character c11 = u'Ё';
	runtime::character c12 = "";
	runtime::character c13 = "\0";

	EXPECT_EQ("q", c1.to_string());
	EXPECT_EQ("w", c2.to_string());
	EXPECT_EQ("e", c3.to_string());
	EXPECT_EQ("%", c4.to_string());
	EXPECT_EQ("$", c5.to_string());
	EXPECT_EQ("(", c6.to_string());
	EXPECT_EQ("я", c7.to_string());
	EXPECT_EQ("Ъ", c8.to_string());
	EXPECT_EQ("щ", c9.to_string());
	EXPECT_EQ("ё", c10.to_string());
	EXPECT_EQ("Ё", c11.to_string());
	EXPECT_EQ('\0', c12.get());
	EXPECT_EQ('\0', c13.get());

	{
		runtime::character c = c1;
		EXPECT_EQ("q", c.to_string());

		c = c4;
		EXPECT_EQ("%", c.to_string());

		c = c7;
		EXPECT_EQ("я", c.to_string());

		c = c10;
		EXPECT_EQ("ё", c.to_string());

		c = c12;
		EXPECT_EQ('\0', c.get());

		c = c13;
		EXPECT_EQ('\0', c.get());
	}

	{
		runtime::character c;
		EXPECT_EQ('\0', c.c);

		c.set('3');
		EXPECT_EQ("3", c.to_string());

		c.set(u'Ы');
		EXPECT_EQ("Ы", c.to_string());

		c.set("ф");
		EXPECT_EQ("ф", c.to_string());

		c.set("абвгде");
		EXPECT_EQ("а", c.to_string());

		c.set("");
		EXPECT_EQ('\0', c.get());

		c.set("\0");
		EXPECT_EQ('\0', c.get());
	}
}

TEST(String, StringOps)
{
	runtime::string obj1;
	runtime::string obj2("string");
	runtime::string obj3(std::string("string"));

	EXPECT_EQ(runtime::object::is_str, obj1.get_type());
	EXPECT_EQ(runtime::object::is_str, obj2.get_type());
	EXPECT_EQ(runtime::object::is_str, obj3.get_type());

	EXPECT_TRUE(obj1.empty());
	EXPECT_FALSE(obj2.empty());
	EXPECT_FALSE(obj3.empty());

	EXPECT_EQ(0, obj1.size());
	EXPECT_EQ(6, obj2.size());
	EXPECT_EQ(6, obj3.size());

	EXPECT_THROW(obj1[1], std::out_of_range);
	EXPECT_EQ(runtime::object("r"), obj2[2]);
	EXPECT_EQ(runtime::object("i"), obj3[3]);

	obj1.swap(obj2);
	EXPECT_TRUE(obj2.empty());
	EXPECT_EQ("string", obj1.as_string().value());
}