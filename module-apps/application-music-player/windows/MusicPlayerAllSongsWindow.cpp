﻿// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MusicPlayerAllSongsWindow.hpp"
#include <application-music-player/ApplicationMusicPlayer.hpp>
#include <data/MusicPlayerStyle.hpp>

#include <Style.hpp>
#include <i18n/i18n.hpp>
#include <gui/widgets/ListView.hpp>
#include <gui/widgets/Icon.hpp>

namespace gui
{
    MusicPlayerAllSongsWindow::MusicPlayerAllSongsWindow(
        app::ApplicationCommon *app, std::shared_ptr<app::music_player::SongsContract::Presenter> windowPresenter)
        : AppWindow(app, gui::name::window::all_songs_window), presenter{std::move(windowPresenter)}
    {
        buildInterface();
    }

    void MusicPlayerAllSongsWindow::buildInterface()
    {
        presenter->attach(this);
        AppWindow::buildInterface();

        setTitle(utils::translate("app_music_player_music_library_window_name"));
        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));

        songsList = new gui::ListView(this,
                                      musicPlayerStyle::allSongsWindow::x,
                                      musicPlayerStyle::allSongsWindow::y,
                                      musicPlayerStyle::allSongsWindow::w,
                                      musicPlayerStyle::allSongsWindow::h,
                                      presenter->getMusicPlayerModelInterface(),
                                      listview::ScrollBarType::Fixed);
        songsList->setBoundaries(Boundaries::Continuous);

        emptyListIcon = new gui::Icon(this,
                                      style::window::default_left_margin,
                                      musicPlayerStyle::mainWindow::musicLibraryScreen::emptyLibraryNoteMargin,
                                      style::window::default_body_width,
                                      style::window::default_body_height,
                                      "mp_note",
                                      utils::translate("app_music_player_music_empty_window_notification"),
                                      musicPlayerStyle::common::imageType);
        emptyListIcon->setVisible(false);

        emptyListIcon->setAlignment(Alignment::Horizontal::Center);
        songsList->emptyListCallback    = [this]() { emptyListIcon->setVisible(true); };
        songsList->notEmptyListCallback = [this]() {
            emptyListIcon->setVisible(false);
            setTitle(utils::translate("app_music_player_music_library_window_name"));
        };
        setFocusItem(songsList);
    }

    void MusicPlayerAllSongsWindow::onBeforeShow([[maybe_unused]] ShowMode mode, [[maybe_unused]] SwitchData *data)
    {
        presenter->attach(this);
        presenter->createData();
    }

    void MusicPlayerAllSongsWindow::updateSongsState(
        [[maybe_unused]] std::optional<db::multimedia_files::MultimediaFilesRecord> record,
        [[maybe_unused]] RecordState state)
    {
        songsList->rebuildList(gui::listview::RebuildType::InPlace);
    }

    void MusicPlayerAllSongsWindow::updateSongProgress([[maybe_unused]] float progress)
    {}

    void MusicPlayerAllSongsWindow::refreshWindow()
    {
        application->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
    }

    void MusicPlayerAllSongsWindow::setNavBarTemporaryMode(const std::string &text)
    {
        navBarTemporaryMode(text, nav_bar::Side::Center, false);
    }

    void MusicPlayerAllSongsWindow::restoreFromNavBarTemporaryMode()
    {
        navBarRestoreFromTemporaryMode();
    }

    bool MusicPlayerAllSongsWindow::onInput(const InputEvent &inputEvent)
    {
        return AppWindow::onInput(inputEvent);
    }
} /* namespace gui */
