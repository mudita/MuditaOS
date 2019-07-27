/*
 *  @file bsp_audio_tests.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 22.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#include "catch.hpp"

#include <stdint.h>
#include <algorithm>


#include "bsp/audio/bsp_audio.hpp"

using namespace bsp;

TEST_CASE( "Testing Linux Audiocodec" ) {
    auto audiocodec = AudioDevice::Create(AudioDevice::Type::Audiocodec,[](const void *inputBuffer,
                                                               void *outputBuffer,
                                                               unsigned long framesPerBuffer)->int32_t {

    });

    REQUIRE(audiocodec);

    AudioDevice::Format format{.sampleRate_Hz=44100,.bitWidth=16,.flags=static_cast<uint32_t >(AudioDevice::Flags ::OutPutStereo)};

    REQUIRE(audiocodec.value()->Start(format) == 0);

    REQUIRE(audiocodec.value()->Stop() == 0);

}

