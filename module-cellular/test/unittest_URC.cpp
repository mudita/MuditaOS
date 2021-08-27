// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Utils.hpp"
#include <algorithm>
#include <cstdint>
#include <string>
#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <time/time_conversion.hpp>
#include <time/time_constants.hpp>

#include "UrcQind.hpp"
#include "UrcCusd.hpp"
#include "UrcCtze.hpp"
#include "UrcCreg.hpp"
#include "UrcCmti.hpp"
#include "UrcClip.hpp"
#include "UrcCpin.hpp"
#include "UrcQiurc.hpp"
#include "UrcRing.hpp"
#include "UrcPoweredDown.hpp"
#include "UrcResponse.hpp"
#include "UrcFactory.hpp"
#include "SimState.hpp"

template <typename urcType> static auto getURC(std::unique_ptr<at::urc::Urc> &urc) -> std::shared_ptr<urcType>
{
    if (urc) {
        auto &rawUrc = *urc.get();
        if (typeid(rawUrc) == typeid(urcType)) {
            return std::unique_ptr<urcType>{static_cast<urcType *>(urc.release())};
        }
    }

    return nullptr;
}

TEST_CASE("+Qind: csq")
{
    SECTION("CSQ")
    {
        auto urc  = at::urc::UrcFactory::Create("+QIND:\"csq\",100,50");
        auto qind = getURC<at::urc::Qind>(urc);
        REQUIRE(qind);
        REQUIRE(qind->isCsq());
        REQUIRE(*qind->getRSSI() == 100);
        REQUIRE(*qind->getBER() == 50);
    }

    SECTION("CSQ with white spaces")
    {
        auto urc  = at::urc::UrcFactory::Create("+QIND: \"csq\" , 100 , 50 ");
        auto qind = getURC<at::urc::Qind>(urc);
        REQUIRE(qind);
        REQUIRE(qind->isCsq());
        REQUIRE(*qind->getRSSI() == 100);
        REQUIRE(*qind->getBER() == 50);
    }

    SECTION("too short")
    {
        auto urc  = at::urc::UrcFactory::Create("+QIND:\"csq\",100");
        auto qind = getURC<at::urc::Qind>(urc);
        REQUIRE(qind);
        REQUIRE_FALSE(qind->isCsq());
        REQUIRE_FALSE(qind->getRSSI());
        REQUIRE_FALSE(qind->getBER());
    }

    SECTION("too long")
    {
        auto urc  = at::urc::UrcFactory::Create("+QIND:\"csq\",100,50,25");
        auto qind = getURC<at::urc::Qind>(urc);
        REQUIRE(qind);
        REQUIRE_FALSE(qind->isCsq());
        REQUIRE_FALSE(qind->getRSSI());
        REQUIRE_FALSE(qind->getBER());
    }

    SECTION("no integer values")
    {
        auto urc  = at::urc::UrcFactory::Create("+QIND:\"csq\",abc,def");
        auto qind = getURC<at::urc::Qind>(urc);
        REQUIRE(qind);
        REQUIRE(qind->isCsq());
        REQUIRE_FALSE(qind->getRSSI());
        REQUIRE_FALSE(qind->getBER());
    }

    SECTION("not CSQ")
    {
        auto urc  = at::urc::UrcFactory::Create("+QIND:\"qsc\",100,50");
        auto qind = getURC<at::urc::Qind>(urc);
        REQUIRE(qind);
        REQUIRE_FALSE(qind->isCsq());
        REQUIRE_FALSE(qind->getRSSI());
        REQUIRE_FALSE(qind->getBER());
    }

    SECTION("valid CSQ")
    {
        std::vector<int> vec = {0, 1, 2, 98, 100, 101, 198, 200};
        for (auto a : vec) {
            auto urc  = at::urc::UrcFactory::Create("+QIND:\"csq\"," + std::to_string(a) + ",50");
            auto qind = getURC<at::urc::Qind>(urc);
            REQUIRE(qind);
            REQUIRE(qind->isCsq());
            REQUIRE(*qind->getRSSI() == a);
        }
    }

    SECTION("not valid RSSI")
    {
        std::vector<std::uint32_t> vec = {99, 199};
        for (auto a : vec) {
            auto urc  = at::urc::UrcFactory::Create("+QIND:\"csq\"," + std::to_string(a) + ",50");
            auto qind = getURC<at::urc::Qind>(urc);
            REQUIRE(qind);
            REQUIRE(qind->isCsq());
            REQUIRE_FALSE(qind->getRSSI());
        }
    }

    SECTION("not valid BER")
    {
        int ber   = 99;
        auto urc  = at::urc::UrcFactory::Create("+QIND:\"csq\",50," + std::to_string(ber));
        auto qind = getURC<at::urc::Qind>(urc);
        REQUIRE(qind);
        REQUIRE(qind->isCsq());
        REQUIRE(*qind->getRSSI() == 50);
        REQUIRE_FALSE(qind->getBER());
    }
}

