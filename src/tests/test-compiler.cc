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

#include "../emel/compiler/compiler.h"

using namespace emel;
using namespace std::literals;

using testing::SizeIs;
using testing::Eq;
using testing::IsEmpty;
using testing::ElementsAreArray;

TEST(Compiler, SortClasses)
{
    std::vector<ast::class_> classes;
    ast::class_ cl;
    cl.name = "Object";
    classes.push_back(cl);
    cl.name = "Name";
    classes.push_back(cl);
    cl.name = "Name2";
    cl.base_name = "Object";
    classes.push_back(cl);
    cl.name = "Name3";
    cl.base_name = "BaseName";
    classes.push_back(cl);
    cl.name = "BaseName";
    cl.base_name = "BaseName2";
    classes.push_back(cl);
    cl.name = "BaseName2";
    cl.base_name = "";
    classes.push_back(cl);

    auto ret = compiler::compiler::topological_sort(classes);
    std::vector<std::string> names;
    names.reserve(ret.size());

    std::transform(ret.begin(), ret.end(),
        std::back_inserter(names),
        [](const ast::class_ &c) { return c.name; });

    EXPECT_THAT(names, ElementsAreArray({
        "Object",
        "Name",
        "Name2",
        "BaseName2",
        "BaseName",
        "Name3"
    }));
}

TEST(Compiler, EmptyObjectClass)
{
    ast::class_ class_;
    class_.name = "Object";

    auto module = std::make_shared<semantic::module>();
    compiler::symbol_table syms;
    semantic::graph_type graph;
    std::vector<value_type> const_pool;
    compiler::codegen c("test", module, syms, graph, const_pool);
    c(class_);
    auto res = c.get_result();

    ASSERT_THAT(const_pool, SizeIs(5));
    EXPECT_THAT(boost::get<std::string>(const_pool[1]), Eq("test"));
    EXPECT_THAT(boost::get<std::string>(const_pool[2]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(const_pool[3]), Eq(""));
    EXPECT_THAT(boost::get<std::string>(const_pool[4]), Eq("~init"));

    EXPECT_THAT(module->name_index, Eq(1));
    ASSERT_THAT(module->classes, SizeIs(1));
    EXPECT_THAT(module->classes.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->nr_args, Eq(0));
    EXPECT_THAT(module->classes.back()->name_index, Eq(2));
    EXPECT_THAT(module->classes.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->code_range.second, Eq(2));
    EXPECT_THAT(module->classes.back()->base_name_index, Eq(3));
    EXPECT_THAT(module->classes.back()->fields_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->methods_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->fields, IsEmpty());
    ASSERT_THAT(module->classes.back()->methods, SizeIs(1));

    EXPECT_THAT(module->classes.back()->methods.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_args, Eq(1));
    EXPECT_THAT(module->classes.back()->methods.back()->name_index, Eq(4));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.second, Eq(2));

    ASSERT_THAT(res.insns, SizeIs(2));
    EXPECT_THAT(res.insns, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),
        insn_encode(opcode::drop_frame, 1)
    }));
}

TEST(Compiler, EmptyClass)
{
    ast::class_ class_;
    class_.name = "Name";

    {
        auto module = std::make_shared<semantic::module>();
        compiler::symbol_table syms;
        semantic::graph_type graph;
        std::vector<value_type> const_pool;
        compiler::codegen c("test", module, syms, graph, const_pool);

        ASSERT_THROW(c(class_), std::runtime_error);
    }

    ast::class_ obj_class_;
    obj_class_.name = "Object";

    auto module = std::make_shared<semantic::module>();
    compiler::symbol_table syms;
    semantic::graph_type graph;
    std::vector<value_type> const_pool;
    compiler::codegen c("test", module, syms, graph, const_pool);
    c(obj_class_);
    c(class_);
    auto res = c.get_result();

    ASSERT_THAT(const_pool, SizeIs(6));
    EXPECT_THAT(boost::get<std::string>(const_pool[1]), Eq("test"));
    EXPECT_THAT(boost::get<std::string>(const_pool[2]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(const_pool[3]), Eq(""));
    EXPECT_THAT(boost::get<std::string>(const_pool[4]), Eq("~init"));
    EXPECT_THAT(boost::get<std::string>(const_pool[5]), Eq("Name"));

    EXPECT_THAT(module->name_index, Eq(1));
    ASSERT_THAT(module->classes, SizeIs(2));
    EXPECT_THAT(module->classes.back()->index, Eq(1));
    EXPECT_THAT(module->classes.back()->nr_args, Eq(0));
    EXPECT_THAT(module->classes.back()->name_index, Eq(5));
    EXPECT_THAT(module->classes.back()->code_range.first, Eq(2));
    EXPECT_THAT(module->classes.back()->code_range.second, Eq(6));
    EXPECT_THAT(module->classes.back()->base_name_index, Eq(2));
    EXPECT_THAT(module->classes.back()->fields_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->methods_offset, Eq(1));
    EXPECT_THAT(module->classes.back()->fields, IsEmpty());
    ASSERT_THAT(module->classes.back()->methods, SizeIs(1));

    EXPECT_THAT(module->classes.back()->methods.back()->index, Eq(1));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_args, Eq(1));
    EXPECT_THAT(module->classes.back()->methods.back()->name_index, Eq(4));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.first, Eq(2));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.second, Eq(6));

    ASSERT_THAT(res.insns, SizeIs(6));
    EXPECT_THAT(res.insns, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),
        insn_encode(opcode::drop_frame, 1),

        insn_encode(opcode::push_frame, 1),

        // call base ctor
        insn_encode(opcode::push_local, 0),
        insn_encode(opcode::fcall, 0),

        insn_encode(opcode::drop_frame, 1)
    }));
}

TEST(Compiler, EmptyClassWithBaseClass)
{
    ast::class_ class_;
    class_.base_name = "BaseName";
    class_.name = "Name";

    ast::class_ obj_class_;
    obj_class_.name = "Object";

    ast::class_ base_class_;
    base_class_.name = "BaseName";

    auto module = std::make_shared<semantic::module>();
    compiler::symbol_table syms;
    semantic::graph_type graph;
    std::vector<value_type> const_pool;
    compiler::codegen c("test", module, syms, graph, const_pool);
    c(obj_class_);
    c(base_class_);
    c(class_);
    auto res = c.get_result();

    ASSERT_THAT(const_pool, SizeIs(7));
    EXPECT_THAT(boost::get<std::string>(const_pool[1]), Eq("test"));
    EXPECT_THAT(boost::get<std::string>(const_pool[2]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(const_pool[3]), Eq(""));
    EXPECT_THAT(boost::get<std::string>(const_pool[4]), Eq("~init"));
    EXPECT_THAT(boost::get<std::string>(const_pool[5]), Eq("BaseName"));
    EXPECT_THAT(boost::get<std::string>(const_pool[6]), Eq("Name"));

    EXPECT_THAT(module->name_index, Eq(1));
    ASSERT_THAT(module->classes, SizeIs(3));
    EXPECT_THAT(module->classes.back()->index, Eq(2));
    EXPECT_THAT(module->classes.back()->nr_args, Eq(0));
    EXPECT_THAT(module->classes.back()->name_index, Eq(6));
    EXPECT_THAT(module->classes.back()->code_range.first, Eq(6));
    EXPECT_THAT(module->classes.back()->code_range.second, Eq(10));
    EXPECT_THAT(module->classes.back()->base_name_index, Eq(5));
    EXPECT_THAT(module->classes.back()->fields_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->methods_offset, Eq(2));
    EXPECT_THAT(module->classes.back()->fields, IsEmpty());
    ASSERT_THAT(module->classes.back()->methods, SizeIs(1));

    EXPECT_THAT(module->classes.back()->methods.back()->index, Eq(2));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_args, Eq(1));
    EXPECT_THAT(module->classes.back()->methods.back()->name_index, Eq(4));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.first, Eq(6));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.second, Eq(10));

    ASSERT_THAT(res.insns, SizeIs(10));
    EXPECT_THAT(res.insns, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),
        insn_encode(opcode::drop_frame, 1),

        // BaseName ctor
        insn_encode(opcode::push_frame, 1),
        insn_encode(opcode::push_local, 0),
        insn_encode(opcode::fcall, 0),
        insn_encode(opcode::drop_frame, 1),

        // Name ctor
        insn_encode(opcode::push_frame, 1),
        insn_encode(opcode::push_local, 0),
        insn_encode(opcode::fcall, 1),
        insn_encode(opcode::drop_frame, 1)
    }));
}

