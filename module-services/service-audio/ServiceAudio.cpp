// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <AudioMessage.hpp>
#include <ServiceAudio.hpp>

#include <Audio/Operation/IdleOperation.hpp>
#include <Audio/Operation/PlaybackOperation.hpp>
#include <Bluetooth/audio/BluetoothAudioDevice.hpp>
#include <module-audio/Audio/VolumeScaler.hpp>
#include <system/messages/SentinelRegistrationMessage.hpp>
#include <service-bluetooth/BluetoothMessage.hpp>
#include <service-bluetooth/Constants.hpp>
#include <service-bluetooth/messages/AudioRouting.hpp>
#include <service-bluetooth/messages/Ring.hpp>
#include <service-bluetooth/messages/AudioNotify.hpp>
#include <service-bluetooth/ServiceBluetoothCommon.hpp>
#include <service-db/Settings.hpp>
#include <service-evtmgr/EventManagerServiceAPI.hpp>
#include <Utils.hpp>

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <utility>

using namespace audio;

inline constexpr auto audioServiceStackSize = 1024 * 4;

static constexpr auto defaultVolumeHigh              = "10";
static constexpr auto defaultVolumeLow               = "5";
static constexpr auto defaultVolumeMuted             = "0";
static constexpr auto defaultTrue                    = "1";
static constexpr auto defaultFalse                   = "0";
static constexpr auto defaultVibrationLevel          = "5";
static constexpr auto defaultCallRingtonePath        = "assets/audio/ringtone/ringtone_drum_2.mp3";
static constexpr auto defaultTextMessageRingtonePath = "assets/audio/sms/sms_drum_2.mp3";
static constexpr auto defaultNotificationsPath       = "assets/audio/alarm/alarm_hang_drum.mp3";
static constexpr auto defaultKeypadSoundPath         = "assets/audio/sms/sms_drum_2.mp3";

