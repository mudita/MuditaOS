/*****< hidstypes.h >**********************************************************/
/*      Copyright 2012 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  HIDSTypes - Stonestreet One Bluetooth Stack HID Service Types.            */
/*                                                                            */
/*  Author:  Tim Thomas                                                       */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   09/01/12  T. Thomas      Initial creation.                               */
/******************************************************************************/
#ifndef __HIDSTYPESH__
#define __HIDSTYPESH__

#include "BTTypes.h"            /* Bluetooth Type Definitions.                */

   /* The following defines the HID Specification Version that is       */
   /* reported in the HID Information field.  The value is formatted as */
   /* 0xJJMN for version JJ.M.N (JJ - major version number, M - minor   */
   /* version number, N - sub-minor version number)                     */
#define HIDS_HID_VERSION_NUMBER                                0x0111

   /* The following defines the Localization Byte value that will be    */
   /* assigned in the HID Information attribute.                        */
#define HIDS_HID_LOCALIZATION_BYTE_NO_LOCALIZATION             0x00

   /* The following MACRO is a utility MACRO that assigns the Human     */
   /* Interface Device Service 16 bit UUID to the specified UUID_16_t   */
   /* variable.  This MACRO accepts one parameter which is a pointer to */
   /* a UUID_16_t variable that is to receive the HIDS UUID Constant    */
   /* value.                                                            */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define HIDS_ASSIGN_HIDS_SERVICE_UUID_16(_x)                   ASSIGN_BLUETOOTH_UUID_16(*((UUID_16_t *)(_x)), 0x18, 0x12)

   /* The following MACRO is a utility MACRO that exists to compare a   */
   /* UUID 16 to the defined HIDS Service UUID in UUID16 form.  This    */
   /* MACRO only returns whether the UUID_16_t variable is equal to the */
   /* HIDS Service UUID (MACRO returns boolean result) NOT less         */
   /* than/greater than.  The first parameter is the UUID_16_t variable */
   /* to compare to the HIDS Service UUID.                              */
#define HIDS_COMPARE_HIDS_SERVICE_UUID_TO_UUID_16(_x)          COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x18, 0x12)

   /* The following defines the Human Interface Device Service UUID that*/
   /* is used when building the HIDS Service Table.                     */
#define HIDS_SERVICE_BLUETOOTH_UUID_CONSTANT                   { 0x12, 0x18 }

   /* The following MACRO is a utility MACRO that assigns the HIDS      */
   /* Protocol Mode Characteristic 16 bit UUID to the specified         */
   /* UUID_16_t variable.  This MACRO accepts one parameter which is the*/
   /* UUID_16_t variable that is to receive the HIDS Protocol Mode      */
   /* UUID Constant value.                                              */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define HIDS_ASSIGN_PROTOCOL_MODE_UUID_16(_x)                     ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x4E)

   /* The following MACRO is a utility MACRO that exists to compare a   */
   /* UUID 16 to the defined HIDS Protocol Mode UUID in UUID16 form.    */
   /* This MACRO only returns whether the UUID_16_t variable is equal to*/
   /* the Protocol Mode UUID (MACRO returns boolean result) NOT less    */
   /* than/greater than.  The first parameter is the UUID_16_t variable */
   /* to compare to the HIDS Protocol Mode UUID.                        */
#define HIDS_COMPARE_HIDS_PROTOCOL_MODE_UUID_TO_UUID_16(_x)       COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x4E)

   /* The following defines the HIDS Protocol Mode Characteristic UUID  */
   /* that is used when building the HIDS Service Table.                */
#define HIDS_PROTOCOL_MODE_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT { 0x4E, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the HIDS      */
   /* Report Map Characteristic 16 bit UUID to the specified UUID_16_t  */
   /* variable.  This MACRO accepts one parameter which is the UUID_16_t*/
   /* variable that is to receive the HIDS Report Map UUID Constant     */
   /* value.                                                            */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define HIDS_ASSIGN_REPORT_MAP_UUID_16(_x)                     ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x4B)

   /* The following MACRO is a utility MACRO that exists to compare a   */
   /* UUID 16 to the defined HIDS Report Map UUID in UUID16 form.  This */
   /* MACRO only returns whether the UUID_16_t variable is equal to the */
   /* Report Map UUID (MACRO returns boolean result) NOT less           */
   /* than/greater than.  The first parameter is the UUID_16_t variable */
   /* to compare to the HIDS Report Map UUID.                           */
#define HIDS_COMPARE_HIDS_REPORT_MAP_UUID_TO_UUID_16(_x)       COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x4B)

   /* The following defines the HIDS Report Map Characteristic UUID that*/
   /* is used when building the HIDS Service Table.                     */
