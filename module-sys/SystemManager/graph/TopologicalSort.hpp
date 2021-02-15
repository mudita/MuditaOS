// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SystemManager/DependencyGraph.hpp"

#include <list>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace sys::graph
{
    class TopologicalSort : public DependencySortingStrategy
    {
      public:
        [[nodiscard]] auto sort(const graph::Nodes &nodes) -> graph::Nodes override;

      private:
        using Edges      = std::unordered_map<std::string_view, std::list<std::string_view>>;
        using VisitedMap = std::unordered_map<std::string_view, bool>;

        [[nodiscard]] auto createEdges(const graph::Nodes &nodes) const -> Edges;
        [[nodiscard]] auto createVisitedMap(const graph::Nodes &nodes) const -> VisitedMap;
        void visit(
            std::string_view node, Edges &edges, VisitedMap &visited, int layer, std::vector<std::string_view> &out);

        [[nodiscard]] auto prepareOutput(const graph::Nodes &nodes, const std::vector<std::string_view> &sorted) const
            -> graph::Nodes;
    };
} // namespace sys::graph
