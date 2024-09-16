// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "ChangeHomescreenLayoutParams.hpp"

#include <Service/Message.hpp>
#include <service-appmgr/Actions.hpp>
#include <service-appmgr/messages/ActionRequest.hpp>

class ChangeHomescreenLayoutMessage : public sys::DataMessage, public app::manager::actions::ConvertibleToAction
{
  public:
    explicit ChangeHomescreenLayoutMessage(std::string newHomescreenLayout)
        : sys::DataMessage{MessageType::MessageTypeUninitialized}, newHomescreenLayout{std::move(newHomescreenLayout)}
    {}

    [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest> override
    {
        return std::make_unique<app::manager::ActionRequest>(
            sender,
            app::manager::actions::ChangeHomescreenLayout,
            std::make_unique<gui::ChangeHomescreenLayoutParams>(newHomescreenLayout));
    }

  private:
    std::string newHomescreenLayout{};
};
