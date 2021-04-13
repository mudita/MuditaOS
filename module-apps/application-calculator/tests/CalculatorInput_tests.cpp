// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include "application-calculator/data/CalculatorInputProcessorText.hpp"
#include <gui/widgets/Text.hpp>
#include <i18n/i18n.hpp>
#include <cstring>

SCENARIO("Input Processor tests")
{
    using bsp::KeyCodes;

    const auto FractionKey       = KeyCodes::FnLeft;
    const auto PlusKey           = KeyCodes::JoystickUp;
    const auto MinusKey          = KeyCodes::JoystickDown;
    const auto MultiplicationKey = KeyCodes::JoystickLeft;
    const auto DivisionKey       = KeyCodes::JoystickRight;

    GIVEN("An empty input")
    {
        utils::setDisplayLanguage("English");

        auto inputField = gui::Text{};
        auto processor  = calc::InputProcessorText{gsl::make_strict_not_null(&inputField)};

        inputField.setInputMode(new InputMode({InputMode::digit}));
        inputField.inputCallback = [&processor](gui::Item &, const gui::InputEvent &event) {
            return processor.handle(event);
        };

        auto shortPressEvent = [](KeyCodes code) -> gui::InputEvent {
            return gui::InputEvent{RawKey{RawKey::State::Released, code, 0, 0},
                                   gui::InputEvent::State::keyReleasedShort,
                                   static_cast<gui::KeyCode>(code)};
        };

        auto longPressEvent = [](KeyCodes code) -> gui::InputEvent {
            return gui::InputEvent{RawKey{RawKey::State::Released, code, 0, 0},
                                   gui::InputEvent::State::keyReleasedLong,
                                   static_cast<gui::KeyCode>(code)};
        };

        auto passEvent = [&](const gui::InputEvent &event) { inputField.onInput(event); };

        auto passShortKeyPress = [&](KeyCodes code) { passEvent(shortPressEvent(code)); };

        auto passLongKeyPress = [&](KeyCodes code) { passEvent(longPressEvent(code)); };

        auto passShortKeyPresses = [&](const std::vector<KeyCodes> &codes) {
            for (const auto &code : codes)
                passShortKeyPress(code);
        };

        auto passLongKeyPresses = [&](const std::vector<KeyCodes> &codes) {
            for (const auto &code : codes)
                passLongKeyPress(code);
        };

        THEN("The text is empty")
        {
            REQUIRE(inputField.isEmpty());

            AND_WHEN("We press enter")
            {
                passShortKeyPress(KeyCodes::JoystickEnter);

                THEN("Nothing happens")
                {
                    REQUIRE(inputField.isEmpty());
                }
            }
        }

        WHEN("We press 1234")
        {
            passShortKeyPresses(
                {KeyCodes::NumericKey1, KeyCodes::NumericKey2, KeyCodes::NumericKey3, KeyCodes::NumericKey4});

            THEN("Text is 1234")
            {
                REQUIRE(inputField.getText() == "1234");
            }

            AND_WHEN("We use the clear method")
            {
                processor.clear();

                THEN("The input is cleared")
                {
                    REQUIRE(inputField.isEmpty());
                }
            }

            AND_WHEN("We use the # key")
            {
                passShortKeyPresses({3, KeyCodes::NumericKeyPnd});

                THEN("The input is deleted")
                {
                    REQUIRE(inputField.getText() == "1");
                }
            }

            AND_WHEN("We long press #")
            {
                passLongKeyPress(KeyCodes::NumericKeyPnd);

                THEN("The input is cleared")
                {
                    REQUIRE(inputField.isEmpty());
                }
            }
        }

        WHEN("We enter leading zeros")
        {
            passShortKeyPresses({KeyCodes::NumericKey0,
                                 KeyCodes::NumericKey0,
                                 KeyCodes::NumericKey1,
                                 KeyCodes::NumericKey2,
                                 KeyCodes::NumericKey3});

            THEN("They are ignored")
            {
                REQUIRE(inputField.getText() == "123");
            }
        }

        WHEN("We start input with the fraction key")
        {
            passShortKeyPresses({FractionKey, KeyCodes::NumericKey1, KeyCodes::NumericKey2, KeyCodes::NumericKey3});

            THEN("Is is ignored")
            {
                REQUIRE(inputField.getText() == "123");
            }
        }

        WHEN("We use the fraction key")
        {
            passShortKeyPresses({KeyCodes::NumericKey0,
                                 FractionKey,
                                 KeyCodes::NumericKey1,
                                 KeyCodes::NumericKey2,
                                 KeyCodes::NumericKey3});

            THEN("A fraction is entered")
            {
                REQUIRE(inputField.getText() == "0.123");
            }

            AND_WHEN("We use the fraction key again")
            {
                passShortKeyPresses({FractionKey, KeyCodes::NumericKey4, FractionKey, KeyCodes::NumericKey5});

                THEN("It is ignored")
                {
                    REQUIRE(inputField.getText() == "0.12345");
                }
            }

            AND_WHEN("We try to enter more than 6 decimals")
            {
                passShortKeyPresses({KeyCodes::NumericKey4,
                                     KeyCodes::NumericKey5,
                                     KeyCodes::NumericKey6,
                                     KeyCodes::NumericKey7,
                                     KeyCodes::NumericKey8,
                                     KeyCodes::NumericKey9});

                THEN("The input is truncated to 6 decimals")
                {
                    REQUIRE(inputField.getText() == "0.123456");
                }
            }
        }

        WHEN("We enter zero")
        {
            passShortKeyPress(KeyCodes::NumericKey0);

            THEN("Zero is shown")
            {
                REQUIRE(inputField.getText() == "0");
            }

            AND_WHEN("We enter following zeros")
            {
                passShortKeyPresses({5, KeyCodes::NumericKey0});

                THEN("They are ignored")
                {
                    REQUIRE(inputField.getText() == "0");
                }
            }

            AND_WHEN("We press enter")
            {
                passShortKeyPress(KeyCodes::JoystickEnter);

                THEN("Nothing happens")
                {
                    REQUIRE(inputField.getText() == "0");
                }
            }
        }

        WHEN("We enter a negative number")
        {
            passShortKeyPresses({MinusKey, KeyCodes::NumericKey2, KeyCodes::NumericKey3});

            THEN("It is shown")
            {
                REQUIRE(inputField.getText() == "-23");
            }
        }

        WHEN("We enter a number")
        {
            passShortKeyPresses({KeyCodes::NumericKey1, KeyCodes::NumericKey2, KeyCodes::NumericKey3});

            AND_WHEN("We long press joy buttons")
            {
                passLongKeyPresses({KeyCodes::JoystickDown,
                                    KeyCodes::JoystickUp,
                                    KeyCodes::JoystickLeft,
                                    KeyCodes::JoystickRight,
                                    KeyCodes::JoystickEnter});

                THEN("They are ignored")
                {
                    REQUIRE(inputField.getText() == "123");
                }
            }

            AND_WHEN("We press plus")
            {
                passShortKeyPress(PlusKey);

                THEN("Input ends with plus sign")
                {
                    REQUIRE(inputField.getText() == "123+");
                }

                AND_WHEN("We enter a number")
                {
                    passShortKeyPresses({KeyCodes::NumericKey4, KeyCodes::NumericKey5, KeyCodes::NumericKey6});

                    THEN("It is shown")
                    {
                        REQUIRE(inputField.getText() == "123+456");
                    }
                }

                AND_WHEN("We enter a negative numer")
                {
                    passShortKeyPresses({MinusKey, KeyCodes::NumericKey5, KeyCodes::NumericKey6});

                    THEN("It is shown")
                    {
                        REQUIRE(inputField.getText() == "123+-56");
                    }
                }

                AND_WHEN("We press the fraction key")
                {
                    passShortKeyPresses({FractionKey, KeyCodes::NumericKey5, KeyCodes::NumericKey6});

                    THEN("It replaces plus")
                    {
                        REQUIRE(inputField.getText() == "123.56");
                    }
                }

                AND_WHEN("We press a different operation key")
                {
                    passShortKeyPress(MultiplicationKey);

                    THEN("It replaces plus")
                    {
                        REQUIRE(inputField.getText() == "123×");
                    }
                }

                AND_WHEN("We enter a proper fraction")
                {
                    passShortKeyPresses(
                        {KeyCodes::NumericKey4, FractionKey, KeyCodes::NumericKey5, KeyCodes::NumericKey6});

                    THEN("It is shown")
                    {
                        REQUIRE(inputField.getText() == "123+4.56");
                    }

                    AND_WHEN("We press the fraction key again")
                    {
                        passShortKeyPresses(
                            {KeyCodes::NumericKey4, FractionKey, KeyCodes::NumericKey5, KeyCodes::NumericKey6});

                        THEN("It is ignored")
                        {
                            REQUIRE(inputField.getText() == "123+4.56456");
                        }
                    }

                    AND_WHEN("We try to enter more than 6 decimals")
                    {
                        passShortKeyPresses({KeyCodes::NumericKey4,
                                             KeyCodes::NumericKey5,
                                             KeyCodes::NumericKey6,
                                             KeyCodes::NumericKey7,
                                             KeyCodes::NumericKey8,
                                             KeyCodes::NumericKey9});

                        THEN("The input is truncated to 6 decimals")
                        {
                            REQUIRE(inputField.getText() == "123+4.564567");
                        }
                    }
                }
            }

            AND_WHEN("We press minus")
            {
                passShortKeyPress(MinusKey);

                THEN("Input ends with minus sign")
                {
                    REQUIRE(inputField.getText() == "123-");
                }
            }

            AND_WHEN("We press multiplication")
            {
                passShortKeyPress(MultiplicationKey);

                THEN("Input ends with multiplication sign")
                {
                    REQUIRE(inputField.getText() == "123×");
                }
            }

            AND_WHEN("We press division")
            {
                passShortKeyPress(DivisionKey);

                THEN("Input ends with division sign")
                {
                    REQUIRE(inputField.getText() == "123÷");
                }
            }
        }
    }
}
