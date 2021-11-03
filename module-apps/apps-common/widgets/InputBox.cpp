// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "InputBox.hpp"

#include <BoxLayout.hpp>
#include <Label.hpp>
#include <Image.hpp>

namespace gui
{
    constexpr uint32_t default_x = style::window::default_left_margin;
    constexpr uint32_t default_w =
        style::window_width - style::window::default_left_margin - style::window::default_right_margin;
    namespace inputHeader
    {
        constexpr uint32_t x = style::window::default_left_margin;
        constexpr uint32_t y = 127;
        constexpr uint32_t w = default_w;
        constexpr uint32_t h = 20;
    } // namespace inputHeader

    namespace horizontalBox
    {
        constexpr uint32_t x        = style::window::default_left_margin;
        constexpr uint32_t y        = 153;
        constexpr uint32_t w        = default_w;
        constexpr uint32_t h        = 33;
        constexpr uint32_t penWidth = 2;
        namespace searchTop
        {
            constexpr uint32_t x = default_w;
            constexpr uint32_t y = 1;
            constexpr uint32_t w = 32;
            constexpr uint32_t h = 32;
        } // namespace searchTop
        namespace inputField
        {
            constexpr uint32_t x = 1;
            constexpr uint32_t y = 1;
            constexpr uint32_t w = default_w - searchTop::w;
            constexpr uint32_t h = 33;
        } // namespace inputField
    }     // namespace horizontalBox
} // namespace gui

namespace gui
{

    auto inputBox(gui::Item *parent, const std::string &header, const std::string &icon) -> gui::Text *
    {
        auto inputField = new Text(nullptr,
                                   horizontalBox::inputField::x,
                                   horizontalBox::inputField::y,
                                   horizontalBox::inputField::w,
                                   horizontalBox::inputField::h);
        inputField->setTextType(TextType::SingleLine);
        inputField->setEditMode(EditMode::Edit);
        inputField->setEdges(RectangleEdge::None);
        inputField->setInputMode(new InputMode({InputMode::ABC, InputMode::abc}));
        inputField->setFont(style::window::font::mediumbold);

        auto horizontalBox = new HBox(parent, horizontalBox::x, horizontalBox::y, horizontalBox::w, horizontalBox::h);
        horizontalBox->addWidget(inputField);
        if (!icon.empty()) {
            horizontalBox->addWidget(new Image(nullptr,
                                               horizontalBox::searchTop::x,
                                               horizontalBox::searchTop::y,
                                               horizontalBox::searchTop::w,
                                               horizontalBox::searchTop::h,
                                               icon));
        }

        horizontalBox->setPenWidth(horizontalBox::penWidth);
        horizontalBox->setEdges(RectangleEdge::Bottom);

        const RectangleEdge edges = RectangleEdge::None;
        const Alignment alignment = Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Bottom);
        auto l                    = new Label(parent, inputHeader::x, inputHeader::y, inputHeader::w, inputHeader::h);

        l->setFont(style::window::font::small);
        l->setEdges(edges);
        l->setText(header);
        l->setAlignment(alignment);

        return inputField;
    }
} // namespace gui
