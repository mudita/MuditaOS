﻿// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "GUIMessage.hpp"
#include <core/DrawCommand.hpp>
#include <gui/Common.hpp>
#include <Service/Message.hpp>

#include <list>
#include <memory>

#include "Service/Message.hpp"
#include "core/DrawCommandForward.hpp"
#include "GUIMessage.hpp"
#include "gui/Common.hpp"

namespace sgui
{

    class DrawMessage : public GUIMessage
    {
      public:
        enum class Type
        {
            NORMAL,
            SUSPEND,
            SHUTDOWN
        };

      public:
        gui::RefreshModes mode;
        std::list<gui::Command> commands;
        Type type = Type::NORMAL;

        DrawMessage(std::list<gui::Command> commandsList, gui::RefreshModes mode);

        void setCommandType(Type type)
        {
            this->type = type;
        }
    };

} // namespace sgui
