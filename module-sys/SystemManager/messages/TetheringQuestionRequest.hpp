// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-sys/Service/Message.hpp>

#include <apps-common/popups/data/PopupRequestParams.hpp>

#include <service-appmgr/Actions.hpp>
#include <service-appmgr/messages/ActionRequest.hpp>

namespace sys
{
    class TetheringQuestionRequest : public sys::DataMessage, public app::manager::actions::ConvertibleToAction
    {
      public:
        TetheringQuestionRequest() : sys::DataMessage(MessageType::MessageTypeUninitialized)
        {}

        std::unique_ptr<app::manager::ActionRequest> toAction() const override
        {
            auto params = std::make_unique<gui::PopupRequestParams>(gui::popup::ID::Tethering);
            return std::make_unique<app::manager::ActionRequest>(
                sender, app::manager::actions::ShowPopup, std::move(params));
        }
    };

    class TetheringQuestionAbort : public sys::DataMessage, public app::manager::actions::ConvertibleToAction
    {
      public:
        TetheringQuestionAbort() : sys::DataMessage(MessageType::MessageTypeUninitialized)
        {}

        std::unique_ptr<app::manager::ActionRequest> toAction() const override
        {
            auto params = std::make_unique<gui::PopupRequestParams>(gui::popup::ID::Tethering);
            return std::make_unique<app::manager::ActionRequest>(
                sender, app::manager::actions::AbortPopup, std::move(params));
        }
    };

    class TetheringEnabledResponse : public sys::ResponseMessage
    {};
} // namespace sys
