// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimContactsImportWindow.hpp"

#include <application-settings/windows/WindowNames.hpp>
#include <InputEvent.hpp>
#include <service-appmgr/Controller.hpp>

namespace gui
{
    SimContactsImportWindow::SimContactsImportWindow(
        app::Application *app, std::unique_ptr<SimContactsImportWindowContract::Presenter> simImportPresenter)
        : AppWindow(app, gui::window::name::import_contacts), presenter(std::move(simImportPresenter))
    {
        presenter->attach(this);
        preventsAutoLock = true;
        buildInterface();
    }

    void SimContactsImportWindow::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::translate("app_settings_network_import_contacts"));

        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::translate(::style::strings::common::back));
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::translate(::style::strings::common::import));

        list = new ListView(this,
                            style::window::default_left_margin,
                            style::window::default_vertical_pos,
                            style::listview::body_width_with_scroll,
                            style::window::default_body_height,
                            presenter->getSimContactsProvider(),
                            listview::ScrollBarType::Fixed);

        emptyListIcon = new gui::Icon(this,
                                      ::style::window::default_left_margin,
                                      ::style::window::default_vertical_pos,
                                      ::style::window::default_body_width,
                                      ::style::window::default_body_height,
                                      "info_icon_W_G",
                                      utils::translate("app_settings_network_import_contacts_from_sim_card_reading"));
        emptyListIcon->setAlignment(Alignment::Horizontal::Center);

        list->emptyListCallback    = [this]() { emptyListIcon->setVisible(true); };
        list->notEmptyListCallback = [this]() { emptyListIcon->setVisible(false); };

        setFocusItem(list);
    }

    void SimContactsImportWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        emptyListIcon->text->setRichText(
            utils::translate("app_settings_network_import_contacts_from_sim_card_reading"));
        emptyListIcon->image->set("update_icon_W_G");
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setActive(BottomBar::Side::CENTER, false);
        bottomBar->setActive(BottomBar::Side::LEFT, false);

        presenter->requestSimContacts();
    }

    void SimContactsImportWindow::onClose(CloseReason reason)
    {
        if (reason != CloseReason::PhoneLock) {
            presenter->eraseProviderData();
        }
    }

    bool SimContactsImportWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isKeyRelease(gui::KeyCode::KEY_LF) && !bottomBar->isActive(BottomBar::Side::LEFT)) {
            return false;
        }

        if (inputEvent.isKeyRelease(gui::KeyCode::KEY_ENTER) && !bottomBar->isActive(BottomBar::Side::CENTER)) {
            return false;
        }

        if (inputEvent.isKeyRelease(gui::KeyCode::KEY_RF) && !bottomBar->isActive(BottomBar::Side::RIGHT)) {
            return false;
        }

        if (inputEvent.isKeyRelease(gui::KeyCode::KEY_LF) && presenter->isRequestCompleted() && onLFInputCallback) {
            onLFInputCallback();
            return true;
        }

        if (inputEvent.isKeyRelease(gui::KeyCode::KEY_ENTER) && presenter->isRequestCompleted() &&
            onEnterInputCallback) {
            onEnterInputCallback();
            return true;
        }

        return AppWindow::onInput(inputEvent);
    }

    void SimContactsImportWindow::contactsReady() noexcept
    {
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        list->rebuildList();

        if (list->isEmpty()) {
            bottomBar->setActive(BottomBar::Side::CENTER, false);
            emptyListIcon->text->setRichText(
                utils::translate("app_settings_network_import_contacts_from_sim_card_no_contacts"));
            emptyListIcon->image->set("info_icon_W_G");
        }
        else {
            setTitle(utils::translate("app_settings_network_import_contacts_from_sim_card"));
            onEnterInputCallback = [&]() {
                displayProgressInfo();
                presenter->saveImportedContacts();
            };
        }
    }

    void SimContactsImportWindow::displayDuplicatesCount(unsigned int duplicatesCount) noexcept
    {
        list->rebuildList();

        setTitle(utils::translate("app_settings_network_import_contacts"));
        emptyListIcon->text->setRichText(
            utils::translate("app_settings_network_import_contacts_from_sim_card_duplicates"),
            {{"$DUPLICATES", std::to_string(duplicatesCount)}});
        emptyListIcon->image->set("info_icon_W_G");
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::translate(::style::strings::common::show));
        bottomBar->setText(gui::BottomBar::Side::LEFT, utils::translate(::style::strings::common::skip));

        onLFInputCallback = [&]() {
            displayProgressInfo();
            presenter->saveImportedContacts();
        };
        onEnterInputCallback = [&]() { presenter->requestDuplicates(); };
    }

    void SimContactsImportWindow::displayDuplicates() noexcept
    {
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::translate(::style::strings::common::replace));
        list->rebuildList();

        setTitle(utils::translate("app_settings_network_import_contacts_duplicates"));
        bottomBar->setActive(BottomBar::Side::RIGHT, true);

        onLFInputCallback    = nullptr;
        onEnterInputCallback = [&]() {
            displayProgressInfo();
            presenter->saveImportedContacts();
        };
    }

    void SimContactsImportWindow::contactsImported() noexcept
    {
        list->rebuildList();

        setTitle(utils::translate("app_settings_network_import_contacts_from_sim_card"));
        emptyListIcon->text->setRichText(
            utils::translate("app_settings_network_import_contacts_from_sim_card_success"));
        emptyListIcon->image->set("success_icon_W_G");
        bottomBar->setActive(BottomBar::Side::RIGHT, false);
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::translate(::style::strings::common::ok));
        bottomBar->setText(gui::BottomBar::Side::LEFT, utils::translate("app_desktop_menu_contacts"));

        onLFInputCallback = [&]() {
            app::manager::Controller::sendAction(application, app::manager::actions::ShowContacts);
        };
        onEnterInputCallback = [&]() { application->switchWindow(gui::window::name::sim_cards); };
    }

    void SimContactsImportWindow::displayProgressInfo()
    {
        list->clear();
        list->emptyListCallback();
        emptyListIcon->text->setRichText(
            utils::translate("app_settings_network_import_contacts_from_sim_card_reading"));
        emptyListIcon->image->set("update_icon_W_G");
        bottomBar->setActive(BottomBar::Side::RIGHT, false);
        bottomBar->setActive(BottomBar::Side::CENTER, false);
        bottomBar->setActive(BottomBar::Side::LEFT, false);
    }
} // namespace gui
