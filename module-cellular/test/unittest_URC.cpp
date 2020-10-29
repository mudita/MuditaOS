// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Utils.hpp"
#include <algorithm>
#include <cstdint>
#include <string>
#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <module-utils/time/time_conversion.hpp>

#include "URC_QIND.hpp"
#include "URC_CUSD.hpp"
#include "URC_CTZE.hpp"
#include "URC_CREG.hpp"

TEST_CASE("+QIND: csq")
{
    SECTION("Not valid data")
    {
        auto qind = at::urc::QIND("Not valid");
        REQUIRE_FALSE(qind.is());
    }

    SECTION("CSQ")
    {
        auto qind = at::urc::QIND("+QIND:\"csq\",100,50");
        REQUIRE(qind.is());
        REQUIRE(qind.isCsq());
        REQUIRE(*qind.getRSSI() == 100);
        REQUIRE(*qind.getBER() == 50);
    }

    SECTION("CSQ with white spaces")
    {
        auto qind = at::urc::QIND("+QIND: \"csq\" , 100 , 50 ");
        REQUIRE(qind.is());
        REQUIRE(qind.isCsq());
        REQUIRE(*qind.getRSSI() == 100);
        REQUIRE(*qind.getBER() == 50);
    }

    SECTION("too short")
    {
        auto qind = at::urc::QIND("+QIND:\"csq\",100");
        REQUIRE(qind.is());
        REQUIRE_FALSE(qind.isCsq());
        REQUIRE_FALSE(qind.getRSSI());
        REQUIRE_FALSE(qind.getBER());
    }

    SECTION("too long")
    {
        auto qind = at::urc::QIND("+QIND:\"csq\",100,50,25");
        REQUIRE(qind.is());
        REQUIRE_FALSE(qind.isCsq());
        REQUIRE_FALSE(qind.getRSSI());
        REQUIRE_FALSE(qind.getBER());
    }

    SECTION("no integer values")
    {
        auto qind = at::urc::QIND("+QIND:\"csq\",abc,def");
        REQUIRE(qind.is());
        REQUIRE(qind.isCsq());
        REQUIRE_FALSE(qind.getRSSI());
        REQUIRE_FALSE(qind.getBER());
    }

    SECTION("not CSQ")
    {
        auto qind = at::urc::QIND("+QIND:\"qsc\",100,50");
        REQUIRE(qind.is());
        REQUIRE_FALSE(qind.isCsq());
        REQUIRE_FALSE(qind.getRSSI());
        REQUIRE_FALSE(qind.getBER());
    }

    SECTION("valid CSQ")
    {
        std::vector<int> vec = {0, 1, 2, 98, 100, 101, 198, 200};
        for (auto a : vec) {
            auto qind = at::urc::QIND("+QIND:\"csq\"," + std::to_string(a) + ",50");
            REQUIRE(qind.is());
            REQUIRE(qind.isCsq());
            REQUIRE(*qind.getRSSI() == a);
        }
    }

    SECTION("not valid RSSI")
    {
        std::vector<std::uint32_t> vec = {99, 199};
        for (auto a : vec) {
            auto qind = at::urc::QIND("+QIND:\"csq\"," + std::to_string(a) + ",50");
            REQUIRE(qind.is());
            REQUIRE(qind.isCsq());
            REQUIRE_FALSE(qind.getRSSI());
        }
    }

    SECTION("not valid BER")
    {
        int ber   = 99;
        auto qind = at::urc::QIND("+QIND:\"csq\",50," + std::to_string(ber));
        REQUIRE(qind.is());
        REQUIRE(qind.isCsq());
        REQUIRE(*qind.getRSSI() == 50);
        REQUIRE_FALSE(qind.getBER());
    }
}

