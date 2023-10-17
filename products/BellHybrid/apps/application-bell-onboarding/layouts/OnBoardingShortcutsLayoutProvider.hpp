// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <utf8/UTF8.hpp>

namespace gui
{
    class Item;

    class OnBoardingShortcutsLayoutProvider
    {
      private:
        const UTF8 image;
        const UTF8 description;
        const bool leftArrow;
        const bool rightArrow;

      public:
        OnBoardingShortcutsLayoutProvider(const UTF8 &image,
                                          const UTF8 &description,
                                          bool leftArrow  = true,
                                          bool rightArrow = true)
            : image{image}, description{description}, leftArrow{leftArrow}, rightArrow{rightArrow} {};
        virtual ~OnBoardingShortcutsLayoutProvider() noexcept = default;

        virtual Item *getLayout() = 0;
    };
}; // namespace gui
