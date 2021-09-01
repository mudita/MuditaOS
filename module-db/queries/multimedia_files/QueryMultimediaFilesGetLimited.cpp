// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryMultimediaFilesGetLimited.hpp"

namespace db::multimedia_files::query
{
    GetLimited::GetLimited(uint32_t offset, uint32_t limit) : Query(Query::Type::Read), offset(offset), limit(limit)
    {}

    auto GetLimited::debugInfo() const -> std::string
    {
        return std::string{"GetLimited"};
    }

    GetLimitedForArtist::GetLimitedForArtist(Artist artist, uint32_t offset, uint32_t limit)
        : Query(Query::Type::Read), artist(artist), offset(offset), limit(limit)
    {}

    auto GetLimitedForArtist::debugInfo() const -> std::string
    {
        return std::string{"GetLimitedForArtist"};
    }

    GetLimitedForAlbum::GetLimitedForAlbum(Album album, uint32_t offset, uint32_t limit)
        : Query(Query::Type::Read), album(album), offset(offset), limit(limit)
    {}

    auto GetLimitedForAlbum::debugInfo() const -> std::string
    {
        return std::string{"GetLimitedForAlbum"};
    }

    GetLimitedResult::GetLimitedResult(std::vector<MultimediaFilesRecord> records) : records(std::move(records))
    {}

    auto GetLimitedResult::getResult() const -> std::vector<MultimediaFilesRecord>
    {
        return records;
    }

    auto GetLimitedResult::debugInfo() const -> std::string
    {
        return std::string{"GetLimitedResult"};
    }

    GetArtistsLimited::GetArtistsLimited(uint32_t offset, uint32_t limit)
        : Query(Query::Type::Read), offset(offset), limit(limit)
    {}

    auto GetArtistsLimited::debugInfo() const -> std::string
    {
        return std::string{"GetArtistsLimited"};
    }

    GetArtistsLimitedResult::GetArtistsLimitedResult(std::vector<Artist> records) : records(std::move(records))
    {}

    auto GetArtistsLimitedResult::getResult() const -> std::vector<Artist>
    {
        return records;
    }

    auto GetArtistsLimitedResult::debugInfo() const -> std::string
    {
        return std::string{"GetArtistsLimitedResult"};
    }

    GetAlbumsLimited::GetAlbumsLimited(uint32_t offset, uint32_t limit)
        : Query(Query::Type::Read), offset(offset), limit(limit)
    {}

    auto GetAlbumsLimited::debugInfo() const -> std::string
    {
        return std::string{"GetAlbumsLimited"};
    }

    GetAlbumsLimitedResult::GetAlbumsLimitedResult(std::vector<Album> records) : records(std::move(records))
    {}

    auto GetAlbumsLimitedResult::getResult() const -> std::vector<Album>
    {
        return records;
    }

    auto GetAlbumsLimitedResult::debugInfo() const -> std::string
    {
        return std::string{"GetAlbumsLimitedResult"};
    }
} // namespace db::multimedia_files::query
