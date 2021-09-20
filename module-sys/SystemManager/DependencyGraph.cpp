// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <SystemManager/DependencyGraph.hpp>

#include <algorithm>
#include <cassert>

namespace sys
{
    namespace graph
    {
        Nodes nodesFrom(const std::vector<std::unique_ptr<BaseServiceCreator>> &services)
        {
            Nodes nodes;
            nodes.reserve(services.size());
            std::transform(services.begin(), services.end(), std::back_inserter(nodes), [](const auto &service) {
                return std::ref(*service);
            });
            return nodes;
        }
    } // namespace graph

    DependencyGraph::DependencyGraph(graph::Nodes nodes, std::unique_ptr<DependencySortingStrategy> &&strategy)
        : nodes{std::move(nodes)}, strategy{std::move(strategy)}
    {}

    auto DependencyGraph::sort() const -> graph::Nodes
    {
        return strategy->sort(nodes);
    }
} // namespace sys
