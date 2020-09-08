/*
 *  @file decoder.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 04.04.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#include "decoder.hpp"
#include "Utils.hpp"

#include "decoderMP3.hpp"
#include "decoderFLAC.hpp"
#include "decoderWAV.hpp"

#include "fileref.h"
#include "tag.h"
#include "tfilestream.h"

namespace audio
{

    decoder::decoder(const char *fileName)
        : filePath(fileName), workerBuffer(std::make_unique<int16_t[]>(workerBufferSize)), tag(std::make_unique<Tags>())
    {

        fd = vfs.fopen(fileName, "r");
        if (fd == NULL) {
            return;
        }

        vfs.fseek(fd, 0, SEEK_END);
        fileSize = vfs.ftell(fd);
        vfs.rewind(fd);
    }

    decoder::~decoder()
    {
        if (fd) {
            vfs.fclose(fd);
        }
    }

    std::unique_ptr<Tags> decoder::fetchTags()
    {
        if (fd) {
            vfs.rewind(fd);
            TagLib::FileStream fileStream(fd);
            TagLib::FileRef tagReader(&fileStream);
            if (!tagReader.isNull() && tagReader.tag()) {
                TagLib::Tag *tags                   = tagReader.tag();
                TagLib::AudioProperties *properties = tagReader.audioProperties();

                tag->title  = tags->title().to8Bit();
                tag->artist = tags->artist().to8Bit();
                tag->album  = tags->album().to8Bit();
                tag->genre  = tags->genre().to8Bit();
                tag->year   = std::to_string(tags->year());

                tag->total_duration_s = properties->length();
                tag->duration_min     = tag->total_duration_s / utils::secondsInMinute;
                tag->duration_hour    = tag->duration_min / utils::secondsInMinute;
                tag->duration_sec     = tag->total_duration_s % utils::secondsInMinute;
                tag->sample_rate      = properties->sampleRate();
                tag->num_channel      = properties->channels();
                tag->bitrate          = properties->bitrate();
            }
            vfs.rewind(fd);
            fetchTagsSpecific();
            vfs.rewind(fd);
        }

        tag->filePath.append(filePath);
        // If title tag empty fill it with raw file name
        if (tag->title.size() == 0) {
            if (const auto pos = filePath.rfind("/"); pos == std::string::npos) {
                tag->title.append(filePath);
            }
            else {
                tag->title.append(&filePath[pos + 1]);
            }
        }
        return std::make_unique<Tags>(*tag);
    }

    std::unique_ptr<decoder> decoder::Create(const char *file)
    {
        std::unique_ptr<decoder> dec;
        if ((strstr(file, ".wav") != NULL) || (strstr(file, ".WAV") != NULL)) {
            dec = std::make_unique<decoderWAV>(file);
        }
        else if ((strstr(file, ".mp3") != NULL) || (strstr(file, ".MP3") != NULL)) {
            dec = std::make_unique<decoderMP3>(file);
        }
        else if ((strstr(file, ".flac") != NULL) || (strstr(file, ".FLAC") != NULL)) {
            dec = std::make_unique<decoderFLAC>(file);
        }
        else {
            return nullptr;
        }

        if (!dec->isInitialized) {
            return nullptr;
        }
        else {
            return dec;
        }
    }

    void decoder::convertmono2stereo(int16_t *pcm, uint32_t samplecount)
    {
        uint32_t i = 0, j = 0;

        memset(workerBuffer.get(), 0, workerBufferSize * sizeof(int16_t));

        for (; j < samplecount; j++) {
            workerBuffer[i++] = pcm[j];
            workerBuffer[i++] = pcm[j];
        }

        memcpy(pcm, &workerBuffer[0], samplecount * 2 * sizeof(int16_t));
    }

} // namespace audio
