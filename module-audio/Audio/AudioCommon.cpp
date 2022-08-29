// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AudioCommon.hpp"
#include "Profiles/Profile.hpp"

#include <Utils.hpp>

#include <sstream>

namespace audio
{
    audio::RetCode GetDeviceError(AudioDevice::RetCode retCode)
    {
        if (retCode != AudioDevice::RetCode::Failure) {
            return RetCode::Success;
        }

        return RetCode::DeviceFailure;
    }

    const std::string str(RetCode retcode)
    {
        return utils::enumToString(retcode);
    }

    const std::string str(const PlaybackType &playbackType) noexcept
    {
        if (playbackType == PlaybackType::None) {
            return "";
        }
        return utils::enumToString(playbackType);
    }

    const std::string str(const Setting &setting) noexcept
    {
        return utils::enumToString(setting);
    }

    const std::string dbPath(const DbPathElement &element)
    {
        return dbPath(element.setting, element.playbackType, element.profileType);
    }

    const std::string dbPath(const Setting &setting, const PlaybackType &playbackType, const Profile::Type &profileType)
    {
        if (profileType == Profile::Type::Idle && playbackType == PlaybackType::None) {
            return std::string();
        }

        std::vector<std::string> pathElements;
        std::string path;

        pathElements.emplace_back(audioDbPrefix);

        if (auto s = str(profileType); !s.empty()) {
            pathElements.emplace_back(s);
        }

        if (auto s = str(playbackType); !s.empty()) {
            pathElements.emplace_back(s);
        }

        if (auto s = str(setting); !s.empty()) {
            pathElements.emplace_back(s);
        }

        for (size_t idx = 0; idx < pathElements.size(); idx++) {
            path.append(pathElements[idx]);
            if (idx != pathElements.size() - 1) {
                path.append(1, dbPathSeparator);
            }
        }

        return path;
    }

    auto GetVolumeText(const audio::Volume &volume) -> std::string
    {
        return (static_cast<std::ostringstream &&>(std::ostringstream() << "Vol: " << std::to_string(volume))).str();
    }
} // namespace audio
