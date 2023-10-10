// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhonebookMainWindow.hpp"
#include "application-phonebook/ApplicationPhonebook.hpp"
#include "application-phonebook/data/PhonebookItemData.hpp"
#include "application-phonebook/data/PhonebookStyle.hpp"

#include <queries/phonebook/QueryContactGet.hpp>

#include <header/AddElementAction.hpp>
#include <header/SearchAction.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-db/DBNotificationMessage.hpp>

namespace gui
{
    PhonebookMainWindow::PhonebookMainWindow(app::ApplicationCommon *app,
                                             std::shared_ptr<SearchRequestModel> model,
                                             std::shared_ptr<PhonebookModel> phonebookModel)
        : AppWindow(app, gui::name::window::main_window),
          model(std::move(model)), phonebookModel{std::move(phonebookModel)}
    {
        buildInterface();
    }

    void PhonebookMainWindow::rebuild()
    {
        contactsList->rebuildList(gui::listview::RebuildType::InPlace);
    }

    void PhonebookMainWindow::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::translate("app_phonebook_title_main"));
        header->navigationIndicatorAdd(new gui::header::AddElementAction(), gui::header::BoxSelection::Left);
        header->navigationIndicatorAdd(new gui::header::SearchAction(), gui::header::BoxSelection::Right);

        navBar->setText(nav_bar::Side::Left, utils::translate(style::strings::common::call));
        navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::open));
        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));

        contactsList = new gui::PhonebookListView(this,
                                                  phonebookStyle::mainWindow::contactsList::x,
                                                  phonebookStyle::mainWindow::contactsList::y,
                                                  phonebookStyle::mainWindow::contactsList::w,
                                                  phonebookStyle::mainWindow::contactsList::h,
                                                  phonebookModel);
        contactsList->setBoundaries(Boundaries::Continuous);
        setFocusItem(contactsList);

        emptyListIcon =
            new gui::Icon(this,
                          0,
                          ::style::window::default_vertical_pos,
                          ::style::window_width,
                          ::style::window_height - ::style::window::default_vertical_pos - ::style::nav_bar::height,
                          "empty_list_add_W_G",
                          utils::translate("app_phonebook_no_contacts"));
        emptyListIcon->setVisible(false);

        contactsList->emptyListCallback    = [this]() { onEmptyList(); };
        contactsList->notEmptyListCallback = [this]() { onListFilled(); };

        phonebookModel->setDisplayMode(static_cast<std::uint32_t>(ContactDisplayMode::SortedByLetter));
        contactsList->rebuildList(gui::listview::RebuildType::Full);

        const auto app = application;
        inputMode      = std::make_unique<InputMode>(
            std::list<InputMode::Mode>{InputMode::ABC, InputMode::abc},
            [app](const UTF8 &text) { app->getCurrentWindow()->navBarTemporaryMode(text); },
            [app]() { app->getCurrentWindow()->navBarRestoreFromTemporaryMode(); },
            [app]() { app->getCurrentWindow()->selectSpecialCharacter(); },
            [app](std::function<void()> restoreFunction) {
                app->getCurrentWindow()->startInputModeRestoreTimer(std::move(restoreFunction));
            });
    }

    void PhonebookMainWindow::destroyInterface()
    {
        erase();
    }

    PhonebookMainWindow::~PhonebookMainWindow()
    {
        destroyInterface();
    }

    void PhonebookMainWindow::onBeforeShow([[maybe_unused]] ShowMode mode, SwitchData *data)
    {
        const auto contactRequest = dynamic_cast<PhonebookSearchRequest *>(data);
        model->setRequested(contactRequest != nullptr);
        if (model->requestedSearch()) {
            enableNewContact = false;

            navBar->setActive(nav_bar::Side::Left, false);
            navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::add));
            navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));

            header->navigationIndicatorRemove(gui::header::BoxSelection::Left);
        }
        else if (!contactsList->isEmpty()) {
            navBar->setText(nav_bar::Side::Left, utils::translate(style::strings::common::call));
            navBar->setActive(nav_bar::Side::Left, true);
        }
    }

    void PhonebookMainWindow::HandleFilteringByLetter(const InputEvent &inputEvent)
    {
        const auto code = translator.handle(inputEvent.getRawKey(), inputMode ? inputMode->get() : "");
        if (code != Profile::none_key) {
            const auto letter = static_cast<char>(code);
            LOG_DEBUG("Filtering by char=' %c'", letter);

            std::string filterLetter;
            filterLetter.push_back(letter);

            LOG_DEBUG("Number of favourites contacts : %" PRIu32, phonebookModel->letterMap.favouritesCount);
            const auto dataOffset = phonebookModel->letterMap.firstLetterDictionary[filterLetter];
            if (dataOffset != phonebookContactsMap::NO_MATCH_FOUND) {
                LOG_DEBUG("Phonebook data offset : %" PRIu32, dataOffset);
                phonebookModel->setDisplayMode(static_cast<std::uint32_t>(ContactDisplayMode::SortedByLetter));
                contactsList->rebuildList(gui::listview::RebuildType::OnOffset, dataOffset);
            }
        }
    }

    bool PhonebookMainWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease()) {
            switch (inputEvent.getKeyCode()) {
            case gui::KeyCode::KEY_LEFT:
                if (enableNewContact) {
                    auto data = std::make_unique<PhonebookItemData>();
                    application->switchWindow(
                        gui::window::name::new_contact, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
                    return true;
                }
                break;

            case gui::KeyCode::KEY_RIGHT:
                if (header->navigationIndicatorVisible(gui::header::BoxSelection::Right)) {
                    application->switchWindow(gui::window::name::search);
                    return true;
                }
                break;

            case gui::KeyCode::KEY_LF:
                if (!navBar->isActive(nav_bar::Side::Left)) {
                    return true;
                }
                break;

            default:
                HandleFilteringByLetter(inputEvent);
                break;
            }
        }
        return AppWindow::onInput(inputEvent);
    }

    bool PhonebookMainWindow::onDatabaseMessage(sys::Message *msgl)
    {
        const auto msgNotification = dynamic_cast<db::NotificationMessage *>(msgl);
        if ((msgNotification != nullptr) && (msgNotification->interface == db::Interface::Name::Contact) &&
            msgNotification->dataModified()) {
            phonebookModel->letterMap = phonebookModel->requestLetterMap();
            rebuild();
            return true;
        }
        return false;
    }

    void PhonebookMainWindow::onEmptyList()
    {
        navBar->setActive(gui::nav_bar::Side::Left, false);
        navBar->setActive(gui::nav_bar::Side::Center, false);
        if (model->requestedSearch()) {
            emptyListIcon->text->setRichText(utils::translate("app_phonebook_no_contacts_yet"));
            emptyListIcon->image->setVisible(false);
        }
        emptyListIcon->setVisible(true);
        header->navigationIndicatorRemove(gui::header::BoxSelection::Right);
        application->refreshWindow(RefreshModes::GUI_REFRESH_DEEP);
    }

    void PhonebookMainWindow::onListFilled()
    {
        if (!model->requestedSearch()) {
            navBar->setActive(gui::nav_bar::Side::Left, true);
            navBar->setActive(gui::nav_bar::Side::Center, true);
            header->navigationIndicatorAdd(new gui::header::SearchAction(), gui::header::BoxSelection::Right);
        }

        emptyListIcon->setVisible(false);
        application->refreshWindow(RefreshModes::GUI_REFRESH_DEEP);
    }
} /* namespace gui */
