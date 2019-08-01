/*
 *  @file bsp_audio.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 22.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_BSP_AUDIO_HPP
#define PUREPHONE_BSP_AUDIO_HPP

#include <optional>
#include <stdint.h>
#include <stddef.h>
#include <memory>
#include "functional"

namespace bsp {

    class AudioDevice {
    public:

        enum class Type {
            Audiocodec,
            Cellular,
            Bluetooth
        };

        enum class InputPath{
            Headphones,
            Microphone,
            None
        };

        enum class OutputPath{
            Headphones,
            HeadphonesMono,
            Earspeaker,
            Loudspeaker,
            None
        };

        enum class Flags {
            OutputMono = 1 << 0,
            OutPutStereo = 1 << 1,
            InputLeft = 1 << 2,
            InputRight = 1 << 3
        };

        using Format = struct {
            uint32_t sampleRate_Hz;   /*!< Sample rate of audio data */
            uint32_t bitWidth;        /*!< Data length of audio data, usually 8/16/24/32 bits */
            uint32_t flags;             /*!< In/Out configuration flags */
            float outputVolume=0.0;
            float inputGain=0.0;
            InputPath inputPath=InputPath ::None;
            OutputPath outputPath=OutputPath ::None;
        };

        /**
* User defined callback.
* It will be invoked when opened stream needs more frames to process( outputBuffer will be != NULL) or if requested frames count
* are available to user( inputBuffer will be != NULL).
* From this callback you can safely use file operations, system calls etc This is because audiostream
* callbacks are not invoked from IRQ context.
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

        using audioCallback_t = std::function<int32_t(const void *inputBuffer,
                                                      void *outputBuffer,
                                                      unsigned long framesPerBuffer)>;


        static std::optional<std::unique_ptr<AudioDevice>> Create(Type type,audioCallback_t callback);

        virtual int32_t Start(const Format &format) = 0;

        virtual int32_t Stop() = 0;

        virtual int32_t OutputVolumeCtrl(float vol) = 0;

        virtual int32_t InputGainCtrl(float gain) = 0;

        virtual int32_t OutputPathCtrl(OutputPath outputPath) = 0;

        virtual int32_t InputPathCtrl(InputPath inputPath) = 0;

        virtual bool IsFormatSupported(const Format& format) = 0;

        float GetOutputVolume(){return currentFormat.outputVolume;}

        float GetInputGain(){return currentFormat.inputGain;}

        OutputPath GetOutputPath(){return currentFormat.outputPath;}

        InputPath GetInputPath(){return currentFormat.inputPath;}


        bsp::AudioDevice::Format GetCurrentFormat(){return currentFormat;}

        audioCallback_t GetAudioCallback(){return callback;}


        AudioDevice(audioCallback_t callback):callback(callback) {}

        virtual ~AudioDevice() {}

    protected:

        bsp::AudioDevice::Format currentFormat;
        audioCallback_t callback = nullptr;

        bool isInitialized = false;
    };

}


#endif //PUREPHONE_BSP_AUDIO_HPP
