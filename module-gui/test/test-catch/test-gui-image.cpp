// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include <list>

#include <module-gui/gui/core/DrawCommand.hpp>
#include <module-gui/gui/core/ImageManager.hpp>
#include <module-gui/gui/widgets/Image.hpp>

TEST_CASE("Constructing with empty image name")
{
    constexpr auto imageName = "";
    gui::Image image{nullptr, imageName};

    std::list<gui::Command> commands;
    image.buildDrawListImplementation(commands);
    REQUIRE(commands.empty());
}

TEST_CASE("Setting an empty image name")
{
    constexpr auto imageName = "";
    gui::Image image{};
    image.set(imageName);

    std::list<gui::Command> commands;
    image.buildDrawListImplementation(commands);
    REQUIRE(commands.empty());
}

TEST_CASE("Setting an incorrect image name")
{
    constexpr auto imageName = "ABC";
    gui::Image image{};
    image.set(imageName);

    std::list<gui::Command> commands;
    image.buildDrawListImplementation(commands);
    REQUIRE(!commands.empty());
}

TEST_CASE("Setting an incorrect image id")
{
    constexpr auto imageId = 1000;
    gui::Image image{};
    image.set(imageId);

    std::list<gui::Command> commands;
    image.buildDrawListImplementation(commands);
    REQUIRE(!commands.empty());
}
