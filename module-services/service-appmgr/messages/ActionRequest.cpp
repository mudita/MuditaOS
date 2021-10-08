// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ActionRequest.hpp"

namespace app::manager
{
    ActionRequest::ActionRequest(const ApplicationName &senderName,
                                 actions::ActionId _actionId,
                                 actions::ActionParamsPtr &&_data)
        : BaseMessage{MessageType::APMAction, senderName}, actionId{_actionId}, data{std::move(_data)}
    {}

    [[nodiscard]] auto ActionRequest::getAction() const noexcept -> actions::ActionId
    {
        return actionId;
    }

    [[nodiscard]] auto ActionRequest::getData() noexcept -> actions::ActionParamsPtr &
    {
        return data;
    }
} // namespace app::manager
