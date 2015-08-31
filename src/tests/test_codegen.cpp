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

#include "../emel/bytecode/codegen.h"

#include <stack>
#include <cassert>

using namespace emel;

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>

using boost::vecS;
using boost::directedS;
using boost::property;

using graph_type = boost::adjacency_list<vecS, vecS, directedS,
    property<boost::vertex_name_t, std::string>>;

/*    std::vector<ast::class_> c_vector;
//    ast::class_ cl;
//    cl.name = "Object";
//    c_vector.push_back(cl);
//    cl.name = "Name";
//    c_vector.push_back(cl);
//    cl.name = "Name2";
//    cl.base_name = "Object";
//    c_vector.push_back(cl);
//    cl.name = "Name3";
//    cl.base_name = "BaseName";
//    c_vector.push_back(cl);
//    cl.name = "BaseName";
//    cl.base_name = "BaseName2";
//    c_vector.push_back(cl);
//    cl.name = "BaseName2";
//    cl.base_name = "";
//    c_vector.push_back(cl);

//    std::unordered_map<std::string,
//        boost::graph_traits<graph_type>::vertex_descriptor> descriptor_map;

//    using class_names_map_type =
//        boost::property_map<graph_type, boost::vertex_name_t>::type;
//    graph_type graph;

//    for(auto &c : c_vector) {
//        if(c.base_name.empty() && c.name != "Object")
//            c.base_name = "Object";
//        auto vertex_descr = boost::add_vertex(c.name, graph);
//        auto res = descriptor_map.emplace(c.name, vertex_descr);
//        assert(res.second); // TODO Ругаться на повторяющиеся имена
//    }

//    for(const auto &c : c_vector)
//        if(!c.base_name.empty())
//            boost::add_edge(descriptor_map[c.name], descriptor_map[c.base_name], graph);

//    class_names_map_type class_names_map = boost::get(boost::vertex_name, graph);

//    boost::graph_traits<graph_type>::vertex_iterator vit, vend;
//    for(std::tie(vit, vend) = boost::vertices(graph); vit != vend; ++vit)
//        std::cerr << *vit << "***" << class_names_map[*vit] << std::endl;

//    std::vector<boost::graph_traits<graph_type>::vertex_descriptor>
//            class_list(boost::num_vertices(graph));

//    boost::topological_sort(graph, std::begin(class_list));

//    for(auto i : class_list)
//        std::cerr << i << "***" << class_names_map[i] << std::endl;*/

using testing::SizeIs;
using testing::Eq;
using testing::IsEmpty;
using testing::ElementsAreArray;

TEST(Codegen, EmptyObjectClass)
{
    ast::class_ class_;
    class_.name = "Object";
    bytecode::symbols syms;
    bytecode::codegen c(syms);
    bytecode::class_info cf = c.generate(class_);

    EXPECT_THAT(cf.const_pool, SizeIs(3));
    EXPECT_THAT(boost::get<std::string>(cf.const_pool[1]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(cf.const_pool[2]), Eq("~init/0"));

    EXPECT_THAT(cf.base_name_index, Eq(0));
    EXPECT_THAT(cf.name_index, Eq(1));
    EXPECT_THAT(cf.vsb, Eq(bytecode::visibility::public_));
    EXPECT_FALSE(cf.is_abstract);
    EXPECT_FALSE(cf.is_interface);

    EXPECT_THAT(cf.fields, IsEmpty());
    EXPECT_THAT(cf.static_fields, IsEmpty());
    EXPECT_THAT(cf.static_methods, IsEmpty());

    EXPECT_THAT(cf.methods, SizeIs(1));
    EXPECT_THAT(cf.methods.front().class_name_index, Eq(1));
    EXPECT_THAT(cf.methods.front().name_index, Eq(2));
    EXPECT_THAT(cf.methods.front().nr_args, Eq(0));
    EXPECT_THAT(cf.methods.front().vsb, Eq(bytecode::visibility::public_));
    EXPECT_FALSE(cf.methods.front().is_native);
    EXPECT_FALSE(cf.methods.front().is_static);
    EXPECT_FALSE(cf.methods.front().is_virtual);
    EXPECT_FALSE(cf.methods.front().is_pure);

    // empty constructor
    EXPECT_THAT(cf.methods.front().code, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),
        insn_encode(opcode::drop_frame, 1)
    }));
}

