// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>
#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include <at/cmd/CLCC.hpp>
#include <at/cmd/CSCA.hpp>
#include <at/cmd/QECCNUM.hpp>

#include "mock/AtCommon_channel.hpp"
#include "PhoneNumber.hpp"
#include "Result.hpp"

namespace at::cmd
{
    struct CLCCStub : public CLCC
    {
        [[nodiscard]] static auto toBool(const std::string &text) -> bool
        {
            return CLCC::toBool(text);
        }
        [[nodiscard]] static auto toUInt(const std::string &text) -> std::uint8_t
        {
            return CLCC::toUInt(text);
        }

        template <typename T>[[nodiscard]] static auto toEnum(const std::string &text) -> std::optional<T>
        {
            return CLCC::toEnum<T>(text);
        }
    };
} // namespace at::cmd

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

TEST_CASE("CLCC parser")
{
    using namespace std::string_literals;
    SECTION("Empty data")
    {
        at::cmd::CLCC cmd;
        at::Result result;
        auto response = cmd.parse(result);
        REQUIRE(!response);
    }
    SECTION("Failing channel")
    {
        at::cmd::CLCC cmd;
        at::FailingChannel channel;
        auto base     = channel.cmd(cmd);
        auto response = cmd.parse(base);
        REQUIRE(!response);
        REQUIRE(response.code == at::Result::Code::ERROR);
    }
    SECTION("Success - one call")
    {
        at::cmd::CLCC cmd;
        at::CLCC_successChannel_oneCall channel;
        auto base     = channel.cmd(cmd);
        auto response = cmd.parse(base);
        REQUIRE(response);
        auto [idx, dir, stateOfCall, mode, multiparty, number, type, alpha, tokens] = response.getData()[0];
        REQUIRE(idx == 1);
        REQUIRE(dir == ModemCall::CallDir::MO);
        REQUIRE(stateOfCall == ModemCall::CallState::Active);
        REQUIRE(mode == ModemCall::CallMode::Voice);
        REQUIRE(multiparty == true);
        REQUIRE(number == utils::PhoneNumber(channel.number).getView());
        REQUIRE(type == "129"s);
        REQUIRE(alpha == "");
    }

    SECTION("Success - two calls")
    {
        at::cmd::CLCC cmd;
        at::CLCC_successChannel_twoCalls channel;
        auto base     = channel.cmd(cmd);
        auto response = cmd.parse(base);
        REQUIRE(response);
        SECTION("First entry")
        {
            auto [idx, dir, stateOfCall, mode, multiparty, number, type, alpha, tokens] = response.getData()[0];
            REQUIRE(idx == 1);
            REQUIRE(dir == ModemCall::CallDir::MO);
            REQUIRE(stateOfCall == ModemCall::CallState::Active);
            REQUIRE(mode == ModemCall::CallMode::Data);
            REQUIRE(multiparty == false);
            REQUIRE(number == utils::PhoneNumber(channel.number1).getView());
            REQUIRE(type == "129"s);
            REQUIRE(alpha == "");
        }

        SECTION("Second entry")
        {
            auto [idx, dir, stateOfCall, mode, multiparty, number, type, alpha, tokens] = response.getData()[1];
            REQUIRE(idx == 2);
            REQUIRE(dir == ModemCall::CallDir::MO);
            REQUIRE(stateOfCall == ModemCall::CallState::Active);
            REQUIRE(mode == ModemCall::CallMode::Voice);
            REQUIRE(multiparty == false);
            REQUIRE(number == utils::PhoneNumber(channel.number2).getView());
            REQUIRE(type == "129"s);
            REQUIRE(alpha == "");
        }
    }

    SECTION("Failed - bad channel result")
    {
        at::cmd::CLCC cmd;
        at::CSCS_badChannel channel;
        auto base = channel.cmd(cmd);
        auto resp = cmd.parse(base);
        REQUIRE(!resp);
        REQUIRE(resp.code == at::Result::Code::ERROR);
    }

    SECTION("Success - only OK data")
    {
        at::cmd::CLCC cmd;
        at::OK_Channel channel;
        auto base     = channel.cmd(cmd);
        auto response = cmd.parse(base);
        REQUIRE(response);
    }

    SECTION("Failed - only OG data")
    {
        at::cmd::CLCC cmd;
        at::OG_Channel channel;
        auto base     = channel.cmd(cmd);
        auto response = cmd.parse(base);
        REQUIRE(response.code == at::Result::Code::ERROR);
    }
}

