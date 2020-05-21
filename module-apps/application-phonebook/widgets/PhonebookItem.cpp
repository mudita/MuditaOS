#include "PhonebookItem.hpp"
#include <Style.hpp>

namespace gui
{

    PhonebookItem::PhonebookItem()
    {
        setMinimumSize(style::window::default_body_width, style::window::label::big_h);
        setMaximumSize(style::window::default_body_width, style::window::label::big_h);

        setRadius(0);
        setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);

        setPenFocusWidth(2);
        setPenWidth(0);

        value = new gui::Label(this, 0, 0, 0, 0);
        value->setPenFocusWidth(0);
        value->setPenWidth(0);
        value->setFont(style::window::font::small);
        value->setAlignment(
            gui::Alignment{gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_CENTER});
    }

    bool PhonebookItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        value->setPosition(10, 0);
        value->setSize(newDim.w, newDim.h);

        return true;
    }

    // sets copy of alarm's
    void PhonebookItem::setContact(std::shared_ptr<ContactRecord> contact)
    {
        this->contact = contact;
        /* alternativeName is used as Surname or Second name */
        value->setText(contact->getFormattedName(ContactRecord::NameFormatType::List));
        markFavourite(contact.get()->isOnFavourites);
    }

    void PhonebookItem::setMarkerItem(UTF8 text)
    {
        value->setText(text);
        value->setLineMode(true);
        activeItem = false;
        setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    }

    UTF8 PhonebookItem::getLabelMarker()
    {
        if (favourite) {
            return "Favourites";
        }
        else {
            return contact->alternativeName.substr(0, 1);
        }
    }

    void PhonebookItem::markFavourite(bool val)
    {
        favourite = val;
        if (val)
            value->setFont(style::window::font::bigbold);
        else
            value->setFont(style::window::font::big);
    }

} /* namespace gui */
