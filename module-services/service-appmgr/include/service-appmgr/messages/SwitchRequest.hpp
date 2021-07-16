// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseMessage.hpp"

#include <module-gui/gui/SwitchData.hpp>

namespace app::manager
{
    class SwitchRequest : public BaseMessage
    {
      public:
        SwitchRequest(const ApplicationName &senderName,
                      ApplicationName applicationName,
                      std::string windowName,
                      std::unique_ptr<gui::SwitchData> data);

        [[nodiscard]] auto getName() const noexcept -> const ApplicationName &;
        [[nodiscard]] auto getWindow() const noexcept -> const std::string &;
        [[nodiscard]] auto getData() noexcept -> std::unique_ptr<gui::SwitchData> &;

      private:
        ApplicationName application;
        std::string window;
        std::unique_ptr<gui::SwitchData> data;
    };
} // namespace app::manager
