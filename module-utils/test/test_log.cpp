// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

#include <log.hpp>
#include <string>

using namespace std;

TEST_CASE("Log tests")
{
    int value                     = -423;
    const char *carray            = "carray";
    string str                    = "string";
    double double_value           = 6.5323;
    unsigned int unsigned_value   = 7821;
    constexpr auto big_array_size = LOGGER_BUFFER_SIZE + 2;
    char big_array[big_array_size];
    memset(big_array, 'X', big_array_size);
    big_array[big_array_size - 1] = '\0';

    int loggerBufferSize = static_cast<int>(LOGGER_BUFFER_SIZE);
    int result           = LOG_TRACE("value: %d", value);
    REQUIRE(0 < result);
    REQUIRE(result <= loggerBufferSize);
    result = LOG_DEBUG("carray: %s", carray);
    REQUIRE(0 < result);
    REQUIRE(result <= loggerBufferSize);
    result = LOG_INFO("str: %s", str.c_str());
    REQUIRE(0 < result);
    REQUIRE(result <= loggerBufferSize);
    result = LOG_WARN("double_value: %f", double_value);
    REQUIRE(0 < result);
    REQUIRE(result <= loggerBufferSize);
    result = LOG_ERROR("unsigned_value: %u", unsigned_value);
    REQUIRE(0 < result);
    REQUIRE(result <= loggerBufferSize);
    result = LOG_FATAL("big_array: %s", big_array);
    REQUIRE(0 < result);
    REQUIRE(result <= loggerBufferSize);
    result = LOG_PRINTF("value: %d, carray: %s, double_value: %f", value, carray, double_value);
    REQUIRE(0 < result);
    REQUIRE(result <= loggerBufferSize);
}
