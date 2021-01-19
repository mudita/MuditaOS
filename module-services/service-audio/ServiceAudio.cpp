// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-audio/AudioMessage.hpp"
#include "service-audio/ServiceAudio.hpp"

#include <Audio/Operation/IdleOperation.hpp>
#include <Audio/Operation/PlaybackOperation.hpp>
#include <service-bluetooth/Constants.hpp>
#include <service-bluetooth/ServiceBluetoothCommon.hpp>
#include <service-bluetooth/BluetoothMessage.hpp>
#include <service-db/Settings.hpp>

#include <type_traits>

const char *ServiceAudio::serviceName = "ServiceAudio";

using namespace audio;

ServiceAudio::ServiceAudio()
    : sys::Service(serviceName, "", 4096 * 2, sys::ServicePriority::Idle),
      audioMux([this](auto... params) { return this->AudioServicesCallback(params...); }),
      settingsProvider(std::make_unique<settings::Settings>(this))
{
    LOG_INFO("[ServiceAudio] Initializing");
    busChannels.push_back(sys::BusChannels::ServiceAudioNotifications);
}

ServiceAudio::~ServiceAudio()
{
    LOG_INFO("[ServiceAudio] Cleaning resources");
}

sys::ReturnCodes ServiceAudio::InitHandler()
{
    static const std::string defaultVolumeHigh = "10";
    static const std::string defaultVolumeLow  = "2";
    static const std::string defaultTrue       = "1";
    static const std::string defaultFalse      = "0";

    settingsCache = {

        // PLAYBACK
        {dbPath(Setting::Volume, PlaybackType::Multimedia, Profile::Type::PlaybackHeadphones), defaultVolumeLow},
        {dbPath(Setting::Volume, PlaybackType::Multimedia, Profile::Type::PlaybackBluetoothA2DP), defaultVolumeLow},
        {dbPath(Setting::Volume, PlaybackType::Multimedia, Profile::Type::PlaybackLoudspeaker), defaultVolumeHigh},

        {dbPath(Setting::Volume, PlaybackType::Notifications, Profile::Type::PlaybackHeadphones), defaultVolumeLow},
        {dbPath(Setting::Volume, PlaybackType::Notifications, Profile::Type::PlaybackBluetoothA2DP), defaultVolumeLow},
        {dbPath(Setting::Volume, PlaybackType::Notifications, Profile::Type::PlaybackLoudspeaker), defaultVolumeHigh},

        {dbPath(Setting::Volume, PlaybackType::KeypadSound, Profile::Type::PlaybackHeadphones), defaultVolumeLow},
        {dbPath(Setting::Volume, PlaybackType::KeypadSound, Profile::Type::PlaybackBluetoothA2DP), defaultVolumeLow},
        {dbPath(Setting::Volume, PlaybackType::KeypadSound, Profile::Type::PlaybackLoudspeaker), defaultVolumeHigh},

        {dbPath(Setting::Volume, PlaybackType::CallRingtone, Profile::Type::PlaybackHeadphones), defaultVolumeLow},
        {dbPath(Setting::Volume, PlaybackType::CallRingtone, Profile::Type::PlaybackBluetoothA2DP), defaultVolumeLow},
        {dbPath(Setting::Volume, PlaybackType::CallRingtone, Profile::Type::PlaybackLoudspeaker), defaultVolumeHigh},

        {dbPath(Setting::Volume, PlaybackType::TextMessageRingtone, Profile::Type::PlaybackHeadphones),
         defaultVolumeLow},
        {dbPath(Setting::Volume, PlaybackType::TextMessageRingtone, Profile::Type::PlaybackBluetoothA2DP),
         defaultVolumeLow},
        {dbPath(Setting::Volume, PlaybackType::TextMessageRingtone, Profile::Type::PlaybackLoudspeaker),
         defaultVolumeHigh},

        // ROUTING
        {dbPath(Setting::Gain, PlaybackType::None, Profile::Type::RoutingBluetoothHSP), "20"},
        {dbPath(Setting::Gain, PlaybackType::None, Profile::Type::RoutingEarspeaker), "0"},
        {dbPath(Setting::Gain, PlaybackType::None, Profile::Type::RoutingLoudspeaker), "20"},
        {dbPath(Setting::Gain, PlaybackType::None, Profile::Type::RoutingHeadphones), "0"},

        {dbPath(Setting::Volume, PlaybackType::None, Profile::Type::RoutingBluetoothHSP), defaultVolumeHigh},
        {dbPath(Setting::Volume, PlaybackType::None, Profile::Type::RoutingEarspeaker), defaultVolumeHigh},
        {dbPath(Setting::Volume, PlaybackType::None, Profile::Type::RoutingHeadphones), defaultVolumeHigh},
        {dbPath(Setting::Volume, PlaybackType::None, Profile::Type::RoutingLoudspeaker), defaultVolumeHigh},

        // RECORDING
        {dbPath(Setting::Gain, PlaybackType::None, Profile::Type::RecordingBuiltInMic), "200"},
        {dbPath(Setting::Gain, PlaybackType::None, Profile::Type::RecordingHeadphones), "100"},
        {dbPath(Setting::Gain, PlaybackType::None, Profile::Type::RecordingBluetoothHSP), "100"},

        // MISC
        {dbPath(Setting::EnableVibration, PlaybackType::Multimedia, Profile::Type::Idle), defaultFalse},
        {dbPath(Setting::EnableVibration, PlaybackType::Notifications, Profile::Type::Idle), defaultTrue},
        {dbPath(Setting::EnableVibration, PlaybackType::KeypadSound, Profile::Type::Idle), defaultFalse},
        {dbPath(Setting::EnableVibration, PlaybackType::CallRingtone, Profile::Type::Idle), defaultTrue},
        {dbPath(Setting::EnableVibration, PlaybackType::TextMessageRingtone, Profile::Type::Idle), defaultTrue},

        {dbPath(Setting::EnableSound, PlaybackType::Multimedia, Profile::Type::Idle), defaultTrue},
        {dbPath(Setting::EnableSound, PlaybackType::Notifications, Profile::Type::Idle), defaultTrue},
        {dbPath(Setting::EnableSound, PlaybackType::KeypadSound, Profile::Type::Idle), defaultTrue},
        {dbPath(Setting::EnableSound, PlaybackType::CallRingtone, Profile::Type::Idle), defaultTrue},
        {dbPath(Setting::EnableSound, PlaybackType::TextMessageRingtone, Profile::Type::Idle), defaultTrue},
    };

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

std::optional<std::string> ServiceAudio::AudioServicesCallback(const sys::Message *msg)
{
    if (const auto *eof = dynamic_cast<const AudioServiceMessage::EndOfFile *>(msg); eof) {
        auto newMsg =
            std::make_shared<AudioNotificationMessage>(AudioNotificationMessage::Type::EndOfFile, eof->GetToken());
        sys::Bus::SendMulticast(newMsg, sys::BusChannels::ServiceAudioNotifications, this);
    }
    else if (const auto *dbReq = dynamic_cast<const AudioServiceMessage::DbRequest *>(msg); dbReq) {
        LOG_DEBUG("ServiceAudio::DBbCallback(%s)", dbReq->GetPath().c_str());
        auto settings_it = settingsCache.find(dbReq->GetPath());
        if (settingsCache.end() == settings_it) {
            LOG_DEBUG("%s does not exist in cache", dbReq->GetPath().c_str());
            return std::nullopt;
        }
        return settings_it->second;
    }
    else if (const auto *btReq = dynamic_cast<const BluetoothProxyMessage *>(msg); btReq) {
        std::shared_ptr<BluetoothProxyMessage> request;
        if (const auto *btStart = dynamic_cast<const BluetoothProxyStartMessage *>(msg); btStart) {
            request = std::make_shared<BluetoothProxyStartMessage>(*btStart);
        }
        else if (const auto *btVolume = dynamic_cast<const BluetoothProxySetVolumeMessage *>(msg); btVolume) {
            request = std::make_shared<BluetoothProxySetVolumeMessage>(*btVolume);
        }
        else if (const auto *btGain = dynamic_cast<const BluetoothProxySetGainMessage *>(msg); btGain) {
            request = std::make_shared<BluetoothProxySetGainMessage>(*btGain);
        }
        else if (const auto *btOutPath = dynamic_cast<const BluetoothProxySetOutputPathMessage *>(msg); btOutPath) {
            request = std::make_shared<BluetoothProxySetOutputPathMessage>(*btOutPath);
        }
        else if (const auto *btInPath = dynamic_cast<const BluetoothProxySetInputPathMessage *>(msg); btInPath) {
            request = std::make_shared<BluetoothProxySetInputPathMessage>(*btInPath);
        }
        else {
            LOG_DEBUG("BluetoothProxyMessage not supported.");
            return std::nullopt;
        }
        sys::Bus::SendUnicast(request, service::name::bluetooth, this);
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

// below static methods will be replaced by final vibration API
static void ExtVibrateOnce()
{
    LOG_ERROR("[Vibration] - Unimplemented - vibration one shot");
}
static void ExtVibrationStart()
{
    LOG_ERROR("[Vibration] - Unimplemented - vibration start");
}
static void ExtVibrationStop()
{
    LOG_ERROR("[Vibration] - Unimplemented - vibration stop");
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
        ExtVibrateOnce();
    }
    else if (input && curVibrationType == VibrationType::Continuous) {
        input.value()->EnableVibration();
    }

    auto &inputs       = audioMux.GetAllInputs();
    auto anyOfInputsOn = std::any_of(inputs.cbegin(), inputs.cend(), [](auto &i) {
        return i.GetVibrationStatus() == AudioMux::VibrationStatus::On;
    });
    if (anyOfInputsOn && !IsVibrationMotorOn()) {
        ExtVibrationStart();
        vibrationMotorStatus = AudioMux::VibrationStatus::On;
    }
    else if (!anyOfInputsOn && IsVibrationMotorOn()) {
        ExtVibrationStop();
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
        sys::Bus::SendMulticast(broadMsg, sys::BusChannels::ServiceAudioNotifications, this);
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

        if (retToken.IsValid()) {
            VibrationUpdate(playbackType, input);
            retCode = audio::RetCode::Success;
        }
    };

    if (opType == Operation::Type::Playback) {
        auto input = audioMux.GetPlaybackInput(playbackType);
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
        AudioStart(input);
        return std::make_unique<AudioStartRoutingResponse>(retCode, retToken);
    }
    return std::make_unique<AudioStartRoutingResponse>(RetCode::OperationNotSet, Token::MakeBadToken());
}

std::unique_ptr<AudioResponseMessage> ServiceAudio::HandleSendEvent(std::shared_ptr<Event> evt)
{
    auto isBT =
        evt->getType() == EventType::BlutoothHSPDeviceState || evt->getType() == EventType::BlutoothA2DPDeviceState;
    if (isBT && evt->getDeviceState() == audio::Event::DeviceState::Connected) {
        auto req = std::make_shared<BluetoothRequestStreamMessage>();
        sys::Bus::SendUnicast(req, service::name::bluetooth, this);
        return std::make_unique<AudioEventResponse>(RetCode::Success);
    }

    for (auto &input : audioMux.GetAllInputs()) {
        input.audio->SendEvent(evt);
    }
    return std::make_unique<AudioEventResponse>(RetCode::Success);
}

std::unique_ptr<AudioResponseMessage> ServiceAudio::HandleStop(const std::vector<audio::PlaybackType> &stopTypes,
                                                               const Token &token,
                                                               bool &muted)
{
    std::vector<std::pair<Token, audio::RetCode>> retCodes;

    auto stopInput = [this](auto inp) {
        if (inp->audio->GetCurrentState() == Audio::State::Idle) {
            return audio::RetCode::Success;
        }
        auto rCode = inp->audio->Stop();
        // Send notification that audio file was stopped
        auto msgStop = std::make_shared<AudioNotificationMessage>(AudioNotificationMessage::Type::Stop, inp->token);
        sys::Bus::SendMulticast(msgStop, sys::BusChannels::ServiceAudioNotifications, this);
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
                muted = true;
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

    // on failure return first false code
    auto it =
        std::find_if_not(retCodes.begin(), retCodes.end(), [](auto p) { return p.second == audio::RetCode::Success; });
    if (it != retCodes.end()) {
        return std::make_unique<AudioStopResponse>(it->second, it->first);
    }

    VibrationUpdate();
    return std::make_unique<AudioStopResponse>(audio::RetCode::Success, token);
}

std::unique_ptr<AudioResponseMessage> ServiceAudio::HandleStop(const std::vector<audio::PlaybackType> &stopTypes,
                                                               const Token &token)
{
    bool muted = false;
    return HandleStop(stopTypes, token, muted);
}

void ServiceAudio::HandleNotification(const AudioNotificationMessage::Type &type, const Token &token)
{
    if (type == AudioNotificationMessage::Type::EndOfFile) {
        auto input = audioMux.GetInput(token);
        if (input && ShouldLoop((*input)->audio->GetCurrentOperationPlaybackType())) {
            (*input)->audio->Start();
        }
        else {
            auto newMsg = std::make_shared<AudioStopRequest>(token);
            sys::Bus::SendUnicast(newMsg, ServiceAudio::serviceName, this);
        }
        return;
    }

    if (type != AudioNotificationMessage::Type::Stop) {
        LOG_DEBUG("Unhandled AudioNotificationMessage");
    }
}

auto ServiceAudio::HandleKeyPressed(const int step) -> std::unique_ptr<AudioKeyPressedResponse>
{
    const std::vector<audio::PlaybackType> typesToMute = {audio::PlaybackType::Notifications,
                                                          audio::PlaybackType::CallRingtone,
                                                          audio::PlaybackType::TextMessageRingtone};

    // mute if 0 and return with parameter shouldn't popup
    bool muted         = false;
    const auto context = getCurrentContext();
    if (step < 0) {
        HandleStop(typesToMute, Token(), muted);
        if (muted) {
            return std::make_unique<AudioKeyPressedResponse>(audio::RetCode::Success, 0, muted, context);
        }
    }
    const auto volume =
        utils::getNumericValue<int>(getSetting(Setting::Volume, Profile::Type::Idle, PlaybackType::None));
    const auto newVolume = std::clamp(volume + step, static_cast<int>(minVolume), static_cast<int>(maxVolume));
    setSetting(Setting::Volume, std::to_string(newVolume), Profile::Type::Idle, PlaybackType::None);
    return std::make_unique<AudioKeyPressedResponse>(audio::RetCode::Success, newVolume, muted, context);
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
    bool isBusy = IsBusy();

    auto &msgType = typeid(*msgl);

    if (msgType == typeid(AudioNotificationMessage)) {
        auto *msg = static_cast<AudioNotificationMessage *>(msgl);
        HandleNotification(msg->type, msg->token);
    }
    else if (msgType == typeid(AudioGetSetting)) {
        auto *msg   = static_cast<AudioGetSetting *>(msgl);
        auto value  = getSetting(msg->setting, msg->profileType, msg->playbackType);
        responseMsg = std::make_shared<AudioResponseMessage>(RetCode::Success, utils::getNumericValue<float>(value));
    }
    else if (msgType == typeid(AudioSetSetting)) {
        auto *msg = static_cast<AudioSetSetting *>(msgl);
        setSetting(msg->setting, msg->val, msg->profileType, msg->playbackType);
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
        sys::Bus::SendMulticast(broadMsg, sys::BusChannels::ServiceAudioNotifications, this);
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

    if (setting == Setting::EnableVibration || setting == Setting::EnableSound) {
        targetProfile = Profile::Type::Idle;
    }

    std::string path = dbPath(setting, targetPlayback, targetProfile);
    if (const auto set_it = settingsCache.find(path); settingsCache.end() != set_it) {
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

    if (profileType == Profile::Type::Idle && playbackType == PlaybackType::None) {
        if (activeInput = audioMux.GetActiveInput(); activeInput.has_value()) {
            const auto &currentOperation = (*activeInput)->audio->GetCurrentOperation();
            updatedProfile               = currentOperation.GetProfile()->GetType();
            updatedPlayback              = (*activeInput)->audio->GetCurrentOperationPlaybackType();
        }
        else if (auto input = audioMux.GetIdleInput(); input && (setting == audio::Setting::Volume)) {
            updatedProfile  = (*input)->audio->GetPriorityPlaybackProfile();
            updatedPlayback = PlaybackType::CallRingtone;
            valueToSet      = std::clamp(utils::getNumericValue<audio::Volume>(value), minVolume, maxVolume);
        }
        else {
            return;
        }
    }

    switch (setting) {
    case Setting::Volume: {
        const auto clampedValue = std::clamp(utils::getNumericValue<audio::Volume>(value), minVolume, maxVolume);
        valueToSet              = std::to_string(clampedValue);
        if (activeInput) {
            retCode = activeInput.value()->audio->SetOutputVolume(clampedValue);
        }
    } break;
    case Setting::Gain: {
        const auto clampedValue = std::clamp(utils::getNumericValue<audio::Gain>(value), minGain, maxGain);
        valueToSet              = std::to_string(clampedValue);
        if (activeInput) {
            retCode = activeInput.value()->audio->SetInputGain(clampedValue);
        }
    } break;
    case Setting::EnableVibration:
    case Setting::EnableSound: {
        updatedProfile = audio::Profile::Type::Idle;
        valueToSet     = value;
    } break;
    }

    if (retCode == RetCode::Success) {
        settingsProvider->setValue(dbPath(setting, updatedPlayback, updatedProfile), valueToSet);
        settingsCache[dbPath(setting, updatedPlayback, updatedProfile)] = valueToSet;
    }
}

const std::pair<audio::Profile::Type, audio::PlaybackType> ServiceAudio::getCurrentContext()
{
    const auto activeInput = audioMux.GetActiveInput();
    if (!activeInput.has_value()) {
        const auto idleInput = audioMux.GetIdleInput();
        if (idleInput.has_value()) {
            return {(*idleInput)->audio->GetPriorityPlaybackProfile(), audio::PlaybackType::CallRingtone};
        }
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
