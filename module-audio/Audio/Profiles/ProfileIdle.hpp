#pragma once

#include "Profile.hpp"

namespace audio
{

    class ProfileIdle : public Profile
    {
      public:
        ProfileIdle() : Profile("Idle", Type::Idle, bsp::AudioDevice::Format{}, bsp::AudioDevice::Type::None, nullptr)
        {}
    };

} // namespace audio

