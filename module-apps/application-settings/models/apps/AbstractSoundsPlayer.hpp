// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/AudioOperations.hpp>

#include <string>

class AbstractSoundsPlayer
{
  public:
    enum class State
    {
        Playing,
        Paused,
        Stopped
    };

    virtual bool play(const std::string &path)                       = 0;
    virtual bool pause()                                             = 0;
    virtual bool resume()                                            = 0;
    virtual bool stop()                                              = 0;
    virtual bool stop(audio::Token token)                            = 0;
    virtual bool isInState(State state) const noexcept               = 0;
    virtual bool previouslyPlayed(const std::string &filePath) const = 0;
    virtual ~AbstractSoundsPlayer()                                  = default;
};
