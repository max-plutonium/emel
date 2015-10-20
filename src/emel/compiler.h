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

#include <experimental/optional>
#include <stack>

namespace emel {

enum class symbol_kind {
    local, field, function, type, module, max_kind
};

using symbol_table =
    basic_symbol_table<semantic::node_ptr,
        symbol_kind, static_cast<std::size_t>(symbol_kind::max_kind)>;

template <typename Tp>
  using optional = std::experimental::optional<Tp>;

struct codegen_result {
    optional<semantic::vertex_descriptor> sem_vertex;
    semantic::node_ptr node;
    std::uint32_t index = 0;
    std::uint32_t nr_stack = 0;
    insn_array insns;
    std::deque<semantic::node_ptr> slots_array;

    void pull_insns(codegen_result &other) {
        for(auto &insn : other.insns)
            insns.push_back(insn);
        other.insns.clear();
    }

    void pull_slots(codegen_result &other) {
        for(auto &slot : other.slots_array)
            slots_array.push_back(std::move(slot));
        other.slots_array.clear();
    }

    void pull_stack(codegen_result &other) {
        nr_stack += other.nr_stack;
        other.nr_stack = 0;
    }
};

// TODO stack and slot usage optimizing (ershov's numbers)
class codegen : public boost::static_visitor<codegen_result>
        , protected bytecode::const_pool_manager
{
protected:
    using const_pool_manager::const_pool;
    std::shared_ptr<semantic::module> module;
    std::shared_ptr<semantic::class_> cur_class;
    std::shared_ptr<semantic::function> cur_function;
    codegen_result result;
    symbol_table &sym_table;
    semantic::graph_type &graph;

    std::unordered_map<std::string, std::shared_ptr<semantic::class_>> class_cache;
    std::string class_name, base_name;
    bool in_ctor = false;

public:
    explicit codegen(const std::string &module_name,
                     std::shared_ptr<semantic::module> m,
                     symbol_table &st,
                     semantic::graph_type &sg,
                     std::vector<value_type> &cp)
        : bytecode::const_pool_manager(cp)
        , module(std::move(m))
        , sym_table(st), graph(sg)
    {
        module->name_index = store_const(module_name);
        sym_table.add_symbol(module_name, module, symbol_kind::module);
    }

    codegen_result get_result() {
        return std::move(result);
    }

    codegen_result operator()(empty_value_type)
    {
        codegen_result res;
        res.index = 0;
        ++res.nr_stack;
        res.insns.push_back(insn_encode(opcode::push_const));
        return res;
    }

    codegen_result operator()(const std::string &value)
    {
        codegen_result res;
        res.index = store_const(value);
        ++res.nr_stack;
        res.insns.push_back(insn_encode(opcode::push_const, res.index));
        return res;
    }

    codegen_result operator()(double value)
    {
        codegen_result res;
        res.index = store_const(value);
        ++res.nr_stack;
        res.insns.push_back(insn_encode(opcode::push_const, res.index));
        return res;
    }

    codegen_result operator()(bool value)
    {
        codegen_result res;
        res.index = store_const(value);
        ++res.nr_stack;
        res.insns.push_back(insn_encode(opcode::push_const, res.index));
        return res;
    }

    codegen_result operator()(ast::class_ &node)
    {
        codegen_result res;
        cur_class = std::make_shared<semantic::class_>(semantic::node::vis_public, 0);
        res.node = cur_class;

        res.index = cur_class->index = module->classes.size();
        module->classes.push_back(cur_class);
        cur_class->name_index = store_const(node.name);
        class_cache.emplace(node.name, cur_class);

        if(node.base_name.empty() && node.name != "Object") {
            cur_class->base_name_index = store_const("Object");
            node.base_name = "Object";
        } else
            cur_class->base_name_index = store_const(node.base_name);

        class_name = node.name;
        base_name = node.base_name;

        if(!base_name.empty()) {
            decltype(cur_class) base_class = class_cache[node.base_name];
            if(!base_class)
                throw std::runtime_error("base class not found");
            cur_class->fields_offset = base_class->fields_offset + base_class->fields.size();
            cur_class->methods_offset = base_class->methods_offset + base_class->methods.size();
        }

        sym_table.add_symbol(node.name, cur_class, symbol_kind::type);

        decltype(node.methods) methods;
        methods.reserve(node.methods.size() + 1); // + ctor

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
            auto func = std::make_shared<semantic::function>(
                semantic::node::vis_public, met.params.size());

            func->index = cur_class->methods_offset + cur_class->methods.size();
            cur_class->methods.push_back(func);
            func->name_index = store_const(met.name);

            sym_table.add_symbol(met.name, std::move(func), symbol_kind::function);
        }

        cur_class->code_range.first = result.insns.size();

        for(std::size_t idx = 0; idx < methods.size(); ++idx) {

            cur_function = std::dynamic_pointer_cast<semantic::function>(cur_class->methods[idx]);
            assert(cur_function);
            in_ctor = cur_function->index == cur_class->methods_offset;

            auto func_res = operator ()(methods[idx]);

            cur_function->code_range.first = result.insns.size();
            result.pull_insns(func_res);
            cur_function->code_range.second = result.insns.size();
            in_ctor = false;
            cur_function.reset();
        }

        cur_class->code_range.second = result.insns.size();

        cur_class.reset();
        class_name.erase();
        base_name.erase();
        return res;
    }

