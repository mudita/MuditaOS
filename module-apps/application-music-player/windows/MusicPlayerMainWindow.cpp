// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MusicPlayerMainWindow.hpp"
#include <application-music-player/data/MusicPlayerStyle.hpp>
#include <application-music-player/ApplicationMusicPlayer.hpp>
#include <apps-common/options/type/OptionSetting.hpp>

#include <service-audio/AudioServiceAPI.hpp>
#include <gui/widgets/ThreeBox.hpp>
#include <gui/widgets/ImageBox.hpp>
#include <gui/widgets/ListView.hpp>
#include <gui/widgets/Icon.hpp>

#include <i18n/i18n.hpp>
#include <Style.hpp>
#include <time/time_constants.hpp>

#include <cassert>

namespace gui
{
    MusicPlayerMainWindow::MusicPlayerMainWindow(
        app::ApplicationCommon *app, std::shared_ptr<app::music_player::SongsContract::Presenter> windowPresenter)
        : OptionWindow(app, name::window::track_info_window), presenter{windowPresenter}
    {
        presenter->attach(this);
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
        updateSongProgress(currentProgress);

        if (myViewMode == ViewMode::START) {
            buildInterfaceStartMode();
        }
        else if (myViewMode == ViewMode::TRACK) {
            buildInterfaceTrackMode();
        }
        else {
            buildInterfaceLibraryMode();
        }
        presenter->songsStateRequest();
    }

