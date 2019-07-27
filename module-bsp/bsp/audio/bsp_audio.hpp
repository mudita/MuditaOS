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

        enum class Flags {
            OutputMono = 1 << 0,
            OutPutStereo = 1 << 1,
            InputLeft = 1 << 2,
            InputRight = 1 << 3
        };

        using AudioFormat = struct {
            uint32_t sampleRate_Hz;   /*!< Sample rate of audio data */
            uint32_t bitWidth;        /*!< Data length of audio data, usually 8/16/24/32 bits */
            uint32_t flags;             /*!< In/Out configuration flags */
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

        virtual int32_t Start(const AudioFormat &format) = 0;

        virtual int32_t Stop() = 0;

        virtual int32_t OutputVolumeCtrl(float vol) = 0;

        virtual int32_t InputGainCtrl(float gain) = 0;

        virtual int32_t OutputPathCtrl(uint32_t outputPath) = 0;

        virtual int32_t InputPathCtrl(uint32_t inputPath) = 0;

        float GetOutputVolume(){return outputVolume;}

        float GetInputGain(){return inputGain;}


        bsp::AudioDevice::AudioFormat GetCurrentFormat(){return currentFormat;}


        AudioDevice(audioCallback_t callback):callback(callback) {}

        virtual ~AudioDevice() {}

    protected:

        bsp::AudioDevice::AudioFormat currentFormat;
        audioCallback_t callback = nullptr;

        bool isInitialized = false;

        float outputVolume = 1.0;
        float inputGain=1.0;
    };

}


#endif //PUREPHONE_BSP_AUDIO_HPP
