// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <FontManager.hpp>
#include <Font.hpp>
#include <Style.hpp>

TEST_CASE("Testo Font")
{
    using namespace gui;
    auto &fm = FontManager::getInstance();
    fm.init("sys/current/assets");

    SECTION("medium font")
    {
        auto f        = Font(27);
        auto raw_font = fm.getFont(style::window::font::medium);
        REQUIRE(fm.getFontByName("gt_pressura_regular_27") == raw_font); /// check that style is ok
        REQUIRE(f.raw() == raw_font);                                    /// check that Font() works ok
        f.setWeight(Font::Weight::Bold);
        REQUIRE(f.raw() == fm.getFont(style::window::font::mediumbold)); // check for changed weight
        f.setSize(30); // we are bold, now set size 30 which is bigbold
        REQUIRE(f.raw() == fm.getFont(style::window::font::bigbold));
    }
}
