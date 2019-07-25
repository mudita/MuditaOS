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

namespace fs = std::filesystem;
std::string testOutPath{};

TEST_CASE( "RecorderOperation tests" ) {

    SECTION("INIT"){
        fs::path dir = fs::current_path();

        std::uintmax_t n = fs::remove_all(dir / "tests_out/audio");
        std::cout << "Deleted " << n << " files or directories\n";

        fs::create_directories(dir / "tests_out/audio");

        testOutPath = dir / "tests_out/audio/";
    }

    SECTION("Simple stereo recording 44100"){
        ProfileRecordingOnBoardMic  profileRecordingOnBoardMic(nullptr,20.0);
        RecorderOperation record((testOutPath + "stereotrec_44100.wav").c_str(),&profileRecordingOnBoardMic,Encoder::Format{.chanNr=2,.sampleRate=44100,.sampleSiz=16});
        record.Start([](uint32_t)->int32_t{
            std::cout<<"Error!\n";
            return 0;
        });
        REQUIRE(record.GetState() == RecorderOperation::State::Active);
        sleep(2);
        record.Stop();
        REQUIRE(record.GetState() == RecorderOperation::State::Idle);
        REQUIRE( record.GetPosition() == Approx(2).margin(0.1) );
    }

    SECTION("Simple mono recording 44100"){
        ProfileRecordingOnBoardMic  profileRecordingOnBoardMic(nullptr,20.0);
        RecorderOperation record((testOutPath + "monorec1_44100.wav").c_str(),&profileRecordingOnBoardMic,Encoder::Format{.chanNr=1,.sampleRate=44100,.sampleSiz=16});
        record.Start([](uint32_t)->int32_t{
            std::cout<<"Error!\n";
            return 0;
        });
        REQUIRE(record.GetState() == RecorderOperation::State::Active);
        sleep(2);
        record.Stop();
        REQUIRE(record.GetState() == RecorderOperation::State::Idle);
        REQUIRE( record.GetPosition() == Approx(2).margin(0.1) );
    }

    SECTION("Simple stereo recording 22050"){
        ProfileRecordingOnBoardMic  profileRecordingOnBoardMic(nullptr,20.0);
        RecorderOperation record((testOutPath + "stereotrec_22050.wav").c_str(),&profileRecordingOnBoardMic,Encoder::Format{.chanNr=2,.sampleRate=22050,.sampleSiz=16});
        record.Start([](uint32_t)->int32_t{
            std::cout<<"Error!\n";
            return 0;
        });
        REQUIRE(record.GetState() == RecorderOperation::State::Active);
        sleep(2);
        record.Stop();
        REQUIRE(record.GetState() == RecorderOperation::State::Idle);
        REQUIRE( record.GetPosition() == Approx(2).margin(0.1) );
    }

    SECTION("Simple mono recording 22050"){
        ProfileRecordingOnBoardMic  profileRecordingOnBoardMic(nullptr,20.0);
        RecorderOperation record((testOutPath + "monorec1_22050.wav").c_str(),&profileRecordingOnBoardMic,Encoder::Format{.chanNr=1,.sampleRate=22050,.sampleSiz=16});
        record.Start([](uint32_t)->int32_t{
            std::cout<<"Error!\n";
            return 0;
        });
        REQUIRE(record.GetState() == RecorderOperation::State::Active);
        sleep(2);
        record.Stop();
        REQUIRE(record.GetState() == RecorderOperation::State::Idle);
        REQUIRE( record.GetPosition() == Approx(2).margin(0.1) );
    }

    SECTION("Simple stereo recording 16000"){
        ProfileRecordingOnBoardMic  profileRecordingOnBoardMic(nullptr,20.0);
        RecorderOperation record((testOutPath + "stereotrec_16000.wav").c_str(),&profileRecordingOnBoardMic,Encoder::Format{.chanNr=2,.sampleRate=16000,.sampleSiz=16});
        record.Start([](uint32_t)->int32_t{
            std::cout<<"Error!\n";
            return 0;
        });
        REQUIRE(record.GetState() == RecorderOperation::State::Active);
        sleep(2);
        record.Stop();
        REQUIRE(record.GetState() == RecorderOperation::State::Idle);
        REQUIRE( record.GetPosition() == Approx(2).margin(0.1) );
    }

    SECTION("Simple mono recording 16000"){
        ProfileRecordingOnBoardMic  profileRecordingOnBoardMic(nullptr,20.0);
        RecorderOperation record((testOutPath + "monorec1_16000.wav").c_str(),&profileRecordingOnBoardMic,Encoder::Format{.chanNr=1,.sampleRate=16000,.sampleSiz=16});
        record.Start([](uint32_t)->int32_t{
            std::cout<<"Error!\n";
            return 0;
        });
        REQUIRE(record.GetState() == RecorderOperation::State::Active);
        sleep(2);
        record.Stop();
        REQUIRE(record.GetState() == RecorderOperation::State::Idle);
        REQUIRE( record.GetPosition() == Approx(2).margin(0.1) );
    }

    SECTION("Simple stereo recording 8000"){
        ProfileRecordingOnBoardMic  profileRecordingOnBoardMic(nullptr,20.0);
        RecorderOperation record((testOutPath + "stereotrec_8000.wav").c_str(),&profileRecordingOnBoardMic,Encoder::Format{.chanNr=2,.sampleRate=8000,.sampleSiz=16});
        record.Start([](uint32_t)->int32_t{
            std::cout<<"Error!\n";
            return 0;
        });
        REQUIRE(record.GetState() == RecorderOperation::State::Active);
        sleep(2);
        record.Stop();
        REQUIRE(record.GetState() == RecorderOperation::State::Idle);
        REQUIRE( record.GetPosition() == Approx(2).margin(0.1) );
    }

    SECTION("Simple mono recording 8000"){
        ProfileRecordingOnBoardMic  profileRecordingOnBoardMic(nullptr,20.0);
        RecorderOperation record((testOutPath + "monorec1_8000.wav").c_str(),&profileRecordingOnBoardMic,Encoder::Format{.chanNr=1,.sampleRate=8000,.sampleSiz=16});
        record.Start([](uint32_t)->int32_t{
            std::cout<<"Error!\n";
            return 0;
        });
        REQUIRE(record.GetState() == RecorderOperation::State::Active);
        sleep(2);
        record.Stop();
        REQUIRE(record.GetState() == RecorderOperation::State::Idle);
        REQUIRE( record.GetPosition() == Approx(2).margin(0.1) );
    }



SECTION("Simple stereo recording with profile switching 44100"){
        ProfileRecordingOnBoardMic  profileRecordingOnBoardMic(nullptr,20.0);
        ProfileRecordingHeadset  profileRecordingHeadset(nullptr,50.0);
        RecorderOperation record((testOutPath + "rec1.wav").c_str(),&profileRecordingOnBoardMic,Encoder::Format{.chanNr=2,.sampleRate=44100,.sampleSiz=16});
        record.Start([](uint32_t)->int32_t{
            std::cout<<"Error!\n";
            return 0;
        });
        REQUIRE(record.GetState() == RecorderOperation::State::Active);
        sleep(1);
        record.SwitchProfile(&profileRecordingHeadset);
        REQUIRE(record.GetState() == RecorderOperation::State::Active);
        sleep(5);
        record.Stop();
        REQUIRE(record.GetState() == RecorderOperation::State::Idle);
        REQUIRE( record.GetPosition() == Approx(6).margin(0.1) );
    }

}
