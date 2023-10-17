// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TextWithIconsWidget.hpp"
#include <Style.hpp>
#include <TextFixedSize.hpp>
#include <Image.hpp>
#include <ApplicationCommon.hpp>

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

namespace
{
    [[nodiscard]] auto createText(const std::string &text, const UTF8 &font)
    {
        auto textFixedSize = new TextFixedSize(nullptr, 0, 0, 0, 0);
        textFixedSize->drawUnderline(false);
        textFixedSize->setMaximumSize(style::window::default_body_width, style::widgets::h);
        textFixedSize->setFont(font);
        textFixedSize->setEdges(gui::RectangleEdge::None);
        textFixedSize->setEditMode(EditMode::Browse);
        textFixedSize->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        textFixedSize->setText(text);
        textFixedSize->activeItem = false;
        return textFixedSize;
    }

    [[nodiscard]] auto createSnippet(const UTF8 &snippetName)
    {
        auto snippet = new gui::Image(snippetName);
        snippet->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        snippet->setMargins(Margins(0, 0, style::widgets::rightMargin, 0));
        snippet->activeItem = false;
        return snippet;
    }
} // namespace

void TextWithIconsWidget::addText(const std::string &text, const UTF8 &font)
{
    addWidget(createText(text, font));
}

TextWithSnippet::TextWithSnippet(const std::string &text, const UTF8 &font, const UTF8 &snippet)
{
    setMinimumSize(style::window::default_body_width, style::widgets::h);
    setEdges(gui::RectangleEdge::None);
    setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));

    addWidget(createSnippet(snippet));
    textWidget = createText(text, font);
    addWidget(textWidget);
}

TextWithSnippet::TextWithSnippet(Item *parent, const UTF8 &font, const UTF8 &snippet)
    : TextWithSnippet("", font, snippet)
{
    parent->addWidget(this);
}
