// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhonebookSearch.hpp"
#include "application-phonebook/ApplicationPhonebook.hpp"
#include "application-phonebook/data/PhonebookItemData.hpp"
#include "widgets/InputBox.hpp"

namespace gui
{
    PhonebookSearch::PhonebookSearch(app::ApplicationCommon *app) : AppWindow(app, gui::window::name::search)
    {
        buildInterface();
    }

    void PhonebookSearch::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::translate("app_phonebook_title_main"));

        inputField = inputBox(this, utils::translate("common_search_uc"), "search_32px_W_G");
        inputField->setInputMode(new InputMode(
            {InputMode::Abc, InputMode::ABC, InputMode::abc, InputMode::digit},
            [=](const UTF8 &Text) { application->getCurrentWindow()->navBarTemporaryMode(Text); },
            [=]() { application->getCurrentWindow()->navBarRestoreFromTemporaryMode(); },
            [=]() { application->getCurrentWindow()->selectSpecialCharacter(); }));

        navBar->setActive(nav_bar::Side::Left, false);
        navBar->setActive(nav_bar::Side::Center, true);
        navBar->setActive(nav_bar::Side::Right, true);

        navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::search));
        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));

        setFocusItem(inputField);
    }

    auto PhonebookSearch::onInput(const InputEvent &inputEvent) -> bool
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }
        if (!inputEvent.isShortRelease()) {
            return false;
        }
        if (!inputEvent.is(gui::KeyCode::KEY_ENTER)) {
            return false;
        }

        std::string searchFilter = utils::trim(inputField->getText());
        if (searchFilter.empty()) {
            return false;
        }

        auto app = dynamic_cast<app::ApplicationPhonebook *>(application);
        if (app == nullptr) {
            LOG_ERROR("Failed to get phonebook application.");
            return false;
        }

        app->onSearchRequest(searchFilter);
        return true;
    }
} // namespace gui
