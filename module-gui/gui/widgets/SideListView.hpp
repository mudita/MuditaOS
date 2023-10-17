// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ListViewEngine.hpp"
#include "ListItemProvider.hpp"
#include "BoxLayout.hpp"
#include <apps-common/widgets/BarGraph.hpp>

namespace gui
{
    enum class PageBarType
    {
        None,
        Round,
    };

    class SideListView : public Rect, public ListViewEngine
    {
      protected:
        HBarGraph *pageBar = nullptr;
        VBox *bodyOverlay  = nullptr;

        auto createPageBar() -> void;
        auto setFocus() -> void override;
        auto applyScrollCallbacks() -> void;

      public:
        SideListView(Item *parent,
                     unsigned int x,
                     unsigned int y,
                     unsigned int w,
                     unsigned int h,
                     std::shared_ptr<ListItemProvider> prov,
                     PageBarType pageBarType = PageBarType::None);

        auto onInput(const InputEvent &inputEvent) -> bool override;
    };
} /* namespace gui */
