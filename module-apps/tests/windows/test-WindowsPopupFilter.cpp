// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WindowsPopupFilter.hpp"
#include "WindowsStack.hpp"
#include "mock/TestWindow.hpp"
#include "popups/data/PopupRequestParamsBase.hpp"
#include <catch2/catch.hpp>
#include <module-apps/apps-common/WindowsPopupQueue.hpp>

TEST_CASE("WindowsPopupQueue::isPermitted", "[!mayfail]")
{
    auto prp = gui::PopupRequestParams(gui::popup::ID::Alarm, gui::popup::popupDisposition(gui::popup::ID::Alarm));
    gui::popup::Filter filter;
    SECTION("filter is ok, there is not stack")
    {
        filter.addAppDependentFilter([](const gui::PopupRequestParams &) -> bool { return true; });
        REQUIRE(filter.isPermitted(prp));
    }

    SECTION("just stack")
    {
        app::WindowsStack stack;
        filter.attachWindowsStack(&stack);

        SECTION("stack is empty - it's ok to show popup")
        {
            REQUIRE(filter.isPermitted(prp));
        }

        SECTION("stack with same popup on top already")
        {
            auto w = std::make_unique<TestWindow>("wn");
            stack.push("wn", std::move(w));
            REQUIRE(filter.isPermitted(prp));
        }

        SECTION("stack with priority")
        {
            auto w = std::make_unique<TestWindow>("wn");
            stack.push("wn",
                       std::move(w),
                       gui::popup::popupDisposition(gui::popup::ID::Alarm, gui::popup::Disposition::Priority::Normal));

            // lesser priority
            {
                auto prp =
                    gui::PopupRequestParams(gui::popup::ID::AlarmActivated,
                                            gui::popup::popupDisposition(gui::popup::ID::AlarmActivated,
                                                                         gui::popup::Disposition::Priority::Low));
                REQUIRE(not filter.isPermitted(prp));
            }
            // higher priority & different popup on top
            {
                auto prp =
                    gui::PopupRequestParams(gui::popup::ID::AlarmActivated,
                                            gui::popup::popupDisposition(gui::popup::ID::AlarmActivated,
                                                                         gui::popup::Disposition::Priority::High));
                REQUIRE(filter.isPermitted(prp));
            }
            // equal priority & different popup on top - this is fine (see i.e. phone lock input )
            {
                auto prp =
                    gui::PopupRequestParams(gui::popup::ID::AlarmActivated,
                                            gui::popup::popupDisposition(gui::popup::ID::AlarmActivated,
                                                                         gui::popup::Disposition::Priority::Normal));
                REQUIRE(filter.isPermitted(prp));
            }

            // comments/expected behaviour in production code and in test are not consistent
            //  equal priority & same popup - this is not fine, i.e. alarm for alarm clock
            {
                auto prp = gui::PopupRequestParams(
                    gui::popup::ID::Alarm,
                    gui::popup::popupDisposition(gui::popup::ID::Alarm, gui::popup::Disposition::Priority::Normal));
                REQUIRE(not filter.isPermitted(prp));
            }
        }
    }
}

TEST_CASE("WindowsPopupQueue::addAppDependentFilter")
{
    gui::popup::Filter filter;
    auto prp = gui::PopupRequestParams(gui::popup::ID::Alarm);
    // create filter that accepts nothing
    filter.addAppDependentFilter([](const gui::PopupRequestParams &) -> bool { return false; });
    REQUIRE(not filter.isPermitted(prp));
}
