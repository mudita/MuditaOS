// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <service-cellular/src/SimCard.hpp>

TEST_CASE("cellular::utils functionality test")
{
    SECTION("pinToString from vector")
    {
        std::vector<unsigned int> v{1, 2, 3, 4};
        std::vector<unsigned int> empty;
        REQUIRE(cellular::utils::pinToString(v) == "1234");
        REQUIRE(cellular::utils::pinToString(empty).empty());
    }
}
