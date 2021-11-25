// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SearchEngineWindow.hpp"

#include <application-notes/ApplicationNotes.hpp>
#include <module-apps/application-notes/data/NotesFoundData.hpp>
#include <apps-common/widgets/InputBox.hpp>

#include <i18n/i18n.hpp>

namespace app::notes
{
    SearchEngineWindow::SearchEngineWindow(ApplicationCommon *application,
                                           std::unique_ptr<SearchEngineWindowContract::Presenter> &&windowPresenter)
        : gui::AppWindow{application, gui::name::window::notes_search}, presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();
    }

    SearchEngineWindow::~SearchEngineWindow() noexcept
    {
        destroyInterface();
    }

    void SearchEngineWindow::buildInterface()
    {
        AppWindow::buildInterface();
        setTitle(utils::translate("app_notes_title_main"));

        navBar->setActive(gui::nav_bar::Side::Center, true);
        navBar->setText(gui::nav_bar::Side::Center, utils::translate(style::strings::common::search));
        navBar->setActive(gui::nav_bar::Side::Right, true);
        navBar->setText(gui::nav_bar::Side::Right, utils::translate(style::strings::common::back));

        input = gui::inputBox(this, utils::translate("common_search_uc"), "search_32px_W_G");
        setFocusItem(input);
    }

    void SearchEngineWindow::destroyInterface()
    {
        erase();
        input = nullptr;
    }

    bool SearchEngineWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(gui::KeyCode::KEY_ENTER)) {
            presenter->searchFor(input->getText());
            return true;
        }
        return AppWindow::onInput(inputEvent);
    }

    void SearchEngineWindow::emptySearch()
    {
        application->switchWindow(gui::name::window::notes_search_result, std::make_unique<NotesFoundData>(""));
    }

    void SearchEngineWindow::processValidSearch(const std::string &searchText)
    {
        application->switchWindow(gui::name::window::notes_search_result, std::make_unique<NotesFoundData>(searchText));
    }
} // namespace app::notes
