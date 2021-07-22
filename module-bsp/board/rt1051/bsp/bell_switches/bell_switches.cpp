// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bell_switches.hpp"

#include <module-utils/Utils.hpp> // for byte conversion functions. it is included first because of magic enum define

#include "bsp/BoardDefinitions.hpp"

#include <fsl_common.h>
#include <timers.h>
#include <map>

using namespace drivers;
using namespace utils;

namespace bsp
{
    namespace bell_switches
    {
        #define CONTACT_OSCILLATION_TIMEOUT_MS 30
        #define CONTACT_OSCILLATION_LATCH_TIMEOUT_MS 300

        enum DEBOUNCE_TIMERS: unsigned int
        {
            Bell_SW_Left = 0,
            Bell_SW_Right,
            Bell_SW_Center,
            Bell_SW_Latch,
            Bell_SW_Wakeup,
            COUNT
        };

        static std::map<BoardDefinitions, uint8_t> notifications
        {
            {BoardDefinitions::BELL_SWITCHES_CENTER, 0x10U},
            {BoardDefinitions::BELL_SWITCHES_LEFT, 0x20U},
            {BoardDefinitions::BELL_SWITCHES_RIGHT, 0x30U},
            {BoardDefinitions::BELL_SWITCHES_LATCH, 0x40U},
            {BoardDefinitions::BELL_WAKEUP, 0x50U}    //disabled
        };

        static std::map<uint8_t, bsp::KeyCodes> keycodes
        {
            {notifications[BoardDefinitions::BELL_SWITCHES_CENTER], bsp::KeyCodes::JoystickEnter},
            {notifications[BoardDefinitions::BELL_SWITCHES_LEFT], bsp::KeyCodes::FnLeft},
            {notifications[BoardDefinitions::BELL_SWITCHES_RIGHT], bsp::KeyCodes::FnRight},
            {notifications[BoardDefinitions::BELL_SWITCHES_LATCH], bsp::KeyCodes::JoystickDown},
            {notifications[BoardDefinitions::BELL_WAKEUP], bsp::KeyCodes::Undefined}    //disabled
        };

        static TimerHandle_t debounce_timer[DEBOUNCE_TIMERS::COUNT]   = {NULL};
        static volatile uint8_t debounce_last_state[DEBOUNCE_TIMERS::COUNT] = {0};

        static xQueueHandle qHandleIrq = NULL;
        std::shared_ptr<DriverGPIO> gpio_sw;
        std::shared_ptr<DriverGPIO> gpio_wakeup;
        
        void debounce_timer_callback(TimerHandle_t timer, uint8_t last_state, std::shared_ptr<drivers::DriverGPIO> gpio, BoardDefinitions pin)
        {
            uint8_t current_state = gpio->ReadPin(static_cast<uint32_t>(pin));
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xTimerStop(timer, 0);

            // If user pressed button and it is not just the contact oscillation
            if (current_state == last_state) {
                if (current_state == 0) {
                    // PRESSED
                    if (qHandleIrq != NULL) {
                        uint8_t val = 0x02 | notifications[pin];
                        xQueueSendFromISR(qHandleIrq, &val, &xHigherPriorityTaskWoken);
                    }
                }
                else {
                    // RELEASED
                    if (qHandleIrq != NULL) {
                        uint8_t val = 0x04 | notifications[pin];
                        xQueueSendFromISR(qHandleIrq, &val, &xHigherPriorityTaskWoken);
                    }
                }
            }

            gpio->EnableInterrupt(1U << static_cast<uint32_t>(pin));
        }

        static void Left_timer_cb(TimerHandle_t timer)
        {
            debounce_timer_callback(timer, debounce_last_state[DEBOUNCE_TIMERS::Bell_SW_Left], gpio_sw, BoardDefinitions::BELL_SWITCHES_LEFT);
        }
        static void Right_timer_cb(TimerHandle_t timer)
        {    
            debounce_timer_callback(timer, debounce_last_state[DEBOUNCE_TIMERS::Bell_SW_Right], gpio_sw, BoardDefinitions::BELL_SWITCHES_RIGHT);
        }
        static void Center_timer_cb(TimerHandle_t timer)
        {
            debounce_timer_callback(timer, debounce_last_state[DEBOUNCE_TIMERS::Bell_SW_Center], gpio_sw, BoardDefinitions::BELL_SWITCHES_CENTER);
        }
        static void Latch_timer_cb(TimerHandle_t timer)
        {
            debounce_timer_callback(timer, debounce_last_state[DEBOUNCE_TIMERS::Bell_SW_Latch], gpio_sw, BoardDefinitions::BELL_SWITCHES_LATCH);
        }
        static void Wakeup_timer_cb(TimerHandle_t timer)
        {
            debounce_timer_callback(timer, debounce_last_state[DEBOUNCE_TIMERS::Bell_SW_Wakeup], gpio_wakeup, BoardDefinitions::BELL_WAKEUP);
        }

