// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <BoxLayout.hpp>

namespace gui
{
    class WhatsNewFeaturesLayout : public VBox
    {
      public:
        WhatsNewFeaturesLayout(const std::string &title,
                               const std::string &description,
                               const std::string &iconName,
                               bool leftArrowState  = true,
                               bool rightArrowState = true);

      private:
        auto buildInterface(const std::string &title,
                            const std::string &description,
                            const std::string &iconName,
                            bool leftArrowState,
                            bool rightArrowState) -> void;
    };
} // namespace gui
