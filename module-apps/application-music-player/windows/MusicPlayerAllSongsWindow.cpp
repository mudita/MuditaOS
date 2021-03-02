// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MusicPlayerAllSongsWindow.hpp"
#include "application-music-player/ApplicationMusicPlayer.hpp"

#include <Style.hpp>
#include <cassert>
#include <i18n/i18n.hpp>
#include <service-audio/AudioServiceAPI.hpp>

namespace gui
{

    MusicPlayerAllSongsWindow::MusicPlayerAllSongsWindow(app::Application *app)
        : AppWindow(app, gui::name::window::all_songs_window), songsModel{
                                                                   std::make_shared<SongsModel>(this->application)}
    {
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

        setTitle(utils::localize.get("app_music_player_all_songs"));

        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_music_player_play"));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        songsList = new gui::ListView(this,
                                      musicPlayerStyle::allSongsWindow::x,
                                      musicPlayerStyle::allSongsWindow::y,
                                      musicPlayerStyle::allSongsWindow::w,
                                      musicPlayerStyle::allSongsWindow::h,
                                      songsModel,
                                      style::listview::ScrollBarType::Fixed);

        setFocusItem(songsList);
    }

    void MusicPlayerAllSongsWindow::destroyInterface()
    {
        erase();
    }

    void MusicPlayerAllSongsWindow::onBeforeShow([[maybe_unused]] ShowMode mode, [[maybe_unused]] SwitchData *data)
    {
        auto app = dynamic_cast<app::ApplicationMusicPlayer *>(application);
        assert(app);

        songsModel->createData(app->getMusicFilesList(),
                               [app](const std::string &fileName) { return app->play(fileName); });
    }

} /* namespace gui */
