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

const char *ServiceAudio::serviceName = "ServiceAudio";

using namespace audio;

ServiceAudio::ServiceAudio()
    : sys::Service(serviceName, "", 4096 * 2, sys::ServicePriority::Idle), audio([this](AudioEvents event) -> int32_t {
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
    else if (auto *msg = dynamic_cast<AudioRequestMessage *>(msgl)) {
        switch (msg->type) {
        case MessageType::AudioPlaybackStart: {
            responseMsg = std::make_shared<AudioResponseMessage>(
                static_cast<RetCode>(audio.Start(Operation::Type::Playback, msg->fileName.c_str())));
        } break;

        case MessageType::AudioRecorderStart: {
            responseMsg = std::make_shared<AudioResponseMessage>(
                static_cast<RetCode>(audio.Start(Operation::Type::Recorder, msg->fileName.c_str())));
        } break;

        case MessageType::AudioRoutingStart: {
            LOG_DEBUG("AudioRoutingStart");
            responseMsg =
                std::make_shared<AudioResponseMessage>(static_cast<RetCode>(audio.Start(Operation::Type::Router)));
        } break;

        case MessageType::AudioStop: {
            responseMsg = std::make_shared<AudioResponseMessage>(static_cast<RetCode>(audio.Stop()));

            // Send notification that audio file was stopped
            auto msg = std::make_shared<AudioNotificationMessage>(
                static_cast<AudioNotificationMessage::Type>(AudioNotificationMessage::Type::Stop));
            sys::Bus::SendMulticast(msg, sys::BusChannels::ServiceAudioNotifications, this);
        } break;

        case MessageType::AudioPause: {
            responseMsg = std::make_shared<AudioResponseMessage>(static_cast<RetCode>(audio.Pause()));
        } break;

        case MessageType::AudioResume: {
            responseMsg = std::make_shared<AudioResponseMessage>(static_cast<RetCode>(audio.Resume()));
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
            responseMsg = std::make_shared<AudioResponseMessage>(static_cast<RetCode>(audio.SendEvent(
                msg->enable ? Operation::Event::StartCallRecording : Operation::Event::StopCallRecording)));
        } break;

        case MessageType::AudioRoutingMute: {
            responseMsg = std::make_shared<AudioResponseMessage>(static_cast<RetCode>(
                audio.SendEvent(msg->enable ? Operation::Event::CallMute : Operation::Event::CallUnmute)));
        } break;

        case MessageType::AudioRoutingSpeakerhone: {
            responseMsg = std::make_shared<AudioResponseMessage>(static_cast<RetCode>(audio.SendEvent(
                msg->enable ? Operation::Event::CallSpeakerphoneOn : Operation::Event::CallSpeakerphoneOff)));
        } break;

        case MessageType::AudioSetOutputVolume: {
            responseMsg = std::make_shared<AudioResponseMessage>(static_cast<RetCode>(audio.SetOutputVolume(msg->val)));
        } break;

        case MessageType::AudioSetInputGain: {
            AudioRequestMessage *msg = reinterpret_cast<AudioRequestMessage *>(msgl);
            responseMsg = std::make_shared<AudioResponseMessage>(static_cast<RetCode>(audio.SetInputGain(msg->val)));
        } break;

        case MessageType::AudioGetOutputVolume: {
            responseMsg = std::make_shared<AudioResponseMessage>(RetCode::Success, audio.GetOutputVolume());
        } break;

        case MessageType::AudioGetInputGain: {
            responseMsg = std::make_shared<AudioResponseMessage>(RetCode::Success, audio.GetInputGain());
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
