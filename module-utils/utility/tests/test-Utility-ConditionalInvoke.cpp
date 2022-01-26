// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include <Utility.hpp>

struct SomeStruct
{
    int value;
};

class ExampleClass
{
  public:
    int function1(int x, const SomeStruct &s)
    {
        return x + s.value;
    }

    void function2()
    {}

    int function3(int x, SomeStruct &s)
    {
        return x + s.value;
    }

    static int static_function1(int x, const SomeStruct &s)
    {
        return x + s.value;
    }

    static void static_function2()
    {}

    static int static_function3(int x, SomeStruct &s)
    {
        return x + s.value;
    }
};

int global_function1(int x, const SomeStruct &s)
{
    return x + s.value;
}

void global_function2()
{}

int global_function3(int x, SomeStruct &s)
{
    return x + s.value;
}

TEST_CASE("Successful global function call")
{
    const auto guard = []() { return true; };

    const auto [called1, ret1] = utility::conditionally_invoke(guard, &global_function1, 10, SomeStruct{5});
    REQUIRE(called1 == true);
    REQUIRE(ret1 == 15);

    int x = 10;
    SomeStruct s1{10};
    const auto [called2, ret2] = utility::conditionally_invoke(guard, &global_function1, x, s1);
    REQUIRE(called2 == true);
    REQUIRE(ret2 == 20);

    const auto called3 = utility::conditionally_invoke(guard, &global_function2);
    REQUIRE(called3 == true);

    SomeStruct s2{15};
    const auto [called4, ret4] = utility::conditionally_invoke(guard, &global_function3, 10, std::ref(s2));
    REQUIRE(called4 == true);
    REQUIRE(ret4 == 25);
}

TEST_CASE("Failed global function call")
{
    const auto guard = []() { return false; };

    const auto [called1, ret1] = utility::conditionally_invoke(guard, &global_function1, 10, SomeStruct{5});
    REQUIRE(called1 == false);
    REQUIRE(ret1 == 0);

    int x = 10;
    SomeStruct s1{10};
    const auto [called2, ret2] = utility::conditionally_invoke(guard, &global_function1, x, s1);
    REQUIRE(called2 == false);
    REQUIRE(ret2 == 0);

    const auto called3 = utility::conditionally_invoke(guard, &global_function2);
    REQUIRE(called3 == false);

    SomeStruct s2{15};
    const auto [called4, ret4] = utility::conditionally_invoke(guard, &global_function3, 10, std::ref(s2));
    REQUIRE(called4 == false);
    REQUIRE(ret4 == 0);
}

TEST_CASE("Successful class static function call")
{
    const auto guard = []() { return true; };

    const auto [called1, ret1] =
        utility::conditionally_invoke(guard, &ExampleClass::static_function1, 10, SomeStruct{5});
    REQUIRE(called1 == true);
    REQUIRE(ret1 == 15);

    int x = 10;
    SomeStruct s1{10};
    const auto [called2, ret2] = utility::conditionally_invoke(guard, &ExampleClass::static_function1, x, s1);
    REQUIRE(called2 == true);
    REQUIRE(ret2 == 20);

    const auto called3 = utility::conditionally_invoke(guard, &ExampleClass::static_function2);
    REQUIRE(called3 == true);

    SomeStruct s2{15};
    const auto [called4, ret4] =
        utility::conditionally_invoke(guard, &ExampleClass::static_function3, 10, std::ref(s2));
    REQUIRE(called4 == true);
    REQUIRE(ret4 == 25);
}

TEST_CASE("Failed class static function call")
{
    const auto guard = []() { return false; };

    const auto [called1, ret1] =
        utility::conditionally_invoke(guard, &ExampleClass::static_function1, 10, SomeStruct{5});
    REQUIRE(called1 == false);
    REQUIRE(ret1 == 0);

    int x = 10;
    SomeStruct s1{10};
    const auto [called2, ret2] = utility::conditionally_invoke(guard, &ExampleClass::static_function1, x, s1);
    REQUIRE(called2 == false);
    REQUIRE(ret2 == 0);

    const auto called3 = utility::conditionally_invoke(guard, &ExampleClass::static_function2);
    REQUIRE(called3 == false);

    SomeStruct s2{15};
    const auto [called4, ret4] =
        utility::conditionally_invoke(guard, &ExampleClass::static_function3, 10, std::ref(s2));
    REQUIRE(called4 == false);
    REQUIRE(ret4 == 0);
}

TEST_CASE("Successful class member function call")
{
    ExampleClass instance;
    const auto guard = []() { return true; };

    const auto [called1, ret1] =
        utility::conditionally_invoke(guard, instance, &ExampleClass::function1, 10, SomeStruct{5});
    REQUIRE(called1 == true);
    REQUIRE(ret1 == 15);

    int x = 10;
    SomeStruct s1{10};
    const auto [called2, ret2] = utility::conditionally_invoke(guard, instance, &ExampleClass::function1, x, s1);
    REQUIRE(called2 == true);
    REQUIRE(ret2 == 20);

    const auto called3 = utility::conditionally_invoke(guard, instance, &ExampleClass::function2);
    REQUIRE(called3 == true);

    SomeStruct s2{15};
    const auto [called4, ret4] =
        utility::conditionally_invoke(guard, instance, &ExampleClass::function3, 10, std::ref(s2));
    REQUIRE(called4 == true);
    REQUIRE(ret4 == 25);
}

TEST_CASE("Failed class member function call")
{
    ExampleClass instance;
    const auto guard = []() { return false; };

    const auto [called1, ret1] =
        utility::conditionally_invoke(guard, instance, &ExampleClass::function1, 10, SomeStruct{5});
    REQUIRE(called1 == false);
    REQUIRE(ret1 == 0);

    int x = 10;
    SomeStruct s1{10};
    const auto [called2, ret2] = utility::conditionally_invoke(guard, instance, &ExampleClass::function1, x, s1);
    REQUIRE(called2 == false);
    REQUIRE(ret2 == 0);

    const auto called3 = utility::conditionally_invoke(guard, instance, &ExampleClass::function2);
    REQUIRE(called3 == false);

    SomeStruct s2{15};
    const auto [called4, ret4] =
        utility::conditionally_invoke(guard, instance, &ExampleClass::function3, 10, std::ref(s2));
    REQUIRE(called4 == false);
    REQUIRE(ret4 == 0);
}
