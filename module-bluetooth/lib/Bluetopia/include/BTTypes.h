/*****< bttypes.h >************************************************************/
/*      Copyright 2000 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  BTTYPES - Common Bluetooth Defined Types.                                 */
/*                                                                            */
/*  Author:  Damon Lange                                                      */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   08/20/00  D. Lange       Initial creation.                               */
/******************************************************************************/
#ifndef __BTTYPESH__
#define __BTTYPESH__

   /* Miscellaneous defined type declarations.                          */

   /* Definitions for compilers that required structure to be explicitly*/
   /* declared as packed.                                               */
#if (defined(__CC_ARM) || defined(__GNUC__))

   /* Realview and GCC compilers.                                       */
   #define __PACKED_STRUCT_BEGIN__
   #define __PACKED_STRUCT_END__     __attribute__ ((packed))

#elif (defined(__ICCARM__))

   /* IAR compiler.                                                     */
   #define __PACKED_STRUCT_BEGIN__   __packed
   #define __PACKED_STRUCT_END__

#endif

#define __BTTYPESH_INC__

#include "BaseTypes.h"
#include "BTBTypes.h"

#undef __BTTYPESH_INC__

#endif
