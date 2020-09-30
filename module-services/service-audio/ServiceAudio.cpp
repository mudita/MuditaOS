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
        auto msg = std::make_shared<AudioNotificationMessage>(
            static_cast<AudioNotificationMessage::Type>(AudioNotificationMessage::Type::EndOfFile), e.token);
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

std::optional<ServiceAudio::VibrationType> ServiceAudio::GetVibrationType(const audio::PlaybackType &type)
{
    if (!IsVibrationEnabled(type)) {
        return std::nullopt;
    }

    if (type == PlaybackType::CallRingtone) {
        return VibrationType::Continuous;
    }
    else if (type == PlaybackType::Notifications || type == PlaybackType::TextMessageRingtone) {
        return VibrationType::OneShot;
    }
    return std::nullopt;
}

void ServiceAudio::VibrationStart(const audio::PlaybackType &type, std::shared_ptr<AudioResponseMessage> &resp)
{
    auto vibType = GetVibrationType(type);
    if (!vibType || vibrationToken.IsValid()) {
        return;
    }

    if (vibType == VibrationType::OneShot) {
        vibrationToken = Token();
        ExtVibrateOnce();
    }
    else if (vibType == VibrationType::Continuous) {
        if (resp && resp->token.IsValid()) {
            // audio has started
            vibrationToken = resp->token;
        }
        else {
            // audio did not start but we still want vibration
            vibrationToken = audioMux.IncrementToken();
            resp           = std::make_unique<AudioResponseMessage>(RetCode::Success, Tags(), vibrationToken);
        }
        ExtVibrationStart();
    }
}

