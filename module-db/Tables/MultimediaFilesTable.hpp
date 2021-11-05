// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Record.hpp"
#include "Table.hpp"
#include <Database/Database.hpp>

#include <string>

namespace db::multimedia_files
{
    using Artist = std::string;
    struct Album
    {
        Artist artist{};
        std::string title{};
    };

    struct Tags
    {
        std::string title{};
        Album album{};
        std::string comment{};
        std::string genre{};
        unsigned year{};
        unsigned track{};
    };

    struct AudioProperties
    {
        unsigned songLength{}; /// in seconds
        unsigned bitrate{};    /// in kb/s
        unsigned sampleRate{}; /// in Hz
        unsigned channels{};   /// 1 - mono, 2 - stereo
    };

    struct FileInfo
    {
        std::string path{};
        std::string mediaType{}; /// mime type e.g. "audio/mp3"
        std::size_t size{};      /// in bytes
    };

    struct TableRow : public Record
    {
        FileInfo fileInfo{};
        Tags tags{};
        AudioProperties audioProperties{};

        auto isValid() const -> bool;
    };

    enum class TableFields
    {
        path,
        media_type,
        size,
        title,
        artist,
        album,
        comment,
        genre,
        year,
        track,
        song_length,
        bitrate,
        sample_rate,
        channels
    };

    class MultimediaFilesTable : public Table<TableRow, TableFields>
    {
      public:
        explicit MultimediaFilesTable(Database *db);
        virtual ~MultimediaFilesTable() = default;

        auto create() -> bool override;
        auto add(TableRow entry) -> bool override;
        auto removeById(uint32_t id) -> bool override;
        auto removeByField(TableFields field, const char *str) -> bool override;
        bool removeAll() override final;
        auto update(TableRow entry) -> bool override;
        auto getById(uint32_t id) -> TableRow override;
        auto getLimitOffset(uint32_t offset, uint32_t limit) -> std::vector<TableRow> override;
        auto getLimitOffsetByField(uint32_t offset, uint32_t limit, TableFields field, const char *str)
            -> std::vector<TableRow> override;
        auto count() -> uint32_t override;
        auto countByFieldId(const char *field, uint32_t id) -> uint32_t override;

        auto getArtistsLimitOffset(uint32_t offset, uint32_t limit) -> std::vector<Artist>;
        auto countArtists() -> uint32_t;

        auto getAlbumsLimitOffset(uint32_t offset, uint32_t limit) -> std::vector<Album>;
        auto countAlbums() -> uint32_t;

        auto getLimitOffset(const Artist &artist, uint32_t offset, uint32_t limit) -> std::vector<TableRow>;
        auto count(const Artist &artist) -> uint32_t;

        auto getLimitOffset(const Album &album, uint32_t offset, uint32_t limit) -> std::vector<TableRow>;
        auto count(const Album &album) -> uint32_t;

        auto getLimitOffsetByPath(const std::string &path, uint32_t offset, uint32_t limit) -> std::vector<TableRow>;
        TableRow getByPath(std::string path);

        /// @note entry.ID is skipped
        bool addOrUpdate(TableRow entry, std::string oldPath = "");

      private:
        auto getFieldName(TableFields field) -> std::string;
    };
} // namespace db::multimedia_files
