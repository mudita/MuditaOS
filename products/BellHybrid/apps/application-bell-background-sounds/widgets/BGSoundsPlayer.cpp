// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BGSoundsPlayer.hpp"
#include <service-audio/AudioMessage.hpp>
#include <service-audio/AudioServiceName.hpp>

namespace app::bgSounds
{
    BGSoundsPlayer::BGSoundsPlayer(app::ApplicationCommon *app) : app::AsyncCallbackReceiver{app}, app{app}
    {}
    void BGSoundsPlayer::start(const std::string &filePath, PlaybackMode mode, OnStateChangeCallback callback)
    {
        auto msg  = std::make_unique<AudioStartPlaybackRequest>(filePath, audio::PlaybackType::Multimedia);
        auto task = app::AsyncRequest::createFromMessage(std::move(msg), service::name::audio);
        auto cb   = [this, _callback = std::move(callback), filePath, mode](auto response) {
            auto result = dynamic_cast<AudioStartPlaybackResponse *>(response);
            if (result == nullptr) {
                return false;
            }
            token          = result->token;
            recentFilePath = std::move(filePath);
            playbackMode   = mode;
            if (_callback) {
                _callback(result->retCode);
            }
            return true;
        };
        task->execute(app, this, std::move(cb));
    }
    void BGSoundsPlayer::stop(OnStateChangeCallback callback)
    {
        if (token.IsValid()) {
            auto msg  = std::make_unique<AudioStopRequest>(token);
            auto task = app::AsyncRequest::createFromMessage(std::move(msg), service::name::audio);
            auto cb   = [_callback = std::move(callback)](auto response) {
                auto result = dynamic_cast<AudioStopResponse *>(response);
                if (result == nullptr) {
                    return false;
                }
                if (_callback) {
                    _callback(result->retCode);
                }
                return true;
            };
            task->execute(app, this, std::move(cb));
        }
    }
    void BGSoundsPlayer::pause(OnStateChangeCallback callback)
    {
        if (token.IsValid()) {
            auto msg  = std::make_unique<AudioPauseRequest>(token);
            auto task = app::AsyncRequest::createFromMessage(std::move(msg), service::name::audio);
            auto cb   = [_callback = std::move(callback)](auto response) {
                auto result = dynamic_cast<AudioPauseResponse *>(response);
                if (result == nullptr) {
                    return false;
                }
                if (_callback) {
                    _callback(result->retCode);
                }
                return true;
            };
            task->execute(app, this, std::move(cb));
        }
    }
    void BGSoundsPlayer::resume(OnStateChangeCallback callback)
    {
        auto msg  = std::make_unique<AudioResumeRequest>(token);
        auto task = app::AsyncRequest::createFromMessage(std::move(msg), service::name::audio);
        auto cb   = [_callback = std::move(callback)](auto response) {
            auto result = dynamic_cast<AudioResumeResponse *>(response);
            if (result == nullptr) {
                return false;
            }
            if (_callback) {
                _callback(result->retCode);
            }
            return true;
        };
        task->execute(app, this, std::move(cb));
    }

    auto BGSoundsPlayer::handle(AudioEOFNotification *msg) -> std::shared_ptr<sys::Message>
    {
        if (token == msg->token && playbackMode == PlaybackMode::Looped) {
            auto msg  = std::make_unique<AudioStartPlaybackRequest>(recentFilePath, audio::PlaybackType::Multimedia);
            auto task = app::AsyncRequest::createFromMessage(std::move(msg), service::name::audio);
            auto cb   = [this](auto response) {
                auto result = dynamic_cast<AudioStartPlaybackResponse *>(response);
                if (result == nullptr) {
                    return false;
                }
                token = result->token;
                return true;
            };
            task->execute(app, this, std::move(cb));
        }
        return sys::msgHandled();
    }
    AbstractBGSoundsPlayer::PlaybackMode BGSoundsPlayer::getCurrentMode() const noexcept
    {
        return playbackMode;
    }
} // namespace app::bgSounds
