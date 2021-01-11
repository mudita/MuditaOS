// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <MessageType.hpp>
#include <Service/Message.hpp>
#include <core/DrawCommand.hpp>

namespace service::gui
{
    class GUIMessage : public sys::DataMessage
    {
      public:
        GUIMessage() : sys::DataMessage(MessageType::GUIMessage)
        {}
    };
} // namespace service::gui