TEST(Codegen, EmptyClass)
{
    ast::class_ class_;
    class_.name = "Name";
    bytecode::symbols syms;
    syms[bytecode::symbol::method]
        .add_symbol("6Object$7~init/0", bytecode::symbol::method);
    bytecode::codegen c(syms);
    bytecode::class_info cf = c.generate(class_);

    EXPECT_THAT(cf.const_pool, SizeIs(4));
    EXPECT_THAT(boost::get<std::string>(cf.const_pool[1]), Eq("Name"));
    EXPECT_THAT(boost::get<std::string>(cf.const_pool[2]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(cf.const_pool[3]), Eq("~init/0"));

    EXPECT_THAT(cf.base_name_index, Eq(2));
    EXPECT_THAT(cf.name_index, Eq(1));
    EXPECT_THAT(cf.vsb, Eq(bytecode::visibility::public_));
    EXPECT_FALSE(cf.is_abstract);
    EXPECT_FALSE(cf.is_interface);

    EXPECT_THAT(cf.fields, IsEmpty());
    EXPECT_THAT(cf.static_fields, IsEmpty());
    EXPECT_THAT(cf.static_methods, IsEmpty());

    EXPECT_THAT(cf.methods, SizeIs(1));
    EXPECT_THAT(cf.methods.front().class_name_index, Eq(1));
    EXPECT_THAT(cf.methods.front().name_index, Eq(3));
    EXPECT_THAT(cf.methods.front().nr_args, Eq(0));
    EXPECT_THAT(cf.methods.front().vsb, Eq(bytecode::visibility::public_));
    EXPECT_FALSE(cf.methods.front().is_native);
    EXPECT_FALSE(cf.methods.front().is_static);
    EXPECT_FALSE(cf.methods.front().is_virtual);
    EXPECT_FALSE(cf.methods.front().is_pure);

    // empty constructor
    EXPECT_THAT(cf.methods.front().code, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),
        insn_encode(opcode::push_local, 0),
        insn_encode(opcode::fcall, 0),
        insn_encode(opcode::drop_frame, 1)
    }));
}

TEST(Codegen, EmptyClassWithBaseClass)
{
    ast::class_ class_;
    class_.base_name = "BaseName";
    class_.name = "Name";
    bytecode::symbols syms;
    syms[bytecode::symbol::method]
        .add_symbol("8BaseName$7~init/0", bytecode::symbol::method);
    bytecode::codegen c(syms);
    bytecode::class_info cf = c.generate(class_);

    EXPECT_THAT(cf.const_pool, SizeIs(4));
    EXPECT_THAT(boost::get<std::string>(cf.const_pool[1]), Eq("Name"));
    EXPECT_THAT(boost::get<std::string>(cf.const_pool[2]), Eq("BaseName"));
    EXPECT_THAT(boost::get<std::string>(cf.const_pool[3]), Eq("~init/0"));

    EXPECT_THAT(cf.base_name_index, Eq(2));
    EXPECT_THAT(cf.name_index, Eq(1));
    EXPECT_THAT(cf.vsb, Eq(bytecode::visibility::public_));
    EXPECT_FALSE(cf.is_abstract);
    EXPECT_FALSE(cf.is_interface);

    EXPECT_THAT(cf.fields, IsEmpty());
    EXPECT_THAT(cf.static_fields, IsEmpty());
    EXPECT_THAT(cf.static_methods, IsEmpty());

    EXPECT_THAT(cf.methods, SizeIs(1));
    EXPECT_THAT(cf.methods.front().class_name_index, Eq(1));
    EXPECT_THAT(cf.methods.front().name_index, Eq(3));
    EXPECT_THAT(cf.methods.front().nr_args, Eq(0));
    EXPECT_THAT(cf.methods.front().vsb, Eq(bytecode::visibility::public_));
    EXPECT_FALSE(cf.methods.front().is_native);
    EXPECT_FALSE(cf.methods.front().is_static);
    EXPECT_FALSE(cf.methods.front().is_virtual);
    EXPECT_FALSE(cf.methods.front().is_pure);

    // empty constructor
    EXPECT_THAT(cf.methods.front().code, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),
        insn_encode(opcode::push_local, 0),
        insn_encode(opcode::fcall, 0),
        insn_encode(opcode::drop_frame, 1)
    }));
}