TEST_CASE("+Qind: act")
{
    SECTION("ACT")
    {
        auto urc  = at::urc::UrcFactory::Create("+QIND:\"act\",\"HSDPA&HSUPA\"");
        auto qind = getURC<at::urc::Qind>(urc);
        REQUIRE(qind);
        REQUIRE(qind->isAct());
        REQUIRE(qind->getAct() == "HSDPA&HSUPA");
        REQUIRE(qind->getAccessTechnology() == Store::Network::AccessTechnology::UtranWHsdpaAndWHsupa);
    }

    SECTION("too short")
    {
        auto urc  = at::urc::UrcFactory::Create("+QIND:\"act\"");
        auto qind = getURC<at::urc::Qind>(urc);
        REQUIRE(qind);
        REQUIRE_FALSE(qind->isAct());
        REQUIRE(qind->getAct() == "");
        REQUIRE(qind->getAccessTechnology() == Store::Network::AccessTechnology::Unknown);
    }

    SECTION("too long")
    {
        auto urc  = at::urc::UrcFactory::Create("+QIND:\"act\",\"HSDPA&HSUPA\",\"HSDPA&HSUPA\"");
        auto qind = getURC<at::urc::Qind>(urc);
        REQUIRE(qind);
        REQUIRE_FALSE(qind->isCsq());
        REQUIRE(qind->getAct() == "");
        REQUIRE(qind->getAccessTechnology() == Store::Network::AccessTechnology::Unknown);
    }

    SECTION("not act")
    {
        auto urc  = at::urc::UrcFactory::Create("+QIND:\"csq\",\"HSDPA&HSUPA\"");
        auto qind = getURC<at::urc::Qind>(urc);
        REQUIRE(qind);
        REQUIRE_FALSE(qind->isAct());
        REQUIRE(qind->getAct() == "");
        REQUIRE(qind->getAccessTechnology() == Store::Network::AccessTechnology::Unknown);
    }

    SECTION("Uknown NAT")
    {
        auto urc  = at::urc::UrcFactory::Create("+QIND:\"act\",\"Super\"");
        auto qind = getURC<at::urc::Qind>(urc);
        REQUIRE(qind);
        REQUIRE(qind->isAct());
        REQUIRE(qind->getAct() == "Super");
        REQUIRE(qind->getAccessTechnology() == Store::Network::AccessTechnology::Unknown);
    }
}

