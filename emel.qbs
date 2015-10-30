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

Project {
    references: [
        'src/emel.qbs',
        'src/frontend-spirit.qbs',
        'src/tests.qbs'
    ]

    property path boostPath: '../../boost_1_59_0'
    readonly property path boostLibsPath: boostPath + '/stage/lib'
    readonly property path boostDLLPath: '../src/boost-dll/include'
    property path objcopyPath: '/usr/bin/objcopy'
    property path stripPath: '/usr/bin/strip'
}
