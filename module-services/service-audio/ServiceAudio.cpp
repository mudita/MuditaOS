/*
 *  @file ServiceAudio.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 29.07.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#include "ServiceAudio.hpp"
#include "messages/AudioMessage.hpp"
#include "Audio/Operation/IdleOperation.hpp"
#include "Audio/Operation/PlaybackOperation.hpp"

#include <type_traits>

const char *ServiceAudio::serviceName = "ServiceAudio";

using namespace audio;

ServiceAudio::ServiceAudio()
    : sys::Service(serviceName, "", 4096 * 2, sys::ServicePriority::Idle),
      audioMux([this](auto... params) { return this->AsyncCallback(params...); },
               [this](auto... params) { return this->DbCallback(params...); })
{
    busChannels.push_back(sys::BusChannels::ServiceAudioNotifications);
    LOG_INFO("[ServiceAudio] Initializing");
}

ServiceAudio::~ServiceAudio()
{
    LOG_INFO("[ServiceAudio] Cleaning resources");
}

sys::ReturnCodes ServiceAudio::InitHandler()
{
    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceAudio::DeinitHandler()
{
    return sys::ReturnCodes::Success;
}

int32_t ServiceAudio::AsyncCallback(PlaybackEvent e)
{
    switch (e.event) {
    case audio::PlaybackEventType::EndOfFile: {
        auto msg = std::make_shared<AudioNotificationMessage>(AudioNotificationMessage::Type::EndOfFile, e.token);
        sys::Bus::SendMulticast(msg, sys::BusChannels::ServiceAudioNotifications, this);
    } break;
    case audio::PlaybackEventType::FileSystemNoSpace:
    case audio::PlaybackEventType::Empty:
        break;
    }
    return 0;
};

uint32_t ServiceAudio::DbCallback(const std::string &path, const uint32_t &defaultValue)
{
    this->addOrIgnoreEntry(path, std::to_string(defaultValue));
    return this->fetchAudioSettingFromDb(path, defaultValue);
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
// below static members will be replaced by accessors to DB entries
static bool IsVibrationEnabled(const audio::PlaybackType &type)
{
    return true;
}
static bool IsPlaybackEnabled(const audio::PlaybackType &type)
{
    return true;
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
    if (auto input = audioMux.GetAvailableInput()) {
        auto tag = (*input)->audio->GetFileTags(fileName.c_str());
        if (tag) {
            return std::make_unique<AudioResponseMessage>(RetCode::Success, tag.value());
        }
        else {
            return std::make_unique<AudioResponseMessage>(RetCode::FileDoesntExist);
        }
    }
    return {};
}

std::unique_ptr<AudioResponseMessage> ServiceAudio::HandlePause(const Token &token)
{
    auto input = audioMux.GetInput(token);
    return HandlePause(input);
}

void ServiceAudio::addOrIgnoreEntry(const std::string &profilePath, const std::string &defaultValue)
{
    auto [code, msg] = DBServiceAPI::GetQueryWithReply(
        this,
        db::Interface::Name::Settings_v2,
        std::make_unique<db::query::settings::AddOrIgnoreQuery>(
            SettingsRecord_v2{SettingsTableRow_v2{{.ID = DB_ID_NONE}, .path = profilePath, .value = defaultValue}}),
        audio::audioOperationTimeout);

    if (code == sys::ReturnCodes::Success && msg != nullptr) {
        auto queryResponse = dynamic_cast<db::QueryResponse *>(msg.get());
        assert(queryResponse != nullptr);

        auto settingsResultResponse = queryResponse->getResult();
        assert(dynamic_cast<db::query::settings::AddOrIgnoreResult *>(settingsResultResponse.get()) != nullptr);
    }
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
    return std::make_unique<AudioPauseResponse>(RetCode::TokenNotFound, Token::MakeBadToken());
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
            if (opType == Operation::Type::Recorder) {
                // since recording works on the same audio as routing
                retToken = input.value()->token;
            }
            else {
                retToken = audioMux.ResetInput(input);
            }

            if (IsPlaybackEnabled(playbackType)) {
                retCode = (*input)->audio->Start(opType, retToken, fileName.c_str(), playbackType);
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
        auto input = audioMux.GetRecordingInput();
        AudioStart(input);
        return std::make_unique<AudioStartRecorderResponse>(retCode, retToken);
    }
    else if (opType == Operation::Type::Router) {
        auto input = audioMux.GetRecordingInput();
        AudioStart(input);
        return std::make_unique<AudioStartRoutingResponse>(retCode, retToken);
    }
    return {};
}

std::unique_ptr<AudioResponseMessage> ServiceAudio::HandleRoutingControl(
    const AudioRoutingControlRequest::ControlType &controlType, const bool enable)
{
    std::optional<Operation::Event> evt;

    if (controlType == AudioRoutingControlRequest::ControlType::Mute) {
        evt = enable ? Operation::Event::CallMute : Operation::Event::CallUnmute;
    }
    else if (controlType == AudioRoutingControlRequest::ControlType::SwitchSpeakerphone) {
        evt = enable ? Operation::Event::CallSpeakerphoneOn : Operation::Event::CallSpeakerphoneOff;
    }
    else if (controlType == AudioRoutingControlRequest::ControlType::SwitchHeadphones) {
        evt = enable ? Operation::Event::HeadphonesPlugin : Operation::Event::HeadphonesUnplug;
    }

    if (auto input = audioMux.GetRoutingInput(); evt && input) {
        return std::make_unique<AudioResponseMessage>((*input)->audio->SendEvent(evt.value()));
    }

    return {};
}

std::unique_ptr<AudioResponseMessage> ServiceAudio::HandleStop(const std::vector<audio::PlaybackType> &stopTypes,
                                                               const Token &token)
{
    std::vector<audio::RetCode> retCodes = {audio::RetCode::Success};

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
        retCodes.emplace_back(stopInput(tokenInput.value()));
    }
    else if (token.IsValid()) {
        return std::make_unique<AudioPauseResponse>(RetCode::TokenNotFound, Token::MakeBadToken());
    }
    // stop with vector of playback types
    else if (!stopTypes.empty()) {
        for (auto &input : audioMux.GetAllInputs()) {
            const auto &currentOperation = input.audio->GetCurrentOperation();
            if (std::find(stopTypes.begin(), stopTypes.end(), currentOperation.GetPlaybackType()) != stopTypes.end()) {
                retCodes.emplace_back(stopInput(&input));
            }
        }
    }
    // stop all audio
    else if (token.IsUninitialized()) {
        for (auto &input : audioMux.GetAllInputs()) {
            retCodes.emplace_back(stopInput(&input));
        }
    }

    // on failure return first false code
    auto it = std::find_if_not(retCodes.begin(), retCodes.end(), [](auto p) { return p == audio::RetCode::Success; });
    if (it != retCodes.end()) {
        return std::make_unique<AudioResponseMessage>(*it);
    }

    VibrationUpdate();
    return std::make_unique<AudioResponseMessage>(audio::RetCode::Success);
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

sys::Message_t ServiceAudio::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
{
    std::shared_ptr<AudioResponseMessage> responseMsg;

    auto &msgType = typeid(*msgl);
    LOG_DEBUG("msgType %d", static_cast<int>(msgl->messageType));

    if (msgType == typeid(AudioNotificationMessage)) {
        auto *msg = static_cast<AudioNotificationMessage *>(msgl);
        HandleNotification(msg->type, msg->token);
    }
    else if (msgType == typeid(AudioGetSetting)) {
        auto *msg   = static_cast<AudioGetSetting *>(msgl);
        auto value  = getSetting(msg->setting, msg->profileType, msg->playbackType);
        responseMsg = std::make_shared<AudioResponseMessage>(RetCode::Success, utils::getValue<float>(value));
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
    else if (msgType == typeid(AudioRoutingControlRequest)) {
        auto *msg   = static_cast<AudioRoutingControlRequest *>(msgl);
        responseMsg = HandleRoutingControl(msg->controlType, msg->enable);
    }
    else if (msgType == typeid(AudioKeyPressed)) {
        auto *msg   = static_cast<AudioKeyPressed *>(msgl);
        responseMsg = std::make_shared<AudioKeyPressedResponseMessage>(
            RetCode::Success, static_cast<float>(msg->volume), msg->shouldPopup);
    }
    else {
        LOG_DEBUG("Unhandled message");
    }

    if (responseMsg) {
        return responseMsg;
    }
    else {
        return std::make_shared<AudioResponseMessage>(RetCode::Failed);
    }
}

void ServiceAudio::updateDbValue(const std::string &path, const audio::Setting &setting, const std::string &value)
{
    if (path.empty()) {
        return;
    }

    auto query = std::make_unique<db::query::settings::UpdateQuery>(
        SettingsRecord_v2({{.ID = DB_ID_NONE}, .path = path, .value = value}));

    DBServiceAPI::GetQuery(this, db::Interface::Name::Settings_v2, std::move(query));
}

void ServiceAudio::updateDbValue(const audio::Operation &currentOperation,
                                 const audio::Setting &setting,
                                 const uint32_t &value)
{
    const auto currentProfile = currentOperation.GetProfile();

    auto dbPath = audio::str(setting, currentOperation.GetPlaybackType(), currentProfile->GetType());
    updateDbValue(dbPath, setting, std::to_string(value));
}

void ServiceAudio::updateDbValue(const audio::Operation &currentOperation,
                                 const audio::Setting &setting,
                                 const bool &value)
{
    auto dbPath = audio::str(setting, currentOperation.GetPlaybackType());
    updateDbValue(dbPath, setting, std::to_string(value));
}

void ServiceAudio::setSetting(const audio::Setting &setting,
                              const std::string &value,
                              const audio::Profile::Type &profileType,
                              const audio::PlaybackType &playbackType)
{
    if (setting == audio::Setting::EnableVibration || setting == audio::Setting::EnableSound) {
        const auto path = audio::str(setting, playbackType);
        addOrIgnoreEntry(path, value);
        updateDbValue(path, setting, value);
    }
    else {
        if (profileType == Profile::Type::Idle) {
            setCurrentSetting(setting, value);
        }
        const auto path = audio::str(setting, playbackType, profileType);
        updateDbValue(path, setting, value);
    }
}

std::string ServiceAudio::getSetting(const audio::Setting &setting,
                                     const audio::Profile::Type &profileType,
                                     const audio::PlaybackType &playbackType)
{
    const std::string defaultValue = "";
    if (profileType == audio::Profile::Type::Idle) {
        if (const auto optSetting = getCurrentSetting(setting); optSetting.has_value()) {
            return optSetting.value();
        }
        return defaultValue;
    }
    const auto path = audio::str(setting, playbackType, profileType);
    return fetchAudioSettingFromDb(path, defaultValue);
}

std::optional<std::string> ServiceAudio::getCurrentSetting(const audio::Setting &setting)
{
    const auto activeInput = audioMux.GetActiveInput();

    if (!activeInput.has_value()) {
        const auto idleInput = audioMux.GetIdleInput();
        if (idleInput.has_value() && setting == audio::Setting::Volume) {
            const auto path = audio::str(audio::Setting::Volume,
                                         audio::PlaybackType::CallRingtone,
                                         (*idleInput)->audio->GetHeadphonesInserted());
            return fetchAudioSettingFromDb<std::string>(path, "");
        }
        return {};
    }

    const auto &currentOperation = (*activeInput)->audio->GetCurrentOperation();
    const auto path             = [&setting, &currentOperation]() -> std::string {
        if (setting == audio::Setting::EnableVibration || setting == audio::Setting::EnableSound) {
            return audio::str(setting, currentOperation.GetPlaybackType());
        }
        else {
            return audio::str(setting, currentOperation.GetPlaybackType(), currentOperation.GetProfile()->GetType());
        }
    }();
    return fetchAudioSettingFromDb<std::string>(path, "");
}

void ServiceAudio::setCurrentSetting(const audio::Setting &setting, const std::string &value)
{
    const auto activeInput = audioMux.GetActiveInput();

    if (!activeInput.has_value()) {
        const auto idleInput = audioMux.GetIdleInput();
        if (idleInput.has_value() && setting == audio::Setting::Volume) {
            const auto path = audio::str(audio::Setting::Volume,
                                         audio::PlaybackType::CallRingtone,
                                         (*idleInput)->audio->GetHeadphonesInserted());
            addOrIgnoreEntry(path, std::to_string(audio::playbackDefaults::defaultLoudspeakerVolume));
            const auto valueToSet = std::clamp(utils::getValue<audio::Volume>(value), minVolume, maxVolume);
            updateDbValue(path, Setting::Volume, std::to_string(valueToSet));
        }
        return;
    }

    auto &audio                 = (*activeInput)->audio;
    const auto &currentOperation = audio->GetCurrentOperation();
    switch (setting) {
    case audio::Setting::Volume: {
        const auto valueToSet = std::clamp(utils::getValue<audio::Volume>(value), minVolume, maxVolume);
        if (audio->SetOutputVolume(valueToSet) == audio::RetCode::Success) {
            updateDbValue(currentOperation, setting, valueToSet);
        }
    } break;
    case audio::Setting::Gain: {
        const auto valueToSet = std::clamp(utils::getValue<audio::Gain>(value), minGain, maxGain);
        if (audio->SetInputGain(valueToSet) == audio::RetCode::Success) {
            updateDbValue(currentOperation, setting, valueToSet);
        }
    } break;
    case audio::Setting::EnableVibration: {
        const auto valueToSet = utils::getValue<audio::Vibrate>(value);
        updateDbValue(currentOperation, setting, valueToSet);
    } break;
    case audio::Setting::EnableSound: {
        const auto valueToSet = utils::getValue<audio::EnableSound>(value);
        updateDbValue(currentOperation, setting, valueToSet);
    }
    }
}
