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

#include <atomic>
#include <bits/allocated_ptr.h>
#include <boost/container/pmr/memory_resource.hpp>
#include <boost/container/pmr/polymorphic_allocator.hpp>
#include <boost/intrusive_ptr.hpp>

namespace emel EMEL_EXPORT { namespace memory EMEL_EXPORT {

template <typename Tp>
using rt_allocator = boost::container::pmr::polymorphic_allocator<Tp>;

class memory
{
public:
	static boost::container::pmr::memory_resource *get_source();
};

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

class atomic_counted
{
public:
	atomic_counted() noexcept : refs(1) { }
	virtual ~atomic_counted() noexcept = default;
	virtual atomic_counted *clone() const = 0;

  template <typename Tp>
  	Tp *get() const noexcept { return static_cast<Tp *>(raw_ptr()); }

	bool acquire() noexcept;
	void release() noexcept;
	bool unique() const noexcept;
	std::int32_t use_count() const noexcept;

private:
	virtual void *raw_ptr() const noexcept = 0;
	virtual void destroy() noexcept = 0;
	atomic_counted(const atomic_counted &) = delete;
	atomic_counted &operator=(const atomic_counted &) = delete;

	std::atomic_int_least32_t refs;
};

using counted_ptr = boost::intrusive_ptr<atomic_counted>;

namespace details {

template <typename Tp, typename Alloc>
class atomic_counted_inplace : public atomic_counted
{
	struct storage : details::ebo_helper<0UL, Alloc>
	{
		using base = details::ebo_helper<0UL, Alloc>;

		explicit storage(Alloc a) noexcept : base(a) { }

		Alloc &get_alloc() const noexcept {
			return base::get(const_cast<storage &>(*this));
		}

		typename std::aligned_storage<sizeof(Tp),
			std::alignment_of<Tp>::value>::type buffer;
	};

public:
	using alloc_type = rt_allocator<atomic_counted_inplace>;

  template <typename... Args>
	atomic_counted_inplace(Alloc a, Args &&...args);

	virtual ~atomic_counted_inplace() noexcept override = default;
	virtual atomic_counted *clone() const override;
	virtual void destroy() noexcept override;

private:
	virtual void *raw_ptr() const noexcept final override {
		return const_cast<void *>(reinterpret_cast<const void *>(&s.buffer));
	}

	storage s;
};

} // namespace details

template <typename Tp, typename Alloc, typename... Args>
atomic_counted *allocate_counted(Alloc a, Args &&...args)
{
	using ac_type = details::atomic_counted_inplace<Tp, Alloc>;
	typename ac_type::alloc_type a2(a);
	auto guard = std::__allocate_guarded(a2);
	ac_type *ptr = guard.get();
	new (ptr) ac_type(std::move(a), std::forward<Args>(args)...);
	guard = nullptr;
	return ptr;
}

template <typename Tp, typename... Args>
atomic_counted *make_counted(Args &&...args)
{
	return allocate_counted<Tp>(rt_allocator<Tp>(memory::get_source()), std::forward<Args>(args)...);
}

namespace details {

template <typename Tp, typename Alloc>
    template <typename... Args>
atomic_counted_inplace<Tp, Alloc>::
atomic_counted_inplace(Alloc a, Args &&...args) : s(a)
{
	std::allocator_traits<Alloc>::construct(a, get<Tp>(), std::forward<Args>(args)...);
}

template <typename Tp, typename Alloc>
atomic_counted *
atomic_counted_inplace<Tp, Alloc>::clone() const
{
	return allocate_counted<Tp>(s.get_alloc(), *get<Tp>());
}

template <typename Tp, typename Alloc>
void
atomic_counted_inplace<Tp, Alloc>::destroy() noexcept
{
	alloc_type a(s.get_alloc());
	std::allocator_traits<Alloc>::destroy(s.get_alloc(), get<Tp>());
	std::__allocated_ptr<alloc_type> guard { a, this };
	this->~atomic_counted_inplace();
}

} // namespace details

void intrusive_ptr_add_ref(atomic_counted *ac);
void intrusive_ptr_release(atomic_counted *ac);

} // namespace memory

} // namespace emel
