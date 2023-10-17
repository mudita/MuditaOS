// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>
#include <popups/AlarmActivatedPopupRequestParams.hpp>
#include <popups/AlarmDeactivatedPopupRequestParams.hpp>
#include <popups/BedtimeReminderPopupRequestParams.hpp>
#include <service-appmgr/Actions.hpp>
#include <service-appmgr/messages/ActionRequest.hpp>

class AlarmActivated : public sys::DataMessage, public app::manager::actions::ConvertibleToAction
{
  public:
    AlarmActivated() : sys::DataMessage{MessageType::MessageTypeUninitialized}
    {}

    [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest> override
    {
        return std::make_unique<app::manager::ActionRequest>(
            sender, app::manager::actions::ShowPopup, std::make_unique<gui::AlarmActivatedPopupRequestParams>());
    }
};

class AlarmDeactivated : public sys::DataMessage, public app::manager::actions::ConvertibleToAction
{
  public:
    AlarmDeactivated() : sys::DataMessage{MessageType::MessageTypeUninitialized}
    {}

    [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest> override
    {
        return std::make_unique<app::manager::ActionRequest>(
            sender, app::manager::actions::ShowPopup, std::make_unique<gui::AlarmDeactivatedPopupRequestParams>());
    }
};

class BedtimeNotification : public sys::DataMessage, public app::manager::actions::ConvertibleToAction
{
  public:
    BedtimeNotification() : sys::DataMessage{MessageType::MessageTypeUninitialized}
    {}

    [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest> override
    {
        return std::make_unique<app::manager::ActionRequest>(
            sender, app::manager::actions::ShowPopup, std::make_unique<gui::BedtimeReminderPopupRequestParams>());
    }
};
