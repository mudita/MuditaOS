// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>
#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include "Modem/ATURCStream.hpp"

TEST_CASE("URC AT Stream Parser")
{
    SECTION("One URC")
    {
        at::ATURCStream stream;
        stream.write("\r\n+URC: one\r\n");
        auto v = stream.getURCList();
        REQUIRE(v.size() == 1);
        REQUIRE(*v.begin() == "\r\n+URC: one\r\n");
    }

    SECTION("One URC with junk at the begin")
    {
        at::ATURCStream stream;
        stream.write("ssss\r\n+URC: one\r\n");
        auto v = stream.getURCList();
        REQUIRE(v.size() == 1);
        REQUIRE(*v.begin() == "\r\n+URC: one\r\n");
    }

    SECTION("test 1")
    {
        at::ATURCStream stream;
        stream.write("ssss\r\n+URC: one\r\n");
        stream.write("\r\n+URC: two\r\n");
        auto v = stream.getURCList();
        REQUIRE(v.size() == 2);
        REQUIRE(*v.begin() == "\r\n+URC: one\r\n");
        v.pop_front();
        REQUIRE(*v.begin() == "\r\n+URC: two\r\n");
    }

    SECTION("test 2")
    {
        at::ATURCStream stream;
        stream.write("ssss\r\n+URC: one\r\n");
        stream.write("\r\n+URC: two\r\n");
        stream.write("\r\n+URC: 3,\"\r\n....");
        stream.write("....\"\r\n");
        auto v = stream.getURCList();
        REQUIRE(v.size() == 3);
        REQUIRE(*v.begin() == "\r\n+URC: one\r\n");
        v.pop_front();
        REQUIRE(*v.begin() == "\r\n+URC: two\r\n");
        v.pop_front();
        REQUIRE(*v.begin() == "\r\n+URC: 3,\"\r\n........\"\r\n");
    }

    SECTION("test 4")
    {
        at::ATURCStream stream;
        stream.write("\r\n+CUSD: 0,\"Pozostalo Ci 24,84zl do wykorzystania do 2021-02-18 23:59:59. Dodatkowo w ramach "
                     "srodkow promocyjnych masz 0,00 zl ");
        stream.write("do wykorzystania do 2020-12-30 23:59:59.\",15\r\n");
        auto v = stream.getURCList();
        REQUIRE(v.size() == 1);
    }

    SECTION("test 5")
    {
        at::ATURCStream stream;
        stream.write("\r\n+CUSD: 1,\"Play 24 Menu\r\n"
                     "1 Konto\r\n"
                     "2 Uslugi / Promocje\r\n"
                     "0 Wyjscie\r\n"
                     "Wiecej opcji znajdziesz w Play24 na http://24.play.pl\",0\r\n");
        auto v = stream.getURCList();
        REQUIRE(v.size() == 1);
    }

    SECTION("test 6")
    {
        at::ATURCStream stream;
        stream.write("\r\n+CUSD: 1,\"Play 24 Menu\r\n"
                     "1 Konto\r\n");
        stream.write("2 Uslugi / Promocje\r\n"
                     "0 Wyjscie\r\n"
                     "Wiecej opcji znajdziesz w Play24 na http://24.play.pl\",0\r\n");
        auto v = stream.getURCList();
        REQUIRE(v.size() == 1);
    }
}
