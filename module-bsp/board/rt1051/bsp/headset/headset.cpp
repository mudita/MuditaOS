#include "headset.hpp"

#include "BoardDefinitions.hpp"
#include "DriverI2C.hpp"
#include "DriverGPIO.hpp"
#include "fsl_common.h"

namespace bsp
{
    namespace headset
    {
        using namespace drivers;

        static const uint8_t HEADSET_I2C_ADDR     = 0x3B;
        static const uint8_t HEADSET_INT_REG_ADDR = 0x01;
        static const uint8_t HEADSET_INT_DIS_ADDR = 0x03;
        static const uint8_t HEADSET_DET_RES_ADDR = 0x0B;

        static const uint8_t HEADSET_INT_DIS_INT_DIS = 1 << 3;
        static const uint8_t HEADSET_INT_DIS_INT_ENA = 0 << 3;
        static const uint8_t HEADSET_INT_DIS_ADC_DIS = 1 << 2;
        static const uint8_t HEADSET_INT_DIS_ADC_ENA = 0 << 2;
        static const uint8_t HEADSET_INT_DIS_DC_DIS  = 1 << 1;
        static const uint8_t HEADSET_INT_DIS_DC_ENA  = 0 << 1;
        static const uint8_t HEADSET_INT_DIS_INS_DIS = 1 << 0;
        static const uint8_t HEADSET_INT_DIS_INS_ENA = 0 << 0;

        static std::shared_ptr<drivers::DriverI2C> i2c;
        static drivers::I2CAddress i2cAddr = {.deviceAddress = HEADSET_I2C_ADDR, .subAddressSize = 0x01};
        static std::shared_ptr<DriverGPIO> gpio;

        static xQueueHandle qHandleIrq = NULL;
        static bool HeadsetInserted    = false;

        static void ReadInsertionStatus(void)
        {
            uint32_t reg;

            i2cAddr.subAddress = HEADSET_INT_REG_ADDR;
            i2c->Read(i2cAddr, (uint8_t *)&reg, 1);

            i2cAddr.subAddress = HEADSET_DET_RES_ADDR;
            i2c->Read(i2cAddr, (uint8_t *)&reg, 1);

            if (reg == 0) {
                HeadsetInserted = false;
                LOG_INFO("Headset removed");
            }
            else {
                HeadsetInserted = true;
                LOG_INFO("Headset inserted");
            }
        }

        status_t Init(xQueueHandle qHandle)
        {
            i2c = DriverI2C::Create(
                static_cast<I2CInstances>(BoardDefinitions::HEADSET_I2C),
                DriverI2CParams{.baudrate = static_cast<uint32_t>(BoardDefinitions::HEADSET_I2C_BAUDRATE)});

            gpio = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::HEADSET_GPIO), DriverGPIOParams{});

            qHandleIrq = qHandle;

            gpio->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Input,
                                              .irqMode  = DriverGPIOPinParams::InterruptMode::IntFallingEdge,
                                              .defLogic = 0,
                                              .pin      = static_cast<uint32_t>(BoardDefinitions::HEADSET_IRQ_PIN)});

            // uint32_t reg = 0x06; /* enable INT and Ins/Rem */
            uint8_t reg =
                HEADSET_INT_DIS_INT_ENA | HEADSET_INT_DIS_ADC_ENA | HEADSET_INT_DIS_DC_ENA | HEADSET_INT_DIS_INS_ENA;
            i2cAddr.subAddress = HEADSET_INT_DIS_ADDR;
            i2c->Write(i2cAddr, (uint8_t *)&reg, 1);

            ReadInsertionStatus();

            gpio->EnableInterrupt(1U << static_cast<uint32_t>(BoardDefinitions::HEADSET_IRQ_PIN));

            return kStatus_Success;
        }

        BaseType_t IRQHandler(void)
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            if (qHandleIrq != NULL) {
                uint8_t val = 0x01;
                xQueueSendFromISR(qHandleIrq, &val, &xHigherPriorityTaskWoken);
            }
            return xHigherPriorityTaskWoken;
        }

        void Handler(void)
        {
            ReadInsertionStatus();
        }

        bool IsInserted(void)
        {
            return HeadsetInserted;
        }

        status_t Deinit(void)
        {
            gpio->DisableInterrupt(1U << static_cast<uint32_t>(BoardDefinitions::HEADSET_IRQ_PIN));
            gpio->ClearPortInterrupts(1U << static_cast<uint32_t>(BoardDefinitions::HEADSET_IRQ_PIN));

            qHandleIrq = NULL;

            i2c.reset();
            gpio.reset();

            return kStatus_Success;
        }
    } // namespace headset
} // namespace bsp