TEST_CASE("+Qind: FOTA")
{
    SECTION("FOTA")
    {
        // fota normal
        auto urc  = at::urc::UrcFactory::Create("+QIND:\"FOTA\",\"HTTPEND\",50");
        auto qind = getURC<at::urc::Qind>(urc);
        REQUIRE(qind);
        REQUIRE(qind->isFotaValid());
        // fota dirty
        urc  = at::urc::UrcFactory::Create("\r\r\n\n+QIND: \r\n\"FOTA\",\"HTTPEND\",50\n\n\r\r");
        qind = getURC<at::urc::Qind>(urc);
        REQUIRE(qind);
        REQUIRE(qind->isFotaValid());
    }

    SECTION("FOTA malformed")
    {
        auto urc  = at::urc::UrcFactory::Create("+QIND:\"FOTA\"");
        auto qind = getURC<at::urc::Qind>(urc);
        REQUIRE(qind);
        REQUIRE(qind->isFota());
        REQUIRE_FALSE(qind->isFotaValid());
    }

    SECTION("FOTA stage and parameter")
    {
        auto urc  = at::urc::UrcFactory::Create("+QIND: \"FOTA\",\"HTTPSTART\"");
        auto qind = getURC<at::urc::Qind>(urc);
        REQUIRE(qind);
        REQUIRE(qind->isFota());
        REQUIRE(qind->isFotaValid());
        REQUIRE(qind->getFotaStage() == at::urc::Qind::FotaStage::HTTPSTART);
        REQUIRE(qind->getFotaParameter().empty());

        urc  = at::urc::UrcFactory::Create("+QIND: \"FOTA\",\"HTTPEND\", 22");
        qind = getURC<at::urc::Qind>(urc);
        REQUIRE(qind);
        REQUIRE(qind->isFota());
        REQUIRE(qind->isFotaValid());
        REQUIRE(qind->getFotaStage() == at::urc::Qind::FotaStage::HTTPEND);
        REQUIRE(qind->getFotaParameter() == "22");

        urc  = at::urc::UrcFactory::Create("+QIND: \"FOTA\",\"START\"");
        qind = getURC<at::urc::Qind>(urc);
        REQUIRE(qind);
        REQUIRE(qind->isFota());
        REQUIRE(qind->isFotaValid());
        REQUIRE(qind->getFotaStage() == at::urc::Qind::FotaStage::START);
        REQUIRE(qind->getFotaParameter().empty());

        urc  = at::urc::UrcFactory::Create("+QIND: \"FOTA\",\"UPDATING\", 99");
        qind = getURC<at::urc::Qind>(urc);
        REQUIRE(qind);
        REQUIRE(qind->isFota());
        REQUIRE(qind->isFotaValid());
        REQUIRE(qind->getFotaStage() == at::urc::Qind::FotaStage::UPDATING);
        REQUIRE(qind->getFotaParameter() == "99");

        urc  = at::urc::UrcFactory::Create("+QIND: \"FOTA\",\"END\", 2");
        qind = getURC<at::urc::Qind>(urc);
        REQUIRE(qind);
        REQUIRE(qind->isFota());
        REQUIRE(qind->isFotaValid());
        REQUIRE(qind->getFotaStage() == at::urc::Qind::FotaStage::END);
        REQUIRE(qind->getFotaParameter() == "2");

        urc  = at::urc::UrcFactory::Create("+QIND: \"FOTA\",\"TEST\", 3");
        qind = getURC<at::urc::Qind>(urc);
        REQUIRE(qind);
        REQUIRE(qind->isFota());
        REQUIRE(qind->isFotaValid());
        REQUIRE_FALSE(qind->getFotaStage());
        REQUIRE(qind->getFotaParameter() == "3");
    }

    SECTION("FOTA dirty")
    {
        auto urc =
            at::urc::UrcFactory::Create("\n\r+QIND: \n\r\r \"FOTA\" ,\r \n \"HTTPEND\", \n\r     test test \r\n");
        auto qind = getURC<at::urc::Qind>(urc);
        REQUIRE(qind);
        REQUIRE(qind->isFota());
        REQUIRE(qind->isFotaValid());
        REQUIRE(qind->getFotaStage() == at::urc::Qind::FotaStage::HTTPEND);
        REQUIRE(qind->getFotaParameter() == "test test");
    }
}

TEST_CASE("+Cusd")
{
    SECTION("Cusd action needed")
    {
        auto urc  = at::urc::UrcFactory::Create("+CUSD:1,\"test msg\",14");
        auto cusd = getURC<at::urc::Cusd>(urc);

        REQUIRE(cusd);
        REQUIRE(cusd->isValid());
        REQUIRE(cusd->isActionNeeded());
        REQUIRE(*cusd->getMessage() == "test msg");
        REQUIRE(*cusd->getStatus() == at::urc::Cusd::StatusType::FurtherUserActionRequired);
        REQUIRE(*cusd->getDCS() == 14);
    }

    SECTION("Cusd action needed with white spaces")
    {
        auto urc  = at::urc::UrcFactory::Create("+CUSD: 0 , \"test msg\" , 15 ");
        auto cusd = getURC<at::urc::Cusd>(urc);
        REQUIRE(cusd);
        REQUIRE(cusd->isValid());
        REQUIRE_FALSE(cusd->isActionNeeded());
        REQUIRE(*cusd->getMessage() == "test msg");
        REQUIRE(*cusd->getStatus() == at::urc::Cusd::StatusType::NoFurtherUserActionRequired);
        REQUIRE(*cusd->getDCS() == 15);
    }

    SECTION("Cusd wrong status and DCS")
    {
        auto urc  = at::urc::UrcFactory::Create("+CUSD:100,\"test msg\", abc");
        auto cusd = getURC<at::urc::Cusd>(urc);
        REQUIRE(cusd);
        REQUIRE(cusd->isValid());
        REQUIRE_FALSE(cusd->isActionNeeded());
        REQUIRE(*cusd->getMessage() == "test msg");
        REQUIRE_FALSE(cusd->getStatus());
        REQUIRE_FALSE(cusd->getDCS());
    }

    SECTION("Cusd action not needed")
    {
        auto urc  = at::urc::UrcFactory::Create("+CUSD:2,\"test msg\",15");
        auto cusd = getURC<at::urc::Cusd>(urc);
        REQUIRE(cusd);
        REQUIRE(cusd->isValid());
        REQUIRE_FALSE(cusd->isActionNeeded());
    }

    SECTION("no Cusd")
    {
        auto urc  = at::urc::UrcFactory::Create("+CSUD:1,\"test msg\",15");
        auto cusd = getURC<at::urc::Cusd>(urc);
        REQUIRE_FALSE(cusd);
    }

    SECTION("too short")
    {
        auto urc  = at::urc::UrcFactory::Create("+CUSD:1,\"test msg\"");
        auto cusd = getURC<at::urc::Cusd>(urc);
        REQUIRE(cusd);
        REQUIRE_FALSE(cusd->isValid());
    }

    SECTION("too long")
    {
        auto urc  = at::urc::UrcFactory::Create("+CUSD:1,\"test msg\",15,15");
        auto cusd = getURC<at::urc::Cusd>(urc);
        REQUIRE(cusd);
        REQUIRE_FALSE(cusd->isValid());
    }
}

