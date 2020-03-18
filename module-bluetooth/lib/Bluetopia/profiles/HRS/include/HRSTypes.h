/*****< hrstypes.h >***********************************************************/
/*      Copyright 2011 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  HRSTypes - Stonestreet One Bluetooth Stack Heart Rate Service Types.      */
/*                                                                            */
/*  Author:  Tim Cook                                                         */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   11/28/11  T. Cook        Initial creation.                               */
/******************************************************************************/
#ifndef __HRSTYPESH__
#define __HRSTYPESH__

#include "BTTypes.h"            /* Bluetooth Type Definitions.                */

   /* The following define the defined HRS Error Codes that may be sent */
   /* in a GATT Error Response.                                         */
#define HRS_ERROR_CODE_CONTROL_POINT_NOT_SUPPORTED       0x80

   /* The following MACRO is a utility MACRO that assigns the Heart Rate*/
   /* Service 16 bit UUID to the specified UUID_16_t variable.  This    */
   /* MACRO accepts one parameter which is a pointer to a UUID_16_t     */
   /* variable that is to receive the HRS UUID Constant value.          */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define HRS_ASSIGN_HRS_SERVICE_UUID_16(_x)               ASSIGN_BLUETOOTH_UUID_16(*((UUID_16_t *)(_x)), 0x18, 0x0D)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined HRS Service UUID in UUID16 form.  This     */
   /* MACRO only returns whether the UUID_16_t variable is equal to the */
   /* HRS Service UUID (MACRO returns boolean result) NOT less          */
   /* than/greater than.  The first parameter is the UUID_16_t variable */
   /* to compare to the HRS Service UUID.                               */
#define HRS_COMPARE_HRS_SERVICE_UUID_TO_UUID_16(_x)      COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x18, 0x0D)

   /* The following defines the Heart Rate Service UUID that is used    */
   /* when building the HRS Service Table.                              */
#define HRS_SERVICE_BLUETOOTH_UUID_CONSTANT              { 0x0D, 0x18 }

   /* The following MACRO is a utility MACRO that assigns the HRS Heart */
   /* Rate Measurement Characteristic 16 bit UUID to the specified      */
   /* UUID_16_t variable.  This MACRO accepts one parameter which is the*/
   /* UUID_16_t variable that is to receive the HRS Heart Rate          */
   /* Measurement UUID Constant value.                                  */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define HRS_ASSIGN_HEART_RATE_MEASUREMENT_UUID_16(_x)    ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x37)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined HRS Heart Rate Measurement UUID in UUID16  */
   /* form.  This MACRO only returns whether the UUID_16_t variable is  */
   /* equal to the Heart Rate Measurement UUID (MACRO returns boolean   */
   /* result) NOT less than/greater than.  The first parameter is the   */
   /* UUID_16_t variable to compare to the HRS Heart Rate Measurement   */
   /* UUID.                                                             */
#define HRS_COMPARE_HRS_HEART_RATE_MEASUREMENT_UUID_TO_UUID_16(_x) COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x37)

   /* The following defines the HRS Heart Rate Measurement              */
   /* Characteristic UUID that is used when building the HRS Service    */
   /* Table.                                                            */
#define HRS_HEART_RATE_MEASUREMENT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT { 0x37, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the HRS Body  */
   /* Sensor Location Characteristic 16 bit UUID to the specified       */
   /* UUID_16_t variable.  This MACRO accepts one parameter which is the*/
   /* UUID_16_t variable that is to receive the HRS Body Sensor Location*/
   /* UUID Constant value.                                              */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define HRS_ASSIGN_BODY_SENSOR_LOCATION_UUID_16(_x)                     ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x38)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined HRS Body Sensor Location UUID in UUID16    */
   /* form.  This MACRO only returns whether the UUID_16_t variable is  */
   /* equal to the Body Sensor Location UUID (MACRO returns boolean     */
   /* result) NOT less than/greater than.  The first parameter is the   */
   /* UUID_16_t variable to compare to the HRS Body Sensor Location     */
   /* UUID.                                                             */
