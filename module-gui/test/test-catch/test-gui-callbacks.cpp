// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <module-gui/gui/widgets/Item.hpp>
#include <mock/TestWindow.hpp>
#include <gui/input/InputEvent.hpp>

// there is strict order on which callbacks should be called
TEST_CASE("gui::Item on input flow test")
{
    bool success = false;
    auto item    = gui::Item();

    SECTION("gui item onInput callback called")
    {
        success            = false;
        item.inputCallback = [&success](gui::Item &, const gui::InputEvent &inputEvent) -> bool {
            success = true;
            return true;
        };
        // trigger callback
        item.onInput(gui::InputEvent({}));
        // test callback called
        REQUIRE(success == true);
    }

    SECTION("gui item resize called")
    {
        success                       = false;
        item.dimensionChangedCallback = [&success](gui::Item &, gui::BoundingBox data) -> bool {
            success = true;
            return true;
        };
        // trigger callback
        item.setSize(item.getWidth(), item.getHeight());
        // test callback called
        REQUIRE(success == true);
    }
}

#include <module-gui/gui/widgets/text/Label.hpp>
#include "mock/InitializedFontManager.hpp"

TEST_CASE("gui::Window on input flow test")
{
    auto win     = gui::TestWindow("Callback test window");
    bool success = false;
    SECTION("gui item onInput callback called")
    {
        success           = false;
        win.inputCallback = [&success](gui::Item &, const gui::InputEvent &inputEvent) -> bool {
            success = true;
            return true;
        };
        // trigger callback
        win.onInput(gui::InputEvent({}));
        // test callback called
        REQUIRE(success == true);
    }

    SECTION("gui item - no input handling, no navigation")
    {
        win.inputCallback = nullptr;
        win.onInput(gui::InputEvent({}));
        REQUIRE(success == false);
    }

    mockup::fontManager();
    auto l1 = new gui::Label(&win, 0, 0, 0, 0, "Test 1");
    auto l2 = new gui::Label(&win, 0, 0, 0, 0, "Test 2");

    SECTION("gui item - no input handling with navigation")
    {
        bool focus_acquired_l1 = false;
        bool focus_acquired_l2 = false;
        bool l1_input_handled  = false;
        l1->setNavigationItem({gui::NavigationDirection::DOWN}, l2);
        l1->focusChangedCallback = [&focus_acquired_l1](gui::Item &it) -> bool {
            if (it.focus) {
                focus_acquired_l1 = true;
            }
            return true;
        };

        l1->inputCallback = [&l1_input_handled](gui::Item &, const gui::InputEvent &inputEvent) -> bool {
            if (inputEvent.is(gui::KeyCode::KEY_TORCH)) {
                l1_input_handled = true;
                return true;
            }
            return false;
        };

        l2->setNavigationItem({gui::NavigationDirection::DOWN}, l1);
        l2->focusChangedCallback = [&focus_acquired_l2](gui::Item &it) -> bool {
            if (it.focus) {
                focus_acquired_l2 = true;
            }
            return true;
        };

        win.setFocusItem(l1);
        focus_acquired_l2 = false;
        win.onInput(gui::InputEvent({}, gui::InputEvent::State::keyReleasedShort, gui::KeyCode::KEY_DOWN));
        REQUIRE(focus_acquired_l2);

        focus_acquired_l1 = false;
        win.onInput(gui::InputEvent({}, gui::InputEvent::State::keyReleasedShort, gui::KeyCode::KEY_DOWN));
        REQUIRE(focus_acquired_l1);

        win.onInput(gui::InputEvent({}, gui::InputEvent::State::keyReleasedShort, gui::KeyCode::KEY_TORCH));
        REQUIRE(l1_input_handled);
    }
}
