// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-sys/Service/Message.hpp>
#include <popups/AlarmPopupRequestParams.hpp>
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
            sender, app::manager::actions::ShowPopup, std::make_unique<gui::AlarmPopupRequestParams>());
    }
};
