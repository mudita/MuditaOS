// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <limits>
#include <module-gui/gui/widgets/TextBlock.hpp>
#include "InitializedFontManager.hpp"

TEST_CASE("TextBlock Ctor/Dtor ")
{
    using namespace gui;
    auto &fontmanager = mockup::fontManager();

    SECTION("empty")
    {
        auto block = TextBlock("", nullptr, TextBlock::End::Newline);
    }

    SECTION("Just text")
    {
        auto block = TextBlock("lol", nullptr, TextBlock::End::Newline);
    }

    SECTION("Text and font")
    {
        REQUIRE(fontmanager.getFont(0) != nullptr);
        auto block = TextBlock("lol", fontmanager.getFont(0), TextBlock::End::Newline);
    }
}

TEST_CASE("Text block - set/update/get text")
{
    using namespace gui;
    const std::string starting_text = "lol";
    auto &fontmanager               = mockup::fontManager();
    REQUIRE(fontmanager.getFont(0) != nullptr);
    auto block = TextBlock(starting_text, fontmanager.getFont(0), TextBlock::End::None);

    SECTION("get")
    {
        REQUIRE(block.getText() == starting_text);
    }

    SECTION("set")
    {
        const std::string update_text = "lol2";
        block.setText(update_text);
        REQUIRE(block.getText() == update_text);
    }

    SECTION("insert char - at position 0")
    {
        std::string char_update   = "a";
        uint32_t char_update_code = 'a';
        auto local_update_text    = char_update + starting_text;

        block.setText(starting_text);
        block.insertChar(char_update_code, 0);

        REQUIRE(block.getText() == local_update_text);
    }

    SECTION("insert char - at position uint32_t max")
    {
        std::string char_update   = "a";
        uint32_t char_update_code = 'a';
        auto local_update_text    = char_update + starting_text;

        block.setText(starting_text);
        block.insertChar(char_update_code, std::numeric_limits<uint32_t>().max());

        REQUIRE(!(block.getText() == local_update_text));
    }

    SECTION("insert char in range <0 - max-size }")
    {
        std::string char_update   = "a";
        uint32_t char_update_code = 'a';
        const auto position       = starting_text.size() - 1;
        // create local test string for comparison
        std::string local_update_text = starting_text;
        local_update_text.reserve(32); // please dont use resize here if you want to have proper length()
        local_update_text.insert(position, char_update);

        block.setText(starting_text);
        block.insertChar(char_update_code, position);

        REQUIRE(block.getText() == local_update_text);
    }

    SECTION("insert char after last char")
    {
        std::string char_update       = "a";
        uint32_t char_update_code     = 'a';
        std::string local_update_text = starting_text + char_update;

        block.setText(starting_text);
        block.insertChar(char_update_code, starting_text.length());

        REQUIRE(block.getText() == local_update_text);
    }

    SECTION("get substring 'from' ")
    {
        auto local_update_text = starting_text + starting_text;
        auto pos               = 2;
        block.setText(local_update_text);
        REQUIRE(block.getText(pos) == std::string(local_update_text.begin() + pos, local_update_text.end()));
    }

    SECTION("get block text length")
    {
        auto local_update_text = starting_text + starting_text;
        block.setText(local_update_text);
        REQUIRE(block.length() == local_update_text.length());
    }
}

