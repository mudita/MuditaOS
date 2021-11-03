// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "InitializedFontManager.hpp"

#include <widgets/text/parsers/TextParse.hpp>
#include <widgets/text/parsers/RichTextParser.hpp>
#include <widgets/text/Text.hpp>
#include <widgets/text/TextFixedSize.hpp>

#include <mock/multi-line-string.hpp>
#include <i18n/i18n.hpp>
#include <Font.hpp>

#include <catch2/catch.hpp>

TEST_CASE("Text ctor")
{
    using namespace gui;
    mockup::fontManager();

    SECTION("none")
    {
        auto text = Text();
        REQUIRE(text.getText() == "");
    }

    SECTION("empty 0")
    {
        auto text = Text(nullptr, 0, 0, 0, 0);
        REQUIRE(text.getText() == "");
    }

    SECTION("one line")
    {
        auto testtext = "text0 text1 text2";
        auto text     = Text(nullptr, 0, 0, 0, 0);
        text.setText(testtext);
        REQUIRE(text.getText() == testtext);
    }
}

namespace gui
{
    class TestText : public Text
    {
      public:
        [[nodiscard]] unsigned int linesSize()
        {
            return lines->get().size();
        }

        void drawLines() override
        {
            Text::drawLines();
            return;
        }

        [[nodiscard]] auto &linesGet()
        {
            return lines->get();
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

        [[nodiscard]] auto *getInputMode()
        {
            return mode;
        }

        auto removeNCharacters(unsigned int n)
        {
            for (unsigned int i = 0; i < n; i++) {
                removeChar();
            }
        }

        auto moveCursor(NavigationDirection direction, unsigned int n)
        {
            cursor->moveCursor(direction, n);
        }

        [[nodiscard]] auto getCursorPosition()
        {
            return cursor->getAbsolutePosition();
        }

        void setExpandMode(ExpandMode expandMode)
        {
            this->expandMode = expandMode;
        }
    };
} // namespace gui

TEST_CASE("Text drawLines")
{

    using namespace gui;

    auto &fontmanager = mockup::fontManager();

    SECTION("one line")
    {
        auto words_count = 3;
        auto testline    = mockup::multiWordString(words_count);
        auto text        = TestText();
        text.setSize(400, 100);
        text.setText(testline);
        text.drawLines();
        REQUIRE(text.linesSize() == 1);
    }

    SECTION("all multiline visible with newline end")
    {
        unsigned int lines_count = 4;
        auto testline            = mockup::multiLineString(lines_count);
        auto text                = TestText();
        text.setSize(300, 500);
        text.setText(
            std::make_unique<TextDocument>(textToTextBlocks(testline, fontmanager.getFont(), TextBlock::End::Newline)));

        text.drawLines();
        // Extra one line for empty newline at end
        REQUIRE(text.linesSize() == lines_count + 1);
    }
}

TEST_CASE("Text buildDrawList")
{
    using namespace gui;

    auto &fontmanager = mockup::fontManager();
    auto lines_count  = 10;
    auto testline     = mockup::multiLineString(lines_count);
    auto font         = fontmanager.getFont();
    auto text         = TestText();
    text.setSize(3000, 3000);
    text.setText(std::make_unique<TextDocument>(textToTextBlocks(testline, font, TextBlock::End::Newline)));

    SECTION("check every element in every line if it's in parent and has non zero sizes apart from last empty line "
            "from newline")
    {
        REQUIRE(text.linesGet().size() > 0);
        for (auto &line : text.linesGet()) {
            if (&line == &text.linesGet().back()) {
                REQUIRE(line.length() == 0);
            }
            else {
                REQUIRE(line.length() > 0);
            }
        }
    }
}

TEST_CASE("handle input mode ABC/abc/1234")
{
    utils::setInputLanguage("English"); /// needed to load input mode
    auto &fontmanager = mockup::fontManager();
    auto font         = fontmanager.getFont();
    auto text         = gui::TestText();
    auto modes        = {InputMode::ABC, InputMode::abc, InputMode::digit};
    auto str          = text.getText();
    auto next_mode    = gui::InputEvent({}, gui::InputEvent::State::keyReleasedShort, gui::KeyCode::KEY_AST);
    auto key_2        = gui::InputEvent(
        {
            RawKey::State::Released,
            bsp::KeyCodes::NumericKey2,
        },
        gui::InputEvent::State::keyReleasedShort);
    text.setInputMode(new InputMode(modes));
    text.setFont(font);

    SECTION("ABC -> abc")
    {
        REQUIRE(text.getInputMode()->is(InputMode::ABC));
        text.onInput(key_2);
        str += "A";
        REQUIRE(str == text.getText());

        text.onInput(next_mode);
        REQUIRE(text.getInputMode()->is(InputMode::abc));
        text.onInput(key_2);
        str += "a";
        REQUIRE(!(str == text.getText())); // not equal because its `key 2 double click..., A was changed to a`
    }

    SECTION("ABC -> abc")
    {
        auto time_long_enough_to_not_be_multipress = 1500;
        text.onInput(next_mode);
        auto rawKey_2 = key_2.getRawKey();
        rawKey_2.timeRelease += time_long_enough_to_not_be_multipress;
        REQUIRE(text.getInputMode()->is(InputMode::abc));
        text.onInput(gui::InputEvent{rawKey_2, key_2.getState()});
        rawKey_2.timeRelease += time_long_enough_to_not_be_multipress;
        str += "a";
        REQUIRE(str == text.getText());

        text.onInput(gui::InputEvent{rawKey_2, key_2.getState()});
        str += "a";
        REQUIRE(str == text.getText());
    }

    SECTION("ABC -> abc -> digit")
    {
        text.onInput(next_mode);
        text.onInput(next_mode);
        REQUIRE(text.getInputMode()->is(InputMode::digit));
        text.onInput(key_2);
        str += "2";
        REQUIRE(str == text.getText());
    }
}

TEST_CASE("handle longpress for digit in ABC mode")
{
    auto text  = gui::TestText();
    auto str   = text.getText() + "1";
    auto key_1 = gui::InputEvent({}, gui::InputEvent::State::keyReleasedLong, gui::KeyCode::KEY_1);
    text.setInputMode(new InputMode({InputMode::ABC}));
    text.onInput(key_1);
    REQUIRE(str == text.getText());
}

TEST_CASE("handle longpress for digit in phone mode")
{
    auto text  = gui::TestText();
    auto str   = text.getText() + "+";
    auto key_0 = gui::InputEvent({}, gui::InputEvent::State::keyReleasedLong, gui::KeyCode::KEY_0);
    text.setInputMode(new InputMode({InputMode::phone}));
    text.onInput(key_0);
    REQUIRE(str == text.getText());
}

TEST_CASE("Handle backspace longpress")
{
    auto text          = gui::TestText();
    auto key_backspace = gui::InputEvent({}, gui::InputEvent::State::keyReleasedLong, gui::KeyCode::KEY_PND);
    text.setInputMode(new InputMode({InputMode::ABC}));

    SECTION("Empty text")
    {
        REQUIRE(text.getText().empty());
        auto input_handled = text.onInput(key_backspace);
        REQUIRE(input_handled == false);
    }

    SECTION("Not empty text")
    {
        text.addText("test");
        REQUIRE(text.getText() == "test");
        text.onInput(key_backspace);
        REQUIRE(text.getText().empty());
    }
}

TEST_CASE("handle text expand")
{
    mockup::fontManager();
    using namespace gui;
    Length w         = 10;
    Length h         = 100;
    BoxLayout layout = BoxLayout(nullptr, 0, 0, w, h);
    auto text        = new gui::TestText();
    text->setMaximumSize(w, h);
    text->setText("");
    layout.addWidget(text);
    REQUIRE(text->area() != BoundingBox{0, 0, 0, 0});
}

TEST_CASE("handle newline")
{
    mockup::fontManager();
    using namespace gui;
    Length w         = 10;
    Length h         = 100;
    BoxLayout layout = BoxLayout(nullptr, 0, 0, w, h);
    auto text        = new gui::TestText();
    layout.addWidget(text);
    text->setMaximumSize(w, h);
    text->addText("\n");
    REQUIRE(text->getText() == "\n");
    text->addText("text");
    REQUIRE(text->getText() == "\ntext");
    text->addText("\n");
    REQUIRE(text->getText() == "\ntext\n");
}

TEST_CASE("handle text block - moved cursor to end")
{
    std::string test_text = "text";
    std::string newline   = "\n";
    auto text             = gui::TestText();
    text.addText(gui::TextBlock(test_text, mockup::fontManager().getFont(), gui::TextBlock::End::None));
    REQUIRE(text.getText() == test_text);
    test_text = test_text + newline;
    text.addText(newline);
    REQUIRE(text.getText() == test_text);
}

TEST_CASE("Text backup overwrite and restore tests")
{
    const std::string testStringOneLine   = "Test String ";
    const std::string testStringTwoLines  = "Test String 1 \n Test String 2";
    const std::string overwriteTestString = "Overwrite test String";

    SECTION("Backup one line text with moved cursor")
    {
        mockup::fontManager();
        auto text = std::make_unique<gui::TestText>();

        text->addText(testStringOneLine);

        unsigned int cursorMoveN = 2;
        text->moveCursor(gui::NavigationDirection::LEFT, cursorMoveN);

        auto backup = text->backupText();

        REQUIRE(backup.document.getText() == text->getText());
        REQUIRE(backup.document.getText().length() == text->getText().length());
        REQUIRE(backup.cursorPos == text->getCursorPosition());

        text->setText(overwriteTestString);

        REQUIRE(text->getText() != testStringOneLine);

        text->restoreFrom(backup);

        REQUIRE(text->getText() == testStringOneLine);
        REQUIRE(text->getCursorPosition() == testStringOneLine.length() - cursorMoveN);
    }

    SECTION("Backup two line text with moved cursor")
    {
        mockup::fontManager();
        auto text = std::make_unique<gui::TestText>();

        text->addText(testStringTwoLines);

        unsigned int cursorMoveN = 10;
        text->moveCursor(gui::NavigationDirection::LEFT, cursorMoveN);

        auto backup = text->backupText();

        REQUIRE(backup.document.getText() == text->getText());
        REQUIRE(backup.document.getText().length() == text->getText().length());
        REQUIRE(backup.cursorPos == text->getCursorPosition());

        text->setText(overwriteTestString);

        REQUIRE(text->getText() != testStringOneLine);

        text->restoreFrom(backup);

        REQUIRE(text->getText() == testStringTwoLines);
        REQUIRE(text->getCursorPosition() == testStringTwoLines.length() - cursorMoveN);
    }
}

TEST_CASE("Text backup with max width restricted - overwrite restore and add tests")
{
    const std::string testStringOneLine   = "Test String ";
    const std::string testStringTwoLines  = "Test String 1 \n Test String 2";
    const std::string overwriteTestString = "Overwrite test String";
    const std::string toAppendString1     = "Some text1";
    const std::string toAppendString2     = "Some text2";

    auto &fontManager = mockup::fontManager();
    auto font         = fontManager.getFont("gt_pressura_light_27");
    gui::TestText text;
    text.setFont(font);
    const auto textMaxHeight = 47;

    SECTION("Backup single line text")
    {
        const std::string expectedString1 = testStringOneLine + toAppendString1;
        const std::string expectedString2 = testStringOneLine + toAppendString1 + toAppendString2;
        text.setMaximumSize(font->getPixelWidth(testStringOneLine), textMaxHeight);

        text.setText(testStringOneLine);
        text.addText(toAppendString1);
        REQUIRE(text.getCursorPosition() == expectedString1.length());

        auto backup = text.backupText();

        REQUIRE(backup.document.getText() == text.getText());
        REQUIRE(backup.document.getText().length() == text.getText().length());
        REQUIRE(backup.cursorPos == text.getCursorPosition());

        text.setText(overwriteTestString);
        REQUIRE(text.getText() != expectedString1);

        text.restoreFrom(backup);
        text.addText(toAppendString2);

        REQUIRE(text.getText() == expectedString2);
        REQUIRE(text.getCursorPosition() == expectedString2.length());
    }

    SECTION("Backup single line text with moved cursor")
    {
        const std::string expectedString1 = testStringOneLine + toAppendString1;
        const std::string expectedString2 = testStringOneLine + toAppendString2 + toAppendString1;
        text.setMaximumSize(font->getPixelWidth(testStringOneLine), textMaxHeight);

        text.setText(testStringOneLine);
        text.addText(toAppendString1);

        const unsigned int cursorMoveN = toAppendString1.length();
        text.moveCursor(gui::NavigationDirection::LEFT, cursorMoveN);
        REQUIRE(text.getCursorPosition() == expectedString1.length() - cursorMoveN);

        auto backup = text.backupText();

        REQUIRE(backup.document.getText() == text.getText());
        REQUIRE(backup.document.getText().length() == text.getText().length());
        REQUIRE(backup.cursorPos == text.getCursorPosition());

        text.setText(overwriteTestString);
        REQUIRE(text.getText() != expectedString1);

        text.restoreFrom(backup);
        text.addText(toAppendString2);

        REQUIRE(text.getText() == expectedString2);
        REQUIRE(text.getCursorPosition() == testStringOneLine.length() + toAppendString2.length());
    }

    SECTION("Backup multi line text")
    {
        const std::string expectedString1 = testStringTwoLines + toAppendString1;
        const std::string expectedString2 = testStringTwoLines + toAppendString1 + toAppendString2;
        text.setMaximumSize(font->getPixelWidth(testStringTwoLines), textMaxHeight);

        text.setText(testStringTwoLines);
        text.addText(toAppendString1);
        REQUIRE(text.getCursorPosition() == expectedString1.length());

        auto backup = text.backupText();

        REQUIRE(backup.document.getText() == text.getText());
        REQUIRE(backup.document.getText().length() == text.getText().length());
        REQUIRE(backup.cursorPos == text.getCursorPosition());

        text.setText(overwriteTestString);
        REQUIRE(text.getText() != expectedString1);

        text.restoreFrom(backup);
        text.addText(toAppendString2);

        REQUIRE(text.getText() == expectedString2);
        REQUIRE(text.getCursorPosition() == expectedString2.length());
    }

    SECTION("Backup multi line text with moved cursor")
    {
        const std::string expectedString1 = testStringTwoLines + toAppendString1;
        const std::string expectedString2 = testStringTwoLines + toAppendString2 + toAppendString1;
        text.setMaximumSize(font->getPixelWidth(testStringTwoLines), textMaxHeight);

        text.setText(testStringTwoLines);
        text.addText(toAppendString1);

        const unsigned int cursorMoveN = toAppendString1.length();
        text.moveCursor(gui::NavigationDirection::LEFT, cursorMoveN);
        REQUIRE(text.getCursorPosition() == expectedString1.length() - cursorMoveN);

        auto backup = text.backupText();

        REQUIRE(backup.document.getText() == text.getText());
        REQUIRE(backup.document.getText().length() == text.getText().length());
        REQUIRE(backup.cursorPos == text.getCursorPosition());

        text.setText(overwriteTestString);
        REQUIRE(text.getText() != expectedString1);

        text.restoreFrom(backup);
        text.addText(toAppendString2);

        REQUIRE(text.getText() == expectedString2);
        REQUIRE(text.getCursorPosition() == testStringTwoLines.length() + toAppendString2.length());
    }
}

TEST_CASE("Text addition bounds - text sings count restricted")
{
    std::string testStringOneLine  = "Test String 1";
    std::string testStringTwoLines = "Test String 1\n Test String 2";

    std::string richTextTwoLines =
        "<text font='gt_pressura' color='12' size='30'>Test</text><text size='25'>String</text><text size='20' "
        "weight='bold'>1</text><br></br><text>Test String 2</text>";

    SECTION("Adding text to max signs count set to 0")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setTextLimitType(gui::TextLimitType::MaxSignsCount, 0);

        text->addText(testStringOneLine);

        REQUIRE(text->getText().empty());
    }

