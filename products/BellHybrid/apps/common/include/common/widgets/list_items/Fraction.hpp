// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "details.hpp"
#include <i18n/i18n.hpp>
#include <utility>

namespace app::list_items
{
    /// Representation of fraction number tailored for GUI/app use
    struct FractionData
    {
        bool operator==(const FractionData &oth) const
        {
            return denominator == oth.denominator and nominator == oth.nominator;
        }
        double to_double() const
        {
            return nominator == denominator ? 0 : static_cast<double>(nominator) / denominator;
        }
        int nominator;
        int denominator;
    };

    struct FractionFormatter
    {
      public:
        FractionFormatter() : off(utils::translate("app_settings_toggle_off"))
        {}
        std::string operator()(FractionData value) const
        {
            return value.nominator == value.denominator
                       ? off
                       : (std::to_string(value.nominator) + "/" + std::to_string(value.denominator));
        }

      private:
        std::string off;
    };

    using FractionSpinner = gui::StringOutputSpinner<Model<FractionData>, FractionFormatter>;
    class Fraction : public details::ListItemBase<FractionSpinner>
    {
      public:
        Fraction(spinner_type::range &&range,
                 gui::AbstractSettingsModel<FractionData> &model,
                 const std::string &topDescription    = "",
                 const std::string &bottomDescription = "")
            : details::ListItemBase<spinner_type>(std::move(range), model, topDescription, bottomDescription)
        {}

      private:
        void control_bottom_description(const FractionData &value) final
        {
            bottomText->setVisible(value.nominator != value.denominator);
        }
    };
} // namespace app::list_items
