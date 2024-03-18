// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/ApplicationCommon.hpp>
#include <ListViewWithArrows.hpp>

#include <optional>

namespace gui
{
    class ListItemProvider;
    class LabelMarkerItem;

    using ListLabel = std::optional<std::string>;

    class ListViewWithLabels : public ListViewWithArrows
    {
      public:
        ListViewWithLabels(Item *parent,
                           unsigned int x,
                           unsigned int y,
                           unsigned int w,
                           unsigned int h,
                           std::shared_ptr<ListItemProvider> prov);

        void reset() override;

      private:
        [[nodiscard]] std::size_t getSlotsLeft() const;
        void addItemsOnPage() override;
        void addLabelMarker(ListItem *item);
        void updateState(ListLabel newMarker);
        LabelMarkerItem *createMarkerItem(ListLabel label);

        ListLabel current{std::nullopt};
        ListLabel previous{std::nullopt};
        ListLabel currentMarker{std::nullopt};
        std::uint32_t itemsOnPage{0};
        bool labelAdded{false};
    };
} // namespace gui
