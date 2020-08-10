#pragma once

#include "bsp_audio.hpp"

namespace audio
{
    using Position = float;
    using Volume   = float;
    using Gain     = float;

    constexpr Volume defaultVolumeStep = 0.1;
    constexpr Gain defaultGainStep     = 1.0;
    constexpr Volume invalidVolume     = -1.0;
    constexpr Gain invalidGain         = -1.0;
    constexpr Volume defaultVolume     = 0.5;
    constexpr Gain defaultGain         = 0.5;

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
