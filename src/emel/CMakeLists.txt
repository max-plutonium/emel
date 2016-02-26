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
set(VERSION 0.0.0)
cmake_minimum_required(VERSION 3.0)

add_definitions("-DEMEL_VERSION=${VERSION}")

IF(RELEASE)
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)
ENDIF()

find_package(Boost 1.60.0 REQUIRED system container thread filesystem locale)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

set(PUBLIC_HEADERS
    ast.h
    compiler/compiler.h
    memory/memory.h
    runtime/object.h
    parser.h
    plugins.h
)

set(HEADERS
    ast.h
    compiler/codegen.h
    compiler/compiler.h
    compiler/const-pool-manager.h
    compiler/symbol_table.h
    memory/memory.h
    runtime/interp.h
    runtime/object.h
    opcodes.h
    parser.h
    plugins.h
    semantic.h
    source-loader.h
    tokens.h
)

set(SOURCES
    ast.cc
    compiler/codegen.cc
    compiler/compiler.cc
    compiler/const-pool-manager.cc
    memory/memory.cc
    runtime/interp.cc
    runtime/object.cc
    opcodes.cc
    parser.cc
    plugins.cc
    semantic.cc
    source-loader.cc
    tokens.cc
)

add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION "${VERSION}" SOVERSION "0")
target_link_libraries(${PROJECT_NAME} stdc++ dl gcmt-dll ${Boost_LIBRARIES})

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "-debug")

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(FILES ${PUBLIC_HEADERS} DESTINATION emel)
