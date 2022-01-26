// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <internal/key_sequences/GenericLongPressSequence.hpp>

namespace
{
    RawKey make_press_back()
    {
        RawKey rawKey{};
        rawKey.state   = RawKey::State::Pressed;
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

    RawKey make_press_not_scanned()
    {
        RawKey rawKey{};
        rawKey.state   = RawKey::State::Pressed;
        rawKey.keyCode = bsp::KeyCodes::JoystickEnter;
        return rawKey;
    }

    RawKey make_release_back()
    {
        RawKey rawKey{};
        rawKey.state   = RawKey::State::Released;
        rawKey.keyCode = bsp::KeyCodes::FnRight;
        return rawKey;
    }

    RawKey make_release_frontlight()
    {
        RawKey rawKey{};
        rawKey.state   = RawKey::State::Released;
        rawKey.keyCode = bsp::KeyCodes::FnLeft;
        return rawKey;
    }

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

    class TestSequence : public GenericLongPressSequence<KeyMap::Back, KeyMap::Frontlight>
    {
      public:
        TestSequence() : GenericLongPressSequence<KeyMap::Back, KeyMap::Frontlight>{mockFactory()}
        {}
    };
} // namespace

TEST_CASE("GenericLongPressSequence")
{
    TestSequence sequence;

    std::uint8_t idleInvoke{};
    std::uint8_t triggerInvoke{};
    std::uint8_t abortInvoke{};
    std::uint8_t readyInvoke{};

    auto onIdle      = [&idleInvoke](auto &) { idleInvoke++; };
    auto onTriggered = [&triggerInvoke](auto &) { triggerInvoke++; };
    auto onReady     = [&readyInvoke](auto &) { readyInvoke++; };
    auto onAbort     = [&abortInvoke](auto &) { abortInvoke++; };

    sequence.setCallbacks(onTriggered, onReady, onIdle, onAbort);

    SECTION("Frontlight&Back pressed ought to invoke trigger callback")
    {
        sequence.process(make_press_back());
        CHECK(not triggerInvoke);

        sequence.process(make_press_frontlight());
        CHECK(triggerInvoke == 1);
    }

    SECTION("Frontlight&Back pressed for specified time interval ought to invoke ready callback")
    {
        sequence.process(make_press_back());
        sequence.process(make_press_frontlight());
        CHECK(not readyInvoke);

        sequence.handleTimer();
        CHECK(readyInvoke == 1);
    }

    SECTION("Frontlight release ought to invoke idle callback, case 1")
    {
        sequence.process(make_press_back());
        sequence.process(make_press_frontlight());
        CHECK(not readyInvoke);

        sequence.handleTimer();
        CHECK(readyInvoke == 1);

        sequence.process(make_release_frontlight());
        CHECK(idleInvoke == 1);
        CHECK(abortInvoke == 0);
    }

    SECTION("Frontlight release ought to invoke idle callback, case 2")
    {
        sequence.process(make_press_back());
        sequence.process(make_press_frontlight());

        sequence.process(make_release_frontlight());
        CHECK(idleInvoke == 1);
        CHECK(abortInvoke == 0);
    }

    SECTION("Back release ought to invoke idle callback, case 1")
    {
        sequence.process(make_press_back());
        sequence.process(make_press_frontlight());
        CHECK(not readyInvoke);

        sequence.handleTimer();
        CHECK(readyInvoke == 1);

        sequence.process(make_release_back());
        CHECK(idleInvoke == 1);
        CHECK(abortInvoke == 0);
    }

    SECTION("Back release ought to invoke idle callback, case 2")
    {
        sequence.process(make_press_back());
        sequence.process(make_press_frontlight());

        sequence.process(make_release_back());
        CHECK(idleInvoke == 1);
        CHECK(abortInvoke == 0);
    }

    SECTION("Press of any key different than Back or Frontlight ought to invoke abort callback, case 1")
    {
        sequence.process(make_press_back());
        sequence.process(make_press_frontlight());

        sequence.process(make_press_not_scanned());
        CHECK(abortInvoke == 1);
    }

    SECTION("Press of any keys different than Back or Frontlight ought to invoke abort callback, case 2")
    {
        sequence.process(make_press_back());
        sequence.process(make_press_frontlight());

        sequence.handleTimer();
        sequence.process(make_press_not_scanned());
        CHECK(abortInvoke == 1);
    }
}
