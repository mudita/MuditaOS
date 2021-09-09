// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AudioOperations.hpp"

#include <log.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <service-audio/AudioServiceName.hpp>
#include <time/ScopedTime.hpp>
#include <purefs/filesystem_paths.hpp>
#include <service-audio/AudioMessage.hpp>

#include <filesystem>

namespace app
{
    AsyncAudioOperations::AsyncAudioOperations(ApplicationCommon *application)
        : app::AsyncCallbackReceiver{application}, application(application)
    {}

    bool AsyncAudioOperations::play(const std::string &filePath, const OnPlayCallback &callback)
    {
        auto msg  = std::make_unique<AudioStartPlaybackRequest>(filePath, audio::PlaybackType::Multimedia);
        auto task = app::AsyncRequest::createFromMessage(std::move(msg), service::name::audio);
        auto cb   = [callback](auto response) {
            auto result = dynamic_cast<AudioStartPlaybackResponse *>(response);
            if (result == nullptr) {
                return false;
            }
            if (callback) {
                callback(result->retCode, result->token);
            }
            return true;
        };
        task->execute(application, this, cb);
        return true;
    }

    bool AsyncAudioOperations::pause(const audio::Token &token, const OnPauseCallback &callback)
    {
        auto msg  = std::make_unique<AudioPauseRequest>(token);
        auto task = app::AsyncRequest::createFromMessage(std::move(msg), service::name::audio);
        auto cb   = [callback](auto response) {
            auto result = dynamic_cast<AudioPauseResponse *>(response);
            if (result == nullptr) {
                return false;
            }
            if (callback) {
                callback(result->retCode, result->token);
            }
            return true;
        };
        task->execute(application, this, cb);
        return true;
    }
    bool AsyncAudioOperations::resume(const audio::Token &token, const OnResumeCallback &callback)
    {
        auto msg  = std::make_unique<AudioResumeRequest>(token);
        auto task = app::AsyncRequest::createFromMessage(std::move(msg), service::name::audio);
        auto cb   = [callback](auto response) {
            auto result = dynamic_cast<AudioResumeResponse *>(response);
            if (result == nullptr) {
                return false;
            }
            if (callback) {
                callback(result->retCode, result->token);
            }
            return true;
        };
        task->execute(application, this, cb);
        return true;
    }
    bool AsyncAudioOperations::stop(const audio::Token &token, const OnStopCallback &callback)
    {
        auto msg  = std::make_unique<AudioStopRequest>(token);
        auto task = app::AsyncRequest::createFromMessage(std::move(msg), service::name::audio);
        auto cb   = [callback](auto response) {
            auto result = dynamic_cast<AudioStopResponse *>(response);
            if (result == nullptr) {
                return false;
            }
            if (callback) {
                callback(result->retCode, result->token);
            }
            return true;
        };
        task->execute(application, this, cb);
        return true;
    }

} // namespace app
