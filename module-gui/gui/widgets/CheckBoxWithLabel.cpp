// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CheckBoxWithLabel.hpp"

namespace gui
{
    CheckBoxWithLabel::CheckBoxWithLabel(
        Item *parent, int x, int y, UTF8 description, std::function<void(CheckBoxWithLabel &)> clickCallback)
    {
        auto body = new gui::HBox(nullptr, x, y, style::window::default_body_width, style::window::label::big_h);
        body->setEdges(gui::RectangleEdge::None);

        label =
            new gui::Label(nullptr, 0, 0, style::checkbox::description_width, style::window::label::big_h, description);
        check = new gui::CheckBox(nullptr,
                                  style::checkbox::check_x,
                                  style::checkbox::check_y,
                                  style::checkbox::check_width,
                                  style::window::label::big_h);

        label->activatedCallback = [=](Item &item) {
            setChecked(!check->isChecked());
            if (clickCallback) {
                clickCallback(*this);
            }
            return true;
        };

        style::window::decorateOption(label);
        style::window::decorate(check);

        /*
         * We need to add widgets after decorate to correctly redraw them
         */
        parent->addWidget(body);
        body->addWidget(label);
        label->addWidget(check);
    }

    bool CheckBoxWithLabel::isChecked() const
    {
        return check->isChecked();
    }

    void CheckBoxWithLabel::setChecked(bool state)
    {
        check->setImageVisible(state);
    }
} // namespace gui
