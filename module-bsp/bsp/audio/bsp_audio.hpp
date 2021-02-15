#pragma once

#include <Audio/Endpoint.hpp>

#include <optional>
#include <memory>
#include <functional>

namespace bsp
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
            Bluetooth
        };

        enum class InputPath
        {
            Headphones,
            Microphone,
            BluetoothHSP,
            None
        };

        enum class OutputPath
        {
            Headphones,
            HeadphonesMono,
            Earspeaker,
            Loudspeaker,
            LoudspeakerMono,
            BluetoothA2DP,
            BluetoothHSP,
            None
        };

        enum class Flags
        {
            OutputMono   = 1 << 0,
            OutputStereo = 1 << 1,
            InputLeft    = 1 << 2,
            InputRight   = 1 << 3,
            InputStereo  = 1 << 4
        };

        using Format = struct
        {
            uint32_t sampleRate_Hz = 0; /*!< Sample rate of audio data */
            uint32_t bitWidth      = 0; /*!< Data length of audio data, usually 8/16/24/32 bits */
            uint32_t flags         = 0; /*!< In/Out configuration flags */
            float outputVolume     = 0.0f;
            float inputGain        = 0.0f;
            InputPath inputPath    = InputPath::None;
            OutputPath outputPath  = OutputPath::None;
        };

        AudioDevice() = default;
        explicit AudioDevice(const audio::Endpoint::Capabilities &sourceCaps,
                             const audio::Endpoint::Capabilities &sinkCaps)
            : IOProxy(sourceCaps, sinkCaps)
        {}

        virtual ~AudioDevice() = default;

        static std::optional<std::unique_ptr<AudioDevice>> Create(Type type);

        virtual RetCode Start(const Format &format) = 0;
        virtual RetCode Stop()                      = 0;

        virtual RetCode OutputVolumeCtrl(float vol)           = 0;
        virtual RetCode InputGainCtrl(float gain)             = 0;
        virtual RetCode OutputPathCtrl(OutputPath outputPath) = 0;
        virtual RetCode InputPathCtrl(InputPath inputPath)    = 0;
        virtual bool IsFormatSupported(const Format &format)  = 0;

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

        Format GetCurrentFormat() const noexcept
        {
            return currentFormat;
        }

      protected:
        Format currentFormat;

        bool isInitialized = false;
    };
} // namespace bsp
