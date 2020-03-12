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
#include <filesystem>

#include "catch.hpp"

#include "vfs.hpp"

#include "Audio/Operation/RecorderOperation.hpp"
#include "Audio/Profiles/ProfileRecordingHeadset.hpp"
#include "Audio/Profiles/ProfileRecordingOnBoardMic.hpp"

using namespace audio;

namespace fs = std::filesystem;
static std::string testOutPath{};

TEST_CASE("RecorderOperation tests")
{

    SECTION("INIT")
    {
        fs::path dir = fs::current_path();

        std::uintmax_t n = fs::remove_all(dir / "tests_out/audio");
        std::cout << "Deleted " << n << " files or directories\n";

        fs::create_directories(dir / "tests_out/audio");

        testOutPath = dir / "tests_out/audio/";
    }
#if 0
    SECTION("Simple stereo recording 44100"){
        RecorderOperation record((testOutPath + "stereotrec_44100.wav").c_str());
        record.Start([](AudioEvents event)->int32_t{
            std::cout<<"Error!\n";
            return 0;
        });
        REQUIRE(record.GetState() == RecorderOperation::State::Active);
        sleep(2);
        record.Stop();
        REQUIRE(record.GetState() == RecorderOperation::State::Idle);
        REQUIRE( record.GetPosition() == Approx(2).margin(0.1) );
    }
#endif

    SECTION("Simple mono recording 44100")
    {
        RecorderOperation record((testOutPath + "monorec1_44100.wav").c_str());
        record.Start([](AudioEvents event) -> int32_t {
            std::cout << "Error!\n";
            return 0;
        });
        REQUIRE(record.GetState() == RecorderOperation::State::Active);
        sleep(2);
        record.Stop();
        REQUIRE(record.GetState() == RecorderOperation::State::Idle);
        REQUIRE(record.GetPosition() == Approx(2).margin(0.1));
    }

    SECTION("Simple stereo recording with profile switching 44100")
    {
        RecorderOperation record((testOutPath + "rec1.wav").c_str());
        record.Start([](AudioEvents event) -> int32_t {
            std::cout << "Error!\n";
            return 0;
        });
        REQUIRE(record.GetState() == RecorderOperation::State::Active);
        sleep(1);
        record.SwitchProfile(Profile::Type::RecordingHeadset);
        REQUIRE(record.GetState() == RecorderOperation::State::Active);
        sleep(5);
        record.Stop();
        REQUIRE(record.GetState() == RecorderOperation::State::Idle);
        REQUIRE(record.GetPosition() == Approx(6).margin(0.1));
    }
}
