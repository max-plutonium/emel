/*
 * Copyright (C) 2016 Max Plutonium <plutonium.max@gmail.com>
 *
 * This file is part of the benchmark suite of the EMEL library.
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
#include <benchmark/benchmark.h>

#include <emel/memory/memory.h>

using namespace emel;

using allocated_type = void *;
static constexpr auto item_size = sizeof(memory::atomic_counted)
	+ sizeof(std::aligned_storage<sizeof(allocated_type),
		std::alignment_of<allocated_type>::value>::type);

static const char *source_name(memory::source_type type)
{
	switch (type) {
		case memory::default_pool: return "default pool";
		case memory::bitmap_pool: return "bitmap pool";
		case memory::gnu_pool: return "gnu pool";
		case memory::mt_pool: return "mt pool";
		case memory::boost_pool: return "boost pool";
		case memory::collectable_gc_pool: return "coll pool";
		case memory::atomic_gc_pool: return "atomic pool";
		case memory::uncollectable_gc_pool: return "uncoll pool";
		case memory::atomic_uncollectable_gc_pool: return "atomic uncoll pool";
	}

	return "";
}

static void Memory_GetSource(benchmark::State &state) {
	while (state.KeepRunning()) {
		benchmark::DoNotOptimize(memory::get_source());
	}
}

static void Memory_MakeOneObject(benchmark::State &state)
{
	const auto st = static_cast<memory::source_type>(state.range_x());
	auto *source = memory::get_source(st);

	if(memory::collectable_gc_pool <= st)
		memory::attach_thread();

	while (state.KeepRunning()) {
		benchmark::DoNotOptimize(memory::counted_ptr(
			memory::allocate_counted<allocated_type>(rt_allocator<allocated_type>(source), allocated_type()),
				false));
	}

	if(memory::collectable_gc_pool <= st)
		memory::detach_thread();

	state.SetLabel(source_name(st));
	state.SetItemsProcessed(state.iterations());
	state.SetBytesProcessed(state.iterations() * item_size);
}

static void Memory_MakeManyObjects(benchmark::State &state)
{
	const auto st = static_cast<memory::source_type>(state.range_x());
	auto *source = memory::get_source(st);

	if(memory::collectable_gc_pool <= st)
		memory::attach_thread();

	const bool need_gc = (memory::collectable_gc_pool == st) || (memory::atomic_gc_pool == st);

	while (state.KeepRunning()) {
		std::vector<memory::counted_ptr, rt_allocator<memory::counted_ptr>>
			ptrs(std::size_t(state.range_y()), rt_allocator<memory::counted_ptr>(source));

		for(auto i = 0; i < state.range_y(); ++i)
			ptrs.emplace_back(memory::allocate_counted<allocated_type>(
				rt_allocator<allocated_type>(source), allocated_type()), need_gc);
	}

	if(memory::collectable_gc_pool <= st)
		memory::detach_thread();

	state.SetLabel(source_name(st));
	state.SetItemsProcessed(state.iterations() * state.range_y());
	state.SetBytesProcessed(state.iterations() * state.range_y() * item_size);
}

static void set_objects_count(benchmark::internal::Benchmark *bench) {
	for (int i = memory::default_pool; i < memory::last_source_type; ++i)
		for (int j = 10; j <= 1000000; j *= 10)
			bench->ArgPair(i, j);
}

BENCHMARK(Memory_GetSource);

BENCHMARK(Memory_MakeOneObject)->DenseRange(memory::default_pool,
	memory::atomic_uncollectable_gc_pool);
BENCHMARK(Memory_MakeOneObject)->DenseRange(memory::default_pool,
	memory::atomic_uncollectable_gc_pool)->ThreadRange(2, 32);

BENCHMARK(Memory_MakeManyObjects)->Apply(set_objects_count);
BENCHMARK(Memory_MakeManyObjects)->Apply(set_objects_count)->ThreadRange(2, 32);