    codegen_result operator()(ast::method &node)
    {
        codegen_result res;
        res.node = cur_function;
        res.index = cur_function->index;

        for(ast::param &param : node.params) {
            std::shared_ptr<semantic::variable> var
                = std::make_shared<semantic::variable>(
                    param.by_ref ? semantic::node::is_ref | semantic::node::is_param
                                 : semantic::node::is_regular | semantic::node::is_param);
            res.slots_array.push_back(var);
            sym_table.add_symbol(param.name, std::move(var), symbol_kind::local);
        }

        res.insns.push_back(insn_encode(opcode::push_frame));

        if(in_ctor && class_name != "Object") {
            auto vec = sym_table.find_symbols("~init", symbol_kind::function);
            if(vec.size() < 2)
                throw std::runtime_error("symbol of base class ctor not defined");

            // ctor is first method always
            const auto base_ctor_offset = class_cache[base_name]->methods_offset;

            // push "this"
            ++res.nr_stack;
            res.insns.push_back(insn_encode(opcode::push_local, 0));
            res.insns.push_back(insn_encode(opcode::fcall, base_ctor_offset));
        }

        for(auto &e : node.exprs) {
            auto expr_res = e.apply_visitor(*this);

            res.pull_insns(expr_res);
            res.pull_slots(expr_res);
            res.pull_stack(expr_res);
        }

        res.insns.front() = insn_encode(opcode::push_frame, res.slots_array.size());
        res.insns.push_back(insn_encode(opcode::drop_frame, res.slots_array.size()));
        sym_table.drop_symbols(res.slots_array.size(), symbol_kind::local);
        return res;
    }

    codegen_result operator()(ast::call &)
    {
        codegen_result res;
        return res;
    }

    codegen_result operator()(ast::assign &node)
    {
        codegen_result res;
        symbol_kind kind = symbol_kind::local;
        auto pair_vector = sym_table.find_symbols(node.var_name);

        if(pair_vector.empty()) {
            semantic::node_ptr new_node;

            if(in_ctor) {
                auto field = std::make_shared<semantic::field>(semantic::node::vis_public);

                res.index = field->index = cur_class->fields_offset + cur_class->fields.size();
                cur_class->fields.push_back(field);
                field->name_index = store_const(node.var_name);

                kind = symbol_kind::field;
                new_node = std::move(field);

            } else {
                auto var = std::make_shared<semantic::variable>();

                res.index = var->index = cur_function->locals.size();
                cur_function->locals.push_back(var);
                res.slots_array.push_back(var);

                kind = symbol_kind::local;
                new_node = std::move(var);
            }

            res.node = new_node;
            sym_table.add_symbol(node.var_name, std::move(new_node), kind);

        } else {
            res.node = *pair_vector.front().first;
            res.index = res.node->index;
            kind = pair_vector.front().second;
        }

        auto rhs_res = node.rhs.apply_visitor(*this);

        res.insns = std::move(rhs_res.insns);
        res.pull_slots(rhs_res);
        res.pull_stack(rhs_res);

        switch (kind) {
            case symbol_kind::local:
                res.insns.push_back(insn_encode(opcode::load_local, res.index));
                break;

            case symbol_kind::field:
                // push "this"
                ++res.nr_stack;
                res.insns.push_back(insn_encode(opcode::push_local, 0));
                res.insns.push_back(insn_encode(opcode::load_field, res.index));
                break;

            default:
                assert(false);
                break;
        }

        return res;
    }