TEST_CASE("+Ctze")
{
    SECTION("Not valid data")
    {
        auto urc  = at::urc::UrcFactory::Create("+QIND:\"csq\"");
        auto ctze = getURC<at::urc::Ctze>(urc);
        REQUIRE_FALSE(ctze);
    }

    SECTION("ctze +08")
    {
        auto urc  = at::urc::UrcFactory::Create("+CTZE: \"+08\",1,\"2020/10/21,13:49:57\"");
        auto ctze = getURC<at::urc::Ctze>(urc);
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
        auto urc  = at::urc::UrcFactory::Create("+CTZE: \"-08\",1,\"2020/10/21,13:49:57\"");
        auto ctze = getURC<at::urc::Ctze>(urc);
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
        auto urc  = at::urc::UrcFactory::Create("+CTZE: \"-00\",0,\"2020/10/21,13:49:57\"");
        auto ctze = getURC<at::urc::Ctze>(urc);
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
        auto urc  = at::urc::UrcFactory::Create("+CTZE: \"-08\",\"2020/10/21,13:49:57\"");
        auto ctze = getURC<at::urc::Ctze>(urc);
        REQUIRE(ctze);
        REQUIRE_FALSE(ctze->isValid());
        // auto timeInfo = ctze->getTimeInfo();
    }
}

