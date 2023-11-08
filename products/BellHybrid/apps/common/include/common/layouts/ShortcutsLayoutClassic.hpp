// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ShortcutsLayoutProvider.hpp"

#include <BoxLayout.hpp>

namespace gui
{
    class BellBaseLayout;

    class ShortcutsLayoutClassic : public ShortcutsLayoutProvider, VBox
    {
      private:
        const UTF8 image;
        const UTF8 description;

        const bool leftArrowVisible;
        const bool rightArrowVisible;

        void buildInterface();

      public:
        ShortcutsLayoutClassic(const UTF8 &image,
                               const UTF8 &description,
                               bool leftArrowVisible  = true,
                               bool rightArrowVisible = true);

        auto getLayout() -> Item * override;
    };
}; // namespace gui
