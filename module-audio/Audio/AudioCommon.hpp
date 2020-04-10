#pragma once

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
        Failed
    };

    enum class AudioEvents
    {
        EndOfFile,
        FileSystemNoSpace
    };
} // namespace audio
