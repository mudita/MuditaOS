// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Audio/Endpoint.hpp>

#include <gmock/gmock.h>

namespace testing::audio
{
    class MockSink : public ::audio::Sink
    {
      public:
        MOCK_METHOD(::audio::Endpoint::Traits, getTraits, (), (const, override));
        MOCK_METHOD(const std::vector<::audio::AudioFormat> &, getSupportedFormats, (), (override));
        MOCK_METHOD(void, onDataSend, (), (override));
        MOCK_METHOD(void, enableOutput, (), (override));
        MOCK_METHOD(void, disableOutput, (), (override));
    };

    class MockSource : public ::audio::Source
    {
      public:
        MOCK_METHOD(::audio::Endpoint::Traits, getTraits, (), (const, override));
        MOCK_METHOD(const std::vector<::audio::AudioFormat> &, getSupportedFormats, (), (override));
        MOCK_METHOD(::audio::AudioFormat, getSourceFormat, (), (override));
        MOCK_METHOD(void, onDataReceive, (), (override));
        MOCK_METHOD(void, enableInput, (), (override));
        MOCK_METHOD(void, disableInput, (), (override));
    };

} // namespace testing::audio
