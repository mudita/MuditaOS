// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NoteMainWindow.hpp"

#include <application-notes/ApplicationNotes.hpp>
#include <module-apps/application-notes/data/NoteSwitchData.hpp>

#include <InputEvent.hpp>

#include <i18n/i18n.hpp>

#include <Style.hpp>
#include <header/AddElementAction.hpp>
#include <header/SearchAction.hpp>
#include <module-apps/application-notes/style/NotesListStyle.hpp>

#include <service-db/DBNotificationMessage.hpp>

namespace app::notes
{
    NoteMainWindow::NoteMainWindow(app::ApplicationCommon *app,
                                   std::unique_ptr<NotesMainWindowContract::Presenter> &&windowPresenter)
        : AppWindow(app, gui::name::window::main_window), presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();
    }

    NoteMainWindow::~NoteMainWindow() noexcept
    {
        destroyInterface();
    }

    void NoteMainWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void NoteMainWindow::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::translate("app_notes_title_main"));
        header->navigationIndicatorAdd(new gui::header::AddElementAction(), gui::header::BoxSelection::Left);
        header->navigationIndicatorAdd(new gui::header::SearchAction(), gui::header::BoxSelection::Right);

        navBar->setActive(gui::nav_bar::Side::Left, true);
        navBar->setText(gui::nav_bar::Side::Left, utils::translate(::style::strings::common::options));
        navBar->setActive(gui::nav_bar::Side::Center, true);
        navBar->setText(gui::nav_bar::Side::Center, utils::translate(::style::strings::common::open));
        navBar->setActive(gui::nav_bar::Side::Right, true);
        navBar->setText(gui::nav_bar::Side::Right, utils::translate(::style::strings::common::back));

        namespace listStyle = app::notes::style::list;
        list                = new gui::ListView(this,
                                 listStyle::X,
                                 listStyle::Y,
                                 listStyle::Width,
                                 listStyle::Height,
                                 presenter->getNotesItemProvider(),
                                 gui::listview::ScrollBarType::Fixed);
        list->setPenWidth(listStyle::PenWidth);
        list->setPenFocusWidth(listStyle::FocusedPenWidth);
        list->focusChangedCallback = [this]([[maybe_unused]] gui::Item &item) {
            onListFilled();
            return true;
        };
        list->setVisible(true);
        list->rebuildList();

        emptyListIcon =
            new gui::Icon(this,
                          0,
                          ::style::window::default_vertical_pos,
                          ::style::window_width,
                          ::style::window_height - ::style::window::default_vertical_pos - ::style::footer::height,
                          "empty_list_add_W_G",
                          utils::translate("app_notes_no_notes"));
        emptyListIcon->setVisible(false);

        list->emptyListCallback    = [this]() { onEmptyList(); };
        list->notEmptyListCallback = [this]() { onListFilled(); };

        setFocusItem(list);
    }

    void NoteMainWindow::destroyInterface()
    {
        erase();
        list          = nullptr;
        emptyListIcon = nullptr;
    }

    void NoteMainWindow::onEmptyList()
    {
        navBar->setActive(gui::nav_bar::Side::Left, false);
        navBar->setActive(gui::nav_bar::Side::Center, false);
        emptyListIcon->setVisible(true);
        header->navigationIndicatorRemove(gui::header::BoxSelection::Right);
        application->refreshWindow(gui::RefreshModes::GUI_REFRESH_DEEP);
    }

    void NoteMainWindow::onListFilled()
    {
        navBar->setActive(gui::nav_bar::Side::Left, true);
        navBar->setActive(gui::nav_bar::Side::Center, true);
        emptyListIcon->setVisible(false);
        header->navigationIndicatorAdd(new gui::header::SearchAction(), gui::header::BoxSelection::Right);
        application->refreshWindow(gui::RefreshModes::GUI_REFRESH_DEEP);
    }

    bool NoteMainWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease()) {
            if (inputEvent.is(gui::KeyCode::KEY_LEFT)) {
                application->switchWindow(gui::name::window::note_create,
                                          std::make_unique<NoteSwitchData>(std::make_shared<NotesRecord>()));
            }
            else if (inputEvent.is(gui::KeyCode::KEY_RIGHT) &&
                     header->navigationIndicatorVisible(gui::header::BoxSelection::Right)) {
                application->switchWindow(gui::name::window::notes_search);
            }
        }
        return AppWindow::onInput(inputEvent);
    }

    bool NoteMainWindow::onDatabaseMessage(sys::Message *msg)
    {
        auto *msgNotification = dynamic_cast<db::NotificationMessage *>(msg);
        if (msgNotification != nullptr) {
            if (msgNotification->interface == db::Interface::Name::Notes) {
                if (msgNotification->dataModified()) {

                    list->rebuildList(::gui::listview::RebuildType::InPlace);

                    return true;
                }
            }
        }
        return false;
    }
} // namespace app::notes
