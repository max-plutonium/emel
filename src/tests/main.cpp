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
#include <cstdio>
#include <gmock/gmock.h>

int main(int argc, const char **argv)
{
    testing::InitGoogleMock(&argc, const_cast<char **>(argv));

    std::string test_src_dir(TESTS_SRC_DIR);

    std::string pwd(argv[0]);
    const auto pos = pwd.rfind('/');
    assert(std::string::npos != pos);
    pwd = pwd.substr(0, pos);

    std::printf("emel gtest\nTEST_DIR = %s\nPWD = %s\n",
                test_src_dir.c_str(), pwd.c_str());

    setenv("EMEL_HOME", test_src_dir.append("/../../test-data").c_str(), 1);
    setenv("LD_LIBRARY_PATH", pwd.append("/../frontend-spirit").c_str(), 1);

    return RUN_ALL_TESTS();
}
