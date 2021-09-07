// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TagsFetcher.hpp"

#include <gsl/util>
#include <Utils.hpp>

#include "fileref.h"
#include "tag.h"
#include "tfilestream.h"

namespace tags::fetcher
{
    std::optional<Tags> fetchTagsInternal(std::string filePath)
    {
        TagLib::FileRef tagReader(filePath.c_str());
        if (!tagReader.isNull() && tagReader.tag()) {
            TagLib::Tag *tags                   = tagReader.tag();
            TagLib::AudioProperties *properties = tagReader.audioProperties();

            constexpr auto unicode = true;

            auto getTitle = [&]() -> std::string {
                const auto title = tags->title().to8Bit(unicode);
                // If title tag empty fill it with raw file name
                if (title.size() == 0) {
                    if (const auto pos = filePath.rfind("/"); pos == std::string::npos) {
                        return filePath;
                    }
                    else {
                        return &filePath[pos + 1];
                    }
                }
                return title;
            };

            const auto artist = tags->artist().to8Bit(unicode);
            const auto album  = tags->album().to8Bit(unicode);
            const auto genre  = tags->genre().to8Bit(unicode);
            const auto year   = std::to_string(tags->year());

            const uint32_t total_duration_s = properties->length();
            const uint32_t duration_min     = total_duration_s / utils::secondsInMinute;
            const uint32_t duration_hour    = duration_min / utils::secondsInMinute;
            const uint32_t duration_sec     = total_duration_s % utils::secondsInMinute;
            const uint32_t sample_rate      = properties->sampleRate();
            const uint32_t num_channel      = properties->channels();
            const uint32_t bitrate          = properties->bitrate();
            const auto title                = getTitle();

            return Tags{total_duration_s,
                        duration_hour,
                        duration_min,
                        duration_sec,
                        sample_rate,
                        num_channel,
                        bitrate,
                        artist,
                        genre,
                        title,
                        album,
                        year,
                        filePath};
        }

        return {};
    }

    Tags fetchTags(std::string filePath)
    {
        return fetchTagsInternal(filePath).value_or(Tags{filePath});
    }

} // namespace tags::fetcher
