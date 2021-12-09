// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <models/SongsRepository.hpp>

#include <optional>
#include <string>
#include <vector>

namespace testing::app::music
{
    class MockSongsRepository : public ::app::music::AbstractSongsRepository
    {
      public:
        MOCK_METHOD(void,
                    getMusicFilesList,
                    (std::uint32_t offset, std::uint32_t limit, const OnGetMusicFilesListCallback &callback),
                    (override));
        MOCK_METHOD(void, initCache, (), (override));
        MOCK_METHOD(std::size_t, getFileIndex, (const std::string &filePath), (const override));
        MOCK_METHOD(std::string, getNextFilePath, (const std::string &filePath), (const override));
        MOCK_METHOD(std::string, getPreviousFilePath, (const std::string &filePath), (const override));
        MOCK_METHOD(std::optional<db::multimedia_files::MultimediaFilesRecord>,
                    getRecord,
                    (const std::string &filePath),
                    (const override));
        MOCK_METHOD(void, updateRepository, (const std::string &filePath), (override));
    };
}; // namespace testing::app::music
