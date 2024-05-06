// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "NumericWithOff.hpp"
#include <LanguageUtils.hpp>

namespace app::list_items
{
    class MinutesWithOff : public NumericWithOff
    {
      public:
        MinutesWithOff(spinner_type::range &&range,
                       gui::AbstractSettingsModel<spinner_type::value_type> &model,
                       const std::string &topDescription    = "",
                       const std::string &bottomDescription = "")
            : NumericWithOff{std::move(range), model, topDescription, bottomDescription}
        {}

      private:
        void control_bottom_description(const spinner_type::value_type &value) final
        {
            bottomText->setVisible(value != 0);
            bottomText->setRichText(utils::language::getCorrectMinutesNumeralForm(value));
        }
    };
} // namespace app::list_items