    SECTION("Adding text to max signs count set to 1")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setTextLimitType(gui::TextLimitType::MaxSignsCount, 1);

        text->addText(testStringOneLine);

        REQUIRE(text->getText().length() == 1);
    }

    SECTION("Adding two lines text to max signs count set to one line sings count")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setTextLimitType(gui::TextLimitType::MaxSignsCount, testStringOneLine.length());

        text->addText(testStringTwoLines);

        REQUIRE(text->getText().length() == testStringOneLine.length());
    }

    SECTION("Adding two lines text to max signs count set to two line sings count")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setTextLimitType(gui::TextLimitType::MaxSignsCount, testStringTwoLines.length());

        text->addText(testStringTwoLines);

        REQUIRE(text->getText().length() == testStringTwoLines.length());
    }

    SECTION("Adding TextBlock with max sign count lower than block size")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setTextLimitType(gui::TextLimitType::MaxSignsCount, testStringOneLine.length());

        text->addText(TextBlock(testStringTwoLines, Font(27).raw(), TextBlock::End::None));

        REQUIRE(text->getText().length() == testStringOneLine.length());
        REQUIRE(text->getText().length() != testStringTwoLines.length());
    }

    SECTION("Adding TextBlock with max sign count greater than block and adding more text after")
    {
        auto additionalSpace = 5;
        auto textLimit       = testStringOneLine.length() + additionalSpace;

        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setTextLimitType(gui::TextLimitType::MaxSignsCount, textLimit);

        text->addText(TextBlock(testStringOneLine, Font(27).raw(), TextBlock::End::None));

        REQUIRE(text->getText().length() == testStringOneLine.length());

        text->addText(testStringOneLine);

        REQUIRE(text->getText().length() == testStringOneLine.length() + additionalSpace);
    }

    SECTION("Adding RichText with max sign count lower than block size")
    {
        unsigned int signCountRestricted = 5;

        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();

        text->setTextLimitType(gui::TextLimitType::MaxSignsCount, signCountRestricted);

        text->addRichText(richTextTwoLines);

        REQUIRE(text->getText().length() == signCountRestricted);
    }

    SECTION("Adding RichText with max sign count greater than block and adding more text after")
    {
        auto additionalSpace = 5;

        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();

        auto format         = text->getTextFormat();
        auto parsedRichText = gui::text::RichTextParser().parse(richTextTwoLines, &format);
        auto textLimit      = parsedRichText->getText().length() + additionalSpace;

        text->setTextLimitType(gui::TextLimitType::MaxSignsCount, textLimit);

        text->addRichText(richTextTwoLines);

        REQUIRE(text->getText().length() == parsedRichText->getText().length());

        text->addText(testStringOneLine);

        REQUIRE(text->getText().length() == parsedRichText->getText().length() + additionalSpace);
    }
}