TEST(Compiler, Constants)
{
    ast::class_ class_;
    class_.name = "Object";

    class_.exprs.emplace_back("hello world!"s);
    class_.exprs.emplace_back(12.3);
    class_.exprs.emplace_back(true);
    class_.exprs.emplace_back(false);

    auto module = std::make_shared<semantic::module>();
    compiler::symbol_table syms;
    semantic::graph_type graph;
    std::vector<value_type> const_pool;
    compiler::codegen c("test", module, syms, graph, const_pool);
    c(class_);
    auto res = c.get_result();

    ASSERT_THAT(const_pool, SizeIs(9));
    EXPECT_THAT(boost::get<std::string>(const_pool[1]), Eq("test"));
    EXPECT_THAT(boost::get<std::string>(const_pool[2]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(const_pool[3]), Eq(""));
    EXPECT_THAT(boost::get<std::string>(const_pool[4]), Eq("~init"));
    EXPECT_THAT(boost::get<std::string>(const_pool[5]), Eq("hello world!"));
    EXPECT_THAT(boost::get<double>(const_pool[6]),      Eq(12.3));
    EXPECT_THAT(boost::get<bool>(const_pool[7]),        Eq(true));
    EXPECT_THAT(boost::get<bool>(const_pool[8]),        Eq(false));

    EXPECT_THAT(module->name_index, Eq(1));
    ASSERT_THAT(module->classes, SizeIs(1));
    EXPECT_THAT(module->classes.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->nr_args, Eq(0));
    EXPECT_THAT(module->classes.back()->name_index, Eq(2));
    EXPECT_THAT(module->classes.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->code_range.second, Eq(6));
    EXPECT_THAT(module->classes.back()->base_name_index, Eq(3));
    EXPECT_THAT(module->classes.back()->fields_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->methods_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->fields, IsEmpty());
    ASSERT_THAT(module->classes.back()->methods, SizeIs(1));

    EXPECT_THAT(module->classes.back()->methods.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_args, Eq(1));
    EXPECT_THAT(module->classes.back()->methods.back()->name_index, Eq(4));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.second, Eq(6));

    ASSERT_THAT(res.insns, SizeIs(6));
    EXPECT_THAT(res.insns, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),

        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::push_const, 8),

        insn_encode(opcode::drop_frame, 1)
    }));
}

// TODO debug info for var_name

TEST(Compiler, AssignsInCtor)
{
    ast::class_ class_;
    class_.name = "Object";

    class_.exprs.emplace_back(ast::assign { "VarName", 1.23 });
    class_.exprs.emplace_back(ast::assign { "ReusedName", 5.0 });
    class_.exprs.emplace_back(ast::assign { "ReusedName", 15.0 });

    auto module = std::make_shared<semantic::module>();
    compiler::symbol_table syms;
    semantic::graph_type graph;
    std::vector<value_type> const_pool;
    compiler::codegen c("test", module, syms, graph, const_pool);
    c(class_);
    auto res = c.get_result();

    ASSERT_THAT(const_pool, SizeIs(10));
    EXPECT_THAT(boost::get<std::string>(const_pool[1]), Eq("test"));
    EXPECT_THAT(boost::get<std::string>(const_pool[2]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(const_pool[3]), Eq(""));
    EXPECT_THAT(boost::get<std::string>(const_pool[4]), Eq("~init"));
    EXPECT_THAT(boost::get<std::string>(const_pool[5]), Eq("VarName"));
    EXPECT_THAT(boost::get<double>(const_pool[6]),      Eq(1.23));
    EXPECT_THAT(boost::get<std::string>(const_pool[7]), Eq("ReusedName"));
    EXPECT_THAT(boost::get<double>(const_pool[8]),      Eq(5.0));
    EXPECT_THAT(boost::get<double>(const_pool[9]),      Eq(15.0));

    EXPECT_THAT(module->name_index, Eq(1));
    ASSERT_THAT(module->classes, SizeIs(1));
    EXPECT_THAT(module->classes.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->nr_args, Eq(0));
    EXPECT_THAT(module->classes.back()->name_index, Eq(2));
    EXPECT_THAT(module->classes.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->code_range.second, Eq(11));
    EXPECT_THAT(module->classes.back()->base_name_index, Eq(3));
    EXPECT_THAT(module->classes.back()->fields_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->methods_offset, Eq(0));
    ASSERT_THAT(module->classes.back()->fields, SizeIs(2));
    ASSERT_THAT(module->classes.back()->methods, SizeIs(1));

    auto field = module->classes.back()->fields[0];
    EXPECT_THAT(field->index, Eq(0));
    EXPECT_THAT(field->nr_args, Eq(0));
    EXPECT_THAT(field->name_index, Eq(5));

    field = module->classes.back()->fields[1];
    EXPECT_THAT(field->index, Eq(1));
    EXPECT_THAT(field->nr_args, Eq(0));
    EXPECT_THAT(field->name_index, Eq(7));

    EXPECT_THAT(module->classes.back()->methods.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_args, Eq(1));
    EXPECT_THAT(module->classes.back()->methods.back()->name_index, Eq(4));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.second, Eq(11));

    ASSERT_THAT(res.insns, SizeIs(11));
    EXPECT_THAT(res.insns, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),

        // VarName field init
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::push_local, 0),
        insn_encode(opcode::load_field, 0),

        // ReusedName field init
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::push_local, 0),
        insn_encode(opcode::load_field, 1),

        // ReusedName field reassign
        insn_encode(opcode::push_const, 9),
        insn_encode(opcode::push_local, 0),
        insn_encode(opcode::load_field, 1),

        insn_encode(opcode::drop_frame, 1)
    }));
}

TEST(Compiler, UnaryOp)
{
    ast::class_ class_;
    class_.name = "Object";

    class_.exprs.emplace_back(ast::un_op { op_kind::not_, 1.23 });
    class_.exprs.emplace_back(ast::un_op { op_kind::neg, 1.23 });

    auto module = std::make_shared<semantic::module>();
    compiler::symbol_table syms;
    semantic::graph_type graph;
    std::vector<value_type> const_pool;
    compiler::codegen c("test", module, syms, graph, const_pool);
    c(class_);
    auto res = c.get_result();

    ASSERT_THAT(const_pool, SizeIs(6));
    EXPECT_THAT(boost::get<std::string>(const_pool[1]), Eq("test"));
    EXPECT_THAT(boost::get<std::string>(const_pool[2]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(const_pool[3]), Eq(""));
    EXPECT_THAT(boost::get<std::string>(const_pool[4]), Eq("~init"));
    EXPECT_THAT(boost::get<double>(const_pool[5]),      Eq(1.23));

    EXPECT_THAT(module->name_index, Eq(1));
    ASSERT_THAT(module->classes, SizeIs(1));
    EXPECT_THAT(module->classes.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->nr_args, Eq(0));
    EXPECT_THAT(module->classes.back()->name_index, Eq(2));
    EXPECT_THAT(module->classes.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->code_range.second, Eq(6));
    EXPECT_THAT(module->classes.back()->base_name_index, Eq(3));
    EXPECT_THAT(module->classes.back()->fields_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->methods_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->fields, IsEmpty());
    ASSERT_THAT(module->classes.back()->methods, SizeIs(1));

    EXPECT_THAT(module->classes.back()->methods.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_args, Eq(1));
    EXPECT_THAT(module->classes.back()->methods.back()->name_index, Eq(4));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.second, Eq(6));

    ASSERT_THAT(res.insns, SizeIs(6));
    EXPECT_THAT(res.insns, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),

        // ~1.23
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::call_op, op_kind::not_),

        // -1.23
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::call_op, op_kind::neg),

        insn_encode(opcode::drop_frame, 1)
    }));
}

