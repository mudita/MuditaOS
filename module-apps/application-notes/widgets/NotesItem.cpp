// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotesItem.hpp"

#include <Style.hpp>
#include <module-apps/application-notes/style/NotesListStyle.hpp>

#include <time/time_conversion_factory.hpp>

namespace gui
{
    NotesItem::NotesItem(std::shared_ptr<NotesRecord> record) : note{std::move(record)}
    {
        buildInterface();
    }

    void NotesItem::buildInterface()
    {
        namespace notesItemStyle = app::notes::style::list::item;
        setMinimumSize(style::window::default_body_width, notesItemStyle::Height);
        setMaximumSize(style::window::default_body_width, notesItemStyle::Height);
        setRadius(notesItemStyle::Radius);
        setEdges(RectangleEdge::Bottom | RectangleEdge::Top);
        setPenFocusWidth(style::window::default_border_focus_w);
        setPenWidth(style::window::default_border_no_focus_w);

        date = new gui::Label(this, 0, 0, 0, 0);
        date->setEdges(RectangleEdge::None);
        date->setFont(style::window::font::small);
        date->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Top});

        title   = createTextField(this, style::window::font::bigbold);
        snippet = createTextField(this, style::window::font::small);
    }

    gui::TextFixedSize *NotesItem::createTextField(Item *parent, const UTF8 &fontName)
    {
        auto item = new gui::TextFixedSize(parent, 0, 0, 0, 0);
        item->setFont(fontName);
        item->setEdges(gui::RectangleEdge::None);
        item->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top});
        item->setPenFocusWidth(::style::window::default_border_focus_w);
        item->setPenWidth(::style::window::default_border_rect_no_focus);
        item->setEditMode(gui::EditMode::Browse);
        item->setCursorStartPosition(CursorStartPosition::DocumentBegin);
        item->drawUnderline(false);
        return item;
    }

    void NotesItem::setSnippet(const UTF8 &noteText)
    {
        title->setText(noteText);
        snippet->setText(noteText);
    }

    void NotesItem::setDateText(std::uint32_t timestamp)
    {
        using namespace utils::time;
        auto dt = TimestampFactory().createTimestamp(TimestampType::DateOrTime, timestamp);
        date->setText(*dt);
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

        // Temporary fix - text is loading really long if no size for it provided.
        setSnippet(note->snippet);
        setDateText(note->date);

        return true;
    }
} // namespace gui
