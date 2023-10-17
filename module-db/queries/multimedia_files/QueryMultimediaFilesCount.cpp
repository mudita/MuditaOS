// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryMultimediaFilesCount.hpp"

#include <string>

namespace db::multimedia_files::query
{

    GetCount::GetCount() : Query(Query::Type::Read)
    {}

    GetCountResult::GetCountResult(unsigned count) : count(count)
    {}

    [[nodiscard]] auto GetCount::debugInfo() const -> std::string
    {
        return "GetCount";
    }

    [[nodiscard]] auto GetCountResult::debugInfo() const -> std::string
    {
        return "GetCountResult";
    }

    auto GetCountResult::getCount() const noexcept -> unsigned
    {
        return count;
    }

    GetCountArtists::GetCountArtists() : Query(Query::Type::Read)
    {}

    [[nodiscard]] auto GetCountArtists::debugInfo() const -> std::string
    {
        return "GetCountArtists";
    }

    GetCountAlbums::GetCountAlbums() : Query(Query::Type::Read)
    {}

    [[nodiscard]] auto GetCountAlbums::debugInfo() const -> std::string
    {
        return "GetCountAlbums";
    }

    GetCountForArtist::GetCountForArtist(const Artist &artist) : Query(Query::Type::Read), artist(artist)
    {}

    [[nodiscard]] auto GetCountForArtist::debugInfo() const -> std::string
    {
        return "GetCountForArtist";
    }

    GetCountForAlbum::GetCountForAlbum(const Album &album) : Query(Query::Type::Read), album(album)
    {}

    [[nodiscard]] auto GetCountForAlbum::debugInfo() const -> std::string
    {
        return "GetCountForAlbum";
    }

} // namespace db::multimedia_files::query
