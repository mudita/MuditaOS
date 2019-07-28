/*
 *  @file decoderWAV.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 06.04.19
 *  @brief 
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#include "decoderWAV.hpp"

decoderWAV::decoderWAV(const char *fileName) :
        decoder(fileName),
        tag(std::make_unique<Tags>()) {

    if (fd == NULL) {
        return;
    }

    if (vfs.fread(&waveHeader, 1, sizeof(waveHeader), fd) != sizeof(WAVE_FormatTypeDef)) {
        return;
    }

    //TODO:M.P; implement support for sample size different than 16bit
    //pcmsamplesbuffer.reserve(1024);

    tag->total_duration_s = (fileSize - sizeof(WAVE_FormatTypeDef)) / waveHeader.ByteRate;
    tag->duration_min = tag->total_duration_s / 60;
    tag->duration_hour = tag->duration_min / 60;
    tag->duration_sec = tag->total_duration_s % 60;
    tag->sample_rate = waveHeader.SampleRate;
    tag->num_channel = waveHeader.NbrChannels;

    sampleRate = waveHeader.SampleRate;
    bitsPerSample = waveHeader.BitPerSample;
    chanNumber = waveHeader.NbrChannels;
}

std::unique_ptr<Tags> decoderWAV::fetchTags() {

    tag->filePath.append(filePath);
    auto pos = filePath.rfind("/");
    if (pos == std::string::npos) {
        tag->title.append(filePath);
    } else {
        tag->title.append(&filePath[pos + 1]);
    }

    return std::make_unique<Tags>(*tag);
}

uint32_t decoderWAV::decode(uint32_t samplesToRead, int16_t *pcmData) {
    uint32_t samples_read = 0;

/* TODO:M.P; implement support for sample size different than 16bit
    if(samplesToRead > pcmsamplesbuffer.max_size()){
        pcmsamplesbuffer.resize(samplesToRead);
    }*/

    switch (bitsPerSample){
        case 8:
            //TODO:M.P not supported
            break;

        case 16:
            samples_read = vfs.fread(pcmData, sizeof(int16_t), samplesToRead, fd);
            break;

        case 24:
            //TODO:M.P not supported
            break;

        case 32:
            //TODO:M.P not supported
            break;

    }

    if (samples_read) {
        /* Calculate frame duration in seconds */
        position += (float) ((float) (chanNumber == 2 ? samplesToRead/chanNumber : samplesToRead ) / (float) (sampleRate));
    }

    return samples_read;
}

void decoderWAV::setPosition(float pos) {

    vfs.fseek(fd, (fileSize * pos) + sizeof(WAVE_FormatTypeDef), SEEK_SET);
    // Calculate new position
    position = (float) ((float) (vfs.ftell(fd) / sizeof(int16_t) / chanNumber) / (float) (sampleRate));

}
