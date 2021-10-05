// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>
#include <popups/AlarmActivatedPopupRequestParams.hpp>
#include <popups/AlarmDeactivatedPopupRequestParams.hpp>
#include <service-appmgr/Actions.hpp>
#include <service-appmgr/messages/ActionRequest.hpp>

class IdleTimerRestart : public sys::DataMessage
{
  public:
    IdleTimerRestart() : sys::DataMessage{MessageType::MessageTypeUninitialized}
    {}
};

class IdleTimerRestartIfActive : public sys::DataMessage
{
  public:
    IdleTimerRestartIfActive() : sys::DataMessage{MessageType::MessageTypeUninitialized}
    {}
};

class IdleTimerStop : public sys::DataMessage
{
  public:
    IdleTimerStop() : sys::DataMessage{MessageType::MessageTypeUninitialized}
    {}
};
