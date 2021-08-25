// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-sys/Service/Message.hpp>

#include <service-appmgr/Actions.hpp>
#include <service-appmgr/messages/ActionRequest.hpp>

namespace sys
{
    class TetheringPhoneModeChangeProhibitedMessage : public sys::DataMessage,
                                                      public app::manager::actions::ConvertibleToAction
    {
      public:
        TetheringPhoneModeChangeProhibitedMessage() : sys::DataMessage(MessageType::MessageTypeUninitialized)
        {}

        std::unique_ptr<app::manager::ActionRequest> toAction() const override
        {
            auto params = std::make_unique<gui::PopupRequestParams>(gui::popup::ID::TetheringPhoneModeChangeProhibited);
            return std::make_unique<app::manager::ActionRequest>(
                sender, app::manager::actions::ShowPopup, std::move(params));
        }
    };

} // namespace sys
