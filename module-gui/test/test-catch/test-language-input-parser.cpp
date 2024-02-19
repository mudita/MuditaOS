// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <Translator.hpp>

namespace
{
    auto quickClick(gui::KeyInputMappedTranslation *translator, RawKey key, const std::string &keymap) -> char32_t
    {
        key.state = RawKey::State::Pressed;
        translator->handle(key, keymap);
        key.state = RawKey::State::Released;
        return static_cast<char32_t>(translator->handle(key, keymap));
    }
} // namespace

TEST_CASE("Test language with no Unicode characters in their names")
{
    SECTION("Parsing English input language")
    {
        SECTION("Getting charKey from lower letters")
        {
            gui::KeyInputMappedTranslation translator;
            RawKey key;

            key.keyCode = bsp::KeyCodes::NumericKey1;
            REQUIRE(translator.handle(key, "English_lower") == '.');
            key.keyCode = bsp::KeyCodes::NumericKey2;
            REQUIRE(translator.handle(key, "English_lower") == 'a');
            key.keyCode = bsp::KeyCodes::NumericKey3;
            REQUIRE(translator.handle(key, "English_lower") == 'd');
            key.keyCode = bsp::KeyCodes::NumericKey4;
            REQUIRE(translator.handle(key, "English_lower") == 'g');
            key.keyCode = bsp::KeyCodes::NumericKey5;
            REQUIRE(translator.handle(key, "English_lower") == 'j');
            key.keyCode = bsp::KeyCodes::NumericKey6;
            REQUIRE(translator.handle(key, "English_lower") == 'm');
            key.keyCode = bsp::KeyCodes::NumericKey7;
            REQUIRE(translator.handle(key, "English_lower") == 'p');
            key.keyCode = bsp::KeyCodes::NumericKey8;
            REQUIRE(translator.handle(key, "English_lower") == 't');
            key.keyCode = bsp::KeyCodes::NumericKey9;
            REQUIRE(translator.handle(key, "English_lower") == 'w');
            key.keyCode = bsp::KeyCodes::NumericKey0;
            REQUIRE(translator.handle(key, "English_lower") == ' ');
        }

        SECTION("Getting charKey from upper letters")
        {
            gui::KeyInputMappedTranslation translator;
            RawKey key;

            key.keyCode = bsp::KeyCodes::NumericKey1;
            REQUIRE(translator.handle(key, "English_upper") == '.');
            key.keyCode = bsp::KeyCodes::NumericKey2;
            REQUIRE(translator.handle(key, "English_upper") == 'A');
            key.keyCode = bsp::KeyCodes::NumericKey3;
            REQUIRE(translator.handle(key, "English_upper") == 'D');
            key.keyCode = bsp::KeyCodes::NumericKey4;
            REQUIRE(translator.handle(key, "English_upper") == 'G');
            key.keyCode = bsp::KeyCodes::NumericKey5;
            REQUIRE(translator.handle(key, "English_upper") == 'J');
            key.keyCode = bsp::KeyCodes::NumericKey6;
            REQUIRE(translator.handle(key, "English_upper") == 'M');
            key.keyCode = bsp::KeyCodes::NumericKey7;
            REQUIRE(translator.handle(key, "English_upper") == 'P');
            key.keyCode = bsp::KeyCodes::NumericKey8;
            REQUIRE(translator.handle(key, "English_upper") == 'T');
            key.keyCode = bsp::KeyCodes::NumericKey9;
            REQUIRE(translator.handle(key, "English_upper") == 'W');
            key.keyCode = bsp::KeyCodes::NumericKey0;
            REQUIRE(translator.handle(key, "English_upper") == ' ');
        }

        SECTION("Getting charKey after clicking button twice")
        {
            gui::KeyInputMappedTranslation translator;
            RawKey key;

            key.keyCode = bsp::KeyCodes::NumericKey2;
            key.state   = RawKey::State::Released;
            translator.handle(key, "English_lower");
            REQUIRE(translator.handle(key, "English_lower") == 'b');
        }
    }
}

