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

vertex_descriptor add_vertex(const std::string &name,
    std::shared_ptr<node> n, graph_type &graph)
{
    vertex_property vertex_prop;
    boost::get_property_value(vertex_prop, boost::vertex_name) = name;
    boost::get_property_value(vertex_prop, boost::vertex_semantic) = std::move(n);
    return boost::add_vertex(vertex_prop, graph);
}

vertex_descriptor add_vertex(std::shared_ptr<node> n, graph_type &graph)
{
    vertex_property vertex_prop;
    boost::get_property_value(vertex_prop, boost::vertex_semantic) = std::move(n);
    return boost::add_vertex(vertex_prop, graph);
}

} // namespace semantic

} // namespace emel
