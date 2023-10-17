// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include "Service/ServiceCreator.hpp"
#include "SystemManager/DependencyGraph.hpp"
#include "SystemManager/graph/TopologicalSort.hpp"

using namespace sys;
using graph::TopologicalSort;

class MockedServiceCreator : public BaseServiceCreator
{
  public:
    using BaseServiceCreator::BaseServiceCreator;

    std::shared_ptr<Service> create() const override
    {
        return nullptr;
    }
};

ServiceManifest createManifest(ServiceManifest::ServiceName name, std::vector<ServiceManifest::ServiceName> deps)
{
    ServiceManifest manifest;
    manifest.name         = std::move(name);
    manifest.dependencies = std::move(deps);
    return manifest;
}

TEST_CASE("Given Dependency Graph When topological sort empty graph then verify if result is empty")
{
    DependencyGraph graph{{}, std::make_unique<TopologicalSort>()};

    const auto &sorted = graph.sort();

    REQUIRE(sorted.empty());
}

TEST_CASE("Given Dependency Graph When topological sort without dependencies then verify order")
{
    std::vector<std::unique_ptr<BaseServiceCreator>> services;
    services.push_back(std::make_unique<MockedServiceCreator>(createManifest("S1", {})));
    services.push_back(std::make_unique<MockedServiceCreator>(createManifest("S2", {})));
    services.push_back(std::make_unique<MockedServiceCreator>(createManifest("S3", {})));
    services.push_back(std::make_unique<MockedServiceCreator>(createManifest("S4", {})));

    // Graph:
    // S1 S2 S3 S4
    DependencyGraph graph{graph::nodesFrom(services), std::make_unique<TopologicalSort>()};

    const auto &sorted = graph.sort();

    REQUIRE(sorted[0].get().getName() == "S1");
    REQUIRE(sorted[1].get().getName() == "S2");
    REQUIRE(sorted[2].get().getName() == "S3");
    REQUIRE(sorted[3].get().getName() == "S4");
}

TEST_CASE("Given Dependency Graph When topological sort simple case then verify order")
{
    std::vector<std::unique_ptr<BaseServiceCreator>> services;
    services.push_back(std::make_unique<MockedServiceCreator>(createManifest("S1", {"S2"})));
    services.push_back(std::make_unique<MockedServiceCreator>(createManifest("S2", {"S3"})));
    services.push_back(std::make_unique<MockedServiceCreator>(createManifest("S3", {"S4"})));
    services.push_back(std::make_unique<MockedServiceCreator>(createManifest("S4", {})));

    // Graph:
    // S1 -> S2 -> S3 -> S4
    DependencyGraph graph{graph::nodesFrom(services), std::make_unique<TopologicalSort>()};

    const auto &sorted = graph.sort();

    REQUIRE(sorted[0].get().getName() == "S4");
    REQUIRE(sorted[1].get().getName() == "S3");
    REQUIRE(sorted[2].get().getName() == "S2");
    REQUIRE(sorted[3].get().getName() == "S1");
}

TEST_CASE("Given Dependency Graph When topological sort all depending on one then verify order")
{
    std::vector<std::unique_ptr<BaseServiceCreator>> services;
    services.push_back(std::make_unique<MockedServiceCreator>(createManifest("S1", {"S2"})));
    services.push_back(std::make_unique<MockedServiceCreator>(createManifest("S2", {})));
    services.push_back(std::make_unique<MockedServiceCreator>(createManifest("S3", {"S2"})));
    services.push_back(std::make_unique<MockedServiceCreator>(createManifest("S4", {"S2"})));

    // Graph:
    // S1 -> S2
    //     / ^
    //   S3  |
    //      S4
    DependencyGraph graph{graph::nodesFrom(services), std::make_unique<TopologicalSort>()};

    const auto &sorted = graph.sort();

    REQUIRE(sorted[0].get().getName() == "S2");
    REQUIRE(sorted[1].get().getName() == "S1");
    REQUIRE(sorted[2].get().getName() == "S3");
    REQUIRE(sorted[3].get().getName() == "S4");
}

