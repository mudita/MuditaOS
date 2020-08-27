#pragma once

#include <map>

#include "Profiles/Profile.hpp"
#include "bsp_audio.hpp"

namespace audio
{
    using Position = float;
    using Volume   = uint32_t;
    using Gain     = uint32_t;

    constexpr Volume defaultVolumeStep = 1;
    constexpr Gain defaultGainStep     = 10;
    constexpr Volume defaultVolume     = 5;
    constexpr Gain defaultGain         = 5;

    constexpr Volume maxVolume = 10;
    constexpr Volume minVolume = 0;

    constexpr Gain maxGain = 100;
    constexpr Gain minGain = 0;

    constexpr uint32_t audioOperationTimeout = 1000;

    enum class ProfileSetup
    {
        Volume,
        Gain
    };

    [[nodiscard]] const std::string str(const Profile::Type &type, const ProfileSetup &setup);

    enum class RetCode
    {
        Success = 0,
        InvokedInIncorrectState,
        UnsupportedProfile,
        UnsupportedEvent,
        InvalidFormat,
        OperationCreateFailed,
        FileDoesntExist,
        FailedToAllocateMemory,
        OperationNotSet,
        ProfileNotSet,
        DeviceFailure,
        Failed
    };

    enum class AudioEvents
    {
        EndOfFile,
        FileSystemNoSpace
    };

    RetCode GetDeviceError(bsp::AudioDevice::RetCode retCode);
    const char *c_str(RetCode retcode);
    [[nodiscard]] auto GetVolumeText(const audio::Volume &volume) -> const std::string;
} // namespace audio
