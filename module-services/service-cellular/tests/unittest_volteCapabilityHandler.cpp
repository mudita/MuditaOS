// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include <module-services/service-cellular/src/volte/VolteCapabilityHandler.hpp>
#include <module-services/service-cellular/src/volte/ImsiParserUS.hpp>
#include <module-services/service-cellular/src/volte/VolteAllowedUSList.hpp>
#include <module-services/service-cellular/src/volte/VolteCapabiltyHandlerCellularInterface.hpp>

TEST_CASE("VoLTE Capability handler")
{
    SECTION("ImsiParserUS success - US IMSI")
    {
        using namespace cellular::service;

        cellular::service::ImsiParserUS parser;
        std::string imsi("3111231234567890");

        auto result = parser.parse(imsi);

        REQUIRE(result.has_value() == true);
        REQUIRE(result.value().MCC == "311");
        REQUIRE(result.value().MNC == "123");
    }

    SECTION("ImsiParserUS failure - non US IMSI")
    {
        using namespace cellular::service;

        cellular::service::ImsiParserUS parser;
        std::string imsi("2601231234567890");

        auto result = parser.parse(imsi);

        REQUIRE(result.has_value() == false);
    }

    SECTION("ImsiParserUS failure -too short IMSI")
    {
        using namespace cellular::service;

        cellular::service::ImsiParserUS parser;
        std::string imsi("2601");

        auto result = parser.parse(imsi);

        REQUIRE(result.has_value() == false);
    }

    SECTION("VolteAllowedUSList success - TMobileUS IMSI")
    {
        using namespace cellular::service;

        VolteAllowedUSList list;
        OperatorInfo operatorInfo{"310", "120"};

        auto result = list.isVolteAllowed(operatorInfo);

        REQUIRE(result == true);
    }

    SECTION("VolteAllowedUSList failure - non TMobileUS MCC")
    {
        using namespace cellular::service;

        VolteAllowedUSList list;
        OperatorInfo operatorInfo{"210", "120"};

        auto result = list.isVolteAllowed(operatorInfo);

        REQUIRE(result == false);
    }

    SECTION("VolteAllowedUSList failure - non TMobileUS MNC")
    {
        using namespace cellular::service;

        VolteAllowedUSList list;
        OperatorInfo operatorInfo{"310", "999"};

        auto result = list.isVolteAllowed(operatorInfo);

        REQUIRE(result == false);
    }

    class MockTMobileUS : public cellular::service::VolteCapabilityCellularInterface
    {
        void setChannel(at::BaseChannel *channel)
        {}
        auto getImsi() -> std::optional<std::string>
        {
            return "310120123456789";
        }
    };

    SECTION("VolteCapabilityHandler succes = TMobileUS")
    {
        using namespace cellular::service;
        VolteCapabilityHandler handler{std::make_unique<ImsiParserUS>(),
                                       std::make_unique<VolteAllowedUSList>(),
                                       std::make_unique<MockTMobileUS>()};
        auto result = handler.isVolteAllowed();
        REQUIRE(result == true);
    }

    class MockNonTMobileUS : public cellular::service::VolteCapabilityCellularInterface
    {
        void setChannel(at::BaseChannel *channel)
        {}
        auto getImsi() -> std::optional<std::string>
        {
            return "310999123456789";
        }
    };

    SECTION("VolteCapabilityHandler failure = non TMobileUS")
    {
        using namespace cellular::service;
        VolteCapabilityHandler handler{std::make_unique<ImsiParserUS>(),
                                       std::make_unique<VolteAllowedUSList>(),
                                       std::make_unique<MockNonTMobileUS>()};
        auto result = handler.isVolteAllowed();
        REQUIRE(result == false);
    }

    class MockFailedToGetImsi : public cellular::service::VolteCapabilityCellularInterface
    {
        void setChannel(at::BaseChannel *channel)
        {}
        auto getImsi() -> std::optional<std::string>
        {
            return std::nullopt;
        }
    };

    SECTION("VolteCapabilityHandler failure = failed to get imsi")
    {
        using namespace cellular::service;
        VolteCapabilityHandler handler{std::make_unique<ImsiParserUS>(),
                                       std::make_unique<VolteAllowedUSList>(),
                                       std::make_unique<MockFailedToGetImsi>()};
        auto result = handler.isVolteAllowed();
        REQUIRE(result == false);
    }
}
