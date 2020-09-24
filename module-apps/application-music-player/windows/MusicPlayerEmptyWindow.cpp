#include "MusicPlayerEmptyWindow.hpp"
#include "application-music-player/ApplicationMusicPlayer.hpp"
#include "application-music-player/data/MusicPlayerStyle.hpp"

#include <Style.hpp>
#include <i18/i18.hpp>
#include <log/log.hpp>
#include <service-audio/api/AudioServiceAPI.hpp>

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

        topBar->setActive(TopBar::Elements::TIME, true);

        img = new gui::Image(this, noteImg::x, noteImg::y, "note");

        text = new Text(this, infoText::x, infoText::y, infoText::w, infoText::h);
        text->setText(utils::localize.get("app_music_player_music_empty_window_notification"));
        text->setTextType(TextType::MULTI_LINE);
        text->setEditMode(EditMode::BROWSE);
        text->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
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