TEST_CASE("+Creg")
{
    SECTION("Creg short")
    {
        auto urc  = at::urc::UrcFactory::Create("+CREG: 0");
        auto creg = getURC<at::urc::Creg>(urc);
        REQUIRE(creg);
        REQUIRE(creg->isValid());
        REQUIRE(creg->isShort());
        REQUIRE_FALSE(creg->isExtended());
        REQUIRE(creg->getStatus() == Store::Network::Status::NotRegistered);
        REQUIRE_FALSE(creg->getLocation());
        REQUIRE_FALSE(creg->getCellId());
        REQUIRE(creg->getAccessTechnology() == Store::Network::AccessTechnology::Unknown);
    }

    SECTION("Creg extended")
    {
        auto urc  = at::urc::UrcFactory::Create("+CREG: 1,\"D509\",\"80D413D\",7");
        auto creg = getURC<at::urc::Creg>(urc);
        REQUIRE(creg);
        REQUIRE(creg->isValid());
        REQUIRE_FALSE(creg->isShort());
        REQUIRE(creg->isExtended());
        REQUIRE(creg->getStatus() == Store::Network::Status::RegisteredHomeNetwork);
        REQUIRE(*creg->getLocation() == "D509");
        REQUIRE(*creg->getCellId() == "80D413D");
        REQUIRE(creg->getAccessTechnology() == Store::Network::AccessTechnology::EUtran);
    }

    SECTION("Creg extended access technology")
    {
        {
            auto urc  = at::urc::UrcFactory::Create("+CREG: 1,\"D509\",\"80D413D\",0");
            auto creg = getURC<at::urc::Creg>(urc);
            REQUIRE(creg->getAccessTechnology() == Store::Network::AccessTechnology::Gsm);
        }
        {
            auto urc  = at::urc::UrcFactory::Create("+CREG: 1,\"D509\",\"80D413D\",1");
            auto creg = getURC<at::urc::Creg>(urc);
            REQUIRE(creg->getAccessTechnology() == Store::Network::AccessTechnology::Unknown);
        }
        {
            auto urc  = at::urc::UrcFactory::Create("+CREG: 1,\"D509\",\"80D413D\",7");
            auto creg = getURC<at::urc::Creg>(urc);
            REQUIRE(creg->getAccessTechnology() == Store::Network::AccessTechnology::EUtran);
        }
        {
            auto urc  = at::urc::UrcFactory::Create("+CREG: 1,\"D509\",\"80D413D\",8");
            auto creg = getURC<at::urc::Creg>(urc);
            REQUIRE(creg->getAccessTechnology() == Store::Network::AccessTechnology::Unknown);
        }
        {
            auto urc  = at::urc::UrcFactory::Create("+CREG: 1,\"D509\",\"80D413D\",ABX");
            auto creg = getURC<at::urc::Creg>(urc);
            REQUIRE(creg->getAccessTechnology() == Store::Network::AccessTechnology::Unknown);
        }
    }

    SECTION("Creg status")
    {
        {
            auto urc  = at::urc::UrcFactory::Create("+CREG: 0,\"D509\",\"80D413D\",0");
            auto creg = getURC<at::urc::Creg>(urc);
            REQUIRE(creg->getStatus() == Store::Network::Status::NotRegistered);
        }
        {
            auto urc  = at::urc::UrcFactory::Create("+CREG: 5,\"D509\",\"80D413D\",0");
            auto creg = getURC<at::urc::Creg>(urc);
            REQUIRE(creg->getStatus() == Store::Network::Status::RegisteredRoaming);
        }
        {
            auto urc  = at::urc::UrcFactory::Create("+CREG: 4,\"D509\",\"80D413D\",0");
            auto creg = getURC<at::urc::Creg>(urc);
            REQUIRE(creg->getStatus() == Store::Network::Status::Unknown);
        }
        {
            auto urc  = at::urc::UrcFactory::Create("+CREG: 6,\"D509\",\"80D413D\",0");
            auto creg = getURC<at::urc::Creg>(urc);
            REQUIRE(creg->getStatus() == Store::Network::Status::Unknown);
        }
        {
            auto urc  = at::urc::UrcFactory::Create("+CREG: A,\"D509\",\"80D413D\",0");
            auto creg = getURC<at::urc::Creg>(urc);
            REQUIRE(creg->getStatus() == Store::Network::Status::Unknown);
        }
    }

    SECTION("Creg no Creg")
    {
        auto urc  = at::urc::UrcFactory::Create("+CEGR: 0");
        auto creg = getURC<at::urc::Creg>(urc);
        REQUIRE_FALSE(creg);
    }

    SECTION("Creg too short")
    {
        auto urc  = at::urc::UrcFactory::Create("+CREG:");
        auto creg = getURC<at::urc::Creg>(urc);
        REQUIRE(creg);
        REQUIRE_FALSE(creg->isValid());
    }

    SECTION("Creg too long")
    {
        auto urc  = at::urc::UrcFactory::Create("+CREG: 0,\"D509\",\"80D413D\",0,1");
        auto creg = getURC<at::urc::Creg>(urc);
        REQUIRE(creg);
        REQUIRE_FALSE(creg->isValid());
    }

    SECTION("Creg wrong length")
    {
        auto urc  = at::urc::UrcFactory::Create("+CREG 0,\"D509\"");
        auto creg = getURC<at::urc::Creg>(urc);
        REQUIRE(creg);
        REQUIRE_FALSE(creg->isValid());
    }
}

