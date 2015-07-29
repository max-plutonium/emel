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
    cpp.defines: ['EMEL_SHARED']
    cpp.objcopyPath: project.objcopyPath
    cpp.stripPath: project.stripPath

    Properties {
        condition: qbs.buildVariant == 'debug'
        cpp.warningLevel: 'all'
        cpp.separateDebugInformation: true
        targetName: name + '-debug'
    }

    Properties {
        condition: qbs.buildVariant == 'release'
        cpp.warningLevel: 'none'
        cpp.optimization: 'fast'
        cpp.visibility: 'minimal'
    }

    Group {
        name: 'public headers'
        qbs.install: true
        qbs.installDir: 'include'

        files: [
            'emel_global.h',
            'ast.h',
            'parser.h'
        ]
    }

    files: [
        'ast.cpp',
        'parser.cpp',
        'tokens.cpp',
        'tokens.h'
    ]

    Group {
        fileTagsFilter: product.type
        qbs.install: true
        qbs.installDir: 'lib'
    }

    Group {
        name: 'spirit frontend'
        files: [
            'spirit/error_handler.cpp',
            'spirit/error_handler.h',
            'spirit/expressions.cpp',
            'spirit/expressions.h',
            'spirit/grammar.cpp',
            'spirit/grammar.h',
            'spirit/keywords.cpp',
            'spirit/keywords.h',
            'spirit/lex.cpp',
            'spirit/lex.h',
            'spirit/skipper.cpp',
            'spirit/skipper.h',
            'spirit/values.cpp',
            'spirit/values.h'
        ]
    }
}
