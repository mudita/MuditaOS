/*
 *  @file decoder.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 04.04.19
 *  @brief 
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#include "decoder.hpp"

#include "decoderMP3.hpp"
#include "decoderFLAC.hpp"
#include "decoderWAV.hpp"

decoder::decoder(const char *fileName)
        : filePath(fileName),
          workerBuffer(std::make_unique<int16_t[]>(workerBufferSize)) {

    fd = vfs.fopen(fileName, "r");
    if (fd == NULL) {
        return;
    }

    vfs.fseek(fd, 0, SEEK_END);
    fileSize = vfs.ftell(fd);
    vfs.rewind(fd);

}

decoder::~decoder() {
    if (fd) {
        vfs.fclose(fd);
    }
}

std::unique_ptr<decoder> decoder::Create(const char *file) {
    std::unique_ptr<decoder> dec;
    if ((strstr(file, ".wav") != NULL) || (strstr(file, ".WAV") != NULL)) {
        dec = std::make_unique<decoderWAV>(file);
    } else if ((strstr(file, ".mp3") != NULL) || (strstr(file, ".MP3") != NULL)) {
        dec = std::make_unique<decoderMP3>(file);
    } else if ((strstr(file, ".flac") != NULL) || (strstr(file, ".FLAC") != NULL)) {
        dec = std::make_unique<decoderFLAC>(file);
    }

    if (dec->fileSize == 0) {
        //error
        return nullptr;
    } else {
        return dec;
    }
}


void decoder::convertmono2stereo(int16_t *pcm, uint32_t samplecount) {
    uint32_t i = 0, j = 0;

    memset(workerBuffer.get(), 0, workerBufferSize * sizeof(int16_t));

    for (; j < samplecount; j++) {
        workerBuffer[i++] = pcm[j];
        workerBuffer[i++] = pcm[j];
    }

    memcpy(pcm, &workerBuffer[0], samplecount * 2 * sizeof(int16_t));
}

