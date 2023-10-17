// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
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

    void PhonebookItem::setContact(std::shared_ptr<ContactRecord> contactRecord)
    {
        contact = std::move(contactRecord);
        contactName->setText(contact->getFormattedName(ContactRecord::NameFormatType::List));
        contactName->setFont(style::window::font::big);
        favourite = contact->isOnFavourites();
        markBlocked(contact->isOnBlocked());
    }

    UTF8 PhonebookItem::getLabelMarker()
    {
        if (favourite && (labelMarkerDisplayMode == LabelMarkerDisplayMode::IncludeFavourites)) {
            // If contact is favorite return proper UTF string
            return utils::translate("app_phonebook_favorite_contacts_title");
        }
        else if (!contact->alternativeName.empty()) {
            // else if return first surname contact letter
            return contact->alternativeName.substr(0, 1);
        }
        else {
            // else return first firstname contact letter
            return contact->primaryName.substr(0, 1);
        }
    }

    void PhonebookItem::markBlocked(bool val)
    {
        if (val) {
            auto blockedIcon = new Image(hBox, 0, 0, "phonebook_blocked_32px_W_G");
            blockedIcon->setAlignment(Alignment(gui::Alignment::Vertical::Center));
            blockedIcon->setMargins(Margins(0, 0, phonebookStyle::contactItem::blocked_right_margin, 0));
        }
    }

    void PhonebookItem::setLabelMarkerDisplayMode(LabelMarkerDisplayMode mode)
    {
        labelMarkerDisplayMode = mode;
    }

    PhonebookMarkItem::PhonebookMarkItem(const UTF8 &markText)
    {
        setMargins(Margins(0, style::margins::big, 0, 0));
        setMinimumSize(phonebookStyle::contactItem::w, phonebookStyle::contactItem::h);

        setEdges(RectangleEdge::None);
        activeItem = false;

        markLabel = new TextWithSnippet(this, style::window::font::small);
        markLabel->setPadding(Padding(style::padding::default_left_text_padding, 0, 0, 0));
        markLabel->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        markLabel->setMinimumHeight(phonebookStyle::contactItem::h);
        markLabel->setMaximumWidth(phonebookStyle::contactItem::w);
        markLabel->textWidget->setText(markText);

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            markLabel->setArea({0, 0, newDim.w, newDim.h});
            markLabel->resizeItems();
            return true;
        };
    }
} /* namespace gui */
