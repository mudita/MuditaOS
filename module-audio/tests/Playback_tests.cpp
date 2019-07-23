/*
 *  @file Playback_tests.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 23.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#include <iostream>
#include <memory>
#include <cstring>
#include <unistd.h>


#include "catch.hpp"

#include "vfs.hpp"

#include "Audio/Playback.hpp"
#include "Audio/Profiles/ProfilePlaybackLoudspeaker.hpp"
#include "Audio/Profiles/ProfilePlaybackHeadphones.hpp"


TEST_CASE( "Playback tests" ) {

    auto cwd = vfs.getcurrdir();
    cwd = cwd.substr(0,cwd.find_last_of("/\\"));
    cwd = cwd.append("/module-audio/tests/samples");

    SECTION("Sample1.wav 16bit 44100Hz stereo")
    {
        ProfilePlaybackLoudspeaker  profileLoudspeaker(nullptr,100,0);
        ProfilePlaybackHeadphones   profileHeadphones(nullptr,20,0);
        Playback sample1((cwd + "/sample1.wav").c_str(),&profileLoudspeaker);
        sample1.Play();

        sleep(1);
        sample1.Pause();
        sample1.SwitchProfile(&profileHeadphones);
        sample1.Resume();

        sleep(6);

    }

}


