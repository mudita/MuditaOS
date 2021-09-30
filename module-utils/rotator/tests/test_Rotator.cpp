// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <rotator/Rotator.hpp>

#include <iostream>
#include <filesystem>

namespace
{
    constexpr std::size_t maxRotationFilesCount = 5;

    class HexRotator : public utils::Rotator<maxRotationFilesCount>
    {
      public:
        HexRotator() : utils::Rotator<maxRotationFilesCount>{".hex"}
        {}

        std::filesystem::path rotatedFilePath(const std::filesystem::path &source, int rotationCount)
        {
            return this->getRotatedFilePath(source, rotationCount);
        }
    };
} // namespace

TEST_CASE("Rotation of hex dump files")
{
    HexRotator rotator;
    constexpr auto fileName = "crashdump.hex";
    SECTION("Handle first dump")
    {
        REQUIRE(rotator.rotateFile(fileName));
    }
    SECTION("Handle first rotate")
    {
        constexpr auto firstRotationName = "crashdump.hex.1";
        std::ofstream file(fileName);
        REQUIRE(rotator.rotateFile(fileName));
        REQUIRE(std::filesystem::exists(firstRotationName));
        REQUIRE_FALSE(std::filesystem::exists(fileName));
        REQUIRE(std::filesystem::remove(firstRotationName));
    }
    SECTION("Handle max rotates")
    {
        for (std::size_t i = 1; i <= maxRotationFilesCount; i++) {
            const auto rotatedFileName = rotator.rotatedFilePath(fileName, i);
            std::ofstream file(fileName);
            REQUIRE(rotator.rotateFile(fileName));
            for (std::size_t j = i - 1; j > 0; j--) {
                REQUIRE(std::filesystem::exists(rotator.rotatedFilePath(fileName, j)));
            }
            if (i == maxRotationFilesCount) {
                REQUIRE_FALSE(std::filesystem::exists(rotator.rotatedFilePath(fileName, i)));
            }
            REQUIRE_FALSE(std::filesystem::exists(fileName));
        }
        for (std::size_t i = 1; i < maxRotationFilesCount; i++) {
            const auto rotatedFileName = rotator.rotatedFilePath(fileName, i);
            REQUIRE(std::filesystem::remove(rotatedFileName));
        }
    }

    SECTION("Handle more than max files")
    {
        for (std::size_t i = 1; i <= maxRotationFilesCount; i++) {
            const auto rotatedFileName = rotator.rotatedFilePath(fileName, i);
            std::ofstream file(fileName);
            REQUIRE(rotator.rotateFile(fileName));
            for (std::size_t j = i - 1; j > 0; j--) {
                REQUIRE(std::filesystem::exists(rotator.rotatedFilePath(fileName, j)));
            }
            if (i == maxRotationFilesCount) {
                REQUIRE_FALSE(std::filesystem::exists(rotator.rotatedFilePath(fileName, i)));
            }
            REQUIRE_FALSE(std::filesystem::exists(fileName));
        }
        std::ofstream file(fileName);
        REQUIRE(rotator.rotateFile(fileName));
        REQUIRE_FALSE(std::filesystem::exists(rotator.rotatedFilePath(fileName, maxRotationFilesCount + 1)));
        for (std::size_t i = 1; i < maxRotationFilesCount; i++) {
            const auto rotatedFileName = rotator.rotatedFilePath(fileName, i);
            REQUIRE(std::filesystem::exists(rotatedFileName));
        }
        REQUIRE_FALSE(std::filesystem::exists(rotator.rotatedFilePath(fileName, maxRotationFilesCount)));
        REQUIRE_FALSE(std::filesystem::exists(fileName));
        for (std::size_t i = 1; i < maxRotationFilesCount; i++) {
            const auto rotatedFileName = rotator.rotatedFilePath(fileName, i);
            REQUIRE(std::filesystem::remove(rotatedFileName));
        }
    }
}
