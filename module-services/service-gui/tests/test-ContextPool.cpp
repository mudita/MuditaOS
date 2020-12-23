// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include "ContextPool.hpp"

#include <chrono>
#include <thread>

using namespace service::gui;

class TestSynchronizationMechanism : public SynchronizationMechanism
{
  public:
    void wait(cpp_freertos::Mutex &lock, std::chrono::milliseconds timeout, Predicate predicate) override
    {
        while (!predicate()) {
            lock.Unlock();
            lock.Unlock();
        }
    }

    void notify() override
    {
        // Nothing to do.
    }
};

TEST_CASE("ContextPoolTests")
{
    const ::gui::Size displaySize{1, 1};
    constexpr auto ContextPoolCapacity = 1;
    constexpr auto ContextPoolFirstId  = 0;
    ContextPool contextPool{displaySize, ContextPoolCapacity, std::make_unique<TestSynchronizationMechanism>()};

    SECTION("Borrow context when all are used")
    {
        const auto [contextId, context] = contextPool.borrowContext();
        REQUIRE(contextId == ContextPoolFirstId);

        std::thread thr{[&contextPool, id = contextId]() {
            std::this_thread::sleep_for(std::chrono::milliseconds{500});
            contextPool.returnContext(id);
        }};
        const auto [nextContextId, nextContext] = contextPool.borrowContext();
        REQUIRE(contextId == nextContextId);

        if (thr.joinable()) {
            thr.join();
        }
    }

    SECTION("Return unused context")
    {
        REQUIRE_NOTHROW(contextPool.returnContext(0));
    }

    SECTION("Return invalid context id")
    {
        REQUIRE_NOTHROW(contextPool.returnContext(10));
    }
}
