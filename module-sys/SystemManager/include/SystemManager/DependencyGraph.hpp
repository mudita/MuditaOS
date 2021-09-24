// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/ServiceCreator.hpp>

#include <memory>
#include <vector>

namespace sys
{
    class BaseServiceCreator; // Forward declaration

    namespace graph
    {
        using Node  = std::reference_wrapper<BaseServiceCreator>;
        using Nodes = std::vector<Node>;

        Nodes nodesFrom(const std::vector<std::unique_ptr<BaseServiceCreator>> &services);
    } // namespace graph

    class DependencySortingStrategy
    {
      public:
        virtual ~DependencySortingStrategy() noexcept = default;

        [[nodiscard]] virtual auto sort(const graph::Nodes &nodes) -> graph::Nodes = 0;
    };

    class DependencyGraph
    {
      public:
        DependencyGraph(graph::Nodes nodes, std::unique_ptr<DependencySortingStrategy> &&strategy);

        [[nodiscard]] auto sort() const -> graph::Nodes;

      private:
        graph::Nodes nodes;
        std::unique_ptr<DependencySortingStrategy> strategy;
    };
} // namespace sys
