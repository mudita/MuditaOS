// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-apps/application-music-player/data/MusicPlayerStyle.hpp"

#include "SongsRepository.hpp"
#include "SongsModelInterface.hpp"

#include <Audio/decoder/Decoder.hpp>

namespace app::music_player
{
    class SongsModel : public SongsModelInterface
    {
      public:
        explicit SongsModel(std::shared_ptr<AbstractSongsRepository> songsRepository);

        void clearData();
        void createData(std::function<bool(const std::string &fileName)>) override;

        [[nodiscard]] auto requestRecordsCount() -> unsigned int override;

        [[nodiscard]] auto getMinimalItemSpaceRequired() const -> unsigned int override;

        auto getItem(gui::Order order) -> gui::ListItem * override;

        void requestRecords(const uint32_t offset, const uint32_t limit) override;

        bool isSongPlaying() const noexcept override;
        void setCurrentSongState(SongState songState) noexcept override;
        std::optional<audio::Token> getCurrentFileToken() const noexcept override;
        void setCurrentFileToken(std::optional<audio::Token> token) noexcept override;

      protected:
        SongState currentSongState = SongState::NotPlaying;

        std::optional<audio::Token> currentFileToken;
        std::shared_ptr<AbstractSongsRepository> songsRepository;
    };
} // namespace app::music_player
