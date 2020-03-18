/*****< htstypes.h >***********************************************************/
/*      Copyright 2011 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  HTSTypes - Stonestreet One Bluetooth Stack Health Thermometer Service     */
/*             Types.                                                         */
/*                                                                            */
/*  Author:  Tim Cook                                                         */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   10/04/11  T. Cook        Initial creation.                               */
/******************************************************************************/
#ifndef __HTSTYPESH__
#define __HTSTYPESH__

#include "BTTypes.h"            /* Bluetooth Type Definitions.                */
#include "GATTType.h"           /* Bluetooth GATT Type Definitions.           */

   /* The following define the defined HTS Error Codes that may be sent */
   /* in a GATT Error Response.                                         */
#define HTS_ERROR_CODE_OUT_OF_RANGE                      0x80

   /* The following MACRO is a utility MACRO that assigns the Health    */
   /* Thermometer Service 16 bit UUID to the specified UUID_16_t        */
   /* variable.  This MACRO accepts one parameter which is a pointer to */
   /* a UUID_16_t variable that is to receive the HTS UUID Constant     */
   /* value.                                                            */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define HTS_ASSIGN_HTS_SERVICE_UUID_16(_x)               ASSIGN_BLUETOOTH_UUID_16(*((UUID_16_t *)(_x)), 0x18, 0x09)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined HTS Service UUID in UUID16 form.  This     */
   /* MACRO only returns whether the UUID_16_t variable is equal to the */
   /* HTS Service UUID (MACRO returns boolean result) NOT less          */
   /* than/greater than.  The first parameter is the UUID_16_t variable */
   /* to compare to the HTS Service UUID.                               */
#define HTS_COMPARE_HTS_SERVICE_UUID_TO_UUID_16(_x)      COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x18, 0x09)

   /* The following defines the Health Thermometer Service UUID that is */
   /* used when building the HTS Service Table.                         */
#define HTS_SERVICE_BLUETOOTH_UUID_CONSTANT              { 0x09, 0x18 }

   /* The following MACRO is a utility MACRO that assigns the HTS       */
   /* Temperature Measurement Characteristic 16 bit UUID to the         */
   /* specified UUID_16_t variable.  This MACRO accepts one parameter   */
   /* which is the UUID_16_t variable that is to receive the HTS        */
   /* Temperature Measurement UUID Constant value.                      */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define HTS_ASSIGN_TEMPERATURE_MEASUREMENT_UUID_16(_x)               ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x1C)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined HTS Temperature Measurement UUID in UUID16 */
   /* form.  This MACRO only returns whether the UUID_16_t variable is  */
   /* equal to the Temperature Measurement UUID (MACRO returns boolean  */
   /* result) NOT less than/greater than.  The first parameter is the   */
   /* UUID_16_t variable to compare to the HTS Temperature Measurement  */
   /* UUID.                                                             */
#define HTS_COMPARE_HTS_TEMPERATURE_MEASUREMENT_UUID_TO_UUID_16(_x) COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x1C)

   /* The following defines the HTS Temperature Measurement             */
   /* Characteristic UUID that is used when building the HTS Service    */
   /* Table.                                                            */
#define HTS_TEMPERATURE_MEASUREMENT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT { 0x1C, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the HTS       */
   /* Temperature Type Characteristic 16 bit UUID to the specified      */
   /* UUID_16_t variable.  This MACRO accepts one parameter which is the*/
   /* UUID_16_t variable that is to receive the HTS Temperature Type    */
   /* UUID Constant value.                                              */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define HTS_ASSIGN_TEMPERATURE_TYPE_UUID_16(_x)                     ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x1D)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined HTS Temperature Type UUID in UUID16 form.  */
   /* This MACRO only returns whether the UUID_16_t variable is equal to*/
   /* the Temperature Type UUID (MACRO returns boolean result) NOT less */
   /* than/greater than.  The first parameter is the UUID_16_t variable */
   /* to compare to the HTS Temperature Type UUID.                      */
#define HTS_COMPARE_HTS_TEMPERATURE_TYPE_UUID_TO_UUID_16(_x)        COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x1D)

   /* The following defines the HTS Temperature Type Characteristic UUID*/
   /* that is used when building the HTS Service Table.                 */
