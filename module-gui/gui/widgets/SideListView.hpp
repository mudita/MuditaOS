// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BoxLayout.hpp"
#include "InputEvent.hpp"
#include "Label.hpp"
#include "SideListItemProvider.hpp"
#include <apps-common/widgets/BarGraph.hpp>

namespace gui
{
    enum class Order;

    class SideListView : public Rect
    {
      protected:
        static constexpr auto progressbarStartValue = 1;

        std::shared_ptr<SideListItemProvider> provider = nullptr;
        HBarGraph *progressbar                         = nullptr;
        HBox *listItemBox                              = nullptr;

        [[nodiscard]] auto slide(Order order) -> bool;
        auto clear() -> void;

      public:
        SideListView();
        SideListView(Item *parent,
                     unsigned int x,
                     unsigned int y,
                     unsigned int w,
                     unsigned int h,
                     std::shared_ptr<SideListItemProvider> prov);
        ~SideListView();

        auto onInput(const gui::InputEvent &inputEvent) -> bool override;
    };
} /* namespace gui */
