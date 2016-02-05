/*
 * Copyright (C) 2015 Max Plutonium <plutonium.max@gmail.com>
 *
 * This file is part of the EMEL library.
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
#pragma once

#include <boost/container/pmr/memory_resource.hpp>

namespace emel { namespace memory {

class gc
{
public:
	static void *alloc_object(std::size_t size);
	static void *alloc_atomic(std::size_t size);
	static void *alloc_array(std::size_t size);
	static void deallocate(void *ptr);

	static void register_finalizer(void *obj, void (*function)(void *, void *), void *arg);
	static std::size_t run_finalizers();
	static bool should_run_finalizers();
	static void finalize_all();
	static void run_gc();

	static std::size_t total_memory();
	static std::size_t free_memory();

	static void set_initial_heap_size(std::size_t size);
	static void set_max_heap_size(std::size_t size);
	static std::size_t set_free_space_divisor(std::size_t div);

	static boost::container::pmr::memory_resource *get_source();
	static void enable_gc();
	static void disable_gc();
	static bool gc_enabled();

	static void *get_base_address(void *obj);
	static bool is_same_object(void *first, void *second);

	static void attach_thread();
	static void detach_thread();
};

template <typename Tp>
static void finalize(void *obj, void *offset)
{
	reinterpret_cast<Tp *>(static_cast<char *>(obj)
		+ reinterpret_cast<std::ptrdiff_t>(offset))->~Tp();
}

template <typename Tp>
static void *alloc(std::size_t size)
{
	void *const ptr = gc::alloc_object(size);
	void *const base_addr = gc::get_base_address(ptr);
	gc::register_finalizer(ptr, &finalize<Tp>,
		reinterpret_cast<void *>(static_cast<char *>(ptr) - static_cast<char *>(base_addr)));
	return ptr;
}

} // namespace memory

} // namespace emel
