#include "PhonebookOptionsNamecard.hpp"
#include "../ApplicationPhonebook.hpp"
#include "InputEvent.hpp"
#include "Label.hpp"
#include "Margins.hpp"
#include "PhonebookContact.hpp"
#include "Text.hpp"
#include "application-call/data/CallSwitchData.hpp"
#include "i18/i18.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include <log/log.hpp>

namespace gui
{

    PhonebookOptionsNamecard::PhonebookOptionsNamecard(app::Application *app) : OptionWindow(app, gui::window::name::options_namecard)
    {
        buildInterface();
        this->addOptions(namecardOptionsList());
    }

    PhonebookOptionsNamecard::~PhonebookOptionsNamecard()
    {}

    bool PhonebookOptionsNamecard::handleSwitchData(SwitchData *data)
    {
        if (data == nullptr) {
            LOG_ERROR("Received null pointer");
            return false;
        }

        PhonebookItemData *item = dynamic_cast<PhonebookItemData *>(data);
        contact                 = item->getContact();
        return true;
    }

    bool PhonebookOptionsNamecard::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.keyCode == KeyCode::KEY_RF && (inputEvent.state == InputEvent::State::keyReleasedShort)) {
            std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
            application->switchWindow("Options", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
            return true;
        }

        return (AppWindow::onInput(inputEvent));
    }

    void PhonebookOptionsNamecard::sendViaSms()
    {
        const std::string vcard = formatVCARD();
    }

    void PhonebookOptionsNamecard::sendViaBluetooth()
    {
        const std::string vcard = formatVCARD();
    }

    const std::string PhonebookOptionsNamecard::formatVCARD()
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

    std::list<gui::Option> PhonebookOptionsNamecard::namecardOptionsList()
    {
        return {
            gui::Option{utils::localize.get("app_phonebook_options_send_bt"),
                        [=](gui::Item &item) {
                          LOG_INFO("Editing contact!");
                          sendViaBluetooth();
                          return true;
                        }},
            gui::Option{utils::localize.get("app_phonebook_options_send_sms"),
                        [=](gui::Item &item) {
                          LOG_INFO("Forwarding namecard!");
                          sendViaSms();
                          return true;
                        }},
        };
    }
} // namespace gui
