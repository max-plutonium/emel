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
#ifndef INTERP_H
#define INTERP_H

#include "../opcodes.h"
#include "object.h"

#include <vector>
#include <stack>
#include <array>

namespace emel { namespace runtime {

struct frame : public object {
    const std::vector<value_type> &const_pool;
    insn_array::const_iterator pc;
    const insn_array::const_iterator start_pc, end_pc;
    std::vector<object> locals, stack;
    std::shared_ptr<frame> super_frame, caller_frame;

    frame(const std::vector<value_type> &const_pool,
          insn_array::const_iterator start_pc,
          insn_array::const_iterator end_pc,
          std::size_t locals_size, std::size_t stack_size,
          std::shared_ptr<frame> super_frame = std::shared_ptr<frame>())
        : const_pool(const_pool), pc(start_pc), start_pc(start_pc), end_pc(end_pc)
        , locals(locals_size)
        , super_frame(super_frame)
    {
        stack.reserve(stack_size);
    }

    void set_caller(std::shared_ptr<frame> caller) {
        caller_frame = std::move(caller);
    }

    void drop_caller() {
        caller_frame.reset();
    }
};

class interp
{
protected:
    std::shared_ptr<frame> top_frame;

public:
  template <typename... Args>
    explicit interp(Args &&...args) {
        top_frame = std::make_shared<frame>(std::forward<Args>(args)...);
    }

  template <typename... Args>
    void push_frame(Args &&...args) {
        auto callee_frame = std::make_shared<frame>(std::forward<Args>(args)...);
        callee_frame->set_caller(std::move(top_frame));
        top_frame = callee_frame;
    }

    void drop_frame()
    {
        auto callee_frame = top_frame;
        top_frame = top_frame->caller_frame;
        callee_frame->drop_caller();
    }

    object run() {
        object ret_value;

        while(true) {
            frame &top = *top_frame;

            assert(top.start_pc <= top.pc);
            assert(top.pc < top.end_pc);

            opcode op;
            std::uint32_t arg;

            std::tie(op, arg) = insn_decode(*top.pc);

            switch (op) {
                case opcode::pop:
                    if(!arg) arg = 1;
                    assert(top.stack.size() >= arg);
                    while(arg--)
                        top.stack.pop_back();
                    break;

                case opcode::dup:
                    assert(!top.stack.empty());
                    if(!arg) arg = 1;
                    while(arg--)
                        top.stack.push_back(top.stack.back());
                    break;

                case opcode::swap:
                    assert(top.stack.size() > 1);
                    std::swap(top.stack.back(), *(top.stack.end() - 2));
                    break;

                case opcode::ret:
                    if(arg > 0) {
                        assert(!top.stack.empty());
                        ret_value = std::move(top.stack.back());
                    }

                    drop_frame();

                    if(!top_frame)
                        return ret_value;
                    else
                        top_frame->stack.push_back(std::move(ret_value));

                    break;

                case opcode::push:
                    top.stack.push_back(object(double(arg)));
                    break;

                case opcode::push_const: {
                    assert(top.const_pool.size() > arg);
                    const auto &value = top.const_pool[arg];
                    switch(value.which()) {
                        case 0: top.stack.push_back(object()); break;
                        case 1: top.stack.push_back(boost::get<std::string>(value)); break;
                        case 2: top.stack.push_back(boost::get<double>(value)); break;
                        case 3: top.stack.push_back(boost::get<bool>(value)); break;
                        default: assert(false);
                    }
                }
                    break;

                case opcode::push_local:
                    assert(top.locals.size() > arg);
                    top.stack.push_back(top.locals[arg]);
                    break;

                case opcode::load_local:
                    assert(!top.stack.empty());
                    assert(top.locals.size() > arg);
                    top.locals[arg] = std::move(top.stack.back());
                    top.stack.pop_back();
                    break;

                case opcode::call_op: {
                    const auto kind = static_cast<op_kind>(arg);
                    if(op_kind::not_ == kind || op_kind::neg == kind) {
                        assert(!top.stack.empty());
                        const object top_object = std::move(top.stack.back());
                        top.stack.pop_back();

                        switch(static_cast<op_kind>(arg)) {
                            case op_kind::not_: top.stack.push_back(!top_object); break;
                            case op_kind::neg: top.stack.push_back(- (double) top_object); break;
                            default: assert(false);
                        }

                    } else {
                        assert(top.stack.size() > 1);
                        const object lhs = std::move(top.stack.back());
                        top.stack.pop_back();
                        const object rhs = std::move(top.stack.back());
                        top.stack.pop_back();

                        object res;

                        switch(static_cast<op_kind>(arg)) {
                            case op_kind::or_: res = lhs || rhs; break;
                            case op_kind::xor_: res = !lhs != !rhs; break;
                            case op_kind::and_: res = lhs && rhs; break;
                            case op_kind::eq: res = lhs == rhs; break;
                            case op_kind::ne: res = lhs != rhs; break;
                            case op_kind::lt: res = lhs < rhs; break;
                            case op_kind::gt: res = lhs > rhs; break;
                            case op_kind::lte: res = lhs <= rhs; break;
                            case op_kind::gte: res = lhs >= rhs; break;
                            case op_kind::add: res = lhs + rhs; break;
                            case op_kind::sub: res = lhs - rhs; break;
                            case op_kind::mul: res = lhs * rhs; break;
                            case op_kind::div: res = lhs / rhs; break;
                            default: assert(false);
                        }

                        if(!res.empty())
                            top.stack.push_back(std::move(res));
                    }
                }
                    break;

                case opcode::brf:
                    assert(top.pc + arg < top.end_pc);
                    top.pc += arg;
                    continue;

                case opcode::brb:
                    assert(top.start_pc <= top.pc - arg);
                    top.pc -= arg;
                    continue;

                case opcode::brf_true:
                    assert(!top.stack.empty());
                    assert(top.pc + arg < top.end_pc);
                    if(static_cast<bool>(top.stack.back())) {
                        top.pc += arg;
                        top.stack.pop_back();
                        continue;
                    }

                    top.stack.pop_back();
                    break;

                case opcode::brf_false:
                    assert(!top.stack.empty());
                    assert(top.pc + arg < top.end_pc);
                    if(!static_cast<bool>(top.stack.back())) {
                        top.pc += arg;
                        top.stack.pop_back();
                        continue;
                    }

                    top.stack.pop_back();
                    break;

                case opcode::brb_true:
                    assert(!top.stack.empty());
                    assert(top.start_pc <= top.pc - arg);
                    if(static_cast<bool>(top.stack.back())) {
                        top.pc -= arg;
                        top.stack.pop_back();
                        continue;
                    }

                    top.stack.pop_back();
                    break;

                case opcode::brb_false:
                    assert(!top.stack.empty());
                    assert(top.start_pc <= top.pc - arg);
                    if(!static_cast<bool>(top.stack.back())) {
                        top.pc -= arg;
                        top.stack.pop_back();
                        continue;
                    }

                    top.stack.pop_back();
                    break;

                default:
                    break;
            }

            ++top.pc;
        }

        return ret_value;
    }
};

} // namespace runtime

} // namespace emel

#endif // INTERP_H
