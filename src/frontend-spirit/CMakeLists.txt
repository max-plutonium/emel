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
project(emel-frontend-spirit)
set(VERSION 0.0.0)
cmake_minimum_required(VERSION 3.0)

add_definitions("-DEMEL_FRONTEND_VERSION=\"${VERSION}\"")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
    -fvisibility=hidden -fvisibility-inlines-hidden")

find_package(Boost 1.59.0 REQUIRED system)

set(HEADERS
    error-handler.h
    expressions.h
    grammar.h
    keywords.h
    lex.h
    position-handler.h
    skipper.h
    spirit-parser.h
    values.h
)

set(SOURCES
    error-handler.cc
    expressions.cc
    grammar.cc
    keywords.cc
    lex.cc
    position-handler.cc
    skipper.cc
    spirit-parser.cc
    values.cc
)

add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION "${VERSION}" SOVERSION "0")
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "-debug")

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
