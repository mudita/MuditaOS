/*
 * @file NotesEditWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 13 sie 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include <memory>
#include <functional>

#include "service-appmgr/ApplicationManager.hpp"

#include "bsp/rtc/rtc.hpp"

#include "../ApplicationNotes.hpp"

#include "service-db/messages/DBMessage.hpp"
#include "i18/i18.hpp"

#include "Label.hpp"
#include "Margins.hpp"
#include "Text.hpp"
#include "NotesEditWindow.hpp"
#include <Style.hpp>

UTF8 textString = "Very long test line ABCDEFGHIJKLMNOPQRST123456789\n"
                  "abcdefghijklmnopqrs 123456789 ABCDEFGHIJKLMONPQRSTUW 12345\n    test\nnew line\n\n\n12345";

namespace gui
{

    NotesEditWindow::NotesEditWindow(app::Application *app) : AppWindow(app, "EditWindow")
    {
        buildInterface();
    }

    void NotesEditWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }
    void NotesEditWindow::buildInterface()
    {
        AppWindow::buildInterface();

        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_notes_save"));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_notes_back"));

        setTitle(utils::localize.get("app_notes_new_note"));

        text = new gui::Text(this, 11, 105, 480 - 22, 600 - 105 - 50);
        text->setFont(style::window::font::medium);
        text->setRadius(5);
        text->setMargins(gui::Margins(10, 5, 10, 5));
        text->activatedCallback = [=](gui::Item &item) {
            UTF8 getstr = text->getText();
            time_t timestamp;
            bsp::rtc_GetCurrentTimestamp(&timestamp);

            UTF8 filename = "sys/data/applications/notes/" + std::to_string(timestamp) + ".txt";

            auto file = vfs.fopen(filename.c_str(), "wb");
            vfs.fwrite(getstr.c_str(), getstr.used() - 1, 1, file);
            vfs.fclose(file);
            return true;
        };
        text->setTextType(gui::Text::TextType::MULTI_LINE);
        text->setEditMode(gui::Text::EditMode::BROWSE);
        text->setInputMode(new InputMode({InputMode::ABC, InputMode::abc}));

        topBar->setActive(TopBar::Elements::TIME, true);
    }

    void NotesEditWindow::destroyInterface()
    {
        erase();
    }

    NotesEditWindow::~NotesEditWindow()
    {
        destroyInterface();
    }

    void NotesEditWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        text->setText(textString);
        setFocusItem(text);
    }

} /* namespace gui */
