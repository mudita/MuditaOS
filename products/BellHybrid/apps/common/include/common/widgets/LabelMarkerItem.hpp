// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ListItem.hpp"
#include "widgets/TextWithIconsWidget.hpp"
#include <common/widgets/ListViewWithLabels.hpp>

namespace gui
{
    class LabelMarkerItem : public ListItem
    {
      public:
        explicit LabelMarkerItem(const UTF8 &labelText);
        virtual ~LabelMarkerItem() = default;
    };
} // namespace gui
