// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include "Result.hpp"
#include <service-cellular/PacketData.hpp>

TEST_CASE("Quectel AT DataTransfer commands")
{
    SECTION("QIACT")
    {
        std::vector<std::shared_ptr<packet_data::APN::Config>> ret;
        at::Result resp;

        // OK
        resp.code = at::Result::Code::OK;
        resp.response.push_back("+QIACT: 1,1,1,\"10.7.157.1\"");
        REQUIRE(resp.code == at::Result::Code::OK);
        REQUIRE(at::response::parseQIACT(resp, ret) == true);
        REQUIRE(ret.size() == 1);
        REQUIRE(ret.at(0)->ip == "10.7.157.1");

        ret.clear();
        // Additional element, should be omitted, also other wrong data
        resp.response.push_back("+QIACT:");
        REQUIRE(at::response::parseQIACT(resp, ret) == true);
        REQUIRE(ret.size() == 1);
        REQUIRE(ret.at(0)->ip == "10.7.157.1");

        ret.clear();
        resp.response.clear();
        // Empty return, should return empty list
        REQUIRE(at::response::parseQIACT(resp, ret) == true);
        REQUIRE(ret.size() == 0);

        ret.clear();
        resp.response.clear();
        // Wrong should return zero
        resp.response.push_back("+QIACT: sa sa+QIACT: fsf fsa");
        REQUIRE(at::response::parseQIACT(resp, ret) == true);
        REQUIRE(ret.size() == 0);
    }
    SECTION("QICSGP")
    {
        /// +QICSGP: 1,"","","",0
        std::shared_ptr<packet_data::APN::Config> ret = std::make_shared<packet_data::APN::Config>();
        at::Result resp;
        resp.code = at::Result::Code::OK;
        // OK
        resp.response.push_back("+QICSGP: 1,\"apn\",\"\",\"\",0");
        REQUIRE(resp.code == at::Result::Code::OK);
        REQUIRE(at::response::parseQICSGP(resp, ret) == true);
        REQUIRE(ret->apn == "apn");

        resp.response.clear();
        // OK
        resp.response.push_back("+QICSGP: 1,\"apn\",\"internet\",\"password\",1");
        REQUIRE(resp.code == at::Result::Code::OK);
        REQUIRE(at::response::parseQICSGP(resp, ret) == true);
        REQUIRE(ret->apn == "apn");
        REQUIRE(ret->username == "internet");
        REQUIRE(ret->password == "password");

        resp.response.clear();
        // empty APN
        resp.response.push_back("+QICSGP: 1,\"\",\"\",\"\",0");
        REQUIRE(at::response::parseQICSGP(resp, ret) == true);
        REQUIRE(ret->apn == "");
        REQUIRE(ret->isEmpty() == true);

        // bad response
        resp.code = at::Result::Code::ERROR;
        REQUIRE(at::response::parseQICSGP(resp, ret) == false);

        resp.response.clear();
        // Bad token
        resp.response.push_back("+QICSXX: 1,\"\",\"\",\"\",0");
        REQUIRE(at::response::parseQICSGP(resp, ret) == false);

        resp.response.clear();
        // Not enought parameters
        resp.response.push_back("+QICSGP: 1,\"\",\"\"");
        REQUIRE(at::response::parseQICSGP(resp, ret) == false);

        resp.response.clear();
        // wrong msg
        resp.response.push_back("ds +QICSGP: adsad +QICSGP: 1,\"\",\"");
        REQUIRE(at::response::parseQICSGP(resp, ret) == false);
    }
}
