// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SettingsSoundItem.hpp"

#include <TextFixedSize.hpp>

namespace gui
{

    SettingsSoundItem::SettingsSoundItem(const std::string &soundName, bool selected)
    {
        setMinimumSize(style::window::default_body_width, style::window::label::big_h);
        setMargins(Margins(0, style::margins::big, 0, 0));

        mMainBox = new HBox(this, 0, 0, 0, 0);
        mMainBox->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        mMainBox->setEdges(RectangleEdge::None);

        TextFixedSize *soundTitle = new TextFixedSize(mMainBox, 0, 0, 0, 0);
        soundTitle->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        soundTitle->setMaximumSize(style::window::default_body_width, style::window::label::big_h);
        soundTitle->setMargins(Margins(10, 0, 0, 0));
        soundTitle->setEdges(RectangleEdge::All);
        soundTitle->drawUnderline(false);
        soundTitle->setFont(style::window::font::big);
        soundTitle->setText(soundName); // setRichText??

        if (selected) {
            mIsSelected = new gui::Image(mMainBox, 0, 0, 0, 0, "small_tick_32px_W_M");
            mIsSelected->setMargins(Margins(style::margins::big, 0, 0, 0));
        }

        dimensionChangedCallback = [&]([[maybe_unused]] gui::Item &item, const BoundingBox &newDim) -> bool {
            mMainBox->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
    }
} // namespace gui