TEST(Compiler, BinaryOp)
{
    ast::class_ class_;
    class_.name = "Object";

    class_.exprs.emplace_back(ast::bin_op { op_kind::div,
        ast::bin_op { op_kind::sub, 1.23, 2.0 }, ast::bin_op { op_kind::add, 1.23, -20.3 }
    });

    auto module = std::make_shared<semantic::module>();
    compiler::symbol_table syms;
    semantic::graph_type graph;
    std::vector<value_type> const_pool;
    compiler::codegen c("test", module, syms, graph, const_pool);
    c(class_);
    auto res = c.get_result();

    ASSERT_THAT(const_pool, SizeIs(8));
    EXPECT_THAT(boost::get<std::string>(const_pool[1]), Eq("test"));
    EXPECT_THAT(boost::get<std::string>(const_pool[2]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(const_pool[3]), Eq(""));
    EXPECT_THAT(boost::get<std::string>(const_pool[4]), Eq("~init"));
    EXPECT_THAT(boost::get<double>(const_pool[5]),      Eq(-20.3));
    EXPECT_THAT(boost::get<double>(const_pool[6]),      Eq(1.23));
    EXPECT_THAT(boost::get<double>(const_pool[7]),      Eq(2.0));

    EXPECT_THAT(module->name_index, Eq(1));
    ASSERT_THAT(module->classes, SizeIs(1));
    EXPECT_THAT(module->classes.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->nr_args, Eq(0));
    EXPECT_THAT(module->classes.back()->name_index, Eq(2));
    EXPECT_THAT(module->classes.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->code_range.second, Eq(9));
    EXPECT_THAT(module->classes.back()->base_name_index, Eq(3));
    EXPECT_THAT(module->classes.back()->fields_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->methods_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->fields, IsEmpty());
    ASSERT_THAT(module->classes.back()->methods, SizeIs(1));

    EXPECT_THAT(module->classes.back()->methods.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_args, Eq(1));
    EXPECT_THAT(module->classes.back()->methods.back()->name_index, Eq(4));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.second, Eq(9));

    ASSERT_THAT(res.insns, SizeIs(9));
    EXPECT_THAT(res.insns, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),

        // 1.23 - 2.0 / 1.23 + -20.3
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::call_op, op_kind::add),
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::call_op, op_kind::sub),
        insn_encode(opcode::call_op, op_kind::div),

        insn_encode(opcode::drop_frame, 1)
    }));
}

TEST(Compiler, TernaryOp)
{
    ast::class_ class_;
    class_.name = "Object";

    class_.exprs.emplace_back(ast::ternary { true, 1.23, -20.3 });

    class_.exprs.emplace_back(ast::ternary {
        ast::bin_op { op_kind::gte, -1.0, 0.0 },
        ast::bin_op { op_kind::mul, -1.0, 1.0 },
        ast::ternary { false, 1.0, -1.0 }
    });

    auto module = std::make_shared<semantic::module>();
    compiler::symbol_table syms;
    semantic::graph_type graph;
    std::vector<value_type> const_pool;
    compiler::codegen c("test", module, syms, graph, const_pool);
    c(class_);
    auto res = c.get_result();

    ASSERT_THAT(const_pool, SizeIs(12));
    EXPECT_THAT(boost::get<std::string>(const_pool[1]), Eq("test"));
    EXPECT_THAT(boost::get<std::string>(const_pool[2]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(const_pool[3]), Eq(""));
    EXPECT_THAT(boost::get<std::string>(const_pool[4]), Eq("~init"));
    EXPECT_THAT(boost::get<bool>(const_pool[5]),        Eq(true));
    EXPECT_THAT(boost::get<double>(const_pool[6]),      Eq(1.23));
    EXPECT_THAT(boost::get<double>(const_pool[7]),      Eq(-20.3));
    EXPECT_THAT(boost::get<double>(const_pool[8]),      Eq(0.0));
    EXPECT_THAT(boost::get<double>(const_pool[9]),      Eq(-1.0));
    EXPECT_THAT(boost::get<double>(const_pool[10]),     Eq(1.0));
    EXPECT_THAT(boost::get<bool>(const_pool[11]),       Eq(false));

    EXPECT_THAT(module->name_index, Eq(1));
    ASSERT_THAT(module->classes, SizeIs(1));
    EXPECT_THAT(module->classes.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->nr_args, Eq(0));
    EXPECT_THAT(module->classes.back()->name_index, Eq(2));
    EXPECT_THAT(module->classes.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->code_range.second, Eq(20));
    EXPECT_THAT(module->classes.back()->base_name_index, Eq(3));
    EXPECT_THAT(module->classes.back()->fields_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->methods_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->fields, IsEmpty());
    ASSERT_THAT(module->classes.back()->methods, SizeIs(1));

    EXPECT_THAT(module->classes.back()->methods.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_args, Eq(1));
    EXPECT_THAT(module->classes.back()->methods.back()->name_index, Eq(4));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.second, Eq(20));

    ASSERT_THAT(res.insns, SizeIs(20));
    EXPECT_THAT(res.insns, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),

        // True ? 1.23 : -20.3
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::brf_false, 3),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::brf, 2),
        insn_encode(opcode::push_const, 7),

        // -1.0 >= 0.0 ? -1.0 * 1.0 : False ? 1.0 : -1.0
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::push_const, 9),
        insn_encode(opcode::call_op, op_kind::gte),
        insn_encode(opcode::brf_false, 5),
        insn_encode(opcode::push_const, 10),
        insn_encode(opcode::push_const, 9),
        insn_encode(opcode::call_op, op_kind::mul),
        insn_encode(opcode::brf, 6),
        insn_encode(opcode::push_const, 11),
        insn_encode(opcode::brf_false, 3),
        insn_encode(opcode::push_const, 10),
        insn_encode(opcode::brf, 2),
        insn_encode(opcode::push_const, 9),

        insn_encode(opcode::drop_frame, 1)
    }));
}

