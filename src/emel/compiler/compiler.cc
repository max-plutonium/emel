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
#include "compiler.h"

#include <boost/graph/topological_sort.hpp>

namespace emel { namespace compiler {

/*static*/
std::vector<ast::class_>
compiler::topological_sort(std::vector<ast::class_> &classes)
{
    std::vector<ast::class_> result;

    using graph_type = boost::adjacency_list<
        boost::vecS, boost::vecS, boost::directedS,
            boost::property<boost::vertex_name_t, std::string>>;

    using class_names_map_type =
        boost::property_map<graph_type, boost::vertex_name_t>::type;

    graph_type dep_graph;

    std::unordered_map<std::string,
        boost::graph_traits<graph_type>::vertex_descriptor>
            descriptor_map(classes.size());

    std::unordered_map<std::string, std::vector<ast::class_>::iterator>
        class_map(classes.size());

    for(auto it = classes.begin(); it != classes.end(); ++it)
        class_map.emplace(it->name, it);

    for(ast::class_ &c : classes) {
        if(c.base_name.empty() && c.name != "Object")
            c.base_name = "Object";
        auto vertex_descr = boost::add_vertex(c.name, dep_graph);
        auto res = descriptor_map.emplace(c.name, vertex_descr);
        assert(res.second);
    }

    for(const ast::class_ &c : classes)
        if(!c.base_name.empty())
            boost::add_edge(descriptor_map[c.name],
                descriptor_map[c.base_name], dep_graph);

    class_names_map_type class_names_map = boost::get(boost::vertex_name, dep_graph);

    std::vector<boost::graph_traits<graph_type>::vertex_descriptor>
        sorted_vertices(boost::num_vertices(dep_graph));

    boost::topological_sort(dep_graph, std::begin(sorted_vertices));

    result.reserve(sorted_vertices.size());

    std::transform(
        sorted_vertices.begin(),
        sorted_vertices.end(),
        std::back_inserter(result),
        [&class_map, &class_names_map]
        (boost::graph_traits<graph_type>::vertex_descriptor vertex) {
            auto it = class_map.at(class_names_map[vertex]);
            return std::move(*it);
        });

    classes.erase(classes.begin(), classes.end());
    return result;
}

} // namespace compiler

} // namespace emel
