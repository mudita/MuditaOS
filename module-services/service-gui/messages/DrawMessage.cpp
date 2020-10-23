// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DrawMessage.hpp"
#include "GUIMessage.hpp"
#include <Common.hpp>
#include <DrawCommand.hpp>

namespace sgui
{

    DrawMessage::DrawMessage(std::list<gui::DrawCommand *> commandsList, gui::RefreshModes mode)
        : GUIMessage(MessageType::GUICommands)
    {

        this->mode = mode;
        [[maybe_unused]] auto ret = std::remove_if(
            commandsList.begin(), commandsList.end(), [](gui::DrawCommand *cmd) { return cmd == nullptr; });
        for (auto cmd : commandsList) {
            commands.push_back(std::unique_ptr<gui::DrawCommand>(cmd));
        }
    }
} /* namespace sgui */