TEST_CASE("+Cmti")
{
    SECTION("Cmti too short")
    {
        auto urc  = at::urc::UrcFactory::Create("+CMTI: \"ME\"");
        auto cmti = getURC<at::urc::Cmti>(urc);
        REQUIRE(cmti);
        REQUIRE_FALSE(cmti->isValid());
    }

    SECTION("Cmti too long")
    {
        auto urc  = at::urc::UrcFactory::Create("+CMTI: \"ME\",1,1");
        auto cmti = getURC<at::urc::Cmti>(urc);
        REQUIRE(cmti);
        REQUIRE_FALSE(cmti->isValid());
    }

    SECTION("Cmti valid")
    {
        auto urc  = at::urc::UrcFactory::Create("+CMTI: \"ME\",1");
        auto cmti = getURC<at::urc::Cmti>(urc);
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
        auto urc  = at::urc::UrcFactory::Create("+CLIP: \"+48123456789\"");
        auto clip = getURC<at::urc::Clip>(urc);
        REQUIRE(clip);
        REQUIRE_FALSE(clip->isValid());
    }

    SECTION("CLIP valid")
    {
        // two parameters
        auto urc  = at::urc::UrcFactory::Create("+CLIP: \"+48123456789\",145");
        auto clip = getURC<at::urc::Clip>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());

        // three parameters
        urc  = at::urc::UrcFactory::Create("+CLIP: \"+48123456789\",145,");
        clip = getURC<at::urc::Clip>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE_FALSE(clip->getSubaddr());
        REQUIRE_FALSE(clip->getAlpha());
        REQUIRE_FALSE(clip->getSatype());
        REQUIRE_FALSE(clip->getCLIValidity());

        // four parameters
        urc  = at::urc::UrcFactory::Create("+CLIP: \"+48123456789\",145,,");
        clip = getURC<at::urc::Clip>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE_FALSE(clip->getSubaddr());
        REQUIRE_FALSE(clip->getAlpha());
        REQUIRE_FALSE(clip->getSatype());
        REQUIRE_FALSE(clip->getCLIValidity());

        // six parameters
        urc  = at::urc::UrcFactory::Create("+CLIP: \"+48123456789\",145,,,");
        clip = getURC<at::urc::Clip>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE_FALSE(clip->getSubaddr());
        REQUIRE_FALSE(clip->getAlpha());
        REQUIRE_FALSE(clip->getSatype());
        REQUIRE_FALSE(clip->getCLIValidity());

        // seven parameters
        urc  = at::urc::UrcFactory::Create("+CLIP: \"+48123456789\",145,,,,");
        clip = getURC<at::urc::Clip>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE_FALSE(clip->getSubaddr());
        REQUIRE_FALSE(clip->getAlpha());
        REQUIRE_FALSE(clip->getSatype());
        REQUIRE_FALSE(clip->getCLIValidity());

        // two parameters with validation not empty
        urc  = at::urc::UrcFactory::Create("+CLIP: \"+48123456789\",145,,,,0");
        clip = getURC<at::urc::Clip>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE_FALSE(clip->getSubaddr());
        REQUIRE_FALSE(clip->getAlpha());
        REQUIRE_FALSE(clip->getSatype());
        REQUIRE(clip->getCLIValidity() == "0");
    }

    SECTION("CLIP phone number")
    {
        auto urc  = at::urc::UrcFactory::Create("+CLIP: \"+48123456789\",145,,,,0");
        auto clip = getURC<at::urc::Clip>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE(clip->getNumber() == "+48123456789");

        urc  = at::urc::UrcFactory::Create("+CLIP: \"\",145,,,,0");
        clip = getURC<at::urc::Clip>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE(clip->getNumber() == "");

        urc  = at::urc::UrcFactory::Create("+CLIP: ,145,,,,0");
        clip = getURC<at::urc::Clip>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE(clip->getNumber() == "");
    }

    SECTION("CLIP address type")
    {
        auto urc  = at::urc::UrcFactory::Create("+CLIP: \"+48123456789\",,,,,0");
        auto clip = getURC<at::urc::Clip>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE_FALSE(clip->getType());

        urc  = at::urc::UrcFactory::Create("+CLIP: \"+48123456789\",577,,,,0");
        clip = getURC<at::urc::Clip>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE_FALSE(clip->getType());

        urc  = at::urc::UrcFactory::Create("+CLIP: \"+48123456789\",\"test\",,,,0");
        clip = getURC<at::urc::Clip>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE_FALSE(clip->getType());

        urc  = at::urc::UrcFactory::Create("+CLIP: \"+48123456789\",test,,,,0");
        clip = getURC<at::urc::Clip>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE_FALSE(clip->getType());

        urc  = at::urc::UrcFactory::Create("+CLIP: \"+48123456789\",129,,,,0");
        clip = getURC<at::urc::Clip>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE(clip->getType() == at::urc::Clip::AddressType::UnknownType);

        urc  = at::urc::UrcFactory::Create("+CLIP: \"+48123456789\",145,,,,0");
        clip = getURC<at::urc::Clip>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE(clip->getType() == at::urc::Clip::AddressType::InternationalNumber);

        urc  = at::urc::UrcFactory::Create("+CLIP: \"+48123456789\",161,,,,0");
        clip = getURC<at::urc::Clip>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE(clip->getType() == at::urc::Clip::AddressType::NationalNumber);
    }

    SECTION("CLIP optional parameters")
    {
        auto urc  = at::urc::UrcFactory::Create("+CLIP: \"+48123456789\",145,,,,");
        auto clip = getURC<at::urc::Clip>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE_FALSE(clip->getAlpha());
        REQUIRE_FALSE(clip->getSatype());
        REQUIRE_FALSE(clip->getSubaddr());
        REQUIRE_FALSE(clip->getCLIValidity());

        urc  = at::urc::UrcFactory::Create("+CLIP: \"+48123456789\",145,1,2,3,0");
        clip = getURC<at::urc::Clip>(urc);
        REQUIRE(clip);
        REQUIRE(clip->isValid());
        REQUIRE(clip->getAlpha() == "3");
        REQUIRE(clip->getSatype() == "2");
        REQUIRE(clip->getSubaddr() == "1");
        REQUIRE(clip->getCLIValidity() == "0");
    }
}

