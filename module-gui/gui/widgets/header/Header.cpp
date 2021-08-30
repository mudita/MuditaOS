// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Header.hpp"
#include "Style.hpp"

#include <Text.hpp>

namespace gui::header
{
    Header::Header(Item *parent, Position x, Position y, Length w, Length h) : HBox(parent, x, y, w, h)
    {
        setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        setEdges(RectangleEdge::None);

        leftBox = new HBox(this, 0, 0, 0, 0);
        leftBox->setMinimumSize(style::header::navigation_indicator::box_width, h);
        leftBox->setAlignment(Alignment(Alignment::Vertical::Top));
        leftBox->setEdges(RectangleEdge::None);
        leftBox->setVisible(false);

        centerBox = new HBox(this, 0, 0, 0, 0);
        centerBox->setEdges(RectangleEdge::None);
        centerBox->setAlignment(Alignment(gui::Alignment::Horizontal::Center));
        centerBox->setMaximumSize(w, h);

        rightBox = new HBox(this, 0, 0, 0, 0);
        rightBox->setMinimumSize(style::header::navigation_indicator::box_width, h);
        rightBox->setAlignment(Alignment(Alignment::Vertical::Top));
        rightBox->setEdges(RectangleEdge::None);
        rightBox->setVisible(false);
    }

    Item *Header::createTitle(const UTF8 &text)
    {
        title = new gui::Label(nullptr, 0, 0, 0, 0);
        title->setMaximumSize(style::window::default_body_width, getHeight());
        title->setFont(style::header::font::title);
        title->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
        title->setPadding(gui::Padding(0, style::header::title::top_padding, 0, 0));
        title->setEdges(RectangleEdge::None);
        title->setEllipsis(Ellipsis::Right);
        title->setVisible(true);
        title->setText(text);
        return title;
    }

    void Header::showOuterBoxes()
    {
        leftBox->setVisible(true);
        rightBox->setVisible(true);
        resizeItems();
    }

    void Header::hideOuterBoxes()
    {
        if (rightBox->empty() && leftBox->empty()) {
            rightBox->setVisible(false);
            leftBox->setVisible(false);
        }
    }

    void Header::addToLeftBox(Item *item)
    {
        leftBox->erase();
        showOuterBoxes();
        leftBox->addWidget(item);

        leftBox->resizeItems();
    }

    void Header::addToCenterBox(Item *item)
    {
        centerBox->erase();
        centerBox->addWidget(item);

        centerBox->resizeItems();
    }

    void Header::addToRightBox(Item *item)
    {
        rightBox->erase();
        showOuterBoxes();
        rightBox->addWidget(item);

        rightBox->resizeItems();
    }

    void Header::setTitle(const UTF8 &text)
    {
        if (!text.empty()) {
            addToCenterBox(createTitle(text));
            setEdges(RectangleEdge::Bottom);
        }
        else {
            centerBox->erase();
            setEdges(RectangleEdge::None);
        }

        resizeItems();
    }

    UTF8 Header::getTitle()
    {
        return title ? title->getText() : "";
    }

    void Header::setTitleVisibility(bool visibility)
    {
        if (title) {
            visibility ? setEdges(RectangleEdge::Bottom) : setEdges(RectangleEdge::None);
            title->setVisible(visibility);
        }
    }

    void Header::navigationIndicatorAdd(Item *item, BoxSelection boxSelection)
    {
        switch (boxSelection) {
        case BoxSelection::Left:
            addToLeftBox(item);
            break;
        case BoxSelection::Center:
            addToCenterBox(item);
            break;
        case BoxSelection::Right:
            addToRightBox(item);
            break;
        }

        resizeItems();
    }

    void Header::navigationIndicatorRemove(BoxSelection boxSelection)
    {
        switch (boxSelection) {
        case BoxSelection::Left:
            leftBox->erase();
            break;
        case BoxSelection::Center:
            centerBox->erase();
            break;
        case BoxSelection::Right:
            rightBox->erase();
            break;
        }

        hideOuterBoxes();
        resizeItems();
    }

    bool Header::navigationIndicatorVisible(BoxSelection boxSelection)
    {
        switch (boxSelection) {
        case BoxSelection::Left:
            return leftBox->visible;
        case BoxSelection::Center:
            return centerBox->visible;
        case BoxSelection::Right:
            return rightBox->visible;
        }
        return false;
    }
} // namespace gui::header
