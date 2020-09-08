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
