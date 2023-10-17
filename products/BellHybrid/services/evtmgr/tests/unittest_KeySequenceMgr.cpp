// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <internal/key_sequences/KeySequenceMgr.hpp>
#include <internal/key_sequences/GenericLongPressSequence.hpp>
#include <keymap/KeyMap.hpp>

namespace
{
    class TimerMock : public sys::Timer
    {
        void start() override
        {}
        void restart(std::chrono::milliseconds newInterval) override
        {}
        void stop() override
        {}
        bool isActive() const noexcept override
        {
            return false;
        }
    };

    sys::TimerHandle mockFactory()
    {
        auto timer = new TimerMock();
        return sys::TimerHandle{timer};
    }

    class TwoKeysSequence : public GenericLongPressSequence<KeyMap::Back, KeyMap::Frontlight>
    {
      public:
        TwoKeysSequence() : GenericLongPressSequence<KeyMap::Back, KeyMap::Frontlight>{mockFactory()}
        {}
    };

    class LongPressSequence : public GenericLongPressSequence<KeyMap::Back>
    {
      public:
        LongPressSequence() : GenericLongPressSequence<KeyMap::Back>{mockFactory()}
        {}
    };

    RawKey make_press_back()
    {
        RawKey rawKey{};
        rawKey.state   = RawKey::State::Pressed;
        rawKey.keyCode = bsp::KeyCodes::FnRight;
        return rawKey;
    }

    RawKey make_release_back()
    {
        RawKey rawKey{};
        rawKey.state   = RawKey::State::Released;
        rawKey.keyCode = bsp::KeyCodes::FnRight;
        return rawKey;
    }

    RawKey make_press_frontlight()
    {
        RawKey rawKey{};
        rawKey.state   = RawKey::State::Pressed;
        rawKey.keyCode = bsp::KeyCodes::FnLeft;
        return rawKey;
    }

    RawKey make_release_frontlight()
    {
        RawKey rawKey{};
        rawKey.state   = RawKey::State::Released;
        rawKey.keyCode = bsp::KeyCodes::FnLeft;
        return rawKey;
    }

} // namespace

TEST_CASE("Single long press")
{
    std::uint8_t actionTriggered{};
    KeySequenceMgr::SequenceCollection collection;

    auto mockLongPressBack      = std::make_unique<LongPressSequence>();
    mockLongPressBack->onAction = [&actionTriggered]() { actionTriggered++; };
    LongPressSequence *mockRef  = mockLongPressBack.get();

    collection.emplace_back(std::move(mockLongPressBack));
    KeySequenceMgr keySequenceMgr{std::move(collection)};

    SECTION("Short press&release")
    {
        keySequenceMgr.process(make_press_back());
        CHECK(not actionTriggered);
        keySequenceMgr.process(make_release_back());
        CHECK(not actionTriggered);
    }

    SECTION("Long press without release")
    {
        keySequenceMgr.process(make_press_back());
        mockRef->handleTimer();
        CHECK(actionTriggered == 1);
    }

    SECTION("Valid sequence")
    {
        keySequenceMgr.process(make_press_back());
        mockRef->handleTimer();
        keySequenceMgr.process(make_release_back());
        CHECK(actionTriggered == 1);
    }
}

