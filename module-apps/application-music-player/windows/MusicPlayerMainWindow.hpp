#pragma once

#include "application-music-player/models/SongsModel.hpp"
#include "AppWindow.hpp"
#include "ListView.hpp"

#include <vector>
#include <string>

namespace gui
{

    class MusicPlayerMainWindow : public AppWindow
    {
        std::shared_ptr<SongsModel> songsModel = nullptr;

        ListView *songsList = nullptr;

      public:
        MusicPlayerMainWindow(app::Application *app);

        // virtual methods
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        bool onDatabaseMessage(sys::Message *msg) override;
    };

} /* namespace gui */
