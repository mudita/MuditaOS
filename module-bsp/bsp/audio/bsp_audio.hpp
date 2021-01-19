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

        /**
         * User defined callback.
         * It will be invoked when opened stream needs more frames to process( outputBuffer will be != NULL) or if
         * requested frames count are available to user( inputBuffer will be != NULL). From this callback you can safely
         * use file operations, system calls etc This is because audiostream callbacks are not invoked from IRQ context.
         *
         * If there is more data to process or read user should return:
         *  'AudiostreamCallbackContinue'
         *  if there is no more data to process or read user should return:
         *  'AudiostreamCallbackComplete'
         *  this will close stream and clean up all internally allocated resources.
         *  In case of error return:
         *  'AudiostreamCallbackAbort'
         *  this has the same effect as AudiostreamCallbackComplete.
         *
         * @param stream[in] - pointer to valid stream
         * @param inputBuffer[in] - pointer to buffer where user should copy PCM data
         * @param outputBuffer[out] - pointer to buffer containing valid PCM data
         * @param framesPerBuffer[in] - how many frames user should copy or read from buffer
         * @param userData[in] - user specified data
         * @return audiostream_callback_err_t
         */
        using audioCallback_t =
            std::function<int32_t(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer)>;

        explicit AudioDevice(audioCallback_t callback) : callback(callback)
        {}

        AudioDevice() = delete;

        virtual ~AudioDevice() = default;

        static std::optional<std::unique_ptr<AudioDevice>> Create(Type type, audioCallback_t callback);

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

        audioCallback_t GetAudioCallback()
        {
            return callback;
        }

      protected:
        Format currentFormat;
        audioCallback_t callback = nullptr;

        bool isInitialized = false;
    };
} // namespace bsp
