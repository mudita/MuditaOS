/*****< hcitrcfg.h >***********************************************************/
/*      Copyright 2012 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*      Copyright 2015 Texas Instruments Incorporated.                        */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  HCITRCFG - HCI Transport Layer Configuration parameters.                  */
/*                                                                            */
/*  Author:  Marcus Funk                                                      */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   11/08/12  M. Funk        Initial creation.                               */
/*   11/02/15  D. Keren       Changing nShutdown pin from PC9 to PC8          */
/******************************************************************************/
#ifndef __HCITRCFGH__
#define __HCITRCFGH__

#include "BTAPITyp.h"            /* Bluetooth API Type Definitions.           */

#include "stm32f4xx.h"           /* STM32F register definitions.              */
#include "stm32f4xx_gpio.h"      /* STM32F GPIO control functions.            */
#include "stm32f4xx_usart.h"     /* STM32F USART control functions.           */
#include "stm32f4xx_rcc.h"       /* STM32F Clock control functions.           */
#include "stm32f4xx_exti.h"      /* STM32F Ext interrupt definitions.         */
#include "stm32f4xx_syscfg.h"    /* STM32F system config definitions.         */

   /* The following definitions define the UART/USART to be used by the */
   /* HCI transport and the pins that will be used by the UART.  Please */
   /* consult the processor's documentation to determine what pins are  */
   /* available for the desired UART.                                   */
   /* * NOTE * The TXD and RXD pins MUST be map-able to the selected    */
   /*          UART.  Additionally, if hardware flow control is desired,*/
   /*          the RTS and CTS pins must also be map-able to the        */
   /*          selected UART.  If software managed flow is used, RTS may*/
   /*          be any available GPIO but CTS must be a GPIO that can be */
   /*          mapped to an available EXTI line.  The RESET pin may be  */
   /*          any available GPIO.                                      */
#define HCITR_UART               5

#define HCITR_TXD_PORT           C
#define HCITR_TXD_PIN            12

#define HCITR_RXD_PORT           D
#define HCITR_RXD_PIN            2

#define HCITR_RTS_PORT           D
#define HCITR_RTS_PIN            1

#define HCITR_CTS_PORT           D
#define HCITR_CTS_PIN            0

#define HCITR_RESET_PORT         D
#define HCITR_RESET_PIN          3

   /* Define the following to enable suspend functionality within       */
   /* HCITRANS.  This will shut down the UART when HCITR_COMSuspend() is*/
   /* called and resume normal functionality when data is received in   */
   /* transmitted.                                                      */
   /* * NOTE * This functionality requires using a lower power protocol */
   /*          such as HCILL and the UART should only be suspended when */
   /*          indicated it is safe to do so by the protocol driver.    */
#define SUPPORT_TRANSPORT_SUSPEND

   /* Define the following to explicitly enable software controled      */
   /* CTS/RTS.  This is necessary if the pin specified for the CTS and  */
   /* RTS control lines are not map-able to the specified USART.        */
   /* * NOTE * This is defined automatically for the UARTs that do not  */
   /*          support hardware flow control.                           */
// #define USE_SOFTWARE_CTS_RTS

   /* Define the following if software managed flow control is being    */
   /* used and the NVIC interrupt for the CTS EXTI line is being also   */
   /* used by another EXTI line.  The specified function can then be    */
   /* called by a global interrupt handler when the CTS EXTI interrupt  */
   /* occurs.                                                           */
   /* * NOTE * If defined when software managed flow control is used,   */
   /*          the NVIC interrupt associated with the CTS EXTI line MUST*/
   /*          be handled externally and call this function.  If not    */
   /*          defined, the interrupt will be handled directly by       */
   /*          HCITRANS.                                                */
// #define HCITR_CTS_IRQ_HANDLER HCITR_CTS_IrqHandler

   /* Define the following to enable debug logging of HCI traffic.  If  */
   /* this macro is defined, all incomming and outgoing traffic will be */
   /* logged via BTPS_OutputMessage().                                  */
// #define HCITR_ENABLE_DEBUG_LOGGING


/************************************************************************/
/* !!!DO NOT MODIFY PAST THIS POINT!!!                                  */
/************************************************************************/

   /* The following section builds the macros that can be used with the */
   /* STM32F standard peripheral libraries based on the above           */
   /* configuration.                                                    */

   /* Standard C style concatenation macros                             */
#define DEF_CONCAT2(_x_, _y_)          __DEF_CONCAT2__(_x_, _y_)
#define __DEF_CONCAT2__(_x_, _y_)      _x_ ## _y_

#define DEF_CONCAT3(_x_, _y_, _z_)     __DEF_CONCAT3__(_x_, _y_, _z_)
#define __DEF_CONCAT3__(_x_, _y_, _z_) _x_ ## _y_ ## _z_

   /* Determine the Peripheral bus that is used by the UART.            */
