// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ServiceAudio.hpp"
#include "system/Constants.hpp"

#include <audio/AudioMessage.hpp>
#include <service-db/Settings.hpp>
#include <system/messages/SentinelRegistrationMessage.hpp>

namespace
{
    constexpr auto stackSize            = 1024 * 4;
    constexpr auto defaultVolume        = "7";
    constexpr auto defaultSnoozeVolume  = "4";
    constexpr auto defaultBedtimeVolume = "6";
    constexpr audio::Volume maxInVolume = 10;
    constexpr audio::Volume minVolume   = 0;
    constexpr auto profileType          = audio::Profile::Type::PlaybackLoudspeaker;
    namespace initializer
    {
        using namespace audio;
        // clang-format off
        static constexpr std::initializer_list<std::pair<audio::DbPathElement, const char *>> values{
            {DbPathElement{Setting::Volume, PlaybackType::Meditation, Profile::Type::PlaybackLoudspeaker},defaultVolume},
            {DbPathElement{Setting::Volume, PlaybackType::Multimedia, Profile::Type::PlaybackLoudspeaker},defaultVolume},
            {DbPathElement{Setting::Volume, PlaybackType::Alarm, Profile::Type::PlaybackLoudspeaker}, defaultVolume},
            {DbPathElement{Setting::Volume, PlaybackType::Bedtime, Profile::Type::PlaybackLoudspeaker}, defaultBedtimeVolume},
            {DbPathElement{Setting::Volume, PlaybackType::PreWakeUp, Profile::Type::PlaybackLoudspeaker},defaultSnoozeVolume},
            {DbPathElement{Setting::Volume, PlaybackType::Snooze, Profile::Type::PlaybackLoudspeaker}, defaultSnoozeVolume},

            /// Profiles below are not used but unfortunately, must exist in order to satisfy audio module requirements
            {DbPathElement{Setting::Volume, PlaybackType::Meditation, Profile::Type::PlaybackHeadphones},defaultVolume},
            {DbPathElement{Setting::Volume, PlaybackType::Meditation, Profile::Type::PlaybackBluetoothA2DP},defaultVolume},
            {DbPathElement{Setting::Volume, PlaybackType::Multimedia, Profile::Type::PlaybackHeadphones},defaultVolume},
            {DbPathElement{Setting::Volume, PlaybackType::Multimedia, Profile::Type::PlaybackBluetoothA2DP},defaultVolume},
            {DbPathElement{Setting::Volume, PlaybackType::Alarm, Profile::Type::PlaybackHeadphones}, defaultVolume},
            {DbPathElement{Setting::Volume, PlaybackType::Alarm, Profile::Type::PlaybackBluetoothA2DP}, defaultVolume},
            {DbPathElement{Setting::Volume, PlaybackType::Bedtime, Profile::Type::PlaybackHeadphones}, defaultVolume},
            {DbPathElement{Setting::Volume, PlaybackType::Bedtime, Profile::Type::PlaybackBluetoothA2DP},defaultVolume},
            {DbPathElement{Setting::Volume, PlaybackType::PreWakeUp, Profile::Type::PlaybackHeadphones}, defaultVolume},
            {DbPathElement{Setting::Volume, PlaybackType::PreWakeUp, Profile::Type::PlaybackBluetoothA2DP},defaultVolume},
            {DbPathElement{Setting::Volume, PlaybackType::Snooze, Profile::Type::PlaybackHeadphones}, defaultVolume},
            {DbPathElement{Setting::Volume, PlaybackType::Snooze, Profile::Type::PlaybackBluetoothA2DP}, defaultVolume},
        };
        // clang-format on
    } // namespace initializer

    class AudioInternalEOFNotificationMessage : public service::AudioNotificationMessage
    {
      public:
        explicit AudioInternalEOFNotificationMessage(audio::Token token) : AudioNotificationMessage(token)
        {}
    };
} // namespace

