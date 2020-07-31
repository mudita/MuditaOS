#include "MusicPlayerMainWindow.hpp"
#include "application-music-player/ApplicationMusicPlayer.hpp"

#include <Style.hpp>
#include <i18/i18.hpp>
#include <log/log.hpp>
#include <service-audio/api/AudioServiceAPI.hpp>

namespace gui
{

    MusicPlayerMainWindow::MusicPlayerMainWindow(app::Application *app)
        : AppWindow(app, gui::name::window::main_window), songsModel{std::make_shared<SongsModel>(this->application)}
    {
        buildInterface();
    }

    void MusicPlayerMainWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void MusicPlayerMainWindow::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::localize.get("app_music_player_all_songs"));

        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_music_player_play"));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        topBar->setActive(TopBar::Elements::TIME, true);

        songsList = new gui::ListView(this,
                                      musicPlayerStyle::mainWindow::x,
                                      musicPlayerStyle::mainWindow::y,
                                      musicPlayerStyle::mainWindow::w,
                                      musicPlayerStyle::mainWindow::h,
                                      songsModel);

        setFocusItem(songsList);
    }

    void MusicPlayerMainWindow::destroyInterface()
    {
        erase();
    }

    void MusicPlayerMainWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        auto app = dynamic_cast<app::ApplicationMusicPlayer *>(application);
        assert(app);

        songsModel->createData(app->getMusicFilesList());
    }

    bool MusicPlayerMainWindow::onDatabaseMessage(sys::Message *msgl)
    {
        return false;
    }

} /* namespace gui */
