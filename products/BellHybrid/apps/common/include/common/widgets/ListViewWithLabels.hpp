// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/ApplicationCommon.hpp>
#include <ListViewWithArrows.hpp>
#include <common/data/RelaxationCommon.hpp>
//#include <common/widgets/LabelListItem.hpp>

#include <optional>
#include <map>

namespace gui
{
    class ListItemProvider;
    class LabelMarkerItem;

    using ItemsType     = std::optional<int>;
    using TypeLabelsMap = std::map<int, std::string>;

    class ListViewWithLabels : public ListViewWithArrows
    {
      public:
        ListViewWithLabels(Item *parent,
                           unsigned int x,
                           unsigned int y,
                           unsigned int w,
                           unsigned int h,
                           std::shared_ptr<ListItemProvider> prov,
                           const TypeLabelsMap labelsPerType = TypeLabelsMap{});

        void reset() override;

      private:
        std::size_t getSlotsLeft();
        void addItemsOnPage() override;
        void addLabelMarker(ListItem *item);
        void updateState(ItemsType newMarker);
        LabelMarkerItem *createMarkerItem(ItemsType type);

        ItemsType currentType{std::nullopt};
        ItemsType previousType{std::nullopt};
        ItemsType currentMarker{std::nullopt};
        std::uint32_t itemsOnPage{0};
        bool labelAdded{false};
        const TypeLabelsMap typeToLabel;
    };

} // namespace gui
