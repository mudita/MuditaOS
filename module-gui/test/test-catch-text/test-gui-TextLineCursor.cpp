// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <widgets/text/Text.hpp>
#include <widgets/text/core/TextBlock.hpp>
#include <mock/buildTextDocument.hpp>
#include <mock/multi-line-string.hpp>
#include "Font.hpp"
#include "InitializedFontManager.hpp"

#include <catch2/catch.hpp>

namespace gui
{
    class TestText : public Text
    {
      public:
        [[nodiscard]] unsigned int linesVisible()
        {
            return lines->countVisible();
        }

        [[nodiscard]] auto &linesGet()
        {
            return lines->get();
        }

        auto moveCursor(NavigationDirection direction, unsigned int n)
        {
            cursor->TextLineCursor::moveCursor(direction, n);
        }

        auto removeNChars(unsigned int n)
        {
            for (unsigned int i = 0; i < n; i++) {
                removeChar();
            }
        }

        [[nodiscard]] auto getSelectedLine()
        {
            return cursor->getSelectedLine();
        }
    };
} // namespace gui

TEST_CASE("TextLineCursor - navigation without scroll", "[!mayfail]")
{
    using namespace gui;

    std::string testStringShortLine  = "Test Line ";                     // 10 sings
    std::string testStringNormalLine = "Test Normal No Line ";           // 20 sings
    std::string testStringLongLine   = "Test Long Long Long Long Line "; // 30 sings

    SECTION("Default position three lines text")
    {
        mockup::fontManager();
        auto text = std::make_unique<gui::TestText>();
        text->setMaximumSize(400, 200);

        text->addText(TextBlock(testStringShortLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringNormalLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringLongLine, Font(27).raw(), TextBlock::End::None));

        auto [selectedLine, selectedLineCursorPosition, selectedLineNumber] = text->getSelectedLine();

        REQUIRE(selectedLine->getText(0).c_str() == testStringLongLine);
        REQUIRE(selectedLineCursorPosition == testStringLongLine.length());
        REQUIRE(selectedLineNumber == 2);
    }

    SECTION("Movement three lines text test -> short|normal|long")
    {
        mockup::fontManager();
        auto text = std::make_unique<gui::TestText>();
        text->setMaximumSize(400, 200);

        std::tuple<const TextLine *, unsigned int, unsigned int> selectedLine;

        text->addText(TextBlock(testStringShortLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringNormalLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringLongLine, Font(27).raw(), TextBlock::End::None));

        selectedLine = text->getSelectedLine();
        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringLongLine);
        REQUIRE(std::get<1>(selectedLine) == testStringLongLine.length());
        REQUIRE(std::get<2>(selectedLine) == 2);

        // Move Down nothing should happen as cursor is at bottom and right end
        text->moveCursor(gui::NavigationDirection::DOWN, 1);

        selectedLine = text->getSelectedLine();
        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringLongLine);
        REQUIRE(std::get<1>(selectedLine) == testStringLongLine.length());
        REQUIRE(std::get<2>(selectedLine) == 2);

        // Move Right nothing should happen as cursor is at bottom and right end
        text->moveCursor(gui::NavigationDirection::RIGHT, 1);

        selectedLine = text->getSelectedLine();
        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringLongLine);
        REQUIRE(std::get<1>(selectedLine) == testStringLongLine.length());
        REQUIRE(std::get<2>(selectedLine) == 2);

        // Move Left to line center, line should not change but position should
        text->moveCursor(gui::NavigationDirection::LEFT, (testStringLongLine.length() / 2));

        selectedLine = text->getSelectedLine();
        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringLongLine);
        REQUIRE(std::get<1>(selectedLine) == (testStringLongLine.length() / 2));
        REQUIRE(std::get<2>(selectedLine) == 2);

        // Move Up line should change and cursor should be at corresponding position as it fits in upper line
        text->moveCursor(gui::NavigationDirection::UP, 1);

        selectedLine = text->getSelectedLine();
        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringNormalLine + "\n");
        REQUIRE(std::get<1>(selectedLine) == (testStringLongLine.length() / 2));
        REQUIRE(std::get<2>(selectedLine) == 1);

        // Move Up line should change and cursor should be at its end as corresponding position does not fit
        text->moveCursor(gui::NavigationDirection::UP, 1);

        selectedLine = text->getSelectedLine();
        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringShortLine + "\n");
        REQUIRE(std::get<1>(selectedLine) == testStringShortLine.length());
        REQUIRE(std::get<2>(selectedLine) == 0);

        // Move Up line should not change and cursor should not change as it is first line
        text->moveCursor(gui::NavigationDirection::UP, 1);

        selectedLine = text->getSelectedLine();
        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringShortLine + "\n");
        REQUIRE(std::get<1>(selectedLine) == testStringShortLine.length());
        REQUIRE(std::get<2>(selectedLine) == 0);

        // Move Left to line and text beginning
        text->moveCursor(gui::NavigationDirection::LEFT, testStringShortLine.length());

        selectedLine = text->getSelectedLine();
        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringShortLine + "\n");
        REQUIRE(std::get<1>(selectedLine) == 0);
        REQUIRE(std::get<2>(selectedLine) == 0);

        // Move two times Down line should change and cursor should be at its beginning
        text->moveCursor(gui::NavigationDirection::DOWN, 2);

        selectedLine = text->getSelectedLine();
        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringLongLine);
        REQUIRE(std::get<1>(selectedLine) == 0);
        REQUIRE(std::get<2>(selectedLine) == 2);

        // Move one time Left line should change and cursor should be at its end
        text->moveCursor(gui::NavigationDirection::LEFT, 1);

        selectedLine = text->getSelectedLine();
        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringNormalLine + "\n");
        REQUIRE(std::get<1>(selectedLine) == testStringNormalLine.length());
        REQUIRE(std::get<2>(selectedLine) == 1);
    }

    SECTION("Movement three lines text test -> long|short|normal")
    {
        mockup::fontManager();
        auto text = std::make_unique<gui::TestText>();
        text->setMaximumSize(400, 200);

        std::tuple<const TextLine *, unsigned int, unsigned int> selectedLine;

        text->addText(TextBlock(testStringLongLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringShortLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringNormalLine, Font(27).raw(), TextBlock::End::None));

        selectedLine = text->getSelectedLine();
        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringNormalLine);
        REQUIRE(std::get<1>(selectedLine) == testStringNormalLine.length());
        REQUIRE(std::get<2>(selectedLine) == 2);

        // Move Up two times, line should change and cursor should be at short line length position
        text->moveCursor(gui::NavigationDirection::UP, 2);

        selectedLine = text->getSelectedLine();
        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringLongLine + "\n");
        REQUIRE(std::get<1>(selectedLine) == testStringShortLine.length());
        REQUIRE(std::get<2>(selectedLine) == 0);

        // Move Right three times, only cursor pos should change
        text->moveCursor(gui::NavigationDirection::RIGHT, 3);

        selectedLine = text->getSelectedLine();
        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringLongLine + "\n");
        REQUIRE(std::get<1>(selectedLine) == testStringShortLine.length() + 3);
        REQUIRE(std::get<2>(selectedLine) == 0);

        // Move Down one time, line should change and cursor should be at short line end as corresponding do not fit
        text->moveCursor(gui::NavigationDirection::DOWN, 1);

        selectedLine = text->getSelectedLine();
        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringShortLine + "\n");
        REQUIRE(std::get<1>(selectedLine) == testStringShortLine.length());
        REQUIRE(std::get<2>(selectedLine) == 1);

        // Move Right one time, line should change and cursor should be at next line beginning
        text->moveCursor(gui::NavigationDirection::RIGHT, 1);

        selectedLine = text->getSelectedLine();
        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringNormalLine);
        REQUIRE(std::get<1>(selectedLine) == 0);
        REQUIRE(std::get<2>(selectedLine) == 2);
    }
}

