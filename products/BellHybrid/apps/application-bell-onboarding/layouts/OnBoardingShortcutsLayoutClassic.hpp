// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "OnBoardingShortcutsLayoutProvider.hpp"

#include <BoxLayout.hpp>

namespace gui
{
    class BellBaseLayout;

    class OnBoardingShortcutsLayoutClassic : public OnBoardingShortcutsLayoutProvider, VBox
    {
      private:
        const UTF8 image;
        const UTF8 description;

        const bool leftArrowVisible;
        const bool rightArrowVisible;

        void buildInterface();

      public:
        OnBoardingShortcutsLayoutClassic(const UTF8 &image,
                                         const UTF8 &description,
                                         bool leftArrowVisible  = true,
                                         bool rightArrowVisible = true);

        auto getLayout() -> Item * override;
    };
}; // namespace gui
