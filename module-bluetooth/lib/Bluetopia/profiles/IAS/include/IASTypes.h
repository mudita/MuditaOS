/*****< iastypes.h >***********************************************************/
/*      Copyright 2012 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  IASTypes - Stonestreet One Bluetooth Stack Immediate Alert Service        */
/*             Types.                                                         */
/*                                                                            */
/*  Author:  Ajay Parashar                                                    */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   08/24/12  A. Parashar    Initial creation.                               */
/******************************************************************************/
#ifndef __IASTYPEH__
#define __IASTYPEH__

#include "BTTypes.h"            /* Bluetooth Type Definitions.                */

   /* The following MACRO is a utility MACRO that assigns the Immediate */
   /* Alert Service 16 bit UUID to the specified UUID_16_t              */
   /* variable.  This MACRO accepts one parameter which is a pointer to */
   /* a UUID_16_t variable that is to receive the IAS UUID Constant     */
   /* value.                                                            */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define IAS_ASSIGN_IAS_SERVICE_UUID_16(_x)               ASSIGN_BLUETOOTH_UUID_16(*((UUID_16_t *)(_x)), 0x18, 0x02)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined IAS Service UUID in UUID16 form.  This     */
   /* MACRO only returns whether the UUID_16_t variable is equal to the */
   /* IAS Service UUID (MACRO returns boolean result) NOT less          */
   /* than/greater than.  The first parameter is the UUID_16_t variable */
   /* to compare to the IAS Service UUID.                               */
#define IAS_COMPARE_IAS_SERVICE_UUID_TO_UUID_16(_x)      COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x18, 0x02)

   /* The following defines the IAS Parameter Service UUID that is      */
   /* used when building the IAS Service Table.                         */
#define IAS_SERVICE_BLUETOOTH_UUID_CONSTANT              { 0x02, 0x18 }

   /* The following MACRO is a utility MACRO that assigns the IAS       */
   /* Alert Level Characteristic 16 bit UUID to the                     */
   /* specified UUID_16_t variable.  This MACRO accepts one parameter   */
   /* which is the UUID_16_t variable that is to receive the IAS        */
   /* Alert Level UUID Constant value.                                  */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define IAS_ASSIGN_ALERT_LEVEL_UUID_16(_x)                     ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x06)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined IAS Alert Level UUID in UUID16             */
   /* form.  This MACRO only returns whether the UUID_16_t variable is  */
   /* equal to the Alert Level UUID (MACRO returns boolean              */
   /* result) NOT less than/greater than.  The first parameter is the   */
   /* UUID_16_t variable to compare to the IAS Alert Level UUID         */
#define IAS_COMPARE_ALERT_LEVEL_UUID_TO_UUID_16(_x)            COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x06)

   /* The following defines the IAS Alert Level Characteristic UUID that*/
   /* is used when building the IAS Service Table.                      */
#define IAS_ALERT_LEVEL_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT { 0x06, 0x2A }

  /* The following defines the IAS GATT Service Flags MASK that should  */
  /* be passed into GATT_Register_Service when the IAS Service is       */
  /* registered.                                                        */
#define IAS_ALERT_LEVEL_NO_ALERT                   0x00
#define IAS_ALERT_LEVEL_MILD_ALERT                 0x01
#define IAS_ALERT_LEVEL_HIGH_ALERT                 0x02

   /* The following MACRO is a utility MACRO that exists to valid that a*/
   /* specified Alert Level is valid.  The only parameter to this       */
   /* function is the Alert Level structure to valid.  This MACRO       */
   /* returns TRUE if the Alert Level is valid or FALSE otherwise.      */
#define IAS_ALERT_LEVEL_VALID(_x)        ((((Byte_t)(_x)) >= IAS_ALERT_LEVEL_NO_ALERT) && (((Byte_t)(_x)) <= IAS_ALERT_LEVEL_HIGH_ALERT))

   /* The following defines the length of the Alert Level Control Point */
   /* characteristic value.                                             */
#define IAS_ALERT_LEVEL_CONTROL_POINT_VALUE_LENGTH        (BYTE_SIZE)

   /* The following defines the IAS GATT Service Flags MASK that should */
   /* be passed into GATT_Register_Service when the IAS Service is      */
   /* registered.                                                       */
#define IAS_SERVICE_FLAGS                (GATT_SERVICE_FLAGS_LE_SERVICE)

#endif
