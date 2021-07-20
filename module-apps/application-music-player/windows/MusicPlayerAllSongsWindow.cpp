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

        setTitle(utils::translate("app_music_player_all_songs"));

        bottomBar->setText(BottomBar::Side::CENTER, utils::translate("app_music_player_play"));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));

        songsList = new gui::ListView(this,
                                      musicPlayerStyle::allSongsWindow::x,
                                      musicPlayerStyle::allSongsWindow::y,
                                      musicPlayerStyle::allSongsWindow::w,
                                      musicPlayerStyle::allSongsWindow::h,
                                      presenter->getMusicPlayerItemProvider(),
                                      listview::ScrollBarType::Fixed);

        setFocusItem(songsList);
    }

    void MusicPlayerAllSongsWindow::destroyInterface()
    {
        erase();
    }

    void MusicPlayerAllSongsWindow::onBeforeShow([[maybe_unused]] ShowMode mode, [[maybe_unused]] SwitchData *data)
    {
        presenter->createData([this](const std::string &fileName) { return presenter->play(fileName); });
    }

} /* namespace gui */