TEST_CASE("Text addition bounds - text widget size restricted")
{
    std::string testStringOneLine  = "Test String 1";
    std::string testStringTwoLines = "Test String 1\nTest String 2";

    std::string testStringFirstLine  = "Test String 1";
    std::string testStringSecondLine = "Test String 2";

    std::string richTextTwoLines =
        "<text font='gt_pressura' color='12' size='30'>Test </text><text size='20'>String </text><text size='27' "
        "weight='bold'>1</text><br></br><text>Test String 2</text>";

    SECTION("Adding text to 0 size text and no parent to grant size")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setTextLimitType(gui::TextLimitType::MaxSize);
        text->setMaximumSize(0, 0);

        text->addText(testStringOneLine);

        REQUIRE(text->getText().empty());
    }

    SECTION("Adding text to text with parent max size set to not fit")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = new gui::TestText();
        text->setTextLimitType(gui::TextLimitType::MaxSize);
        BoxLayout layout = BoxLayout(nullptr, 0, 0, 0, 0);
        layout.setMaximumSize(10, 10);
        layout.addWidget(text);

        text->addText(testStringOneLine);

        REQUIRE(text->getText().empty());
    }

    SECTION("Adding two lines text to text with no parent and size set to fit one line")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setTextLimitType(gui::TextLimitType::MaxSize);
        text->setMaximumSize(200, 30);

        text->addText(testStringTwoLines);

        REQUIRE(text->linesSize() == 1);
        REQUIRE(text->getText() == testStringOneLine);
        REQUIRE(text->getText() != testStringTwoLines);
    }

    SECTION("Adding two lines text to text with no parent and size set to fit two line")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setTextLimitType(gui::TextLimitType::MaxSize);
        text->setMaximumSize(200, 60);

        text->addText(testStringTwoLines);

        REQUIRE(text->linesSize() == 2);
        REQUIRE(text->getText() != testStringOneLine);
        REQUIRE(text->getText() == testStringTwoLines);
    }

    SECTION("Adding two lines text to text with space for two lines and parent size set to fit one line")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = new gui::TestText();
        text->setTextLimitType(gui::TextLimitType::MaxSize);
        text->setMaximumSize(200, 60);
        BoxLayout layout = BoxLayout(nullptr, 0, 0, 0, 0);
        layout.setMaximumSize(200, 30);
        layout.addWidget(text);

        text->addText(testStringTwoLines);

        REQUIRE(text->linesSize() == 1);
        REQUIRE(text->getText() == testStringOneLine);
        REQUIRE(text->getText() != testStringTwoLines);
    }

    SECTION("Adding text block to 0 size text and no parent to grant size")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setTextLimitType(gui::TextLimitType::MaxSize);
        text->setMaximumSize(0, 0);

        text->addText(TextBlock(testStringOneLine, Font(27).raw(), TextBlock::End::None));

        REQUIRE(text->getText().empty());
    }

    SECTION("Adding text block to text with parent max size set to not fit")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = new gui::TestText();
        text->setTextLimitType(gui::TextLimitType::MaxSize);
        BoxLayout layout = BoxLayout(nullptr, 0, 0, 0, 0);
        layout.setMaximumSize(10, 10);
        layout.addWidget(text);

        text->addText(TextBlock(testStringOneLine, Font(27).raw(), TextBlock::End::None));

        REQUIRE(text->getText().empty());
    }

    SECTION("Adding two lines text block to text with no parent and size set to fit one line")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setTextLimitType(gui::TextLimitType::MaxSize);
        text->setMaximumSize(150, 30);

        text->addText(TextBlock(testStringFirstLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringSecondLine, Font(27).raw(), TextBlock::End::None));

        REQUIRE(text->linesSize() == 1);
        REQUIRE(text->getText() == testStringFirstLine);
        REQUIRE(text->getText() != testStringTwoLines);
    }

    SECTION("Adding two lines text block to text with no parent and size set to fit two line")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setTextLimitType(gui::TextLimitType::MaxSize);
        text->setMaximumSize(200, 60);

        text->addText(TextBlock(testStringFirstLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringSecondLine, Font(27).raw(), TextBlock::End::None));

        REQUIRE(text->linesSize() == 2);
        REQUIRE(text->getText() != testStringFirstLine);
        REQUIRE(text->getText() == testStringTwoLines);
    }

    SECTION("Adding two lines text block to text with space for two lines and parent size set to fit one line")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = new gui::TestText();
        text->setTextLimitType(gui::TextLimitType::MaxSize);
        text->setMaximumSize(200, 60);
        BoxLayout layout = BoxLayout(nullptr, 0, 0, 0, 0);
        layout.setMaximumSize(150, 30);
        layout.addWidget(text);

        text->addText(TextBlock(testStringFirstLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringSecondLine, Font(27).raw(), TextBlock::End::None));

        REQUIRE(text->linesSize() == 1);
        REQUIRE(text->getText() == testStringFirstLine);
        REQUIRE(text->getText() != testStringTwoLines);
    }

    SECTION("Adding RichText to 0 size text and no parent to grant size")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setTextLimitType(gui::TextLimitType::MaxSize);
        text->setMaximumSize(0, 0);

        text->addRichText(richTextTwoLines);

        REQUIRE(text->getText().empty());
    }

    SECTION("Adding RichText to text with parent max size set to not fit")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = new gui::TestText();
        text->setTextLimitType(gui::TextLimitType::MaxSize);
        BoxLayout layout = BoxLayout(nullptr, 0, 0, 0, 0);
        layout.setMaximumSize(10, 10);
        layout.addWidget(text);

        text->addRichText(richTextTwoLines);

        REQUIRE(text->getText().empty());
    }

    SECTION("Adding two lines RichText to text with no parent and size set to fit one line")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setTextLimitType(gui::TextLimitType::MaxSize);
        text->setMaximumSize(140, 40);

        text->addRichText(richTextTwoLines);

        REQUIRE(text->linesSize() == 1);
        REQUIRE(text->getText() == testStringFirstLine);
        REQUIRE(text->getText() != testStringTwoLines);
    }

    SECTION("Adding two lines RichText to text with no parent and size set to fit two line")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setTextLimitType(gui::TextLimitType::MaxSize);
        text->setMaximumSize(200, 80);

        text->addRichText(richTextTwoLines);

        REQUIRE(text->linesSize() == 2);
        REQUIRE(text->getText() != testStringFirstLine);
        REQUIRE(text->getText() == testStringTwoLines);
    }

    SECTION("Adding two lines RichText to text with space for two lines and parent size set to fit one line")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = new gui::TestText();
        text->setTextLimitType(gui::TextLimitType::MaxSize);
        text->setMaximumSize(200, 80);
        BoxLayout layout = BoxLayout(nullptr, 0, 0, 0, 0);
        layout.setMaximumSize(140, 40);
        layout.addWidget(text);

        text->addRichText(richTextTwoLines);

        REQUIRE(text->linesSize() == 1);
        REQUIRE(text->getText() == testStringFirstLine);
        REQUIRE(text->getText() != testStringTwoLines);
    }
}

