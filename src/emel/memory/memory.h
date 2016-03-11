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
#pragma once

#include <atomic>
#include <bits/allocated_ptr.h>
#include <boost/container/pmr/memory_resource.hpp>
#include <boost/container/pmr/polymorphic_allocator.hpp>
#include <boost/intrusive_ptr.hpp>

namespace emel EMEL_EXPORT {

namespace details {

template<std::size_t Nm, typename Tp,
	bool = !std::is_final<Tp>::value && std::is_empty<Tp>::value>
struct ebo_helper;

/// Specialization using EBO.
template<std::size_t Nm, typename Tp>
struct ebo_helper<Nm, Tp, true> : private Tp
{
  template<typename... Args>
	explicit ebo_helper(Args &&...args) : Tp(std::forward<Args>(args)...) { }

	static Tp &get(ebo_helper &obj) noexcept { return static_cast<Tp &>(obj); }
};

/// Specialization not using EBO.
template<std::size_t Nm, typename Tp>
struct ebo_helper<Nm, Tp, false>
{
  template<typename... Args>
	explicit ebo_helper(Args &&...args) : tp(std::forward<Args>(args)...) { }

	static Tp &get(ebo_helper &obj) noexcept { return obj.tp; }

private:
	Tp tp;
};

} // namespace details

template <typename Tp>
using rt_allocator = boost::container::pmr::polymorphic_allocator<Tp>;

class memory
{
public:
	static std::size_t run_finalizers();
	static bool should_run_finalizers();
	static void finalize_all();
	static void run_gc();

	struct collectable_memory_usage {
		std::size_t heap_size;
		std::size_t free_bytes;
		std::size_t unmapped_bytes;
		std::size_t bytes_since_last_gc;
		std::size_t total_allocated_bytes;
	};

	static collectable_memory_usage get_collectable_memory_usage();
	static std::size_t get_collectable_heap_size();
	static std::size_t get_free_collectable_memory();
	static std::size_t get_total_collectable_bytes();

	static void set_initial_collectable_heap_size(std::size_t size);
	static void set_max_collectable_heap_size(std::size_t size);
	static std::size_t set_collectable_free_space_divisor(std::size_t div);

	static void enable_gc();
	static void disable_gc();
	static bool gc_enabled();

	static void *get_base_address(void *obj);
	static bool is_collectable(void *obj);
	static std::size_t get_collectable_size(void *obj);

	static bool attach_thread();
	static void detach_thread();

	enum source_type {
		bitmap_pool, gnu_pool, mt_pool, boost_pool,
		collectable_gc_pool, atomic_gc_pool, uncollectable_gc_pool,
		atomic_uncollectable_gc_pool, last_source_type
	};

	using resource_type = boost::container::pmr::memory_resource;
	static resource_type *get_source(source_type = bitmap_pool);

	class atomic_counted
	{
	public:
		atomic_counted() noexcept : refs(1), weak_refs(1) { }
		virtual ~atomic_counted() noexcept = default;
		virtual atomic_counted *clone() const = 0;

	  template <typename Tp>
		Tp *get() const noexcept { return static_cast<Tp *>(get_raw()); }

		bool acquire() noexcept;
		void release() noexcept;
		bool unique() const noexcept;
		std::int32_t use_count() const noexcept;
		std::int32_t weak_count() const noexcept;

	protected:
		bool weak_acquire() noexcept;
		void weak_release() noexcept;

	private:
		virtual void dispose() noexcept = 0;
		virtual void destroy() noexcept = 0;
		virtual void *get_raw() const noexcept = 0;
		atomic_counted(const atomic_counted &) = delete;
		atomic_counted &operator=(const atomic_counted &) = delete;
		friend class memory;

		std::atomic_int_least32_t refs, weak_refs;
	};

	using counted_ptr = boost::intrusive_ptr<atomic_counted>;

private:

  template <typename Tp, typename Alloc>
	class atomic_counted_inplace final : public atomic_counted
	{
		struct storage : details::ebo_helper<0, Alloc>
		{
			using base0 = details::ebo_helper<0, Alloc>;

			explicit storage(Alloc a) noexcept : base0(a) { }

			Alloc &get_alloc() const noexcept {
				return base0::get(const_cast<storage &>(*this));
			}

			typename std::aligned_storage<sizeof(Tp),
				std::alignment_of<Tp>::value>::type buffer;
		};

	public:
		using alloc_type = rt_allocator<atomic_counted_inplace>;