#define HTS_TEMPERATURE_TYPE_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT { 0x1D, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the HTS       */
   /* Intermediate Temperature Characteristic 16 bit UUID to the        */
   /* specified UUID_16_t variable.  This MACRO accepts one parameter   */
   /* which is the UUID_16_t variable that is to receive the HTS        */
   /* Intermediate Temperature UUID Constant value.                     */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define HTS_ASSIGN_INTERMEDIATE_TEMPERATURE_UUID_16(_x)              ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x1E)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined HTS Intermediate Temperature UUID in UUID16*/
   /* form.  This MACRO only returns whether the UUID_16_t variable is  */
   /* equal to the Intermediate Temperature UUID (MACRO returns boolean */
   /* result) NOT less than/greater than.  The first parameter is the   */
   /* UUID_16_t variable to compare to the HTS Intermediate Temperature */
   /* UUID.                                                             */
#define HTS_COMPARE_HTS_INTERMEDIATE_TEMPERATURE_UUID_TO_UUID_16(_x) COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x1E)

   /* The following defines the HTS Intermediate Temperature            */
   /* Characteristic UUID that is used when building the HTS Service    */
   /* Table.                                                            */
#define HTS_INTERMEDIATE_TEMPERATURE_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT { 0x1E, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the HTS       */
   /* Measurement Interval Characteristic 16 bit UUID to the specified  */
   /* UUID_16_t variable.  This MACRO accepts one parameter which is the*/
   /* UUID_16_t variable that is to receive the HTS Measurement Interval*/
   /* UUID Constant value.                                              */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define HTS_ASSIGN_MEASUREMENT_INTERVAL_UUID_16(_x)                     ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x21)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined HTS Measurement Interval UUID in UUID16    */
   /* form.  This MACRO only returns whether the UUID_16_t variable is  */
   /* equal to the Measurement Interval UUID (MACRO returns boolean     */
   /* result) NOT less than/greater than.  The first parameter is the   */
   /* UUID_16_t variable to compare to the HTS Measurement Interval     */
   /* UUID.                                                             */
#define HTS_COMPARE_HTS_MEASUREMENT_INTERVAL_UUID_TO_UUID_16(_x)        COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x21)

   /* The following defines the HTS Measurement Interval Characteristic */
   /* UUID that is used when building the HTS Service Table.            */
#define HTS_MEASUREMENT_INTERVAL_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT { 0x21, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the HTS Valid */
   /* Range Descriptor 16 bit UUID to the specified UUID_16_t variable. */
   /* This MACRO accepts one parameter which is the UUID_16_t variable  */
   /* that is to receive the HTS Valid Range Descriptor UUID Constant   */
   /* value.                                                            */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define HTS_ASSIGN_VALID_RANGE_DESCRIPTOR_UUID_16(_x)              ASSIGN_BLUETOOTH_UUID_16((_x), 0x29, 0x06)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined HTS Valid Range Descriptor UUID in UUID16  */
   /* form.  This MACRO only returns whether the UUID_16_t variable is  */
   /* equal to the Valid Range Descriptor UUID (MACRO returns boolean   */
   /* result) NOT less than/greater than.  The first parameter is the   */
   /* UUID_16_t variable to compare to the HTS Valid Range Descriptor   */
   /* UUID.                                                             */
#define HTS_COMPARE_HTS_VALID_RANGE_DESCRIPTOR_UUID_TO_UUID_16(_x) COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x29, 0x06)

   /* The following defines the HTS Valid Range Descriptor UUID that is */
   /* used when building the HTS Service Table.                         */
#define HTS_VALID_RANGE_DESCRIPTOR_BLUETOOTH_UUID_CONSTANT         { 0x06, 0x29 }

   /* The following define the valid Temperature Measurement Flags bit  */
   /* that may be set in the Flags field of a Temperature Measurement or*/
   /* Intermediate Temperature value.                                   */
#define HTS_TEMPERATURE_MEASUREMENT_FLAGS_FAHRENHEIT       0x01
#define HTS_TEMPERATURE_MEASUREMENT_FLAGS_TIME_STAMP       0x02
#define HTS_TEMPERATURE_MEASUREMENT_FLAGS_TEMPERATURE_TYPE 0x04

   /* The following define the valid Temperature Type enumerated values */
   /* that may be set as the value for the Temperature Type             */
   /* characteristic value.                                             */