TEST_CASE("Text addition bounds - text widget line size restricted")
{
    std::string testStringOneLine  = "Test String 1";
    std::string testStringTwoLines = "Test String 1\nTest String 2";

    std::string testStringFirstLine  = "Test String 1";
    std::string testStringSecondLine = "Test String 2";

    std::string richTextTwoLines =
        "<text font='gt_pressura' color='12' size='30'>Test </text><text size='20'>String </text><text size='27' "
        "weight='bold'>1</text><br></br><text>Test String 2</text>";

    SECTION("Adding text to 0 line size text")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setTextLimitType(gui::TextLimitType::MaxLines, 0);
        text->setMaximumSize(150, 100);

        text->addText(testStringOneLine);

        REQUIRE(text->getText().empty());
    }

    SECTION("Adding two lines text to 1 line size text")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setTextLimitType(gui::TextLimitType::MaxLines, 1);
        text->setMaximumSize(150, 100);

        text->addText(testStringTwoLines);

        REQUIRE(text->linesSize() == 1);
        REQUIRE(text->getText() == testStringOneLine);
        REQUIRE(text->getText() != testStringTwoLines);
    }

    SECTION("Adding two lines text to 2 line size text")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setTextLimitType(gui::TextLimitType::MaxLines, 2);
        text->setMaximumSize(150, 100);

        text->addText(testStringTwoLines);

        REQUIRE(text->linesSize() == 2);
        REQUIRE(text->getText() == testStringTwoLines);
        REQUIRE(text->getText() != testStringOneLine);
    }

    SECTION("Adding text block to 0 line size text")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setTextLimitType(gui::TextLimitType::MaxLines, 0);
        text->setMaximumSize(150, 100);

        text->addText(TextBlock(testStringOneLine, Font(27).raw(), TextBlock::End::None));

        REQUIRE(text->getText().empty());
    }

    SECTION("Adding two block lines text to 1 line size text")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setTextLimitType(gui::TextLimitType::MaxLines, 1);
        text->setMaximumSize(150, 100);

        text->addText(TextBlock(testStringFirstLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringSecondLine, Font(27).raw(), TextBlock::End::None));

        REQUIRE(text->linesSize() == 1);
        REQUIRE(text->getText() == testStringOneLine);
        REQUIRE(text->getText() != testStringTwoLines);
    }

    SECTION("Adding two lines text to 2 line size text")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setTextLimitType(gui::TextLimitType::MaxLines, 2);
        text->setMaximumSize(150, 100);

        text->addText(TextBlock(testStringFirstLine, Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock(testStringSecondLine, Font(27).raw(), TextBlock::End::None));

        REQUIRE(text->linesSize() == 2);
        REQUIRE(text->getText() == testStringTwoLines);
        REQUIRE(text->getText() != testStringOneLine);
    }

    SECTION("Adding RichText to 0 line size text")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setTextLimitType(gui::TextLimitType::MaxLines, 0);
        text->setMaximumSize(150, 100);

        text->addRichText(richTextTwoLines);

        REQUIRE(text->getText().empty());
    }

    SECTION("Adding two lines RichText to 1 line size text")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setTextLimitType(gui::TextLimitType::MaxLines, 1);
        text->setMaximumSize(140, 100);

        text->addRichText(richTextTwoLines);

        REQUIRE(text->linesSize() == 1);
        REQUIRE(text->getText() == testStringFirstLine);
        REQUIRE(text->getText() != testStringTwoLines);
    }

    SECTION("Adding two lines RichText to 2 line size text")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setTextLimitType(gui::TextLimitType::MaxLines, 2);
        text->setMaximumSize(140, 100);

        text->addRichText(richTextTwoLines);

        REQUIRE(text->linesSize() == 2);
        REQUIRE(text->getText() == testStringTwoLines);
        REQUIRE(text->getText() != testStringFirstLine);
    }
}

