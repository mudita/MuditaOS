// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallLogOptionsWindow.hpp"
#include "data/CallLogSwitchData.hpp"
#include "data/CallLogInternals.hpp"

#include <i18n/i18n.hpp>
#include <OptionContact.hpp>
#include <OptionWindow.hpp>

namespace gui
{

    CalllogWindowOptions::CalllogWindowOptions(app::ApplicationCommon *app, std::string windowName)
        : OptionWindow(app, windowName)
    {}

    void CalllogWindowOptions::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        if (auto message = dynamic_cast<calllog::CallLogSwitchData *>(data)) {
            record = message->getRecord();
        }

        auto currentRecordValid = getRecordValid();

        if (recordValidCode == ValidCodes::Uninitialized || recordValidCode == currentRecordValid) {
            options = calllogWindowOptions(dynamic_cast<app::ApplicationCallLog *>(application), record);
            optionsList->rebuildList(listview::RebuildType::InPlace);
        }
        else {
            application->switchWindow(calllog::settings::MainWindowStr);
        }

        recordValidCode = currentRecordValid;
    }

    CalllogWindowOptions::ValidCodes CalllogWindowOptions::getRecordValid()
    {
        auto contact = DBServiceAPI::MatchContactByPhoneNumber(application, record.phoneNumber);
        return contact == nullptr ? ValidCodes::Invalid
                                  : (contact->isTemporary() ? ValidCodes::Invalid : ValidCodes::Valid);
    }

    std::list<Option> CalllogWindowOptions::calllogWindowOptions(app::ApplicationCallLog *app,
                                                                 const CalllogRecord &record)
    {
        std::list<gui::Option> options;
        if (record.presentation != PresentationType::PR_UNKNOWN) {
            auto contactRecord = DBServiceAPI::MatchContactByPhoneNumber(app, record.phoneNumber);
            if (!contactRecord || !contactRecord->isValid() || contactRecord->isTemporary()) {
                ContactRecord newRecord;
                newRecord.numbers.emplace_back(record.phoneNumber);
                options.emplace_back(gui::Option{
                    std::make_unique<gui::option::Contact>(app, gui::option::ContactOperation::Add, newRecord)});
            }
            else {
                options.emplace_back(gui::Option{std::make_unique<gui::option::Contact>(
                    app, gui::option::ContactOperation::Details, *contactRecord)});
            }
        }

        // add option delete call option
        options.push_back(gui::Option(
            utils::translate("app_calllog_options_delete_call"),
            [=](gui::Item &item) { return app->removeCalllogEntry(record); },
            gui::option::Arrow::Disabled));
        return options;
    }

} // namespace gui