TEST_CASE("TextLineCursor - navigation with scroll", "[.]")
{
    using namespace gui;

    std::string testStringShortLine  = "Test Line ";                     // 10 sings
    std::string testStringNormalLine = "Test Normal No Line ";           // 20 sings
    std::string testStringLongLine   = "Test Long Long Long Long Line "; // 30 sings

    SECTION("One line text space text containing three lines - scroll init position at document begin")
    {

        mockup::fontManager();
        auto text = std::make_unique<gui::TestText>();
        text->setMaximumSize(400, 40);
        text->setCursorStartPosition(gui::CursorStartPosition::DocumentBegin);

        std::tuple<const TextLine *, unsigned int, unsigned int> selectedLine;

        REQUIRE(text->linesVisible() == 0);

        text->addText(TextBlock(testStringLongLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringShortLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringNormalLine, Font(27).raw(), TextBlock::End::None));

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringLongLine + "\n");
        REQUIRE(std::get<1>(selectedLine) == 0);
        REQUIRE(std::get<2>(selectedLine) == 0);
        REQUIRE(text->linesVisible() == 1);

        // Move Left two times, nothing should change
        text->moveCursor(gui::NavigationDirection::LEFT, 2);

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringLongLine + "\n");
        REQUIRE(std::get<1>(selectedLine) == 0);
        REQUIRE(std::get<2>(selectedLine) == 0);
        REQUIRE(text->linesVisible() == 1);

        // Move Down one time, line should change and cursor should be at its beginning
        text->moveCursor(gui::NavigationDirection::DOWN, 1);

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringShortLine + "\n");
        REQUIRE(std::get<1>(selectedLine) == 0);
        REQUIRE(std::get<2>(selectedLine) == 0);
        REQUIRE(text->linesVisible() == 1);

        // Move Left one time, line should change and cursor should be at previous line end
        text->moveCursor(gui::NavigationDirection::LEFT, 1);

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringLongLine + "\n");
        REQUIRE(std::get<1>(selectedLine) == testStringLongLine.length());
        REQUIRE(std::get<2>(selectedLine) == 0);
        REQUIRE(text->linesVisible() == 1);

        // Move Down one time, line should change and cursor should be at its end as current line is longer
        text->moveCursor(gui::NavigationDirection::DOWN, 1);

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringShortLine + "\n");
        REQUIRE(std::get<1>(selectedLine) == testStringShortLine.length());
        REQUIRE(std::get<2>(selectedLine) == 0);
        REQUIRE(text->linesVisible() == 1);

        // Move Right one time, line should change and cursor should be at its beginning
        text->moveCursor(gui::NavigationDirection::RIGHT, 1);

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringNormalLine);
        REQUIRE(std::get<1>(selectedLine) == 0);
        REQUIRE(std::get<2>(selectedLine) == 0);
        REQUIRE(text->linesVisible() == 1);

        // Move Down one time, nothing should change as it is document end
        text->moveCursor(gui::NavigationDirection::DOWN, 1);

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringNormalLine);
        REQUIRE(std::get<1>(selectedLine) == 0);
        REQUIRE(std::get<2>(selectedLine) == 0);
        REQUIRE(text->linesVisible() == 1);

        // Move Right for string length + 1, cursor should stop at end without line changing at it is document end
        text->moveCursor(gui::NavigationDirection::RIGHT, testStringNormalLine.length() + 1);

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringNormalLine);
        REQUIRE(std::get<1>(selectedLine) == testStringNormalLine.length());
        REQUIRE(std::get<2>(selectedLine) == 0);
        REQUIRE(text->linesVisible() == 1);

        // Move UP three times, lines should change and cursor should be at shortest line position.
        text->moveCursor(gui::NavigationDirection::UP, 3);

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringLongLine + "\n");
        REQUIRE(std::get<1>(selectedLine) == testStringShortLine.length());
        REQUIRE(std::get<2>(selectedLine) == 0);
        REQUIRE(text->linesVisible() == 1);
    }

    SECTION("One line text space text containing three lines - scroll init position at document end")
    {

        mockup::fontManager();
        auto text = std::make_unique<gui::TestText>();
        text->setMaximumSize(400, 40);
        text->setCursorStartPosition(gui::CursorStartPosition::DocumentEnd);

        std::tuple<const TextLine *, unsigned int, unsigned int> selectedLine;

        REQUIRE(text->linesVisible() == 0);

        text->addText(TextBlock(testStringShortLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringLongLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringNormalLine, Font(27).raw(), TextBlock::End::None));

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringNormalLine);
        REQUIRE(std::get<1>(selectedLine) == testStringNormalLine.length());
        REQUIRE(std::get<2>(selectedLine) == 0);
        REQUIRE(text->linesVisible() == 1);

        // Move UP three times, lines should change and cursor should be at shortest line position.
        text->moveCursor(gui::NavigationDirection::UP, 3);

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringShortLine + "\n");
        REQUIRE(std::get<1>(selectedLine) == testStringShortLine.length());
        REQUIRE(std::get<2>(selectedLine) == 0);
        REQUIRE(text->linesVisible() == 1);

        // Move Right one time, line should change and cursor should be at its beginning
        text->moveCursor(gui::NavigationDirection::RIGHT, 1);

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringLongLine + "\n");
        REQUIRE(std::get<1>(selectedLine) == 0);
        REQUIRE(std::get<2>(selectedLine) == 0);
        REQUIRE(text->linesVisible() == 1);

        // Move Right for line length + 1, line should change and cursor should be at its beginning
        text->moveCursor(gui::NavigationDirection::RIGHT, testStringLongLine.length() + 1);

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringNormalLine);
        REQUIRE(std::get<1>(selectedLine) == 0);
        REQUIRE(std::get<2>(selectedLine) == 0);
        REQUIRE(text->linesVisible() == 1);
    }

    SECTION("One 10 lines text space text containing three lines - scroll init position at document begin")
    {
        mockup::fontManager();
        auto text = std::make_unique<gui::TestText>();
        text->setMaximumSize(400, 90);
        text->setCursorStartPosition(gui::CursorStartPosition::DocumentBegin);

        std::tuple<const TextLine *, unsigned int, unsigned int> selectedLine;

        REQUIRE(text->linesVisible() == 0);

        text->addText(TextBlock(testStringLongLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringShortLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringNormalLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringLongLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringShortLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringNormalLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringLongLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringShortLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringNormalLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringLongLine, Font(27).raw(), TextBlock::End::None));

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringLongLine + "\n");
        REQUIRE(std::get<1>(selectedLine) == 0);
        REQUIRE(std::get<2>(selectedLine) == 0);
        REQUIRE(text->linesVisible() == 3);

        // Move Left two times, nothing should change
        text->moveCursor(gui::NavigationDirection::LEFT, 2);

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringLongLine + "\n");
        REQUIRE(std::get<1>(selectedLine) == 0);
        REQUIRE(std::get<2>(selectedLine) == 0);
        REQUIRE(text->linesVisible() == 3);

        // Move Down two times, lines should change but without scroll
        text->moveCursor(gui::NavigationDirection::DOWN, 2);

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringNormalLine + "\n");
        REQUIRE(std::get<1>(selectedLine) == 0);
        REQUIRE(std::get<2>(selectedLine) == 2);

        // Move Down one time, lines should change with scroll
        text->moveCursor(gui::NavigationDirection::DOWN, 1);

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringLongLine + "\n");
        REQUIRE(std::get<1>(selectedLine) == 0);
        REQUIRE(std::get<2>(selectedLine) == 2);

        // Move Right for line length, lines should change with scroll
        text->moveCursor(gui::NavigationDirection::RIGHT, testStringLongLine.length() + 1);

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringShortLine + "\n");
        REQUIRE(std::get<1>(selectedLine) == 0);
        REQUIRE(std::get<2>(selectedLine) == 2);

        // Move Down ten times, lines should change with scroll and stop at last one
        text->moveCursor(gui::NavigationDirection::DOWN, 10);

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringLongLine);
        REQUIRE(std::get<1>(selectedLine) == 0);
        REQUIRE(std::get<2>(selectedLine) == 2);
    }

    SECTION("One 10 lines text space text containing three lines - scroll init position at document end")
    {
        mockup::fontManager();
        auto text = std::make_unique<gui::TestText>();
        text->setMaximumSize(400, 90);
        text->setCursorStartPosition(gui::CursorStartPosition::DocumentEnd);

        std::tuple<const TextLine *, unsigned int, unsigned int> selectedLine;

        REQUIRE(text->linesVisible() == 0);

        text->addText(TextBlock(testStringLongLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringShortLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringNormalLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringLongLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringShortLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringNormalLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringLongLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringShortLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringNormalLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringLongLine, Font(27).raw(), TextBlock::End::None));

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringLongLine);
        REQUIRE(std::get<1>(selectedLine) == testStringLongLine.length());
        REQUIRE(std::get<2>(selectedLine) == 2);
        REQUIRE(text->linesVisible() == 3);

        // Move Left two times, nothing should change
        text->moveCursor(gui::NavigationDirection::RIGHT, 2);

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringLongLine);
        REQUIRE(std::get<1>(selectedLine) == testStringLongLine.length());
        REQUIRE(std::get<2>(selectedLine) == 2);
        REQUIRE(text->linesVisible() == 3);

        // Move Up two times, lines should change and cursor should be at shortest line length
        text->moveCursor(gui::NavigationDirection::UP, 2);

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringShortLine + "\n");
        REQUIRE(std::get<1>(selectedLine) == testStringShortLine.length());
        REQUIRE(std::get<2>(selectedLine) == 0);
        REQUIRE(text->linesVisible() == 3);

        // Move Left for line length, line should change and cursor should be at previous line end
        text->moveCursor(gui::NavigationDirection::LEFT, testStringShortLine.length() + 1);

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringLongLine + "\n");
        REQUIRE(std::get<1>(selectedLine) == testStringLongLine.length());
        REQUIRE(std::get<2>(selectedLine) == 0);
        REQUIRE(text->linesVisible() == 3);

        // Move Up ten times, line should change and cursor should be at shortest line position
        text->moveCursor(gui::NavigationDirection::UP, 10);

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringLongLine + "\n");
        REQUIRE(std::get<1>(selectedLine) == testStringShortLine.length());
        REQUIRE(std::get<2>(selectedLine) == 0);
        REQUIRE(text->linesVisible() == 3);
    }
}

