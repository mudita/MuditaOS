/*****< llstypes.h >***********************************************************/
/*      Copyright 2012 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  LLSTypes - Stonestreet One Bluetooth Stack Link Loss Service              */
/*             Types.                                                         */
/*                                                                            */
/*  Author:  Ajay Parashar                                                    */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   06/25/12  A. Parashar    Initial creation.                               */
/******************************************************************************/
#ifndef __LLSTYPESH__
#define __LLSTYPESH__

#include "BTTypes.h"            /* Bluetooth Type Definitions.                */

   /* The following MACRO is a utility MACRO that assigns the Link Loss */
   /* Service 16 bit UUID to the specified UUID_16_t  Variable.         */
   /* This MACRO accepts one parameter which is a pointer to            */
   /* a UUID_16_t variable that is to receive the LLS UUID Constant     */
   /* value.                                                            */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define LLS_ASSIGN_LLS_SERVICE_UUID_16(_x)               ASSIGN_BLUETOOTH_UUID_16(*((UUID_16_t *)(_x)), 0x18, 0x03)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined LLS Service UUID in UUID16 form.  This     */
   /* MACRO only returns whether the UUID_16_t variable is equal to the */
   /* LLS Service UUID (MACRO returns boolean result) NOT less          */
   /* than/greater than.  The first parameter is the UUID_16_t variable */
   /* to compare to the LLS Service UUID.                               */
#define LLS_COMPARE_LLS_SERVICE_UUID_TO_UUID_16(_x)      COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x18, 0x03)

   /* The following defines the Link Loss Service UUID that is used when*/
   /* building the LLS Service Table.                                   */
#define LLS_SERVICE_BLUETOOTH_UUID_CONSTANT              { 0x03, 0x18 }

   /* The following MACRO is a utility MACRO that assigns the LLS       */
   /* Alert Level Characteristic 16 bit UUID to the                     */
   /* specified UUID_16_t variable.  This MACRO accepts one parameter   */
   /* which is the UUID_16_t variable that is to receive the LLS        */
   /* Alert Level UUID Constant value.                                  */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define LLS_ASSIGN_ALERT_LEVEL_UUID_16(_x)                     ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x06)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined LLS Alert Level UUID in UUID16             */
   /* form.  This MACRO only returns whether the UUID_16_t variable is  */
   /* equal to the Alert Level UUID (MACRO returns boolean              */
   /* result) NOT less than/greater than.  The first parameter is the   */
   /* UUID_16_t variable to compare to the LLS Alert Level UUID         */
#define LLS_COMPARE_LLS_ALERT_LEVEL_UUID_TO_UUID_16(_x)        COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x06)

   /* The following defines the LLS Alert Level Characteristic UUID     */
   /* that is used when building the LLS Service Table.                 */
#define LLS_ALERT_LEVEL_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT { 0x06, 0x2A }

  /* The following defines the valid values that may be used as the     */
  /* Operand value of a Alert Level characteristic.                     */
#define LLS_ALERT_LEVEL_NO_ALERT                         0x00
#define LLS_ALERT_LEVEL_MILD_ALERT                       0x01
#define LLS_ALERT_LEVEL_HIGH_ALERT                       0x02

   /* The following defines the length of the Alert Level characteristic*/
   /* value.                                                            */
#define LLS_ALERT_LEVEL_LENGTH                           (NON_ALIGNED_BYTE_SIZE)

   /* The following MACRO is a utility MACRO that exists to valid that a*/
   /* specified Alert Level is valid.  The only parameter to this       */
   /* function is the Alert Level structure to valid.  This MACRO       */
   /* returns TRUE if the Alert Level is valid or FALSE otherwise.      */
#define LLS_ALERT_LEVEL_VALID(_x)        ((((Byte_t)(_x)) >= LLS_ALERT_LEVEL_NO_ALERT) && (((Byte_t)(_x)) <= LLS_ALERT_LEVEL_HIGH_ALERT))

   /* The following defines the LLS GATT Service Flags MASK that should */
   /* be passed into GATT_Register_Service when the LLS Service is      */
   /* registered.                                                       */
#define LLS_SERVICE_FLAGS                                (GATT_SERVICE_FLAGS_LE_SERVICE)

#endif
