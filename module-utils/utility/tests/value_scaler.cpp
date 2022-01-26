// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include <ValueScaler.hpp>

#include <climits>

using namespace utils;

TEST_CASE("Linear approximation")
{
    SECTION("Corner cases")
    {
        SECTION("Unsigned")
        {
            constexpr auto entries = std::array<Entry<std::uint32_t>, 1>{{{{100, 100}, {100, 100}}}};
            REQUIRE_FALSE(find_and_scale_value(entries, 0U));
            REQUIRE(find_and_scale_value(entries, 100U).value() == 100);
            REQUIRE_FALSE(find_and_scale_value(entries, 101U));
        }

        SECTION("Signed")
        {
            constexpr auto entries = std::array<Entry<std::int32_t>, 1>{{{{-100, -100}, {-100, -100}}}};
            REQUIRE_FALSE(find_and_scale_value(entries, 0));
            REQUIRE(find_and_scale_value(entries, -100).value() == -100);
            REQUIRE_FALSE(find_and_scale_value(entries, 101));
            REQUIRE_FALSE(find_and_scale_value(entries, -101));
        }

        SECTION("Float")
        {
            constexpr auto entries = std::array<Entry<float>, 1>{{{{-100, -100}, {-100, -100}}}};
            REQUIRE_FALSE(find_and_scale_value(entries, 0.0f));
            REQUIRE(find_and_scale_value(entries, -100.0f).value() == -100.0f);
            REQUIRE_FALSE(find_and_scale_value(entries, 101.0f));
            REQUIRE_FALSE(find_and_scale_value(entries, -101.0f));
        }

        SECTION("Basic use case")
        {
            constexpr auto entries = std::array<Entry<std::int32_t>, 8>{{{{-10, -1}, {0, 0}},
                                                                         {{0, 5}, {1, 1}},
                                                                         {{6, 10}, {2, 5}},
                                                                         {{11, 30}, {6, 29}},
                                                                         {{31, 50}, {30, 53}},
                                                                         {{51, 70}, {54, 77}},
                                                                         {{71, 99}, {78, 99}},
                                                                         {{100, 100}, {100, 100}}}};

            REQUIRE_FALSE(find_and_scale_value(entries, -11));
            REQUIRE(find_and_scale_value(entries, -10).value() == 0);
            REQUIRE(find_and_scale_value(entries, -1).value() == 0);
            REQUIRE(find_and_scale_value(entries, 0).value() == 1);
            REQUIRE(find_and_scale_value(entries, 6).value() == 2);
            REQUIRE(find_and_scale_value(entries, 10).value() == 5);
            REQUIRE(find_and_scale_value(entries, 99).value() == 99);
        }
    }
}
