// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Utils.hpp>

#include <filesystem>
#include <string>
#include <set>

namespace utils
{
    template <std::size_t maxRotationFilesCount>
    class Rotator
    {
      protected:
        const std::string extension;

        std::size_t getRotatedFilesCount(const std::filesystem::path &path, std::error_code &ec) const
        {
            std::size_t count = 0;
            for (std::size_t i = 1; i < maxRotationFilesCount; i++) {
                if (!std::filesystem::exists(getRotatedFilePath(path, i), ec)) {
                    break;
                }
                if (ec) {
                    break;
                }
                count++;
            }
            return count;
        }

        std::string getRotatedFileExtension(int count) const
        {
            return extension + "." + std::to_string(count);
        }

        std::filesystem::path getRotatedFilePath(const std::filesystem::path &source, int rotationCount) const
        {
            auto path = source;
            path.replace_extension(getRotatedFileExtension(rotationCount));
            return path;
        }

        uint getFileNumber(const std::string &filename) const
        {
            const auto position = filename.rfind(".");
            if (position == std::string::npos) {
                return 0;
            }
            const auto indexStartPosition = position + 1;
            auto numberLength             = filename.length() - indexStartPosition;
            return utils::toNumeric(filename.substr(indexStartPosition, numberLength));
        }

        std::filesystem::path incrementFileNumber(const std::filesystem::path &crashdumpPath) const
        {
            auto newCrashdumpName = crashdumpPath.string();
            auto position         = newCrashdumpName.rfind(".");
            if (position == std::string::npos) {
                return crashdumpPath;
            }
            const auto indexStartPosition = position + 1;
            const auto numberLength       = newCrashdumpName.length() - indexStartPosition;
            auto fileNumber               = utils::toNumeric(newCrashdumpName.substr(indexStartPosition, numberLength));
            auto incrementedFileNumber    = fileNumber ? std::to_string(++fileNumber) : "hex.0";
            newCrashdumpName.replace(indexStartPosition, numberLength, incrementedFileNumber);
            return std::filesystem::path(newCrashdumpName);
        }

      public:
        explicit Rotator(std::string extension) : extension{extension}
        {}

        virtual ~Rotator() = default;

        bool rotateFile(const std::filesystem::path &path) const
        {
            std::error_code ec;
            if (const auto firstDump = !std::filesystem::exists(path, ec); ec) {
                return false;
            }
            else if (firstDump) {
                return true;
            }
            const auto rotatedFiles = getRotatedFilesCount(path, ec);
            if (ec) {
                return false;
            }
            for (std::size_t i = rotatedFiles; i > 0; i--) {
                const auto src = getRotatedFilePath(path, i);
                if (i == maxRotationFilesCount - 1) {
                    std::filesystem::remove(src, ec);
                    if (ec) {
                        return false;
                    }
                    continue;
                }
                const auto dest = getRotatedFilePath(path, i + 1);
                std::filesystem::rename(src, dest, ec);
                if (ec) {
                    return false;
                }
            }
            auto rotatedLogPath = getRotatedFilePath(path, 1);
            std::filesystem::rename(path, rotatedLogPath, ec);
            return (ec) ? false : true;
        }

        bool rotateFiles(const std::filesystem::path &path) const
        {
            std::error_code ec;
            std::set<std::filesystem::path> crashdumps{};

            for (const auto &entry : std::filesystem::directory_iterator(path)) {
                crashdumps.insert(entry.path());
            }

            for (const auto &crashdump : crashdumps) {
                if (getFileNumber(crashdump.string()) >= maxRotationFilesCount) {
                    std::filesystem::remove(crashdump, ec);
                    if (ec) {
                        return false;
                    }
                    continue;
                }
                std::filesystem::rename(crashdump, incrementFileNumber(crashdump), ec);
                if (ec) {
                    return false;
                }
            }

            return (ec) ? false : true;
        }
    };
} // namespace utils
