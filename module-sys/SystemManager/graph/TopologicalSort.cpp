// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TopologicalSort.hpp"

#include <algorithm>
#include <cassert>

namespace sys::graph
{
    auto TopologicalSort::sort(const graph::Nodes &nodes) -> graph::Nodes
    {
        std::vector<std::string_view> sortedNames;
        auto edges   = createEdges(nodes);
        auto visited = createVisitedMap(nodes);
        auto layer   = 1;
        for (const auto &node : nodes) {
            const auto &nodeName = node.get().getName();
            if (visited[nodeName] == false) {
                visit(nodeName, edges, visited, layer, sortedNames);
            }
        }
        return prepareOutput(nodes, sortedNames);
    }

    void TopologicalSort::visit(
        std::string_view node, Edges &edges, VisitedMap &visited, int layer, std::vector<std::string_view> &out)
    {
        visited[node] = true;
        for (const auto &dependency : edges[node]) {
            if (visited[dependency] == false) {
                layer += 1;
                visit(dependency, edges, visited, layer, out);
            }
        }
        out.emplace_back(node);
    }

    auto TopologicalSort::createEdges(const graph::Nodes &nodes) const -> Edges
    {
        Edges edges;
        for (const auto &node : nodes) {
            const auto &nodeName     = node.get().getName();
            const auto &dependencies = node.get().getDependencies();
            for (const auto &dependency : dependencies) {
                edges[nodeName].push_back(dependency);
            }
        }
        return edges;
    }

    auto TopologicalSort::createVisitedMap(const graph::Nodes &nodes) const -> VisitedMap
    {
        VisitedMap visited;
        std::for_each(
            nodes.begin(), nodes.end(), [&visited](const auto &node) { visited[node.get().getName()] = false; });
        return visited;
    }

    auto TopologicalSort::prepareOutput(const graph::Nodes &nodes, const std::vector<std::string_view> &sorted) const
        -> graph::Nodes
    {
        graph::Nodes output;
        output.reserve(sorted.size());
        for (const auto &name : sorted) {
            const auto it = std::find_if(
                nodes.cbegin(), nodes.cend(), [&name](const auto &node) { return node.get().getName() == name; });

            // Either nodes have been modified by mistake or there's no particular dependency in the nodes container.
            assert(it != nodes.end());
            output.emplace_back(*it);
        }
        return output;
    }
} // namespace sys::graph
