// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <response.hpp>
#include <Result.hpp>

TEST_CASE("Response COPS")
{

    SECTION("OK COPS")
    {
        at::Result resp;
        resp.code = at::Result::Code::OK;
        std::vector<at::response::cops::Operator> ret;

        resp.response.push_back("+COPS: "
                                "(2,\"PLAY\",\"PLAY\",\"26006\",2),(1,\"Play\",\"Play\",\"26006\",0),(1,\"Plus\","
                                "\"PLUS\",\"26001\",2),(1,\"Plus\",\"PLUS\",\"26001\",0),(1,\"Orange "
                                "PL\",\"Orange\",\"26003\",0),(1,\"T-Mobile.pl\",\"TM PL\",\"26002\",0),(1,\"Orange "
                                "PL\",\"Orange\",\"26003\",2),(1,\"T-Mobile.pl\",\"TM PL\",\"26002\",2),,(0-4),(0-2)");
        REQUIRE(resp.code == at::Result::Code::OK);
        REQUIRE(at::response::parseCOPS(resp, ret) == true);
        REQUIRE(ret.size() == 8);
        REQUIRE(ret[0].shortName == "PLAY");
        REQUIRE(ret[0].numericName == "26006");
        REQUIRE(ret[0].status == at::response::cops::OperatorStatus::Current);
        REQUIRE(ret[0].technology == at::response::cops::AccessTechnology::UTRAN);
    }

    SECTION("COPS Without params")
    {
        at::Result resp;
        resp.code = at::Result::Code::OK;
        std::vector<at::response::cops::Operator> ret;

        resp.response.push_back(
            "+COPS: "
            "(2,\"PLAY\",\"PLAY\",\"26006\",2),(1,\"Play\",\"Play\",\"26006\",0),(1,\"Plus\",\"PLUS\",\"26001\",2),(1,"
            "\"Plus\",\"PLUS\",\"26001\",0),(1,\"Orange PL\",\"Orange\",\"26003\",0),(1,\"T-Mobile.pl\",\"TM "
            "PL\",\"26002\",0),(1,\"Orange PL\",\"Orange\",\"26003\",2),(1,\"T-Mobile.pl\",\"TM PL\",\"26002\",2)");
        REQUIRE(resp.code == at::Result::Code::OK);
        REQUIRE(at::response::parseCOPS(resp, ret) == true);
        REQUIRE(ret.size() == 8);
        REQUIRE(ret[0].shortName == "PLAY");
        REQUIRE(ret[0].numericName == "26006");
        REQUIRE(ret[0].status == at::response::cops::OperatorStatus::Current);
        REQUIRE(ret[0].technology == at::response::cops::AccessTechnology::UTRAN);
    }

    SECTION("COPS without optional ACT")
    {
        at::Result resp;
        resp.code = at::Result::Code::OK;
        std::vector<at::response::cops::Operator> ret;

        resp.response.push_back("+COPS: (2,\"PLAY\",\"PLAY\",\"26006\")");
        REQUIRE(resp.code == at::Result::Code::OK);
        REQUIRE(at::response::parseCOPS(resp, ret) == true);
        REQUIRE(ret[0].shortName == "PLAY");
        REQUIRE(ret[0].numericName == "26006");
        REQUIRE(ret[0].status == at::response::cops::OperatorStatus::Current);
    }

    SECTION("COPS Name LONG SHORT")
    {
        at::Result resp;
        resp.code = at::Result::Code::OK;
        std::vector<at::response::cops::Operator> ret;

        resp.response.push_back("+COPS: (2,\"LONG\",\"SHORT\",\"26006\")");
        REQUIRE(resp.code == at::Result::Code::OK);
        REQUIRE(at::response::parseCOPS(resp, ret) == true);
        REQUIRE(ret[0].shortName == "SHORT");
        REQUIRE(ret[0].longName == "LONG");
        REQUIRE(ret[0].numericName == "26006");
        REQUIRE(ret[0].status == at::response::cops::OperatorStatus::Current);
    }

    SECTION("COPS not enought op parameters")
    {
        at::Result resp;
        resp.code = at::Result::Code::OK;
        std::vector<at::response::cops::Operator> ret;
        resp.response.push_back("+COPS: (2,\"PLAY\",\"PLAY\")");
        REQUIRE(at::response::parseCOPS(resp, ret) == false);
    }

    SECTION("COPS not wrong (")
    {
        at::Result resp;
        resp.code = at::Result::Code::OK;
        std::vector<at::response::cops::Operator> ret;
        resp.response.push_back("+COPS: (2,\"PLAY\",\"PLAY\", 4)(");
        REQUIRE(at::response::parseCOPS(resp, ret) == false);
    }

    SECTION("OK COPS? - return operator")
    {
        at::Result resp;
        resp.code = at::Result::Code::OK;
        at::response::cops::CurrentOperatorInfo ret;
        resp.response.push_back("+COPS: 0,0,\"PLAY\",2");
        resp.response.push_back("OK");
        REQUIRE(resp.code == at::Result::Code::OK);
        REQUIRE(at::response::parseCOPS(resp, ret) == true);
        REQUIRE(ret.getOperator());
        REQUIRE(ret.getMode() == at::response::cops::CopsMode::Automatic);
        REQUIRE(ret.getFormat() == at::response::cops::NameFormat::Long);
        auto op = *ret.getOperator();
        REQUIRE(op.longName == "PLAY");
        REQUIRE(op.technology == at::response::cops::AccessTechnology::UTRAN);
    }

    SECTION("OK COPS? - return operator no act")
    {
        at::Result resp;
        resp.code = at::Result::Code::OK;
        at::response::cops::CurrentOperatorInfo ret;
        resp.response.push_back("+COPS: 0,0,\"PLAY\"");
        resp.response.push_back("OK");
        REQUIRE(resp.code == at::Result::Code::OK);
        REQUIRE(at::response::parseCOPS(resp, ret) == true);
        REQUIRE(ret.getOperator());
        REQUIRE(ret.getMode() == at::response::cops::CopsMode::Automatic);
        REQUIRE(ret.getFormat() == at::response::cops::NameFormat::Long);
        auto op = *ret.getOperator();
        REQUIRE(op.longName == "PLAY");
    }

    SECTION("OK COPS? - no operator")
    {
        at::Result resp;
        resp.code = at::Result::Code::OK;
        at::response::cops::CurrentOperatorInfo ret;
        resp.response.push_back("+COPS: 0");
        resp.response.push_back("OK");
        REQUIRE(resp.code == at::Result::Code::OK);
        REQUIRE(at::response::parseCOPS(resp, ret) == true);
        REQUIRE(ret.getMode() == at::response::cops::CopsMode::Automatic);
    }
    SECTION("WRONG COPS? - to many")
    {
        at::Result resp;
        resp.code = at::Result::Code::OK;
        at::response::cops::CurrentOperatorInfo ret;
        resp.response.push_back("+COPS: 0,0,\"PLAY\",2, 3");
        resp.response.push_back("OK");
        REQUIRE(resp.code == at::Result::Code::OK);
        REQUIRE(at::response::parseCOPS(resp, ret) == false);
    }
}
TEST_CASE("Response CLIR")
{
    SECTION("OK CLIR? - according to subscription, not provisioned")
    {
        at::Result resp;
        resp.code = at::Result::Code::OK;
        resp.response.push_back("+CLIR: 0,0");
        resp.response.push_back("OK");
        REQUIRE(resp.code == at::Result::Code::OK);
        auto response = at::response::clir::parse(resp.response[0]);
        REQUIRE(response != std::nullopt);
        REQUIRE(response->serviceState == at::response::clir::ServiceState::AccordingToSubscription);
        REQUIRE(response->serviceStatus == at::response::clir::ServiceStatus::NotProvisioned);
    }

    SECTION("OK CLIR? - disabled, temporary allowed")
    {
        at::Result resp;
        resp.code = at::Result::Code::OK;
        resp.response.push_back("+CLIR: 2,4");
        resp.response.push_back("OK");
        REQUIRE(resp.code == at::Result::Code::OK);
        auto response = at::response::clir::parse(resp.response[0]);
        REQUIRE(response != std::nullopt);
        REQUIRE(response->serviceState == at::response::clir::ServiceState::ServiceDisabled);
        REQUIRE(response->serviceStatus == at::response::clir::ServiceStatus::TemporaryAllowed);
    }

    SECTION("WRONG CLIR? - invalid service state")
    {
        at::Result resp;
        resp.code = at::Result::Code::OK;
        resp.response.push_back("+CLIR: 6,4");
        resp.response.push_back("OK");
        REQUIRE(resp.code == at::Result::Code::OK);
        auto response = at::response::clir::parse(resp.response[0]);
        REQUIRE(response == std::nullopt);
    }

    SECTION("WRONG CLIR? - invalid service status")
    {
        at::Result resp;
        resp.code = at::Result::Code::OK;
        resp.response.push_back("+CLIR: 1,99");
        resp.response.push_back("OK");
        REQUIRE(resp.code == at::Result::Code::OK);
        auto response = at::response::clir::parse(resp.response[0]);
        REQUIRE(response == std::nullopt);
    }

    SECTION("WRONG CLIR? - to many toknes")
    {
        at::Result resp;
        resp.code = at::Result::Code::OK;
        resp.response.push_back("+CLIR: 1,4,6");
        resp.response.push_back("OK");
        REQUIRE(resp.code == at::Result::Code::OK);
        auto response = at::response::clir::parse(resp.response[0]);
        REQUIRE(response == std::nullopt);
    }

    SECTION("WRONG CLIR? - to little tokens")
    {
        at::Result resp;
        resp.code = at::Result::Code::OK;
        resp.response.push_back("+CLIR: 1");
        resp.response.push_back("OK");
        REQUIRE(resp.code == at::Result::Code::OK);
        auto response = at::response::clir::parse(resp.response[0]);
        REQUIRE(response == std::nullopt);
    }

    SECTION("WRONG CLIR? - invalid token")
    {
        at::Result resp;
        resp.code = at::Result::Code::OK;
        resp.response.push_back("+CLI: 1,1");
        resp.response.push_back("OK");
        REQUIRE(resp.code == at::Result::Code::OK);
        auto response = at::response::clir::parse(resp.response[0]);
        REQUIRE(response == std::nullopt);
    }
}

