// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <SMSParser.hpp>
#include <module-cellular/at/Result.hpp>
#include <ucs2/UCS2.hpp>

TEST_CASE("SMSParseHandler functionality")
{
    auto prepareQCMGRFrame =
        [](const UCS2 &number, std::uint16_t uid = 0, std::uint8_t current = 0, std::uint8_t total = 0) {
            std::string qcmgrFrame =
                R"(+QCMGR: "REC UNREAD",")" + std::string(number.str()) + R"(",,"22/09/16,14:49:11+08")";
            if (uid > 0) {
                qcmgrFrame += "," + std::to_string(uid) + "," + std::to_string(current) + "," + std::to_string(total);
            }
            return qcmgrFrame;
        };

    SECTION("Prepare single sms frame")
    {
        UTF8 number("+48123456789");
        UCS2 ucs2number(number);
        std::string qcmgr =
            R"(+QCMGR: "REC UNREAD","002B00340038003100320033003400350036003700380039",,"22/09/16,14:49:11+08")";

        REQUIRE(prepareQCMGRFrame(ucs2number) == qcmgr);
    }

    SECTION("Prepare multiple sms frame")
    {
        UTF8 number("+48123456789");
        UCS2 ucs2number(number);
        std::string qcmgr =
            R"(+QCMGR: "REC UNREAD","002B00340038003100320033003400350036003700380039",,"22/09/16,14:49:11+08",79,2,4)";

        REQUIRE(prepareQCMGRFrame(ucs2number, 79, 2, 4) == qcmgr);
    }

    SECTION("Wrong response")
    {
        std::vector<std::string> resp;
        resp.emplace_back("Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris.");
        at::Result result(at::Result::Code::OK, resp);

        bool messageParsed = SMSParser::parse(&result.response[0]);
        REQUIRE_FALSE(messageParsed);
    }

    SECTION("Single SMS")
    {
        UTF8 number("+48123456789");
        UTF8 message("Hello World 1234567890 +-*/=");
        UCS2 ucs2number(number);

        std::vector<std::string> resp;
        resp.emplace_back(prepareQCMGRFrame(ucs2number));
        resp.emplace_back(std::string(UCS2(message).str()));
        at::Result result(at::Result::Code::OK, resp);

        bool messageParsed = SMSParser::parse(&result.response[0]);
        REQUIRE(messageParsed);

        UTF8 decodedNumber = UCS2(SMSParser::getNumber()).toUTF8();
        REQUIRE(decodedNumber == number);

        UTF8 decodedMessage = UCS2(SMSParser::getMessage()).toUTF8();
        REQUIRE(decodedMessage == message);
    }

    SECTION("Concatenated SMS")
    {
        UTF8 number("+01222333444");
        UCS2 ucs2number(number);

        UTF8 message1("Lorem ipsum dolor sit amet, consectetur adipiscing elit");
        UTF8 message2(", sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.");
        UTF8 message3("Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea "
                      "commodo consequat.");
        UTF8 message4(
            "Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.");

        std::vector<std::string> resp;
        resp.emplace_back(prepareQCMGRFrame(ucs2number, 34, 1, 4));
        resp.emplace_back(std::string(UCS2(message1).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number, 34, 2, 4));
        resp.emplace_back(std::string(UCS2(message2).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number, 34, 3, 4));
        resp.emplace_back(std::string(UCS2(message3).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number, 34, 4, 4));
        resp.emplace_back(std::string(UCS2(message4).str()));
        at::Result result(at::Result::Code::OK, resp);

        for (size_t i = 0; i < resp.size(); ++i) {
            bool messageParsed = SMSParser::parse(&result.response[i]);
            (void)messageParsed;
        }

        UTF8 decodedNumber = UCS2(SMSParser::getNumber()).toUTF8();
        REQUIRE(decodedNumber == number);

        UTF8 decodedMessage = UCS2(SMSParser::getMessage()).toUTF8();
        UTF8 message        = message1 + message2 + message3 + message4;
        REQUIRE(decodedMessage == message);
    }

    SECTION("Concatenated and single in the middle SMS")
    {
        UTF8 number1("+01222333444");
        UCS2 ucs2number1(number1);

        UTF8 message11("Excepteur sint occaecat cupidatat non proident,");
        UTF8 message12("sunt in culpa qui officia deserunt mollit anim id est laborum.");
        UTF8 message13("Sed ut perspiciatis unde omnis iste natus error sit voluptatem");

        UTF8 number2("+99000111222");
        UCS2 ucs2number2(number2);
        UTF8 message21("Totam rem aperiam, eaque ipsa quae ab illo inventore veritatis et quasi architecto beatae "
                       "vitae dicta sunt explicabo");

        std::vector<std::string> resp;
        resp.emplace_back(prepareQCMGRFrame(ucs2number1, 34, 1, 3));
        resp.emplace_back(std::string(UCS2(message11).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number1, 34, 2, 3));
        resp.emplace_back(std::string(UCS2(message12).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number2));
        resp.emplace_back(std::string(UCS2(message21).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number1, 34, 3, 3));
        resp.emplace_back(std::string(UCS2(message13).str()));
        at::Result result(at::Result::Code::OK, resp);

        for (size_t i = 0; i < resp.size(); ++i) {
            bool messageParsed = SMSParser::parse(&result.response[i]);
            if (messageParsed) {
                UTF8 decodedMessage = UCS2(SMSParser::getMessage()).toUTF8();
                UTF8 decodedNumber  = UCS2(SMSParser::getNumber()).toUTF8();
                if (decodedNumber == number1) {
                    UTF8 message = message11 + message12 + message13;
                    REQUIRE(decodedMessage == message);
                }
                else if (decodedNumber == number2) {
                    REQUIRE(decodedMessage == message21);
                }
            }
        }
    }

    SECTION("Mixed two concatenated SMS")
    {
        UTF8 number1("+2334567812");
        UCS2 ucs2number1(number1);
        UTF8 message11("Nemo enim ipsam voluptatem quia voluptas sit aspernatur aut odit aut fugit,");
        UTF8 message12("sed quia consequuntur magni dolores eos qui ratione voluptatem sequi nesciunt.");
        UTF8 message13("Neque porro quisquam est, qui dolorem ipsum quia dolor sit amet, consectetur,");
        UTF8 message14("adipisci velit, sed quia non numquam eius modi tempora incidunt ut labore et dolore magnam "
                       "aliquam quaerat voluptatem.");
        UTF8 message15("Ut enim ad minima veniam, quis nostrum exercitationem ullam corporis suscipit laboriosam,");
        UTF8 message16(" nisi ut aliquid ex ea commodi consequatur?");

        UTF8 number2("+12998877661");
        UCS2 ucs2number2(number2);
        UTF8 message21(
            "Quis autem vel eum iure reprehenderit qui in ea voluptate velit esse quam nihil molestiae consequatur,");
        UTF8 message22("vel illum qui dolorem eum fugiat quo voluptas nulla pariatur?");
        UTF8 message23("At vero eos et accusamus et iusto odio dignissimos ducimus qui ");
        UTF8 message24("blanditiis praesentium voluptatum deleniti atque corrupti quos dolores et quas molestias");
        UTF8 message25("excepturi sint occaecati cupiditate non provident, similique sunt in culpa qui officia "
                       "deserunt mollitia animi");

        std::vector<std::string> resp;
        resp.emplace_back(prepareQCMGRFrame(ucs2number1, 12, 1, 6));
        resp.emplace_back(std::string(UCS2(message11).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number1, 12, 2, 6));
        resp.emplace_back(std::string(UCS2(message12).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number2, 56, 1, 5));
        resp.emplace_back(std::string(UCS2(message21).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number2, 56, 2, 5));
        resp.emplace_back(std::string(UCS2(message22).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number1, 12, 3, 6));
        resp.emplace_back(std::string(UCS2(message13).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number2, 56, 3, 5));
        resp.emplace_back(std::string(UCS2(message23).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number2, 56, 4, 5));
        resp.emplace_back(std::string(UCS2(message24).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number2, 56, 5, 5));
        resp.emplace_back(std::string(UCS2(message25).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number1, 12, 4, 6));
        resp.emplace_back(std::string(UCS2(message14).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number1, 12, 5, 6));
        resp.emplace_back(std::string(UCS2(message15).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number1, 12, 6, 6));
        resp.emplace_back(std::string(UCS2(message16).str()));
        at::Result result(at::Result::Code::OK, resp);

        for (size_t i = 0; i < resp.size(); ++i) {
            bool messageParsed = SMSParser::parse(&result.response[i]);
            if (messageParsed) {
                UTF8 decodedMessage = UCS2(SMSParser::getMessage()).toUTF8();
                UTF8 decodedNumber  = UCS2(SMSParser::getNumber()).toUTF8();
                if (decodedNumber == number1) {
                    UTF8 message = message11 + message12 + message13 + message14 + message15 + message16;
                    REQUIRE(decodedMessage == message);
                }
                else if (decodedNumber == number2) {
                    UTF8 message = message21 + message22 + message23 + message24 + message25;
                    REQUIRE(decodedMessage == message);
                }
            }
        }
    }

    SECTION("Mixed order of two concatenated SMS")
    {
        UTF8 number1("+2334567812");
        UCS2 ucs2number1(number1);
        UTF8 message11("Nemo enim ipsam voluptatem quia voluptas sit aspernatur aut odit aut fugit,");
        UTF8 message12("sed quia consequuntur magni dolores eos qui ratione voluptatem sequi nesciunt.");
        UTF8 message13("Neque porro quisquam est, qui dolorem ipsum quia dolor sit amet, consectetur,");
        UTF8 message14("adipisci velit, sed quia non numquam eius modi tempora incidunt ut labore et dolore magnam "
                       "aliquam quaerat voluptatem.");
        UTF8 message15("Ut enim ad minima veniam, quis nostrum exercitationem ullam corporis suscipit laboriosam,");
        UTF8 message16(" nisi ut aliquid ex ea commodi consequatur?");

        UTF8 number2("+12998877661");
        UCS2 ucs2number2(number2);
        UTF8 message21(
            "Quis autem vel eum iure reprehenderit qui in ea voluptate velit esse quam nihil molestiae consequatur,");
        UTF8 message22("vel illum qui dolorem eum fugiat quo voluptas nulla pariatur?");
        UTF8 message23("At vero eos et accusamus et iusto odio dignissimos ducimus qui ");
        UTF8 message24("blanditiis praesentium voluptatum deleniti atque corrupti quos dolores et quas molestias");
        UTF8 message25("excepturi sint occaecati cupiditate non provident, similique sunt in culpa qui officia "
                       "deserunt mollitia animi");

        std::vector<std::string> resp;
        resp.emplace_back(prepareQCMGRFrame(ucs2number2, 56, 3, 5));
        resp.emplace_back(std::string(UCS2(message23).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number1, 12, 2, 6));
        resp.emplace_back(std::string(UCS2(message12).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number1, 12, 6, 6));
        resp.emplace_back(std::string(UCS2(message16).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number1, 12, 3, 6));
        resp.emplace_back(std::string(UCS2(message13).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number2, 56, 4, 5));
        resp.emplace_back(std::string(UCS2(message24).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number2, 56, 5, 5));
        resp.emplace_back(std::string(UCS2(message25).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number2, 56, 2, 5));
        resp.emplace_back(std::string(UCS2(message22).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number1, 12, 5, 6));
        resp.emplace_back(std::string(UCS2(message15).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number2, 56, 1, 5));
        resp.emplace_back(std::string(UCS2(message21).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number1, 12, 4, 6));
        resp.emplace_back(std::string(UCS2(message14).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number1, 12, 1, 6));
        resp.emplace_back(std::string(UCS2(message11).str()));

        at::Result result(at::Result::Code::OK, resp);

        for (size_t i = 0; i < resp.size(); ++i) {
            bool messageParsed = SMSParser::parse(&result.response[i]);
            if (messageParsed) {
                UTF8 decodedMessage = UCS2(SMSParser::getMessage()).toUTF8();
                UTF8 decodedNumber  = UCS2(SMSParser::getNumber()).toUTF8();
                if (decodedNumber == number1) {
                    UTF8 message = message11 + message12 + message13 + message14 + message15 + message16;
                    REQUIRE(decodedMessage == message);
                }
                else if (decodedNumber == number2) {
                    UTF8 message = message21 + message22 + message23 + message24 + message25;
                    REQUIRE(decodedMessage == message);
                }
            }
        }
    }

    SECTION("Mixed concatenated and single SMS")
    {
        UTF8 number1("+2334567812");
        UCS2 ucs2number1(number1);
        UTF8 message11("Id est laborum et dolorum fuga.");
        UTF8 message12("Nam libero tempore, cum soluta nobis est eligendi optio cumque nihil impedit quo minus id");
        UTF8 message13("quod maxime placeat facere possimus, omnis voluptas assumenda est.");

        UTF8 number2("+12998877661");
        UCS2 ucs2number2(number2);
        UTF8 message21("Temporibus autem quibusdam et aut officiis debitis aut rerum necessitatibus saepe eveniet");
        UTF8 message22("ut et voluptates repudiandae sint et molestiae non recusandae.");
        UTF8 message23("Itaque earum rerum hic tenetur a sapiente delectus,");
        UTF8 message24(
            " ut aut reiciendis voluptatibus maiores alias consequatur aut perferendis doloribus asperiores repellat.");

        UTF8 number3("+67453423143");
        UCS2 ucs2number3(number3);
        UTF8 message3("Aenean magna ligula, hendrerit non tristique sed, auctor id ex.");

        UTF8 number4("+89010203040");
        UCS2 ucs2number4(number4);
        UTF8 message4(
            "Donec tincidunt nibh at odio iaculis accumsan. Aliquam mollis metus vitae mauris molestie scelerisque.");

        UTF8 number5("+00010020030");
        UCS2 ucs2number5(number5);
        UTF8 message5("Et harum quidem rerum facilis est et expedita distinctio.");

        std::vector<std::string> resp;
        resp.emplace_back(prepareQCMGRFrame(ucs2number1, 12, 1, 6));
        resp.emplace_back(std::string(UCS2(message11).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number1, 12, 2, 6));
        resp.emplace_back(std::string(UCS2(message12).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number2, 56, 1, 5));
        resp.emplace_back(std::string(UCS2(message21).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number2, 56, 2, 5));
        resp.emplace_back(std::string(UCS2(message22).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number4));
        resp.emplace_back(std::string(UCS2(message4).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number1, 12, 3, 6));
        resp.emplace_back(std::string(UCS2(message13).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number2, 56, 3, 5));
        resp.emplace_back(std::string(UCS2(message23).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number3));
        resp.emplace_back(std::string(UCS2(message3).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number2, 56, 4, 5));
        resp.emplace_back(std::string(UCS2(message24).str()));
        resp.emplace_back(prepareQCMGRFrame(ucs2number5));
        resp.emplace_back(std::string(UCS2(message5).str()));
        at::Result result(at::Result::Code::OK, resp);

        for (size_t i = 0; i < resp.size(); ++i) {
            bool messageParsed = SMSParser::parse(&result.response[i]);
            if (messageParsed) {
                UTF8 decodedMessage = UCS2(SMSParser::getMessage()).toUTF8();
                UTF8 decodedNumber  = UCS2(SMSParser::getNumber()).toUTF8();
                if (decodedNumber == number1) {
                    UTF8 message = message11 + message12 + message13;
                    REQUIRE(decodedMessage == message);
                }
                else if (decodedNumber == number2) {
                    UTF8 message = message21 + message22 + message23 + message24;
                    REQUIRE(decodedMessage == message);
                }
                else if (decodedNumber == number3) {
                    REQUIRE(decodedMessage == message3);
                }
                else if (decodedNumber == number4) {
                    REQUIRE(decodedMessage == message4);
                }
                else if (decodedNumber == number5) {
                    REQUIRE(decodedMessage == message5);
                }
            }
        }
    }
}
