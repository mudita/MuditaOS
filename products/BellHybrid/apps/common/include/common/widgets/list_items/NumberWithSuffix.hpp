// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "details.hpp"
#include <i18n/i18n.hpp>
#include <utility>

namespace app::list_items
{
    struct NumberWithSuffixFormatter
    {
      public:
        NumberWithSuffixFormatter()
            : suffix(utils::translate("common_minute_short")), off(utils::translate("app_settings_toggle_off"))
        {}
        std::string operator()(const std::uint8_t &value) const
        {
            return value == 0 ? off : (std::to_string(value) + " " + suffix);
        }

      private:
        std::string suffix;
        std::string off;
    };

    using NumberWithSuffixSpinner = gui::StringOutputSpinner<Model<std::uint8_t, true>, NumberWithSuffixFormatter>;
    class NumberWithSuffix : public details::ListItemBase<NumberWithSuffixSpinner>
    {
      public:
        NumberWithSuffix(spinner_type::range &&range,
                         gui::AbstractSettingsModel<spinner_type::value_type> &model,
                         const std::string &topDescription    = "",
                         const std::string &bottomDescription = "")
            : details::ListItemBase<spinner_type>(std::move(range), model, topDescription, bottomDescription)
        {}

      private:
        void control_bottom_description(const spinner_type::value_type &value) final
        {
            bottomText->setVisible(value != 0);
        }
    };
} // namespace app::list_items
