#
# Copyright (C) 2015, 2016 Max Plutonium <plutonium.max@gmail.com>
#
# This file is part of the test suite of the EMEL library.
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
project(emel-tests)
cmake_minimum_required(VERSION 3.0)

add_definitions("-DTESTS_SRC_DIR=\"${emel-tests_SOURCE_DIR}\"")

include_directories(SYSTEM ../googletest/googlemock/include)
include_directories(SYSTEM ../googletest/googletest/include)

find_package(Boost 1.59.0 REQUIRED)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

set(SOURCES
    main.cc
    test-compiler.cc
    test-interp.cc
    test-object.cc
    test-memory.cc
    test-opcodes.cc
    test-parser.cc
    test-string.cc
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} emel gmock)
add_dependencies(${PROJECT_NAME} emel-frontend-spirit)

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "-debug")

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
