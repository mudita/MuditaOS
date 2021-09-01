// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <queries/RecordQuery.hpp>
#include <queries/Filter.hpp>
#include <Interface/MultimediaFilesRecord.hpp>
#include <Common/Common.hpp>

#include <string>

namespace db::multimedia_files::query
{
    class GetCount : public Query
    {
      public:
        explicit GetCount();
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };
    class GetCountForArtist : public Query
    {
      public:
        explicit GetCountForArtist(const Artist &artist);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        const Artist artist;
    };
    class GetCountForAlbum : public Query
    {
      public:
        explicit GetCountForAlbum(const Album &album);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        const Album album;
    };

    class GetCountResult : public QueryResult
    {
        unsigned count;

      public:
        explicit GetCountResult(unsigned count);
        [[nodiscard]] auto getCount() const noexcept -> unsigned;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class GetCountArtists : public Query
    {
      public:
        explicit GetCountArtists();
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class GetCountAlbums : public Query
    {
      public:
        explicit GetCountAlbums();
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

}; // namespace db::multimedia_files::query