void ServiceAudio::VibrationStop(const Token &token)
{
    if (vibrationToken.IsValid() && vibrationToken == token) {
        ExtVibrationStop();
        vibrationToken = Token();
    }
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

std::unique_ptr<AudioResponseMessage> ServiceAudio::HandlePause(std::optional<AudioRequestMessage *> msg)
{

    audio::RetCode retCode = audio::RetCode::Failed;
    audio::Token retToken  = Token::MakeBadToken();

    auto pauseInput = [this](AudioMux::Input &audioInput) {
        auto playbackType = audioInput.audio->GetCurrentOperationPlaybackType();
        if (playbackType && IsResumable(*playbackType)) {
            audioInput.audio->Pause();
            return audioInput.token;
        }
        else {
            audioInput.audio->Stop();
            auto broadMsg = std::make_shared<AudioNotificationMessage>(
                static_cast<AudioNotificationMessage::Type>(AudioNotificationMessage::Type::Stop), audioInput.token);
            sys::Bus::SendMulticast(broadMsg, sys::BusChannels::ServiceAudioNotifications, this);
            audioMux.IncrementToken(&audioInput);
            return Token::MakeBadToken();
        }
    };

    if (msg) {
        std::optional<AudioMux::Input *> input = audioMux.GetInput((*msg)->token);
        if (!input) {
            retToken = pauseInput(**input);
        }
    }
    else {
        for (auto &audioInput : audioMux.GetAllInputs()) {
            if (audioInput.audio->GetCurrentOperation()->GetState() == Operation::State::Active) {
                pauseInput(audioInput);
            }
        }
        return std::make_unique<AudioResponseMessage>(audio::RetCode::Success, Tags(), retToken);
    }

    return std::make_unique<AudioResponseMessage>(retCode, Tags(), retToken);
}

template <typename... Args>
std::unique_ptr<AudioResponseMessage> ServiceAudio::HandleStart(std::optional<AudioMux::Input *> input,
                                                                Operation::Type opType,
                                                                Args... args)
{
    audio::RetCode retCode = audio::RetCode::Failed;
    audio::Token retToken  = Token::MakeBadToken();

    if (input) {

        HandlePause();
        retToken = audioMux.IncrementToken(input);
        retCode  = (*input)->audio->Start(opType, retToken, args...);
    }
    if (retCode != audio::RetCode::Success) {
        retToken = Token::MakeBadToken();
    }
    return std::make_unique<AudioResponseMessage>(retCode, Tags(), retToken);
}

std::unique_ptr<AudioResponseMessage> ServiceAudio::HandleStop(AudioStopMessage *msg)
{
    const auto playbackTypesToStop       = msg->stopVec;
    std::vector<audio::RetCode> retCodes = {audio::RetCode::Success};

    auto stopInput = [this](auto inp) {
        if (inp->audio->GetCurrentState() == Audio::State::Idle) {
            return audio::RetCode::Success;
        }
        auto rCode = inp->audio->Stop();
        // Send notification that audio file was stopped
        auto msgStop = std::make_shared<AudioNotificationMessage>(
            static_cast<AudioNotificationMessage::Type>(AudioNotificationMessage::Type::Stop), inp->token);
        sys::Bus::SendMulticast(msgStop, sys::BusChannels::ServiceAudioNotifications, this);
        audioMux.IncrementToken(inp);
        return rCode;
    };

    // stop by token
    if (auto tokenInput = audioMux.GetInput(msg->token)) {
        retCodes.emplace_back(stopInput(tokenInput.value()));
    }
    // stop with vector of playback types
    else if (!playbackTypesToStop.empty()) {
        for (auto &input : audioMux.GetAllInputs()) {
            const auto currentOperation = input.audio->GetCurrentOperation();
            if (currentOperation && (std::find(playbackTypesToStop.begin(),
                                               playbackTypesToStop.end(),
                                               currentOperation->GetPlaybackType()) != playbackTypesToStop.end())) {
                retCodes.emplace_back(stopInput(&input));
            }
        }
    }
    // stop all audio
    else if (!msg->token.IsUninitialized()) {
        for (auto &input : audioMux.GetAllInputs()) {
            retCodes.emplace_back(stopInput(&input));
        }
    }

    // on failure return first false code
    auto it = std::find_if_not(retCodes.begin(), retCodes.end(), [](auto p) { return p == audio::RetCode::Success; });
    if (it != retCodes.end()) {
        return std::make_unique<AudioResponseMessage>(*it);
    }
    return std::make_unique<AudioResponseMessage>(audio::RetCode::Success);
}

sys::Message_t ServiceAudio::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
{
    std::shared_ptr<AudioResponseMessage> responseMsg;

    auto msgType = static_cast<int>(msgl->messageType);
    LOG_DEBUG("msgType %d", msgType);

    if (auto *msg = dynamic_cast<AudioNotificationMessage *>(msgl)) {
        switch (msg->type) {
        case AudioNotificationMessage::Type::EndOfFile: {
            auto input = audioMux.GetInput(msg->token);
            if (input && ShouldLoop((*input)->audio->GetCurrentOperationPlaybackType())) {
                (*input)->audio->Start();
            }
            else {
                auto newMsg = std::make_shared<AudioStopMessage>(msg->token);
                sys::Bus::SendUnicast(newMsg, ServiceAudio::serviceName, this);
            }
        } break;

        case AudioNotificationMessage::Type::Stop: {

        } break;
        default:
            LOG_DEBUG("Unhandled AudioNotificationMessage");
            break;
        }
    }
    else if (auto *msg = dynamic_cast<AudioGetSetting *>(msgl)) {
        auto value  = getSetting(msg->setting, msg->profileType, msg->playbackType);
        responseMsg = std::make_shared<AudioResponseMessage>(RetCode::Success, utils::getValue<float>(value));
    }
    else if (auto *msg = dynamic_cast<AudioSetSetting *>(msgl)) {
        setSetting(msg->setting, msg->val, msg->profileType, msg->playbackType);
        responseMsg     = std::make_shared<AudioResponseMessage>(RetCode::Success);
    }
    else if (auto *msg = dynamic_cast<AudioStopMessage *>(msgl)) {
        VibrationStop(msg->token);
        responseMsg = HandleStop(msg);
    }
    else if (auto *msg = dynamic_cast<AudioStartMessage *>(msgl)) {
        if (IsPlaybackEnabled(msg->playbackType)) {
            if (auto input = audioMux.GetPlaybackInput(msg->token, msg->playbackType)) {
                responseMsg = HandleStart(input, Operation::Type::Playback, msg->fileName.c_str(), msg->playbackType);
            }
        }
        VibrationStart(msg->playbackType, responseMsg);
    }
    else if (auto *msg = dynamic_cast<AudioRequestMessage *>(msgl)) {
        switch (msg->type) {
        case MessageType::AudioRecorderStart: {
            if (auto input = audioMux.GetRecordingInput()) {
                responseMsg = std::make_shared<AudioResponseMessage>(
                    (*input)->audio->Start(Operation::Type::Recorder, (*input)->token, msg->fileName.c_str()));
            }
        } break;

        case MessageType::AudioRoutingStart: {
            LOG_DEBUG("AudioRoutingStart");
            if (auto input = audioMux.GetRoutingInput(true)) {
                responseMsg = HandleStart(input, Operation::Type::Router);
            }
        } break;

        case MessageType::AudioPause: {
            responseMsg = HandlePause(msg);
        } break;

        case MessageType::AudioResume: {
            if (auto input = audioMux.GetInput(msg->token)) {
                responseMsg = std::make_shared<AudioResponseMessage>((*input)->audio->Resume());
            }
        } break;

        case MessageType::AudioGetFileTags: {
            if (auto input = audioMux.GetAvailableInput()) {
                auto tag = (*input)->audio->GetFileTags(msg->fileName.c_str());
                if (tag) {
                    responseMsg = std::make_shared<AudioResponseMessage>(RetCode::Success, tag.value());
                }
                else {
                    responseMsg = std::make_shared<AudioResponseMessage>(RetCode::FileDoesntExist);
                }
            }
        } break;

        case MessageType::AudioRoutingRecordCtrl: {
            if (auto input = audioMux.GetRecordingInput()) {
                responseMsg = std::make_shared<AudioResponseMessage>((*input)->audio->SendEvent(
                    msg->enable ? Operation::Event::StartCallRecording : Operation::Event::StopCallRecording));
            }
        } break;

        case MessageType::AudioRoutingMute: {
            if (auto input = audioMux.GetRecordingInput()) {
                responseMsg = std::make_shared<AudioResponseMessage>((*input)->audio->SendEvent(
                    msg->enable ? Operation::Event::CallMute : Operation::Event::CallUnmute));
            }
        } break;

        case MessageType::AudioRoutingSpeakerhone: {
            if (auto input = audioMux.GetRecordingInput()) {
                responseMsg = std::make_shared<AudioResponseMessage>((*input)->audio->SendEvent(
                    msg->enable ? Operation::Event::CallSpeakerphoneOn : Operation::Event::CallSpeakerphoneOff));
            }
        } break;
        case MessageType::AudioRoutingHeadset: {
            if (auto input = audioMux.GetRecordingInput()) {
                responseMsg = std::make_shared<AudioResponseMessage>((*input)->audio->SendEvent(
                    msg->enable ? Operation::Event::HeadphonesPlugin : Operation::Event::HeadphonesUnplug));
            }
        } break;

        default:
            LOG_ERROR("Unhandled AudioRequestMessage");
            responseMsg = std::make_shared<AudioResponseMessage>(RetCode::Failed);
            break;
        }
    }
    else {
        LOG_DEBUG("Unhandled message");
    }

    if (responseMsg) {
        return responseMsg;
    }
    else {
        return std::make_shared<AudioResponseMessage>(RetCode::Failed, Tags{}, Token::MakeBadToken());
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

void ServiceAudio::updateDbValue(const audio::Operation *currentOperation,
                                 const audio::Setting &setting,
                                 const uint32_t &value)
{
    if (currentOperation == nullptr) {
        return;
    }
    const auto currentProfile = currentOperation->GetProfile();

    auto dbPath = audio::str(setting, currentOperation->GetPlaybackType(), currentProfile->GetType());
    updateDbValue(dbPath, setting, std::to_string(value));
}

void ServiceAudio::updateDbValue(const audio::Operation *currentOperation,
                                 const audio::Setting &setting,
                                 const bool &value)
{
    if (currentOperation == nullptr) {
        return;
    }

    auto dbPath = audio::str(setting, currentOperation->GetPlaybackType());
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

    const auto currentOperation = (*activeInput)->audio->GetCurrentOperation();
    const auto path             = [&setting, &currentOperation]() -> std::string {
        if (setting == audio::Setting::EnableVibration || setting == audio::Setting::EnableSound) {
            return audio::str(setting, currentOperation->GetPlaybackType());
        }
        else {
            return audio::str(setting, currentOperation->GetPlaybackType(), currentOperation->GetProfile()->GetType());
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
    const auto currentOperation = audio->GetCurrentOperation();
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
