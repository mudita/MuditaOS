// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MockTagsFetcher.hpp"

#include <models/SongsRepository.hpp>
#include <tags_fetcher/TagsFetcher.hpp>
#include <filesystem>
#include <fstream>
#include <stdexcept>

using ::testing::Return;
using ::testing::app::music_player::MockTagsFetcher;
namespace fs = std::filesystem;

constexpr auto testDir  = "appmusic-test";
constexpr auto emptyDir = "empty";
constexpr auto musicDir = "music";
constexpr auto bazDir   = "bazdir";
auto testDirPath        = fs::path(testDir);
auto emptyDirPath       = testDirPath / emptyDir;
auto musicDirPath       = testDirPath / musicDir;
auto bazDirPath         = musicDirPath / bazDir;

class SongsRepositoryFixture : public ::testing::Test
{
  protected:
    static void SetUpTestSuite()
    {
        if (fs::exists(testDirPath)) {
            TearDownTestSuite();
        }

        fs::create_directory(testDirPath);
        fs::create_directory(emptyDirPath);
        fs::create_directory(musicDirPath);

        createFile(musicDirPath / "foo");
        createFile(musicDirPath / "bar");

        fs::create_directory(bazDirPath);

        createFile(bazDirPath / "baz");

        fs::create_directory(musicDirPath / "bazzinga");
    }

    static void createFile(const std::string &path)
    {
        std::ofstream file(path);
        file << "app music test file";
    }

    static void TearDownTestSuite()
    {
        fs::remove_all(testDir);
    }

    auto getMockedRepository(const std::string &directoryToScan)
    {
        return std::make_unique<app::music_player::SongsRepository>(std::make_unique<MockTagsFetcher>(),
                                                                    directoryToScan);
    }
};

TEST_F(SongsRepositoryFixture, LazyInit)
{
    auto repo       = getMockedRepository(testDir);
    auto musicFiles = repo->getMusicFilesList();
    EXPECT_EQ(musicFiles.size(), 0);
}

TEST_F(SongsRepositoryFixture, Empty)
{
    auto repo = getMockedRepository(testDirPath / emptyDir);
    repo->scanMusicFilesList();

    auto musicFiles = repo->getMusicFilesList();
    EXPECT_EQ(musicFiles.size(), 0);
}

TEST_F(SongsRepositoryFixture, ScanEmptyFiles)
{
    auto tagsFetcherMock = std::make_unique<MockTagsFetcher>();
    auto rawMock         = tagsFetcherMock.get();
    auto repo = std::make_unique<app::music_player::SongsRepository>(std::move(tagsFetcherMock), musicDirPath);

    EXPECT_CALL(*rawMock, getFileTags).Times(2);
    repo->scanMusicFilesList();

    auto musicFiles = repo->getMusicFilesList();
    EXPECT_EQ(musicFiles.size(), 0);
}

TEST_F(SongsRepositoryFixture, ScanWithTagsReturn)
{
    auto tagsFetcherMock = std::make_unique<MockTagsFetcher>();
    auto rawMock         = tagsFetcherMock.get();
    auto repo = std::make_unique<app::music_player::SongsRepository>(std::move(tagsFetcherMock), musicDirPath);

    auto fooTags = tags::fetcher::Tags{"foo"};
    auto barTags = tags::fetcher::Tags{"foo"};

    ON_CALL(*rawMock, getFileTags(fs::path(musicDirPath / "foo").c_str())).WillByDefault(Return(fooTags));
    ON_CALL(*rawMock, getFileTags(fs::path(musicDirPath / "bar").c_str())).WillByDefault(Return(barTags));
    EXPECT_CALL(*rawMock, getFileTags).Times(2);
    repo->scanMusicFilesList();

    auto musicFiles = repo->getMusicFilesList();
    EXPECT_EQ(musicFiles.size(), 2);
}

TEST_F(SongsRepositoryFixture, FileIndex)
{
    auto tagsFetcherMock = std::make_unique<MockTagsFetcher>();
    auto rawMock         = tagsFetcherMock.get();
    auto repo = std::make_unique<app::music_player::SongsRepository>(std::move(tagsFetcherMock), musicDirPath);

    auto fooPath = musicDirPath / "foo";
    auto barPath = musicDirPath / "bar";

    auto fooTags = tags::fetcher::Tags(fooPath);
    auto barTags = tags::fetcher::Tags(barPath);

    ON_CALL(*rawMock, getFileTags(fs::path(musicDirPath / "foo").c_str())).WillByDefault(Return(fooTags));
    ON_CALL(*rawMock, getFileTags(fs::path(musicDirPath / "bar").c_str())).WillByDefault(Return(barTags));
    EXPECT_CALL(*rawMock, getFileTags).Times(2);
    repo->scanMusicFilesList();

    auto fooIndex = repo->getFileIndex(fooPath);
    auto barIndex = repo->getFileIndex(barPath);

    EXPECT_NE(fooIndex, static_cast<std::size_t>(-1));
    EXPECT_EQ(fooIndex, 1);

    EXPECT_NE(barIndex, static_cast<std::size_t>(-1));
    EXPECT_EQ(barIndex, 0);

    auto bazIndex = repo->getFileIndex("baz");
    EXPECT_EQ(bazIndex, static_cast<std::size_t>(-1));

    EXPECT_EQ(repo->getNextFilePath(barTags.filePath), fooTags.filePath);
    EXPECT_EQ(repo->getNextFilePath(fooTags.filePath), "");
    EXPECT_EQ(repo->getNextFilePath("rand"), "");
    EXPECT_EQ(repo->getNextFilePath(""), "");

    EXPECT_EQ(repo->getPreviousFilePath(barTags.filePath), "");
    EXPECT_EQ(repo->getPreviousFilePath(fooTags.filePath), barTags.filePath);
    EXPECT_EQ(repo->getPreviousFilePath("rand"), "");
    EXPECT_EQ(repo->getPreviousFilePath(""), "");
}
