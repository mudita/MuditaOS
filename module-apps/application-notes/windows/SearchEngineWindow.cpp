// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SearchEngineWindow.hpp"

#include <module-apps/application-notes/ApplicationNotes.hpp>
#include <module-apps/application-notes/data/NotesFoundData.hpp>
#include <module-apps/widgets/InputBox.hpp>

#include <i18n/i18n.hpp>

namespace app::notes
{
    SearchEngineWindow::SearchEngineWindow(Application *application,
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

        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::translate(style::strings::common::search));
        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));

        input = gui::inputBox(this, utils::translate("common_search_uc"), "search");
        setFocusItem(input);
    }

    void SearchEngineWindow::destroyInterface()
    {
        erase();
        input = nullptr;
    }

    bool SearchEngineWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortPress() && inputEvent.is(gui::KeyCode::KEY_ENTER)) {
            presenter->searchFor(input->getText());
            return true;
        }
        return AppWindow::onInput(inputEvent);
    }

    void SearchEngineWindow::notesFound(const std::vector<NotesRecord> &notes, const std::string &searchText)
    {
        application->switchWindow(gui::name::window::notes_search_result,
                                  std::make_unique<NotesFoundData>(searchText, notes));
    }
} // namespace app::notes
