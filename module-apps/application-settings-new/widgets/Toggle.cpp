// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Toggle.hpp"

namespace gui
{
    Toggle::Toggle(Item *parent, int x, int y, int w, int h, UTF8 text, bool defaultState)
        : HBox(parent, x, y, w, h), text(std::move(text)), state(defaultState)
    {
        setEdges(gui::RectangleEdge::None);
        buildToggle();
    }

    void Toggle::doToggle()
    {
        state = !state;
        buildToggle();
    }

    bool Toggle::getState() const noexcept
    {
        return state;
    }

    void Toggle::buildToggle()
    {
        removeWidget(label);

        label = new gui::Label(nullptr,
                               0,
                               0,
                               style::window_width - (2 * style::window::default_left_margin) - toggle_width,
                               style::window::label::big_h,
                               text);
        label->setEdges(gui::RectangleEdge::None);
        label->activatedCallback = [this](Item &) {
            doToggle();
            return false;
        };

        UTF8 onOffText =
            state ? utils::localize.get("app_settings_toggle_on") : utils::localize.get("app_settings_toggle_off");

        indicator =
            new gui::Label(label, style::toggle::toggle_x, 0, toggle_width, style::window::label::big_h, onOffText);

        style::window::decorateOption(label);
        style::window::decorate(indicator);

        addWidget(label);
        setVisible(true);
    }
}; // namespace gui
