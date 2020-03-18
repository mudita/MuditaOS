/*****< tpstypes.h >***********************************************************/
/*      Copyright 2012 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  TPSTypes - Stonestreet One Bluetooth Stack Tx Power Service Types.        */
/*                                                                            */
/*  Author:  Jay Wheeler                                                      */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   08/15/12  J. Wheeler     Initial creation.                               */
/******************************************************************************/
#ifndef __TPSTYPESH__
#define __TPSTYPESH__

#include "BTTypes.h"            /* Bluetooth Type Definitions.                */

   /* The following MACRO is a utility MACRO that assigns the Tx Power  */
   /* Service 16 bit UUID to the specified UUID_16_t variable.          */
   /* This MACRO accepts one parameter which is a pointer to            */
   /* a UUID_16_t variable that is to receive the TPS UUID Constant     */
   /* value.                                                            */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define TPS_ASSIGN_TPS_SERVICE_UUID_16(_x)               ASSIGN_BLUETOOTH_UUID_16(*((UUID_16_t *)(_x)), 0x18, 0x04)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined TPS Service UUID in UUID16 form.  This     */
   /* MACRO only returns whether the UUID_16_t variable is equal to the */
   /* TPS Service UUID (MACRO returns boolean result) NOT less than /   */
   /* greater than.  The first parameter is the UUID_16_t variable to   */
   /* compare to the TPS Service UUID.                                  */
#define TPS_COMPARE_TPS_SERVICE_UUID_TO_UUID_16(_x)      COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x18, 0x04)

   /* The following defines the Tx Power Service UUID that is used when */
   /* building the TPS Service Table.                                   */
#define TPS_SERVICE_BLUETOOTH_UUID_CONSTANT              { 0x04, 0x18 }

   /* The following MACRO is a utility MACRO that assigns the TPS       */
   /* Tx Power Level Characteristic 16 bit UUID to the specified        */
   /* UUID_16_t variable.  This MACRO accepts one parameter which is    */
   /* the UUID_16_t variable that is to receive the TPS Tx Power Level  */
   /* UUID Constant value.                                              */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define TPS_ASSIGN_TX_POWER_LEVEL_UUID_16(_x)                      ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x07)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined TPS Tx Power Level UUID in UUID16 form .   */
   /* This MACRO only returns whether the UUID_16_t variable is equal   */
   /* to the Tx Power Level UUID (MACRO returns boolean result) NOT     */
   /* less than/greater than.  The first parameter is the UUID_16_t     */
   /* variable to compare to the Tx Power Level UUID.                   */
#define TPS_COMPARE_TPS_TX_POWER_LEVEL_UUID_TO_UUID_16(_x)         COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x07)

   /* The following defines the TPS Tx Power Level Characteristic UUID  */
   /* that is used when building the TPS Service Table.                 */
#define TPS_TX_POWER_LEVEL_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0x07, 0x2A }

   /* The following defines the length of the Measurement Interval      */
   /* characteristic value.                                             */
#define TPS_TX_POWER_LEVEL_LENGTH                        (NON_ALIGNED_BYTE_SIZE)

   /* The following values define the minimum and maximum acceptable    */
   /* values for the Tx Power Level characteristic.                     */
#define TPS_TX_POWER_MIN_VALUE                           (-100)
#define TPS_TX_POWER_MAX_VALUE                           (20)

   /* The following MACRO is a utility MACRO that can be used to verify */
   /* that a given Tx Power Level value is within the valid range (as   */
   /* defined above.)                                                   */
#define TPS_TX_POWER_LEVEL_VALID(_x)                     (((_x) >= TPS_TX_POWER_MIN_VALUE) && ((_x) <= TPS_TX_POWER_MAX_VALUE))

   /* The following defines the TPS GATT Service Flags MASK that should */
   /* be passed into GATT_Register_Service when the TPS Service is      */
   /* registered.                                                       */
#define TPS_SERVICE_FLAGS                                (GATT_SERVICE_FLAGS_LE_SERVICE)

#endif
