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
      audio(
          [this](AudioEvents event) -> int32_t {
              switch (event) {
              case AudioEvents::EndOfFile: {
                  auto msg = std::make_shared<AudioNotificationMessage>(
                      static_cast<AudioNotificationMessage::Type>(AudioNotificationMessage::Type::EndOfFile));
                  sys::Bus::SendMulticast(msg, sys::BusChannels::ServiceAudioNotifications, this);
              } break;
              case AudioEvents::FileSystemNoSpace:
                  break;
              }
              return 0;
          },
          [this](const std::string &path, const uint32_t &defaultValue) -> uint32_t {
              this->addOrIgnoreEntry(path, defaultValue);
              return this->fetchAudioSettingFromDb(path, defaultValue);
          })
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

sys::ReturnCodes ServiceAudio::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
{
    LOG_FATAL("[ServiceAudio] PowerModeHandler: %s", c_str(mode));
    return sys::ReturnCodes::Success;
}

void ServiceAudio::TickHandler(uint32_t id)
{}

sys::Message_t ServiceAudio::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
{
    std::shared_ptr<sys::ResponseMessage> responseMsg;

    auto msgType = static_cast<int>(msgl->messageType);
    LOG_DEBUG("msgType %d", msgType);

    if (auto *msg = dynamic_cast<AudioNotificationMessage *>(msgl)) {
        switch (msg->type) {

        case AudioNotificationMessage::Type::EndOfFile: {
            std::shared_ptr<AudioRequestMessage> msg = std::make_shared<AudioRequestMessage>(MessageType::AudioStop);
            sys::Bus::SendUnicast(msg, ServiceAudio::serviceName, this);
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
        const auto currentOperation = audio.GetCurrentOperation();
        const auto stopVec          = msg->stopVec;
        if (currentOperation &&
            (std::find(stopVec.begin(), stopVec.end(), currentOperation->GetPlaybackType()) == stopVec.end())) {
            responseMsg = std::make_shared<AudioResponseMessage>(RetCode::Success);
        }
        else {
            responseMsg = std::make_shared<AudioResponseMessage>(audio.Stop());

            // Send notification that audio file was stopped
            auto retMsg = std::make_shared<AudioNotificationMessage>(
                static_cast<AudioNotificationMessage::Type>(AudioNotificationMessage::Type::Stop));
            sys::Bus::SendMulticast(retMsg, sys::BusChannels::ServiceAudioNotifications, this);
        }
    }
    else if (auto *msg = dynamic_cast<AudioRequestMessage *>(msgl)) {
        switch (msg->type) {
        case MessageType::AudioPlaybackStart: {
            responseMsg = std::make_shared<AudioResponseMessage>(
                audio.Start(Operation::Type::Playback, msg->fileName.c_str(), msg->playbackType));
        } break;

        case MessageType::AudioRecorderStart: {
            responseMsg =
                std::make_shared<AudioResponseMessage>(audio.Start(Operation::Type::Recorder, msg->fileName.c_str()));
        } break;

        case MessageType::AudioRoutingStart: {
            LOG_DEBUG("AudioRoutingStart");
            responseMsg = std::make_shared<AudioResponseMessage>(audio.Start(Operation::Type::Router));
        } break;

        case MessageType::AudioPause: {
            responseMsg = std::make_shared<AudioResponseMessage>(audio.Pause());
        } break;

        case MessageType::AudioResume: {
            responseMsg = std::make_shared<AudioResponseMessage>(audio.Resume());
        } break;

        case MessageType::AudioGetFileTags: {
            auto tag = audio.GetFileTags(msg->fileName.c_str());
            if (tag) {
                responseMsg = std::make_shared<AudioResponseMessage>(RetCode::Success, tag.value());
            }
            else {
                responseMsg = std::make_shared<AudioResponseMessage>(RetCode::FileDoesntExist);
            }
        } break;

        case MessageType::AudioRoutingRecordCtrl: {
            responseMsg = std::make_shared<AudioResponseMessage>(audio.SendEvent(
                msg->enable ? Operation::Event::StartCallRecording : Operation::Event::StopCallRecording));
        } break;

        case MessageType::AudioRoutingMute: {
            responseMsg = std::make_shared<AudioResponseMessage>(
                audio.SendEvent(msg->enable ? Operation::Event::CallMute : Operation::Event::CallUnmute));
        } break;

        case MessageType::AudioRoutingSpeakerhone: {
            responseMsg = std::make_shared<AudioResponseMessage>(audio.SendEvent(
                msg->enable ? Operation::Event::CallSpeakerphoneOn : Operation::Event::CallSpeakerphoneOff));
        } break;

        case MessageType::AudioRoutingHeadset: {
            responseMsg = std::make_shared<AudioResponseMessage>(
                audio.SendEvent(msg->enable ? Operation::Event::HeadphonesPlugin : Operation::Event::HeadphonesUnplug));
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

    return responseMsg;
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
    const auto currentOperation   = audio.GetCurrentOperation();
    const auto noOngoingOperation = [&currentOperation]() -> bool {
        return currentOperation == nullptr || currentOperation->GetState() == audio::Operation::State::Idle;
    };

    if (noOngoingOperation()) {
        if (setting == audio::Setting::Volume) {
            const auto path =
                audio::str(audio::PlaybackType::CallRingtone, audio::Setting::Volume, audio.GetHeadphonesInserted());
            return fetchAudioSettingFromDb<audio::Volume>(path, 0);
        }
        return {};
    }

    const auto path =
        audio::str(currentOperation->GetProfile()->GetType(), setting, currentOperation->GetPlaybackType());
    return (setting == audio::Setting::Volume) ? fetchAudioSettingFromDb<audio::Volume>(path, 0)
                                               : fetchAudioSettingFromDb<audio::Gain>(path, 0);
}

void ServiceAudio::setCurrentSetting(const audio::Setting &setting, const uint32_t &value)
{
    const auto currentOperation = audio.GetCurrentOperation();
    const auto valueToSet       = [&value, &setting]() -> std::optional<uint32_t> {
        switch (setting) {
        case audio::Setting::Volume: {
            return std::clamp(value, minVolume, maxVolume);
        }
        case audio::Setting::Gain: {
            return std::clamp(value, minGain, maxGain);
        }
        }
        return {};
    }();
    if (!valueToSet.has_value()) {
        return;
    }
    auto idleOperation = dynamic_cast<const IdleOperation *>(currentOperation);

    const auto noOngoingOperation = [&currentOperation, &idleOperation]() -> bool {
        return currentOperation == nullptr || idleOperation != nullptr ||
               currentOperation->GetState() == audio::Operation::State::Idle;
    };

    if (noOngoingOperation()) {
        if (setting == audio::Setting::Volume) {
            const auto path =
                audio::str(audio::PlaybackType::CallRingtone, audio::Setting::Volume, audio.GetHeadphonesInserted());
            addOrIgnoreEntry(path, audio::playbackDefaults::defaultLoudspeakerVolume);
            updateDbValue(path, Setting::Volume, valueToSet.value());
        }
    }
    else {
        switch (setting) {
        case audio::Setting::Volume: {
            audio.SetOutputVolume(valueToSet.value());
            updateDbValue(currentOperation, audio::Setting::Volume, valueToSet.value());
        } break;
        case audio::Setting::Gain: {
            audio.SetInputGain(valueToSet.value());
            updateDbValue(currentOperation, audio::Setting::Gain, valueToSet.value());
        } break;
        }
    }
}