static constexpr std::initializer_list<std::pair<audio::DbPathElement, const char *>> cacheInitializer{

    // PLAYBACK
    {DbPathElement{Setting::Volume, PlaybackType::Multimedia, Profile::Type::PlaybackHeadphones}, defaultVolumeLow},
    {DbPathElement{Setting::Volume, PlaybackType::Multimedia, Profile::Type::PlaybackBluetoothA2DP}, defaultVolumeLow},
    {DbPathElement{Setting::Volume, PlaybackType::Multimedia, Profile::Type::PlaybackLoudspeaker}, defaultVolumeHigh},

    {DbPathElement{Setting::Volume, PlaybackType::System, Profile::Type::PlaybackHeadphones}, defaultVolumeLow},
    {DbPathElement{Setting::Volume, PlaybackType::System, Profile::Type::PlaybackBluetoothA2DP}, defaultVolumeLow},
    {DbPathElement{Setting::Volume, PlaybackType::System, Profile::Type::PlaybackLoudspeaker}, defaultVolumeHigh},

    {DbPathElement{Setting::Volume, PlaybackType::Alarm, Profile::Type::Idle}, defaultVolumeLow},

    {DbPathElement{Setting::IsSystemSound, PlaybackType::Notifications, Profile::Type::Idle}, defaultTrue},
    {DbPathElement{Setting::IsSystemSound, PlaybackType::KeypadSound, Profile::Type::Idle}, defaultTrue},
    {DbPathElement{Setting::IsSystemSound, PlaybackType::CallRingtone, Profile::Type::Idle}, defaultTrue},
    {DbPathElement{Setting::IsSystemSound, PlaybackType::TextMessageRingtone, Profile::Type::Idle}, defaultTrue},
    {DbPathElement{Setting::IsSystemSound, PlaybackType::Meditation, Profile::Type::Idle}, defaultTrue},
    {DbPathElement{Setting::IsSystemSound, PlaybackType::Alarm, Profile::Type::Idle}, defaultTrue},
    {DbPathElement{Setting::IsSystemSound, PlaybackType::None, Profile::Type::Idle}, defaultFalse},
    {DbPathElement{Setting::IsSystemSound, PlaybackType::Multimedia, Profile::Type::Idle}, defaultFalse},

    // ROUTING
    {DbPathElement{Setting::Gain, PlaybackType::None, Profile::Type::RoutingBluetoothHSP}, "20"},
    {DbPathElement{Setting::Gain, PlaybackType::None, Profile::Type::RoutingBluetoothHFP}, "20"},

    {DbPathElement{Setting::Gain, PlaybackType::None, Profile::Type::RoutingEarspeaker}, "3"},
    {DbPathElement{Setting::Gain, PlaybackType::None, Profile::Type::RoutingLoudspeaker}, "10"},
    {DbPathElement{Setting::Gain, PlaybackType::None, Profile::Type::RoutingHeadphones}, "0"},

    {DbPathElement{Setting::Volume, PlaybackType::None, Profile::Type::RoutingBluetoothHSP}, defaultVolumeHigh},
    {DbPathElement{Setting::Volume, PlaybackType::None, Profile::Type::RoutingBluetoothHFP}, defaultVolumeHigh},

    {DbPathElement{Setting::Volume, PlaybackType::None, Profile::Type::RoutingEarspeaker}, defaultVolumeHigh},
    {DbPathElement{Setting::Volume, PlaybackType::None, Profile::Type::RoutingHeadphones}, defaultVolumeHigh},
    {DbPathElement{Setting::Volume, PlaybackType::None, Profile::Type::RoutingLoudspeaker}, defaultVolumeHigh},

    // RECORDING
    {DbPathElement{Setting::Gain, PlaybackType::None, Profile::Type::RecordingBuiltInMic}, "200"},
    {DbPathElement{Setting::Gain, PlaybackType::None, Profile::Type::RecordingHeadphones}, "100"},
    {DbPathElement{Setting::Gain, PlaybackType::None, Profile::Type::RecordingBluetoothHSP}, "100"},

    // MISC
    {DbPathElement{Setting::EnableVibration, PlaybackType::Multimedia, Profile::Type::Idle}, defaultFalse},
    {DbPathElement{Setting::EnableVibration, PlaybackType::Notifications, Profile::Type::Idle}, defaultTrue},
    {DbPathElement{Setting::EnableVibration, PlaybackType::KeypadSound, Profile::Type::Idle}, defaultFalse},
    {DbPathElement{Setting::EnableVibration, PlaybackType::CallRingtone, Profile::Type::Idle}, defaultTrue},
    {DbPathElement{Setting::EnableVibration, PlaybackType::TextMessageRingtone, Profile::Type::Idle}, defaultTrue},
    {DbPathElement{Setting::EnableVibration, PlaybackType::Meditation, Profile::Type::Idle}, defaultFalse},
    {DbPathElement{Setting::EnableVibration, PlaybackType::Alarm, Profile::Type::Idle}, defaultTrue},

    {DbPathElement{Setting::VibrationLevel, PlaybackType::System, Profile::Type::Idle}, defaultVibrationLevel},

    {DbPathElement{Setting::EnableSound, PlaybackType::Multimedia, Profile::Type::Idle}, defaultTrue},
    {DbPathElement{Setting::EnableSound, PlaybackType::Notifications, Profile::Type::Idle}, defaultTrue},
    {DbPathElement{Setting::EnableSound, PlaybackType::KeypadSound, Profile::Type::Idle}, defaultTrue},
    {DbPathElement{Setting::EnableSound, PlaybackType::CallRingtone, Profile::Type::Idle}, defaultTrue},
    {DbPathElement{Setting::EnableSound, PlaybackType::TextMessageRingtone, Profile::Type::Idle}, defaultTrue},
    {DbPathElement{Setting::EnableSound, PlaybackType::Meditation, Profile::Type::Idle}, defaultTrue},
    {DbPathElement{Setting::EnableSound, PlaybackType::Alarm, Profile::Type::Idle}, defaultTrue},

    {DbPathElement{Setting::Sound, PlaybackType::Notifications, Profile::Type::Idle}, defaultNotificationsPath},
    {DbPathElement{Setting::Sound, PlaybackType::KeypadSound, Profile::Type::Idle}, defaultKeypadSoundPath},
    {DbPathElement{Setting::Sound, PlaybackType::CallRingtone, Profile::Type::Idle}, defaultCallRingtonePath},
    {DbPathElement{Setting::Sound, PlaybackType::TextMessageRingtone, Profile::Type::Idle},
     defaultTextMessageRingtonePath},
};

ServiceAudio::ServiceAudio()
    : sys::Service(service::name::audio, "", audioServiceStackSize, sys::ServicePriority::Idle),
      audioMux([this](auto... params) { return this->AudioServicesCallback(params...); }),
      cpuSentinel(std::make_shared<sys::CpuSentinel>(service::name::audio, this)),
      settingsProvider(std::make_unique<settings::Settings>())
{
    LOG_INFO("[ServiceAudio] Initializing");
    bus.channels.push_back(sys::BusChannel::ServiceAudioNotifications);

    auto sentinelRegistrationMsg = std::make_shared<sys::SentinelRegistrationMessage>(cpuSentinel);
    bus.sendUnicast(std::move(sentinelRegistrationMsg), ::service::name::system_manager);

    connect(typeid(A2DPDeviceVolumeChanged),
            [this](sys::Message *msg) -> sys::MessagePointer { return handleA2DPVolumeChangedOnBluetoothDevice(msg); });
    connect(typeid(HSPDeviceVolumeChanged),
            [this](sys::Message *msg) -> sys::MessagePointer { return handleHSPVolumeChangedOnBluetoothDevice(msg); });
    connect(typeid(HFPDeviceVolumeChanged),
            [this](sys::Message *msg) -> sys::MessagePointer { return handleHFPVolumeChangedOnBluetoothDevice(msg); });
    connect(typeid(message::bluetooth::AudioPause),
            [this](sys::Message *msg) -> sys::MessagePointer { return handleA2DPAudioPause(); });
    connect(typeid(message::bluetooth::AudioStart),
            [this](sys::Message *msg) -> sys::MessagePointer { return handleA2DPAudioStart(); });
}

ServiceAudio::~ServiceAudio()
{
    LOG_INFO("[ServiceAudio] Cleaning resources");
}

