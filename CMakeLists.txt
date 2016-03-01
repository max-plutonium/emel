#
# Copyright (C) 2015, 2016 Max Plutonium <plutonium.max@gmail.com>
#
# This file is part of the EMEL library.
#
# The EMEL library is free software: you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public License
# as published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# The EMEL library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with the EMEL library. If not, see
# <http://www.gnu.org/licenses/>.
#
project(emel)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
add_definitions("-DEMEL_EXPORT=__attribute__((visibility (\"default\")))")
add_definitions("-DGC_NAMESPACE -DGC_THREADS")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wstrict-aliasing -fstack-check -DGC_DEBUG -DDEBUG_THREADS")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto -march=native")

include_directories(SYSTEM src/deps/bdwgc/include)
include_directories(SYSTEM src/deps/boost-dll/include)
include_directories(SYSTEM src/deps/google-benchmark/include)
include_directories(SYSTEM src/deps/google-sparsehash/src)
include_directories(SYSTEM src/deps/google-test/googlemock/include)
include_directories(SYSTEM src/deps/google-test/googletest/include)

set(enable_threads YES)
set(enable_cplusplus YES)
set(enable_parallel_mark YES)
add_subdirectory(src/deps/bdwgc)

add_subdirectory(src/deps/google-benchmark)
add_subdirectory(src/deps/google-test/googlemock)

add_subdirectory(src/emel)
add_subdirectory(src/frontend-spirit)
add_subdirectory(src/tests)
