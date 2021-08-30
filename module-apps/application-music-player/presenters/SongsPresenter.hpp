// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/AudioOperations.hpp>
#include <apps-common/BasePresenter.hpp>

#include <module-apps/application-music-player/models/SongsModelInterface.hpp>

namespace app::music_player
{
    class SongsContract
    {
      public:
        class View
        {
          public:
            virtual ~View() noexcept                                        = default;
            virtual void updateSongsState()                                 = 0;
            virtual void refreshWindow()                                    = 0;
            virtual void setBottomBarTemporaryMode(const std::string &text) = 0;
            virtual void restoreFromBottomBarTemporaryMode()                = 0;
        };
        class Presenter : public BasePresenter<SongsContract::View>
        {
          public:
            using OnPlayingStateChangeCallback = std::function<void(SongState)>;

            virtual ~Presenter() noexcept = default;

            virtual std::shared_ptr<SongsListItemProvider> getMusicPlayerItemProvider() const = 0;
            virtual void createData()                                                         = 0;

            virtual bool play(const std::string &filePath) = 0;
            virtual bool pause()                           = 0;
            virtual bool resume()                          = 0;
            virtual bool stop()                            = 0;

            virtual void setPlayingStateCallback(OnPlayingStateChangeCallback cb) = 0;
            virtual bool handleAudioStopNotifiaction(audio::Token token)          = 0;
            virtual bool handleAudioEofNotification(audio::Token token)           = 0;
            virtual bool handleAudioPausedNotification(audio::Token token)        = 0;
            virtual bool handleAudioResumedNotification(audio::Token token)       = 0;
        };
    };

    class SongsPresenter : public SongsContract::Presenter
    {
      public:
        explicit SongsPresenter(std::shared_ptr<SongsModelInterface> songsListItemProvider,
                                std::unique_ptr<AbstractAudioOperations> &&audioOperations);

        std::shared_ptr<SongsListItemProvider> getMusicPlayerItemProvider() const override;

        void createData() override;

        bool play(const std::string &filePath) override;
        bool pause() override;
        bool resume() override;
        bool stop() override;

        void setPlayingStateCallback(std::function<void(SongState)> cb) override;
        bool handleAudioStopNotifiaction(audio::Token token) override;
        bool handleAudioEofNotification(audio::Token token) override;
        bool handleAudioPausedNotification(audio::Token token) override;
        bool handleAudioResumedNotification(audio::Token token) override;

      private:
        void updateViewSongState();
        void refreshView();

        /// Request state dependant audio operation
        bool requestAudioOperation(const std::string &filePath = "");
        void setViewBottomBarTemporaryMode(const std::string &text);
        void restoreViewBottomBarFromTemporaryMode();
        std::shared_ptr<SongsModelInterface> songsModelInterface;
        std::unique_ptr<AbstractAudioOperations> audioOperations;
        std::function<void(SongState)> changePlayingStateCallback = nullptr;
    };
} // namespace app::music_player
