#pragma once

namespace audio
{

    using Position = float;
    using Volume   = float;
    using Gain     = float;

    constexpr Volume defaultVolumeStep = 0.1;
    constexpr Volume defaultGainStep   = 1.0;

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
        Failed
    };

    enum class AudioEvents
    {
        EndOfFile,
        FileSystemNoSpace
    };

} // namespace audio
