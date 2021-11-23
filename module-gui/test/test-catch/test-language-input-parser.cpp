// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <Translator.hpp>

TEST_CASE("Parsing English input language", "[!mayfail]")
{

    gui::KeyInputMappedTranslation translator;
    RawKey key;

    SECTION("Getting charKey from lower letters")
    {
        key.keyCode = bsp::KeyCodes::NumericKey1;
        REQUIRE(translator.handle(key, "English_lower") == 46);
        key.keyCode = bsp::KeyCodes::NumericKey2;
        REQUIRE(translator.handle(key, "English_lower") == 97);
        key.keyCode = bsp::KeyCodes::NumericKey3;
        REQUIRE(translator.handle(key, "English_lower") == 100);
        key.keyCode = bsp::KeyCodes::NumericKey4;
        REQUIRE(translator.handle(key, "English_lower") == 103);
        key.keyCode = bsp::KeyCodes::NumericKey5;
        REQUIRE(translator.handle(key, "English_lower") == 106);
        key.keyCode = bsp::KeyCodes::NumericKey6;
        REQUIRE(translator.handle(key, "English_lower") == 109);
        key.keyCode = bsp::KeyCodes::NumericKey7;
        REQUIRE(translator.handle(key, "English_lower") == 112);
        key.keyCode = bsp::KeyCodes::NumericKey8;
        REQUIRE(translator.handle(key, "English_lower") == 116);
        key.keyCode = bsp::KeyCodes::NumericKey9;
        REQUIRE(translator.handle(key, "English_lower") == 119);
        key.keyCode = bsp::KeyCodes::NumericKey0;
        REQUIRE(translator.handle(key, "English_lower") == 32);
    }

    SECTION("Getting charKey from upper letters")
    {
        key.keyCode = bsp::KeyCodes::NumericKey1;
        REQUIRE(translator.handle(key, "English_upper") == 46);
        key.keyCode = bsp::KeyCodes::NumericKey2;
        REQUIRE(translator.handle(key, "English_upper") == 65);
        key.keyCode = bsp::KeyCodes::NumericKey3;
        REQUIRE(translator.handle(key, "English_upper") == 68);
        key.keyCode = bsp::KeyCodes::NumericKey4;
        REQUIRE(translator.handle(key, "English_upper") == 71);
        key.keyCode = bsp::KeyCodes::NumericKey5;
        REQUIRE(translator.handle(key, "English_upper") == 74);
        key.keyCode = bsp::KeyCodes::NumericKey6;
        REQUIRE(translator.handle(key, "English_upper") == 77);
        key.keyCode = bsp::KeyCodes::NumericKey7;
        REQUIRE(translator.handle(key, "English_upper") == 80);
        key.keyCode = bsp::KeyCodes::NumericKey8;
        REQUIRE(translator.handle(key, "English_upper") == 84);
        key.keyCode = bsp::KeyCodes::NumericKey9;
        REQUIRE(translator.handle(key, "English_upper") == 87);
        key.keyCode = bsp::KeyCodes::NumericKey0;
        REQUIRE(translator.handle(key, "English_upper") == 32);
    }
}

TEST_CASE("Parsing numeric keyboard", "[!mayfail]")
{
    gui::KeyInputMappedTranslation translator;
    RawKey key;

    key.keyCode = bsp::KeyCodes::NumericKey1;
    REQUIRE(translator.handle(key, "numeric") == 49);
    key.keyCode = bsp::KeyCodes::NumericKey2;
    REQUIRE(translator.handle(key, "numeric") == 50);
    key.keyCode = bsp::KeyCodes::NumericKey3;
    REQUIRE(translator.handle(key, "numeric") == 51);
    key.keyCode = bsp::KeyCodes::NumericKey4;
    REQUIRE(translator.handle(key, "numeric") == 52);
    key.keyCode = bsp::KeyCodes::NumericKey5;
    REQUIRE(translator.handle(key, "numeric") == 53);
    key.keyCode = bsp::KeyCodes::NumericKey6;
    REQUIRE(translator.handle(key, "numeric") == 54);
    key.keyCode = bsp::KeyCodes::NumericKey7;
    REQUIRE(translator.handle(key, "numeric") == 55);
    key.keyCode = bsp::KeyCodes::NumericKey8;
    REQUIRE(translator.handle(key, "numeric") == 56);
    key.keyCode = bsp::KeyCodes::NumericKey9;
    REQUIRE(translator.handle(key, "numeric") == 57);
    key.keyCode = bsp::KeyCodes::NumericKey0;
    REQUIRE(translator.handle(key, "numeric") == 48);
}

TEST_CASE("Getting charKey after clicking button twice", "[!mayfail]")
{
    gui::KeyInputMappedTranslation translator;
    RawKey key;

    key.keyCode = bsp::KeyCodes::NumericKey2;
    key.state   = RawKey::State::Released;
    translator.handle(key, "English_lower");
    REQUIRE(translator.handle(key, "English_lower") == 98);
}