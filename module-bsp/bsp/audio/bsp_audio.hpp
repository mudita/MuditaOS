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

namespace bsp {

    class Audio {
    public:

        enum class Type {
            Audiocodec,
            Cellular,
            Bluetooth
        };

        enum class MonoStereo {
            Stereo,
            MonoLeft,
            MonoRight
        };

        using AudioFormat = struct {
            uint32_t sampleRate_Hz;   /*!< Sample rate of audio data */
            uint32_t bitWidth;        /*!< Data length of audio data, usually 8/16/24/32 bits */
            MonoStereo stereo;         /*!< Mono or stereo */
            uint8_t *data;           /*!< Data start address to transfer. */
            size_t dataSize;         /*!< Transfer size. */
        };

        static std::optional<std::unique_ptr<Audio>> Create(Type type);
        virtual int32_t Start(const AudioFormat& format) = 0;
        virtual int32_t Stop() = 0;
        virtual int32_t Pause() = 0;
        virtual int32_t Resume() = 0;
        virtual int32_t OutputVolumeCtrl(uint32_t volume) = 0;
        virtual int32_t InputGainCtrl(int8_t gain) = 0;
        virtual int32_t OutputPathCtrl(uint32_t outputPath) = 0;
        virtual int32_t InputPathCtrl(uint32_t inputPath) = 0;

        Audio() {}
        virtual ~Audio() {}

    protected:



        bool isInitialized = false;
    };

}


#endif //PUREPHONE_BSP_AUDIO_HPP
