// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

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