TEST_CASE("+CUSD")
{
    SECTION("Not valid data")
    {
        auto cusd = at::urc::CUSD("Not valid");
        REQUIRE_FALSE(cusd.is());
    }

    SECTION("CUSD action needed")
    {
        auto cusd = at::urc::CUSD("+CUSD:1,\"test msg\",14");
        REQUIRE(cusd.is());
        REQUIRE(cusd.isValid());
        REQUIRE(cusd.isActionNeeded());
        REQUIRE(*cusd.getMessage() == "test msg");
        REQUIRE(*cusd.getStatus() == at::urc::CUSD::StatusType::FurtherUserActionRequired);
        REQUIRE(*cusd.getDCS() == 14);
    }

    SECTION("CUSD action needed with white spaces")
    {
        auto cusd = at::urc::CUSD("+CUSD: 0 , \"test msg\" , 15 ");
        REQUIRE(cusd.is());
        REQUIRE(cusd.isValid());
        REQUIRE_FALSE(cusd.isActionNeeded());
        REQUIRE(*cusd.getMessage() == "test msg");
        REQUIRE(*cusd.getStatus() == at::urc::CUSD::StatusType::NoFurtherUserActionRequired);
        REQUIRE(*cusd.getDCS() == 15);
    }

    SECTION("CUSD wrong status and DCS")
    {
        auto cusd = at::urc::CUSD("+CUSD:100,\"test msg\", abc");
        REQUIRE(cusd.is());
        REQUIRE(cusd.isValid());
        REQUIRE_FALSE(cusd.isActionNeeded());
        REQUIRE(*cusd.getMessage() == "test msg");
        REQUIRE_FALSE(cusd.getStatus());
        REQUIRE_FALSE(cusd.getDCS());
    }

    SECTION("CUSD action not needed")
    {
        auto cusd = at::urc::CUSD("+CUSD:2,\"test msg\",15");
        REQUIRE(cusd.is());
        REQUIRE(cusd.isValid());
        REQUIRE_FALSE(cusd.isActionNeeded());
    }

    SECTION("no CUSD")
    {
        auto cusd = at::urc::CUSD("+CSUD:1,\"test msg\",15");
        REQUIRE_FALSE(cusd.is());
    }

    SECTION("too short")
    {
        auto cusd = at::urc::CUSD("+CUSD:1,\"test msg\"");
        REQUIRE(cusd.is());
        REQUIRE_FALSE(cusd.isValid());
    }

    SECTION("too long")
    {
        auto cusd = at::urc::CUSD("+CUSD:1,\"test msg\",15,15");
        REQUIRE(cusd.is());
        REQUIRE_FALSE(cusd.isValid());
    }
}