TEST(Compiler, IfBlock)
{
    ast::class_ class_;
    class_.name = "Object";

    class_.exprs.emplace_back(ast::if_ {
        true, { -1.23 }, { }
    });

    class_.exprs.emplace_back(ast::if_ {
        false, { -1.23 }, { 456.0 }
    });

    auto module = std::make_shared<semantic::module>();
    compiler::symbol_table syms;
    semantic::graph_type graph;
    std::vector<value_type> const_pool;
    compiler::codegen c("test", module, syms, graph, const_pool);
    c(class_);
    auto res = c.get_result();

    ASSERT_THAT(const_pool, SizeIs(9));
    EXPECT_THAT(boost::get<std::string>(const_pool[1]), Eq("test"));
    EXPECT_THAT(boost::get<std::string>(const_pool[2]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(const_pool[3]), Eq(""));
    EXPECT_THAT(boost::get<std::string>(const_pool[4]), Eq("~init"));
    EXPECT_THAT(boost::get<bool>(const_pool[5]),        Eq(true));
    EXPECT_THAT(boost::get<double>(const_pool[6]),      Eq(-1.23));
    EXPECT_THAT(boost::get<bool>(const_pool[7]),        Eq(false));
    EXPECT_THAT(boost::get<double>(const_pool[8]),      Eq(456.0));

    EXPECT_THAT(module->name_index, Eq(1));
    ASSERT_THAT(module->classes, SizeIs(1));
    EXPECT_THAT(module->classes.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->nr_args, Eq(0));
    EXPECT_THAT(module->classes.back()->name_index, Eq(2));
    EXPECT_THAT(module->classes.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->code_range.second, Eq(10));
    EXPECT_THAT(module->classes.back()->base_name_index, Eq(3));
    EXPECT_THAT(module->classes.back()->fields_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->methods_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->fields, IsEmpty());
    ASSERT_THAT(module->classes.back()->methods, SizeIs(1));

    EXPECT_THAT(module->classes.back()->methods.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_args, Eq(1));
    EXPECT_THAT(module->classes.back()->methods.back()->name_index, Eq(4));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.second, Eq(10));

    ASSERT_THAT(res.insns, SizeIs(10));
    EXPECT_THAT(res.insns, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),

        // if(true) -1.23
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::brf_false, 2),
        insn_encode(opcode::push_const, 6),

        // if(false) -1.23
        // else 456.0
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::brf_false, 3),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::brf, 2),
        insn_encode(opcode::push_const, 8),

        insn_encode(opcode::drop_frame, 1)
    }));
}

TEST(Compiler, ForLoop)
{
    ast::class_ class_;
    class_.name = "Object";

    class_.exprs.emplace_back(ast::for_ { });
    class_.exprs.emplace_back(ast::for_ { empty_value, empty_value, empty_value, { 1.23 } });

    class_.exprs.emplace_back(ast::for_ { 999.2, empty_value, empty_value, { } });
    class_.exprs.emplace_back(ast::for_ { 999.2, empty_value, empty_value, { 1.23 } });

    class_.exprs.emplace_back(ast::for_ { empty_value, 152.0, empty_value, { } });
    class_.exprs.emplace_back(ast::for_ { empty_value, 152.0, empty_value, { 1.23 } });

    class_.exprs.emplace_back(ast::for_ { 999.2, 152.0, empty_value, { } });
    class_.exprs.emplace_back(ast::for_ { 999.2, 152.0, empty_value, { 1.23 } });

    class_.exprs.emplace_back(ast::for_ { empty_value, empty_value, 229.7, { } });
    class_.exprs.emplace_back(ast::for_ { empty_value, empty_value, 229.7, { 1.23 } });

    class_.exprs.emplace_back(ast::for_ { 999.2, empty_value, 229.7, { } });
    class_.exprs.emplace_back(ast::for_ { 999.2, empty_value, 229.7, { 1.23 } });

    class_.exprs.emplace_back(ast::for_ { empty_value, 152.0, 229.7, { } });
    class_.exprs.emplace_back(ast::for_ { empty_value, 152.0, 229.7, { 1.23 } });

    class_.exprs.emplace_back(ast::for_ { 999.2, 152.0, 229.7, { } });
    class_.exprs.emplace_back(ast::for_ { 999.2, 152.0, 229.7, { 1.23 } });

    class_.exprs.emplace_back(ast::for_ { empty_value, false, empty_value, { 1.23 } });
    class_.exprs.emplace_back(ast::for_ { empty_value, true, empty_value, { } });
    class_.exprs.emplace_back(ast::for_ { empty_value, true, empty_value, { 1.23 } });

    auto module = std::make_shared<semantic::module>();
    compiler::symbol_table syms;
    semantic::graph_type graph;
    std::vector<value_type> const_pool;
    compiler::codegen c("test", module, syms, graph, const_pool);
    c(class_);
    auto res = c.get_result();

    ASSERT_THAT(const_pool, SizeIs(9));
    EXPECT_THAT(boost::get<std::string>(const_pool[1]), Eq("test"));
    EXPECT_THAT(boost::get<std::string>(const_pool[2]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(const_pool[3]), Eq(""));
    EXPECT_THAT(boost::get<std::string>(const_pool[4]), Eq("~init"));
    EXPECT_THAT(boost::get<double>(const_pool[5]),      Eq(1.23));
    EXPECT_THAT(boost::get<double>(const_pool[6]),      Eq(999.2));
    EXPECT_THAT(boost::get<double>(const_pool[7]),      Eq(152.0));
    EXPECT_THAT(boost::get<double>(const_pool[8]),      Eq(229.7));

    EXPECT_THAT(module->name_index, Eq(1));
    ASSERT_THAT(module->classes, SizeIs(1));
    EXPECT_THAT(module->classes.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->nr_args, Eq(0));
    EXPECT_THAT(module->classes.back()->name_index, Eq(2));
    EXPECT_THAT(module->classes.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->code_range.second, Eq(61));
    EXPECT_THAT(module->classes.back()->base_name_index, Eq(3));
    EXPECT_THAT(module->classes.back()->fields_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->methods_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->fields, IsEmpty());
    ASSERT_THAT(module->classes.back()->methods, SizeIs(1));

    EXPECT_THAT(module->classes.back()->methods.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_args, Eq(1));
    EXPECT_THAT(module->classes.back()->methods.back()->name_index, Eq(4));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.second, Eq(61));

    ASSERT_THAT(res.insns, SizeIs(61));
    EXPECT_THAT(res.insns, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),

        // for(;;) { }
        insn_encode(opcode::brb, 0),

        // for(;;) { 1.23 }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::brb, 1),

        // for(999.2;;) { }
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::brb, 0),

        // for(999.2;;) { 1.23 }
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::brb, 1),

        // for(; 152.0;) { }
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::brf_false, 2),
        insn_encode(opcode::brb, 2),

        // for(; 152.0;) { 1.23 }
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::brf_false, 3),
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::brb, 3),

        // for(999.2; 152.0;) { }
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::brf_false, 2),
        insn_encode(opcode::brb, 2),

        // for(999.2; 152.0;) { 1.23 }
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::brf_false, 3),
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::brb, 3),

        // for(;; 229.7) { }
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::brb, 1),

        // for(;; 229.7) { 1.23 }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::brb, 2),

        // for(999.2;; 229.7) { }
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::brb, 1),

        // for(999.2;; 229.7) { 1.23 }
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::brb, 2),

        // for(; 152.0; 229.7) { }
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::brf_false, 3),
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::brb, 3),

        // for(; 152.0; 229.7) { 1.23 }
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::brf_false, 4),
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::brb, 4),

        // for(999.2; 152.0; 229.7) { }
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::brf_false, 3),
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::brb, 3),

        // for(999.2; 152.0; 229.7) { 1.23 }
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::brf_false, 4),
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::brb, 4),

        // for(; false;) { 1.23 }
        // generate nothing

        // for(; true;) { }
        insn_encode(opcode::brb, 0),

        // for(; true;) { 1.23 }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::brb, 1),

        insn_encode(opcode::drop_frame, 1)
    }));
}

