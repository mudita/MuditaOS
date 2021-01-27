// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>
#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <at/cmd/CSCA.hpp>
#include "mock/AtCommon_channel.hpp"
#include "PhoneNumber.hpp"
#include "Result.hpp"

TEST_CASE("CSCA parser test")
{
    SECTION("empty failed data")
    {
        at::cmd::CSCA cmd;
        at::Result base_result; // normally returned from cmux->exec(), TODO getter for dumb result ala exe
        auto &result = cmd.parse(base_result);
        REQUIRE(!result);
        REQUIRE(result.smsCenterAddress == "");
        REQUIRE(result.smsTypeOfAddress == "");
    }

    SECTION("failing channel")
    {
        at::cmd::CSCA cmd;
        at::CSCS_badChannel channel;
        auto base = channel.cmd(cmd);
        auto resp = cmd.parse(base);
        REQUIRE(!resp);
        REQUIRE(resp.code == at::Result::Code::ERROR);
    }

    SECTION("bad data")
    {
        at::cmd::CSCA cmd(at::cmd::Modifier::Get);
        at::CSCA_emptyData channel;
        auto base = channel.cmd(cmd);
        auto resp = cmd.parse(base);
        REQUIRE(!resp);
        REQUIRE(resp.code == at::Result::Code::PARSING_ERROR);
    }

    SECTION("proper data")
    {
        at::cmd::CSCA cmd;
        at::CSCS_successChannel channel;
        auto base = channel.cmd(cmd);
        auto resp = cmd.parse(base);
        REQUIRE(resp);
        REQUIRE(resp.smsCenterAddress == channel.smsCenterAddress);
        REQUIRE(resp.smsTypeOfAddress == channel.smsTypeOfAddress);
    }

    SECTION("failing data")
    {}
}

TEST_CASE("CSCA set data")
{
    SECTION("set proper data")
    {
        const std::string play_sms_center = "+48 790 998 250";
        const int example_type_of_address = 145;
        utils::PhoneNumber nr(play_sms_center);

        const std::string expected_result =
            "AT+CSCA=\"" + nr.getView().getE164() + "\"," + std::to_string(example_type_of_address);

        at::cmd::CSCA cmd;
        cmd.set(nr.getView(), example_type_of_address);
        SECTION("We fail - `NONE` modifier set")
        {
            REQUIRE(cmd.getCmd() != expected_result);
        }

        cmd.setModifier(at::cmd::Modifier::Set);

        SECTION("Success: proper modifier and data set")
        {
            REQUIRE(cmd.getCmd() == expected_result);
        }
    }
}
