// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "catch.hpp"
#include "DLC_channel.h"
#include "TS0710_Frame.h"

TEST_CASE("test-DLC_channel")
{
    DLCChannel *_class = new DLC_channel();
    REQUIRE(_class->getName() == "none");
    delete _class;

    _class = new DLCChannel(2, "Test2");
    REQUIRE(_class->getName() == "Test2");
    REQUIRE(_class->getDLCI() == 2);
    std::vector<uint8_t> v;
    v.push_back(0xAA);
    v.push_back(0x00);
    v.push_back(0xAA);
    _class->SendData(v);
    delete _class;
}

TEST_CASE("test-Frame")
{
    TS0710_Frame::frame_t frame;
    frame.Address = static_cast<uint8_t>(2 << 2) | (1 << 1); // set C/R = 1 - command
    frame.Control = static_cast<uint8_t>(TypeOfFrame_e::UIH);
    frame.data.push_back(0x00);
    frame.data.push_back(0x01);
    frame.data.push_back(0x02);
    TS0710_Frame *_class = new TS0710_Frame(frame);

    // check for not extended address
    REQUIRE(_class->isComplete() == true);

    std::vector<uint8_t> v = _class->getSerData();
    delete _class;

    _class = new TS0710_Frame(v);
    REQUIRE(_class->isComplete() == true);
    delete _class;

    v.pop_back();
    _class = new TS0710_Frame(v);
    REQUIRE(_class->isComplete() == false);
    delete _class;

    // check for extended address
    v.clear();
    frame.data.clear();
    for (int i = 0; i < 256; i++)
        frame.data.push_back(0xAA);
    _class = new TS0710_Frame(frame);

    REQUIRE(_class->isComplete() == true);
    v = _class->getSerData();
    delete _class;

    _class = new TS0710_Frame(v);
    REQUIRE(_class->isComplete() == true);
    delete _class;

    v.pop_back();
    _class = new TS0710_Frame(v);
    REQUIRE(_class->isComplete() == false);
    delete _class;
}
