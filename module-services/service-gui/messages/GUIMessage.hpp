// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <MessageType.hpp>
#include <Service/Message.hpp>
#include <core/DrawCommand.hpp>

namespace sgui
{

    /*
     * @brief Template for all messages that go to gui service
     */
    class GUIMessage : public sys::DataMessage
    {
        bool suspend  = false;
        bool shutdown = false;

      public:
        GUIMessage(MessageType messageType, bool suspend = false, bool shutdown = false)
            : sys::DataMessage(messageType), suspend{suspend}, shutdown{shutdown} {};
        virtual ~GUIMessage(){};
        bool getSuspend()
        {
            return suspend;
        };
        bool getShutdown()
        {
            return shutdown;
        };
    };

} /* namespace sgui */
