// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <Translator.hpp>

#include <vfs.hpp>

extern class vfs vfs;

TEST_CASE("Parsing English input language")
{

    gui::KeyInputMappedTranslation translator;
    RawKey key;

    SECTION("Getting charKey from lower letters")
    {
        vfs.Init();

        key.key_code = bsp::KeyCodes::NumericKey1;
        REQUIRE(translator.handle(key, "English_lower") == 46);
        key.key_code = bsp::KeyCodes::NumericKey2;
        REQUIRE(translator.handle(key, "English_lower") == 97);
        key.key_code = bsp::KeyCodes::NumericKey3;
        REQUIRE(translator.handle(key, "English_lower") == 100);
        key.key_code = bsp::KeyCodes::NumericKey4;
        REQUIRE(translator.handle(key, "English_lower") == 103);
        key.key_code = bsp::KeyCodes::NumericKey5;
        REQUIRE(translator.handle(key, "English_lower") == 106);
        key.key_code = bsp::KeyCodes::NumericKey6;
        REQUIRE(translator.handle(key, "English_lower") == 109);
        key.key_code = bsp::KeyCodes::NumericKey7;
        REQUIRE(translator.handle(key, "English_lower") == 112);
        key.key_code = bsp::KeyCodes::NumericKey8;
        REQUIRE(translator.handle(key, "English_lower") == 116);
        key.key_code = bsp::KeyCodes::NumericKey9;
        REQUIRE(translator.handle(key, "English_lower") == 119);
        key.key_code = bsp::KeyCodes::NumericKey0;
        REQUIRE(translator.handle(key, "English_lower") == 32);
    }

    SECTION("Getting charKey from upper letters")
    {
        vfs.Init();

        key.key_code = bsp::KeyCodes::NumericKey1;
        REQUIRE(translator.handle(key, "English_upper") == 46);
        key.key_code = bsp::KeyCodes::NumericKey2;
        REQUIRE(translator.handle(key, "English_upper") == 65);
        key.key_code = bsp::KeyCodes::NumericKey3;
        REQUIRE(translator.handle(key, "English_upper") == 68);
        key.key_code = bsp::KeyCodes::NumericKey4;
        REQUIRE(translator.handle(key, "English_upper") == 71);
        key.key_code = bsp::KeyCodes::NumericKey5;
        REQUIRE(translator.handle(key, "English_upper") == 74);
        key.key_code = bsp::KeyCodes::NumericKey6;
        REQUIRE(translator.handle(key, "English_upper") == 77);
        key.key_code = bsp::KeyCodes::NumericKey7;
        REQUIRE(translator.handle(key, "English_upper") == 80);
        key.key_code = bsp::KeyCodes::NumericKey8;
        REQUIRE(translator.handle(key, "English_upper") == 84);
        key.key_code = bsp::KeyCodes::NumericKey9;
        REQUIRE(translator.handle(key, "English_upper") == 87);
        key.key_code = bsp::KeyCodes::NumericKey0;
        REQUIRE(translator.handle(key, "English_upper") == 32);
    }
}

TEST_CASE("Parsing numeric keyboard")
{
    vfs.Init();

    gui::KeyInputMappedTranslation translator;
    RawKey key;

    key.key_code = bsp::KeyCodes::NumericKey1;
    REQUIRE(translator.handle(key, "numeric") == 49);
    key.key_code = bsp::KeyCodes::NumericKey2;
    REQUIRE(translator.handle(key, "numeric") == 50);
    key.key_code = bsp::KeyCodes::NumericKey3;
    REQUIRE(translator.handle(key, "numeric") == 51);
    key.key_code = bsp::KeyCodes::NumericKey4;
    REQUIRE(translator.handle(key, "numeric") == 52);
    key.key_code = bsp::KeyCodes::NumericKey5;
    REQUIRE(translator.handle(key, "numeric") == 53);
    key.key_code = bsp::KeyCodes::NumericKey6;
    REQUIRE(translator.handle(key, "numeric") == 54);
    key.key_code = bsp::KeyCodes::NumericKey7;
    REQUIRE(translator.handle(key, "numeric") == 55);
    key.key_code = bsp::KeyCodes::NumericKey8;
    REQUIRE(translator.handle(key, "numeric") == 56);
    key.key_code = bsp::KeyCodes::NumericKey9;
    REQUIRE(translator.handle(key, "numeric") == 57);
    key.key_code = bsp::KeyCodes::NumericKey0;
    REQUIRE(translator.handle(key, "numeric") == 48);
}

TEST_CASE("Getting charKey after clicking button twice")
{
    vfs.Init();

    gui::KeyInputMappedTranslation translator;
    gui::KeyBaseTranslation baseTranslation;
    RawKey key;

    key.key_code                   = bsp::KeyCodes::NumericKey2;
    key.state                      = RawKey::State::Released;
    baseTranslation.prev_key_press = key;
    translator.handle(key, "English_lower");
    REQUIRE(translator.handle(key, "English_lower") == 98);
}