TEST(Compiler, WhileLoop)
{
    ast::class_ class_;
    class_.name = "Object";

    class_.exprs.emplace_back(ast::while_ { 1.23, { } });
    class_.exprs.emplace_back(ast::while_ { 1.23, { -1.23 } });
    class_.exprs.emplace_back(ast::while_ { true, { } });
    class_.exprs.emplace_back(ast::while_ { true, { -1.23 } });
    class_.exprs.emplace_back(ast::while_ { false, { } });
    class_.exprs.emplace_back(ast::while_ { false, { -1.23 } });

    auto module = std::make_shared<semantic::module>();
    compiler::symbol_table syms;
    semantic::graph_type graph;
    std::vector<value_type> const_pool;
    compiler::codegen c("test", module, syms, graph, const_pool);
    c(class_);
    auto res = c.get_result();

    ASSERT_THAT(const_pool, SizeIs(7));
    EXPECT_THAT(boost::get<std::string>(const_pool[1]), Eq("test"));
    EXPECT_THAT(boost::get<std::string>(const_pool[2]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(const_pool[3]), Eq(""));
    EXPECT_THAT(boost::get<std::string>(const_pool[4]), Eq("~init"));
    EXPECT_THAT(boost::get<double>(const_pool[5]),      Eq(1.23));
    EXPECT_THAT(boost::get<double>(const_pool[6]),      Eq(-1.23));

    EXPECT_THAT(module->name_index, Eq(1));
    ASSERT_THAT(module->classes, SizeIs(1));
    EXPECT_THAT(module->classes.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->nr_args, Eq(0));
    EXPECT_THAT(module->classes.back()->name_index, Eq(2));
    EXPECT_THAT(module->classes.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->code_range.second, Eq(12));
    EXPECT_THAT(module->classes.back()->base_name_index, Eq(3));
    EXPECT_THAT(module->classes.back()->fields_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->methods_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->fields, IsEmpty());
    ASSERT_THAT(module->classes.back()->methods, SizeIs(1));

    EXPECT_THAT(module->classes.back()->methods.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_args, Eq(1));
    EXPECT_THAT(module->classes.back()->methods.back()->name_index, Eq(4));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.second, Eq(12));

    ASSERT_THAT(res.insns, SizeIs(12));
    EXPECT_THAT(res.insns, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),

        // while(1.23) { }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::brf_false, 2),
        insn_encode(opcode::brb, 2),

        // while(1.23) { -1.23 }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::brf_false, 3),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::brb, 3),

        // while(true) { }
        insn_encode(opcode::brb, 0),

        // while(true) { -1.23 }
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::brb, 1),

        // while(false) { }
        // generate nothing

        // while(false) { -1.23 }
        // generate nothing

        insn_encode(opcode::drop_frame, 1)
    }));
}

TEST(Compiler, SwitchBlockWithNumbers)
{
    ast::class_ class_;
    class_.name = "Object";

    class_.exprs.emplace_back(ast::switch_ { 1.23, { } });

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { 2.34 }, { } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { 2.34 }, { 3.45 } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { 2.34, 10.23 }, { 3.45 } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { 2.34 }, { } },
        ast::case_ { { 10.23 }, { 3.47 } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { 2.34 }, { } },
        ast::case_ { { 10.23 }, { } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { 2.34 }, { 3.45 } },
        ast::case_ { { 10.23 }, { 3.47 } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { 2.34, 10.23 }, { 3.45 } },
        ast::case_ { { 1.23, 3.45 }, { 3.47, 2.34 } }
    }});

    auto module = std::make_shared<semantic::module>();
    compiler::symbol_table syms;
    semantic::graph_type graph;
    std::vector<value_type> const_pool;
    compiler::codegen c("test", module, syms, graph, const_pool);
    c(class_);
    auto res = c.get_result();

    ASSERT_THAT(const_pool, SizeIs(10));
    EXPECT_THAT(boost::get<std::string>(const_pool[1]), Eq("test"));
    EXPECT_THAT(boost::get<std::string>(const_pool[2]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(const_pool[3]), Eq(""));
    EXPECT_THAT(boost::get<std::string>(const_pool[4]), Eq("~init"));
    EXPECT_THAT(boost::get<double>(const_pool[5]),      Eq(1.23));
    EXPECT_THAT(boost::get<double>(const_pool[6]),      Eq(2.34));
    EXPECT_THAT(boost::get<double>(const_pool[7]),      Eq(3.45));
    EXPECT_THAT(boost::get<double>(const_pool[8]),      Eq(10.23));
    EXPECT_THAT(boost::get<double>(const_pool[9]),      Eq(3.47));

    EXPECT_THAT(module->name_index, Eq(1));
    ASSERT_THAT(module->classes, SizeIs(1));
    EXPECT_THAT(module->classes.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->nr_args, Eq(0));
    EXPECT_THAT(module->classes.back()->name_index, Eq(2));
    EXPECT_THAT(module->classes.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->code_range.second, Eq(46));
    EXPECT_THAT(module->classes.back()->base_name_index, Eq(3));
    EXPECT_THAT(module->classes.back()->fields_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->methods_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->fields, IsEmpty());
    ASSERT_THAT(module->classes.back()->methods, SizeIs(1));

    EXPECT_THAT(module->classes.back()->methods.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_args, Eq(1));
    EXPECT_THAT(module->classes.back()->methods.back()->name_index, Eq(4));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.second, Eq(46));

    ASSERT_THAT(res.insns, SizeIs(46));
    EXPECT_THAT(res.insns, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),

        // switch(1.23) { }
        // generate nothing

        // switch(1.23) {
        //  case 2.34:
        // }
        // generate nothing

        // switch(1.23) {
        //  case 2.34: 3.45
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::call_op, op_kind::eq),
        insn_encode(opcode::brf_false, 2),
        insn_encode(opcode::push_const, 7),

        // switch(1.23) {
        //  case 2.34: case 10.23: 3.45
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::dup, 1),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::call_op, op_kind::eq),
        insn_encode(opcode::brf_true, 4),
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::call_op, op_kind::eq),
        insn_encode(opcode::brf_false, 2),
        insn_encode(opcode::push_const, 7),

        // switch(1.23) {
        //  case 2.34:
        //  case 10.23: 3.47
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::call_op, op_kind::eq),
        insn_encode(opcode::brf_false, 2),
        insn_encode(opcode::push_const, 9),

        // switch(1.23) {
        //  case 2.34:
        //  case 10.23:
        // }
        // generate nothing

        // switch(1.23) {
        //  case 2.34: 3.45
        //  case 10.23: 3.47
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::push, 4),
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::push, 2),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::br_table, 2),
        insn_encode(opcode::brf, 4),
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::brf, 2),
        insn_encode(opcode::push_const, 9),

        // switch(1.23) {
        //  case 2.34: case 10.23: 3.45
        //  case 1.23: case 3.45: 3.47, 2.34
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::push, 5),
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::push, 2),
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::push, 5),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::push, 2),
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::br_table, 4),
        insn_encode(opcode::brf, 5),
        insn_encode(opcode::push_const, 9),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::brf, 2),
        insn_encode(opcode::push_const, 7),

        insn_encode(opcode::drop_frame, 1)
    }));
}

