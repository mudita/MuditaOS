// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Interface/ContactRecord.hpp"
#include "Label.hpp"
#include "Image.hpp"
#include "ListItem.hpp"
#include <BoxLayout.hpp>
#include <TextFixedSize.hpp>

namespace gui
{

    class PhonebookItem : public ListItem
    {
        gui::Label *contactName = nullptr;
        gui::HBox *hBox         = nullptr;

        bool favourite = false;
        void markFavourite(bool val);
        void markBlocked(bool val);
        LabelMarkerDisplayMode labeMarkerDisplayMode = LabelMarkerDisplayMode::IncludeFavourites;

      public:
        std::shared_ptr<ContactRecord> contact = nullptr;

        PhonebookItem();
        virtual ~PhonebookItem() = default;

        // sets copy of contact
        void setContact(std::shared_ptr<ContactRecord> note);
        void setMarkerItem(UTF8 text);
        UTF8 getLabelMarker();

        void setLabelMarkerDisplayMode(LabelMarkerDisplayMode mode);
    };

} /* namespace gui */
