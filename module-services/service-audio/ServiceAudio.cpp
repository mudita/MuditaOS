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

const char *ServiceAudio::serviceName = "ServiceAudio";

using namespace audio;

ServiceAudio::ServiceAudio()
    : sys::Service(serviceName, "", 4096 * 2, sys::ServicePriority::Idle),
      audioMux([this](auto... params) { return this->AsyncCallback(params...); },
               [this](auto... params) { return this->DbCallback(params...); })
{
    busChannels.push_back(sys::BusChannels::ServiceAudioNotifications);
    LOG_INFO("[ServiceAudio] Initializing");

    // this is just a test
    connect(PlaybackStartReq("", PlaybackType::None), [&](sys::DataMessage *req, sys::ResponseMessage *response) {
        if (auto *audioReq = dynamic_cast<PlaybackStartReq *>(req)) {
            LOG_ERROR("Play %s", audioReq->fileName.c_str());
        }
        return std::make_shared<AudioResponseMessage>(audio::RetCode::Success, 1.55);
    });
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
    this->addOrIgnoreEntry(path, defaultValue);
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

constexpr bool ServiceAudio::ShouldLoop(const audio::PlaybackType &type) const
{
    return type == audio::PlaybackType::CallRingtone;
}

std::unique_ptr<AudioResponseMessage> ServiceAudio::HandlePause(std::optional<AudioRequestMessage *> msg)
{

    audio::RetCode retCode = audio::RetCode::Failed;
    audio::Token retToken  = Token::MakeBadToken();

    auto pauseInput = [this](AudioMux::Input &audioInput) {
        if (IsResumable(audioInput.audio.GetCurrentOperation()->GetPlaybackType())) {
            audioInput.audio.Pause();
            return audioInput.token;
        }
        else {
            audioInput.audio.Stop();
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
            if (audioInput.audio.GetCurrentOperation()->GetState() == Operation::State::Active) {
                pauseInput(audioInput);
            }
        }
        return std::make_unique<AudioResponseMessage>(audio::RetCode::Success, Tags(), retToken);
    }

    return std::make_unique<AudioResponseMessage>(retCode, Tags(), retToken);
}

template <typename... Args>
std::unique_ptr<AudioResponseMessage> ServiceAudio::HandleStart(std::optional<AudioMux::Input *> input,
                                                                AudioRequestMessage *msg,
                                                                Operation::Type opType,
                                                                Args... args)
{
    audio::RetCode retCode = audio::RetCode::Failed;
    audio::Token retToken  = Token::MakeBadToken();

    if (input) {

        HandlePause();
        retToken = audioMux.IncrementToken(input);
        retCode  = (*input)->audio.Start(opType, retToken, args...);
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
        if (inp->audio.GetCurrentState() == Audio::State::Idle) {
            return audio::RetCode::Success;
        }
        auto rCode = inp->audio.Stop();
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
            const auto currentOperation = input.audio.GetCurrentOperation();
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
    std::shared_ptr<sys::ResponseMessage> responseMsg;

    auto msgType = static_cast<int>(msgl->messageType);
    LOG_DEBUG("msgType %d", msgType);

    if (auto *msg = dynamic_cast<AudioNotificationMessage *>(msgl)) {
        switch (msg->type) {

        case AudioNotificationMessage::Type::EndOfFile: {
            auto input = audioMux.GetInput(msg->token);
            if (input && ShouldLoop((*input)->audio.GetCurrentOperation()->GetPlaybackType())) {
                (*input)->audio.Start();
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
        responseMsg = std::make_shared<AudioResponseMessage>(RetCode::Success, value);
    }
    else if (auto *msg = dynamic_cast<AudioSetSetting *>(msgl)) {
        setSetting(msg->setting, msg->val, msg->profileType, msg->playbackType);
        responseMsg     = std::make_shared<AudioResponseMessage>(RetCode::Success);
    }
    else if (auto *msg = dynamic_cast<AudioStopMessage *>(msgl)) {
        responseMsg = HandleStop(msg);
    }
    else if (auto *msg = dynamic_cast<AudioRequestMessage *>(msgl)) {
        switch (msg->type) {
        case MessageType::AudioPlaybackStart: {
            if (auto input = audioMux.GetPlaybackInput(msg->token, msg->playbackType)) {
                responseMsg =
                    HandleStart(input, msg, Operation::Type::Playback, msg->fileName.c_str(), msg->playbackType);
            }
        } break;

        case MessageType::AudioRecorderStart: {
            if (auto input = audioMux.GetRecordingInput()) {
                responseMsg = std::make_shared<AudioResponseMessage>(
                    (*input)->audio.Start(Operation::Type::Recorder, (*input)->token, msg->fileName.c_str()));
            }
        } break;

        case MessageType::AudioRoutingStart: {
            LOG_DEBUG("AudioRoutingStart");
            if (auto input = audioMux.GetRoutingInput(true)) {
                responseMsg = HandleStart(input, msg, Operation::Type::Router);
            }
        } break;

        case MessageType::AudioPause: {
            responseMsg = HandlePause(msg);
        } break;

        case MessageType::AudioResume: {
            if (auto input = audioMux.GetInput(msg->token)) {
                responseMsg = std::make_shared<AudioResponseMessage>((*input)->audio.Resume());
            }
        } break;

        case MessageType::AudioGetFileTags: {
            if (auto input = audioMux.GetInput()) {
                auto tag = (*input)->audio.GetFileTags(msg->fileName.c_str());
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
                responseMsg = std::make_shared<AudioResponseMessage>((*input)->audio.SendEvent(
                    msg->enable ? Operation::Event::StartCallRecording : Operation::Event::StopCallRecording));
            }
        } break;

        case MessageType::AudioRoutingMute: {
            if (auto input = audioMux.GetRecordingInput()) {
                responseMsg = std::make_shared<AudioResponseMessage>(
                    (*input)->audio.SendEvent(msg->enable ? Operation::Event::CallMute : Operation::Event::CallUnmute));
            }
        } break;

        case MessageType::AudioRoutingSpeakerhone: {
            if (auto input = audioMux.GetRecordingInput()) {
                responseMsg = std::make_shared<AudioResponseMessage>((*input)->audio.SendEvent(
                    msg->enable ? Operation::Event::CallSpeakerphoneOn : Operation::Event::CallSpeakerphoneOff));
            }
        } break;

        case MessageType::AudioRoutingHeadset: {
            if (auto input = audioMux.GetRecordingInput()) {
                responseMsg = std::make_shared<AudioResponseMessage>((*input)->audio.SendEvent(
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

void ServiceAudio::updateDbValue(const std::string &path, const audio::Setting &setting, const uint32_t &value)
{
    if (path.empty()) {
        return;
    }

    auto query = std::make_unique<db::query::settings::UpdateQuery>(
        SettingsRecord_v2({{.ID = DB_ID_NONE}, .path = path, .value = std::to_string(value)}));

    DBServiceAPI::GetQuery(this, db::Interface::Name::Settings_v2, std::move(query));
}

void ServiceAudio::updateDbValue(const audio::Operation *currentOperation,
                                 const audio::Setting &setting,
                                 const uint32_t &value)
{
    if (currentOperation == nullptr) {
        return;
    }
    const auto *currentProfile = currentOperation->GetProfile();
    if (currentProfile == nullptr) {
        return;
    }

    auto dbPath = audio::str(currentProfile->GetType(), setting, currentOperation->GetPlaybackType());
    updateDbValue(dbPath, setting, value);
}

void ServiceAudio::setSetting(const audio::Setting &setting,
                              const uint32_t value,
                              const audio::Profile::Type &profileType,
                              const audio::PlaybackType &playbackType)
{
    if (profileType == Profile::Type::Idle) {
        setCurrentSetting(setting, value);
    }
    const auto path = audio::str(profileType, audio::Setting::Volume, playbackType);
    updateDbValue(path, audio::Setting::Volume, value);
}

uint32_t ServiceAudio::getSetting(const audio::Setting &setting,
                                  const audio::Profile::Type &profileType,
                                  const audio::PlaybackType &playbackType)
{
    if (profileType == audio::Profile::Type::Idle) {
        return getCurrentSetting(setting).value_or(0);
    }
    const auto path = audio::str(profileType, setting, playbackType);
    return fetchAudioSettingFromDb<audio::Volume>(path, 0);
}

std::optional<uint32_t> ServiceAudio::getCurrentSetting(const audio::Setting &setting)
{
    const auto activeInput = audioMux.GetActiveInput();

    if (!activeInput.has_value()) {
        const auto idleInput = audioMux.GetIdleInput();
        if (idleInput.has_value() && setting == audio::Setting::Volume) {
            const auto path = audio::str(
                audio::PlaybackType::CallRingtone, audio::Setting::Volume, (*idleInput)->audio.GetHeadphonesInserted());
            return fetchAudioSettingFromDb<audio::Volume>(path, 0);
        }
        return {};
    }

    const auto currentOperation = (*activeInput)->audio.GetCurrentOperation();
    const auto path =
        audio::str(currentOperation->GetProfile()->GetType(), setting, currentOperation->GetPlaybackType());
    return (setting == audio::Setting::Volume) ? fetchAudioSettingFromDb<audio::Volume>(path, 0)
                                               : fetchAudioSettingFromDb<audio::Gain>(path, 0);
}

void ServiceAudio::setCurrentSetting(const audio::Setting &setting, const uint32_t &value)
{
    const auto activeInput = audioMux.GetActiveInput();

    if (!activeInput.has_value()) {
        const auto idleInput = audioMux.GetIdleInput();
        if (idleInput.has_value() && setting == audio::Setting::Volume) {
            const auto path = audio::str(
                audio::PlaybackType::CallRingtone, audio::Setting::Volume, (*idleInput)->audio.GetHeadphonesInserted());
            addOrIgnoreEntry(path, audio::playbackDefaults::defaultLoudspeakerVolume);
            const auto valueToSet = std::clamp(value, minVolume, maxVolume);
            updateDbValue(path, Setting::Volume, valueToSet);
        }
        return;
    }

    auto &audio                 = (*activeInput)->audio;
    const auto currentOperation = audio.GetCurrentOperation();
    switch (setting) {
    case audio::Setting::Volume: {
        const auto valueToSet = std::clamp(value, minVolume, maxVolume);
        audio.SetOutputVolume(valueToSet);
        updateDbValue(currentOperation, audio::Setting::Volume, valueToSet);
    } break;
    case audio::Setting::Gain: {
        const auto valueToSet = std::clamp(value, minGain, maxGain);
        audio.SetInputGain(valueToSet);
        updateDbValue(currentOperation, audio::Setting::Gain, valueToSet);
    } break;
    }
}
