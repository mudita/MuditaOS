// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>
#include <popups/AlarmActivatedPopupRequestParams.hpp>
#include <popups/AlarmDeactivatedPopupRequestParams.hpp>
#include <service-appmgr/Actions.hpp>
#include <service-appmgr/messages/ActionRequest.hpp>

class StartIdleTimerMessage : public sys::DataMessage
{
  public:
    StartIdleTimerMessage() : sys::DataMessage{MessageType::MessageTypeUninitialized}
    {}
};

class RestartIdleTimerMessage : public sys::DataMessage
{
  public:
    RestartIdleTimerMessage() : sys::DataMessage{MessageType::MessageTypeUninitialized}
    {}
};

class StopIdleTimerMessage : public sys::DataMessage
{
  public:
    StopIdleTimerMessage() : sys::DataMessage{MessageType::MessageTypeUninitialized}
    {}
};
