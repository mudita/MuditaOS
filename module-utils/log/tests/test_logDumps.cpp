// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <string>
#include <log/Logger.hpp>
#include <CrashdumpMetadataStore/CrashdumpMetadataStore.hpp>

namespace
{
    inline constexpr auto productName       = "TestProduct";
    inline constexpr auto osVersion         = "6.6.6";
    inline constexpr auto commitHash        = "baadf00d";
    inline constexpr auto serialNumber      = "141120222134";
    inline constexpr auto filenamePrefix    = "MuditaOS";
    inline constexpr auto filenameExtension = ".log";
    inline constexpr auto filenameSeparator = "_";

    const std::filesystem::path logsDir = "./ut_logs";

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

    void prepareCrashdumpStore()
    {
        Store::CrashdumpMetadata::getInstance().setProductName(productName);
        Store::CrashdumpMetadata::getInstance().setOsVersion(osVersion);
        Store::CrashdumpMetadata::getInstance().setCommitHash(commitHash);
        Store::CrashdumpMetadata::getInstance().setSerialNumber(serialNumber);
    }

} // namespace

TEST_CASE("Test if logs are dumped to a file without rotation")
{
    static constexpr auto maxFileSize   = 1024 * 1024; // 1 MB
    static constexpr auto testLogString = "12345678";

    auto app = Log::Application{"TestApp", "rev", "tag", "branch"};

    /* Prepare the environment */
    prepareCrashdumpStore();

    const auto &osMetadata = Store::CrashdumpMetadata::getInstance().getMetadataString();
    const auto logFilename = std::string(filenamePrefix) + filenameSeparator + osMetadata + filenameExtension;
    const auto logFilePath = logsDir / logFilename;

    if (std::filesystem::exists(logsDir)) {
        std::filesystem::remove_all(logsDir);
    }
    std::filesystem::create_directory(logsDir);

    /* Initialize the logger with test parameters */
    Log::Logger::get().init(std::move(app), maxFileSize);
    REQUIRE(countFiles(logsDir) == 0);

    /* Dump logs */
    LOG_ERROR("%s", testLogString);
    Log::Logger::get().dumpToFile(logsDir);
    REQUIRE(countFiles(logsDir) == 1);
    REQUIRE(checkIfLogFilesExist(logFilePath, 1) == true);

    LOG_ERROR("%s", testLogString);
    Log::Logger::get().dumpToFile(logsDir);
    REQUIRE(countFiles(logsDir) == 1);
    REQUIRE(checkIfLogFilesExist(logFilePath, 1) == true);

    /* Clean-up the environment */
    std::filesystem::remove_all(logsDir);
}

TEST_CASE("Test if log files rotate")
{
    static constexpr auto maxFileSize   = 10; // 10 bytes
    static constexpr auto testLogString = "12345678";

    auto app = Log::Application{"TestApp", "rev", "tag", "branch"};

    /* Prepare the environment */
    prepareCrashdumpStore();

    const auto &osMetadata = Store::CrashdumpMetadata::getInstance().getMetadataString();
    const auto logFilename = std::string(filenamePrefix) + filenameSeparator + osMetadata + filenameExtension;
    const auto logFilePath = logsDir / logFilename;

    if (std::filesystem::exists(logsDir)) {
        std::filesystem::remove_all(logsDir);
    }
    std::filesystem::create_directory(logsDir);

    // Initialize the logger with test parameters.
    Log::Logger::get().init(std::move(app), maxFileSize);
    REQUIRE(countFiles(logsDir) == 0);

    /* Dump logs. Dumping logs to a file causes files rotation. */
    LOG_ERROR("%s", testLogString);
    Log::Logger::get().dumpToFile(logsDir);
    REQUIRE(countFiles(logsDir) == 1);
    REQUIRE(checkIfLogFilesExist(logFilePath, 1) == true);

    LOG_ERROR("%s", testLogString);
    Log::Logger::get().dumpToFile(logsDir);
    REQUIRE(countFiles(logsDir) == 2);
    REQUIRE(checkIfLogFilesExist(logFilePath, 2) == true);

    LOG_ERROR("%s", testLogString);
    Log::Logger::get().dumpToFile(logsDir);
    REQUIRE(countFiles(logsDir) == 3);
    REQUIRE(checkIfLogFilesExist(logFilePath, 3) == true);

    LOG_ERROR("%s", testLogString);
    Log::Logger::get().dumpToFile(logsDir);
    REQUIRE(countFiles(logsDir) == 3);
    REQUIRE(checkIfLogFilesExist(logFilePath, 3) == true);

    LOG_ERROR("%s", testLogString);
    Log::Logger::get().dumpToFile(logsDir);
    REQUIRE(countFiles(logsDir) == 3);
    REQUIRE(checkIfLogFilesExist(logFilePath, 3) == true);

    // Clean-up the environment
    std::filesystem::remove_all(logsDir);
}
