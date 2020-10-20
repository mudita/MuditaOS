// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "catch.hpp"
#include "TS0710_DLC_RELEASE.h"
TEST_CASE("test-TS0710_DLC_RELEASE")
{
    TS0710_DLC_RELEASE *_class = new TS0710_DLC_RELEASE(0);

    delete _class;
}
