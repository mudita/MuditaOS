// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SoundsPlayer.hpp"

SoundsPlayer::SoundsPlayer(app::ApplicationCommon *app)
    : audioOperations{std::make_unique<app::AsyncAudioOperations>(app)}
{}

bool SoundsPlayer::play(const std::string &path)
{
    return audioOperations->play(path, [this, path](audio::RetCode retCode, audio::Token token) {
        if (retCode != audio::RetCode::Success || !token.IsValid()) {
            LOG_ERROR("Audio preview callback failed with retcode = %s. Token validity: %d",
                      str(retCode).c_str(),
                      token.IsValid());
            return;
        }
        currentState = State::Playing;
        currentToken = token;
        currentPath  = path;
    });
}

bool SoundsPlayer::pause()
{
    return audioOperations->pause(currentToken, [this](audio::RetCode retCode, audio::Token token) {
        if (token != currentToken) {
            LOG_ERROR("Audio preview pause failed: wrong token");
            return;
        }
        if (retCode != audio::RetCode::Success || !token.IsValid()) {
            LOG_ERROR("Audio preview pause failed with retcode = %s. Token validity: %d",
                      str(retCode).c_str(),
                      token.IsValid());
            return;
        }
        currentState = State::Paused;
    });
}

bool SoundsPlayer::resume()
{
    return audioOperations->resume(currentToken, [this](audio::RetCode retCode, audio::Token token) {
        if (token != currentToken) {
            LOG_ERROR("Audio preview resume failed: wrong token");
            return;
        }

        if (retCode != audio::RetCode::Success || !token.IsValid()) {
            LOG_ERROR("Audio preview pause failed with retcode = %s. Token validity: %d",
                      str(retCode).c_str(),
                      token.IsValid());
            return;
        }

        currentState = State::Playing;
    });
}

bool SoundsPlayer::stop()
{
    if (currentToken.IsValid()) {
        return audioOperations->stop(currentToken,
                                     [this](audio::RetCode, audio::Token) { currentState = State::Stopped; });
    }
    return false;
}

bool SoundsPlayer::stop(audio::Token token)
{
    if (currentToken.IsValid() && currentToken == token) {
        currentState = State::Stopped;
        return true;
    }
    return false;
}

bool SoundsPlayer::isInState(State state) const noexcept
{
    return currentState == state;
}

bool SoundsPlayer::previouslyPlayed(const std::string &filePath) const
{
    return currentToken.IsValid() && currentPath == filePath;
}
