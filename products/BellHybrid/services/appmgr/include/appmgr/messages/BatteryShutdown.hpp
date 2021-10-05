// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>
#include <apps-common/popups/data/PopupRequestParams.hpp>
#include <service-appmgr/Actions.hpp>
#include <service-appmgr/messages/ActionRequest.hpp>

class BatteryShutdown : public sys::DataMessage, public app::manager::actions::ConvertibleToAction
{
  public:
    BatteryShutdown() : sys::DataMessage{MessageType::MessageTypeUninitialized}
    {}

    [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest> override
    {
        return std::make_unique<app::manager::ActionRequest>(sender,
                                                             app::manager::actions::DisplayLowBatteryScreen,
                                                             std::make_unique<app::manager::actions::ActionParams>());
    }
};
