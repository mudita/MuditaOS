// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include "DrawCommandsQueue.hpp"
#include "MockedSynchronizationMechanism.hpp"

#include <chrono>
#include <thread>

using namespace service::gui;

TEST_CASE("DrawCommandsQueueTests")
{
    constexpr auto ExpectedQueueSize = 4U;
    DrawCommandsQueue queue{ExpectedQueueSize, std::make_unique<MockedSynchronizationMechanism>()};

    SECTION("Enqueue")
    {
        queue.enqueue(DrawCommandsQueue::QueueItem{});
        REQUIRE(queue.size() == 1U);
    }

    SECTION("Single-thread dequeue")
    {
        queue.enqueue(DrawCommandsQueue::QueueItem{});
        REQUIRE(queue.size() == 1U);

        const auto item = queue.dequeue();
        REQUIRE(item.commands.empty());
        REQUIRE(item.refreshMode == ::gui::RefreshModes::GUI_REFRESH_FAST);
        REQUIRE(queue.size() == 0);
    }

    SECTION("Multi-thread dequeue")
    {
        std::thread thr{[&queue]() {
            std::this_thread::sleep_for(std::chrono::milliseconds{100});
            queue.enqueue(DrawCommandsQueue::QueueItem{});
        }};

        const auto item = queue.dequeue();
        if (thr.joinable()) {
            thr.join();
        }

        REQUIRE(item.commands.empty());
        REQUIRE(item.refreshMode == ::gui::RefreshModes::GUI_REFRESH_FAST);
        REQUIRE(queue.size() == 0U);
    }

    SECTION("Clear")
    {
        queue.enqueue(DrawCommandsQueue::QueueItem{});
        queue.enqueue(DrawCommandsQueue::QueueItem{});
        REQUIRE(queue.size() == 2U);

        queue.clear();
        REQUIRE(queue.size() == 0);
    }

    SECTION("Clear and get max refresh mode")
    {
        queue.enqueue(DrawCommandsQueue::QueueItem{});
        queue.enqueue(DrawCommandsQueue::QueueItem{{}, ::gui::RefreshModes::GUI_REFRESH_DEEP});
        REQUIRE(queue.size() == 2U);

        const auto maxRefreshMode = queue.getMaxRefreshModeAndClear();
        REQUIRE(queue.size() == 0);
        REQUIRE(maxRefreshMode == ::gui::RefreshModes::GUI_REFRESH_DEEP);
    }
}
