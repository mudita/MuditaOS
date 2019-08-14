 /*
 *  @file bsp_keyboard.c
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 25 oct 2018
 *  @brief Keyboard BSP implementation
 *  @copyright Copyright (C) 2018 mudita.com
 *  @details
 */

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "timers.h"

#include "../board.h"
#include "fsl_common.h"
#include "TCA8418.h"
#include "bsp/common.hpp"
#include "bsp/keyboard/key_codes.hpp"

#include "bsp/BoardDefinitions.hpp"

#include "drivers/DriverInterface.hpp"
#include "drivers/i2c/DriverI2C.hpp"

namespace bsp {

    using namespace drivers;

    static std::shared_ptr<drivers::DriverI2C> i2c;
    static drivers::I2CAddress i2cAddr = {.deviceAddress=TCA8418_I2C_ADDRESS,.subAddressSize=TCA8418_I2C_ADDRESS_SIZE};

#define KEYBOARD_CONTACT_OSCILLATION_TIMEOUT_MS    20

    static TimerHandle_t s_right_functional_check_timer = NULL;
    static volatile uint8_t s_rigth_functional_last_state = 0;

    static void s_bsp_keyboard_right_functional_check_timer_cb(TimerHandle_t timer);

    static xQueueHandle qHandleIrq = NULL;

