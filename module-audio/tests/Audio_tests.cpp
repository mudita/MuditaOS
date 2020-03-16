/*
 *  @file Audio_tests.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 29.07.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#include <iostream>
#include <memory>
#include <filesystem>
#include <cstring>
#include <unistd.h>

#include "catch.hpp"

#include "Audio/Audio.hpp"

using namespace audio;

namespace fs = std::filesystem;
static std::string testOutPath{};

TEST_CASE("Audio API tests")
{

    auto cwd = vfs.getcurrdir();
    cwd      = cwd.substr(0, cwd.find_last_of("/\\"));
    cwd      = cwd.append("/module-audio/tests/samples");

#if 0
    SECTION("INIT"){
        fs::path dir = fs::current_path();

        std::uintmax_t n = fs::remove_all(dir / "tests_out/audio");
        std::cout << "Deleted " << n << " files or directories\n";

        fs::create_directories(dir / "tests_out/audio");

        testOutPath = dir / "tests_out/audio/";
    }


    SECTION("Switch: Idle->Playback->Recording->Idle"){

        Audio audioTest([](AudioEvents events)->int32_t{std::cout << "End of file\n"; return 0;});

        REQUIRE(audioTest.Start(Operation::Type::Playback,(cwd + "/sample1.wav").c_str()) == static_cast<int32_t >(RetCode::Success));
        REQUIRE(audioTest.GetCurrentState() == Audio::State::Playback);
        sleep(1);
        REQUIRE(audioTest.Start(Operation::Type::Recorder,"rec1.wav") == static_cast<int32_t >(RetCode::Success));
        REQUIRE(audioTest.GetCurrentState() == Audio::State::Recording);
        sleep(1);
        REQUIRE(audioTest.Stop() == static_cast<int32_t >(RetCode::Success));
        REQUIRE(audioTest.GetCurrentState() == Audio::State::Idle);

        fs::remove("rec1.wav");
    }
#endif
    SECTION("Switch: Idle->Routing->Idle")
    {

        Audio audioTest([](AudioEvents events) -> int32_t {
            std::cout << "End of file\n";
            return 0;
        });

        REQUIRE(audioTest.Start(Operation::Type::Router, "") == static_cast<int32_t>(RetCode::Success));
        REQUIRE(audioTest.GetCurrentState() == Audio::State::Routing);
        sleep(1);
        REQUIRE(audioTest.Stop() == static_cast<int32_t>(RetCode::Success));
        REQUIRE(audioTest.GetCurrentState() == Audio::State::Idle);

        fs::remove("rec1.wav");
    }
}