TEST(Compiler, SwitchBlockWithStrings)
{
    ast::class_ class_;
    class_.name = "Object";

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { "one"s }, { } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { "one"s }, { 3.45 } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { "one"s, "two"s }, { 3.45 } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { "one"s }, { } },
        ast::case_ { { "two"s }, { 3.47 } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { "one"s }, { } },
        ast::case_ { { "two"s }, { } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { "one"s }, { 3.45 } },
        ast::case_ { { "two"s }, { 3.47 } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { "one"s, "two"s }, { 3.45, 1.23 } },
        ast::case_ { { std::string("") }, { 3.47 } }
    }});

    auto module = std::make_shared<semantic::module>();
    compiler::symbol_table syms;
    semantic::graph_type graph;
    std::vector<value_type> const_pool;
    compiler::codegen c("test", module, syms, graph, const_pool);
    c(class_);
    auto res = c.get_result();

    ASSERT_THAT(const_pool, SizeIs(10));
    EXPECT_THAT(boost::get<std::string>(const_pool[1]), Eq("test"));
    EXPECT_THAT(boost::get<std::string>(const_pool[2]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(const_pool[3]), Eq(""));
    EXPECT_THAT(boost::get<std::string>(const_pool[4]), Eq("~init"));
    EXPECT_THAT(boost::get<double>(const_pool[5]),      Eq(1.23));
    EXPECT_THAT(boost::get<std::string>(const_pool[6]), Eq("one"));
    EXPECT_THAT(boost::get<double>(const_pool[7]),      Eq(3.45));
    EXPECT_THAT(boost::get<std::string>(const_pool[8]), Eq("two"));
    EXPECT_THAT(boost::get<double>(const_pool[9]),      Eq(3.47));

    EXPECT_THAT(module->name_index, Eq(1));
    ASSERT_THAT(module->classes, SizeIs(1));
    EXPECT_THAT(module->classes.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->nr_args, Eq(0));
    EXPECT_THAT(module->classes.back()->name_index, Eq(2));
    EXPECT_THAT(module->classes.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->code_range.second, Eq(44));
    EXPECT_THAT(module->classes.back()->base_name_index, Eq(3));
    EXPECT_THAT(module->classes.back()->fields_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->methods_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->fields, IsEmpty());
    ASSERT_THAT(module->classes.back()->methods, SizeIs(1));

    EXPECT_THAT(module->classes.back()->methods.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_args, Eq(1));
    EXPECT_THAT(module->classes.back()->methods.back()->name_index, Eq(4));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.second, Eq(44));

    ASSERT_THAT(res.insns, SizeIs(44));
    EXPECT_THAT(res.insns, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),

        // switch(1.23) {
        //  case "one":
        // }
        // generate nothing

        // switch(1.23) {
        //  case "one": 3.45
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::call_op, op_kind::eq),
        insn_encode(opcode::brf_false, 2),
        insn_encode(opcode::push_const, 7),

        // switch(1.23) {
        //  case "one": case "two": 3.45
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::dup, 1),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::call_op, op_kind::eq),
        insn_encode(opcode::brf_true, 4),
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::call_op, op_kind::eq),
        insn_encode(opcode::brf_false, 2),
        insn_encode(opcode::push_const, 7),

        // switch(1.23) {
        //  case "one":
        //  case "two": 3.47
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::call_op, op_kind::eq),
        insn_encode(opcode::brf_false, 2),
        insn_encode(opcode::push_const, 9),

        // switch(1.23) {
        //  case "one":
        //  case "two":
        // }
        // generate nothing

        // switch(1.23) {
        //  case "one": 3.45
        //  case "two": 3.47
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::push, 4),
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::push, 2),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::br_table, 2),
        insn_encode(opcode::brf, 4),
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::brf, 2),
        insn_encode(opcode::push_const, 9),

        // switch(1.23) {
        //  case "one": case "two": 3.45, 1.23
        //  case "": 3.47
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::push, 4),
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::push, 4),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::push, 2),
        insn_encode(opcode::push_const, 3),
        insn_encode(opcode::br_table, 3),
        insn_encode(opcode::brf, 5),
        insn_encode(opcode::push_const, 9),
        insn_encode(opcode::brf, 3),
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::push_const, 5),

        insn_encode(opcode::drop_frame, 1)
    }));
}

TEST(Compiler, SwitchBlockWithBooleans)
{
    ast::class_ class_;
    class_.name = "Object";

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { true }, { } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { true }, { 3.45 } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { false }, { 3.45 } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { false }, { 3.45 } },
        ast::case_ { { true }, { 3.47 } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { true }, { 3.45 } },
        ast::case_ { { false }, { 3.47 } }
    }});

    auto module = std::make_shared<semantic::module>();
    compiler::symbol_table syms;
    semantic::graph_type graph;
    std::vector<value_type> const_pool;
    compiler::codegen c("test", module, syms, graph, const_pool);
    c(class_);
    auto res = c.get_result();

    ASSERT_THAT(const_pool, SizeIs(10));
    EXPECT_THAT(boost::get<std::string>(const_pool[1]), Eq("test"));
    EXPECT_THAT(boost::get<std::string>(const_pool[2]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(const_pool[3]), Eq(""));
    EXPECT_THAT(boost::get<std::string>(const_pool[4]), Eq("~init"));
    EXPECT_THAT(boost::get<double>(const_pool[5]),      Eq(1.23));
    EXPECT_THAT(boost::get<bool>(const_pool[6]),        Eq(true));
    EXPECT_THAT(boost::get<double>(const_pool[7]),      Eq(3.45));
    EXPECT_THAT(boost::get<bool>(const_pool[8]),        Eq(false));
    EXPECT_THAT(boost::get<double>(const_pool[9]),      Eq(3.47));

    EXPECT_THAT(module->name_index, Eq(1));
    ASSERT_THAT(module->classes, SizeIs(1));
    EXPECT_THAT(module->classes.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->nr_args, Eq(0));
    EXPECT_THAT(module->classes.back()->name_index, Eq(2));
    EXPECT_THAT(module->classes.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->code_range.second, Eq(22));
    EXPECT_THAT(module->classes.back()->base_name_index, Eq(3));
    EXPECT_THAT(module->classes.back()->fields_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->methods_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->fields, IsEmpty());
    ASSERT_THAT(module->classes.back()->methods, SizeIs(1));

    EXPECT_THAT(module->classes.back()->methods.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_args, Eq(1));
    EXPECT_THAT(module->classes.back()->methods.back()->name_index, Eq(4));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.second, Eq(22));

    ASSERT_THAT(res.insns, SizeIs(22));
    EXPECT_THAT(res.insns, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),

        // switch(1.23) {
        //  case True:
        // }
        // generate nothing

        // switch(1.23) {
        //  case True: 3.45
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::call_op, op_kind::eq),
        insn_encode(opcode::brf_false, 2),
        insn_encode(opcode::push_const, 7),

        // switch(1.23) {
        //  case False: 3.45
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::call_op, op_kind::eq),
        insn_encode(opcode::brf_false, 2),
        insn_encode(opcode::push_const, 7),

        // switch(1.23) {
        //  case False: 3.45
        //  case True: 3.47
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::brf_false, 3),
        insn_encode(opcode::push_const, 9),
        insn_encode(opcode::brf, 2),
        insn_encode(opcode::push_const, 7),

        // switch(1.23) {
        //  case True: 3.45
        //  case False: 3.47
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::brf_false, 3),
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::brf, 2),
        insn_encode(opcode::push_const, 9),

        insn_encode(opcode::drop_frame, 1)
    }));
}

