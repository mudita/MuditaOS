// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MusicPlayerEmptyWindow.hpp"
#include "application-music-player/ApplicationMusicPlayer.hpp"
#include "application-music-player/data/MusicPlayerStyle.hpp"

#include <Style.hpp>
#include <i18n/i18n.hpp>
#include <log/log.hpp>
#include <service-audio/AudioServiceAPI.hpp>

namespace gui
{
    using namespace musicPlayerStyle::emptyWindow;

    MusicPlayerEmptyWindow::MusicPlayerEmptyWindow(app::Application *app)
        : AppWindow(app, gui::name::window::main_window)
    {
        buildInterface();
    }

    void MusicPlayerEmptyWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void MusicPlayerEmptyWindow::buildInterface()
    {
        AppWindow::buildInterface();

        bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get("app_music_player_music_library"));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_music_player_quit"));

        img = new gui::Image(this, noteImg::x, noteImg::y, "note");

        text = new Text(this, infoText::x, infoText::y, infoText::w, infoText::h);
        text->setText(utils::localize.get("app_music_player_music_empty_window_notification"));
        text->setTextType(TextType::MultiLine);
        text->setEditMode(EditMode::Browse);
        text->setEdges(RectangleEdge::None);
        text->setFont(style::window::font::medium);
        text->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));

        placeHolder = new gui::Image(this, placeHolderImg::x, placeHolderImg::y, "placeholder_player");
    }

    void MusicPlayerEmptyWindow::destroyInterface()
    {
        erase();
    }

    void MusicPlayerEmptyWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {}

    bool MusicPlayerEmptyWindow::onDatabaseMessage(sys::Message *msgl)
    {
        return false;
    }

    bool MusicPlayerEmptyWindow::onInput(const InputEvent &inputEvent)
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        if (!inputEvent.isShortPress()) {
            return false;
        }

        if (inputEvent.is(gui::KeyCode::KEY_LF)) {
            application->switchWindow(gui::name::window::all_songs_window);
            return true;
        }

        return false;
    }

} /* namespace gui */
