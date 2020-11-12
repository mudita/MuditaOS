// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-appmgr/messages/BaseMessage.hpp>

namespace app::manager
{
    BaseMessage::BaseMessage(MessageType type, ApplicationName sender)
        : sys::DataMessage(type), senderName{std::move(sender)}
    {}

    [[nodiscard]] auto BaseMessage::getSenderName() const noexcept -> const ApplicationName &
    {
        return senderName;
    }
} // namespace app::manager
