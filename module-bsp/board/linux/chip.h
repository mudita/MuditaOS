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

#include <stdio.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

//! Test if in interrupt mode
static inline bool isIRQ()
{
    return false;
}

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* CHIP_H_ */
