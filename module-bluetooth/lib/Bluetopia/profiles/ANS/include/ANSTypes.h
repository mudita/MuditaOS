/*****< anstype.h >************************************************************/
/*      Copyright 2011 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  ANSType - Stonestreet One Bluetooth Stack Alert Notification Service      */
/*            Types.                                                          */
/*                                                                            */
/*  Author:  Tim Cook                                                         */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   09/26/11  T. Cook        Initial creation.                               */
/******************************************************************************/
#ifndef __ANSTYPEH__
#define __ANSTYPEH__

#include "BTTypes.h"            /* Bluetooth Type Definitions.                */

   /* The following defines the Alert Notification ATT Error Codes that */
   /* are define in the ATT Application Error code range.               */
#define ANS_ERROR_CODE_COMMAND_NOT_SUPPORTED           0xA0

   /* The following MACRO is a utility MACRO that assigns the Phone     */
   /* Alert Status Service 16 bit UUID to the specified UUID_16_t       */
   /* variable.  This MACRO accepts one parameter which is a pointer to */
   /* a UUID_16_t variable that is to receive the ANS UUID Constant     */
   /* value.                                                            */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define ANS_ASSIGN_ANS_SERVICE_UUID_16(_x)               ASSIGN_BLUETOOTH_UUID_16(*((UUID_16_t *)(_x)), 0x18, 0x11)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined ANS Service UUID in UUID16 form.  This     */
   /* MACRO only returns whether the UUID_16_t variable is equal to the */
   /* ANS Service UUID (MACRO returns boolean result) NOT less          */
   /* than/greater than.  The first parameter is the UUID_16_t variable */
   /* to compare to the ANS Service UUID.                               */
#define ANS_COMPARE_ANS_SERVICE_UUID_TO_UUID_16(_x)      COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x18, 0x11)

   /* The following defines the Phone Alert Status Service UUID that is */
   /* used when building the ANS Service Table.                         */
#define ANS_SERVICE_BLUETOOTH_UUID_CONSTANT              { 0x11, 0x18 }

   /* The following MACRO is a utility MACRO that assigns the ANS Alert */
   /* Status Characteristic 16 bit UUID to the specified UUID_16_t      */
   /* variable.  This MACRO accepts one parameter which is the UUID_16_t*/
   /* variable that is to receive the ANS Supported New Alert Category  */
   /* UUID Constant value.                                              */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define ANS_ASSIGN_SUPORTED_NEW_ALERT_CATEGORY_UUID_16(_x) ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x47)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined ANS Supported New Alert Category UUID in   */
   /* UUID16 form.  This MACRO only returns whether the UUID_16_t       */
   /* variable is equal to the Supported New Alert Category UUID (MACRO */
   /* returns boolean result) NOT less than/greater than.  The first    */
   /* parameter is the UUID_16_t variable to compare to the ANS         */
   /* Supported New Alert Category UUID.                                */
#define ANS_COMPARE_ANS_SUPORTED_NEW_ALERT_CATEGORY_UUID_TO_UUID_16(_x) COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x47)

   /* The following defines the ANS Supported New Alert Category        */
   /* Characteristic UUID that is used when building the ANS Service    */
   /* Table.                                                            */
#define ANS_SUPORTED_NEW_ALERT_CATEGORY_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT { 0x47, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the ANS New   */
   /* Alert Characteristic 16 bit UUID to the specified UUID_16_t       */
   /* variable.  This MACRO accepts one parameter which is the UUID_16_t*/
   /* variable that is to receive the ANS New Alert UUID Constant value.*/
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define ANS_ASSIGN_NEW_ALERT_UUID_16(_x)                       ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x46)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined ANS New Alert UUID in UUID16 form.  This   */
   /* MACRO only returns whether the UUID_16_t variable is equal to the */
   /* New Alert UUID (MACRO returns boolean result) NOT less            */
   /* than/greater than.  The first parameter is the UUID_16_t variable */
   /* to compare to the ANS New Alert UUID.                             */
#define ANS_COMPARE_ANS_NEW_ALERT_UUID_TO_UUID_16(_x)          COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x46)

   /* The following defines the ANS New Alert Characteristic UUID that  */
   /* is used when building the ANS Service Table.                      */
