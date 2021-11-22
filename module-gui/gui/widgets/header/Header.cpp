// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Header.hpp"
#include "Style.hpp"

#include <Text.hpp>

namespace gui::header
{
    Header::Header(Item *parent, Position x, Position y, Length w, Length h) : HThreeBox(parent, x, y, w, h)
    {
        setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        setEdges(RectangleEdge::None);

        firstBox = new HBox(this, 0, 0, 0, 0);
        firstBox->setMinimumSize(style::header::navigation_indicator::box_width, h);
        firstBox->setAlignment(Alignment(Alignment::Vertical::Top));
        firstBox->setEdges(RectangleEdge::None);
        firstBox->setVisible(false);

        centerBox = new HBox(this, 0, 0, 0, 0);
        centerBox->setEdges(RectangleEdge::None);
        centerBox->setAlignment(Alignment(gui::Alignment::Horizontal::Center));
        centerBox->setMaximumSize(w, h);

        lastBox = new HBox(this, 0, 0, 0, 0);
        lastBox->setMinimumSize(style::header::navigation_indicator::box_width, h);
        lastBox->setAlignment(Alignment(Alignment::Vertical::Top));
        lastBox->setEdges(RectangleEdge::None);
        lastBox->setVisible(false);
    }

    Item *Header::createTitle(const UTF8 &text)
    {
        title = new gui::Label(nullptr, 0, 0, 0, 0);
        title->setMaximumSize(getWidth() - 2 * style::header::title::margins, getHeight());
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
        firstBox->setVisible(true);
        lastBox->setVisible(true);
        resizeItems();
    }

    void Header::hideOuterBoxes()
    {
        if (lastBox->empty() && firstBox->empty()) {
            lastBox->setVisible(false);
            firstBox->setVisible(false);
        }
    }

    void Header::addToLeftBox(Item *item)
    {
        firstBox->erase();
        showOuterBoxes();
        firstBox->addWidget(item);

        firstBox->resizeItems();
    }

    void Header::addToCenterBox(Item *item)
    {
        centerBox->erase();
        centerBox->addWidget(item);

        centerBox->resizeItems();
    }

    void Header::addToRightBox(Item *item)
    {
        lastBox->erase();
        showOuterBoxes();
        lastBox->addWidget(item);

        lastBox->resizeItems();
    }

    void Header::setTitle(const UTF8 &text)
    {
        addToCenterBox(createTitle(text));

        setEdges(RectangleEdge::Bottom);
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
            firstBox->erase();
            break;
        case BoxSelection::Center:
            centerBox->erase();
            break;
        case BoxSelection::Right:
            lastBox->erase();
            break;
        }

        hideOuterBoxes();
        resizeItems();
    }

    bool Header::navigationIndicatorVisible(BoxSelection boxSelection)
    {
        switch (boxSelection) {
        case BoxSelection::Left:
            return firstBox->visible && !firstBox->empty();
        case BoxSelection::Center:
            return centerBox->visible && !centerBox->empty();
        case BoxSelection::Right:
            return lastBox->visible && !lastBox->empty();
        }
        return false;
    }
} // namespace gui::header
