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
#ifndef COMPILER_H
#define COMPILER_H

#include "symbol_table.h"
#include "ast.h"
#include "semantic.h"
#include "bytecode/const_pool_manager.h"

#include <stack>

namespace emel {

enum class symbol_kind {
    local, field, function, type, module, max_kind
};

using symbol_table =
    basic_symbol_table<semantic::vertex_descriptor,
        symbol_kind, static_cast<std::size_t>(symbol_kind::max_kind)>;

using codegen_result = semantic::vertex_descriptor;

class codegen : public boost::static_visitor<codegen_result>
        , protected bytecode::const_pool_manager
{
protected:
    using const_pool_manager::const_pool;
    std::shared_ptr<semantic::module> module;
    symbol_table &sym_table;
    semantic::graph_type &graph;

    std::unordered_map<std::string, std::shared_ptr<semantic::class_>> class_cache;
    std::string class_name, base_name;
    bool in_ctor = false;

    semantic::names_map_type names_map;
    semantic::nodes_map_type nodes_map;

    std::deque<semantic::vertex_descriptor> semantic_stack;
    std::stack<semantic::vertex_descriptor> tails_stack;
    std::vector<semantic::vertex_descriptor> terminator_stack;
    std::stack<std::size_t, decltype(terminator_stack)> continues_stack;
    std::stack<std::size_t, decltype(terminator_stack)> breaks_stack;

public:
    explicit codegen(const std::string &module_name,
                     std::shared_ptr<semantic::module> m,
                     symbol_table &st,
                     semantic::graph_type &sg,
                     std::vector<value_type> &cp)
        : bytecode::const_pool_manager(cp)
        , module(std::move(m))
        , sym_table(st), graph(sg)
        , names_map(boost::get(boost::vertex_name, graph))
        , nodes_map(boost::get(boost::vertex_semantic, graph))
    {
        store_const("");
        module->name_index = store_const(module_name);
        auto v = semantic::add_vertex(module_name, module, graph);
        sym_table.add_symbol(module_name, v, symbol_kind::module);
    }

    codegen_result operator()(empty_value_type)
    {
        semantic::node_ptr n = std::make_shared<semantic::node>();
        n->index = 0;
        ++n->nr_stack;
        n->insns.push_back(insn_encode(opcode::push_const));
        return semantic::add_vertex(std::move(n), graph);
    }

    codegen_result operator()(const std::string &value)
    {
        if(value.empty())
            return operator ()(empty_value);

        semantic::node_ptr n = std::make_shared<semantic::node>();
        n->index = store_const(value);
        ++n->nr_stack;
        n->insns.push_back(insn_encode(opcode::push_const, n->index));
        return semantic::add_vertex(std::move(n), graph);
    }

    codegen_result operator()(double value)
    {
        if(value == 0.0)
            return operator ()(empty_value);

        semantic::node_ptr n = std::make_shared<semantic::node>();
        n->index = store_const(value);
        ++n->nr_stack;
        n->insns.push_back(insn_encode(opcode::push_const, n->index));
        return semantic::add_vertex(std::move(n), graph);
    }

    codegen_result operator()(bool value)
    {
        if(!value)
            return operator ()(empty_value);

        semantic::node_ptr n = std::make_shared<semantic::node>();
        n->index = store_const(1);
        ++n->nr_stack;
        n->insns.push_back(insn_encode(opcode::push_const, n->index));
        return semantic::add_vertex(std::move(n), graph);
    }

    codegen_result operator()(ast::class_ &node)
    {
        std::shared_ptr<semantic::class_> class_
            = std::make_shared<semantic::class_>(semantic::node::vis_public, 0);

        module->classes.push_back(class_);
        class_->index = module->classes.size() - 1;
        class_->name_index = store_const(node.name);
        class_cache.emplace(node.name, class_);

        if(node.base_name.empty() && node.name != "Object") {
            class_->base_name_index = store_const("Object");
            node.base_name = "Object";
        } else
            class_->base_name_index = store_const(node.base_name);

        class_name = node.name;
        base_name = node.base_name;

        if(!base_name.empty()) {
            decltype(class_) base_class = class_cache[base_name];
            if(!base_class)
                throw std::runtime_error("base class not found");
            class_->fields_offset = base_class->fields_offset + base_class->fields.size();
            class_->methods_offset = base_class->methods_offset + base_class->methods.size();
        }

        auto class_vertex = semantic::add_vertex(node.name, class_, graph);
        sym_table.add_symbol(node.name, class_vertex, symbol_kind::type);
        semantic_stack.push_front(class_vertex);

        decltype(node.methods) methods;
        methods.reserve(node.methods.size() + 1);

        // TODO Конструктор с параметрами: подсчет аргументов,
        // передача их конструктору базового класса
        {
            ast::param par;
            par.by_ref = false;
            par.name = "this";
            methods.emplace_back("~init", std::vector<ast::param> { par },
                std::move(node.exprs));

            for(ast::method &m : node.methods)
                methods.push_back(std::move(m));
        }

        for(const ast::method &met : methods) {
            std::shared_ptr<semantic::function> func
                = std::make_shared<semantic::function>(semantic::node::vis_public, met.params.size());

            class_->methods.push_back(func);
            func->index = class_->methods_offset + class_->methods.size() - 1;
            func->name_index = store_const(met.name);

            auto func_vertex = semantic::add_vertex(met.name, std::move(func), graph);
            sym_table.add_symbol(met.name, func_vertex, symbol_kind::function);
        }

        class_->code_range.first = module->insns.size();

        for(ast::method &m : methods) {
            semantic::vertex_descriptor func_vertex
                = *sym_table.find_symbol(m.name, symbol_kind::function).first;

            std::shared_ptr<semantic::function> func
                = std::dynamic_pointer_cast<semantic::function>(nodes_map[func_vertex]);

            semantic_stack.push_front(func_vertex);
            in_ctor = func->index == class_->methods_offset;

            operator ()(m);

            func->code_range.first = module->insns.size();

            for(auto &insn : func->insns)
                module->insns.push_back(insn);
            func->insns.clear();

            func->code_range.second = module->insns.size();

            in_ctor = false;
            semantic_stack.pop_front();
        }

        class_->code_range.second = module->insns.size();

        semantic_stack.pop_front();
        class_name.erase();
        base_name.erase();
        return class_vertex;
    }

    codegen_result operator()(ast::method &node)
    {
        semantic::vertex_descriptor func_vertex = semantic_stack.front();
        std::shared_ptr<semantic::function> func
            = std::dynamic_pointer_cast<semantic::function>(nodes_map[func_vertex]);

        for(ast::param &param : node.params) {
            std::shared_ptr<semantic::variable> var
                = std::make_shared<semantic::variable>(
                    param.by_ref ? semantic::node::is_ref | semantic::node::is_param
                                 : semantic::node::is_regular | semantic::node::is_param);
            func->slots.push_back(var);
            auto param_vertex = semantic::add_vertex(param.name, std::move(var), graph);
            sym_table.add_symbol(param.name, param_vertex, symbol_kind::local);
        }

        func->insns.push_back(insn_encode(opcode::push_frame));

        if(in_ctor && class_name != "Object") {
            auto vec = sym_table.find_symbols("~init", symbol_kind::function);
            if(vec.size() < 2)
                throw std::runtime_error("symbol of base class ctor not defined");

            // ctor is first method always
            const auto base_ctor_offset = class_cache[base_name]->methods_offset;

            // push "this"
            ++func->nr_stack;
            func->insns.push_back(insn_encode(opcode::push_local, 0));
            func->insns.push_back(insn_encode(opcode::fcall, base_ctor_offset));
        }

        for(auto &e : node.exprs) {
            semantic::node_ptr n = nodes_map[e.apply_visitor(*this)];

            for(auto &insn : n->insns)
                func->insns.push_back(insn);
            n->insns.clear();

            for(auto &slot : n->slots)
                func->slots.push_back(std::move(slot));
            n->slots.clear();

            func->nr_stack += n->nr_stack;
            n->nr_stack = 0;
        }

        func->insns.front() = insn_encode(opcode::push_frame, func->slots.size());
        func->insns.push_back(insn_encode(opcode::drop_frame, func->slots.size()));
        sym_table.drop_symbols(func->slots.size(), symbol_kind::local);
        return func_vertex;
    }

    codegen_result operator()(ast::call &)
    {
    }

    codegen_result operator()(ast::assign &node)
    {
        semantic::vertex_descriptor vertex = 0;
        symbol_kind kind = symbol_kind::local;
        auto pair_vector = sym_table.find_symbols(node.var_name);

        if(pair_vector.empty()) {
            semantic::node_ptr new_node;

            if(in_ctor) {
                std::shared_ptr<semantic::field> field
                    = std::make_shared<semantic::field>(semantic::node::vis_public);

                std::shared_ptr<semantic::class_> class_ = class_cache[class_name];
                class_->fields.push_back(field);
                field->index = class_->fields_offset + class_->fields.size() - 1;
                field->name_index = store_const(node.var_name);
                kind = symbol_kind::field;
                new_node = std::move(field);

            } else {
                semantic::vertex_descriptor func_vertex = semantic_stack.front();
                std::shared_ptr<semantic::function> func
                    = std::dynamic_pointer_cast<semantic::function>(nodes_map[func_vertex]);

                std::shared_ptr<semantic::variable> var
                    = std::make_shared<semantic::variable>();
                func->slots.push_back(var);
                kind = symbol_kind::local;
                new_node = std::move(var);
            }

            vertex = semantic::add_vertex(node.var_name, std::move(new_node), graph);
            sym_table.add_symbol(node.var_name, vertex, kind);

        } else {
            vertex = *pair_vector.front().first;
            kind = pair_vector.front().second;
        }

        semantic::node_ptr reused_node = nodes_map[vertex];
        semantic::node_ptr n = nodes_map[node.rhs.apply_visitor(*this)];

        reused_node->insns = std::move(n->insns);

        for(auto &slot : n->slots)
            reused_node->slots.push_back(std::move(slot));
        n->slots.clear();

        reused_node->nr_stack += n->nr_stack;
        n->nr_stack = 0;

        switch (kind) {
            case symbol_kind::local:
                reused_node->insns.push_back(insn_encode(opcode::load_local, reused_node->index));
                break;

            case symbol_kind::field:
                // push "this"
                ++reused_node->nr_stack;
                reused_node->insns.push_back(insn_encode(opcode::push_local, 0));
                reused_node->insns.push_back(insn_encode(opcode::load_field, reused_node->index));
                break;

            default:
                break;
        }

        return vertex;
    }

    codegen_result operator()(ast::variable &)
    {
    }

    codegen_result operator()(ast::try_ &)
    {
    }

    codegen_result operator()(ast::return_ &)
    {
    }

    codegen_result operator()(ast::break_ &)
    {
    }

    codegen_result operator()(ast::continue_ &)
    {
    }

    codegen_result operator()(ast::switch_ &)
    {
    }

    codegen_result operator()(ast::case_ &)
    {
    }

    codegen_result operator()(ast::for_ &)
    {
    }

    codegen_result operator()(ast::ternary &node)
    {
        semantic::node_ptr n = std::make_shared<semantic::node>();
        semantic::node_ptr cond = nodes_map[node.cond.apply_visitor(*this)];

        for(auto &insn : cond->insns)
            n->insns.push_back(insn);
        cond->insns.clear();

        for(auto &slot : cond->slots)
            n->slots.push_back(std::move(slot));
        cond->slots.clear();

        n->nr_stack += cond->nr_stack;
        cond->nr_stack = 0;

        auto start_idx = n->insns.size();
        n->insns.push_back(insn_encode(opcode::brf_false));

        semantic::node_ptr first = nodes_map[node.first.apply_visitor(*this)];

        for(auto &insn : first->insns)
            n->insns.push_back(insn);
        first->insns.clear();

        for(auto &slot : first->slots)
            n->slots.push_back(std::move(slot));
        first->slots.clear();

        n->nr_stack += first->nr_stack;
        first->nr_stack = 0;

        n->insns.push_back(insn_encode(opcode::brf));
        auto end_idx = n->insns.size();

        auto offset = end_idx - start_idx;
        n->insns[start_idx] = insn_encode(opcode::brf_false, offset);

        start_idx = end_idx - 1;

        semantic::node_ptr second = nodes_map[node.second.apply_visitor(*this)];

        for(auto &insn : second->insns)
            n->insns.push_back(insn);
        second->insns.clear();

        for(auto &slot : second->slots)
            n->slots.push_back(std::move(slot));
        second->slots.clear();

        n->nr_stack += second->nr_stack;
        second->nr_stack = 0;

        end_idx = n->insns.size();

        offset = end_idx - start_idx;
        n->insns[start_idx] = insn_encode(opcode::brf, offset);
        return semantic::add_vertex(std::move(n), graph);
    }

    codegen_result operator()(ast::if_ &node)
    {
        semantic::node_ptr n = std::make_shared<semantic::node>();
        semantic::node_ptr cond = nodes_map[node.cond.apply_visitor(*this)];

        for(auto &insn : cond->insns)
            n->insns.push_back(insn);
        cond->insns.clear();

        for(auto &slot : cond->slots)
            n->slots.push_back(std::move(slot));
        cond->slots.clear();

        n->nr_stack += cond->nr_stack;
        cond->nr_stack = 0;

        auto start_idx = n->insns.size();
        n->insns.push_back(insn_encode(opcode::brf_false));

        for(auto &then : node.then_exprs) {
            semantic::node_ptr then_node = nodes_map[then.apply_visitor(*this)];

            for(auto &insn : then_node->insns)
                n->insns.push_back(insn);
            then_node->insns.clear();

            for(auto &slot : then_node->slots)
                n->slots.push_back(std::move(slot));
            then_node->slots.clear();

            n->nr_stack += then_node->nr_stack;
            then_node->nr_stack = 0;
        }

        if(!node.else_exprs.empty())
            n->insns.push_back(insn_encode(opcode::brf));

        auto end_idx = n->insns.size();
        auto offset = end_idx - start_idx;
        n->insns[start_idx] = insn_encode(opcode::brf_false, offset);

        for(auto &else_ : node.else_exprs) {
            semantic::node_ptr else_node = nodes_map[else_.apply_visitor(*this)];

            for(auto &insn : else_node->insns)
                n->insns.push_back(insn);
            else_node->insns.clear();

            for(auto &slot : else_node->slots)
                n->slots.push_back(std::move(slot));
            else_node->slots.clear();

            n->nr_stack += else_node->nr_stack;
            else_node->nr_stack = 0;
        }

        if(!node.else_exprs.empty()) {
            start_idx = end_idx - 1;
            end_idx = n->insns.size();
            offset = end_idx - start_idx;
            n->insns[start_idx] = insn_encode(opcode::brf, offset);
        }

        return semantic::add_vertex(std::move(n), graph);
    }

    codegen_result operator()(ast::while_ &node)
    {
        semantic::node_ptr n = std::make_shared<semantic::node>();
        std::size_t br_idx = 0;

        // Для упрощения "вечного" цикла
        const bool forever_loop = typeid(bool) == node.cond.type()
            && boost::get<bool>(node.cond) != 0.0;

        if(!forever_loop) {
            semantic::node_ptr cond = nodes_map[node.cond.apply_visitor(*this)];

            for(auto &insn : cond->insns)
                n->insns.push_back(insn);
            cond->insns.clear();

            for(auto &slot : cond->slots)
                n->slots.push_back(std::move(slot));
            cond->slots.clear();

            n->nr_stack += cond->nr_stack;
            cond->nr_stack = 0;

            br_idx = n->insns.size();
            n->insns.push_back(insn_encode(opcode::brf_false));
        }

        for(auto &then : node.exprs) {
            semantic::node_ptr then_node = nodes_map[then.apply_visitor(*this)];

            for(auto &insn : then_node->insns)
                n->insns.push_back(insn);
            then_node->insns.clear();

            for(auto &slot : then_node->slots)
                n->slots.push_back(std::move(slot));
            then_node->slots.clear();

            n->nr_stack += then_node->nr_stack;
            then_node->nr_stack = 0;
        }

        auto end_idx = n->insns.size();
        auto offset = end_idx/* - start_idx*/;
        n->insns.push_back(insn_encode(opcode::brb, offset));

        if(!forever_loop)
            n->insns[br_idx] = insn_encode(opcode::brf_false, offset);

        return semantic::add_vertex(std::move(n), graph);
    }

    codegen_result operator()(ast::bin_op &node)
    {
        semantic::node_ptr n = std::make_shared<semantic::node>();
        semantic::node_ptr rhs = nodes_map[node.rhs.apply_visitor(*this)];

        for(auto &insn : rhs->insns)
            n->insns.push_back(insn);
        rhs->insns.clear();

        for(auto &slot : rhs->slots)
            n->slots.push_back(std::move(slot));
        rhs->slots.clear();

        n->nr_stack += rhs->nr_stack;
        rhs->nr_stack = 0;

        semantic::node_ptr lhs = nodes_map[node.lhs.apply_visitor(*this)];

        for(auto &insn : lhs->insns)
            n->insns.push_back(insn);
        lhs->insns.clear();

        for(auto &slot : lhs->slots)
            n->slots.push_back(std::move(slot));
        lhs->slots.clear();

        n->nr_stack += lhs->nr_stack;
        lhs->nr_stack = 0;

        n->insns.push_back(insn_encode(opcode::call_op, static_cast<std::uint32_t>(node.k)));
        return semantic::add_vertex(std::move(n), graph);
    }

    codegen_result operator()(ast::un_op &node)
    {
        const auto vertex = node.rhs.apply_visitor(*this);
        semantic::node_ptr n = nodes_map[vertex];
        n->insns.push_back(insn_encode(opcode::call_op, static_cast<std::uint32_t>(node.k)));
        return vertex;
    }

};

class compiler
{
public:
    static std::vector<ast::class_>
    topological_sort(std::vector<ast::class_> &classes);
};

} // namespace emel

#endif // COMPILER_H
