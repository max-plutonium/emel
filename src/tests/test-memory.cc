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

#include <emel/memory/memory.h>

using namespace emel;

using testing::InSequence;

class mock_atomic_counted
{
public:
	mock_atomic_counted() = default;
	mock_atomic_counted(memory::atomic_counted *link) : link(link) { }
	mock_atomic_counted(const mock_atomic_counted &o) : link(o.link) { o.copy(); }
	~mock_atomic_counted() { if(link) link->release(); dtor(); }
	MOCK_METHOD0(dtor, void ());
	MOCK_CONST_METHOD0(copy, void ());

	memory::atomic_counted *link = nullptr;
};

TEST(Memory, CountedAcquireRelease)
{
	memory::counted_ptr p(memory::make_counted<mock_atomic_counted>(), false);

	EXPECT_TRUE(p->unique());
	EXPECT_EQ(1, p->use_count());
	EXPECT_EQ(1, p->weak_count());

	p->acquire();

	EXPECT_FALSE(p->unique());
	EXPECT_EQ(2, p->use_count());
	EXPECT_EQ(1, p->weak_count());

	p->release();

	EXPECT_TRUE(p->unique());
	EXPECT_EQ(1, p->use_count());
	EXPECT_EQ(1, p->weak_count());

	EXPECT_CALL(*p->get<mock_atomic_counted>(), dtor());

	p->release();
	p.detach();
}

TEST(Memory, CountedClone)
{
	InSequence seq;

	memory::counted_ptr p1(memory::make_counted<mock_atomic_counted>(), false);

	EXPECT_TRUE(p1->unique());
	EXPECT_EQ(1, p1->use_count());
	EXPECT_EQ(1, p1->weak_count());

	EXPECT_CALL(*p1->get<mock_atomic_counted>(), copy());

	memory::counted_ptr p2(p1->clone(), false);

	EXPECT_TRUE(p1->unique());
	EXPECT_EQ(1, p1->use_count());
	EXPECT_EQ(1, p1->weak_count());

	EXPECT_TRUE(p2->unique());
	EXPECT_EQ(1, p2->use_count());
	EXPECT_EQ(1, p2->weak_count());

	EXPECT_CALL(*p2->get<mock_atomic_counted>(), dtor());
	EXPECT_CALL(*p1->get<mock_atomic_counted>(), dtor());
}

TEST(Memory, CountedShared)
{
	InSequence seq;

	memory::counted_ptr p1(memory::make_counted<mock_atomic_counted>(), false);

	EXPECT_TRUE(p1->unique());
	EXPECT_EQ(1, p1->use_count());
	EXPECT_EQ(1, p1->weak_count());

	memory::counted_ptr p2 = p1;

	EXPECT_FALSE(p1->unique());
	EXPECT_EQ(2, p1->use_count());
	EXPECT_EQ(1, p1->weak_count());

	EXPECT_FALSE(p2->unique());
	EXPECT_EQ(2, p2->use_count());
	EXPECT_EQ(1, p2->weak_count());

	EXPECT_CALL(*p1->get<mock_atomic_counted>(), dtor());
}

TEST(Memory, GC)
{
	auto stat = memory::get_collectable_memory_usage();
	auto hs = memory::get_collectable_heap_size();
	auto fm = memory::get_free_collectable_memory();
	auto tb = memory::get_total_collectable_bytes();

	EXPECT_EQ(65536, stat.heap_size);
	EXPECT_EQ(65536, stat.free_bytes);
	EXPECT_EQ(0, stat.unmapped_bytes);
	EXPECT_EQ(0, stat.bytes_since_last_gc);
	EXPECT_EQ(0, stat.total_allocated_bytes);

	EXPECT_EQ(65536, hs);
	EXPECT_EQ(65536, fm);
	EXPECT_EQ(0, tb);

	EXPECT_TRUE(memory::gc_enabled());
	memory::disable_gc();
	EXPECT_FALSE(memory::gc_enabled());
	memory::enable_gc();
	EXPECT_TRUE(memory::gc_enabled());

	auto p1 = memory::get_source(memory::collectable_pool)->allocate(12, 16);
	auto p2 = memory::get_source(memory::collectable_pool)->allocate(12, 16);

	auto stat2 = memory::get_collectable_memory_usage();
	auto hs2 = memory::get_collectable_heap_size();
	auto fm2 = memory::get_free_collectable_memory();
	auto tb2 = memory::get_total_collectable_bytes();

	auto sz1 = memory::get_collectable_size(p1);
	auto sz2 = memory::get_collectable_size(p2);

	EXPECT_EQ(65536, stat2.heap_size);
	EXPECT_EQ(hs2 - tb2, stat2.free_bytes);
	EXPECT_EQ(0, stat2.unmapped_bytes);
	EXPECT_EQ(sz1 + sz2, stat2.bytes_since_last_gc);
	EXPECT_EQ(sz1 + sz2, stat2.total_allocated_bytes);

	EXPECT_EQ(65536, hs2);
	EXPECT_EQ(hs2 - tb2, fm2);
	EXPECT_EQ(4096, tb2);

	EXPECT_GT(p1, memory::get_base_address(p1));
	EXPECT_TRUE(memory::is_collectable(p1));
	EXPECT_FALSE(memory::is_collectable(&p1));

	p1 = p2 = nullptr;

	memory::run_gc();

	auto stat3 = memory::get_collectable_memory_usage();
	auto hs3 = memory::get_collectable_heap_size();
	auto fm3 = memory::get_free_collectable_memory();
	auto tb3 = memory::get_total_collectable_bytes();

	EXPECT_EQ(65536, stat3.heap_size);
	EXPECT_EQ(hs3 - tb3, stat3.free_bytes);
	EXPECT_EQ(0, stat3.unmapped_bytes);
	EXPECT_EQ(0, stat3.bytes_since_last_gc);
	EXPECT_EQ(sz1 + sz2, stat3.total_allocated_bytes);

	EXPECT_EQ(65536, hs3);
	EXPECT_EQ(hs3 - tb3, fm3);
	EXPECT_EQ(4096, tb3);
}