TEST_CASE("CLCC set data")
{
    using namespace std::literals;
    at::cmd::CLCC cmd;
    SECTION("None modifier set")
    {
        constexpr auto expectedResult = "AT+CLCC"sv;
        REQUIRE(cmd.getCmd() == expectedResult);
    }

    SECTION("Get modifier set")
    {
        constexpr auto expectedResult = "AT+CLCC?"sv;
        cmd.setModifier(at::cmd::Modifier::Get);
        REQUIRE(cmd.getCmd() == expectedResult);
    }
}

TEST_CASE("CLCC conversion methods")
{
    SECTION("String to bool")
    {
        SECTION("Proper data - true")
        {
            REQUIRE(at::cmd::CLCCStub::toBool("1") == true);
        }
        SECTION("Proper data - true")
        {
            REQUIRE(at::cmd::CLCCStub::toBool("0") == false);
        }
        SECTION("Invaild data")
        {
            REQUIRE_THROWS(at::cmd::CLCCStub::toBool("-1") == false);
        }
    }
    SECTION("String to uint32")
    {
        REQUIRE(at::cmd::CLCCStub::toUInt("1") == 1);
        REQUIRE(at::cmd::CLCCStub::toUInt("32") == 32);
        REQUIRE(at::cmd::CLCCStub::toUInt("255") == std::numeric_limits<std::uint8_t>::max());
        REQUIRE_THROWS(at::cmd::CLCCStub::toUInt("256"));
    }
    SECTION("String to enum")
    {
        SECTION("Call dir")
        {
            REQUIRE(at::cmd::CLCCStub::toEnum<ModemCall::CallDir>("-1") == std::nullopt);
            REQUIRE(at::cmd::CLCCStub::toEnum<ModemCall::CallDir>("0").value() == ModemCall::CallDir::MO);
            REQUIRE(at::cmd::CLCCStub::toEnum<ModemCall::CallDir>("1").value() == ModemCall::CallDir::MT);
            REQUIRE(at::cmd::CLCCStub::toEnum<ModemCall::CallDir>("2") == std::nullopt);
        }
        SECTION("Call state")
        {
            REQUIRE(at::cmd::CLCCStub::toEnum<ModemCall::CallState>("-1") == std::nullopt);
            REQUIRE(at::cmd::CLCCStub::toEnum<ModemCall::CallState>("0").value() == ModemCall::CallState::Active);
            REQUIRE(at::cmd::CLCCStub::toEnum<ModemCall::CallState>("5").value() == ModemCall::CallState::Waiting);
            REQUIRE(at::cmd::CLCCStub::toEnum<ModemCall::CallState>("6") == std::nullopt);
        }
        SECTION("Call mode")
        {
            REQUIRE(at::cmd::CLCCStub::toEnum<ModemCall::CallMode>("-1") == std::nullopt);
            REQUIRE(at::cmd::CLCCStub::toEnum<ModemCall::CallMode>("0").value() == ModemCall::CallMode::Voice);
            REQUIRE(at::cmd::CLCCStub::toEnum<ModemCall::CallMode>("2").value() == ModemCall::CallMode::FAX);
            REQUIRE(at::cmd::CLCCStub::toEnum<ModemCall::CallMode>("3") == std::nullopt);
        }
    }
}
