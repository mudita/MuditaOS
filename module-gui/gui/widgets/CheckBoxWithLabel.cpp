// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CheckBoxWithLabel.hpp"

namespace gui
{
    CheckBoxWithLabel::CheckBoxWithLabel(Item *parent,
                                         uint32_t x,
                                         uint32_t y,
                                         uint32_t w,
                                         uint32_t h,
                                         const UTF8 &description,
                                         const std::function<void(const UTF8 &text)> &navBarTemporaryMode,
                                         const std::function<void()> &navBarRestoreFromTemporaryMode,
                                         gui::nav_bar::Side textSide)
        : HBox{parent, x, y, w, h}
    {
        setEdges(gui::RectangleEdge::None);

        check = new gui::CheckBox(this, 0, 0, 0, 0, navBarTemporaryMode, navBarRestoreFromTemporaryMode, textSide);
        check->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        check->setMinimumSize(60, style::window::label::big_h);
        setChecked(true);

        label = new gui::Label(this, 0, 0, 0, 0, description);
        label->setEdges(RectangleEdge::None);
        label->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        label->setMargins(gui::Margins(style::window::default_left_margin, 0, 0, 0));
        label->setFont(style::window::font::big);
        label->setMinimumHeight(style::window::label::big_h);
        label->setMaximumWidth(style::window::default_body_width);

        focusChangedCallback = [&](Item &item) {
            if (focus) {
                label->setFont(style::window::font::bigbold);
            }
            else {
                label->setFont(style::window::font::big);
            }
            return true;
        };

        resizeItems();
    }

    bool CheckBoxWithLabel::isChecked() const
    {
        return check->isChecked();
    }

    void CheckBoxWithLabel::setChecked(bool state)
    {
        check->setCheck(state);
    }
} // namespace gui
