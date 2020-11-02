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
#include "URC_CMTI.hpp"
#include "URC_CLIP.hpp"

TEST_CASE("+QIND: csq")
{
    SECTION("CSQ")
    {
        auto urc  = at::urc::URC::Create("+QIND:\"csq\",100,50");
        auto qind = at::urc::getURC<at::urc::QIND>(urc);
        REQUIRE(qind);
        REQUIRE(qind->isCsq());
        REQUIRE(*qind->getRSSI() == 100);
        REQUIRE(*qind->getBER() == 50);
    }

    SECTION("CSQ with white spaces")
    {
        auto urc  = at::urc::URC::Create("+QIND: \"csq\" , 100 , 50 ");
        auto qind = at::urc::getURC<at::urc::QIND>(urc);
        REQUIRE(qind);
        REQUIRE(qind->isCsq());
        REQUIRE(*qind->getRSSI() == 100);
        REQUIRE(*qind->getBER() == 50);
    }

    SECTION("too short")
    {
        auto urc  = at::urc::URC::Create("+QIND:\"csq\",100");
        auto qind = at::urc::getURC<at::urc::QIND>(urc);
        REQUIRE(qind);
        REQUIRE_FALSE(qind->isCsq());
        REQUIRE_FALSE(qind->getRSSI());
        REQUIRE_FALSE(qind->getBER());
    }

    SECTION("too long")
    {
        auto urc  = at::urc::URC::Create("+QIND:\"csq\",100,50,25");
        auto qind = at::urc::getURC<at::urc::QIND>(urc);
        REQUIRE(qind);
        REQUIRE_FALSE(qind->isCsq());
        REQUIRE_FALSE(qind->getRSSI());
        REQUIRE_FALSE(qind->getBER());
    }

    SECTION("no integer values")
    {
        auto urc  = at::urc::URC::Create("+QIND:\"csq\",abc,def");
        auto qind = at::urc::getURC<at::urc::QIND>(urc);
        REQUIRE(qind);
        REQUIRE(qind->isCsq());
        REQUIRE_FALSE(qind->getRSSI());
        REQUIRE_FALSE(qind->getBER());
    }

    SECTION("not CSQ")
    {
        auto urc  = at::urc::URC::Create("+QIND:\"qsc\",100,50");
        auto qind = at::urc::getURC<at::urc::QIND>(urc);
        REQUIRE(qind);
        REQUIRE_FALSE(qind->isCsq());
        REQUIRE_FALSE(qind->getRSSI());
        REQUIRE_FALSE(qind->getBER());
    }

    SECTION("valid CSQ")
    {
        std::vector<int> vec = {0, 1, 2, 98, 100, 101, 198, 200};
        for (auto a : vec) {
            auto urc  = at::urc::URC::Create("+QIND:\"csq\"," + std::to_string(a) + ",50");
            auto qind = at::urc::getURC<at::urc::QIND>(urc);
            REQUIRE(qind);
            REQUIRE(qind->isCsq());
            REQUIRE(*qind->getRSSI() == a);
        }
    }

    SECTION("not valid RSSI")
    {
        std::vector<std::uint32_t> vec = {99, 199};
        for (auto a : vec) {
            auto urc  = at::urc::URC::Create("+QIND:\"csq\"," + std::to_string(a) + ",50");
            auto qind = at::urc::getURC<at::urc::QIND>(urc);
            REQUIRE(qind);
            REQUIRE(qind->isCsq());
            REQUIRE_FALSE(qind->getRSSI());
        }
    }

    SECTION("not valid BER")
    {
        int ber   = 99;
        auto urc  = at::urc::URC::Create("+QIND:\"csq\",50," + std::to_string(ber));
        auto qind = at::urc::getURC<at::urc::QIND>(urc);
        REQUIRE(qind);
        REQUIRE(qind->isCsq());
        REQUIRE(*qind->getRSSI() == 50);
        REQUIRE_FALSE(qind->getBER());
    }
}

