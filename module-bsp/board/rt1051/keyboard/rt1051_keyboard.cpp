/*
 *  @file bsp_keyboard.c
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 25 oct 2018
 *  @brief Keyboard BSP implementation
 *  @copyright Copyright (C) 2018 mudita.com
 *  @details
 */

#include "rt1051_keyboard.hpp"

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "../board.h"
#include "fsl_common.h"
#include "../common/i2c.h"
#include "TCA8418.h"

namespace bsp {

#define KEYBOARD_CONTACT_OSCILLATION_TIMEOUT_MS    20

    static std::function<void(KeyEvents event,KeyCodes code)> user_event_callback = NULL;
    static TaskHandle_t keyboard_worker_handle = NULL;
    static TimerHandle_t s_right_functional_check_timer = NULL;
    static volatile uint8_t s_rigth_functional_last_state = 0;

    static void s_bsp_keyboard_right_functional_check_timer_cb(TimerHandle_t timer);

    static void bsp_keyboard_worker(void *pvp);


    status_t rt1501_keyboard_Init(WorkerKbd* worker){
        //assert(event);
        // Store user specified event callback
        //user_event_callback = event;

        /* Define the init structure for the input RF key pin*/
        gpio_pin_config_t right_functional_pin_config = {kGPIO_DigitalInput, 0, kGPIO_IntRisingOrFallingEdge};
        /* Init input KEYBOARD IRQ. */
        GPIO_PinInit(BOARD_KEYBOARD_RF_BUTTON_PORT, BOARD_KEYBOARD_RF_BUTTON_PIN, &right_functional_pin_config);
        /* Define the init structure for the output RST pin*/
        gpio_pin_config_t rstpin_config = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};
        /* Init output CS GPIO. */
        GPIO_PinInit(BOARD_KEYBOARD_RESET_GPIO, BOARD_KEYBOARD_RESET_GPIO_PIN, &rstpin_config);
        /* Define the init structure for the input IRQ pin*/
        gpio_pin_config_t irqpin_config = {kGPIO_DigitalInput, 0, kGPIO_IntFallingEdge};
        /* Init input KEYBOARD IRQ. */
        GPIO_PinInit(BOARD_KEYBOARD_IRQ_GPIO, BOARD_KEYBOARD_IRQ_GPIO_PIN, &irqpin_config);

        //
        //TODO change magic value to enum
        std::vector<xQueueHandle> queues = worker->getQueues();
        xQueueHandle qhandle = queues[2];
        if (xTaskCreate(bsp_keyboard_worker, "keyboard", 512, qhandle, 0, &keyboard_worker_handle) != pdPASS) {
            return kStatus_Fail;
        }

        // Reset keyboard controller
        GPIO_PinWrite(BOARD_KEYBOARD_RESET_GPIO, BOARD_KEYBOARD_RESET_GPIO_PIN, 0);
        vTaskDelay(100);
        GPIO_PinWrite(BOARD_KEYBOARD_RESET_GPIO, BOARD_KEYBOARD_RESET_GPIO_PIN, 1);
        vTaskDelay(100);

        uint32_t reg = 0;
        status_t error = 0;

        /* Assemble a mask for row and column registers */
        reg = ~(~0 << TCA8418_ROWS_COUNT);
        reg += (~(~0 << TCA8418_COL_COUNT)) << 8;

        /* Set registers to keypad mode */
        error |= bsp_i2c_Send(BOARD_GetI2CInstance(), TCA8418_I2C_ADDRESS, REG_KP_GPIO1, (uint8_t *) &reg, 1);
        reg = reg >> 8;
        error |= bsp_i2c_Send(BOARD_GetI2CInstance(), TCA8418_I2C_ADDRESS, REG_KP_GPIO2, (uint8_t *) &reg, 1);
        reg = reg >> 16;
        error |= bsp_i2c_Send(BOARD_GetI2CInstance(), TCA8418_I2C_ADDRESS, REG_KP_GPIO3, (uint8_t *) &reg, 1);

