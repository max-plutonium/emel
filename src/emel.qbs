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
import qbs

DynamicLibrary {
    name: 'emel'
    version: '0.0.0'
    Depends { name: 'cpp' }

    cpp.cxxLanguageVersion: 'c++14'
    cpp.systemIncludePaths: [project.boostPath]
    cpp.libraryPaths: [project.boostLibsPath]
    cpp.dynamicLibraries: ['boost_system', 'boost_filesystem', 'dl']
    cpp.defines: [
        'EMEL_EXPORT=__attribute__((visibility ("default")))',
        'EMEL_VERSION="' + version + '"'
    ]
    cpp.objcopyPath: project.objcopyPath
    cpp.stripPath: project.stripPath

    Properties {
        condition: qbs.buildVariant == 'debug'
        cpp.warningLevel: 'all'
        //cpp.separateDebugInformation: true
        targetName: name + '-debug'
    }

    Properties {
        condition: qbs.buildVariant == 'release'
        cpp.warningLevel: 'none'
        cpp.optimization: 'fast'
        cpp.visibility: 'minimal'
    }

    Group {
        fileTagsFilter: product.type
        qbs.install: true
        qbs.installDir: 'lib'
    }

    Group {
        name: 'public headers'
        qbs.install: true
        qbs.installDir: 'emel'

        files: [
            'emel/ast.h',
            'emel/opcodes.h',
            'emel/parser.h',
            'emel/plugins.h'
        ]
    }

    files: [
        'emel/opcodes.cpp',
        'emel/ast.cpp',
        'emel/parser.cpp',
        'emel/plugins.cpp',
        'emel/tokens.cpp',
        'emel/tokens.h'
    ]
}
