/*
 *  @file decoderFLAC.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 06.04.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#include "decoderFLAC.hpp"

#define DR_FLAC_IMPLEMENTATION
#define DR_FLAC_NO_STDIO
#define DR_FLAC_NO_OGG
#define DR_FLAC_NO_CRC
#define DR_FLAC_NO_SIMD

#include "dr_flac.h"

namespace audio
{

    decoderFLAC::decoderFLAC(const char *fileName) : decoder(fileName), tags(std::make_unique<Tags>())
    {

        if (fileSize == 0) {
            return;
        }

        flac = drflac_open_with_metadata(drflac_read, drflac_seek, drflac_meta, this);
        if (flac == NULL) {
            return;
        }

        isInitialized = true;
    }

    decoderFLAC::~decoderFLAC()
    {
        drflac_close(flac);
    }

    std::unique_ptr<Tags> decoderFLAC::fetchTags()
    {
        tags->filePath.append(filePath);
        if (tags->title.size() == 0) {
            auto pos = filePath.rfind("/");
            if (pos == std::string::npos) {
                tags->title.append(filePath);
            }
            else {
                tags->title.append(&filePath[pos + 1]);
            }
        }
        return std::make_unique<Tags>(*tags);
    }

    uint32_t decoderFLAC::decode(uint32_t samplesToRead, int16_t *pcmData)
    {

        uint32_t samples_read = 0;

        samples_read = drflac_read_s16(flac, samplesToRead, (drflac_int16 *)pcmData);

        if (samples_read) {
            /* Calculate frame duration in seconds */
            position +=
                (float)((float)(chanNumber == 2 ? samplesToRead / chanNumber : samplesToRead) / (float)sampleRate);
        }

        return samples_read;
    }

    void decoderFLAC::setPosition(float pos)
    {

        drflac_seek_to_sample(flac, totalSamplesCount * pos);

        // Calculate new position
        position = (float)((float)(totalSamplesCount * pos) / chanNumber / (float)(sampleRate));
    }

    /* Data encoded in UTF-8 */
    void decoderFLAC::flac_parse_text(uint8_t *in, uint32_t taglen, uint32_t datalen, uint8_t *out, uint32_t outlen)
    {
        /* Little Endian here. */
        uint32_t size = datalen - taglen > outlen - 1 ? outlen - 1 : datalen - taglen;
        memcpy(out, in + taglen, size);
        out[size] = '\0';
    }

    size_t decoderFLAC::drflac_read(void *pUserData, void *pBufferOut, size_t bytesToRead)
    {
        decoderFLAC *userdata = (decoderFLAC *)pUserData;
        return vfs.fread(pBufferOut, 1, bytesToRead, userdata->fd);
    }

    drflac_bool32 decoderFLAC::drflac_seek(void *pUserData, int offset, drflac_seek_origin origin)
    {
        decoderFLAC *userdata = (decoderFLAC *)pUserData;
        return !vfs.fseek(userdata->fd, offset, origin == drflac_seek_origin_start ? SEEK_SET : SEEK_CUR);
    }

    void decoderFLAC::drflac_meta(void *pUserData, drflac_metadata *pMetadata)
    {
        decoderFLAC *userdata = (decoderFLAC *)pUserData;

        /* We're only interested in VORBIS comments which contain album/track/artist info */
        if (pMetadata->type == DRFLAC_METADATA_BLOCK_TYPE_VORBIS_COMMENT) {
            uint8_t buff[128] = {0};

            /*ommit vendor string*/
            uint8_t *p = (uint8_t *)pMetadata->data.vorbis_comment.comments;

            uint8_t i = 0;
            for (i = 0; i < pMetadata->data.vorbis_comment.commentCount; i++) {
                /* Little Endian */
                uint32_t *taglen = (uint32_t *)p;
                p += sizeof(*taglen);

                uint8_t *pp           = (uint8_t *)strchr((char *)p, '=');
                uint8_t j             = 0;
                uint32_t tolower_size = pp - p;
                /*convert to lower case*/
                for (j = 0; j < tolower_size; ++j)
                    p[j] = tolower(p[j]);

                if (!memcmp(p, "artist=", 7)) {
                    userdata->flac_parse_text(p, 7, *taglen, buff, sizeof buff);
                    userdata->tags->artist.append(reinterpret_cast<char *>(&buff[0]));
                }

                if (!memcmp(p, "title=", 6)) {
                    userdata->flac_parse_text(p, 6, *taglen, buff, sizeof buff);
                    userdata->tags->title.append(reinterpret_cast<char *>(&buff[0]));
                }

                if (!memcmp(p, "album=", 6)) {
                    userdata->flac_parse_text(p, 6, *taglen, buff, sizeof buff);
                    userdata->tags->album.append(reinterpret_cast<char *>(&buff[0]));
                }

                if (!memcmp(p, "genre=", 6)) {
                    userdata->flac_parse_text(p, 6, *taglen, buff, sizeof buff);
                    userdata->tags->genre.append(reinterpret_cast<char *>(&buff[0]));
                }

                if (!memcmp(p, "date=", 5)) {
                    userdata->flac_parse_text(p, 5, *taglen, buff, sizeof buff);
                    userdata->tags->year.append(reinterpret_cast<char *>(&buff[0]));
                }

                p += *taglen;
            }
        }
        else if (pMetadata->type == DRFLAC_METADATA_BLOCK_TYPE_STREAMINFO) {
            userdata->tags->total_duration_s =
                (pMetadata->data.streaminfo.totalSampleCount / (pMetadata->data.streaminfo.bitsPerSample / 8)) /
                pMetadata->data.streaminfo.sampleRate;

            if (pMetadata->data.streaminfo.channels == 1) {
                userdata->tags->total_duration_s *= 2;
            }

            userdata->tags->duration_min  = userdata->tags->total_duration_s / 60;
            userdata->tags->duration_hour = userdata->tags->duration_min / 60;
            userdata->tags->duration_sec  = userdata->tags->total_duration_s % 60;
            userdata->tags->sample_rate   = pMetadata->data.streaminfo.sampleRate;
            userdata->tags->num_channel   = pMetadata->data.streaminfo.channels;

            userdata->sampleRate = pMetadata->data.streaminfo.sampleRate;
            userdata->chanNumber = pMetadata->data.streaminfo.channels;

            userdata->totalSamplesCount = pMetadata->data.streaminfo.totalSampleCount;
        }
    }

} // namespace audio