        /* Enable column debouncing */
        error |= bsp_i2c_Send(BOARD_GetI2CInstance(), TCA8418_I2C_ADDRESS, REG_DEBOUNCE_DIS1, (uint8_t *) &reg, 1);
        reg = reg >> 8;
        error |= bsp_i2c_Send(BOARD_GetI2CInstance(), TCA8418_I2C_ADDRESS, REG_DEBOUNCE_DIS2, (uint8_t *) &reg, 1);
        reg = reg >> 16;
        error |= bsp_i2c_Send(BOARD_GetI2CInstance(), TCA8418_I2C_ADDRESS, REG_DEBOUNCE_DIS3, (uint8_t *) &reg, 1);

        if (error != kStatus_Success) {
            return error;
        }

        reg = CFG_INT_CFG | CFG_OVR_FLOW_IEN | CFG_KE_IEN;
        // Enable interrupts
        error = bsp_i2c_Send(BOARD_GetI2CInstance(), TCA8418_I2C_ADDRESS, REG_CFG, (uint8_t *) &reg, 1);


        // Clear keys FIFO, IRQs
        uint8_t val = 0;
        uint8_t i = 0;

        // Get key pressed/released count
        bsp_i2c_Receive(BOARD_GetI2CInstance(), TCA8418_I2C_ADDRESS, REG_KEY_LCK_EC, (uint8_t *) &val, 1);

        uint8_t key_count = val & 0xF;
        for (i = 0; i < key_count; i++) {
            bsp_i2c_Receive(BOARD_GetI2CInstance(), TCA8418_I2C_ADDRESS, REG_KEY_EVENT_A, (uint8_t *) &val, 1);
        }

        //Clear all interrupts, even IRQs we didn't check (GPI, CAD, LCK)
        reg = 0xff;
        bsp_i2c_Send(BOARD_GetI2CInstance(), TCA8418_I2C_ADDRESS, REG_INT_STAT, (uint8_t *) &reg, 1);

        if (s_right_functional_check_timer == NULL) {
            s_right_functional_check_timer = xTimerCreate(
                    "RfKeyTim",
                    pdMS_TO_TICKS(KEYBOARD_CONTACT_OSCILLATION_TIMEOUT_MS),
                    false,
                    NULL,
                    s_bsp_keyboard_right_functional_check_timer_cb
            );
            if (s_right_functional_check_timer == NULL) {
                return kStatus_Fail;
            }
        }

        /* Enable GPIO pin interrupt */
        GPIO_PortEnableInterrupts(BOARD_KEYBOARD_RF_BUTTON_PORT, 1U << BOARD_KEYBOARD_RF_BUTTON_PIN);
        GPIO_PortEnableInterrupts(BOARD_KEYBOARD_IRQ_GPIO, 1U << BOARD_KEYBOARD_IRQ_GPIO_PIN);

