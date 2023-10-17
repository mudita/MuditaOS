// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ListViewEngine.hpp"
#include "ListItemProvider.hpp"
#include "BoxLayout.hpp"

namespace gui
{

    class ListViewScroll : public Rect
    {
      private:
        unsigned int storedStartIndex = 0;
        unsigned int currentPage      = listview::nPos;
        unsigned int pagesCount       = 0;
        int topMargin                 = style::margins::big;

        void updateProportional(const ListViewScrollUpdateData &data);
        void updateFixed(const ListViewScrollUpdateData &data);
        void updatePreRendered(const ListViewScrollUpdateData &data);

      public:
        listview::ScrollBarType type = listview::ScrollBarType::None;

        ListViewScroll(
            Item *parent, unsigned int x, unsigned int y, unsigned int w, unsigned int h, listview::ScrollBarType type);

        bool shouldShowScroll(unsigned int listPageSize, unsigned int elementsCount);
        void updateStartConditions(const ListViewScrollSetupData &data);
        void update(const ListViewScrollUpdateData &data);
        void setTopMargin(int _topMargin);
    };

    class ListView : public Rect, public ListViewEngine
    {
      protected:
        ListViewScroll *scroll = nullptr;
        void applyScrollCallbacks();
        void determineFetchType(listview::ScrollBarType scrollType);
        void setFocus() override;

      public:
        ListView(Item *parent,
                 unsigned int x,
                 unsigned int y,
                 unsigned int w,
                 unsigned int h,
                 std::shared_ptr<ListItemProvider> prov,
                 listview::ScrollBarType scrollType = listview::ScrollBarType::Proportional);

        void setScrollTopMargin(int value);
        void clear() override;
        void setAlignment(const Alignment &value) override;

        // virtual methods from Item
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
        auto handleRequestResize(const Item *, Length request_w, Length request_h) -> Size override;
    };

} /* namespace gui */
