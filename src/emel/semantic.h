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
#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "opcodes.h"

#include <boost/graph/adjacency_list.hpp>

namespace boost {
    struct dequeS { };

  template <typename Tp>
    struct container_gen<dequeS, Tp> {
        using type = std::deque<Tp>;
    };

  template <>
    struct parallel_edge_traits<dequeS> {
        using type = allow_parallel_edge_tag;
    };

    struct deque_tag :
        virtual public graph_detail::random_access_container_tag,
        virtual public graph_detail::back_insertion_sequence_tag
        // this causes problems for push_dispatch...
        //   virtual public front_insertion_sequence_tag
    { };

  template <typename Tp, typename Alloc>
    deque_tag container_category(const std::deque<Tp, Alloc> &)
      { return deque_tag(); }

  template <typename Tp, typename Alloc>
    graph_detail::unstable_tag iterator_stability(const std::deque<Tp, Alloc> &)
      { return graph_detail::unstable_tag(); }

    namespace graph_detail {
      template <typename Tp, typename Alloc>
        struct container_traits<std::deque<Tp, Alloc>> {
            using category = deque_tag;
            using iterator_stability = unstable_tag;
        };

    } // namespace graph_detail

    namespace detail {
      template <>
        struct is_random_access<dequeS> {
            enum { value = true };
            using type = mpl::true_;
        };

    } // namespace detail

    enum vertex_semantic_t { vertex_semantic = 1000 };
    BOOST_INSTALL_PROPERTY(vertex, semantic);

    enum vertex_insns_t { vertex_insns = 1001 };
    BOOST_INSTALL_PROPERTY(vertex, insns);

    enum vertex_value_t { vertex_value = 1002 };
    BOOST_INSTALL_PROPERTY(vertex, value);

    enum vertex_op_t { vertex_op = 1003 };
    BOOST_INSTALL_PROPERTY(vertex, op);

    enum edge_content_t { edge_content = 1004 };
    BOOST_INSTALL_PROPERTY(edge, content);

} // namespace boost

namespace emel { namespace semantic {

struct node {
    enum type {
        is_module,
        is_class,
        is_interface,
        is_field,
        is_function,
        is_block,
        is_expr,
        is_variable,
        is_value
    };

    enum kind {
        is_regular = 0x0,
        is_ref = 0x1,
        is_param = 0x2
    };

    enum attribute {
        is_global = 0x0,
        is_local = 0x1,
        is_virtual = 0x2,
        is_static = 0x4,
        is_const = 0x8,
        is_pure = 0x10,
        is_abstract = 0x20,
        is_native = 0x30
    };

    enum visibility {
        vis_public,
        vis_module,
        vis_protected,
        vis_private
    };

    type t = is_value;
    kind k = is_regular;
    attribute a = is_local;
    visibility v = vis_private;
    std::uint32_t nr_slots = 0;
    std::uint32_t nr_stack = 0;
    std::uint32_t nr_args = 0;
};

enum edge_kind {
    inheritance,
    content, usage
};

using vertex_property =
    boost::property<boost::vertex_name_t, std::string,
        boost::property<boost::vertex_semantic_t, node,
            boost::property<boost::vertex_insns_t, insn_array,
                boost::property<boost::vertex_value_t, value_type,
                    boost::property<boost::vertex_op_t, op_kind>>>>>;

using edge_property =
    boost::property<boost::edge_content_t, edge_kind>;

using graph_type = boost::adjacency_list<
    boost::dequeS, boost::dequeS, boost::bidirectionalS,
    vertex_property, edge_property>;

using names_map_type =
    boost::property_map<graph_type, boost::vertex_name_t>::type;

using nodes_map_type =
    boost::property_map<graph_type, boost::vertex_semantic_t>::type;

using insns_map_type =
    boost::property_map<graph_type, boost::vertex_insns_t>::type;

using values_map_type =
    boost::property_map<graph_type, boost::vertex_value_t>::type;

using ops_map_type =
    boost::property_map<graph_type, boost::vertex_op_t>::type;

using edge_content_map_type =
    boost::property_map<graph_type, boost::edge_content_t>::type;

using vertex_descriptor =
    boost::graph_traits<graph_type>::vertex_descriptor;

using edge_descriptor =
    boost::graph_traits<graph_type>::edge_descriptor;

using adjacency_iterator =
    boost::graph_traits<graph_type>::adjacency_iterator;

using inv_adjacency_iterator =
    boost::inv_adjacency_iterator_generator<graph_type>::type;

using out_edge_iterator =
    boost::graph_traits<graph_type>::out_edge_iterator;

using in_edge_iterator =
    boost::graph_traits<graph_type>::in_edge_iterator;

using vertex_iterator =
    boost::graph_traits<graph_type>::vertex_iterator;

using edge_iterator =
    boost::graph_traits<graph_type>::edge_iterator;

} // namespace semantic

} // namespace emel

#endif // SEMANTIC_H
