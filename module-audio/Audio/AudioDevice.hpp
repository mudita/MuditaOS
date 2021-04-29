// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Audio/Endpoint.hpp>

#include <optional>
#include <memory>
#include <functional>

namespace audio
{

    class AudioDevice : public audio::IOProxy
    {

      public:
        enum class RetCode
        {
            Success = 0,
            Failure
        };

        enum class Type
        {
            None,
            Audiocodec,
            Cellular,
            BluetoothA2DP,
            BluetoothHSP
        };

        virtual ~AudioDevice() = default;

        virtual RetCode Start()
        {
            return RetCode::Success;
        }

        virtual RetCode Stop()
        {
            return RetCode::Success;
        }

        virtual RetCode setOutputVolume(float vol) = 0;
        virtual RetCode setInputGain(float gain)   = 0;

        auto getSinkFormat() -> AudioFormat override
        {
            return getSourceFormat();
        }
    };
} // namespace audio
