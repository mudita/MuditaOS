// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

#include <ucs2/UCS2.hpp>

TEST_CASE("UCS2 to UTF8 conversion")
{
    const char *messageRawBody = "0054006500730074";
    UTF8 decodedMessage        = UCS2(std::string(messageRawBody)).toUTF8();
    REQUIRE((uint32_t)('T') == decodedMessage[0]);
    REQUIRE((uint32_t)('e') == decodedMessage[1]);
    REQUIRE((uint32_t)('s') == decodedMessage[2]);
    REQUIRE((uint32_t)('t') == decodedMessage[3]);

    SECTION("UTF8 IS ASCII combination")
    {
        auto utf8 = UCS2(std::string("00380034003800370037003300380034003800340036003900380032")).toUTF8();
        REQUIRE(utf8.isASCIICombination());
        auto ascii = utf8.toASCII();
        REQUIRE(ascii == std::string("TWITTER"));
    }

    SECTION("UTF8 IS NOT ASCII combination")
    {
        auto utf8 = UCS2(std::string("003200390031")).toUTF8();
        REQUIRE_FALSE(utf8.isASCIICombination());
        REQUIRE_FALSE(utf8.toASCII().has_value());
    }
}

TEST_CASE("UCS2 from UTF8 emoji 😁")
{
    UTF8 utf8("😁");
    UCS2 ucs2       = UCS2(utf8);
    std::string str = "D83DDE01";
    REQUIRE(ucs2.str() == str);
}

TEST_CASE("UCS2 from UTF8 emoji 🍣")
{
    UTF8 utf8("🍣");
    UCS2 ucs2       = UCS2(utf8);
    std::string str = "D83CDF63";
    REQUIRE(ucs2.str() == str);
}

TEST_CASE("UCS2 text with emojis int the middle from UTF8 code")
{
    UTF8 utf8("ęą😁ęą🍣ęą");
    UCS2 ucs2 = UCS2(utf8);
    std::string expected("01190105" //ęą
                         "D83DDE01" // 😁
                         "01190105" // ęą
                         "D83CDF63" // 🍣
                         "01190105");
    REQUIRE(ucs2.str() == expected);
}

TEST_CASE("UTF8 to UCS2 conversion")
{
    UTF8 utf8("Test");
    UCS2 ucs2(utf8);
    std::string str = "0054006500730074";
    REQUIRE(ucs2.str() == str);
}

TEST_CASE("TEST special input characters from UTF8")
{
    UTF8 utf8("ęą");
    UCS2 ucs2(utf8);
    std::string str = "01190105";
    REQUIRE(ucs2.str() == str);
}

TEST_CASE("TEST special input characters from std::string")
{
    UCS2 ucs2(std::string("01190105"));
    UTF8 utf8("ęą");
    REQUIRE(ucs2.toUTF8() == utf8);
}

TEST_CASE("UTF8 emoji 🍣 from UCS2 code")
{
    UCS2 ucs2(std::string("D83CDF63"));
    UTF8 utf8("🍣");
    REQUIRE(ucs2.toUTF8() == utf8);
}

TEST_CASE("UTF8 emoji 😁 and text ęą from UCS2 code")
{
    UCS2 ucs2(std::string("D83DDE0101190105"));
    UTF8 utf8("😁ęą");
    REQUIRE(ucs2.toUTF8() == utf8);
}

TEST_CASE("UTF8 emoji 😁 and text abc from UCS2 code")
{
    UCS2 ucs2(std::string("D83CDF63006100620063"));
    UTF8 utf8("🍣abc");
    REQUIRE(ucs2.toUTF8() == utf8);
}

TEST_CASE("UTF8 text with emojis int the middle from UCS2 code")
{
    UCS2 ucs2(std::string("01190105" //ęą
                          "D83DDE01" // 😁
                          "01190105" // ęą
                          "D83CDF63" // 🍣
                          "01190105"));
    UTF8 utf8("ęą😁ęą🍣ęą");
    REQUIRE(ucs2.toUTF8() == utf8);
}

