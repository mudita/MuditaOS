// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-phonebook/data/PhonebookInternals.hpp"
#include "application-phonebook/data/PhonebookItemData.hpp"
#include "application-phonebook/widgets/ContactListItem.hpp"

#include <ListItem.hpp>
#include <Text.hpp>

namespace gui
{
    class OutputLinesTextWithLabelWidget : public ContactListItem
    {
        phonebookInternals::ListItemName listItemName;

      public:
        OutputLinesTextWithLabelWidget(phonebookInternals::ListItemName listItemName);

        ~OutputLinesTextWithLabelWidget() override = default;
        VBox *vBox                                 = nullptr;
        Label *titleLabel                          = nullptr;
        Text *multilineText                        = nullptr;

        auto handleRequestResize(const Item *, Length request_w, Length request_h) -> Size override;

      private:
        void applyItemNameSpecificSettings();

        void addressHandler();
        void noteHandler();
    };

} /* namespace gui */
