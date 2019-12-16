/*
 * @file PhonebookItem.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 10 wrz 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "PhonebookItem.hpp"
#include <Style.hpp>

namespace gui
{

PhonebookItem::PhonebookItem()
{
    minWidth = 436;
    minHeight = style::window::label::big_h;
    maxWidth = 436;
    maxHeight = 200;

    setRadius(0);
    setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);

    setPenFocusWidth(2);
    setPenWidth(0);

    value = new gui::Label(this, 0, 0, 0, 0);
    value->setPenFocusWidth(0);
    value->setPenWidth(0);
    value->setFont(style::window::font::medium);
    value->setAlignement(gui::Alignment{gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_CENTER});
}

PhonebookItem::~PhonebookItem()
{
    if (value)
    {
        removeWidget(value);
        delete value;
        value = nullptr;
    }
}

bool PhonebookItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
{
    value->setPosition(11, 0);
    value->setSize(newDim.w - 22, newDim.h);

    return true;
}

// sets copy of alarm's
void PhonebookItem::setContact(std::shared_ptr<ContactRecord> contact)
{
    this->contact = contact;
    /* alternativeName is used as Surname or Second name */
    value->setText(contact->primaryName + " " + contact->alternativeName);
}

void PhonebookItem::setValue(UTF8 text)
{
    value->setText(text);
    value->setLineMode(true);
    setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
}

UTF8 PhonebookItem::getValue()
{
    return value->getText();
}

bool PhonebookItem::onActivated(void *data)
{
    LOG_INFO("ITEM WAS PRESSED");
    return true;
}

void PhonebookItem::markFavourite(bool val)
{
    favourite = val;
    if (val)
        value->setFont(style::window::font::bigbold);
    else
        value->setFont(style::window::font::medium);
}

std::shared_ptr<ContactRecord> PhonebookItem::getContact()
{
    return (contact);
}

} /* namespace gui */
