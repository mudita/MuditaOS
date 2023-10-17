// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <widgets/text/TextFixedSize.hpp>
#include "InitializedFontManager.hpp"

#include <catch2/catch.hpp>

namespace gui
{
    class TestText : public TextFixedSize
    {
      public:
        [[nodiscard]] auto getLine()
        {
            return lines->getLine();
        }

        auto moveCursor(NavigationDirection direction, unsigned int n)
        {
            cursor->moveCursor(direction, n);
        }

        auto removeNChars(unsigned int n)
        {
            for (unsigned int i = 0; i < n; i++) {
                removeChar();
            }
        }

        [[nodiscard]] auto checkOneLine() -> bool
        {
            return lines->size() == 1;
        }

        auto checkCursorDocumentBegin()
        {
            return cursor->atBegin();
        }

        auto checkCursorDocumentEnd()
        {
            return cursor->atEnd();
        }

        auto getLineText()
        {
            return getLine()->getText(0);
        }

        bool checkEllipsis(TextEllipsis ellipsis)
        {
            auto leftCondition =
                getLineText().substr(0, UTF8(text::ellipsis_signs).length()) == UTF8(text::ellipsis_signs);
            auto rightCondition =
                getLineText().substr(getLineText().length() - UTF8(text::ellipsis_signs).length(),
                                     UTF8(text::ellipsis_signs).length()) == UTF8(text::ellipsis_signs);

            switch (ellipsis) {
            case TextEllipsis::Left:
                return leftCondition && !rightCondition;
            case TextEllipsis::Right:
                return !leftCondition && rightCondition;
            case TextEllipsis::Both:
                return leftCondition && rightCondition;
            default:
                return false;
            }
        }
    };
} // namespace gui

TEST_CASE("TextInLineCursor - navigation test with ellipsis")
{
    using namespace gui;

    std::string testStringLine = "Test Long Long Long Long Line "; // 30 sings
    unsigned int lineLength    = 10;

    SECTION("Space for 10 sings - navigation from document begin")
    {
        mockup::fontManager();
        auto text = std::make_unique<gui::TestText>();
        text->setSize(120, 30);
        text->setTextType(gui::TextType::SingleLine);
        text->setCursorStartPosition(gui::CursorStartPosition::DocumentBegin);

        text->setText(testStringLine);

        REQUIRE(text->checkOneLine());
        REQUIRE(text->getLine()->length() == lineLength);
        REQUIRE(text->getLine()->getText(0) == testStringLine.substr(0, lineLength));
        REQUIRE(text->checkCursorDocumentBegin());

        // Move cursor at line ending -> content should be the same
        text->moveCursor(gui::NavigationDirection::RIGHT, lineLength);
        REQUIRE(text->getLineText() == testStringLine.substr(0, lineLength));

        // Move cursor one sign more and check content
        text->moveCursor(gui::NavigationDirection::RIGHT, 1);
        REQUIRE(text->getLineText() == testStringLine.substr(1, lineLength));

        // Move cursor 5 signs more and check content
        text->moveCursor(gui::NavigationDirection::RIGHT, 5);
        REQUIRE(text->getLineText() == testStringLine.substr(6, lineLength));

        // Move cursor to document end;
        text->moveCursor(gui::NavigationDirection::RIGHT, text->getText().length());
        REQUIRE(text->getLineText() ==
                testStringLine.substr(text->getText().length() - lineLength - 1, text->getText().length()));
        REQUIRE(text->checkCursorDocumentEnd());

        // Move cursor back for text length - cursor should be at document begin.
        text->moveCursor(gui::NavigationDirection::LEFT, text->getText().length());
        REQUIRE(text->getLineText() == testStringLine.substr(0, lineLength));
        REQUIRE(text->checkCursorDocumentBegin());
    }

    SECTION("Space for 10 sings - navigation from document end")
    {
        mockup::fontManager();
        auto text = std::make_unique<gui::TestText>();
        text->setSize(120, 30);
        text->setTextType(gui::TextType::SingleLine);
        text->setCursorStartPosition(gui::CursorStartPosition::DocumentEnd);

        text->setText(testStringLine);

        REQUIRE(text->checkOneLine());
        REQUIRE(text->getLine()->length() == lineLength + 1);
        REQUIRE(text->getLine()->getText(0) ==
                testStringLine.substr(text->getText().length() - lineLength - 1, lineLength + 1));
        REQUIRE(text->checkCursorDocumentEnd());

        // Move cursor at line begin -> content should be the same
        text->moveCursor(gui::NavigationDirection::LEFT, lineLength + 1);
        REQUIRE(text->getLine()->getText(0) ==
                testStringLine.substr(text->getText().length() - lineLength - 1, lineLength + 1));

        // Move cursor one sign more and check content
        text->moveCursor(gui::NavigationDirection::LEFT, 1);
        REQUIRE(text->getLineText() ==
                testStringLine.substr(text->getText().length() - lineLength - 2, lineLength + 1));

        // Move cursor back for text length - cursor should be at document begin.
        text->moveCursor(gui::NavigationDirection::LEFT, text->getText().length());
        REQUIRE(text->getLineText() == testStringLine.substr(0, lineLength));
        REQUIRE(text->checkCursorDocumentBegin());
    }
}

