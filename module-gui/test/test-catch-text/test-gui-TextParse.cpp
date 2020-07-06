#include <catch2/catch.hpp>
#include <limits>
#include <module-gui/gui/widgets/TextParse.hpp>
#include <sstream>
#include <mock/multi-line-string.hpp>

TEST_CASE("TextParse")
{
    using namespace gui;

    SECTION("empty")
    {
        auto res = textToTextBlocks("", nullptr);
        REQUIRE(res.size() == 0);
    }

    SECTION("oneline")
    {
        auto res = textToTextBlocks("this is text", nullptr);
        REQUIRE(res.size() == 1);
    }

    SECTION("multi-line")
    {
        const auto no_lines = 3;
        auto text           = mockup::multiLineString(no_lines);
        auto res            = textToTextBlocks(text, nullptr);
        REQUIRE(res.size() == no_lines);
    }
}
