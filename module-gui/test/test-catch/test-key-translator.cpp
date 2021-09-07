// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <Translator.hpp>

TEST_CASE("Regular key press and release")
{
    gui::KeyBaseTranslation translator;
    RawKey key;

    key.state  = RawKey::State::Pressed;
    auto event = translator.set(key);
    REQUIRE(event.getState() == gui::InputEvent::State::keyPressed);

    key.state = RawKey::State::Released;
    event     = translator.set(key);
    REQUIRE(event.getState() == gui::InputEvent::State::keyReleasedShort);
}

TEST_CASE("Key release before first key press")
{
    gui::KeyBaseTranslation translator;
    RawKey key;

    key.state  = RawKey::State::Released;
    auto event = translator.set(key);
    REQUIRE(event.getState() == gui::InputEvent::State::Undefined);
}

TEST_CASE("Key long release")
{
    gui::KeyBaseTranslation translator;
    RawKey key;
    constexpr auto timeToLongRelease = 1001;

    key.state     = RawKey::State::Pressed;
    key.timePress = 0;
    auto event    = translator.set(key);
    REQUIRE(event.getState() == gui::InputEvent::State::keyPressed);

    key.state       = RawKey::State::Released;
    key.timeRelease = timeToLongRelease;
    event           = translator.set(key);
    REQUIRE(event.getState() == gui::InputEvent::State::keyReleasedLong);
}

TEST_CASE("Key Moved")
{
    gui::KeyBaseTranslation translator;
    RawKey key;

    key.state  = RawKey::State::Moved;
    auto event = translator.set(key);
    REQUIRE(event.getState() == gui::InputEvent::State::keyMoved);
}

TEST_CASE("Key Undefined")
{
    gui::KeyBaseTranslation translator;
    RawKey key;

    auto event = translator.set(key);
    REQUIRE(event.getState() == gui::InputEvent::State::Undefined);
}

TEST_CASE("External long press timeout check")
{
    gui::KeyBaseTranslation translator;
    RawKey key;
    constexpr auto timeBeforeLongRelease = 500;
    constexpr auto timeToLongRelease     = 1001;

    key.state     = RawKey::State::Pressed;
    key.timePress = 1;
    translator.set(key);

    REQUIRE(!translator.isKeyPressTimedOut(timeBeforeLongRelease));
    REQUIRE(translator.isKeyPressTimedOut(timeToLongRelease));
}
