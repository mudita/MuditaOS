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

        enum class Flags
        {
            OutputMono   = 1 << 0,
            OutputStereo = 1 << 1,
            InputLeft    = 1 << 2,
            InputRight   = 1 << 3,
            InputStereo  = 1 << 4
        };

        enum class InputPath
        {
            Headphones,
            Microphone,
            None
        };

        enum class OutputPath
        {
            Headphones,
            Earspeaker,
            Loudspeaker,
            None
        };

        using Configuration = struct
        {
            uint32_t sampleRate_Hz = 0; /*!< Sample rate of audio data */
            uint32_t bitWidth      = 0; /*!< Data length of audio data, usually 8/16/24/32 bits */
            uint32_t flags         = 0; /*!< In/Out configuration flags */
            float outputVolume     = 0.0f;
            float inputGain        = 0.0f;
            InputPath inputPath    = InputPath::None;
            OutputPath outputPath  = OutputPath::None;
        };

        virtual ~AudioDevice() = default;

        virtual RetCode Start(const Configuration &format) = 0;
        virtual RetCode Stop()                             = 0;

        virtual RetCode OutputVolumeCtrl(float vol)                 = 0;
        virtual RetCode InputGainCtrl(float gain)                   = 0;
        virtual RetCode OutputPathCtrl(OutputPath outputPath)       = 0;
        virtual RetCode InputPathCtrl(InputPath inputPath)          = 0;
        virtual bool IsFormatSupported(const Configuration &format) = 0;

        float GetOutputVolume() const noexcept
        {
            return currentFormat.outputVolume;
        }

        float GetInputGain() const noexcept
        {
            return currentFormat.inputGain;
        }

        OutputPath GetOutputPath() const noexcept
        {
            return currentFormat.outputPath;
        }

        InputPath GetInputPath() const noexcept
        {
            return currentFormat.inputPath;
        }

        Configuration GetCurrentFormat() const noexcept
        {
            return currentFormat;
        }

      protected:
        Configuration currentFormat;

        bool isInitialized = false;
    };
} // namespace audio
