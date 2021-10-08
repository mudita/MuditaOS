// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TextParse.hpp"
#include <catch2/catch.hpp>
#include <limits>
#include <module-gui/gui/widgets/TextDocument.hpp>
#include <sstream>
#include <mock/multi-line-string.hpp>
#include <mock/buildTextDocument.hpp>

TEST_CASE("TextDocument ctor")
{
    using namespace gui;

    SECTION("empty")
    {
        /// textToTextBlocks have it's own tests
        auto document = TextDocument(textToTextBlocks("", nullptr));
        REQUIRE(document.getText() == UTF8());
    }

    SECTION("one block (one paragraph)")
    {
        const auto no_lines = 3;
        std::string text    = mockup::multiLineString(no_lines);
        auto document       = TextDocument(textToTextBlocks(text, nullptr));
        REQUIRE(!(document.getText() == ""));
        REQUIRE(document.getText() == text);
    }

    SECTION("many blocks")
    {
        const auto no_lines = 3;
        std::string text    = mockup::multiLineString(no_lines);
        auto blocks         = textToTextBlocks(text, nullptr);

        auto document = TextDocument(blocks);
        document.append(std::move(blocks));

        REQUIRE(!(document.getText() == ""));
        REQUIRE(document.getText() == std::string(text + text));
    }
}

TEST_CASE("TextDocument getBlockCursor")
{
    using namespace gui;

    SECTION("empty")
    {
        auto document = TextDocument({});
        auto cursor   = document.getBlockCursor(100);
        REQUIRE(cursor.getBlockNumber() == text::npos);
        auto text = document.getText(cursor);
        REQUIRE(text == "");
    }

    auto [document, testfont] = mockup::buildTestDocument();
    REQUIRE(testfont != nullptr);

    SECTION("middle")
    {
        unsigned int pos = 10;
        REQUIRE(document.getText().length() > pos);
        auto cursor = document.getBlockCursor(pos);
        REQUIRE(cursor.getPosition() != text::npos);
        REQUIRE(cursor.getBlockNumber() != text::npos);
        auto text = document.getText(cursor);
        REQUIRE(!(text == ""));
    }
}