#define HIDS_REPORT_MAP_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT { 0x4B, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the HIDS      */
   /* Report Characteristic 16 bit UUID to the specified UUID_16_t      */
   /* variable.  This MACRO accepts one parameter which is the UUID_16_t*/
   /* variable that is to receive the HIDS Report UUID Constant value.  */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define HIDS_ASSIGN_REPORT_UUID_16(_x)                         ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x4D)

   /* The following MACRO is a utility MACRO that exists to compare a   */
   /* UUID 16 to the defined HIDS Report UUID in UUID16 form.  This     */
   /* MACRO only returns whether the UUID_16_t variable is equal to the */
   /* Report UUID (MACRO returns boolean result) NOT less than/greater  */
   /* than.  The first parameter is the UUID_16_t variable to compare to*/
   /* the HIDS Report UUID.                                             */
#define HIDS_COMPARE_HIDS_REPORT_UUID_TO_UUID_16(_x)           COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x4D)

   /* The following defines the HIDS Report Characteristic UUID that is */
   /* used when building the HIDS Service Table.                        */
#define HIDS_REPORT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT     { 0x4D, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the HIDS Boot */
   /* Keyboard Input Report Characteristic 16 bit UUID to the specified */
   /* UUID_16_t variable.  This MACRO accepts one parameter which is the*/
   /* UUID_16_t variable that is to receive the HIDS Boot Keyboard Input*/
   /* Report UUID Constant value.                                       */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define HIDS_ASSIGN_BOOT_KEYBOARD_INPUT_REPORT_UUID_16(_x)                      ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x22)

   /* The following MACRO is a utility MACRO that exists to compare a   */
   /* UUID 16 to the defined HIDS Boot Keyboard Input Report UUID in    */
   /* UUID16 form.  This MACRO only returns whether the UUID_16_t       */
   /* variable is equal to the Boot Keyboard Input Report UUID (MACRO   */
   /* returns boolean result) NOT less than/greater than.  The first    */
   /* parameter is the UUID_16_t variable to compare to the HIDS Boot   */
   /* Keyboard Input Report UUID.                                       */
#define HIDS_COMPARE_HIDS_BOOT_KEYBOARD_INPUT_REPORT_UUID_TO_UUID_16(_x)        COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x22)

   /* The following defines the HIDS Boot Keyboard Input Report         */
   /* Characteristic UUID that is used when building the HIDS Service   */
   /* Table.                                                            */
#define HIDS_BOOT_KEYBOARD_INPUT_REPORT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0x22, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the HIDS Boot */
   /* Keyboard Output Report Characteristic 16 bit UUID to the specified*/
   /* UUID_16_t variable.  This MACRO accepts one parameter which is the*/
   /* UUID_16_t variable that is to receive the HIDS Boot Keyboard      */
   /* Output Report UUID Constant value.                                */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define HIDS_ASSIGN_BOOT_KEYBOARD_OUTPUT_REPORT_UUID_16(_x)                         ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x32)

   /* The following MACRO is a utility MACRO that exists to compare a   */
   /* UUID 16 to the defined HIDS Boot Keyboard Output Report UUID in   */
   /* UUID16 form.  This MACRO only returns whether the UUID_16_t       */
   /* variable is equal to the Boot Keyboard Output Report UUID (MACRO  */
   /* returns boolean result) NOT less than/greater than.  The first    */
   /* parameter is the UUID_16_t variable to compare to the HIDS Boot   */
   /* Keyboard Output Report UUID.                                      */
#define HIDS_COMPARE_HIDS_BOOT_KEYBOARD_OUTPUT_REPORT_UUID_TO_UUID_16(_x)           COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x32)

   /* The following defines the HIDS Boot Keyboard Output Report        */
   /* Characteristic UUID that is used when building the HIDS Service   */
   /* Table.                                                            */
#define HIDS_BOOT_KEYBOARD_OUTPUT_REPORT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT     { 0x32, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the HIDS Boot */
   /* Mouse Input Report Characteristic 16 bit UUID to the specified    */
   /* UUID_16_t variable.  This MACRO accepts one parameter which is the*/
   /* UUID_16_t variable that is to receive the HIDS Boot Mouse Input   */
   /* Report UUID Constant value.                                       */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define HIDS_ASSIGN_BOOT_MOUSE_INPUT_REPORT_UUID_16(_x)                         ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x33)

   /* The following MACRO is a utility MACRO that exists to compare a   */
   /* UUID 16 to the defined HIDS Boot Mouse Input Report UUID in UUID16*/
   /* form.  This MACRO only returns whether the UUID_16_t variable is  */
   /* equal to the Boot Mouse Input Report UUID (MACRO returns boolean  */
   /* result) NOT less than/greater than.  The first parameter is the   */
   /* UUID_16_t variable to compare to the HIDS Boot Mouse Input Report */
   /* UUID.                                                             */
