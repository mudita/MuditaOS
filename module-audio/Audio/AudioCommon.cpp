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

    const std::string str(const Setting &setting, const PlaybackType &playbackType, const Profile::Type &profileType)
    {
        std::stringstream ss;
        const auto typeStr = str(profileType);
        if (typeStr.empty()) {
            return "";
        }
        const auto op = str(playbackType);
        if (op.empty()) {
            ss << "audio/" << str(profileType) << "/" << str(setting);
        }
        else {
            ss << "audio/" << str(profileType) << "/" << str(playbackType) << "/" << str(setting);
        }
        return ss.str();
    }

    const std::string str(const Setting &setting, const PlaybackType &playbackType, const bool headphonesInserted)
    {
        const auto playbackCall = (headphonesInserted) ? str(setting, playbackType, Profile::Type::PlaybackHeadphones)
                                                       : str(setting, playbackType, Profile::Type::PlaybackLoudspeaker);
        switch (playbackType) {
        case PlaybackType::None: {
            return "";
        }
        case PlaybackType::Multimedia: {
            return playbackCall;
        }
        case PlaybackType::Notifications: {
            return playbackCall;
        }
        case PlaybackType::KeypadSound: {
            return playbackCall;
        }
        case PlaybackType::CallRingtone: {
            return playbackCall;
        }
        case PlaybackType::TextMessageRingtone: {
            return playbackCall;
        }
        }
        return "";
    }

    const std::string str(const Setting &setting, const PlaybackType &playbackType)
    {
        std::stringstream ss;
        const auto txtPlaybackType = str(playbackType);
        if (txtPlaybackType.empty()) {
            return "";
        }
        ss << "audio/" << txtPlaybackType << "/" << str(setting);
        return ss.str();
    }

    auto GetVolumeText(const audio::Volume &volume) -> const std::string
    {
        return (static_cast<std::ostringstream &&>(std::ostringstream() << "Vol: " << std::to_string(volume))).str();
    }
} // namespace audio
