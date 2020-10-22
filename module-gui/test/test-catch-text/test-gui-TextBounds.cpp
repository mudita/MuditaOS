// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <limits>
#include <module-gui/gui/widgets/TextBlock.hpp>
#include <mock/buildTextDocument.hpp>
#include <module-gui/gui/widgets/Text.hpp>
#include <mock/multi-line-string.hpp>

TEST_CASE("Text bounds")
{
    using namespace gui;

    auto key_up = gui::InputEvent(
        {
            RawKey::State::Released,
            bsp::KeyCodes::Undefined,
        },
        gui::InputEvent::State::keyReleasedShort,
        gui::KeyCode::KEY_UP);

    auto key_left = gui::InputEvent(
        {
            RawKey::State::Released,
            bsp::KeyCodes::Undefined,
        },
        gui::InputEvent::State::keyReleasedShort,
        gui::KeyCode::KEY_LEFT);

    auto key_down = gui::InputEvent(
        {
            RawKey::State::Released,
            bsp::KeyCodes::Undefined,
        },
        gui::InputEvent::State::keyReleasedShort,
        gui::KeyCode::KEY_DOWN);

    auto texts            = mockup::lineStrings(6);
    auto [document, font] = mockup::buildMultilineTestDocument(texts);

    auto text = gui::Text();
    text.setText(std::make_unique<gui::TextDocument>(document));
    text.setFont(font);

    SECTION("moving cursor - hit bound")
    {
        auto lines  = Lines(&text);
        auto cursor = new gui::TextLineCursor(&text);

        lines.emplace(TextLine(*cursor, 1000));
        lines.emplace(TextLine(*cursor, 1000));
        lines.emplace(TextLine(*cursor, 1000));
        lines.emplace(TextLine(*cursor, 1000));
        lines.emplace(TextLine(*cursor, 1000));

        cursor->moveCursor(NavigationDirection::DOWN);
        cursor->moveCursor(NavigationDirection::DOWN);
        cursor->moveCursor(NavigationDirection::DOWN);

        auto bound = lines.checkNavigationBounds(*cursor, key_down);
        REQUIRE(bound == InputBound::HIT_BOUND);
    }

    SECTION("moving cursor - reach data end")
    {
        auto lines  = Lines(&text);
        auto cursor = new gui::TextLineCursor(&text);

        lines.emplace(TextLine(*cursor, 100));
        auto bound = lines.checkNavigationBounds(*cursor, key_up);
        REQUIRE(bound == InputBound::NO_DATA);

        cursor->moveCursor(NavigationDirection::RIGHT);
        bound = lines.checkNavigationBounds(*cursor, key_left);
        REQUIRE(bound == InputBound::CAN_MOVE);

        cursor->moveCursor(NavigationDirection::DOWN);
        bound = lines.checkNavigationBounds(*cursor, key_up);
        REQUIRE(bound == InputBound::NO_DATA);
    }
}