sys::ReturnCodes ServiceAudio::InitHandler()
{
    settingsProvider->init(service::ServiceProxy(shared_from_this()));
    std::transform(std::begin(cacheInitializer),
                   std::end(cacheInitializer),
                   std::inserter(settingsCache, std::end(settingsCache)),
                   [](auto &el) { return std::make_pair(dbPath(el.first), el.second); });

    for (const auto &setting : settingsCache) {
        settingsProvider->registerValueChange(
            setting.first, [this](const std::string &name, std::string value) { settingsChanged(name, value); });
    }

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceAudio::DeinitHandler()
{
    settingsProvider->deinit();
    return sys::ReturnCodes::Success;
}

void ServiceAudio::ProcessCloseReason(sys::CloseReason closeReason)
{
    if (const auto &activeInputOpt = audioMux.GetActiveInput(); activeInputOpt.has_value()) {
        const auto activeInput = activeInputOpt.value();
        activeInput->audio->Stop();
    }
    sendCloseReadyMessage(this);
}

std::optional<std::string> ServiceAudio::AudioServicesCallback(const sys::Message *msg)
{
    if (const auto *eof = dynamic_cast<const AudioServiceMessage::EndOfFile *>(msg); eof) {
        bus.sendUnicast(std::make_shared<AudioInternalEOFNotificationMessage>(eof->GetToken()), service::name::audio);
    }
    else if (const auto *dbReq = dynamic_cast<const AudioServiceMessage::DbRequest *>(msg); dbReq) {

        auto selectedPlayback = generatePlayback(dbReq->playback, dbReq->setting);
        auto selectedProfile  = generateProfile(dbReq->profile, dbReq->playback);

        std::string path = dbPath(dbReq->setting, selectedPlayback, selectedProfile);
        LOG_DEBUG("ServiceAudio::DBbCallback(%s)", path.c_str());
        auto settings_it = settingsCache.find(path);

        if (settingsCache.end() == settings_it) {
            LOG_DEBUG("%s does not exist in cache", path.c_str());
            return std::nullopt;
        }
        return settings_it->second;
    }
    else if (const auto *deviceMsg = dynamic_cast<const AudioServiceMessage::AudioDeviceCreated *>(msg); deviceMsg) {
        if (deviceMsg->getDeviceType() == AudioDevice::Type::BluetoothA2DP) {
            auto startBluetoothAudioMsg = std::make_shared<BluetoothAudioStartMessage>(
                std::static_pointer_cast<bluetooth::A2DPAudioDevice>(deviceMsg->getDevice()));
            bus.sendUnicast(std::move(startBluetoothAudioMsg), service::name::bluetooth);
        }
        else if (deviceMsg->getDeviceType() == AudioDevice::Type::BluetoothHSP) {
            auto startBluetoothAudioMsg = std::make_shared<BluetoothAudioStartMessage>(
                std::static_pointer_cast<bluetooth::CVSDAudioDevice>(deviceMsg->getDevice()));
            bus.sendUnicast(std::move(startBluetoothAudioMsg), service::name::bluetooth);
        }
        else if (deviceMsg->getDeviceType() == AudioDevice::Type::BluetoothHFP) {
            auto startBluetoothAudioMsg = std::make_shared<BluetoothAudioStartMessage>(
                std::static_pointer_cast<bluetooth::CVSDAudioDevice>(deviceMsg->getDevice()));
            bus.sendUnicast(std::move(startBluetoothAudioMsg), service::name::bluetooth);
        }
    }
    else {
        LOG_DEBUG("Message received but not handled - no effect.");
    }

    return std::nullopt;
};

sys::ReturnCodes ServiceAudio::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
{
    LOG_FATAL("[ServiceAudio] PowerModeHandler: %s", c_str(mode));
    return sys::ReturnCodes::Success;
}

constexpr bool ServiceAudio::IsResumable(const audio::PlaybackType &type) const
{
    return type == audio::PlaybackType::Multimedia;
}

constexpr bool ServiceAudio::ShouldLoop(const std::optional<audio::PlaybackType> &type) const
{
    return type.value_or(audio::PlaybackType::None) == audio::PlaybackType::CallRingtone ||
           type.value_or(audio::PlaybackType::None) == audio::PlaybackType::Alarm
#if ENABLE_PLAYBACK_AUTO_REPEAT == 1
           || type.value_or(audio::PlaybackType::None) == audio::PlaybackType::Multimedia
#endif
        ;
}

bool ServiceAudio::IsVibrationEnabled(const audio::PlaybackType &type)
{
    auto isEnabled =
        utils::getNumericValue<audio::Vibrate>(getSetting(Setting::EnableVibration, Profile::Type::Idle, type));
    return isEnabled;
}

bool ServiceAudio::IsSystemSound(const audio::PlaybackType &type)
{
    auto isSystemSound =
        utils::getNumericValue<audio::IsSystemSound>(getSetting(Setting::IsSystemSound, Profile::Type::Idle, type));
    return isSystemSound;
}

audio::PlaybackType ServiceAudio::generatePlayback(const audio::PlaybackType &type, const Setting &setting)
{
    if (setting == Setting::Volume && IsSystemSound(type)) {
        return PlaybackType::System;
    }
    return type;
}

Profile::Type ServiceAudio::generateProfile(const audio::Profile::Type &profile, const PlaybackType &playbackType)
{
    if (playbackType == audio::PlaybackType::Alarm && !IsSystemSound(playbackType)) {
        return audio::Profile::Type::Idle;
    }

    return profile;
}

bool ServiceAudio::IsOperationEnabled(const audio::PlaybackType &plType, const Operation::Type &opType)
{
    if (opType == Operation::Type::Router || opType == Operation::Type::Recorder) {
        return true;
    }
    auto isEnabled =
        utils::getNumericValue<audio::EnableSound>(getSetting(Setting::EnableSound, Profile::Type::Idle, plType));
    return isEnabled;
}

std::string ServiceAudio::GetSound(const audio::PlaybackType &plType)
{
    return getSetting(Setting::Sound, Profile::Type::Idle, plType);
}

ServiceAudio::VibrationType ServiceAudio::GetVibrationType(const audio::PlaybackType &type)
{
    auto isOnVibrationMap = [&](auto vibrationType, auto playbackType) {
        auto continuousVibrations = vibrationMap[vibrationType];

        return std::any_of(continuousVibrations.begin(), continuousVibrations.end(), [type](const auto &playBackType) {
            return playBackType == type;
        });
    };

    if (!IsVibrationEnabled(type)) {
        return VibrationType::None;
    }

    if (isOnVibrationMap(VibrationType::Continuous, type)) {
        return VibrationType::Continuous;
    }
    else if (isOnVibrationMap(VibrationType::OneShot, type)) {
        return VibrationType::OneShot;
    }
    return VibrationType::None;
}

void ServiceAudio::VibrationUpdate(const audio::PlaybackType &type, std::optional<AudioMux::Input *> input)
{
    auto vibrationLevel = utils::getNumericValue<audio::VibrationLevel>(
        getSetting(Setting::VibrationLevel, Profile::Type::Idle, PlaybackType::System));
    EventManagerServiceAPI::setVibrationLevel(this, vibrationLevel);

    switch (const auto curVibrationType = GetVibrationType(type); curVibrationType) {
    case VibrationType::None:
        DisableVibration(input);
        break;
    case VibrationType::OneShot:
        EnableOneShotVibration();
        break;
    case VibrationType::Continuous:
        EnableContinuousVibration(input);
        break;
    }
}

void ServiceAudio::DisableVibration(std::optional<audio::AudioMux::Input *> input)
{
    if (IsVibrationMotorOn()) {
        EventManagerServiceAPI::vibrationStop(this);
        vibrationMotorStatus = AudioMux::VibrationStatus::Off;
    }
    if (input) {
        input.value()->DisableVibration();
    }
}

void ServiceAudio::EnableOneShotVibration()
{
    if (!IsVibrationMotorOn()) {
        EventManagerServiceAPI::vibrationPulseOnce(this);
    }
}

void ServiceAudio::EnableContinuousVibration(std::optional<audio::AudioMux::Input *> input)
{
    if (input) {
        input.value()->EnableVibration();
    }

    auto &inputs       = audioMux.GetAllInputs();
    auto anyOfInputsOn = std::any_of(inputs.cbegin(), inputs.cend(), [](auto &i) {
        return i.GetVibrationStatus() == AudioMux::VibrationStatus::On;
    });
    if (anyOfInputsOn && !IsVibrationMotorOn()) {
        EventManagerServiceAPI::vibrationPulseRepeatUntilStop(this);
        vibrationMotorStatus = AudioMux::VibrationStatus::On;
    }
}

std::unique_ptr<AudioResponseMessage> ServiceAudio::HandlePause(const Token &token)
{
    auto input = audioMux.GetInput(token);
    return HandlePause(input);
}

std::unique_ptr<AudioResponseMessage> ServiceAudio::HandlePause(std::optional<AudioMux::Input *> input)
{
    auto retCode  = audio::RetCode::Failed;
    auto retToken = Token();

    if (!input) {
        return std::make_unique<AudioPauseResponse>(RetCode::TokenNotFound, Token::MakeBadToken());
    }

    auto &audioInput = input.value();

    auto playbackType = audioInput->audio->GetCurrentOperationPlaybackType();
    if (IsResumable(playbackType)) {
        retCode  = audioInput->audio->Pause();
        retToken = audioInput->token;
        audioInput->DisableVibration();
    }
    else {
        retCode = StopInput(audioInput);
    }

    VibrationUpdate();
    return std::make_unique<AudioPauseResponse>(retCode, retToken);
}

std::unique_ptr<AudioResponseMessage> ServiceAudio::HandleResume(const Token &token)
{
    if (auto input = audioMux.GetInput(token)) {
        VibrationUpdate(input.value()->audio->GetCurrentOperationPlaybackType(), input);
        return std::make_unique<AudioResumeResponse>((*input)->audio->Resume(), token);
    }
    return std::make_unique<AudioResumeResponse>(RetCode::TokenNotFound, Token::MakeBadToken());
}

std::unique_ptr<AudioResponseMessage> ServiceAudio::HandleStart(const Operation::Type opType,
                                                                const std::string fileName,
                                                                const audio::PlaybackType &playbackType)
{
    auto retCode  = audio::RetCode::Failed;
    auto retToken = Token::MakeBadToken();

    auto AudioStart = [&](auto &input) {
        if (input) {
            for (auto &audioInput : audioMux.GetAllInputs()) {
                StopInput(&audioInput);
            }
            retToken = audioMux.ResetInput(input);

            if (IsOperationEnabled(playbackType, opType)) {
                try {
                    retCode = (*input)->audio->Start(opType, retToken, fileName.c_str(), playbackType);
                }
                catch (const AudioInitException &audioException) {
                    retCode = audio::RetCode::FailedToAllocateMemory;
                }
            }
        }

        VibrationUpdate(playbackType, input);
    };

    if (opType == Operation::Type::Playback) {
        auto input = audioMux.GetPlaybackInput(playbackType);
        if (playbackType == audio::PlaybackType::CallRingtone && bluetoothVoiceProfileConnected && input &&
            (*input)->audio->GetPriorityPlaybackProfile() == Profile::Type::PlaybackBluetoothA2DP) {
            LOG_DEBUG("Sending Bluetooth start ringing");
            bus.sendUnicast(std::make_shared<message::bluetooth::Ring>(message::bluetooth::Ring::State::Enable),
                            service::name::bluetooth);
            return std::make_unique<AudioStartPlaybackResponse>(audio::RetCode::Success, retToken);
        }
        else if (bluetoothA2DPConnected) {
            if (playbackType != audio::PlaybackType::CallRingtone) {
                LOG_DEBUG("Sending Bluetooth start stream request");
                bus.sendUnicast(std::make_shared<BluetoothMessage>(BluetoothMessage::Request::Play),
                                service::name::bluetooth);
            }
        }

        AudioStart(input);
        return std::make_unique<AudioStartPlaybackResponse>(retCode, retToken);
    }
    else if (opType == Operation::Type::Recorder) {
        auto input = audioMux.GetIdleInput();
        AudioStart(input);
        return std::make_unique<AudioStartRecorderResponse>(retCode, retToken);
    }
    else if (opType == Operation::Type::Router) {
        auto input = audioMux.GetRoutingInput(true);
        if (bluetoothVoiceProfileConnected) {
            LOG_DEBUG("Sending Bluetooth start routing");
            bus.sendUnicast(std::make_shared<message::bluetooth::StartAudioRouting>(), service::name::bluetooth);
        }
        AudioStart(input);
        return std::make_unique<AudioStartRoutingResponse>(retCode, retToken);
    }
    return std::make_unique<AudioStartRoutingResponse>(RetCode::OperationNotSet, Token::MakeBadToken());
}

std::unique_ptr<AudioResponseMessage> ServiceAudio::HandleSendEvent(std::shared_ptr<Event> evt)
{
    // update bluetooth state
    if (evt->getType() == EventType::BlutoothA2DPDeviceState) {
        const auto newState = evt->getDeviceState() == Event::DeviceState::Connected;
        if (newState != bluetoothA2DPConnected) {
            LOG_DEBUG("Bluetooth connection status changed: %s", newState ? "connected" : "disconnected");
            bluetoothA2DPConnected = newState;
            const auto playbacksToBeStopped{
                (bluetoothA2DPConnected) ? std::vector<audio::PlaybackType>{audio::PlaybackType::Alarm,
                                                                            audio::PlaybackType::Meditation,
                                                                            audio::PlaybackType::Notifications,
                                                                            audio::PlaybackType::TextMessageRingtone}
                                         : std::vector<audio::PlaybackType>{audio::PlaybackType::Alarm,
                                                                            audio::PlaybackType::Meditation,
                                                                            audio::PlaybackType::Multimedia,
                                                                            audio::PlaybackType::Notifications,
                                                                            audio::PlaybackType::TextMessageRingtone}};
            HandleStop(playbacksToBeStopped, audio::Token());
        }
    }
    else if (evt->getType() == EventType::BlutoothHSPDeviceState ||
             evt->getType() == EventType::BlutoothHFPDeviceState) {
        auto newState = evt->getDeviceState() == Event::DeviceState::Connected;
        if (newState != bluetoothVoiceProfileConnected) {
            LOG_DEBUG("Bluetooth connection status changed: %s", newState ? "connected" : "disconnected");
            bluetoothVoiceProfileConnected = newState;
        }
    }

    // update information about endpoints availability
    for (auto &input : audioMux.GetAllInputs()) {
        input.audio->SendEvent(evt);
    }

    return std::make_unique<AudioEventResponse>(RetCode::Success);
}

std::unique_ptr<AudioResponseMessage> ServiceAudio::HandleStop(const std::vector<audio::PlaybackType> &stopTypes,
                                                               const Token &token)
{
    std::vector<std::pair<Token, audio::RetCode>> retCodes;

    // stop by token
    if (auto tokenInput = audioMux.GetInput(token); token.IsValid() && tokenInput) {
        retCodes.emplace_back(std::make_pair(token, StopInput(tokenInput.value())));
    }
    else if (token.IsValid()) {
        return std::make_unique<AudioStopResponse>(RetCode::TokenNotFound, Token::MakeBadToken());
    }
    // stop with vector of playback types
    else if (!stopTypes.empty()) {
        for (auto &input : audioMux.GetAllInputs()) {
            const auto &currentOperation = input.audio->GetCurrentOperation();
            if (std::find(stopTypes.begin(), stopTypes.end(), currentOperation.GetPlaybackType()) != stopTypes.end()) {
                auto t = input.token;
                retCodes.emplace_back(t, StopInput(&input));
            }
        }
    }
    // stop all audio
    else if (token.IsUninitialized()) {
        for (auto &input : audioMux.GetAllInputs()) {
            auto t = input.token;
            retCodes.emplace_back(t, StopInput(&input));
        }
    }

    // stop bluetooth stream if available
    if (bluetoothA2DPConnected || bluetoothVoiceProfileConnected) {
        LOG_DEBUG("Sending Bluetooth stop request");
        bus.sendUnicast(std::make_shared<BluetoothMessage>(BluetoothMessage::Request::Stop), service::name::bluetooth);
    }

    // on failure return first false code
    auto it =
        std::find_if_not(retCodes.begin(), retCodes.end(), [](auto p) { return p.second == audio::RetCode::Success; });
    if (it != retCodes.end()) {
        return std::make_unique<AudioStopResponse>(it->second, it->first);
    }

    VibrationUpdate();
    return std::make_unique<AudioStopResponse>(audio::RetCode::Success, token);
}

auto ServiceAudio::StopInput(audio::AudioMux::Input *input, StopReason stopReason) -> audio::RetCode
{
    if (input->audio->GetCurrentState() == Audio::State::Idle) {
        return audio::RetCode::Success;
    }
    const auto rCode = input->audio->Stop();
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
    VibrationUpdate();
    return rCode;
}

void ServiceAudio::HandleEOF(const Token &token)
{
    if (const auto input = audioMux.GetInput(token); input) {
        if (ShouldLoop((*input)->audio->GetCurrentOperationPlaybackType())) {
            (*input)->audio->Start();
            if ((*input)->audio->IsMuted()) {
                (*input)->audio->Mute();
            }
        }
        else {
            StopInput(*input, StopReason::Eof);
        }
    }
}

auto ServiceAudio::HandleKeyPressed(const int step) -> sys::MessagePointer
{
    auto context = getCurrentContext();

    if (IsSystemSound(context.second) || context.second == audio::PlaybackType::Alarm) {
        // active system sounds can be only muted, no volume control is possible
        if (step < 0) {
            MuteCurrentOperation();
            return sys::msgHandled();
        }
        else {
            return sys::msgHandled();
        }
    }

    const auto currentVolume =
        utils::getNumericValue<int>(getSetting(Setting::Volume, Profile::Type::Idle, PlaybackType::None));

    const auto newVolume = std::clamp(currentVolume + step, static_cast<int>(minVolume), static_cast<int>(maxVolume));
    if (auto input = audioMux.GetIdleInput(); input) {
        // when no active input change volume of system sounds
        auto updatedProfile = (*input)->audio->GetPriorityPlaybackProfile();
        setSetting(Setting::Volume, std::to_string(newVolume), updatedProfile, PlaybackType::System);
        context.second = PlaybackType::CallRingtone;
    }
    else {
        // update volume of currently active sound
        setSetting(Setting::Volume, std::to_string(newVolume));
    }
    bus.sendMulticast(std::make_shared<VolumeChanged>(newVolume, context), sys::BusChannel::ServiceAudioNotifications);
    return sys::msgHandled();
}

void ServiceAudio::MuteCurrentOperation()
{
    for (auto &input : audioMux.GetAllInputs()) {
        input.audio->Mute();
    }
}

bool ServiceAudio::IsBusy()
{
    for (auto &input : audioMux.GetAllInputs()) {
        if (input.audio->GetCurrentState() != Audio::State::Idle) {
            return true;
        }
    }
    return false;
}

sys::MessagePointer ServiceAudio::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
{
    sys::MessagePointer responseMsg;
    const auto isBusy = IsBusy();
    auto &msgType     = typeid(*msgl);

    if (msgType == typeid(AudioInternalEOFNotificationMessage)) {
        auto *msg = static_cast<AudioInternalEOFNotificationMessage *>(msgl);
        HandleEOF(msg->token);
    }
    else if (msgType == typeid(AudioGetSetting)) {
        auto *msg   = static_cast<AudioGetSetting *>(msgl);
        auto value  = getSetting(msg->setting, Profile::Type::Idle, msg->playbackType);
        responseMsg = std::make_shared<AudioResponseMessage>(RetCode::Success, value);
    }
    else if (msgType == typeid(AudioSetSetting)) {
        auto *msg = static_cast<AudioSetSetting *>(msgl);
        setSetting(msg->setting, msg->val, Profile::Type::Idle, msg->playbackType);
        responseMsg = std::make_shared<AudioResponseMessage>(RetCode::Success);
    }
    else if (msgType == typeid(AudioStopRequest)) {
        auto *msg   = static_cast<AudioStopRequest *>(msgl);
        responseMsg = HandleStop(msg->stopVec, msg->token);
    }
    else if (msgType == typeid(AudioStartPlaybackRequest)) {
        auto *msg   = static_cast<AudioStartPlaybackRequest *>(msgl);
        responseMsg = HandleStart(Operation::Type::Playback, msg->fileName.c_str(), msg->playbackType);
    }
    else if (msgType == typeid(AudioStartRecorderRequest)) {
        auto *msg   = static_cast<AudioStartRecorderRequest *>(msgl);
        responseMsg = HandleStart(Operation::Type::Recorder, msg->fileName.c_str());
    }
    else if (msgType == typeid(AudioStartRoutingRequest)) {
        LOG_DEBUG("AudioRoutingStart");
        responseMsg = HandleStart(Operation::Type::Router);
    }
    else if (msgType == typeid(AudioPauseRequest)) {
        auto *msg   = static_cast<AudioPauseRequest *>(msgl);
        responseMsg = HandlePause(msg->token);
    }
    else if (msgType == typeid(AudioResumeRequest)) {
        auto *msg   = static_cast<AudioResumeRequest *>(msgl);
        responseMsg = HandleResume(msg->token);
    }
    else if (msgType == typeid(AudioEventRequest)) {
        auto *msg   = static_cast<AudioEventRequest *>(msgl);
        responseMsg = HandleSendEvent(msg->getEvent());
    }
    else if (msgType == typeid(AudioKeyPressedRequest)) {
        auto *msg   = static_cast<AudioKeyPressedRequest *>(msgl);
        responseMsg = HandleKeyPressed(msg->step);
    }

    if (const auto curIsBusy = IsBusy(); isBusy != curIsBusy) {
        curIsBusy ? cpuSentinel->HoldMinimumFrequency(bsp::CpuFrequencyMHz::Level_6)
                  : cpuSentinel->ReleaseMinimumFrequency();
    }

    if (responseMsg) {
        return responseMsg;
    }
    else {
        return std::make_shared<AudioResponseMessage>(RetCode::Failed);
    }
}

std::string ServiceAudio::getSetting(const Setting &setting,
                                     const Profile::Type &profileType,
                                     const PlaybackType &playbackType)
{
    const std::string defaultValue = {};
    auto targetProfile             = profileType;
    auto targetPlayback            = playbackType;

    if (profileType == Profile::Type::Idle && playbackType == PlaybackType::None) {
        if (const auto activeInput = audioMux.GetActiveInput(); activeInput.has_value()) {
            const auto &currentOperation    = (*activeInput)->audio->GetCurrentOperation();
            const auto &currentProfile      = currentOperation.GetProfile()->GetType();
            const auto &currentPlaybackType = (*activeInput)->audio->GetCurrentOperationPlaybackType();

            targetProfile  = currentProfile;
            targetPlayback = currentPlaybackType;
        }
        else if (auto input = audioMux.GetIdleInput(); input && (setting == Setting::Volume)) {
            targetProfile = (*input)->audio->GetPriorityPlaybackProfile();

            targetPlayback = PlaybackType::CallRingtone;
        }
        else {
            return defaultValue;
        }
    }

    switch (setting) {
    case Setting::EnableVibration:
    case Setting::VibrationLevel:
    case Setting::EnableSound:
    case Setting::Sound:
    case Setting::IsSystemSound:
        targetProfile = Profile::Type::Idle;
        break;
    case Setting::Volume:
        if (targetPlayback == PlaybackType::Alarm) {
            targetProfile = Profile::Type::Idle;
        }

        targetPlayback = generatePlayback(targetPlayback);
        break;
    case Setting::Gain:
        break;
    }

    const auto path = dbPath(setting, targetPlayback, targetProfile);
    if (const auto set_it = settingsCache.find(path); settingsCache.end() != set_it) {
        LOG_INFO("Get audio setting %s = %s", path.c_str(), set_it->second.c_str());
        return set_it->second;
    }

    LOG_ERROR("ServiceAudio::getSetting setting name %s does not exist", path.c_str());
    return std::string{};
}

void ServiceAudio::setSetting(const Setting &setting,
                              const std::string &value,
                              const Profile::Type &profileType,
                              const PlaybackType &playbackType)
{
    std::string valueToSet, path;
    auto retCode         = audio::RetCode::Success;
    auto updatedProfile  = profileType;
    auto updatedPlayback = playbackType;

    std::optional<AudioMux::Input *> activeInput;

    // request changing currently active audio playback
    if (profileType == Profile::Type::Idle && playbackType == PlaybackType::None) {
        if (activeInput = audioMux.GetActiveInput(); activeInput.has_value()) {
            const auto &currentOperation = (*activeInput)->audio->GetCurrentOperation();
            updatedProfile               = currentOperation.GetProfile()->GetType();
            updatedPlayback              = (*activeInput)->audio->GetCurrentOperationPlaybackType();
        }
        else {
            LOG_DEBUG("%s has not been set - no active playback.", utils::enumToString(setting).c_str());
            return;
        }
    }

    switch (setting) {
    case Setting::Volume: {
        const auto clampedValue = std::clamp(utils::getNumericValue<audio::Volume>(value), minVolume, maxVolume);
        valueToSet              = std::to_string(clampedValue);
        if (activeInput) {
            retCode = activeInput.value()->audio->SetOutputVolume(clampedValue);
            break;
        }
        if (updatedPlayback == PlaybackType::Alarm) {
            updatedProfile = Profile::Type::Idle;
        }
        updatedPlayback = generatePlayback(updatedPlayback);
        break;
    } break;
    case Setting::Gain: {
        const auto clampedValue = std::clamp(utils::getNumericValue<audio::Gain>(value), minGain, maxGain);
        valueToSet              = std::to_string(clampedValue);
        if (activeInput) {
            retCode = activeInput.value()->audio->SetInputGain(clampedValue);
        }
    } break;
    case Setting::EnableVibration:
    case Setting::VibrationLevel:
    case Setting::EnableSound:
    case Setting::Sound:
    case Setting::IsSystemSound: {
        updatedProfile = audio::Profile::Type::Idle;
        valueToSet     = value;
    } break;
    }

    if (retCode == RetCode::Success) {
        settingsProvider->setValue(dbPath(setting, updatedPlayback, updatedProfile), valueToSet);
        settingsCache[dbPath(setting, updatedPlayback, updatedProfile)] = valueToSet;
    }
}

const audio::Context ServiceAudio::getCurrentContext()
{
    const auto activeInput = audioMux.GetActiveInput();
    if (!activeInput.has_value()) {
        const auto idleInput = audioMux.GetIdleInput();
        return {(*idleInput)->audio->GetPriorityPlaybackProfile(), audio::PlaybackType::None};
    }
    auto &audio                  = (*activeInput)->audio;
    const auto &currentOperation = audio->GetCurrentOperation();
    const auto currentProfile    = currentOperation.GetProfile();
    const auto playbackType      = generatePlayback(currentOperation.GetPlaybackType());
    return {currentProfile->GetType(), playbackType};
}

void ServiceAudio::settingsChanged(const std::string &name, std::string value)
{
    if (value.empty()) {
        return;
    }
    if (auto s_it = settingsCache.find(name); settingsCache.end() != s_it) {
        s_it->second = value;
        return;
    }
    LOG_ERROR("ServiceAudio::settingsChanged received notification about not registered setting: %s", name.c_str());
}

void ServiceAudio::onVolumeChanged(Volume volume)
{
    const auto [profileType, playbackType] = getCurrentContext();
    settingsProvider->setValue(dbPath(Setting::Volume, playbackType, profileType), std::to_string(volume));
    settingsCache[dbPath(Setting::Volume, playbackType, profileType)] = std::to_string(volume);
    bus.sendMulticast(std::make_shared<VolumeChanged>(volume, std::make_pair(profileType, playbackType)),
                      sys::BusChannel::ServiceAudioNotifications);
}

auto ServiceAudio::handleA2DPVolumeChangedOnBluetoothDevice(sys::Message *msgl) -> sys::MessagePointer
{
    auto *a2dpMsg = dynamic_cast<A2DPDeviceVolumeChanged *>(msgl);
    assert(a2dpMsg != nullptr);
    const auto volume = volume::scaler::a2dp::toSystemVolume(a2dpMsg->getVolume());
    onVolumeChanged(volume);
    return sys::msgHandled();
}

auto ServiceAudio::handleHSPVolumeChangedOnBluetoothDevice(sys::Message *msgl) -> sys::MessagePointer
{
    auto *hspMsg = dynamic_cast<HSPDeviceVolumeChanged *>(msgl);
    assert(hspMsg != nullptr);
    const auto volume = volume::scaler::hsp::toSystemVolume(hspMsg->getVolume());
    onVolumeChanged(volume);
    return sys::msgHandled();
}

auto ServiceAudio::handleHFPVolumeChangedOnBluetoothDevice(sys::Message *msgl) -> sys::MessagePointer
{
    auto *hfpMsg = dynamic_cast<HFPDeviceVolumeChanged *>(msgl);
    assert(hfpMsg != nullptr);
    const auto volume = volume::scaler::hfp::toSystemVolume(hfpMsg->getVolume());
    onVolumeChanged(volume);
    return sys::msgHandled();
}
auto ServiceAudio::handleA2DPAudioPause() -> sys::MessagePointer
{
    if (auto input = audioMux.GetPlaybackInput(audio::PlaybackType::Multimedia);
        input && (*input)->audio->Pause() == RetCode::Success) {
        bus.sendMulticast(std::make_shared<AudioPausedNotification>((*input)->token),
                          sys::BusChannel::ServiceAudioNotifications);
    }
    return sys::msgHandled();
}

auto ServiceAudio::handleA2DPAudioStart() -> sys::MessagePointer
{
    if (auto input = audioMux.GetPlaybackInput(audio::PlaybackType::Multimedia);
        input && (*input)->audio->Resume() == RetCode::Success) {
        bus.sendMulticast(std::make_shared<AudioResumedNotification>((*input)->token),
                          sys::BusChannel::ServiceAudioNotifications);
    }
    return sys::msgHandled();
}
