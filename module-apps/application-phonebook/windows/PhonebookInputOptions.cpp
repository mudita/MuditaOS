// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhonebookInputOptions.hpp"
#include "application-phonebook/data/PhonebookItemData.hpp"

#include <Clipboard.hpp>
#include <ApplicationCommon.hpp>

namespace gui
{
    PhonebookInputOptions::PhonebookInputOptions(app::ApplicationCommon *app, std::string windowName)
        : OptionWindow(app, windowName)
    {}

    void PhonebookInputOptions::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (const auto message = dynamic_cast<PhonebookInputOptionData *>(data)) {
            addOptions(inputOptionsList(message->getInputText()));
            optionsList->rebuildList();
        }
    }

    auto PhonebookInputOptions::inputOptionsList(gui::Text *text) -> std::list<gui::Option>
    {
        std::list<gui::Option> options;

        if (!text->isEmpty()) {
            options.emplace_back(utils::translate("common_text_copy"), [=](gui::Item &item) {
                Clipboard::getInstance().copy(text->getText());
                application->returnToPreviousWindow();
                return true;
            });
        }

        if (Clipboard::getInstance().hasData()) {
            options.emplace_back(utils::translate("common_text_paste"), [=](gui::Item &item) {
                /* Single line text ellipsis implementation doesn't properly support
                 * text that consists of multiple blocks - use character addition
                 * type, so that whole text is stored in one block. */
                text->addText(Clipboard::getInstance().paste(), AdditionType::perChar);
                application->returnToPreviousWindow();
                return true;
            });
        }

        return options;
    }
} // namespace gui
