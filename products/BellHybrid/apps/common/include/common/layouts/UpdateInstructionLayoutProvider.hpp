// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <utf8/UTF8.hpp>

namespace gui
{
    class Item;

    class UpdateInstructionLayoutProvider
    {
      protected:
        const UTF8 image;
        const UTF8 title;
        const bool leftArrowVisible;
        const bool rightArrowVisible;

      public:
        UpdateInstructionLayoutProvider(const UTF8 &image,
                                        const UTF8 &title,
                                        bool leftArrow  = true,
                                        bool rightArrow = true)
            : image{image}, title{title}, leftArrowVisible{leftArrow}, rightArrowVisible{rightArrow} {};
        virtual ~UpdateInstructionLayoutProvider() noexcept = default;

        virtual Item *getLayout() = 0;
    };
}; // namespace gui
