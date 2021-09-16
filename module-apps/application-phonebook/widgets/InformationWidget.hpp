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
    class TextWithIconsWidget;

    class InformationWidget : public ContactListItem
    {
      public:
        InformationWidget(app::ApplicationCommon *app);
        ~InformationWidget() override = default;
        VBox *vBox                               = nullptr;
        Label *titleLabel                        = nullptr;
        TextWithIconsWidget *primaryNumberHBox   = nullptr;
        TextWithIconsWidget *secondaryNumberHBox = nullptr;
        Text *emailText                          = nullptr;
        Item *savedFocusItem                     = nullptr;
    };

} /* namespace gui */
