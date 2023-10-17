// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "details.hpp"
#include <i18n/i18n.hpp>
#include <utility>

namespace app::list_items
{
    class Text : public details::ListItemBase<gui::StringSpinner>
    {
      public:
        Text(gui::StringSpinner::range &&range,
             gui::AbstractSettingsModel<gui::StringSpinner::value_type> &model,
             const std::string &topDescription    = "",
             const std::string &bottomDescription = "")
            : details::ListItemBase<gui::StringSpinner>(std::move(range), model, topDescription, bottomDescription)
        {
            raw_spinner()->setFont(gui::bell_style::font_center);
        }
    };
} // namespace app::list_items