        int32_t init(xQueueHandle qHandle)
        {
            qHandleIrq = qHandle;

            // Switches
            gpio_sw = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::BELL_SWITCHES_GPIO), DriverGPIOParams{});
            //wakeup
            gpio_wakeup = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::BELL_WAKEUP_GPIO), DriverGPIOParams{});

            // Center switch
            gpio_sw->ClearPortInterrupts(1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_CENTER));
            gpio_sw->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Input,
                                              .irqMode  = DriverGPIOPinParams::InterruptMode::IntRisingOrFallingEdge,
                                              .defLogic = 0,
                                              .pin      = static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_CENTER)});

            // Left switch
            gpio_sw->ClearPortInterrupts(1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_LEFT));
            gpio_sw->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Input,
                                              .irqMode  = DriverGPIOPinParams::InterruptMode::IntRisingOrFallingEdge,
                                              .defLogic = 0,
                                              .pin      = static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_LEFT)});

            // Right switch
            gpio_sw->ClearPortInterrupts(1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_RIGHT));
            gpio_sw->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Input,
                                              .irqMode  = DriverGPIOPinParams::InterruptMode::IntRisingOrFallingEdge,
                                              .defLogic = 0,
                                              .pin      = static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_RIGHT)});

            // Latch
            gpio_sw->ClearPortInterrupts(1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_LATCH));
            gpio_sw->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Input,
                                              .irqMode  = DriverGPIOPinParams::InterruptMode::IntRisingOrFallingEdge,
                                              .defLogic = 0,
                                              .pin      = static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_LATCH)});       

            // Wakeup
            gpio_wakeup->ClearPortInterrupts(1 << static_cast<uint32_t>(BoardDefinitions::BELL_WAKEUP));
            gpio_wakeup->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Input,
                                              .irqMode  = DriverGPIOPinParams::InterruptMode::IntFallingEdge,
                                              .defLogic = 0,
                                              .pin      = static_cast<uint32_t>(BoardDefinitions::BELL_WAKEUP)});  

        

            if (debounce_timer[DEBOUNCE_TIMERS::Bell_SW_Left] == NULL) {
                debounce_timer[DEBOUNCE_TIMERS::Bell_SW_Left] = xTimerCreate("Bell_SW_Left",
                                                          pdMS_TO_TICKS(CONTACT_OSCILLATION_TIMEOUT_MS),
                                                          false,
                                                          NULL,
                                                          Left_timer_cb);
            }
            if (debounce_timer[DEBOUNCE_TIMERS::Bell_SW_Left] == NULL) {
                return kStatus_Fail;
            }
            if (debounce_timer[DEBOUNCE_TIMERS::Bell_SW_Right] == NULL) {
                debounce_timer[DEBOUNCE_TIMERS::Bell_SW_Right] = xTimerCreate("Bell_SW_Right",
                                                          pdMS_TO_TICKS(CONTACT_OSCILLATION_TIMEOUT_MS),
                                                          false,
                                                          NULL,
                                                          Right_timer_cb);
            }
            if (debounce_timer[DEBOUNCE_TIMERS::Bell_SW_Right] == NULL) {
                return kStatus_Fail;
            }
            if (debounce_timer[DEBOUNCE_TIMERS::Bell_SW_Center] == NULL) {
                debounce_timer[DEBOUNCE_TIMERS::Bell_SW_Center] = xTimerCreate("Bell_SW_Center",
                                                          pdMS_TO_TICKS(CONTACT_OSCILLATION_TIMEOUT_MS),
                                                          false,
                                                          NULL,
                                                          Center_timer_cb);
            }
            if (debounce_timer[DEBOUNCE_TIMERS::Bell_SW_Center] == NULL) {
                return kStatus_Fail;
            }
            if (debounce_timer[DEBOUNCE_TIMERS::Bell_SW_Latch] == NULL) {
                debounce_timer[DEBOUNCE_TIMERS::Bell_SW_Latch] = xTimerCreate("Bell_SW_Latch",
                                                          pdMS_TO_TICKS(CONTACT_OSCILLATION_LATCH_TIMEOUT_MS),
                                                          false,
                                                          NULL,
                                                          Latch_timer_cb);
            }
            if (debounce_timer[DEBOUNCE_TIMERS::Bell_SW_Latch] == NULL) {
                return kStatus_Fail;
            }
            if (debounce_timer[DEBOUNCE_TIMERS::Bell_SW_Wakeup] == NULL) {
                debounce_timer[DEBOUNCE_TIMERS::Bell_SW_Wakeup] = xTimerCreate("Bell_SW_Wakeup",
                                                          pdMS_TO_TICKS(CONTACT_OSCILLATION_TIMEOUT_MS),
                                                          false,
                                                          NULL,
                                                          Wakeup_timer_cb);
            }
            if (debounce_timer[DEBOUNCE_TIMERS::Bell_SW_Wakeup] == NULL) {
                return kStatus_Fail;
            }   

            enableIRQ();           

            return kStatus_Success;
        }

        void deinit()
        {
            qHandleIrq = nullptr;
            disableIRQ();
        }

        BaseType_t bell_switches_Cent_Right_Latch_IRQHandler(uint32_t mask)
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            gpio_sw->ClearPortInterrupts(mask);
            BoardDefinitions gpio_index = BoardDefinitions::BELL_SWITCHES_CENTER;
            unsigned int debounce_index = DEBOUNCE_TIMERS::COUNT;

            if (mask & (1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_CENTER)))
            {
                gpio_index = BoardDefinitions::BELL_SWITCHES_CENTER;
                debounce_index = DEBOUNCE_TIMERS::Bell_SW_Center;
            }
            if (mask & (1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_RIGHT)))
            {
                gpio_index = BoardDefinitions::BELL_SWITCHES_RIGHT;
                debounce_index = DEBOUNCE_TIMERS::Bell_SW_Right;
            }
            if (mask & (1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_LATCH)))
            {
                gpio_index = BoardDefinitions::BELL_SWITCHES_LATCH;
                debounce_index = DEBOUNCE_TIMERS::Bell_SW_Latch;
            }

            if (debounce_timer[debounce_index] != NULL) {
                gpio_sw->DisableInterrupt(1U << static_cast<uint32_t>(gpio_index));

                if (gpio_sw->ReadPin(static_cast<uint32_t>(gpio_index)) == 0) {
                    debounce_last_state[debounce_index] = 0;
                }
                else {
                    debounce_last_state[debounce_index] = 1;
                }

                xTimerResetFromISR(debounce_timer[debounce_index], &xHigherPriorityTaskWoken);
            }

            return xHigherPriorityTaskWoken;
        }

        BaseType_t bell_switches_Left_IRQHandler() 
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            gpio_sw->ClearPortInterrupts(1U << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_LEFT));

            if (debounce_timer[DEBOUNCE_TIMERS::Bell_SW_Left] != NULL) {
                gpio_sw->DisableInterrupt(1U << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_LEFT));

                if (gpio_sw->ReadPin(static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_LEFT)) == 0) {
                    debounce_last_state[DEBOUNCE_TIMERS::Bell_SW_Left] = 0;
                }
                else {
                    debounce_last_state[DEBOUNCE_TIMERS::Bell_SW_Left] = 1;
                }

                xTimerResetFromISR(debounce_timer[DEBOUNCE_TIMERS::Bell_SW_Left], &xHigherPriorityTaskWoken);
            }

            return xHigherPriorityTaskWoken;
        }

        BaseType_t bell_wakeup_IRQHandler()
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            gpio_wakeup->ClearPortInterrupts(1U << static_cast<uint32_t>(BoardDefinitions::BELL_WAKEUP));

            if (debounce_timer[DEBOUNCE_TIMERS::Bell_SW_Wakeup] != NULL) {
                gpio_wakeup->DisableInterrupt(1U << static_cast<uint32_t>(BoardDefinitions::BELL_WAKEUP));

                if (gpio_wakeup->ReadPin(static_cast<uint32_t>(BoardDefinitions::BELL_WAKEUP)) == 0) {
                    debounce_last_state[DEBOUNCE_TIMERS::Bell_SW_Wakeup] = 0;
                }
                else {
                    debounce_last_state[DEBOUNCE_TIMERS::Bell_SW_Wakeup] = 1;
                }

                xTimerResetFromISR(debounce_timer[DEBOUNCE_TIMERS::Bell_SW_Wakeup], &xHigherPriorityTaskWoken);
            }

            return xHigherPriorityTaskWoken;
        }

        void enableIRQ()
        {
            gpio_sw->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_CENTER));
            gpio_sw->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_LEFT));
            gpio_sw->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_RIGHT));
            gpio_sw->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_LATCH));
            /* Disable wakeup as it's not yet used in the code and may lead to false event interpretation */
            //gpio_wakeup->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_WAKEUP));
        }

        void disableIRQ()
        {
            gpio_sw->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_CENTER));
            gpio_sw->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_LEFT));
            gpio_sw->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_RIGHT));
            gpio_sw->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_LATCH));
            gpio_wakeup->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_WAKEUP));
        }

        void bell_get_key(const uint8_t &notification, uint8_t &event, uint8_t &code)
        {
            code  = static_cast<uint8_t>(keycodes[notification & 0xF0]);
            
            if (notification & 0x02) {
                event = static_cast<uint8_t>(KeyEvents::Pressed);
            }

            if (notification & 0x04) {
                event = static_cast<uint8_t>(KeyEvents::Released);
            }
        }
        
    } // namespace bell_switches
} // namespace bsp
