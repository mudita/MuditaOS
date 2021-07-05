// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SearchResultsWindow.hpp"

#include <application-notes/ApplicationNotes.hpp>
#include <module-apps/application-notes/data/NotesFoundData.hpp>
#include <module-apps/application-notes/style/NotesListStyle.hpp>
#include <apps-common/windows/DialogMetadata.hpp>
#include <apps-common/messages/DialogMetadataMessage.hpp>

namespace app::notes
{
    SearchResultsWindow::SearchResultsWindow(Application *application,
                                             std::unique_ptr<NotesSearchWindowContract::Presenter> &&windowPresenter)
        : AppWindow(application, gui::name::window::notes_search_result), presenter(std::move(windowPresenter))
    {
        buildInterface();
    }

    SearchResultsWindow::~SearchResultsWindow() noexcept
    {
        destroyInterface();
    }

    void SearchResultsWindow::buildInterface()
    {
        AppWindow::buildInterface();
        setTitle(utils::translate("app_notes_title_main"));

        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::translate(::style::strings::common::open));
        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::translate(::style::strings::common::back));

        list = new gui::ListView(this,
                                 style::list::X,
                                 style::list::Y,
                                 style::list::Width,
                                 style::list::Height,
                                 presenter->getNotesItemProvider(),
                                 gui::listview::ScrollBarType::Fixed);
        list->setScrollTopMargin(::style::margins::small);
        list->emptyListCallback = [&]() { onNothingFound(presenter->getSearchText()); };
        setFocusItem(list);
    }

    void SearchResultsWindow::destroyInterface()
    {
        erase();
        list = nullptr;
    }

    void SearchResultsWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        if (auto foundNotesData = dynamic_cast<NotesFoundData *>(data); foundNotesData != nullptr) {
            presenter->setSearchText(foundNotesData->getSearchText());
            list->rebuildList();
        }
    }

    void SearchResultsWindow::onNothingFound(const std::string &searchText)
    {
        gui::DialogMetadata meta{utils::translate("common_results_prefix") + searchText,
                                 "search_big",
                                 utils::translate("app_notes_search_no_results")};
        auto data                        = std::make_unique<gui::DialogMetadataMessage>(meta);
        data->ignoreCurrentWindowOnStack = true;
        application->switchWindow(gui::name::window::note_dialog, std::move(data));
    }

    void SearchResultsWindow::onResultsFilled()
    {
        list->rebuildList(gui::listview::RebuildType::InPlace);
    }

    bool SearchResultsWindow::onDatabaseMessage(sys::Message *msg)
    {
        if (auto *message = dynamic_cast<db::NotificationMessage *>(msg);
            message != nullptr && message->interface == db::Interface::Name::Notes) {
            presenter->handleSearchResults(message);
        }
        return false;
    }
} // namespace app::notes
