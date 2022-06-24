// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <Label.hpp>
#include <mock/TestWindow.hpp>

TEST_CASE("Test BoundingBox intersect")
{
    auto lol = new gui::Item();
    auto val = lol;
    REQUIRE(val->type == gui::ItemType::LABEL);
    delete lol;
}
