// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SwitchRequest.hpp"

namespace app::manager
{
    SwitchRequest::SwitchRequest(const ApplicationName &senderName,
                                 ApplicationName applicationName,
                                 std::string windowName,
                                 std::unique_ptr<gui::SwitchData> data)
        : BaseMessage(MessageType::APMSwitch, senderName),
          application{std::move(applicationName)}, window{std::move(windowName)}, data{std::move(data)}
    {}

    [[nodiscard]] auto SwitchRequest::getName() const noexcept -> const ApplicationName &
    {
        return application;
    }

    [[nodiscard]] auto SwitchRequest::getWindow() const noexcept -> const std::string &
    {
        return window;
    }

    [[nodiscard]] auto SwitchRequest::getData() noexcept -> std::unique_ptr<gui::SwitchData> &
    {
        return data;
    }
} // namespace app::manager