TEST_CASE("+CUSD")
{
    SECTION("CUSD action needed")
    {
        auto urc  = at::urc::URC::Create("+CUSD:1,\"test msg\",14");
        auto cusd = at::urc::getURC<at::urc::CUSD>(urc);

        REQUIRE(cusd);
        REQUIRE(cusd->isValid());
        REQUIRE(cusd->isActionNeeded());
        REQUIRE(*cusd->getMessage() == "test msg");
        REQUIRE(*cusd->getStatus() == at::urc::CUSD::StatusType::FurtherUserActionRequired);
        REQUIRE(*cusd->getDCS() == 14);
    }

    SECTION("CUSD action needed with white spaces")
    {
        auto urc  = at::urc::URC::Create("+CUSD: 0 , \"test msg\" , 15 ");
        auto cusd = at::urc::getURC<at::urc::CUSD>(urc);
        REQUIRE(cusd);
        REQUIRE(cusd->isValid());
        REQUIRE_FALSE(cusd->isActionNeeded());
        REQUIRE(*cusd->getMessage() == "test msg");
        REQUIRE(*cusd->getStatus() == at::urc::CUSD::StatusType::NoFurtherUserActionRequired);
        REQUIRE(*cusd->getDCS() == 15);
    }

    SECTION("CUSD wrong status and DCS")
    {
        auto urc  = at::urc::URC::Create("+CUSD:100,\"test msg\", abc");
        auto cusd = at::urc::getURC<at::urc::CUSD>(urc);
        REQUIRE(cusd);
        REQUIRE(cusd->isValid());
        REQUIRE_FALSE(cusd->isActionNeeded());
        REQUIRE(*cusd->getMessage() == "test msg");
        REQUIRE_FALSE(cusd->getStatus());
        REQUIRE_FALSE(cusd->getDCS());
    }

    SECTION("CUSD action not needed")
    {
        auto urc  = at::urc::URC::Create("+CUSD:2,\"test msg\",15");
        auto cusd = at::urc::getURC<at::urc::CUSD>(urc);
        REQUIRE(cusd);
        REQUIRE(cusd->isValid());
        REQUIRE_FALSE(cusd->isActionNeeded());
    }

    SECTION("no CUSD")
    {
        auto urc  = at::urc::URC::Create("+CSUD:1,\"test msg\",15");
        auto cusd = at::urc::getURC<at::urc::CUSD>(urc);
        REQUIRE_FALSE(cusd);
    }

    SECTION("too short")
    {
        auto urc  = at::urc::URC::Create("+CUSD:1,\"test msg\"");
        auto cusd = at::urc::getURC<at::urc::CUSD>(urc);
        REQUIRE(cusd);
        REQUIRE_FALSE(cusd->isValid());
    }

    SECTION("too long")
    {
        auto urc  = at::urc::URC::Create("+CUSD:1,\"test msg\",15,15");
        auto cusd = at::urc::getURC<at::urc::CUSD>(urc);
        REQUIRE(cusd);
        REQUIRE_FALSE(cusd->isValid());
    }
}

TEST_CASE("+CTZE")
{
    SECTION("Not valid data")
    {
        auto urc  = at::urc::URC::Create("+QIND:\"csq\"");
        auto ctze = at::urc::getURC<at::urc::CTZE>(urc);
        REQUIRE_FALSE(ctze);
    }

    SECTION("ctze +08")
    {
        auto urc  = at::urc::URC::Create("+CTZE: \"+08\",1,\"2020/10/21,13:49:57\"");
        auto ctze = at::urc::getURC<at::urc::CTZE>(urc);
        REQUIRE(ctze);
        REQUIRE(ctze->isValid());
        auto timeInfo = ctze->getTimeInfo();
        std::stringstream ss;
        ss << std::put_time(&timeInfo, "%Y/%m/%d,%H:%M:%S");
        REQUIRE(ss.str() == "2020/10/21,15:49:57");

        REQUIRE(ctze->getTimeZoneOffset() == 8 * utils::time::minutesInQuarterOfHour * utils::time::secondsInMinute);
        REQUIRE(ctze->getTimeZoneString() == "+08,1");
        ss.str(std::string());
        timeInfo = ctze->getGMTTime();
        ss << std::put_time(&timeInfo, "%Y/%m/%d,%H:%M:%S");
        REQUIRE(ss.str() == "2020/10/21,13:49:57");
    }

    SECTION("ctze -08")
    {
        auto urc  = at::urc::URC::Create("+CTZE: \"-08\",1,\"2020/10/21,13:49:57\"");
        auto ctze = at::urc::getURC<at::urc::CTZE>(urc);
        REQUIRE(ctze);
        REQUIRE(ctze->isValid());
        auto timeInfo = ctze->getTimeInfo();
        std::ostringstream ss;
        ss << std::put_time(&timeInfo, "%Y/%m/%d,%H:%M:%S");
        REQUIRE(ss.str() == "2020/10/21,11:49:57");

        REQUIRE(ctze->getTimeZoneOffset() == -8 * utils::time::minutesInQuarterOfHour * utils::time::secondsInMinute);
        REQUIRE(ctze->getTimeZoneString() == "-08,1");
        ss.str(std::string());
        timeInfo = ctze->getGMTTime();
        ss << std::put_time(&timeInfo, "%Y/%m/%d,%H:%M:%S");
        REQUIRE(ss.str() == "2020/10/21,13:49:57");
    }

    SECTION("ctze -00")
    {
        auto urc  = at::urc::URC::Create("+CTZE: \"-00\",0,\"2020/10/21,13:49:57\"");
        auto ctze = at::urc::getURC<at::urc::CTZE>(urc);
        REQUIRE(ctze);
        REQUIRE(ctze->isValid());
        auto timeInfo = ctze->getTimeInfo();
        std::ostringstream ss;
        ss << std::put_time(&timeInfo, "%Y/%m/%d,%H:%M:%S");
        REQUIRE(ss.str() == "2020/10/21,13:49:57");

        REQUIRE(ctze->getTimeZoneOffset() == 0);
        REQUIRE(ctze->getTimeZoneString() == "-00,0");
        ss.str(std::string());
        timeInfo = ctze->getGMTTime();
        ss << std::put_time(&timeInfo, "%Y/%m/%d,%H:%M:%S");
        REQUIRE(ss.str() == "2020/10/21,13:49:57");
    }

    SECTION("too short")
    {
        auto urc  = at::urc::URC::Create("+CTZE: \"-08\",\"2020/10/21,13:49:57\"");
        auto ctze = at::urc::getURC<at::urc::CTZE>(urc);
        REQUIRE(ctze);
        REQUIRE_FALSE(ctze->isValid());
        // auto timeInfo = ctze->getTimeInfo();
    }
}

