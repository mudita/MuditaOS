/*
 * @file NotesMainWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 31 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include <memory>
#include <functional>

#include "InputEvent.hpp"
#include "service-appmgr/ApplicationManager.hpp"

#include "../ApplicationNotes.hpp"

#include "service-db/messages/DBNotesMessage.hpp"
#include "i18/i18.hpp"

#include "Label.hpp"
#include "Margins.hpp"
#include "NotesMainWindow.hpp"
#include <Style.hpp>

namespace gui
{

    NotesMainWindow::NotesMainWindow(app::Application *app)
        : AppWindow(app, gui::name::window::main_window), notesModel{std::make_shared<NotesModel>(app)}
    {
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

        list = new gui::ListView(this, 16, 105, 480 - 32, 440, notesModel);
        list->setPenFocusWidth(0);
        list->setPenWidth(0);

        setFocusItem(list);

        setTitle(utils::localize.get("app_notes_title_main"));

        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::open));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        topBar->setActive(TopBar::Elements::TIME, true);
    }

    void NotesMainWindow::destroyInterface()
    {
        erase();
    }

    void NotesMainWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        list->rebuildList();
    }

    bool NotesMainWindow::onInput(const InputEvent &inputEvent)
    {

        // process only if key is released
        if ((inputEvent.state != InputEvent::State::keyReleasedShort) ||
            (inputEvent.state != InputEvent::State::keyReleasedLong)) {
            if (inputEvent.keyCode == KeyCode::KEY_LEFT) {
                application->switchWindow("EditWindow");
                return true;
            }
        }

        return AppWindow::onInput(inputEvent);
    }

    bool NotesMainWindow::onDatabaseMessage(sys::Message *msgl)
    {
        DBNotesResponseMessage *msg = reinterpret_cast<DBNotesResponseMessage *>(msgl);
        if (notesModel->updateRecords(std::move(msg->records)))
            return true;

        return false;
    }

} /* namespace gui */
