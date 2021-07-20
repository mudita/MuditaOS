// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-music-player/presenters/SongsPresenter.hpp>
#include <AppWindow.hpp>
#include <TextFixedSize.hpp>

namespace gui
{
    class ListView;
    class MusicPlayerAllSongsWindow : public AppWindow, public app::music_player::SongsContract::View
    {
        std::shared_ptr<app::music_player::SongsContract::Presenter> presenter;
        ListView *songsList = nullptr;

      public:
        explicit MusicPlayerAllSongsWindow(app::Application *app,
                                           std::shared_ptr<app::music_player::SongsContract::Presenter> presenter);

        // virtual methods
        void onBeforeShow([[maybe_unused]] ShowMode mode, [[maybe_unused]] SwitchData *data) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

} /* namespace gui */
