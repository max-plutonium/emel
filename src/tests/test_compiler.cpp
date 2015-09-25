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

#include "../emel/compiler.h"

using namespace emel;

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

    auto ret = compiler::topological_sort(classes);
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
    symbol_table syms;
    semantic::graph_type graph;
    std::vector<value_type> const_pool;
    codegen c("test", module, syms, graph, const_pool);
    codegen_result cr = c(class_);

    ASSERT_THAT(const_pool, SizeIs(4));
    EXPECT_THAT(boost::get<std::string>(const_pool[1]), Eq("test"));
    EXPECT_THAT(boost::get<std::string>(const_pool[2]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(const_pool[3]), Eq("~init"));

    EXPECT_THAT(module->name_index, Eq(1));
    ASSERT_THAT(module->classes, SizeIs(1));
    EXPECT_THAT(module->classes.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->insns, IsEmpty());
    EXPECT_THAT(module->classes.back()->slots, IsEmpty());
    EXPECT_THAT(module->classes.back()->nr_stack, Eq(0));
    EXPECT_THAT(module->classes.back()->nr_args, Eq(0));
    EXPECT_THAT(module->classes.back()->name_index, Eq(2));
    EXPECT_THAT(module->classes.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->code_range.second, Eq(2));
    EXPECT_THAT(module->classes.back()->base_name_index, Eq(0));
    EXPECT_THAT(module->classes.back()->fields_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->methods_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->fields, IsEmpty());
    ASSERT_THAT(module->classes.back()->methods, SizeIs(1));

    EXPECT_THAT(module->classes.back()->methods.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->insns, IsEmpty());
    EXPECT_THAT(module->classes.back()->methods.back()->slots, SizeIs(1));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_stack, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_args, Eq(1));
    EXPECT_THAT(module->classes.back()->methods.back()->name_index, Eq(3));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.second, Eq(2));

    ASSERT_THAT(module->insns, SizeIs(2));
    EXPECT_THAT(module->insns, ElementsAreArray({
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
        symbol_table syms;
        semantic::graph_type graph;
        std::vector<value_type> const_pool;
        codegen c("test", module, syms, graph, const_pool);

        ASSERT_THROW(c(class_), std::runtime_error);
    }

    ast::class_ obj_class_;
    obj_class_.name = "Object";

    auto module = std::make_shared<semantic::module>();
    symbol_table syms;
    semantic::graph_type graph;
    std::vector<value_type> const_pool;
    codegen c("test", module, syms, graph, const_pool);
    c(obj_class_);
    codegen_result cr = c(class_);

    ASSERT_THAT(const_pool, SizeIs(5));
    EXPECT_THAT(boost::get<std::string>(const_pool[1]), Eq("test"));
    EXPECT_THAT(boost::get<std::string>(const_pool[2]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(const_pool[3]), Eq("~init"));
    EXPECT_THAT(boost::get<std::string>(const_pool[4]), Eq("Name"));

    EXPECT_THAT(module->name_index, Eq(1));
    ASSERT_THAT(module->classes, SizeIs(2));
    EXPECT_THAT(module->classes.back()->index, Eq(1));
    EXPECT_THAT(module->classes.back()->insns, IsEmpty());
    EXPECT_THAT(module->classes.back()->slots, IsEmpty());
    EXPECT_THAT(module->classes.back()->nr_stack, Eq(0));
    EXPECT_THAT(module->classes.back()->nr_args, Eq(0));
    EXPECT_THAT(module->classes.back()->name_index, Eq(4));
    EXPECT_THAT(module->classes.back()->code_range.first, Eq(2));
    EXPECT_THAT(module->classes.back()->code_range.second, Eq(6));
    EXPECT_THAT(module->classes.back()->base_name_index, Eq(2));
    EXPECT_THAT(module->classes.back()->fields_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->methods_offset, Eq(1));
    EXPECT_THAT(module->classes.back()->fields, IsEmpty());
    ASSERT_THAT(module->classes.back()->methods, SizeIs(1));

    EXPECT_THAT(module->classes.back()->methods.back()->index, Eq(1));
    EXPECT_THAT(module->classes.back()->methods.back()->insns, IsEmpty());
    EXPECT_THAT(module->classes.back()->methods.back()->slots, SizeIs(1));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_stack, Eq(1));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_args, Eq(1));
    EXPECT_THAT(module->classes.back()->methods.back()->name_index, Eq(3));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.first, Eq(2));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.second, Eq(6));

    ASSERT_THAT(module->insns, SizeIs(6));
    EXPECT_THAT(module->insns, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),
        insn_encode(opcode::drop_frame, 1),
        insn_encode(opcode::push_frame, 1),
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
    symbol_table syms;
    semantic::graph_type graph;
    std::vector<value_type> const_pool;
    codegen c("test", module, syms, graph, const_pool);
    c(obj_class_);
    c(base_class_);
    codegen_result cr = c(class_);

    ASSERT_THAT(const_pool, SizeIs(6));
    EXPECT_THAT(boost::get<std::string>(const_pool[1]), Eq("test"));
    EXPECT_THAT(boost::get<std::string>(const_pool[2]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(const_pool[3]), Eq("~init"));
    EXPECT_THAT(boost::get<std::string>(const_pool[4]), Eq("BaseName"));
    EXPECT_THAT(boost::get<std::string>(const_pool[5]), Eq("Name"));

    EXPECT_THAT(module->name_index, Eq(1));
    ASSERT_THAT(module->classes, SizeIs(3));
    EXPECT_THAT(module->classes.back()->index, Eq(2));
    EXPECT_THAT(module->classes.back()->insns, IsEmpty());
    EXPECT_THAT(module->classes.back()->slots, IsEmpty());
    EXPECT_THAT(module->classes.back()->nr_stack, Eq(0));
    EXPECT_THAT(module->classes.back()->nr_args, Eq(0));
    EXPECT_THAT(module->classes.back()->name_index, Eq(5));
    EXPECT_THAT(module->classes.back()->code_range.first, Eq(6));
    EXPECT_THAT(module->classes.back()->code_range.second, Eq(10));
    EXPECT_THAT(module->classes.back()->base_name_index, Eq(4));
    EXPECT_THAT(module->classes.back()->fields_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->methods_offset, Eq(2));
    EXPECT_THAT(module->classes.back()->fields, IsEmpty());
    ASSERT_THAT(module->classes.back()->methods, SizeIs(1));

    EXPECT_THAT(module->classes.back()->methods.back()->index, Eq(2));
    EXPECT_THAT(module->classes.back()->methods.back()->insns, IsEmpty());
    EXPECT_THAT(module->classes.back()->methods.back()->slots, SizeIs(1));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_stack, Eq(1));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_args, Eq(1));
    EXPECT_THAT(module->classes.back()->methods.back()->name_index, Eq(3));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.first, Eq(6));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.second, Eq(10));

    ASSERT_THAT(module->insns, SizeIs(10));
    EXPECT_THAT(module->insns, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),
        insn_encode(opcode::drop_frame, 1),
        insn_encode(opcode::push_frame, 1),
        insn_encode(opcode::push_local, 0),
        insn_encode(opcode::fcall, 0),
        insn_encode(opcode::drop_frame, 1),
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
    class_.exprs.emplace_back(std::string("hello world!"));
    class_.exprs.emplace_back(12.3);
    class_.exprs.emplace_back(true);
    class_.exprs.emplace_back(false);

    auto module = std::make_shared<semantic::module>();
    symbol_table syms;
    semantic::graph_type graph;
    std::vector<value_type> const_pool;
    codegen c("test", module, syms, graph, const_pool);
    codegen_result cr = c(class_);

    ASSERT_THAT(const_pool, SizeIs(7));
    EXPECT_THAT(boost::get<std::string>(const_pool[1]), Eq("test"));
    EXPECT_THAT(boost::get<std::string>(const_pool[2]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(const_pool[3]), Eq("~init"));
    EXPECT_THAT(boost::get<std::string>(const_pool[4]), Eq("hello world!"));
    EXPECT_THAT(boost::get<double>(const_pool[5]),      Eq(12.3));
    EXPECT_THAT(boost::get<double>(const_pool[6]),      Eq(1));

    EXPECT_THAT(module->name_index, Eq(1));
    ASSERT_THAT(module->classes, SizeIs(1));
    EXPECT_THAT(module->classes.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->insns, IsEmpty());
    EXPECT_THAT(module->classes.back()->slots, IsEmpty());
    EXPECT_THAT(module->classes.back()->nr_stack, Eq(0));
    EXPECT_THAT(module->classes.back()->nr_args, Eq(0));
    EXPECT_THAT(module->classes.back()->name_index, Eq(2));
    EXPECT_THAT(module->classes.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->code_range.second, Eq(6));
    EXPECT_THAT(module->classes.back()->base_name_index, Eq(0));
    EXPECT_THAT(module->classes.back()->fields_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->methods_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->fields, IsEmpty());
    ASSERT_THAT(module->classes.back()->methods, SizeIs(1));

    EXPECT_THAT(module->classes.back()->methods.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->insns, IsEmpty());
    EXPECT_THAT(module->classes.back()->methods.back()->slots, SizeIs(1));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_stack, Eq(4));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_args, Eq(1));
    EXPECT_THAT(module->classes.back()->methods.back()->name_index, Eq(3));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.second, Eq(6));

    ASSERT_THAT(module->insns, SizeIs(6));
    EXPECT_THAT(module->insns, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),
        insn_encode(opcode::push_const, 4),
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::push_const, 0),
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
    symbol_table syms;
    semantic::graph_type graph;
    std::vector<value_type> const_pool;
    codegen c("test", module, syms, graph, const_pool);
    codegen_result cr = c(class_);

    ASSERT_THAT(const_pool, SizeIs(9));
    EXPECT_THAT(boost::get<std::string>(const_pool[1]), Eq("test"));
    EXPECT_THAT(boost::get<std::string>(const_pool[2]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(const_pool[3]), Eq("~init"));
    EXPECT_THAT(boost::get<std::string>(const_pool[4]), Eq("VarName"));
    EXPECT_THAT(boost::get<double>(const_pool[5]),      Eq(1.23));
    EXPECT_THAT(boost::get<std::string>(const_pool[6]), Eq("ReusedName"));
    EXPECT_THAT(boost::get<double>(const_pool[7]),      Eq(5.0));
    EXPECT_THAT(boost::get<double>(const_pool[8]),      Eq(15.0));

    EXPECT_THAT(module->name_index, Eq(1));
    ASSERT_THAT(module->classes, SizeIs(1));
    EXPECT_THAT(module->classes.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->insns, IsEmpty());
    EXPECT_THAT(module->classes.back()->slots, IsEmpty());
    EXPECT_THAT(module->classes.back()->nr_stack, Eq(0));
    EXPECT_THAT(module->classes.back()->nr_args, Eq(0));
    EXPECT_THAT(module->classes.back()->name_index, Eq(2));
    EXPECT_THAT(module->classes.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->code_range.second, Eq(11));
    EXPECT_THAT(module->classes.back()->base_name_index, Eq(0));
    EXPECT_THAT(module->classes.back()->fields_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->methods_offset, Eq(0));
    ASSERT_THAT(module->classes.back()->fields, SizeIs(2));
    ASSERT_THAT(module->classes.back()->methods, SizeIs(1));

    auto field = module->classes.back()->fields[0];
    EXPECT_THAT(field->index, Eq(0));
    EXPECT_THAT(field->insns, IsEmpty());
    EXPECT_THAT(field->slots, IsEmpty());
    EXPECT_THAT(field->nr_stack, Eq(0));
    EXPECT_THAT(field->nr_args, Eq(0));
    EXPECT_THAT(field->name_index, Eq(4));

    field = module->classes.back()->fields[1];
    EXPECT_THAT(field->index, Eq(1));
    EXPECT_THAT(field->insns, IsEmpty());
    EXPECT_THAT(field->slots, IsEmpty());
    EXPECT_THAT(field->nr_stack, Eq(0));
    EXPECT_THAT(field->nr_args, Eq(0));
    EXPECT_THAT(field->name_index, Eq(6));

    EXPECT_THAT(module->classes.back()->methods.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->insns, IsEmpty());
    EXPECT_THAT(module->classes.back()->methods.back()->slots, SizeIs(1));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_stack, Eq(6));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_args, Eq(1));
    EXPECT_THAT(module->classes.back()->methods.back()->name_index, Eq(3));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.second, Eq(11));

    ASSERT_THAT(module->insns, SizeIs(11));
    EXPECT_THAT(module->insns, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::push_local, 0),
        insn_encode(opcode::load_field, 0),
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::push_local, 0),
        insn_encode(opcode::load_field, 1),
        insn_encode(opcode::push_const, 8),
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
    symbol_table syms;
    semantic::graph_type graph;
    std::vector<value_type> const_pool;
    codegen c("test", module, syms, graph, const_pool);
    codegen_result cr = c(class_);

    ASSERT_THAT(const_pool, SizeIs(5));
    EXPECT_THAT(boost::get<std::string>(const_pool[1]), Eq("test"));
    EXPECT_THAT(boost::get<std::string>(const_pool[2]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(const_pool[3]), Eq("~init"));
    EXPECT_THAT(boost::get<double>(const_pool[4]),      Eq(1.23));

    EXPECT_THAT(module->name_index, Eq(1));
    ASSERT_THAT(module->classes, SizeIs(1));
    EXPECT_THAT(module->classes.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->insns, IsEmpty());
    EXPECT_THAT(module->classes.back()->slots, IsEmpty());
    EXPECT_THAT(module->classes.back()->nr_stack, Eq(0));
    EXPECT_THAT(module->classes.back()->nr_args, Eq(0));
    EXPECT_THAT(module->classes.back()->name_index, Eq(2));
    EXPECT_THAT(module->classes.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->code_range.second, Eq(6));
    EXPECT_THAT(module->classes.back()->base_name_index, Eq(0));
    EXPECT_THAT(module->classes.back()->fields_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->methods_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->fields, IsEmpty());
    ASSERT_THAT(module->classes.back()->methods, SizeIs(1));

    EXPECT_THAT(module->classes.back()->methods.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->insns, IsEmpty());
    EXPECT_THAT(module->classes.back()->methods.back()->slots, SizeIs(1));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_stack, Eq(2));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_args, Eq(1));
    EXPECT_THAT(module->classes.back()->methods.back()->name_index, Eq(3));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.second, Eq(6));

    ASSERT_THAT(module->insns, SizeIs(6));
    EXPECT_THAT(module->insns, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),
        insn_encode(opcode::push_const, 4),
        insn_encode(opcode::call_op, static_cast<std::uint32_t>(op_kind::not_)),
        insn_encode(opcode::push_const, 4),
        insn_encode(opcode::call_op, static_cast<std::uint32_t>(op_kind::neg)),
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
    symbol_table syms;
    semantic::graph_type graph;
    std::vector<value_type> const_pool;
    codegen c("test", module, syms, graph, const_pool);
    codegen_result cr = c(class_);

    ASSERT_THAT(const_pool, SizeIs(7));
    EXPECT_THAT(boost::get<std::string>(const_pool[1]), Eq("test"));
    EXPECT_THAT(boost::get<std::string>(const_pool[2]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(const_pool[3]), Eq("~init"));
    EXPECT_THAT(boost::get<double>(const_pool[4]),      Eq(-20.3));
    EXPECT_THAT(boost::get<double>(const_pool[5]),      Eq(1.23));
    EXPECT_THAT(boost::get<double>(const_pool[6]),      Eq(2.0));

    EXPECT_THAT(module->name_index, Eq(1));
    ASSERT_THAT(module->classes, SizeIs(1));
    EXPECT_THAT(module->classes.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->insns, IsEmpty());
    EXPECT_THAT(module->classes.back()->slots, IsEmpty());
    EXPECT_THAT(module->classes.back()->nr_stack, Eq(0));
    EXPECT_THAT(module->classes.back()->nr_args, Eq(0));
    EXPECT_THAT(module->classes.back()->name_index, Eq(2));
    EXPECT_THAT(module->classes.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->code_range.second, Eq(9));
    EXPECT_THAT(module->classes.back()->base_name_index, Eq(0));
    EXPECT_THAT(module->classes.back()->fields_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->methods_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->fields, IsEmpty());
    ASSERT_THAT(module->classes.back()->methods, SizeIs(1));

    EXPECT_THAT(module->classes.back()->methods.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->insns, IsEmpty());
    EXPECT_THAT(module->classes.back()->methods.back()->slots, SizeIs(1));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_stack, Eq(4));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_args, Eq(1));
    EXPECT_THAT(module->classes.back()->methods.back()->name_index, Eq(3));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.second, Eq(9));

    ASSERT_THAT(module->insns, SizeIs(9));
    EXPECT_THAT(module->insns, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),
        insn_encode(opcode::push_const, 4),
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::call_op, static_cast<std::uint32_t>(op_kind::add)),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::call_op, static_cast<std::uint32_t>(op_kind::sub)),
        insn_encode(opcode::call_op, static_cast<std::uint32_t>(op_kind::div)),
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
    symbol_table syms;
    semantic::graph_type graph;
    std::vector<value_type> const_pool;
    codegen c("test", module, syms, graph, const_pool);
    codegen_result cr = c(class_);

    ASSERT_THAT(const_pool, SizeIs(8));
    EXPECT_THAT(boost::get<std::string>(const_pool[1]), Eq("test"));
    EXPECT_THAT(boost::get<std::string>(const_pool[2]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(const_pool[3]), Eq("~init"));
    EXPECT_THAT(boost::get<double>(const_pool[4]),      Eq(1.0));
    EXPECT_THAT(boost::get<double>(const_pool[5]),      Eq(1.23));
    EXPECT_THAT(boost::get<double>(const_pool[6]),      Eq(-20.3));
    EXPECT_THAT(boost::get<double>(const_pool[7]),      Eq(-1.0));

    EXPECT_THAT(module->name_index, Eq(1));
    ASSERT_THAT(module->classes, SizeIs(1));
    EXPECT_THAT(module->classes.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->insns, IsEmpty());
    EXPECT_THAT(module->classes.back()->slots, IsEmpty());
    EXPECT_THAT(module->classes.back()->nr_stack, Eq(0));
    EXPECT_THAT(module->classes.back()->nr_args, Eq(0));
    EXPECT_THAT(module->classes.back()->name_index, Eq(2));
    EXPECT_THAT(module->classes.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->code_range.second, Eq(20));
    EXPECT_THAT(module->classes.back()->base_name_index, Eq(0));
    EXPECT_THAT(module->classes.back()->fields_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->methods_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->fields, IsEmpty());
    ASSERT_THAT(module->classes.back()->methods, SizeIs(1));

    EXPECT_THAT(module->classes.back()->methods.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->insns, IsEmpty());
    EXPECT_THAT(module->classes.back()->methods.back()->slots, SizeIs(1));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_stack, Eq(10));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_args, Eq(1));
    EXPECT_THAT(module->classes.back()->methods.back()->name_index, Eq(3));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.second, Eq(20));

    ASSERT_THAT(module->insns, SizeIs(20));
    EXPECT_THAT(module->insns, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),
        insn_encode(opcode::push_const, 4),
        insn_encode(opcode::brf_false, 3),
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::brf, 2),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::push_const, 0),
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::call_op, static_cast<std::uint32_t>(op_kind::gte)),
        insn_encode(opcode::brf_false, 5),
        insn_encode(opcode::push_const, 4),
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::call_op, static_cast<std::uint32_t>(op_kind::mul)),
        insn_encode(opcode::brf, 6),
        insn_encode(opcode::push_const, 0),
        insn_encode(opcode::brf_false, 3),
        insn_encode(opcode::push_const, 4),
        insn_encode(opcode::brf, 2),
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::drop_frame, 1)
    }));
}

