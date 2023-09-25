// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include <module-services/service-cellular/src/volte/VolteCapabilityHandler.hpp>
#include <module-services/service-cellular/src/volte/VolteAllowedList.hpp>
#include <module-services/service-cellular/src/volte/VolteCapabiltyHandlerCellularInterface.hpp>
#include <module-cellular/modem/BaseChannel.hpp>

TEST_CASE("VoLTE Capability handler")
{
    struct BaseChannelStub final : at::BaseChannel
    {
        virtual ~BaseChannelStub() = default;

        virtual auto cmd(const std::string &, std::chrono::milliseconds = at::default_timeout, size_t = 0) -> at::Result
        {
            return at::Result{};
        }
        virtual auto cmd(const at::AT &) -> at::Result
        {
            return at::Result{};
        }
        virtual auto cmd(const at::Cmd &) -> at::Result
        {
            return at::Result{};
        }
        virtual void cmdLog(std::string, const at::Result &, std::chrono::milliseconds)
        {}
        virtual void cmdInit()
        {}
        virtual void cmdSend(std::string)
        {}
        virtual size_t cmdReceive(std::uint8_t *, std::chrono::milliseconds)
        {
            return 0;
        }
        virtual void cmdPost()
        {}
    };

    static BaseChannelStub baseChannelStub;

    class MockTMobileUS : public cellular::service::VolteCapabilityCellularInterface
    {
        auto getImsi(at::BaseChannel &) -> std::optional<std::string> override
        {
            return "310120123456789";
        }
    };

    SECTION("VolteCapabilityHandler success = TMobileUS")
    {
        using namespace cellular::service;
        VolteCapabilityHandler handler{std::make_unique<VolteAllowedList>(), std::make_unique<MockTMobileUS>()};
        auto result = handler.isVolteAllowed(baseChannelStub);
        REQUIRE(result == true);
    }

    SECTION("VolteCapabilityHandler support normal = TMobileUS")
    {
        using namespace cellular::service;
        VolteCapabilityHandler handler{std::make_unique<VolteAllowedList>(), std::make_unique<MockTMobileUS>()};
        auto result = handler.getSupportStatus(baseChannelStub);
        REQUIRE(result == ImsiParser::SupportStatus::Normal);
    }

    class MockNotAllowedIMSI : public cellular::service::VolteCapabilityCellularInterface
    {
        auto getImsi(at::BaseChannel &) -> std::optional<std::string>
        {
            return "310999123456789";
        }
    };

    SECTION("VolteCapabilityHandler failure = not allowed IMSI")
    {
        using namespace cellular::service;
        VolteCapabilityHandler handler{std::make_unique<VolteAllowedList>(), std::make_unique<MockNotAllowedIMSI>()};
        auto result = handler.isVolteAllowed(baseChannelStub);
        REQUIRE(result == false);
    }

    class MockAllowedBetaIMSI : public cellular::service::VolteCapabilityCellularInterface
    {
        auto getImsi(at::BaseChannel &) -> std::optional<std::string>
        {
            return "260032137213769"; // Orange PL HNI
        }
    };

    SECTION("VolteCapabilityHandler support beta = Orange PL")
    {
        using namespace cellular::service;
        VolteCapabilityHandler handler{std::make_unique<VolteAllowedList>(), std::make_unique<MockAllowedBetaIMSI>()};
        auto result = handler.getSupportStatus(baseChannelStub);
        REQUIRE(result == ImsiParser::SupportStatus::Beta);
    }

    class MockFailedToGetImsi : public cellular::service::VolteCapabilityCellularInterface
    {
        auto getImsi(at::BaseChannel &) -> std::optional<std::string>
        {
            return std::nullopt;
        }
    };

    SECTION("VolteCapabilityHandler failure = failed to get IMSI")
    {
        using namespace cellular::service;
        VolteCapabilityHandler handler{std::make_unique<VolteAllowedList>(), std::make_unique<MockFailedToGetImsi>()};
        auto result = handler.isVolteAllowed(baseChannelStub);
        REQUIRE(result == false);
    }

    SECTION("VolteCapabilityHandler support unknown = failed to get IMSI")
    {
        using namespace cellular::service;
        VolteCapabilityHandler handler{std::make_unique<VolteAllowedList>(), std::make_unique<MockFailedToGetImsi>()};
        auto result = handler.getSupportStatus(baseChannelStub);
        REQUIRE(result == ImsiParser::SupportStatus::Unsupported);
    }
}
