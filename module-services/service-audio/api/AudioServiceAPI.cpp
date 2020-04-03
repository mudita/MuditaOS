/*
 *  @file AudioSerivceAPI.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 29.07.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#include "AudioServiceAPI.hpp"
#include "Service/Bus.hpp"
#include "../ServiceAudio.hpp"

using namespace audio;
namespace AudioServiceAPI
{
    namespace
    {
        auto SendAudioRequest(sys::Service *serv, std::shared_ptr<sys::Message> msg)
        {
            auto ret = sys::Bus::SendUnicast(msg, ServiceAudio::serviceName, serv, sys::defaultCmdTimeout);
            if (ret.first == sys::ReturnCodes::Success) {
                if (auto resp = std::dynamic_pointer_cast<AudioResponseMessage>(ret.second)) {
                    return resp;
                }
                LOG_ERROR("not AudioResponseMessage");
                return std::make_shared<AudioResponseMessage>(audio::RetCode::Failed);
            }
            LOG_ERROR("Command Failed with %d error", static_cast<int>(ret.first));
            return std::make_shared<AudioResponseMessage>(audio::RetCode::Failed);
        }
    } // namespace

    RetCode PlaybackStart(sys::Service *serv, const std::string &fileName)
    {
        std::shared_ptr<AudioRequestMessage> msg =
            std::make_shared<AudioRequestMessage>(MessageType::AudioPlaybackStart);
        msg->fileName = fileName;

        return SendAudioRequest(serv, msg)->retCode;
    }

    RetCode RecordingStart(sys::Service *serv, const std::string &fileName)
    {
        std::shared_ptr<AudioRequestMessage> msg =
            std::make_shared<AudioRequestMessage>(MessageType::AudioRecorderStart);
        msg->fileName = fileName;

        return SendAudioRequest(serv, msg)->retCode;
    }

    audio::RetCode RoutingStart(sys::Service *serv)
    {
        std::shared_ptr<AudioRequestMessage> msg =
            std::make_shared<AudioRequestMessage>(MessageType::AudioRoutingStart);

        return SendAudioRequest(serv, msg)->retCode;
    }

    audio::RetCode RoutingRecordCtrl(sys::Service *serv, bool enable)
    {
        std::shared_ptr<AudioRequestMessage> msg =
            std::make_shared<AudioRequestMessage>(MessageType::AudioRoutingRecordCtrl);
        msg->enable = enable;

        return SendAudioRequest(serv, msg)->retCode;
    }

    audio::RetCode RoutingMute(sys::Service *serv, bool enable)
    {
        std::shared_ptr<AudioRequestMessage> msg = std::make_shared<AudioRequestMessage>(MessageType::AudioRoutingMute);
        msg->enable                              = enable;

        return SendAudioRequest(serv, msg)->retCode;
    }

    audio::RetCode RoutingSpeakerPhone(sys::Service *serv, bool enable)
    {
        std::shared_ptr<AudioRequestMessage> msg =
            std::make_shared<AudioRequestMessage>(MessageType::AudioRoutingSpeakerhone);
        msg->enable = enable;

        return SendAudioRequest(serv, msg)->retCode;
    }

    RetCode Stop(sys::Service *serv)
    {
        std::shared_ptr<AudioRequestMessage> msg = std::make_shared<AudioRequestMessage>(MessageType::AudioStop);

        return SendAudioRequest(serv, msg)->retCode;
    }

    RetCode Pause(sys::Service *serv)
    {
        std::shared_ptr<AudioRequestMessage> msg = std::make_shared<AudioRequestMessage>(MessageType::AudioPause);

        return SendAudioRequest(serv, msg)->retCode;
    }

    RetCode Resume(sys::Service *serv)
    {
        std::shared_ptr<AudioRequestMessage> msg = std::make_shared<AudioRequestMessage>(MessageType::AudioResume);

        return SendAudioRequest(serv, msg)->retCode;
    }

    RetCode SetInputGain(sys::Service *serv, const Gain gain)
    {
        std::shared_ptr<AudioRequestMessage> msg =
            std::make_shared<AudioRequestMessage>(MessageType::AudioSetInputGain);
        msg->val = gain;

        return SendAudioRequest(serv, msg)->retCode;
    }

    RetCode GetInputGain(sys::Service *serv, Gain &gain)
    {
        std::shared_ptr<AudioRequestMessage> msg =
            std::make_shared<AudioRequestMessage>(MessageType::AudioGetInputGain);

        auto resp = SendAudioRequest(serv, msg);
        if (resp->retCode == audio::RetCode::Success) {
            gain = resp->val;
        }
        gain = 0;
        return resp->retCode;
    }

    RetCode SetOutputVolume(sys::Service *serv, const Volume vol)
    {
        std::shared_ptr<AudioRequestMessage> msg =
            std::make_shared<AudioRequestMessage>(MessageType::AudioSetOutputVolume);
        msg->val = vol;

        return SendAudioRequest(serv, msg)->retCode;
    }

    RetCode GetOutputVolume(sys::Service *serv, Volume &vol)
    {
        std::shared_ptr<AudioRequestMessage> msg =
            std::make_shared<AudioRequestMessage>(MessageType::AudioGetOutputVolume);

        auto resp = SendAudioRequest(serv, msg);
        if (resp->retCode == audio::RetCode::Success) {
            vol = resp->val;
        }
        vol = 0;
        return resp->retCode;
    }

    std::optional<Tags> GetFileTags(sys::Service *serv, const std::string &fileName)
    {
        std::shared_ptr<AudioRequestMessage> msg = std::make_shared<AudioRequestMessage>(MessageType::AudioGetFileTags);
        msg->fileName                            = fileName;

        auto resp = SendAudioRequest(serv, msg);
        if (resp->retCode == audio::RetCode::Success) {
            return resp->tags;
        }
        else {
            return {};
        }
    }
} // namespace AudioServiceAPI
