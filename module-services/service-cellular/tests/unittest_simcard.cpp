// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <service-cellular/src/SimCard.hpp>

#include <at/Cmd.hpp>
#include <at/Result.hpp>
#include <at/ATFactory.hpp>

#include <module-cellular/test/mock/AtCommon_channel.hpp>

namespace at
{
    class QPINC_Channel : public ChannelMock
    {
      public:
        Result::Code code;
        at::EquipmentErrorCode err;
        const std::string pinAttempts;
        const std::string pukAttempts;

        QPINC_Channel(Result::Code code          = Result::Code::OK,
                      at::EquipmentErrorCode err = at::EquipmentErrorCode::NoInformation,
                      int pinAttempts            = 2,
                      int pukAttempts            = 1)
            : code(code), err(err), pinAttempts(std::to_string(pinAttempts)), pukAttempts(std::to_string(pukAttempts))
        {}

        auto cmd(const at::Cmd &at) -> Result override
        {
            const auto &cmd = at.getCmd();
            if (cmd.rfind("AT+QPINC=", 0) == 0)
                return Result(Result::Code::OK, {"+QPINC:0," + pinAttempts + "," + pukAttempts, "OK"});
            Result result(code, {});
            result.errorCode = err;
            return result;
        }
    };
} // namespace at

TEST_CASE("SimCard functionality test")
{
    SECTION("simCodeToString()")
    {
        cellular::api::SimCode v{1, 2, 3, 4};
        cellular::api::SimCode empty;
        REQUIRE(cellular::internal::simCodeToString(v) == "1234");
        REQUIRE(cellular::internal::simCodeToString(empty).empty());
    }

    SECTION("Check Channel mockup sanity")
    {
        auto channel = at::QPINC_Channel();

        auto resp = channel.cmd(at::factory(at::AT::QPINC));
        REQUIRE(resp.code == at::Result::Code::OK);

        at::response::qpinc::AttemptsCounters ret;
        REQUIRE(at::response::parseQPINC(resp, ret));

        REQUIRE(resp.code == at::Result::Code::OK);
        REQUIRE(ret.PinCounter == 2);
        REQUIRE(ret.PukCounter == 1);
    }

    SECTION("Unlock with PIN - sucessful")
    {
        cellular::service::SimCard simCard;

        auto mockChannel = at::QPINC_Channel();
        simCard.setChannel(&mockChannel);

        bool event             = false;
        simCard.onUnhandledCME = [&event](unsigned int code) { event = true; };

        auto result = simCard.handlePinUnlock({});
        REQUIRE(result);
        REQUIRE(!event);
    }

    SECTION("Unlock with PIN - incorrect password")
    {
        cellular::service::SimCard simCard;

        auto mockChannel = at::QPINC_Channel(at::Result::Code::ERROR, at::EquipmentErrorCode::IncorrectPassword);
        simCard.setChannel(&mockChannel);

        bool event             = false;
        simCard.onUnhandledCME = [&event](unsigned int code) { event = true; };

        auto result = simCard.handlePinUnlock({});
        REQUIRE(!result);
        REQUIRE(!event);
    }

    SECTION("Unlock with PIN - unhandled CME")
    {
        cellular::service::SimCard simCard;

        auto mockChannel = at::QPINC_Channel(at::Result::Code::ERROR);
        simCard.setChannel(&mockChannel);

        bool event             = false;
        simCard.onUnhandledCME = [&event](unsigned int code) { event = true; };

        auto result = simCard.handlePinUnlock({});
        REQUIRE(!result);
        REQUIRE(event);
    }
}
