// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ListViewEngine.hpp"
#include "ListItemProvider.hpp"
#include "BoxLayout.hpp"
#include "ScrollBar.hpp"

namespace gui
{
    class ListView : public Rect, public ListViewEngine
    {
      protected:
        ListScrollBar *scroll = nullptr;
        void applyScrollCallbacks();
        void determineFetchType(ScrollBarType scrollType);
        void setFocus() override;

      public:
        ListView(Item *parent,
                 unsigned int x,
                 unsigned int y,
                 unsigned int w,
                 unsigned int h,
                 std::shared_ptr<ListItemProvider> prov,
                 ScrollBarType scrollType = ScrollBarType::Proportional);

        void setScrollTopMargin(int value);
        void clear() override;
        void setAlignment(const Alignment &value) override;

        // virtual methods from Item
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
        auto handleRequestResize(const Item *, Length request_w, Length request_h) -> Size override;
    };

} /* namespace gui */
