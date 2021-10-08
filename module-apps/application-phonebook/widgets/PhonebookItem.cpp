// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhonebookItem.hpp"
#include "application-phonebook/data/PhonebookStyle.hpp"
#include <Style.hpp>
#include "Image.hpp"

namespace gui
{

    PhonebookItem::PhonebookItem()
    {
        setMargins(Margins(0, style::margins::big, 0, 0));
        setMinimumSize(phonebookStyle::contactItem::w, phonebookStyle::contactItem::h);

        setEdges(RectangleEdge::Bottom | RectangleEdge::Top);

        hBox = new gui::HBox(this, 0, 0, 0, 0);
        hBox->setEdges(gui::RectangleEdge::None);
        hBox->setPenFocusWidth(style::window::default_border_focus_w);
        hBox->setPenWidth(style::window::default_border_rect_no_focus);

        contactName = new gui::Label(hBox, 0, 0, 0, 0);
        contactName->setPenFocusWidth(0);
        contactName->setPenWidth(0);
        contactName->setFont(style::window::font::small);
        contactName->setPadding(Padding(style::padding::default_left_text_padding, 0, 0, 0));
        contactName->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        contactName->setMinimumHeight(phonebookStyle::contactItem::h);
        contactName->setMaximumWidth(phonebookStyle::contactItem::w);

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            hBox->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
    }

    // sets copy of alarm's
    void PhonebookItem::setContact(std::shared_ptr<ContactRecord> contact)
    {
        this->contact = contact;
        /* alternativeName is used as Surname or Second name */
        contactName->setText(contact->getFormattedName(ContactRecord::NameFormatType::List));
        markFavourite(contact->isOnFavourites());
        markBlocked(contact->isOnBlocked());
    }

    void PhonebookItem::setMarkerItem(UTF8 text)
    {
        contactName->setText(text);
        contactName->setLineMode(true);
        activeItem = false;
        setEdges(RectangleEdge::None);
    }

    UTF8 PhonebookItem::getLabelMarker()
    {
        if (favourite && (labeMarkerDisplayMode == LabelMarkerDisplayMode::IncludeFavourites)) {
            // If contact is favorite return proper UTF string
            return phonebookStyle::contactItem::favourites_string;
        }
        else {
            // else return first surname contact letter
            return contact->alternativeName.substr(0, 1);
        }
    }

    void PhonebookItem::markFavourite(bool val)
    {
        favourite = val;
        if (val)
            contactName->setFont(style::window::font::bigbold);
        else
            contactName->setFont(style::window::font::big);
    }

    void PhonebookItem::markBlocked(bool val)
    {
        if (val) {
            auto blockedIcon = new Image(hBox, 0, 0, "block");
            blockedIcon->setAlignment(Alignment(gui::Alignment::Vertical::Center));
            blockedIcon->setMargins(Margins(0, 0, phonebookStyle::contactItem::blocked_right_margin, 0));
        }
    }

    void PhonebookItem::setLabelMarkerDisplayMode(LabelMarkerDisplayMode mode)
    {
        labeMarkerDisplayMode = mode;
    }

} /* namespace gui */
