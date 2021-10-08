// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "IdleOperation.hpp"

#include "Audio/Profiles/ProfileIdle.hpp"

namespace audio
{

    IdleOperation::IdleOperation([[maybe_unused]] const char *file) : Operation(nullptr)
    {
        supportedProfiles.emplace_back(Profile::Create(Profile::Type::Idle), true);
        currentProfile = supportedProfiles[0].profile;
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