TEST_CASE("UCS2 to UTF8 long string conversion")
{
    const char *messageRawBody = "004C006F00720065006D00200069007000730075006D00200064006F006C006F007200200073006900740"
                                 "0200061006D00650074002C00200063006"
                                 "F006E00730065006300740065007400750072002000610064006900700069007300630069006E00670020"
                                 "0065006C00690074002E002000500065006C006C0065006E007400650073007"
                                 "10075006500200073006F006C006C0069006300690074007500640069006E002000760061007200690075"
                                 "007300200069007000730075006D002C0020006100200073007500730063006"
                                 "9007000690074002000660065006C00690073002000730061006700690074007400690073002000750074"
                                 "002E00200041006C0069007100750061006D002000740072006900730074006"
                                 "900710075006500200062006C0061006E006400690074002000660065006C00690073002C002000760069"
                                 "00740061006500200063006F006D006D006F0064006F0020006D00610073007"
                                 "300610020006D006100780069006D0075007300200069006E002E0020004D0061006500630065006E0061"
                                 "00730020006100740020006D006F006C0065007300740069006500200070007"
                                 "5007200750073002E0020005300650064002000760065006C00690074002000760065006C00690074002C"
                                 "002000660069006E00690062007500730020007100750069007300200065007"
                                 "30074002000610063002C00630075007200730075007300200076006F006C007500740070006100740020"
                                 "006D00610067006E0061002E00200056006500730074006900620075006C007"
                                 "5006D00200063006F006E00760061006C006C0069007300200075006C0074007200690063006900650073"
                                 "00200061006E00740065002C002000760065006C00200067007200610076006"
                                 "9006400610020006C006900670075006C006100200065006700650073007400610073002000610063002E"
                                 "00200041006C0069007100750061006D0020007600690074006100650020007"
                                 "00075006C00760069006E00610072002000750072006E0061002E00200055007400200076006F006C0075"
                                 "0074007000610074002000660065006C0069007300200061006C00690071007"
                                 "5006500740020006500660066006900630069007400750072002000740065006D0070006F0072002E0020"
                                 "005300650064002000630075007200730075007300200063006F006E0064006"
                                 "9006D0065006E00740075006D00200061006E00740065002E00200056006500730074006900620075006C"
                                 "0075006D0020006600650075006700690061007400200061007200630075002"
                                 "00061006300200069007000730075006D0020006D006100780069006D00750073002C00200069006E0020"
                                 "0074007200690073007400690071007500650020006A007500730074006F002"
                                 "00061007500630074006F0072002E002000530075007300700065006E0064006900730073006500200075"
                                 "0074002000700075006C00760069006E006100720020006D006100730073006"
                                 "1002E00200053006500640020006600610075006300690062007500730020006100740020007000750072"
                                 "007500730020007600690074006100650020006C006F0062006F00720074006"
                                 "90073002E00200041006C0069007100750061006D002000650067006500740020006E0065007100750065"
                                 "0020007100750069007300200065007300740020006D0061007400740069007"
                                 "30020006D006F006C006C00690073002E002000500068006100730065006C006C00750073002000710075"
                                 "00690073002000730065006D007000650072002000720069007300750073002"
                                 "E00200049006E00740065007200640075006D0020006500740020006D0061006C00650073007500610064"
                                 "0061002000660061006D0065007300200061006300200061006E00740065002"
                                 "00069007000730075006D0020007000720069006D0069007300200069006E002000660061007500630069"
                                 "006200750073002E0020004D006F007200620069002000760065006C0020006"
                                 "C006900670075006C006100200065007500200061006E0074006500200065006C0065006D0065006E0074"
                                 "0075006D0020006C006100630069006E00690061002E0055007400200069006"
                                 "4002000720068006F006E0063007500730020006E006900620068002E";
    UTF8 decodedMessage = UCS2(std::string(messageRawBody)).toUTF8();
    UTF8 longUtf8Message("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Pellentesque sollicitudin varius "
                         "ipsum, a suscipit felis sagittis ut."
                         " Aliquam tristique blandit felis, vitae commodo massa maximus in. Maecenas at molestie "
                         "purus. Sed velit velit, finibus quis est ac,"
                         "cursus volutpat magna. Vestibulum convallis ultricies ante, vel gravida ligula egestas ac. "
                         "Aliquam vitae pulvinar urna. Ut volutpat"
                         " felis aliquet efficitur tempor. Sed cursus condimentum ante. Vestibulum feugiat arcu ac "
                         "ipsum maximus, in tristique justo auctor."
                         " Suspendisse ut pulvinar massa. Sed faucibus at purus vitae lobortis. Aliquam eget neque "
                         "quis est mattis mollis. Phasellus quis"
                         " semper risus. Interdum et malesuada fames ac ante ipsum primis in faucibus. Morbi vel "
                         "ligula eu ante elementum lacinia."
                         "Ut id rhoncus nibh.");
    REQUIRE(decodedMessage == longUtf8Message);
}

