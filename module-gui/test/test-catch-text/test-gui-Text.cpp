#include "BoundingBox.hpp"
#include "BoxLayout.hpp"
#include "Common.hpp"
#include "InitializedFontManager.hpp"
#include "TextParse.hpp"
#include "i18/i18.hpp"
#include "mock/InitializedFontManager.hpp"
#include <catch2/catch.hpp>
#include <limits>

#include <module-gui/gui/widgets/Text.hpp>
#include <mock/multi-line-string.hpp>
#include <algorithm>
#include <mock/BlockFactory.hpp>
#include <RawFont.hpp>

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
        auto text     = Text(nullptr, 0, 0, 0, 0, testtext);
        REQUIRE(text.getText() == testtext);
    }
}

namespace gui
{
    class TestText : public Text
    {
      public:
        unsigned int linesSize()
        {
            return lines.get().size();
        }

        void drawLines() override
        {
            Text::drawLines();
            return;
        }

        auto &linesGet()
        {
            return lines.get();
        }

        [[nodiscard]] auto *getInputMode()
        {
            return mode;
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

    SECTION("all multiline visible")
    {
        unsigned int lines_count = 5;
        auto testline            = mockup::multiLineString(lines_count);
        auto text                = TestText();
        text.setSize(300, 500);
        text.setText(std::make_unique<TextDocument>(
            textToTextBlocks(testline, fontmanager.getFont(0), TextBlock::End::Newline)));

        text.drawLines();
        REQUIRE(text.linesSize() == lines_count);
    }

    // Test deactivated
    //    SECTION("get 2 lines out of N")
    //    {
    //        unsigned int lines_count         = 5;
    //        const unsigned int lines_to_show = 2;
    //        REQUIRE(lines_to_show < lines_count);
    //        auto testline    = mockup::multiLineString(lines_count);
    //        auto font        = fontmanager.getFont(0);
    //        auto line_height = font->info.line_height;
    //        auto text        = TestText();
    //        text.setMaximumSize(6, line_height * lines_to_show);
    //        text.setText(std::make_unique<TextDocument>(
    //            textToTextBlocks(testline, fontmanager.getFont(0), TextBlock::End::Newline)));
    //
    //        text.drawLines();
    //        REQUIRE(text.linesSize() == lines_to_show);
    //    }
}

TEST_CASE("Text buildDrawList")
{
    using namespace gui;

    auto &fontmanager = mockup::fontManager();
    auto lines_count  = 10;
    auto testline     = mockup::multiLineString(lines_count);
    auto font         = fontmanager.getFont(0);
    auto text         = TestText();
    text.setSize(3000, 3000);
    text.setText(std::make_unique<TextDocument>(textToTextBlocks(testline, font, TextBlock::End::Newline)));

    SECTION("check every element in every line if it's in parent and has non zero sizes")
    {
        REQUIRE(text.linesGet().size() > 0);
        for (auto &line : text.linesGet()) {
            REQUIRE(line.length() > 0);
        }
    }
}

TEST_CASE("handle input mode ABC/abc/1234")
{
    utils::localize.Switch(utils::Lang::En); /// needed to load input mode

    auto text      = gui::TestText();
    auto modes     = {InputMode::ABC, InputMode::abc, InputMode::digit};
    auto str       = text.getText();
    auto next_mode = gui::InputEvent({}, gui::InputEvent::State::keyReleasedShort, gui::KeyCode::KEY_AST);
    auto key_2     = gui::InputEvent(
        {
            RawKey::State::Released,
            bsp::KeyCodes::NumericKey2,
        },
        gui::InputEvent::State::keyReleasedShort);
    text.setInputMode(new InputMode(modes));

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
        auto time_long_enough_to_not_be_multipress = 1000;
        text.onInput(next_mode);
        key_2.key.time_release += time_long_enough_to_not_be_multipress;
        REQUIRE(text.getInputMode()->is(InputMode::abc));
        text.onInput(key_2);
        key_2.key.time_release += time_long_enough_to_not_be_multipress;
        str += "a";
        REQUIRE(str == text.getText());

        text.onInput(key_2);
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

TEST_CASE("handle text expand")
{
    mockup::fontManager();
    using namespace gui;
    Length w         = 10;
    Length h         = 100;
    BoxLayout layout = BoxLayout(nullptr, 0, 0, w, h);
    auto text        = new gui::TestText();
    layout.addWidget(text);
    text->setMaximumSize(w, h);
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
    text.addText(gui::TextBlock(test_text, mockup::fontManager().getFont(0), gui::TextBlock::End::None));
    REQUIRE(text.getText() == test_text);
    test_text = test_text + newline;
    text.addText(newline);
    REQUIRE(text.getText() == test_text);
}