TEST_CASE("+CPIN")
{
    SECTION("CPIN supported")
    {
        auto urc  = at::urc::UrcFactory::Create("+CPIN: SIM PIN");
        auto cpin = getURC<at::urc::Cpin>(urc);
        REQUIRE(cpin);
        REQUIRE(cpin->isValid());
        REQUIRE(cpin->getState() == at::SimState::SimPin);
        REQUIRE(cpin->getMessage() == "SIM PIN");
    }
    SECTION("CPIN not supported")
    {
        auto urc  = at::urc::UrcFactory::Create("+CPIN: \"Unknown\"");
        auto cpin = getURC<at::urc::Cpin>(urc);
        REQUIRE(cpin);
        REQUIRE(cpin->getState() == at::SimState::Unknown);
    }
}

TEST_CASE("POWERED DOWN")
{
    SECTION("POWERED DOWN valid")
    {
        auto urc = at::urc::UrcFactory::Create("POWERED DOWN");
        auto pwd = getURC<at::urc::PoweredDown>(urc);
        REQUIRE(pwd);
        REQUIRE(pwd->isValid());
        REQUIRE(pwd->isImmediatePowerDown());
        REQUIRE_FALSE(pwd->isNormalPowerDown());

        urc = at::urc::UrcFactory::Create("NORMAL POWER DOWN");
        pwd = getURC<at::urc::PoweredDown>(urc);
        REQUIRE(pwd);
        REQUIRE(pwd->isValid());
        REQUIRE_FALSE(pwd->isImmediatePowerDown());
        REQUIRE(pwd->isNormalPowerDown());

        // dirty
        urc = at::urc::UrcFactory::Create("\n\r POWERED DOWN\n\r ");
        pwd = getURC<at::urc::PoweredDown>(urc);
        REQUIRE(pwd);
        REQUIRE(pwd->isValid());
        REQUIRE(pwd->isImmediatePowerDown());
        REQUIRE_FALSE(pwd->isNormalPowerDown());

        urc = at::urc::UrcFactory::Create(" \n\r  NORMAL POWER DOWN \n\r ");
        pwd = getURC<at::urc::PoweredDown>(urc);
        REQUIRE(pwd);
        REQUIRE(pwd->isValid());
        REQUIRE_FALSE(pwd->isImmediatePowerDown());
        REQUIRE(pwd->isNormalPowerDown());
    }

    SECTION("POWERED DOWN invalid")
    {
        auto pwd = at::urc::PoweredDown("TEST");
        REQUIRE_FALSE(pwd.isValid());
        REQUIRE_FALSE(pwd.isImmediatePowerDown());
        REQUIRE_FALSE(pwd.isNormalPowerDown());
    }
}

