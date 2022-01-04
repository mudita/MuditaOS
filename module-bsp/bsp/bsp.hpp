#ifndef MODULE_BSP_BSP_HPP
#define MODULE_BSP_BSP_HPP

namespace bsp
{
    //! Board hardware init
    void BoardInit();

    // Board cuttoff power
    void BoardPowerOff();

    // Board system reset
    void BoardReboot();

    // Register platform exit functions
    int RegisterPlatformExitFunction(void (*func)());
}



#endif //MODULE_BSP_BSP_HPP
