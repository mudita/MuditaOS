/*****< gapstype.h >***********************************************************/
/*      Copyright 2011 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  GAPSType - Stonestreet One Bluetooth Stack Generic Access Profile Service */
/*             Types.                                                         */
/*                                                                            */
/*  Author:  Tim Cook                                                         */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   09/29/11  T. Cook        Initial creation.                               */
/******************************************************************************/
#ifndef __GAPSTYPEH__
#define __GAPSTYPEH__

#include "BTTypes.h"            /* Bluetooth Type Definitions.                */

   /* The following defines the maximum allowable GAP Device Name.      */
#define GAP_MAXIMUM_DEVICE_NAME_LENGTH                   ((BTPS_CONFIGURATION_GAPS_MAXIMUM_SUPPORTED_DEVICE_NAME > 248)?248:BTPS_CONFIGURATION_GAPS_MAXIMUM_SUPPORTED_DEVICE_NAME)

   /* The following MACRO is a utility MACRO that assigns the Generic   */
   /* Access Profile Service 16 bit UUID to the specified UUID_16_t     */
   /* variable.  This MACRO accepts one parameter which is the UUID_16_t*/
   /* variable that is to receive the GAP UUID Constant value.          */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define GAP_ASSIGN_GAP_SERVICE_UUID_16(_x)               ASSIGN_BLUETOOTH_UUID_16((_x), 0x18, 0x00)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined GAP Service UUID in UUID16 form.  This     */
   /* MACRO only returns whether the UUID_16_t variable is equal to the */
   /* GAP Service UUID (MACRO returns boolean result) NOT less          */
   /* than/greater than.  The first parameter is the UUID_16_t variable */
   /* to compare to the GAP Service UUID.                               */
#define GAP_COMPARE_GAP_SERVICE_UUID_TO_UUID_16(_x)      COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x18, 0x00)

   /* The following defines the GAP Service UUID that is used when      */
   /* building the GAPS Service Table.                                  */
#define GAP_SERVICE_BLUETOOTH_UUID_CONSTANT              { 0x00, 0x18 }

   /* The following MACRO is a utility MACRO that assigns the GAP Device*/
   /* Name Characteristic 16 bit UUID to the specified UUID_16_t        */
   /* variable.  This MACRO accepts one parameter which is the UUID_16_t*/
   /* variable that is to receive the GAP Device Name UUID Constant     */
   /* value.                                                            */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define GAP_ASSIGN_DEVICE_NAME_UUID16(_x)                      ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x00)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined GAP Device Name UUID in UUID16 form.  This */
   /* MACRO only returns whether the UUID_16_t variable is equal to the */
   /* GAP Device Name UUID (MACRO returns boolean result) NOT less      */
   /* than/greater than.  The first parameter is the UUID_16_t variable */
   /* to compare to the GAP Device Name UUID.                           */
#define GAP_COMPARE_GAP_DEVICE_NAME_UUID_TO_UUID_16(_x)        COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x00)

   /* The following defines the GAP Device Name Characteristic UUID that*/
   /* is used when building the GAP Service Table.                      */
#define GAP_DEVICE_NAME_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT { 0x00, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the GAP       */
   /* Service Device Appearence Characteristic 16 bit UUID to the       */
   /* specified UUID_16_t variable.  This MACRO accepts one parameter   */
   /* accepts one parameter which is the UUID_16_t variable that is to  */
   /* receive the GAP Characteristic UUID Constant value.               */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define GAP_ASSIGN_DEVICE_APPEARENCE_UUID16(_x)                      ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x01)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined GAP Device Appearance UUID in UUID16 form. */
   /* This MACRO only returns whether the UUID_16_t variable is equal to*/
   /* the GAP Device Appearance UUID (MACRO returns boolean result) NOT */
   /* less than/greater than.  The first parameter is the UUID_16_t     */
   /* variable to compare to the GAP Device Appearance UUID.            */
#define GAP_COMPARE_GAP_DEVICE_APPEARANCE_UUID_TO_UUID_16(_x)        COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x01)

   /* The following defines the GAP Device Appearance Characteristic    */
   /* UUID that is used when building the GAP Service Table.            */
#define GAP_DEVICE_APPEARANCE_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT { 0x01, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the GAP       */
   /* Peripheral Privacy Flag Characteristic 16 bit UUID to the         */
   /* specified UUID_16_t variable.  This MACRO accepts one parameter   */
   /* accepts one parameter which is the UUID_16_t variable that is to  */
   /* receive the GAP Peripheral Privacy Flag UUID Constant value.      */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define GAP_ASSIGN_PERIPHERAL_PRIVACY_FLAG_UUID16(_x) ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x02)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined GAP Peripheral Privacy Flag UUID in UUID16 */
   /* form.  This MACRO only returns whether the UUID_16_t variable is  */
   /* equal to the GAP Peripheral Privacy Flag UUID (MACRO returns      */
   /* boolean result) NOT less than/greater than.  The first parameter  */
   /* is the UUID_16_t variable to compare to the GAP Peripheral Privacy*/
   /* Flag UUID.                                                        */