TEST_CASE("Urc RESPONSE")
{
    SECTION("Urc RESPONSE valid")
    {
        auto urc = at::urc::UrcFactory::Create("OK");
        auto rsp = getURC<at::urc::UrcResponse>(urc);
        REQUIRE(rsp);
        REQUIRE(rsp->getURCResponseType() == at::urc::UrcResponse::URCResponseType::Ok);

        urc = at::urc::UrcFactory::Create("CONNECT");
        rsp = getURC<at::urc::UrcResponse>(urc);
        REQUIRE(rsp);
        REQUIRE(rsp->getURCResponseType() == at::urc::UrcResponse::URCResponseType::Connect);

        urc = at::urc::UrcFactory::Create("NO CARRIER");
        rsp = getURC<at::urc::UrcResponse>(urc);
        REQUIRE(rsp);
        REQUIRE(rsp->getURCResponseType() == at::urc::UrcResponse::URCResponseType::NoCarrier);

        urc = at::urc::UrcFactory::Create("ERROR");
        rsp = getURC<at::urc::UrcResponse>(urc);
        REQUIRE(rsp);
        REQUIRE(rsp->getURCResponseType() == at::urc::UrcResponse::URCResponseType::Error);

        urc = at::urc::UrcFactory::Create("NO DIALTONE");
        rsp = getURC<at::urc::UrcResponse>(urc);
        REQUIRE(rsp);
        REQUIRE(rsp->getURCResponseType() == at::urc::UrcResponse::URCResponseType::NoDialtone);

        urc = at::urc::UrcFactory::Create("BUSY");
        rsp = getURC<at::urc::UrcResponse>(urc);
        REQUIRE(rsp);
        REQUIRE(rsp->getURCResponseType() == at::urc::UrcResponse::URCResponseType::Busy);

        urc = at::urc::UrcFactory::Create("NO ANSWER");
        rsp = getURC<at::urc::UrcResponse>(urc);
        REQUIRE(rsp);
        REQUIRE(rsp->getURCResponseType() == at::urc::UrcResponse::URCResponseType::NoAnswer);
    }

    SECTION("Urc RESPONSE dirty")
    {
        auto urc = at::urc::UrcFactory::Create("\n\n \rNO ANSWER\n\r\n");
        auto rsp = getURC<at::urc::UrcResponse>(urc);
        REQUIRE(rsp);
        REQUIRE(rsp->getURCResponseType() == at::urc::UrcResponse::URCResponseType::NoAnswer);
    }
}

TEST_CASE("+Qind: SMS DONE")
{
    SECTION("SMS DONE")
    {
        // SMS DONE normal
        auto urc  = at::urc::UrcFactory::Create("+QIND: SMS DONE");
        auto qind = getURC<at::urc::Qind>(urc);
        REQUIRE(qind);
        REQUIRE(qind->isSmsDone());
        // SMS DONE dirty
        urc  = at::urc::UrcFactory::Create("\r\r\n\n+QIND: SMS DONE\n\n\r\r");
        qind = getURC<at::urc::Qind>(urc);
        REQUIRE(qind);
        REQUIRE(qind->isSmsDone());
    }
}

TEST_CASE("+Qiurc: TCP Context and connection message")
{
    SECTION("PDP Context deactivate - normal message")
    {
        /// +QIURC:"pdpdeact",<contextID>

        auto urc   = at::urc::UrcFactory::Create("+QIURC: \"pdpdeact\",1");
        auto qiurc = getURC<at::urc::Qiurc>(urc);

        REQUIRE(qiurc);
        REQUIRE(qiurc->getType());
        REQUIRE(qiurc->isValid());
        REQUIRE(*qiurc->getType() == at::urc::Qiurc::QIUrcMessages::DeactivateContext);
        REQUIRE(*qiurc->getFirstParam() == "1");
    }

    SECTION("PDP Context deactivate - corrupted, but OK format")
    {
        auto urc   = at::urc::UrcFactory::Create("+QIURC:\"pdpdeactwww\",1");
        auto qiurc = getURC<at::urc::Qiurc>(urc);
        REQUIRE(qiurc);
        REQUIRE(qiurc->getType() == std::nullopt);
    }

    SECTION("PDP Context deactivate - wrong param count (in case simple implementation)")
    {
        auto urc   = at::urc::UrcFactory::Create("+QIURC:\"pdpdeactwww\",1,3");
        auto qiurc = getURC<at::urc::Qiurc>(urc);
        REQUIRE(qiurc->getType() == std::nullopt);
    }
}

TEST_CASE("RING")
{
    struct RingTest
    {
        std::string body;
        std::optional<at::urc::Ring::RingType> ringType;
    };

    std::vector<RingTest> testCases = {
        {"RING", at::urc::Ring::Normal},
        {"+CRING: ASYNC", at::urc::Ring::Async},
        {"+CRING: SYNC", at::urc::Ring::Sync},
        {"+CRING: REL ASYNC", at::urc::Ring::RelAsync},
        {"\r\n+CRING: REL SYNC", at::urc::Ring::RelSync},
        {"+CRING: FAX", at::urc::Ring::Fax},
        {"+CRING: VOICE", at::urc::Ring::Voice},
        {"+CRING: BUMMER", std::nullopt},
    };

    for (auto &test : testCases) {
        auto urc  = at::urc::UrcFactory::Create(test.body);
        auto ring = getURC<at::urc::Ring>(urc);
        REQUIRE(ring);
        if (test.ringType) {
            REQUIRE(ring->getType());
            REQUIRE(ring->getType().value() == test.ringType);
        }
        else {
            REQUIRE(!ring->isValid());
            REQUIRE(!ring->getType());
        }
    }
}
