/*
 * Copyright (C) 2016 Max Plutonium <plutonium.max@gmail.com>
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
#include "memory.h"

#include <boost/container/pmr/synchronized_pool_resource.hpp>
#include <boost/container/pmr/resource_adaptor.hpp>

#include <ext/bitmap_allocator.h>
#include <ext/mt_allocator.h>
#include <ext/pool_allocator.h>

#include <gc.h>
#include <javaxfc.h>

#include <mutex>

namespace emel {

template <typename Tp>
using gnu_pool_allocator = __gnu_cxx::__pool_alloc<Tp>;

template <typename Tp>
using bitmap_allocator = __gnu_cxx::bitmap_allocator<Tp>;

template <typename Tp>
using mt_allocator = __gnu_cxx::__mt_alloc<Tp>;

class gc_memory_resource final : public boost::container::pmr::memory_resource
{
protected:
	virtual void *do_allocate(size_t bytes, size_t alignment) override;
	virtual void do_deallocate(void *ptr, size_t bytes, size_t alignment) override;
	virtual bool do_is_equal(const memory::resource_type &other) const noexcept override;

public:
	enum memory_kind {
		collectable, atomic, uncollectable, atomic_uncollectable
	};

	explicit gc_memory_resource(memory_kind k);

private:
	const memory_kind k;
};

template <typename Tp>
using pmr_adaptor = boost::container::pmr::resource_adaptor<Tp>;

static std::once_flag s_flag;
static std::array<boost::container::pmr::memory_resource *, memory::last_source_type> s_sources;

static void once_init()
{
	static pmr_adaptor<bitmap_allocator<char>> bitmap_pool_instance;
	static pmr_adaptor<gnu_pool_allocator<char>> gnu_pool_instance;
	static pmr_adaptor<mt_allocator<char>> mt_pool_instance;
	static boost::container::pmr::synchronized_pool_resource boost_pool_instance;
	static gc_memory_resource collectable_gc_instance(gc_memory_resource::collectable);
	static gc_memory_resource atomic_gc_instance(gc_memory_resource::atomic);
	static gc_memory_resource uncollectable_gc_instance(gc_memory_resource::uncollectable);
	static gc_memory_resource atomic_uncollectable_gc_instance(gc_memory_resource::atomic_uncollectable);

	auto opt = mt_pool_instance.get_allocator()._M_get_options();
	opt._M_align = 16; // FIXME 8-byte ptr alignment
	opt._M_chunk_size = 65536 - 4 * sizeof(void *);
	opt._M_max_bytes = opt._M_chunk_size;
	mt_pool_instance.get_allocator()._M_set_options(opt);

	GC_set_all_interior_pointers(true);
	GC_set_java_finalization(true);
	GC_INIT();
	GC_allow_register_threads();

	s_sources = {
		boost::container::pmr::new_delete_resource(),
		&bitmap_pool_instance, &gnu_pool_instance,
		&mt_pool_instance, &boost_pool_instance,
		&collectable_gc_instance, &atomic_gc_instance,
		&uncollectable_gc_instance, &atomic_uncollectable_gc_instance
	};
}

void *gc_memory_resource::do_allocate(size_t bytes, size_t /*alignment*/)
{
	switch (k) {
		case collectable: return GC_MALLOC(bytes);
		case atomic: return GC_MALLOC_ATOMIC(bytes);
		case uncollectable: return GC_MALLOC_UNCOLLECTABLE(bytes);
		case atomic_uncollectable: return GC_MALLOC_ATOMIC_UNCOLLECTABLE(bytes);
	}

	return nullptr;
}

void gc_memory_resource::do_deallocate(void *ptr, size_t /*bytes*/, size_t /*alignment*/)
{
	if(__builtin_expect(nullptr != GC_base(ptr), true))
		GC_FREE(ptr);
}

bool gc_memory_resource::do_is_equal(const memory::resource_type &other) const noexcept
{
	return (typeid(other) == typeid(gc_memory_resource) && this == &other);
}

gc_memory_resource::gc_memory_resource(gc_memory_resource::memory_kind k) : k(k)
{
}

/*static*/
void memory::register_finalizer(atomic_counted *obj)
{
	assert(obj);

	// finalizer must owns obj's counter
	const bool res = obj->weak_acquire();
	assert(res);

	void *const base = GC_base(obj);

	GC_register_finalizer_no_order(base, [](void *obj, void *offset) {
		atomic_counted *const ptr = reinterpret_cast<atomic_counted *>(
			reinterpret_cast<char *>(obj) + reinterpret_cast<std::ptrdiff_t>(offset));

		if(ptr->use_count())
			ptr->release();

		assert(1 == ptr->weak_count());
		GC_register_finalizer_unreachable(ptr, [](void *obj, void *) {
			reinterpret_cast<atomic_counted *>(obj)->weak_release();
		}, nullptr, nullptr, nullptr);

	}, reinterpret_cast<void *>(reinterpret_cast<char *>(obj)
		- reinterpret_cast<char *>(base)), nullptr, nullptr);
}