TEST_CASE("UTF8 to UCS2 long string conversion")
{
    UTF8 longUtf8Message("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Pellentesque sollicitudin varius "
                         "ipsum, a suscipit felis sagittis ut."
                         " Aliquam tristique blandit felis, vitae commodo massa maximus in. Maecenas at molestie "
                         "purus. Sed velit velit, finibus quis est ac,"
                         "cursus volutpat magna. Vestibulum convallis ultricies ante, vel gravida ligula egestas ac. "
                         "Aliquam vitae pulvinar urna. Ut volutpat"
                         " felis aliquet efficitur tempor. Sed cursus condimentum ante. Vestibulum feugiat arcu ac "
                         "ipsum maximus, in tristique justo auctor."
                         " Suspendisse ut pulvinar massa. Sed faucibus at purus vitae lobortis. Aliquam eget neque "
                         "quis est mattis mollis. Phasellus quis"
                         " semper risus. Interdum et malesuada fames ac ante ipsum primis in faucibus. Morbi vel "
                         "ligula eu ante elementum lacinia."
                         "Ut id rhoncus nibh.");

    UCS2 longUcs2Message(longUtf8Message);
    REQUIRE(
        longUcs2Message.str() ==
        std::string("004C006F00720065006D00200069007000730075006D00200064006F006C006F0072002000730069007400200061006D00"
                    "650074002C00200063006F006E00730065006300740065007400750072002000610064006900700069007300630069006E"
                    "006700200065006C00690074002E002000500065006C006C00"
                    "65006E00740065007300710075006500200073006F006C006C0069006300690074007500640069006E0020007600610072"
                    "00690075007300200069007000730075006D002C0020006100"
                    "2000730075007300630069007000690074002000660065006C006900730020007300610067006900740074006900730020"
                    "00750074002E00200041006C0069007100750061006D002000"
                    "740072006900730074006900710075006500200062006C0061006E006400690074002000660065006C00690073002C0020"
                    "0076006900740061006500200063006F006D006D006F006400"
                    "6F0020006D00610073007300610020006D006100780069006D0075007300200069006E002E0020004D0061006500630065"
                    "006E006100730020006100740020006D006F006C0065007300"
                    "7400690065002000700075007200750073002E0020005300650064002000760065006C00690074002000760065006C0069"
                    "0074002C002000660069006E00690062007500730020007100"
                    "75006900730020006500730074002000610063002C00630075007200730075007300200076006F006C0075007400700061"
                    "00740020006D00610067006E0061002E002000560065007300"
                    "74006900620075006C0075006D00200063006F006E00760061006C006C0069007300200075006C00740072006900630069"
                    "0065007300200061006E00740065002C002000760065006C00"
                    "2000670072006100760069006400610020006C006900670075006C00610020006500670065007300740061007300200061"
                    "0063002E00200041006C0069007100750061006D0020007600"
                    "69007400610065002000700075006C00760069006E00610072002000750072006E0061002E00200055007400200076006F"
                    "006C00750074007000610074002000660065006C0069007300"
                    "200061006C006900710075006500740020006500660066006900630069007400750072002000740065006D0070006F0072"
                    "002E0020005300650064002000630075007200730075007300"
                    "200063006F006E00640069006D0065006E00740075006D00200061006E00740065002E0020005600650073007400690062"
                    "0075006C0075006D0020006600650075006700690061007400"
                    "20006100720063007500200061006300200069007000730075006D0020006D006100780069006D00750073002C00200069"
                    "006E0020007400720069007300740069007100750065002000"
                    "6A007500730074006F00200061007500630074006F0072002E002000530075007300700065006E00640069007300730065"
                    "002000750074002000700075006C00760069006E0061007200"
                    "20006D0061007300730061002E002000530065006400200066006100750063006900620075007300200061007400200070"
                    "00750072007500730020007600690074006100650020006C00"
                    "6F0062006F0072007400690073002E00200041006C0069007100750061006D002000650067006500740020006E00650071"
                    "00750065002000710075006900730020006500730074002000"
                    "6D006100740074006900730020006D006F006C006C00690073002E002000500068006100730065006C006C007500730020"
                    "0071007500690073002000730065006D007000650072002000"
                    "720069007300750073002E00200049006E00740065007200640075006D0020006500740020006D0061006C006500730075"
                    "006100640061002000660061006D0065007300200061006300"
                    "200061006E0074006500200069007000730075006D0020007000720069006D0069007300200069006E0020006600610075"
                    "00630069006200750073002E0020004D006F00720062006900"
                    "2000760065006C0020006C006900670075006C006100200065007500200061006E0074006500200065006C0065006D0065"
                    "006E00740075006D0020006C006100630069006E0069006100"
                    "2E00550074002000690064002000720068006F006E0063007500730020006E006900620068002E"));
}
