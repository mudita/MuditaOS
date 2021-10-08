// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "catch.hpp"
#include "TS0710_DATA.h"
#include "TS0710_Frame.h"

#if defined(__cplusplus)
extern "C"
{
#endif
#include "uart.h"
#if defined(__cplusplus)
}
#endif

#define TEST_DATA_SIZE 220

TEST_CASE("test-TS0710_DATA")
{
    DLC_ESTABL_SystemParameters_t system_parameters;
    system_parameters.TypeOfFrame             = TypeOfFrame_e::UIH;
    system_parameters.ConvergenceLayer        = 1;
    system_parameters.Priority                = 1;
    system_parameters.AckTime                 = 100; // 100ms default
    system_parameters.MaxFrameSize            = 128;
    system_parameters.MaxNumOfRetransmissions = 3; // default 3
    system_parameters.ErrRecovWindowSize      = 2; // default 2

    std::vector<uint8_t> User_data;
    for (int i = 0; i < TEST_DATA_SIZE; i++)
        User_data.push_back(0xAA);

    TS0710_DATA *_class = new TS0710_DATA(2, system_parameters, User_data);
    uint8_t data[256];
    // uint32_t len = UartReceive(data);
    uint32_t len = cellular->Read(static_cast<void *>(data), sizeof(data));
    // check length of last received frame - should be reminder of (TEST_DATA_SIZE / system_parameters.MaxFrameSize)
    REQUIRE((data[3] >> 1) == (TEST_DATA_SIZE % system_parameters.MaxFrameSize));

    delete _class;
}
