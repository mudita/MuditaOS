/*
 *  @file decoder.h
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 04.04.19
 *  @brief 
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#ifndef UNTITLED1_DECODER_H
#define UNTITLED1_DECODER_H

#include <stdint.h>
#include <string>
#include <memory>
#include <cstring>

#include "vfs.hpp"

namespace audio {

    struct Tags {

        /* Total audio duration in seconds */
        uint32_t total_duration_s;
        /* Audio duration - hours part */
        uint32_t duration_hour;
        /* Audio duration - minutes part */
        uint32_t duration_min;
        /* Audio duration - seconds part */
        uint32_t duration_sec;

        /* Sample rate */
        uint32_t sample_rate;
        /* Number of channels */
        uint32_t num_channel;

        std::string artist;
        std::string genre;
        std::string title;
        std::string album;
        std::string year;
        std::string filePath;

        Tags() {}

        // Copy constructor
        Tags(const Tags &p2) {

            total_duration_s = p2.total_duration_s;
            duration_hour = p2.duration_hour;
            duration_min = p2.duration_min;
            duration_sec = p2.duration_sec;
            sample_rate = p2.sample_rate;
            num_channel = p2.num_channel;
            artist = p2.artist;
            genre = p2.genre;
            title = p2.title;
            album = p2.album;
            year = p2.year;
            filePath = p2.filePath;

        }
    };


    class decoder {

    public:

        decoder(const char *fileName);

        virtual ~decoder();

        virtual uint32_t decode(uint32_t samplesToRead, int16_t *pcmData) = 0;

        virtual std::unique_ptr<Tags> fetchTags() = 0;

        // Range 0 - 1
        virtual void setPosition(float pos) = 0;

        uint32_t getSampleRate() {
            return sampleRate;
        }

        uint32_t getChannelNumber() {
            return chanNumber;
        }

        float getCurrentPosition() {
            return position;
        }


        // Factory method
        static std::unique_ptr<decoder> Create(const char *file);


    protected:

        void convertmono2stereo(int16_t *pcm, uint32_t samplecount);

        const uint32_t workerBufferSize = 1024 * 8;

        uint32_t sampleRate = 0;
        uint32_t chanNumber = 0;
        float position = 0;
        vfs::FILE *fd = nullptr;
        uint32_t fileSize = 0;
        std::string filePath;

        // Worker buffer used for converting mono stream to stereo
        std::unique_ptr<int16_t[]> workerBuffer;

    };

}


#endif //UNTITLED1_DECODER_H
