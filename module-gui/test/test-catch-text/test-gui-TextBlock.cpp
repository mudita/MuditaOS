// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
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
        REQUIRE(fontmanager.getFont() != nullptr);
        auto block = TextBlock("lol", fontmanager.getFont(), TextBlock::End::Newline);
    }
}

TEST_CASE("Text block - set/update/get text")
{
    using namespace gui;
    const std::string starting_text = "lol";
    auto &fontmanager               = mockup::fontManager();
    REQUIRE(fontmanager.getFont() != nullptr);
    auto block = TextBlock(starting_text, fontmanager.getFont(), TextBlock::End::None);

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

TEST_CASE("Text block - remove text")
{
    mockup::fontManager();
    using namespace gui;
    const std::string starting_text = "lol";
    auto &fontmanager               = FontManager::getInstance();
    REQUIRE(fontmanager.getFont() != nullptr);
    auto block = TextBlock(starting_text, fontmanager.getFont(), TextBlock::End::Newline);

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

    auto block_newline = TextBlock(empty_text, fontmanager.getFont(), TextBlock::End::Newline);
    auto block_empty   = TextBlock(empty_text, fontmanager.getFont(), TextBlock::End::None);

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