#define HIDS_COMPARE_HIDS_BOOT_MOUSE_INPUT_REPORT_UUID_TO_UUID_16(_x)           COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x33)

   /* The following defines the HIDS Boot Mouse Input Report            */
   /* Characteristic UUID that is used when building the HIDS Service   */
   /* Table.                                                            */
#define HIDS_BOOT_MOUSE_INPUT_REPORT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT     { 0x33, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the HIDS HID  */
   /* Information Characteristic 16 bit UUID to the specified UUID_16_t */
   /* variable.  This MACRO accepts one parameter which is the UUID_16_t*/
   /* variable that is to receive the HIDS HID Information UUID Constant*/
   /* value.                                                            */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define HIDS_ASSIGN_HID_INFORMATION_UUID_16(_x)                         ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x4A)

   /* The following MACRO is a utility MACRO that exists to compare a   */
   /* UUID 16 to the defined HIDS HID Information UUID in UUID16 form.  */
   /* This MACRO only returns whether the UUID_16_t variable is equal to*/
   /* the HID Information UUID (MACRO returns boolean result) NOT less  */
   /* than/greater than.  The first parameter is the UUID_16_t variable */
   /* to compare to the HIDS HID Information UUID.                      */
#define HIDS_COMPARE_HIDS_HID_INFORMATION_UUID_TO_UUID_16(_x)           COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x4A)

   /* The following defines the HIDS HID Information Characteristic UUID*/
   /* that is used when building the HIDS Service Table.                */
#define HIDS_HID_INFORMATION_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT     { 0x4A, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the HIDS HID  */
   /* Control Point Characteristic 16 bit UUID to the specified         */
   /* UUID_16_t variable.  This MACRO accepts one parameter which is the*/
   /* UUID_16_t variable that is to receive the HIDS HID Control Point  */
   /* UUID Constant value.                                              */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define HIDS_ASSIGN_HID_CONTROL_POINT_UUID_16(_x)                       ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x4C)

   /* The following MACRO is a utility MACRO that exists to compare a   */
   /* UUID 16 to the defined HIDS HID Control Point UUID in UUID16 form.*/
   /* This MACRO only returns whether the UUID_16_t variable is equal to*/
   /* the HID Control Point UUID (MACRO returns boolean result) NOT less*/
   /* than/greater than.  The first parameter is the UUID_16_t variable */
   /* to compare to the HIDS HID Control Point UUID.                    */
#define HIDS_COMPARE_HIDS_HID_CONTROL_POINT_UUID_TO_UUID_16(_x)         COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x4C)

   /* The following defines the HIDS HID Control Point Characteristic   */
   /* UUID that is used when building the HIDS Service Table.           */
#define HIDS_HID_CONTROL_POINT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT   { 0x4C, 0x2A }

   /* HID Characteristic Descriptor Attribute Types.                    */

   /* The following MACRO is a utility MACRO that assigns the HIDS      */
   /* Profile Report Reference Descriptor UUID.  This MACRO accepts one */
   /* parameter which is the UUID_16_t variable that is to receive the  */
   /* Report Reference Descriptor UUID Constant value.                  */
#define HIDS_ASSIGN_REPORT_REFERENCE_DESCRIPTOR_UUID_16(_x)             ASSIGN_BLUETOOTH_UUID_16((_x), 0x29, 0x08)

   /* The following MACRO is a utility MACRO that exists to compare an  */
   /* Descriptor UUID to the defined Report Reference Descriptor UUID in*/
   /* UUID16 form.  This MACRO only returns whether the UUID_16_t       */
   /* variable is equal to the Report Reference Descriptor UUID (MACRO  */
   /* returns boolean result) NOT less than/greater than.  The first    */
   /* parameter is the UUID_16_t variable to compare to the Report      */
   /* Reference Descriptor UUID.                                        */
#define HIDS_COMPARE_REPORT_REFERENCE_DESCRIPTOR_UUID_TO_UUID_16(_x)    COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x29, 0x08)

   /* The following defines the HIDS Report Reference Descriptor UUID   */
   /* that is used when building the HIDS Service Table.                */
