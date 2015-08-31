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
#include "const_pool_manager.h"
#include "symbols_manager.h"
#include "insns_manager.h"

#include <boost/lexical_cast.hpp>

namespace emel { namespace bytecode {

class codegen : public boost::static_visitor<void>
        , protected const_pool_manager, protected symbols_manager
        , protected insns_manager
{
protected:
    class_info result;
    bool in_ctor = false;

    std::string class_name() const;
    std::string base_name() const;

public:
    explicit codegen(symbols &syms);

    class_info generate(ast::class_ &node);

    void operator()(const std::string &value);

    void operator()(double value);

    void operator()(bool value);

    void operator()(ast::class_ &node);

    void operator()(ast::method &node);

    void operator()(ast::while_ &)
    {
    }

    void operator()(ast::for_ &)
    {
    }

    void operator()(ast::if_ &)
    {
    }

    void operator()(ast::case_ &)
    {
    }

    void operator()(ast::switch_ &)
    {
    }

    void operator()(ast::continue_ &)
    {
    }

    void operator()(ast::break_ &)
    {
    }

    void operator()(ast::return_ &)
    {
    }

    void operator()(ast::try_ &)
    {
    }

    void operator()(ast::assign &node);

    void operator()(ast::ternary &node);

    void operator()(ast::bin_op &node);

    void operator()(ast::variable &)
    {
    }

    void operator()(ast::un_op &node);

    void operator()(ast::call &)
    {
    }
};

} // namespace bytecode

} // namespace emel

#endif // CODEGEN_H
