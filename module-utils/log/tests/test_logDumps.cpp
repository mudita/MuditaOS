// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

#include <string>

#include <log/Logger.hpp>

namespace
{
    int countFiles(const std::filesystem::path &dir)
    {
        int sum = 0;
        for ([[maybe_unused]] auto const &entry : std::filesystem::directory_iterator{dir}) {
            ++sum;
        }
        return sum;
    }

    bool checkIfLogFilesExist(const std::filesystem::path &path, int expectedFilesCount)
    {
        for (int i = 0; i < expectedFilesCount; ++i) {
            auto filePath = path;
            if (i > 0) {
                filePath.replace_extension(".log." + std::to_string(i));
            }
            if (!std::filesystem::exists(filePath)) {
                return false;
            }
        }
        return true;
    }
} // namespace

TEST_CASE("Test if logs are dumped to a file without rotation")
{
    auto app                            = Log::Application{"TestApp", "rev", "tag", "branch"};
    constexpr auto MaxFileSize          = 1024 * 1024; // 1 MB
    constexpr auto TestLog              = "12345678";
    const std::filesystem::path logsDir = "./ut_logs";
    const auto testLogFile              = logsDir / "TestApp.log";

    // Prepare the environment.
    if (std::filesystem::exists(logsDir)) {
        std::filesystem::remove_all(logsDir);
    }
    std::filesystem::create_directory(logsDir);

    // Initialize the logger with test parameters.
    Log::Logger::get().init(std::move(app), MaxFileSize);
    REQUIRE(countFiles(logsDir) == 0);

    // Dump logs.
    LOG_ERROR(TestLog);
    Log::Logger::get().dumpToFile(testLogFile);
    REQUIRE(countFiles(logsDir) == 1);
    REQUIRE(checkIfLogFilesExist(testLogFile, 1));

    LOG_ERROR(TestLog);
    Log::Logger::get().dumpToFile(testLogFile);
    REQUIRE(countFiles(logsDir) == 1);
    REQUIRE(checkIfLogFilesExist(testLogFile, 1));

    // Clean-up the environment
    std::filesystem::remove_all(logsDir);
}

TEST_CASE("Test if log files rotate")
{
    auto app                            = Log::Application{"TestApp", "rev", "tag", "branch"};
    constexpr auto MaxFileSize          = 10; // 10 bytes
    constexpr auto TestLog              = "12345678";
    const std::filesystem::path logsDir = "./ut_logs";
    const auto testLogFile              = logsDir / "TestApp.log";

    // Prepare the environment.
    if (std::filesystem::exists(logsDir)) {
        std::filesystem::remove_all(logsDir);
    }
    std::filesystem::create_directory(logsDir);

    // Initialize the logger with test parameters.
    Log::Logger::get().init(std::move(app), MaxFileSize);
    REQUIRE(countFiles(logsDir) == 0);

    // Dump logs.
    // Dumping logs to a file causes a log rotation.
    LOG_ERROR(TestLog);
    Log::Logger::get().dumpToFile(testLogFile);
    REQUIRE(countFiles(logsDir) == 1);
    REQUIRE(checkIfLogFilesExist(testLogFile, 1));

    LOG_ERROR(TestLog);
    Log::Logger::get().dumpToFile(testLogFile);
    REQUIRE(countFiles(logsDir) == 2);
    REQUIRE(checkIfLogFilesExist(testLogFile, 2));

    LOG_ERROR(TestLog);
    Log::Logger::get().dumpToFile(testLogFile);
    REQUIRE(countFiles(logsDir) == 3);
    REQUIRE(checkIfLogFilesExist(testLogFile, 3));

    LOG_ERROR(TestLog);
    Log::Logger::get().dumpToFile(testLogFile);
    REQUIRE(countFiles(logsDir) == 3);
    REQUIRE(checkIfLogFilesExist(testLogFile, 3));

    LOG_ERROR(TestLog);
    Log::Logger::get().dumpToFile(testLogFile);
    REQUIRE(countFiles(logsDir) == 3);
    REQUIRE(checkIfLogFilesExist(testLogFile, 3));

    // Clean-up the environment
    std::filesystem::remove_all(logsDir);
}
