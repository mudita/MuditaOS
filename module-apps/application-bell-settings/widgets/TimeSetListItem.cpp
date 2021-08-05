// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TimeSetListItem.hpp"
#include "BellSettingsStyle.hpp"

#include <gui/input/InputEvent.hpp>
#include <widgets/TimeSetFmtSpinner.hpp>

namespace gui
{
    TimeSetListItem::TimeSetListItem(
        gui::Length x, gui::Length y, gui::Length w, gui::Length h, std::string description)
        : SideListItem(std::move(description))
    {
        setMinimumSize(style::sidelistview::list_item::w, style::sidelistview::list_item::h);
        timeSetFmtSpinner = new TimeSetFmtSpinner(body);
        timeSetFmtSpinner->setMinimumSize(bell_settings_style::time_set_spinner_list_item::w,
                                          bell_settings_style::time_set_spinner_list_item::h);

        timeSetFmtSpinner->setMargins(calculateMargins());
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
    Margins TimeSetListItem::calculateMargins() const noexcept
    {
        constexpr auto ratio = 4;
        constexpr Position availableHeight =
            style::sidelistview::list_item::h -
            (bell_settings_style::time_fmt_set_list_item::h + style::sidelistview::top_message::h);
        return Margins{0, availableHeight / ratio, 0, 0};
    }
} /* namespace gui */