#define HIDS_REPORT_REFERENCE_DESCRIPTOR_BLUETOOTH_UUID_CONSTANT        { 0x08, 0x29 }

   /* The following MACRO is a utility MACRO that assigns the HIDS      */
   /* External Report Reference Descriptor UUID.  This MACRO accepts one*/
   /* parameter which is the UUID_16_t variable that is to receive the  */
   /* External Report Reference Descriptor UUID  Constant value.        */
#define HIDS_ASSIGN_EXTERNAL_REPORT_REFERENCE_DESCRIPTOR_UUID(_x)             ASSIGN_BLUETOOTH_UUID_16((_x), 0x29, 0x07)

   /* The following MACRO is a utility MACRO that exists to compare an  */
   /* Attribute Type to the defined Characteristic External Report      */
   /* Reference UUID in UUID16 form.  This MACRO only returns whether   */
   /* the UUID_16_t variable is equal to the External Report Reference  */
   /* UUID (MACRO returns boolean result) NOT less than/greater than.   */
   /* The first parameter is the UUID_16_t variable to compare to the   */
   /* External Report Reference UUID.                                   */
#define HIDS_COMPARE_EXTERNAL_REPORT_REFERENCE_DESCRIPTOR_UUID_TO_UUID_16(_x) COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x29, 0x07)

   /* The following defines the HIDS HID External Report Reference      */
   /* Descriptor UUID that is used when building the HIDS Service Table.*/
#define HIDS_EXTERNAL_REPORT_REFERENCE_DESCRIPTOR_BLUETOOTH_UUID_CONSTANT     { 0x07, 0x29 }

   /* The following define the valid Protocol Mode vales that may be set*/
   /* in the Protocol Mode value.                                       */
#define HIDS_PROTOCOL_MODE_BOOT                                    0x00
#define HIDS_PROTOCOL_MODE_REPORT                                  0x01

   /* The following defines the length of the HID Protocol Mode         */
   /* characteristic value.                                             */
#define HIDS_PROTOCOL_MODE_VALUE_LENGTH                            (NON_ALIGNED_BYTE_SIZE)

   /* The following define the valid Report Type vales that may be set  */
   /* in the Report Reference Type value.                               */
#define HIDS_REPORT_REFERENCE_REPORT_TYPE_INPUT_REPORT             0x01
#define HIDS_REPORT_REFERENCE_REPORT_TYPE_OUTPUT_REPORT            0x02
#define HIDS_REPORT_REFERENCE_REPORT_TYPE_FEATURE_REPORT           0x03

   /* The following define the valid HID Information Flags bit that may */
   /* be set in the Flags field of a HID Information.                   */
#define HIDS_HID_INFORMATION_FLAGS_REMOTE_WAKE                     0x01
#define HIDS_HID_INFORMATION_FLAGS_NORMALLY_CONNECTABLE            0x02

   /* The following define the valid Control Point Command values that  */
   /* may be set in the Control Point value.                            */
#define HIDS_CONTROL_POINT_COMMAND_SUSPEND                         0x00
#define HIDS_CONTROL_POINT_COMMAND_EXIT_SUSPEND                    0x01

   /* The following defines the length of the HID Control Point         */
   /* characteristic value.                                             */
#define HIDS_CONTROL_POINT_VALUE_LENGTH                            (NON_ALIGNED_BYTE_SIZE)

   /* The following structure is used to represent the structure of a   */
   /* HID Information value.                                            */
typedef __PACKED_STRUCT_BEGIN__ struct _tagHIDS_HID_Information_t
{
   NonAlignedWord_t HID_Version;
   NonAlignedByte_t CountryCode;
   NonAlignedByte_t Flags;
} __PACKED_STRUCT_END__ HIDS_HID_Information_t;

#define HIDS_HID_INFORMATION_SIZE               (sizeof(HIDS_HID_Information_t))

   /* The following structure defines the format of a Report Reference  */
   /* value.  The report reference is used to indicate the type of HID  */
   /* report that is currently configured.                              */
typedef __PACKED_STRUCT_BEGIN__ struct _tagHIDS_Report_Reference_t
{
   NonAlignedByte_t Report_ID;
   NonAlignedByte_t Report_Type;
} __PACKED_STRUCT_END__ HIDS_Report_Reference_t;

#define HIDS_REPORT_REFERENCE_SIZE              (sizeof(HIDS_Report_Reference_t))

   /* The following defines the HIDS GATT Service Flags MASK that should*/
   /* be passed into GATT_Register_Service when the HIDS Service is     */
   /* registered.                                                       */
#define HIDS_SERVICE_FLAGS                                    (GATT_SERVICE_FLAGS_LE_SERVICE)

#endif