// TODO Call
// TODO TryBlock
// TODO SwitchBlock

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
    symbol_table syms;
    semantic::graph_type graph;
    std::vector<value_type> const_pool;
    codegen c("test", module, syms, graph, const_pool);
    codegen_result cr = c(class_);

    ASSERT_THAT(const_pool, SizeIs(7));
    EXPECT_THAT(boost::get<std::string>(const_pool[1]), Eq("test"));
    EXPECT_THAT(boost::get<std::string>(const_pool[2]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(const_pool[3]), Eq("~init"));
    EXPECT_THAT(boost::get<double>(const_pool[4]),      Eq(1.0));
    EXPECT_THAT(boost::get<double>(const_pool[5]),      Eq(-1.23));
    EXPECT_THAT(boost::get<double>(const_pool[6]),      Eq(456.0));

    EXPECT_THAT(module->name_index, Eq(1));
    ASSERT_THAT(module->classes, SizeIs(1));
    EXPECT_THAT(module->classes.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->insns, IsEmpty());
    EXPECT_THAT(module->classes.back()->slots, IsEmpty());
    EXPECT_THAT(module->classes.back()->nr_stack, Eq(0));
    EXPECT_THAT(module->classes.back()->nr_args, Eq(0));
    EXPECT_THAT(module->classes.back()->name_index, Eq(2));
    EXPECT_THAT(module->classes.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->code_range.second, Eq(10));
    EXPECT_THAT(module->classes.back()->base_name_index, Eq(0));
    EXPECT_THAT(module->classes.back()->fields_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->methods_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->fields, IsEmpty());
    ASSERT_THAT(module->classes.back()->methods, SizeIs(1));

    EXPECT_THAT(module->classes.back()->methods.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->insns, IsEmpty());
    EXPECT_THAT(module->classes.back()->methods.back()->slots, SizeIs(1));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_stack, Eq(5));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_args, Eq(1));
    EXPECT_THAT(module->classes.back()->methods.back()->name_index, Eq(3));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.second, Eq(10));

    ASSERT_THAT(module->insns, SizeIs(10));
    EXPECT_THAT(module->insns, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),
        insn_encode(opcode::push_const, 4),
        insn_encode(opcode::brf_false, 2),
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::brf_false, 3),
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::brf, 2),
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::drop_frame, 1)
    }));
}

