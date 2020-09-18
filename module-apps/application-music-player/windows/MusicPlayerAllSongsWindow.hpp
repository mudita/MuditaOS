#pragma once

#include "application-music-player/models/SongsModel.hpp"
#include "AppWindow.hpp"
#include "ListView.hpp"
#include <TextFixedSize.hpp>

#include <vector>
#include <string>

namespace gui
{

    class MusicPlayerAllSongsWindow : public AppWindow
    {
        std::shared_ptr<SongsModel> songsModel = nullptr;

        ListView *songsList = nullptr;
        Label *soundLabel   = nullptr;

        auto setCurrentVolume(const std::function<void(const audio::Volume &)> &successCallback,
                              const std::function<void(const audio::Volume &)> &errCallback = nullptr) -> bool;

      public:
        MusicPlayerAllSongsWindow(app::Application *app);

        // virtual methods
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        bool onDatabaseMessage(sys::Message *msg) override;
        bool onInput(const InputEvent &inputEvent) final;
    };

} /* namespace gui */
