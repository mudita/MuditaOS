#include "AudioCommon.hpp"

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

} // namespace audio