/*static*/
std::size_t memory::run_finalizers()
{
	return std::size_t(GC_invoke_finalizers());
}

/*static*/
bool memory::should_run_finalizers()
{
	return bool(GC_should_invoke_finalizers());
}

/*static*/
void memory::finalize_all()
{
	GC_finalize_all();
}

/*static*/
void memory::run_gc()
{
	GC_gcollect();
}

/*static*/
memory::collectable_memory_usage memory::get_collectable_memory_usage()
{
	collectable_memory_usage stat;
	GC_get_heap_usage_safe(&stat.heap_size, &stat.free_bytes,
		&stat.unmapped_bytes, &stat.bytes_since_last_gc, &stat.total_allocated_bytes);
	return stat;
}

/*static*/
std::size_t memory::get_collectable_heap_size()
{
	return GC_get_heap_size();
}

/*static*/
std::size_t memory::get_free_collectable_memory()
{
	return GC_get_free_bytes();
}

/*static*/
std::size_t memory::get_total_collectable_bytes()
{
	return GC_get_memory_use();
}

/*static*/
void memory::set_initial_collectable_heap_size(std::size_t size)
{
	const std::size_t current_size = GC_get_heap_size();
	if(size > current_size)
		GC_expand_hp(size - current_size);
}

/*static*/
void memory::set_max_collectable_heap_size(std::size_t size)
{
	GC_set_max_heap_size(size);
}

/*static*/
std::size_t memory::set_collectable_free_space_divisor(std::size_t div)
{
	const auto prev_divisor = GC_get_free_space_divisor();
	GC_set_free_space_divisor(div);
	return prev_divisor;
}

/*static*/
void memory::enable_gc()
{
	GC_enable();
}

/*static*/
void memory::disable_gc()
{
	GC_disable();
}

/*static*/
bool memory::gc_enabled()
{
	return !GC_is_disabled();
}

/*static*/
void *memory::get_base_address(void *obj)
{
	const auto res = GC_base(obj);
	return res ? res : obj;
}

/*static*/
bool memory::is_collectable(void *obj)
{
	return bool(GC_is_heap_ptr(obj));
}

/*static*/
std::size_t memory::get_collectable_size(void *obj)
{
	return GC_size(obj);
}

/*static*/
bool memory::attach_thread()
{
	GC_stack_base sb;
	GC_get_stack_base(&sb);
	return GC_DUPLICATE != GC_register_my_thread(&sb);
}

/*static*/
void memory::detach_thread()
{
	GC_unregister_my_thread();
}

/*static*/
memory::resource_type *memory::get_source(source_type t)
{
	std::call_once(s_flag, once_init);
	return s_sources.at(t);
}

bool memory::atomic_counted::acquire() noexcept
{
	auto count = use_count();
	do {
		if(count <= 0)
			return false;
	} while(refs.compare_exchange_weak(count, count + 1,
			std::memory_order_acq_rel, std::memory_order_relaxed));

	return true;
}

void memory::atomic_counted::release() noexcept
{
	if(1 == refs.fetch_sub(1, std::memory_order_relaxed))
	{
		dispose();

		// to ensure that the effects of dispose() are observed
		// in the thread that runs destroy().
		std::atomic_thread_fence(std::memory_order_acq_rel);

		if(1 == weak_refs.fetch_sub(1, std::memory_order_relaxed))
			destroy();
	}
}

bool memory::atomic_counted::weak_acquire() noexcept
{
	auto count = weak_count();
	do {
		if(count <= 0)
			return false;
	} while(weak_refs.compare_exchange_weak(count, count + 1,
			std::memory_order_acq_rel, std::memory_order_relaxed));

	return true;
}

void memory::atomic_counted::weak_release() noexcept
{
	if(1 == weak_refs.fetch_sub(1, std::memory_order_relaxed))
	{
		assert(0 >= use_count());

		// destroy() must observe the effects of dispose().
		std::atomic_thread_fence(std::memory_order_acq_rel);
		destroy();
	}
}

bool memory::atomic_counted::unique() const noexcept
{
	return 1 == refs.load(std::memory_order_relaxed);
}

std::int32_t memory::atomic_counted::use_count() const noexcept
{
	return refs.load(std::memory_order_relaxed);
}

std::int32_t memory::atomic_counted::weak_count() const noexcept
{
	return weak_refs.load(std::memory_order_relaxed);
}

void intrusive_ptr_add_ref(memory::atomic_counted *ac)
{
	const auto res = ac->acquire();
	assert(res);
}

void intrusive_ptr_release(memory::atomic_counted *ac)
{
	ac->release();
}

} // namespace emel
