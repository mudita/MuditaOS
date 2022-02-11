#include <limits>
#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include "prof.h"

TEST_CASE("prof api test")
{
    struct prof_pool_init_data init{0};

    prof_pool_init(init);
    auto pp = prof_pool_get_data();
    REQUIRE(pp.size == 0);
    prof_pool_deinit();
}

TEST_CASE("overflow")
{
    struct prof_pool_init_data init
    {
        0
    };
    prof_pool_init(init);
    prof_pool_data_set(0,-1);
    REQUIRE(prof_pool_overflow() == 1);
    prof_pool_deinit();
}

TEST_CASE("prof api sum")
{
    struct prof_pool_init_data init{1};

    prof_pool_init(init);
    auto pp = prof_pool_get_data();
    REQUIRE(pp.size == 1);
    const auto switches = 10;
    const auto ts = 10;
    for (auto i =0; i < switches ; ++i)
    {
        prof_pool_data_set(0,ts);
    }

    task_prof_data mem[1];
    prof_pool_flush(mem, 1);
    REQUIRE(mem->switches == switches);
    REQUIRE(mem->exec_time == switches*ts);

    prof_pool_deinit();
}
