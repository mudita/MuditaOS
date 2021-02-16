// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include "Audio/decoder/Decoder.hpp"

#include "Audio/decoder/decoderMP3.hpp"
#include "Audio/decoder/decoderFLAC.hpp"
#include "Audio/decoder/decoderWAV.hpp"

#include "Audio/AudioCommon.hpp"

#include "Audio/AudioMux.hpp"
#include "Audio/Audio.hpp"
#include "Audio/Operation/Operation.hpp"


TEST_CASE("Test audio tags")
{
    SECTION(" Encoder tests ")
    {
        std::vector<std::string> testExtensions = {"flac", "wav", "mp3"};
        for (auto ext : testExtensions) {
            auto dec = audio::Decoder::Create(("testfiles/audio." + ext).c_str());
            REQUIRE(dec);
            auto tags = dec->fetchTags();
            REQUIRE(tags);
            REQUIRE(tags->title == ext + " Test track title");
            REQUIRE(tags->artist == ext + " Test artist name");
            REQUIRE(tags->album == ext + " Test album title");
            REQUIRE(tags->year == "2020");
        }
    }
}

TEST_CASE("Audio settings string creation")
{
    SECTION("Create volume string for playback loudspeaker, multimedia")
    {
        const auto str = audio::dbPath(
            audio::Setting::Volume, audio::PlaybackType::Multimedia, audio::Profile::Type::PlaybackLoudspeaker);
        REQUIRE_FALSE(str.empty());
        REQUIRE(str == "audio/PlaybackLoudspeaker/Multimedia/Volume");
    }

    SECTION("Create volume string for routing speakerphone")
    {
        const auto str =
            audio::dbPath(audio::Setting::Volume, audio::PlaybackType::None, audio::Profile::Type::RoutingLoudspeaker);
        REQUIRE_FALSE(str.empty());
        REQUIRE(str == "audio/RoutingLoudspeaker/Volume");
    }

    SECTION("Create gain string for recording built-in microphone")
    {
        const auto str =
            audio::dbPath(audio::Setting::Gain, audio::PlaybackType::None, audio::Profile::Type::RecordingBuiltInMic);
        REQUIRE_FALSE(str.empty());
        REQUIRE(str == "audio/RecordingBuiltInMic/Gain");
    }

    SECTION("Create empty volume string when Idle")
    {
        const auto str = audio::dbPath(audio::Setting::Volume, audio::PlaybackType::None, audio::Profile::Type::Idle);
        REQUIRE(str.empty());
    }
}

class MockAudio : public audio::Audio
{

  public:
    MockAudio(audio::Audio::State state, audio::PlaybackType plbckType, audio::Operation::State opState)
        : audio::Audio([](const sys::Message *e) { return std::optional<std::string>(); }), state(state),
          plbckType(plbckType), opState(opState)
    {}

    audio::PlaybackType GetCurrentOperationPlaybackType() const override
    {
        return plbckType;
    }

    audio::Operation::State GetCurrentOperationState() const override
    {
        return opState;
    }

    State GetCurrentState() const override
    {
        return state;
    }

    State state = State::Idle;
    audio::PlaybackType plbckType;
    audio::Operation::State opState;
};

