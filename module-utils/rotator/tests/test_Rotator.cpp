// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include <rotator/Rotator.hpp>

#include <fstream>
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

        std::filesystem::path incrementFileIndex(const std::filesystem::path &source)
        {
            return this->incrementFileNumber(source);
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

TEST_CASE("Rotate files with serial_number and timestamp")
{
    HexRotator rotator;
    std::string path("./rotator_tests");

    SECTION("Increment filename index")
    {
        std::string filename          = "123_12345_crashdump.hex.1";
        std::string expected_filename = "123_12345_crashdump.hex.2";
        auto result                   = rotator.incrementFileIndex(std::filesystem::path(filename));
        REQUIRE(result == expected_filename);
    }

    SECTION("Rotate files")
    {

        std::vector<std::string> test_filenames = {
            "/123_12345_crashdump.hex.1", "/123_12346_crashdump.hex.2", "/123_12347_crashdump.hex.3"};
        std::vector<std::string> expected_test_filenames = {
            "/123_12345_crashdump.hex.2", "/123_12346_crashdump.hex.3", "/123_12347_crashdump.hex.4"};

        std::filesystem::create_directory(path);

        for (const auto &tf : test_filenames) {
            std::ofstream(std::string(path + tf).c_str());
        }
        REQUIRE(rotator.rotateFiles(std::filesystem::path(path)));
        for (const auto &etf : expected_test_filenames) {
            REQUIRE(std::filesystem::exists(std::string(path + etf)));
        }

        std::filesystem::remove_all(path);
    }

    SECTION("Rotate files - max number reach")
    {

        std::vector<std::string> test_filenames = {"/123_12345_crashdump.hex.1",
                                                   "/123_12346_crashdump.hex.2",
                                                   "/123_12347_crashdump.hex.3",
                                                   "/123_12349_crashdump.hex.4",
                                                   "/123_12351_crashdump.hex.5"};

        std::vector<std::string> expected_test_filenames = {"/123_12345_crashdump.hex.2",
                                                            "/123_12346_crashdump.hex.3",
                                                            "/123_12347_crashdump.hex.4",
                                                            "/123_12349_crashdump.hex.5"};

        std::string file_expected_to_not_exist = "/123_12351_crashdump.hex.6";

        std::filesystem::create_directory(path);

        for (const auto &tf : test_filenames) {
            std::ofstream(std::string(path + tf).c_str());
        }
        REQUIRE(rotator.rotateFiles(std::filesystem::path(path)));
        for (const auto &tf : test_filenames) {
            REQUIRE_FALSE(std::filesystem::exists(std::string(path + tf)));
        }
        for (const auto &etf : expected_test_filenames) {
            REQUIRE(std::filesystem::exists(std::string(path + etf)));
        }
        REQUIRE_FALSE(std::filesystem::exists(std::string(path + file_expected_to_not_exist)));

        std::filesystem::remove_all(path);
    }

    SECTION("Increment filename - no number at the end")
    {
        std::string filename          = "123_12345_crashdump.hex";
        std::string expected_filename = "123_12345_crashdump.hex.0";
        auto result                   = rotator.incrementFileIndex(std::filesystem::path(filename));
        REQUIRE(result == expected_filename);
    }

    // in case of fail in earlier tests
    std::filesystem::remove_all(path);
}
