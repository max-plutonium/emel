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
#include <gmock/gmock.h>

#include "../emel/parser.h"

using namespace emel;

// TODO нужно тестировать обработку ошибок
// TODO test for id parsing

struct mock_parser : public parser {
    MOCK_CONST_METHOD4(parse, bool(source_iter,
        source_iter, const std::string &, ast::node &));
};

using testing::_;
using testing::Eq;
using testing::EndsWith;
using testing::Return;
using testing::Ref;
using testing::IsEmpty;
using testing::SizeIs;

TEST(Parser, ParseEmptyString)
{
    const std::string str;
    mock_parser prsr;
    ast::node ret;

    EXPECT_CALL(prsr, parse(_, _, _, _)).Times(0);
    EXPECT_FALSE(prsr.parse_string(str, ret));
}

TEST(Parser, ParseDir)
{
    mock_parser prsr;
    std::vector<ast::node> ret;

    EXPECT_CALL(prsr, parse(_, _, EndsWith(".emel"), _)).WillRepeatedly(Return(true));
    ret = prsr.parse_dir("../../../../emel/test-data/");
    EXPECT_FALSE(ret.empty());
}

TEST(Parser, InvokeParserViaParseString)
{
    const std::string str = " ";
    mock_parser prsr;
    ast::node ret;

    EXPECT_CALL(prsr, parse(Eq(str.cbegin()), Eq(str.cend()),
                            Eq("fake.emel"), Ref(ret))).WillOnce(Return(true));
    EXPECT_TRUE(prsr.parse_string(str, ret));
    EXPECT_EQ(0, ret.which());
    EXPECT_EQ(typeid(empty_value_type), ret.type());
}

TEST(Parser, ParseEmptyClass)
{
    const std::vector<std::string> str_vec {
        " class name end class ",
        " CLASS name END CLASS ",
        " Class name ENDCLASS",
        " CLAss name EndClass ",
        " class\n\tname endclass ",
        " class\n\tname \n\n\t\tendclass \n\n\t\t",
        "\n\n\t\tclass name \n\n\t\tendclass\n\n\t\t",
        "class name \n\n\t\t end class ",
        "class name \n\n\t\t end\n\n\t\t class\n\n\t\t ",
    };

    auto prsr = parser::instance();
    ast::node ret;

    for(auto &str : str_vec)
    {
        EXPECT_TRUE(prsr->parse_string(str, ret));
        ASSERT_EQ(typeid(ast::class_), ret.type());

        ast::class_ c = boost::get<ast::class_>(ret);
        EXPECT_EQ("name", c.name);
        EXPECT_THAT(c.base_name, IsEmpty());
        EXPECT_THAT(c.exprs, IsEmpty());
        EXPECT_THAT(c.methods, IsEmpty());
    }
}

TEST(Parser, ParseEmptyClassWithBaseClass)
{
    const std::vector<std::string> str_vec {
        " class name : base end class ",
        " class \n\n\t\tname : base end class ",
        " class \n\n\t\tname \n\n\t\t: base end class \n\n\t\t",
        "\n\n\t\t class\n\n\t\t name :\n\n\t\t base end class ",
        " class \n\n\t\tname \n\n\t\t:\n\n\t\t base\n\n\t\t end class\n\n\t\t ",
        " class \n\n\t\tname \n\n\t\t:\n\n\t\t base \n\n\t\tend\n\n\t\t class ",
        "\n\n\t\t class\n\n\t\t name : base \n\n\t\tend \n\n\t\tclass ",
        "\n\n\t\t class name : \n\n\t\tbase \n\n\t\tend\n\n\t\t class \n\n\t\t",
        " class name \n\n\t\t: base end \n\n\t\tclass ",
        " \n\n\t\tclass name :\n\n\t\t base\n\n\t\t end class \n\n\t\t",
    };

    auto prsr = parser::instance();
    ast::node ret;

    for(auto &str : str_vec)
    {
        EXPECT_TRUE(prsr->parse_string(str, ret));
        ASSERT_EQ(typeid(ast::class_), ret.type());

        ast::class_ c = boost::get<ast::class_>(ret);
        EXPECT_EQ("name", c.name);
        EXPECT_EQ("base", c.base_name);
        EXPECT_THAT(c.exprs, IsEmpty());
        EXPECT_THAT(c.methods, IsEmpty());
    }
}

TEST(Parser, ParseStrings)
{
    const std::string str =
        "class name : base "
        " \"hello\"; \"wo\\\"rl\\\"d\";"
        "end class";

    auto prsr = parser::instance();
    ast::node ret;

    EXPECT_TRUE(prsr->parse_string(str, ret));
    ASSERT_EQ(typeid(ast::class_), ret.type());

    ast::class_ c = boost::get<ast::class_>(ret);
    EXPECT_EQ("name", c.name);
    EXPECT_EQ("base", c.base_name);
    EXPECT_THAT(c.exprs, SizeIs(2));
    EXPECT_THAT(c.methods, IsEmpty());

    ASSERT_EQ(typeid(std::string), c.exprs.at(0).type());
    EXPECT_EQ("hello", boost::get<std::string>(c.exprs.at(0)));
    ASSERT_EQ(typeid(std::string), c.exprs.at(1).type());
    EXPECT_EQ("wo\"rl\"d", boost::get<std::string>(c.exprs.at(1)));
}

TEST(Parser, ParseNumbers)
{
    const std::vector<std::string> str_vec {
        "class name : base "
        " 1; 1.2; 10.; 1000.02; 0.456; 0x1000;"
        "end class ",
        "class name : base "
        " \n\n\t\t1\n\n\t\t; \n\n\t\t1.2\n\n\t\t;\n\n\t\t 10.\n\n\t\t;"
        "\n\n\t\t1000.02\n\n\t\t;\n\n\t\t0.456\n\n\t\t; \n\n0x1000\n\n\t\t;"
        "end class ",
    };

    auto prsr = parser::instance();
    ast::node ret;

    for(auto &str : str_vec)
    {
        EXPECT_TRUE(prsr->parse_string(str, ret));
        ASSERT_EQ(typeid(ast::class_), ret.type());

        ast::class_ c = boost::get<ast::class_>(ret);
        EXPECT_EQ("name", c.name);
        EXPECT_EQ("base", c.base_name);
        EXPECT_THAT(c.exprs, SizeIs(6));
        EXPECT_THAT(c.methods, IsEmpty());

        ASSERT_EQ(typeid(double), c.exprs.at(0).type());
        EXPECT_EQ(1, boost::get<double>(c.exprs.at(0)));
        ASSERT_EQ(typeid(double), c.exprs.at(1).type());
        EXPECT_EQ(1.2, boost::get<double>(c.exprs.at(1)));
        ASSERT_EQ(typeid(double), c.exprs.at(2).type());
        EXPECT_EQ(10, boost::get<double>(c.exprs.at(2)));
        ASSERT_EQ(typeid(double), c.exprs.at(3).type());
        EXPECT_EQ(1000.02, boost::get<double>(c.exprs.at(3)));
        ASSERT_EQ(typeid(double), c.exprs.at(4).type());
        EXPECT_EQ(0.456, boost::get<double>(c.exprs.at(4)));
        ASSERT_EQ(typeid(double), c.exprs.at(5).type());
        EXPECT_EQ(4096, boost::get<double>(c.exprs.at(5)));
    }
}