    void MusicPlayerMainWindow::buildInterfaceStartMode()
    {
        using namespace musicPlayerStyle::mainWindow;

        auto mainBox = new VBox(this, 0, 0, style::window_width, style::window_height);
        mainBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));

        ImageBox *note = new ImageBox(mainBox, 0, 0, 0, 0, new Image("mp_note", musicPlayerStyle::common::imageType));
        note->setMinimumSize(startScreen::noteSize, startScreen::noteSize);
        note->setMargins(Margins(0, startScreen::noteUpMargin, 0, startScreen::noteDownMargin));
        note->setEdges(RectangleEdge::None);

        Text *desc = new Text(mainBox, 0, 0, 0, 0);
        desc->setMinimumSize(startScreen::descriptionWidth, startScreen::descriptionHeight);
        desc->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        desc->setTextType(TextType::MultiLine);
        desc->setEditMode(EditMode::Browse);
        desc->setFont(style::window::font::medium);
        desc->setRichText(utils::translate("app_music_player_start_window_notification"));
        desc->setMargins(Margins(0, 0, 0, startScreen::descriptionBottomMargin));

        buildPlayButtonsInterface(mainBox);

        auto spacer = new HBox(mainBox, 0, 0, 0, 0);
        spacer->setMinimumSize(10, startScreen::buttonsBottomMargin);
        spacer->setEdges(RectangleEdge::None);

        buildSwipeDownInterface(mainBox);

        mainBox->resizeItems();

        bottomBar->setActive(BottomBar::Side::LEFT, false);
        bottomBar->setActive(BottomBar::Side::CENTER, false);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate("app_music_player_quit"));
    }

    void MusicPlayerMainWindow::buildInterfaceTrackMode()
    {
        using namespace musicPlayerStyle::mainWindow;

        auto mainBox = new VBox(this, 0, 0, style::window_width, style::window_height);
        mainBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));

        titleText = new Text(mainBox, 0, 0, style::window_width, trackInfoScreen::titleHeight);
        titleText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        titleText->setTextType(TextType::SingleLine);
        titleText->setEditMode(EditMode::Browse);
        titleText->setFont(style::window::font::mediumbigbold);
        titleText->setRichText(currentTitle);
        titleText->setMargins(Margins(0, trackInfoScreen::topMargin, 0, 0));

        artistText = new Text(mainBox, 0, 0, style::window_width, trackInfoScreen::artistHeight);
        artistText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        artistText->setTextType(TextType::SingleLine);
        artistText->setEditMode(EditMode::Browse);
        artistText->setFont(style::window::font::medium);
        artistText->setRichText(currentArtist);
        artistText->setMargins(Margins(0, trackInfoScreen::internalMargin, 0, 0));

        buildTrackProgressInterface(mainBox);
        buildPlayButtonsInterface(mainBox);

        auto spacer = new HBox(mainBox);
        spacer->setMinimumSize(10, trackInfoScreen::swipeBarMargin);
        spacer->setEdges(RectangleEdge::None);

        buildSwipeDownInterface(mainBox);

        mainBox->resizeItems();

        bottomBar->setActive(BottomBar::Side::LEFT, false);
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate("app_music_player_quit"));
    }

    void MusicPlayerMainWindow::buildInterfaceLibraryMode()
    {
        using namespace musicPlayerStyle::mainWindow;

        setTitle(utils::translate("app_music_player_music_library_window_name"));

        auto mainBox = new VBox(this, 0, 0, style::window_width, style::window_height);
        mainBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));

        buildTrackInfoInterface(mainBox);

        ImageBox *swipe =
            new ImageBox(mainBox, 0, 0, 0, 0, new Image("mp_line_arrow_up", musicPlayerStyle::common::imageType));
        swipe->setMinimumSizeToFitImage();
        swipe->setMargins(Margins(0, musicLibraryScreen::topArrowMargin, 0, musicLibraryScreen::bottomArrowMargin));
        swipe->setEdges(RectangleEdge::None);

        options.clear();
        auto addOption = [this](UTF8 name, const std::string &window = "", bool permissionToChangeViewMode = false) {
            options.emplace_back(std::make_unique<option::OptionSettings>(
                name,
                [=](Item &item) {
                    if (window.empty()) {
                        return true;
                    }
                    LOG_INFO("switching to window %s", window.c_str());
                    application->switchWindow(window, nullptr);
                    return true;
                },
                [=](Item &item) {
                    if (!item.focus) {
                        return true;
                    }
                    isPermissionToChangeViewMode = permissionToChangeViewMode;
                    if (window.empty()) {
                        clearBottomBarText(BottomBar::Side::CENTER);
                    }
                    else {
                        bottomBar->setText(BottomBar::Side::CENTER, utils::translate("common_select"));
                    }
                    return true;
                },
                this,
                window.empty() ? option::SettingRightItem::Disabled : option::SettingRightItem::ArrowWhite));
        };

        addOption(utils::translate("app_music_player_all_songs"), name::window::all_songs_window, true);
        addOption(utils::translate("app_music_player_artists"));
        addOption(utils::translate("app_music_player_albums"));
        addOption(utils::translate("app_music_player_playlists"));

        optionsList                         = new ListView(mainBox,
                                   0,
                                   0,
                                   option::window::optionsListW,
                                   musicLibraryScreen::optionListHeight,
                                   optionsModel,
                                   listview::ScrollBarType::None);
        optionsList->prepareRebuildCallback = [this]() { recreateOptions(); };
        optionsModel->createData(options);
        optionsList->rebuildList(listview::RebuildType::InPlace);
        setFocusItem(optionsList);

        mainBox->resizeItems();

        bottomBar->setActive(BottomBar::Side::LEFT, false);
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::translate("common_select"));
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate("app_music_player_quit"));
    }

    void MusicPlayerMainWindow::buildPlayButtonsInterface(VBox *parent)
    {
        using namespace musicPlayerStyle::mainWindow;

        using Box3    = HThreeBox<HBox, HBox, HBox>;
        Box3 *buttons = new Box3(parent);
        buttons->setMinimumSize(playButtons::width, playButtons::height);
        buttons->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        buttons->setEdges(RectangleEdge::None);

        buttons->firstBox = new HBox(buttons);
        buttons->firstBox->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        buttons->firstBox->setMinimumSize(playButtons::width / 3 - 1,
                                          playButtons::height - playButtons::internalMargin);
        buttons->firstBox->setEdges(RectangleEdge::None);

        rewImageBox =
            new ImageBox(buttons->firstBox, 0, 0, 0, 0, new Image("mp_prev_gray", musicPlayerStyle::common::imageType));
        rewImageBox->setMinimumSizeToFitImage();

        buttons->centerBox = new HBox(buttons);
        buttons->centerBox->setMinimumSize(playButtons::width / 3, playButtons::height - playButtons::internalMargin);
        buttons->centerBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        buttons->centerBox->setEdges(RectangleEdge::None);
        pauseImageBox = new ImageBox(
            buttons->centerBox, 0, 0, 0, 0, new Image("mp_pause_gray", musicPlayerStyle::common::imageType));
        pauseImageBox->setMinimumSizeToFitImage();

        buttons->lastBox = new HBox(buttons);
        buttons->lastBox->setMinimumSize(playButtons::width / 3 - 1, playButtons::height - playButtons::internalMargin);
        buttons->lastBox->setAlignment(Alignment(Alignment::Horizontal::Right, Alignment::Vertical::Center));
        buttons->lastBox->setEdges(RectangleEdge::None);
        ffImageBox =
            new ImageBox(buttons->lastBox, 0, 0, 0, 0, new Image("mp_next_gray", musicPlayerStyle::common::imageType));
        ffImageBox->setMinimumSizeToFitImage();

        buttons->resizeItems();
    }

    void MusicPlayerMainWindow::buildSwipeDownInterface(VBox *parent)
    {
        using namespace musicPlayerStyle::mainWindow;

        Text *musicLib = new Text(parent, 0, 0, 0, 0);
        musicLib->setMinimumSize(lineArrow::textWidth, lineArrow::textHeight);
        musicLib->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        musicLib->setTextType(TextType::SingleLine);
        musicLib->setEditMode(EditMode::Browse);
        musicLib->setFont(style::window::font::verysmall);
        musicLib->setRichText(utils::translate("app_music_player_music_library"));
        musicLib->setMargins(Margins(0, 0, 0, lineArrow::internalMargin));

        ImageBox *swipe =
            new ImageBox(parent, 0, 0, 0, 0, new Image("mp_line_arrow_down", musicPlayerStyle::common::imageType));
        swipe->setMinimumSizeToFitImage();
        swipe->setEdges(RectangleEdge::None);
    }

    void MusicPlayerMainWindow::buildTrackProgressInterface(VBox *parent)
    {
        using namespace musicPlayerStyle::mainWindow;

        auto mainBox = new VBox(parent, 0, 0, style::window_width, trackProgress::height);
        mainBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        mainBox->setMinimumSize(style::window_width, trackProgress::height);
        mainBox->setMargins(Margins(0, trackProgress::topMargin, 0, trackProgress::bottomMargin));
        mainBox->setEdges(RectangleEdge::None);

        auto topBox = new HBox(mainBox);
        topBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center)); //
        topBox->setMinimumSize(trackProgress::barWidth, trackProgress::barHeight);
        topBox->setEdges(RectangleEdge::None);

        unsigned spacerWidth =
            (trackProgress::barWidth - progressBarSize * trackProgress::barThickness) / (progressBarSize - 1);
        for (auto i = 0; i < progressBarSize; ++i) {
            if (i < currentProgressBarsBlack) {
                progressBarItems[i] = new Image(topBox, "mp_bar", musicPlayerStyle::common::imageType);
            }
            else {
                progressBarItems[i] = new Image(topBox, "mp_bar_gray", musicPlayerStyle::common::imageType);
            }
            progressBarItems[i]->setMinimumSize(trackProgress::barThickness, trackProgress::barHeight);
            HBox *spacer = new HBox(topBox, 0, 0, spacerWidth, trackProgress::barHeight);
            spacer->setEdges(RectangleEdge::None);
        }

        auto bottomBox = new HBox(mainBox);
        bottomBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Bottom));
        bottomBox->setMinimumSize(trackProgress::barWidth, trackProgress::bottomHeight);
        bottomBox->setEdges(RectangleEdge::None);

        auto letfBox = new HBox(bottomBox);
        letfBox->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Bottom)); //
        letfBox->setMinimumSize(trackProgress::barWidth / 2 - 1, trackProgress::bottomHeight);
        letfBox->setEdges(RectangleEdge::None);
        currentTimeText = new Text(letfBox, 0, 0, trackProgress::descriptionWidth, trackProgress::descriptionHeight);
        currentTimeText->setMaximumWidth(trackProgress::barWidth / 2 - 1);
        currentTimeText->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        currentTimeText->setTextType(TextType::SingleLine);
        currentTimeText->setEditMode(EditMode::Browse);
        currentTimeText->setFont(style::window::font::verysmall);
        currentTimeText->setRichText(currentTimeString);

        auto rightBox = new HBox(bottomBox);
        rightBox->setAlignment(Alignment(Alignment::Horizontal::Right, Alignment::Vertical::Bottom)); //
        rightBox->setMinimumSize(trackProgress::barWidth / 2 - 1, trackProgress::bottomHeight);
        rightBox->setEdges(RectangleEdge::None);
        totalTimeText = new Text(rightBox, 0, 0, trackProgress::descriptionWidth, trackProgress::descriptionHeight);
        totalTimeText->setMaximumWidth(trackProgress::barWidth / 2 - 1);
        totalTimeText->setAlignment(Alignment(Alignment::Horizontal::Right, Alignment::Vertical::Center));
        totalTimeText->setTextType(TextType::SingleLine);
        totalTimeText->setEditMode(EditMode::Browse);
        totalTimeText->setFont(style::window::font::verysmall);
        totalTimeText->setRichText(currentTotalTimeString);
    }

    void MusicPlayerMainWindow::buildTrackInfoInterface(VBox *parent)
    {
        using namespace musicPlayerStyle::mainWindow;

        HBox *songData = new HBox(parent);
        songData->setMinimumSize(trackInfo::width, trackInfo::height);
        songData->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        songData->setMargins(Margins(0, trackInfo::topMargin, 0, 0));
        songData->setEdges(RectangleEdge::None);

        stateImageBox = new ImageBox(
            songData, 0, 0, 0, 0, new Image("mp_now_playing_icon_pause_gray", musicPlayerStyle::common::imageType));
        stateImageBox->setMinimumSizeToFitImage();
        stateImageBox->setEdges(RectangleEdge::None);
        stateImageBox->setMargins(Margins(0, 0, trackInfo::internalMargin, 0));

        HBox *textBox = new HBox(songData);
        textBox->setMinimumSize(trackInfo::width - trackInfo::internalMargin - trackInfo::height, trackInfo::height);
        textBox->setEdges(RectangleEdge::None);
        textBox->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        descriptionText = new Text(textBox, 0, 0, 0, 0);
        descriptionText->setMinimumSize(trackInfo::width - trackInfo::height - trackInfo::internalMargin,
                                        trackInfo::height);
        descriptionText->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        descriptionText->setTextType(TextType::SingleLine);
        descriptionText->setEditMode(EditMode::Browse);
        descriptionText->setFont(style::window::font::verysmall);
    }

    void MusicPlayerMainWindow::destroyInterface()
    {
        setFocusItem(nullptr);

        erase();

        titleText       = nullptr;
        artistText      = nullptr;
        currentTimeText = nullptr;
        totalTimeText   = nullptr;

        rewImageBox   = nullptr;
        pauseImageBox = nullptr;
        ffImageBox    = nullptr;

        stateImageBox   = nullptr;
        descriptionText = nullptr;
        memset(progressBarItems, 0, progressBarSize * sizeof(Image *));

        optionsList = nullptr;
    }

    void MusicPlayerMainWindow::onBeforeShow([[maybe_unused]] ShowMode mode, [[maybe_unused]] SwitchData *data)
    {
        presenter->attach(this);
        rebuild();
    }

    void MusicPlayerMainWindow::updateSongsState(std::optional<db::multimedia_files::MultimediaFilesRecord> record,
                                                 RecordState state)
    {
        if (record) {
            currentTitle = record->tags.title;
            if (currentTitle.empty())
                currentTitle = utils::translate("app_music_player_uknown_title");

            currentArtist = record->tags.album.artist;
            if (currentArtist.empty())
                currentArtist = utils::translate("app_music_player_uknown_artist");

            currentTotalTime = record->audioProperties.songLength;
            if (myViewMode == ViewMode::START) {
                changeCurrentMode(ViewMode::TRACK);
            }
        }
        else {
            currentTitle     = "";
            currentArtist    = "";
            currentTotalTime = 0;

            if (myViewMode == ViewMode::TRACK) {
                changeCurrentMode(ViewMode::START);
            }
        }

        updateVisibleTrackData(state);
    }

    void MusicPlayerMainWindow::updateSongProgress(float progress)
    {
        progress        = std::clamp(progress, 0.f, 1.f);
        currentProgress = progress;

        updateVisibleProgressData();
    }

    void MusicPlayerMainWindow::refreshWindow()
    {
        RefreshModes mode = RefreshModes::GUI_REFRESH_FAST;
        if (needToDeepRedrawScreen) {
            needToDeepRedrawScreen = false;
            mode                   = RefreshModes::GUI_REFRESH_DEEP;
        }
        application->refreshWindow(mode);
    }

    void MusicPlayerMainWindow::setBottomBarTemporaryMode(const std::string &text)
    {
        bottomBarTemporaryMode(text, BottomBar::Side::CENTER, false);
    }

    void MusicPlayerMainWindow::restoreFromBottomBarTemporaryMode()
    {
        bottomBarRestoreFromTemporaryMode();
    }

    void MusicPlayerMainWindow::changeCurrentMode(ViewMode m)
    {
        RefreshModes refreshMode = RefreshModes::GUI_REFRESH_FAST;

        if (myViewMode != m) {
            myViewMode  = m;
            refreshMode = RefreshModes::GUI_REFRESH_DEEP;
        }

        rebuild();
        application->refreshWindow(refreshMode);
    }

    void MusicPlayerMainWindow::updateVisibleTrackData(RecordState state) noexcept
    {
        auto isPlaying = state == RecordState::Playing;
        auto isPaused  = state == RecordState::Paused;

        if (titleText != nullptr)
            titleText->setRichText(currentTitle);
        if (artistText != nullptr)
            artistText->setRichText(currentArtist);

        if (totalTimeText != nullptr)
            totalTimeText->setRichText(currentTotalTimeString);
        if (rewImageBox != nullptr)
            rewImageBox->setImage((isPlaying || isPaused) ? "mp_prev" : "mp_prev_gray",
                                  musicPlayerStyle::common::imageType);
        if (pauseImageBox != nullptr)
            pauseImageBox->setImage(isPlaying ? "mp_pause" : ((isPaused) ? "mp_play" : "mp_pause_gray"),
                                    musicPlayerStyle::common::imageType);
        if (ffImageBox != nullptr)
            ffImageBox->setImage((isPlaying || isPaused) ? "mp_next" : "mp_next_gray",
                                 musicPlayerStyle::common::imageType);

        if (stateImageBox != nullptr) {
            stateImageBox->setImage(isPlaying
                                        ? "mp_now_playing_icon"
                                        : (isPaused ? "mp_now_playing_icon_pause" : "mp_now_playing_icon_pause_gray"),
                                    musicPlayerStyle::common::imageType);
        }
        if (descriptionText != nullptr) {
            std::string trackDescription;
            if (!isPaused && !isPlaying) {
                trackDescription = utils::translate("app_music_player_empty_track_notification");
            }
            else {
                trackDescription = currentTitle + " - " + currentArtist;
            }
            descriptionText->setRichText(trackDescription);
        }

        if (myViewMode == ViewMode::TRACK) {
            bottomBar->setText(BottomBar::Side::CENTER,
                               isPlaying ? utils::translate("common_pause") : utils::translate("common_resume"));
        }
    }

    void MusicPlayerMainWindow::updateVisibleProgressData(void) noexcept
    {
        if (myViewMode != ViewMode::TRACK) {
            return;
        }
        constexpr auto maxTimeToDisplaySize{10};
        char timeToDisplay[maxTimeToDisplaySize];

        uint8_t passedBarsNumber = std::round(currentProgress * progressBarSize);
        if (passedBarsNumber != currentProgressBarsBlack) {

            if (passedBarsNumber < currentProgressBarsBlack) {
                needToDeepRedrawScreen = true;
            }
            currentProgressBarsBlack = passedBarsNumber;
            for (auto i = 0; i < progressBarSize; ++i) {
                if (progressBarItems[i] == nullptr)
                    continue;

                if (i < currentProgressBarsBlack) {
                    progressBarItems[i]->set("mp_bar", musicPlayerStyle::common::imageType);
                }
                else {
                    progressBarItems[i]->set("mp_bar_gray", musicPlayerStyle::common::imageType);
                }
            }
        }

        snprintf(timeToDisplay,
                 maxTimeToDisplaySize,
                 "%d:%02d",
                 static_cast<int>(currentTotalTime) / utils::time::secondsInMinute,
                 static_cast<int>(currentTotalTime) % utils::time::secondsInMinute);
        currentTotalTimeString = timeToDisplay;

        auto elapsedTime = currentTotalTime * currentProgress;
        snprintf(timeToDisplay,
                 maxTimeToDisplaySize,
                 "%d:%02d",
                 static_cast<int>(elapsedTime) / utils::time::secondsInMinute,
                 static_cast<int>(elapsedTime) % utils::time::secondsInMinute);
        currentTimeString = timeToDisplay;

        if (totalTimeText != nullptr)
            totalTimeText->setRichText(currentTotalTimeString);
        if (currentTimeText != nullptr)
            currentTimeText->setRichText(currentTimeString);
    }

    bool MusicPlayerMainWindow::onInput(const InputEvent &inputEvent)
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        if (inputEvent.isShortRelease(KeyCode::KEY_DOWN) &&
            (myViewMode == ViewMode::START || myViewMode == ViewMode::TRACK)) {
            changeCurrentMode(ViewMode::LIBRARY);
            return true;
        }

        if (inputEvent.is(KeyCode::KEY_UP) && myViewMode == ViewMode::LIBRARY && isPermissionToChangeViewMode) {
            if (presenter->getMusicPlayerModelInterface()->getCurrentSongContext().filePath.empty()) {
                changeCurrentMode(ViewMode::START);
            }
            else {
                changeCurrentMode(ViewMode::TRACK);
            }
            return true;
        }

        if (myViewMode == ViewMode::TRACK) {
            return onInputTrackMode(inputEvent);
        }

        return false;
    }

    bool MusicPlayerMainWindow::onInputTrackMode(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_LEFT)) {
            presenter->playPrevious();
            return true;
        }
        else if (inputEvent.isShortRelease(KeyCode::KEY_RIGHT)) {
            presenter->playNext();
            return true;
        }
        else if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            presenter->handlePlayOrPauseRequest();
        }

        return true;
    }
} /* namespace gui */
