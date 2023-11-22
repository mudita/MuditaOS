// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DateSetListItem.hpp"
#include "BellSettingsStyle.hpp"

#include <gui/input/InputEvent.hpp>
#include <widgets/DateSetSpinner.hpp>

namespace gui
{
    DateSetListItem::DateSetListItem(const UTF8 &topDesc, DateSetSpinner::Type type) : BellSideListItem()
    {
        setMinimumSize(style::sidelistview::list_item::w, style::sidelistview::list_item::h);

        auto title = new TextFixedSize(body->firstBox);
        title->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::outer_layouts_h);
        title->setFont(style::bell_sidelist_item::title_font);
        title->setEdges(gui::RectangleEdge::None);
        title->activeItem = false;
        title->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        title->setRichText(topDesc);
        title->drawUnderline(false);

        dateSetSpinner = new DateSetSpinner(body->getCenterBox(), type, title, 0, 0, 0, 0);
        dateSetSpinner->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::h);
        dateSetSpinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        setFocusItem(body);

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            body->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };

        focusChangedCallback = [&](Item &) {
            setFocusItem(focus ? body : nullptr);
            if (focus) {
                setFocusItem(body);
            }
            else {
                setFocusItem(nullptr);
                if (onNextCallback) {
                    onNextCallback(*this);
                }
            }
            return true;
        };

        inputCallback = [&](Item &, const InputEvent &inputEvent) -> bool { return body->onInput(inputEvent); };
    }

    YearSetListItem::YearSetListItem(const UTF8 &topDesc) : DateSetListItem(topDesc, DateSetSpinner::Type::year){};

    MonthSetListItem::MonthSetListItem(const UTF8 &topDesc) : DateSetListItem(topDesc, DateSetSpinner::Type::month){};

    DaySetListItem::DaySetListItem(const UTF8 &topDesc) : DateSetListItem(topDesc, DateSetSpinner::Type::day){};

} /* namespace gui */
