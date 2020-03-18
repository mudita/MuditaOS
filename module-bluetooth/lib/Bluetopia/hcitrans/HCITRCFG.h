/*****< hcitrcfg.h >***********************************************************/
/*      Copyright 2012 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*      Copyright 2015 Texas Instruments Incorporated.                        */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  HCITRCFG - HCI Transport Layer Configuration parameters.                  */ /*                                                                            */
/*  Author:  Marcus Funk                                                      */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   11/08/12  M. Funk        Initial creation.                               */
/*   11/02/15  D. Keren       Changing nShutdown pin from PC9 to PC8          */
/******************************************************************************/
#pragma once

#include "BTAPITyp.h"            /* Bluetooth API Type Definitions.           */

/* Define the following to enable suspend functionality within
   HCITRANS.  This will shut down the UART when HCITR_COMSuspend() is
   called and resume normal functionality when data is received in
   transmitted.
   * NOTE * This functionality requires using a lower power protocol
            such as HCILL and the UART should only be suspended when
            indicated it is safe to do so by the protocol driver.    */
// #define SUPPORT_TRANSPORT_SUSPEND

/* Define the following to explicitly enable software controled
   CTS/RTS.  This is necessary if the pin specified for the CTS and
   RTS control lines are not map-able to the specified USART.
   * NOTE * This is defined automatically for the UARTs that do not
            support hardware flow control.                           */
#define USE_SOFTWARE_CTS_RTS

/* Define the following if software managed flow control is being
   used and the NVIC interrupt for the CTS EXTI line is being also
   used by another EXTI line.  The specified function can then be
   called by a global interrupt handler when the CTS EXTI interrupt
   occurs.
   * NOTE * If defined when software managed flow control is used,
            the NVIC interrupt associated with the CTS EXTI line MUST
            be handled externally and call this function.  If not
            defined, the interrupt will be handled directly by
            HCITRANS.                                                */
// #define HCITR_CTS_IRQ_HANDLER HCITR_CTS_IrqHandler

/* Define the following to enable debug logging of HCI traffic.  If
   this macro is defined, all incomming and outgoing traffic will be
   logged via BTPS_OutputMessage().                                  */
#define HCITR_ENABLE_DEBUG_LOGGING
