﻿// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DrawMessage.hpp"

#include "GUIMessage.hpp"

#include <Common.hpp>
#include <DrawCommand.hpp>
#include <MessageType.hpp>

namespace sgui
{

    DrawMessage::DrawMessage(const std::list<gui::DrawCommand *> &commandsList, gui::RefreshModes mode, DrawCommand cmd)
        : GUIMessage(MessageType::GUICommands), command{cmd}
    {

        this->mode = mode;
        for (auto cmd : commandsList) {
            if (cmd)
                commands.push_back(std::unique_ptr<gui::DrawCommand>(cmd));
        }
    }

    DrawMessage::~DrawMessage()
    {}

} /* namespace sgui */
