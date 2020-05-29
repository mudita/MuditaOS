#include "../ApplicationPhonebook.hpp"
#include "../data/PhonebookItemData.hpp"
#include "PhonebookNamecardOptions.hpp"

namespace gui
{

    PhonebookNamecardOptions::PhonebookNamecardOptions(app::Application *app)
        : OptionWindow(app, gui::window::name::namecard_options)
    {
        buildInterface();
        this->addOptions(namecardOptionsList());
    }

    PhonebookNamecardOptions::~PhonebookNamecardOptions() = default;

    auto PhonebookNamecardOptions::handleSwitchData(SwitchData *data) -> bool
    {
        if (data == nullptr) {
            LOG_ERROR("Received null pointer");
            return false;
        }

        auto *item = dynamic_cast<PhonebookItemData *>(data);
        contact    = item->getContact();
        return true;
    }

    auto PhonebookNamecardOptions::onInput(const InputEvent &inputEvent) -> bool
    {
        if (inputEvent.keyCode == KeyCode::KEY_RF && (inputEvent.state == InputEvent::State::keyReleasedShort)) {
            std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
            application->switchWindow("Options", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
            return true;
        }

        return (AppWindow::onInput(inputEvent));
    }

    void PhonebookNamecardOptions::sendViaSms()
    {
        const std::string vcard = formatVCARD();
    }

    void PhonebookNamecardOptions::sendViaBluetooth()
    {
        const std::string vcard = formatVCARD();
    }

    auto PhonebookNamecardOptions::formatVCARD() -> const std::string
    {
        const std::string priNumber = (contact->numbers.size() > 0) ? contact->numbers[0].numberE164.c_str() : "";
        const std::string secNumber = (contact->numbers.size() > 1) ? contact->numbers[1].numberE164.c_str() : "";
        std::ostringstream vcard;
        vcard << "BEGIN:VCARD\n"
              << "VERSION:3.0\n"
              << "N:" << contact->alternativeName.c_str() << ";" << contact->primaryName.c_str() << ";;;\n"
              << "FN:" << contact->primaryName.c_str() << " " << contact->alternativeName.c_str() << "\n"
              << "TEL;TYPE=HOME,VOICE:" << priNumber << "\n";

        if (secNumber.length() > 0) {
            vcard << "TEL;TYPE=HOME,VOICE:" << secNumber << "\n";
        }

        vcard << "ADR;TYPE=HOME:;;" << contact->address.c_str() << "\n"
              << "EMAIL:" << contact->mail.c_str() << "\n"
              << "END:VCARD";

        LOG_INFO("formatted vcard:");
        LOG_INFO("%s", vcard.str().c_str());
        return (vcard.str());
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
