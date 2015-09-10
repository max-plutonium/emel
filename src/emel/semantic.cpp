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
#include "semantic.h"

namespace emel { namespace semantic {

vertex_descriptor create_module(const std::string &name, graph_type &graph)
{
    node module_node;
    module_node.t = node::is_module;
    module_node.k = node::is_regular;
    module_node.a = node::is_global;
    module_node.v = node::vis_public;

    vertex_property vertex_prop;
    boost::get_property_value(vertex_prop, boost::vertex_name) = name;
    boost::get_property_value(vertex_prop, boost::vertex_semantic) = module_node;
    return boost::add_vertex(vertex_prop, graph);
}

vertex_descriptor create_class(const std::string &name,
    node::visibility vis, graph_type &graph)
{
    node class_node;
    class_node.t = node::is_class;
    class_node.k = node::is_regular;
    class_node.a = node::is_global;
    class_node.v = vis;

    vertex_property vertex_prop;
    boost::get_property_value(vertex_prop, boost::vertex_name) = name;
    boost::get_property_value(vertex_prop, boost::vertex_semantic) = class_node;
    return boost::add_vertex(vertex_prop, graph);
}

vertex_descriptor create_field(const std::string &name,
    node::visibility vis, graph_type &graph)
{
    node field_node;
    field_node.t = node::is_field;
    field_node.k = node::is_regular;
    field_node.a = node::is_local;
    field_node.v = vis;

    vertex_property vertex_prop;
    boost::get_property_value(vertex_prop, boost::vertex_name) = name;
    boost::get_property_value(vertex_prop, boost::vertex_semantic) = field_node;
    return boost::add_vertex(vertex_prop, graph);
}

vertex_descriptor create_method(const std::string &name,
    node::visibility vis, std::size_t num_args, graph_type &graph)
{
    node method_node;
    method_node.t = node::is_function;
    method_node.k = node::is_regular;
    method_node.a = node::is_local;
    method_node.v = vis;
    method_node.nr_args = num_args;

    vertex_property vertex_prop;
    boost::get_property_value(vertex_prop, boost::vertex_name) = name;
    boost::get_property_value(vertex_prop, boost::vertex_semantic) = method_node;
    return boost::add_vertex(vertex_prop, graph);
}

vertex_descriptor create_param(const std::string &name,
    bool is_ref, graph_type &graph)
{
    node param_node;
    param_node.t = node::is_variable;
    param_node.k = node::is_param;
    if(is_ref)
        param_node.k = static_cast<node::kind>(param_node.k | node::is_ref);
    param_node.a = node::is_local;
    param_node.v = node::vis_private;

    vertex_property vertex_prop;
    boost::get_property_value(vertex_prop, boost::vertex_name) = name;
    boost::get_property_value(vertex_prop, boost::vertex_semantic) = param_node;
    return boost::add_vertex(vertex_prop, graph);
}

vertex_descriptor create_variable(const std::string &name,
    bool is_ref, graph_type &graph)
{
    node var_node;
    var_node.t = node::is_variable;
    var_node.k = node::is_regular;
    if(is_ref)
        var_node.k = static_cast<node::kind>(var_node.k | node::is_ref);
    var_node.a = node::is_local;
    var_node.v = node::vis_private;

    vertex_property vertex_prop;
    boost::get_property_value(vertex_prop, boost::vertex_name) = name;
    boost::get_property_value(vertex_prop, boost::vertex_semantic) = var_node;
    return boost::add_vertex(vertex_prop, graph);
}

vertex_descriptor create_expr(op_kind op, graph_type &graph)
{
    node expr_node;
    expr_node.t = node::is_expr;
    expr_node.k = node::is_regular;
    expr_node.a = node::is_local;
    expr_node.v = node::vis_private;

    vertex_property vertex_prop;
    boost::get_property_value(vertex_prop, boost::vertex_semantic) = expr_node;
    boost::get_property_value(vertex_prop, boost::vertex_op) = op;
    return boost::add_vertex(vertex_prop, graph);
}

vertex_descriptor create_value(value_type value, graph_type &graph)
{
    node value_node;
    value_node.t = node::is_value;
    value_node.k = node::is_regular;
    value_node.a = node::is_local;
    value_node.v = node::vis_private;

    vertex_property vertex_prop;
    boost::get_property_value(vertex_prop, boost::vertex_semantic) = value_node;
    boost::get_property_value(vertex_prop, boost::vertex_value) = value;
    return boost::add_vertex(vertex_prop, graph);
}

vertex_descriptor create_block(graph_type &graph)
{
    node block_node;
    block_node.t = node::is_block;
    block_node.k = node::is_regular;
    block_node.a = node::is_local;
    block_node.v = node::vis_private;

    vertex_property vertex_prop;
    boost::get_property_value(vertex_prop, boost::vertex_semantic) = block_node;
    return boost::add_vertex(vertex_prop, graph);
}

} // namespace semantic

} // namespace emel
