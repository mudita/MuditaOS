// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>
#include <Audio/decoder/Decoder.hpp>
#include <atomic>

namespace gui
{
    class MusicPlayerAllSongsWindow;
    namespace name
    {
        namespace window
        {
            inline constexpr auto all_songs_window = "All Songs";
            inline constexpr auto player_window    = "Player";
            inline constexpr auto empty_window     = "Empty";
        }; // namespace window
    };     // namespace name
};         // namespace gui

namespace app
{
    inline constexpr auto name_music_player = "ApplicationMusicPlayer";

    class ApplicationMusicPlayer : public Application
    {
        std::optional<audio::Token> currentFileToken;

        std::atomic_bool isTrackPlaying = false;

      public:
        explicit ApplicationMusicPlayer(std::string name                    = name_music_player,
                                        std::string parent                  = {},
                                        sys::phone_modes::PhoneMode mode    = sys::phone_modes::PhoneMode::Connected,
                                        StartInBackground startInBackground = {false});

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl,
                                                [[maybe_unused]] sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override
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
        void startPlaying();
        void togglePlaying();

        std::optional<audio::Tags> getFileTags(const std::string &filePath);

        void handlePlayResponse(sys::Message *msg);
        void handleStopResponse(sys::Message *msg);
    };

    template <> struct ManifestTraits<ApplicationMusicPlayer>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch, manager::actions::PhoneModeChanged},
                    locks::AutoLockPolicy::DetermineByAppState};
        }
    };
} /* namespace app */
