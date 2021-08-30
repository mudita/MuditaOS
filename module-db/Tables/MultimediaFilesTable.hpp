// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "EventsTable.hpp"

#include <Common/Common.hpp>
#include <Database/Database.hpp>

#include <string>

struct MultimediaFilesTableRow : public Record
{
    std::string path;
    std::string mediaType; /// mime type e.g. "audio/mp3"
    std::size_t size{};
    struct
    {
        std::string title;
        std::string artist;
        std::string album;
        std::string comment;
        std::string genre;
        unsigned year{};
        unsigned track{};
    } tags;
    struct
    {
        unsigned songLength{};
        unsigned bitrate{};
        unsigned sampleRate{};
        unsigned channels{}; /// 1 - mono, 2 - stereo
    } audioProperties;

    auto isValid() const -> bool;
};

enum class MultimediaFilesTableFields
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

class MultimediaFilesTable : public Table<MultimediaFilesTableRow, MultimediaFilesTableFields>
{
  public:
    explicit MultimediaFilesTable(Database *db);
    virtual ~MultimediaFilesTable() = default;

    auto create() -> bool override;
    auto add(MultimediaFilesTableRow entry) -> bool override;
    auto removeById(uint32_t id) -> bool override;
    auto removeByField(MultimediaFilesTableFields field, const char *str) -> bool override;
    bool removeAll() override final;
    auto update(MultimediaFilesTableRow entry) -> bool override;
    auto getById(uint32_t id) -> MultimediaFilesTableRow override;
    auto getLimitOffset(uint32_t offset, uint32_t limit) -> std::vector<MultimediaFilesTableRow> override;
    auto getLimitOffsetByField(uint32_t offset, uint32_t limit, MultimediaFilesTableFields field, const char *str)
        -> std::vector<MultimediaFilesTableRow> override;
    auto count() -> uint32_t override;
    auto countByFieldId(const char *field, uint32_t id) -> uint32_t override;

  private:
    auto getFieldName(MultimediaFilesTableFields field) -> std::string;
};
