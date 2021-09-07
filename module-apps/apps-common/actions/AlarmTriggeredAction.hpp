// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AlarmRingingData.hpp"

#include <service-appmgr/Actions.hpp>
#include <service-appmgr/messages/ActionRequest.hpp>

namespace app::actions
{
    class AlarmTriggeredAction : public sys::DataMessage, public app::manager::actions::ConvertibleToAction
    {
      public:
        AlarmTriggeredAction() : sys::DataMessage(MessageType::MessageTypeUninitialized)
        {}

        std::unique_ptr<app::manager::ActionRequest> toAction() const override
        {
            auto params = std::make_unique<AlarmRingingData>();
            return std::make_unique<app::manager::ActionRequest>(
                sender, app::manager::actions::ShowAlarm, std::move(params));
        }
    };

} // namespace app::actions
