/*****< btpsfile.h >***********************************************************/
/*      Copyright 2008 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  BTPSFILE - Stonestreet One Bluetooth Stack File Access Abstraction Type   */
/*             Definitions, Constants, and Prototypes.                        */
/*                                                                            */
/*  Author:  David Wooldridge                                                 */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   03/05/08  D. Wooldridge  Initial implementation.                         */
/*   06/17/09  D. Lange       Updated to support new API type schema.         */
/******************************************************************************/
#ifndef __BTPSFILEH__
#define __BTPSFILEH__

   /* The following constant represents the maximum supported Path by   */
   /* the functions provided by this module.                            */
#define BTPS_MAXIMUM_PATH_LENGTH                            512

   /* The following constant represents the maximum supported File Name */
   /* (including Path) supported by the functions in this module.       */
#define BTPS_MAXIMUM_FILE_NAME_LENGTH                       (BTPS_MAXIMUM_PATH_LENGTH + 1)

   /* The following constants define the directory delimeter in both    */
   /* character and string format (for the specified platform).         */
#define BTPS_DIRECTORY_DELIMETER_CHARACTER                  '\\'
#define BTPS_DIRECTORY_DELIMETER_STRING                     "\\"

#include "BFILEAPI.h"           /* Bluetooth File Abstraction Prototypes.     */

#endif