TEST_CASE("Response CLIP")
{
    SECTION("OK CLIP? - clip provisioned, display urc")
    {
        at::Result resp;
        resp.code = at::Result::Code::OK;
        resp.response.push_back("+CLIP: 1,1");
        resp.response.push_back("OK");
        REQUIRE(resp.code == at::Result::Code::OK);
        auto response = at::response::clip::parse(resp.response[0]);
        REQUIRE(response != std::nullopt);
        REQUIRE(response->clipState == at::response::clip::ClipState::Provisioned);
        REQUIRE(response->urcState == at::response::clip::UrcState::DisplayUrc);
    }

    SECTION("OK CLIP? - clip not provisioned, supress urc")
    {
        at::Result resp;
        resp.code = at::Result::Code::OK;
        resp.response.push_back("+CLIP: 0,0");
        resp.response.push_back("OK");
        REQUIRE(resp.code == at::Result::Code::OK);
        auto response = at::response::clip::parse(resp.response[0]);
        REQUIRE(response != std::nullopt);
        REQUIRE(response->clipState == at::response::clip::ClipState::NotProvisioned);
        REQUIRE(response->urcState == at::response::clip::UrcState::SupressUrc);
    }

    SECTION("WRONG CLIP? - clip invalid clip state")
    {
        at::Result resp;
        resp.code = at::Result::Code::OK;
        resp.response.push_back("+CLIP: 1,99");
        resp.response.push_back("OK");
        REQUIRE(resp.code == at::Result::Code::OK);
        auto response = at::response::clip::parse(resp.response[0]);
        REQUIRE(response == std::nullopt);
    }

    SECTION("WRONG CLIP? - clip urc state")
    {
        at::Result resp;
        resp.code = at::Result::Code::OK;
        resp.response.push_back("+CLIP: 77,1");
        resp.response.push_back("OK");
        REQUIRE(resp.code == at::Result::Code::OK);
        auto response = at::response::clip::parse(resp.response[0]);
        REQUIRE(response == std::nullopt);
    }

    SECTION("WRONG CLIP? - to many tokens")
    {
        at::Result resp;
        resp.code = at::Result::Code::OK;
        resp.response.push_back("+CLIP: 1,1,3");
        resp.response.push_back("OK");
        REQUIRE(resp.code == at::Result::Code::OK);
        auto response = at::response::clip::parse(resp.response[0]);
        REQUIRE(response == std::nullopt);
    }

    SECTION("WRONG CLIP? - to little tokens")
    {
        at::Result resp;
        resp.code = at::Result::Code::OK;
        resp.response.push_back("+CLIP: 0");
        resp.response.push_back("OK");
        REQUIRE(resp.code == at::Result::Code::OK);
        auto response = at::response::clip::parse(resp.response[0]);
        REQUIRE(response == std::nullopt);
    }
    SECTION("WRONG CLIP? - invalid token")
    {
        at::Result resp;
        resp.code = at::Result::Code::OK;
        resp.response.push_back("+CLI: 1,1");
        resp.response.push_back("OK");
        REQUIRE(resp.code == at::Result::Code::OK);
        auto response = at::response::clip::parse(resp.response[0]);
        REQUIRE(response == std::nullopt);
    }
}

