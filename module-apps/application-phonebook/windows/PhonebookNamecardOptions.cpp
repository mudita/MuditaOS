// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-appmgr/Controller.hpp>
#include "PhonebookNamecardOptions.hpp"
#include "application-phonebook/ApplicationPhonebook.hpp"
#include "application-phonebook/data/PhonebookItemData.hpp"
#include "application-phonebook/data/PhonebookUtils.hpp"
#include "application-messages/data/SMSdata.hpp"

namespace gui
{

    PhonebookNamecardOptions::PhonebookNamecardOptions(app::ApplicationCommon *app)
        : OptionWindow(app, gui::window::name::namecard_options)
    {
        addOptions(namecardOptionsList());
    }

    auto PhonebookNamecardOptions::handleSwitchData(SwitchData *data) -> bool
    {
        auto *item = dynamic_cast<PhonebookItemData *>(data);
        if (item == nullptr) {
            LOG_WARN("Received null pointer");
            return false;
        }

        contact = item->getContact();
        return true;
    }

    auto PhonebookNamecardOptions::onInput(const InputEvent &inputEvent) -> bool
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
            application->switchWindow(
                gui::window::name::contact_options, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
            return true;
        }

        return AppWindow::onInput(inputEvent);
    }

    auto PhonebookNamecardOptions::sendViaSms() -> bool
    {
        auto data = std::make_unique<SMSSendRequest>(utils::PhoneNumber::View{}, contact->getAsString());
        data->ignoreCurrentWindowOnStack = true;
        return app::manager::Controller::sendAction(application,
                                                    app::manager::actions::CreateSms,
                                                    std::move(data),
                                                    app::manager::OnSwitchBehaviour::RunInBackground);
    }

    auto PhonebookNamecardOptions::namecardOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> l;
        l.emplace_back(gui::Option{utils::translate("app_phonebook_options_send_sms"), [=](gui::Item &item) {
                                       LOG_INFO("Sending namecard via SMS!");
                                       sendViaSms();
                                       return true;
                                   }});
        return l;
    }
} // namespace gui