TEST(Memory, CollectedAcquireRelease)
{
	memory::counted_ptr p(memory::make_collectable<mock_atomic_counted>(), false);

	memory::run_gc();

	EXPECT_TRUE(p->unique());
	EXPECT_EQ(1, p->use_count());
	EXPECT_EQ(2, p->weak_count());

	p->acquire();

	memory::run_gc();

	EXPECT_FALSE(p->unique());
	EXPECT_EQ(2, p->use_count());
	EXPECT_EQ(2, p->weak_count());

	p->release();

	memory::run_gc();

	EXPECT_TRUE(p->unique());
	EXPECT_EQ(1, p->use_count());
	EXPECT_EQ(2, p->weak_count());

	EXPECT_CALL(*p->get<mock_atomic_counted>(), dtor());

	p.detach();

	memory::run_gc();
	memory::run_gc();
}

TEST(Memory, CollectedClone)
{
	InSequence seq;

	memory::counted_ptr p1(memory::make_collectable<mock_atomic_counted>(), false);

	memory::run_gc();

	EXPECT_TRUE(p1->unique());
	EXPECT_EQ(1, p1->use_count());
	EXPECT_EQ(2, p1->weak_count());

	EXPECT_CALL(*p1->get<mock_atomic_counted>(), copy());

	memory::counted_ptr p2(p1->clone(), false);

	memory::run_gc();

	EXPECT_TRUE(p1->unique());
	EXPECT_EQ(1, p1->use_count());
	EXPECT_EQ(2, p1->weak_count());

	EXPECT_TRUE(p2->unique());
	EXPECT_EQ(1, p2->use_count());
	EXPECT_EQ(2, p2->weak_count());

	EXPECT_CALL(*p1->get<mock_atomic_counted>(), dtor());
	EXPECT_CALL(*p2->get<mock_atomic_counted>(), dtor());

	p1.detach();
	p2.detach();

	memory::run_gc();
	memory::run_gc();
}

TEST(Memory, CollectedShared)
{
	InSequence seq;

	memory::counted_ptr p1(memory::make_collectable<mock_atomic_counted>(), false);

	EXPECT_TRUE(p1->unique());
	EXPECT_EQ(1, p1->use_count());
	EXPECT_EQ(2, p1->weak_count());

	memory::counted_ptr p2 = p1;

	EXPECT_FALSE(p1->unique());
	EXPECT_EQ(2, p1->use_count());
	EXPECT_EQ(2, p1->weak_count());

	EXPECT_FALSE(p2->unique());
	EXPECT_EQ(2, p2->use_count());
	EXPECT_EQ(2, p2->weak_count());

	memory::run_gc();

	EXPECT_CALL(*p1->get<mock_atomic_counted>(), dtor());
}

TEST(Memory, CollectedShouldCollectCyclicRefs)
{
	InSequence seq;

	auto p1  = memory::make_collectable<mock_atomic_counted>();
	auto p2  = memory::make_collectable<mock_atomic_counted>(p1);
	p1->get<mock_atomic_counted>()->link = p2;

	EXPECT_CALL(*p2->get<mock_atomic_counted>(), dtor());
	EXPECT_CALL(*p1->get<mock_atomic_counted>(), dtor());

	memory::run_gc();
	p2 = 0;
	memory::run_gc();
	p1 = 0;

	memory::run_gc();
	memory::run_gc();
}
