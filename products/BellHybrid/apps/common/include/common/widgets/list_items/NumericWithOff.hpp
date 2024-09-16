// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "details.hpp"
#include <i18n/i18n.hpp>
#include <utility>

namespace app::list_items
{
    struct NumericWithOffFormatter
    {
      public:
        NumericWithOffFormatter() : off(utils::translate("app_settings_toggle_off"))
        {}
        std::string operator()(const std::uint8_t &value) const
        {
            return (value == 0) ? off : std::to_string(value);
        }

      private:
        std::string off;
    };

    using NumericWithOffSpinner = gui::U8IntegerSpinnerWithFormatter<NumericWithOffFormatter>;
    class NumericWithOff : public details::ListItemBase<NumericWithOffSpinner>
    {
      public:
        NumericWithOff(spinner_type::range &&range,
                       gui::AbstractSettingsModel<spinner_type::value_type> &model,
                       const std::string &topDescription    = "",
                       const std::string &bottomDescription = "")
            : details::ListItemBase<spinner_type>(std::move(range), model, topDescription, bottomDescription)
        {}

      private:
        void control_bottom_description(const spinner_type::value_type &value) override
        {
            bottomText->setVisible(value != 0);
        }
    };
} // namespace app::list_items
