// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

// Right now there are no asserts here, each test/section / test should have asserts
// these tests check memory sanity only

#include <catch2/catch.hpp>
#include <module-gui/gui/core/Context.hpp>
#include <log.hpp>

TEST_CASE("test context size and position")
{
    auto ctx = new gui::Context(30, 30);
    ctx->fill(0);

    gui::Context *test = ctx->get(17, 17, 30, 30);

    delete test;
    test = ctx->get(-17, -17, 30, 30);
    delete test;

    delete ctx;
}

TEST_CASE("insertContextTest")
{

    gui::Context *dstCtx;
    gui::Context *insCtx;

    SECTION("RECTANGLE INSIDE")
    {
        dstCtx = new gui::Context(30, 30);
        dstCtx->fill(0);
        insCtx = new gui::Context(static_cast<unsigned short>(28), static_cast<unsigned short>(28));
        insCtx->fill(15);
        dstCtx->insert(1, 1, insCtx);
        delete dstCtx;
        delete insCtx;
    }

    SECTION("2 COLUMNS ON RIGHT SIDE, TOP AND BOTTOM ROW UNTOUCHED")
    {
        dstCtx = new gui::Context(static_cast<unsigned short>(30), static_cast<unsigned short>(30));
        dstCtx->fill(0);
        insCtx = new gui::Context(static_cast<unsigned short>(28), static_cast<unsigned short>(28));
        insCtx->fill(15);
        dstCtx->insert(28, 1, insCtx);
        delete dstCtx;
        delete insCtx;
    }

    SECTION("2 COLUMNS ON LEFT SIDE, TOP AND BOTTOM ROW UNTOUCHED")
    {
        dstCtx = new gui::Context(static_cast<unsigned short>(30), static_cast<unsigned short>(30));
        dstCtx->fill(0);
        insCtx = new gui::Context(static_cast<unsigned short>(28), static_cast<unsigned short>(28));
        insCtx->fill(15);
        dstCtx->insert(-26, 1, insCtx);
        delete dstCtx;
        delete insCtx;
    }

    SECTION("2 COLUMNS ON RIGHT SIDE")
    {
        dstCtx = new gui::Context(static_cast<unsigned short>(30), static_cast<unsigned short>(30));
        dstCtx->fill(0);
        insCtx = new gui::Context(static_cast<unsigned short>(32), static_cast<unsigned short>(32));
        insCtx->fill(15);
        dstCtx->insert(28, -1, insCtx);
        delete dstCtx;
        delete insCtx;
    }

    SECTION("2 COLUMNS ON LEFT SIDE")
    {
        dstCtx = new gui::Context(static_cast<unsigned short>(30), static_cast<unsigned short>(30));
        dstCtx->fill(0);
        insCtx = new gui::Context(static_cast<unsigned short>(32), static_cast<unsigned short>(32));
        insCtx->fill(15);
        dstCtx->insert(-30, -1, insCtx);
        delete dstCtx;
        delete insCtx;
    }
}
