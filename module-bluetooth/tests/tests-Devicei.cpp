// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include "Device.hpp"
#include <btstack_util.h>

Devicei genDev()
{
    Devicei from("from");
    bd_addr_t addr{0, 1, 2, 3, 4, 5};
    from.setAddress(&addr);
    from.pageScanRepetitionMode = 1;
    from.clockOffset            = 1;
    from.classOfDevice          = 1;
    from.state                  = DEVICE_STATE::REMOTE_NAME_REQUEST;
    from.deviceState            = DeviceState::Paired;
    from.isPairingSSP           = true;
    return from;
}

TEST_CASE("Devicei - create copy and move")
{
    Devicei from = genDev();
    Devicei to;

    SECTION("create - by copy")
    {
        SECTION("ctor")
        {
            to = Devicei(from);
        }
        SECTION("operator")
        {
            to.operator=(from);
        }
    }

    SECTION("move")
    {
        // please see that that section depends on previous section working fine
        Devicei base = from;
        SECTION("ctor")
        {
            to = std::move(base);
        }
        SECTION("operator")
        {
            to.operator=(std::move(from));
        }
    }

    REQUIRE(from == to);
    REQUIRE(!(from != to));
    REQUIRE(bd_addr_cmp(from.address, to.address) == 0);
    REQUIRE(from.pageScanRepetitionMode == to.pageScanRepetitionMode);
    REQUIRE(from.clockOffset == to.clockOffset);
    REQUIRE(from.classOfDevice == to.classOfDevice);
    REQUIRE(from.state == to.state);
    REQUIRE(from.deviceState == to.deviceState);
    REQUIRE(from.isPairingSSP == to.isPairingSSP);
}