TEST_CASE("TextLineCursor - addition and deletion with scroll", "[.]")
{
    using namespace gui;

    std::string testStringShortLine  = "Test Line ";                     // 10 sings
    std::string testStringNormalLine = "Test Normal No Line ";           // 20 sings
    std::string testStringLongLine   = "Test Long Long Long Long Line "; // 30 sings

    SECTION("One line text space text containing three lines - scroll init position at document begin")
    {

        mockup::fontManager();
        auto text = std::make_unique<gui::TestText>();
        text->setMaximumSize(600, 40);
        text->setCursorStartPosition(gui::CursorStartPosition::DocumentBegin);

        std::tuple<const TextLine *, unsigned int, unsigned int> selectedLine;

        REQUIRE(text->linesVisible() == 0);

        text->addText(TextBlock(testStringLongLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringShortLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringNormalLine, Font(27).raw(), TextBlock::End::None));

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringLongLine + "\n");
        REQUIRE(std::get<1>(selectedLine) == 0);
        REQUIRE(std::get<2>(selectedLine) == 0);
        REQUIRE(text->linesVisible() == 1);

        // Add empty new line at beginning, other lines should shift down
        text->addText("\n");

        selectedLine = text->getSelectedLine();
        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == std::string("\n"));
        REQUIRE(std::get<1>(selectedLine) == 0);
        REQUIRE(std::get<2>(selectedLine) == 0);
        REQUIRE(text->linesVisible() == 1);

        // Move Down three times, lines should change
        text->moveCursor(gui::NavigationDirection::DOWN, 3);

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringNormalLine);
        REQUIRE(std::get<1>(selectedLine) == 0);
        REQUIRE(std::get<2>(selectedLine) == 0);
        REQUIRE(text->linesVisible() == 1);

        // As we are line beginning remove character which should remove newline and merge two lines
        text->removeChar();

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringShortLine + testStringNormalLine);
        REQUIRE(std::get<1>(selectedLine) == testStringShortLine.length());
        REQUIRE(std::get<2>(selectedLine) == 0);
        REQUIRE(text->linesVisible() == 1);

        // Remove whole line (+1 for newline), line should change and what left in line should merge
        text->removeNChars(testStringShortLine.length() + 1);

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringLongLine + testStringNormalLine);
        REQUIRE(std::get<1>(selectedLine) == testStringLongLine.length());
        REQUIRE(std::get<2>(selectedLine) == 0);
        REQUIRE(text->linesVisible() == 1);
    }

    SECTION("One 10 lines text space text containing three lines - scroll init position at document end")
    {
        mockup::fontManager();
        auto text = std::make_unique<gui::TestText>();
        text->setMaximumSize(400, 90);
        text->setCursorStartPosition(gui::CursorStartPosition::DocumentEnd);

        std::tuple<const TextLine *, unsigned int, unsigned int> selectedLine;

        REQUIRE(text->linesVisible() == 0);

        text->addText(TextBlock(testStringLongLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringShortLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringNormalLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringLongLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringShortLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringNormalLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringLongLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringShortLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringNormalLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringLongLine, Font(27).raw(), TextBlock::End::None));

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringLongLine);
        REQUIRE(std::get<1>(selectedLine) == testStringLongLine.length());
        REQUIRE(std::get<2>(selectedLine) == 2);
        REQUIRE(text->linesVisible() == 3);

        // Move Up two times, lines should change and cursor should be at shortest line length
        text->moveCursor(gui::NavigationDirection::UP, 1);

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringNormalLine + "\n");
        REQUIRE(std::get<1>(selectedLine) == testStringNormalLine.length());
        REQUIRE(std::get<2>(selectedLine) == 1);
        REQUIRE(text->linesVisible() == 3);

        // Remove whole line (+1 for newline), there should be additional previous line added for free space.
        text->removeNChars(testStringNormalLine.length() + 1);

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringShortLine + "\n");
        REQUIRE(std::get<1>(selectedLine) == testStringShortLine.length());
        REQUIRE(std::get<2>(selectedLine) == 1);
        REQUIRE(text->linesVisible() == 3);

        // Go to document ending (with multiple moves)
        text->moveCursor(gui::NavigationDirection::DOWN, 10);
        text->moveCursor(gui::NavigationDirection::RIGHT, 100);

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == testStringLongLine);
        REQUIRE(std::get<1>(selectedLine) == testStringLongLine.length());
        REQUIRE(std::get<2>(selectedLine) == 2);
        REQUIRE(text->linesVisible() == 3);

        // Add character so line would change (extra spaces to cover line width)
        std::string addText = "Additional Text for testing";
        text->addText(std::string("        " + addText));

        selectedLine = text->getSelectedLine();

        REQUIRE(std::get<0>(selectedLine)->getText(0).c_str() == addText);
        REQUIRE(std::get<1>(selectedLine) == addText.length());
        REQUIRE(std::get<2>(selectedLine) == 2);
        REQUIRE(text->linesVisible() == 3);
    }
}
