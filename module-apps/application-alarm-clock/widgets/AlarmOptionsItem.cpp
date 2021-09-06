// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmOptionsItem.hpp"

#include "AlarmClockStyle.hpp"
#include <InputEvent.hpp>
#include <Style.hpp>

namespace gui
{
    AlarmOptionsItem::AlarmOptionsItem(const std::string &description, const std::vector<UTF8> &options)
    {
        setMinimumSize(style::listview::item_width_with_without_scroll, style::alarmClock::window::item::options::h);

        setEdges(RectangleEdge::None);
        setMargins(gui::Margins(style::widgets::leftMargin, style::margins::large, 0, 0));

        vBox = new gui::VBox(this, 0, 0, 0, 0);
        vBox->setEdges(gui::RectangleEdge::None);
        vBox->activeItem = false;

        descriptionLabel = new gui::Label(vBox, 0, 0, 0, 0);
        descriptionLabel->setMinimumSize(style::window::default_body_width,
                                         style::alarmClock::window::item::options::label_h);
        descriptionLabel->setEdges(gui::RectangleEdge::None);
        descriptionLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top));
        descriptionLabel->setFont(style::window::font::small);
        descriptionLabel->activeItem = false;
        descriptionLabel->setText(description);

        optionSpinner = new gui::TextSpinnerBox(vBox, options, Boundaries::Continuous);
        optionSpinner->setMinimumSize(style::window::default_body_width,
                                      style::alarmClock::window::item::options::spinner_h);

        focusChangedCallback = [&](gui::Item &item) {
            setFocusItem(focus ? optionSpinner : nullptr);
            return true;
        };

        inputCallback = [&](Item &item, const InputEvent &event) { return optionSpinner->onInput(event); };

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            vBox->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
    }
} /* namespace gui */