TEST(Compiler, SwitchBlockWithDefaults)
{
    ast::class_ class_;
    class_.name = "Object";

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { empty_value }, { } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { empty_value }, { 3.45 } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { false }, { 3.45 } },
        ast::case_ { { empty_value }, { } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { false }, { 3.45 } },
        ast::case_ { { empty_value }, { 3.47 } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { true }, { 3.45 } },
        ast::case_ { { empty_value }, { 3.47 } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { true }, { 3.45 } },
        ast::case_ { { false }, { "one"s } },
        ast::case_ { { empty_value }, { 3.47 } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { "one"s }, { 3.45 } },
        ast::case_ { { empty_value }, { 3.47 } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { "one"s }, { 3.45 } },
        ast::case_ { { "two"s }, { 3.47 } },
        ast::case_ { { empty_value }, { 1.23 } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { 3.45, 3.47 }, { 3.45, 3.47 } },
        ast::case_ { { empty_value }, { 3.47, 3.45 } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { 3.45 }, { 3.45, 3.47 } },
        ast::case_ { { empty_value, 3.47 }, { 3.47, 3.45 } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { "one"s, "two"s }, { 3.45, 3.47 } },
        ast::case_ { { empty_value }, { 3.47, 3.45 } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { "one"s }, { 3.45, 3.47 } },
        ast::case_ { { empty_value, "two"s }, { 3.47, 3.45 } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { true }, { 3.45, 3.47 } },
        ast::case_ { { empty_value, false }, { 3.47, 3.45 } }
    }});

    auto module = std::make_shared<semantic::module>();
    compiler::symbol_table syms;
    semantic::graph_type graph;
    std::vector<value_type> const_pool;
    compiler::codegen c("test", module, syms, graph, const_pool);
    c(class_);
    auto res = c.get_result();

    ASSERT_THAT(const_pool, SizeIs(12));
    EXPECT_THAT(boost::get<std::string>(const_pool[1]), Eq("test"));
    EXPECT_THAT(boost::get<std::string>(const_pool[2]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(const_pool[3]), Eq(""));
    EXPECT_THAT(boost::get<std::string>(const_pool[4]), Eq("~init"));
    EXPECT_THAT(boost::get<double>(const_pool[5]),      Eq(1.23));
    EXPECT_THAT(boost::get<double>(const_pool[6]),      Eq(3.45));
    EXPECT_THAT(boost::get<bool>(const_pool[7]),        Eq(false));
    EXPECT_THAT(boost::get<double>(const_pool[8]),      Eq(3.47));
    EXPECT_THAT(boost::get<bool>(const_pool[9]),        Eq(true));
    EXPECT_THAT(boost::get<std::string>(const_pool[10]), Eq("one"));
    EXPECT_THAT(boost::get<std::string>(const_pool[11]), Eq("two"));

    EXPECT_THAT(module->name_index, Eq(1));
    ASSERT_THAT(module->classes, SizeIs(1));
    EXPECT_THAT(module->classes.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->nr_args, Eq(0));
    EXPECT_THAT(module->classes.back()->name_index, Eq(2));
    EXPECT_THAT(module->classes.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->code_range.second, Eq(92));
    EXPECT_THAT(module->classes.back()->base_name_index, Eq(3));
    EXPECT_THAT(module->classes.back()->fields_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->methods_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->fields, IsEmpty());
    ASSERT_THAT(module->classes.back()->methods, SizeIs(1));

    EXPECT_THAT(module->classes.back()->methods.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_args, Eq(1));
    EXPECT_THAT(module->classes.back()->methods.back()->name_index, Eq(4));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.second, Eq(92));

    ASSERT_THAT(res.insns, SizeIs(92));
    EXPECT_THAT(res.insns, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),

        // switch(1.23) {
        //  default:
        // }
        // generate nothing

        // switch(1.23) {
        //  default: 3.45
        // }
        insn_encode(opcode::push_const, 6),

        // switch(1.23) {
        //  case False: 3.45
        //  default:
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::call_op, op_kind::eq),
        insn_encode(opcode::brf_false, 2),
        insn_encode(opcode::push_const, 6),

        // switch(1.23) {
        //  case False: 3.45
        //  default: 3.47
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::brf_true, 3),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::brf, 2),
        insn_encode(opcode::push_const, 8),

        // switch(1.23) {
        //  case True: 3.45
        //  default: 3.47
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::brf_false, 3),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::brf, 2),
        insn_encode(opcode::push_const, 8),

        // switch(1.23) {
        //  case True: 3.45
        //  case False: "one"
        //  default: 3.47
        // }
        // default block is not generated
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::brf_false, 3),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::brf, 2),
        insn_encode(opcode::push_const, 10),

        // switch(1.23) {
        //  case "one": 3.45
        //  default: 3.47
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::push, 3),
        insn_encode(opcode::push_const, 10),
        insn_encode(opcode::br_table, 1),
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::brf, 2),
        insn_encode(opcode::push_const, 6),

        // switch(1.23) {
        //  case "one": 3.45
        //  case "two": 3.47
        //  default: 1.23
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::push, 5),
        insn_encode(opcode::push_const, 11),
        insn_encode(opcode::push, 3),
        insn_encode(opcode::push_const, 10),
        insn_encode(opcode::br_table, 2),
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::brf, 4),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::brf, 2),
        insn_encode(opcode::push_const, 8),

        // switch(1.23) {
        //  case 3.45: case 3.47: 3.45, 3.47
        //  default: 3.47, 3.45
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::push, 4),
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::push, 4),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::br_table, 2),
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::brf, 3),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::push_const, 8),

        // switch(1.23) {
        //  case 3.45: 3.45, 3.47
        //  default: case 3.47: 3.47, 3.45
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::push, 1),
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::push, 4),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::br_table, 2),
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::brf, 3),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::push_const, 8),

        // switch(1.23) {
        //  case "one": case "two": 3.45, 3.47
        //  default: 3.47, 3.45
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::push, 4),
        insn_encode(opcode::push_const, 11),
        insn_encode(opcode::push, 4),
        insn_encode(opcode::push_const, 10),
        insn_encode(opcode::br_table, 2),
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::brf, 3),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::push_const, 8),

        // switch(1.23) {
        //  case "one": 3.45, 3.47
        //  default: case "two": 3.47, 3.45
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::push, 1),
        insn_encode(opcode::push_const, 11),
        insn_encode(opcode::push, 4),
        insn_encode(opcode::push_const, 10),
        insn_encode(opcode::br_table, 2),
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::brf, 3),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::push_const, 8),

        // switch(1.23) {
        //  case True: 3.45, 3.47
        //  default: case False: 3.47, 3.45
        // }
        // default block is not generated
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::brf_false, 4),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::brf, 3),
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::push_const, 6),

        insn_encode(opcode::drop_frame, 1)
    }));
}

