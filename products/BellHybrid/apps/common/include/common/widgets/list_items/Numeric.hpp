// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "details.hpp"
#include <i18n/i18n.hpp>
#include <utility>

namespace app::list_items
{
    class Numeric : public details::ListItemBase<gui::U8IntegerSpinner>
    {
      public:
        Numeric(gui::U8IntegerSpinner::range &&range,
                gui::AbstractSettingsModel<gui::U8IntegerSpinner::value_type> &model,
                const std::string &topDescription    = "",
                const std::string &bottomDescription = "")
            : details::ListItemBase<gui::U8IntegerSpinner>(std::move(range), model, topDescription, bottomDescription)
        {}
    };
} // namespace app::list_items