        return kStatus_Success;
    }


    status_t rt1501_keyboard_Deinit(void) {
        user_event_callback = NULL;
        vTaskDelete(keyboard_worker_handle);

        /* Enable GPIO pin interrupt */
        GPIO_PortDisableInterrupts(BOARD_KEYBOARD_IRQ_GPIO, 1U << BOARD_KEYBOARD_IRQ_GPIO_PIN);
        GPIO_PortDisableInterrupts(BOARD_KEYBOARD_RF_BUTTON_PORT, 1U << BOARD_KEYBOARD_RF_BUTTON_PIN);

        //Clear all interrupts, even IRQs we didn't check (GPI, CAD, LCK)
        uint8_t reg = 0xff;
        bsp_i2c_Send(BOARD_GetI2CInstance(), TCA8418_I2C_ADDRESS, REG_INT_STAT, (uint8_t *) &reg, 1);

        // Keep keyboard controller in reset state
        GPIO_PinWrite(BOARD_KEYBOARD_RESET_GPIO, BOARD_KEYBOARD_RESET_GPIO_PIN, 0);

        return kStatus_Success;
    }

    static void bsp_keyboard_worker(void *pvp) {
        uint32_t ulNotificationValue = 0;

        xQueueHandle qhandle = reinterpret_cast<xQueueHandle>(pvp);


        while (1) {
            xTaskNotifyWait(0xFFFFFFFF, 0xFFFFFFFF, &ulNotificationValue, portMAX_DELAY);

            if (ulNotificationValue & 0x01) {
                uint8_t retval = 0;
                // Read how many key events has been registered
                bsp_i2c_Receive(BOARD_GetI2CInstance(), TCA8418_I2C_ADDRESS, REG_KEY_LCK_EC, (uint8_t *) &retval,
                                1);

                uint8_t events_count = retval & 0xF;
                uint8_t i = 0;
                // Iterate over and parse each event
                for (i = 0; i < events_count; i++) {
                    uint8_t key = 0;
                    uint8_t rel_pres = 0;
                    bsp_i2c_Receive(BOARD_GetI2CInstance(), TCA8418_I2C_ADDRESS, REG_KEY_EVENT_A,
                                    (uint8_t *) &retval,
                                    1);

                    key = retval & 0x7F;
                    // rel_pres: 0 - key release, 1 - key press
                    rel_pres = (retval & 0x80) >> 7; // key release/pressed is stored on the last bit

                    if (rel_pres == 0) {
                    } else {
                    }
                    KeyState keyState;
                    keyState.code = static_cast<KeyCodes >(key);
                    keyState.event = static_cast<KeyEvents >(rel_pres);
                    xQueueSend(qhandle, &keyState, 100);
                }

                //Clear all interrupts
                uint8_t reg = 0xff;

                bsp_i2c_Send(BOARD_GetI2CInstance(), TCA8418_I2C_ADDRESS, REG_INT_STAT, (uint8_t *) &reg, 1);

            }

            if (ulNotificationValue & 0x02) {
            	KeyState keyState;
            	keyState.code = KeyCodes::FnRight;
            	keyState.event = KeyEvents::Pressed;
            	xQueueSend(qhandle, &keyState, 100);
                //user_event_callback(KeyEvents::Pressed, KeyCodes::FnRight);
            }

            if (ulNotificationValue & 0x04) {
            	KeyState keyState;
				keyState.code = KeyCodes::FnRight;
				keyState.event = KeyEvents::Released;
				xQueueSend(qhandle, &keyState, 100);
                //user_event_callback(KeyEvents::Released, KeyCodes::FnRight);
            }
        }
    }

    BaseType_t rt1501_keyboard_right_functional_IRQHandler(void) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        GPIO_ClearPinsInterruptFlags(BOARD_KEYBOARD_RF_BUTTON_PORT, 1U << BOARD_KEYBOARD_RF_BUTTON_PIN);

        if (s_right_functional_check_timer != NULL) {
            GPIO_DisableInterrupts(BOARD_KEYBOARD_RF_BUTTON_PORT, 1U << BOARD_KEYBOARD_RF_BUTTON_PIN);

            if (GPIO_ReadPinInput(BOARD_KEYBOARD_RF_BUTTON_PORT, BOARD_KEYBOARD_RF_BUTTON_PIN) == 0) {
                s_rigth_functional_last_state = 0;
            } else {
                s_rigth_functional_last_state = 1;
            }

            xTimerResetFromISR(s_right_functional_check_timer, &xHigherPriorityTaskWoken);
        }

        return xHigherPriorityTaskWoken;
    }


    BaseType_t rt1501_keyboard_IRQHandler(void) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        if (keyboard_worker_handle) {
            xTaskNotifyFromISR(keyboard_worker_handle, 0x01, eSetBits, &xHigherPriorityTaskWoken);
        }

        return xHigherPriorityTaskWoken;
    }

    static void s_bsp_keyboard_right_functional_check_timer_cb(TimerHandle_t timer) {
        uint8_t right_functional_current_state = GPIO_PinRead(BOARD_KEYBOARD_RF_BUTTON_PORT,
                                                              BOARD_KEYBOARD_RF_BUTTON_PIN);
        xTimerStop(timer, 0);

        // If user pressed button and it is not just the contact oscillation
        if (right_functional_current_state == s_rigth_functional_last_state) {
            if (right_functional_current_state == 0) {
                // RIGHT FUNCTIONAL PRESSED
                if (keyboard_worker_handle) {
                    xTaskNotify(keyboard_worker_handle, 0x02, eSetBits);
                }
            } else {
                // RIGHT FUNCTIONAL PRESSED
                if (keyboard_worker_handle) {
                    xTaskNotify(keyboard_worker_handle, 0x04, eSetBits);
                }
            }
        }

        GPIO_EnableInterrupts(BOARD_KEYBOARD_RF_BUTTON_PORT, 1U << BOARD_KEYBOARD_RF_BUTTON_PIN);
    }
}
