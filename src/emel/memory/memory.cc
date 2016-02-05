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
#include "memory.h"

#include <mutex>
#include <boost/container/pmr/resource_adaptor.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <ext/pool_allocator.h>
#include <ext/bitmap_allocator.h>
#include <ext/mt_allocator.h>

namespace emel { namespace memory {

template <typename Tp>
using boost_pool_allocator = boost::fast_pool_allocator<Tp,
		boost::default_user_allocator_new_delete, std::mutex, 1024>;

template <typename Tp>
using gnu_pool_allocator = __gnu_cxx::__pool_alloc<Tp>;

template <typename Tp>
using bitmap_allocator = __gnu_cxx::bitmap_allocator<Tp>;

template <typename Tp>
using mt_allocator = __gnu_cxx::__mt_alloc<Tp>;

static std::once_flag s_flag;

static void once_init(mt_allocator<char> &a)
{
	auto opt = a._M_get_options();
	opt._M_align = 16;
	opt._M_chunk_size = 8192 - 4 * sizeof(void *);
	opt._M_max_bytes = opt._M_chunk_size;
	a._M_set_options(opt);
}

/*static*/ boost::container::pmr::memory_resource *memory::get_source()
{
	static boost::container::pmr::resource_adaptor<mt_allocator<char>> instance;
	std::call_once(s_flag, once_init, std::ref(instance.get_allocator()));
	return &instance;
}

bool atomic_counted::acquire() noexcept
{
	auto count = use_count();
	do {
		if(count <= 0)
			return false;
	} while(refs.compare_exchange_weak(count, count + 1,
			std::memory_order_acq_rel, std::memory_order_relaxed));

	return true;
}

void atomic_counted::release() noexcept
{
	if(1 == refs.fetch_sub(1, std::memory_order_release))
		destroy();
}

bool atomic_counted::unique() const noexcept
{
	return 1 == refs.load(std::memory_order_relaxed);
}

std::int32_t atomic_counted::use_count() const noexcept
{
	return refs.load(std::memory_order_relaxed);
}

void intrusive_ptr_add_ref(atomic_counted *ac)
{
	const auto res = ac->acquire();
	assert(res);
}

void intrusive_ptr_release(atomic_counted *ac)
{
	ac->release();
}

} // namespace memory

} // namespace emel
