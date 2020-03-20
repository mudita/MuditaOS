#include "board.h"
#include "fsl_gpio.h"
#include "segger/log/log.hpp"
#include "rt1051_harness.hpp"

using namespace bsp::harness::gpio;

void hwGpioWrite(Pin pin, uint32_t state)
{
    switch (pin) {
    case Pin::SIM_PRESENCE:
        GPIO_PinWrite(BSP_CELLULAR_SIM_CARD_PRESENCE_PORT, BSP_CELLULAR_SIM_CARD_PRESENCE_PIN, state);
        break;
    case Pin::SIM_SELECT:
        GPIO_PinWrite(BSP_CELLULAR_SIMSEL_PORT, BSP_CELLULAR_SIMSEL_PIN, state);
        break;
    default:
        LOG_ERROR("unhandled pin: %d", pin);
    }
}

bool hwGpioRead(Pin pin)
{
    switch (pin) {
    case Pin::SIM_PRESENCE:
        return GPIO_PinRead(BSP_CELLULAR_SIM_CARD_PRESENCE_PORT, BSP_CELLULAR_SIM_CARD_PRESENCE_PIN) > 0;
    case Pin::SIM_SELECT:
        return GPIO_PinRead(BSP_CELLULAR_SIMSEL_PORT, BSP_CELLULAR_SIMSEL_PIN) > 0;
    default:
        LOG_ERROR("Unhandled pin: %d", pin);
        return false;
    }
}