    codegen_result operator()(ast::variable &)
    {
        codegen_result res;
        return res;
    }

    codegen_result operator()(ast::try_ &)
    {
        codegen_result res;
        return res;
    }

    codegen_result operator()(ast::return_ &)
    {
        codegen_result res;
        return res;
    }

    codegen_result operator()(ast::break_ &)
    {
        codegen_result res;
        return res;
    }

    codegen_result operator()(ast::continue_ &)
    {
        codegen_result res;
        return res;
    }

    codegen_result operator()(ast::switch_ &node)
    {
        codegen_result res;

        // remove blocks without expressions
        for(auto idx = node.blocks.size(); idx; --idx) {
            const auto &case_ = boost::get<ast::case_>(node.blocks[idx - 1]);

            if(case_.exprs.empty())
                node.blocks.erase(node.blocks.begin() + idx - 1);
        }

        if(node.blocks.empty())
            return res;

        auto cond_res = node.cond.apply_visitor(*this);

        if(1 == node.blocks.size()) {
            auto &case_ = boost::get<ast::case_>(node.blocks.front());
            std::vector<std::size_t> br_indices;
            optional<std::size_t> brf_idx;

            const auto match_values_size = case_.match_values.size();
            bool need_to_emit_cond = true;

            for(std::size_t idx = 0; idx < match_values_size; ++idx) {
                auto value_res = case_.match_values[idx].apply_visitor(*this);

                // if default
                if(0 == value_res.index)
                    continue;

                if(need_to_emit_cond) {
                    res.pull_insns(cond_res);
                    res.pull_stack(cond_res);

                    const auto dup_count = match_values_size - 1;

                    if(0 != dup_count) {
                        res.insns.push_back(insn_encode(opcode::dup, dup_count));
                        res.nr_stack += dup_count;
                    }

                    need_to_emit_cond = false;
                }

                res.pull_insns(value_res);
                res.pull_slots(value_res);
                res.pull_stack(value_res);

                res.insns.push_back(insn_encode(opcode::call_op, op_kind::eq));

                if(idx != match_values_size - 1) {
                    br_indices.push_back(res.insns.size());
                    res.insns.push_back(insn_encode(opcode::brf_true));
                } else {
                    brf_idx = res.insns.size();
                    res.insns.push_back(insn_encode(opcode::brf_false));
                }
            }

            const auto end_idx = res.insns.size();

            for(auto idx : br_indices)
                res.insns[idx] = insn_encode(opcode::brf_true, end_idx - idx);

            if(brf_idx)
                res.insns[brf_idx.value()] = insn_encode(opcode::brf_false, end_idx - brf_idx.value() + 1);

            for(auto &expr : case_.exprs) {
                auto expr_res = expr.apply_visitor(*this);

                res.pull_insns(expr_res);
                res.pull_slots(expr_res);
                res.pull_stack(expr_res);
            }

        } else {
            std::map<double, std::size_t> doubles;
            std::map<std::string, std::size_t> strings;
            optional<std::size_t> default_branch_idx, true_branch_idx, false_branch_idx;

            res.pull_insns(cond_res);
            res.pull_stack(cond_res);

            for(std::size_t case_idx = 0; case_idx < node.blocks.size(); ++ case_idx) {
                auto &case_ = boost::get<ast::case_>(node.blocks[case_idx]);

                if(case_.exprs.empty())
                    continue;

                for(auto &match_value : case_.match_values) {
                    const auto value_res = match_value.apply_visitor(*this);
                    const auto &value = const_pool.at(value_res.index);
                    switch(value.which()) {
                        case 0:
                            assert(!default_branch_idx);
                            default_branch_idx = case_idx;
                            break;

                        case 1: {
                            const auto pair = strings.emplace(boost::get<std::string>(value), case_idx);
                            assert(pair.second);
                            break;
                        }

                        case 2: {
                            const auto pair = doubles.emplace(boost::get<double>(value), case_idx);
                            assert(pair.second);
                            break;
                        }

                        case 3: {
                            const bool v = boost::get<bool>(value);
                            assert(v ? !true_branch_idx : !false_branch_idx);
                            (v ? true_branch_idx : false_branch_idx) = case_idx;
                            break;
                        }

                        default:
                            assert(false);
                    }
                }
            }

            {
                std::size_t dup_count = 0;

                if(!doubles.empty())
                    ++dup_count;

                if(!strings.empty())
                    ++dup_count;

                if(true_branch_idx || false_branch_idx)
                    ++dup_count;

                if(--dup_count) {
                    res.insns.push_back(insn_encode(opcode::dup, dup_count));
                    res.nr_stack += dup_count;
                }
            }

            std::vector<std::size_t> push_indices_for_doubles, push_indices_for_strings, brf_indices;
            std::size_t double_table_insn_idx, string_table_insn_idx;

            if(!doubles.empty()) {
                std::for_each(doubles.rbegin(), doubles.rend(),
                    [&](const std::pair<double, std::size_t> &entry) {
                        push_indices_for_doubles.push_back(res.insns.size());
                        res.nr_stack += 2;
                        res.insns.push_back(insn_encode(opcode::push));
                        res.insns.push_back(insn_encode(opcode::push_const, store_const(entry.first)));
                    });

                double_table_insn_idx = res.insns.size();
                res.insns.push_back(insn_encode(opcode::br_table, doubles.size()));
            }

            if(!strings.empty()) {
                std::for_each(strings.rbegin(), strings.rend(),
                    [&](const std::pair<std::string, std::size_t> &entry) {
                        push_indices_for_strings.push_back(res.insns.size());
                        res.nr_stack += 2;
                        res.insns.push_back(insn_encode(opcode::push));
                        res.insns.push_back(insn_encode(opcode::push_const, store_const(entry.first)));
                    });

                string_table_insn_idx = res.insns.size();
                res.insns.push_back(insn_encode(opcode::br_table, strings.size()));
            }

            // case index to code index
            std::unordered_map<std::size_t, std::size_t> used_blocks;

            const auto bool_branch_idx = true_branch_idx ? true_branch_idx : false_branch_idx;

            if(bool_branch_idx) {
                const auto br_idx = res.insns.size();

                res.insns.push_back((bool_branch_idx == true_branch_idx)
                    ? insn_encode(opcode::brf_false) : insn_encode(opcode::brf_true));

                const auto pair = used_blocks.emplace(bool_branch_idx.value(), res.insns.size());
                assert(pair.second); // should be always true

                auto &case_ = boost::get<ast::case_>(node.blocks[bool_branch_idx.value()]);
                for(auto &expr : case_.exprs) {
                    auto expr_res = expr.apply_visitor(*this);

                    res.pull_insns(expr_res);
                    res.pull_slots(expr_res);
                    res.pull_stack(expr_res);
                }

                if(true_branch_idx && false_branch_idx)
                    res.insns.push_back(insn_encode(opcode::brf));

                const auto end_idx = (false_branch_idx && true_branch_idx) || !default_branch_idx
                        ? res.insns.size() : res.insns.size() + 1;

                res.insns[br_idx] = (bool_branch_idx == true_branch_idx)
                    ? insn_encode(opcode::brf_false, end_idx - br_idx)
                    : insn_encode(opcode::brf_true, end_idx - br_idx);
            }

            if(false_branch_idx && true_branch_idx) {
                assert(bool_branch_idx == true_branch_idx);
                const auto brf_idx = res.insns.size() - 1;

                const auto pair = used_blocks.emplace(false_branch_idx.value(), res.insns.size());
                assert(pair.second); // false- and true- branches shouldn't be identical

                auto &case_ = boost::get<ast::case_>(node.blocks[false_branch_idx.value()]);
                for(auto &expr : case_.exprs) {
                    auto expr_res = expr.apply_visitor(*this);

                    res.pull_insns(expr_res);
                    res.pull_slots(expr_res);
                    res.pull_stack(expr_res);
                }

                res.insns[brf_idx] = insn_encode(opcode::brf, res.insns.size() - brf_idx);
            }

            else if(default_branch_idx) {
                if(bool_branch_idx) {
                    brf_indices.push_back(res.insns.size());
                    res.insns.push_back(insn_encode(opcode::brf));
                }

                const auto pair = used_blocks.emplace(default_branch_idx.value(), res.insns.size());

                if(pair.second) {
                    auto &case_ = boost::get<ast::case_>(node.blocks[default_branch_idx.value()]);
                    for(auto &expr : case_.exprs) {
                        auto expr_res = expr.apply_visitor(*this);

                        res.pull_insns(expr_res);
                        res.pull_slots(expr_res);
                        res.pull_stack(expr_res);
                    }
                }
            }

            if(!doubles.empty() || !strings.empty()) {
                brf_indices.push_back(res.insns.size());
                res.insns.push_back(insn_encode(opcode::brf));
            }

            if(!doubles.empty()) {
                std::size_t code_slot_idx = doubles.size();

                std::for_each(doubles.begin(), doubles.end(),
                    [&](const std::pair<double, std::size_t> &entry) {

                    const auto pair = used_blocks.emplace(entry.second, res.insns.size());

                    assert(double_table_insn_idx > 0);
                    const auto branch_offset = pair.first->second - double_table_insn_idx;
                    res.insns[push_indices_for_doubles[code_slot_idx - 1]]
                            = insn_encode(opcode::push, branch_offset);

                    if(pair.second) {
                        auto &case_ = boost::get<ast::case_>(node.blocks[entry.second]);
                        for(auto &expr : case_.exprs) {
                            auto expr_res = expr.apply_visitor(*this);

                            res.pull_insns(expr_res);
                            res.pull_slots(expr_res);
                            res.pull_stack(expr_res);
                        }

                        brf_indices.push_back(res.insns.size());
                        res.insns.push_back(insn_encode(opcode::brf));
                    }

                    --code_slot_idx;
                });
            }

            if(!strings.empty()) {
                std::size_t code_slot_idx = strings.size();

                std::for_each(strings.begin(), strings.end(),
                    [&](const std::pair<std::string, std::size_t> &entry) {

                    const auto pair = used_blocks.emplace(entry.second, res.insns.size());

                    assert(string_table_insn_idx > 0);
                    const auto branch_offset = pair.first->second - string_table_insn_idx;
                    res.insns[push_indices_for_strings[code_slot_idx - 1]]
                            = insn_encode(opcode::push, branch_offset);

                    if(pair.second) {
                        auto &case_ = boost::get<ast::case_>(node.blocks[entry.second]);
                        for(auto &expr : case_.exprs) {
                            auto expr_res = expr.apply_visitor(*this);

                            res.pull_insns(expr_res);
                            res.pull_slots(expr_res);
                            res.pull_stack(expr_res);
                        }

                        brf_indices.push_back(res.insns.size());
                        res.insns.push_back(insn_encode(opcode::brf));
                    }

                    --code_slot_idx;
                });
            }

            if(insn_encode(opcode::brf) == res.insns.back()) {
                res.insns.pop_back();
                brf_indices.pop_back();
            }

            for(auto idx : brf_indices)
                res.insns[idx] = insn_encode(opcode::brf, res.insns.size() - idx);
        }

        return res;
    }

