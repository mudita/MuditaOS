// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>

#include <catch2/catch.hpp>
#include "modem/ATURCStream.hpp"

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

    SECTION("One URC multiple \"\"")
    {
        at::ATURCStream stream;
        stream.write("\r\n+URC: one \"\"\"\"\r\n");
        auto v = stream.getURCList();
        REQUIRE(v.size() == 1);
        REQUIRE(*v.begin() == "\r\n+URC: one \"\"\"\"\r\n");
    }

    SECTION("One URC no closed \"")
    {
        at::ATURCStream stream;
        stream.write("\r\n+URC: one \"\"\"\r\n");
        auto v = stream.getURCList();
        REQUIRE(v.size() == 0);
    }

    SECTION("One URC with junk at the begin")
    {
        at::ATURCStream stream;
        stream.write("ssss\r\n+URC: one\r\n");
        auto v = stream.getURCList();
        REQUIRE(v.size() == 1);
        REQUIRE(*v.begin() == "\r\n+URC: one\r\n");
    }

    SECTION("Two URC with junk at the begin")
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

    SECTION("Three URC with junk at the begin")
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

    SECTION("URC split in empty literal")
    {
        at::ATURCStream stream;
        stream.write("\r\n+URC: \"");
        stream.write("\"\r\n");
        auto v = stream.getURCList();
        REQUIRE(v.size() == 1);
        REQUIRE(*v.begin() == "\r\n+URC: \"\"\r\n");
    }

    SECTION("Long URC in two parts")
    {
        at::ATURCStream stream;
        stream.write("\r\n+CUSD: 0,\"Pozostalo Ci 24,84zl do wykorzystania do 2021-02-18 23:59:59. Dodatkowo w ramach "
                     "srodkow promocyjnych masz 0,00 zl ");
        stream.write("do wykorzystania do 2020-12-30 23:59:59.\",15\r\n");
        auto v = stream.getURCList();
        REQUIRE(v.size() == 1);
    }

    SECTION("Long URC with new line in string section")
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

    SECTION("Long URC with new line in string section in two parts")
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

    SECTION("One urc (URC without +)")
    {
        at::ATURCStream stream;
        stream.write("\r\nNO CARRIER\r\n");
        auto v = stream.getURCList();
        REQUIRE(v.size() == 1);
        REQUIRE(*v.begin() == "\r\nNO CARRIER\r\n");
    }

    SECTION("Two mixed URC")
    {
        at::ATURCStream stream;
        stream.write("\r\nNO CARRIER\r\n");
        stream.write("\r\n+URC: two\r\n");
        auto v = stream.getURCList();
        REQUIRE(v.size() == 2);
        REQUIRE(*v.begin() == "\r\nNO CARRIER\r\n");
        v.pop_front();
        REQUIRE(*v.begin() == "\r\n+URC: two\r\n");
    }

    SECTION("Two mixed URC with junk at the beginning")
    {
        at::ATURCStream stream;
        stream.write("junk\r\nNO CARRIER\r\n");
        stream.write("\r\n+URC: two\r\n");
        auto v = stream.getURCList();
        REQUIRE(v.size() == 2);
        REQUIRE(*v.begin() == "\r\nNO CARRIER\r\n");
        v.pop_front();
        REQUIRE(*v.begin() == "\r\n+URC: two\r\n");
    }

    SECTION("Three mixed URC with junk at the beginning, read partially")
    {
        at::ATURCStream stream;
        stream.write("ssss\r\n+URC: one\r\n");
        stream.write("\r\n+URC: two\r\n");
        stream.write("\r\n+URC: 3,\"\r\n....");
        auto v = stream.getURCList();
        REQUIRE(v.size() == 2);
        REQUIRE(*v.begin() == "\r\n+URC: one\r\n");
        v.pop_front();
        REQUIRE(*v.begin() == "\r\n+URC: two\r\n");

        stream.write("....\"\r\n");
        v = stream.getURCList();
        REQUIRE(v.size() == 1);
        REQUIRE(*v.begin() == "\r\n+URC: 3,\"\r\n........\"\r\n");
    }

    SECTION("Standard URC  1 - simple one")
    {
        at::ATURCStream stream;
        stream.write("\r\nRING\r\n");
        auto v = stream.getURCList();
        REQUIRE(v.size() == 1);
        REQUIRE(*v.begin() == "\r\nRING\r\n");
    }

    SECTION("Standard URC  2 - with literals")
    {
        at::ATURCStream stream;
        stream.write("\r\n+CMTI: \"ME\",0\r\n");
        auto v = stream.getURCList();
        REQUIRE(v.size() == 1);
        REQUIRE(*v.begin() == "\r\n+CMTI: \"ME\",0\r\n");
    }

    SECTION("Standard URC  3 - with symbol /")
    {
        at::ATURCStream stream;
        stream.write("\r\n+QCFG: \"urc/cache\",0\r\n");
        auto v = stream.getURCList();
        REQUIRE(v.size() == 1);
        REQUIRE(*v.begin() == "\r\n+QCFG: \"urc/cache\",0\r\n");
    }

    SECTION("Standard URC  4 - with symbol &")
    {
        at::ATURCStream stream;
        stream.write("\r\n+QIND: \"act\",\"HSDPA&HSUPA\"\r\n");
        auto v = stream.getURCList();
        REQUIRE(v.size() == 1);
        REQUIRE(*v.begin() == "\r\n+QIND: \"act\",\"HSDPA&HSUPA\"\r\n");
    }

    SECTION("Standard URC  5 - two string section")
    {
        at::ATURCStream stream;
        stream.write("\r\n+QIND: \"act\",\"UNKNOWN\"\r\n");
        auto v = stream.getURCList();
        REQUIRE(v.size() == 1);
        REQUIRE(*v.begin() == "\r\n+QIND: \"act\",\"UNKNOWN\"\r\n");
    }

    SECTION("Standard URC  6 - mixed sections")
    {
        at::ATURCStream stream;
        stream.write("\r\n+CREG: 1,\"D509\",\"80D413D\",7\r\n");
        auto v = stream.getURCList();
        REQUIRE(v.size() == 1);
        REQUIRE(*v.begin() == "\r\n+CREG: 1,\"D509\",\"80D413D\",7\r\n");
    }

    SECTION("Standard URC  7 - POWERED DOWN")
    {
        at::ATURCStream stream;
        stream.write("\r\nPOWERED DOWN\r\n");
        auto v = stream.getURCList();
        REQUIRE(v.size() == 1);
        REQUIRE(*v.begin() == "\r\nPOWERED DOWN\r\n");
    }

    SECTION("Standard URC  8 - empty \"\"")
    {
        at::ATURCStream stream;
        stream.write("\r\n+CLIP: \"+48600222222\",145,\"\",0,,0\r\n");
        auto v = stream.getURCList();
        REQUIRE(v.size() == 1);
        REQUIRE(*v.begin() == "\r\n+CLIP: \"+48600222222\",145,\"\",0,,0\r\n");
    }

    SECTION("Standard URC  9 - CLIP without empty \"\" multi ,")
    {
        at::ATURCStream stream;
        stream.write("\r\n+CLIP: \"+48600222222\",145,,,,0\r\n");
        auto v = stream.getURCList();
        REQUIRE(v.size() == 1);
        REQUIRE(*v.begin() == "\r\n+CLIP: \"+48600222222\",145,,,,0\r\n");
    }
}
