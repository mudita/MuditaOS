// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SwitchBackRequest.hpp"

namespace app::manager
{
    SwitchBackRequest::SwitchBackRequest(const ApplicationName &name, std::unique_ptr<gui::SwitchData> data)
        : BaseMessage(MessageType::APMSwitchPrevApp, name), data{std::move(data)}
    {}

    [[nodiscard]] auto SwitchBackRequest::getData() noexcept -> std::unique_ptr<gui::SwitchData> &
    {
        return data;
    }
} // namespace app::manager
