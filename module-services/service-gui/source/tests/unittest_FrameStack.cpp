// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "module-services/service-gui/source/DrawData.hpp"
#include <cstdio>
#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <RTOSWrapper/include/thread.hpp>

#include "../FrameStack.hpp"

using namespace service::renderer;

auto procesor_success = [](sgui::DrawData &data, ::gui::Context &context) -> bool { return true; };
auto procesor_failure = [](sgui::DrawData &data, ::gui::Context &context) -> bool { return false; };

class EmptyDrawData
{
    gui::RefreshModes mode = gui::RefreshModes::GUI_REFRESH_FAST;

  public:
    EmptyDrawData() = default;
    EmptyDrawData(gui::RefreshModes mode) : mode(mode)
    {}

    operator sgui::DrawData()
    {
        return sgui::DrawData(gui::Commands{}, mode);
    }
};

TEST_CASE("DrawFrame")
{

    cpp_freertos::MutexStandard mutex;
    auto frame = DrawFrame(mutex, EmptyDrawData(), {10, 10});
    REQUIRE(frame.getState() == DrawFrame::State::Fresh);

    SECTION("case 1: we process successfully ")
    {
        REQUIRE(frame.processFrame(procesor_success) == true);
        REQUIRE(frame.getState() == DrawFrame::State::Processed);
    }

    SECTION("case 2: we process with fail but process ")
    {
        REQUIRE(frame.processFrame(procesor_failure) == false);
        INFO("we processed frame either way - so it should be processed")
        REQUIRE(frame.getState() == DrawFrame::State::Processed);
    }

    SECTION("case 3: we hit lock guard - ignore frame")
    {
        REQUIRE(false);
    }
}

TEST_CASE("FrameStack - negative flow, nothing to process")
{
    REQUIRE(false);
}

TEST_CASE("FrameStack - positive flow")
{
    auto fs = FrameStack(gui::Size{480, 600});

    REQUIRE(fs.processLastFrame(procesor_success) == false);

    REQUIRE(fs.emplaceDrawData(EmptyDrawData()) == true);

    REQUIRE(fs.processLastFrame(procesor_success) == true);

    service::eink::ImageData data;

    REQUIRE(fs.takeLastProcessedFrame(data) == true);

    REQUIRE(data.context.getH() == 600);

    REQUIRE(data.mode == gui::RefreshModes::GUI_REFRESH_FAST);
}

TEST_CASE("FrameStack - negative flow, frame dropping")
{
    auto fs = FrameStack(gui::Size{480, 600});

    SECTION("case 1: simple frame drop")
    {
        REQUIRE(fs.emplaceDrawData(EmptyDrawData()) == true);
        REQUIRE(fs.emplaceDrawData(EmptyDrawData()) == true);
        REQUIRE(fs.emplaceDrawData(EmptyDrawData()) == true);
        REQUIRE(fs.emplaceDrawData(EmptyDrawData()) == true);
        REQUIRE(fs.processLastFrame(procesor_success));
    }

    SECTION("case 2: frame drop with refresh promotion")
    {
        REQUIRE(fs.emplaceDrawData(EmptyDrawData()) == true);
        REQUIRE(fs.emplaceDrawData(EmptyDrawData(gui::RefreshModes::GUI_REFRESH_DEEP)) == true);
        REQUIRE(fs.emplaceDrawData(EmptyDrawData()) == true);
        REQUIRE(fs.emplaceDrawData(EmptyDrawData()) == true);
        INFO("Deep refresh should be promoted up to last rendered frame");
        REQUIRE(fs.processLastFrame(procesor_success));
        service::eink::ImageData data;
        REQUIRE(fs.takeLastProcessedFrame(data) == true);
        REQUIRE(data.mode == gui::RefreshModes::GUI_REFRESH_DEEP);
    }

    SECTION("case 3: process one frame, drop 2 frames - we should have one frame processed and one ready")
    {
        // TODO some ID to EmpyDrawData? some ID to frames in general?
        REQUIRE(fs.emplaceDrawData(EmptyDrawData()) == true);
        REQUIRE(fs.processLastFrame(procesor_success) == true);
        REQUIRE(fs.emplaceDrawData(EmptyDrawData()) == true);
        REQUIRE(fs.emplaceDrawData(EmptyDrawData()) == true);
        REQUIRE(fs.emplaceDrawData(EmptyDrawData()) == true);

        REQUIRE(false && "There are 2 frames: 1 processed first, one last one ready to process");
        REQUIRE(false && "we can pop one frame ( processed one) ");
        REQUIRE(false && "we can't pop more frames");
        REQUIRE(false && "we can process one frame ");
        REQUIRE(false && "we can't process more frames ");
        REQUIRE(false && "after processing we can pop second frame");
        REQUIRE(false && "after procesing and poping there are no frames left");
    }

    SECTION("case 4: process 2 frames, we should have one (only last one) frame processed")
    {
        REQUIRE(false);
    }

    SECTION("case 4: process 1 frame, add 3 frames, we should have 2 frames (one processed, one ready to process) ")
    {

        REQUIRE(false);
    }

    SECTION("case 4: process 1 frame, add 3 frames process 1 frame : we should have last frame processed")
    {
        REQUIRE(false);
    }
}
