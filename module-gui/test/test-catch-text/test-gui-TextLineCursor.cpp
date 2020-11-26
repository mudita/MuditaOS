// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <limits>
#include <module-gui/gui/widgets/TextBlock.hpp>
#include <mock/buildTextDocument.hpp>
#include <module-gui/gui/widgets/Text.hpp>
#include <mock/multi-line-string.hpp>
#include "Font.hpp"
#include "InitializedFontManager.hpp"

namespace gui
{
    class TestText : public Text
    {
      public:
        [[nodiscard]] unsigned int linesSize()
        {
            return lines->get().size();
        }

        [[nodiscard]] auto &linesGet()
        {
            return lines->get();
        }

        auto moveCursor(NavigationDirection direction, unsigned int n)
        {
            cursor->TextCursor::moveCursor(direction, n);
        }

        [[nodiscard]] auto getSelectedLine()
        {
            return cursor->getSelectedLine();
        }
    };
} // namespace gui

// To be corrected with Text scrolling
TEST_CASE("TextLineCursor")
{
    using namespace gui;

    std::string testStringShortLine  = "Test Line ";                     // 10 sings
    std::string testStringNormalLine = "Test Normal No Line ";           // 20 sings
    std::string testStringLongLine   = "Test Long Long Long Long Line "; // 30 sings

    SECTION("Default position three lines text")
    {
        mockup::fontManager();
        auto text = new gui::TestText();
        text->setMaximumSize(400, 200);

        text->addText(TextBlock(testStringShortLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringNormalLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringLongLine, Font(27).raw(), TextBlock::End::None));

        auto [selectedLine, selectedLineCursorPos, selectedLineNr] = text->getSelectedLine();

        REQUIRE(selectedLine->getText(0).c_str() == testStringLongLine);
        REQUIRE(selectedLineCursorPos == testStringLongLine.length());
        REQUIRE(selectedLineNr == 2);
    }

    SECTION("Movement three lines text test -> short|normal|long")
    {
        mockup::fontManager();
        auto text = new gui::TestText();
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
        auto text = new gui::TestText();
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
