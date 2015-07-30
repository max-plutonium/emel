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
    name: 'emel-frontend-spirit'
    version: '0.0.0'
    Depends { name: 'cpp' }
    Depends { name: 'emel' }

    cpp.cxxLanguageVersion: 'c++14'
    cpp.systemIncludePaths: [project.boostPath]
    cpp.defines: [
        'EMEL_EXPORT=__attribute__((visibility ("default")))',
        'EMEL_FRONTEND_VERSION="' + version + '"'
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

    files: [
        'frontend-spirit/spirit_parser.cpp',
        'frontend-spirit/spirit_parser.h',
        'frontend-spirit/error_handler.cpp',
        'frontend-spirit/error_handler.h',
        'frontend-spirit/expressions.cpp',
        'frontend-spirit/expressions.h',
        'frontend-spirit/grammar.cpp',
        'frontend-spirit/grammar.h',
        'frontend-spirit/keywords.cpp',
        'frontend-spirit/keywords.h',
        'frontend-spirit/lex.cpp',
        'frontend-spirit/lex.h',
        'frontend-spirit/skipper.cpp',
        'frontend-spirit/skipper.h',
        'frontend-spirit/values.cpp',
        'frontend-spirit/values.h',
    ]
}
