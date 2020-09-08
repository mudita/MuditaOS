/*
 *  @file decoderFLAC.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 06.04.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#include "decoderFLAC.hpp"
#include "Utils.hpp"

#define DR_FLAC_IMPLEMENTATION
#define DR_FLAC_NO_STDIO
#define DR_FLAC_NO_OGG
#define DR_FLAC_NO_CRC
#define DR_FLAC_NO_SIMD

#include "dr_flac.h"

namespace audio
{

    decoderFLAC::decoderFLAC(const char *fileName) : decoder(fileName)
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

        if (pMetadata->type == DRFLAC_METADATA_BLOCK_TYPE_STREAMINFO) {
            userdata->tag->total_duration_s =
                (pMetadata->data.streaminfo.totalSampleCount / (pMetadata->data.streaminfo.bitsPerSample / 8)) /
                pMetadata->data.streaminfo.sampleRate;

            if (pMetadata->data.streaminfo.channels == 1) {
                userdata->tag->total_duration_s *= 2;
            }

            userdata->tag->duration_min  = userdata->tag->total_duration_s / utils::secondsInMinute;
            userdata->tag->duration_hour = userdata->tag->duration_min / utils::secondsInMinute;
            userdata->tag->duration_sec  = userdata->tag->total_duration_s % utils::secondsInMinute;
            userdata->tag->sample_rate   = pMetadata->data.streaminfo.sampleRate;
            userdata->tag->num_channel   = pMetadata->data.streaminfo.channels;

            userdata->sampleRate = pMetadata->data.streaminfo.sampleRate;
            userdata->chanNumber = pMetadata->data.streaminfo.channels;

            userdata->totalSamplesCount = pMetadata->data.streaminfo.totalSampleCount;
        }
    }

} // namespace audio
