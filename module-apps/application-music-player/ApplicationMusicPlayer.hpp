// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>

namespace gui
{
    class MusicPlayerAllSongsWindow;
    namespace name
    {
        namespace window
        {
            inline const std::string all_songs_window = "All Songs";
            inline const std::string player_window    = "Player";
            inline const std::string empty_window     = "Empty";
        }; // namespace window
    };     // namespace name
};         // namespace gui

namespace app
{
    const inline std::string name_music_player = "ApplicationMusicPlayer";

    class ApplicationMusicPlayer : public Application
    {

      public:
        ApplicationMusicPlayer(std::string name                    = name_music_player,
                               std::string parent                  = {},
                               StartInBackground startInBackground = {false});
        virtual ~ApplicationMusicPlayer();

        sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }

        void createUserInterface() final;
        void destroyUserInterface() final;

        std::vector<audio::Tags> getMusicFilesList();
        bool play(const std::string &fileName);
        bool pause();
        bool resume();
        bool stop();
        std::optional<audio::Tags> getFileTags(const std::string &filePath);
    };

    template <> struct ManifestTraits<ApplicationMusicPlayer>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch}};
        }
    };
} /* namespace app */
