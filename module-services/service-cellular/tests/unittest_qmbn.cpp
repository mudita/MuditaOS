// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <service-cellular/QMBNManager.hpp>

TEST_CASE("Quectel MBN test")
{
    SECTION("Parse list of MBN")
    {
        std::vector<at::response::qmbncfg::MBNConfig> ret;
        at::Result resp;
        resp.code = at::Result::Code::OK;
        resp.response.push_back("+QMBNCFG: \"List\",2,0,0,\"hVoLTE-Verizon\",0x05010141,202004091");

        REQUIRE(at::response::parseQMBNCFGlist(resp, ret) == true);
        REQUIRE(ret.size() == 1);
        REQUIRE(ret[0].fileName == "hVoLTE-Verizon");
        REQUIRE(ret[0].version == "0x05010141");
        REQUIRE(ret[0].releaseDate == "202004091");

        REQUIRE(ret[0].activated == 0);
        REQUIRE(ret[0].selected == 0);
    }
}
