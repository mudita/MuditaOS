#include "AudioCommon.hpp"
#include <cmath>
#include <sstream>
#include <Utils.hpp>
#include "Profiles/Profile.hpp"

namespace audio
{
    audio::RetCode GetDeviceError(bsp::AudioDevice::RetCode retCode)
    {
        if (retCode == bsp::AudioDevice::RetCode::Success) {
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

    const std::string dbPath(const Setting &setting, const PlaybackType &playbackType, const Profile::Type &profileType)
    {
        if (profileType == Profile::Type::Idle && playbackType == PlaybackType::None) {
            return std::string();
        }

        std::string path(audioDbPrefix);
        if (auto s = str(profileType); !s.empty()) {
            path.append(s + "/");
        }
        if (auto s = str(playbackType); !s.empty()) {
            path.append(s + "/");
        }
        if (auto s = str(setting); !s.empty()) {
            path.append(s);
        }
        return path;
    }

    auto GetVolumeText(const audio::Volume &volume) -> const std::string
    {
        return (static_cast<std::ostringstream &&>(std::ostringstream() << "Vol: " << std::to_string(volume))).str();
    }
} // namespace audio
