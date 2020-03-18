/*****< btpsapit.h >***********************************************************/
/*      Copyright 2000 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  BTPSAPIT - Stonestreet One Bluetooth Protocol Stack API Type Definitions. */
/*                                                                            */
/*  Author:  Damon Lange                                                      */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   12/11/00  D. Lange       Initial creation.                               */
/******************************************************************************/
#ifndef __BTPSAPITH__
#define __BTPSAPITH__

   /* The following definition defines the API Calling Convention for   */
   /* ALL the exported Bluetoth API Functions.                          */
#ifndef BTPSAPI

   #define BTPSAPI

#endif

   /* The following conditional declaration controls the specific       */
   /* linkage that may/may not be required to specify Bluetooth API     */
   /* functions.                                                        */
#ifndef BTPSAPI_DECLARATION

   #define BTPSAPI_DECLARATION

#endif

   /* This define (BTPSCONST) is used to conditionally support the use  */
   /* of the const modifier.                                            */
   /* * NOTE * This is only provided for older platforms that do not    */
   /*          support the const modifier.                              */
#ifndef BTPSCONST

   #define BTPSCONST const

#endif

   /* In case the compiler does not support exception handling, define  */
   /* the exception handling to do nothing.                             */
#ifndef __BTPSTRY

   #define __BTPSTRY

#endif

#ifndef __BTPSEXCEPT

   #define __BTPSEXCEPT(_x)

#endif

#endif
