// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "catch.hpp"
#include "TS0710_DLC_ESTABL.h"

TEST_CASE("test-TS0710_DLC_ESTABL")
{
    TS0710_DLC_ESTABL *_class = new TS0710_DLC_ESTABL(0);
    REQUIRE(_class->getResponse() == true);
    delete _class;

    DLC_ESTABL_SystemParameters_t system_parameters;
    _class = new TS0710_DLC_ESTABL(0, system_parameters);
    REQUIRE(_class->getResponse() == true);
    delete _class;
}