#define ANS_NEW_ALERT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT   { 0x46, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the ANS New   */
   /* Alert Characteristic 16 bit UUID to the specified UUID_16_t       */
   /* variable.  This MACRO accepts one parameter which is the UUID_16_t*/
   /* variable that is to receive the ANS Supported Unread Alert        */
   /* Category UUID Constant value.                                     */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define ANS_ASSIGN_SUPPORTED_UNREAD_ALERT_CATEGORY_UUID_16(_x) ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x48)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined ANS Supported Unread Alert Category UUID in*/
   /* UUID16 form.  This MACRO only returns whether the UUID_16_t       */
   /* variable is equal to the Supported Unread Alert Category UUID     */
   /* (MACRO returns boolean result) NOT less than/greater than.  The   */
   /* first parameter is the UUID_16_t variable to compare to the ANS   */
   /* Supported Unread Alert Category UUID.                             */
#define ANS_COMPARE_ANS_SUPPORTED_UNREAD_ALERT_CATEGORY_UUID_TO_UUID_16(_x) COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x48)

   /* The following defines the ANS Supported Unread Alert Category     */
   /* Characteristic UUID that is used when building the ANS Service    */
   /* Table.                                                            */
#define ANS_SUPPORTED_UNREAD_ALERT_CATEGORY_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT { 0x48, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the ANS Unread*/
   /* Alert Status Characteristic 16 bit UUID to the specified UUID_16_t*/
   /* variable.  This MACRO accepts one parameter which is the UUID_16_t*/
   /* variable that is to receive the ANS Unread Alert Status UUID      */
   /* Constant value.                                                   */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define ANS_ASSIGN_UNREAD_ALERT_STATUS_UUID_16(_x)                     ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x45)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined ANS Unread Alert Status UUID in UUID16     */
   /* form.  This MACRO only returns whether the UUID_16_t variable is  */
   /* equal to the Unread Alert Status UUID (MACRO returns boolean      */
   /* result) NOT less than/greater than.  The first parameter is the   */
   /* UUID_16_t variable to compare to the ANS Unread Alert Status UUID.*/
#define ANS_COMPARE_ANS_UNREAD_ALERT_STATUS_UUID_TO_UUID_16(_x)        COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x45)

   /* The following defines the ANS Unread Alert Status Characteristic  */
   /* UUID that is used when building the ANS Service Table.            */
#define ANS_UNREAD_ALERT_STATUS_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT { 0x45, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the ANS       */
   /* Control Point Characteristic 16 bit UUID to the specified         */
   /* UUID_16_t variable.  This MACRO accepts one parameter which is the*/
   /* UUID_16_t variable that is to receive the ANS Control Point UUID  */
   /* Constant value.                                                   */
   /* * NOTE * The UUID will be assigned into the UUID_16_t variable in */
   /*          Little-Endian format.                                    */
#define ANS_ASSIGN_CONTROL_POINT_UUID_16(_x)                       ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x44)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined ANS Control Point UUID in UUID16 form.     */
   /* This MACRO only returns whether the UUID_16_t variable is equal to*/
   /* the Control Point UUID (MACRO returns boolean result) NOT less    */
   /* than/greater than.  The first parameter is the UUID_16_t variable */
   /* to compare to the ANS Control Point UUID.                         */
#define ANS_COMPARE_ANS_CONTROL_POINT_UUID_TO_UUID_16(_x)          COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x44)

   /* The following defines the ANS Control Point Characteristic UUID   */
   /* that is used when building the ANS Service Table.                 */
#define ANS_CONTROL_POINT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT   { 0x44, 0x2A }

   /* The following structure defines the structure of a New Alert      */
   /* notification.                                                     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagANS_New_Alert_Notification_t
{
   NonAlignedByte_t CategoryID;
   NonAlignedByte_t NumberOfNewAlerts;
   NonAlignedByte_t TextStringInformation[1];
} __PACKED_STRUCT_END__ ANS_New_Alert_Notification_t;

#define ANS_NEW_ALERT_NOTIFICATION_DATA_SIZE(_x)         (BTPS_STRUCTURE_OFFSET(ANS_New_Alert_Notification_t, TextStringInformation) + ((_x)*BYTE_SIZE))

   /* The following structure defines the structure of a New Alert      */
   /* notification.                                                     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagANS_Unread_Alert_Status_Notification_t
{
   NonAlignedByte_t CategoryID;
   NonAlignedByte_t UnreadCount;
} __PACKED_STRUCT_END__ ANS_Unread_Alert_Status_Notification_t;

#define ANS_UNREAD_ALERT_STATUS_NOTIFICATION_DATA_SIZE   (sizeof(ANS_Unread_Alert_Status_Notification_t))

   /* The following structure defines the structure of a Alert          */
   /* Notification Control Point Command.                               */