TEST_CASE("Text addition bounds - multiple limits tests")
{
    std::string testStringOneLine  = "Test String 1";
    std::string testStringTwoLines = "Test String 1\nTest String 2";

    std::string richTextTwoLines =
        "<text font='gt_pressura' color='12' size='30'>Test </text><text size='20'>String </text><text size='27' "
        "weight='bold'>1</text><br></br><text>Test String 2</text>";

    SECTION("Adding text to lower limit set to signs count and size and lines on higher limit")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();

        text->setTextLimitType(gui::TextLimitType::MaxLines, 2);
        text->setTextLimitType(gui::TextLimitType::MaxSignsCount, testStringOneLine.length());
        text->setTextLimitType(gui::TextLimitType::MaxSize);
        text->setMaximumSize(150, 100);

        text->addText(testStringTwoLines);

        REQUIRE(text->linesSize() == 1);
        REQUIRE(text->getText() == testStringOneLine);
        REQUIRE(text->getText().length() == testStringOneLine.length());
    }

    SECTION("Adding text to lower limit set to lines count and size and signs count on higher limit")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();

        unsigned int signsLimit = 100;

        text->setTextLimitType(gui::TextLimitType::MaxLines, 1);
        text->setTextLimitType(gui::TextLimitType::MaxSignsCount, signsLimit);
        text->setTextLimitType(gui::TextLimitType::MaxSize);
        text->setMaximumSize(150, 100);

        text->addText(testStringOneLine);
        text->addText(testStringOneLine);
        text->addText(testStringOneLine);

        REQUIRE(text->linesSize() == 1);
        REQUIRE(text->getText().length() != signsLimit);
    }

    SECTION("Adding text to lower limit set to size and lines size and signs count on higher limit")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();

        unsigned int signsLimit = 100;

        text->setTextLimitType(gui::TextLimitType::MaxLines, 3);
        text->setTextLimitType(gui::TextLimitType::MaxSignsCount, signsLimit);
        text->setTextLimitType(gui::TextLimitType::MaxSize);
        text->setMaximumSize(140, 30);

        text->addText(testStringOneLine);
        text->addText(testStringOneLine);
        text->addText(testStringOneLine);

        REQUIRE(text->linesSize() == 1);
        REQUIRE(text->getText() == testStringOneLine);
        REQUIRE(text->getText().length() != signsLimit);
    }

    SECTION("Adding RichText to lower limit set to signs count and size and lines on higher limit")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();

        auto format         = text->getTextFormat();
        auto parsedRichText = gui::text::RichTextParser().parse(richTextTwoLines, &format);

        text->setTextLimitType(gui::TextLimitType::MaxLines, 4);
        text->setTextLimitType(gui::TextLimitType::MaxSignsCount, parsedRichText->getText().length());
        text->setTextLimitType(gui::TextLimitType::MaxSize);
        text->setMaximumSize(300, 100);

        text->addRichText(richTextTwoLines);

        REQUIRE(text->linesSize() == 2);
        REQUIRE(text->getText().length() == parsedRichText->getText().length());
    }

    SECTION("Adding RichText to lower limit set to lines count and size and signs count on higher limit")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();

        unsigned int signsLimit = 100;

        text->setTextLimitType(gui::TextLimitType::MaxLines, 1);
        text->setTextLimitType(gui::TextLimitType::MaxSignsCount, signsLimit);
        text->setTextLimitType(gui::TextLimitType::MaxSize);
        text->setMaximumSize(300, 100);

        text->addRichText(richTextTwoLines);

        REQUIRE(text->linesSize() == 1);
        REQUIRE(text->getText().length() != signsLimit);
    }

    SECTION("Adding RichText to lower limit set to size and lines size and signs count on higher limit")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();

        unsigned int signsLimit = 100;

        text->setTextLimitType(gui::TextLimitType::MaxLines, 3);
        text->setTextLimitType(gui::TextLimitType::MaxSignsCount, signsLimit);
        text->setTextLimitType(gui::TextLimitType::MaxSize);
        text->setMaximumSize(140, 30);

        text->addRichText(richTextTwoLines);

        REQUIRE(text->linesSize() == 1);
        REQUIRE(text->getText().length() != signsLimit);
    }
}

