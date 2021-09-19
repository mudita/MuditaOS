// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-db/Interface/MultimediaFilesRecord.hpp"

#include <Common/Query.hpp>

#include <string>

namespace db::multimedia_files::query
{
    class GetLimited : public Query
    {
      public:
        GetLimited(uint32_t offset, uint32_t limit);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        const uint32_t offset = 0;
        const uint32_t limit  = 0;
    };

    class GetLimitedForArtist : public Query
    {
      public:
        GetLimitedForArtist(Artist artist, uint32_t offset, uint32_t limit);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        const Artist artist;

        const uint32_t offset = 0;
        const uint32_t limit  = 0;
    };

    class GetLimitedForAlbum : public Query
    {
      public:
        GetLimitedForAlbum(Album album, uint32_t offset, uint32_t limit);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        const Album album;

        const uint32_t offset = 0;
        const uint32_t limit  = 0;
    };

    class GetLimitedResult : public QueryResult
    {
        const std::vector<MultimediaFilesRecord> records;
        unsigned int dbRecordsCount;

      public:
        explicit GetLimitedResult(std::vector<MultimediaFilesRecord> records, unsigned int dbRecordsCount);
        [[nodiscard]] auto getResult() const -> std::vector<MultimediaFilesRecord>;
        [[nodiscard]] auto getCount() const noexcept -> unsigned int;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class GetArtistsLimited : public Query
    {
      public:
        GetArtistsLimited(uint32_t offset, uint32_t limit);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        const uint32_t offset = 0;
        const uint32_t limit  = 0;
    };

    class GetArtistsLimitedResult : public QueryResult
    {
        const std::vector<Artist> records;
        unsigned int dbRecordsCount;

      public:
        explicit GetArtistsLimitedResult(std::vector<Artist> records, unsigned int dbRecordsCount);
        [[nodiscard]] auto getResult() const -> std::vector<Artist>;
        [[nodiscard]] auto getCount() const noexcept -> unsigned int;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class GetAlbumsLimited : public Query
    {
      public:
        GetAlbumsLimited(uint32_t offset, uint32_t limit);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        const uint32_t offset = 0;
        const uint32_t limit  = 0;
    };

    class GetAlbumsLimitedResult : public QueryResult
    {
        const std::vector<Album> records;
        unsigned int dbRecordsCount;

      public:
        explicit GetAlbumsLimitedResult(std::vector<Album> records, unsigned int dbRecordsCount);
        [[nodiscard]] auto getResult() const -> std::vector<Album>;
        [[nodiscard]] auto getCount() const noexcept -> unsigned int;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };
} // namespace db::multimedia_files::query
