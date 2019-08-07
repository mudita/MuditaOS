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

    auto audiocellular = AudioDevice::Create(AudioDevice::Type::Cellular,[](const void *inputBuffer,
                                                                           void *outputBuffer,
                                                                           unsigned long framesPerBuffer)->int32_t {

    });

    REQUIRE(audiocellular);

    AudioDevice::Format formataudiocodec{.sampleRate_Hz=44100,.bitWidth=16,.flags=static_cast<uint32_t >(AudioDevice::Flags ::OutPutStereo)};

    REQUIRE(audiocodec.value()->Start(formataudiocodec) == 0);

    AudioDevice::Format formatcellular{.sampleRate_Hz=16000,.bitWidth=16,.flags=static_cast<uint32_t >(AudioDevice::Flags ::InputStereo) | static_cast<uint32_t >(AudioDevice::Flags ::OutPutStereo)};

    REQUIRE(audiocellular.value()->Start(formatcellular) == 0);

    REQUIRE(audiocodec.value()->Stop() == 0);
    REQUIRE(audiocellular.value()->Stop() == 0);


}

