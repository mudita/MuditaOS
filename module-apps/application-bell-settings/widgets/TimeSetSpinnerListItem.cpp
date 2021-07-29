// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellSettingsStyle.hpp"
#include "TimeSetSpinnerListItem.hpp"

#include <gui/input/InputEvent.hpp>
#include <widgets/TimeSetFmtSpinner.hpp>

namespace gui
{
    TimeSetSpinnerListItem::TimeSetSpinnerListItem(
        gui::Length x, gui::Length y, gui::Length w, gui::Length h, std::string description)
        : SideListItem(std::move(description))
    {
        setMinimumSize(style::sidelistview::list_item::w, style::sidelistview::list_item::h);
        timeSetFmtSpinner = new TimeSetFmtSpinner(body);
        timeSetFmtSpinner->setMinimumSize(gui::bell_settings_style::time_set_spinner_list_item::w,
                                          gui::bell_settings_style::time_set_spinner_list_item::h);
        setFocusItem(body);

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            body->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };

        focusChangedCallback = [&](Item &item) {
            setFocusItem(focus ? body : nullptr);
            return true;
        };

        inputCallback = [&](Item &, const InputEvent &inputEvent) -> bool { return body->onInput(inputEvent); };
    }
} /* namespace gui */
