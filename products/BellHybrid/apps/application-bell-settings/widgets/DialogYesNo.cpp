// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DialogYesNo.hpp"

#include <data/BellSettingsStyle.hpp>

#include <apps-common/messages/DialogMetadataMessage.hpp>
#include <common/options/OptionBellMenu.hpp>
#include <apps-common/windows/Dialog.hpp>
#include <TextFixedSize.hpp>

using namespace gui;

BellDialogYesNo::BellDialogYesNo(app::ApplicationCommon *app, const std::string &name)
    : BellShortOptionWindow(app, name)
{
    optionsList->setBoundaries(Boundaries::Continuous);

    statusBar->setVisible(false);
    header->setTitleVisibility(true);
}

std::list<gui::Option> BellDialogYesNo::dialogOptionsList(std::function<bool()> yesAction)
{
    std::list<gui::Option> dialogOptionList;
    auto addDialogSettings = [&](const UTF8 &name, const gui::bell_dialog::Options &optionName) {
        dialogOptionList.emplace_back(std::make_unique<gui::option::OptionBellMenu>(
            name,
            [=](gui::Item &item) {
                if (optionName == bell_dialog::Options::YES) {
                    return yesAction();
                }
                else if (optionName == bell_dialog::Options::NO) {
                    application->returnToPreviousWindow();
                }
                return true;
            },
            [=](gui::Item &item) {
                // put focus change callback here
                return true;
            },
            this));
    };

    addDialogSettings(utils::translate(style::strings::common::no), bell_dialog::Options::NO);
    addDialogSettings(utils::translate(style::strings::common::yes), bell_dialog::Options::YES);

    return dialogOptionList;
}

void BellDialogYesNo::onBeforeShow(ShowMode, SwitchData *data)
{
    if (auto metadata = dynamic_cast<DialogMetadataMessage *>(data); metadata) {
        setListTitle(metadata->get().text);
        addOptions(dialogOptionsList(metadata->get().action));
    }
}
