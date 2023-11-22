// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/ApplicationCommon.hpp>
#include <ListViewWithArrows.hpp>
#include "data/RelaxationCommon.hpp"

namespace gui
{
    class ListItemProvider;

    class RelaxationListView : public ListViewWithArrows
    {
        using MusicType = app::relaxation::MusicType;

      public:
        RelaxationListView(Item *parent,
                           unsigned int x,
                           unsigned int y,
                           unsigned int w,
                           unsigned int h,
                           std::shared_ptr<ListItemProvider> prov);

        void reset() override;

      private:
        void addItemsOnPage() override;
        void addLabelMarker(ListItem *item);
        std::size_t getSlotsLeft();
        void updateState(MusicType newMarker);

        MusicType currentType{MusicType::Undefined};
        MusicType previousType{MusicType::Undefined};
        MusicType currentMarker{MusicType::Undefined};
        std::uint32_t itemsOnPage{0};
        bool labelAdded{false};
    };

} // namespace gui
