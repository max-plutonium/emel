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
#include <boost/type_index.hpp>

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
	virtual void dispose() noexcept = 0;

  template <typename Tp>
  	Tp *get() const noexcept { return static_cast<Tp *>(
		get_raw(boost::typeindex::type_id<Tp>()));
  	}

	bool acquire() noexcept;
	void release() noexcept;
	bool unique() const noexcept;
	std::int32_t use_count() const noexcept;

private:
	virtual void destroy() noexcept = 0;
	virtual void *get_raw(boost::typeindex::type_index) const noexcept = 0;
	atomic_counted(const atomic_counted &) = delete;
	atomic_counted &operator=(const atomic_counted &) = delete;

	std::atomic_int_least32_t refs;
};

using counted_ptr = boost::intrusive_ptr<atomic_counted>;

namespace details {

struct empty_type_info { };

template <typename Tp, typename Alloc, typename TypeInfo = empty_type_info>
class atomic_counted_inplace final : public atomic_counted
{
	struct storage : details::ebo_helper<0, Alloc>
			, details::ebo_helper<1, TypeInfo>
	{
		using base0 = details::ebo_helper<0, Alloc>;
		using base1 = details::ebo_helper<1, TypeInfo>;

		explicit storage(Alloc a, TypeInfo t) noexcept : base0(a), base1(t) { }

		Alloc &get_alloc() const noexcept {
			return base0::get(const_cast<storage &>(*this));
		}

		TypeInfo &get_info() const noexcept {
			return base1::get(const_cast<storage &>(*this));
		}

		typename std::aligned_storage<sizeof(Tp),
			std::alignment_of<Tp>::value>::type buffer;
	};

public:
	using alloc_type = rt_allocator<atomic_counted_inplace>;

  template <typename... Args>
	atomic_counted_inplace(Alloc a, TypeInfo info, Args &&...args);

private:
	virtual ~atomic_counted_inplace() noexcept override;
	virtual atomic_counted *clone() const override;
	virtual void dispose() noexcept override;
	virtual void destroy() noexcept override;
	virtual void *get_raw(boost::typeindex::type_index ti) const noexcept override;

	std::atomic_bool disposed;
	storage s;
};

} // namespace details

struct use_type_info { };

template <typename Tp, typename Alloc, typename TypeInfo, typename... Args>
atomic_counted *allocate_counted(use_type_info, Alloc a, TypeInfo info, Args &&...args)
{
	using ac_type = details::atomic_counted_inplace<Tp, Alloc, TypeInfo>;
	typename ac_type::alloc_type a2(a);
	auto guard = std::__allocate_guarded(a2);
	ac_type *ptr = guard.get();
	new (ptr) ac_type(std::move(a), std::move(info), std::forward<Args>(args)...);
	guard = nullptr;
	return ptr;
}

template <typename Tp, typename TypeInfo, typename... Args>
atomic_counted *make_counted(use_type_info, TypeInfo info, Args &&...args)
{
	return allocate_counted<Tp>(rt_allocator<Tp>(memory::get_source()),
		std::move(info), std::forward<Args>(args)...);
}

template <typename Tp, typename Alloc, typename... Args>
atomic_counted *allocate_counted(Alloc a, Args &&...args)
{
	return allocate_counted<Tp>(use_type_info(), std::move(a),
		details::empty_type_info(), std::forward<Args>(args)...);
}

template <typename Tp, typename... Args>
atomic_counted *make_counted(Args &&...args)
{
	return allocate_counted<Tp>(rt_allocator<Tp>(memory::get_source()),
		std::forward<Args>(args)...);
}

namespace details {

template <typename Tp, typename Alloc, typename TypeInfo>
    template <typename... Args>
atomic_counted_inplace<Tp, Alloc, TypeInfo>::
atomic_counted_inplace(Alloc a, TypeInfo info, Args &&...args) : disposed(false), s(a, std::move(info))
{
	std::allocator_traits<Alloc>::construct(a,
		const_cast<Tp *>(reinterpret_cast<const Tp *>(&s.buffer)),
			std::forward<Args>(args)...);
}

template <typename Tp, typename Alloc, typename TypeInfo>
atomic_counted_inplace<Tp, Alloc, TypeInfo>::
~atomic_counted_inplace() noexcept
{
	dispose();
}

template <typename Tp, typename Alloc, typename TypeInfo>
atomic_counted *
atomic_counted_inplace<Tp, Alloc, TypeInfo>::clone() const
{
	return allocate_counted<Tp>(use_type_info(), s.get_alloc(), s.get_info(),
		*reinterpret_cast<const Tp *>(&s.buffer));
}

template <typename Tp, typename Alloc, typename TypeInfo>
void
atomic_counted_inplace<Tp, Alloc, TypeInfo>::dispose() noexcept
{
	if(!disposed.exchange(true, std::memory_order_acq_rel))
		std::allocator_traits<Alloc>::destroy(s.get_alloc(),
			const_cast<Tp *>(reinterpret_cast<const Tp *>(&s.buffer)));
}

template <typename Tp, typename Alloc, typename TypeInfo>
void
atomic_counted_inplace<Tp, Alloc, TypeInfo>::destroy() noexcept
{
	alloc_type a(s.get_alloc());
	std::__allocated_ptr<alloc_type> guard { a, this };
	this->~atomic_counted_inplace();
}

template <typename Tp, typename Alloc, typename TypeInfo>
void *
atomic_counted_inplace<Tp, Alloc, TypeInfo>::
get_raw(boost::typeindex::type_index ti) const noexcept
{
	if(ti.equal(boost::typeindex::type_id<Tp>()))
		if(!disposed.load(std::memory_order_relaxed))
			return const_cast<void *>(reinterpret_cast<const void *>(&s.buffer));
		else
			return nullptr;
	else if(ti.equal(boost::typeindex::type_id<TypeInfo>()))
		return reinterpret_cast<void *>(std::addressof(s.get_info()));
	else if(ti.equal(boost::typeindex::type_id<Alloc>()))
		return reinterpret_cast<void *>(std::addressof(s.get_alloc()));
	else
		return nullptr;
}

} // namespace details

void intrusive_ptr_add_ref(atomic_counted *ac);
void intrusive_ptr_release(atomic_counted *ac);

} // namespace memory

} // namespace emel
