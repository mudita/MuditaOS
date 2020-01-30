
/*
 * @file main_tests.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 14.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#include <iostream>
#include <memory>
#include <cstring>
#include <unistd.h>


#include "catch.hpp"

#include "Modem/ATParser.hpp"


TEST_CASE("Tokenizer tests")
{
    std::string delimiter = "\r\n";

    std::string inputStr1 = "\r\nOK\r\n\r\nNO CARRIER\r\n\r\nUNFINIS";
    auto ret = ATParser::Tokenizer(inputStr1, delimiter, 2);
    REQUIRE(ret.size() == 2);
    REQUIRE(ret[0] == "OK");
    REQUIRE(ret[1] == "NO CARRIER");

    inputStr1 = "\r\nOK\r\n\r\nNO CARRIER\r\n\r\nFINISHED\r\n";
    ret = ATParser::Tokenizer(inputStr1, delimiter, 3);
    REQUIRE(ret.size() == 3);
    REQUIRE(ret[0] == "OK");
    REQUIRE(ret[1] == "NO CARRIER");
    REQUIRE(ret[2] == "FINISHED");

    inputStr1 = "\r\nOK\r\n\r\nNO CARRIER\r\n\r\nFINISHED\r\n";
    ret = ATParser::Tokenizer(inputStr1, delimiter, 2);
    REQUIRE(ret.size() == 2);
    REQUIRE(ret[0] == "OK");
    REQUIRE(ret[1] == "NO CARRIER");
    REQUIRE(inputStr1 == "\r\nFINISHED\r\n");
}

