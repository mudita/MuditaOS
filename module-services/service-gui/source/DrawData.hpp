// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <core/DrawCommand.hpp>
#include <core/DrawCommandForward.hpp>
#include <utility>

namespace sgui
{
    class DrawData
    {
        gui::Commands commands;
        gui::RefreshModes mode;

      public:
        DrawData(gui::Commands commands, gui::RefreshModes mode) : commands(std::move(commands)), mode(mode)
        {}

        DrawData(const DrawData &) = delete;
        DrawData &operator=(const DrawData &) = delete;

        DrawData(DrawData &&r)
        {
            commands = std::move(r.commands);
            mode     = std::move(r.mode);
        }

        DrawData &operator=(DrawData &&r)
        {
            commands = std::move(r.commands);
            mode     = std::move(r.mode);
            return *this;
        }

        virtual ~DrawData() = default;

        [[nodiscard]] auto &getCommands() noexcept
        {
            return commands;
        }

        [[nodiscard]] auto getMode() const noexcept
        {
            return mode;
        }
    };
} // namespace sgui
