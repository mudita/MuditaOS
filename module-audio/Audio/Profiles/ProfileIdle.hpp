// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "Profile.hpp"

namespace audio
{

    class ProfileIdle : public Profile
    {
      public:
        ProfileIdle() : Profile("Idle", Type::Idle, audio::codec::Configuration{}, AudioDevice::Type::None)
        {}
    };

} // namespace audio
