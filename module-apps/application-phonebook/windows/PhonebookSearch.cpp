// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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

        inputField = inputBox(this, utils::translate("common_search_uc"), "search");
        inputField->setInputMode(new InputMode(
            {InputMode::ABC, InputMode::abc, InputMode::digit},
            [=](const UTF8 &Text) { application->getCurrentWindow()->bottomBarTemporaryMode(Text); },
            [=]() { application->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); },
            [=]() { application->getCurrentWindow()->selectSpecialCharacter(); }));

        bottomBar->setActive(BottomBar::Side::LEFT, false);
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);

        bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::search));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));

        setFocusItem(inputField);
    }

    auto PhonebookSearch::handleSwitchData(SwitchData *data) -> bool
    {
        if (data == nullptr) {
            LOG_ERROR("Received null pointer");
            return false;
        }

        auto item = dynamic_cast<PhonebookSearchQuery *>(data);
        assert(item != nullptr);
        inputField->setText(item->getQuery());

        return true;
    }

    void PhonebookSearch::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        inputField->clear();
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
