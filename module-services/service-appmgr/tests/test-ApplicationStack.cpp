// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include <service-appmgr/model/ApplicationStack.hpp>

using namespace app::manager;

TEST_CASE("ApplicationStack - push")
{
    ApplicationStack stack;
    SECTION("First push")
    {
        stack.push({"A", false});
        REQUIRE(stack.size() == 1);
    }
    SECTION("Multiple pushes")
    {
        stack.push({"A", false});
        stack.push({"B", false});
        stack.push({"A", false});
        REQUIRE(stack.size() == 3);
    }
}

TEST_CASE("ApplicationStack - pop")
{
    SECTION("Empty stack")
    {
        ApplicationStack stack;
        stack.pop();
        REQUIRE(stack.isEmpty());
    }
    SECTION("Non-empty stack")
    {
        ApplicationStack stack;
        stack.push({"A", false});
        REQUIRE(!stack.isEmpty());

        stack.pop();
        REQUIRE(stack.isEmpty());
    }
}

TEST_CASE("ApplicationStack - erase")
{
    SECTION("Empty stack")
    {
        ApplicationStack stack;
        stack.eraseFirstOf("A");
        REQUIRE(stack.isEmpty());
    }
    SECTION("Non-empty stack")
    {
        ApplicationStack stack;
        stack.push({"A", false});
        stack.push({"B", false});
        stack.push({"C", false});

        stack.eraseFirstOf("B");
        REQUIRE(stack.size() == 2);
        REQUIRE(stack.front().appName == "C");
    }
}

TEST_CASE("ApplicationStack - clear")
{
    SECTION("Empty stack")
    {
        ApplicationStack stack;
        stack.clear();
        REQUIRE(stack.isEmpty());
    }
    SECTION("Non-empty stack")
    {
        ApplicationStack stack;
        stack.push({"A", false});
        stack.push({"B", false});
        stack.push({"C", false});
        REQUIRE(stack.size() == 3);

        stack.clear();
        REQUIRE(stack.isEmpty());
    }
}

TEST_CASE("ApplicationStack - contains")
{
    SECTION("Empty stack")
    {
        ApplicationStack stack;
        REQUIRE(!stack.contains("A"));
    }
    SECTION("Non-empty stack")
    {
        ApplicationStack stack;
        stack.push({"A", false});
        stack.push({"B", false});

        REQUIRE(stack.contains("A"));
        REQUIRE(!stack.contains("C"));
    }
}

TEST_CASE("ApplicationStack - find")
{
    SECTION("Empty stack")
    {
        ApplicationStack stack;
        const auto &appsA = stack.find("A");
        REQUIRE(appsA.empty());
    }
    SECTION("Non-empty stack")
    {
        ApplicationStack stack;
        stack.push({"A", false});
        stack.push({"B", false});
        stack.push({"A", false});
        stack.push({"A", false});

        const auto &appsA = stack.find("A");
        REQUIRE(appsA.size() == 3);

        const auto &appsB = stack.find("B");
        REQUIRE(appsB.size() == 1);

        const auto &appsC = stack.find("C");
        REQUIRE(appsC.empty());
    }
}

TEST_CASE("ApplicationStack - getIndexOf")
{
    ApplicationStack stack;
    SECTION("Empty stack")
    {
        REQUIRE(stack.getIndexOf("A") == -1);
    }
    SECTION("One element - get index")
    {
        stack.push({"A", false});
        REQUIRE(stack.getIndexOf("A") == 0);
    }
    SECTION("Multiple elements - get index of backmost")
    {
        stack.push({"A", false});
        stack.push({"B", false});
        stack.push({"C", false});
        REQUIRE(stack.getIndexOf("A") == 2);
    }
    SECTION("Multiple elements - get index of topmost")
    {
        stack.push({"A", false});
        stack.push({"B", false});
        stack.push({"C", false});
        stack.push({"A", false});
        REQUIRE(stack.getIndexOf("A") == 0);
    }
    SECTION("Multiple elements - get index of previous to topmost")
    {
        stack.push({"A", false});
        stack.push({"B", false});
        stack.push({"C", false});
        stack.push({"A", false});
        REQUIRE(stack.getIndexOf("C") == 1);
    }
}
