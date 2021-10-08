// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <module-gui/gui/core/BoundingBox.hpp>
#include <module-gui/gui/widgets/Item.hpp>
#include <module-gui/gui/core/Axes.hpp>

namespace gui
{
    class TestItem : public gui::Item
    {};
} // namespace gui

TEST_CASE("Set size tests")
{
    auto item = gui::TestItem();

    SECTION("default sizes")
    {
        REQUIRE(item.area().w == gui::BoundingBox::zero_size);
        REQUIRE(item.area().h == gui::BoundingBox::zero_size);
        REQUIRE(item.area().x == gui::BoundingBox::zero_position);
        REQUIRE(item.area().x == gui::BoundingBox::zero_position);

        REQUIRE(item.area(gui::Item::Area::Min).w == gui::BoundingBox::zero_size);
        REQUIRE(item.area(gui::Item::Area::Min).h == gui::BoundingBox::zero_size);
        REQUIRE(item.area(gui::Item::Area::Min).x == gui::BoundingBox::zero_position);
        REQUIRE(item.area(gui::Item::Area::Min).x == gui::BoundingBox::zero_position);

        REQUIRE(item.area(gui::Item::Area::Max).w == gui::BoundingBox::zero_size);
        REQUIRE(item.area(gui::Item::Area::Max).h == gui::BoundingBox::zero_size);
        REQUIRE(item.area(gui::Item::Area::Max).x == gui::BoundingBox::zero_position);
        REQUIRE(item.area(gui::Item::Area::Max).x == gui::BoundingBox::zero_position);
    }

    // minimum - affects only minimal size, no callbacks
    SECTION("set minimum size")
    {
        item.setMinimumSize(gui::BoundingBox::min_size, gui::Axis::X);

        // check if minimum is minimum
        REQUIRE(item.area(gui::Item::Area::Min).w == gui::BoundingBox::min_size);
        REQUIRE(item.area(gui::Item::Area::Min).w == gui::BoundingBox::zero_size);

        // check if minimal element size can be maximal
        item.setMinimumSize(gui::BoundingBox::max_size, gui::Axis::X);
        REQUIRE(item.area(gui::Item::Area::Min).w == gui::BoundingBox::max_size);

        item.setMinimumSize(gui::BoundingBox::zero_size, gui::Axis::X);
    }

    // same as above
    // maximum - affects only minimal size, no callbacks
    SECTION("set maximim size")
    {
        REQUIRE(item.area(gui::Item::Area::Max).w == gui::BoundingBox::zero_size);

        item.setMaximumSize(gui::BoundingBox::max_size, gui::Axis::X);
        REQUIRE(item.area(gui::Item::Area::Max).w == gui::BoundingBox::max_size);

        // reset item max size to minimum - shall be the same as zero_size
        // size cant be smaller than zero
        item.setMaximumSize(gui::BoundingBox::min_size, gui::Axis::X);
        REQUIRE(item.area(gui::Item::Area::Max).w == gui::BoundingBox::zero_size);
    }

    SECTION("set position")
    {
        item.setPosition(gui::BoundingBox::max_position, gui::Axis::X);
        REQUIRE(item.area().x == gui::BoundingBox::max_position);
        REQUIRE(item.area().x > gui::BoundingBox::min_position);

        item.setPosition(gui::BoundingBox::min_position, gui::Axis::X);
        REQUIRE(item.area().x == gui::BoundingBox::min_position);
        REQUIRE(item.area().x < gui::BoundingBox::max_position);

        item.setPosition(gui::BoundingBox::zero_position, gui::Axis::X);
        REQUIRE(item.area().x == gui::BoundingBox::zero_position);
        REQUIRE(item.area().x > gui::BoundingBox::min_position);
        REQUIRE(item.area().x < gui::BoundingBox::max_position);
    }
}
