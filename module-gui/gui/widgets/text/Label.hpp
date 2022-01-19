// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "TextFixedSize.hpp"

namespace gui
{
    class Label : public TextFixedSize
    {
      private:
      public:
        explicit Label(Item *parent,
                       Position x            = 0U,
                       Position y            = 0U,
                       Length w              = 0U,
                       Length h              = 0U,
                       const UTF8 &inputText = UTF8{});

        uint32_t getTextNeedSpace(const UTF8 &text = "") const noexcept;
    };

} /* namespace gui */