typedef __PACKED_STRUCT_BEGIN__ struct _tagANS_Control_Point_Command_Value_t
{
   NonAlignedByte_t CommandID;
   NonAlignedByte_t CategoryID;
} __PACKED_STRUCT_END__ ANS_Control_Point_Command_Value_t;

#define ANS_CONTROL_POINT_COMMAND_VALUE_DATA_SIZE        (sizeof(ANS_Control_Point_Command_Value_t))

   /* The following defines the length of the Supported New Alert       */
   /* Category characteristic value.                                    */
#define ANS_SUPPORTED_NEW_ALERT_CATEGORY_VALUE_LENGTH                (WORD_SIZE)

   /* The following defines the length of the Supported Unread Category */
   /* characteristic value.                                             */
#define ANS_SUPPORTED_UNREAD_ALERT_CATEGORY_VALUE_LENGTH             (WORD_SIZE)

   /* The following defines the valid Alert Category ID Bit masks that  */
   /* may be specified in the first byte of the Supported New Alert     */
   /* Category and Supported Unread Alert Category characteristic       */
   /* values.                                                           */
#define ANS_ALERT_CATEGORY_ID_BIT_MASK_ZERO_SIMPLE_ALERT             0x01
#define ANS_ALERT_CATEGORY_ID_BIT_MASK_ZERO_EMAIL                    0x02
#define ANS_ALERT_CATEGORY_ID_BIT_MASK_ZERO_NEWS                     0x04
#define ANS_ALERT_CATEGORY_ID_BIT_MASK_ZERO_CALL                     0x08
#define ANS_ALERT_CATEGORY_ID_BIT_MASK_ZERO_MISSED_CALL              0x10
#define ANS_ALERT_CATEGORY_ID_BIT_MASK_ZERO_SMS_MMS                  0x20
#define ANS_ALERT_CATEGORY_ID_BIT_MASK_ZERO_VOICE_MAIL               0x40
#define ANS_ALERT_CATEGORY_ID_BIT_MASK_ZERO_SCHEDULE                 0x80

   /* The following defines the valid Alert Category ID Bit masks that  */
   /* may be specified in the second byte of the Supported New Alert    */
   /* Category and Supported Unread Alert Category characteristic       */
   /* values.                                                           */
#define ANS_ALERT_CATEGORY_ID_BIT_MASK_ONE_HIGH_PRIORITIZED_ALERT    0x01
#define ANS_ALERT_CATEGORY_ID_BIT_MASK_ONE_INSTANT_MESSAGE           0x02

   /* The following defines the valid Command IDs of commands that may  */
   /* be written into the Alert Notification Control Point              */
   /* characteristic.                                                   */
#define ANS_COMMAND_ID_ENABLE_NEW_INCOMING_ALERT_NOTIFICATION        0x00
#define ANS_COMMAND_ID_ENABLE_UNREAD_CATEGORY_STATUS_NOTIFICATION    0x01
#define ANS_COMMAND_ID_DISABLE_NEW_INCOMING_ALERT_NOTIFICATION       0x02
#define ANS_COMMAND_ID_DISABLE_UNREAD_CATEGORY_STATUS_NOTIFICATION   0x03
#define ANS_COMMAND_ID_NOTIFY_NEW_INCOMING_ALERT_IMMEDIATELY         0x04
#define ANS_COMMAND_ID_NOTIFY_UNREAD_CATEGORY_STATUS_IMMEDIATELY     0x05

   /* The following defines the valid ANS Alert Category ID values that */
   /* may be received or sent.                                          */
#define ANS_ALERT_CATEGORY_ID_SIMPLE_ALERT                           0x00
#define ANS_ALERT_CATEGORY_ID_EMAIL                                  0x01
#define ANS_ALERT_CATEGORY_ID_NEWS                                   0x02
#define ANS_ALERT_CATEGORY_ID_CALL                                   0x03
#define ANS_ALERT_CATEGORY_ID_MISSED_CALL                            0x04
#define ANS_ALERT_CATEGORY_ID_SMS_MMS                                0x05
#define ANS_ALERT_CATEGORY_ID_VOICE_MAIL                             0x06
#define ANS_ALERT_CATEGORY_ID_SCHEDULE                               0x07
#define ANS_ALERT_CATEGORY_ID_HIGH_PRIORITY_ALERT                    0x08
#define ANS_ALERT_CATEGORY_ID_INSTANT_MESSAGE                        0x09
#define ANS_ALERT_CATEGORY_ID_ALL_CATEGORIES                         0xFF

#endif
