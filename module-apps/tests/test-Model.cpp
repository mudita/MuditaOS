// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include <apps-common/widgets/spinners/Model.hpp>

TEST_CASE("Model")
{
    SECTION("Empty")
    {
        using UINT8Model = Model<std::uint8_t>;

        auto container = UINT8Model{{}, UINT8Model::Boundaries::Fixed};

        REQUIRE(container.get() == 0);
    }

    SECTION("Force underlying structure")
    {
        using ForcedContainer = Model<std::uint8_t, true>;

        auto container = ForcedContainer{{5, 10, 2, 30, 25}, ForcedContainer::Boundaries::Fixed};

        REQUIRE(container.get() == 5);
        container.set(25);
        REQUIRE(container.get() == 25);
        container.set(2);
        REQUIRE(container.get() == 2);
    }

    SECTION("Fundamental types")
    {
        using UINT8Model      = Model<std::uint8_t>;
        auto range            = UINT8Model::range{0, 10, 1};
        const auto boundaries = UINT8Model::Boundaries::Fixed;
        SECTION("set")
        {
            auto container = UINT8Model{range, boundaries};

            REQUIRE(container.get() == 0);
            container.set(5);
            REQUIRE(container.get() == 5);
        }

        SECTION("check step")
        {
            auto container = UINT8Model{UINT8Model::range{0, 6, 3}, boundaries};

            REQUIRE(container.get() == 0);
            container.next();
            REQUIRE(container.get() == 3);
            container.previous();
            REQUIRE(container.get() == 0);
        }

        SECTION("Check boundaries")
        {
            SECTION("Fixed")
            {
                auto container = UINT8Model{range, boundaries};

                REQUIRE(container.get() == 0);
                container.previous();
                REQUIRE(container.get() == 0);

                container.set(10);
                REQUIRE(container.get() == 10);
                container.next();
                REQUIRE(container.get() == 10);
            }

            SECTION("Continuous")
            {
                auto container = UINT8Model{range, UINT8Model::Boundaries::Continuous};

                REQUIRE(container.get() == 0);
                container.previous();
                REQUIRE(container.get() == 10);

                container.set(10);
                REQUIRE(container.get() == 10);
                container.next();
                REQUIRE(container.get() == 0);
            }
        }

        SECTION("is_min/is_max")
        {
            auto container = UINT8Model{range, boundaries};

            REQUIRE(container.is_min());
            REQUIRE_FALSE(container.is_max());

            container.next();
            REQUIRE_FALSE(container.is_min());
            REQUIRE_FALSE(container.is_max());

            container.set(10);
            REQUIRE_FALSE(container.is_min());
            REQUIRE(container.is_max());
        }

        SECTION("set_range")
        {
            auto container = UINT8Model{range, boundaries};

            REQUIRE(container.get() == 0);

            container.set_range(UINT8Model::range{1, 3, 1});
            REQUIRE(container.get() == 1);
        }
    }

    SECTION("Complex types")
    {
        using ComplexContainer = Model<std::string>;
        const auto one         = "one";
        const auto two         = "two";
        const auto three       = "three";
        auto range             = ComplexContainer::range{one, two, three};
        const auto boundaries  = ComplexContainer::Boundaries::Fixed;
        SECTION("set")
        {

            auto container = ComplexContainer{range, boundaries};

            REQUIRE(container.get() == one);
            container.set(two);
            REQUIRE(container.get() == two);
        }

        SECTION("check step")
        {
            auto container = ComplexContainer{range, boundaries};

            REQUIRE(container.get() == one);
            container.next();
            REQUIRE(container.get() == two);
        }

        SECTION("Check boundaries")
        {
            SECTION("Fixed")
            {
                auto container = ComplexContainer{range, boundaries};

                REQUIRE(container.get() == one);
                container.previous();
                REQUIRE(container.get() == one);

                container.set(three);
                REQUIRE(container.get() == three);
                container.next();
                REQUIRE(container.get() == three);
            }

            SECTION("Continuous")
            {
                auto container = ComplexContainer{range, ComplexContainer::Boundaries::Continuous};

                REQUIRE(container.get() == one);
                container.previous();
                REQUIRE(container.get() == three);

                container.set(three);
                REQUIRE(container.get() == three);
                container.next();
                REQUIRE(container.get() == one);
            }
        }

        SECTION("is_min/is_max")
        {
            auto container = ComplexContainer{range, boundaries};

            REQUIRE(container.is_min());
            REQUIRE_FALSE(container.is_max());

            container.next();
            REQUIRE_FALSE(container.is_min());
            REQUIRE_FALSE(container.is_max());

            container.set(three);
            REQUIRE_FALSE(container.is_min());
            REQUIRE(container.is_max());
        }

        SECTION("set_range")
        {
            auto container = ComplexContainer{range, boundaries};

            REQUIRE(container.get() == one);

            container.set_range(ComplexContainer::range{"two", "three", "four"});
            REQUIRE(container.get() == two);
        }
    }
}