TEST(Parser, ParseBooleans)
{
    const std::string str =
        "class name : base true; false; TRUE; FALSE; True; False;end class";

    auto prsr = parser::instance();
    ast::node ret;

    EXPECT_TRUE(prsr->parse_string(str, ret));
    ASSERT_EQ(typeid(ast::class_), ret.type());

    ast::class_ c = boost::get<ast::class_>(ret);
    EXPECT_EQ("name", c.name);
    EXPECT_EQ("base", c.base_name);
    EXPECT_THAT(c.exprs, SizeIs(6));
    EXPECT_THAT(c.methods, IsEmpty());

    ASSERT_EQ(typeid(bool), c.exprs.at(0).type());
    EXPECT_TRUE(boost::get<bool>(c.exprs.at(0)));
    ASSERT_EQ(typeid(bool), c.exprs.at(1).type());
    EXPECT_FALSE(boost::get<bool>(c.exprs.at(1)));
    ASSERT_EQ(typeid(bool), c.exprs.at(2).type());
    EXPECT_TRUE(boost::get<bool>(c.exprs.at(2)));
    ASSERT_EQ(typeid(bool), c.exprs.at(3).type());
    EXPECT_FALSE(boost::get<bool>(c.exprs.at(3)));
    ASSERT_EQ(typeid(bool), c.exprs.at(4).type());
    EXPECT_TRUE(boost::get<bool>(c.exprs.at(4)));
    ASSERT_EQ(typeid(bool), c.exprs.at(5).type());
    EXPECT_FALSE(boost::get<bool>(c.exprs.at(5)));
}

TEST(Parser, ParseAssignExpr)
{
    const std::vector<std::string> str_vec {
        "class name : base a = 1; b \n\n\t\tasExternal= -1; endclass",
        "class name : base \n\n\t\ta byval = 1; b AsExternal=\n\n\t\t-1; endclass",
        "class name : base a\n\n\t\t = 1\n\n\t\t;b\n\n\t\t ASEXTERNAL = -1; endclass",
        "class name : base \n\n\t\ta = \n\n\t\t1\n\n\t\t;\n\n\t\tb asexternal=\n\n\t\t-1; endclass",
        "class name : base \n\n\t\ta \n\n\t\tbyval= \n\n\t\t1\n\n\t\t;b \n\n\t\tasExternal\n\n\t\t =-1\n\n\t\t; endclass",
    };

    auto prsr = parser::instance();
    ast::node ret;

    for(auto &str : str_vec)
    {
        EXPECT_TRUE(prsr->parse_string(str, ret));
        ASSERT_EQ(typeid(ast::class_), ret.type());

        ast::class_ c = boost::get<ast::class_>(ret);
        EXPECT_EQ("name", c.name);
        EXPECT_EQ("base", c.base_name);
        EXPECT_THAT(c.exprs, SizeIs(2));
        EXPECT_THAT(c.methods, IsEmpty());

        {
            ASSERT_EQ(typeid(ast::assign), c.exprs.at(0).type());
            ast::assign asgn = boost::get<ast::assign>(c.exprs.at(0));
            EXPECT_EQ("a", asgn.var_name);
            EXPECT_FALSE(asgn.as_external);
            ASSERT_EQ(typeid(double), asgn.rhs.type());
            EXPECT_EQ(1, boost::get<double>(asgn.rhs));
        }

        {
            ASSERT_EQ(typeid(ast::assign), c.exprs.at(1).type());
            ast::assign asgn = boost::get<ast::assign>(c.exprs.at(1));
            EXPECT_EQ("b", asgn.var_name);
            EXPECT_TRUE(asgn.as_external);
            ASSERT_EQ(typeid(double), asgn.rhs.type());
            EXPECT_EQ(-1, boost::get<double>(asgn.rhs));
        }
    }
}

TEST(Parser, ParseUnaryOpOnVariable)
{
    const std::vector<std::string> str_vec {
        "class name : base -a; ~b; -refof c; ~valof d; endclass",
        "class name : base \n\n\t\t-a; ~\n\n\t\tb; \n\n\t\t-refof c; ~valof \n\n\t\td; endclass",
        "class name : base -a; \n\n\t\t~\n\n\t\tb;\n\n\t\t -\n\n\t\trefof\n\n\t\t "
        "c\n\n\t\t; ~valof d\n\n\t\t;\n\n\t\t endclass",
        "class name : base \n\n\t\t-a; \n\n\t\t~b; -\n\n\t\trefof c; \n\n\t\t~valof \n\n\t\td; endclass",
        "class name : base \n\n\t\t-\n\n\t\ta\n\n\t\t; \n\n\t\t~\n\n\t\tb\n\n\t\t;"
        "\n\n\t\t -\n\n\t\trefof \n\n\t\tc\n\n\t\t; ~\n\n\t\tvalof d\n\n\t\t; endclass",
        "class name : base -a; ~b; -REFOF c; ~VALOF d; endclass",
        "class name : base -a; ~b; -Refof c; ~Valof d; endclass",
        "class name : base -a; ~b; - refof c; ~ valof d; endclass",
        "class name : base  \n- \na \n; \n ~ \nb \n; \n - \nrefof \n c \n; \n ~ \nvalof \n d \n; \n endclass",
    };

    auto prsr = parser::instance();
    ast::node ret;

    for(auto &str : str_vec)
    {
        EXPECT_TRUE(prsr->parse_string(str, ret));
        ASSERT_EQ(typeid(ast::class_), ret.type());

        ast::class_ c = boost::get<ast::class_>(ret);
        EXPECT_EQ("name", c.name);
        EXPECT_EQ("base", c.base_name);
        EXPECT_THAT(c.exprs, SizeIs(4));
        EXPECT_THAT(c.methods, IsEmpty());

        {
            ASSERT_EQ(typeid(ast::un_op), c.exprs.at(0).type());
            ast::un_op op = boost::get<ast::un_op>(c.exprs.at(0));
            EXPECT_EQ(op_kind::neg, op.k);
            ASSERT_EQ(typeid(ast::variable), op.rhs.type());
            ast::variable var = boost::get<ast::variable>(op.rhs);
            EXPECT_EQ("a", var.name);
            EXPECT_FALSE(var.ref_of);
            EXPECT_FALSE(var.val_of);
        }

        {
            ASSERT_EQ(typeid(ast::un_op), c.exprs.at(1).type());
            ast::un_op op = boost::get<ast::un_op>(c.exprs.at(1));
            EXPECT_EQ(op_kind::not_, op.k);
            ASSERT_EQ(typeid(ast::variable), op.rhs.type());
            ast::variable var2 = boost::get<ast::variable>(op.rhs);
            EXPECT_EQ("b", var2.name);
            EXPECT_FALSE(var2.ref_of);
            EXPECT_FALSE(var2.val_of);
        }

        {
            ASSERT_EQ(typeid(ast::un_op), c.exprs.at(2).type());
            ast::un_op op = boost::get<ast::un_op>(c.exprs.at(2));
            EXPECT_EQ(op_kind::neg, op.k);
            ASSERT_EQ(typeid(ast::variable), op.rhs.type());
            ast::variable var3 = boost::get<ast::variable>(op.rhs);
            EXPECT_EQ("c", var3.name);
            EXPECT_TRUE(var3.ref_of);
            EXPECT_FALSE(var3.val_of);
        }

        {
            ASSERT_EQ(typeid(ast::un_op), c.exprs.at(3).type());
            ast::un_op op = boost::get<ast::un_op>(c.exprs.at(3));
            EXPECT_EQ(op_kind::not_, op.k);
            ASSERT_EQ(typeid(ast::variable), op.rhs.type());
            ast::variable var4 = boost::get<ast::variable>(op.rhs);
            EXPECT_EQ("d", var4.name);
            EXPECT_FALSE(var4.ref_of);
            EXPECT_TRUE(var4.val_of);
        }
    }
}

