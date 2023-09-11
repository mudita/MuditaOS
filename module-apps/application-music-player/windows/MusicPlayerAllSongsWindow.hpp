// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-music-player/presenters/SongsPresenter.hpp>
#include <AppWindow.hpp>
#include <TextFixedSize.hpp>

namespace gui
{
    class ListView;
    class Icon;
    class MusicPlayerAllSongsWindow : public AppWindow, public app::music_player::SongsContract::View
    {
      public:
        explicit MusicPlayerAllSongsWindow(app::ApplicationCommon *app,
                                           std::shared_ptr<app::music_player::SongsContract::Presenter> presenter);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;

        void updateSongsState(std::optional<db::multimedia_files::MultimediaFilesRecord> record,
                              RecordState state) override;
        void updateSongProgress(float progress) override;
        void refreshWindow() override;
        void setNavBarTemporaryMode(const std::string &text) override;
        void restoreFromNavBarTemporaryMode() override;

      private:
        std::shared_ptr<app::music_player::SongsContract::Presenter> presenter;
        ListView *songsList = nullptr;
        Icon *emptyListIcon = nullptr;
    };
} /* namespace gui */
