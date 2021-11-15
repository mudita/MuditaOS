// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-phonebook/data/PhonebookInternals.hpp"
#include "application-phonebook/data/PhonebookItemData.hpp"
#include "application-phonebook/widgets/ContactListItem.hpp"

#include <ListItem.hpp>
#include <Text.hpp>
#include <TextFixedSize.hpp>

namespace gui
{
    class InputLinesWithLabelWidget : public ContactListItem
    {
      public:
        explicit InputLinesWithLabelWidget(
            phonebookInternals::ListItemName listItemName,
            const std::function<void(const UTF8 &text, bool emptyOthers)> &navBarTemporaryMode = nullptr,
            const std::function<void()> &navBarRestoreFromTemporaryMode                        = nullptr,
            const std::function<void()> &selectSpecialCharacter                                = nullptr,
            const std::function<void(Text *text)> &inputOptions                                = nullptr,
            unsigned int lines                                                                 = 1);

        VBox *vBox               = nullptr;
        Label *titleLabel        = nullptr;
        TextFixedSize *inputText = nullptr;

      private:
        phonebookInternals::ListItemName listItemName;
        void applyItemNameSpecificSettings();

        std::function<void(const UTF8 &text, bool emptyOthers)> navBarTemporaryMode = nullptr;
        std::function<void()> navBarRestoreFromTemporaryMode                        = nullptr;
        std::function<void(Text *text)> inputOptions                                = nullptr;

        void firstNameHandler();
        void secondNameHandler();
        void numberHandler();
        void secondNumberHandler();
        void emailHandler();
        void addressHandler();
        void noteHandler();
    };

} /* namespace gui */