TEST(Compiler, SwitchBlockWithMixedCases)
{
    ast::class_ class_;
    class_.name = "Object";

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { 2.34 }, { 3.45 } },
        ast::case_ { { "one"s }, { 3.47 } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { 2.34 }, { 3.45 } },
        ast::case_ { { "one"s }, { 3.47 } },
        ast::case_ { { empty_value }, { 2.34 } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { 2.34 }, { 3.45 } },
        ast::case_ { { true }, { 3.47 } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { 2.34 }, { 3.45 } },
        ast::case_ { { true }, { 3.47 } },
        ast::case_ { { empty_value }, { 2.34 } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { "one"s }, { 3.45 } },
        ast::case_ { { true }, { 3.47 } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { "one"s }, { 3.45 } },
        ast::case_ { { true }, { 3.47 } },
        ast::case_ { { empty_value }, { 2.34 } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { 2.34 }, { 3.45 } },
        ast::case_ { { "one"s }, { 2.34 } },
        ast::case_ { { true }, { 3.47 } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { 2.34 }, { 3.45 } },
        ast::case_ { { "one"s }, { 2.34 } },
        ast::case_ { { true }, { 3.47 } },
        ast::case_ { { empty_value }, { 2.34 } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { 2.34, "one"s }, { 3.45 } },
        ast::case_ { { ""s, true }, { 3.47 } },
        ast::case_ { { empty_value }, { 2.34 } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { "one"s, true }, { 3.45 } },
        ast::case_ { { false, 2.34 }, { 3.47 } },
        ast::case_ { { empty_value }, { 2.34 } }
    }});

    class_.exprs.emplace_back(ast::switch_ { 1.23, {
        ast::case_ { { "one"s, false, 2.34 }, { 3.45 } },
        ast::case_ { { empty_value }, { 3.47 } }
    }});

    auto module = std::make_shared<semantic::module>();
    compiler::symbol_table syms;
    semantic::graph_type graph;
    std::vector<value_type> const_pool;
    compiler::codegen c("test", module, syms, graph, const_pool);
    c(class_);
    auto res = c.get_result();

    ASSERT_THAT(const_pool, SizeIs(12));
    EXPECT_THAT(boost::get<std::string>(const_pool[1]), Eq("test"));
    EXPECT_THAT(boost::get<std::string>(const_pool[2]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(const_pool[3]), Eq(""));
    EXPECT_THAT(boost::get<std::string>(const_pool[4]), Eq("~init"));
    EXPECT_THAT(boost::get<double>(const_pool[5]),      Eq(1.23));
    EXPECT_THAT(boost::get<double>(const_pool[6]),      Eq(2.34));
    EXPECT_THAT(boost::get<std::string>(const_pool[7]), Eq("one"));
    EXPECT_THAT(boost::get<double>(const_pool[8]),      Eq(3.45));
    EXPECT_THAT(boost::get<double>(const_pool[9]),      Eq(3.47));
    EXPECT_THAT(boost::get<bool>(const_pool[10]),       Eq(true));
    EXPECT_THAT(boost::get<bool>(const_pool[11]),       Eq(false));

    EXPECT_THAT(module->name_index, Eq(1));
    ASSERT_THAT(module->classes, SizeIs(1));
    EXPECT_THAT(module->classes.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->nr_args, Eq(0));
    EXPECT_THAT(module->classes.back()->name_index, Eq(2));
    EXPECT_THAT(module->classes.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->code_range.second, Eq(137));
    EXPECT_THAT(module->classes.back()->base_name_index, Eq(3));
    EXPECT_THAT(module->classes.back()->fields_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->methods_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->fields, IsEmpty());
    ASSERT_THAT(module->classes.back()->methods, SizeIs(1));

    EXPECT_THAT(module->classes.back()->methods.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_args, Eq(1));
    EXPECT_THAT(module->classes.back()->methods.back()->name_index, Eq(4));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.second, Eq(137));

    ASSERT_THAT(res.insns, SizeIs(137));
    EXPECT_THAT(res.insns, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),

        // switch(1.23) {
        //  case 2.34: 3.45
        //  case "one": 3.47
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::dup, 1),
        insn_encode(opcode::push, 5),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::br_table, 1),
        insn_encode(opcode::push, 4),
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::br_table, 1),
        insn_encode(opcode::brf, 4),
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::brf, 2),
        insn_encode(opcode::push_const, 9),

        // switch(1.23) {
        //  case 2.34: 3.45
        //  case "one": 3.47
        //  default: 2.34
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::dup, 1),
        insn_encode(opcode::push, 6),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::br_table, 1),
        insn_encode(opcode::push, 5),
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::br_table, 1),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::brf, 4),
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::brf, 2),
        insn_encode(opcode::push_const, 9),

        // switch(1.23) {
        //  case 2.34: 3.45
        //  case True: 3.47
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::dup, 1),
        insn_encode(opcode::push, 4),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::br_table, 1),
        insn_encode(opcode::brf_false, 2),
        insn_encode(opcode::push_const, 9),
        insn_encode(opcode::brf, 2),
        insn_encode(opcode::push_const, 8),

        // switch(1.23) {
        //  case 2.34: 3.45
        //  case True: 3.47
        //  default: 2.34
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::dup, 1),
        insn_encode(opcode::push, 6),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::br_table, 1),
        insn_encode(opcode::brf_false, 3),
        insn_encode(opcode::push_const, 9),
        insn_encode(opcode::brf, 4),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::brf, 2),
        insn_encode(opcode::push_const, 8),

        // switch(1.23) {
        //  case "one": 3.45
        //  case True: 3.47
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::dup, 1),
        insn_encode(opcode::push, 4),
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::br_table, 1),
        insn_encode(opcode::brf_false, 2),
        insn_encode(opcode::push_const, 9),
        insn_encode(opcode::brf, 2),
        insn_encode(opcode::push_const, 8),

        // switch(1.23) {
        //  case "one": 3.45
        //  case True: 3.47
        //  default: 2.34
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::dup, 1),
        insn_encode(opcode::push, 6),
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::br_table, 1),
        insn_encode(opcode::brf_false, 3),
        insn_encode(opcode::push_const, 9),
        insn_encode(opcode::brf, 4),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::brf, 2),
        insn_encode(opcode::push_const, 8),

        // switch(1.23) {
        //  case 2.34: 3.45
        //  case "one": 2.34
        //  case True: 3.47
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::dup, 2),
        insn_encode(opcode::push, 7),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::br_table, 1),
        insn_encode(opcode::push, 6),
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::br_table, 1),
        insn_encode(opcode::brf_false, 2),
        insn_encode(opcode::push_const, 9),
        insn_encode(opcode::brf, 4),
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::brf, 2),
        insn_encode(opcode::push_const, 6),

        // switch(1.23) {
        //  case 2.34: 3.45
        //  case "one": 2.34
        //  case True: 3.47
        //  default: 2.34
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::dup, 2),
        insn_encode(opcode::push, 9),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::br_table, 1),
        insn_encode(opcode::push, 8),
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::br_table, 1),
        insn_encode(opcode::brf_false, 3),
        insn_encode(opcode::push_const, 9),
        insn_encode(opcode::brf, 6),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::brf, 4),
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::brf, 2),
        insn_encode(opcode::push_const, 6),

        // switch(1.23) {
        //  case 2.34: case "one": 3.45
        //  case "": case True: 3.47
        //  default: 2.34
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::dup, 2),
        insn_encode(opcode::push, 11),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::br_table, 1),
        insn_encode(opcode::push, 6),
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::push, 2),
        insn_encode(opcode::push_const, 3),
        insn_encode(opcode::br_table, 2),
        insn_encode(opcode::brf_false, 3),
        insn_encode(opcode::push_const, 9),
        insn_encode(opcode::brf, 4),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::brf, 2),
        insn_encode(opcode::push_const, 8),

        // switch(1.23) {
        //  case "one": case True: 3.45
        //  case False: case 2.34: 3.47
        //  default: 2.34
        // }
        // default block is not generated
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::dup, 2),
        insn_encode(opcode::push, 7),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::br_table, 1),
        insn_encode(opcode::push, 2),
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::br_table, 1),
        insn_encode(opcode::brf_false, 3),
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::brf, 2),
        insn_encode(opcode::push_const, 9),

        // switch(1.23) {
        //  case "one": case False: case 2.34: 3.45
        //  default: 3.47
        // }
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::dup, 2),
        insn_encode(opcode::push, 5),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::br_table, 1),
        insn_encode(opcode::push, 2),
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::br_table, 1),
        insn_encode(opcode::brf_true, 3),
        insn_encode(opcode::push_const, 8),
        insn_encode(opcode::brf, 2),
        insn_encode(opcode::push_const, 9),

        insn_encode(opcode::drop_frame, 1)
    }));
}

// TODO Call
// TODO TryBlock
// TODO Branches
// TODO MethodDef
// TODO Assigns
// TODO Variables
// TODO CallVariableMethod
// TODO BaseMethodNotFound
