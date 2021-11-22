// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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
        if (auto message = dynamic_cast<PhonebookInputOptionData *>(data)) {
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

        if (Clipboard::getInstance().gotData()) {
            options.emplace_back(utils::translate("common_text_paste"), [=](gui::Item &item) {
                text->addText(Clipboard::getInstance().paste(), AdditionType::perBlock);
                application->returnToPreviousWindow();
                return true;
            });
        }

        return options;
    }
} // namespace gui
