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

#include <experimental/memory_resource>

#include <mutex>
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

template <typename Tp>
using rt_allocator = mt_allocator<Tp>;

class pool
{
public:
	static std::experimental::pmr::memory_resource *get_source();
};

} // namespace memory

} // namespace emel
