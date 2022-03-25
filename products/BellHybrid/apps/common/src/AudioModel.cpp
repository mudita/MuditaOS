// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "models/AudioModel.hpp"

#include <ApplicationCommon.hpp>
#include <audio/AudioMessage.hpp>

namespace
{
    constexpr audio::PlaybackType convertPlaybackType(app::AbstractAudioModel::PlaybackType type)
    {
        using Type = app::AbstractAudioModel::PlaybackType;
        switch (type) {
        case Type::Multimedia:
            return audio::PlaybackType::Multimedia;
        case Type::Alarm:
            return audio::PlaybackType::Alarm;
        case Type::Snooze:
            return audio::PlaybackType::Snooze;
        case Type::PreWakeup:
            return audio::PlaybackType::PreWakeUp;
        case Type::Bedtime:
            return audio::PlaybackType::Bedtime;
        case Type::Meditation:
            return audio::PlaybackType::Meditation;
        default:
            return audio::PlaybackType::Alarm;
        }
    }

    void reportError(const char *prefix, audio::RetCode code)
    {
        if (code != audio::RetCode::Success) {
            LOG_ERROR("%s request error: %s", prefix, magic_enum::enum_name(code).data());
        }
    }

    auto SendAudioRequest(sys::Service *serv, std::shared_ptr<service::AudioMessage> msg)
    {
        auto msgType = static_cast<int>(msg->type);
        auto ret     = serv->bus.sendUnicastSync(msg, service::audioServiceName, sys::BusProxy::defaultTimeout);
        if (ret.first == sys::ReturnCodes::Success) {
            if (auto resp = std::dynamic_pointer_cast<service::AudioResponseMessage>(ret.second)) {
                return resp;
            }
            LOG_ERROR("msgType %d - not AudioResponseMessage", msgType);
            return std::make_shared<service::AudioResponseMessage>(audio::RetCode::Failed);
        }
        LOG_ERROR("Command %d Failed with %d error", msgType, static_cast<int>(ret.first));
        return std::make_shared<service::AudioResponseMessage>(audio::RetCode::Failed);
    }

} // namespace

namespace app
{

    AudioModel::AudioModel(ApplicationCommon *app) : app::AsyncCallbackReceiver{app}, app{app}
    {
        app->connect(typeid(service::AudioEOFNotification), [&](sys::Message *msg) -> sys::MessagePointer {
            playbackFinishedFlag = true;
            if (playbackFinishedCallback) {
                playbackFinishedCallback();
            }

            return sys::msgHandled();
        });
    }

    AudioModel::~AudioModel()
    {
        app->disconnect(typeid(service::AudioEOFNotification));
    }

    void AudioModel::play(const std::string &filePath, PlaybackType type, OnStateChangeCallback &&callback)
    {
        playbackFinishedFlag = false;
        auto msg  = std::make_unique<service::AudioStartPlaybackRequest>(filePath, convertPlaybackType(type));
        auto task = app::AsyncRequest::createFromMessage(std::move(msg), service::audioServiceName);
        auto cb              = [_callback = callback, this](auto response) {
            auto result     = dynamic_cast<service::AudioStartPlaybackResponse *>(response);
            if (result == nullptr) {
                return false;
            }
            lastPlayedToken = result->token;

            if (_callback) {
                _callback(result->retCode);
            }
            reportError("play", result->retCode);

            return true;
        };
        task->execute(app, this, std::move(cb));
    }

    void AudioModel::stopAny(OnStateChangeCallback &&callback)
    {
        stop(audio::Token(), std::move(callback));
    }

    void AudioModel::stop(audio::Token token, OnStateChangeCallback &&callback)
    {
        auto msg  = std::make_unique<service::AudioStopRequest>(token);
        auto task = app::AsyncRequest::createFromMessage(std::move(msg), service::audioServiceName);
        auto cb   = [_callback = callback](auto response) {
            auto result = dynamic_cast<service::AudioStopResponse *>(response);
            if (result == nullptr) {
                return false;
            }
            if (_callback) {
                _callback(result->retCode);
            }
            reportError("stop", result->retCode);
            return true;
        };
        task->execute(app, this, std::move(cb));
    }

