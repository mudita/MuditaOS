// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "IdleOperation.hpp"

#include "Audio/Profiles/ProfileIdle.hpp"

namespace audio
{

    IdleOperation::IdleOperation([[maybe_unused]] const char *file) : Operation{true}
    {
        supportedProfiles.emplace_back(true, Profile::Create(Profile::Type::Idle, nullptr));
        currentProfile = supportedProfiles[0].second;
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
