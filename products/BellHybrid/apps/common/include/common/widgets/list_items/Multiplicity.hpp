// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Numeric.hpp"
#include <common/LanguageUtils.hpp>

namespace app::list_items
{
    class Multiplicity : public Numeric
    {
      public:
        Multiplicity(spinner_type::range &&range,
                     gui::AbstractSettingsModel<spinner_type::value_type> &model,
                     const std::string &topDescription    = "",
                     const std::string &bottomDescription = "")
            : Numeric{std::move(range), model, topDescription, bottomDescription}
        {}

      private:
        void control_bottom_description(const spinner_type::value_type &value) final
        {
            bottomText->setRichText(utils::language::getCorrectMultiplicityForm(value));
        }
    };
} // namespace app::list_items
