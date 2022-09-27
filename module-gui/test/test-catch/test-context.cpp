// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

// Right now there are no asserts here, each test/section / test should have asserts
// these tests check memory sanity only

#include <catch2/catch.hpp>
#include <module-gui/gui/core/Context.hpp>
#include <log/log.hpp>
#include "ImageManager.hpp"
#include "DrawCommand.hpp"
#include <sstream>
#include <memory>

TEST_CASE("test context size and position")
{
    auto ctx = gui::Context(30, 30);
    ctx.fill(0);

    gui::Context test = ctx.get(17, 17, 30, 30);

    test = ctx.get(-17, -17, 30, 30);
}

TEST_CASE("insertContextTest")
{

    gui::Context dstCtx;
    gui::Context insCtx;

    SECTION("RECTANGLE INSIDE")
    {
        dstCtx = gui::Context(30, 30);
        dstCtx.fill(0);
        insCtx = gui::Context(28, 28);
        insCtx.fill(15);
        dstCtx.insert(1, 1, insCtx);
    }

    SECTION("2 COLUMNS ON RIGHT SIDE, TOP AND BOTTOM ROW UNTOUCHED")
    {
        dstCtx = gui::Context(30, 30);
        dstCtx.fill(0);
        insCtx = gui::Context(28, 28);
        insCtx.fill(15);
        dstCtx.insert(28, 1, insCtx);
    }

    SECTION("2 COLUMNS ON LEFT SIDE, TOP AND BOTTOM ROW UNTOUCHED")
    {
        dstCtx = gui::Context(30, 30);
        dstCtx.fill(0);
        insCtx = gui::Context(28, 28);
        insCtx.fill(15);
        dstCtx.insert(-26, 1, insCtx);
    }

    SECTION("2 COLUMNS ON RIGHT SIDE")
    {
        dstCtx = gui::Context(30, 30);
        dstCtx.fill(0);
        insCtx = gui::Context(32, 32);
        insCtx.fill(15);
        dstCtx.insert(28, -1, insCtx);
    }

    SECTION("2 COLUMNS ON LEFT SIDE")
    {
        dstCtx = gui::Context(30, 30);
        dstCtx.fill(0);
        insCtx = gui::Context(32, 32);
        insCtx.fill(15);
        dstCtx.insert(-30, -1, insCtx);
    }
}

TEST_CASE("Draw vector image test")
{

    constexpr std::string_view properContextDump = "ffffffffffffffffffffffffffffffff\n"
                                                   "ffffffffffffffffffffffffffffffff\n"
                                                   "ffffffffffffffffffffffffffffffff\n"
                                                   "ffffffffffffffffffffffffffffffff\n"
                                                   "fffffffffffffff00fffffffffffffff\n"
                                                   "ffffffffffffff0000ffffffffffffff\n"
                                                   "ffffffffffffff0000ffffffffffffff\n"
                                                   "ffffffffffffff0000ffffffffffffff\n"
                                                   "ffffffffffffff0000ffffffffffffff\n"
                                                   "ffffffffffffff0000ffffffffffffff\n"
                                                   "ffffffffffffff0000ffffffffffffff\n"
                                                   "ffffffffffffff0000ffffffffffffff\n"
                                                   "ffffffffffffff0000ffffffffffffff\n"
                                                   "ffffffffffffff0000ffffffffffffff\n"
                                                   "fffff0000000000000000000000fffff\n"
                                                   "ffff000000000000000000000000ffff\n"
                                                   "ffff000000000000000000000000ffff\n"
                                                   "fffff0000000000000000000000fffff\n"
                                                   "ffffffffffffff0000ffffffffffffff\n"
                                                   "ffffffffffffff0000ffffffffffffff\n"
                                                   "ffffffffffffff0000ffffffffffffff\n"
                                                   "ffffffffffffff0000ffffffffffffff\n"
                                                   "ffffffffffffff0000ffffffffffffff\n"
                                                   "ffffffffffffff0000ffffffffffffff\n"
                                                   "ffffffffffffff0000ffffffffffffff\n"
                                                   "ffffffffffffff0000ffffffffffffff\n"
                                                   "ffffffffffffff0000ffffffffffffff\n"
                                                   "fffffffffffffff00fffffffffffffff\n"
                                                   "ffffffffffffffffffffffffffffffff\n"
                                                   "ffffffffffffffffffffffffffffffff\n"
                                                   "ffffffffffffffffffffffffffffffff\n"
                                                   "ffffffffffffffffffffffffffffffff\n";

    auto context = gui::Context(32, 32);

    gui::ImageManager::getInstance().init(".");
    auto id                 = gui::ImageManager::getInstance().getImageMapID("plus_32px_W_M");
    gui::ImageMap *imageMap = gui::ImageManager::getInstance().getImageMap(id);
    CAPTURE(id);

    REQUIRE(imageMap != nullptr);
    REQUIRE(imageMap->getType() == gui::ImageMap::Type::VECMAP);

    auto drawCommand    = gui::DrawImage();
    drawCommand.imageID = id;
    drawCommand.areaH   = 32;
    drawCommand.areaW   = 32;
    drawCommand.draw(&context);
    std::string dump;

    std::uint32_t offset = 0;
    for (std::uint32_t y = 0; y < context.getH(); y++) {
        for (std::uint32_t x = 0; x < context.getW(); x++) {
            std::uint32_t value = *(context.getData() + offset);
            std::stringstream stream;
            stream << std::hex << value;
            dump.append(stream.str());
            offset++;
        }
        dump.append("\n");
    }
    CAPTURE(dump);
    REQUIRE(dump.length() == properContextDump.length());
    REQUIRE(dump == properContextDump);
}