namespace service
{
    Audio::Audio()
        : sys::Service(audioServiceName, "", stackSize, sys::ServicePriority::Idle),
          audioMux([this](auto... params) { return this->AudioServicesCallback(params...); }),
          cpuSentinel(std::make_shared<sys::CpuSentinel>(audioServiceName, this)),
          settingsProvider(std::make_unique<settings::Settings>())
    {
        LOG_INFO("%s Initializing", audioServiceName);
        bus.channels.push_back(sys::BusChannel::ServiceAudioNotifications);

        auto sentinelRegistrationMsg = std::make_shared<sys::SentinelRegistrationMessage>(cpuSentinel);
        bus.sendUnicast(std::move(sentinelRegistrationMsg), service::name::system_manager);

        connect(typeid(AudioStartPlaybackRequest), [this](sys::Message *msg) -> sys::MessagePointer {
            auto *msgl = static_cast<AudioStartPlaybackRequest *>(msg);
            return handleStart(audio::Operation::Type::Playback, msgl->fileName.c_str(), msgl->playbackType);
        });

        connect(typeid(AudioInternalEOFNotificationMessage), [this](sys::Message *msg) -> sys::MessagePointer {
            auto *msgl = static_cast<AudioInternalEOFNotificationMessage *>(msg);
            handleEOF(msgl->token);
            return sys::msgHandled();
        });

        connect(typeid(AudioStopRequest), [this](sys::Message *msg) -> sys::MessagePointer {
            auto *msgl = static_cast<AudioStopRequest *>(msg);
            return handleStop(msgl->stopVec, msgl->token);
        });

        connect(typeid(AudioSetVolume), [this](sys::Message *msg) -> sys::MessagePointer {
            auto *msgl = static_cast<AudioSetVolume *>(msg);
            return handleSetVolume(msgl->playbackType, msgl->val);
        });

        connect(typeid(AudioGetVolume), [this](sys::Message *msg) -> sys::MessagePointer {
            auto *msgl = static_cast<AudioGetVolume *>(msg);
            return handleGetVolume(msgl->playbackType);
        });

        connect(typeid(AudioPauseRequest), [this](sys::Message *msg) -> sys::MessagePointer { return handlePause(); });

        connect(typeid(AudioResumeRequest),
                [this](sys::Message *msg) -> sys::MessagePointer { return handleResume(); });
    }
    sys::MessagePointer Audio::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        return sys::msgNotHandled();
    }
    sys::ReturnCodes Audio::InitHandler()
    {
        settingsProvider->init(service::ServiceProxy(weak_from_this()));
        initializeDatabase();

        return sys::ReturnCodes::Success;
    }
    sys::ReturnCodes Audio::DeinitHandler()
    {
        return sys::ReturnCodes::Success;
    }
    void Audio::ProcessCloseReason([[maybe_unused]] sys::CloseReason closeReason)
    {
        if (const auto &activeInputOpt = audioMux.GetActiveInput(); activeInputOpt.has_value()) {
            const auto activeInput = activeInputOpt.value();
            activeInput->audio->Stop();
        }
    }
    auto Audio::handleStart(const audio::Operation::Type opType,
                            const std::string &fileName,
                            const audio::PlaybackType &playbackType) -> std::unique_ptr<AudioResponseMessage>
    {
        auto retCode  = audio::RetCode::Failed;
        auto retToken = audio::Token::MakeBadToken();

        auto AudioStart = [&](auto &input) {
            if (input) {
                for (auto &audioInput : audioMux.GetAllInputs()) {
                    stopInput(&audioInput);
                }
                retToken = audioMux.ResetInput(input);

                try {
                    retCode = (*input)->audio->Start(opType, retToken, fileName.c_str(), playbackType);
                }
                catch (const audio::AudioInitException &audioException) {
                    retCode = audio::RetCode::FailedToAllocateMemory;
                }
            }
        };
        auto input = audioMux.GetPlaybackInput(playbackType);
        AudioStart(input);
        manageCpuSentinel();

        return std::make_unique<AudioStartPlaybackResponse>(retCode, retToken);
    }
    auto Audio::handleStop(const std::vector<audio::PlaybackType> &stopTypes, const audio::Token &token)
        -> std::unique_ptr<AudioResponseMessage>
    {
        std::vector<std::pair<audio::Token, audio::RetCode>> retCodes;

        for (auto &input : audioMux.GetAllInputs()) {
            auto t = input.token;
            if (token.IsValid() && t == token) {
                retCodes.emplace_back(t, stopInput(&input));
            }
            if (token.IsUninitialized()) {
                retCodes.emplace_back(t, stopInput(&input));
            }
        }

        // on failure return first false code
        auto it = std::find_if_not(
            retCodes.begin(), retCodes.end(), [](auto p) { return p.second == audio::RetCode::Success; });
        if (it != retCodes.end()) {
            return std::make_unique<AudioStopResponse>(it->second, it->first);
        }

        return std::make_unique<AudioStopResponse>(audio::RetCode::Success, token);
    }
    auto Audio::stopInput(audio::AudioMux::Input *input, Audio::StopReason stopReason) -> audio::RetCode
    {
        if (input->audio->GetCurrentState() == audio::Audio::State::Idle) {
            return audio::RetCode::Success;
        }
        const auto retCode = input->audio->Stop();
        // Send notification that audio file was stopped
        std::shared_ptr<AudioNotificationMessage> msg;
        if (stopReason == StopReason::Eof) {
            msg = std::make_shared<AudioEOFNotification>(input->token);
        }
        else {
            msg = std::make_shared<AudioStopNotification>(input->token);
        }
        bus.sendMulticast(std::move(msg), sys::BusChannel::ServiceAudioNotifications);
        audioMux.ResetInput(input);
        manageCpuSentinel();
        return retCode;
    }
    constexpr auto Audio::shouldLoop(const std::optional<audio::PlaybackType> &type) const -> bool
    {
        return type == audio::PlaybackType::Alarm;
    }
    auto Audio::isBusy() const -> bool
    {
        const auto &inputs = audioMux.GetAllInputs();
        return std::any_of(inputs.begin(), inputs.end(), [](const auto &input) {
            return input.audio->GetCurrentState() != audio::Audio::State::Idle;
        });
    }
    void Audio::handleEOF(const audio::Token &token)
    {
        if (const auto input = audioMux.GetInput(token); input) {
            if (shouldLoop((*input)->audio->GetCurrentOperationPlaybackType())) {
                (*input)->audio->Start();
                if ((*input)->audio->IsMuted()) {
                    (*input)->audio->Mute();
                }
            }
            else {
                stopInput(*input, StopReason::Eof);
            }
        }
    }
    auto Audio::AudioServicesCallback(const sys::Message *msg) -> std::optional<std::string>
    {
        std::optional<std::string> ret;
        if (const auto *eof = dynamic_cast<const AudioServiceMessage::EndOfFile *>(msg); eof) {
            bus.sendUnicast(std::make_shared<AudioInternalEOFNotificationMessage>(eof->GetToken()), audioServiceName);
        }
        else if (const auto *dbReq = dynamic_cast<const AudioServiceMessage::DbRequest *>(msg); dbReq) {
            auto selectedPlayback = dbReq->playback;
            auto selectedProfile  = dbReq->profile;
            if (const auto result =
                    settingsProvider->getValue(dbPath(dbReq->setting, selectedPlayback, selectedProfile));
                not result.empty()) {
                ret.emplace(result);
            }
        }
        else {
            LOG_DEBUG("Message received but not handled - no effect.");
        }

        return ret;
    }
    auto Audio::handleSetVolume(const audio::PlaybackType &playbackType, const std::string &value)
        -> std::unique_ptr<AudioResponseMessage>
    {
        constexpr auto setting  = audio::Setting::Volume;
        auto retCode            = audio::RetCode::Success;
        const auto clampedValue = std::clamp(utils::getNumericValue<audio::Volume>(value), minVolume, maxInVolume);

        if (const auto activeInput = audioMux.GetActiveInput(); activeInput) {
            if (activeInput) {
                retCode = activeInput.value()->audio->SetOutputVolume(clampedValue);
            }
        }

        if (retCode == audio::RetCode::Success) {
            settingsProvider->setValue(dbPath(setting, playbackType, profileType), std::to_string(clampedValue));
        }
        return std::make_unique<AudioResponseMessage>(retCode);
    }
    auto Audio::handleGetVolume(const audio::PlaybackType &playbackType) -> std::unique_ptr<AudioResponseMessage>
    {
        constexpr auto setting = audio::Setting::Volume;

        const auto path = dbPath(setting, playbackType, profileType);
        if (const auto value = settingsProvider->getValue(path); not value.empty()) {
            return std::make_unique<AudioResponseMessage>(audio::RetCode::Success, value);
        }

        return std::make_unique<AudioResponseMessage>(audio::RetCode::Failed);
    }
    sys::ReturnCodes Audio::SwitchPowerModeHandler([[maybe_unused]] const sys::ServicePowerMode mode)
    {
        return sys::ReturnCodes::Success;
    }
    auto Audio::handlePause() -> std::unique_ptr<AudioResponseMessage>
    {
        auto retCode = audio::RetCode::InvokedInIncorrectState;
        if (const auto activeInput = audioMux.GetActiveInput(); activeInput) {
            auto playbackType = (*activeInput)->audio->GetCurrentOperationPlaybackType();
            if (isResumable(playbackType)) {
                retCode = activeInput.value()->audio->Pause();
            }
            else {
                retCode = audio::RetCode::UnsupportedEvent;
            }
        }
        manageCpuSentinel();
        return std::make_unique<AudioResponseMessage>(retCode);
    }
    auto Audio::handleResume() -> std::unique_ptr<AudioResponseMessage>
    {
        auto retCode = audio::RetCode::InvokedInIncorrectState;
        if (const auto activeInput = audioMux.GetActiveInput();
            activeInput && activeInput.value()->audio->GetCurrentOperationState() == audio::Operation::State::Paused) {
            retCode = activeInput.value()->audio->Resume();
        }
        manageCpuSentinel();
        return std::make_unique<AudioResponseMessage>(retCode);
    }
    constexpr auto Audio::isResumable(audio::PlaybackType type) const -> bool
    {
        return type == audio::PlaybackType::Multimedia;
    }
    void Audio::manageCpuSentinel()
    {
        isBusy() ? cpuSentinel->HoldMinimumFrequency(bsp::CpuFrequencyMHz::Level_6)
                 : cpuSentinel->ReleaseMinimumFrequency();
    }
    void Audio::initializeDatabase()
    {
        for (const auto &entry : initializer::values) {
            const auto path = dbPath(entry.first);
            if (settingsProvider->getValue(path).empty()) {
                settingsProvider->setValue(path, entry.second);
            }
        }
    }
} // namespace service
