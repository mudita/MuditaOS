// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-audio/AudioServiceAPI.hpp"
#include "service-audio/ServiceAudio.hpp"
#include "service-audio/AudioMessage.hpp"

#include <Audio/decoder/Decoder.hpp>
#include <Service/Common.hpp>
#include <log/log.hpp>

#include <utility>

namespace sys
{
    class Service;
} // namespace sys

using namespace audio;
namespace AudioServiceAPI
{
    namespace
    {
        auto SendAudioRequest(sys::Service *serv, std::shared_ptr<AudioMessage> msg)
        {
            auto msgType = static_cast<int>(msg->type);
            LOG_DEBUG("Msg type %d", msgType);
            auto ret = serv->bus.sendUnicast(msg, service::name::audio, sys::BusProxy::defaultTimeout);
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
        return serv->bus.sendUnicast(msg, service::name::audio);
    }

    bool RecordingStart(sys::Service *serv, const std::string &fileName)
    {
        auto msg = std::make_shared<AudioStartRecorderRequest>(fileName);
        return serv->bus.sendUnicast(msg, service::name::audio);
    }

    bool RoutingStart(sys::Service *serv)
    {
        auto msg = std::make_shared<AudioStartRoutingRequest>();
        return serv->bus.sendUnicast(msg, service::name::audio);
    }

    bool Stop(sys::Service *serv, const std::vector<audio::PlaybackType> &stopVec)
    {
        if (stopVec.empty()) {
            return true;
        }
        auto msg = std::make_shared<AudioStopRequest>(stopVec);
        return serv->bus.sendUnicast(msg, service::name::audio);
    }

    bool Stop(sys::Service *serv, const audio::Token &token)
    {
        auto msg = std::make_shared<AudioStopRequest>(token);
        return serv->bus.sendUnicast(msg, service::name::audio);
    }

    bool StopAll(sys::Service *serv)
    {
        auto msg = std::make_shared<AudioStopRequest>();
        return serv->bus.sendUnicast(msg, service::name::audio);
    }

    bool Pause(sys::Service *serv, const audio::Token &token)
    {
        auto msg = std::make_shared<AudioPauseRequest>(token);
        return serv->bus.sendUnicast(msg, service::name::audio);
    }

    bool Resume(sys::Service *serv, const audio::Token &token)
    {
        auto msg = std::make_shared<AudioResumeRequest>(token);
        return serv->bus.sendUnicast(msg, service::name::audio);
    }

    bool SendEvent(sys::Service *serv, std::shared_ptr<audio::Event> evt)
    {
        auto msg = std::make_shared<AudioEventRequest>(std::move(evt));
        return serv->bus.sendUnicast(msg, service::name::audio);
    }

    bool SendEvent(sys::Service *serv, audio::EventType eType, audio::Event::DeviceState state)
    {
        auto msg = std::make_shared<AudioEventRequest>(eType, state);
        return serv->bus.sendUnicast(msg, service::name::audio);
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

    template audio::RetCode SetSetting<uint32_t>(
        sys::Service *, const Setting &, const uint32_t, const PlaybackType &, const Profile::Type &);

    template audio::RetCode SetSetting<bool>(
        sys::Service *, const Setting &, const bool, const PlaybackType &, const Profile::Type &);

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

    bool KeyPressed(sys::Service *serv, const int step)
    {
        auto msg = std::make_shared<AudioKeyPressedRequest>(step);
        return serv->bus.sendUnicast(msg, service::name::audio);
    }

} // namespace AudioServiceAPI
