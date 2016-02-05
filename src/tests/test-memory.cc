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

#include "../emel/memory/memory.h"

using namespace emel::memory;

using testing::InSequence;

class mock_atomic_counted
{
public:
	mock_atomic_counted() = default;
	mock_atomic_counted(mock_atomic_counted &o) { o.copy(); }
	~mock_atomic_counted() { dtor(); }
	MOCK_METHOD0(dtor, void ());
	MOCK_METHOD0(copy, void ());
};

TEST(Memory, AcquireRelease)
{
	counted_ptr p(make_counted<mock_atomic_counted>(), false);

	EXPECT_TRUE(p->unique());
	EXPECT_EQ(1, p->use_count());

	p->acquire();

	EXPECT_FALSE(p->unique());
	EXPECT_EQ(2, p->use_count());

	p->release();

	EXPECT_TRUE(p->unique());
	EXPECT_EQ(1, p->use_count());

	EXPECT_CALL(*p->get<mock_atomic_counted>(), dtor());

	p->release();
	p.detach();
}

TEST(Memory, Clone)
{
	InSequence seq;

	counted_ptr p(make_counted<mock_atomic_counted>(), false);

	EXPECT_TRUE(p->unique());
	EXPECT_EQ(1, p->use_count());

	EXPECT_CALL(*p->get<mock_atomic_counted>(), copy());

	counted_ptr p2(p->clone(), false);

	EXPECT_TRUE(p->unique());
	EXPECT_EQ(1, p->use_count());
	EXPECT_TRUE(p2->unique());
	EXPECT_EQ(1, p2->use_count());

	EXPECT_CALL(*p2->get<mock_atomic_counted>(), dtor());
	EXPECT_CALL(*p->get<mock_atomic_counted>(), dtor());
}
