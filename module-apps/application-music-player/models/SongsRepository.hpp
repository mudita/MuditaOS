// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/Application.hpp>
#include <Audio/decoder/Decoder.hpp>

namespace app::music_player
{
    class AbstractSongsRepository
    {
      public:
        virtual ~AbstractSongsRepository() noexcept = default;

        virtual std::vector<audio::Tags> getMusicFilesList()                        = 0;
        virtual std::optional<audio::Tags> getFileTags(const std::string &filePath) = 0;
    };

    class SongsRepository : public AbstractSongsRepository
    {
      public:
        explicit SongsRepository(Application *application);

        std::vector<audio::Tags> getMusicFilesList() override;
        std::optional<audio::Tags> getFileTags(const std::string &filePath) override;

      private:
        Application *application = nullptr;
    };
} // namespace app::music_player
