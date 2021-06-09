// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Header.hpp"
#include "Style.hpp"
#include "IceBox.hpp"
#include "AddElementBox.hpp"
#include "SearchBox.hpp"

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

        centralBox = new HBox(this, 0, 0, 0, 0);
        centralBox->setEdges(RectangleEdge::None);
        centralBox->setMaximumSize(w, h);

        rightBox = new HBox(this, 0, 0, 0, 0);
        rightBox->setMinimumSize(style::header::navigation_indicator::box_width, h);
        rightBox->setAlignment(Alignment(Alignment::Vertical::Top));
        rightBox->setEdges(RectangleEdge::None);
        rightBox->setVisible(false);
    }

    void Header::createTitle(const UTF8 &text)
    {
        title = new gui::Label(centralBox, 0, 0, 0, 0);
        title->setMaximumSize(getWidth(), getHeight());
        title->setFont(style::header::title_font);
        title->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
        title->setPadding(gui::Padding(0, style::header::title::top_padding, 0, 0));
        title->setEdges(RectangleEdge::None);
        title->setEllipsis(Ellipsis::Right);
        title->setVisible(true);
        title->setText(text);
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

    void Header::createIceBox()
    {
        leftBox->erase();
        showOuterBoxes();
        new gui::IceBox(leftBox);

        leftBox->resizeItems();
    }

    void Header::createAddElementBox()
    {
        leftBox->erase();
        showOuterBoxes();
        new gui::AddElementBox(leftBox);

        leftBox->resizeItems();
    }

    void Header::createSearchBox()
    {
        rightBox->erase();
        showOuterBoxes();
        new gui::SearchBox(rightBox);

        rightBox->resizeItems();
    }

    void Header::setTitle(const UTF8 &text)
    {
        centralBox->erase();
        createTitle(text);

        setEdges(RectangleEdge::Bottom);
        centralBox->resizeItems();
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

    void Header::navigationIndicatorAdd(NavigationIndicator indicator)
    {
        switch (indicator) {
        case NavigationIndicator::IceBox:
            createIceBox();
            break;
        case NavigationIndicator::AddElementBox:
            createAddElementBox();
            break;
        case NavigationIndicator::SearchBox:
            createSearchBox();
            break;
        }

        resizeItems();
    }

    void Header::navigationIndicatorRemove(NavigationIndicator indicator)
    {
        switch (indicator) {
        case NavigationIndicator::IceBox:
            leftBox->erase();
            break;
        case NavigationIndicator::AddElementBox:
            leftBox->erase();
            break;
        case NavigationIndicator::SearchBox:
            rightBox->erase();
            break;
        }

        hideOuterBoxes();
        resizeItems();
    }

    bool Header::navigationIndicatorVisible(NavigationIndicator indicator)
    {
        switch (indicator) {
        case NavigationIndicator::IceBox:
            return leftBox->visible;
        case NavigationIndicator::AddElementBox:
            return leftBox->visible;
        case NavigationIndicator::SearchBox:
            return rightBox->visible;
        }
        return false;
    }
} // namespace gui::header
