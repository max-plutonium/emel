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
#include "gc.h"

#include <cassert>

#define GC_THREADS
#include <gc_allocator.h>
#include <javaxfc.h>

namespace emel { namespace memory {

class gc_memory_resource : public std::experimental::pmr::memory_resource
{
protected:
	virtual void *do_allocate(size_t bytes, size_t alignment) override;
	virtual void do_deallocate(void *ptr, size_t bytes, size_t alignment) override;
	virtual bool do_is_equal(const std::experimental::pmr::memory_resource &other) const noexcept override;
};

void *gc_memory_resource::do_allocate(size_t bytes, size_t /*alignment*/)
{
	return gc::alloc_object(bytes);
}

void gc_memory_resource::do_deallocate(
	void *ptr, size_t /*bytes*/, size_t /*alignment*/)
{
	gc::deallocate(ptr);
}

bool gc_memory_resource::do_is_equal(
	const std::experimental::pmr::memory_resource &other) const noexcept
{
	return (typeid(other) == typeid(gc_memory_resource)
		&& this == &other);
}

/*static*/ void *gc::alloc_object(std::size_t size)
{
	return GC_MALLOC(size);
}

/*static*/ void *gc::alloc_atomic(std::size_t size)
{
	return GC_MALLOC_ATOMIC(size);
}

/*static*/ void *gc::alloc_array(std::size_t size)
{
	return GC_MALLOC(size);
}

void gc::deallocate(void *ptr)
{
	GC_FREE(ptr);
}

/*static*/ void gc::register_finalizer(
	void *obj, void (*function)(void *, void *), void *arg)
{
	GC_REGISTER_FINALIZER_NO_ORDER(
		obj, function, arg, nullptr, nullptr);
}

/*static*/ std::size_t gc::run_finalizers()
{
	return std::size_t(GC_invoke_finalizers());
}

/*static*/ bool gc::should_run_finalizers()
{
	return bool(GC_should_invoke_finalizers());
}

/*static*/ void gc::finalize_all()
{
	GC_finalize_all();
}

/*static*/ void gc::run_gc()
{
	GC_gcollect();
}

/*static*/ std::size_t gc::total_memory()
{
	return GC_get_heap_size();
}

/*static*/ std::size_t gc::free_memory()
{
	return GC_get_free_bytes();
}

/*static*/ void gc::set_initial_heap_size(std::size_t size)
{
	const std::size_t current_size = GC_get_heap_size();
	if(size > current_size)
		GC_expand_hp(size - current_size);
}

/*static*/ void gc::set_max_heap_size(std::size_t size)
{
	GC_set_max_heap_size(size);
}

/*static*/ std::size_t gc::set_free_space_divisor(std::size_t div)
{
	const auto prev_divisor = GC_get_free_space_divisor();
	GC_set_free_space_divisor(div);
	return prev_divisor;
}

/*static*/ void gc::init()
{
	GC_set_all_interior_pointers(true);
	GC_set_java_finalization(true);
	GC_INIT();
	GC_allow_register_threads();
}

/*static*/ std::experimental::pmr::memory_resource *gc::get_source()
{
	static gc_memory_resource instance;
	//static std::experimental::pmr::resource_adaptor<gc_allocator<char>> instance;
	return &instance;
}

/*static*/ void gc::enable_gc()
{
	GC_enable();
}

/*static*/ void gc::disable_gc()
{
	GC_disable();
}

/*static*/ bool gc::gc_enabled()
{
	return !GC_is_disabled();
}

/*static*/ void *gc::get_base_address(void *obj)
{
	return GC_base(obj);
}

/*static*/ bool gc::is_same_object(void *first, void *second)
{
	return nullptr != GC_same_obj(first, second);
}

/*static*/ void gc::attach_thread()
{
    GC_stack_base sb;
	auto res = GC_get_stack_base(&sb);
	assert(GC_UNIMPLEMENTED != res);

	res = GC_register_my_thread(&sb);
	assert(GC_DUPLICATE != res);
}

/*static*/ void gc::detach_thread()
{
	const auto res = GC_unregister_my_thread();
	assert(GC_SUCCESS == res);
}

} // namespace memory

} // namespace emel
