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

#include <emel/type-system/context.h>

using namespace emel;

using testing::NotNull;
using testing::IsNull;
using testing::Eq;

TEST(TypeContext, ShouldRegisterModuleByName)
{
	context_info_storage globals = context_info_storage::create();

	auto mod1 = globals.register_module("emel");
	auto mod2 = globals.register_module("emel/lang");

	EXPECT_THAT(mod1.first, NotNull());
	EXPECT_THAT(mod2.first, NotNull());
	EXPECT_NE(mod1.first, mod2.first);
	EXPECT_TRUE(mod1.second);
	EXPECT_TRUE(mod2.second);

	auto mod3 = globals.register_module("emel");
	EXPECT_EQ(mod1.first, mod3.first);
	EXPECT_FALSE(mod3.second);

	auto mod4 = globals.register_module("undefined/lang");
	EXPECT_THAT(mod4.first, IsNull());
	EXPECT_FALSE(mod4.second);

	EXPECT_THAT(mod1.first->parent, IsNull());
	EXPECT_EQ(mod1.first, mod2.first->parent);
}

TEST(TypeContext, ShouldGetModuleByName)
{
	context_info_storage globals = context_info_storage::create();

	globals.register_module("emel");
	globals.register_module("emel/lang");

	auto mod1 = globals.get_module("emel");
	auto mod2 = globals.get_module("emel/lang");

	EXPECT_THAT(mod1.first, NotNull());
	EXPECT_THAT(mod2.first, NotNull());
	EXPECT_NE(mod1.first, mod2.first);
	EXPECT_TRUE(mod1.second);
	EXPECT_TRUE(mod2.second);

	auto mod3 = globals.get_module("emel");
	EXPECT_EQ(mod1.first, mod3.first);
	EXPECT_TRUE(mod3.second);

	auto mod4 = globals.get_module("undefined/lang");
	EXPECT_THAT(mod4.first, IsNull());
	EXPECT_FALSE(mod4.second);

	EXPECT_THAT(mod1.first->parent, IsNull());
	EXPECT_EQ(mod1.first, mod2.first->parent);
}

TEST(TypeContext, ShouldRegisterTypeByName)
{
	context_info_storage globals = context_info_storage::create();

	globals.register_module("emel");
	auto mod = globals.register_module("emel/lang");

	auto ty1 = globals.register_ti(type::ptr, "emel/lang/ptr");
	auto ty2 = globals.register_ti(type::bool_, "emel/lang/bool");

	EXPECT_THAT(ty1.first, NotNull());
	EXPECT_THAT(ty2.first, NotNull());
	EXPECT_NE(ty1.first, ty2.first);
	EXPECT_TRUE(ty1.second);
	EXPECT_TRUE(ty2.second);

	auto ty3 = globals.register_ti(type::ptr, "emel/lang/ptr");
	EXPECT_EQ(ty1.first, ty3.first);
	EXPECT_FALSE(ty3.second);

	auto ty4 = globals.register_ti(type::ptr, "undefined/lang");
	EXPECT_THAT(ty4.first, IsNull());
	EXPECT_FALSE(ty4.second);

	EXPECT_EQ(mod.first, ty1.first->parent);
	EXPECT_EQ(mod.first, ty2.first->parent);
}

TEST(TypeContext, ShouldGetTypeByName)
{
	context_info_storage globals = context_info_storage::create();

	globals.register_module("emel");
	auto mod = globals.register_module("emel/lang");

	globals.register_ti(type::ptr, "emel/lang/ptr");
	globals.register_ti(type::bool_, "emel/lang/bool");

	auto ty1 = globals.get_ti("emel/lang/ptr");
	auto ty2 = globals.get_ti("emel/lang/bool");

	EXPECT_THAT(ty1.first, NotNull());
	EXPECT_THAT(ty2.first, NotNull());
	EXPECT_NE(ty1.first, ty2.first);
	EXPECT_TRUE(ty1.second);
	EXPECT_TRUE(ty2.second);

	auto ty3 = globals.get_ti("emel/lang/ptr");
	EXPECT_EQ(ty1.first, ty3.first);
	EXPECT_TRUE(ty3.second);

	auto ty4 = globals.get_ti("undefined/lang");
	EXPECT_THAT(ty4.first, IsNull());
	EXPECT_FALSE(ty4.second);

	EXPECT_EQ(mod.first, ty1.first->parent);
	EXPECT_EQ(mod.first, ty2.first->parent);
}