TEST(Parser, ParseBinaryOp)
{
    const std::vector<std::string> str_vec {
        "class name : base "
        "1+1; 2-2; 3*3; 4/4; 5<=5; 6>=6; 7<7; 8>8; 9==9; 10!=10;"
        "11&11; 12^12; 13|13; endclass",
        "class name : base "
        "\n\n\t\t1\n\n\t\t+\n\n\t\t1\n\n\t\t;\n\n\t\t"
        "2\n\n\t\t-2\n\n\t\t; 3\n\n\t\t*3\n\n\t\t; 4\n\n\t\t / 4\n\n\t\t;"
        " 5 <= \n\n\t\t5 ; 6\n\n\t\t >=\n\n\t\t 6; 7 \n\n\t\t< \n\n\t\t7; "
        "8\n\n\t\t > 8; 9 \n\n\t\t== \n\n\t\t9; 10 \n\n\t\t!= \n\n\t\t10\n\n\t\t;\n\n\t\t"
        "11\n\n\t\t & \n\n\t\t11; 12\n\n\t\t^ \n\n\t\t12; 13 | 13; endclass",
    };

    const std::vector<op_kind> op_kind_vec {
        op_kind::add, op_kind::sub,
        op_kind::mul, op_kind::div,
        op_kind::lte, op_kind::gte,
        op_kind::lt, op_kind::gt,
        op_kind::eq, op_kind::ne,
        op_kind::and_, op_kind::xor_, op_kind::or_
    };

    auto prsr = parser::instance();
    ast::node ret;

    for(auto &str : str_vec)
    {
        EXPECT_TRUE(prsr->parse_string(str, ret));
        ASSERT_EQ(typeid(ast::class_), ret.type());

        ast::class_ c = boost::get<ast::class_>(ret);
        EXPECT_EQ("name", c.name);
        EXPECT_EQ("base", c.base_name);
        EXPECT_THAT(c.exprs, SizeIs(13));
        EXPECT_THAT(c.methods, IsEmpty());

        for(std::size_t i = 0; i < c.exprs.size(); ++i)
        {
            ASSERT_EQ(typeid(ast::bin_op), c.exprs.at(i).type());
            ast::bin_op op = boost::get<ast::bin_op>(c.exprs.at(i));

            EXPECT_EQ(op_kind_vec.at(i), op.k);

            ASSERT_EQ(typeid(double), op.lhs.type());
            ASSERT_EQ(typeid(double), op.rhs.type());
            EXPECT_EQ(i + 1, boost::get<double>(op.lhs));
            EXPECT_EQ(i + 1, boost::get<double>(op.rhs));
        }

    }
}

TEST(Parser, ParseTernaryOp)
{
    const std::vector<std::string> str_vec {
        "class name : base 1 ? 2 : 3; "
        "1 ? 2 ? 3 : 4 : 5; 1 ? 2 : 3 ? 4 : 5; "
        "endclass",
        "class name : base 1\n\n\t\t ? 2\n\n\t\t : 3\n\n\t\t; "
        "1 ? \n\n\t\t2 \n\n\t\t?\n\n\t\t 3\n\n\t\t : 4\n\n\t\t : 5;"
        "1\n\n\t\t ?\n\n\t\t 2\n\n\t\t : 3\n\n\t\t ? \n\n\t\t4 :\n\n\t\t 5\n\n\t\t; "
        "endclass",
        "class name : base \n\n\t\t1 \n\n\t\t? 2\n\n\t\t : \n\n\t\t3\n\n\t\t; "
        "1\n\n\t\t ? \n\n\t\t2\n\n\t\t ?\n\n\t\t 3\n\n\t\t :\n\n\t\t 4 :"
        "\n\n\t\t5; 1 \n\n\t\t? \n\n\t\t2\n\n\t\t : \n\n\t\t3 \n\n\t\t? "
        "\n\n\t\t4 \n\n\t\t: \n\n\t\t5\n\n\t\t; "
        "endclass",
        "class name : base \n\n\t\t1\n\n\t\t ? \n\n\t\t2 \n\n\t\t:\n\n\t\t"
        "3\n\n\t\t;\n\n\t\t "
        "\n\n\t\t1 \n\n\t\t? \n\n\t\t2 \n\n\t\t?\n\n\t\t 3"
        "\n\n\t\t:\n\n\t\t 4\n\n\t\t : \n\n\t\t5\n\n\t\t;\n\n\t\t"
        "\n\n\t\t 1 \n\n\t\t? \n\n\t\t2\n\n\t\t :\n\n\t\t"
        "3\n\n\t\t ? \n\n\t\t4\n\n\t\t : \n\n\t\t5\n\n\t\t;\n\n\t\t "
        "endclass",
        "class name : base 1?2:3; "
        "1? 2?3:4:5;1?2:3?4:5; "
        "endclass",
    };

    auto prsr = parser::instance();
    ast::node ret;

    for(auto &str : str_vec)
    {
        EXPECT_TRUE(prsr->parse_string(str, ret));
        ASSERT_EQ(typeid(ast::class_), ret.type());

        ast::class_ c = boost::get<ast::class_>(ret);
        EXPECT_EQ("name", c.name);
        EXPECT_EQ("base", c.base_name);
        EXPECT_THAT(c.exprs, SizeIs(3));
        EXPECT_THAT(c.methods, IsEmpty());

        {
            ASSERT_EQ(typeid(ast::ternary), c.exprs.at(0).type());
            ast::ternary op = boost::get<ast::ternary>(c.exprs.at(0));
            ASSERT_EQ(typeid(double), op.cond.type());
            EXPECT_EQ(1, boost::get<double>(op.cond));
            ASSERT_EQ(typeid(double), op.first.type());
            EXPECT_EQ(2, boost::get<double>(op.first));
            ASSERT_EQ(typeid(double), op.second.type());
            EXPECT_EQ(3, boost::get<double>(op.second));
        }

        {
            ASSERT_EQ(typeid(ast::ternary), c.exprs.at(1).type());
            ast::ternary op = boost::get<ast::ternary>(c.exprs.at(1));
            ASSERT_EQ(typeid(double), op.cond.type());
            EXPECT_EQ(1, boost::get<double>(op.cond));
            ASSERT_EQ(typeid(ast::ternary), op.first.type());
            ast::ternary op1 = boost::get<ast::ternary>(op.first);
            ASSERT_EQ(typeid(double), op1.cond.type());
            EXPECT_EQ(2, boost::get<double>(op1.cond));
            ASSERT_EQ(typeid(double), op1.cond.type());
            EXPECT_EQ(3, boost::get<double>(op1.first));
            ASSERT_EQ(typeid(double), op1.second.type());
            EXPECT_EQ(4, boost::get<double>(op1.second));
            ASSERT_EQ(typeid(double), op.second.type());
            EXPECT_EQ(5, boost::get<double>(op.second));
        }

        {
            ASSERT_EQ(typeid(ast::ternary), c.exprs.at(2).type());
            ast::ternary op = boost::get<ast::ternary>(c.exprs.at(2));
            ASSERT_EQ(typeid(ast::ternary), op.cond.type());
            ast::ternary op1 = boost::get<ast::ternary>(op.cond);

            ASSERT_EQ(typeid(double), op1.cond.type());
            EXPECT_EQ(1, boost::get<double>(op1.cond));
            ASSERT_EQ(typeid(double), op1.first.type());
            EXPECT_EQ(2, boost::get<double>(op1.first));
            ASSERT_EQ(typeid(double), op1.second.type());
            EXPECT_EQ(3, boost::get<double>(op1.second));

            ASSERT_EQ(typeid(double), op.first.type());
            EXPECT_EQ(4, boost::get<double>(op.first));
            ASSERT_EQ(typeid(double), op.second.type());
            EXPECT_EQ(5, boost::get<double>(op.second));
        }
    }
}

