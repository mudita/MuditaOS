// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <models/SongsRepository.hpp>

#include <optional>

namespace testing::app::music
{
    class MockTagsFetcher : public ::app::music::AbstractTagsFetcher
    {
      public:
        MOCK_METHOD(std::optional<tags::fetcher::Tags>, getFileTags, (const std::string &filePath), (const override));
    };
}; // namespace testing::app::music
