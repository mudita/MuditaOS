/*
 * @file unittests_audio.cpp
 * @brief Audio system unit tests
 * @copyright Copyright (C) 2020 mudita.com
 * @details
 */

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include "Audio/decoder/decoder.hpp"

#include "Audio/decoder/decoderMP3.hpp"
#include "Audio/decoder/decoderFLAC.hpp"
#include "Audio/decoder/decoderWAV.hpp"

#include "Audio/AudioCommon.hpp"

class vfs vfs;

TEST_CASE("Test audio tags")
{
    std::vector<std::string> testExtensions = {"flac", "wav", "mp3"};
    for (auto ext : testExtensions) {
        auto dec = audio::decoder::Create(("testfiles/audio." + ext).c_str());
        REQUIRE(dec);
        auto tags = dec->fetchTags();
        REQUIRE(tags);
        REQUIRE(tags->title == ext + " Test track title");
        REQUIRE(tags->artist == ext + " Test artist name");
        REQUIRE(tags->album == ext + " Test album title");
        REQUIRE(tags->year == "2020");
    }
}

TEST_CASE("Audio settings string creation")
{
    SECTION("Create volume string for playback loudspeaker, multimedia")
    {
        const auto str = audio::str(
            audio::Profile::Type::PlaybackLoudspeaker, audio::Setting::Volume, audio::PlaybackType::Multimedia);
        REQUIRE_FALSE(str.empty());
        REQUIRE(str == "audio/PlaybackLoudspeaker/Multimedia/volume");
    }

    SECTION("Create volume string for routing speakerphone")
    {
        const auto str = audio::str(audio::Profile::Type::RoutingSpeakerphone, audio::Setting::Volume);
        REQUIRE_FALSE(str.empty());
        REQUIRE(str == "audio/RoutingSpeakerphone/volume");
    }

    SECTION("Create gain string for recording built-in microphone")
    {
        const auto str = audio::str(audio::Profile::Type::RecordingBuiltInMic, audio::Setting::Gain);
        REQUIRE_FALSE(str.empty());
        REQUIRE(str == "audio/RecordingBuiltInMic/gain");
    }

    SECTION("Create empty volume string when Idle")
    {
        const auto str = audio::str(audio::Profile::Type::Idle, audio::Setting::Volume);
        REQUIRE(str.empty());
    }
}