#define HTS_TEMPERATURE_TYPE_ARMPIT                      1
#define HTS_TEMPERATURE_TYPE_BODY                        2
#define HTS_TEMPERATURE_TYPE_EAR                         3
#define HTS_TEMPERATURE_TYPE_FINGER                      4
#define HTS_TEMPERATURE_TYPE_GASTRO_INTESTINAL_TRACT     5
#define HTS_TEMPERATURE_TYPE_MOUTH                       6
#define HTS_TEMPERATURE_TYPE_RECTUM                      7
#define HTS_TEMPERATURE_TYPE_TOE                         8
#define HTS_TEMPERATURE_TYPE_TYMPANUM                    9

   /* The following MACRO is a utility MACRO that exists to valid that a*/
   /* specified Temperature Type is valid.  The only parameter to this  */
   /* function is the Temperature Type to valid.  This MACRO returns    */
   /* TRUE if the Temperature Type is valid or FALSE otherwise.         */
#define HTS_TEMPERATURE_TYPE_VALID_TEMPERATURE_TYPE(_x)       ((((Byte_t)(_x)) >= HTS_TEMPERATURE_TYPE_ARMPIT) && (((Byte_t)(_x)) <= HTS_TEMPERATURE_TYPE_TYMPANUM))

   /* The following structure is used to represent the structure of a   */
   /* temperature value.  This structure is a little-endian structure   */
   /* and must be assigned in little-endian format.                     */
   /* * NOTE * As defined in IEEE-11073 this value consists of a signed */
   /*          8 bit exponent, followed by a signed 24 bit mantissa.    */
   /*          The number is represented by the following formula:      */
   /*                       (mantissa) * (10**exponent)                 */
   /* * NOTE * The most significant byte in this structure represents   */
   /*          the signed 8 bit exponent, the other 3 bytes represent   */
   /*          the signed 24 bit mantissa (in little endian format).    */
   /* * NOTE * For example to represent the value 96.2 the structure    */
   /*          COULD (there is more than 1 representation) be configured*/
   /*          as follows:                                              */
   /*                        Value0   = 0xC2                            */
   /*                        Value1   = 0x03                            */
   /*                        Value2   = 0x00                            */
   /*                        Exponent = 0xFF                            */
typedef __PACKED_STRUCT_BEGIN__ struct _tagHTS_Temperature_Data_t
{
   NonAlignedByte_t Value0;
   NonAlignedByte_t Value1;
   NonAlignedByte_t Value2;
   NonAlignedByte_t Exponent;
} __PACKED_STRUCT_END__ HTS_Temperature_Data_t;

   /* The following MACRO is a utility MACRO that exists to assign the  */
   /* individual Byte values into the specified HTS_Temperature_Data_t  */
   /* variable.  The Bytes are NOT in Little Endian Format, however,    */
   /* they are assigned to the HTS_Temperature_Data_t variable in Little*/
   /* Endian Format.  The first parameter is the HTS_Temperature_Data_t */
   /* variable (of type BD_ADDR_t) to assign, and the next four         */
   /* parameters are the Individual HTS_Temperature_Data_t Byte values  */
   /* to assign to the variable.                                        */
   /* * NOTE * The second parameter to this MACRO (_a) represents the   */
   /*          signed 8 bit exponent to assign into the                 */
   /*          HTS_Temperature_Data_t structure.                        */
   /* * NOTE * The third through fifth parameter specifies the 24 bit   */
   /*          signed mantissa to asssign into the                      */
   /*          HTS_Temperature_Data_t.                                  */
   /* * NOTE * For example to assign the value 96.2 into a              */
   /*          HTS_Temperature_Data_t structure, this MACRO COULD be    */
   /*          invoked as follows (there is more than 1 correct         */
   /*          representation of 96.2):                                 */
   /*                        96.2 = 962 x (10 ** -1)                    */
   /*            ASSIGN_TEMPERATURE_DATA(Dest, 0xFF, 0x00, 0x03, 0xC2)  */
