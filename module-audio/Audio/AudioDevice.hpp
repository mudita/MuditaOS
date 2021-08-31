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
            BluetoothHSP,
            BluetoothHFP
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

        virtual RetCode Pause()
        {
            return RetCode::Success;
        }

        virtual RetCode Resume()
        {
            return RetCode::Success;
        }

        /// Set device output volume
        /// @param vol desired volume from 0 to 10
        /// @return RetCode::Success if OK, or RetCode::Failure otherwise
        virtual RetCode setOutputVolume(float vol) = 0;

        /// Set device input gain
        /// @param gain desired input gain from 0 to 100
        /// @return RetCode::Success if OK, or RetCode::Failure otherwise
        virtual RetCode setInputGain(float gain)   = 0;

        auto getSinkFormat() -> AudioFormat override
        {
            return getSourceFormat();
        }
    };
} // namespace audio
