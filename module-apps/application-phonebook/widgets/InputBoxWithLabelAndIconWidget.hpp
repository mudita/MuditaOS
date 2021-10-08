// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-phonebook/data/PhonebookInternals.hpp"
#include "application-phonebook/data/PhonebookItemData.hpp"
#include "application-phonebook/widgets/ContactListItem.hpp"

#include <BoxLayout.hpp>
#include <Image.hpp>
#include <Label.hpp>
#include <ListItem.hpp>

namespace gui
{
    class InputBoxWithLabelAndIconWidget : public ContactListItem
    {

        phonebookInternals::ListItemName listItemName;

      public:
        InputBoxWithLabelAndIconWidget(phonebookInternals::ListItemName listItemName,
                                       std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr,
                                       std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr);
        ~InputBoxWithLabelAndIconWidget() override = default;
        gui::HBox *hBox              = nullptr;
        gui::Label *inputBoxLabel    = nullptr;
        gui::Label *descriptionLabel = nullptr;
        gui::Image *iconImage        = nullptr;
        gui::Image *tickImage        = nullptr;

      private:
        std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr;
        std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr;

        void applyItemNameSpecificSettings();

        void speedDialKeyHandler();
        void addToFavouritesHandler();
        void addToICEHandler();
    };

} /* namespace gui */
