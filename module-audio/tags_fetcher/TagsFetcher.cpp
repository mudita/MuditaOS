// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TagsFetcher.hpp"
#include <Utils.hpp>
#include "fileref.h"

namespace tags::fetcher
{
    namespace
    {
        std::string getTitleFromFilePath(const std::string &path)
        {
            const auto pos = path.rfind('/');
            if (pos != std::string::npos) {
                return path.substr(pos + 1);
            }
            return path;
        }

        std::optional<Tags> fetchTagsInternal(const std::string &filePath)
        {
            const TagLib::FileRef tagReader(filePath.c_str());
            const auto tags = tagReader.tag();
            if (!tagReader.isNull() && (tags != nullptr)) {
                const auto properties = tagReader.audioProperties();

                constexpr bool unicode = true;

                const auto artist  = tags->artist().to8Bit(unicode);
                const auto album   = tags->album().to8Bit(unicode);
                const auto genre   = tags->genre().to8Bit(unicode);
                const auto year    = tags->year();
                const auto comment = tags->comment().to8Bit(unicode);
                const auto track   = tags->track();

                auto title = tags->title().to8Bit(unicode);
                if (title.empty()) {
                    title = getTitleFromFilePath(filePath);
                }

                const uint32_t total_duration_s = properties->length();
                const uint32_t duration_min     = total_duration_s / utils::secondsInMinute;
                const uint32_t duration_hour    = duration_min / utils::secondsInMinute;
                const uint32_t duration_sec     = total_duration_s % utils::secondsInMinute;
                const uint32_t sample_rate      = properties->sampleRate();
                const uint32_t num_channel      = properties->channels();
                const uint32_t bitrate          = properties->bitrate();

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
                            filePath,
                            comment,
                            track};
            }

            return {};
        }
    }

    Tags fetchTags(const std::string &filePath)
    {
        return fetchTagsInternal(filePath).value_or(Tags{filePath, getTitleFromFilePath(filePath)});
    }

} // namespace tags::fetcher