#define GAP_COMPARE_GAP_PERIPHERAL_PRIVACY_FLAG_UUID_TO_UUID_16(_x)  COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x02)

   /* The following defines the GAP Peripheral Privacy Flag             */
   /* Characteristic UUID that is used when building the GAP Service    */
   /* Table.                                                            */
#define GAP_PERIPHERAL_PRIVACY_FLAG_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT { 0x02, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the GAP       */
   /* Service Reconnection Address Characteristic 16 bit UUID to the    */
   /* specified UUID_16_t variable.  This MACRO accepts one parameter   */
   /* accepts one parameter which is the UUID_16_t variable that is to  */
   /* receive the GAP Reconnection Address UUID Constant value.         */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define GAP_ASSIGN_RECONNECTION_ADDRESS_UUID16(_x)                      ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x03)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined GAP Reconnection Address UUID in UUID16    */
   /* form.  This MACRO only returns whether the UUID_16_t variable is  */
   /* equal to the GAP Reconnection Address UUID (MACRO returns boolean */
   /* result) NOT less than/greater than.  The first parameter is the   */
   /* UUID_16_t variable to compare to the GAP Reconnection Address     */
   /* UUID.                                                             */
#define GAP_COMPARE_GAP_RECONNECTION_ADDRESS_UUID_TO_UUID_16(_x)        COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x03)

   /* The following defines the GAP Reconnection Address Characteristic */
   /* UUID that is used when building the GAP Service Table.            */
#define GAP_RECONNECTION_ADDRESS_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT { 0x03, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the GAP       */
   /* Peripheral Preferred Connection Parameters Characteristic 16 bit  */
   /* UUID to the specified UUID_16_t variable.  This MACRO accepts one */
   /* parameter which is the UUID_16_t variable that is to receive the  */
   /* GAP Peripheral Preferred Connection Parameters UUID Constant      */
   /* value.                                                            */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define GAP_ASSIGN_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_UUID16(_x) ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x04)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined GAP Peripheral Preferred Connection        */
   /* Parameters UUID in UUID16 form.  This MACRO only returns whether  */
   /* the UUID_16_t variable is equal to the GAP Peripheral Preferred   */
   /* Connection Parameters UUID (MACRO returns boolean result) NOT less*/
   /* than/greater than.  The first parameter is the UUID_16_t variable */
   /* to compare to the GAP Peripheral Preferred Connection Parameters  */
   /* UUID.                                                             */
#define GAP_COMPARE_GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_UUID_TO_UUID_16(_x) COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x04)

   /* The following defines the GAP Peripheral Preferred Connection     */
   /* Parameters UUID that is used when building the GAP Service Table. */
#define GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT { 0x04, 0x2A }

   /* The following MACRO is a utility MACRO that is used to validate   */
   /* the length of a Write into the Device Name Characteristic (if it  */
   /* is writable).  The only parameter to this MACRO is the length of  */
   /* the Value that is being written.                                  */
#define GAP_DEVICE_NAME_VALID_LENGTH(_x)                 ((((Word_t)(_x)) >= 0) && (((Word_t)(_x)) <= GAP_MAXIMUM_DEVICE_NAME_LENGTH))

   /* The following defines the valid Device Appearence Characteristic  */
   /* Value Length.                                                     */
#define GAP_DEVICE_APPEARENCE_VALUE_LENGTH               (WORD_SIZE)

   /* The following define the value GAP Device Appearence Values.      */
