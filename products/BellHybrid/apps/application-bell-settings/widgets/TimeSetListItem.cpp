// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TimeSetListItem.hpp"
#include "BellSettingsStyle.hpp"

#include <gui/input/InputEvent.hpp>
#include <widgets/TimeSetFmtSpinner.hpp>

namespace
{
    constexpr auto focusFontName   = style::window::font::supersizeme;
    constexpr auto noFocusFontName = style::window::font::supersizemelight;
} /* namespace */

namespace gui
{
    TimeSetListItem::TimeSetListItem(
        gui::Length x, gui::Length y, gui::Length w, gui::Length h, std::string description)
    {
        setupTopTextBox(description);
        setMinimumSize(style::sidelistview::list_item::w, style::sidelistview::list_item::h);
        timeSetSpinner = new TimeSetFmtSpinner(body->getCenterBox());
        timeSetSpinner->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::h);
        timeSetSpinner->setFont(focusFontName, noFocusFontName);
        timeSetSpinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        setFocusItem(body);

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            body->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };

        focusChangedCallback = [&](Item &item) {
            if (getTimeFormat != nullptr) {
                timeSetSpinner->setTimeFormat(getTimeFormat());
                body->resizeItems();
            }
            setFocusItem(focus ? body : nullptr);
            return true;
        };

        inputCallback = [&](Item &, const InputEvent &inputEvent) -> bool { return body->onInput(inputEvent); };
    }
} /* namespace gui */
