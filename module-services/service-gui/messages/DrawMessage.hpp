// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "GUIMessage.hpp"

#include <core/DrawCommand.hpp>
#include <gui/Common.hpp>
#include <Service/Message.hpp>

#include <list>
#include <memory>

namespace gui
{
    class DrawCommand;
} // namespace gui

namespace sgui
{

    /*
     *
     */
    class DrawMessage : public GUIMessage
    {
      public:
        enum class DrawCommand
        {
            NORMAL,
            SUSPEND,
            SHUTDOWN
        };

      public:
        gui::RefreshModes mode;
        std::list<std::unique_ptr<gui::DrawCommand>> commands;

        /**
         * flag that informs that this is last rendering before suspending system.
         */
        DrawCommand command = DrawCommand::NORMAL;

        DrawMessage(const std::list<gui::DrawCommand *> &commandsList,
                    gui::RefreshModes mode,
                    DrawCommand cmd = DrawCommand::NORMAL);
        virtual ~DrawMessage();
    };

} /* namespace sgui */