    codegen_result operator()(ast::case_ &)
    {
        assert(false);
        std::abort();
        __builtin_unreachable();
    }

    codegen_result operator()(ast::for_ &node)
    {
        codegen_result res;

        const bool init_empty = node.init.which() == 0;
        bool cond_empty = node.cond.which() == 0;
        const bool step_empty = node.step.which() == 0;

        if(!cond_empty && (typeid(bool) == node.cond.type())) {
            if(boost::get<bool>(node.cond))
                cond_empty = true; // make forever loop
            else
                return res; // generate nothing
        }

        if(!init_empty) {
            auto init_res = node.init.apply_visitor(*this);

            res.pull_insns(init_res);
            res.pull_slots(init_res);
            res.pull_stack(init_res);
        }

        const auto cond_idx = res.insns.size();

        if(!cond_empty) {
            auto cond_res = node.cond.apply_visitor(*this);

            res.pull_insns(cond_res);
            res.pull_slots(cond_res);
            res.pull_stack(cond_res);
        }

        const auto br_idx = res.insns.size();

        if(!cond_empty)
            res.insns.push_back(insn_encode(opcode::brf_false));

        for(auto &expr : node.exprs) {
            auto expr_res = expr.apply_visitor(*this);

            res.pull_insns(expr_res);
            res.pull_slots(expr_res);
            res.pull_stack(expr_res);
        }

        if(!step_empty) {
            auto step_res = node.step.apply_visitor(*this);

            res.pull_insns(step_res);
            res.pull_slots(step_res);
            res.pull_stack(step_res);
        }

        const auto offset = res.insns.size() - cond_idx;
        res.insns.push_back(insn_encode(opcode::brb, offset));

        if(!cond_empty)
            res.insns[br_idx] = insn_encode(opcode::brf_false, offset);

        return res;
    }

