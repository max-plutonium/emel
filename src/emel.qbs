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
    cpp.cxxStandardLibrary: 'libstdc++'
    cpp.dynamicLibraries: [
        'dl', 'boost_system', 'boost_filesystem',
        'boost_locale'
    ]
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
            'emel/parser.h',
            'emel/plugins.h'
        ]
    }

    files: [
        'emel/ast.cpp',
        'emel/bytecode/bytecode.cpp',
        'emel/bytecode/bytecode.h',
        'emel/bytecode/const_pool_manager.cpp',
        'emel/bytecode/const_pool_manager.h',
        'emel/bytecode/insns_manager.cpp',
        'emel/bytecode/insns_manager.h',
        'emel/opcodes.cpp',
        'emel/opcodes.h',
        'emel/parser.cpp',
        'emel/plugins.cpp',
        'emel/semantic.cpp',
        'emel/semantic.h',
        'emel/source_loader.cpp',
        'emel/source_loader.h',
        'emel/symbol_table.h',
        'emel/tokens.cpp',
        'emel/tokens.h'
    ]
}
