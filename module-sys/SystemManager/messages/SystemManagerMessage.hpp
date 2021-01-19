// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <MessageType.hpp>
#include <Service/Message.hpp>

#include "SystemManager/Constants.hpp"
#include "SystemManager/data/SystemManagerActionsParams.hpp"
#include <service-appmgr/service-appmgr/Actions.hpp>

namespace sys
{

    class SystemManagerMessage : public sys::DataMessage
    {
      public:
        SystemManagerMessage() : sys::DataMessage(MessageType::PMChangePowerMode){};
    };

    class CriticalBatteryLevelNotification : public sys::Message, public app::manager::actions::ConvertibleToAction
    {
      public:
        explicit CriticalBatteryLevelNotification(bool isActive) : sys::Message(), isActive(isActive)
        {}

        [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest>
        {
            return std::make_unique<app::manager::ActionRequest>(
                service::name::system_manager,
                app::manager::actions::DisplayLowBatteryNotification,
                std::make_unique<app::manager::actions::LowBatteryNotificationParams>(isActive));
        }

      private:
        bool isActive;
    };

} // namespace sys