#include <stack>

TEST(Parser, OpPrecedence)
{
    const std::string str =
        "class name : base "
        "1+2--3*~4/5<=6>=7<8>9==10!=11&12^13|14;"
        "1^2|3&4 >5== 6!=7 -8<=9>=10<11+-12*~13/14;"
        "1==2 != 3& 4^ 5 |6 +7 -8 <=0>= 10<11> -12* ~13 /14;"
        "(1&2^3!=(5<6>7==8))|(9-(10+11)<=12>=13/(-14*~15));"
        "endclass";

    std::vector<op_kind> op_kind_vec {
        op_kind::neg, op_kind::not_,
        op_kind::mul, op_kind::div,
        op_kind::add, op_kind::sub,
        op_kind::lte, op_kind::gte,
        op_kind::lt, op_kind::gt,
        op_kind::eq, op_kind::ne,
        op_kind::and_, op_kind::xor_, op_kind::or_
    };

    auto prsr = parser::instance();
    ast::node ret;

    EXPECT_TRUE(prsr->parse_string(str, ret));
    ASSERT_EQ(typeid(ast::class_), ret.type());
    ast::class_ c = boost::get<ast::class_>(ret);

    for(ast::node &expr_node : c.exprs)
    {
        ASSERT_EQ(typeid(ast::bin_op), expr_node.type());
        ast::bin_op op = boost::get<ast::bin_op>(expr_node);

        std::stack<op_kind, decltype(op_kind_vec)>
                op_kind_stack(op_kind_vec);

        std::function<void (const ast::node &)> dfs;

        dfs = [&dfs, &op_kind_stack](const ast::node &n)
        {
            ASSERT_EQ(typeid(ast::bin_op), n.type());
            ast::bin_op op = boost::get<ast::bin_op>(n);

            EXPECT_EQ(op_kind_stack.top(), op.k);

            if(typeid(ast::bin_op) == op.lhs.type()) {
                op_kind_stack.pop();
                dfs(op.lhs);
            }

            if(typeid(ast::bin_op) == op.rhs.type()) {
                op_kind_stack.pop();
                dfs(op.rhs);
            }
        };

        dfs(op);
    }
}