TEST_CASE("Given Dependency Graph When topological sort advanced case then verify order 1")
{
    std::vector<std::unique_ptr<BaseServiceCreator>> services;
    services.push_back(std::make_unique<MockedServiceCreator>(createManifest("S1", {"S2", "S3"})));
    services.push_back(std::make_unique<MockedServiceCreator>(createManifest("S2", {"S4"})));
    services.push_back(std::make_unique<MockedServiceCreator>(createManifest("S3", {"S5"})));
    services.push_back(std::make_unique<MockedServiceCreator>(createManifest("S4", {"S6"})));
    services.push_back(std::make_unique<MockedServiceCreator>(createManifest("S5", {"S6"})));
    services.push_back(std::make_unique<MockedServiceCreator>(createManifest("S6", {})));

    // Graph:
    //    -> S2 -> S4 -->
    // S1 -> S3 -> S5 -> S6
    DependencyGraph graph{graph::nodesFrom(services), std::make_unique<TopologicalSort>()};

    const auto &sorted = graph.sort();

    REQUIRE(sorted[0].get().getName() == "S6");
    REQUIRE(sorted[1].get().getName() == "S4");
    REQUIRE(sorted[2].get().getName() == "S2");
    REQUIRE(sorted[3].get().getName() == "S5");
    REQUIRE(sorted[4].get().getName() == "S3");
    REQUIRE(sorted[5].get().getName() == "S1");
}

TEST_CASE("Given Dependency Graph When topological sort advanced case then verify order 2")
{
    std::vector<std::unique_ptr<BaseServiceCreator>> services;
    services.push_back(std::make_unique<MockedServiceCreator>(createManifest("S1", {"S2", "S3", "S4"})));
    services.push_back(std::make_unique<MockedServiceCreator>(createManifest("S2", {})));
    services.push_back(std::make_unique<MockedServiceCreator>(createManifest("S3", {"S5", "S6"})));
    services.push_back(std::make_unique<MockedServiceCreator>(createManifest("S4", {"S6"})));
    services.push_back(std::make_unique<MockedServiceCreator>(createManifest("S5", {})));
    services.push_back(std::make_unique<MockedServiceCreator>(createManifest("S6", {})));

    // Graph:
    //    --> S4 ---\>
    //   /     ----> S6
    //  /     /
    // S1 -> S3 -> S5
    //   \-> S2
    DependencyGraph graph{graph::nodesFrom(services), std::make_unique<TopologicalSort>()};

    const auto &sorted = graph.sort();

    REQUIRE(sorted[0].get().getName() == "S2");
    REQUIRE(sorted[1].get().getName() == "S5");
    REQUIRE(sorted[2].get().getName() == "S6");
    REQUIRE(sorted[3].get().getName() == "S3");
    REQUIRE(sorted[4].get().getName() == "S4");
    REQUIRE(sorted[5].get().getName() == "S1");
}

TEST_CASE("Given Dependency Graph When topological sort on directed cyclic graph then verify order")
{
    std::vector<std::unique_ptr<BaseServiceCreator>> services;
    services.push_back(std::make_unique<MockedServiceCreator>(createManifest("S1", {"S2"})));
    services.push_back(std::make_unique<MockedServiceCreator>(createManifest("S2", {"S3"})));
    services.push_back(std::make_unique<MockedServiceCreator>(createManifest("S3", {"S1"})));

    // Graph:
    // S1 -> S2 -> S3
    //  \---------/
    DependencyGraph graph{graph::nodesFrom(services), std::make_unique<TopologicalSort>()};

    const auto &sorted = graph.sort();

    // The order is not correct. Dependency Graph doesn't handle cyclic graphs properly
    REQUIRE(sorted[0].get().getName() == "S3");
    REQUIRE(sorted[1].get().getName() == "S2");
    REQUIRE(sorted[2].get().getName() == "S1");
}