TEST_CASE("Text newline navigation and deletion tests")
{
    std::string testStringBlock1 = "Test String 1";
    std::string testStringBlock2 = "Test String 2";
    std::string emptyParagraph   = "<p></p>";

    SECTION("Empty new block at start and delete from text end")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setMaximumSize(600, 200);

        text->addRichText("<text>" + emptyParagraph + testStringBlock1 + "</text>");

        REQUIRE(text->linesSize() == 2);
        REQUIRE((*text->lineGet(0)).getText(0) == "\n");
        REQUIRE((*text->lineGet(1)).getText(0) == testStringBlock1);

        text->removeNCharacters(text->getText().length());

        REQUIRE(text->linesSize() == 0);
        REQUIRE(text->linesGet().empty());
    }

    SECTION("Empty new block at start and delete from text center")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setMaximumSize(600, 200);

        text->addRichText("<text>" + emptyParagraph + testStringBlock1 + emptyParagraph + testStringBlock2 + "</text>");

        REQUIRE(text->linesSize() == 3);
        REQUIRE((*text->lineGet(0)).getText(0) == "\n");
        REQUIRE((*text->lineGet(1)).getText(0) == testStringBlock1 + "\n");
        REQUIRE((*text->lineGet(2)).getText(0) == testStringBlock2);

        text->moveCursor(gui::NavigationDirection::LEFT, testStringBlock2.length());
        text->removeNCharacters(1);

        REQUIRE(text->linesSize() == 2);
        REQUIRE((*text->lineGet(0)).getText(0) == "\n");
        REQUIRE((*text->lineGet(1)).getText(0) == testStringBlock1 + testStringBlock2);

        text->removeNCharacters(testStringBlock1.length());

        REQUIRE(text->linesSize() == 2);
        REQUIRE((*text->lineGet(0)).getText(0) == "\n");
        REQUIRE((*text->lineGet(1)).getText(0) == testStringBlock2);

        text->removeNCharacters(1);

        REQUIRE(text->linesSize() == 1);
        REQUIRE((*text->lineGet(0)).getText(0) == testStringBlock2);

        text->removeNCharacters(testStringBlock2.length());

        REQUIRE(text->linesSize() == 0);
        REQUIRE(text->linesGet().empty());
    }

    SECTION("Empty new block at start and delete from text beginning")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setCursorStartPosition(gui::CursorStartPosition::DocumentBegin);
        text->setMaximumSize(600, 200);

        text->addRichText("<text>" + emptyParagraph + testStringBlock1 + emptyParagraph + testStringBlock2 + "</text>");

        REQUIRE(text->linesSize() == 3);
        REQUIRE((*text->lineGet(0)).getText(0) == "\n");
        REQUIRE((*text->lineGet(1)).getText(0) == testStringBlock1 + "\n");
        REQUIRE((*text->lineGet(2)).getText(0) == testStringBlock2);

        text->removeNCharacters(1);

        REQUIRE(text->linesSize() == 2);
        REQUIRE((*text->lineGet(0)).getText(0) == testStringBlock1 + "\n");
        REQUIRE((*text->lineGet(1)).getText(0) == testStringBlock2);

        text->removeNCharacters(testStringBlock1.length() + 1);

        REQUIRE(text->linesSize() == 1);
        REQUIRE((*text->lineGet(0)).getText(0) == testStringBlock2);

        text->removeNCharacters(testStringBlock2.length());

        REQUIRE(text->linesSize() == 0);
        REQUIRE(text->linesGet().empty());
    }

    SECTION("Empty new block at end and delete from text beginning")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setMaximumSize(600, 200);

        text->addRichText("<text>" + testStringBlock1 + emptyParagraph + "</text>");

        REQUIRE(text->linesSize() == 2);
        REQUIRE((*text->lineGet(0)).getText(0) == testStringBlock1 + "\n");
        REQUIRE((*text->lineGet(1)).getText(0) == "");

        text->moveCursor(gui::NavigationDirection::LEFT, testStringBlock1.length());
        text->removeNCharacters(testStringBlock1.length());

        REQUIRE(text->linesSize() == 2);
        REQUIRE((*text->lineGet(0)).getText(0) == "\n");
        REQUIRE((*text->lineGet(1)).getText(0) == "");

        text->removeNCharacters(1);
        REQUIRE(text->linesSize() == 0);
        REQUIRE(text->linesGet().empty());
    }
}