TEST_CASE("Response CLCK")
{
    SECTION("OK CLCK QUERY - all disabled")
    {

        at::Result resp;
        resp.code = at::Result::Code::OK;
        std::vector<at::response::clck::ClckParsed> ret;
        resp.response.push_back("+CLCK: 0,255");
        resp.response.push_back("OK");

        REQUIRE(resp.code == at::Result::Code::OK);
        REQUIRE(at::response::clck::parseQueryResponse(resp.response, ret) == true);
        REQUIRE(ret.size() == 1);
        REQUIRE(ret[0].status == at::response::clck::Status::Disable);
        REQUIRE(ret[0].serviceClass == at::response::mmi::ServiceClass::AllDisabled);
    }

    SECTION("OK CLCK QUERY - voice enabled")
    {

        at::Result resp;
        resp.code = at::Result::Code::OK;
        std::vector<at::response::clck::ClckParsed> ret;
        resp.response.push_back("+CLCK: 1,1");
        resp.response.push_back("OK");

        REQUIRE(resp.code == at::Result::Code::OK);
        REQUIRE(at::response::clck::parseQueryResponse(resp.response, ret) == true);
        REQUIRE(ret.size() == 1);
        REQUIRE(ret[0].status == at::response::clck::Status::Enable);
        REQUIRE(ret[0].serviceClass == at::response::mmi::ServiceClass::Voice);
    }

    SECTION("OK CLCK QUERY - voice, fax enabled")
    {

        at::Result resp;
        resp.code = at::Result::Code::OK;
        std::vector<at::response::clck::ClckParsed> ret;
        resp.response.push_back("+CLCK: 1,1");
        resp.response.push_back("+CLCK: 1,4");
        resp.response.push_back("OK");

        REQUIRE(resp.code == at::Result::Code::OK);
        REQUIRE(at::response::clck::parseQueryResponse(resp.response, ret) == true);
        REQUIRE(ret.size() == 2);

        REQUIRE(ret[0].status == at::response::clck::Status::Enable);
        REQUIRE(ret[0].serviceClass == at::response::mmi::ServiceClass::Voice);

        REQUIRE(ret[1].status == at::response::clck::Status::Enable);
        REQUIRE(ret[1].serviceClass == at::response::mmi::ServiceClass::Fax);
    }

    SECTION("WRONG CLCK QUERY - invalid status")
    {

        at::Result resp;
        resp.code = at::Result::Code::OK;
        std::vector<at::response::clck::ClckParsed> ret;
        resp.response.push_back("+CLCK: 8,1");
        resp.response.push_back("+CLCK: 1,4");
        resp.response.push_back("OK");

        REQUIRE(resp.code == at::Result::Code::OK);
        REQUIRE(at::response::clck::parseQueryResponse(resp.response, ret) == false);
        REQUIRE(ret.size() == 0);
    }

    SECTION("WRONG CLCK QUERY - invalid class")
    {

        at::Result resp;
        resp.code = at::Result::Code::OK;
        std::vector<at::response::clck::ClckParsed> ret;
        resp.response.push_back("+CLCK: 1,1");
        resp.response.push_back("+CLCK: 1,99");
        resp.response.push_back("OK");

        REQUIRE(resp.code == at::Result::Code::OK);
        REQUIRE(at::response::clck::parseQueryResponse(resp.response, ret) == false);
        REQUIRE(ret.size() == 0);
    }

    SECTION("WRONG CLCK QUERY - too short")
    {

        at::Result resp;
        resp.code = at::Result::Code::OK;
        std::vector<at::response::clck::ClckParsed> ret;
        resp.response.push_back("+CLCK: 1");
        resp.response.push_back("+CLCK: 1,4");
        resp.response.push_back("OK");

        REQUIRE(resp.code == at::Result::Code::OK);
        REQUIRE(at::response::clck::parseQueryResponse(resp.response, ret) == false);
        REQUIRE(ret.size() == 0);
    }

    SECTION("WRONG CLCK QUERY - invalid response token")
    {

        at::Result resp;
        resp.code = at::Result::Code::OK;
        std::vector<at::response::clck::ClckParsed> ret;
        resp.response.push_back("+CLC: 1,1");
        resp.response.push_back("+CLCK: 1,4");
        resp.response.push_back("OK");

        REQUIRE(resp.code == at::Result::Code::OK);
        REQUIRE(at::response::clck::parseQueryResponse(resp.response, ret) == false);
        REQUIRE(ret.size() == 0);
    }
}

