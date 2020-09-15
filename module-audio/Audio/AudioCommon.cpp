#include "AudioCommon.hpp"
#include <cmath>
#include <sstream>

namespace audio
{
    audio::RetCode GetDeviceError(bsp::AudioDevice::RetCode retCode)
    {
        if (retCode == bsp::AudioDevice::RetCode::Success) {
            return RetCode::Success;
        }

        return RetCode::DeviceFailure;
    }

    const char *c_str(RetCode retcode)
    {
        switch (retcode) {
        case RetCode::Success:
            return "Success";
        case RetCode::InvokedInIncorrectState:
            return "InvokedInIncorrectState";
        case RetCode::UnsupportedProfile:
            return "UnsupportedProfile";
        case RetCode::UnsupportedEvent:
            return "UnsupportedEvent";
        case RetCode::InvalidFormat:
            return "InvalidFormat";
        case RetCode::OperationCreateFailed:
            return "OperationCreateFailed";
        case RetCode::FileDoesntExist:
            return "FileDoesntExist";
        case RetCode::FailedToAllocateMemory:
            return "FailedToAllocateMemory";
        case RetCode::OperationNotSet:
            return "OperationNotSet";
        case RetCode::ProfileNotSet:
            return "ProfileNotSet";
        case RetCode::DeviceFailure:
            return "DeviceFailure";
        case RetCode::Failed:
            return "Failed";
        }
        return "";
    }

    const std::string str(const PlaybackType &playbackType) noexcept
    {
        switch (playbackType) {
        case PlaybackType::None: {
            return "";
        }
        case PlaybackType::Multimedia: {
            return "Multimedia";
        }
        case PlaybackType::Notifications: {
            return "Notifications";
        }
        case PlaybackType::KeypadSound: {
            return "KeypadSound";
        }
        case PlaybackType::CallRingtone: {
            return "CallRingtone";
        }
        case PlaybackType::TextMessageRingtone: {
            return "TextMessageRingtone";
        }
        }
        return "";
    }

    const std::string str(const Setting &setting) noexcept
    {
        switch (setting) {
        case Setting::Volume:
            return "volume";
            break;
        case Setting::Gain:
            return "gain";
            break;
        }
        return "";
    }

    const std::string str(const Profile::Type &profileType, const Setting &setup, const PlaybackType &playbackType)
    {
        std::stringstream ss;
        const auto typeStr = str(profileType);
        if (typeStr.empty()) {
            return "";
        }
        const auto op = str(playbackType);
        if (op.empty()) {
            ss << "audio/" << str(profileType) << "/" << str(setup);
        }
        else {
            ss << "audio/" << str(profileType) << "/" << str(playbackType) << "/" << str(setup);
        }
        return ss.str();
    }

    const std::string str(const PlaybackType &playbackType, const Setting &setup, const bool headphonesInserted)
    {
        const auto playbackCall = (headphonesInserted) ? str(Profile::Type::PlaybackHeadphones, setup, playbackType)
                                                       : str(Profile::Type::PlaybackLoudspeaker, setup, playbackType);
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

    auto GetVolumeText(const audio::Volume &volume) -> const std::string
    {
        return (static_cast<std::ostringstream &&>(std::ostringstream() << "Vol: " << std::to_string(volume))).str();
    }
} // namespace audio
