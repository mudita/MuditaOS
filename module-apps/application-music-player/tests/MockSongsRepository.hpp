// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <models/SongsRepository.hpp>

#include <optional>
#include <string>
#include <vector>

namespace testing::app::music_player
{
    class MockSongsRepository : public ::app::music_player::AbstractSongsRepository
    {
      public:
        MOCK_METHOD(void, scanMusicFilesList, (), (override));
        MOCK_METHOD(std::vector<tags::fetcher::Tags>, getMusicFilesList, (), (const override));
        MOCK_METHOD(std::size_t, getFileIndex, (const std::string &filePath), (const override));
        MOCK_METHOD(std::string, getNextFilePath, (const std::string &filePath), (const override));
        MOCK_METHOD(std::string, getPreviousFilePath, (const std::string &filePath), (const override));
    };
}; // namespace testing::app::music_player
