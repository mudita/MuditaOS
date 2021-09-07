// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <gtest/gtest.h>

#include "MockSongsRepository.hpp"

#include <models/SongsModel.hpp>

#include <memory>
#include <optional>

using ::app::music_player::SongsModel;
using ::testing::Return;
using ::testing::app::music_player::MockSongsRepository;

TEST(SongsModel, Init)
{
    auto mockRepo = std::make_shared<MockSongsRepository>();
    auto model    = SongsModel(mockRepo);

    EXPECT_EQ(model.requestRecordsCount(), 0);
    EXPECT_FALSE(model.isSongPlaying());
}

TEST(SongsModel, EmptyContext)
{
    auto mockRepo = std::make_shared<MockSongsRepository>();
    auto model    = SongsModel(mockRepo);

    auto ctx = model.getCurrentSongContext();

    EXPECT_EQ(ctx.currentFileToken, std::nullopt);
    EXPECT_TRUE(ctx.filePath.empty());
    EXPECT_EQ(ctx.currentSongState, app::music_player::SongState::NotPlaying);
}

TEST(SongsModel, createDataNoSongs)
{
    auto mockRepo = std::make_shared<MockSongsRepository>();
    auto model    = SongsModel(mockRepo);

    EXPECT_CALL(*mockRepo, scanMusicFilesList);
    EXPECT_CALL(*mockRepo, getMusicFilesList).WillRepeatedly(Return(std::vector<tags::fetcher::Tags>()));
    model.createData([](const std::string &) { return true; }, []() {}, [](const UTF8 &) {}, []() {});
}