TEST_CASE("RichText newline and empty lines tests")
{
    std::string testStringBlock1 = "Test String 1";
    std::string testStringBlock2 = "Test String 2";
    std::string testStringBlock3 = "Test String 3";
    std::string emptyParagraph   = "<p></p>";

    SECTION("Paragraph inside no newlines blocks")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setMaximumSize(600, 200);

        text->addRichText("<text>" + testStringBlock1 + "<p>" + testStringBlock2 + "</p>" + testStringBlock3 +
                          "</text>");

        REQUIRE(text->linesSize() == 3);
        REQUIRE((*text->lineGet(0)).getText(0) == testStringBlock1 + "\n");
        REQUIRE((*text->lineGet(1)).getText(0) == testStringBlock2 + "\n");
        REQUIRE((*text->lineGet(2)).getText(0) == testStringBlock3);
    }

    SECTION("Two empty paragraphs at beginning")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setMaximumSize(600, 200);

        text->addRichText("<text>" + emptyParagraph + emptyParagraph + testStringBlock1 + testStringBlock2 +
                          testStringBlock3 + "</text>");

        REQUIRE(text->linesSize() == 3);
        REQUIRE((*text->lineGet(0)).getText(0) == "\n");
        REQUIRE((*text->lineGet(1)).getText(0) == "\n");
        REQUIRE((*text->lineGet(2)).getText(0) == testStringBlock1 + testStringBlock2 + testStringBlock3);
    }

    SECTION("Two empty paragraphs at end")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setMaximumSize(600, 200);

        text->addRichText("<text>" + testStringBlock1 + testStringBlock2 + testStringBlock3 + emptyParagraph +
                          emptyParagraph + "</text>");

        REQUIRE(text->linesSize() == 3);
        REQUIRE((*text->lineGet(0)).getText(0) == testStringBlock1 + testStringBlock2 + testStringBlock3 + "\n");
        REQUIRE((*text->lineGet(1)).getText(0) == "\n");
        REQUIRE((*text->lineGet(2)).getText(0) == "");
    }

    SECTION("One empty paragraphs at beginning, one in center and one at end")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setMaximumSize(600, 200);

        text->addRichText("<text>" + emptyParagraph + testStringBlock1 + emptyParagraph + testStringBlock2 +
                          testStringBlock3 + emptyParagraph + "</text>");

        REQUIRE(text->linesSize() == 4);
        REQUIRE((*text->lineGet(0)).getText(0) == "\n");
        REQUIRE((*text->lineGet(1)).getText(0) == testStringBlock1 + "\n");
        REQUIRE((*text->lineGet(2)).getText(0) == testStringBlock2 + testStringBlock3 + "\n");
        REQUIRE((*text->lineGet(3)).getText(0) == "");
    }

    SECTION("Text inside paragraph at beginning")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setMaximumSize(600, 200);

        text->addRichText("<text><p>" + testStringBlock1 + "</p>" + testStringBlock2 + testStringBlock3 + "</text>");

        REQUIRE(text->linesSize() == 2);
        REQUIRE((*text->lineGet(0)).getText(0) == testStringBlock1 + "\n");
        REQUIRE((*text->lineGet(1)).getText(0) == testStringBlock2 + testStringBlock3);
    }

    SECTION("Text empty paragraph and text inside paragraph at beginning")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setMaximumSize(600, 200);

        text->addRichText("<text>" + emptyParagraph + "<p>" + testStringBlock1 + "</p></text>");

        REQUIRE(text->linesSize() == 3);
        REQUIRE((*text->lineGet(0)).getText(0) == "\n");
        REQUIRE((*text->lineGet(1)).getText(0) == testStringBlock1 + "\n");
        REQUIRE((*text->lineGet(2)).getText(0) == "");
    }
}