#define GAP_DEVICE_APPEARENCE_VALUE_UNKNOWN                                                        0
#define GAP_DEVICE_APPEARENCE_VALUE_GENERIC_PHONE                                                  64
#define GAP_DEVICE_APPEARENCE_VALUE_GENERIC_COMPUTER                                               128
#define GAP_DEVICE_APPEARENCE_VALUE_GENERIC_WATCH                                                  192
#define GAP_DEVICE_APPEARENCE_VALUE_SPORTS_WATCH                                                   193
#define GAP_DEVICE_APPEARENCE_VALUE_GENERIC_CLOCK                                                  256
#define GAP_DEVICE_APPEARENCE_VALUE_GENERIC_DISPLAY                                                320
#define GAP_DEVICE_APPEARENCE_VALUE_GENERIC_GENERIC_REMOTE_CONTROL                                 384
#define GAP_DEVICE_APPEARENCE_VALUE_GENERIC_EYE_GLASSES                                            448
#define GAP_DEVICE_APPEARENCE_VALUE_GENERIC_TAG                                                    512
#define GAP_DEVICE_APPEARENCE_VALUE_GENERIC_KEYRING                                                576
#define GAP_DEVICE_APPEARENCE_VALUE_GENERIC_MEDIA_PLAYER                                           640
#define GAP_DEVICE_APPEARENCE_VALUE_GENERIC_BARCODE_SCANNER                                        704
#define GAP_DEVICE_APPEARENCE_VALUE_GENERIC_THERMOMETER                                            768
#define GAP_DEVICE_APPEARENCE_VALUE_THERMOMETER_EAR                                                769
#define GAP_DEVICE_APPEARENCE_VALUE_GENERIC_HEART_RATE_SENSOR                                      832
#define GAP_DEVICE_APPEARENCE_VALUE_BELT_HEART_RATE_SENSOR                                         833
#define GAP_DEVICE_APPEARENCE_VALUE_GENERIC_BLOOD_PRESSURE                                         896
#define GAP_DEVICE_APPEARENCE_VALUE_BLOOD_PRESSURE_ARM                                             897
#define GAP_DEVICE_APPEARENCE_VALUE_BLOOD_PRESSURE_WRIST                                           898
#define GAP_DEVICE_APPEARENCE_VALUE_HUMAN_INTERFACE_DEVICE                                         960
#define GAP_DEVICE_APPEARENCE_VALUE_HID_KEYBOARD                                                   961
#define GAP_DEVICE_APPEARENCE_VALUE_HID_MOUSE                                                      962
#define GAP_DEVICE_APPEARENCE_VALUE_HID_JOYSTICK                                                   963
#define GAP_DEVICE_APPEARENCE_VALUE_HID_GAMEPAD                                                    964
#define GAP_DEVICE_APPEARENCE_VALUE_HID_DIGITIZER_TABLET                                           965
#define GAP_DEVICE_APPEARENCE_VALUE_HID_CARD_READER                                                966
#define GAP_DEVICE_APPEARENCE_VALUE_HID_DIGITAL_PEN                                                967
#define GAP_DEVICE_APPEARENCE_VALUE_HID_BARCODE_SCANNER                                            968
#define GAP_DEVICE_APPEARENCE_VALUE_GENERIC_GLUCOSE_METER                                          1024
#define GAP_DEVICE_APPEARENCE_VALUE_GENERIC_RUNNING_WALKING_SENSOR                                 1088
#define GAP_DEVICE_APPEARENCE_VALUE_RUNNING_WALKING_SENSOR_IN_SHOE                                 1089
#define GAP_DEVICE_APPEARENCE_VALUE_RUNNING_WALKING_SENSOR_ON_SHOE                                 1090
#define GAP_DEVICE_APPEARENCE_VALUE_RUNNING_WALKING_SENSOR_ON_HIP                                  1091
#define GAP_DEVICE_APPEARENCE_VALUE_GENERIC_CYCLING                                                1152
#define GAP_DEVICE_APPEARENCE_VALUE_CYCLING_CYCLING_COMPUTER                                       1153
#define GAP_DEVICE_APPEARENCE_VALUE_CYCLING_SPEED_SENSOR                                           1154
#define GAP_DEVICE_APPEARENCE_VALUE_CYCLING_CADENCE_SENSOR                                         1155
#define GAP_DEVICE_APPEARENCE_VALUE_CYCLING_POWER_SENSOR                                           1156
#define GAP_DEVICE_APPEARENCE_VALUE_CYCLING_SPEED_AND_CADENCE_SENSOR                               1157
#define GAP_DEVICE_APPEARENCE_VALUE_GENERIC_PULSE_OXIMETER                                         3136
#define GAP_DEVICE_APPEARENCE_VALUE_PULSE_OXIMETER_FINGERTIP                                       3137
#define GAP_DEVICE_APPEARENCE_VALUE_PULSE_OXIMETER_WRIST_WORN                                      3138
#define GAP_DEVICE_APPEARENCE_VALUE_GENERIC_WEIGHT_SCALE                                           3200
#define GAP_DEVICE_APPEARENCE_VALUE_GENERIC_OUTDOOR_SPORTS_ACTIVITY                                5184
#define GAP_DEVICE_APPEARENCE_VALUE_OUTDOOR_SPORTS_ACTIVITY_LOCATION_DISPLAY_DEVICE                5185
#define GAP_DEVICE_APPEARENCE_VALUE_OUTDOOR_SPORTS_ACTIVITY_LOCATION_AND_NAVIGATION_DISPLAY_DEVICE 5186
#define GAP_DEVICE_APPEARENCE_VALUE_OUTDOOR_SPORTS_ACTIVITY_LOCATION_POD                           5187
#define GAP_DEVICE_APPEARENCE_VALUE_OUTDOOR_SPORTS_ACTIVITY_LOCATION_AND_NAVIGATION_POD            5188

   /* The following structure represents the format of the GAP          */
   /* Peripheral Preferred Connection Parameters characteristic.        */
typedef __PACKED_STRUCT_BEGIN__ struct _tagGAP_Peripheral_Preferred_Connection_Parameters_t
{
   NonAlignedWord_t Minimum_Connection_Interval;
   NonAlignedWord_t Maximum_Connection_Interval;
   NonAlignedWord_t Slave_Latency;
   NonAlignedWord_t Supervision_Timeout_Multiplier;
} __PACKED_STRUCT_END__ GAP_Peripheral_Preferred_Connection_Parameters_t;

#define GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_DATA_SIZE   (sizeof(GAP_Peripheral_Preferred_Connection_Parameters_t))

#define GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_NO_SPECIFIC_PREFERRED  0xFFFF

#endif