TEST_CASE("Response CCWA?")
{
    SECTION("OK CCWA?  - all disabled")
    {

        at::Result resp;
        resp.code = at::Result::Code::OK;
        std::vector<at::response::ccwa::CcwaParsed> ret;
        resp.response.push_back("+CCWA: 0,255");
        resp.response.push_back("OK");

        REQUIRE(resp.code == at::Result::Code::OK);
        REQUIRE(at::response::ccwa::parse(resp.response, ret) == true);
        REQUIRE(ret.size() == 1);
        REQUIRE(ret[0].status == at::response::ccwa::Status::Disable);
        REQUIRE(ret[0].serviceClass == at::response::ccwa::ServiceClass::AllDisabled);
    }

    SECTION("OK CCWA?  - voice, fax enabled")
    {

        at::Result resp;
        resp.code = at::Result::Code::OK;
        std::vector<at::response::ccwa::CcwaParsed> ret;
        resp.response.push_back("+CCWA: 1,1");
        resp.response.push_back("+CCWA: 1,4");
        resp.response.push_back("OK");

        REQUIRE(resp.code == at::Result::Code::OK);
        REQUIRE(at::response::ccwa::parse(resp.response, ret) == true);
        REQUIRE(ret.size() == 2);

        REQUIRE(ret[0].status == at::response::ccwa::Status::Enable);
        REQUIRE(ret[0].serviceClass == at::response::ccwa::ServiceClass::Voice);

        REQUIRE(ret[1].status == at::response::ccwa::Status::Enable);
        REQUIRE(ret[1].serviceClass == at::response::ccwa::ServiceClass::Fax);
    }

    SECTION("WRONG CCWA?  - invalid status")
    {

        at::Result resp;
        resp.code = at::Result::Code::OK;
        std::vector<at::response::ccwa::CcwaParsed> ret;
        resp.response.push_back("+CCWA: 9,1");
        resp.response.push_back("+CCWA: 1,4");
        resp.response.push_back("OK");

        REQUIRE(resp.code == at::Result::Code::OK);
        REQUIRE(at::response::ccwa::parse(resp.response, ret) == false);
        REQUIRE(ret.size() == 0);
    }

    SECTION("WRONG CCWA?  - invalid class")
    {

        at::Result resp;
        resp.code = at::Result::Code::OK;
        std::vector<at::response::ccwa::CcwaParsed> ret;
        resp.response.push_back("+CCWA: 1,1");
        resp.response.push_back("+CCWA: 1,66");
        resp.response.push_back("OK");

        REQUIRE(resp.code == at::Result::Code::OK);
        REQUIRE(at::response::ccwa::parse(resp.response, ret) == false);
        REQUIRE(ret.size() == 0);
    }

    SECTION("WRONG CCWA?  - invalid token")
    {

        at::Result resp;
        resp.code = at::Result::Code::OK;
        std::vector<at::response::ccwa::CcwaParsed> ret;
        resp.response.push_back("+CCW: 1,1");
        resp.response.push_back("+CCWA: 1,66");
        resp.response.push_back("OK");

        REQUIRE(resp.code == at::Result::Code::OK);
        REQUIRE(at::response::ccwa::parse(resp.response, ret) == false);
        REQUIRE(ret.size() == 0);
    }

    SECTION("WRONG CCWA?  - to little tokens")
    {

        at::Result resp;
        resp.code = at::Result::Code::OK;
        std::vector<at::response::ccwa::CcwaParsed> ret;
        resp.response.push_back("+CCWA: 1,1");
        resp.response.push_back("+CCWA: 1");
        resp.response.push_back("OK");

        REQUIRE(resp.code == at::Result::Code::OK);
        REQUIRE(at::response::ccwa::parse(resp.response, ret) == false);
        REQUIRE(ret.size() == 0);
    }

    SECTION("WRONG CCWA?  - to many tokens")
    {

        at::Result resp;
        resp.code = at::Result::Code::OK;
        std::vector<at::response::ccwa::CcwaParsed> ret;
        resp.response.push_back("+CCWA: 1,1,9");
        resp.response.push_back("+CCWA: 1,4");
        resp.response.push_back("OK");

        REQUIRE(resp.code == at::Result::Code::OK);
        REQUIRE(at::response::ccwa::parse(resp.response, ret) == false);
        REQUIRE(ret.size() == 0);
    }
}