TEST_CASE("+CTZE")
{
    SECTION("Not valid data")
    {
        auto ctze = at::urc::CTZE("Not valid");
        REQUIRE_FALSE(ctze.is());
        REQUIRE_FALSE(ctze.isValid());
    }

    SECTION("ctze +08")
    {
        auto ctze = at::urc::CTZE("+CTZE: \"+08\",1,\"2020/10/21,13:49:57\"");
        REQUIRE(ctze.is());
        REQUIRE(ctze.isValid());
        auto timeInfo = ctze.getTimeInfo();
        std::stringstream ss;
        ss << std::put_time(&timeInfo, "%Y/%m/%d,%H:%M:%S");
        REQUIRE(ss.str() == "2020/10/21,15:49:57");

        REQUIRE(ctze.getTimeZoneOffset() == 8 * utils::time::minutesInQuarterOfHour * utils::time::secondsInMinute);
        REQUIRE(ctze.getTimeZoneString() == "+08,1");
        ss.str(std::string());
        timeInfo = ctze.getGMTTime();
        ss << std::put_time(&timeInfo, "%Y/%m/%d,%H:%M:%S");
        REQUIRE(ss.str() == "2020/10/21,13:49:57");
    }

    SECTION("ctze -08")
    {
        auto ctze = at::urc::CTZE("+CTZE: \"-08\",1,\"2020/10/21,13:49:57\"");
        REQUIRE(ctze.is());
        REQUIRE(ctze.isValid());
        auto timeInfo = ctze.getTimeInfo();
        std::ostringstream ss;
        ss << std::put_time(&timeInfo, "%Y/%m/%d,%H:%M:%S");
        REQUIRE(ss.str() == "2020/10/21,11:49:57");

        REQUIRE(ctze.getTimeZoneOffset() == -8 * utils::time::minutesInQuarterOfHour * utils::time::secondsInMinute);
        REQUIRE(ctze.getTimeZoneString() == "-08,1");
        ss.str(std::string());
        timeInfo = ctze.getGMTTime();
        ss << std::put_time(&timeInfo, "%Y/%m/%d,%H:%M:%S");
        REQUIRE(ss.str() == "2020/10/21,13:49:57");
    }

    SECTION("ctze -00")
    {
        auto ctze = at::urc::CTZE("+CTZE: \"-00\",0,\"2020/10/21,13:49:57\"");
        REQUIRE(ctze.is());
        REQUIRE(ctze.isValid());
        auto timeInfo = ctze.getTimeInfo();
        std::ostringstream ss;
        ss << std::put_time(&timeInfo, "%Y/%m/%d,%H:%M:%S");
        REQUIRE(ss.str() == "2020/10/21,13:49:57");

        REQUIRE(ctze.getTimeZoneOffset() == 0);
        REQUIRE(ctze.getTimeZoneString() == "-00,0");
        ss.str(std::string());
        timeInfo = ctze.getGMTTime();
        ss << std::put_time(&timeInfo, "%Y/%m/%d,%H:%M:%S");
        REQUIRE(ss.str() == "2020/10/21,13:49:57");
    }

    SECTION("too short")
    {
        auto ctze = at::urc::CTZE("+CTZE: \"-08\",\"2020/10/21,13:49:57\"");
        REQUIRE(ctze.is());
        REQUIRE_FALSE(ctze.isValid());
        // auto timeInfo = ctze.getTimeInfo();
    }
}