#define HRS_COMPARE_HRS_BODY_SENSOR_LOCATION_UUID_TO_UUID_16(_x)        COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x38)

   /* The following defines the HRS Body Sensor Location Characteristic */
   /* UUID that is used when building the HRS Service Table.            */
#define HRS_BODY_SENSOR_LOCATION_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT { 0x38, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the HRS Heart */
   /* Rate Control Point Characteristic 16 bit UUID to the specified    */
   /* UUID_16_t variable.  This MACRO accepts one parameter which is the*/
   /* UUID_16_t variable that is to receive the HRS Heart Rate Control  */
   /* Point UUID Constant value.                                        */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define HRS_ASSIGN_HEART_RATE_CONTROL_POINT_UUID_16(_x)              ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x39)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined HRS Heart Rate Control Point UUID in UUID16*/
   /* form.  This MACRO only returns whether the UUID_16_t variable is  */
   /* equal to the Heart Rate Control Point UUID (MACRO returns boolean */
   /* result) NOT less than/greater than.  The first parameter is the   */
   /* UUID_16_t variable to compare to the HRS Heart Rate Control Point */
   /* UUID.                                                             */
#define HRS_COMPARE_HRS_HEART_RATE_CONTROL_POINT_UUID_TO_UUID_16(_x) COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x39)

   /* The following defines the HRS Heart Rate Control Point            */
   /* Characteristic UUID that is used when building the HRS Service    */
   /* Table.                                                            */
#define HRS_HEART_RATE_CONTROL_POINT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT { 0x39, 0x2A }

   /* The following define the valid Heart Rate Measurement Flags bit   */
   /* that may be set in the Flags field of a Heart Rate Measurement.   */
#define HRS_HEART_RATE_MEASUREMENT_FLAGS_HEART_RATE_IS_WORD                0x01
#define HRS_HEART_RATE_MEASUREMENT_FLAGS_SENSOR_CONTACT_STATUS_DETECTED    0x02
#define HRS_HEART_RATE_MEASUREMENT_FLAGS_SENSOR_CONTACT_STATUS_SUPPORTED   0x04
#define HRS_HEART_RATE_MEASUREMENT_FLAGS_ENERGY_EXPENDED_PRESENT           0x08
#define HRS_HEART_RATE_MEASUREMENT_FLAGS_RR_INTERVAL_PRESENT               0x10

   /* The following define the valid Body Sensor Location enumerated    */
   /* values that may be set as the value for the Body Sensor Location  */
   /* characteristic value.                                             */
#define HRS_BODY_SENSOR_LOCATION_OTHER                   0
#define HRS_BODY_SENSOR_LOCATION_CHEST                   1
#define HRS_BODY_SENSOR_LOCATION_WRIST                   2
#define HRS_BODY_SENSOR_LOCATION_FINGER                  3
#define HRS_BODY_SENSOR_LOCATION_HAND                    4
#define HRS_BODY_SENSOR_LOCATION_EAR_LOBE                5
#define HRS_BODY_SENSOR_LOCATION_FOOT                    6

   /* The following MACRO is a utility MACRO that exists to valid that a*/
   /* specified Body Sensor Location is valid.  The only parameter to   */
   /* this function is the Body Sensor Location to valid.  This MACRO   */
   /* returns TRUE if the Body Sensor Location is valid or FALSE        */
   /* otherwise.                                                        */
#define HRS_BODY_SENSOR_LOCATION_VALID_TEMPERATURE_TYPE(_x)       ((((Byte_t)(_x)) >= HRS_BODY_SENSOR_LOCATION_OTHER) && (((Byte_t)(_x)) <= HRS_BODY_SENSOR_LOCATION_FOOT))

   /* The following define the valid Heart Rate Control Point commands  */
   /* that may be written to the Heart Rate Control Point characteristic*/
   /* value.                                                            */
#define HRS_HEART_RATE_CONTROL_POINT_RESET_ENERGY_EXPENDED        0x01

   /* The following MACRO is a utility MACRO that exists to determine if*/
   /* the value written to the Heart Rate Control Point is a valid      */
   /* command.                                                          */
