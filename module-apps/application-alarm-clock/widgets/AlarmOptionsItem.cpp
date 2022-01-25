// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmOptionsItem.hpp"

#include "AlarmClockStyle.hpp"
#include <InputEvent.hpp>
#include <Style.hpp>

namespace gui
{
    AlarmOptionsItem::AlarmOptionsItem(const std::string &description, const std::vector<UTF8> &options)
    {
        setMinimumSize(style::listview::item_width_with_without_scroll, style::text_spinner_label::h);

        setEdges(RectangleEdge::None);
        setMargins(gui::Margins(style::widgets::leftMargin, style::margins::large, 0, 0));

        optionSpinner = new gui::TextSpinnerBoxWithLabel(this, description, options, Boundaries::Continuous);

        focusChangedCallback = [&](gui::Item &item) {
            setFocusItem(focus ? optionSpinner : nullptr);
            return true;
        };

        inputCallback = [&](Item &item, const InputEvent &event) { return optionSpinner->onInput(event); };

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            optionSpinner->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
    }
} /* namespace gui */
