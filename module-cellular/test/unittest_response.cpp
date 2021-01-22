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
