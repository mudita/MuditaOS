// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <utf8/UTF8.hpp>

namespace gui
{
    class Item;

    class ShortcutsLayoutProvider
    {
      private:
        const UTF8 image;
        const UTF8 description;
        const bool leftArrow;
        const bool rightArrow;

      public:
        ShortcutsLayoutProvider(const UTF8 &image,
                                const UTF8 &description,
                                bool leftArrow  = true,
                                bool rightArrow = true)
            : image{image}, description{description}, leftArrow{leftArrow}, rightArrow{rightArrow} {};
        virtual ~ShortcutsLayoutProvider() noexcept = default;

        virtual Item *getLayout() = 0;
    };
}; // namespace gui
