// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-audio/AudioMessage.hpp"
#include "service-audio/ServiceAudio.hpp"

#include <Audio/Operation/IdleOperation.hpp>
#include <Audio/Operation/PlaybackOperation.hpp>
#include <Bluetooth/audio/BluetoothAudioDevice.hpp>
#include <module-audio/Audio/VolumeScaler.hpp>
#include <service-bluetooth/Constants.hpp>
#include <service-bluetooth/ServiceBluetoothCommon.hpp>
#include <service-bluetooth/BluetoothMessage.hpp>
#include <service-bluetooth/messages/AudioRouting.hpp>
#include <service-bluetooth/messages/Ring.hpp>
#include <service-db/Settings.hpp>
#include <service-evtmgr/EventManagerServiceAPI.hpp>

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <utility>

using namespace audio;

inline constexpr auto audioServiceStackSize = 1024 * 4;

static constexpr auto defaultVolumeHigh  = "10";
static constexpr auto defaultVolumeLow   = "2";
static constexpr auto defaultVolumeMuted = "0";
static constexpr auto defaultTrue        = "1";
static constexpr auto defaultFalse       = "0";
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

    {DbPathElement{Setting::Volume, PlaybackType::Alarm, Profile::Type::PlaybackHeadphones}, defaultVolumeLow},
    {DbPathElement{Setting::Volume, PlaybackType::Alarm, Profile::Type::PlaybackBluetoothA2DP}, defaultVolumeLow},
    {DbPathElement{Setting::Volume, PlaybackType::Alarm, Profile::Type::PlaybackLoudspeaker}, defaultVolumeHigh},

    // ROUTING
    {DbPathElement{Setting::Gain, PlaybackType::None, Profile::Type::RoutingBluetoothHSP}, "20"},
    {DbPathElement{Setting::Gain, PlaybackType::None, Profile::Type::RoutingEarspeaker}, "3"},
    {DbPathElement{Setting::Gain, PlaybackType::None, Profile::Type::RoutingLoudspeaker}, "10"},
    {DbPathElement{Setting::Gain, PlaybackType::None, Profile::Type::RoutingHeadphones}, "0"},

    {DbPathElement{Setting::Volume, PlaybackType::None, Profile::Type::RoutingBluetoothHSP}, defaultVolumeHigh},
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
      settingsProvider(std::make_unique<settings::Settings>(this))
{
    LOG_INFO("[ServiceAudio] Initializing");
    bus.channels.push_back(sys::BusChannel::ServiceAudioNotifications);

    connect(typeid(BluetoothDeviceVolumeChanged),
            [this](sys::Message *msg) -> sys::MessagePointer { return handleVolumeChangedOnBluetoothDevice(msg); });
}

ServiceAudio::~ServiceAudio()
{
    LOG_INFO("[ServiceAudio] Cleaning resources");
}

sys::ReturnCodes ServiceAudio::InitHandler()
{
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
    return sys::ReturnCodes::Success;
}

void ServiceAudio::ProcessCloseReason(sys::CloseReason closeReason)
{
    sendCloseReadyMessage(this);
}