TEST_CASE("Test languages containing Unicode characters in their names")
{
    SECTION("Parsing French input language")
    {
        gui::KeyInputMappedTranslation translator;
        RawKey key;

        SECTION("Getting charKey from lower letters")
        {
            SECTION("Numeric key 2")
            {
                key.keyCode = bsp::KeyCodes::NumericKey2;
                REQUIRE(quickClick(&translator, key, "Français_lower") == 'a');
                REQUIRE(quickClick(&translator, key, "Français_lower") == 'b');
                REQUIRE(quickClick(&translator, key, "Français_lower") == 'c');
                REQUIRE(quickClick(&translator, key, "Français_lower") == U'à');
                REQUIRE(quickClick(&translator, key, "Français_lower") == U'â');
                REQUIRE(quickClick(&translator, key, "Français_lower") == U'ç');
            }

            SECTION("Numeric key 3")
            {
                key.keyCode = bsp::KeyCodes::NumericKey3;
                REQUIRE(quickClick(&translator, key, "Français_lower") == 'd');
                REQUIRE(quickClick(&translator, key, "Français_lower") == 'e');
                REQUIRE(quickClick(&translator, key, "Français_lower") == 'f');
                REQUIRE(quickClick(&translator, key, "Français_lower") == U'é');
                REQUIRE(quickClick(&translator, key, "Français_lower") == U'è');
                REQUIRE(quickClick(&translator, key, "Français_lower") == U'ê');
                REQUIRE(quickClick(&translator, key, "Français_lower") == U'ë');
            }
        }

        SECTION("Getting charKey from upper letters")
        {
            SECTION("Numeric key 2")
            {
                key.keyCode = bsp::KeyCodes::NumericKey2;
                REQUIRE(quickClick(&translator, key, "Français_upper") == 'A');
                REQUIRE(quickClick(&translator, key, "Français_upper") == 'B');
                REQUIRE(quickClick(&translator, key, "Français_upper") == 'C');
                REQUIRE(quickClick(&translator, key, "Français_upper") == U'À');
                REQUIRE(quickClick(&translator, key, "Français_upper") == U'Â');
                REQUIRE(quickClick(&translator, key, "Français_upper") == U'Ç');
            }

            SECTION("Numeric key 3")
            {
                key.keyCode = bsp::KeyCodes::NumericKey3;
                REQUIRE(quickClick(&translator, key, "Français_upper") == 'D');
                REQUIRE(quickClick(&translator, key, "Français_upper") == 'E');
                REQUIRE(quickClick(&translator, key, "Français_upper") == 'F');
                REQUIRE(quickClick(&translator, key, "Français_upper") == U'É');
                REQUIRE(quickClick(&translator, key, "Français_upper") == U'È');
                REQUIRE(quickClick(&translator, key, "Français_upper") == U'Ê');
                REQUIRE(quickClick(&translator, key, "Français_upper") == U'Ë');
            }
        }
    }

    SECTION("Parsing Spanish input language")
    {
        gui::KeyInputMappedTranslation translator;
        RawKey key;

        SECTION("Getting charKey from lower letters")
        {
            SECTION("Numeric key 2")
            {
                key.keyCode = bsp::KeyCodes::NumericKey2;
                REQUIRE(quickClick(&translator, key, "Español_lower") == 'a');
                REQUIRE(quickClick(&translator, key, "Español_lower") == 'b');
                REQUIRE(quickClick(&translator, key, "Español_lower") == 'c');
                REQUIRE(quickClick(&translator, key, "Español_lower") == U'á');
            }

            SECTION("Numeric key 3")
            {
                key.keyCode = bsp::KeyCodes::NumericKey3;
                REQUIRE(quickClick(&translator, key, "Español_lower") == 'd');
                REQUIRE(quickClick(&translator, key, "Español_lower") == 'e');
                REQUIRE(quickClick(&translator, key, "Español_lower") == 'f');
                REQUIRE(quickClick(&translator, key, "Español_lower") == U'é');
            }
        }

        SECTION("Getting charKey from upper letters")
        {
            SECTION("Numeric key 2")
            {
                key.keyCode = bsp::KeyCodes::NumericKey2;
                REQUIRE(quickClick(&translator, key, "Español_upper") == 'A');
                REQUIRE(quickClick(&translator, key, "Español_upper") == 'B');
                REQUIRE(quickClick(&translator, key, "Español_upper") == 'C');
                REQUIRE(quickClick(&translator, key, "Español_upper") == U'Á');
            }

            SECTION("Numeric key 3")
            {
                key.keyCode = bsp::KeyCodes::NumericKey3;
                REQUIRE(quickClick(&translator, key, "Español_upper") == 'D');
                REQUIRE(quickClick(&translator, key, "Español_upper") == 'E');
                REQUIRE(quickClick(&translator, key, "Español_upper") == 'F');
                REQUIRE(quickClick(&translator, key, "Español_upper") == U'É');
            }
        }
    }
}

TEST_CASE("Parsing numeric keyboard")
{
    gui::KeyInputMappedTranslation translator;
    RawKey key;

    key.keyCode = bsp::KeyCodes::NumericKey1;
    REQUIRE(translator.handle(key, "numeric") == '1');
    key.keyCode = bsp::KeyCodes::NumericKey2;
    REQUIRE(translator.handle(key, "numeric") == '2');
    key.keyCode = bsp::KeyCodes::NumericKey3;
    REQUIRE(translator.handle(key, "numeric") == '3');
    key.keyCode = bsp::KeyCodes::NumericKey4;
    REQUIRE(translator.handle(key, "numeric") == '4');
    key.keyCode = bsp::KeyCodes::NumericKey5;
    REQUIRE(translator.handle(key, "numeric") == '5');
    key.keyCode = bsp::KeyCodes::NumericKey6;
    REQUIRE(translator.handle(key, "numeric") == '6');
    key.keyCode = bsp::KeyCodes::NumericKey7;
    REQUIRE(translator.handle(key, "numeric") == '7');
    key.keyCode = bsp::KeyCodes::NumericKey8;
    REQUIRE(translator.handle(key, "numeric") == '8');
    key.keyCode = bsp::KeyCodes::NumericKey9;
    REQUIRE(translator.handle(key, "numeric") == '9');
    key.keyCode = bsp::KeyCodes::NumericKey0;
    REQUIRE(translator.handle(key, "numeric") == '0');
}
