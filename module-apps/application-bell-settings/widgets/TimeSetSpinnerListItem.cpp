// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellSettingsStyle.hpp"
#include "TimeSetSpinnerListItem.hpp"

#include <widgets/TimeSetSpinner.hpp>

namespace gui
{
    TimeSetSpinnerListItem::TimeSetSpinnerListItem(
        gui::Length x, gui::Length y, gui::Length w, gui::Length h, std::string description)
        : SideListItem(std::move(description))
    {
        setMinimumSize(style::sidelistview::list_item::w, style::sidelistview::list_item::h);

        timeSetSpinner = new TimeSetSpinner(body, 0, 0, 0, 0);
        timeSetSpinner->setMinimumSize(gui::bell_settings_style::time_set_spinner_list_item::w,
                                       gui::bell_settings_style::time_set_spinner_list_item::h);
        body->setFocusItem(timeSetSpinner);

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            body->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
        inputCallback = [&](gui::Item &item, const gui::InputEvent &event) { return body->onInput(event); };
    }
} /* namespace gui */
