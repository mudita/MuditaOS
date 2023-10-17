// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include <log/log.hpp>
#include <string>

TEST_CASE("Log tests")
{
    const auto lineBufferSize = log_getMaxLineLength();

    const int value                   = -423;
    const char *carray            = "carray";
    const std::string str             = "string";
    const double double_value         = 6.5323;
    const unsigned int unsigned_value = 7821;
    char big_array[lineBufferSize + 2];
    memset(big_array, 'X', sizeof(big_array));
    big_array[sizeof(big_array) - 1] = '\0';

    const auto loggerBufferSize = static_cast<int>(lineBufferSize);
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
