// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

TEST_CASE("sml gui")
{
    SECTION("simple flow")
    {
        REQUIRE("await eink started state from SystemManager");
        REQUIRE("wait for frame to render");
        REQUIRE("render frame");
        REQUIRE("send frame to ui");
    }

    SECTION("full flow")
    {
        REQUIRE("await eink started state from SystemManager");
        REQUIRE("wait for frame to render");
        REQUIRE("new 1 frame added");
        REQUIRE("render 1 frame");
        REQUIRE("new 2 frame added");
        REQUIRE("new 3 frame added, 2 skipped");
        REQUIRE("render 3 frame, 1 skipped");
        REQUIRE("new 4 frame added");
        REQUIRE("send last (3) rendered frame to ui");
        REQUIRE("render 4 frame");
        REQUIRE("send last (4) rendered frame to ui");
    }

    SECTION("power down flow")
    {
        REQUIRE("await eink started state from SystemManager");
        // event: power down
        REQUIRE("await Shutdown frame to render before shutdown");
        REQUIRE("render Shutdown frame");
        REQUIRE("send Shutdown frame to render before shutdown");
        REQUIRE("close service (clean all assets)");
    }

    SECTION("sleep mode")
    {
        REQUIRE("await eink started state from SystemManager");
        // event: power down
        REQUIRE("await Shutdown frame to render before shutdown");
        REQUIRE("render Shutdown frame");
        REQUIRE("send Shutdown frame to render before shutdown");
        REQUIRE("set to sleep (do not clean all assets)");
    }
}

TEST_CASE("sml eink")
{
    SECTION("simple flow")
    {
        REQUIRE("start service (system manager shall know)");
        REQUIRE("start eink device");
        REQUIRE("await eink frame to draw");
        REQUIRE("draw eink frame on screen");
    }

    SECTION("full flow")
    {
        REQUIRE("start service (system manager shall know)");
        REQUIRE("start eink device");
        REQUIRE("await eink frame to draw");
        REQUIRE("we are drawing this frame");
        REQUIRE("get frame while drawing -> last one saved as to render later");
        REQUIRE("get frame while drawing -> last one saved as to render later");
        REQUIRE("get frame while drawing -> last one saved as to render later");
        REQUIRE("get frame while drawing -> last one saved as to render later");
        REQUIRE("get frame while drawing -> last one saved as to render later");
        REQUIRE("get frame while drawing -> last one saved as to render later");
        REQUIRE("draw last eink frame on screen");
    }
}