TEST_CASE("Test AudioMux")
{
    using namespace audio;

    int16_t tkId = 0;

    auto insertAudio = [](std::vector<AudioMux::Input> &inputs,
                          Audio::State state,
                          PlaybackType plbckType,
                          Operation::State opState,
                          int16_t t) {
        inputs.emplace_back(AudioMux::Input(std::make_unique<MockAudio>(state, plbckType, opState), Token(t)));
        return t;
    };

    SECTION("Check Audio::Mux GetInput by Token")
    {
        int16_t tokenIdx = 1;
        std::vector<AudioMux::Input> audioInputs;
        AudioMux aMux(audioInputs);

        GIVEN("One Input")
        {
            WHEN("Input exists")
            {
                tkId =
                    insertAudio(audioInputs, Audio::State::Idle, PlaybackType::None, Operation::State::Idle, tokenIdx);
                REQUIRE(aMux.GetInput(Token(tkId)) != std::nullopt);
            }

            WHEN("Input does not exists")
            {
                insertAudio(audioInputs, Audio::State::Idle, PlaybackType::None, Operation::State::Idle, tokenIdx);
                REQUIRE(aMux.GetInput(Token(tokenIdx + 1)) == std::nullopt);
            }

            WHEN("Token passed is not valid")
            {
                insertAudio(audioInputs, Audio::State::Idle, PlaybackType::None, Operation::State::Idle, tokenIdx);
                REQUIRE(aMux.GetInput(Token::MakeBadToken()) == std::nullopt);
            }

            WHEN("Token passed is uninitialized")
            {
                insertAudio(audioInputs, Audio::State::Idle, PlaybackType::None, Operation::State::Idle, tokenIdx);
                REQUIRE(aMux.GetInput(Token()) == std::nullopt);
            }
        }

        GIVEN("N Inputs")
        {
            insertAudio(audioInputs, Audio::State::Idle, PlaybackType::None, Operation::State::Idle, tokenIdx++);
            insertAudio(audioInputs, Audio::State::Idle, PlaybackType::None, Operation::State::Idle, tokenIdx++);
            tkId = insertAudio(audioInputs, Audio::State::Idle, PlaybackType::None, Operation::State::Idle, tokenIdx++);
            insertAudio(audioInputs, Audio::State::Idle, PlaybackType::None, Operation::State::Idle, tokenIdx++);

            WHEN("Input exists")
            {
                REQUIRE(aMux.GetInput(Token(tkId)) != std::nullopt);
            }

            WHEN("Input does not exists")
            {
                insertAudio(audioInputs, Audio::State::Idle, PlaybackType::None, Operation::State::Idle, tokenIdx);
                REQUIRE(aMux.GetInput(Token(tokenIdx + 1)) == std::nullopt);
            }
        }
    }

    SECTION("Check Audio::Mux GetInput by State")
    {
        int16_t tokenIdx = 1;
        std::vector<AudioMux::Input> audioInputs;
        AudioMux aMux(audioInputs);

        auto testState1       = Audio::State::Playback;
        auto testState2       = Audio::State::Recording;
        auto testStateExcuded = Audio::State::Routing;

        GIVEN("One Input")
        {
            WHEN("Input with given State exists")
            {
                tkId = insertAudio(audioInputs, testState1, PlaybackType::None, Operation::State::Idle, tokenIdx);
                WHEN("One State in vector")
                {
                    auto retInput = aMux.GetInput({testState1});
                    REQUIRE(retInput != std::nullopt);
                    REQUIRE((*retInput)->token == Token(tkId));
                }
                WHEN("Multiple State enums in vector")
                {
                    auto retInput = aMux.GetInput({testState2, testState1});
                    REQUIRE(retInput != std::nullopt);
                    REQUIRE((*retInput)->token == Token(tkId));
                }
            }

            WHEN("Input with given State does not exists")
            {
                insertAudio(audioInputs, testStateExcuded, PlaybackType::None, Operation::State::Idle, tokenIdx);
                REQUIRE(aMux.GetInput(Token(tokenIdx + 1)) == std::nullopt);
            }
        }

        GIVEN("N Inputs")
        {
            insertAudio(audioInputs, Audio::State::Idle, PlaybackType::None, Operation::State::Idle, tokenIdx++);
            insertAudio(audioInputs, Audio::State::Idle, PlaybackType::None, Operation::State::Idle, tokenIdx++);
            tkId = insertAudio(audioInputs, testState1, PlaybackType::None, Operation::State::Idle, tokenIdx++);
            insertAudio(audioInputs, testState2, PlaybackType::None, Operation::State::Idle, tokenIdx++);
            WHEN("Input with given State exists")
            {
                WHEN("One State in vector")
                {
                    auto retInput = aMux.GetInput({testState1});
                    REQUIRE(retInput != std::nullopt);
                    REQUIRE((*retInput)->token == Token(tkId));
                }
                WHEN("Multiple State enums in vector")
                {
                    auto retInput = aMux.GetInput({testState1, testState2});
                    REQUIRE(retInput != std::nullopt);
                    REQUIRE((*retInput)->token == Token(tkId));
                }
                WHEN("Multiple State enums in vector (changed order in vector)")
                {
                    auto retInput = aMux.GetInput({testState2, testState1});
                    REQUIRE(retInput != std::nullopt);
                    REQUIRE((*retInput)->token == Token(tkId));
                }
            }
        }
    }

    SECTION("Check Audio::Mux GetAvailableInput")
    {
        int16_t tokenIdx = 1;
        std::vector<AudioMux::Input> audioInputs;
        AudioMux aMux(audioInputs);

        auto testPlaybackTypeLowPrio  = PlaybackType::Alarm;
        auto testPlaybackTypeMidPrio  = PlaybackType::Multimedia;
        auto testPlaybackTypeHighPrio = PlaybackType::CallRingtone;

        auto mergableType = PlaybackType::Notifications;

        GIVEN("One Input")
        {
            WHEN("When idle input available")
            {
                tkId =
                    insertAudio(audioInputs, Audio::State::Idle, PlaybackType::None, Operation::State::Idle, tokenIdx);
                auto retInput = aMux.GetAvailableInput(testPlaybackTypeLowPrio);
                REQUIRE(retInput != std::nullopt);
                REQUIRE((*retInput)->token == Token(tkId));
            }
            WHEN("Should reject due to higher priority PlaybackType active")
            {
                tkId = insertAudio(
                    audioInputs, Audio::State::Playback, testPlaybackTypeMidPrio, Operation::State::Idle, tokenIdx);
                auto retInput = aMux.GetAvailableInput(testPlaybackTypeLowPrio);
                REQUIRE(retInput == std::nullopt);
            }
            WHEN("Should take over due to lower priority PlaybackType active")
            {
                tkId = insertAudio(
                    audioInputs, Audio::State::Playback, testPlaybackTypeLowPrio, Operation::State::Idle, tokenIdx);
                auto retInput = aMux.GetAvailableInput(testPlaybackTypeMidPrio);
                REQUIRE(retInput != std::nullopt);
                REQUIRE((*retInput)->token == Token(tkId));
            }
            WHEN("Should merge due to same mergable type active")
            {
                tkId = insertAudio(audioInputs, Audio::State::Playback, mergableType, Operation::State::Idle, tokenIdx);
                auto retInput = aMux.GetAvailableInput(mergableType);
                REQUIRE(retInput == std::nullopt);
            }
        }

        GIVEN("N Inputs")
        {
            insertAudio(
                audioInputs, Audio::State::Playback, testPlaybackTypeLowPrio, Operation::State::Idle, tokenIdx++);
            tkId = insertAudio(
                audioInputs, Audio::State::Playback, testPlaybackTypeLowPrio, Operation::State::Idle, tokenIdx++);
            WHEN("When idle input available")
            {
                tkId =
                    insertAudio(audioInputs, Audio::State::Idle, PlaybackType::None, Operation::State::Idle, tokenIdx);
                auto retInput = aMux.GetAvailableInput(testPlaybackTypeLowPrio);
                REQUIRE(retInput != std::nullopt);
                REQUIRE((*retInput)->token == Token(tkId));
            }
            WHEN("Should reject due to higher priority PlaybackType active")
            {
                insertAudio(
                    audioInputs, Audio::State::Playback, testPlaybackTypeHighPrio, Operation::State::Idle, tokenIdx);
                auto retInput = aMux.GetAvailableInput(testPlaybackTypeLowPrio);
                REQUIRE(retInput == std::nullopt);
            }
            WHEN("Should reject due to higher priority PlaybackType active even if Idle available")
            {
                insertAudio(audioInputs, Audio::State::Idle, PlaybackType::None, Operation::State::Idle, tokenIdx);
                insertAudio(
                    audioInputs, Audio::State::Playback, testPlaybackTypeHighPrio, Operation::State::Idle, tokenIdx);
                auto retInput = aMux.GetAvailableInput(testPlaybackTypeLowPrio);
                REQUIRE(retInput == std::nullopt);
            }
            WHEN("Should take over due to lower priority PlaybackType active")
            {
                auto retInput = aMux.GetAvailableInput(testPlaybackTypeMidPrio);
                REQUIRE(retInput != std::nullopt);
                REQUIRE((*retInput)->token == Token(tkId));
            }
            WHEN("Should merge due to same mergable type active")
            {
                insertAudio(audioInputs, Audio::State::Playback, mergableType, Operation::State::Idle, tokenIdx);
                auto retInput = aMux.GetAvailableInput(mergableType);
                REQUIRE(retInput == std::nullopt);
            }
            WHEN("Should merge due to same mergable type active even if Idle available")
            {
                insertAudio(audioInputs, Audio::State::Idle, PlaybackType::None, Operation::State::Idle, tokenIdx);
                insertAudio(
                    audioInputs, Audio::State::Playback, testPlaybackTypeMidPrio, Operation::State::Idle, tokenIdx);
                auto retInput = aMux.GetAvailableInput(testPlaybackTypeMidPrio);
                REQUIRE(retInput != std::nullopt);
            }
        }
    }

    SECTION("Check Audio::Mux GetPlaybackInput")
    {
        int16_t tokenIdx = 1;
        std::vector<AudioMux::Input> audioInputs;
        AudioMux aMux(audioInputs);

        auto testPlaybackTypeLowPrio  = PlaybackType::Multimedia;
        auto testPlaybackTypeHighPrio = PlaybackType::CallRingtone;

        GIVEN("One Input")
        {
            WHEN("When free inputs available")
            {
                tkId = insertAudio(
                    audioInputs, Audio::State::Playback, PlaybackType::Multimedia, Operation::State::Active, tokenIdx);
                auto retInput = aMux.GetPlaybackInput(testPlaybackTypeLowPrio);
                REQUIRE(retInput != std::nullopt);
                REQUIRE((*retInput)->token == Token(tkId));
            }
            WHEN("Should reject due to Recording active")
            {
                tkId = insertAudio(
                    audioInputs, Audio::State::Recording, PlaybackType::None, Operation::State::Idle, tokenIdx);
                auto retInput = aMux.GetPlaybackInput(testPlaybackTypeHighPrio);
                REQUIRE(retInput == std::nullopt);
            }
            WHEN("Should reject due to Routing active")
            {
                tkId = insertAudio(
                    audioInputs, Audio::State::Routing, PlaybackType::None, Operation::State::Idle, tokenIdx);
                auto retInput = aMux.GetPlaybackInput(testPlaybackTypeHighPrio);
                REQUIRE(retInput == std::nullopt);
            }
        }
    }

    SECTION("Check Audio::Mux GetRoutingInput")
    {
        int16_t tokenIdx = 1;
        std::vector<AudioMux::Input> audioInputs;
        AudioMux aMux(audioInputs);

        GIVEN("One Input")
        {
            WHEN("When Routing input active")
            {
                tkId = insertAudio(
                    audioInputs, Audio::State::Routing, PlaybackType::None, Operation::State::Active, tokenIdx);
                auto retInput = aMux.GetRoutingInput();
                REQUIRE(retInput != std::nullopt);
                REQUIRE((*retInput)->token == Token(tkId));
            }
            WHEN("When active Routing not available without force")
            {
                tkId          = insertAudio(audioInputs,
                                   Audio::State::Playback,
                                   PlaybackType::CallRingtone,
                                   Operation::State::Active,
                                   tokenIdx);
                auto retInput = aMux.GetRoutingInput();
                REQUIRE(retInput == std::nullopt);
            }
            WHEN("When active Routing not available with force")
            {
                tkId          = insertAudio(audioInputs,
                                   Audio::State::Playback,
                                   PlaybackType::CallRingtone,
                                   Operation::State::Active,
                                   tokenIdx);
                auto retInput = aMux.GetRoutingInput(true);
                REQUIRE(retInput != std::nullopt);
                REQUIRE((*retInput)->token == Token(tkId));
            }
        }
    }
}