    // TODO Optimize booleans in condition
    codegen_result operator()(ast::ternary &node)
    {
        codegen_result res;

        auto cond_res = node.cond.apply_visitor(*this);

        res.pull_insns(cond_res);
        res.pull_slots(cond_res);
        res.pull_stack(cond_res);

        auto start_idx = res.insns.size();
        res.insns.push_back(insn_encode(opcode::brf_false));

        auto first_expr_res = node.first.apply_visitor(*this);

        res.pull_insns(first_expr_res);
        res.pull_slots(first_expr_res);
        res.pull_stack(first_expr_res);

        res.insns.push_back(insn_encode(opcode::brf));
        auto end_idx = res.insns.size();

        auto offset = end_idx - start_idx;
        res.insns[start_idx] = insn_encode(opcode::brf_false, offset);

        start_idx = end_idx - 1;

        auto second_expr_res = node.second.apply_visitor(*this);

        res.pull_insns(second_expr_res);
        res.pull_slots(second_expr_res);
        res.pull_stack(second_expr_res);

        end_idx = res.insns.size();

        offset = end_idx - start_idx;
        res.insns[start_idx] = insn_encode(opcode::brf, offset);

        return res;
    }

    // TODO Optimize booleans in condition
    codegen_result operator()(ast::if_ &node)
    {
        codegen_result res;

        auto cond_res = node.cond.apply_visitor(*this);

        res.pull_insns(cond_res);
        res.pull_slots(cond_res);
        res.pull_stack(cond_res);

        auto start_idx = res.insns.size();
        res.insns.push_back(insn_encode(opcode::brf_false));

        for(auto &then : node.then_exprs) {
            auto then_expr_res = then.apply_visitor(*this);

            res.pull_insns(then_expr_res);
            res.pull_slots(then_expr_res);
            res.pull_stack(then_expr_res);
        }

        if(!node.else_exprs.empty() && !node.then_exprs.empty())
            res.insns.push_back(insn_encode(opcode::brf));

        auto end_idx = res.insns.size();
        res.insns[start_idx] = insn_encode(opcode::brf_false, end_idx - start_idx);

        for(auto &else_ : node.else_exprs) {
            auto else_expr_res = else_.apply_visitor(*this);

            res.pull_insns(else_expr_res);
            res.pull_slots(else_expr_res);
            res.pull_stack(else_expr_res);
        }

        if(!node.else_exprs.empty() && !node.then_exprs.empty()) {
            start_idx = end_idx - 1;
            end_idx = res.insns.size();
            res.insns[start_idx] = insn_encode(opcode::brf, end_idx - start_idx);
        }

        return res;
    }

