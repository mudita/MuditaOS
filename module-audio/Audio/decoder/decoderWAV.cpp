/*
 *  @file decoderWAV.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 06.04.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#include "decoderWAV.hpp"

namespace audio
{

    decoderWAV::decoderWAV(const char *fileName) : decoder(fileName)
    {

        if (fileSize == 0) {
            return;
        }

        if (fd == NULL) {
            return;
        }

        if (vfs.fread(&waveHeader, 1, sizeof(waveHeader), fd) != sizeof(WAVE_FormatTypeDef)) {
            return;
        }

        // TODO:M.P; implement support for sample size different than 16bit
        // pcmsamplesbuffer.reserve(1024);

        sampleRate    = waveHeader.SampleRate;
        bitsPerSample = waveHeader.BitPerSample;
        chanNumber    = waveHeader.NbrChannels;

        isInitialized = true;
    }

    uint32_t decoderWAV::decode(uint32_t samplesToRead, int16_t *pcmData)
    {
        uint32_t samples_read = 0;

        /* TODO:M.P; implement support for sample size different than 16bit
            if(samplesToRead > pcmsamplesbuffer.max_size()){
                pcmsamplesbuffer.resize(samplesToRead);
            }*/

        switch (bitsPerSample) {
        case 8:
            // TODO:M.P not supported
            break;

        case 16:
            samples_read = vfs.fread(pcmData, sizeof(int16_t), samplesToRead, fd);
            break;

        case 24:
            // TODO:M.P not supported
            break;

        case 32:
            // TODO:M.P not supported
            break;
        }

        if (samples_read) {
            /* Calculate frame duration in seconds */
            position +=
                (float)((float)(chanNumber == 2 ? samplesToRead / chanNumber : samplesToRead) / (float)(sampleRate));
        }

        return samples_read;
    }

    void decoderWAV::setPosition(float pos)
    {

        vfs.fseek(fd, (fileSize * pos) + sizeof(WAVE_FormatTypeDef), SEEK_SET);
        // Calculate new position
        position = (float)((float)(vfs.ftell(fd) / sizeof(int16_t) / chanNumber) / (float)(sampleRate));
    }

} // namespace audio
