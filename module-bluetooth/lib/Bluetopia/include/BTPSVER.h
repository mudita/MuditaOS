/*****< btpsver.h >************************************************************/
/*                                                                            */
/* Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/     */
/* ALL RIGHTS RESERVED.                                                       */
/*                                                                            */
/*  BTPSVER - Texas Instruments Bluetooth Protocol Stack Version Information. */
/*                                                                            */
/*  Author:  Ram Malovany                                                     */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   09/12/14  R. Malovany    Initial creation.                               */
/*   05/03/15  D. Horowitz    Delete platforms defines and macros             */
/*   12/16/15  D. Horowitz    Version changed to 4.1.0.0                      */
/*   05/05/16  L. Gersi       Version changed to 4.2.0.1                      */
/*   07/07/16  D. Horowitz    Version changed to 4.2.0.5                      */
/*   09/29/16  D. Horowitz    Version changed to 4.2.0.6                      */
/*   10/20/16  D. Horowitz    Version changed to 4.2.0.7                      */
/*   11/09/16  D. Horowitz    Version changed to 4.2.1.0                      */
/******************************************************************************/
#ifndef __BTPSVER_H__
#define __BTPSVER_H__


/* Bluetooth Protocol Stack Core Support .                      */
#define BTPS_VERSION_MAJOR_CORE_SUPPORT                         4

/* Bluetooth Protocol Stack Core Support .                      */
#define BTPS_VERSION_MINOR_CORE_SUPPORT                         2


 /* Bluetooth Protocol Stack Major Version Release Numbers.     */
#ifndef BTPS_VERSION_MAJOR_VERSION_NUMBER
    #define BTPS_VERSION_MAJOR_VERSION_NUMBER                   1
#endif

/* Bluetooth Protocol Stack Minor Version Release Numbers.      */
#ifndef BTPS_VERSION_MINOR_VERSION_NUMBER
    #define BTPS_VERSION_MINOR_VERSION_NUMBER                   0
#endif




   /* Constants used to convert numeric constants to string constants   */
   /* (used in MACRO's below).                                          */
#define BTPS_VERSION_NUMBER_TO_STRING(_x)                   #_x
#define BTPS_VERSION_CONSTANT_TO_STRING(_y)                 BTPS_VERSION_NUMBER_TO_STRING(_y)

   /* Bluetooth Protocol Stack Constant Version String of the form      */
   /*    "a.b.c.d"                                                      */
   /* where:                                                            */
   /*    a - BTPS_VERSION_MAJOR_CORE_SUPPORT                            */
   /*    b - BTPS_VERSION_MINOR_CORE_SUPPORT                            */
   /*    c - BTPS_VERSION_MAJOR_VERSION_NUMBER                          */
   /*    d - BTPS_VERSION_MINOR_VERSION_NUMBER                          */


#define BTPS_VERSION_VERSION_STRING                         BTPS_VERSION_CONSTANT_TO_STRING(BTPS_VERSION_MAJOR_CORE_SUPPORT) "." BTPS_VERSION_CONSTANT_TO_STRING(BTPS_VERSION_MINOR_CORE_SUPPORT) "." BTPS_VERSION_CONSTANT_TO_STRING(BTPS_VERSION_MAJOR_VERSION_NUMBER) "." BTPS_VERSION_CONSTANT_TO_STRING(BTPS_VERSION_MINOR_VERSION_NUMBER)



   /* File/Product/Company Name Copyrights and Descriptions.            */
#define BTPS_VERSION_PRODUCT_NAME_STRING                    "TI's Bluetooth Protocol Stack"

#define BTPS_VERSION_COMPANY_NAME_STRING                    "Texas Instruments"

#define BTPS_VERSION_COPYRIGHT_STRING                       "Copyright (C) 2014 Texas Instruments"

#endif
