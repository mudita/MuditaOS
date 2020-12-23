// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include "ContextPool.hpp"
#include "MockedSynchronizationMechanism.hpp"

#include <chrono>
#include <thread>

using namespace service::gui;

TEST_CASE("ContextPoolTests")
{
    const ::gui::Size displaySize{1, 1};
    constexpr auto ContextPoolCapacity = 1;
    constexpr auto ContextPoolFirstId  = 0;
    ContextPool contextPool{displaySize, ContextPoolCapacity, std::make_unique<MockedSynchronizationMechanism>()};
    REQUIRE(!contextPool.isAnyContextLocked());

    SECTION("Borrow context when all are used")
    {
        const auto [contextId, context] = contextPool.borrowContext();
        REQUIRE(contextId == ContextPoolFirstId);
        REQUIRE(contextPool.isAnyContextLocked());

        std::thread thr{[&contextPool, id = contextId]() {
            std::this_thread::sleep_for(std::chrono::milliseconds{500});
            contextPool.returnContext(id);
        }};
        const auto [nextContextId, nextContext] = contextPool.borrowContext();
        REQUIRE(contextId == nextContextId);

        if (thr.joinable()) {
            thr.join();
        }
        contextPool.returnContext(nextContextId);
        REQUIRE(!contextPool.isAnyContextLocked());
    }

    SECTION("Return unused context")
    {
        REQUIRE(!contextPool.isAnyContextLocked());
        REQUIRE_NOTHROW(contextPool.returnContext(0));
    }

    SECTION("Return invalid context id")
    {
        REQUIRE(!contextPool.isAnyContextLocked());
        REQUIRE_NOTHROW(contextPool.returnContext(10));
    }
}
