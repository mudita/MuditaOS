// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <widgets/text/TextFixedSize.hpp>
#include <module-gui/test/mock/InitializedFontManager.hpp>

#include <catch2/catch.hpp>

namespace gui
{
    class TestTextFixedSize : public TextFixedSize
    {
      public:
        TestTextFixedSize() : TextFixedSize(nullptr, 0, 0, 0, 0){};

        [[nodiscard]] unsigned int linesSize()
        {
            return lines->get().size();
        }

        void drawLines() override
        {
            TextFixedSize::drawLines();
        }

        [[nodiscard]] auto lineGet(unsigned int nr)
        {
            auto line = lines->get().begin();

            if (nr >= lines->size()) {
                nr = lines->size() - 1;
            }

            std::advance(line, nr);
            return line;
        }

        auto removeNCharacters(unsigned int n)
        {
            for (unsigned int i = 0; i < n; i++) {
                removeChar();
            }
        }
    };
} // namespace gui

TEST_CASE("TextFixedSize drawLines")
{
    using namespace gui;
    mockup::fontManager();

    std::string testString1 = "Test \n";
    std::string testString2 = "Long \n";
    std::string testString3 = "Long \n";
    std::string testString4 = "Long \n";
    std::string testString5 = "Line \n";
    std::string testString6 = "End";

    std::string testStringLongLine = testString1 + testString2 + testString3 + testString4 + testString5 + testString6;

    SECTION("Empty text for space to draw 4 lines")
    {
        auto text = TestTextFixedSize();
        text.setSize(20, 120);

        text.setFont(style::window::font::medium);
        text.drawLines();

        REQUIRE(text.linesSize() == 4);
    }

    SECTION("Empty text for space to draw 4 lines but lines count limited to 2")
    {
        auto text = TestTextFixedSize();
        text.setSize(20, 120);

        text.setFont(style::window::font::medium);
        text.setLines(2);
        text.drawLines();

        REQUIRE(text.linesSize() == 2);
    }

    SECTION("Text for space to draw 4 lines - cursor moved to beginning")
    {
        auto text = TestTextFixedSize();
        text.setSize(200, 120);

        text.setCursorStartPosition(gui::CursorStartPosition::DocumentBegin);
        text.setFont(style::window::font::medium);
        text.setText(testStringLongLine);

        REQUIRE(text.lineGet(0)->getText(0) == testString1);
        REQUIRE(text.lineGet(1)->getText(0) == testString2);
        REQUIRE(text.lineGet(2)->getText(0) == testString3);
        REQUIRE(text.lineGet(3)->getText(0) == testString4);

        // There should be added extra invisible line.
        REQUIRE(text.linesSize() == 5);

        REQUIRE(text.lineGet(4)->getText(0) == testString5);
        REQUIRE_FALSE(text.lineGet(4)->isVisible());
    }

    SECTION("Text for space to draw 4 lines - cursor moved to end")
    {
        auto text = TestTextFixedSize();
        text.setSize(200, 120);

        text.setFont(style::window::font::medium);
        text.setText(testStringLongLine);

        REQUIRE(text.lineGet(0)->getText(0) == testString3);
        REQUIRE(text.lineGet(1)->getText(0) == testString4);
        REQUIRE(text.lineGet(2)->getText(0) == testString5);
        REQUIRE(text.lineGet(3)->getText(0) == testString6);
        REQUIRE(text.linesSize() == 4);
    }

    SECTION("Text for space to draw 4 lines but lines count limited to 2 - cursor moved to beginning")
    {
        auto text = TestTextFixedSize();
        text.setSize(200, 120);

        text.setCursorStartPosition(gui::CursorStartPosition::DocumentBegin);
        text.setFont(style::window::font::medium);
        text.setLines(2);
        text.setText(testStringLongLine);

        REQUIRE(text.lineGet(0)->getText(0) == testString1);
        REQUIRE(text.lineGet(1)->getText(0) == testString2);

        // There should be added extra invisible line.
        REQUIRE(text.linesSize() == 3);

        REQUIRE(text.lineGet(2)->getText(0) == testString3);
        REQUIRE_FALSE(text.lineGet(4)->isVisible());
    }

    SECTION("Text for space to draw 4 lines but lines count limited to 2 - cursor moved to end")
    {
        auto text = TestTextFixedSize();
        text.setSize(200, 120);

        text.setFont(style::window::font::medium);
        text.setLines(2);
        text.setText(testStringLongLine);

        REQUIRE(text.lineGet(0)->getText(0) == testString5);
        REQUIRE(text.lineGet(1)->getText(0) == testString6);
        REQUIRE(text.linesSize() == 2);
    }
}

TEST_CASE("TextFixedSize remove Char")
{
    using namespace gui;
    mockup::fontManager();

    std::string testString1 = "Test Line Line Line ";
    std::string testString2 = "End";

    std::string testStringLongLine = testString1 + testString2;

    SECTION("Remove char from last line on size limited space")
    {
        auto text = TestTextFixedSize();
        text.setSize(220, 30);

        text.setFont(style::window::font::medium);
        text.setText(testStringLongLine);

        REQUIRE(text.lineGet(0)->getText(0) == testString2);
        REQUIRE(text.linesSize() == 1);

        text.removeNCharacters(testString2.size());
        REQUIRE(text.lineGet(0)->getText(0) == testString1);
        REQUIRE(text.linesSize() == 1);
    }

    SECTION("Remove char from last line on line limited space")
    {
        auto text = TestTextFixedSize();
        text.setSize(220, 120);

        text.setFont(style::window::font::medium);
        text.setLines(1);
        text.setText(testStringLongLine);

        REQUIRE(text.lineGet(0)->getText(0) == testString2);
        REQUIRE(text.linesSize() == 1);

        text.removeNCharacters(testString2.size());
        REQUIRE(text.lineGet(0)->getText(0) == testString1);
        REQUIRE(text.linesSize() == 1);
    }
}
