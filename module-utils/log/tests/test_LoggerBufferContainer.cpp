// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <LoggerBufferContainer.hpp>

TEST_CASE("Test if proper buffer is chosen")
{
    LoggerBufferContainer buffer;

    size_t bufferIndex = buffer.getCurrentIndex();
    REQUIRE(bufferIndex == 0);

    buffer.nextBuffer();
    bufferIndex = buffer.getCurrentIndex();
    REQUIRE(bufferIndex == 1);

    buffer.nextBuffer();
    bufferIndex = buffer.getCurrentIndex();
    REQUIRE(bufferIndex == 0);

    buffer.nextBuffer();
    bufferIndex = buffer.getCurrentIndex();
    REQUIRE(bufferIndex == 1);
}
