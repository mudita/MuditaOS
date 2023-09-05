// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TagsFetcher.hpp"
#include <riff/wav/wavproperties.h>
#include "fileref.h"
#include <time/time_constants.hpp>

namespace tags::fetcher
{
    namespace
    {
        bool isFormatSupported(TagLib::AudioProperties *properties)
        {
            /* WAV formats from taglib wav properties :
             * 0 for unknown, 1 for PCM, 2 for ADPCM, 3 for 32/64-bit IEEE754
             *
             * MuditaOS does not support WAV ADPCM encoding
             * */
            constexpr auto WAV_ADPCM = 2;
            auto wav_properties      = dynamic_cast<const TagLib::RIFF::WAV::Properties *>(properties);
            if (wav_properties == nullptr) {
                return true;
            }
            return wav_properties->format() != WAV_ADPCM;
        }

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
            const TagLib::ConstMemoryConstrainedFileRef tagReader(filePath.c_str(), TAGSFETCHER_MAX_TAG_SIZE);
            const auto tags = tagReader.tag();
            if (!tagReader.isNull() && (tags != nullptr)) {
                const auto properties = tagReader.audioProperties();

                if (!isFormatSupported(properties)) {
                    return std::nullopt;
                }

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
                const uint32_t duration_min     = total_duration_s / utils::time::secondsInMinute;
                const uint32_t duration_hour    = duration_min / utils::time::secondsInMinute;
                const uint32_t duration_sec     = total_duration_s % utils::time::secondsInMinute;
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
    } // namespace

    Tags fetchTags(const std::string &filePath)
    {
        return fetchTagsInternal(filePath).value_or(Tags{filePath, getTitleFromFilePath(filePath)});
    }

} // namespace tags::fetcher
