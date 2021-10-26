// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationMessages.hpp"
#include "SearchStart.hpp"

#include <i18n/i18n.hpp>
#include <widgets/InputBox.hpp>

#include <cassert>

namespace gui
{

    SMSSearch::SMSSearch(app::ApplicationCommon *app) : AppWindow(app, name::window::thread_sms_search)
    {
        buildInterface();
        setTitle(utils::translate("app_messages_title_main"));
        navBar->setActive(nav_bar::Side::Center, true);
        navBar->setActive(nav_bar::Side::Right, true);
        navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::search));
        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));
        body = new gui::Item();
        body->setBoundingBox(bodySize());
        addWidget(body);
        auto text = inputBox(this, utils::translate("common_search_uc"), "search");
        text->setInputMode(new InputMode(
            {InputMode::ABC, InputMode::abc, InputMode::digit},
            [=](const UTF8 &Text) { application->getCurrentWindow()->navBarTemporaryMode(Text); },
            [=]() { application->getCurrentWindow()->navBarRestoreFromTemporaryMode(); },
            [=]() { application->getCurrentWindow()->selectSpecialCharacter(); }));

        inputCallback = [=](Item &, const InputEvent &inputEvent) -> bool {
            auto app = dynamic_cast<app::ApplicationMessages *>(application);
            assert(app);
            if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
                auto search_text = text->getText();
                app->showSearchResults(utils::translate("common_search_results") + ": " + std::string(search_text),
                                       search_text);
                return true;
            }
            return false;
        };

        setFocusItem(text);
    }

    bool SMSSearch::onInput(const InputEvent &inputEvent)
    {
        return AppWindow::onInput(inputEvent) || (inputCallback && inputCallback(*this, inputEvent));
    }

} // namespace gui
