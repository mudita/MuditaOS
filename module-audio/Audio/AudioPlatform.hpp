// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AudioDeviceFactory.hpp"

#include <memory>

namespace audio
{
    class AudioPlatform
    {
      public:
        static std::unique_ptr<AudioDeviceFactory> GetDeviceFactory();
    };

}; // namespace audio
