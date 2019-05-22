/*
 *  @file chip.h
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 26.02.2018
 *  @brief
 *  @copyright COpyright (C) 2018 mudita.com
 *  @details
 */

#ifndef CHIP_H_
#define CHIP_H_

#include "cmsis/cmsis_gcc.h"
#include "cmsis/core_cm7.h"
#include <stdio.h>
#include <stdbool.h>


//#define userprintf(...) SEGGER_RTT_printf(0, __VA_ARGS__)

//! Test if in interrupt mode
static inline bool isIRQ()
{
    return (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0 ;
}



#endif /* CHIP_H_ */
