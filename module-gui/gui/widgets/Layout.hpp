// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include "LayoutSizeStore.hpp"

namespace gui
{

    class Item;

    enum class LayoutHorizontalPolicy
    {
        LAYOUT_POLICY_HORIZONTAL_EXPAND = 0x01,
        LAYOUT_POLICY_HORIZONTAL_SHIRNH = 0x02,
        LAYOUT_POLICY_HORIZONTAL_STATIC = 0x04,
    };

    enum class LayoutVerticalPolicy
    {
        LAYOUT_POLICY_VERTICAL_EXPAND = 0x01,
        LAYOUT_POLICY_VERTICAL_SHRINK = 0x02,
        LAYOUT_POLICY_VERTICAL_STATIC = 0x04,
    };

    class Layout
    {
      public:
        virtual ~Layout()                          = default;
        std::unique_ptr<LayoutSizeStore> sizeStore = nullptr;
    };

} /* namespace gui */
