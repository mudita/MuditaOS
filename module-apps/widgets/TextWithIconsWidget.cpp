// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TextWithIconsWidget.hpp"
#include <Style.hpp>
#include <TextFixedSize.hpp>
#include <Image.hpp>
#include "Application.hpp"

using namespace gui;

TextWithIconsWidget::TextWithIconsWidget(gui::Item *parent)
{
    assert(parent);
    setMinimumSize(style::window::default_body_width, style::widgets::h);
    setEdges(gui::RectangleEdge::None);
    setAlignment(Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
    parent->addWidget(this);
}
void TextWithIconsWidget::addIcon(ImageBox *icon)
{
    assert(icon);
    icon->setMargins(Margins(0, 0, style::widgets::rightMargin, 0));
    addWidget(icon);
    icons.push_back(icon);
}

void TextWithIconsWidget::addText(const std::string &text, const UTF8 &font)
{
    auto numberText = new TextFixedSize(this, 0, 0, 0, 0);
    numberText->setUnderline(false);
    numberText->setMaximumSize(style::window::default_body_width, style::widgets::h);
    numberText->setFont(font);
    numberText->setEdges(gui::RectangleEdge::None);
    numberText->setEditMode(EditMode::Browse);
    numberText->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
    numberText->setMargins(Margins(style::widgets::leftMargin, 0, 0, 0));
    numberText->setRichText(text);
    numberText->activeItem = false;
}
