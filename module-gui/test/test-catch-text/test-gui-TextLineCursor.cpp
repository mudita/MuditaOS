#include <catch2/catch.hpp>
#include <limits>
#include <module-gui/gui/widgets/TextBlock.hpp>
#include <mock/buildTextDocument.hpp>
#include <module-gui/gui/widgets/Text.hpp>
#include <mock/multi-line-string.hpp>

TEST_CASE("TextLineCursor")
{
    using namespace gui;

    SECTION("default")
    {
        auto texts            = mockup::lineStrings(3);
        auto [document, font] = mockup::buildMultilineTestDocument(texts);
        gui::Text text;
        text.setText(std::make_unique<gui::TextDocument>(document));
        auto cursor = new gui::TextLineCursor(&text);

        REQUIRE(cursor->getScreenLine() == 0);
    }

    SECTION("move cursor down")
    {
        auto texts            = mockup::lineStrings(3);
        auto [document, font] = mockup::buildMultilineTestDocument(texts);

        gui::Text text;
        text.setText(std::make_unique<gui::TextDocument>(document));
        auto cursor = new gui::TextLineCursor(&text);
        cursor->moveCursor(NavigationDirection::DOWN);
        REQUIRE(cursor->getScreenLine() == 1);
    }

    SECTION("move cursor right")
    {
        auto texts            = mockup::lineStrings(3);
        auto [document, font] = mockup::buildMultilineTestDocument(texts);

        gui::Text text;
        text.setText(std::make_unique<gui::TextDocument>(document));
        auto cursor = new gui::TextLineCursor(&text);
        cursor->moveCursor(NavigationDirection::RIGHT);
        REQUIRE(cursor->getScreenLine() == 0);

        for (size_t i = 0; i < document.getBlocks().front().length(); i++) {
            cursor->moveCursor(NavigationDirection::RIGHT);
        }
        REQUIRE(cursor->getScreenLine() == 1);
    }

    SECTION("move cursor to end")
    {
        auto texts            = mockup::lineStrings(3);
        auto [document, font] = mockup::buildMultilineTestDocument(texts);

        gui::Text text;
        text.setText(std::make_unique<gui::TextDocument>(document));
        auto cursor = new gui::TextLineCursor(&text);
        cursor->moveCursor(NavigationDirection::RIGHT);
        REQUIRE(cursor->getScreenLine() == 0);

        for (size_t i = 0; i < document.getText().length(); i++) {
            cursor->moveCursor(NavigationDirection::RIGHT);
        }
        REQUIRE(cursor->getScreenLine() == 2);
    }
}