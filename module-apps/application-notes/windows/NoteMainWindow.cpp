// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NoteMainWindow.hpp"

#include <module-apps/application-notes/ApplicationNotes.hpp>
#include <module-apps/application-notes/data/NoteSwitchData.hpp>

#include <InputEvent.hpp>

#include <i18n/i18n.hpp>

#include <Style.hpp>
#include <module-apps/application-notes/style/NotesListStyle.hpp>

#include <module-services/service-db/service-db/DBNotificationMessage.hpp>

namespace app::notes
{
    NoteMainWindow::NoteMainWindow(app::Application *app,
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
        headerIndicatorAdd(gui::header::NavigationIndicator::AddElementBox);
        headerIndicatorAdd(gui::header::NavigationIndicator::SearchBox);

        bottomBar->setActive(gui::BottomBar::Side::LEFT, true);
        bottomBar->setText(gui::BottomBar::Side::LEFT, utils::translate(::style::strings::common::options));
        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::translate(::style::strings::common::open));
        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::translate(::style::strings::common::back));

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
                          "phonebook_empty_grey_circle_W_G",
                          utils::translate("app_notes_no_notes"));
        emptyListIcon->focusChangedCallback = [this]([[maybe_unused]] gui::Item &item) {
            onEmptyList();
            return true;
        };

        list->emptyListCallback    = [this]() { onEmptyList(); };
        list->notEmptyListCallback = [this]() { onListFilled(); };

        setFocusItem(list);
    }

    void NoteMainWindow::destroyInterface()
    {
        erase();
        list            = nullptr;
        emptyListIcon   = nullptr;
    }

    void NoteMainWindow::onEmptyList()
    {
        bottomBar->setActive(gui::BottomBar::Side::LEFT, false);
        bottomBar->setActive(gui::BottomBar::Side::CENTER, false);
        emptyListIcon->setVisible(true);
        headerIndicatorRemove(gui::header::NavigationIndicator::AddElementBox);
        headerIndicatorRemove(gui::header::NavigationIndicator::SearchBox);
    }

    void NoteMainWindow::onListFilled()
    {
        bottomBar->setActive(gui::BottomBar::Side::LEFT, true);
        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        emptyListIcon->setVisible(false);
        headerIndicatorAdd(gui::header::NavigationIndicator::AddElementBox);
        headerIndicatorAdd(gui::header::NavigationIndicator::SearchBox);
    }

    bool NoteMainWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease()) {
            if (inputEvent.is(gui::KeyCode::KEY_LEFT)) {
                application->switchWindow(gui::name::window::note_edit,
                                          std::make_unique<NoteSwitchData>(NotesRecord{}));
            }
            else if (inputEvent.is(gui::KeyCode::KEY_RIGHT)) {
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
