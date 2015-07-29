#
# Copyright (C) 2015 Max E. Kosykh
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
TEMPLATE = lib
QT =
TARGET = emel
DEFINES += EMEL_SHARED
VERSION = 0.0.0
QMAKE_CXXFLAGS += -std=gnu++14
QMAKE_CXXFLAGS_RELEASE += -Ofast -fvisibility=hidden -fvisibility-inlines-hidden
CONFIG -= qt

BOOST_PATH = $$absolute_path($$PWD)/../../boost_1_58_0
BOOST_LIB_PATH = $$BOOST_PATH/stage/lib
INCLUDEPATH += $$BOOST_PATH

### Building settings ###

DESTDIR = ../bin

CONFIG(debug, debug|release) {
    CONFIG += warn_on
    MOC_DIR = ../bin/debug
    OBJECTS_DIR = ../bin/debug

    CONFIG += separate_debug_info

    TARGET = $$join(TARGET,,, -debug)

} else: CONFIG(release, debug|release) {
    CONFIG += warn_off
    MOC_DIR = ../bin/release
    OBJECTS_DIR = ../bin/release

   g++*: QMAKE_POST_LINK += \
        $$QMAKE_OBJCOPY --strip-unneeded \
            $$DESTDIR/$(TARGET) $$escape_expand(\\n\\t)
}


### Files ###

HEADERS += \
    emel_global.h \
    ast.h \
    parser.h \
    tokens.h \
    spirit/lex.h \
    spirit/skipper.h \
    spirit/keywords.h \
    spirit/grammar.h \
    spirit/values.h \
    spirit/error_handler.h \
    spirit/expressions.h

SOURCES += \
    ast.cpp \
    parser.cpp \
    tokens.cpp \
    spirit/lex.cpp \
    spirit/skipper.cpp \
    spirit/keywords.cpp \
    spirit/grammar.cpp \
    spirit/values.cpp \
    spirit/error_handler.cpp \
    spirit/expressions.cpp
