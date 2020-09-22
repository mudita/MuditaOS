#include "InitializedFontManager.hpp"
#include <catch2/catch.hpp>
#include <limits>
#include <memory>
#include <sstream>
#include <limits>
#include <module-gui/gui/widgets/TextLine.hpp>

#include <mock/buildTextDocument.hpp>
#include <mock/multi-line-string.hpp>
#include <mock/BlockFactory.hpp>

const auto maxwidth = std::numeric_limits<unsigned int>().max();

TEST_CASE("TextLine - ctor")
{
    using namespace gui;

    SECTION("no document")
    {
        auto line = gui::TextLine(nullptr, 0, maxwidth);
        REQUIRE(line.length() == 0);
    }

    SECTION("first line of multiline - all fits in")
    {
        auto texts            = mockup::lineStrings(3);
        auto [document, font] = mockup::buildMultilineTestDocument(texts);
        auto line             = gui::TextLine(&document, 0, std::numeric_limits<unsigned int>().max());

        REQUIRE(line.length() > 0);
        REQUIRE(line.length() == texts.front().length());
    }

    SECTION("oneline line - all fits in")
    {
        auto text             = mockup::multiWordString(5);
        auto [document, font] = mockup::buildOnelineTestDocument(text);
        auto line             = gui::TextLine(&document, 0, std::numeric_limits<unsigned int>().max());

        REQUIRE(line.length() > 0);
        REQUIRE(line.length() == text.length());
    }
}

TEST_CASE("TextLine - non fitting text")
{
    SECTION("text > one line to show")
    {
        auto text             = mockup::multiWordString(5);
        auto [document, font] = mockup::buildOnelineTestDocument(text);
        auto line             = gui::TextLine(&document, 0, std::numeric_limits<unsigned int>::max());

        REQUIRE(line.length() != 0);
        REQUIRE(line.length() <= text.length());
    }
}

TEST_CASE("TextLine - multiple styles text")
{
    using namespace gui;
    mockup::fontManager();

    auto getTextLen = [](auto blocks) -> unsigned int {
        unsigned int ret = 0;
        for (auto el : blocks) {
            ret += el.length();
        }
        return ret;
    };

    SECTION("multi - style text, one paragraph")
    {
        auto testblock = mockup::getBlock(mockup::BlockFactory::Type::Block0);
        auto document  = TextDocument(testblock);
        auto line      = gui::TextLine(&document, 0, maxwidth);
        REQUIRE(line.width() > 0);
        REQUIRE(line.length() == getTextLen(testblock));
    }

    SECTION("multi - style text, two paragraphs, first paragraph")
    {
        auto testblock = mockup::getBlock(mockup::BlockFactory::Type::Block0);
        auto block1    = mockup::getBlock(mockup::BlockFactory::Type::Block1);
        auto block0    = mockup::getBlock(mockup::BlockFactory::Type::Block0);

        testblock.insert(testblock.end(), block1.begin(), block1.end());
        auto document = TextDocument(testblock);
        auto line     = gui::TextLine(&document, 0, maxwidth);
        REQUIRE(line.length() == getTextLen(block0));
    }

    SECTION("multi - style text, two paragraphs, second paragraph")
    {

        auto testblock = mockup::getBlock(mockup::BlockFactory::Type::Block0);
        auto block1    = mockup::getBlock(mockup::BlockFactory::Type::Block1);
        auto block0    = mockup::getBlock(mockup::BlockFactory::Type::Block0);

        testblock.insert(testblock.end(), block1.begin(), block1.end());
        auto document = TextDocument(testblock);
        auto line     = gui::TextLine(&document, getTextLen(block0), maxwidth);
        REQUIRE(line.length() == getTextLen(block1));
    }

    SECTION("multi - empty text, end newline")
    {
        auto testblock = mockup::getBlock(mockup::BlockFactory::Type::NoneBlock0);
        auto document  = TextDocument(testblock);
        auto line      = gui::TextLine(&document, 0, maxwidth);
        REQUIRE(line.length() == 0);
        REQUIRE(line.width() == 0);
    }

    SECTION("multi - empty text, end none")
    {
        auto testblock = mockup::getBlock(mockup::BlockFactory::Type::NoneBlock1);
        auto document  = TextDocument(testblock);
        auto line      = gui::TextLine(&document, 0, maxwidth);
        REQUIRE(line.length() == 0);
        REQUIRE(line.width() == 0);
    }
}

TEST_CASE("TextLine - elements sizes checkup")
{
    using namespace gui;

    auto testblock = mockup::getBlock(mockup::BlockFactory::Type::Block0);
    auto document  = TextDocument(testblock);
    auto text_line = gui::TextLine(&document, 0, maxwidth);

    REQUIRE(text_line.length() > 0);
    const Item *it = nullptr;
    for (unsigned int cnt = 0; cnt < text_line.count(); ++cnt) {
        it = text_line.getElement(cnt);
        REQUIRE(it != nullptr);
        REQUIRE(it->getWidth() > 0);
        REQUIRE(it->getHeight() > 0);
    }
}
