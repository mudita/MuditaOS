/*
 *  @file IdleOperation.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 25.07.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#include "IdleOperation.hpp"

#include "Audio/Profiles/ProfileIdle.hpp"

namespace audio
{

    IdleOperation::IdleOperation([[maybe_unused]] const char *file) : Operation{true}
    {
        availableProfiles.push_back(Profile::Create(Profile::Type::Idle, nullptr));
        currentProfile = availableProfiles[0];
    }

    audio::RetCode IdleOperation::SetOutputVolume(float vol)
    {
        return RetCode::Success;
    }

    audio::RetCode IdleOperation::SetInputGain(float gain)
    {
        return RetCode::Success;
    }
} // namespace audio
