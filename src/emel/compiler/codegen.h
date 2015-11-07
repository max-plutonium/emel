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
#ifndef CODEGEN_H
#define CODEGEN_H

#include "../ast.h"
#include "../semantic.h"
#include "const_pool_manager.h"
#include "symbol_table.h"

namespace emel { namespace compiler {

enum class symbol_kind {
    local, field, function, type, module, max_kind
};

using symbol_table =
    basic_symbol_table<semantic::node_ptr,
        symbol_kind, static_cast<std::size_t>(symbol_kind::max_kind)>;

struct codegen_result {
    semantic::node_ptr node;
    std::uint32_t index = 0;
    std::uint32_t nr_stack = 0;
    insn_array insns;
    std::deque<semantic::node_ptr> slots_array;

    void pull_insns(codegen_result &other);
    void pull_slots(codegen_result &other);
    void pull_stack(codegen_result &other);
};

// TODO stack and slot usage optimizing (ershov's numbers)
class EMEL_EXPORT codegen : public boost::static_visitor<codegen_result>
        , protected const_pool_manager
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
                     std::vector<value_type> &cp);

    codegen_result get_result();
    codegen_result operator()(empty_value_type);
    codegen_result operator()(const std::string &value);
    codegen_result operator()(double value);
    codegen_result operator()(bool value);
    codegen_result operator()(ast::class_ &node);
    codegen_result operator()(ast::method &node);
    codegen_result operator()(ast::call &);
    codegen_result operator()(ast::assign &node);
    codegen_result operator()(ast::variable &);
    codegen_result operator()(ast::try_ &);
    codegen_result operator()(ast::return_ &);
    codegen_result operator()(ast::break_ &);
    codegen_result operator()(ast::continue_ &);
    codegen_result operator()(ast::switch_ &node);
    codegen_result operator()(ast::case_ &);
    codegen_result operator()(ast::for_ &node);
    codegen_result operator()(ast::ternary &node);
    codegen_result operator()(ast::if_ &node);
    codegen_result operator()(ast::while_ &node);
    codegen_result operator()(ast::bin_op &node);
    codegen_result operator()(ast::un_op &node);
};

} // namespace compiler

} // namespace emel

#endif // CODEGEN_H