    codegen_result operator()(ast::while_ &node)
    {
        codegen_result res;
        optional<std::size_t> br_idx;

        if(typeid(bool) == node.cond.type()) {
            if(!boost::get<bool>(node.cond))
                return res; // generate nothing
        } else {
            auto cond_res = node.cond.apply_visitor(*this);

            res.pull_insns(cond_res);
            res.pull_slots(cond_res);
            res.pull_stack(cond_res);

            br_idx = res.insns.size();
            res.insns.push_back(insn_encode(opcode::brf_false));
        }

        for(auto &then : node.exprs) {
            auto then_expr_res = then.apply_visitor(*this);

            res.pull_insns(then_expr_res);
            res.pull_slots(then_expr_res);
            res.pull_stack(then_expr_res);
        }

        const auto offset = res.insns.size();
        res.insns.push_back(insn_encode(opcode::brb, offset));

        if(br_idx)
            res.insns[br_idx.value()] = insn_encode(opcode::brf_false, offset);

        return res;
    }

    codegen_result operator()(ast::bin_op &node)
    {
        codegen_result res;

        auto rhs_res = node.rhs.apply_visitor(*this);

        res.pull_insns(rhs_res);
        res.pull_slots(rhs_res);
        res.pull_stack(rhs_res);

        auto lhs_res = node.lhs.apply_visitor(*this);

        res.pull_insns(lhs_res);
        res.pull_slots(lhs_res);
        res.pull_stack(lhs_res);

        res.insns.push_back(insn_encode(opcode::call_op, node.k));

        return res;
    }

    codegen_result operator()(ast::un_op &node)
    {
        auto res = node.rhs.apply_visitor(*this);
        res.insns.push_back(insn_encode(opcode::call_op, node.k));
        return res;
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
