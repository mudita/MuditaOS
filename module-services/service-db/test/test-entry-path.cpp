// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <map>
#include <service-db/EntryPath.hpp>

using namespace settings;

TEST_CASE("Entry Path - is less comparison - different settings scope")
{
    EntryPath lhs{"mode", "service", "profile", "variable", SettingsScope::Global};
    EntryPath rhs{"mode", "service", "profile", "variable", SettingsScope::AppLocal};
    REQUIRE(lhs < rhs);
}

TEST_CASE("Entry Path - is less comparison - Global scope")
{
    SECTION("is lesser")
    {
        EntryPath lhs1{"mode", "service", "profile", "variableA", SettingsScope::Global};
        EntryPath rhs1{"mode", "service", "profile", "variableB", SettingsScope::Global};
        REQUIRE(lhs1 < rhs1);
    }

    SECTION("is equal")
    {
        EntryPath lhs2{"mode", "service", "profile", "variableA", SettingsScope::Global};
        EntryPath rhs2{"mode", "service", "profile", "variableA", SettingsScope::Global};
        REQUIRE(!(lhs2 < rhs2));
    }

    SECTION("is greater")
    {
        EntryPath lhs3{"mode", "service", "profile", "variableB", SettingsScope::Global};
        EntryPath rhs3{"mode", "service", "profile", "variableA", SettingsScope::Global};
        REQUIRE(!(lhs3 < rhs3));
    }
}

TEST_CASE("Entry Path - is less comparison - AppLocal scope")
{
    SECTION("All parts are lesser")
    {
        EntryPath lhs1{"modeA", "serviceA", "profileA", "variableA", SettingsScope::AppLocal};
        EntryPath rhs1{"modeB", "serviceB", "profileB", "variableB", SettingsScope::AppLocal};
        REQUIRE(lhs1 < rhs1);
    }

    SECTION("All parts are equal")
    {
        EntryPath lhs2{"modeA", "serviceA", "profileA", "variableA", SettingsScope::AppLocal};
        EntryPath rhs2{"modeA", "serviceA", "profileA", "variableA", SettingsScope::AppLocal};
        REQUIRE(!(lhs2 < rhs2));
    }

    SECTION("One part is lesser, others are equal")
    {
        EntryPath lhs3{"modeA", "serviceA", "profileA", "variableA", SettingsScope::AppLocal};
        EntryPath rhs3{"modeA", "serviceB", "profileA", "variableA", SettingsScope::AppLocal};
        REQUIRE(lhs3 < rhs3);
    }

    SECTION("Lesser part before greater part")
    {
        EntryPath lhs4{"modeA", "serviceA", "profileC", "variableA", SettingsScope::AppLocal};
        EntryPath rhs4{"modeB", "serviceA", "profileA", "variableA", SettingsScope::AppLocal};
        REQUIRE(lhs4 < rhs4);
    }
}

TEST_CASE("Entry Path - std::map::find - EGD-6486")
{
    EntryPath TestValGlobal1{"", "", "", "variableA", SettingsScope::Global};
    EntryPath TestValGlobal1Full{"Mode", "Service", "Profile", "variableA", SettingsScope::Global};
    EntryPath TestValLocal1{"", "", "", "variableA", SettingsScope::AppLocal};
    EntryPath TestValLocalBFull{"modeA", "serviceA", "profileC", "variableB", SettingsScope::AppLocal};

    std::map<EntryPath, std::string> map = {
        {TestValGlobal1, "1"},
        {TestValGlobal1, "2"},
        {TestValLocal1, "3"},
        {TestValLocalBFull, "3"},
    };

    REQUIRE(map.size() == 3);

    SECTION("Find global variable")
    {
        auto it1 = map.find(TestValGlobal1);
        REQUIRE(it1 != map.end());

        auto it2 = map.find(TestValGlobal1Full);
        REQUIRE(it2 != map.end());

        REQUIRE(it1 == it2);
    }

    SECTION("Find app local variable")
    {
        auto it = map.find(TestValLocalBFull);
        REQUIRE(it != map.end());
    }

    SECTION("Find app local variable - and erase")
    {
        auto it = map.find(TestValLocal1);
        REQUIRE(it != map.end());
        map.erase(it);

        auto it1 = map.find(TestValGlobal1);
        REQUIRE(it1 != map.end());
    }

    SECTION("Find app Global variable - and erase")
    {
        auto it1 = map.find(TestValGlobal1);
        REQUIRE(it1 != map.end());
        map.erase(it1);

        auto it = map.find(TestValLocal1);
        REQUIRE(it != map.end());
    }
}
