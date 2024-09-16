// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "details.hpp"
#include <i18n/i18n.hpp>
#include <utility>

namespace app::list_items
{
    class NumericWithBar : public details::ListItemBaseWithBar<gui::U8IntegerSpinner>
    {
      public:
        NumericWithBar(gui::U8IntegerSpinner::range &&range,
                       gui::AbstractSettingsModel<gui::U8IntegerSpinner::value_type> &model,
                       const std::uint8_t barSteps,
                       const std::string &topDescription    = "",
                       const std::string &bottomDescription = "")
            : details::ListItemBaseWithBar<gui::U8IntegerSpinner>(
                  std::move(range), model, barSteps, topDescription, bottomDescription)
        {}
    };
} // namespace app::list_items
