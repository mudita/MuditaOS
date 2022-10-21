// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include "Audio/decoder/Decoder.hpp"
#include "Audio/AudioCommon.hpp"
#include "Audio/AudioMux.hpp"
#include "Audio/Audio.hpp"
#include "Audio/Operation/Operation.hpp"
#include <Audio/Operation/RouterOperation.hpp>

using namespace audio;

#include <memory>
#include <utility>

TEST_CASE("Audio Decoder")
{
    std::vector<std::string> testExtensions = {"flac", "wav", "mp3"};
    for (auto ext : testExtensions) {
        auto dec = audio::Decoder::Create(("testfiles/audio." + ext).c_str());
        REQUIRE(dec);
    }
}

TEST_CASE(" Tags fetcher ")
{
    std::vector<std::string> testExtensions = {"flac", "wav", "mp3"};
    for (auto ext : testExtensions) {
        auto tags = tags::fetcher::fetchTags(("testfiles/audio." + ext).c_str());
        REQUIRE(tags.title == ext + " Test track title - łąki");
        REQUIRE(tags.artist == ext + " Test artist name - łąki");
        REQUIRE(tags.album == ext + " Test album title - łąki");
        REQUIRE(tags.year == 2020);
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

    SECTION("System settings change")
    {
        struct TestCase
        {
            PlaybackType playbackType;
            Setting setting;
            std::string path;
        };

        std::vector<TestCase> testCases = {
            // system volume
            {PlaybackType::System, Setting::Volume, "audio/RecordingBuiltInMic/Notifications/Volume"},
            {PlaybackType::Meditation, Setting::Volume, "audio/RecordingBuiltInMic/Meditation/Volume"},
            {PlaybackType::CallRingtone, Setting::Volume, "audio/RecordingBuiltInMic/CallRingtone/Volume"},
            {PlaybackType::KeypadSound, Setting::Volume, "audio/RecordingBuiltInMic/KeypadSound/Volume"},
            {PlaybackType::TextMessageRingtone,
             Setting::Volume,
             "audio/RecordingBuiltInMic/TextMessageRingtone/Volume"},
            {PlaybackType::Notifications, Setting::Volume, "audio/RecordingBuiltInMic/Notifications/Volume"},
            {PlaybackType::Alarm, Setting::Volume, "audio/RecordingBuiltInMic/Alarm/Volume"},
            {PlaybackType::Multimedia, Setting::Volume, "audio/RecordingBuiltInMic/Multimedia/Volume"},
            {PlaybackType::None, Setting::Volume, "audio/RecordingBuiltInMic/Volume"},

            // IsSystemSound
            {PlaybackType::System, Setting::IsSystemSound, "audio/RecordingBuiltInMic/Notifications/IsSystemSound"},
            {PlaybackType::Meditation, Setting::IsSystemSound, "audio/RecordingBuiltInMic/Meditation/IsSystemSound"},
            {PlaybackType::CallRingtone,
             Setting::IsSystemSound,
             "audio/RecordingBuiltInMic/CallRingtone/IsSystemSound"},
            {PlaybackType::KeypadSound, Setting::IsSystemSound, "audio/RecordingBuiltInMic/KeypadSound/IsSystemSound"},
            {PlaybackType::TextMessageRingtone,
             Setting::IsSystemSound,
             "audio/RecordingBuiltInMic/TextMessageRingtone/IsSystemSound"},
            {PlaybackType::Notifications,
             Setting::IsSystemSound,
             "audio/RecordingBuiltInMic/Notifications/IsSystemSound"},
            {PlaybackType::Alarm, Setting::IsSystemSound, "audio/RecordingBuiltInMic/Alarm/IsSystemSound"},
            {PlaybackType::Multimedia, Setting::IsSystemSound, "audio/RecordingBuiltInMic/Multimedia/IsSystemSound"},
            {PlaybackType::None, Setting::IsSystemSound, "audio/RecordingBuiltInMic/IsSystemSound"},

            // EnableSound
            {PlaybackType::System, Setting::EnableSound, "audio/RecordingBuiltInMic/Notifications/EnableSound"},
            {PlaybackType::Meditation, Setting::EnableSound, "audio/RecordingBuiltInMic/Meditation/EnableSound"},
            {PlaybackType::CallRingtone, Setting::EnableSound, "audio/RecordingBuiltInMic/CallRingtone/EnableSound"},
            {PlaybackType::KeypadSound, Setting::EnableSound, "audio/RecordingBuiltInMic/KeypadSound/EnableSound"},
            {PlaybackType::TextMessageRingtone,
             Setting::EnableSound,
             "audio/RecordingBuiltInMic/TextMessageRingtone/EnableSound"},
            {PlaybackType::Notifications, Setting::EnableSound, "audio/RecordingBuiltInMic/Notifications/EnableSound"},
            {PlaybackType::Alarm, Setting::EnableSound, "audio/RecordingBuiltInMic/Alarm/EnableSound"},
            {PlaybackType::Multimedia, Setting::EnableSound, "audio/RecordingBuiltInMic/Multimedia/EnableSound"},
            {PlaybackType::None, Setting::EnableSound, "audio/RecordingBuiltInMic/EnableSound"},
        };

        for (auto &testCase : testCases) {
            const auto str =
                audio::dbPath(testCase.setting, testCase.playbackType, audio::Profile::Type::RecordingBuiltInMic);
            REQUIRE_FALSE(str.empty());
            REQUIRE(str == testCase.path);
        }
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

    void setConnected(EventType deviceUpdateEvent)
    {
        audioSinkState.setConnected(deviceUpdateEvent, true);
    }

    void setDisconnected(EventType deviceUpdateEvent)
    {
        audioSinkState.setConnected(deviceUpdateEvent, false);
    }

    State state = State::Idle;
    audio::PlaybackType plbckType;
    audio::Operation::State opState;
};

class MockRouterOperation : public RouterOperation
{
    bool loudspeakerEnabled = false;

    bool isLoudspeakerOn() const
    {
        return std::find_if(supportedProfiles.begin(), supportedProfiles.end(), [this](const SupportedProfile &s) {
                   return (s.profile->GetType() == Profile::Type::RoutingLoudspeaker) && s.isAvailable &&
                          loudspeakerEnabled;
               }) != std::end(supportedProfiles);
    }

  public:
    /* value - volume and gain value */
    explicit MockRouterOperation(AudioServiceMessage::Callback callback)
        : RouterOperation(std::string(""), std::move(callback))
    {}

    std::optional<Profile::Type> getPriorityProfile() const
    {
        if (isLoudspeakerOn()) {
            return Profile::Type::RoutingLoudspeaker;
        }
        return Operation::GetPriorityProfile();
    }

    void enableLoudspeaker() noexcept
    {
        loudspeakerEnabled = true;
    }

    void disableLoudspeaker() noexcept
    {
        loudspeakerEnabled = false;
    }
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

        auto testPlaybackTypeLowPrio  = PlaybackType::Multimedia;
        auto testPlaybackTypeMidPrio  = PlaybackType::Alarm;
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

SCENARIO("Profile playback priorities tests")
{
    GIVEN("Audio idle instance")
    {
        MockAudio audio{Audio::State::Idle, PlaybackType::None, Operation::State::Idle};
        WHEN("All audio devices are connected")
        {
            audio.setConnected(EventType::JackState);
            audio.setConnected(EventType::BlutoothA2DPDeviceState);
            THEN("Headphones are prioritized")
            {
                REQUIRE(audio.GetPriorityPlaybackProfile() == Profile::Type::PlaybackHeadphones);
            }
            AND_WHEN("Bluetooth is disconnected")
            {
                audio.setDisconnected(EventType::BlutoothA2DPDeviceState);
                THEN("Headphones are prioritized")
                {
                    REQUIRE(audio.GetPriorityPlaybackProfile() == Profile::Type::PlaybackHeadphones);
                }
            }
            AND_WHEN("Headphones are disconnected")
            {
                audio.setDisconnected(EventType::JackState);
                THEN("Bluetooth is prioritized")
                {
                    REQUIRE(audio.GetPriorityPlaybackProfile() == Profile::Type::PlaybackBluetoothA2DP);
                }
            }
        }
        WHEN("Only bluetooth device is connected")
        {
            audio.setConnected(EventType::BlutoothA2DPDeviceState);
            THEN("Bluetooth is prioritized")
            {
                REQUIRE(audio.GetPriorityPlaybackProfile() == Profile::Type::PlaybackBluetoothA2DP);
            }
        }
        WHEN("Only headphones are connected")
        {
            audio.setConnected(EventType::JackState);
            THEN("Headphones are prioritized")
            {
                REQUIRE(audio.GetPriorityPlaybackProfile() == Profile::Type::PlaybackHeadphones);
            }
        }
        WHEN("All audio devices are disconnected")
        {
            THEN("Loudspeaker is prioritized")
            {
                REQUIRE(audio.GetPriorityPlaybackProfile() == Profile::Type::PlaybackLoudspeaker);
            }
        }
    }
}

SCENARIO("Router playback priorities tests")
{
    GIVEN("Router operation instance")
    {
        MockRouterOperation routerOperation([](const sys::Message *e) { return "1"; });
        WHEN("All audio devices are disconnected, loudspeaker is off")
        {
            routerOperation.SendEvent(std::make_shared<Event>(audio::EventType::CallLoudspeakerOff));
            THEN("Earspeaker is prioritized")
            {
                REQUIRE(routerOperation.getPriorityProfile() == Profile::Type::RoutingEarspeaker);
            }
            AND_WHEN("Loudspeaker is on")
            {
                routerOperation.SendEvent(std::make_shared<Event>(audio::EventType::CallLoudspeakerOn));
                routerOperation.enableLoudspeaker();
                THEN("Loudspeaker is prioritized")
                {
                    REQUIRE(routerOperation.getPriorityProfile() == Profile::Type::RoutingLoudspeaker);
                }
            }
        }

        WHEN("All audio devices are connected, loudspeaker is off")
        {
            routerOperation.SendEvent(
                std::make_shared<Event>(audio::EventType::BlutoothHSPDeviceState, Event::DeviceState::Connected));
            routerOperation.SendEvent(
                std::make_shared<Event>(audio::EventType::JackState, Event::DeviceState::Connected));
            routerOperation.SendEvent(std::make_shared<Event>(audio::EventType::CallLoudspeakerOff));

            THEN("Headphones are prioritized")
            {
                REQUIRE(routerOperation.getPriorityProfile() == Profile::Type::RoutingHeadphones);
            }
            AND_WHEN("Loudspeaker is on")
            {
                routerOperation.SendEvent(std::make_shared<Event>(audio::EventType::CallLoudspeakerOn));
                routerOperation.enableLoudspeaker();
                THEN("Loudspeaker is prioritized")
                {
                    REQUIRE(routerOperation.getPriorityProfile() == Profile::Type::RoutingLoudspeaker);
                }
            }

            WHEN("Headphones are disconnected")
            {
                routerOperation.SendEvent(
                    std::make_shared<Event>(audio::EventType::JackState, Event::DeviceState::Disconnected));

                THEN("Bluetooth HSP is prioritized")
                {
                    REQUIRE(routerOperation.getPriorityProfile() == Profile::Type::RoutingBluetoothHSP);
                }
                AND_WHEN("Loudspeaker is on")
                {
                    routerOperation.SendEvent(std::make_shared<Event>(audio::EventType::CallLoudspeakerOn));
                    routerOperation.enableLoudspeaker();
                    THEN("Loudspeaker is prioritized")
                    {
                        REQUIRE(routerOperation.getPriorityProfile() == Profile::Type::RoutingLoudspeaker);
                    }
                }
            }
        }

        WHEN("Only bluetooth HSP is connected, loudspeaker is off")
        {
            routerOperation.SendEvent(
                std::make_shared<Event>(audio::EventType::BlutoothHSPDeviceState, Event::DeviceState::Connected));
            routerOperation.SendEvent(std::make_shared<Event>(audio::EventType::CallLoudspeakerOff));

            THEN("Bluetooth HSP is prioritized")
            {
                REQUIRE(routerOperation.getPriorityProfile() == Profile::Type::RoutingBluetoothHSP);
            }
            AND_WHEN("Loudspeaker is on")
            {
                routerOperation.SendEvent(std::make_shared<Event>(audio::EventType::CallLoudspeakerOn));
                routerOperation.enableLoudspeaker();
                THEN("Loudspeaker is prioritized")
                {
                    REQUIRE(routerOperation.getPriorityProfile() == Profile::Type::RoutingLoudspeaker);
                }
            }
        }

        WHEN("Only headphones are connected, loudspeaker is off")
        {
            routerOperation.SendEvent(
                std::make_shared<Event>(audio::EventType::JackState, Event::DeviceState::Connected));
            routerOperation.SendEvent(std::make_shared<Event>(audio::EventType::CallLoudspeakerOff));

            THEN("Headphones are prioritized")
            {
                REQUIRE(routerOperation.getPriorityProfile() == Profile::Type::RoutingHeadphones);
            }
            AND_WHEN("Loudspeaker is on")
            {
                routerOperation.SendEvent(std::make_shared<Event>(audio::EventType::CallLoudspeakerOn));
                routerOperation.enableLoudspeaker();
                THEN("Loudspeaker is prioritized")
                {
                    REQUIRE(routerOperation.getPriorityProfile() == Profile::Type::RoutingLoudspeaker);
                }
            }
        }
    }
}
