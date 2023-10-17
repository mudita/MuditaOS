// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CustomCheckBoxWithLabel.hpp"
#include "AlarmClockStyle.hpp"

#include <application-alarm-clock/data/AlarmsData.hpp>

#include <InputEvent.hpp>

namespace gui
{
    CustomCheckBoxWithLabel::CustomCheckBoxWithLabel(const std::string &description,
                                                     bool initialState,
                                                     const std::function<void(const UTF8 &text)> &navBarTemporaryMode,
                                                     const std::function<void()> &navBarRestoreFromTemporaryMode)
        : initialState(initialState)
    {
        setEdges(RectangleEdge::None);
        setMinimumSize(style::window::default_body_width, style::window::label::big_h);
        setMargins(gui::Margins(style::widgets::leftMargin, style::margins::big, 0, 0));

        checkBoxWithLabel = new gui::CheckBoxWithLabel(
            this, 0, 0, 0, 0, description, navBarTemporaryMode, navBarRestoreFromTemporaryMode, nav_bar::Side::Left);

        inputCallback = [&]([[maybe_unused]] Item &item, const InputEvent &event) {
            return checkBoxWithLabel->onInput(event);
        };

        focusChangedCallback = [&](Item &item) {
            setFocusItem(focus ? checkBoxWithLabel : nullptr);
            return true;
        };

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            checkBoxWithLabel->setArea({0, 0, newDim.w, newDim.h});
            checkBoxWithLabel->setChecked(this->initialState);
            return true;
        };
    }

    bool CustomCheckBoxWithLabel::isChecked()
    {
        return checkBoxWithLabel->isChecked();
    }

} // namespace gui
