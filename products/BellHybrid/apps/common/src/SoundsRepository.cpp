// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SoundsRepository.hpp"

#include <algorithm>

namespace fs = std::filesystem;

namespace
{
    constexpr auto allowedExtensions = {".wav", ".mp3", ".flac"};
} // namespace

SoundsRepository::SoundsRepository(std::filesystem::path dirToScan)
{
    for (auto const &entry : std::filesystem::directory_iterator(dirToScan)) {
        processEntry(entry);
    }
}
std::optional<std::filesystem::path> SoundsRepository::titleToPath(const UTF8 &title) const
{
    const auto res =
        std::find_if(samples.begin(), samples.end(), [title](const auto &e) { return e.title == title.c_str(); });
    if (res != samples.end()) {
        return std::filesystem::path{res->filePath};
    }
    return {};
}
std::optional<UTF8> SoundsRepository::pathToTitle(std::filesystem::path path) const
{
    const auto res =
        std::find_if(samples.begin(), samples.end(), [&path](const auto &e) { return e.filePath == path; });
    if (res != samples.end()) {
        return res->title;
    }
    return {};
}

std::vector<UTF8> SoundsRepository::getSongTitles()
{
    std::vector<UTF8> ret;
    std::transform(samples.begin(), samples.end(), std::back_inserter(ret), [](const auto &e) { return e.title; });
    return ret;
}

void SoundsRepository::processEntry(const std::filesystem::recursive_directory_iterator::value_type &entry)
{
    if (fs::is_regular_file(entry)) {
        for (const auto &ext : allowedExtensions) {
            if (fs::path(entry).extension() == ext) {
                samples.emplace_back(tags::fetcher::fetchTags(fs::absolute(entry).string()));
            }
        }
    }
}
