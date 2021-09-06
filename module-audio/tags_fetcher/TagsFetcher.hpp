
// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace tags::fetcher
{
    struct Tags
    {

        /* Total audio duration in seconds */
        uint32_t total_duration_s = 0;
        /* Audio duration - hours part */
        uint32_t duration_hour = 0;
        /* Audio duration - minutes part */
        uint32_t duration_min = 0;
        /* Audio duration - seconds part */
        uint32_t duration_sec = 0;

        /* Sample rate */
        uint32_t sample_rate = 0;
        /* Number of channels */
        uint32_t num_channel = 0;
        /* bitrate */
        uint32_t bitrate = 0;

        std::string artist;
        std::string genre;
        std::string album;
        uint32_t year = 0;
        std::string filePath;
        std::string title;
        std::string comment;
        uint32_t track = 0;

        Tags() = default;
        Tags(uint32_t total_duration_s,
             uint32_t duration_hour,
             uint32_t duration_min,
             uint32_t duration_sec,
             uint32_t sample_rate,
             uint32_t num_channel,
             uint32_t bitrate,
             std::string artist,
             std::string genre,
             std::string title,
             std::string album,
             uint32_t year,
             std::string filePath,
             std::string comment,
             uint32_t track)
            : total_duration_s{total_duration_s}, duration_hour{duration_hour}, duration_min{duration_min},
              duration_sec{duration_sec}, sample_rate{sample_rate},
              num_channel{num_channel}, bitrate{bitrate}, artist{artist}, genre{genre}, album{album}, year{year},
              filePath{filePath}, title{title}, comment{comment}, track{track}
        {}

        explicit Tags(std::string filePath) : filePath{filePath}, title{filePath}
        {}
    };

    Tags fetchTags(std::string fileName);
} // namespace tags::fetcher
