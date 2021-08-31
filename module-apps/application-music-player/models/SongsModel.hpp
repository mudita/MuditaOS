// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-apps/application-music-player/data/MusicPlayerStyle.hpp"

#include "SongContext.hpp"
#include "SongsRepository.hpp"
#include "SongsModelInterface.hpp"

#include <Audio/decoder/Decoder.hpp>

namespace app::music_player
{
    class SongsModel : public SongsModelInterface
    {
      public:
        explicit SongsModel(std::shared_ptr<AbstractSongsRepository> songsRepository);

        void createData(OnShortReleaseCallback shortReleaseCallback,
                        OnLongPressCallback longPressCallback,
                        OnSetBottomBarTemporaryCallback bottomBarTemporaryMode,
                        OnRestoreBottomBarTemporaryCallback bottomBarRestoreFromTemporaryMode) override;

        [[nodiscard]] auto requestRecordsCount() -> unsigned int override;

        [[nodiscard]] auto getMinimalItemSpaceRequired() const -> unsigned int override;

        auto getItem(gui::Order order) -> gui::ListItem * override;

        void requestRecords(uint32_t offset, uint32_t limit) override;

        size_t getCurrentIndex() const override;
        std::string getNextFilePath(const std::string &filePath) const override;
        std::string getPreviousFilePath(const std::string &filePath) const override;

        bool isSongPlaying() const noexcept override;
        void setCurrentSongState(SongState songState) noexcept override;
        std::optional<audio::Token> getCurrentFileToken() const noexcept override;

        SongContext getCurrentSongContext() const noexcept override;
        void setCurrentSongContext(SongContext context) override;
        void clearCurrentSongContext() override;

        void clearData() override;

      private:
        void clearCurrentItemState();
        void updateCurrentItemState();
        SongContext songContext;

        std::shared_ptr<AbstractSongsRepository> songsRepository;
    };
} // namespace app::music_player
