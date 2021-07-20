// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ListViewEngine.hpp"
#include "ListItemProvider.hpp"
#include "BoxLayout.hpp"
#include <widgets/BarGraph.hpp>

namespace gui
{
    class SideListView : public Rect, public ListViewEngine
    {
      protected:
        HBarGraph *pageBar  = nullptr;
        HBox *arrowsOverlay = nullptr;
        VBox *bodyOverlay   = nullptr;

        auto createArrowsOverlay(unsigned int x, unsigned y, unsigned int w, unsigned int h) -> void;
        auto createPageBar() -> void;
        auto setFocus() -> void override;
        auto applyScrollCallbacks() -> void;

      public:
        SideListView(Item *parent,
                     unsigned int x,
                     unsigned int y,
                     unsigned int w,
                     unsigned int h,
                     std::shared_ptr<ListItemProvider> prov);

        auto onInput(const InputEvent &inputEvent) -> bool override;
    };
} /* namespace gui */
