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
#include "codegen.h"

namespace emel { namespace bytecode {

std::string codegen::class_name() const
{
    return boost::get<std::string>(result.const_pool[result.name_index]);
}

std::string codegen::base_name() const
{
    return boost::get<std::string>(result.const_pool[result.base_name_index]);
}

codegen::codegen(symbols &syms)
    : const_pool_manager(result.const_pool), symbols_manager(syms)
    , insns_manager(result.methods)
{
    store_const("");
}

class_info codegen::generate(ast::class_ &node)
{
    operator ()(node);
    return std::move(result);
}

void codegen::operator()(const std::string &value)
{
    emit_insn(opcode::push_const, store_const(value));
}

void codegen::operator()(double value)
{
    emit_insn(opcode::push_const, store_const(value));
}

void codegen::operator()(bool value)
{
    emit_insn(opcode::push_const, store_const(value ? 1 : 0));
}

void codegen::operator()(ast::class_ &node)
{
    result.name_index = store_const(node.name);
    if(node.base_name.empty() && node.name != "Object")
        result.base_name_index = store_const("Object");
    else
        result.base_name_index = store_const(node.base_name);
    result.vsb = visibility::public_;
    result.is_abstract = false;
    result.is_interface = false;

    auto args = std::vector<ast::param>();
    node.methods.emplace_back("~init/" + boost::lexical_cast<std::string>(args.size()),
    std::move(args), std::move(node.exprs));

    for(auto &m : node.methods)
        operator()(m);
}

void codegen::operator()(ast::method &node)
{
    const std::string &mname = node.name;
    method_info mi;
    mi.class_name_index = result.name_index;
    mi.name_index = store_const(mname);
    mi.nr_args = node.params.size();
    mi.vsb = visibility::public_;
    mi.is_native = false;
    mi.is_static = false;
    mi.is_virtual = false;
    mi.is_pure = false;
    result.methods.push_back(std::move(mi));

    add_symbol(name_encode(class_name(), mname, symbol::method), symbol::method);

    auto nr_slots = num_symbols(symbol::local);

    if(!result.methods.back().is_static)
        add_symbol("~this", symbol::local);

    for(ast::param &param : node.params) {
        auto type = param.by_ref ? symbol::param | symbol::ref : symbol::param;
        add_symbol(param.name, symbol::local, static_cast<symbol::kind>(type));
    }

    in_ctor = mname.find("~init/") != std::string::npos;

    start_frame();
    nr_slots = num_symbols(symbol::local) - nr_slots;
    emit_insn(opcode::push_frame, nr_slots);

    // TODO Конструктор с параметрами: подсчет аргументов,
    // передача их конструктору базового класса
    if(in_ctor && !base_name().empty()) {
        auto pair = find_symbol(name_encode(base_name(), std::string("~init/0"),
        symbol::method), symbol::method);
        if(!pair.second)
            throw std::runtime_error("symbol of base class ctor not defined");
        const auto base_ctor_offset = pair.first->index;
        emit_insn(opcode::push_local, find_symbol("~this", symbol::local).first->index);
        emit_insn(opcode::fcall, base_ctor_offset);
    }

    for(auto &expr : node.exprs)
        expr.apply_visitor(*this);

    drop_symbols(num_slots(), symbol::local);
    fini_frame();
    emit_insn(opcode::drop_frame, nr_slots);

    in_ctor = false;
}

void codegen::operator()(ast::assign &node)
{
    const auto type = in_ctor ? symbol::field : symbol::local;
    symbol *sym; bool found;
    std::tie(sym, found) = find_symbol(name_encode(class_name(),
    node.var_name, type), type);

    if(!found && in_ctor) {
        field_info fi;
        fi.class_name_index = result.name_index;
        fi.name_index = store_const(node.var_name);
        fi.vsb = visibility::public_;
        fi.is_static = false;
        fi.is_const = false;
        result.fields.push_back(std::move(fi));

        sym = &add_symbol(name_encode(class_name(),
        node.var_name, type), type);
    }

    node.rhs.apply_visitor(*this);

    if(in_ctor) {
        emit_insn(opcode::push_local, find_symbol("~this", symbol::local).first->index);
        emit_insn(opcode::load_field, sym->index);
    }
    else
        emit_insn(opcode::load_local, sym->index);
}

void codegen::operator()(ast::ternary &node)
{
    node.cond.apply_visitor(*this);
    auto start = emit_insn(opcode::brf_false);
    node.first.apply_visitor(*this);
    auto end = emit_insn(opcode::brf);
    auto offset = end - start;
    edit_insn(start, opcode::brf_false, offset);
    start = end;
    node.second.apply_visitor(*this);
    end = last_insn_index();
    offset = end - start;
    edit_insn(start, opcode::brf, offset);
}

void codegen::operator()(ast::if_ &node)
{
    node.cond.apply_visitor(*this);
    auto start = emit_insn(opcode::brf_false);

    for(auto &then : node.then_exprs)
        then.apply_visitor(*this);

    auto end = node.else_exprs.empty()
            ? last_insn_index() : emit_insn(opcode::brf);
    auto offset = end - start;
    edit_insn(start, opcode::brf_false, offset);

    for(auto &else_ : node.else_exprs)
        else_.apply_visitor(*this);

    if(!node.else_exprs.empty()) {
        start = end;
        end = last_insn_index();
        offset = end - start;
        edit_insn(start, opcode::brf, offset);
    }
}

void codegen::operator()(ast::while_ &node)
{
    auto start = last_insn_index();
    std::size_t br_idx = 0;

    // Для упрощения "вечного" цикла
    const bool forever_loop = typeid(bool) == node.cond.type()
        && boost::get<bool>(node.cond) != 0.0;

    if(!forever_loop) {
        node.cond.apply_visitor(*this);
        br_idx = emit_insn(opcode::brf_false);
    }

    for(auto &then : node.exprs)
        then.apply_visitor(*this);

    auto end = last_insn_index();
    auto offset = end - start;
    emit_insn(opcode::brb, offset);

    if(!forever_loop)
        edit_insn(br_idx, opcode::brf_false, offset - 1);
}

void codegen::operator()(ast::bin_op &node)
{
    node.rhs.apply_visitor(*this);
    node.lhs.apply_visitor(*this);
    emit_insn(opcode::call_op, static_cast<std::uint32_t>(node.k));
}

void codegen::operator()(ast::un_op &node)
{
    node.rhs.apply_visitor(*this);
    emit_insn(opcode::call_op, static_cast<std::uint32_t>(node.k));
}

} // namespace bytecode

} // namespace emel