#if ((HCITR_UART == 1) ||(HCITR_UART == 6))
   #define HCITR_UART_APB              2
#else
   #define HCITR_UART_APB              1
#endif

   /* Determine the type of UART.                                       */
#if ((HCITR_UART == 1) || (HCITR_UART == 2) || (HCITR_UART == 3) || (HCITR_UART == 6))

   #define HCITR_UART_TYPE             USART

#elif ((HCITR_UART == 4) || (HCITR_UART == 5))

   #define HCITR_UART_TYPE             UART

   /* These UARTs do not support hardware flow control so make sure that*/
   /* software managed flow control is used.                            */
   #ifndef USE_SOFTWARE_CTS_RTS
      #define USE_SOFTWARE_CTS_RTS
   #endif

#else
   #error Unknown HCITR_UART
#endif

   /* The following section builds the macro names that can be used with*/
   /* the STM32F standard peripheral libraries.                         */

   /* UART control mapping.                                             */
#define HCITR_UART_BASE                (DEF_CONCAT2(HCITR_UART_TYPE, HCITR_UART))
#define HCITR_UART_IRQ                 (DEF_CONCAT3(HCITR_UART_TYPE, HCITR_UART, _IRQn))
#define HCITR_UART_IRQ_HANDLER         (DEF_CONCAT3(HCITR_UART_TYPE, HCITR_UART, _IRQHandler))

#define HCITR_UART_RCC_PERIPH_CLK_CMD  (DEF_CONCAT3(RCC_APB, HCITR_UART_APB, PeriphClockCmd))
#define HCITR_UART_RCC_PERIPH_CLK_BIT  (DEF_CONCAT3(DEF_CONCAT3(RCC_APB, HCITR_UART_APB, Periph_), HCITR_UART_TYPE, HCITR_UART))
#define HCITR_UART_GPIO_AF             (DEF_CONCAT3(GPIO_AF_, HCITR_UART_TYPE, HCITR_UART))

   /* GPIO mapping.                                                     */
#define HCITR_TXD_GPIO_PORT            (DEF_CONCAT2(GPIO, HCITR_TXD_PORT))
#define HCITR_RXD_GPIO_PORT            (DEF_CONCAT2(GPIO, HCITR_RXD_PORT))
#define HCITR_RTS_GPIO_PORT            (DEF_CONCAT2(GPIO, HCITR_RTS_PORT))
#define HCITR_CTS_GPIO_PORT            (DEF_CONCAT2(GPIO, HCITR_CTS_PORT))
#define HCITR_RESET_GPIO_PORT          (DEF_CONCAT2(GPIO, HCITR_RESET_PORT))

#define HCITR_TXD_GPIO_AHB_BIT         (DEF_CONCAT2(RCC_AHB1Periph_GPIO, HCITR_TXD_PORT))
#define HCITR_RXD_GPIO_AHB_BIT         (DEF_CONCAT2(RCC_AHB1Periph_GPIO, HCITR_RXD_PORT))
#define HCITR_RTS_GPIO_AHB_BIT         (DEF_CONCAT2(RCC_AHB1Periph_GPIO, HCITR_RTS_PORT))
#define HCITR_CTS_GPIO_AHB_BIT         (DEF_CONCAT2(RCC_AHB1Periph_GPIO, HCITR_CTS_PORT))
#define HCITR_RESET_GPIO_AHB_BIT       (DEF_CONCAT2(RCC_AHB1Periph_GPIO, HCITR_RESET_PORT))

   /* Interrupt mapping.                                                */
#if (defined(SUPPORT_TRANSPORT_SUSPEND) || defined(USE_SOFTWARE_CTS_RTS))

   #if (HCITR_CTS_PIN   < 5)
      #define HCITR_CTS_EXTI_NUMBER    HCITR_CTS_PIN
   #elif (HCITR_CTS_PIN < 10)
      #define HCITR_CTS_EXTI_NUMBER    9_5
   #elif (HCITR_CTS_PIN < 16)
      #define HCITR_CTS_EXTI_NUMBER    15_10
   #endif

   /* NOTE: "EXTI" is defined in the STM32F std periph headers so can   */
   /* not be used directly.                                             */
   #define HCITR_CTS_IRQ               (DEF_CONCAT3(EXT, DEF_CONCAT2(I, HCITR_CTS_EXTI_NUMBER), _IRQn))
   #define HCITR_CTS_EXTI_PORT         (DEF_CONCAT2(EXTI_PortSourceGPIO, HCITR_CTS_PORT))
   #define HCITR_CTS_IRQ_LINE          (DEF_CONCAT2(EXTI_Line, HCITR_CTS_PIN))

   #ifndef HCITR_CTS_IRQ_HANDLER
      #define HCITR_CTS_IRQ_HANDLER    (DEF_CONCAT3(EXT, DEF_CONCAT2(I, HCITR_CTS_EXTI_NUMBER), _IRQHandler))
   #endif

#endif

#endif

