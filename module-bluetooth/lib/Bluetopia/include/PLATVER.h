/*****< platver.h >************************************************************/
/*                                                                            */
/* Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/     */
/* ALL RIGHTS RESERVED.                                                       */
/*                                                                            */
/*  PLATVER - Texas Instruments Bluetooth Platform Version Information. 	  */
/*                                                                            */
/*  Author:  Dan Horowitz                                                     */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   dd/mm/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   03/05/15  D. Horowitz    Initial creation.                               */
/*   05/05/16  L. Gersi		  Update version to 0.1.                          */
/*   29/05/16  D. Horowitz	  Update version to 0.2.                          */
/*   07/07/16  D. Horowitz	  Update version to 0.5.                          */
/*   29/09/16  D. Horowitz	  Update version to 0.7.                          */
/*   27/10/16  D. Horowitz	  Update version to 0.8.                          */
/*   03/11/16  D. Horowitz	  Update version to 1.0.                          */
/******************************************************************************/
#ifndef __PLATVER_H__
#define __PLATVER_H__

 /* Customer Major Version Release Numbers.                     		*/
#ifndef PLATFORM_MAJOR_VERSION_NUMBER
    #define PLATFORM_MAJOR_VERSION_NUMBER                  			1
#endif

/* Bluetooth Protocol Stack Minor Version Release Numbers.      		*/
#ifndef PLATFORM_MINOR_VERSION_NUMBER
    #define PLATFORM_MINOR_VERSION_NUMBER                  			0
#endif



   /* Constants used to convert numeric constants to string constants   */
   /* (used in MACRO's below).                                          */
#define PLATFORM_VERSION_NUMBER_TO_STRING(_x)                   	#_x
#define PLATFORM_VERSION_CONSTANT_TO_STRING(_y)                 	BTPS_VERSION_NUMBER_TO_STRING(_y)
   /*  Customer Version String of the form                              */
   /*    "a.b"                                                          */
   /* where:                                                            */
   /*    a - PLATFORM_MAJOR_VERSION_NUMBER                        	 	*/
   /*    b - PLATFORM_MINOR_VERSION_NUMBER                         		*/

#define PLATFORM_VERSION_STRING                        				PLATFORM_VERSION_CONSTANT_TO_STRING(PLATFORM_MAJOR_VERSION_NUMBER) "." PLATFORM_VERSION_CONSTANT_TO_STRING(PLATFORM_MINOR_VERSION_NUMBER)

#endif
