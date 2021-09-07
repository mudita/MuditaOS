// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "decoderWAV.hpp"

#include "Audio/AudioCommon.hpp"

#include "riff/wav/wavfile.h"

namespace audio
{

    decoderWAV::decoderWAV(const char *fileName) : Decoder(fileName)
    {

        if (fileSize == 0) {
            return;
        }

        if (fd == NULL) {
            return;
        }

        if (std::fread(&waveHeader, 1, sizeof(waveHeader), fd) != sizeof(WAVE_FormatTypeDef)) {
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
            samples_read = std::fread(pcmData, sizeof(int16_t), samplesToRead, fd);
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

        std::fseek(fd, (fileSize * pos) + sizeof(WAVE_FormatTypeDef), SEEK_SET);
        // Calculate new position
        position = (float)((float)(std::ftell(fd) / sizeof(int16_t) / chanNumber) / (float)(sampleRate));
    }

    auto decoderWAV::getBitWidth() -> unsigned int
    {
        TagLib::RIFF::WAV::File wavFile(filePath.c_str());
        auto properties = wavFile.audioProperties();
        return properties->bitsPerSample();
    }

} // namespace audio
