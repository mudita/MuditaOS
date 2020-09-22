#include "PhonebookNamecardOptions.hpp"
#include "application-phonebook/ApplicationPhonebook.hpp"
#include "application-phonebook/data/PhonebookItemData.hpp"
#include "application-phonebook/data/PhonebookUtils.hpp"
#include "UiCommonActions.hpp"

namespace gui
{

    PhonebookNamecardOptions::PhonebookNamecardOptions(app::Application *app)
        : OptionWindow(app, gui::window::name::namecard_options)
    {
        buildInterface();
        this->addOptions(namecardOptionsList());
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
        if (inputEvent.keyCode == KeyCode::KEY_RF && (inputEvent.state == InputEvent::State::keyReleasedShort)) {
            std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
            application->switchWindow(
                gui::window::name::contact_options, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
            return true;
        }

        return (AppWindow::onInput(inputEvent));
    }

    auto PhonebookNamecardOptions::sendViaSms() -> bool
    {
        const UTF8 contactData = contact->getAsString();
        const utils::PhoneNumber::View emptyNumber;
        return app::sms(application, app::SmsOperation::New, emptyNumber, contactData);
    }

    void PhonebookNamecardOptions::sendViaBluetooth()
    {
        const UTF8 contactData = contact->getAsString();
    }

    auto PhonebookNamecardOptions::namecardOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> l;

        l.emplace_back(gui::Option{utils::localize.get("app_phonebook_options_send_bt"), [=](gui::Item &item) {
                                       LOG_INFO("Sending namecard via bluetooth!");
                                       sendViaBluetooth();
                                       return true;
                                   }});
        l.emplace_back(gui::Option{utils::localize.get("app_phonebook_options_send_sms"), [=](gui::Item &item) {
                                       LOG_INFO("Sending namecard via SMS!");
                                       sendViaSms();
                                       return true;
                                   }});
        return l;
    }
} // namespace gui