// TODO ForLoop

TEST(Compiler, WhileLoop)
{
    ast::class_ class_;
    class_.name = "Object";
    class_.exprs.emplace_back(ast::while_ {
        false, { -1.23 }
    });
    class_.exprs.emplace_back(ast::while_ {
        true, { -1.23 }
    });

    auto module = std::make_shared<semantic::module>();
    symbol_table syms;
    semantic::graph_type graph;
    std::vector<value_type> const_pool;
    codegen c("test", module, syms, graph, const_pool);
    codegen_result cr = c(class_);

    ASSERT_THAT(const_pool, SizeIs(5));
    EXPECT_THAT(boost::get<std::string>(const_pool[1]), Eq("test"));
    EXPECT_THAT(boost::get<std::string>(const_pool[2]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(const_pool[3]), Eq("~init"));
    EXPECT_THAT(boost::get<double>(const_pool[4]),      Eq(-1.23));

    EXPECT_THAT(module->name_index, Eq(1));
    ASSERT_THAT(module->classes, SizeIs(1));
    EXPECT_THAT(module->classes.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->insns, IsEmpty());
    EXPECT_THAT(module->classes.back()->slots, IsEmpty());
    EXPECT_THAT(module->classes.back()->nr_stack, Eq(0));
    EXPECT_THAT(module->classes.back()->nr_args, Eq(0));
    EXPECT_THAT(module->classes.back()->name_index, Eq(2));
    EXPECT_THAT(module->classes.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->code_range.second, Eq(8));
    EXPECT_THAT(module->classes.back()->base_name_index, Eq(0));
    EXPECT_THAT(module->classes.back()->fields_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->methods_offset, Eq(0));
    EXPECT_THAT(module->classes.back()->fields, IsEmpty());
    ASSERT_THAT(module->classes.back()->methods, SizeIs(1));

    EXPECT_THAT(module->classes.back()->methods.back()->index, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->insns, IsEmpty());
    EXPECT_THAT(module->classes.back()->methods.back()->slots, SizeIs(1));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_stack, Eq(3));
    EXPECT_THAT(module->classes.back()->methods.back()->nr_args, Eq(1));
    EXPECT_THAT(module->classes.back()->methods.back()->name_index, Eq(3));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.first, Eq(0));
    EXPECT_THAT(module->classes.back()->methods.back()->code_range.second, Eq(8));

    ASSERT_THAT(module->insns, SizeIs(8));
    EXPECT_THAT(module->insns, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),
        insn_encode(opcode::push_const, 0),
        insn_encode(opcode::brf_false, 3),
        insn_encode(opcode::push_const, 4),
        insn_encode(opcode::brb, 3),
        insn_encode(opcode::push_const, 4),
        insn_encode(opcode::brb, 1),
        insn_encode(opcode::drop_frame, 1)
    }));
}

// TODO Branches
// TODO MethodDef
// TODO Assigns
// TODO Variables
// TODO CallVariableMethod
// TODO BaseMethodNotFound