    status_t keyboard_Init(xQueueHandle qHandle){

        i2c = DriverInterface<DriverI2C>::Create(static_cast<I2CInstances >(BoardDefinitions::KEYBOARD_I2C),
                                                 DriverI2CParams{.baudrate=static_cast<uint32_t >(BoardDefinitions::KEYBOARD_I2C_BAUDRATE)});

    	qHandleIrq = qHandle;
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
        i2cAddr.subAddress = REG_KP_GPIO1;
        i2c->Write(i2cAddr, (uint8_t *) &reg, 1);
        reg = reg >> 8;
        i2cAddr.subAddress = REG_KP_GPIO2;
        i2c->Write(i2cAddr, (uint8_t *) &reg, 1);
        reg = reg >> 16;
        i2cAddr.subAddress = REG_KP_GPIO3;
        i2c->Write(i2cAddr, (uint8_t *) &reg, 1);

        /* Enable column debouncing */
        i2cAddr.subAddress = REG_DEBOUNCE_DIS1;
        i2c->Write(i2cAddr, (uint8_t *) &reg, 1);
        reg = reg >> 8;
        i2cAddr.subAddress = REG_DEBOUNCE_DIS2;
        i2c->Write(i2cAddr, (uint8_t *) &reg, 1);
        reg = reg >> 16;
        i2cAddr.subAddress = REG_DEBOUNCE_DIS3;
        i2c->Write(i2cAddr, (uint8_t *) &reg, 1);

        if (error != kStatus_Success) {
            return error;
        }

        reg = CFG_INT_CFG | CFG_OVR_FLOW_IEN | CFG_KE_IEN;
        // Enable interrupts
        i2cAddr.subAddress = REG_CFG;
        i2c->Write(i2cAddr, (uint8_t *) &reg, 1);


        // Clear keys FIFO, IRQs
        uint8_t val = 0;
        uint8_t i = 0;

        // Get key pressed/released count
        i2cAddr.subAddress = REG_KEY_LCK_EC;
        i2c->Read(i2cAddr, (uint8_t *) &val, 1);

        uint8_t key_count = val & 0xF;
        for (i = 0; i < key_count; i++) {
            i2cAddr.subAddress = REG_KEY_EVENT_A;
            i2c->Read(i2cAddr, (uint8_t *) &val, 1);
        }

        //Clear all interrupts, even IRQs we didn't check (GPI, CAD, LCK)
        reg = 0xff;
        i2cAddr.subAddress = REG_INT_STAT;
        i2c->Write(i2cAddr, (uint8_t *) &reg, 1);

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


    status_t keyboard_Deinit(void) {

        /* Enable GPIO pin interrupt */
        GPIO_PortDisableInterrupts(BOARD_KEYBOARD_IRQ_GPIO, 1U << BOARD_KEYBOARD_IRQ_GPIO_PIN);
        GPIO_PortDisableInterrupts(BOARD_KEYBOARD_RF_BUTTON_PORT, 1U << BOARD_KEYBOARD_RF_BUTTON_PIN);

        //Clear all interrupts, even IRQs we didn't check (GPI, CAD, LCK)
        uint8_t reg = 0xff;
        i2cAddr.subAddress = REG_INT_STAT;
        i2c->Write(i2cAddr, (uint8_t *) &reg, 1);

        // Keep keyboard controller in reset state
        GPIO_PinWrite(BOARD_KEYBOARD_RESET_GPIO, BOARD_KEYBOARD_RESET_GPIO_PIN, 0);

        //Clear IRQ queue handle
        qHandleIrq = NULL;

        i2c.reset();
        i2cAddr = {};

        return kStatus_Success;
    }


    void keyboard_get_data(const uint8_t& notification, uint8_t& event, uint8_t& code)
    {
    	if (notification & 0x01) {
			uint8_t retval = 0;
			// Read how many key events has been registered
            i2cAddr.subAddress = REG_KEY_LCK_EC;
            i2c->Read(i2cAddr, (uint8_t *) &retval, 1);

			uint8_t events_count = retval & 0xF;
			uint8_t i = 0;
			// Iterate over and parse each event
			for (i = 0; i < events_count; i++) {
				uint8_t key = 0;
				uint8_t rel_pres = 0;

                i2cAddr.subAddress = REG_KEY_EVENT_A;
                i2c->Read(i2cAddr, (uint8_t *) &retval, 1);

				key = retval & 0x7F;
				// rel_pres: 0 - key release, 1 - key press
				rel_pres = (retval & 0x80) >> 7; // key release/pressed is stored on the last bit

				if (rel_pres == 0) {
				} else {
				}
				code = key;
				event = rel_pres;
			}

			//Clear all interrupts
			uint8_t reg = 0xff;

            i2cAddr.subAddress = REG_INT_STAT;
            i2c->Write(i2cAddr, (uint8_t *) &reg, 1);

		}

		if (notification & 0x02) {

			code = static_cast<uint8_t>(bsp::KeyCodes::FnRight);
			event = static_cast<uint8_t>(KeyEvents::Pressed);
		}

		if (notification & 0x04) {

			code = static_cast<uint8_t>(KeyCodes::FnRight);
			event = static_cast<uint8_t>(KeyEvents::Released);
		}
	}

    BaseType_t keyboard_right_functional_IRQHandler(void) {
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


    BaseType_t keyboard_IRQHandler(void) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        if(qHandleIrq != NULL){
        	uint8_t val = 0x01;
        	xQueueSendFromISR(qHandleIrq, &val, &xHigherPriorityTaskWoken );
        }
        return xHigherPriorityTaskWoken;
    }

    static void s_bsp_keyboard_right_functional_check_timer_cb(TimerHandle_t timer) {
        uint8_t right_functional_current_state = GPIO_PinRead(BOARD_KEYBOARD_RF_BUTTON_PORT,
                                                              BOARD_KEYBOARD_RF_BUTTON_PIN);
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xTimerStop(timer, 0);

        // If user pressed button and it is not just the contact oscillation
        if (right_functional_current_state == s_rigth_functional_last_state) {
            if (right_functional_current_state == 0) {
                // RIGHT FUNCTIONAL PRESSED
            	if(qHandleIrq != NULL){
					uint8_t val = 0x02;
					xQueueSendFromISR(qHandleIrq, &val, &xHigherPriorityTaskWoken );
                }
            } else {
            	// RIGHT FUNCTIONAL RELEASED
            	if(qHandleIrq != NULL){
					uint8_t val = 0x04;
					xQueueSendFromISR(qHandleIrq, &val, &xHigherPriorityTaskWoken );
                }
            }
        }

        GPIO_EnableInterrupts(BOARD_KEYBOARD_RF_BUTTON_PORT, 1U << BOARD_KEYBOARD_RF_BUTTON_PIN);
    }
}
