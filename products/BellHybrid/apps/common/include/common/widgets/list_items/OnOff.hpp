// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "details.hpp"
#include <i18n/i18n.hpp>
#include <utility>

namespace app::list_items
{
    class OnOff : public details::ListItemBase<gui::StringSpinner>
    {
      public:
        explicit OnOff(gui::AbstractSettingsModel<gui::StringSpinner::value_type> &model,
                       const std::string &topDescription    = "",
                       const std::string &bottomDescription = "")
            : details::ListItemBase<gui::StringSpinner>(
                  {utils::translate("app_settings_toggle_off"), utils::translate("app_settings_toggle_on")},
                  model,
                  topDescription,
                  bottomDescription)
        {}

        [[nodiscard]] bool is_active() const
        {
            return value() == utils::translate("app_settings_toggle_on");
        }
    };
} // namespace app::list_items
