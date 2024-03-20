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

    class ListViewWithLabels : public ListViewWithArrows
    {
      public:
        ListViewWithLabels(Item *parent,
                           unsigned int x,
                           unsigned int y,
                           unsigned int w,
                           unsigned int h,
                           std::shared_ptr<ListItemProvider> prov);

      private:
        [[nodiscard]] std::size_t getSlotsLeft() const;
        void addItemsOnPage() override;
        void addItems();
        void addLabelItem();
        void getLabels();
        std::uint32_t getLabelsCount(unsigned int index);
        LabelMarkerItem *createMarkerItem(const std::string &label);

        std::vector<std::pair<std::string, std::uint32_t>> labelFiles;
        unsigned int currentFocusIndex{0};
        unsigned int hiddenItemIndex{0};
        std::uint32_t itemsOnPage{0};
        std::uint32_t labelsCount{0};
        bool wasSetFocus{false};
    };
} // namespace gui
