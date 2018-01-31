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

#include <experimental/any>
#include <experimental/optional>
#include <stack>
#include <array>

namespace emel { namespace runtime {

using any = std::experimental::any;
using std::experimental::any_cast;

template <typename Tp>
  using optional = std::experimental::optional<Tp>;

struct frame {
    const std::vector<value_type> &const_pool;
    insn_array::const_iterator pc;
    const insn_array::const_iterator end_pc;
    std::vector<any> locals;
    std::deque<any> stack;

    frame(const std::vector<value_type> &const_pool,
          insn_array::const_iterator pc,
          insn_array::const_iterator end_pc,
          std::size_t /*locals_size*/, std::size_t /*stack_size*/)
        : const_pool(const_pool), pc(pc), end_pc(end_pc)
        //, locals(locals_size), stack(stack_size)
    {
    }
};

class interp
{
protected:
    std::stack<frame> frame_stack;

public:
  template <typename... Args>
    explicit interp(Args &&...args) {
        frame_stack.emplace(std::forward<Args>(args)...);
    }

  template <typename... Args>
    void push_frame(Args &&...args) {
        frame &top = frame_stack.top();
        ++top.pc;
        assert(top.pc != top.end_pc);
        frame_stack.emplace(std::forward<Args>(args)...);
    }

    void drop_frame()
    {
        frame_stack.pop();
    }

    any run() {
        any ret_value;

        while(true) {
            frame &top = frame_stack.top();

            if(top.pc == top.end_pc)
                break;

            opcode op;
            std::uint32_t arg;

            std::tie(op, arg) = insn_decode(*top.pc++);

            switch (op) {
                case opcode::push_const: {
                    const auto &value = top.const_pool.at(arg);
                    switch(value.which()) {
                        case 0: top.stack.push_front(any()); break;
                        case 1: top.stack.push_front(boost::get<std::string>(value)); break;
                        case 2: top.stack.push_front(boost::get<double>(value)); break;
                        case 3: top.stack.push_front(boost::get<bool>(value)); break;
                        default: assert(false);
                    }
                }
                    break;

                case opcode::call_op: {
                    const auto kind = static_cast<op_kind>(arg);
                    if(op_kind::not_ == kind || op_kind::neg == kind) {
                        assert(top.stack.size() > 0);
                        const auto rhs = any_cast<double>(top.stack.at(0));
                        top.stack.pop_front();

                        switch(static_cast<op_kind>(arg)) {
                            case op_kind::not_: top.stack.push_front(!rhs); break;
                            case op_kind::neg: top.stack.push_front(-rhs); break;
                            default: assert(false);
                        }

                    } else {
                        assert(top.stack.size() > 1);
                        const auto lhs = any_cast<double>(top.stack.at(0));
                        const auto rhs = any_cast<double>(top.stack.at(1));
                        top.stack.pop_front();
                        top.stack.pop_front();

                        optional<bool> bres;
                        optional<double> dres;

                        switch(static_cast<op_kind>(arg)) {
                            case op_kind::or_: bres = lhs || rhs; break;
                            case op_kind::xor_: bres = !lhs != !rhs; break;
                            case op_kind::and_: bres = lhs && rhs; break;
                            case op_kind::eq: bres = lhs == rhs; break;
                            case op_kind::ne: bres = lhs != rhs; break;
                            case op_kind::lt: bres = lhs < rhs; break;
                            case op_kind::gt: bres = lhs > rhs; break;
                            case op_kind::lte: bres = lhs <= rhs; break;
                            case op_kind::gte: bres = lhs >= rhs; break;
                            case op_kind::add: dres = lhs + rhs; break;
                            case op_kind::sub: dres = lhs - rhs; break;
                            case op_kind::mul: dres = lhs * rhs; break;
                            case op_kind::div: dres = lhs / rhs; break; // TODO div by zero
                            default: assert(false);
                        }

                        if(bres)
                            top.stack.push_front(bres.value());
                        else
                            top.stack.push_front(dres.value());
                    }
                }
                    break;

                case opcode::ret:
                    if(arg > 0) {
                        assert(top.stack.size() > 0);
                        ret_value = std::move(top.stack.front());
                    }
                    drop_frame();
                    return ret_value;

                default:
                    break;
            }
        }

        return ret_value;
    }
};

} // namespace runtime

} // namespace emel

#endif // INTERP_H
