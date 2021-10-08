// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseMessage.hpp"

#include <module-gui/gui/SwitchData.hpp>

namespace app::manager
{
    class SwitchBackRequest : public BaseMessage
    {
      public:
        SwitchBackRequest(const ApplicationName &name, std::unique_ptr<gui::SwitchData> data = nullptr);

        [[nodiscard]] auto getData() noexcept -> std::unique_ptr<gui::SwitchData> &;

      private:
        std::unique_ptr<gui::SwitchData> data;
    };
} // namespace app::manager