TEST(Parser, ParseCall)
{
    const std::vector<std::string> str_vec {
        "class name "
        "call(); call(1,2,3);"
        "base::call(1, 2);"
        "a.call(1,2);"
        "call().call2();"
        "call(1,2,3).call2(4, 5, 6);"
        "base::call(1, 2).call2(3,4);"
        "a.call(1,2).call2(3,4);"
        "endclass",
        "class name "
        "\n\tcall(\n\t)\n\t;\n\t call\n\t(\n\t1,\n\t2,\n\t3\n\t)\n\t;\n\t"
        "base\n\t::\n\tcall(\n\t1, \n\t2\n\t)\n\t;\n\t"
        "a\n\t.\n\tcall\n\t(\n\t1\n\t,2\n\t);"
        "call\n\t(\n\t)\n\t.\n\tcall2\n\t(\n\t);"
        "call(\n\t1,2,3)\n\t.call2\n\t(4,\n\t 5,\n\t 6);"
        "base\n\t::call\n\t(\n\t1,\n\t 2).\n\tcall2(\n\t3,4\n\t);"
        "\n\ta\n\t.\n\tcall(1,2)\n\t.call2(3\n\t,4\n\t);"
        "endclass",
    };

    auto prsr = parser::instance();
    ast::node ret;

    for(auto &str : str_vec)
    {
        EXPECT_TRUE(prsr->parse_string(str, ret));
        ASSERT_EQ(typeid(ast::class_), ret.type());

        ast::class_ c = boost::get<ast::class_>(ret);
        EXPECT_THAT(c.exprs, SizeIs(8));
        EXPECT_THAT(c.methods, IsEmpty());

        // call();
        {
            ASSERT_EQ(typeid(ast::call), c.exprs.at(0).type());
            ast::call op = boost::get<ast::call>(c.exprs.at(0));
            EXPECT_THAT(op.names, SizeIs(1));
            EXPECT_THAT(op.args, IsEmpty());
            EXPECT_EQ(0, op.chain_call.which());
            ASSERT_EQ(typeid(std::string), op.names.at(0).type());
            EXPECT_EQ("call", boost::get<std::string>(op.names.at(0)));
        }

        // call(1, 2, 3);
        {
            ASSERT_EQ(typeid(ast::call), c.exprs.at(1).type());
            ast::call op = boost::get<ast::call>(c.exprs.at(1));
            EXPECT_THAT(op.names, SizeIs(1));
            EXPECT_THAT(op.args, SizeIs(3));
            EXPECT_EQ(0, op.chain_call.which());

            ASSERT_EQ(typeid(std::string), op.names.at(0).type());
            EXPECT_EQ("call", boost::get<std::string>(op.names.at(0)));

            for(std::size_t i = 0; i < op.args.size(); ++i) {
                ASSERT_EQ(typeid(double), op.args.at(i).type());
                EXPECT_EQ(i + 1, boost::get<double>(op.args.at(i)));
            }
        }

        // base::call(1, 2);
        {
            ASSERT_EQ(typeid(ast::call), c.exprs.at(2).type());
            ast::call op = boost::get<ast::call>(c.exprs.at(2));
            EXPECT_THAT(op.names, SizeIs(2));
            EXPECT_THAT(op.args, SizeIs(2));
            EXPECT_EQ(0, op.chain_call.which());

            ASSERT_EQ(typeid(std::string), op.names.at(0).type());
            EXPECT_EQ("base", boost::get<std::string>(op.names.at(0)));
            ASSERT_EQ(typeid(std::string), op.names.at(1).type());
            EXPECT_EQ("call", boost::get<std::string>(op.names.at(1)));

            for(std::size_t i = 0; i < op.args.size(); ++i) {
                ASSERT_EQ(typeid(double), op.args.at(i).type());
                EXPECT_EQ(i + 1, boost::get<double>(op.args.at(i)));
            }
        }

        // a.call(1, 2);
        {
            ASSERT_EQ(typeid(ast::call), c.exprs.at(3).type());
            ast::call op = boost::get<ast::call>(c.exprs.at(3));
            EXPECT_THAT(op.names, SizeIs(2));
            EXPECT_THAT(op.args, SizeIs(2));
            EXPECT_EQ(0, op.chain_call.which());

            ASSERT_EQ(typeid(std::string), op.names.at(0).type());
            EXPECT_EQ("a", boost::get<std::string>(op.names.at(0)));
            ASSERT_EQ(typeid(std::string), op.names.at(1).type());
            EXPECT_EQ("call", boost::get<std::string>(op.names.at(1)));

            for(std::size_t i = 0; i < op.args.size(); ++i) {
                ASSERT_EQ(typeid(double), op.args.at(i).type());
                EXPECT_EQ(i + 1, boost::get<double>(op.args.at(i)));
            }
        }

        // call().call2();
        {
            ASSERT_EQ(typeid(ast::call), c.exprs.at(4).type());
            ast::call op = boost::get<ast::call>(c.exprs.at(4));
            EXPECT_THAT(op.names, SizeIs(1));
            EXPECT_THAT(op.args, IsEmpty());

            ASSERT_EQ(typeid(std::string), op.names.at(0).type());
            EXPECT_EQ("call", boost::get<std::string>(op.names.at(0)));

            ASSERT_EQ(typeid(ast::call), op.chain_call.type());
            ast::call op1 = boost::get<ast::call>(op.chain_call);
            EXPECT_THAT(op1.names, SizeIs(1));
            EXPECT_THAT(op1.args, IsEmpty());

            ASSERT_EQ(typeid(std::string), op1.names.at(0).type());
            EXPECT_EQ("call2", boost::get<std::string>(op1.names.at(0)));
        }

        // call(1, 2, 3).call2(4, 5, 6);
        {
            ASSERT_EQ(typeid(ast::call), c.exprs.at(5).type());
            ast::call op = boost::get<ast::call>(c.exprs.at(5));
            EXPECT_THAT(op.names, SizeIs(1));
            EXPECT_THAT(op.args, SizeIs(3));

            ASSERT_EQ(typeid(std::string), op.names.at(0).type());
            EXPECT_EQ("call", boost::get<std::string>(op.names.at(0)));

            for(std::size_t i = 0; i < op.args.size(); ++i) {
                ASSERT_EQ(typeid(double), op.args.at(i).type());
                EXPECT_EQ(i + 1, boost::get<double>(op.args.at(i)));
            }

            ASSERT_EQ(typeid(ast::call), op.chain_call.type());
            ast::call op1 = boost::get<ast::call>(op.chain_call);
            EXPECT_THAT(op1.names, SizeIs(1));
            EXPECT_THAT(op1.args, SizeIs(3));

            ASSERT_EQ(typeid(std::string), op1.names.at(0).type());
            EXPECT_EQ("call2", boost::get<std::string>(op1.names.at(0)));

            for(std::size_t i = 0; i < op1.args.size(); ++i) {
                ASSERT_EQ(typeid(double), op1.args.at(i).type());
                EXPECT_EQ(i + 4, boost::get<double>(op1.args.at(i)));
            }
        }

        // base::call(1, 2).call2(3, 4);
        {
            ASSERT_EQ(typeid(ast::call), c.exprs.at(6).type());
            ast::call op = boost::get<ast::call>(c.exprs.at(6));
            EXPECT_THAT(op.names, SizeIs(2));
            EXPECT_THAT(op.args, SizeIs(2));

            ASSERT_EQ(typeid(std::string), op.names.at(0).type());
            EXPECT_EQ("base", boost::get<std::string>(op.names.at(0)));
            ASSERT_EQ(typeid(std::string), op.names.at(1).type());
            EXPECT_EQ("call", boost::get<std::string>(op.names.at(1)));

            for(std::size_t i = 1; i < op.args.size(); ++i) {
                ASSERT_EQ(typeid(double), op.args.at(i - 1).type());
                EXPECT_EQ(i, boost::get<double>(op.args.at(i - 1)));
            }

            ASSERT_EQ(typeid(ast::call), op.chain_call.type());
            ast::call op1 = boost::get<ast::call>(op.chain_call);
            EXPECT_THAT(op1.names, SizeIs(1));
            EXPECT_THAT(op1.args, SizeIs(2));

            ASSERT_EQ(typeid(std::string), op1.names.at(0).type());
            EXPECT_EQ("call2", boost::get<std::string>(op1.names.at(0)));

            for(std::size_t i = 0; i < op1.args.size(); ++i) {
                ASSERT_EQ(typeid(double), op1.args.at(i).type());
                EXPECT_EQ(i + 3, boost::get<double>(op1.args.at(i)));
            }
        }

        // a.call(1, 2).call2(3, 4);
        {
            ASSERT_EQ(typeid(ast::call), c.exprs.at(7).type());
            ast::call op = boost::get<ast::call>(c.exprs.at(7));
            EXPECT_THAT(op.names, SizeIs(2));
            EXPECT_THAT(op.args, SizeIs(2));

            ASSERT_EQ(typeid(std::string), op.names.at(0).type());
            EXPECT_EQ("a", boost::get<std::string>(op.names.at(0)));
            ASSERT_EQ(typeid(std::string), op.names.at(1).type());
            EXPECT_EQ("call", boost::get<std::string>(op.names.at(1)));

            for(std::size_t i = 0; i < op.args.size(); ++i) {
                ASSERT_EQ(typeid(double), op.args.at(i).type());
                EXPECT_EQ(i + 1, boost::get<double>(op.args.at(i)));
            }

            ASSERT_EQ(typeid(ast::call), op.chain_call.type());
            ast::call op1 = boost::get<ast::call>(op.chain_call);
            EXPECT_THAT(op1.names, SizeIs(1));
            EXPECT_THAT(op1.args, SizeIs(2));

            ASSERT_EQ(typeid(std::string), op1.names.at(0).type());
            EXPECT_EQ("call2", boost::get<std::string>(op1.names.at(0)));

            for(std::size_t i = 0; i < op1.args.size(); ++i) {
                ASSERT_EQ(typeid(double), op1.args.at(i).type());
                EXPECT_EQ(i + 3, boost::get<double>(op1.args.at(i)));
            }
        }
    }
}

TEST(Parser, ParseTryBlock)
{
    const std::vector<std::string> str_vec {
        "class name "
        "try(abc) catch try(abc) 1+2; catch "
        "endclass",
        "class name "
        "try\n\t(\n\tabc\n\t) \n\tcatch\n\t try\n\t(\n\tabc)\n\t 1+2\n\t;\n\t catch\n\t "
        "endclass",
        "class name "
        "TRY\n\t(\n\tabc\n\t) \n\tCATCH\n\t Try\n\t(\n\tabc)\n\t 1+2\n\t;\n\t Catch\n\t "
        "endclass",
    };

    auto prsr = parser::instance();
    ast::node ret;

    for(auto &str : str_vec)
    {
        EXPECT_TRUE(prsr->parse_string(str, ret));
        ASSERT_EQ(typeid(ast::class_), ret.type());

        ast::class_ c = boost::get<ast::class_>(ret);
        EXPECT_EQ("name", c.name);
        EXPECT_THAT(c.exprs, SizeIs(2));
        EXPECT_THAT(c.methods, IsEmpty());

        {
            ASSERT_EQ(typeid(ast::try_), c.exprs.at(0).type());
            ast::try_ try_ = boost::get<ast::try_>(c.exprs.at(0));
            EXPECT_EQ("abc", try_.var_name);
            EXPECT_THAT(try_.exprs, IsEmpty());
        }

        {
            ASSERT_EQ(typeid(ast::try_), c.exprs.at(1).type());
            ast::try_ try_ = boost::get<ast::try_>(c.exprs.at(1));
            EXPECT_EQ("abc", try_.var_name);
            EXPECT_THAT(try_.exprs, SizeIs(1));
            EXPECT_EQ(typeid(ast::bin_op), try_.exprs.at(0).type());
        }
    }
}

