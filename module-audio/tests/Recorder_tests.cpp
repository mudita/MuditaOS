/*
 *  @file Recorder_tests.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 24.07.19
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

#include "Audio/Recorder.hpp"
#include "Audio/Profiles/ProfilePlaybackLoudspeaker.hpp"
#include "Audio/Profiles/ProfilePlaybackHeadphones.hpp"

TEST_CASE( "Recorder tests" ) {

    auto cwd = vfs.getcurrdir();
    cwd = cwd.substr(0,cwd.find_last_of("/\\"));
    cwd = cwd.append("/module-audio/tests/samples");

    SECTION("Simple recording"){
        ProfilePlaybackLoudspeaker  profileLoudspeaker(nullptr,100,0);
        Recorder record((cwd + "/rec1.wav").c_str(),&profileLoudspeaker);
        record.Start();
        REQUIRE(record.GetState() == Recorder::State::Recording);
        sleep(2);
        record.Stop();
        REQUIRE(record.GetState() == Recorder::State::Idle);
    }

}
