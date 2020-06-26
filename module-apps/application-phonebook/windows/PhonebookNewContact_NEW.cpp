#include "PhonebookNewContact_NEW.hpp"
#include "application-phonebook/ApplicationPhonebook.hpp"
#include "PhonebookContact.hpp"
#include "Dialog.hpp"

#include <service-db/api/DBServiceAPI.hpp>

namespace gui
{

    PhonebookNewContact_NEW::PhonebookNewContact_NEW(app::Application *app)
        : AppWindow(app, gui::window::name::new_contact), newContactModel{new NewContactModel(app)}
    {
        buildInterface();
    }

    void PhonebookNewContact_NEW::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void PhonebookNewContact_NEW::buildInterface()
    {
        AppWindow::buildInterface();

        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::save));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        topBar->setActive(TopBar::Elements::TIME, true);

        setTitle(utils::localize.get("app_phonebook_contact_title"));

        list = new gui::ListView(this,
                                 phonebookStyle::mainWindow::newContactsList::x,
                                 phonebookStyle::mainWindow::newContactsList::y,
                                 phonebookStyle::mainWindow::newContactsList::w,
                                 phonebookStyle::mainWindow::newContactsList::h);
        list->setItemSpanSize(style::listview::item_span_big);
        list->setProvider(newContactModel);

        setFocusItem(list);
    }

    void PhonebookNewContact_NEW::destroyInterface()
    {
        erase();
    }

    PhonebookNewContact_NEW::~PhonebookNewContact_NEW()
    {
        destroyInterface();
    }

    void PhonebookNewContact_NEW::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (mode == ShowMode::GUI_SHOW_INIT) {
            //            list->clear();
            list->setElementsCount(newContactModel->getItemCount());
        }
    }

    bool PhonebookNewContact_NEW::onInput(const InputEvent &inputEvent)
    {

        if (inputEvent.keyCode == gui::KeyCode::KEY_ENTER) {

            newContactModel->saveData();
            return true;
        }

        if (AppWindow::onInput(inputEvent)) {
            return (true);
        }

        if (inputEvent.state != InputEvent::State::keyReleasedShort)
            return (false);

        application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);

        return (false);
    }
    //
    //    auto PhonebookNewContact_NEW::verifyAndSave() -> bool
    //    {
    //        ContactRecord record, errNumPrim, errNumAlt, errSpeedDial;
    //
    //        /** basic sanity checks */
    //        if (contact == nullptr) // new contact
    //        {
    //            /** secondary verification against database data */
    //            DBServiceAPI::ContactVerificationError err =
    //                DBServiceAPI::verifyContact(application, record, errNumPrim, errNumAlt, errSpeedDial);
    //            LOG_INFO("Contact data verification result: \"%s\"",
    //            DBServiceAPI::getVerificationErrorString(err).c_str()); switch (err) { case DBServiceAPI::noError:
    //                break;
    //            case DBServiceAPI::primaryNumberError:
    //                showDialogDuplicatedNumber(record, record.numbers[0].numberE164);
    //                return false;
    //            case DBServiceAPI::secondaryNumberError:
    //                showDialogDuplicatedNumber(record, record.numbers[1].numberE164);
    //                return false;
    //            case DBServiceAPI::speedDialError:
    //                showDialogDuplicatedSpeedDialNumber(record);
    //                return false;
    //            }
    //
    //            if (DBServiceAPI::ContactAdd(application, record) == false) {
    //                LOG_ERROR("verifyAndSave failed to ADD contact");
    //                return (false);
    //            }
    //            else {
    //                contact                               = std::make_shared<ContactRecord>(record);
    //                std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
    //                application->switchWindow(gui::window::name::contact, gui::ShowMode::GUI_SHOW_INIT,
    //                std::move(data)); LOG_INFO("verifyAndSave contact ADDED"); return (true);
    //            }
    //        }
    //        else if (contact->ID != DB_ID_NONE) {
    //            if (DBServiceAPI::ContactUpdate(application, *contact) == false) {
    //                LOG_ERROR("verifyAndSave failed to UPDATE contact");
    //                return (false);
    //            }
    //            else {
    //                contact                               = std::make_shared<ContactRecord>(record);
    //                std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
    //                application->switchWindow(gui::window::name::contact, gui::ShowMode::GUI_SHOW_INIT,
    //                std::move(data)); LOG_INFO("verifyAndSave contact UPDATED"); return (true);
    //            }
    //        }
    //
    //        return (false);
    //    }
    //    const std::string PhonebookNewContact_NEW::getCountryPrefix()
    //    {
    //        std::string imsi = CellularServiceAPI::GetIMSI(application, false);
    //        if (imsi == "") {
    //            LOG_ERROR("Can't get IMSI code from cellular, fall back to Poland country code");
    //            return (app::defaultCountryCode);
    //        }
    //        LOG_DEBUG("getCountryPrefix imsi:%s", imsi.c_str());
    //        const uint32_t country_code = DBServiceAPI::GetCountryCodeByMCC(application, std::stoul(imsi));
    //        if (country_code <= 0) {
    //            LOG_ERROR("Can't get country code from database, fall back to Poland country code");
    //            return (app::defaultCountryCode);
    //        }
    //        LOG_DEBUG("getCountryPrefix country_code: %" PRIu32, country_code);
    //        std::string buf = "+";
    //        buf += std::to_string(country_code);
    //
    //        LOG_DEBUG("getCountryPrefix return: \"%s\"", buf.c_str());
    //        return (buf);
    //    }
    //
    //    void PhonebookNewContact_NEW::showDialogDuplicatedNumber(ContactRecord &newContactRecord, const UTF8
    //    duplicatedNumber)
    //    {
    //        auto dialog = dynamic_cast<gui::DialogYesNo
    //        *>(this->application->getWindow(gui::window::name::dialog_yes_no)); assert(dialog != nullptr); auto meta
    //        = dialog->meta; auto contactRecordsPtr = DBServiceAPI::ContactGetByPhoneNumber(this->application,
    //        duplicatedNumber); auto oldContactRecord  = !contactRecordsPtr->empty() ? contactRecordsPtr->front() :
    //        ContactRecord{}; newContactRecord.ID    = oldContactRecord.ID; meta.action            = [=]() -> bool {
    //            if (!DBServiceAPI::ContactUpdate(this->application, newContactRecord)) {
    //                LOG_ERROR("Contact id=%" PRIu32 " update failed", newContactRecord.ID);
    //                return false;
    //            }
    //            this->application->switchWindow(gui::name::window::main_window);
    //            return true;
    //        };
    //        std::string duplicatedNumberPhrase = utils::localize.get("app_phonebook_duplicate_numbers");
    //        fillContactData(duplicatedNumberPhrase, oldContactRecord);
    //        meta.text  = duplicatedNumberPhrase;
    //        meta.title = duplicatedNumber;
    //        meta.icon  = "info_big_circle_W_G";
    //        dialog->update(meta);
    //        this->application->switchWindow(dialog->getName());
    //    }
    //
    //    void PhonebookNewContact_NEW::showDialogDuplicatedSpeedDialNumber(ContactRecord &newContactRecord)
    //    {
    //        auto dialog = dynamic_cast<gui::DialogYesNo
    //        *>(this->application->getWindow(gui::window::name::dialog_yes_no)); assert(dialog != nullptr); auto meta
    //        = dialog->meta; auto contactRecordsPtr = DBServiceAPI::ContactGetBySpeeddial(this->application,
    //        newContactRecord.speeddial); auto oldContactRecord  = !contactRecordsPtr->empty() ?
    //        contactRecordsPtr->front() : ContactRecord{}; newContactRecord.ID    = oldContactRecord.ID; meta.action =
    //        [=]() -> bool {
    //            if (!DBServiceAPI::ContactUpdate(this->application, newContactRecord)) {
    //                LOG_ERROR("Contact id=%" PRIu32 " update failed", newContactRecord.ID);
    //                return false;
    //            }
    //            this->application->switchWindow(gui::name::window::main_window);
    //            return true;
    //        };
    //        std::string duplicatedSpeedDialPhrase = utils::localize.get("app_phonebook_duplicate_numbers");
    //        fillContactData(duplicatedSpeedDialPhrase, oldContactRecord);
    //        std::string duplicatedSpeedDialTitle = utils::localize.get("app_phonebook_duplicate_speed_dial_title");
    //        fillContactData(duplicatedSpeedDialTitle, oldContactRecord);
    //        meta.text  = duplicatedSpeedDialPhrase;
    //        meta.title = duplicatedSpeedDialTitle;
    //        meta.icon  = "info_big_circle_W_G";
    //        dialog->update(meta);
    //        this->application->switchWindow(dialog->getName());
    //    }

} // namespace gui
