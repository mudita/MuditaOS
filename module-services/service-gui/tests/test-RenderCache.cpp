// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include "RenderCache.hpp"

using namespace service::gui;

TEST_CASE("Render cache - default initialized")
{
    RenderCache cache;

    REQUIRE(!cache.isRenderCached());
}

TEST_CASE("Render cache - cache")
{
    RenderCache cache;

    cache.cache({1, ::gui::RefreshModes::GUI_REFRESH_DEEP});

    REQUIRE(cache.isRenderCached());
    REQUIRE(cache.getCachedRender()->contextId == 1);
    REQUIRE(cache.getCachedRender()->refreshMode == ::gui::RefreshModes::GUI_REFRESH_DEEP);
}

TEST_CASE("Render cache - cache and invalidate")
{
    RenderCache cache;

    cache.cache({1, ::gui::RefreshModes::GUI_REFRESH_DEEP});
    cache.invalidate();

    REQUIRE(!cache.isRenderCached());
}

TEST_CASE("Render cache - exchange cached item")
{
    RenderCache cache;

    cache.cache({1, ::gui::RefreshModes::GUI_REFRESH_DEEP});
    cache.cache({2, ::gui::RefreshModes::GUI_REFRESH_FAST});

    REQUIRE(cache.isRenderCached());
    REQUIRE(cache.getCachedRender()->contextId == 2);
    REQUIRE(cache.getCachedRender()->refreshMode == ::gui::RefreshModes::GUI_REFRESH_DEEP);
}
