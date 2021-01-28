// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>
#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include <at/cmd/CSCA.hpp>
#include <at/cmd/QECCNUM.hpp>

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

TEST_CASE("QECCNUM parser")
{
    SECTION("empty data")
    {
        at::cmd::QECCNUM cmd;
        at::Result base_result;
        auto &result = cmd.parse(base_result);
        REQUIRE(!result);
    }

    SECTION("no numbers")
    {
        at::cmd::QECCNUM cmd;
        at::GenericChannel channel(at::Result::Code::OK, {"+QECCNUM: 1", "+QECCNUM: 2"});
        auto base = channel.cmd(cmd);
        auto resp = cmd.parse(base);
        REQUIRE(!resp);
    }

    SECTION("only no sim numbers")
    {
        at::cmd::QECCNUM cmd;
        at::GenericChannel channel(at::Result::Code::OK, {"+QECCNUM: 0,112,999", "+QECCNUM: 1"});
        auto base = channel.cmd(cmd);
        auto resp = cmd.parse(base);
        REQUIRE(resp);
        REQUIRE(resp.eccNumbersNoSim == std::vector<std::string>({"112", "999"}));
        REQUIRE(resp.eccNumbersSim.empty());
    }

    SECTION("only sim numbers")
    {
        at::cmd::QECCNUM cmd;
        at::GenericChannel channel(at::Result::Code::OK, {"+QECCNUM: 1,112,998"});
        auto base = channel.cmd(cmd);
        auto resp = cmd.parse(base);
        REQUIRE(resp);
        REQUIRE(resp.eccNumbersNoSim.empty());
        REQUIRE(resp.eccNumbersSim == std::vector<std::string>({"112", "998"}));
    }

    SECTION("sim and no sim numbers")
    {
        at::cmd::QECCNUM cmd;
        at::GenericChannel channel(at::Result::Code::OK, {"+QECCNUM: 0,112,999", "+QECCNUM: 1,4564,25435,325454"});
        auto base = channel.cmd(cmd);
        auto resp = cmd.parse(base);
        REQUIRE(resp);
        REQUIRE(resp.eccNumbersNoSim == std::vector<std::string>({"112", "999"}));
        REQUIRE(resp.eccNumbersSim == std::vector<std::string>({"4564", "25435", "325454"}));
    }

    SECTION("add number")
    {
        at::cmd::QECCNUM cmdAddNoSim(
            at::cmd::QECCNUM::Mode::Add, at::cmd::QECCNUM::NumberType::WithoutSim, {"600800900", "200300500"});
        REQUIRE(cmdAddNoSim.getCmd() == "AT+QECCNUM=1,1,\"600800900\",\"200300500\"");
        at::cmd::QECCNUM cmdAddSim(
            at::cmd::QECCNUM::Mode::Add, at::cmd::QECCNUM::NumberType::WithSim, {"600800900", "112"});
        REQUIRE(cmdAddSim.getCmd() == "AT+QECCNUM=1,0,\"600800900\",\"112\"");
    }
}
