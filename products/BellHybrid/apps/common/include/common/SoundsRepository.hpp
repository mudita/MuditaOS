// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-audio/tags_fetcher/TagsFetcher.hpp>
#include <utf8/UTF8.hpp>

#include <filesystem>
#include <optional>
#include <vector>

class AbstractSoundsRepository
{
  public:
    virtual ~AbstractSoundsRepository()                                               = default;
    virtual std::optional<std::filesystem::path> titleToPath(const UTF8 &title) const = 0;
    virtual std::optional<UTF8> pathToTitle(std::filesystem::path) const              = 0;
    virtual std::vector<UTF8> getSongTitles()                                         = 0;
};

class SoundsRepository : public AbstractSoundsRepository
{
  public:
    explicit SoundsRepository(std::filesystem::path dirToScan);

    std::optional<std::filesystem::path> titleToPath(const UTF8 &title) const override;
    std::optional<UTF8> pathToTitle(std::filesystem::path path) const override;
    std::vector<UTF8> getSongTitles() override;

  private:
    void processEntry(const std::filesystem::recursive_directory_iterator::value_type &entry);

    std::vector<tags::fetcher::Tags> samples;
};