std::optional<std::string> ServiceAudio::AudioServicesCallback(const sys::Message *msg)
{
    if (const auto *eof = dynamic_cast<const AudioServiceMessage::EndOfFile *>(msg); eof) {
        auto newMsg =
            std::make_shared<AudioNotificationMessage>(AudioNotificationMessage::Type::EndOfFile, eof->GetToken());
        bus.sendMulticast(std::move(newMsg), sys::BusChannel::ServiceAudioNotifications);
    }
    else if (const auto *dbReq = dynamic_cast<const AudioServiceMessage::DbRequest *>(msg); dbReq) {
        std::string path = dbPath(dbReq->setting, dbReq->playback, dbReq->profile);
        LOG_DEBUG("ServiceAudio::DBbCallback(%s)", path.c_str());
        auto settings_it = settingsCache.find(path);
        if (settingsCache.end() == settings_it) {
            LOG_DEBUG("%s does not exist in cache", path.c_str());
            return std::nullopt;
        }
        return settings_it->second;
    }
    else if (const auto *deviceMsg = dynamic_cast<const AudioServiceMessage::AudioDeviceCreated *>(msg); deviceMsg) {
        if (deviceMsg->getDeviceType() == AudioDevice::Type::Bluetooth) {
            auto startBluetoothAudioMsg = std::make_shared<BluetoothAudioStartMessage>(
                std::static_pointer_cast<bluetooth::BluetoothAudioDevice>(deviceMsg->getDevice()));
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
    return type.value_or(audio::PlaybackType::None) == audio::PlaybackType::CallRingtone;
}

bool ServiceAudio::IsVibrationEnabled(const audio::PlaybackType &type)
{
    auto isEnabled =
        utils::getNumericValue<audio::Vibrate>(getSetting(Setting::EnableVibration, Profile::Type::Idle, type));
    return isEnabled;
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
    if (!IsVibrationEnabled(type)) {
        return VibrationType::None;
    }

    if (type == PlaybackType::CallRingtone) {
        return VibrationType::Continuous;
    }
    else if (type == PlaybackType::Notifications || type == PlaybackType::TextMessageRingtone) {
        return VibrationType::OneShot;
    }
    return VibrationType::None;
}

void ServiceAudio::VibrationUpdate(const audio::PlaybackType &type, std::optional<AudioMux::Input *> input)
{
    auto curVibrationType = GetVibrationType(type);
    if (curVibrationType == VibrationType::OneShot && !IsVibrationMotorOn()) {
        EventManagerServiceAPI::vibraPulseOnce(this);
    }
    else if (input && curVibrationType == VibrationType::Continuous) {
        input.value()->EnableVibration();
    }

    auto &inputs       = audioMux.GetAllInputs();
    auto anyOfInputsOn = std::any_of(inputs.cbegin(), inputs.cend(), [](auto &i) {
        return i.GetVibrationStatus() == AudioMux::VibrationStatus::On;
    });
    if (anyOfInputsOn && !IsVibrationMotorOn()) {
        EventManagerServiceAPI::vibraPulseRepeatUntilStop(this);
        vibrationMotorStatus = AudioMux::VibrationStatus::On;
    }
    else if ((!anyOfInputsOn && IsVibrationMotorOn()) || (anyOfInputsOn && IsVibrationMotorOn())) {
        EventManagerServiceAPI::vibraStop(this);
        vibrationMotorStatus = AudioMux::VibrationStatus::Off;
    }
}

std::unique_ptr<AudioResponseMessage> ServiceAudio::HandleGetFileTags(const std::string &fileName)
{
    if (auto tag = Audio::GetFileTags(fileName.c_str())) {
        return std::make_unique<AudioResponseMessage>(RetCode::Success, tag.value());
    }
    return std::make_unique<AudioResponseMessage>(RetCode::FileDoesntExist);
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
        retCode = audioInput->audio->Stop();
        auto broadMsg =
            std::make_shared<AudioNotificationMessage>(AudioNotificationMessage::Type::Stop, audioInput->token);
        bus.sendMulticast(std::move(broadMsg), sys::BusChannel::ServiceAudioNotifications);
        audioMux.ResetInput(audioInput);
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
                HandlePause(&audioInput);
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
        // stop bluetooth stream if available
        if (bluetoothConnected) {
            if (playbackType == audio::PlaybackType::CallRingtone) {
                LOG_DEBUG("Sending Bluetooth start ringing");
                bus.sendUnicast(std::make_shared<message::bluetooth::Ring>(message::bluetooth::Ring::State::Enable),
                                service::name::bluetooth);
            }
            else {
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
        if (bluetoothConnected) {
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
        auto newState = evt->getDeviceState() == Event::DeviceState::Connected;
        if (newState != bluetoothConnected) {
            LOG_DEBUG("Bluetooth connection status changed: %s", newState ? "connected" : "disconnected");
            bluetoothConnected = newState;
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

    auto stopInput = [this](auto inp) {
        if (inp->audio->GetCurrentState() == Audio::State::Idle) {
            return audio::RetCode::Success;
        }
        auto rCode = inp->audio->Stop();
        // Send notification that audio file was stopped
        auto msgStop = std::make_shared<AudioNotificationMessage>(AudioNotificationMessage::Type::Stop, inp->token);
        bus.sendMulticast(msgStop, sys::BusChannel::ServiceAudioNotifications);
        audioMux.ResetInput(inp);
        return rCode;
    };

    // stop by token
    if (auto tokenInput = audioMux.GetInput(token); token.IsValid() && tokenInput) {
        retCodes.emplace_back(std::make_pair(token, stopInput(tokenInput.value())));
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
                retCodes.emplace_back(t, stopInput(&input));
            }
        }
    }
    // stop all audio
    else if (token.IsUninitialized()) {
        for (auto &input : audioMux.GetAllInputs()) {
            auto t = input.token;
            retCodes.emplace_back(t, stopInput(&input));
        }
    }

    // stop bluetooth stream if available
    if (bluetoothConnected) {
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

void ServiceAudio::HandleNotification(const AudioNotificationMessage::Type &type, const Token &token)
{
    if (type == AudioNotificationMessage::Type::EndOfFile) {
        auto input = audioMux.GetInput(token);
        if (input && ShouldLoop((*input)->audio->GetCurrentOperationPlaybackType())) {
            (*input)->audio->Start();
            if ((*input)->audio->IsMuted()) {
                (*input)->audio->Mute();
            }
        }
        else {
            auto newMsg = std::make_shared<AudioStopRequest>(token);
            bus.sendUnicast(newMsg, service::name::audio);
        }
        return;
    }

    if (type != AudioNotificationMessage::Type::Stop) {
        LOG_DEBUG("Unhandled AudioNotificationMessage");
    }
}

auto ServiceAudio::HandleKeyPressed(const int step) -> sys::MessagePointer
{
    auto context = getCurrentContext();

    const auto currentVolume =
        utils::getNumericValue<int>(getSetting(Setting::Volume, Profile::Type::Idle, PlaybackType::None));

    if (isSystemSound(context.second)) {
        // active system sounds can be only muted, no volume control is possible
        if (step < 0) {
            MuteCurrentOperation();
            return sys::msgHandled();
        }
        else {
            return sys::msgHandled();
        }
    }

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
    bus.sendMulticast(std::make_unique<VolumeChanged>(newVolume, context), sys::BusChannel::ServiceAudioNotifications);
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

    if (msgType == typeid(AudioNotificationMessage)) {
        auto *msg = static_cast<AudioNotificationMessage *>(msgl);
        HandleNotification(msg->type, msg->token);
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
    else if (msgType == typeid(AudioGetFileTagsRequest)) {
        auto *msg   = static_cast<AudioGetFileTagsRequest *>(msgl);
        responseMsg = HandleGetFileTags(msg->fileName);
    }
    else if (msgType == typeid(AudioEventRequest)) {
        auto *msg   = static_cast<AudioEventRequest *>(msgl);
        responseMsg = HandleSendEvent(msg->getEvent());
    }
    else if (msgType == typeid(AudioKeyPressedRequest)) {
        auto *msg   = static_cast<AudioKeyPressedRequest *>(msgl);
        responseMsg = HandleKeyPressed(msg->step);
    }

    auto curIsBusy = IsBusy();
    if (isBusy != curIsBusy) {
        auto broadMsg = std::make_shared<AudioNotificationMessage>(
            curIsBusy ? AudioNotificationMessage::Type::ServiceWakeUp : AudioNotificationMessage::Type::ServiceSleep);
        bus.sendMulticast(broadMsg, sys::BusChannel::ServiceAudioNotifications);
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
    case Setting::EnableSound:
    case Setting::Sound:
        targetProfile = Profile::Type::Idle;
        break;
    case Setting::Volume:
        if (targetPlayback == PlaybackType::Alarm) {
            targetProfile = Profile::Type::Idle;
        }
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
    case Setting::EnableSound:
    case Setting::Sound: {
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
    return {currentProfile->GetType(), currentOperation.GetPlaybackType()};
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
auto ServiceAudio::handleVolumeChangedOnBluetoothDevice(sys::Message *msgl) -> sys::MessagePointer
{
    auto *msg                              = static_cast<BluetoothDeviceVolumeChanged *>(msgl);
    const auto volume                      = volume::scaler::toSystemVolume(msg->getVolume());
    const auto [profileType, playbackType] = getCurrentContext();
    settingsProvider->setValue(dbPath(Setting::Volume, playbackType, profileType), std::to_string(volume));
    settingsCache[dbPath(Setting::Volume, playbackType, profileType)] = std::to_string(volume);
    bus.sendMulticast(std::make_unique<VolumeChanged>(volume, std::make_pair(profileType, playbackType)),
                      sys::BusChannel::ServiceAudioNotifications);
    return sys::msgHandled();
}
