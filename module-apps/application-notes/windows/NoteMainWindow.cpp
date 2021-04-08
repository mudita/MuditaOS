// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NoteMainWindow.hpp"

#include <module-apps/application-notes/ApplicationNotes.hpp>
#include <module-apps/application-notes/data/NoteSwitchData.hpp>

#include <InputEvent.hpp>

#include <i18n/i18n.hpp>

#include <gui/widgets/Label.hpp>
#include <gui/widgets/BottomBar.hpp>
#include <gui/widgets/TopBar.hpp>

#include <Style.hpp>
#include <module-apps/application-notes/style/NotesListStyle.hpp>
#include <module-apps/application-notes/style/NotesMainWindowStyle.hpp>

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

        setTitle(utils::localize.get("app_notes_title_main"));

        bottomBar->setActive(gui::BottomBar::Side::LEFT, true);
        bottomBar->setText(gui::BottomBar::Side::LEFT, utils::localize.get(::style::strings::common::options));
        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::localize.get(::style::strings::common::open));
        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(::style::strings::common::back));

        namespace windowStyle = app::notes::style::main_window;
        leftArrowImage        = new gui::Image(this,
                                        windowStyle::new_note_arrow::X,
                                        windowStyle::new_note_arrow::Y,
                                        0,
                                        0,
                                        windowStyle::new_note_arrow::ImageSource);
        rightArrowImage       = new gui::Image(this,
                                         windowStyle::search_arrow::X,
                                         windowStyle::search_arrow::Y,
                                         0,
                                         0,
                                         windowStyle::search_arrow::ImageSource);
        newNoteImage          = new gui::Image(this,
                                      windowStyle::new_note_image::X,
                                      windowStyle::new_note_image::Y,
                                      0,
                                      0,
                                      windowStyle::new_note_image::ImageSource);
        searchImage           = new gui::Image(this,
                                     windowStyle::search_image::X,
                                     windowStyle::search_image::Y,
                                     0,
                                     0,
                                     windowStyle::search_image::ImageSource);

        namespace listStyle = app::notes::style::list;
        list                = new gui::ListView(this,
                                 listStyle::X,
                                 listStyle::Y,
                                 listStyle::Width,
                                 listStyle::Height,
                                 presenter->getNotesItemProvider(),
                                 ::style::listview::ScrollBarType::Fixed);
        list->setPenWidth(listStyle::PenWidth);
        list->setPenFocusWidth(listStyle::FocusedPenWidth);
        list->focusChangedCallback = [this]([[maybe_unused]] gui::Item &item) {
            onListFilled();
            return true;
        };
        list->setVisible(true);
        list->rebuildList();

        emptyListIcon                       = new gui::Icon(this,
                                      0,
                                      ::style::header::height,
                                      ::style::window_width,
                                      ::style::window_height - ::style::header::height - ::style::footer::height,
                                      "phonebook_empty_grey_circle_W_G",
                                      utils::localize.get("app_notes_no_notes"));
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
        leftArrowImage  = nullptr;
        rightArrowImage = nullptr;
        newNoteImage    = nullptr;
        searchImage     = nullptr;
        emptyListIcon   = nullptr;
    }

    void NoteMainWindow::onEmptyList()
    {
        bottomBar->setActive(gui::BottomBar::Side::LEFT, false);
        bottomBar->setActive(gui::BottomBar::Side::CENTER, false);
        emptyListIcon->setVisible(true);
        rightArrowImage->setVisible(false);
        searchImage->setVisible(false);
    }

    void NoteMainWindow::onListFilled()
    {
        bottomBar->setActive(gui::BottomBar::Side::LEFT, true);
        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        emptyListIcon->setVisible(false);
        rightArrowImage->setVisible(true);
        searchImage->setVisible(true);
    }

    bool NoteMainWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortPress()) {
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

                    list->rebuildList(::style::listview::RebuildType::InPlace);

                    return true;
                }
            }
        }
        return false;
    }
} // namespace app::notes
