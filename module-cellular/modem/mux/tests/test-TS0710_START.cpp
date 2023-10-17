// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "catch.hpp"
#include "TS0710_START.h"
TEST_CASE("test-TS0710_START")
{
    TS0710_START::START_SystemParameters_t system_parameters;
    TS0710_START *_class = new TS0710_START(TS0710_START::Mode_e::Basic, system_parameters);

    // wait for confirmation
    REQUIRE(_class->ConnectionStatus() == true);
    REQUIRE(_class->getCtrlChannel()->getName() == "Control");

    delete _class;
}
