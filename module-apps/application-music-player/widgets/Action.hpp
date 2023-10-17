// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Image.hpp>

#include <map>

namespace gui
{
    class Action : public Image
    {
      public:
        enum class State
        {
            Play,
            Pause
        };

        using StateMap = std::map<const State, const std::string>;

        static constexpr auto play            = "play";
        static constexpr auto pause           = "pause";
        static const inline StateMap stateMap = {{State::Play, play}, {State::Pause, pause}};

        void setState(State state);
        Action(Item *parent, uint32_t x, uint32_t y);

      private:
        State state = State::Pause;
    };
} // namespace gui
