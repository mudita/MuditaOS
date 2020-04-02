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
    RetCode PlaybackStart(sys::Service *serv, const std::string &fileName)
    {
        std::shared_ptr<AudioRequestMessage> msg =
            std::make_shared<AudioRequestMessage>(MessageType::AudioPlaybackStart);
        msg->fileName = fileName;

        auto ret = sys::Bus::SendUnicast(msg, ServiceAudio::serviceName, serv, 5000);
        return reinterpret_cast<AudioResponseMessage *>(ret.second.get())->retCode;
    }

    RetCode RecordingStart(sys::Service *serv, const std::string &fileName)
    {
        std::shared_ptr<AudioRequestMessage> msg =
            std::make_shared<AudioRequestMessage>(MessageType::AudioRecorderStart);
        msg->fileName = fileName;

        auto ret = sys::Bus::SendUnicast(msg, ServiceAudio::serviceName, serv, 5000);
        return reinterpret_cast<AudioResponseMessage *>(ret.second.get())->retCode;
    }

    audio::RetCode RoutingStart(sys::Service *serv)
    {
        std::shared_ptr<AudioRequestMessage> msg =
            std::make_shared<AudioRequestMessage>(MessageType::AudioRoutingStart);

        auto ret = sys::Bus::SendUnicast(msg, ServiceAudio::serviceName, serv, 5000);
        return reinterpret_cast<AudioResponseMessage *>(ret.second.get())->retCode;
    }

    audio::RetCode RoutingRecordCtrl(sys::Service *serv, bool enable)
    {
        std::shared_ptr<AudioRequestMessage> msg =
            std::make_shared<AudioRequestMessage>(MessageType::AudioRoutingRecordCtrl);
        msg->enable = enable;
        auto ret    = sys::Bus::SendUnicast(msg, ServiceAudio::serviceName, serv, 5000);
        return reinterpret_cast<AudioResponseMessage *>(ret.second.get())->retCode;
    }

    audio::RetCode RoutingMute(sys::Service *serv, bool enable)
    {
        std::shared_ptr<AudioRequestMessage> msg = std::make_shared<AudioRequestMessage>(MessageType::AudioRoutingMute);
        msg->enable                              = enable;
        auto ret                                 = sys::Bus::SendUnicast(msg, ServiceAudio::serviceName, serv, 5000);
        return reinterpret_cast<AudioResponseMessage *>(ret.second.get())->retCode;
    }

    audio::RetCode RoutingSpeakerPhone(sys::Service *serv, bool enable)
    {
        std::shared_ptr<AudioRequestMessage> msg =
            std::make_shared<AudioRequestMessage>(MessageType::AudioRoutingSpeakerhone);
        msg->enable = enable;
        auto ret    = sys::Bus::SendUnicast(msg, ServiceAudio::serviceName, serv, 5000);
        return reinterpret_cast<AudioResponseMessage *>(ret.second.get())->retCode;
    }

    RetCode Stop(sys::Service *serv)
    {
        std::shared_ptr<AudioRequestMessage> msg = std::make_shared<AudioRequestMessage>(MessageType::AudioStop);

        auto ret = sys::Bus::SendUnicast(msg, ServiceAudio::serviceName, serv, 5000);
        return reinterpret_cast<AudioResponseMessage *>(ret.second.get())->retCode;
    }

    RetCode Pause(sys::Service *serv)
    {
        std::shared_ptr<AudioRequestMessage> msg = std::make_shared<AudioRequestMessage>(MessageType::AudioPause);

        auto ret = sys::Bus::SendUnicast(msg, ServiceAudio::serviceName, serv, 5000);
        return reinterpret_cast<AudioResponseMessage *>(ret.second.get())->retCode;
    }

    RetCode Resume(sys::Service *serv)
    {
        std::shared_ptr<AudioRequestMessage> msg = std::make_shared<AudioRequestMessage>(MessageType::AudioResume);

        auto ret = sys::Bus::SendUnicast(msg, ServiceAudio::serviceName, serv, 5000);
        return reinterpret_cast<AudioResponseMessage *>(ret.second.get())->retCode;
    }

    RetCode SetInputGain(sys::Service *serv, const Gain gain)
    {
        std::shared_ptr<AudioRequestMessage> msg =
            std::make_shared<AudioRequestMessage>(MessageType::AudioSetInputGain);
        msg->val = gain;

        auto ret = sys::Bus::SendUnicast(msg, ServiceAudio::serviceName, serv, 5000);
        return reinterpret_cast<AudioResponseMessage *>(ret.second.get())->retCode;
    }

    RetCode GetInputGain(sys::Service *serv, Gain &gain)
    {
        std::shared_ptr<AudioRequestMessage> msg =
            std::make_shared<AudioRequestMessage>(MessageType::AudioGetInputGain);

        auto ret  = sys::Bus::SendUnicast(msg, ServiceAudio::serviceName, serv, 5000);
        auto resp = dynamic_cast<AudioResponseMessage *>(ret.second.get());
        if (resp != nullptr) {
            gain = resp->val;
            return resp->retCode;
        }
        gain = 0;
        return RetCode::Failed;
    }

    RetCode SetOutputVolume(sys::Service *serv, const Volume vol)
    {
        std::shared_ptr<AudioRequestMessage> msg =
            std::make_shared<AudioRequestMessage>(MessageType::AudioSetOutputVolume);
        msg->val = vol;

        auto ret = sys::Bus::SendUnicast(msg, ServiceAudio::serviceName, serv, 5000);
        return reinterpret_cast<AudioResponseMessage *>(ret.second.get())->retCode;
    }

    RetCode GetOutputVolume(sys::Service *serv, Volume &vol)
    {
        std::shared_ptr<AudioRequestMessage> msg =
            std::make_shared<AudioRequestMessage>(MessageType::AudioGetOutputVolume);

        auto ret  = sys::Bus::SendUnicast(msg, ServiceAudio::serviceName, serv, 5000);
        auto resp = dynamic_cast<AudioResponseMessage *>(ret.second.get());
        if (resp != nullptr) {
            vol = resp->val;
            return resp->retCode;
        }
        vol = 0;
        return RetCode::Failed;
    }

    std::optional<Tags> GetFileTags(sys::Service *serv, const std::string &fileName)
    {
        std::shared_ptr<AudioRequestMessage> msg = std::make_shared<AudioRequestMessage>(MessageType::AudioGetFileTags);
        msg->fileName                            = fileName;

        auto ret = sys::Bus::SendUnicast(msg, ServiceAudio::serviceName, serv, 5000);
        if (reinterpret_cast<AudioResponseMessage *>(ret.second.get())->retCode != RetCode::Success) {
            return {};
        }
        else {
            return reinterpret_cast<AudioResponseMessage *>(ret.second.get())->tags;
        }
    }
} // namespace AudioServiceAPI