#define HRS_HEART_RATE_CONTROL_POINT_VALID_COMMAND(_x)           (((Byte_t)(_x)) == HRS_HEART_RATE_CONTROL_POINT_RESET_ENERGY_EXPENDED)

   /* The following structure represents the header of the Heart Rate   */
   /* Measurement value that is present in every Heart Rate Measurement.*/
typedef __PACKED_STRUCT_BEGIN__ struct _tagHRS_Heart_Rate_Measurement_Header_t
{
   NonAlignedByte_t  Flags;
} __PACKED_STRUCT_END__ HRS_Heart_Rate_Measurement_Header_t;

#define HRS_HEART_RATE_MEASUREMENT_HEADER_DATA_SIZE      (sizeof(HRS_Heart_Rate_Measurement_Header_t))

   /* The following structure represents a Generic Heart Rate           */
   /* Measurement value with the variable data that it contains.        */
typedef __PACKED_STRUCT_BEGIN__ struct _tagHRS_Heart_Rate_Measurement_t
{
   HRS_Heart_Rate_Measurement_Header_t Header;
   NonAlignedByte_t                    Variable_Data[1];
} __PACKED_STRUCT_END__ HRS_Heart_Rate_Measurement_t;

#define HRS_HEART_RATE_MEASUREMENT_SIZE(_x)         (BTPS_STRUCTURE_OFFSET(HRS_Heart_Rate_Measurement_t, Variable_Data) + ((_x)*BYTE_SIZE))

   /* The following MACRO is a utility MACRO that exists to aid in      */
   /* calculating the minimum size of a Heart Rate Measurement value    */
   /* based on the Heart Rate Measurement Flags.  The only parameter to */
   /* this MACRO is the Heart Rate Measurement Flags.                   */
#define HRS_HEART_RATE_MEASUREMENT_MINIMUM_LENGTH(_x)    (HRS_HEART_RATE_MEASUREMENT_HEADER_DATA_SIZE + (((_x) & HRS_HEART_RATE_MEASUREMENT_FLAGS_HEART_RATE_IS_WORD)?WORD_SIZE:BYTE_SIZE) + (((_x) & HRS_HEART_RATE_MEASUREMENT_FLAGS_ENERGY_EXPENDED_PRESENT)?WORD_SIZE:0) + (((_x) & HRS_HEART_RATE_MEASUREMENT_FLAGS_RR_INTERVAL_PRESENT)?WORD_SIZE:0))

   /* The following MACRO is a utility MACRO that exists to aid in      */
   /* calculating the length of a Heart Rate Measurement value based on */
   /* the Heart Rate Measurement Flags for a given number of RR         */
   /* Intervals.  The first parameter to this MACRO is the Heart Rate   */
   /* Measurement Flags.  The final parameter to this macro is the      */
   /* number of RR Intervals in the Heart Rate Measurement.             */
#define HRS_HEART_RATE_MEASUREMENT_LENGTH(_x, _y)        (HRS_HEART_RATE_MEASUREMENT_HEADER_DATA_SIZE + (((_x) & HRS_HEART_RATE_MEASUREMENT_FLAGS_HEART_RATE_IS_WORD)?WORD_SIZE:BYTE_SIZE) + (((_x) & HRS_HEART_RATE_MEASUREMENT_FLAGS_ENERGY_EXPENDED_PRESENT)?WORD_SIZE:0) + (((_x) & HRS_HEART_RATE_MEASUREMENT_FLAGS_RR_INTERVAL_PRESENT)?((_y)*WORD_SIZE):0))

   /* The following defines the length of the Body Sensor Location      */
   /* characteristic value.                                             */
#define HRS_BODY_SENSOR_LOCATION_VALUE_LENGTH            (BYTE_SIZE)

   /* The following defines the length of the Heart Rate Control Point  */
   /* characteristic value.                                             */
#define HRS_HEART_RATE_CONTROL_POINT_VALUE_LENGTH        (BYTE_SIZE)

   /* The following defines the HRS GATT Service Flags MASK that should */
   /* be passed into GATT_Register_Service when the HRS Service is      */
   /* registered.                                                       */
#define HRS_SERVICE_FLAGS                                (GATT_SERVICE_FLAGS_LE_SERVICE)

#endif