TEST_CASE("+CREG")
{
    SECTION("Not valid data")
    {
        auto creg = at::urc::CUSD("Not valid");
        REQUIRE_FALSE(creg.is());
    }

    SECTION("CREG short")
    {
        auto creg = at::urc::CREG("+CREG: 0");
        REQUIRE(creg.is());
        REQUIRE(creg.isValid());
        REQUIRE(creg.isShort());
        REQUIRE_FALSE(creg.isExtended());
        REQUIRE(creg.getStatus() == Store::Network::Status::NotRegistered);
        REQUIRE_FALSE(creg.getLocation());
        REQUIRE_FALSE(creg.getCellId());
        REQUIRE(creg.getAccessTechnology() == Store::Network::AccessTechnology::Unknown);
    }

    SECTION("CREG extended")
    {
        auto creg = at::urc::CREG("+CREG: 1,\"D509\",\"80D413D\",7");
        REQUIRE(creg.is());
        REQUIRE(creg.isValid());
        REQUIRE_FALSE(creg.isShort());
        REQUIRE(creg.isExtended());
        REQUIRE(creg.getStatus() == Store::Network::Status::RegisteredHomeNetwork);
        REQUIRE(*creg.getLocation() == "D509");
        REQUIRE(*creg.getCellId() == "80D413D");
        REQUIRE(creg.getAccessTechnology() == Store::Network::AccessTechnology::EUtran);
    }

    SECTION("CREG extended access technology")
    {
        {
            auto creg = at::urc::CREG("+CREG: 1,\"D509\",\"80D413D\",0");
            REQUIRE(creg.getAccessTechnology() == Store::Network::AccessTechnology::Gsm);
        }
        {
            auto creg = at::urc::CREG("+CREG: 1,\"D509\",\"80D413D\",1");
            REQUIRE(creg.getAccessTechnology() == Store::Network::AccessTechnology::Unknown);
        }
        {
            auto creg = at::urc::CREG("+CREG: 1,\"D509\",\"80D413D\",7");
            REQUIRE(creg.getAccessTechnology() == Store::Network::AccessTechnology::EUtran);
        }
        {
            auto creg = at::urc::CREG("+CREG: 1,\"D509\",\"80D413D\",8");
            REQUIRE(creg.getAccessTechnology() == Store::Network::AccessTechnology::Unknown);
        }
        {
            auto creg = at::urc::CREG("+CREG: 1,\"D509\",\"80D413D\",ABX");
            REQUIRE(creg.getAccessTechnology() == Store::Network::AccessTechnology::Unknown);
        }
    }

    SECTION("CREG status")
    {
        {
            auto creg = at::urc::CREG("+CREG: 0,\"D509\",\"80D413D\",0");
            REQUIRE(creg.getStatus() == Store::Network::Status::NotRegistered);
        }
        {
            auto creg = at::urc::CREG("+CREG: 5,\"D509\",\"80D413D\",0");
            REQUIRE(creg.getStatus() == Store::Network::Status::RegisteredRoaming);
        }
        {
            auto creg = at::urc::CREG("+CREG: 4,\"D509\",\"80D413D\",0");
            REQUIRE(creg.getStatus() == Store::Network::Status::Unknown);
        }
        {
            auto creg = at::urc::CREG("+CREG: 6,\"D509\",\"80D413D\",0");
            REQUIRE(creg.getStatus() == Store::Network::Status::Unknown);
        }
        {
            auto creg = at::urc::CREG("+CREG: A,\"D509\",\"80D413D\",0");
            REQUIRE(creg.getStatus() == Store::Network::Status::Unknown);
        }
    }

    SECTION("CREG no CREG")
    {
        auto creg = at::urc::CREG("+CEGR: 0");
        REQUIRE_FALSE(creg.is());
        REQUIRE_FALSE(creg.isValid());
        REQUIRE(creg.isShort());
        REQUIRE_FALSE(creg.isExtended());
        REQUIRE(creg.getStatus() == Store::Network::Status::Unknown);
        REQUIRE_FALSE(creg.getLocation());
        REQUIRE_FALSE(creg.getCellId());
        REQUIRE(creg.getAccessTechnology() == Store::Network::AccessTechnology::Unknown);
    }

    SECTION("CREG too short")
    {
        auto creg = at::urc::CREG("+CREG:");
        REQUIRE(creg.is());
        REQUIRE_FALSE(creg.isValid());
        REQUIRE_FALSE(creg.isShort());
        REQUIRE_FALSE(creg.isExtended());
        REQUIRE(creg.getStatus() == Store::Network::Status::Unknown);
        REQUIRE_FALSE(creg.getLocation());
        REQUIRE_FALSE(creg.getCellId());
        REQUIRE(creg.getAccessTechnology() == Store::Network::AccessTechnology::Unknown);
    }

    SECTION("CREG too long")
    {
        auto creg = at::urc::CREG("+CREG: 0,\"D509\",\"80D413D\",0,1");
        REQUIRE(creg.is());
        REQUIRE_FALSE(creg.isValid());
        REQUIRE_FALSE(creg.isShort());
        REQUIRE_FALSE(creg.isExtended());
        REQUIRE(creg.getStatus() == Store::Network::Status::Unknown);
        REQUIRE_FALSE(creg.getLocation());
        REQUIRE_FALSE(creg.getCellId());
        REQUIRE(creg.getAccessTechnology() == Store::Network::AccessTechnology::Unknown);
    }

    SECTION("CREG wrong length")
    {
        auto creg = at::urc::CREG("+CREG: 0,\"D509\"");
        REQUIRE(creg.is());
        REQUIRE_FALSE(creg.isValid());
        REQUIRE_FALSE(creg.isShort());
        REQUIRE_FALSE(creg.isExtended());
        REQUIRE(creg.getStatus() == Store::Network::Status::Unknown);
        REQUIRE_FALSE(creg.getLocation());
        REQUIRE_FALSE(creg.getCellId());
        REQUIRE(creg.getAccessTechnology() == Store::Network::AccessTechnology::Unknown);
    }
}