    void AudioModel::stopPlayedByThis(OnStateChangeCallback &&callback)
    {
        stop(lastPlayedToken, std::move(callback));
    }

    void AudioModel::setVolume(AbstractAudioModel::Volume volume,
                               PlaybackType playbackType,
                               OnStateChangeCallback &&callback)
    {
        auto msg = std::make_unique<service::AudioSetVolume>(convertPlaybackType(playbackType), std::to_string(volume));
        auto task = app::AsyncRequest::createFromMessage(std::move(msg), service::audioServiceName);
        auto cb   = [_callback = callback](auto response) {
            auto result = dynamic_cast<service::AudioResponseMessage *>(response);
            if (result == nullptr) {
                return false;
            }
            if (_callback) {
                _callback(result->retCode);
            }
            reportError("setVolume", result->retCode);
            return true;
        };
        task->execute(app, this, std::move(cb));
    }
    void AudioModel::pause(OnStateChangeCallback &&callback)
    {
        auto msg  = std::make_unique<service::AudioPauseRequest>();
        auto task = app::AsyncRequest::createFromMessage(std::move(msg), service::audioServiceName);
        auto cb   = [_callback = callback](auto response) {
            auto result = dynamic_cast<service::AudioResponseMessage *>(response);
            if (result == nullptr) {
                return false;
            }
            if (_callback) {
                _callback(result->retCode);
            }

            reportError("pause", result->retCode);
            return true;
        };
        task->execute(app, this, std::move(cb));
    }
    void AudioModel::resume(OnStateChangeCallback &&callback)
    {
        auto msg  = std::make_unique<service::AudioResumeRequest>();
        auto task = app::AsyncRequest::createFromMessage(std::move(msg), service::audioServiceName);
        auto cb   = [_callback = callback](auto response) {
            auto result = dynamic_cast<service::AudioResponseMessage *>(response);
            if (result == nullptr) {
                return false;
            }
            if (_callback) {
                _callback(result->retCode);
            }
            reportError("resume", result->retCode);
            return true;
        };
        task->execute(app, this, std::move(cb));
    }
    void AudioModel::getVolume(AbstractAudioModel::PlaybackType playbackType,
                               AbstractAudioModel::OnGetValueCallback &&callback)
    {
        auto msg  = std::make_unique<service::AudioGetVolume>(convertPlaybackType(playbackType));
        auto task = app::AsyncRequest::createFromMessage(std::move(msg), service::audioServiceName);
        auto cb   = [_callback = callback](auto response) {
            auto result = dynamic_cast<service::AudioResponseMessage *>(response);
            if (result == nullptr) {
                return false;
            }
            if (_callback) {
                _callback(result->retCode, utils::getNumericValue<AbstractAudioModel::Volume>(result->val));
            }

            reportError("getVolume", result->retCode);
            return true;
        };
        task->execute(app, this, std::move(cb));
    }
    std::optional<AbstractAudioModel::Volume> AudioModel::getVolume(AbstractAudioModel::PlaybackType playbackType)
    {
        auto msg       = std::make_shared<service::AudioGetVolume>(convertPlaybackType(playbackType));
        const auto ret = SendAudioRequest(app, msg);
        if (ret) {
            return utils::getNumericValue<audio::Volume>(ret->val);
        }

        return {};
    }

    void AudioModel::setPlaybackFinishedCb(OnPlaybackFinishedCallback &&callback)
    {
        playbackFinishedCallback = callback;
    }

    bool AudioModel::hasPlaybackFinished()
    {
        return playbackFinishedFlag;
    }

} // namespace app
