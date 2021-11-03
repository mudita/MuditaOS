// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <widgets/text/parsers/TextParse.hpp>
#include <mock/multi-line-string.hpp>

#include <catch2/catch.hpp>

TEST_CASE("TextParse")
{
    using namespace gui;

    SECTION("empty")
    {
        auto res = textToTextBlocks("", nullptr);
        REQUIRE(res.size() == 0);
    }

    SECTION("oneline without newline")
    {
        auto res = textToTextBlocks("this is text", nullptr, gui::TextBlock::End::None);
        REQUIRE(res.size() == 1);
    }

    SECTION("oneline with newline")
    {
        auto res = textToTextBlocks("this is text", nullptr, gui::TextBlock::End::Newline);
        REQUIRE(res.size() == 2);
    }

    SECTION("multi-line without newline")
    {
        const auto no_lines = 3;
        auto text           = mockup::multiLineString(no_lines);
        auto res            = textToTextBlocks(text, nullptr, gui::TextBlock::End::None);
        REQUIRE(res.size() == no_lines);
    }

    SECTION("multi-line with newline")
    {
        const auto no_lines = 3;
        auto text           = mockup::multiLineString(no_lines);
        auto res            = textToTextBlocks(text, nullptr, gui::TextBlock::End::Newline);
        REQUIRE(res.size() == no_lines + 1);
    }
}
