// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-music-player/presenters/SongsPresenter.hpp>
#include <AppWindow.hpp>
#include <TextFixedSize.hpp>
#include <OptionWindow.hpp>
#include <module-apps/apps-common/InternalModel.hpp>

namespace gui
{
    class ListView;
    class ImageBox;
    class OptionSettings;

    /// Start MP app window with possible three internal states:
    /// - init screen
    /// - music library screen
    /// - track preview screen
    class MusicPlayerMainWindow : public OptionWindow, public app::music_player::SongsContract::View
    {
      public:
        /// number of vertical items in track's progress bar
        static constexpr uint8_t progressBarSize = 27;

        /// current view mode (switching with up / down arrow)
        enum class ViewMode
        {
            START = 1, // start screen "press down arrow to choose..."
            TRACK,     // track info
            LIBRARY    // Music library
        };

        explicit MusicPlayerMainWindow(app::ApplicationCommon *app,
                                       std::shared_ptr<app::music_player::SongsContract::Presenter> presenter);

        void onBeforeShow([[maybe_unused]] ShowMode mode, [[maybe_unused]] SwitchData *data) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        bool onInput(const InputEvent &inputEvent) override;

        void updateSongsState(std::optional<db::multimedia_files::MultimediaFilesRecord> record,
                              RecordState state) override;
        void updateSongProgress(float progress) override;
        void refreshWindow() override;
        void setBottomBarTemporaryMode(const std::string &text) override;
        void restoreFromBottomBarTemporaryMode() override;

        void changeCurrentMode(ViewMode m);

      protected:
        void buildInterfaceStartMode();
        void buildInterfaceTrackMode();
        void buildInterfaceLibraryMode();

        void buildPlayButtonsInterface(VBox *parent);
        void buildSwipeDownInterface(VBox *parent);
        void buildTrackProgressInterface(VBox *parent);
        void buildTrackInfoInterface(VBox *parent);
        void updateVisibleTrackData(RecordState state) noexcept;
        void updateVisibleProgressData(void) noexcept;

        bool onInputTrackMode(const InputEvent &inputEvent);

      private:
        ViewMode myViewMode = ViewMode::START;

        std::shared_ptr<app::music_player::SongsContract::Presenter> presenter;
        Text *titleText                          = nullptr;
        Text *artistText                         = nullptr;
        Text *currentTimeText                    = nullptr;
        Text *totalTimeText                      = nullptr;
        ImageBox *rewImageBox                    = nullptr;
        ImageBox *pauseImageBox                  = nullptr;
        ImageBox *ffImageBox                     = nullptr;
        ImageBox *stateImageBox                  = nullptr;
        Text *descriptionText                    = nullptr;
        Image *progressBarItems[progressBarSize] = {nullptr};

        float currentProgress            = 0.f;
        uint32_t currentTotalTime        = 0;
        uint8_t currentProgressBarsBlack = 0;
        std::string currentTitle;
        std::string currentArtist;
        std::string currentTimeString;
        std::string currentTotalTimeString;
        bool isPermissionToChangeViewMode = false;
        bool needToDeepRedrawScreen       = false;
    };

} /* namespace gui */
