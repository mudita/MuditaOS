/*
 * @file NotesItem.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 12 sie 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "NotesItem.hpp"
#include <Style.hpp>

namespace gui
{

    NotesItem::NotesItem(NotesModel *model, bool mode24H) : model{model}, mode24H{mode24H}
    {
        setMinimumSize(style::window::default_body_width, 146);
        setMaximumSize(style::window::default_body_width, 146);

        setRadius(8);

        setPenFocusWidth(3);
        setPenWidth(1);

        hour = new gui::Label(this, 0, 0, 0, 0);
        hour->setPenFocusWidth(0);
        hour->setPenWidth(0);
        hour->setFont(style::window::font::medium);
        hour->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Top});

        title = new gui::Label(this, 0, 0, 0, 0);
        title->setPenFocusWidth(0);
        title->setPenWidth(0);
        title->setFont(style::window::font::bigbold);
        title->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top});

        snippet = new gui::Label(this, 0, 0, 0, 0);
        snippet->setPenFocusWidth(0);
        snippet->setPenWidth(0);
        snippet->setFont(style::window::font::small);
        snippet->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
    }

    NotesItem::~NotesItem()
    {
        note = nullptr;
    }

    bool NotesItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        hour->setPosition(11, 0);
        hour->setSize(newDim.w - 22, 40);

        title->setPosition(11, 0);
        title->setSize(68, 40);

        snippet->setPosition(11, 40);
        snippet->setSize(newDim.w - 22, newDim.h - 40);
        return true;
    }

    // sets copy of alarm's
    void NotesItem::setNote(std::shared_ptr<NotesRecord> &note)
    {
        this->note = note;
        // set values of the labels
        title->setText(std::to_string(note->ID));
        snippet->setText(note->path);
    }

    bool NotesItem::onActivated(void *data)
    {
        LOG_INFO("ITEM WAS PRESSED");
        return true;
    }

} /* namespace gui */