#define ASSIGN_TEMPERATURE_DATA(_dest, _a, _b, _c, _d) \
{                                                      \
   (_dest).Value0   = (NonAlignedByte_t)(_d);          \
   (_dest).Value1   = (NonAlignedByte_t)(_c);          \
   (_dest).Value2   = (NonAlignedByte_t)(_b);          \
   (_dest).Exponent = (NonAlignedByte_t)(_a);          \
}

   /* The following structure defines the format of a Temperature       */
   /* Measurement value that must always be specified in the Temperature*/
   /* Measurement or Intermediate Temperature characteristic value.     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagHTS_Temperature_Measurement_Header_t
{
   NonAlignedByte_t       Flags;
   HTS_Temperature_Data_t Temperature;
} __PACKED_STRUCT_END__ HTS_Temperature_Measurement_Header_t;

#define HTS_TEMPERATURE_MEASUREMENT_HEADER_DATA_SIZE     (sizeof(HTS_Temperature_Measurement_Header_t))

   /* The following structure defines the format of a Temperature       */
   /* Measurement value with an optional Time Stamp that may be set in  */
   /* the Temperature Measurement or Intermediate Temperature           */
   /* characteristic value.                                             */
typedef __PACKED_STRUCT_BEGIN__ struct _tagHTS_Temperature_Measurement_With_Time_Stamp_t
{
   HTS_Temperature_Measurement_Header_t Measurement_Header;
   GATT_Date_Time_Characteristic_t      Time_Stamp;
   NonAlignedByte_t                     Temperature_Type;
} __PACKED_STRUCT_END__ HTS_Temperature_Measurement_With_Time_Stamp_t;

#define HTS_TEMPERATURE_MEASUREMENT_WITH_TIME_STAMP_DATA_SIZE (sizeof(HTS_Temperature_Measurement_With_Time_Stamp_t))

   /* The following structure defines the format of a Temperature       */
   /* Measurement value without an optional Time Stamp that may be set  */
   /* in the Temperature Measurement or Intermediate Temperature        */
   /* characteristic value.                                             */
typedef __PACKED_STRUCT_BEGIN__ struct _tagHTS_Temperature_Measurement_Without_Time_Stamp_t
{
   HTS_Temperature_Measurement_Header_t Measurement_Header;
   NonAlignedByte_t                     Temperature_Type;
} __PACKED_STRUCT_END__ HTS_Temperature_Measurement_Without_Time_Stamp_t;

#define HTS_TEMPERATURE_MEASUREMENT_WITHOUT_TIME_STAMP_DATA_SIZE (sizeof(HTS_Temperature_Measurement_Without_Time_Stamp_t))

   /* The following MACRO is a utility MACRO that exists to aid in      */
   /* calculating the size of a Temperature Measurement value based on  */
   /* the Temperature Measurement Flags.  The only parameter to this    */
   /* MACRO is the Temperature Measurement Flags.                       */
#define HTS_TEMPERATURE_MEASUREMENT_LENGTH(_x)           (HTS_TEMPERATURE_MEASUREMENT_HEADER_DATA_SIZE + (((_x) & HTS_TEMPERATURE_MEASUREMENT_FLAGS_TEMPERATURE_TYPE)?BYTE_SIZE:0) + (((_x) & HTS_TEMPERATURE_MEASUREMENT_FLAGS_TIME_STAMP)?GATT_DATE_TIME_CHARACTERISTIC_DATA_SIZE:0))

   /* The following structure defines the format of a Valid Range value */
   /* that may be sent or received.                                     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagHTS_Valid_Range_t
{
   NonAlignedWord_t Lower_Bounds;
   NonAlignedWord_t Upper_Bounds;
} __PACKED_STRUCT_END__ HTS_Valid_Range_t;

#define HTS_VALID_RANGE_SIZE                             (sizeof(HTS_Valid_Range_t))

   /* The following defines the length of the Temperature Type          */
   /* characteristic value.                                             */
#define HTS_TEMPERATURE_TYPE_VALUE_LENGTH                (BYTE_SIZE)

   /* The following defines the length of the Measurement Interval      */
   /* characteristic value.                                             */
#define HTS_MEASUREMENT_INTERVAL_VALUE_LENGTH            (WORD_SIZE)

   /* The following defines the HTS GATT Service Flags MASK that should */
   /* be passed into GATT_Register_Service when the HTS Service is      */
   /* registered.                                                       */
#define HTS_SERVICE_FLAGS                                (GATT_SERVICE_FLAGS_LE_SERVICE)

#endif
