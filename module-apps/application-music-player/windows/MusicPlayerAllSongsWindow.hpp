// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-music-player/models/SongsModel.hpp"
#include "AppWindow.hpp"
#include <TextFixedSize.hpp>

namespace gui
{
    class ListView;
    class MusicPlayerAllSongsWindow : public AppWindow
    {
        std::shared_ptr<SongsModel> songsModel = nullptr;

        ListView *songsList = nullptr;

      public:
        explicit MusicPlayerAllSongsWindow(app::Application *app);

        // virtual methods
        void onBeforeShow([[maybe_unused]] ShowMode mode, [[maybe_unused]] SwitchData *data) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

} /* namespace gui */
