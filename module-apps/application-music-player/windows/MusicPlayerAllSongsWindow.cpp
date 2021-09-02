// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MusicPlayerAllSongsWindow.hpp"
#include <application-music-player/ApplicationMusicPlayer.hpp>
#include <data/MusicPlayerStyle.hpp>

#include <Style.hpp>
#include <cassert>
#include <i18n/i18n.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <gui/widgets/ListView.hpp>
#include <gui/widgets/Icon.hpp>

namespace gui
{

    MusicPlayerAllSongsWindow::MusicPlayerAllSongsWindow(
        app::Application *app, std::shared_ptr<app::music_player::SongsContract::Presenter> windowPresenter)
        : AppWindow(app, gui::name::window::all_songs_window), presenter{windowPresenter}
    {
        presenter->attach(this);
        buildInterface();
    }

    void MusicPlayerAllSongsWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void MusicPlayerAllSongsWindow::buildInterface()
    {
        AppWindow::buildInterface();

        bottomBar->setText(BottomBar::Side::CENTER, utils::translate("app_music_player_music_library"));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate("app_music_player_quit"));

        songsList = new gui::ListView(this,
                                      musicPlayerStyle::allSongsWindow::x,
                                      musicPlayerStyle::allSongsWindow::y,
                                      musicPlayerStyle::allSongsWindow::w,
                                      musicPlayerStyle::allSongsWindow::h,
                                      presenter->getMusicPlayerItemProvider(),
                                      listview::ScrollBarType::Fixed);

        emptyListIcon = new gui::Icon(this,
                                      ::style::window::default_left_margin,
                                      ::style::window::default_vertical_pos,
                                      ::style::window::default_body_width,
                                      ::style::window::default_body_height,
                                      "note",
                                      utils::translate("app_music_player_music_empty_window_notification"));

        emptyListIcon->setAlignment(Alignment::Horizontal::Center);
        songsList->emptyListCallback    = [this]() { emptyListIcon->setVisible(true); };
        songsList->notEmptyListCallback = [this]() {
            emptyListIcon->setVisible(false);
            setTitle(utils::translate("app_music_player_music_library_window_name"));
        };
        setFocusItem(songsList);
    }

    void MusicPlayerAllSongsWindow::destroyInterface()
    {
        erase();
    }

    void MusicPlayerAllSongsWindow::onBeforeShow([[maybe_unused]] ShowMode mode, [[maybe_unused]] SwitchData *data)
    {
        presenter->attach(this);
        auto index = presenter->getMusicPlayerItemProvider()->getCurrentIndex();

        songsList->rebuildList(listview::RebuildType::OnPageElement, index);
    }

    void MusicPlayerAllSongsWindow::updateSongsState()
    {
        songsList->rebuildList(gui::listview::RebuildType::InPlace);
    }

    void MusicPlayerAllSongsWindow::refreshWindow()
    {
        application->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
    }

    void MusicPlayerAllSongsWindow::setBottomBarTemporaryMode(const std::string &text)
    {
        bottomBarTemporaryMode(text, BottomBar::Side::CENTER, false);
    }

    void MusicPlayerAllSongsWindow::restoreFromBottomBarTemporaryMode()
    {
        bottomBarRestoreFromTemporaryMode();
    }

    bool MusicPlayerAllSongsWindow::onInput(const InputEvent &inputEvent)
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        if (inputEvent.isShortRelease(gui::KeyCode::KEY_ENTER)) {
            presenter->createData();
            return true;
        }

        return false;
    }
} /* namespace gui */
