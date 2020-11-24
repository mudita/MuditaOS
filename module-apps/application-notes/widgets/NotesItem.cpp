// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotesItem.hpp"

#include <Style.hpp>
#include <module-apps/application-notes/style/NotesListStyle.hpp>

namespace gui
{
    NotesItem::NotesItem(std::shared_ptr<NotesRecord> record, bool mode24H) : note{std::move(record)}, mode24H{mode24H}
    {
        namespace notesItemStyle = app::notes::style::list::item;
        setMinimumSize(style::window::default_body_width, notesItemStyle::Height);
        setMaximumSize(style::window::default_body_width, notesItemStyle::Height);
        setRadius(notesItemStyle::Radius);
        setEdges(RectangleEdge::Bottom | RectangleEdge::Top);
        setPenFocusWidth(style::window::default_border_focus_w);
        setPenWidth(style::window::default_border_no_focus_w);

        title = createEmptyLabel(this);
        title->setFont(style::window::font::bigbold);
        title->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top});

        date = createEmptyLabel(this);
        date->setFont(style::window::font::medium);
        date->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Top});

        snippet = createEmptyLabel(this);
        snippet->setFont(style::window::font::small);
        snippet->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left});

        setSnippet(note->snippet);
        setDateText(note->date);
    }

    gui::Label *NotesItem::createEmptyLabel(Item *parent)
    {
        auto label = new gui::Label(parent, 0, 0, 0, 0);
        label->setPenFocusWidth(0);
        label->setPenWidth(0);
        return label;
    }

    void NotesItem::setSnippet(const UTF8 &noteText)
    {
        title->setText(noteText);
        snippet->setText(noteText);
    }

    void NotesItem::setDateText(std::uint32_t timestamp)
    {
        date->setText(utils::to_string(timestamp));
    }

    bool NotesItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        namespace notesItemStyle = app::notes::style::list::item;
        title->setPosition(notesItemStyle::LeftPadding, notesItemStyle::VerticalPadding);
        title->setSize(notesItemStyle::title::Width, notesItemStyle::title::Height);

        date->setPosition(notesItemStyle::LeftPadding, notesItemStyle::VerticalPadding);
        date->setSize(newDim.w - (notesItemStyle::LeftPadding + notesItemStyle::RightPadding),
                      notesItemStyle::date::Height);

        snippet->setPosition(notesItemStyle::LeftPadding,
                             notesItemStyle::VerticalPadding + notesItemStyle::title::Height +
                                 notesItemStyle::snippet::TopMargin);
        snippet->setSize(newDim.w - (notesItemStyle::LeftPadding + notesItemStyle::RightPadding),
                         newDim.h - (2 * notesItemStyle::VerticalPadding) - notesItemStyle::title::Height -
                             notesItemStyle::snippet::TopMargin);
        return true;
    }
} /* namespace gui */