TEST(Parser, ParseSwitchBlock)
{
    const std::vector<std::string> str_vec {
        "class name "
        "switch(2+3) endswitch "
        "switch(2+3) "
        "case 1: 1+2;"
        "case 1: case 2: case 3: 1+2;"
        "case \"123\n123\\\"123\\\"123\": 1+2;"
        "case true: case false: 1+2;"
        "default: 1+2;"
        "endswitch "
        "endclass",
        "class name "
        "SWITCH\n\t(\n\t2+\n\t3\n\t) \n\tEND \n\tSWITCH\n\t "
        "SWiTCh\n\t(2+3) \n\t"
        "CASE\n\t 1\n\t: \n\t1+2\n\t;\n\t"
        "\n\tCase \n\t1\n\t:\n\t CASE\n\t 2:\n\t CaSE 3\n\t:\n\t 1+2;"
        "caSE\n\t \"123\n123\\\"123\\\"123\"\n\t: \n\t1+2;"
        "Case\n\t TRUE: \n\tCasE \n\tFalse\n\t: 1+2;"
        "\n\tDEFAULT\n\t: \n\t1+2;"
        "ENDSWITCH\n\t "
        "endclass",
        "class name "
        "Switch(2+3)\n\t EndSwitch "
        "Switch\n\t(2+3) \n\t"
        "Case \n\t1: 1+\n\t2;\n\t"
        "Case\n\t 1:\n\t Case \n\t2:\n\t Case \n\t3\n\t: 1+2;"
        "Case\n\t \"123\n123\\\"123\\\"123\"\n\t:\n\t 1+2;"
        "Case \n\ttrue\n\t:\n\t Case \n\tfalse\n\t:\n\t 1\n\t+2\n\t;"
        "Default:\n\t 1+2;"
        "\n\tEnd \n\t\n\tSwitch\n\t "
        "endclass",
    };

    auto prsr = parser::instance();
    ast::node ret;

    for(auto &str : str_vec)
    {
        EXPECT_TRUE(prsr->parse_string(str, ret));
        ASSERT_EQ(typeid(ast::class_), ret.type());

        ast::class_ c = boost::get<ast::class_>(ret);
        EXPECT_EQ("name", c.name);
        EXPECT_THAT(c.exprs, SizeIs(2));
        EXPECT_THAT(c.methods, IsEmpty());

        {
            ASSERT_EQ(typeid(ast::switch_), c.exprs.at(0).type());
            ast::switch_ switch_ = boost::get<ast::switch_>(c.exprs.at(0));
            ASSERT_EQ(typeid(ast::bin_op), switch_.cond.type());
            EXPECT_THAT(switch_.blocks, IsEmpty());
        }

        {
            ASSERT_EQ(typeid(ast::switch_), c.exprs.at(1).type());
            ast::switch_ switch_ = boost::get<ast::switch_>(c.exprs.at(1));
            ASSERT_EQ(typeid(ast::bin_op), switch_.cond.type());
            EXPECT_THAT(switch_.blocks, SizeIs(5));

            // case 1: 1+2;
            {
                ASSERT_EQ(typeid(ast::case_), switch_.blocks.at(0).type());
                ast::case_ case_ = boost::get<ast::case_>(switch_.blocks.at(0));
                EXPECT_THAT(case_.match_values, SizeIs(1));
                EXPECT_THAT(case_.exprs, SizeIs(1));
                ASSERT_EQ(typeid(double), case_.match_values.at(0).type());
                EXPECT_EQ(1, boost::get<double>(case_.match_values.at(0)));
                ASSERT_EQ(typeid(ast::bin_op), case_.exprs.at(0).type());
            }

            // case 1: case 2: case 3: 1+2;
            {
                ASSERT_EQ(typeid(ast::case_), switch_.blocks.at(1).type());
                ast::case_ case_ = boost::get<ast::case_>(switch_.blocks.at(1));
                EXPECT_THAT(case_.match_values, SizeIs(3));
                EXPECT_THAT(case_.exprs, SizeIs(1));
                ASSERT_EQ(typeid(double), case_.match_values.at(0).type());
                EXPECT_EQ(1, boost::get<double>(case_.match_values.at(0)));
                ASSERT_EQ(typeid(double), case_.match_values.at(1).type());
                EXPECT_EQ(2, boost::get<double>(case_.match_values.at(1)));
                ASSERT_EQ(typeid(double), case_.match_values.at(2).type());
                EXPECT_EQ(3, boost::get<double>(case_.match_values.at(2)));
                ASSERT_EQ(typeid(ast::bin_op), case_.exprs.at(0).type());
            }

            // case \"123\n123\\\"123\\\"123\": 1+2;
            {
                ASSERT_EQ(typeid(ast::case_), switch_.blocks.at(2).type());
                ast::case_ case_ = boost::get<ast::case_>(switch_.blocks.at(2));
                EXPECT_THAT(case_.match_values, SizeIs(1));
                EXPECT_THAT(case_.exprs, SizeIs(1));
                ASSERT_EQ(typeid(std::string), case_.match_values.at(0).type());
                EXPECT_EQ("123\n123\"123\"123", boost::get<std::string>(case_.match_values.at(0)));
                ASSERT_EQ(typeid(ast::bin_op), case_.exprs.at(0).type());
            }

            // case true: case false: 1+2;
            {
                ASSERT_EQ(typeid(ast::case_), switch_.blocks.at(3).type());
                ast::case_ case_ = boost::get<ast::case_>(switch_.blocks.at(3));
                EXPECT_THAT(case_.match_values, SizeIs(2));
                EXPECT_THAT(case_.exprs, SizeIs(1));
                ASSERT_EQ(typeid(bool), case_.match_values.at(0).type());
                EXPECT_TRUE(boost::get<bool>(case_.match_values.at(0)));
                ASSERT_EQ(typeid(bool), case_.match_values.at(1).type());
                EXPECT_FALSE(boost::get<bool>(case_.match_values.at(1)));
                ASSERT_EQ(typeid(ast::bin_op), case_.exprs.at(0).type());
            }

            // default: 1+2;
            {
                ASSERT_EQ(typeid(ast::case_), switch_.blocks.at(4).type());
                ast::case_ case_ = boost::get<ast::case_>(switch_.blocks.at(4));
                EXPECT_THAT(case_.match_values, IsEmpty());
                EXPECT_THAT(case_.exprs, SizeIs(1));
                ASSERT_EQ(typeid(ast::bin_op), case_.exprs.at(0).type());
            }
        }
    }
}

