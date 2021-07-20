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

namespace app::music_player
{
    AudioOperations::AudioOperations(Application *application)
        : app::AsyncCallbackReceiver{application}, application(application)
    {}

    bool AudioOperations::play(const std::string &filePath, const OnPlayCallback &callback)
    {
        auto msg  = std::make_unique<AudioStartPlaybackRequest>(filePath, audio::PlaybackType::Multimedia);
        auto task = app::AsyncRequest::createFromMessage(std::move(msg), service::name::audio);
        auto cb   = [callback](auto response) {
            auto result = dynamic_cast<AudioStartPlaybackResponse *>(response);
            if (result == nullptr) {
                return false;
            }
            if (callback) {
                callback(result->token);
            }
            return true;
        };
        task->execute(application, this, cb);
        return true;
    }

    bool AudioOperations::pause(const audio::Token &token)
    {
        return AudioServiceAPI::Pause(application, token);
    }
    bool AudioOperations::resume(const audio::Token &token)
    {
        return AudioServiceAPI::Resume(application, token);
    }
    bool AudioOperations::stop(const audio::Token &token, const OnStopCallback &callback)
    {
        auto msg  = std::make_unique<AudioStopRequest>(token);
        auto task = app::AsyncRequest::createFromMessage(std::move(msg), service::name::audio);
        auto cb   = [callback](auto response) {
            auto result = dynamic_cast<AudioStopResponse *>(response);
            if (result == nullptr) {
                return false;
            }
            if (callback) {
                callback(result->token);
            }
            return true;
        };
        task->execute(application, this, cb);
        return true;
    }

} // namespace app::music_player