TEST_CASE("Many long press sequences")
{
    KeySequenceMgr::SequenceCollection collection;

    std::uint8_t actionTriggered_1{};
    auto longPressBack_1      = std::make_unique<LongPressSequence>();
    longPressBack_1->onAction = [&actionTriggered_1]() { actionTriggered_1++; };
    LongPressSequence *ref_1  = longPressBack_1.get();
    collection.emplace_back(std::move(longPressBack_1));

    std::uint8_t actionTriggered_2{};
    auto longPressBack_2      = std::make_unique<LongPressSequence>();
    longPressBack_2->onAction = [&actionTriggered_2]() { actionTriggered_2++; };
    LongPressSequence *ref_2  = longPressBack_2.get();
    collection.emplace_back(std::move(longPressBack_2));

    std::uint8_t actionTriggered_3{};
    auto longPressBack_3      = std::make_unique<LongPressSequence>();
    longPressBack_3->onAction = [&actionTriggered_3]() { actionTriggered_3++; };
    LongPressSequence *ref_3  = longPressBack_3.get();
    collection.emplace_back(std::move(longPressBack_3));

    KeySequenceMgr keySequenceMgr{std::move(collection)};

    SECTION("Third sequence ready -> ought be invoked")
    {
        keySequenceMgr.process(make_press_back());
        CHECK(not actionTriggered_1);
        CHECK(not actionTriggered_2);

        ref_1->handleTimer();
        CHECK(not actionTriggered_1);
        CHECK(not actionTriggered_2);

        ref_2->handleTimer();
        CHECK(not actionTriggered_1);
        CHECK(not actionTriggered_2);

        ref_3->handleTimer();
        CHECK(not actionTriggered_1);
        CHECK(not actionTriggered_2);
        CHECK(actionTriggered_3 == 1);

        keySequenceMgr.process(make_release_back());
        CHECK(not actionTriggered_1);
        CHECK(not actionTriggered_2);
        CHECK(actionTriggered_3 == 1);
    }

    SECTION("Two sequences ready & release -> only second sequence ought to be invoked")
    {
        keySequenceMgr.process(make_press_back());
        CHECK(not actionTriggered_1);
        CHECK(not actionTriggered_2);

        ref_1->handleTimer();
        CHECK(not actionTriggered_1);
        CHECK(not actionTriggered_2);

        ref_2->handleTimer();
        CHECK(not actionTriggered_1);
        CHECK(not actionTriggered_2);

        keySequenceMgr.process(make_release_back());
        CHECK(not actionTriggered_1);
        CHECK(actionTriggered_2 == 1);
        CHECK(not actionTriggered_3);
    }

    SECTION("First sequence ready -> should be invoked")
    {
        keySequenceMgr.process(make_press_back());
        CHECK(not actionTriggered_1);
        CHECK(not actionTriggered_2);

        ref_1->handleTimer();
        CHECK(not actionTriggered_1);
        CHECK(not actionTriggered_2);

        keySequenceMgr.process(make_release_back());
        CHECK(actionTriggered_1 == 1);
        CHECK(not actionTriggered_2);
    }

    SECTION("Second sequence ready -> should be invoked")
    {
        keySequenceMgr.process(make_press_back());
        CHECK(not actionTriggered_1);
        CHECK(not actionTriggered_2);
        CHECK(not actionTriggered_3);

        ref_1->handleTimer();
        CHECK(not actionTriggered_1);
        CHECK(not actionTriggered_2);
        CHECK(not actionTriggered_3);

        ref_2->handleTimer();
        CHECK(not actionTriggered_1);
        CHECK(not actionTriggered_2);
        CHECK(not actionTriggered_3);

        keySequenceMgr.process(make_release_back());
        CHECK(not actionTriggered_1);
        CHECK(actionTriggered_2 == 1);
        CHECK(not actionTriggered_3);
    }

    SECTION("Press&release without ready -> no action ought to be invoked")
    {
        keySequenceMgr.process(make_press_back());
        CHECK(not actionTriggered_1);
        CHECK(not actionTriggered_2);

        CHECK(not actionTriggered_1);
        CHECK(not actionTriggered_2);

        keySequenceMgr.process(make_release_back());
        CHECK(not actionTriggered_1);
        CHECK(not actionTriggered_2);
    }
}

TEST_CASE("Two keys sequence")
{
    KeySequenceMgr::SequenceCollection collection;

    std::uint8_t actionTriggered_1{};
    auto sequence          = std::make_unique<TwoKeysSequence>();
    sequence->onAction     = [&actionTriggered_1]() { actionTriggered_1++; };
    TwoKeysSequence *ref_1 = sequence.get();
    collection.emplace_back(std::move(sequence));

    KeySequenceMgr keySequenceMgr{std::move(collection)};

    SECTION("Valid sequence -> action ought to be invoked regardless of the type of the key release")
    {
        keySequenceMgr.process(make_press_frontlight());
        keySequenceMgr.process(make_press_back());
        CHECK(not actionTriggered_1);

        ref_1->handleTimer();
        CHECK(actionTriggered_1 == 1);

        keySequenceMgr.process(make_release_frontlight());
        CHECK(actionTriggered_1 == 1);
        keySequenceMgr.process(make_release_back());
        CHECK(actionTriggered_1 == 1);
    }

    SECTION("Key release in the middle of sequence -> no action ought to be invoked")
    {
        keySequenceMgr.process(make_press_frontlight());
        keySequenceMgr.process(make_press_back());
        CHECK(not actionTriggered_1);

        keySequenceMgr.process(make_release_back());
        ref_1->handleTimer();
        CHECK(not actionTriggered_1);

        keySequenceMgr.process(make_release_frontlight());
        CHECK(not actionTriggered_1);
    }
}

