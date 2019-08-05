/*
 *  @file Encoder.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 23.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "Encoder.hpp"
#include "EncoderWAV.hpp"

#include <stdint.h>
#include <string>
#include <memory>
#include <cstring>

namespace audio {

    Encoder::Encoder(const char *fileName, const Format &frmt)
            : filePath(fileName), format(frmt) {

        fd = vfs.fopen(fileName, "w");
        if (fd == NULL) {
            return;
        }
        isInitialized = true;
    }

    Encoder::~Encoder() {
        if (fd) {
            vfs.fclose(fd);
        }
    }

    std::unique_ptr<Encoder> Encoder::Create(const char *file, const Format &frmt) {
        std::unique_ptr<Encoder> enc;
        if ((strstr(file, ".wav") != NULL) || (strstr(file, ".WAV") != NULL)) {
            enc = std::make_unique<EncoderWAV>(file, frmt);
        }

        if (enc->isInitialized) {
            return enc;
        } else {
            return nullptr;
        }
    }

}