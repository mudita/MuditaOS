// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SensitiveLog.hpp"
#include <cstring>
#include <fstream>
#include <LockGuard.hpp>
#include <gsl/util>

namespace
{
    inline constexpr int statusSuccess = 1;
    inline constexpr auto lineSize     = 224;
    inline constexpr auto bufferLength = 100;

    __attribute__((section(".sensitiveLogSection"))) static char buffer[bufferLength][lineSize];
    std::uint8_t line{0};

    std::uint8_t getNextLine()
    {
        if (line < bufferLength - 1) {
            line++;
        }
        else {
            line = 0;
        }
        return line;
    }
    const std::string currentDateTime()
    {
        time_t now = std::time(0);
        struct tm tstruct;
        char buf[80];
        tstruct = *std::localtime(&now);

        std::strftime(buf, sizeof(buf), "%Y_%m_%d-%H_%M", &tstruct);

        return buf;
    }

} // namespace

namespace Log
{
    void SensitiveLog::put(const std::string &item)
    {
        if (!permissionToLog) {
            return;
        }
        LockGuard lock(mutex);
        strncpy(buffer[getNextLine()], item.c_str(), lineSize);
    }

    int SensitiveLog::dumpToFile(const std::filesystem::path &logDirectoryPath)
    {
        LockGuard lock(mutex);

        const std::string logFileName = currentDateTime() + ".log";
        const auto logFilePath        = logDirectoryPath / logFileName;

        std::ofstream logFile(logFilePath);
        if (!logFile) {
            return -EIO;
        }

        auto fileHandlerCleanup = gsl::finally([&logFile]() { logFile.close(); });

        for (std::uint8_t i = 0; i < bufferLength; i++) {
            buffer[i][lineSize - 1]   = '\0';
            const std::string toWrite = buffer[i];
            logFile.write(toWrite.c_str(), toWrite.size());
        }
        if (logFile.bad()) {
            return -EIO;
        }

        // clear buffer
        std::memset(buffer, '\0', sizeof(buffer));

        permissionToLog = true;

        return statusSuccess;
    }
} // namespace Log