TEST_CASE("TextInLineCursor - navigation test without ellipsis")
{
    using namespace gui;

    std::string testStringLine = "Test Long Long Long Long Line "; // 30 sings
    unsigned int lineLength    = 10;

    SECTION("Space for 10 sings - navigation with left ellipsis")
    {
        mockup::fontManager();
        auto text = std::make_unique<gui::TestText>();
        text->setSize(120, 30);
        text->setTextType(gui::TextType::SingleLine);
        text->setCursorStartPosition(gui::CursorStartPosition::DocumentBegin);
        text->setTextEllipsisType(gui::TextEllipsis::Left);

        text->setText(testStringLine);

        REQUIRE(text->checkOneLine());
        REQUIRE(text->getLine()->length() == lineLength);
        REQUIRE(text->getLine()->getText(0) == testStringLine.substr(0, lineLength));
        REQUIRE(text->checkCursorDocumentBegin());

        // Move cursor at line ending -> content should be the same
        text->moveCursor(gui::NavigationDirection::RIGHT, lineLength);
        REQUIRE(text->getLineText() == testStringLine.substr(0, lineLength));

        // Move cursor at text ending -> left ellipsis should appear
        text->moveCursor(gui::NavigationDirection::RIGHT, text->getText().length());
        REQUIRE(text->checkEllipsis(gui::TextEllipsis::Left));

        // Move cursor at text beginning -> no ellipsis
        text->moveCursor(gui::NavigationDirection::LEFT, text->getText().length());
        REQUIRE(!text->checkEllipsis(gui::TextEllipsis::Left));
    }

    SECTION("Space for 10 sings - navigation with right ellipsis")
    {
        mockup::fontManager();
        auto text = std::make_unique<gui::TestText>();
        text->setSize(120, 30);
        text->setTextType(gui::TextType::SingleLine);
        text->setCursorStartPosition(gui::CursorStartPosition::DocumentBegin);
        text->setTextEllipsisType(gui::TextEllipsis::Right);

        text->setText(testStringLine);

        REQUIRE(text->checkOneLine());
        REQUIRE(text->checkCursorDocumentBegin());
        REQUIRE(text->checkEllipsis(gui::TextEllipsis::Right));

        // Move cursor at text end -> no ellipsis
        text->moveCursor(gui::NavigationDirection::RIGHT, text->getText().length());
        REQUIRE(!text->checkEllipsis(gui::TextEllipsis::Right));

        // Move cursor at text beginning -> right ellipsis should appear
        text->moveCursor(gui::NavigationDirection::LEFT, text->getText().length());
        REQUIRE(text->checkEllipsis(gui::TextEllipsis::Right));
    }

    SECTION("Space for 10 sings - navigation with both ellipsis")
    {
        mockup::fontManager();
        auto text = std::make_unique<gui::TestText>();
        text->setSize(120, 30);
        text->setTextType(gui::TextType::SingleLine);
        text->setCursorStartPosition(gui::CursorStartPosition::DocumentBegin);
        text->setTextEllipsisType(gui::TextEllipsis::Both);

        text->setText(testStringLine);

        REQUIRE(text->checkOneLine());
        REQUIRE(text->checkCursorDocumentBegin());
        REQUIRE(text->checkEllipsis(gui::TextEllipsis::Right));
        REQUIRE(!text->checkEllipsis(gui::TextEllipsis::Both));

        // Move cursor at middle -> both ellipsis
        text->moveCursor(gui::NavigationDirection::RIGHT, lineLength);
        REQUIRE(text->checkEllipsis(gui::TextEllipsis::Both));

        // Move cursor at text end -> left ellipsis should appear
        text->moveCursor(gui::NavigationDirection::RIGHT, text->getText().length());
        REQUIRE(text->checkEllipsis(gui::TextEllipsis::Left));
        REQUIRE(!text->checkEllipsis(gui::TextEllipsis::Both));

        // Move cursor at text beginning -> right ellipsis should appear
        text->moveCursor(gui::NavigationDirection::LEFT, text->getText().length());
        REQUIRE(text->checkEllipsis(gui::TextEllipsis::Right));
        REQUIRE(!text->checkEllipsis(gui::TextEllipsis::Both));
    }
}
