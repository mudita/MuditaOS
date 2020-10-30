﻿// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DrawMessage.hpp"
#include "GUIMessage.hpp"
#include <Common.hpp>
#include <DrawCommand.hpp>

namespace sgui
{

    DrawMessage::DrawMessage(std::list<gui::Command> commands, gui::RefreshModes mode)
        : GUIMessage(MessageType::GUICommands), mode(mode), commands(std::move(commands))
    {
    }
} /* namespace sgui */
