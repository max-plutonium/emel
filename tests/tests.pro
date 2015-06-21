#
# Copyright (C) 2015 Max E. Kosykh
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
TEMPLATE = app
QT =
TARGET = emel-tests
CONFIG += console
CONFIG -= app_bundle
QMAKE_CXXFLAGS += -std=gnu++14
QMAKE_CXXFLAGS_RELEASE += -Ofast
CONFIG -= qt

BOOST_PATH = $$absolute_path($$PWD)/../../boost_1_57_0
BOOST_LIB_PATH = $$BOOST_PATH/stage/lib
INCLUDEPATH += $$BOOST_PATH

GTEST_PATH = $$absolute_path($$PWD)/../../gmock-1.7.0/fused-src
INCLUDEPATH += $$GTEST_PATH

LIBS += -lpthread -lgomp -lgccjit

BUILDROOT = ../bin

CONFIG(debug, debug|release) {
    CONFIG += warn_on
    BUILDSUFFIX = debug
    LIBS += -L$$BUILDROOT
    unix: LIBS += -lemel-debug
    else: win32: LIBS += -lemel-debug0

    TARGET = $$join(TARGET,,, -debug)

} else: CONFIG(release, debug|release) {
    CONFIG += warn_off
    BUILDSUFFIX = release
    LIBS += -L$$BUILDROOT
    unix: LIBS += -lemel
    else: win32: LIBS += -lemel0
}


DESTDIR = $$BUILDROOT
MOC_DIR = $$BUILDROOT/$$BUILDSUFFIX/tests
OBJECTS_DIR = $$BUILDROOT/$$BUILDSUFFIX/tests

SOURCES += $$GTEST_PATH/gmock-gtest-all.cc \
    main.cpp \
    test_parser.cpp

include(deployment.pri)
qtcAddDeployment()