TEST(Codegen, Constants)
{
    ast::class_ class_;
    class_.name = "Object";
    class_.exprs.emplace_back(std::string("hello world!"));
    class_.exprs.emplace_back(12.3);
    class_.exprs.emplace_back(true);
    class_.exprs.emplace_back(false);
    bytecode::symbols syms;
    bytecode::codegen c(syms);
    bytecode::class_info cf = c.generate(class_);

    EXPECT_THAT(cf.const_pool, SizeIs(7));
    EXPECT_THAT(boost::get<std::string>(cf.const_pool[1]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(cf.const_pool[2]), Eq("~init/0"));
    EXPECT_THAT(boost::get<std::string>(cf.const_pool[3]), Eq("hello world!"));
    EXPECT_THAT(boost::get<double>(cf.const_pool[4]), Eq(12.3));
    EXPECT_THAT(boost::get<double>(cf.const_pool[5]), Eq(1));
    EXPECT_THAT(boost::get<double>(cf.const_pool[6]), Eq(0));

    EXPECT_THAT(cf.base_name_index, Eq(0));
    EXPECT_THAT(cf.name_index, Eq(1));
    EXPECT_THAT(cf.vsb, Eq(bytecode::visibility::public_));
    EXPECT_FALSE(cf.is_abstract);
    EXPECT_FALSE(cf.is_interface);

    EXPECT_THAT(cf.fields, IsEmpty());
    EXPECT_THAT(cf.static_fields, IsEmpty());
    EXPECT_THAT(cf.static_methods, IsEmpty());

    EXPECT_THAT(cf.methods, SizeIs(1));
    EXPECT_THAT(cf.methods.front().class_name_index, Eq(1));
    EXPECT_THAT(cf.methods.front().name_index, Eq(2));
    EXPECT_THAT(cf.methods.front().nr_args, Eq(0));
    EXPECT_THAT(cf.methods.front().vsb, Eq(bytecode::visibility::public_));
    EXPECT_FALSE(cf.methods.front().is_native);
    EXPECT_FALSE(cf.methods.front().is_static);
    EXPECT_FALSE(cf.methods.front().is_virtual);
    EXPECT_FALSE(cf.methods.front().is_pure);

    // constructor
    EXPECT_THAT(cf.methods.front().code, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),
        insn_encode(opcode::push_const, 3),
        insn_encode(opcode::push_const, 4),
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::drop_frame, 1)
    }));
}

// TODO debug info with var_name

