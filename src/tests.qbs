/*
 * Copyright (C) 2015 Max Plutonium <plutonium.max@gmail.com>
 *
 * This file is part of the test suite of the EMEL library.
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
import qbs

CppApplication {
    name : 'emel-tests'
    Depends { name: 'emel' }

    property path gmockPath: '../../gmock-1.7.0/fused-src'

    cpp.cxxLanguageVersion: 'c++14'
    cpp.systemIncludePaths: [project.boostPath, gmockPath]
    cpp.libraryPaths: [project.boostLibsPath]
    cpp.cxxStandardLibrary: 'libstdc++'
    cpp.dynamicLibraries: ['pthread']
    cpp.defines: ['EMEL_EXPORT=__attribute__((visibility ("default")))']
    cpp.objcopyPath: project.objcopyPath
    cpp.stripPath: project.stripPath

    Properties {
        condition: qbs.buildVariant == 'debug'
        cpp.warningLevel: 'all'
        targetName: name + '-debug'
    }

    Properties {
        condition: qbs.buildVariant == 'release'
        cpp.warningLevel: 'none'
        cpp.optimization: 'fast'
    }

    Group {
        fileTagsFilter: product.type
        qbs.install: true
        qbs.installDir: 'bin'
    }

    files: [
        'tests/main.cpp',
        'tests/test_opcodes.cpp',
        'tests/test_parser.cpp',
        gmockPath + '/gmock-gtest-all.cc'
    ]
}
