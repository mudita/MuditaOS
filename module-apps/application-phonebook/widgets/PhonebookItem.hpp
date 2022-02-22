// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Interface/ContactRecord.hpp"
#include "Label.hpp"
#include "Image.hpp"
#include "ListItem.hpp"
#include <BoxLayout.hpp>
#include <TextFixedSize.hpp>
#include <apps-common/widgets/TextWithIconsWidget.hpp>

namespace gui
{

    class PhonebookItem : public ListItem
    {
        gui::Label *contactName = nullptr;
        gui::HBox *hBox         = nullptr;

        bool favourite = false;
        void markBlocked(bool val);
        LabelMarkerDisplayMode labelMarkerDisplayMode = LabelMarkerDisplayMode::IncludeFavourites;

      public:
        std::shared_ptr<ContactRecord> contact;

        PhonebookItem();
        virtual ~PhonebookItem() = default;

        // sets copy of contact
        void setContact(std::shared_ptr<ContactRecord> note);
        UTF8 getLabelMarker();

        void setLabelMarkerDisplayMode(LabelMarkerDisplayMode mode);
    };

    class PhonebookMarkItem : public ListItem
    {
      private:
        TextWithSnippet *markLabel = nullptr;

      public:
        explicit PhonebookMarkItem(const UTF8 &markText);
    };

} /* namespace gui */