TEST(Parser, ParseIfBlock)
{
    const std::vector<std::string> str_vec {
        "class name "
        "if(1+2) endif if(1+2) 1+2; endif "
        "if(1+2) 1+2; else 1+2; endif "
        "if(1+2) 1+2; else if(1+2) 1+2; endif endif "
        "endclass",
        "class name "
        "IF\n\t(1+2\n\t)\n\t ENDIF \n\tIF\n\t(1+2) \n\t1+2; END \n\tIF\n\t "
        "If\n\t(1+2) \n\t1+2;\n\t ELSE\n\t 1+2; End\n\t If\n\t "
        "\n\tiF(1+2) 1+2;\n\t Else\n\t If\n\t(1+2)\n\t 1+2; \n\tEndIf\n\t end \n\tif\n\t "
        "endclass",
    };

    auto prsr = parser::instance();
    ast::node ret;

    for(auto &str : str_vec)
    {
        EXPECT_TRUE(prsr->parse_string(str, ret));
        ASSERT_EQ(typeid(ast::class_), ret.type());

        ast::class_ c = boost::get<ast::class_>(ret);
        EXPECT_EQ("name", c.name);
        EXPECT_THAT(c.exprs, SizeIs(4));
        EXPECT_THAT(c.methods, IsEmpty());

        {
            ASSERT_EQ(typeid(ast::if_), c.exprs.at(0).type());
            ast::if_ if_ = boost::get<ast::if_>(c.exprs.at(0));
            EXPECT_EQ(typeid(ast::bin_op), if_.cond.type());
            EXPECT_THAT(if_.then_exprs, IsEmpty());
            EXPECT_THAT(if_.else_exprs, IsEmpty());
        }

        {
            ASSERT_EQ(typeid(ast::if_), c.exprs.at(1).type());
            ast::if_ if_ = boost::get<ast::if_>(c.exprs.at(1));
            EXPECT_EQ(typeid(ast::bin_op), if_.cond.type());
            EXPECT_THAT(if_.then_exprs, SizeIs(1));
            EXPECT_EQ(typeid(ast::bin_op), if_.then_exprs.at(0).type());
            EXPECT_THAT(if_.else_exprs, IsEmpty());
        }

        {
            ASSERT_EQ(typeid(ast::if_), c.exprs.at(2).type());
            ast::if_ if_ = boost::get<ast::if_>(c.exprs.at(2));
            EXPECT_EQ(typeid(ast::bin_op), if_.cond.type());
            EXPECT_THAT(if_.then_exprs, SizeIs(1));
            EXPECT_EQ(typeid(ast::bin_op), if_.then_exprs.at(0).type());
            EXPECT_THAT(if_.else_exprs, SizeIs(1));
            EXPECT_EQ(typeid(ast::bin_op), if_.else_exprs.at(0).type());
        }

        {
            ASSERT_EQ(typeid(ast::if_), c.exprs.at(3).type());
            ast::if_ if_ = boost::get<ast::if_>(c.exprs.at(3));
            EXPECT_EQ(typeid(ast::bin_op), if_.cond.type());
            EXPECT_THAT(if_.then_exprs, SizeIs(1));
            EXPECT_EQ(typeid(ast::bin_op), if_.then_exprs.at(0).type());
            EXPECT_THAT(if_.else_exprs, SizeIs(1));
            EXPECT_EQ(typeid(ast::if_), if_.else_exprs.at(0).type());
            ast::if_ if1 = boost::get<ast::if_>(if_.else_exprs.at(0));
            EXPECT_EQ(typeid(ast::bin_op), if1.cond.type());
            EXPECT_THAT(if1.then_exprs, SizeIs(1));
            EXPECT_EQ(typeid(ast::bin_op), if1.then_exprs.at(0).type());
            EXPECT_THAT(if1.else_exprs, IsEmpty());
        }
    }
}

TEST(Parser, ParseForLoop)
{
    const std::vector<std::string> str_vec {
        "class name "
        "for(;;) endfor for(1+2;1+2;1+2) 1+2; endfor "
        "for(1+2;;) endfor for(;1+2;1+2) 1+2; endfor "
        "for(;1+2;) endfor for(1+2;;1+2) 1+2; endfor "
        "for(;;1+2) endfor for(1+2;1+2;) 1+2; endfor "
        "endclass",
        "class name "
        "FOR\n\t(\n\t;\n\t;\n\t) \n\tENDFOR \n\tFor\n\t(\n\t1+2;\n\t1+2\n\t;1+2\n\t) 1+2;\n\t EndFor "
        "FOR\n\t(\n\t1+2\n\t;\n\t;\n\t)\n\t END\n\t FOR For\n\t(;\n\t1+2;\n\t1+2) 1+2\n\t; End \n\tFor \n\t"
        "for (\n\t;1+2\n\t;)\n\t end\n\t for \n\tfOR\n\t(1+2;;1+2) \n\t1+2;\n\t eNDfOR\n\t "
        "FOr\n\t(;\n\t;1+2) \n\tEndfor FOR\n\t\n\t(\n\t1+2;\n\t1+2;)\n\t 1+2; \n\tEND\n\t For "
        "endclass",
    };

    auto prsr = parser::instance();
    ast::node ret;

    for(auto &str : str_vec)
    {
        EXPECT_TRUE(prsr->parse_string(str, ret));
        ASSERT_EQ(typeid(ast::class_), ret.type());

        ast::class_ c = boost::get<ast::class_>(ret);
        EXPECT_EQ("name", c.name);
        EXPECT_THAT(c.exprs, SizeIs(8));
        EXPECT_THAT(c.methods, IsEmpty());

        // for(;;) endfor
        {
            ASSERT_EQ(typeid(ast::for_), c.exprs.at(0).type());
            ast::for_ for_ = boost::get<ast::for_>(c.exprs.at(0));
            EXPECT_EQ(0, for_.init.which());
            EXPECT_EQ(0, for_.cond.which());
            EXPECT_EQ(0, for_.step.which());
            EXPECT_THAT(for_.exprs, IsEmpty());
        }

        // for(1+2;1+2;1+2) 1+2; endfor
        {
            ASSERT_EQ(typeid(ast::for_), c.exprs.at(1).type());
            ast::for_ for_ = boost::get<ast::for_>(c.exprs.at(1));
            EXPECT_EQ(typeid(ast::bin_op), for_.init.type());
            EXPECT_EQ(typeid(ast::bin_op), for_.cond.type());
            EXPECT_EQ(typeid(ast::bin_op), for_.step.type());
            EXPECT_THAT(for_.exprs, SizeIs(1));
            EXPECT_EQ(typeid(ast::bin_op), for_.exprs.at(0).type());
        }

        // for(1+2;;) endfor
        {
            ASSERT_EQ(typeid(ast::for_), c.exprs.at(2).type());
            ast::for_ for_ = boost::get<ast::for_>(c.exprs.at(2));
            EXPECT_EQ(typeid(ast::bin_op), for_.init.type());
            EXPECT_EQ(0, for_.cond.which());
            EXPECT_EQ(0, for_.step.which());
            EXPECT_THAT(for_.exprs, IsEmpty());
        }

        // for(;1+2;1+2) 1+2; endfor
        {
            ASSERT_EQ(typeid(ast::for_), c.exprs.at(3).type());
            ast::for_ for_ = boost::get<ast::for_>(c.exprs.at(3));
            EXPECT_EQ(0, for_.init.which());
            EXPECT_EQ(typeid(ast::bin_op), for_.cond.type());
            EXPECT_EQ(typeid(ast::bin_op), for_.step.type());
            EXPECT_THAT(for_.exprs, SizeIs(1));
            EXPECT_EQ(typeid(ast::bin_op), for_.exprs.at(0).type());
        }

        // for(;1+2;) endfor
        {
            ASSERT_EQ(typeid(ast::for_), c.exprs.at(4).type());
            ast::for_ for_ = boost::get<ast::for_>(c.exprs.at(4));
            EXPECT_EQ(0, for_.init.which());
            EXPECT_EQ(typeid(ast::bin_op), for_.cond.type());
            EXPECT_EQ(0, for_.step.which());
            EXPECT_THAT(for_.exprs, IsEmpty());
        }

        // for(1+2;;1+2) 1+2; endfor
        {
            ASSERT_EQ(typeid(ast::for_), c.exprs.at(5).type());
            ast::for_ for_ = boost::get<ast::for_>(c.exprs.at(5));
            EXPECT_EQ(typeid(ast::bin_op), for_.init.type());
            EXPECT_EQ(0, for_.cond.which());
            EXPECT_EQ(typeid(ast::bin_op), for_.step.type());
            EXPECT_THAT(for_.exprs, SizeIs(1));
            EXPECT_EQ(typeid(ast::bin_op), for_.exprs.at(0).type());
        }

        // for(;;1+2) endfor
        {
            ASSERT_EQ(typeid(ast::for_), c.exprs.at(6).type());
            ast::for_ for_ = boost::get<ast::for_>(c.exprs.at(6));
            EXPECT_EQ(0, for_.init.which());
            EXPECT_EQ(0, for_.cond.which());
            EXPECT_EQ(typeid(ast::bin_op), for_.step.type());
            EXPECT_THAT(for_.exprs, IsEmpty());
        }

        // for(1+2;1+2;) 1+2; endfor
        {
            ASSERT_EQ(typeid(ast::for_), c.exprs.at(7).type());
            ast::for_ for_ = boost::get<ast::for_>(c.exprs.at(7));
            EXPECT_EQ(typeid(ast::bin_op), for_.init.type());
            EXPECT_EQ(typeid(ast::bin_op), for_.cond.type());
            EXPECT_EQ(0, for_.step.which());
            EXPECT_THAT(for_.exprs, SizeIs(1));
            EXPECT_EQ(typeid(ast::bin_op), for_.exprs.at(0).type());
        }
    }
}

