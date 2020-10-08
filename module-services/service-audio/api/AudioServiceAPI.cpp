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
        auto SendAudioRequest(sys::Service *serv, std::shared_ptr<AudioMessage> msg)
        {
            auto msgType = static_cast<int>(msg->type);
            LOG_DEBUG("Msg type %d", msgType);
            auto ret = sys::Bus::SendUnicast(msg, ServiceAudio::serviceName, serv, sys::defaultCmdTimeout);
            if (ret.first == sys::ReturnCodes::Success) {
                if (auto resp = std::dynamic_pointer_cast<AudioResponseMessage>(ret.second)) {
                    LOG_DEBUG("Msg type %d done", msgType);
                    return resp;
                }
                LOG_ERROR("msgType %d - not AudioResponseMessage", msgType);
                return std::make_shared<AudioResponseMessage>(audio::RetCode::Failed);
            }
            LOG_ERROR("Command %d Failed with %d error", msgType, static_cast<int>(ret.first));
            return std::make_shared<AudioResponseMessage>(audio::RetCode::Failed);
        }
    } // namespace

    bool PlaybackStart(sys::Service *serv, const audio::PlaybackType &playbackType, const std::string &fileName)
    {
        auto msg = std::make_shared<AudioStartPlaybackRequest>(fileName, playbackType);
        return sys::Bus::SendUnicast(msg, ServiceAudio::serviceName, serv);
    }

    bool RecordingStart(sys::Service *serv, const std::string &fileName)
    {
        auto msg = std::make_shared<AudioStartRecorderRequest>(fileName);
        return sys::Bus::SendUnicast(msg, ServiceAudio::serviceName, serv);
    }

    bool RoutingStart(sys::Service *serv)
    {
        auto msg = std::make_shared<AudioStartRoutingRequest>();
        return sys::Bus::SendUnicast(msg, ServiceAudio::serviceName, serv);
    }

    bool RoutingMute(sys::Service *serv, bool enable)
    {
        auto msg = std::make_shared<AudioRoutingControlRequest>(AudioRoutingControlRequest::ControlType::Mute, enable);
        return sys::Bus::SendUnicast(msg, ServiceAudio::serviceName, serv);
    }

    bool RoutingSpeakerPhone(sys::Service *serv, bool enable)
    {
        auto msg = std::make_shared<AudioRoutingControlRequest>(
            AudioRoutingControlRequest::ControlType::SwitchSpeakerphone, enable);
        return sys::Bus::SendUnicast(msg, ServiceAudio::serviceName, serv);
    }

    bool RoutingHeadset(sys::Service *serv, bool enable)
    {
        auto msg = std::make_shared<AudioRoutingControlRequest>(
            AudioRoutingControlRequest::ControlType::SwitchHeadphones, enable);
        return sys::Bus::SendUnicast(msg, ServiceAudio::serviceName, serv);
    }

    bool Stop(sys::Service *serv, const std::vector<audio::PlaybackType> &stopVec)
    {
        if (stopVec.empty()) {
            return true;
        }
        auto msg = std::make_shared<AudioStopRequest>(stopVec);
        return sys::Bus::SendUnicast(msg, ServiceAudio::serviceName, serv);
    }

    bool Stop(sys::Service *serv, const audio::Token &token)
    {
        auto msg = std::make_shared<AudioStopRequest>(token);
        return sys::Bus::SendUnicast(msg, ServiceAudio::serviceName, serv);
    }

    bool StopAll(sys::Service *serv)
    {
        auto msg = std::make_shared<AudioStopRequest>();
        return sys::Bus::SendUnicast(msg, ServiceAudio::serviceName, serv);
    }

    bool Pause(sys::Service *serv, const audio::Token &token)
    {
        auto msg = std::make_shared<AudioPauseRequest>(token);
        return sys::Bus::SendUnicast(msg, ServiceAudio::serviceName, serv);
    }

    bool Resume(sys::Service *serv, const audio::Token &token)
    {
        auto msg = std::make_shared<AudioResumeRequest>(token);
        return sys::Bus::SendUnicast(msg, ServiceAudio::serviceName, serv);
    }

    template <typename T>
    audio::RetCode GetSetting(sys::Service *serv,
                              const audio::Setting &setting,
                              T &value,
                              const audio::PlaybackType &playbackType,
                              const audio::Profile::Type &profileType)
    {
        auto msg  = std::make_shared<AudioGetSetting>(profileType, playbackType, setting);
        auto resp = SendAudioRequest(serv, msg);
        if (resp->retCode == RetCode::Success) {
            value = resp->val;
        }

        return resp->retCode;
    }

    template audio::RetCode GetSetting<uint32_t>(sys::Service *serv,
                                                 const audio::Setting &setting,
                                                 uint32_t &value,
                                                 const audio::PlaybackType &playbackType,
                                                 const audio::Profile::Type &profileType);

    template audio::RetCode GetSetting<bool>(sys::Service *serv,
                                             const audio::Setting &setting,
                                             bool &value,
                                             const audio::PlaybackType &playbackType,
                                             const audio::Profile::Type &profileType);

    template <typename T>
    audio::RetCode SetSetting(sys::Service *serv,
                              const audio::Setting &setting,
                              const T value,
                              const audio::PlaybackType &playbackType,
                              const audio::Profile::Type &profileType)
    {
        auto msg = std::make_shared<AudioSetSetting>(profileType, playbackType, setting, std::to_string(value));

        return SendAudioRequest(serv, msg)->retCode;
    }

    template audio::RetCode SetSetting<uint32_t>(sys::Service *serv,
                                                 const audio::Setting &setting,
                                                 const uint32_t value,
                                                 const audio::PlaybackType &playbackType,
                                                 const audio::Profile::Type &profileType);

    template audio::RetCode SetSetting<bool>(sys::Service *serv,
                                             const audio::Setting &setting,
                                             const bool value,
                                             const audio::PlaybackType &playbackType,
                                             const audio::Profile::Type &profileType);

    std::optional<Tags> GetFileTags(sys::Service *serv, const std::string &fileName)
    {
        auto msg = std::make_shared<AudioGetFileTagsRequest>(fileName);

        auto resp = SendAudioRequest(serv, msg);
        if (resp->retCode == audio::RetCode::Success) {
            return resp->tags;
        }
        else {
            return std::nullopt;
        }
    }

    bool GetSettingAsync(sys::Service *serv,
                         const audio::Setting &setting,
                         const audio::Profile::Type &profileType,
                         const audio::PlaybackType &playbackType)
    {
        auto msg = std::make_shared<AudioGetSetting>(profileType, playbackType, setting);
        return sys::Bus::SendUnicast(msg, ServiceAudio::serviceName, serv);
    }

    bool KeyPressed(sys::Service *serv, const int step, const std::vector<audio::PlaybackType> &typesToMute)
    {
        audio::Volume volume;
        bool shouldPopup = false;
        if (step < 0) {
            AudioServiceAPI::Stop(serv, typesToMute);
        }
        auto ret = AudioServiceAPI::GetSetting(serv, audio::Setting::Volume, volume);
        if (ret == audio::RetCode::Success) {
            if ((static_cast<int>(volume) + step) >= 0) {
                AudioServiceAPI::SetSetting(serv, audio::Setting::Volume, volume + step);
                shouldPopup = true;
            }
        }

        auto msg = std::make_shared<AudioKeyPressed>(shouldPopup, volume);
        return sys::Bus::SendUnicast(msg, ServiceAudio::serviceName, serv);
    }
} // namespace AudioServiceAPI
