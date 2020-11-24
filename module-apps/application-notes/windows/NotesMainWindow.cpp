// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotesMainWindow.hpp"

#include <InputEvent.hpp>
#include <service-db/DBNotesMessage.hpp>

#include <i18/i18.hpp>

#include <gui/widgets/Label.hpp>
#include <gui/widgets/BottomBar.hpp>
#include <gui/widgets/TopBar.hpp>

#include <Style.hpp>
#include <module-apps/application-notes/style/NotesListStyle.hpp>

namespace app::notes
{
    NotesMainWindow::NotesMainWindow(app::Application *app,
                                     std::unique_ptr<NotesMainWindowContract::Presenter> &&windowPresenter)
        : AppWindow(app, gui::name::window::main_window), presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();
    }

    void NotesMainWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void NotesMainWindow::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::localize.get("app_notes_title_main"));

        namespace listStyle = app::notes::style::list;
        list                = new gui::ListView(
            this, listStyle::X, listStyle::Y, listStyle::Width, listStyle::Height, presenter->getNotesItemProvider());
        list->setPenWidth(listStyle::PenWidth);
        list->setPenFocusWidth(listStyle::FocusedPenWidth);

        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::localize.get(::style::strings::common::open));

        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(::style::strings::common::back));

        topBar->setActive(gui::TopBar::Elements::TIME, true);

        setFocusItem(list);
    }

    void NotesMainWindow::destroyInterface()
    {
        erase();
        list = nullptr;
    }

    void NotesMainWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        list->rebuildList();
    }

    bool NotesMainWindow::onInput(const gui::InputEvent &inputEvent)
    {
        return AppWindow::onInput(inputEvent);
    }

    bool NotesMainWindow::onDatabaseMessage(sys::Message *msgl)
    {
        auto *msg            = static_cast<DBNotesResponseMessage *>(msgl);
        const auto &notesDao = presenter->getNotesDAO();
        return notesDao->updateRecords(std::move(*msg->records));
    }
} // namespace app::notes