TEST_CASE("+CREG")
{
    SECTION("CREG short")
    {
        auto urc  = at::urc::URC::Create("+CREG: 0");
        auto creg = at::urc::getURC<at::urc::CREG>(urc);
        REQUIRE(creg);
        REQUIRE(creg->isValid());
        REQUIRE(creg->isShort());
        REQUIRE_FALSE(creg->isExtended());
        REQUIRE(creg->getStatus() == Store::Network::Status::NotRegistered);
        REQUIRE_FALSE(creg->getLocation());
        REQUIRE_FALSE(creg->getCellId());
        REQUIRE(creg->getAccessTechnology() == Store::Network::AccessTechnology::Unknown);
    }

    SECTION("CREG extended")
    {
        auto urc  = at::urc::URC::Create("+CREG: 1,\"D509\",\"80D413D\",7");
        auto creg = at::urc::getURC<at::urc::CREG>(urc);
        REQUIRE(creg);
        REQUIRE(creg->isValid());
        REQUIRE_FALSE(creg->isShort());
        REQUIRE(creg->isExtended());
        REQUIRE(creg->getStatus() == Store::Network::Status::RegisteredHomeNetwork);
        REQUIRE(*creg->getLocation() == "D509");
        REQUIRE(*creg->getCellId() == "80D413D");
        REQUIRE(creg->getAccessTechnology() == Store::Network::AccessTechnology::EUtran);
    }

    SECTION("CREG extended access technology")
    {
        {
            auto urc  = at::urc::URC::Create("+CREG: 1,\"D509\",\"80D413D\",0");
            auto creg = at::urc::getURC<at::urc::CREG>(urc);
            REQUIRE(creg->getAccessTechnology() == Store::Network::AccessTechnology::Gsm);
        }
        {
            auto urc  = at::urc::URC::Create("+CREG: 1,\"D509\",\"80D413D\",1");
            auto creg = at::urc::getURC<at::urc::CREG>(urc);
            REQUIRE(creg->getAccessTechnology() == Store::Network::AccessTechnology::Unknown);
        }
        {
            auto urc  = at::urc::URC::Create("+CREG: 1,\"D509\",\"80D413D\",7");
            auto creg = at::urc::getURC<at::urc::CREG>(urc);
            REQUIRE(creg->getAccessTechnology() == Store::Network::AccessTechnology::EUtran);
        }
        {
            auto urc  = at::urc::URC::Create("+CREG: 1,\"D509\",\"80D413D\",8");
            auto creg = at::urc::getURC<at::urc::CREG>(urc);
            REQUIRE(creg->getAccessTechnology() == Store::Network::AccessTechnology::Unknown);
        }
        {
            auto urc  = at::urc::URC::Create("+CREG: 1,\"D509\",\"80D413D\",ABX");
            auto creg = at::urc::getURC<at::urc::CREG>(urc);
            REQUIRE(creg->getAccessTechnology() == Store::Network::AccessTechnology::Unknown);
        }
    }

    SECTION("CREG status")
    {
        {
            auto urc  = at::urc::URC::Create("+CREG: 0,\"D509\",\"80D413D\",0");
            auto creg = at::urc::getURC<at::urc::CREG>(urc);
            REQUIRE(creg->getStatus() == Store::Network::Status::NotRegistered);
        }
        {
            auto urc  = at::urc::URC::Create("+CREG: 5,\"D509\",\"80D413D\",0");
            auto creg = at::urc::getURC<at::urc::CREG>(urc);
            REQUIRE(creg->getStatus() == Store::Network::Status::RegisteredRoaming);
        }
        {
            auto urc  = at::urc::URC::Create("+CREG: 4,\"D509\",\"80D413D\",0");
            auto creg = at::urc::getURC<at::urc::CREG>(urc);
            REQUIRE(creg->getStatus() == Store::Network::Status::Unknown);
        }
        {
            auto urc  = at::urc::URC::Create("+CREG: 6,\"D509\",\"80D413D\",0");
            auto creg = at::urc::getURC<at::urc::CREG>(urc);
            REQUIRE(creg->getStatus() == Store::Network::Status::Unknown);
        }
        {
            auto urc  = at::urc::URC::Create("+CREG: A,\"D509\",\"80D413D\",0");
            auto creg = at::urc::getURC<at::urc::CREG>(urc);
            REQUIRE(creg->getStatus() == Store::Network::Status::Unknown);
        }
    }

    SECTION("CREG no CREG")
    {
        auto urc  = at::urc::URC::Create("+CEGR: 0");
        auto creg = at::urc::getURC<at::urc::CREG>(urc);
        REQUIRE_FALSE(creg);
    }

    SECTION("CREG too short")
    {
        auto urc  = at::urc::URC::Create("+CREG:");
        auto creg = at::urc::getURC<at::urc::CREG>(urc);
        REQUIRE(creg);
        REQUIRE_FALSE(creg->isValid());
    }

    SECTION("CREG too long")
    {
        auto urc  = at::urc::URC::Create("+CREG: 0,\"D509\",\"80D413D\",0,1");
        auto creg = at::urc::getURC<at::urc::CREG>(urc);
        REQUIRE(creg);
        REQUIRE_FALSE(creg->isValid());
    }

    SECTION("CREG wrong length")
    {
        auto urc  = at::urc::URC::Create("+CREG: 0,\"D509\"");
        auto creg = at::urc::getURC<at::urc::CREG>(urc);
        REQUIRE(creg);
        REQUIRE_FALSE(creg->isValid());
    }
}

