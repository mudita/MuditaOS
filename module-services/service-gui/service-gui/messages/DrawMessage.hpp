// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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

namespace service::gui
{
    class DrawMessage : public GUIMessage
    {
      public:
        enum class Type
        {
            NORMAL,
            SUSPEND
        } type = Type::NORMAL;

      public:
        ::gui::RefreshModes mode;
        std::list<::gui::Command> commands;

        DrawMessage(std::list<::gui::Command> commandsList, ::gui::RefreshModes mode);

        void setCommandType(Type value) noexcept
        {
            type = value;
        }

        bool isType(Type value) const noexcept
        {
            return type == value;
        }
    };
} // namespace service::gui
