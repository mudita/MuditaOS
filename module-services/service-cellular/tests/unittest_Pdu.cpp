// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <service-cellular/Pdu.hpp>
#include <Utils.hpp>

TEST_CASE("Pdu parser test")
{
    SECTION("Valid MMS Notification Play")
    {
        const char *hexMsg =
            "0006226170706C69636174696F6E2F766E642E7761702E6D6D732D6D65737361676500AF848C82983132333435363738008D918A80"
            "8E0302BF5F8805810303F47F83687474703A2F2F6D6D73632E706C61792E706C2F6D6D732F776170656E632F313233343536373800"
            "8918802B34383938373635343332312F545950453D504C4D4E00";
        std::string byteMsg    = utils::hexToBytes<std::string>(hexMsg);
        auto const mmsNotifOpt = pdu::parse(byteMsg);
        REQUIRE(mmsNotifOpt->fromAddress == "+48987654321/TYPE=PLMN");
        REQUIRE(mmsNotifOpt->contentLocation == "http://mmsc.play.pl/mms/wapenc/12345678");
    }

    SECTION("Valid MMS Notification T-Mobile")
    {
        const char *hexMsg =
            "C50603BEAF848C829831383261316237323931643835663230644070322E6D6D73632E742D6D6F62696C652E706C008D918A80"
            "8E0285C288048102A8BE83687474703A2F2F31302E3234322E3234332E34303A393830302F6D6D313F69643D32323038313531"
            "3832613162373238313932366630356262008918802B34383530353035303530352F545950453D504C4D4E009600";
        std::string byteMsg    = utils::hexToBytes<std::string>(hexMsg);
        auto const mmsNotifOpt = pdu::parse(byteMsg);
        REQUIRE(mmsNotifOpt->fromAddress == "+48505050505/TYPE=PLMN");
        REQUIRE(mmsNotifOpt->contentLocation == "http://10.242.243.40:9800/mm1?id=220815182a1b7281926f05bb");
    }

    SECTION("Incomplete MMS Notification T-Mobile")
    {
        // Incomplete From MMS header data value
        const char *hexMsg =
            "C50603BEAF848C829831383261316237323931643835663230644070322E6D6D73632E742D6D6F62696C652E706C008D918A80"
            "8E0285C288048102A8BE83687474703A2F2F31302E3234322E3234332E34303A393830302F6D6D313F69643D32323038313531"
            "3832613162373238313932366630356262008918802B34383530";
        std::string byteMsg    = utils::hexToBytes<std::string>(hexMsg);
        auto const mmsNotifOpt = pdu::parse(byteMsg);
        REQUIRE(bool(mmsNotifOpt) == false);
    }

    SECTION("Valid MMS Notification T-Mobile 2")
    {
        const char *hexMsg =
            "600603BEAF848C82983138326135373163376432353265383033644070312E6D6D73632E742D6D6F62696C652E706C008D918A"
            "808E0302211C88058103093A7F83687474703A2F2F31302E3230352E34332E34303A393830302F6D6D313F69643D3232303831"
            "36313832613537316335343632643638386266008918802B34383238373238373238372F545950453D504C4D4E009600";
        std::string byteMsg    = utils::hexToBytes<std::string>(hexMsg);
        auto const mmsNotifOpt = pdu::parse(byteMsg);
        REQUIRE(mmsNotifOpt->fromAddress == "+48287287287/TYPE=PLMN");
        REQUIRE(mmsNotifOpt->contentLocation == "http://10.205.43.40:9800/mm1?id=220816182a571c5462d688bf");
    }

    SECTION("Valid MMS Notification Plus")
    {
        const char *hexMsg =
            "000607BE8DE9AF84B4808C8298303130322D67616B32346E326E643679393230008D908918802B34383738323738323738322F"
            "545950453D504C4D4E008A808E0302211C88058103093A7E83687474703A2F2F6D6D733372783A383030322F303130322D6761"
            "6B32346E326E64367939323000";
        std::string byteMsg    = utils::hexToBytes<std::string>(hexMsg);
        auto const mmsNotifOpt = pdu::parse(byteMsg);
        REQUIRE(mmsNotifOpt->fromAddress == "+48782782782/TYPE=PLMN");
        REQUIRE(mmsNotifOpt->contentLocation == "http://mms3rx:8002/0102-gak24n2nd6y920");
    }

    SECTION("Valid MMS Notification Orange")
    {
        const char *hexMsg =
            "000607BEAF848DDBB4818C8298416263646566676869008D928918802B34383132333435363738392F545950453D504C4D4E008681"
            "8A808E0285AE88048102A8BF83687474703A2F2F67656F6D323A383030322F6D6D73632F633F41626364656667686900";
        std::string byteMsg    = utils::hexToBytes<std::string>(hexMsg);
        auto const mmsNotifOpt = pdu::parse(byteMsg);
        REQUIRE(mmsNotifOpt->fromAddress == "+48123456789/TYPE=PLMN");
        REQUIRE(mmsNotifOpt->contentLocation == "http://geom2:8002/mmsc/c?Abcdefghi");
    }

    SECTION("Valid MMS Notification without From field")
    {
        const char *hexMsg = "000607BEAF848DDBB4818C8298416263646566676869008D9286818A808E0285AE88048102A8BF83687474703"
                             "A2F2F67656F6D323A383030322F6D6D73632F633F41626364656667686900";
        std::string byteMsg    = utils::hexToBytes<std::string>(hexMsg);
        auto const mmsNotifOpt = pdu::parse(byteMsg);
        REQUIRE(mmsNotifOpt->fromAddress == "");
        REQUIRE(mmsNotifOpt->contentLocation == "http://geom2:8002/mmsc/c?Abcdefghi");
    }

    SECTION("PDU Type != Push")
    {
        const char *hexMsg =
            "000507BEAF848DDBB4818C8298416263646566676869008D928918802B34383132333435363738392F545950453D504C4D4E008681"
            "8A808E0285AE88048102A8BF83687474703A2F2F67656F6D323A383030322F6D6D73632F633F41626364656667686900";
        std::string byteMsg    = utils::hexToBytes<std::string>(hexMsg);
        auto const mmsNotifOpt = pdu::parse(byteMsg);
        REQUIRE(bool(mmsNotifOpt) == false);
    }

    SECTION("X-Wap-Application-ID != x-wap-application:mms.ua")
    {
        const char *hexMsg =
            "000607BEAF838DDBB4818C8298416263646566676869008D928918802B34383132333435363738392F545950453D504C4D4E008681"
            "8A808E0285AE88048102A8BF83687474703A2F2F67656F6D323A383030322F6D6D73632F633F41626364656667686900";
        std::string byteMsg    = utils::hexToBytes<std::string>(hexMsg);
        auto const mmsNotifOpt = pdu::parse(byteMsg);
        REQUIRE(bool(mmsNotifOpt) == false);
    }

    SECTION("Missing X-Wap-Application-ID")
    {
        const char *hexMsg =
            "000605BE8DDBB4818C8298416263646566676869008D928918802B34383132333435363738392F545950453D504C4D4E0086818A80"
            "8E0285AE88048102A8BF83687474703A2F2F67656F6D323A383030322F6D6D73632F633F41626364656667686900";
        std::string byteMsg    = utils::hexToBytes<std::string>(hexMsg);
        auto const mmsNotifOpt = pdu::parse(byteMsg);
        REQUIRE(bool(mmsNotifOpt) == false);
    }

    SECTION("X-Mms-Message-Type != m-notification-ind")
    {
        const char *hexMsg =
            "000607BEAF848DDBB4818C8198416263646566676869008D928918802B34383132333435363738392F545950453D504C4D4E008681"
            "8A808E0285AE88048102A8BF83687474703A2F2F67656F6D323A383030322F6D6D73632F633F41626364656667686900";
        std::string byteMsg    = utils::hexToBytes<std::string>(hexMsg);
        auto const mmsNotifOpt = pdu::parse(byteMsg);
        REQUIRE(bool(mmsNotifOpt) == false);
    }

    SECTION("Missing X-Mms-Message-Type")
    {
        const char *hexMsg =
            "000607BEAF848DDBB4818C8198416263646566676869008D928918802B34383132333435363738392F545950453D504C4D4E008681"
            "8A808E0285AE88048102A8BF83687474703A2F2F67656F6D323A383030322F6D6D73632F633F41626364656667686900";
        std::string byteMsg    = utils::hexToBytes<std::string>(hexMsg);
        auto const mmsNotifOpt = pdu::parse(byteMsg);
        REQUIRE(bool(mmsNotifOpt) == false);
    }

    SECTION("Missing PDU Content")
    {
        const char *hexMsg     = "000608BEAF848DDBB481";
        std::string byteMsg    = utils::hexToBytes<std::string>(hexMsg);
        auto const mmsNotifOpt = pdu::parse(byteMsg);
        REQUIRE(bool(mmsNotifOpt) == false);
    }

    SECTION("Invalid PDU header length")
    {
        const char *hexMsg     = "000607BEAF848DDBB481";
        std::string byteMsg    = utils::hexToBytes<std::string>(hexMsg);
        auto const mmsNotifOpt = pdu::parse(byteMsg);
        REQUIRE(bool(mmsNotifOpt) == false);
    }
}
