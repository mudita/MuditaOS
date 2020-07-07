#include "PhonebookNewContact.hpp"

#include "application-phonebook/ApplicationPhonebook.hpp"
#include "PhonebookContact.hpp"
#include "Dialog.hpp"

#include <service-db/api/DBServiceAPI.hpp>

#include <PhoneNumber.hpp>

namespace gui
{

    PhonebookNewContact::PhonebookNewContact(app::Application *app) : AppWindow(app, gui::window::name::new_contact)
    {
        buildInterface();
    }

    void PhonebookNewContact::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void PhonebookNewContact::buildInterface()
    {
        AppWindow::buildInterface();

        bottomBar->setActive(BottomBar::Side::LEFT, false);
        bottomBar->setActive(BottomBar::Side::CENTER, false);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_phonebook_save"));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_phonebook_back"));

        topBar->setActive(TopBar::Elements::TIME, true);

        title = new gui::Label(this, 0, 50, 480, 54);
        title->setFilled(false);
        title->setBorderColor(gui::ColorFullBlack);
        title->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        title->setMargins(Margins(0, 0, 0, 18));
        title->setFont(style::window::font::small);
        title->setText(utils::localize.get("app_phonebook_contact_title"));
        title->setAlignment(
            gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));

        // page 1 labels and text
        for (uint32_t i = 0; i < 5; i++) {

            page1.labels[i] = new gui::Label(this, 30, 114 + 87 * i, 450, 35);
            page1.labels[i]->setFilled(false);
            page1.labels[i]->setBorderColor(gui::ColorFullBlack);
            page1.labels[i]->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
            page1.labels[i]->setFont(style::window::font::small);
            if (i == 0)
                page1.labels[i]->setFont(style::footer::font::bold);

            page1.labels[i]->setAlignment(
                gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_BOTTOM));

            page1.text[i] = new gui::Text(this, 30, 157 + 87 * i, 420, 42);
            page1.text[i]->setFilled(false);
            page1.text[i]->setPenFocusWidth(2);
            page1.text[i]->setPenWidth(1);
            page1.text[i]->setBorderColor(gui::ColorFullBlack);
            page1.text[i]->setUnderline(true);
            page1.text[i]->setTextType(TextType::SINGLE_LINE);
            page1.text[i]->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
            page1.text[i]->setFont(style::window::font::small);
            page1.text[i]->setInputMode(new InputMode({InputMode::ABC, InputMode::abc}));

            if ((i == 2) || (i == 3)) {
                page1.text[i]->focusChangedCallback = [=](gui::Item &item) {
                    if (item.focus) {
                        page1.text[i]->setFont(style::window::font::bigbold);
                    }
                    else {
                        page1.text[i]->setFont(style::window::font::small);
                    }
                    return true;
                };
            }
            else {
                page1.text[i]->focusChangedCallback = [=](gui::Item &item) {
                    if (item.focus) {
                        page1.text[i]->setFont(style::window::font::bigbold);
                    }
                    else {
                        page1.text[i]->setFont(style::window::font::small);
                    }
                    return true;
                };

                page1.text[i]->contentCallback = [=](gui::Item &item) { return true; };
            }

            page1.text[i]->setTextType(TextType::SINGLE_LINE);
            page1.text[i]->setEditMode(EditMode::EDIT);
        }

        page1.text[2]->setInputMode(new InputMode({InputMode::digit}));
        page1.text[3]->setInputMode(new InputMode({InputMode::digit}));

        // set navigation for page 1
        for (uint32_t i = 0; i < 4; i++) {
            page1.text[i]->setNavigationItem(NavigationDirection::DOWN, page1.text[i + 1]);
            page1.text[i + 1]->setNavigationItem(NavigationDirection::UP, page1.text[i]);
        }
        page1.text[4]->setNavigationItem(NavigationDirection::UP, page1.text[3]);

        page1.labels[0]->setText(utils::localize.get("app_phonebook_new_contact_first_name"));
        page1.labels[1]->setText(utils::localize.get("app_phonebook_new_contact_second_name"));
        page1.labels[2]->setText(utils::localize.get("app_phonebook_new_contact_number_1"));
        page1.labels[3]->setText(utils::localize.get("app_phonebook_new_contact_number_2"));
        page1.labels[4]->setText(utils::localize.get("app_phonebook_new_contact_email"));

        // page2
        page2.speedValue = new gui::Label(this, 30, 105, 56, 47);
        page2.speedValue->setFilled(false);
        page2.speedValue->setBorderColor(gui::ColorFullBlack);
        page2.speedValue->setPenFocusWidth(3);
        page2.speedValue->setPenWidth(1);
        page2.speedValue->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        page2.speedValue->setFont(style::window::font::small);
        page2.speedValue->setAlignment(
            gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));

        page2.speedValue->focusChangedCallback = [=](gui::Item &item) { return true; };

        page2.speedValue->inputCallback = [=](gui::Item &item, const InputEvent &inputEvent) {
            if (inputEvent.keyCode == KeyCode::KEY_PND) {
                page2.speedValue->clear();
                return true;
            }
            int val = gui::toNumeric(inputEvent.keyCode);
            if ((val >= 0) && (val <= 9)) {
                page2.speedValue->setText(std::to_string(val));
                return true;
            }
            return false;
        };

        page2.speedDescription = new gui::Label(this, 100, 105, 330, 47);
        page2.speedDescription->setFilled(false);
        page2.speedDescription->setBorderColor(gui::ColorFullBlack);
        page2.speedDescription->setPenFocusWidth(0);
        page2.speedDescription->setPenWidth(0);
        page2.speedDescription->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        page2.speedDescription->setFont(style::window::font::small);
        page2.speedDescription->setAlignment(
            gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_BOTTOM));

        page2.imageSpeed = new gui::Image(this, 416, 122, 0, 0, "small_circle");

        page2.favValue = new gui::Label(this, 30, 161, 56, 47);
        page2.favValue->setFilled(false);
        page2.favValue->setBorderColor(gui::ColorFullBlack);
        page2.favValue->setPenFocusWidth(3);
        page2.favValue->setPenWidth(1);
        page2.favValue->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        page2.favValue->setFont(style::window::font::small);
        page2.favValue->setAlignment(
            gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_BOTTOM));

        page2.favValue->focusChangedCallback = [=](gui::Item &item) {
            if (item.focus) {
                bottomBar->setText(BottomBar::Side::CENTER, "SELECT");
            }
            else {
                bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_phonebook_save"));
            }
            return true;
        };

        page2.favValue->inputCallback = [=](gui::Item &item, const InputEvent &inputEvent) {
            if ((inputEvent.keyCode == KeyCode::KEY_ENTER) &&
                ((inputEvent.state == InputEvent::State::keyReleasedShort) ||
                 (inputEvent.state == InputEvent::State::keyReleasedLong))) {
                if (page2.favSelected) {
                    page2.imageTick->setVisible(false);
                    page2.favSelected = false;
                    LOG_INFO("Removed from favourites");
                }
                else {
                    page2.imageTick->setVisible(true);
                    page2.favSelected = true;
                    LOG_INFO("Added to favourites");
                }
                application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
                return true;
            }
            return false;
        };

        page2.favDescription = new gui::Label(this, 100, 161, 330, 47);
        page2.favDescription->setFilled(false);
        page2.favDescription->setBorderColor(gui::ColorFullBlack);
        page2.favDescription->setPenFocusWidth(0);
        page2.favDescription->setPenWidth(0);
        page2.favDescription->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        page2.favDescription->setFont(style::window::font::small);
        page2.favDescription->setAlignment(
            gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_BOTTOM));
        page2.favSelected = false;
        page2.imageTick   = new gui::Image(this, 43, 174, 0, 0, "small_tick");
        page2.imageTick->setVisible(false);

        page2.imageFav = new gui::Image(this, 416, 177, 0, 0, "small_heart");

        page2.speedDescription->setText(utils::localize.get("app_phonebook_new_speed_dial_key"));
        page2.favDescription->setText(utils::localize.get("app_phonebook_new_addto_fav"));

        page2.addressLabel = new gui::Label(this, 30, 241, 450, 35);
        page2.addressLabel->setFilled(false);
        page2.addressLabel->setBorderColor(gui::ColorFullBlack);
        page2.addressLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        page2.addressLabel->setFont(style::window::font::small);
        page2.addressLabel->setAlignment(
            gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_BOTTOM));
        page2.addressLabel->setText(utils::localize.get("app_phonebook_new_contact_address"));

        page2.noteLabel = new gui::Label(this, 30, 357, 450, 35);
        page2.noteLabel->setFilled(false);
        page2.noteLabel->setBorderColor(gui::ColorFullBlack);
        page2.noteLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        page2.noteLabel->setFont(style::window::font::small);
        page2.noteLabel->setAlignment(
            gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_BOTTOM));
        page2.noteLabel->setText(utils::localize.get("app_phonebook_new_contact_note"));

        for (uint32_t i = 0; i < 2; i++) {
            page2.text[i] = new gui::Text(this, 30, 272 + 126 * i, 420, 90);
            page2.text[i]->setFilled(false);
            page2.text[i]->setPenFocusWidth(2);
            page2.text[i]->setPenWidth(1);
            page2.text[i]->setUnderline(true);
            page2.text[i]->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
            page2.text[i]->setBorderColor(gui::ColorFullBlack);
            page2.text[i]->setMargins(Margins(0, 0, 0, 0));
            page2.text[i]->setFont(style::window::font::small);
            page2.text[i]->setInputMode(new InputMode({InputMode::ABC, InputMode::abc}));
            page2.text[i]->contentCallback = [=](gui::Item &item) { return true; };
        }

        page2.speedValue->setNavigationItem(NavigationDirection::DOWN, page2.favValue);

        page2.favValue->setNavigationItem(NavigationDirection::DOWN, page2.text[0]);
        page2.favValue->setNavigationItem(NavigationDirection::UP, page2.speedValue);

        page2.text[0]->setNavigationItem(NavigationDirection::DOWN, page2.text[1]);
        page2.text[0]->setNavigationItem(NavigationDirection::UP, page2.favValue);

        page2.text[1]->setNavigationItem(NavigationDirection::UP, page2.text[0]);
    }

    void PhonebookNewContact::destroyInterface()
    {
        erase();
    }

    PhonebookNewContact::~PhonebookNewContact()
    {
        destroyInterface();
    }

    void PhonebookNewContact::saveStateChanged()
    {
        bottomBar->setActive(BottomBar::Side::CENTER, true);
    }

    void PhonebookNewContact::copyInputData(ContactRecord &contactRecord)
    {
        contactRecord.primaryName     = page1.text[0]->getText();
        contactRecord.alternativeName = page1.text[1]->getText();
        contactRecord.numbers.push_back(ContactRecord::Number(utils::PhoneNumber(page1.text[2]->getText()).getView()));
        // Temporary disable saving secondary number since multiple numbers are not supported yet, and this could lead
        // to confusing errors
        // record.numbers.push_back(ContactRecord::Number(page1.text[3]->getText(), page1.text[3]->getText()));
        contactRecord.mail           = page1.text[4]->getText();
        contactRecord.address        = page2.text[0]->getText();
        contactRecord.note           = page2.text[1]->getText();
        contactRecord.isOnFavourites = page2.favSelected;
        contactRecord.speeddial      = page2.speedValue->getText();
    }

    void PhonebookNewContact::setContactData()
    {
        if (contact) {
            page1.text[0]->setText(contact->primaryName);
            page1.text[1]->setText(contact->alternativeName);
            if (contact->numbers.size() > 0) {
                if (contact->numbers[0].number.getEntered().size() == 0) {
                    page1.text[2]->setText(getCountryPrefix());
                }
                else {
                    page1.text[2]->setText(contact->numbers[0].number.getEntered());
                }
            }
            if (contact->numbers.size() > 1) {
                if (contact->numbers[1].number.getEntered().size() == 0) {
                    page1.text[3]->setText(getCountryPrefix());
                }
                else {
                    page1.text[3]->setText(contact->numbers[1].number.getEntered());
                }
            }
            page1.text[4]->setText(contact->mail);

            page2.text[0]->setText(contact->address);
            page2.text[1]->setText(contact->note);
            if (contact->isOnFavourites) {
                page2.imageTick->setVisible(true);
                page2.favSelected = true;
            }
            else {
                page2.imageTick->setVisible(false);
                page2.favSelected = false;
            }
            if (contact->speeddial.length() > 0)
                page2.speedValue->setText(contact->speeddial);
            else
                page2.speedValue->clear();

            saveStateChanged();
        }
        else {
            LOG_DEBUG("new contact create");
            rebuild();
        }
    }

    void PhonebookNewContact::switchPage(uint32_t page)
    {
        if (page == 0) {
            page1.setVisible(true);
            page2.setVisible(false);
        }
        else if (page == 1) {
            page1.setVisible(false);
            page2.setVisible(true);
        }
    }

    void PhonebookNewContact::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        switchPage(0);
        setFocusItem(page1.text[0]);
    }

    bool PhonebookNewContact::handleSwitchData(SwitchData *data)
    {
        if (data == nullptr)
            return false;

        PhonebookItemData *item = dynamic_cast<PhonebookItemData *>(data);
        if (item) {
            contact = item->getContact();

            if (contact != nullptr && contact->ID != DB_ID_NONE && contact->contactType != ContactType::TEMPORARY) {
                title->setText(utils::localize.get("app_phonebook_options_edit"));
            }
            else {
                title->setText(utils::localize.get("app_phonebook_contact_title"));
            }
            setContactData();
        }

        return (true);
    }

    bool PhonebookNewContact::onInput(const InputEvent &inputEvent)
    {
        if (AppWindow::onInput(inputEvent)) {
            return (true);
        }

        if (inputEvent.state != InputEvent::State::keyReleasedShort)
            return (false);

        saveStateChanged();

        if (inputEvent.keyCode == KeyCode::KEY_ENTER) {
            // if focus is on the favourite selection field do nothing
            if (focusItem == page2.favValue) {
                return true;
            }

            return (verifyAndSave());
        }
        else if (inputEvent.keyCode == KeyCode::KEY_DOWN) {
            LOG_INFO("switching to second page");
            if (getFocusItem() == page1.text[4]) {
                switchPage(1);
                setFocusItem(page2.speedValue);
            }
        }
        else if (inputEvent.keyCode == KeyCode::KEY_UP) {
            LOG_INFO("switching to first page");
            if (getFocusItem() == page2.speedValue) {
                switchPage(0);
                setFocusItem(page1.text[4]);
            }
        }

        application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);

        return (false);
    }

    auto PhonebookNewContact::verifyAndSave() -> bool
    {
        ContactRecord record, errNumPrim, errNumAlt, errSpeedDial;
        copyInputData(record);

        /** basic sanity checks */
        if (contact == nullptr) // new contact
        {
            /** secondary verification against database data */
            DBServiceAPI::ContactVerificationError err =
                DBServiceAPI::verifyContact(application, record, errNumPrim, errNumAlt, errSpeedDial);
            LOG_INFO("Contact data verification result: \"%s\"", DBServiceAPI::getVerificationErrorString(err).c_str());
            switch (err) {
            case DBServiceAPI::noError:
                break;
            case DBServiceAPI::primaryNumberError:
                showDialogDuplicatedNumber(record, record.numbers[0].number);
                return false;
            case DBServiceAPI::secondaryNumberError:
                showDialogDuplicatedNumber(record, record.numbers[1].number);
                return false;
            case DBServiceAPI::speedDialError:
                showDialogDuplicatedSpeedDialNumber(record);
                return false;
            }

            if (DBServiceAPI::ContactAdd(application, record) == false) {
                LOG_ERROR("verifyAndSave failed to ADD contact");
                return (false);
            }
            else {
                contact                               = std::make_shared<ContactRecord>(record);
                std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
                application->switchWindow(gui::window::name::contact, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
                LOG_INFO("verifyAndSave contact ADDED");
                return (true);
            }
        }
        else if (contact->ID != DB_ID_NONE) {
            assert(contact != nullptr);
            copyInputData(*contact);
            if (contact->contactType == ContactType::TEMPORARY) {
                contact->contactType = ContactType::USER;
            }
            if (DBServiceAPI::ContactUpdate(application, *contact) == false) {
                LOG_ERROR("verifyAndSave failed to UPDATE contact");
                return (false);
            }
            else {
                contact                               = std::make_shared<ContactRecord>(record);
                std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
                application->switchWindow(gui::window::name::contact, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
                LOG_INFO("verifyAndSave contact UPDATED");
                return (true);
            }
        }

        return (false);
    }
    const std::string PhonebookNewContact::getCountryPrefix()
    {
        std::string imsi = CellularServiceAPI::GetIMSI(application, false);
        if (imsi == "") {
            LOG_ERROR("Can't get IMSI code from cellular, fall back to Poland country code");
            return (app::defaultCountryCode);
        }
        LOG_DEBUG("getCountryPrefix imsi:%s", imsi.c_str());
        const uint32_t country_code = DBServiceAPI::GetCountryCodeByMCC(application, std::stoul(imsi));
        if (country_code <= 0) {
            LOG_ERROR("Can't get country code from database, fall back to Poland country code");
            return (app::defaultCountryCode);
        }
        LOG_DEBUG("getCountryPrefix country_code: %" PRIu32, country_code);
        std::string buf = "+";
        buf += std::to_string(country_code);

        LOG_DEBUG("getCountryPrefix return: \"%s\"", buf.c_str());
        return (buf);
    }

    void PhonebookNewContact::showDialogDuplicatedNumber(ContactRecord &newContactRecord,
                                                         const utils::PhoneNumber::View &duplicatedNumber)
    {
        auto dialog = dynamic_cast<gui::DialogYesNo *>(this->application->getWindow(gui::window::name::dialog_yes_no));
        assert(dialog != nullptr);
        auto meta             = dialog->meta;
        auto matchedContact   = DBServiceAPI::MatchContactByPhoneNumber(this->application, duplicatedNumber);
        auto oldContactRecord = (matchedContact != nullptr) ? *matchedContact : ContactRecord{};
        newContactRecord.ID   = oldContactRecord.ID;
        meta.action           = [=]() -> bool {
            if (!DBServiceAPI::ContactUpdate(this->application, newContactRecord)) {
                LOG_ERROR("Contact id=%" PRIu32 " update failed", newContactRecord.ID);
                return false;
            }
            this->application->switchWindow(gui::name::window::main_window);
            return true;
        };
        std::string duplicatedNumberPhrase = utils::localize.get("app_phonebook_duplicate_numbers");
        fillContactData(duplicatedNumberPhrase, oldContactRecord);
        meta.text  = duplicatedNumberPhrase;
        meta.title = duplicatedNumber.getFormatted();
        meta.icon  = "info_big_circle_W_G";
        dialog->update(meta);
        this->application->switchWindow(dialog->getName());
    }

    void PhonebookNewContact::showDialogDuplicatedSpeedDialNumber(ContactRecord &newContactRecord)
    {
        auto dialog = dynamic_cast<gui::DialogYesNo *>(this->application->getWindow(gui::window::name::dialog_yes_no));
        assert(dialog != nullptr);
        auto meta              = dialog->meta;
        auto contactRecordsPtr = DBServiceAPI::ContactGetBySpeeddial(this->application, newContactRecord.speeddial);
        auto oldContactRecord  = !contactRecordsPtr->empty() ? contactRecordsPtr->front() : ContactRecord{};
        newContactRecord.ID    = oldContactRecord.ID;
        meta.action            = [=]() -> bool {
            if (!DBServiceAPI::ContactUpdate(this->application, newContactRecord)) {
                LOG_ERROR("Contact id=%" PRIu32 " update failed", newContactRecord.ID);
                return false;
            }
            this->application->switchWindow(gui::name::window::main_window);
            return true;
        };
        std::string duplicatedSpeedDialPhrase = utils::localize.get("app_phonebook_duplicate_numbers");
        fillContactData(duplicatedSpeedDialPhrase, oldContactRecord);
        std::string duplicatedSpeedDialTitle = utils::localize.get("app_phonebook_duplicate_speed_dial_title");
        fillContactData(duplicatedSpeedDialTitle, oldContactRecord);
        meta.text  = duplicatedSpeedDialPhrase;
        meta.title = duplicatedSpeedDialTitle;
        meta.icon  = "info_big_circle_W_G";
        dialog->update(meta);
        this->application->switchWindow(dialog->getName());
    }

} // namespace gui