TEST_CASE("Two keys sequence type mixed with long press sequences")
{
    KeySequenceMgr::SequenceCollection collection;

    std::uint8_t actionTriggered_1{};
    auto sequence1      = std::make_unique<LongPressSequence>();
    sequence1->onAction = [&actionTriggered_1]() { actionTriggered_1++; };
    // LongPressSequence *ref_1 = sequence1.get();
    collection.emplace_back(std::move(sequence1));

    std::uint8_t actionTriggered_2{};
    auto sequence2      = std::make_unique<LongPressSequence>();
    sequence2->onAction = [&actionTriggered_2]() { actionTriggered_2++; };
    // LongPressSequence *ref_2 = sequence2.get();
    collection.emplace_back(std::move(sequence2));

    std::uint8_t actionTriggered_3{};
    auto sequence3         = std::make_unique<TwoKeysSequence>();
    sequence3->onAction    = [&actionTriggered_3]() { actionTriggered_3++; };
    TwoKeysSequence *ref_3 = sequence3.get();
    collection.emplace_back(std::move(sequence3));

    KeySequenceMgr keySequenceMgr{std::move(collection)};

    SECTION("Interrupt third sequence")
    {
        keySequenceMgr.process(make_press_back());
        keySequenceMgr.process(make_press_frontlight());
        CHECK(not actionTriggered_1);
        CHECK(not actionTriggered_2);
        CHECK(not actionTriggered_3);

        keySequenceMgr.process(make_release_frontlight());
        CHECK(not actionTriggered_1);
        CHECK(not actionTriggered_2);
        CHECK(not actionTriggered_3);

        keySequenceMgr.process(make_release_back());
        CHECK(not actionTriggered_1);
        CHECK(not actionTriggered_2);
        CHECK(not actionTriggered_3);
    }

    SECTION("Two keys sequence ready, back pressed first")
    {
        keySequenceMgr.process(make_press_back());
        keySequenceMgr.process(make_press_frontlight());
        CHECK(not actionTriggered_1);
        CHECK(not actionTriggered_2);
        CHECK(not actionTriggered_3);

        ref_3->handleTimer();
        CHECK(not actionTriggered_1);
        CHECK(not actionTriggered_2);
        CHECK(actionTriggered_3 == 1);

        keySequenceMgr.process(make_release_back());
        CHECK(not actionTriggered_1);
        CHECK(not actionTriggered_2);
        CHECK(actionTriggered_3 == 1);

        keySequenceMgr.process(make_release_frontlight());
        CHECK(not actionTriggered_1);
        CHECK(not actionTriggered_2);
        CHECK(actionTriggered_3 == 1);
    }

    SECTION("Two keys sequence ready, frontlight pressed first")
    {
        keySequenceMgr.process(make_press_frontlight());
        keySequenceMgr.process(make_press_back());
        CHECK(not actionTriggered_1);
        CHECK(not actionTriggered_2);
        CHECK(not actionTriggered_3);

        ref_3->handleTimer();
        CHECK(not actionTriggered_1);
        CHECK(not actionTriggered_2);
        CHECK(actionTriggered_3 == 1);

        keySequenceMgr.process(make_release_back());
        CHECK(not actionTriggered_1);
        CHECK(not actionTriggered_2);
        CHECK(actionTriggered_3 == 1);

        keySequenceMgr.process(make_release_frontlight());
        CHECK(not actionTriggered_1);
        CHECK(not actionTriggered_2);
        CHECK(actionTriggered_3 == 1);
    }
}