	  template <typename... Args>
		atomic_counted_inplace(Alloc a, Args &&...args);

	private:
		virtual ~atomic_counted_inplace() noexcept override;
		virtual atomic_counted *clone() const override;
		virtual void dispose() noexcept override;
		virtual void destroy() noexcept override;
		virtual void *get_raw() const noexcept override;

		storage s;
	};

	static void register_finalizer(atomic_counted *obj);

public:
	struct use_type_info { };

  template <typename Tp, typename Alloc, typename... Args>
	static atomic_counted *
	allocate_counted(Alloc a, Args &&...args);

  template <typename Tp, typename... Args>
	static atomic_counted *
	make_counted(Args &&...args);

  template <typename Tp, typename... Args>
	static atomic_counted *
	make_collectable(Args &&...args);

}; // class memory

template <typename Tp, typename Alloc>
    template <typename... Args>
memory::atomic_counted_inplace<Tp, Alloc>::
atomic_counted_inplace(Alloc a, Args &&...args) : s(a)
{
	std::allocator_traits<Alloc>::construct(a,
		reinterpret_cast<Tp *>(&s.buffer), std::forward<Args>(args)...);
}

template <typename Tp, typename Alloc>
memory::atomic_counted_inplace<Tp, Alloc>::
~atomic_counted_inplace() noexcept
{
	assert(0 >= use_count());
	assert(0 == weak_count());
}

template <typename Tp, typename Alloc>
memory::atomic_counted *
memory::atomic_counted_inplace<Tp, Alloc>::clone() const
{
	assert(use_count());
	assert(weak_count());

	const auto &alloc = s.get_alloc();
	auto *const ptr = allocate_counted<Tp>(alloc, *reinterpret_cast<const Tp *>(&s.buffer));

	if(rt_allocator<Tp>(get_source(collectable_gc_pool)) == alloc)
		register_finalizer(ptr);
	return ptr;
}

template <typename Tp, typename Alloc>
void
memory::atomic_counted_inplace<Tp, Alloc>::dispose() noexcept
{
	std::allocator_traits<Alloc>::destroy(s.get_alloc(),
		const_cast<Tp *>(reinterpret_cast<const Tp *>(&s.buffer)));
}

template <typename Tp, typename Alloc>
void
memory::atomic_counted_inplace<Tp, Alloc>::destroy() noexcept
{
	alloc_type a(s.get_alloc());
	std::__allocated_ptr<alloc_type> guard { a, this };
	this->~atomic_counted_inplace();
}

template <typename Tp, typename Alloc>
void *
memory::atomic_counted_inplace<Tp, Alloc>::get_raw() const noexcept
{
	if(use_count() > 0)
		return const_cast<void *>(reinterpret_cast<const void *>(&s.buffer));
	return nullptr;
}

template <typename Tp, typename Alloc, typename... Args>
/*static*/ memory::atomic_counted *
memory::allocate_counted(Alloc a, Args &&...args)
{
	using ac_type = atomic_counted_inplace<Tp, Alloc>;
	typename ac_type::alloc_type a2(a);
	auto guard = std::__allocate_guarded(a2);
	ac_type *const ptr = guard.get();
	new (ptr) ac_type(std::move(a), std::forward<Args>(args)...);
	guard = nullptr;
	return ptr;
}

template <typename Tp, typename... Args>
/*static*/ memory::atomic_counted *
memory::make_counted(Args &&...args)
{
	return allocate_counted<Tp>(rt_allocator<Tp>(get_source()),
		std::forward<Args>(args)...);
}

template <typename Tp, typename... Args>
/*static*/ memory::atomic_counted *
memory::make_collectable(Args &&...args)
{
	auto *const ptr = allocate_counted<Tp>(
		rt_allocator<Tp>(get_source(collectable_gc_pool)),
			std::forward<Args>(args)...);

	register_finalizer(ptr);
	return ptr;
}

EMEL_EXPORT void intrusive_ptr_add_ref(memory::atomic_counted *ac);
EMEL_EXPORT void intrusive_ptr_release(memory::atomic_counted *ac);

template <typename Tp>
struct memory_ptr : public memory::counted_ptr
{
	using memory::counted_ptr::counted_ptr;

	Tp &operator *() const {
		return *memory::counted_ptr::operator*().template get<Tp>();
	}

	Tp *operator ->() const {
		return memory::counted_ptr::operator->()->template get<Tp>();
	}
};

} // namespace emel
