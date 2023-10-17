// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <MessageType.hpp>
#include <Service/Message.hpp>

#include "system/Constants.hpp"
#include "SystemManager/data/SystemManagerActionsParams.hpp"
#include <service-appmgr/Actions.hpp>

namespace sys
{

    class SystemManagerMessage : public sys::DataMessage
    {
      public:
        SystemManagerMessage() : sys::DataMessage(MessageType::PMChangePowerMode){};
    };

    class CriticalBatteryLevelNotification : public sys::DataMessage, public app::manager::actions::ConvertibleToAction
    {
      public:
        explicit CriticalBatteryLevelNotification(bool isActive, bool isCharging = false)
            : isActive(isActive), isCharging(isCharging)
        {}

        [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest>
        {
            return std::make_unique<app::manager::ActionRequest>(
                service::name::system_manager,
                app::manager::actions::DisplayLowBatteryScreen,
                std::make_unique<app::manager::actions::LowBatteryNotificationParams>(isActive, isCharging));
        }

      private:
        bool isActive;
        bool isCharging;
    };

} // namespace sys