TEST(Codegen, AssignsInCtor)
{
    ast::class_ class_;
    class_.name = "Object";
    class_.exprs.emplace_back(ast::assign { "VarName", 1.23 });
    class_.exprs.emplace_back(ast::assign { "ReusedName", 5.0 });
    class_.exprs.emplace_back(ast::assign { "ReusedName", 15.0 });
    bytecode::symbols syms;
    bytecode::codegen c(syms);
    bytecode::class_info cf = c.generate(class_);

    EXPECT_THAT(cf.const_pool, SizeIs(8));
    EXPECT_THAT(boost::get<std::string>(cf.const_pool[1]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(cf.const_pool[2]), Eq("~init/0"));
    EXPECT_THAT(boost::get<std::string>(cf.const_pool[3]), Eq("VarName"));
    EXPECT_THAT(boost::get<double>(cf.const_pool[4]), Eq(1.23));
    EXPECT_THAT(boost::get<std::string>(cf.const_pool[5]), Eq("ReusedName"));
    EXPECT_THAT(boost::get<double>(cf.const_pool[6]), Eq(5.0));
    EXPECT_THAT(boost::get<double>(cf.const_pool[7]), Eq(15.0));

    EXPECT_THAT(cf.base_name_index, Eq(0));
    EXPECT_THAT(cf.name_index, Eq(1));
    EXPECT_THAT(cf.vsb, Eq(bytecode::visibility::public_));
    EXPECT_FALSE(cf.is_abstract);
    EXPECT_FALSE(cf.is_interface);

    EXPECT_THAT(cf.fields, SizeIs(2));
    EXPECT_THAT(cf.fields[0].class_name_index, Eq(1));
    EXPECT_THAT(cf.fields[0].name_index, Eq(3));
    EXPECT_THAT(cf.fields[0].vsb, Eq(bytecode::visibility::public_));
    EXPECT_FALSE(cf.fields[0].is_static);
    EXPECT_FALSE(cf.fields[0].is_const);
    EXPECT_THAT(cf.fields[1].class_name_index, Eq(1));
    EXPECT_THAT(cf.fields[1].name_index, Eq(5));
    EXPECT_THAT(cf.fields[1].vsb, Eq(bytecode::visibility::public_));
    EXPECT_FALSE(cf.fields[1].is_static);
    EXPECT_FALSE(cf.fields[1].is_const);

    EXPECT_THAT(cf.static_fields, IsEmpty());
    EXPECT_THAT(cf.static_methods, IsEmpty());

    EXPECT_THAT(cf.methods, SizeIs(1));
    EXPECT_THAT(cf.methods.front().class_name_index, Eq(1));
    EXPECT_THAT(cf.methods.front().name_index, Eq(2));
    EXPECT_THAT(cf.methods.front().nr_args, Eq(0));
    EXPECT_THAT(cf.methods.front().vsb, Eq(bytecode::visibility::public_));
    EXPECT_FALSE(cf.methods.front().is_native);
    EXPECT_FALSE(cf.methods.front().is_static);
    EXPECT_FALSE(cf.methods.front().is_virtual);
    EXPECT_FALSE(cf.methods.front().is_pure);

    // constructor
    EXPECT_THAT(cf.methods.front().code, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),
        insn_encode(opcode::push_const, 4),
        insn_encode(opcode::push_local, 0),
        insn_encode(opcode::load_field, 0),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::push_local, 0),
        insn_encode(opcode::load_field, 1),
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::push_local, 0),
        insn_encode(opcode::load_field, 1),
        insn_encode(opcode::drop_frame, 1)
    }));
}

