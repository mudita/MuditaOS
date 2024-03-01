// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-audio/tags_fetcher/TagsFetcher.hpp>
#include <apps-common/models/SongsRepository.hpp>
#include <utf8/UTF8.hpp>

#include <filesystem>
#include <optional>
#include <vector>

class AbstractSimpleSoundsRepository
{
  public:
    virtual ~AbstractSimpleSoundsRepository()                                         = default;
    virtual std::optional<std::filesystem::path> titleToPath(const UTF8 &title) const = 0;
    virtual std::optional<UTF8> pathToTitle(std::filesystem::path) const              = 0;
    virtual std::vector<UTF8> getSongTitles()                                         = 0;
};

class SimpleSoundsRepository : public AbstractSimpleSoundsRepository
{
  public:
    explicit SimpleSoundsRepository(std::filesystem::path dirToScan);

    std::optional<std::filesystem::path> titleToPath(const UTF8 &title) const override;
    std::optional<UTF8> pathToTitle(std::filesystem::path path) const override;
    std::vector<UTF8> getSongTitles() override;

  private:
    void processEntry(const std::filesystem::recursive_directory_iterator::value_type &entry);

    std::vector<tags::fetcher::Tags> samples;
};

class AbstractSoundsRepository
{
  public:
    using OnGetMusicFilesListCallback =
        std::function<bool(const std::vector<db::multimedia_files::MultimediaFilesRecord> &, std::uint32_t)>;

    virtual ~AbstractSoundsRepository() noexcept = default;

    virtual void init()                                                     = 0;
    virtual void getMusicFiles(std::uint32_t offset,
                               std::uint32_t limit,
                               const OnGetMusicFilesListCallback &callback) = 0;

    virtual std::uint32_t getFilesCount() = 0;
    virtual void updateFilesCount()       = 0;
};

class SoundsRepository : public AbstractSoundsRepository, public app::AsyncCallbackReceiver
{
  public:
    enum class SortingBy
    {
        IdAscending,
        TitleAscending
    };
    struct PathSorting
    {
        std::string prefix;
        SortingBy sorting;
    };

    SoundsRepository(app::ApplicationCommon *application, const PathSorting &path);
    SoundsRepository(app::ApplicationCommon *application, const std::vector<PathSorting> &pathsVector);
    void init() override;
    void getMusicFiles(std::uint32_t offset,
                       std::uint32_t limit,
                       const OnGetMusicFilesListCallback &viewUpdateCallback) override;

    std::uint32_t getFilesCount() override;
    void updateFilesCount() override;

  private:
    struct PathDetails
    {
        std::uint32_t id;
        std::uint32_t count;
        SortingBy sorting;
        std::string prefix;
    };

    app::ApplicationCommon *application;
    std::vector<PathDetails> paths;
    app::music::FilesCache musicFilesViewCache;

    void updateFilesCount(const std::uint32_t id, const std::string &path);
    bool updateCountCallback(const std::uint32_t id, const std::uint32_t count);
    void getMusicFiles(const std::string &path,
                       const SortingBy sorting,
                       const std::uint32_t offset,
                       const std::uint32_t limit,
                       const OnGetMusicFilesListCallback &viewUpdateCallback);
};
