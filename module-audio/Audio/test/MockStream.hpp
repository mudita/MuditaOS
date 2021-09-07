// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Audio/AbstractStream.hpp>

#include <gmock/gmock.h>

namespace testing::audio
{

    class MockStream : public ::audio::AbstractStream
    {
      public:
        MOCK_METHOD(bool, push, (void *data, std::size_t dataSize), (override));
        MOCK_METHOD(bool, push, (const Span &span), (override));
        MOCK_METHOD(bool, push, (), (override));
        MOCK_METHOD(bool, pop, (Span & span), (override));
        MOCK_METHOD(bool, reserve, (Span & span), (override));
        MOCK_METHOD(void, commit, (), (override));
        MOCK_METHOD(void, release, (), (override));
        MOCK_METHOD(bool, peek, (Span & span), (override));
        MOCK_METHOD(void, consume, (), (override));
        MOCK_METHOD(void, unpeek, (), (override));
        MOCK_METHOD(void, reset, (), (override));
        MOCK_METHOD(void, registerListener, (EventListener * listener), (override));
        MOCK_METHOD(void, unregisterListeners, (EventListener * listener), (override));
        MOCK_METHOD(bool, isEmpty, (), (const, noexcept, override));
        MOCK_METHOD(bool, isFull, (), (const, noexcept, override));
        MOCK_METHOD(Traits, getInputTraits, (), (const, noexcept, override));
        MOCK_METHOD(Traits, getOutputTraits, (), (const, noexcept, override));
    };

    class MockStreamEventListener : public ::audio::AbstractStream::EventListener
    {
      public:
        MOCK_METHOD(void,
                    onEvent,
                    (::audio::AbstractStream * stream, ::audio::AbstractStream::Event event),
                    (override));
    };

} // namespace testing::audio
