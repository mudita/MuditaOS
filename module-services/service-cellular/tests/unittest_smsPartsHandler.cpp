// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <service-cellular/src/SMSPartsHandler.hpp>

namespace
{
    constexpr auto singleSMSLimit            = 67;
    constexpr auto singleUCS2CharacterLength = 4;
    constexpr auto singleSMSLimitUCS2        = singleUCS2CharacterLength * singleSMSLimit;
    const std::string smileEmojiUTF          = "\U0001f601";
    const std::string winkEmojiUTF           = "\U0001f609";
} // namespace

TEST_CASE("SMS Parts handler")
{
    using namespace cellular::internal::sms;

    SECTION("Fit to SMS limit")
    {
        std::string smsAt67SignsLimit = "qwertyuiopasdfghjklzxcvbnm1234567890qwertyuiopasdfghjklzxcvbnm12345";
        SMSPartsHandler parts{smsAt67SignsLimit};
        CHECK(parts.isSinglePartSMS());
        CHECK(parts.getNextSmsPart().length() <= singleSMSLimitUCS2);
        CHECK(parts.getNextSmsPart().length() == 0);
    }

    SECTION("Fit to SMS limit with emoji")
    {
        std::string smsAt67SignsLimit =
            "qwertyuiopasdfghjklzxcvbnm1234567890qwertyuiopasdfghjklzxcvbnm1" + smileEmojiUTF;
        CHECK(smsAt67SignsLimit.length() == singleSMSLimit);
        SMSPartsHandler parts{smsAt67SignsLimit};
        CHECK(parts.isSinglePartSMS());
        CHECK(parts.getNextSmsPart().length() <= singleSMSLimitUCS2);
        CHECK(parts.getNextSmsPart().length() == 0);
    }

    SECTION("Above single SMS limit")
    {
        std::string smsAbove67SignsLimit = "qwertyuiopasdfghjklzxcvbnm1234567890qwertyuiopasdfghjklzxcvbnm123456";
        SMSPartsHandler parts{smsAbove67SignsLimit};
        CHECK(!parts.isSinglePartSMS());
        CHECK(parts.getNextSmsPart().length() <= singleSMSLimitUCS2);
        CHECK(parts.getNextSmsPart().length() == singleUCS2CharacterLength);
        CHECK(parts.getNextSmsPart().length() == 0);
    }

    SECTION("Above single SMS limit with emoji")
    {
        std::string smsAbove67SignsLimit =
            "qwertyuiopasdfghjklzxcvbnm1234567890qwertyuiopasdfghjklzxcvbnm1234" + smileEmojiUTF + "abcd";
        CHECK(smsAbove67SignsLimit.length() > singleSMSLimit);
        SMSPartsHandler parts{smsAbove67SignsLimit};
        CHECK(!parts.isSinglePartSMS());
        CHECK(parts.getNextSmsPart().length() <= singleSMSLimitUCS2);
        CHECK(parts.getNextSmsPart().length() > 0);
    }

    SECTION("3-part SMS limit")
    {
        std::string smsAt67SignsLimit = "qwertyuiopasdfghjklzxcvbnm1234567890qwertyuiopasdfghjklzxcvbnm12345";
        SMSPartsHandler parts{smsAt67SignsLimit + smsAt67SignsLimit + smsAt67SignsLimit};
        CHECK(!parts.isSinglePartSMS());
        CHECK(parts.getPartsCount() == 3);
        CHECK(parts.getNextSmsPart().length() == singleSMSLimitUCS2);
        CHECK(parts.getNextSmsPart().length() == singleSMSLimitUCS2);
        CHECK(parts.getNextSmsPart().length() == singleSMSLimitUCS2);
        CHECK(parts.getNextSmsPart().length() == 0);
    }

    SECTION("Multi-part SMS with emojis")
    {
        std::string multiPartSMS =
            "qwertyuiopasdf" + smileEmojiUTF + "ghjklzxcvbnm1234567890qwertyuiopasdfghjklzxcvbnm12345" + smileEmojiUTF +
            smileEmojiUTF + smileEmojiUTF + "ghjklzxcvbnm1234567890qwertyuiopasdfghjklzxcvbnm12345" + smileEmojiUTF +
            "teststringteststring" + smileEmojiUTF;
        SMSPartsHandler parts{multiPartSMS};
        UTF8 multiPartSMSUTF8{multiPartSMS};
        unsigned numOfParts = (multiPartSMSUTF8.length() / singleSMSLimit);
        numOfParts += (multiPartSMSUTF8.length() % singleSMSLimit) ? 1 : 0;
        CHECK(!parts.isSinglePartSMS());
        CHECK(parts.getPartsCount() == numOfParts);

        for (unsigned i = 0; i < numOfParts; ++i) {
            CHECK(parts.getNextSmsPart().length() <= singleSMSLimitUCS2);
        }
    }

    SECTION("7-part SMS limit")
    {
        std::string smsAbove7PartLimit =
            std::string{"qwertyuiopasdfghjklzxcvbnm1234567890qwertyuiopasdfghjklzxcvbnm12345"} +
            std::string{"qwertyuiopasdfghjklzxcvbnm1234567890qwertyuiopasdfghjklzxcvbnm12345"} +
            std::string{"qwertyuiopasdfghjklzxcvbnm1234567890qwertyuiopasdfghjklzxcvbnm12345"} +
            std::string{"qwertyuiopasdfghjklzxcvbnm1234567890qwertyuiopasdfghjklzxcvbnm12345"} +
            std::string{"qwertyuiopasdfghjklzxcvbnm1234567890qwertyuiopasdfghjklzxcvbnm12345"} +
            std::string{"qwertyuiopasdfghjklzxcvbnm1234567890qwertyuiopasdfghjklzxcvbnm12345"} +
            std::string{"qwertyuiopasdfghjklzxcvbnm1234567890qwertyuiopasdfghjklzxcvbnm12345"} + std::string{"qwerty"};
        SMSPartsHandler parts{smsAbove7PartLimit};
        CHECK(parts.isPartsCountAboveLimit());
        CHECK(parts.getPartsCount() == 8);
    }
}

