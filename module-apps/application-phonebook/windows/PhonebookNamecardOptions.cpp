#include "PhonebookNamecardOptions.hpp"
#include "application-phonebook/ApplicationPhonebook.hpp"
#include "application-phonebook/data/PhonebookItemData.hpp"
#include "application-phonebook/data/PhonebookUtils.hpp"

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

    void PhonebookNamecardOptions::sendViaSms()
    {
        const std::string vcard = PhonebookUtils::formatVCard(*contact);
    }

    void PhonebookNamecardOptions::sendViaBluetooth()
    {
        const std::string vcard = PhonebookUtils::formatVCard(*contact);
    }

    auto PhonebookNamecardOptions::namecardOptionsList() -> std::list<gui::Option>
    {
        return {
            gui::Option{utils::localize.get("app_phonebook_options_send_bt"),
                        [=](gui::Item &item) {
                            LOG_INFO("Sending namecard via bluetooth!");
                            sendViaBluetooth();
                            return true;
                        }},
            gui::Option{utils::localize.get("app_phonebook_options_send_sms"),
                        [=](gui::Item &item) {
                            LOG_INFO("Sending namecard via SMS!");
                            sendViaSms();
                            return true;
                        }},
        };
    }
} // namespace gui
