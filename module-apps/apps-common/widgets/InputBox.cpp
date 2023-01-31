// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "InputBox.hpp"

#include <BoxLayout.hpp>
#include <Label.hpp>
#include <ImageBox.hpp>

namespace gui
{
    namespace input_Box
    {
        constexpr auto label_h = 30;
        constexpr auto text_h  = 33;
        constexpr auto h_box_h = text_h + 3;
        constexpr auto h       = label_h + style::margins::big + h_box_h;
    } // namespace input_Box

} // namespace gui

namespace gui
{
    auto inputBox(gui::Item *parent, const std::string &header, const std::string &icon) -> gui::Text *
    {
        auto verticalBox = new VBox(parent,
                                    style::window::default_left_margin,
                                    style::window::default_vertical_pos,
                                    style::window::default_body_width,
                                    input_Box::h);
        verticalBox->setEdges(RectangleEdge::None);

        auto label = new Label(verticalBox);
        label->setMinimumHeight(input_Box::label_h);
        label->setMaximumWidth(style::window::default_body_width);
        label->setFont(style::window::font::small);
        label->setEdges(RectangleEdge::None);
        label->setText(header);
        label->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Bottom));

        auto horizontalBox = new HBox(verticalBox);
        horizontalBox->setAlignment({gui::Alignment::Vertical::Top});
        horizontalBox->setMargins({0, style::margins::big, 0, 0});
        horizontalBox->setMinimumHeight(input_Box::h_box_h);
        horizontalBox->setMinimumWidth(style::window::default_body_width);
        horizontalBox->setPenWidth(style::window::default_border_rect_no_focus);
        horizontalBox->setEdges(RectangleEdge::Bottom);

        auto inputField = new TextFixedSize(horizontalBox);
        inputField->drawUnderline(false);
        inputField->setMinimumHeight(input_Box::text_h);
        inputField->setMaximumWidth(style::window::default_body_width);
        inputField->setTextType(TextType::SingleLine);
        inputField->setTextEllipsisType(TextEllipsis::Both);
        inputField->setEditMode(EditMode::Edit);
        inputField->setEdges(RectangleEdge::None);
        inputField->setInputMode(new InputMode({InputMode::Abc, InputMode::ABC, InputMode::abc}));
        inputField->setFont(style::window::font::mediumbold);

        if (!icon.empty()) {
            auto imageBox = new ImageBox(horizontalBox, new Image(icon));
            imageBox->setMinimumSizeToFitImage();
            imageBox->activeItem = false; // Prevent switching focus to the image
        }

        verticalBox->resizeItems();

        return inputField;
    }
} // namespace gui
