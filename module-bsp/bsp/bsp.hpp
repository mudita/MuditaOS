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
}



#endif //MODULE_BSP_BSP_HPP
