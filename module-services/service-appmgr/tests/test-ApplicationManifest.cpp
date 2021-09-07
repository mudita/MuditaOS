// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include <service-appmgr/ApplicationManifest.hpp>

using namespace app::manager;

TEST_CASE("Check if manifest registers actions")
{
    SECTION("Check if contains registered action")
    {
        ApplicationManifest manifest{{actions::Launch}};
        REQUIRE(manifest.contains(actions::Launch));
    }
    SECTION("Check if doesn't contain an action that wasn't registered")
    {
        ApplicationManifest manifest{{actions::Launch}};
        REQUIRE(!manifest.contains(actions::AbortCall));
    }
    SECTION("Checks if doesn't contain an action when the manifest is empty")
    {
        ApplicationManifest manifest;
        REQUIRE(!manifest.contains(actions::Launch));
    }
}

TEST_CASE("Check if manifest serves action flags")
{
    ApplicationManifest manifest{{actions::Launch, {actions::AbortCall, actions::ActionFlag::AcceptWhenInBackground}}};

    REQUIRE(manifest.getActionFlag(actions::Launch) == actions::ActionFlag::None);
    REQUIRE(manifest.getActionFlag(actions::AbortCall) == actions::ActionFlag::AcceptWhenInBackground);
    REQUIRE(manifest.getActionFlag(actions::UserAction) == actions::ActionFlag::None);
}