TEST_CASE("Text block - set/update/get text with navigation and max length")
{
    using namespace gui;
    const std::string starting_text = "some starting text";
    auto &fontmanager               = mockup::fontManager();
    REQUIRE(fontmanager.getFont(0) != nullptr);
    auto block = TextBlock(starting_text, fontmanager.getFont(0), TextBlock::End::None);

    SECTION("get forward/back on empty text with invalid starting pos and length")
    {
        auto emptyTextBlock     = TextBlock("", fontmanager.getFont(0), TextBlock::End::None);
        const auto starting_pos = 10;
        REQUIRE(emptyTextBlock.getText(starting_pos, NavigationDirection::RIGHT) == std::string{});
        REQUIRE(emptyTextBlock.getText(starting_pos, NavigationDirection::LEFT) == std::string{});
    }

    SECTION("get forward full")
    {
        const auto starting_pos = 0;
        REQUIRE(block.getText(starting_pos, NavigationDirection::RIGHT, starting_text.size()) == starting_text);
    }

    SECTION("get forward full with overflowing max length")
    {
        const auto starting_pos = 0;
        REQUIRE(block.getText(starting_pos, NavigationDirection::RIGHT) == starting_text);
    }

    SECTION("get forward with non-zero starting position")
    {
        const auto starting_pos = 5;
        REQUIRE(block.getText(starting_pos, NavigationDirection::RIGHT, starting_text.size() - starting_pos) ==
                starting_text.substr(starting_pos));
    }

    SECTION("get forward with non-zero starting position with overflowing max length")
    {
        const auto starting_pos = 5;
        REQUIRE(block.getText(starting_pos, NavigationDirection::RIGHT) == starting_text.substr(starting_pos));
    }

    SECTION("get forward from the end of text")
    {
        const auto starting_pos = starting_text.size();
        REQUIRE(block.getText(starting_pos, NavigationDirection::RIGHT) == std::string{});
    }

    SECTION("get forward with overflowing starting position")
    {
        const auto starting_pos = starting_text.size() + 10;
        REQUIRE(block.getText(starting_pos, NavigationDirection::RIGHT) == std::string{});
    }

    SECTION("get back only first char ")
    {
        const auto starting_pos = 1;
        REQUIRE(block.getText(starting_pos, NavigationDirection::LEFT) == std::string{starting_text.front()});
    }

    SECTION("get back only last char")
    {
        const auto starting_pos = starting_text.size();
        const auto length       = 1;
        REQUIRE(block.getText(starting_pos, NavigationDirection::LEFT, length) == std::string{starting_text.back()});
    }

    SECTION("get back full")
    {
        const auto starting_pos = starting_text.size();
        REQUIRE(block.getText(starting_pos, NavigationDirection::LEFT, starting_text.size()) == starting_text);
    }

    SECTION("get back full with overflowing max length")
    {
        const auto starting_pos = starting_text.size();
        REQUIRE(block.getText(starting_pos, NavigationDirection::LEFT) == starting_text);
    }

    SECTION("get back with non-zero starting position")
    {
        const auto starting_pos = 5;
        REQUIRE(block.getText(starting_pos, NavigationDirection::LEFT, starting_text.size() - starting_pos) ==
                starting_text.substr(0, starting_pos));
    }

    SECTION("get back with non-zero starting position with overflowing max length")
    {
        const auto starting_pos = 5;
        REQUIRE(block.getText(starting_pos, NavigationDirection::LEFT) == starting_text.substr(0, starting_pos));
    }

    SECTION("get back with overflowing starting position")
    {
        const auto starting_pos = starting_text.size() + 10;
        const auto length       = 10;
        REQUIRE(block.getText(starting_pos, NavigationDirection::LEFT, length) ==
                starting_text.substr(starting_text.size() - length));
    }

    SECTION("get back with overflowing starting position with overflowing max length")
    {
        const auto starting_pos = starting_text.size() + 10;
        REQUIRE(block.getText(starting_pos, NavigationDirection::LEFT) == starting_text);
    }

    SECTION("get forward and get back gives whole text")
    {
        const auto starting_pos = 10;
        auto leftHandSide       = block.getText(starting_pos, NavigationDirection::LEFT);
        auto rightHandSide      = block.getText(starting_pos, NavigationDirection::RIGHT);
        REQUIRE(leftHandSide + rightHandSide == starting_text);
    }
}

TEST_CASE("Text block - remove text")
{
    mockup::fontManager();
    using namespace gui;
    const std::string starting_text = "lol";
    auto &fontmanager               = FontManager::getInstance();
    REQUIRE(fontmanager.getFont(0) != nullptr);
    auto block = TextBlock(starting_text, fontmanager.getFont(0), TextBlock::End::Newline);

    SECTION("remove first char")
    {
        std::string local_update_text = std::string(starting_text.begin() + 1, starting_text.end());
        block.setText(starting_text);
        block.removeChar(0);
        REQUIRE(block.getText() == local_update_text);
    }

    SECTION("remove out of range char")
    {
        block.setText(starting_text);
        block.removeChar(std::numeric_limits<uint32_t>().max());
        REQUIRE(block.getText() == starting_text);
    }

    SECTION("remove char in range")
    {
        const auto position           = 1;
        std::string local_update_text = starting_text;
        local_update_text.erase(position, 1);
        block.setText(starting_text);
        block.removeChar(position);
        REQUIRE(block.getText() == local_update_text);
    }

    SECTION("remove last char")
    {
        const auto position           = starting_text.size() - 1;
        std::string local_update_text = starting_text;
        local_update_text.erase(position, 1);

        block.setText(starting_text);
        block.removeChar(position);
        REQUIRE(block.getText() == local_update_text);
    }
}

TEST_CASE("Text block - get width")
{
    using namespace gui;
    const std::string empty_text;
    auto &fontmanager = mockup::fontManager();

    auto block_newline = TextBlock(empty_text, fontmanager.getFont(0), TextBlock::End::Newline);
    auto block_empty   = TextBlock(empty_text, fontmanager.getFont(0), TextBlock::End::None);

    SECTION("get width == 0 for text with newline")
    {
        REQUIRE(block_newline.getWidth() == 0);
    }

    SECTION("get width > 0 if text not empty (with newline)")
    {
        REQUIRE(block_newline.getFormat()->getFont() != nullptr);
        block_newline.setText("lol");
        REQUIRE(block_newline.getWidth() > 0);
    }

    SECTION("get width == 0 if empty text set")
    {
        REQUIRE(block_empty.getWidth() == 0);
    }
}