TEST(Parser, ParseWhileLoop)
{
    const std::vector<std::string> str_vec {
        "class name "
        "while(1+2) endwhile while(1+2) 1+2; endwhile "
        "endclass",
        "class name "
        "WHILE\n\t(\n\t1+2\n\t) \n\tENDWHILE \n\tWHILE\n\t(1+2) \n\t1+2; \n\tEND WHILE "
        "endclass",
        "class name "
        "\n\tWhile\n\t(\n\t1+2\n\t) \n\tEndWhile \n\tWhile\n\t(\n\t1+2) \n\t1+2; \n\tEndWhile\n\t "
        "endclass",
    };

    auto prsr = parser::instance();
    ast::node ret;

    for(auto &str : str_vec)
    {
        EXPECT_TRUE(prsr->parse_string(str, ret));
        ASSERT_EQ(typeid(ast::class_), ret.type());

        ast::class_ c = boost::get<ast::class_>(ret);
        EXPECT_EQ("name", c.name);
        EXPECT_THAT(c.exprs, SizeIs(2));
        EXPECT_THAT(c.methods, IsEmpty());

        {
            ASSERT_EQ(typeid(ast::while_), c.exprs.at(0).type());
            ast::while_ while_ = boost::get<ast::while_>(c.exprs.at(0));
            EXPECT_EQ(typeid(ast::bin_op), while_.cond.type());
            EXPECT_THAT(while_.exprs, IsEmpty());
        }

        {
            ASSERT_EQ(typeid(ast::while_), c.exprs.at(1).type());
            ast::while_ while_ = boost::get<ast::while_>(c.exprs.at(1));
            EXPECT_EQ(typeid(ast::bin_op), while_.cond.type());
            EXPECT_THAT(while_.exprs, SizeIs(1));
            EXPECT_EQ(typeid(ast::bin_op), while_.exprs.at(0).type());
        }
    }
}

TEST(Parser, ParseBranches)
{
    const std::vector<std::string> str_vec {
        "class name "
        "continue; break; return; return 1+2; "
        "endclass",
        "class name "
        "CONTINUE; BREAK; RETURN; RETURN 1+2; "
        "endclass",
        "class name "
        "Continue; Break; Return; Return 1+2; "
        "endclass",
    };

    auto prsr = parser::instance();
    ast::node ret;

    for(auto &str : str_vec)
    {
        EXPECT_TRUE(prsr->parse_string(str, ret));
        ASSERT_EQ(typeid(ast::class_), ret.type());

        ast::class_ c = boost::get<ast::class_>(ret);
        EXPECT_EQ("name", c.name);
        EXPECT_THAT(c.exprs, SizeIs(4));
        EXPECT_THAT(c.methods, IsEmpty());

        ASSERT_EQ(typeid(ast::continue_), c.exprs.at(0).type());
        ASSERT_EQ(typeid(ast::break_), c.exprs.at(1).type());
        ASSERT_EQ(typeid(ast::return_), c.exprs.at(2).type());
        ast::return_ return_ = boost::get<ast::return_>(c.exprs.at(2));
        EXPECT_EQ(0, return_.e.which());
        ASSERT_EQ(typeid(ast::return_), c.exprs.at(3).type());
        ast::return_ return1 = boost::get<ast::return_>(c.exprs.at(3));
        EXPECT_EQ(typeid(ast::bin_op), return1.e.type());
    }
}

TEST(Parser, ParseMethodDef)
{
    const std::vector<std::string> str_vec {
        "class name "
        "method() {} method1(arg1) {} method2(arg1,arg2 byref) { 1+2; } "
        "endclass",
    };

    auto prsr = parser::instance();
    ast::node ret;

    for(auto &str : str_vec)
    {
        EXPECT_TRUE(prsr->parse_string(str, ret));
        ASSERT_EQ(typeid(ast::class_), ret.type());

        ast::class_ c = boost::get<ast::class_>(ret);
        EXPECT_EQ("name", c.name);
        EXPECT_THAT(c.exprs, IsEmpty());
        EXPECT_THAT(c.methods, SizeIs(3));

        {
            ast::method &method = c.methods.at(0);
            EXPECT_EQ("method", method.name);
            EXPECT_THAT(method.params, IsEmpty());
            EXPECT_THAT(method.exprs, IsEmpty());
        }

        {
            ast::method &method = c.methods.at(1);
            EXPECT_EQ("method1", method.name);
            EXPECT_THAT(method.params, SizeIs(1));
            EXPECT_EQ("arg1", method.params.at(0).name);
            EXPECT_FALSE(method.params.at(0).by_ref);
            EXPECT_THAT(method.exprs, IsEmpty());
        }

        {
            ast::method &method = c.methods.at(2);
            EXPECT_EQ("method2", method.name);
            EXPECT_THAT(method.params, SizeIs(2));
            EXPECT_EQ("arg1", method.params.at(0).name);
            EXPECT_FALSE(method.params.at(0).by_ref);
            EXPECT_EQ("arg2", method.params.at(1).name);
            EXPECT_TRUE(method.params.at(1).by_ref);
            EXPECT_THAT(method.exprs, SizeIs(1));
            EXPECT_EQ(typeid(ast::bin_op), method.exprs.at(0).type());
        }
    }
}
