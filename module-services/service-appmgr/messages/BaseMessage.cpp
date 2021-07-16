// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BaseMessage.hpp"

namespace app::manager
{

    BaseMessage::BaseMessage(ApplicationName sender)
        : sys::DataMessage(MessageType::APMGeneric), senderName(std::move(sender))
    {}

    BaseMessage::BaseMessage(MessageType type, ApplicationName sender)
        : sys::DataMessage(type), senderName{std::move(sender)}
    {}

    [[nodiscard]] auto BaseMessage::getSenderName() const noexcept -> const ApplicationName &
    {
        return senderName;
    }
} // namespace app::manager
