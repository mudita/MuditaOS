// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <modem/mux/CellularMuxFrame.h>
#include <modem/mux/CellularMuxTypes.h>
#include <bsp/cellular/bsp_cellular.hpp>

TEST_CASE("TS0170 frame")
{

    SECTION("Create frame from command")
    {
        auto DLCI                   = 2;
        auto cmuxMinimumFrameLength = 6;

        std::string command("AT\r");
        std::vector<uint8_t> commandData(command.begin(), command.end());

        CellularMuxFrame::frame_t tempFrame;
        tempFrame.Address = static_cast<uint8_t>(DLCI << 2);
        tempFrame.Control = static_cast<uint8_t>(TypeOfFrame_e::UIH);
        tempFrame.data    = commandData;

        CellularMuxFrame frame(tempFrame);

        REQUIRE(frame.isComplete(frame.getSerData()) == true);
        REQUIRE(frame.isMyChannel(DLCI) == true);
        REQUIRE(frame.getFrameDLCI(frame.getSerData()) == DLCI);
        REQUIRE(frame.getSerData().size() == cmuxMinimumFrameLength + command.length());
    }

    SECTION("Deserialise frame")
    {
        std::vector<uint8_t> tempFrame{0xf9, 0x09, 0xef, 0x07, 0x41, 0x54, 0x0d, 0x35, 0xf9};
        auto DLCI = 2;

        CellularMuxFrame frame(tempFrame);
        auto deserialisedFrame = frame.getFrame();

        REQUIRE(frame.getFrameDLCI(frame.getSerData()) == DLCI);

        REQUIRE(deserialisedFrame.data.size() == 3);
        REQUIRE(deserialisedFrame.data[0] == 'A');
        REQUIRE(deserialisedFrame.data[1] == 'T');
        REQUIRE(deserialisedFrame.data[2] == '\r');
    }

    SECTION("Incomplete frame")
    {
        std::vector<uint8_t> tempFrame{0xf9, 0x09, 0xef, 0x07, 0x41, 0x54, 0x0d, 0x35};
        auto DLCI = 2;

        CellularMuxFrame frame(tempFrame);

        REQUIRE(frame.getFrameDLCI(frame.getSerData()) == DLCI);
        REQUIRE(frame.isComplete(frame.getSerData()) == false);
    }

    SECTION("Incomplete data")
    {
        std::vector<uint8_t> tempFrame{0xf9, 0x09, 0xef, 0x07, 0x54, 0x0d, 0x35, 0xf9};
        auto DLCI = 2;

        CellularMuxFrame frame(tempFrame);

        REQUIRE(frame.getFrameDLCI(frame.getSerData()) == DLCI);
        REQUIRE(frame.isComplete(frame.getSerData()) == false);
    }

    SECTION("Invalid length")
    {
        std::vector<uint8_t> tempFrame{0xf9, 0x09, 0xef, 0x09, 0x41, 0x54, 0x0d, 0x35, 0xf9};
        auto DLCI = 2;

        CellularMuxFrame frame(tempFrame);

        REQUIRE(frame.getFrameDLCI(frame.getSerData()) == DLCI);
        REQUIRE(frame.isComplete(frame.getSerData()) == false);
    }
}
