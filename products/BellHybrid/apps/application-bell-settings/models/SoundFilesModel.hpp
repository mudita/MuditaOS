// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <utf8/UTF8.hpp>

#include <filesystem>
#include <string>
#include <vector>

namespace app::bell_settings
{
    enum class AlarmSoundType
    {
        PreWakeUp
    };

    class AlarmSoundsModel
    {
      public:
        explicit AlarmSoundsModel(AlarmSoundType type);
        auto getSoundFiles() const -> std::vector<std::filesystem::path>;
        static auto getSoundFilesDir(AlarmSoundType type) -> std::filesystem::path;

      private:
        static auto scanDirForFiles(const std::filesystem::path &dir, std::vector<std::string> extensions)
            -> std::vector<std::filesystem::path>;

        AlarmSoundType type;
    };

    struct AlarmSoundDelegate
    {
      public:
        static auto fromModel(const AlarmSoundsModel &model) -> std::vector<AlarmSoundDelegate>;

        AlarmSoundDelegate() = default;
        explicit AlarmSoundDelegate(const std::filesystem::path &filePath);
        AlarmSoundDelegate(AlarmSoundType type, const std::string &fileName);

        operator UTF8() const;

        friend bool operator==(const AlarmSoundDelegate &lhs, const AlarmSoundDelegate &rhs) noexcept
        {
            return lhs.path == rhs.path;
        }

        friend bool operator!=(const AlarmSoundDelegate &lhs, const AlarmSoundDelegate &rhs) noexcept
        {
            return !operator==(lhs, rhs);
        }

        const std::filesystem::path path;
    };
} // namespace app::bell_settings
