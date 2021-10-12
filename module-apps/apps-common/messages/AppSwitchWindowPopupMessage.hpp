// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AppMessage.hpp"
#include "Disposition.hpp"
#include <functional>

namespace app
{
    class AppSwitchWindowPopupMessage : public AppSwitchWindowMessage
    {
        const gui::popup::Disposition disposition;

      public:
        AppSwitchWindowPopupMessage(const std::string &window,
                                    std::unique_ptr<gui::SwitchData> data,
                                    SwitchReason reason,
                                    const gui::popup::Disposition &disposition)
            : AppSwitchWindowMessage(
                  window, "", std::forward<decltype(data)>(data), gui::ShowMode::GUI_SHOW_INIT, reason),
              disposition(disposition)
        {}

        virtual bool toPopupRequest() const override
        {
            return true;
        }

        std::pair<const std::string &, gui::popup::Disposition> getSwitchData() override
        {
            return {window, disposition};
        }
    };
} // namespace app
