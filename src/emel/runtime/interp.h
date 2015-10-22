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

struct frame {
    const std::vector<value_type> &const_pool;
    insn_array::const_iterator pc;
    const insn_array::const_iterator end_pc;
    std::vector<object, boost::pool_allocator<object>> locals, stack;

    frame(const std::vector<value_type> &const_pool,
          insn_array::const_iterator pc,
          insn_array::const_iterator end_pc,
          std::size_t locals_size, std::size_t stack_size)
        : const_pool(const_pool), pc(pc), end_pc(end_pc)
    {
        locals.reserve(locals_size);
        stack.reserve(stack_size);
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
        frame_stack.emplace(std::forward<Args>(args)...);
    }

    void drop_frame()
    {
        frame_stack.pop();
    }

    object run() {
        object ret_value;

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
                        case 0: top.stack.push_back(object()); break;
                        case 1: top.stack.push_back(boost::get<std::string>(value)); break;
                        case 2: top.stack.push_back(boost::get<double>(value)); break;
                        case 3: top.stack.push_back(boost::get<bool>(value)); break;
                        default: assert(false);
                    }
                }
                    break;

                case opcode::call_op: {
                    const auto kind = static_cast<op_kind>(arg);
                    if(op_kind::not_ == kind || op_kind::neg == kind) {
                        assert(top.stack.size() > 0);
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

                case opcode::ret:
                    if(arg > 0) {
                        assert(top.stack.size() > 0);
                        ret_value = std::move(top.stack.back());
                    }
                    drop_frame();

                    if(frame_stack.empty())
                        return ret_value;
                    else
                        frame_stack.top().stack.push_back(std::move(ret_value));

                    break;

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
