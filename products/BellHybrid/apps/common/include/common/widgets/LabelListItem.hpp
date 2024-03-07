// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ListItem.hpp"
#include "widgets/TextWithIconsWidget.hpp"

namespace gui
{
    template <typename T>
    class LabelListItem : public ListItem
    {
      private:
        std::optional<T> labelType{};

      public:
        explicit LabelListItem(std::optional<T> type) : labelType{type}
        {}
        virtual ~LabelListItem() = default;
        std::optional<T> getType()
        {
            return labelType;
        }
    };

    class LabelMarkerItem : public ListItem
    {
      public:
        explicit LabelMarkerItem(const UTF8 &labelText);
        virtual ~LabelMarkerItem() = default;
    };
} // namespace gui