TEST_CASE("Navigating down between input texts")
{
    using namespace gui;
    const InputEvent keyDown{{}, InputEvent::State::keyReleasedShort, KeyCode::KEY_DOWN};

    mockup::fontManager();
    SECTION("Empty texts")
    {
        auto layout                 = VBox(nullptr, 0, 0, 100, 200);
        [[maybe_unused]] auto text1 = new TextFixedSize(&layout, 0, 0, 100, 150);
        [[maybe_unused]] auto text2 = new TextFixedSize(&layout, 0, 150, 100, 50);
        layout.setFocus(true);

        REQUIRE(layout.getFocusItemIndex() == 0);

        layout.onInput(keyDown);
        REQUIRE(layout.getFocusItemIndex() == 1);
    }

    SECTION("Non-empty texts and no new line at the end [EGD-7047]")
    {
        constexpr auto testString = "Test String";

        auto layout = VBox(nullptr, 0, 0, 100, 200);
        auto text1  = new TextFixedSize(&layout, 0, 0, 100, 150);
        text1->addText(TextBlock(testString, Font(27).raw(), TextBlock::End::None));
        text1->setCursorStartPosition(CursorStartPosition::DocumentBegin);

        [[maybe_unused]] auto text2 = new TextFixedSize(&layout, 0, 150, 100, 50);
        layout.setFocus(true);

        REQUIRE(layout.getFocusItemIndex() == 0);

        layout.onInput(keyDown);
        REQUIRE(layout.getFocusItemIndex() == 1);
    }
}

TEST_CASE("Text word line breaking tests")
{
    std::string testStringBlock1 = "Test ";
    std::string testStringBlock2 = "String ";
    std::string testStringBlock3 = "LongLongLong";
    std::string testStringBlock4 = testStringBlock1 + testStringBlock3 + testStringBlock2;
    std::string testStringBlock5 = testStringBlock1 + testStringBlock2;
    std::string emptyParagraph   = "<p></p>";

    SECTION("Breaking lines on space on whole words with long words")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setMaximumSize(300, 200);

        text->addRichText("<text>" + testStringBlock4 + testStringBlock5 + "</text>");

        REQUIRE(text->linesSize() == 2);
        REQUIRE((*text->lineGet(0)).getText(0) == testStringBlock4);
        REQUIRE((*text->lineGet(1)).getText(0) == testStringBlock5);
    }

    SECTION("Breaking lines on space on whole words with short word")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setMaximumSize(250, 200);

        text->addRichText("<text>" + testStringBlock5 + testStringBlock5 + testStringBlock5 + "</text>");

        REQUIRE(text->linesSize() == 2);
        REQUIRE((*text->lineGet(0)).getText(0) == testStringBlock5 + testStringBlock5);
        REQUIRE((*text->lineGet(1)).getText(0) == testStringBlock5);
    }

    SECTION("Breaking lines on newline before breaking it on space")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setMaximumSize(250, 200);

        text->addRichText("<text>" + testStringBlock5 + emptyParagraph + testStringBlock5 + "</text>");

        REQUIRE(text->linesSize() == 2);
        REQUIRE((*text->lineGet(0)).getText(0) == testStringBlock5 + "\n");
        REQUIRE((*text->lineGet(1)).getText(0) == testStringBlock5);
    }

    SECTION("Breaking lines by splitting word with dash")
    {
        mockup::fontManager();
        using namespace gui;
        auto text = std::make_unique<gui::TestText>();
        text->setMaximumSize(220, 200);

        text->addRichText("<text>" + testStringBlock4 + "</text>");

        REQUIRE(text->linesSize() == 2);
        REQUIRE((*text->lineGet(0)).getText(0) == testStringBlock1 + testStringBlock3 + "-");
        REQUIRE((*text->lineGet(1)).getText(0) == testStringBlock2);
    }
}