TEST(Codegen, UnaryOp)
{
    ast::class_ class_;
    class_.name = "Object";
    class_.exprs.emplace_back(ast::un_op { op_kind::not_, 1.23 });
    class_.exprs.emplace_back(ast::un_op { op_kind::neg, 1.23 });
    bytecode::symbols syms;
    bytecode::codegen c(syms);
    bytecode::class_info cf = c.generate(class_);

    EXPECT_THAT(cf.const_pool, SizeIs(4));
    EXPECT_THAT(boost::get<std::string>(cf.const_pool[1]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(cf.const_pool[2]), Eq("~init/0"));
    EXPECT_THAT(boost::get<double>(cf.const_pool[3]), Eq(1.23));

    EXPECT_THAT(cf.base_name_index, Eq(0));
    EXPECT_THAT(cf.name_index, Eq(1));
    EXPECT_THAT(cf.vsb, Eq(bytecode::visibility::public_));
    EXPECT_FALSE(cf.is_abstract);
    EXPECT_FALSE(cf.is_interface);

    EXPECT_THAT(cf.fields, IsEmpty());
    EXPECT_THAT(cf.static_fields, IsEmpty());
    EXPECT_THAT(cf.static_methods, IsEmpty());

    EXPECT_THAT(cf.methods, SizeIs(1));
    EXPECT_THAT(cf.methods.front().class_name_index, Eq(1));
    EXPECT_THAT(cf.methods.front().name_index, Eq(2));
    EXPECT_THAT(cf.methods.front().nr_args, Eq(0));
    EXPECT_THAT(cf.methods.front().vsb, Eq(bytecode::visibility::public_));
    EXPECT_FALSE(cf.methods.front().is_native);
    EXPECT_FALSE(cf.methods.front().is_static);
    EXPECT_FALSE(cf.methods.front().is_virtual);
    EXPECT_FALSE(cf.methods.front().is_pure);

    // constructor
    EXPECT_THAT(cf.methods.front().code, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),
        insn_encode(opcode::push_const, 3),
        insn_encode(opcode::call_op, static_cast<std::uint32_t>(op_kind::not_)),
        insn_encode(opcode::push_const, 3),
        insn_encode(opcode::call_op, static_cast<std::uint32_t>(op_kind::neg)),
        insn_encode(opcode::drop_frame, 1)
    }));
}

TEST(Codegen, BinaryOp)
{
    ast::class_ class_;
    class_.name = "Object";
    class_.exprs.emplace_back(ast::bin_op { op_kind::div,
        ast::bin_op { op_kind::sub, 1.23, 2.0 }, ast::bin_op { op_kind::add, 1.23, -20.3 }
    });
    bytecode::symbols syms;
    bytecode::codegen c(syms);
    bytecode::class_info cf = c.generate(class_);

    EXPECT_THAT(cf.const_pool, SizeIs(6));
    EXPECT_THAT(boost::get<std::string>(cf.const_pool[1]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(cf.const_pool[2]), Eq("~init/0"));
    EXPECT_THAT(boost::get<double>(cf.const_pool[3]), Eq(-20.3));
    EXPECT_THAT(boost::get<double>(cf.const_pool[4]), Eq(1.23));
    EXPECT_THAT(boost::get<double>(cf.const_pool[5]), Eq(2.0));

    EXPECT_THAT(cf.base_name_index, Eq(0));
    EXPECT_THAT(cf.name_index, Eq(1));
    EXPECT_THAT(cf.vsb, Eq(bytecode::visibility::public_));
    EXPECT_FALSE(cf.is_abstract);
    EXPECT_FALSE(cf.is_interface);

    EXPECT_THAT(cf.fields, IsEmpty());
    EXPECT_THAT(cf.static_fields, IsEmpty());
    EXPECT_THAT(cf.static_methods, IsEmpty());

    EXPECT_THAT(cf.methods, SizeIs(1));
    EXPECT_THAT(cf.methods.front().class_name_index, Eq(1));
    EXPECT_THAT(cf.methods.front().name_index, Eq(2));
    EXPECT_THAT(cf.methods.front().nr_args, Eq(0));
    EXPECT_THAT(cf.methods.front().vsb, Eq(bytecode::visibility::public_));
    EXPECT_FALSE(cf.methods.front().is_native);
    EXPECT_FALSE(cf.methods.front().is_static);
    EXPECT_FALSE(cf.methods.front().is_virtual);
    EXPECT_FALSE(cf.methods.front().is_pure);

    // constructor
    EXPECT_THAT(cf.methods.front().code, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),
        insn_encode(opcode::push_const, 3),
        insn_encode(opcode::push_const, 4),
        insn_encode(opcode::call_op, static_cast<std::uint32_t>(op_kind::add)),
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::push_const, 4),
        insn_encode(opcode::call_op, static_cast<std::uint32_t>(op_kind::sub)),
        insn_encode(opcode::call_op, static_cast<std::uint32_t>(op_kind::div)),
        insn_encode(opcode::drop_frame, 1)
    }));
}

