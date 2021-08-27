// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <Utils.hpp>
#include "decoderFLAC.hpp"
#include "flac/flacfile.h"

#define DR_FLAC_IMPLEMENTATION
#define DR_FLAC_NO_STDIO
#define DR_FLAC_NO_OGG
#define DR_FLAC_NO_CRC
#define DR_FLAC_NO_SIMD

#include "dr_flac.h"

namespace audio
{

    decoderFLAC::decoderFLAC(const char *fileName) : Decoder(fileName)
    {

        if (fileSize == 0) {
            return;
        }

        flac = drflac_open(drflac_read, drflac_seek, this);
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
        return std::fread(pBufferOut, 1, bytesToRead, userdata->fd);
    }

    drflac_bool32 decoderFLAC::drflac_seek(void *pUserData, int offset, drflac_seek_origin origin)
    {
        decoderFLAC *userdata = (decoderFLAC *)pUserData;
        return !std::fseek(userdata->fd, offset, origin == drflac_seek_origin_start ? SEEK_SET : SEEK_CUR);
    }

    auto decoderFLAC::getBitWidth() -> unsigned int
    {
        TagLib::FLAC::File flacFile(filePath.c_str());
        auto properties = flacFile.audioProperties();
        return properties->bitsPerSample();
    }

} // namespace audio
