// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractSoundsPlayer.hpp"

class SoundsPlayer : public AbstractSoundsPlayer
{
  public:
    explicit SoundsPlayer(app::Application *app);

    bool play(const std::string &path) override;
    bool pause() override;
    bool resume() override;
    bool stop() override;
    bool stop(audio::Token token) override;
    bool isInState(State state) const noexcept override;
    bool previouslyPlayed(const std::string &filePath) const override;

  private:
    std::unique_ptr<app::AbstractAudioOperations> audioOperations;

    State currentState = State::Stopped;
    audio::Token currentToken;
    std::string currentPath;
};