TEST(Codegen, TernaryOp)
{
    ast::class_ class_;
    class_.name = "Object";
    class_.exprs.emplace_back(ast::ternary { true, 1.23, -20.3 });
    class_.exprs.emplace_back(ast::ternary {
        ast::bin_op { op_kind::gte, -1.0, 0.0 },
        ast::bin_op { op_kind::mul, -1.0, 1.0 },
        ast::ternary { false, 1.0, -1.0 }
    });
    bytecode::symbols syms;
    bytecode::codegen c(syms);
    bytecode::class_info cf = c.generate(class_);

    EXPECT_THAT(cf.const_pool, SizeIs(8));
    EXPECT_THAT(boost::get<std::string>(cf.const_pool[1]), Eq("Object"));
    EXPECT_THAT(boost::get<std::string>(cf.const_pool[2]), Eq("~init/0"));
    EXPECT_THAT(boost::get<double>(cf.const_pool[3]), Eq(1.0));
    EXPECT_THAT(boost::get<double>(cf.const_pool[4]), Eq(1.23));
    EXPECT_THAT(boost::get<double>(cf.const_pool[5]), Eq(-20.3));
    EXPECT_THAT(boost::get<double>(cf.const_pool[6]), Eq(0.0));
    EXPECT_THAT(boost::get<double>(cf.const_pool[7]), Eq(-1.0));

    EXPECT_THAT(cf.base_name_index, Eq(0));
    EXPECT_THAT(cf.name_index, Eq(1));
    EXPECT_THAT(cf.vsb, Eq(bytecode::visibility::public_));
    EXPECT_FALSE(cf.is_abstract);
    EXPECT_FALSE(cf.is_interface);

    EXPECT_THAT(cf.fields, IsEmpty());
    EXPECT_THAT(cf.static_fields, IsEmpty());
    EXPECT_THAT(cf.static_methods, IsEmpty());

    EXPECT_THAT(cf.methods, SizeIs(1));
    EXPECT_THAT(cf.methods.front().class_name_index, Eq(1));
    EXPECT_THAT(cf.methods.front().name_index, Eq(2));
    EXPECT_THAT(cf.methods.front().nr_args, Eq(0));
    EXPECT_THAT(cf.methods.front().vsb, Eq(bytecode::visibility::public_));
    EXPECT_FALSE(cf.methods.front().is_native);
    EXPECT_FALSE(cf.methods.front().is_static);
    EXPECT_FALSE(cf.methods.front().is_virtual);
    EXPECT_FALSE(cf.methods.front().is_pure);

    // constructor
    EXPECT_THAT(cf.methods.front().code, ElementsAreArray({
        insn_encode(opcode::push_frame, 1),
        insn_encode(opcode::push_const, 3),
        insn_encode(opcode::br_false, 2),
        insn_encode(opcode::push_const, 4),
        insn_encode(opcode::br, 1),
        insn_encode(opcode::push_const, 5),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::call_op, static_cast<std::uint32_t>(op_kind::gte)),
        insn_encode(opcode::br_false, 4),
        insn_encode(opcode::push_const, 3),
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::call_op, static_cast<std::uint32_t>(op_kind::mul)),
        insn_encode(opcode::br, 5),
        insn_encode(opcode::push_const, 6),
        insn_encode(opcode::br_false, 2),
        insn_encode(opcode::push_const, 3),
        insn_encode(opcode::br, 1),
        insn_encode(opcode::push_const, 7),
        insn_encode(opcode::drop_frame, 1)
    }));
}