TEST_CASE("+CMTI")
{
    SECTION("CMTI too short")
    {
        auto urc  = at::urc::URC::Create("+CMTI: \"ME\"");
        auto cmti = at::urc::getURC<at::urc::CMTI>(urc);
        REQUIRE(cmti);
        REQUIRE_FALSE(cmti->isValid());
    }

    SECTION("CMTI too long")
    {
        auto urc  = at::urc::URC::Create("+CMTI: \"ME\",1,1");
        auto cmti = at::urc::getURC<at::urc::CMTI>(urc);
        REQUIRE(cmti);
        REQUIRE_FALSE(cmti->isValid());
    }

    SECTION("CMTI valid")
    {
        auto urc  = at::urc::URC::Create("+CMTI: \"ME\",1");
        auto cmti = at::urc::getURC<at::urc::CMTI>(urc);
        REQUIRE(cmti);
        REQUIRE(cmti->isValid());
        REQUIRE(cmti->getIndex() == "1");
        REQUIRE(cmti->getMemory() == "ME");
    }
}

TEST_CASE("+CLIP")
{
    SECTION("CLIP too short")
    {
        auto urc  = at::urc::URC::Create("+CLIP: \"+48123456789\"");
        auto clip = at::urc::getURC<at::urc::CLIP>(urc);
        REQUIRE(clip);
        REQUIRE_FALSE(clip->isValid());
    }

    SECTION("CLIP valid")
    {
        // two parameters
        auto urc  = at::urc::URC::Create("+CLIP: \"+48123456789\",145");
        auto clip = at::urc::getURC<at::urc::CLIP>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());

        // three parameters
        urc  = at::urc::URC::Create("+CLIP: \"+48123456789\",145,");
        clip = at::urc::getURC<at::urc::CLIP>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE_FALSE(clip->getSubaddr());
        REQUIRE_FALSE(clip->getAlpha());
        REQUIRE_FALSE(clip->getSatype());
        REQUIRE_FALSE(clip->getCLIValidity());

        // four parameters
        urc  = at::urc::URC::Create("+CLIP: \"+48123456789\",145,,");
        clip = at::urc::getURC<at::urc::CLIP>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE_FALSE(clip->getSubaddr());
        REQUIRE_FALSE(clip->getAlpha());
        REQUIRE_FALSE(clip->getSatype());
        REQUIRE_FALSE(clip->getCLIValidity());

        // six parameters
        urc  = at::urc::URC::Create("+CLIP: \"+48123456789\",145,,,");
        clip = at::urc::getURC<at::urc::CLIP>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE_FALSE(clip->getSubaddr());
        REQUIRE_FALSE(clip->getAlpha());
        REQUIRE_FALSE(clip->getSatype());
        REQUIRE_FALSE(clip->getCLIValidity());

        // seven parameters
        urc  = at::urc::URC::Create("+CLIP: \"+48123456789\",145,,,,");
        clip = at::urc::getURC<at::urc::CLIP>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE_FALSE(clip->getSubaddr());
        REQUIRE_FALSE(clip->getAlpha());
        REQUIRE_FALSE(clip->getSatype());
        REQUIRE_FALSE(clip->getCLIValidity());

        // two parameters with validation not empty
        urc  = at::urc::URC::Create("+CLIP: \"+48123456789\",145,,,,0");
        clip = at::urc::getURC<at::urc::CLIP>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE_FALSE(clip->getSubaddr());
        REQUIRE_FALSE(clip->getAlpha());
        REQUIRE_FALSE(clip->getSatype());
        REQUIRE(clip->getCLIValidity() == "0");
    }

    SECTION("CLIP phone number")
    {
        auto urc  = at::urc::URC::Create("+CLIP: \"+48123456789\",145,,,,0");
        auto clip = at::urc::getURC<at::urc::CLIP>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE(clip->getNumber() == "+48123456789");

        urc  = at::urc::URC::Create("+CLIP: \"\",145,,,,0");
        clip = at::urc::getURC<at::urc::CLIP>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE(clip->getNumber() == "");

        urc  = at::urc::URC::Create("+CLIP: ,145,,,,0");
        clip = at::urc::getURC<at::urc::CLIP>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE(clip->getNumber() == "");
    }

    SECTION("CLIP address type")
    {
        auto urc  = at::urc::URC::Create("+CLIP: \"+48123456789\",,,,,0");
        auto clip = at::urc::getURC<at::urc::CLIP>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE(clip->getType() == at::urc::CLIP::AddressType::FailedToParse);

        urc  = at::urc::URC::Create("+CLIP: \"+48123456789\",577,,,,0");
        clip = at::urc::getURC<at::urc::CLIP>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE(clip->getType() == at::urc::CLIP::AddressType::FailedToParse);

        urc  = at::urc::URC::Create("+CLIP: \"+48123456789\",\"test\",,,,0");
        clip = at::urc::getURC<at::urc::CLIP>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE(clip->getType() == at::urc::CLIP::AddressType::FailedToParse);

        urc  = at::urc::URC::Create("+CLIP: \"+48123456789\",test,,,,0");
        clip = at::urc::getURC<at::urc::CLIP>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE(clip->getType() == at::urc::CLIP::AddressType::FailedToParse);

        urc  = at::urc::URC::Create("+CLIP: \"+48123456789\",129,,,,0");
        clip = at::urc::getURC<at::urc::CLIP>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE(clip->getType() == at::urc::CLIP::AddressType::UnknownType);

        urc  = at::urc::URC::Create("+CLIP: \"+48123456789\",145,,,,0");
        clip = at::urc::getURC<at::urc::CLIP>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE(clip->getType() == at::urc::CLIP::AddressType::InternationalNumber);

        urc  = at::urc::URC::Create("+CLIP: \"+48123456789\",161,,,,0");
        clip = at::urc::getURC<at::urc::CLIP>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE(clip->getType() == at::urc::CLIP::AddressType::NationalNumber);
    }

    SECTION("CLIP optional parameters")
    {
        auto urc  = at::urc::URC::Create("+CLIP: \"+48123456789\",145,,,,");
        auto clip = at::urc::getURC<at::urc::CLIP>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE_FALSE(clip->getAlpha());
        REQUIRE_FALSE(clip->getSatype());
        REQUIRE_FALSE(clip->getSubaddr());
        REQUIRE_FALSE(clip->getCLIValidity());

        urc  = at::urc::URC::Create("+CLIP: \"+48123456789\",145,1,2,3,0");
        clip = at::urc::getURC<at::urc::CLIP>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE(clip->getAlpha() == "3");
        REQUIRE(clip->getSatype() == "2");
        REQUIRE(clip->getSubaddr() == "1");
        REQUIRE(clip->getCLIValidity() == "0");
    }
}
