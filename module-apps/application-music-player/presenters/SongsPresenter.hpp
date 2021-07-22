// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AudioOperations.hpp"

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
            virtual ~View() noexcept = default;
        };
        class Presenter : public BasePresenter<SongsContract::View>
        {
          public:
            virtual ~Presenter() noexcept = default;

            virtual std::shared_ptr<SongsListItemProvider> getMusicPlayerItemProvider() const = 0;
            virtual void createData(std::function<bool(const std::string &fileName)>)         = 0;

            virtual bool play(const std::string &filePath) = 0;
            virtual bool pause()                           = 0;
            virtual bool resume()                          = 0;
            virtual bool stop()                            = 0;
            virtual void togglePlaying()                   = 0;

            virtual void setPlayingStateCallback(std::function<void(SongsModelInterface::SongState)> cb) = 0;
        };
    };

    class SongsPresenter : public SongsContract::Presenter
    {
      public:
        explicit SongsPresenter(std::shared_ptr<SongsModelInterface> songsListItemProvider,
                                std::unique_ptr<AbstractAudioOperations> &&audioOperations);

        std::shared_ptr<SongsListItemProvider> getMusicPlayerItemProvider() const override;

        void createData(std::function<bool(const std::string &fileName)>) override;

        bool play(const std::string &filePath) override;
        bool pause() override;
        bool resume() override;
        bool stop() override;
        void togglePlaying() override;
        void setPlayingStateCallback(std::function<void(SongsModelInterface::SongState)> cb) override;

      private:
        std::shared_ptr<SongsModelInterface> songsModelInterface;
        std::unique_ptr<AbstractAudioOperations> audioOperations;
        std::function<void(SongsModelInterface::SongState)> changePlayingStateCallback = nullptr;
    };
} // namespace app::music_player