TEST_CASE("EGD-7372 cases")
{
    using namespace cellular::internal::sms;

    SECTION("First")
    {
        std::string testString =
            "Hello everyone! Mudita Pure test today? I propose a meeting at 3:30PM " + winkEmojiUTF + ".";
        SMSPartsHandler parts{testString};
        CHECK(parts.getPartsCount() == 2);
        CHECK(parts.getNextSmsPart().length() <= singleSMSLimitUCS2);
        CHECK(parts.getNextSmsPart().length() <= singleSMSLimitUCS2);
    }

    SECTION("Second")
    {
        std::string testString =
            "Hello everyone! Mudita Pure test today? " + winkEmojiUTF + " I propose a meeting at 3:30PM.";
        SMSPartsHandler parts{testString};
        CHECK(parts.getPartsCount() == 2);
        CHECK(parts.getNextSmsPart().length() <= singleSMSLimitUCS2);
        CHECK(parts.getNextSmsPart().length() <= singleSMSLimitUCS2);
    }

    SECTION("Third")
    {
        std::string testString = "Hello everyone! Mudita Pure test today? I propose a meeting at 3:30PM " +
                                 winkEmojiUTF + ". Extra random text.";
        SMSPartsHandler parts{testString};
        CHECK(parts.getPartsCount() == 2);
        CHECK(parts.getNextSmsPart().length() <= singleSMSLimitUCS2);
        CHECK(parts.getNextSmsPart().length() <= singleSMSLimitUCS2);
    }

    SECTION("Fourth")
    {
        std::string testString =
            "Hello! Mudita Pure test today? I propose a meeting at 3:30PM " + winkEmojiUTF + ". Extra random text.";
        SMSPartsHandler parts{testString};
        CHECK(parts.getPartsCount() == 2);
        CHECK(parts.getNextSmsPart().length() <= singleSMSLimitUCS2);
        CHECK(parts.getNextSmsPart().length() <= singleSMSLimitUCS2);
    }
}