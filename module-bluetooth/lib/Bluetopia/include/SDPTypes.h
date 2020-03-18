/*****< sdptypes.h >***********************************************************/
/*      Copyright 2000 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  SDPTYPES - Bluetooth SDP Type Definitions/Constants.                      */
/*                                                                            */
/*  Author:  Damon Lange                                                      */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   09/11/00  D. Lange       Initial creation.                               */
/******************************************************************************/
#ifndef __SDPTYPESH__
#define __SDPTYPESH__

#include "BTTypes.h"            /* Bluetooth Type Definitions.                */

   /* The following Constants represent the defined Bluetooth SDP       */
   /* Server Version that this SDP Server Implementation supports.      */
   /* These values are used with the                                    */
   /* SDP_ATTRIBUTE_ID_VERSION_NUMBER_LIST Attribute for the SDP Server.*/
#define SDP_SERVER_MAJOR_VERSION                        0x01
#define SDP_SERVER_MINOR_VERSION                        0x00

   /* The following MACRO is a utility MACRO that builds an SDP Server  */
   /* Version Information value based upon a given SDP Server Major and */
   /* SDP Server Minor Version.  The first parameter to this MACRO is   */
   /* the SDP Server Major Version, and the second parameter is the     */
   /* SDP Server Minor Versio.  This function returns a Word_t that is  */
   /* the SDP Version Value that was built from the specified SDP       */
   /* Server Major and Minor Versions.                                  */
#define SDP_MAKE_SERVER_VERSION_VALUE(_x, _y)           ((Word_t)(((((Word_t)(_x)) & 0xFF) << 8) | ((_y) & 0xFF)))

   /* The following MACRO is a utility MACRO that retrieves the SDP     */
   /* Server Major Version Information value based upon a given SDP     */
   /* Server Version Value.  The first parameter to this MACRO is a     */
   /* Word_t which represents the SDP Server Version Value.  This MACRO */
   /* returns a Byte_t which represents the SDP Server Major Version    */
   /* value that was retrieved from the SDP Server Version Information. */
#define SDP_GET_SERVER_MAJOR_VERSION(_x)                ((Byte_t)(((_x) >> 8) & 0xFF))

   /* The following MACRO is a utility MACRO that retrieves the SDP     */
   /* Server Minor Version Information value based upon a given SDP     */
   /* Server Version Value.  The first parameter to this MACRO is a     */
   /* Word_t which represents the SDP Server Version Value.  This MACRO */
   /* returns a Byte_t which represents the SDP Server Minor Version    */
   /* value that was retrieved from the SDP Server Version Information. */
#define SDP_GET_SERVER_MINOR_VERSION(_x)                ((Byte_t)(((_x) & 0xFF)))

   /* The following Constant represents the Service Discovery Protocol  */
   /* Protocol Service Multiplexor (PSM) value when SDP runs over the   */
   /* L2CAP Protocol (which is the only defined mechanism).             */
#define SDP_PROTOCOL_SERVICE_MULTIPLEXOR                                0x01
#define SDP_PSM                                                         0x01

   /* SDP Universally Unique Identifiers (UUID).                        */

   /* SDP BASE_UUID.                                                    */

   /* The following MACRO is a utility MACRO that assigns the SDP       */
   /* Bluetooth Base Universally Unique Identifier (BASE_UUID) to the   */
   /* specified UUID_128_t variable.  This MACRO accepts one parameter  */
   /* which is the UUID_128_t variable that is receive the BASE_UUID    */
   /* Constant value.                                                   */
#define SDP_ASSIGN_BASE_UUID(_x)                ASSIGN_SDP_UUID_128((_x), 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /* SDP Protocol UUID's.                                              */

   /* The following MACRO is a utility MACRO that assigns the SDP       */
   /* Bluetooth Universally Unique Identifier (SDP_UUID_16) to the      */
   /* specified UUID_16_t variable.  This MACRO accepts one parameter   */
   /* which is the UUID_16_t variable that is receive the SDP_UUID_16   */
   /* Constant value.                                                   */
#define SDP_ASSIGN_SDP_UUID_16(_x)              ASSIGN_SDP_UUID_16((_x), 0x00, 0x01)

   /* The following MACRO is a utility MACRO that assigns the SDP       */
   /* Bluetooth Universally Unique Identifier (SDP_UUID_32) to the      */
   /* specified UUID_32_t variable.  This MACRO accepts one parameter   */
   /* which is the UUID_32_t variable that is receive the SDP_UUID_32   */
   /* Constant value.                                                   */
#define SDP_ASSIGN_SDP_UUID_32(_x)              ASSIGN_SDP_UUID_32((_x), 0x00, 0x00, 0x00, 0x01)

   /* The following MACRO is a utility MACRO that assigns the SDP       */
   /* Bluetooth Universally Unique Identifier (SDP_UUID_128) to the     */
   /* specified UUID_128_t variable.  This MACRO accepts one parameter  */
   /* which is the UUID_128_t variable that is receive the SDP_UUID_128 */
   /* Constant value.                                                   */
#define SDP_ASSIGN_SDP_UUID_128(_x)             ASSIGN_SDP_UUID_128((_x), 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /* SDP Service Class UUID's.                                         */

   /* The following MACRO is a utility MACRO that assigns the SDP Server*/
   /* Bluetooth Universally Unique Identifier (SDP_SERVER_UUID_16) to   */
   /* the specified UUID_16_t variable.  This MACRO accepts one         */
   /* parameter which is the UUID_16_t variable that is to receive the  */
   /* SDP_SERVER_UUID_16 Constant value.                                */
#define SDP_ASSIGN_SDP_SERVER_UUID_16(_x)       ASSIGN_SDP_UUID_16((_x), 0x10, 0x00)

   /* The following MACRO is a utility MACRO that assigns the SDP Server*/
   /* Bluetooth Universally Unique Identifier (SDP_SERVER_UUID_32) to   */
   /* the specified UUID_32_t variable.  This MACRO accepts one         */
   /* parameter which is the UUID_32_t variable that is to receive the  */
   /* SDP_SERVER_UUID_32 Constant value.                                */
#define SDP_ASSIGN_SDP_SERVER_UUID_32(_x)       ASSIGN_SDP_UUID_32((_x), 0x00, 0x00, 0x10, 0x00)

   /* The following MACRO is a utility MACRO that assigns the SDP Server*/
   /* Bluetooth Universally Unique Identifier (SDP_SERVER_UUID_128) to  */
   /* the specified UUID_128_t variable.  This MACRO accepts one        */
   /* parameter which is the UUID_128_t variable that is receive the    */
   /* SDP_SERVER_UUID_128 Constant value.                               */
#define SDP_ASSIGN_SDP_SERVER_UUID_128(_x)      ASSIGN_SDP_UUID_128((_x), 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /* The following MACRO is a utility MACRO that assigns the Browse    */
   /* Group Bluetooth Universally Unique Identifier                     */
   /* (BROWSE_GROUP_UUID_16) to the specified UUID_16_t variable.  This */
   /* MACRO accepts one parameter which is the UUID_16_t variable that  */
   /* is to receive the BROWSE_GROUP_UUID_16 Constant value.            */
#define SDP_ASSIGN_BROWSE_GROUP_UUID_16(_x)     ASSIGN_SDP_UUID_16((_x), 0x10, 0x01)

   /* The following MACRO is a utility MACRO that assigns the Browse    */
   /* Group Bluetooth Universally Unique Identifier                     */
   /* (BROWSE_GROUP_UUID_32) to the specified UUID_32_t variable.  This */
   /* MACRO accepts one parameter which is the UUID_32_t variable that  */
   /* is to receive the BROWSE_GROUP_UUID_32 Constant value.            */
#define SDP_ASSIGN_BROWSE_GROUP_UUID_32(_x)     ASSIGN_SDP_UUID_32((_x), 0x00, 0x00, 0x10, 0x01)

   /* The following MACRO is a utility MACRO that assigns the Browse    */
   /* Group Bluetooth Universally Unique Identifier                     */
   /* (BROWSE_GROUP_UUID_128) to the specified UUID_128_t variable.     */
   /* This MACRO accepts one parameter which is the UUID_128_t          */
   /* variable that is to receive the BROWSE_GROUP_UUID_128 Constant    */
   /* value.                                                            */
#define SDP_ASSIGN_BROWSE_GROUP_UUID_128(_x)    ASSIGN_SDP_UUID_128((_x), 0x00, 0x00, 0x10, 0x01, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /* The following MACRO is a utility MACRO that assigns the Public    */
   /* Browse Group Bluetooth Universally Unique Identifier              */
   /* (PUBLIC_BROWSE_GROUP_UUID_16) to the specified UUID_16_t variable.*/
   /* This MACRO accepts one parameter which is the UUID_16_t variable  */
   /* that is to receive the PUBLIC_BROWSE_GROUP_UUID_16 Constant value.*/
#define SDP_ASSIGN_PUBLIC_BROWSE_GROUP_UUID_16(_x)  ASSIGN_SDP_UUID_16((_x), 0x10, 0x02)

   /* The following MACRO is a utility MACRO that assigns the Public    */
   /* Browse Group Bluetooth Universally Unique Identifier              */
   /* (PUBLIC_BROWSE_GROUP_UUID_32) to the specified UUID_32_t variable.*/
   /* This MACRO accepts one parameter which is the UUID_32_t variable  */
   /* that is to receive the PUBLIC_BROWSE_GROUP_UUID_32 Constant value.*/
#define SDP_ASSIGN_PUBLIC_BROWSE_GROUP_UUID_32(_x)  ASSIGN_SDP_UUID_32((_x), 0x00, 0x00, 0x10, 0x02)

   /* The following MACRO is a utility MACRO that assigns the Public    */
   /* Browse Group Bluetooth Universally Unique Identifier              */
   /* (PUBLIC_BROWSE_GROUP_UUID_128) to the specified UUID_128_t        */
   /* variable.  This MACRO accepts one parameter which is the          */
   /* UUID_128_t variable that is to receive the                        */
   /* PUBLIC_BROWSE_GROUP_UUID_128 Constant value.                      */
#define SDP_ASSIGN_PUBLIC_BROWSE_GROUP_UUID_128(_x) ASSIGN_SDP_UUID_128((_x), 0x00, 0x00, 0x10, 0x02, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /* The following definition is the assigned Bluetooth SDP Server     */
   /* Protocol Name.                                                    */
#define SDP_SERVER_PROTOCOL_NAME                        "sdp.bt"

   /* The following Constant represents the defined Bluetooth Service   */
   /* Discovery Protocol SDP Server Handle (Reserved).                  */
#define SDP_SERVICE_RECORD_HANDLE_SERVER                                0x00000000

   /* The following Constants represent the defined Bluetooth Service   */
   /* Discovery Protocol Record Handle limits.                          */
#define SDP_SERVICE_RECORD_HANDLE_MINIMUM                               0x00010000
#define SDP_SERVICE_RECORD_HANDLE_MAXIMUM                               0xFFFFFFFF

   /* The following Service Class UUID's represent the defined Generic  */
   /* Bluetooth Service Class ID's.                                     */

   /* The following MACRO is a utility MACRO that assigns the Plug and  */
   /* Play Information Service Class Bluetooth Universally Unique       */
   /* Identifier (PNP_INFORMATION_UUID_16) to the specified UUID_16_t   */
   /* variable.  This MACRO accepts one parameter which is the UUID_16_t*/
   /* variable that is to receive the PNP_INFORMATION_UUID_16 Constant  */
   /* value.                                                            */
#define SDP_ASSIGN_PNP_INFORMATION_UUID_16(_x)             ASSIGN_SDP_UUID_16((_x), 0x12, 0x00)

   /* The following MACRO is a utility MACRO that assigns the Plug and  */
   /* Play Information Service Class Bluetooth Universally Unique       */
   /* Identifier (PNP_INFORMATION_UUID_32) to the specified UUID_32_t   */
   /* variable.  This MACRO accepts one parameter which is the UUID_32_t*/
   /* variable that is to receive the PNP_INFORMATION_UUID_32 Constant  */
   /* value.                                                            */
#define SDP_ASSIGN_PNP_INFORMATION_UUID_32(_x)             ASSIGN_SDP_UUID_32((_x), 0x00, 0x00, 0x12, 0x00)

   /* The following MACRO is a utility MACRO that assigns the Plug and  */
   /* Play Information Service Class Bluetooth Universally Unique       */
   /* Identifier (PNP_INFORMATION_UUID_128) to the specified UUID_128_t */
   /* variable.  This MACRO accepts one parameter which is the          */
   /* UUID_128_t variable that is to receive the                        */
   /* PNP_INFORMATION_UUID_128 Constant value.                          */
#define SDP_ASSIGN_PNP_INFORMATION_UUID_128(_x)            ASSIGN_SDP_UUID_128((_x), 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /* The following MACRO is a utility MACRO that assigns the Generic   */
   /* Networking Service Class Bluetooth Universally Unique Identifier  */
   /* (GENERIC_NETWORKING_UUID_16) to the specified UUID_16_t variable. */
   /* This MACRO accepts one parameter which is the UUID_16_t variable  */
   /* that is to receive the GENERIC_NETWORKING_UUID_16 Constant value. */
#define SDP_ASSIGN_GENERIC_NETWORKING_UUID_16(_x)          ASSIGN_SDP_UUID_16((_x), 0x12, 0x01)

   /* The following MACRO is a utility MACRO that assigns the Generic   */
   /* Networking Service Class Bluetooth Universally Unique Identifier  */
   /* (GENERIC_NETWORKING_UUID_32) to the specified UUID_32_t variable. */
   /* This MACRO accepts one parameter which is the UUID_32_t variable  */
   /* that is to receive the GENERIC_NETWORKING_UUID_32 Constant value. */
#define SDP_ASSIGN_GENERIC_NETWORKING_UUID_32(_x)          ASSIGN_SDP_UUID_32((_x), 0x00, 0x00, 0x12, 0x01)

   /* The following MACRO is a utility MACRO that assigns the Generic   */
   /* Networking Service Class Bluetooth Universally Unique Identifier  */
   /* (GENERIC_NETWORKING_UUID_128) to the specified UUID_128_t         */
   /* variable.  This MACRO accepts one parameter which is the          */
   /* UUID_128_t variable that is to receive the                        */
   /* GENERIC_NETWORKING_UUID_128 Constant value.                       */
#define SDP_ASSIGN_GENERIC_NETWORKING_UUID_128(_x)         ASSIGN_SDP_UUID_128((_x), 0x00, 0x00, 0x12, 0x01, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /* The following MACRO is a utility MACRO that assigns the Generic   */
   /* File Transfer Service Class Bluetooth Universally Unique          */
   /* Identifier (GENERIC_FILE_TRANSFER_UUID_16) to the specified       */
   /* UUID_16_t variable.  This MACRO accepts one parameter which is the*/
   /* UUID_16_t variable that is to receive the                         */
   /* GENERIC_FILE_TRANSFER_UUID_16 Constant value.                     */
#define SDP_ASSIGN_GENERIC_FILE_TRANSFER_UUID_16(_x)       ASSIGN_SDP_UUID_16((_x), 0x12, 0x02)

   /* The following MACRO is a utility MACRO that assigns the Generic   */
   /* File Transfer Service Class Bluetooth Universally Unique          */
   /* Identifier (GENERIC_FILE_TRANSFER_UUID_32) to the specified       */
   /* UUID_32_t variable.  This MACRO accepts one parameter which is the*/
   /* UUID_32_t variable that is to receive the                         */
   /* GENERIC_FILE_TRANSFER_UUID_32 Constant value.                     */
#define SDP_ASSIGN_GENERIC_FILE_TRANSFER_UUID_32(_x)       ASSIGN_SDP_UUID_32((_x), 0x00, 0x00, 0x12, 0x02)

   /* The following MACRO is a utility MACRO that assigns the Generic   */
   /* File Transfer Service Class Bluetooth Universally Unique          */
   /* Identifier (GENERIC_FILE_TRANSFER_UUID_128) to the specified      */
   /* UUID_128_t variable.  This MACRO accepts one parameter which is   */
   /* the UUID_128_t variable that is to receive the                    */
   /* GENERIC_FILE_TRANSFER_UUID_128 Constant value.                    */
#define SDP_ASSIGN_GENERIC_FILE_TRANSFER_UUID_128(_x)      ASSIGN_SDP_UUID_128((_x), 0x00, 0x00, 0x12, 0x02, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /* The following MACRO is a utility MACRO that assigns the Generic   */
   /* Audio Service Class Bluetooth Universally Unique Identifier       */
   /* (GENERIC_AUDIO_UUID_16) to the specified UUID_16_t variable.  This*/
   /* MACRO accepts one parameter which is the UUID_16_t variable that  */
   /* is to receive the GENERIC_AUDIO_UUID_16 Constant value.           */
#define SDP_ASSIGN_GENERIC_AUDIO_UUID_16(_x)               ASSIGN_SDP_UUID_16((_x), 0x12, 0x03)

   /* The following MACRO is a utility MACRO that assigns the Generic   */
   /* Audio Service Class Bluetooth Universally Unique Identifier       */
   /* (GENERIC_AUDIO_UUID_32) to the specified UUID_32_t variable.  This*/
   /* MACRO accepts one parameter which is the UUID_32_t variable that  */
   /* is to receive the GENERIC_AUDIO_UUID_32 Constant value.           */
#define SDP_ASSIGN_GENERIC_AUDIO_UUID_32(_x)               ASSIGN_SDP_UUID_32((_x), 0x00, 0x00, 0x12, 0x03)

   /* The following MACRO is a utility MACRO that assigns the Generic   */
   /* Audio Service Class Bluetooth Universally Unique Identifier       */
   /* (GENERIC_AUDIO_UUID_128) to the specified UUID_128_t variable.    */
   /* This MACRO accepts one parameter which is the UUID_128_t variable */
   /* that is to receive the GENERIC_AUDIO_UUID_128 Constant value.     */
#define SDP_ASSIGN_GENERIC_AUDIO_UUID_128(_x)              ASSIGN_SDP_UUID_128((_x), 0x00, 0x00, 0x12, 0x03, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /* The following MACRO is a utility MACRO that assigns the Generic   */
   /* Telephony Service Class Bluetooth Universally Unique Identifier   */
   /* (GENERIC_TELEPHONY_UUID_16) to the specified UUID_16_t variable.  */
   /* This MACRO accepts one parameter which is the UUID_16_t variable  */
   /* that is to receive the GENERIC_TELEPHONY_UUID_16 Constant value.  */
#define SDP_ASSIGN_GENERIC_TELEPHONY_UUID_16(_x)           ASSIGN_SDP_UUID_16((_x), 0x12, 0x04)

   /* The following MACRO is a utility MACRO that assigns the Generic   */
   /* Telephony Service Class Bluetooth Universally Unique Identifier   */
   /* (GENERIC_TELEPHONY_UUID_32) to the specified UUID_32_t variable.  */
   /* This MACRO accepts one parameter which is the UUID_32_t variable  */
   /* that is to receive the GENERIC_TELEPHONY_UUID_32 Constant value.  */
#define SDP_ASSIGN_GENERIC_TELEPHONY_UUID_32(_x)           ASSIGN_SDP_UUID_32((_x), 0x00, 0x00, 0x12, 0x04)

   /* The following MACRO is a utility MACRO that assigns the Generic   */
   /* Telephony Service Class Bluetooth Universally Unique Identifier   */
   /* (GENERIC_TELEPHONY_UUID_128) to the specified UUID_128_t variable.*/
   /* This MACRO accepts one parameter which is the UUID_128_t variable */
   /* that is to receive the GENERIC_TELEPHONY_UUID_128 Constant value. */
#define SDP_ASSIGN_GENERIC_TELEPHONY_UUID_128(_x)          ASSIGN_SDP_UUID_128((_x), 0x00, 0x00, 0x12, 0x04, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /* The following Constants represent the defined Bluetooth Service   */
   /* Discovery Protocol Universal Attribute Identifiers.               */
#define SDP_ATTRIBUTE_ID_SERVICE_RECORD_HANDLE                          0x0000
#define SDP_ATTRIBUTE_ID_SERVICE_CLASS_ID_LIST                          0x0001
#define SDP_ATTRIBUTE_ID_SERVICE_RECORD_STATE                           0x0002
#define SDP_ATTRIBUTE_ID_SERVICE_ID                                     0x0003
#define SDP_ATTRIBUTE_ID_PROTOCOL_DESCRIPTOR_LIST                       0x0004
#define SDP_ATTRIBUTE_ID_BROWSE_GROUP_LIST                              0x0005
#define SDP_ATTRIBUTE_ID_LANGUAGE_BASE_ATTRIBUTE_ID_LIST                0x0006
#define SDP_ATTRIBUTE_ID_SERVICE_INFO_TIME_TO_LIVE                      0x0007
#define SDP_ATTRIBUTE_ID_SERVICE_AVAILABILITY                           0x0008
#define SDP_ATTRIBUTE_ID_BLUETOOTH_PROFILE_DESCRIPTOR_LIST              0x0009
#define SDP_ATTRIBUTE_ID_DOCUMENTATION_URL                              0x000A
#define SDP_ATTRIBUTE_ID_CLIENT_EXECUTABLE_URL                          0x000B
#define SDP_ATTRIBUTE_ID_ICON_10                                        0x000C
#define SDP_ATTRIBUTE_ID_ICON_URL                                       0x000C
#define SDP_ATTRIBUTE_ID_ADDITIONAL_PROTOCOL_DESCRIPTOR_LISTS           0x000D
#define SDP_ATTRIBUTE_ID_VERSION_NUMBER_LIST                            0x0200
#define SDP_ATTRIBUTE_ID_GROUP_ID                                       0x0200
#define SDP_ATTRIBUTE_ID_IP_SUBNET                                      0x0200
#define SDP_ATTRIBUTE_ID_HID_DEVICE_RELEASE_NUMBER                      0x0200
#define SDP_ATTRIBUTE_ID_DID_SPECIFICATION_ID                           0x0200
#define SDP_ATTRIBUTE_ID_HDP_SUPPORTED_FEATURES                         0x0200
#define SDP_ATTRIBUTE_ID_HDP_SUPPORTED_FEATURES_LIST                    0x0200
#define SDP_ATTRIBUTE_ID_VERSION_NUMBER_LIST                            0x0200
#define SDP_ATTRIBUTE_ID_GOEP_L2CAP_PSM                                 0x0200
#define SDP_ATTRIBUTE_ID_SERVICE_DATABASE_STATE                         0x0201
#define SDP_ATTRIBUTE_ID_HID_PARSER_VERSION                             0x0201
#define SDP_ATTRIBUTE_ID_DID_VENDOR_ID                                  0x0201
#define SDP_ATTRIBUTE_ID_HID_DEVICE_SUBCLASS                            0x0202
#define SDP_ATTRIBUTE_ID_DID_PRODUCT_ID                                 0x0202
#define SDP_ATTRIBUTE_ID_HID_COUNTRY_CODE                               0x0203
#define SDP_ATTRIBUTE_ID_DID_VERSION                                    0x0203
#define SDP_ATTRIBUTE_ID_HID_VIRTUAL_CABLE                              0x0204
#define SDP_ATTRIBUTE_ID_DID_PRIMARY_RECORD                             0x0204
#define SDP_ATTRIBUTE_ID_HID_RECONNECT_INITIATE                         0x0205
#define SDP_ATTRIBUTE_ID_DID_VENDOR_ID_SOURCE                           0x0205
#define SDP_ATTRIBUTE_ID_HID_DESCRIPTOR_LIST                            0x0206
#define SDP_ATTRIBUTE_ID_HID_LANGUAGE_ID_BASE_LIST                      0x0207
#define SDP_ATTRIBUTE_ID_HID_SDP_DISABLE                                0x0208
#define SDP_ATTRIBUTE_ID_HID_BATTERY_POWER                              0x0209
#define SDP_ATTRIBUTE_ID_HID_REMOTE_WAKE                                0x020A
#define SDP_ATTRIBUTE_ID_HID_PROFILE_VERSION                            0x020B
#define SDP_ATTRIBUTE_ID_HID_SUPERVISION_TIMEOUT                        0x020C
#define SDP_ATTRIBUTE_ID_HID_NORMALLY_CONNECTABLE                       0x020D
#define SDP_ATTRIBUTE_ID_HID_BOOT_DEVICE                                0x020E
#define SDP_ATTRIBUTE_ID_HID_SNIFF_SUBRATING_HOST_MAXIMUM_LATENCY       0x020F
#define SDP_ATTRIBUTE_ID_HID_SNIFF_SUBRATING_HOST_MINIMUM_TIMEOUT       0x0210
#define SDP_ATTRIBUTE_ID_SERVICE_VERSION                                0x0300
#define SDP_ATTRIBUTE_ID_IEEE_1284_ID                                   0x0300
#define SDP_ATTRIBUTE_ID_EXTERNAL_NETWORK                               0x0301
#define SDP_ATTRIBUTE_ID_SUPPORTED_DATA_STORES_LIST                     0x0301
#define SDP_ATTRIBUTE_ID_NETWORK                                        0x0301
#define SDP_ATTRIBUTE_ID_HDP_DATA_EXCHANGE_SPECIFICATION                0x0301
#define SDP_ATTRIBUTE_ID_REMOTE_AUDIO_VOLUME_CONTROL                    0x0302
#define SDP_ATTRIBUTE_ID_FAX_CLASS_1_SUPPORT                            0x0302
#define SDP_ATTRIBUTE_ID_DEVICE_NAME                                    0x0302
#define SDP_ATTRIBUTE_ID_HDP_MCAP_SUPPORTED_PROCEDURES                  0x0302
#define SDP_ATTRIBUTE_ID_SUPPORTED_FORMATS_LIST                         0x0303
#define SDP_ATTRIBUTE_ID_FAX_CLASS_2_0_SUPPORT                          0x0303
#define SDP_ATTRIBUTE_ID_FAX_CLASS_2_SUPPORT                            0x0304
#define SDP_ATTRIBUTE_ID_FRIENDLY_NAME                                  0x0304
#define SDP_ATTRIBUTE_ID_AUDIO_FEEDBACK_SUPPORT                         0x0305
#define SDP_ATTRIBUTE_ID_NETWORK_ADDRESS                                0x0306
#define SDP_ATTRIBUTE_ID_DEVICE_LOCATION                                0x0306
#define SDP_ATTRIBUTE_ID_WAP_GATEWAY                                    0x0307
#define SDP_ATTRIBUTE_ID_HOME_PAGE_URL                                  0x0308
#define SDP_ATTRIBUTE_ID_WAP_STACK_TYPE                                 0x0309
#define SDP_ATTRIBUTE_ID_SECURITY_DESCRIPTION                           0x030A
#define SDP_ATTRIBUTE_ID_NET_ACCESS_TYPE                                0x030B
#define SDP_ATTRIBUTE_ID_MAX_NET_ACCESS_RATE                            0x030C
#define SDP_ATTRIBUTE_ID_IP_V4_SUBNET                                   0x030D
#define SDP_ATTRIBUTE_ID_IP_V6_SUBNET                                   0x030E
#define SDP_ATTRIBUTE_ID_SUPPORTED_CAPABILITIES                         0x0310
#define SDP_ATTRIBUTE_ID_SUPPORTED_FEATURES                             0x0311
#define SDP_ATTRIBUTE_ID_SUPPORTED_FUNCTIONS                            0x0312
#define SDP_ATTRIBUTE_ID_TOTAL_IMAGING_DATA_CAPACITY                    0x0313
#define SDP_ATTRIBUTE_ID_SUPPORTED_REPOSITORIES                         0x0314
#define SDP_ATTRIBUTE_ID_MAS_INSTANCE_ID                                0x0315
#define SDP_ATTRIBUTE_ID_SUPPORTED_MESSAGE_TYPES                        0x0316
#define SDP_ATTRIBUTE_ID_DOCUMENT_FORMATS_SUPPORTED                     0x0350
#define SDP_ATTRIBUTE_ID_CHARACTER_REPERTOIRES_SUPPORTED                0x0352
#define SDP_ATTRIBUTE_ID_XHTML_PRINT_IMAGE_FORMATS_SUPPORTED            0x0354
#define SDP_ATTRIBUTE_ID_COLOR_SUPPORTED                                0x0356
#define SDP_ATTRIBUTE_ID_1284_ID                                        0x0358
#define SDP_ATTRIBUTE_ID_PRINTER_NAME                                   0x035A
#define SDP_ATTRIBUTE_ID_PRINTER_LOCATION                               0x035C
#define SDP_ATTRIBUTE_ID_DUPLEX_SUPPORTED                               0x035E
#define SDP_ATTRIBUTE_ID_MEDIA_TYPES_SUPPORTED                          0x0360
#define SDP_ATTRIBUTE_ID_MAX_MEDIA_WIDTH                                0x0362
#define SDP_ATTRIBUTE_ID_MAX_MEDIA_LENGTH                               0x0364
#define SDP_ATTRIBUTE_ID_ENHANCED_LAYOUT_SUPPORTED                      0x0366
#define SDP_ATTRIBUTE_ID_RUI_FORMATS_SUPPORTED                          0x0368
#define SDP_ATTRIBUTE_ID_REFERENCED_PRINTING_RUI_SUPPORTED              0x0370
#define SDP_ATTRIBUTE_ID_DIRECT_PRINTING_RUI_SUPPORTED                  0x0372
#define SDP_ATTRIBUTE_ID_REFERENCED_PRINTING_TOP_URL                    0x0374
#define SDP_ATTRIBUTE_ID_DIRECT_PRINTING_TOP_URL                        0x0376
#define SDP_ATTRIBUTE_ID_PRINTER_ADMIN_RUI_TOP_URL                      0x0378
#define SDP_ATTRIBUTE_ID_BPP_DEVICE_NAME                                0x037A

   /* The following Constants represent the defined Bluetooth Service   */
   /* Discovery Protocol Universal Attribute Offset ID Values.  These   */
   /* values *MUST* be added to the Language Base Attribute ID List     */
   /* Attribute to compute the correct Attribute ID for the specified   */
   /* Device.                                                           */
#define SDP_ATTRIBUTE_OFFSET_ID_SERVICE_NAME                            0x0000
#define SDP_ATTRIBUTE_OFFSET_ID_SERVICE_DESCRIPTION                     0x0001
#define SDP_ATTRIBUTE_OFFSET_ID_PROVIDER_NAME                           0x0002

   /* The following Constant represents the Maximum Defined String      */
   /* Length of an SDP Service Description String which is used with    */
   /* the SDP_ATTRIBUTE_OFFSET_ID_SERVICE_DESCRIPTION Attribute.        */
#define SDP_MAX_SERVICE_DESCRIPTION_ATTRIBUTE_STRING_LENGTH             200

   /* The following Constants represent the defined Bluetooth SDP CPU   */
   /* Type ID Strings used with the SDP Server.                         */
#define SDP_CPU_TYPE_ID_STRING_ALPHA                    "alpha"
#define SDP_CPU_TYPE_ID_STRING_ARM                      "arm"
#define SDP_CPU_TYPE_ID_STRING_INTEL_80X86              "i86"
#define SDP_CPU_TYPE_ID_STRING_INTEL_I960               "i960"
#define SDP_CPU_TYPE_ID_STRING_JAVA_VIRTUAL_MACHINE     "jvm"
#define SDP_CPU_TYPE_ID_STRING_MIPS                     "mips"
#define SDP_CPU_TYPE_ID_STRING_POWER_PC                 "ppc"
#define SDP_CPU_TYPE_ID_STRING_HITACHI_SH3              "sh3"
#define SDP_CPU_TYPE_ID_STRING_HITACHI_SH4              "sh4"
#define SDP_CPU_TYPE_ID_STRING_SUN_SPARC                "sparc"

   /* The following Constants represent the defined Bluetooth SDP       */
   /* Kernel Type ID Strings used with the SDP Server.                  */
#define SDP_KERNEL_ID_STRING_CHORUS                     "chorus"
#define SDP_KERNEL_ID_STRING_LINUX                      "linux"
#define SDP_KERNEL_ID_STRING_JAVAOS                     "javaos"
#define SDP_KERNEL_ID_STRING_OS9                        "os9"
#define SDP_KERNEL_ID_STRING_QNX                        "qnx"
#define SDP_KERNEL_ID_STRING_VXWORKS                    "vxworks"

   /* The following Constants represent the defined Bluetooth SDP OS    */
   /* and OS-Version ID Strings used with the SDP Server.               */
#define SDP_OS_ID_STRING_AMIGAOS                        "amigaos"
#define SDP_OS_ID_STRING_BEOS_4_5                       "beos4.5"
#define SDP_OS_ID_STRING_BEOS                           "beos"
#define SDP_OS_ID_STRING_EJAVA                          "ejava"
#define SDP_OS_ID_STRING_EPOCC                          "epocc"
#define SDP_OS_ID_STRING_EPOCE                          "epoce"
#define SDP_OS_ID_STRING_EPOCQ                          "epocq"
#define SDP_OS_ID_STRING_EPOCS                          "epocs"
#define SDP_OS_ID_STRING_GNU                            "gnu"
#define SDP_OS_ID_STRING_JRE_1_1                        "jre1.1"
#define SDP_OS_ID_STRING_JRE_1_2                        "jre1.2"
#define SDP_OS_ID_STRING_MACOS                          "macos"
#define SDP_OS_ID_STRING_MACOS_X                        "macosx"
#define SDP_OS_ID_STRING_OS2                            "os2"
#define SDP_OS_ID_STRING_OS9                            "os9"
#define SDP_OS_ID_STRING_PALMOS                         "palmos"
#define SDP_OS_ID_STRING_PJAVA                          "pjava"
#define SDP_OS_ID_STRING_PJAVA_1_1                      "pjava1.1"
#define SDP_OS_ID_STRING_PHOTON                         "photon"
#define SDP_OS_ID_STRING_PLAN9                          "plan9"
#define SDP_OS_ID_STRING_QNX                            "qnx"
#define SDP_OS_ID_STRING_RTJAVA                         "rtjava"
#define SDP_OS_ID_STRING_WIN_95                         "win95"
#define SDP_OS_ID_STRING_WIN_98                         "win98"
#define SDP_OS_ID_STRING_WIN_2000                       "win2000"
#define SDP_OS_ID_STRING_WIN_CE                         "wince"
#define SDP_OS_ID_STRING_WIN_NT_4                       "winnt4"
#define SDP_OS_ID_STRING_WIN_32                         "win32"
#define SDP_OS_ID_STRING_WIN_64                         "win64"

   /* The following Constants represent the defined Bluetooth SDP       */
   /* Object Format ID Strings used with the SDP Server.                */
#define SDP_OBJECT_FORMAT_ID_STRING_AOUT                "aout"
#define SDP_OBJECT_FORMAT_ID_STRING_BOUT                "bout"
#define SDP_OBJECT_FORMAT_ID_STRING_COFF                "coff"
#define SDP_OBJECT_FORMAT_ID_STRING_ELF                 "elf"
#define SDP_OBJECT_FORMAT_ID_STRING_JAR                 "jar"

   /* The following Constants represent the defined Bluetooth SDP       */
   /* Manufacturer ID Strings used with the SDP Server.                 */
#define SDP_MANUFACTURER_ID_STRING_AMIGA                "amiga"
#define SDP_MANUFACTURER_ID_STRING_APPLE                "apple"
#define SDP_MANUFACTURER_ID_STRING_BE                   "be"
#define SDP_MANUFACTURER_ID_STRING_ERICSSON             "ericsson"
#define SDP_MANUFACTURER_ID_STRING_IBM                  "ibm"
#define SDP_MANUFACTURER_ID_STRING_INTEL                "intel"
#define SDP_MANUFACTURER_ID_STRING_LUCENT               "lucent"
#define SDP_MANUFACTURER_ID_STRING_MICROSOFT            "microsoft"
#define SDP_MANUFACTURER_ID_STRING_MICROWARE            "microware"
#define SDP_MANUFACTURER_ID_STRING_MOTOROLA             "motorola"
#define SDP_MANUFACTURER_ID_STRING_NOKIA                "nokia"
#define SDP_MANUFACTURER_ID_STRING_PALM                 "palm"
#define SDP_MANUFACTURER_ID_STRING_PSION                "psion"
#define SDP_MANUFACTURER_ID_STRING_QNX                  "qnx"
#define SDP_MANUFACTURER_ID_STRING_SUN                  "sun"
#define SDP_MANUFACTURER_ID_STRING_SYMBIAN              "symbian"
#define SDP_MANUFACTURER_ID_STRING_TOSHIBA              "toshiba"
#define SDP_MANUFACTURER_ID_STRING_UNKNOWN              "unknown"

   /* The following Constants represent the defined Bluetooth SDP       */
   /* Linux Distribution ID Strings used with the SDP Server.           */
#define SDP_LINUX_DISTRIBUTION_ID_STRING_CALDERA        "caldera"
#define SDP_LINUX_DISTRIBUTION_ID_STRING_DEBIAN         "debian"
#define SDP_LINUX_DISTRIBUTION_ID_STRING_DLX            "dlx"
#define SDP_LINUX_DISTRIBUTION_ID_STRING_DOSLINUX       "doslinux"
#define SDP_LINUX_DISTRIBUTION_ID_STRING_LINUXPRO       "linuxpro"
#define SDP_LINUX_DISTRIBUTION_ID_STRING_LINUXWARE      "linuxware"
#define SDP_LINUX_DISTRIBUTION_ID_STRING_MANDRAKE       "mandrake"
#define SDP_LINUX_DISTRIBUTION_ID_STRING_MKLINUX        "mklinux"
#define SDP_LINUX_DISTRIBUTION_ID_STRING_REDHAT         "redhat"
#define SDP_LINUX_DISTRIBUTION_ID_STRING_SLACKWARE      "slackware"
#define SDP_LINUX_DISTRIBUTION_ID_STRING_STAMPEDE       "stampede"
#define SDP_LINUX_DISTRIBUTION_ID_STRING_SUSE           "suse"
#define SDP_LINUX_DISTRIBUTION_ID_STRING_TURBOLINUX     "turbolinux"
#define SDP_LINUX_DISTRIBUTION_ID_STRING_YGGDRASIL      "yggdrasil"

   /* The following Constants represent the defined Bluetooth SDP       */
   /* Icon File Format Tag Strings used with the SDP Server.            */
#define SDP_ICON_FILE_FORMAT_TAG_PNG                    "png"
#define SDP_ICON_FILE_FORMAT_TAG_GIF                    "gif"
#define SDP_ICON_FILE_FORMAT_TAG_BMP                    "bmp"

   /* The following Constant represents the defined Bluetooth SDP       */
   /* Icon File Monochrome Tag String used with the SDP Server.         */
#define SDP_ICON_FILE_MONOCHROME_TAG                    "m"

   /* The following Constant represents the defined Bluetooth SDP       */
   /* Default Attribute ID Base Offset.                                 */
#define SDP_DEFAULT_LANGUAGE_BASE_ATTRIBUTE_ID                          0x0100

   /* The following are SDP Character Encoding Constants that are based */
   /* upon the IANA Database.  This Database can be found at:           */
   /* http://www.isi.edu/in-notes/iana/assignments/character-sets       */
   /* SDP Recommends using the UTF-8 Character Encoding Scheme which is */
   /* defined to be MIBE-num 108 (0x006A).                              */
#define SDP_UTF_8_CHARACTER_ENCODING                                    0x006A

   /* The following are SDP Natural Language Constants that are based   */
   /* upon the ISO 639: 1988 (E/F) Language Codes.  These codes are     */
   /* encoded using the Character Encoding.                             */
#define SDP_NATURAL_LANGUAGE_ENGLISH_UTF_8                              0x656E

   /* The following Constants represent the Bit Mask's to apply to the  */
   /* Data Element Size Descriptor to determine the correct Data Type   */
   /* and/or Data Size.                                                 */
#define SDP_TYPE_DESCRIPTOR_MASK                                        0xF8
#define SDP_SIZE_INDEX_MASK                                             0x07

   /* The following Constants represent the Bit Shift values to apply   */
   /* to the Data Element Size Descriptor to determine the correct Data */
   /* Type and/or Data Size.                                            */
#define SDP_TYPE_DESCRIPTOR_SHIFT_VALUE                                 0x03
#define SDP_SIZE_INDEX_SHIFT_VALUE                                      0x00

   /* The following Constants represent the actual Bluetooth Defined    */
   /* SDP Data Element Type Definitions (NOT their physical value when  */
   /* placed in the Data Element Type Descriptor.                       */
#define SDP_DATA_TYPE_NIL_VALUE                                         0x00
#define SDP_DATA_TYPE_NULL_VALUE                                        0x00
#define SDP_DATA_TYPE_UNSIGNED_INTEGER_VALUE                            0x01
#define SDP_DATA_TYPE_SIGNED_2_COMPLEMENT_INTEGER_VALUE                 0x02
#define SDP_DATA_TYPE_UUID_VALUE                                        0x03
#define SDP_DATA_TYPE_TEXT_STRING_VALUE                                 0x04
#define SDP_DATA_TYPE_BOOLEAN_VALUE                                     0x05
#define SDP_DATA_TYPE_ELEMENT_SEQUENCE_VALUE                            0x06
#define SDP_DATA_TYPE_ELEMENT_ALTERNATIVE_VALUE                         0x07
#define SDP_DATA_TYPE_URL_VALUE                                         0x08

   /* The following Constants represent the actual Bluetooth Defined    */
   /* SDP Data Element Size Definitions (NOT their physical value when  */
   /* placed in the Data Element Type Descriptor.                       */
#define SDP_DATA_SIZE_1_BYTE_VALUE                                      0x00
#define SDP_DATA_SIZE_2_BYTES_VALUE                                     0x01
#define SDP_DATA_SIZE_4_BYTES_VALUE                                     0x02
#define SDP_DATA_SIZE_8_BYTES_VALUE                                     0x03
#define SDP_DATA_SIZE_16_BYTES_VALUE                                    0x04
#define SDP_DATA_SIZE_VARIABLE_1_BYTE_VALUE                             0x05
#define SDP_DATA_SIZE_VARIABLE_2_BYTES_VALUE                            0x06
#define SDP_DATA_SIZE_VARIABLE_4_BYTES_VALUE                            0x07

   /* The following Constants represent the actual Bluetooth Defined    */
   /* SDP Data Element Type Definitions as they are actually present    */
   /* in the SDP Data Type Element (when SDP_TYPE_DESCRIPTOR_MASK is    */
   /* applied).                                                         */
#define SDP_DATA_TYPE_NIL                               (SDP_DATA_TYPE_NIL_VALUE << SDP_TYPE_DESCRIPTOR_SHIFT_VALUE)
#define SDP_DATA_TYPE_NULL                              (SDP_DATA_TYPE_NULL_VALUE << SDP_TYPE_DESCRIPTOR_SHIFT_VALUE)
#define SDP_DATA_TYPE_UNSIGNED_INTEGER                  (SDP_DATA_TYPE_UNSIGNED_INTEGER_VALUE << SDP_TYPE_DESCRIPTOR_SHIFT_VALUE)
#define SDP_DATA_TYPE_SIGNED_2_COMPLEMENT_INTEGER       (SDP_DATA_TYPE_SIGNED_2_COMPLEMENT_INTEGER_VALUE << SDP_TYPE_DESCRIPTOR_SHIFT_VALUE)
#define SDP_DATA_TYPE_UUID                              (SDP_DATA_TYPE_UUID_VALUE << SDP_TYPE_DESCRIPTOR_SHIFT_VALUE)
#define SDP_DATA_TYPE_TEXT_STRING                       (SDP_DATA_TYPE_TEXT_STRING_VALUE << SDP_TYPE_DESCRIPTOR_SHIFT_VALUE)
#define SDP_DATA_TYPE_BOOLEAN                           (SDP_DATA_TYPE_BOOLEAN_VALUE << SDP_TYPE_DESCRIPTOR_SHIFT_VALUE)
#define SDP_DATA_TYPE_ELEMENT_SEQUENCE                  (SDP_DATA_TYPE_ELEMENT_SEQUENCE_VALUE << SDP_TYPE_DESCRIPTOR_SHIFT_VALUE)
#define SDP_DATA_TYPE_ELEMENT_ALTERNATIVE               (SDP_DATA_TYPE_ELEMENT_ALTERNATIVE_VALUE << SDP_TYPE_DESCRIPTOR_SHIFT_VALUE)
#define SDP_DATA_TYPE_URL                               (SDP_DATA_TYPE_URL_VALUE << SDP_TYPE_DESCRIPTOR_SHIFT_VALUE)

   /* The following Constants represent the actual Bluetooth Defined    */
   /* SDP Data Element Size Definitions as they are actually present    */
   /* in the SDP Data Type Element (when SDP_SIZE_INDEX_MASK is         */
   /* applied).                                                         */
#define SDP_DATA_SIZE_1_BYTE                            (SDP_DATA_SIZE_1_BYTE_VALUE << SDP_SIZE_INDEX_SHIFT_VALUE)
#define SDP_DATA_SIZE_2_BYTES                           (SDP_DATA_SIZE_2_BYTES_VALUE << SDP_SIZE_INDEX_SHIFT_VALUE)
#define SDP_DATA_SIZE_4_BYTES                           (SDP_DATA_SIZE_4_BYTES_VALUE << SDP_SIZE_INDEX_SHIFT_VALUE)
#define SDP_DATA_SIZE_8_BYTES                           (SDP_DATA_SIZE_8_BYTES_VALUE << SDP_SIZE_INDEX_SHIFT_VALUE)
#define SDP_DATA_SIZE_16_BYTES                          (SDP_DATA_SIZE_16_BYTES_VALUE << SDP_SIZE_INDEX_SHIFT_VALUE)
#define SDP_DATA_SIZE_VARIABLE_1_BYTE                   (SDP_DATA_SIZE_VARIABLE_1_BYTE_VALUE << SDP_SIZE_INDEX_SHIFT_VALUE)
#define SDP_DATA_SIZE_VARIABLE_2_BYTES                  (SDP_DATA_SIZE_VARIABLE_2_BYTES_VALUE << SDP_SIZE_INDEX_SHIFT_VALUE)
#define SDP_DATA_SIZE_VARIABLE_4_BYTES                  (SDP_DATA_SIZE_VARIABLE_4_BYTES_VALUE << SDP_SIZE_INDEX_SHIFT_VALUE)

   /* The following MACRO is a Utility MACRO that exists to aid in code */
   /* readability.  This MACRO accepts as input, the SDP Data Element   */
   /* Type and the SDP Data Element Size and returns the formatted      */
   /* Data Element Type.                                                */
#define MAKE_SDP_DATA_ELEMENT_TYPE(_x, _y)              ((Byte_t)(_x) | (Byte_t)(_y))

   /* The following type definition represents the Type Definition for  */
   /* the defined Bluetooth SDP Data Element.  It should be noted that  */
   /* the second member of this type is actually a Variable Data member */
   /* which could contain 0 or more bytes depending upon the value of   */
   /* the first Data Type member.  A MACRO is provided that will        */
   /* calculate the correct size of the structure when defining         */
   /* variables and/or allocating memory to hold SDP Data Types.        */
typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_Generic_Data_Element_t
{
   NonAlignedByte_t SDP_Data_Element_Type;
   NonAlignedByte_t Variable_Data[1];
} __PACKED_STRUCT_END__ SDP_Generic_Data_Element_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of a Generic         */
   /* SDP Data Element Data Structure based upon the number of RAW      */
   /* Bytes required.  The first parameter to this MACRO is the size    */
   /* (in Bytes) of the Data Element to store (NOT including the        */
   /* SDP_Data_Element_Type Byte).                                      */
#define SDP_GENERIC_DATA_ELEMENT_SIZE(_x)               (sizeof(SDP_Generic_Data_Element_t) - sizeof(Byte_t) + (unsigned int)(_x))

   /* The following Declarations are provided to allow the programmer   */
   /* a simple means to manipulate SDP Data Types.  The Actual Data     */
   /* Types themselves, may not be accurate, because the contents may   */
   /* need to be interpreted differently (for example, using an         */
   /* unsigned 16 bit value, which might hold a signed 16 bit value).   */
   /* This is done so that types that are not defined for the processor */
   /* can be manipulated in a portable manner (for example 16 byte      */
   /* integers).                                                        */
   /* * NOTE * SDP is a Big Endian protocol.  The declarations below    */
   /*          are defined to be in the Host Machine Endian Format,     */
   /*          however the User is free to swap Byte ordering at will   */
   /*          as required for the current use.                         */

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_NIL_Type_t
{
   Byte_t SDP_Data_Element_Type;
} __PACKED_STRUCT_END__ SDP_NIL_Type_t;

#define SDP_NIL_TYPE_SIZE                               (sizeof(SDP_NIL_Type_t))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_NULL_Type_t
{
   Byte_t SDP_Data_Element_Type;
} __PACKED_STRUCT_END__ SDP_NULL_Type_t;

#define SDP_NULL_TYPE_SIZE                              (sizeof(SDP_NULL_Type_t))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_Unsigned_Integer_1_Byte_t
{
   Byte_t SDP_Data_Element_Type;
   Byte_t Value;
} __PACKED_STRUCT_END__ SDP_Unsigned_Integer_1_Byte_t;

#define SDP_UNSIGNED_INTEGER_1_BYTE_SIZE                (sizeof(SDP_Unsigned_Integer_1_Byte_t))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_Unsigned_Integer_2_Bytes_t
{
   Byte_t           SDP_Data_Element_Type;
   NonAlignedWord_t Value;
} __PACKED_STRUCT_END__ SDP_Unsigned_Integer_2_Bytes_t;

#define SDP_UNSIGNED_INTEGER_2_BYTES_SIZE               (sizeof(SDP_Unsigned_Integer_2_Bytes_t))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_Unsigned_Integer_4_Bytes_t
{
   Byte_t            SDP_Data_Element_Type;
   NonAlignedDWord_t Value;
} __PACKED_STRUCT_END__ SDP_Unsigned_Integer_4_Bytes_t;

#define SDP_UNSIGNED_INTEGER_4_BYTES_SIZE               (sizeof(SDP_Unsigned_Integer_4_Bytes_t))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_Unsigned_Integer_8_Bytes_t
{
   Byte_t SDP_Data_Element_Type;
   Byte_t Byte_Value[8];
} __PACKED_STRUCT_END__ SDP_Unsigned_Integer_8_Bytes_t;

#define SDP_UNSIGNED_INTEGER_8_BYTES_SIZE               (sizeof(SDP_Unsigned_Integer_8_Bytes_t))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_Unsigned_Integer_16_Bytes_t
{
   Byte_t SDP_Data_Element_Type;
   Byte_t Byte_Value[16];
} __PACKED_STRUCT_END__ SDP_Unsigned_Integer_16_Bytes_t;

#define SDP_UNSIGNED_INTEGER_16_BYTES_SIZE              (sizeof(SDP_Unsigned_Integer_16_Bytes_t))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_Signed_Integer_1_Byte_t
{
   Byte_t  SDP_Data_Element_Type;
   SByte_t Value;
} __PACKED_STRUCT_END__ SDP_Signed_Integer_1_Byte_t;

#define SDP_SIGNED_INTEGER_1_BYTE_SIZE                  (sizeof(SDP_Signed_Integer_1_Byte_t))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_Signed_Integer_2_Bytes_t
{
   Byte_t            SDP_Data_Element_Type;
   NonAlignedSWord_t Value;
} __PACKED_STRUCT_END__ SDP_Signed_Integer_2_Bytes_t;

#define SDP_SIGNED_INTEGER_2_BYTES_SIZE                 (sizeof(SDP_Signed_Integer_2_Bytes_t))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_Signed_Integer_4_Bytes_t
{
   Byte_t             SDP_Data_Element_Type;
   NonAlignedSDWord_t Value;
} __PACKED_STRUCT_END__ SDP_Signed_Integer_4_Bytes_t;

#define SDP_SIGNED_INTEGER_4_BYTES_SIZE                 (sizeof(SDP_Signed_Integer_4_Bytes_t))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_Signed_Integer_8_Bytes_t
{
   Byte_t SDP_Data_Element_Type;
   Byte_t Byte_Value[8];
} __PACKED_STRUCT_END__ SDP_Signed_Integer_8_Bytes_t;

#define SDP_SIGNED_INTEGER_8_BYTES_SIZE                 (sizeof(SDP_Signed_Integer_8_Bytes_t))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_Signed_Integer_16_Bytes_t
{
   Byte_t SDP_Data_Element_Type;
   Byte_t Byte_Value[16];
} __PACKED_STRUCT_END__ SDP_Signed_Integer_16_Bytes_t;

#define SDP_SIGNED_INTEGER_16_BYTES_SIZE                (sizeof(SDP_Signed_Integer_16_Bytes_t))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_UUID_2_Bytes_t
{
   Byte_t    SDP_Data_Element_Type;
   UUID_16_t Value;
} __PACKED_STRUCT_END__ SDP_UUID_2_Bytes_t;

#define SDP_UUID_2_BYTES_SIZE                           (sizeof(SDP_UUID_2_Bytes_t))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_UUID_4_Bytes_t
{
   Byte_t    SDP_Data_Element_Type;
   UUID_32_t Value;
} __PACKED_STRUCT_END__ SDP_UUID_4_Bytes_t;

#define SDP_UUID_4_BYTES_SIZE                           (sizeof(SDP_UUID_4_Bytes_t))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_UUID_16_Bytes_t
{
   Byte_t     SDP_Data_Element_Type;
   UUID_128_t Value;
} __PACKED_STRUCT_END__ SDP_UUID_16_Bytes_t;

#define SDP_UUID_16_BYTES_SIZE                          (sizeof(SDP_UUID_16_Bytes_t))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_Text_1_Byte_Length_t
{
   Byte_t SDP_Data_Element_Type;
   Byte_t Text_Length;
   Byte_t Variable_Data[1];
} __PACKED_STRUCT_END__ SDP_Text_1_Byte_Length_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an                */
   /* SDP Text Data Element Structure based upon the number of          */
   /* characters that are required by the Text String.  The first       */
   /* parameter to this MACRO is the length (in bytes) of the Text      */
   /* String (NOT including the SDP Data Element Type and SDP Text      */
   /* Length).                                                          */
#define SDP_TEXT_1_BYTE_LENGTH_SIZE(_x)                 (sizeof(SDP_Text_1_Byte_Length_t) - sizeof(Byte_t) + (unsigned int)(_x))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_Text_2_Bytes_Length_t
{
   Byte_t           SDP_Data_Element_Type;
   NonAlignedWord_t Text_Length;
   Byte_t           Variable_Data[1];
} __PACKED_STRUCT_END__ SDP_Text_2_Bytes_Length_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an                */
   /* SDP Text Data Element Structure based upon the number of          */
   /* characters that are required by the Text String.  The first       */
   /* parameter to this MACRO is the length (in bytes) of the Text      */
   /* String (NOT including the SDP Data Element Type and SDP Text      */
   /* Length).                                                          */
#define SDP_TEXT_2_BYTES_LENGTH_SIZE(_x)                (sizeof(SDP_Text_2_Bytes_Length_t) - sizeof(Byte_t) + (unsigned int)(_x))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_Text_4_Bytes_Length_t
{
   Byte_t            SDP_Data_Element_Type;
   NonAlignedDWord_t Text_Length;
   Byte_t            Variable_Data[1];
} __PACKED_STRUCT_END__ SDP_Text_4_Bytes_Length_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an                */
   /* SDP Text Data Element Structure based upon the number of          */
   /* characters that are required by the Text String.  The first       */
   /* parameter to this MACRO is the length (in bytes) of the Text      */
   /* String (NOT including the SDP Data Element Type and SDP Text      */
   /* Length).                                                          */
#define SDP_TEXT_4_BYTES_LENGTH_SIZE(_x)                (sizeof(SDP_Text_4_Bytes_Length_t) - sizeof(Byte_t) + (unsigned int)(_x))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_Boolean_t
{
   Byte_t SDP_Data_Element_Type;
   Byte_t Value;
} __PACKED_STRUCT_END__ SDP_Boolean_t;

#define SDP_BOOLEAN_SIZE                                (sizeof(SDP_Boolean_t))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_Data_Element_Sequence_1_Byte_Length_t
{
   Byte_t SDP_Data_Element_Type;
   Byte_t Data_Element_Sequence_Length;
   Byte_t Variable_Data[1];
} __PACKED_STRUCT_END__ SDP_Data_Element_Sequence_1_Byte_Length_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an                */
   /* SDP Data_Element_Sequence Data Element Structure based upon the   */
   /* number of RAW Bytes that are required bye the                     */
   /* Data_Element_Sequence Field.  The first parameter to this MACRO   */
   /* is the size (in Bytes) of the Data_Element_Sequence Field (NOT    */
   /* including the SDP Data Element Type and SDP Data_Element_Sequence */
   /* Length).                                                          */
#define SDP_DATA_ELEMENT_SEQUENCE_1_BYTE_LENGTH_SIZE(_x) (sizeof(SDP_Data_Element_Sequence_1_Byte_Length_t) - sizeof(Byte_t) + (unsigned int)(_x))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_Data_Element_Sequence_2_Bytes_Length_t
{
   Byte_t           SDP_Data_Element_Type;
   NonAlignedWord_t Data_Element_Sequence_Length;
   Byte_t           Variable_Data[1];
} __PACKED_STRUCT_END__ SDP_Data_Element_Sequence_2_Bytes_Length_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an                */
   /* SDP Data_Element_Sequence Data Element Structure based upon the   */
   /* number of RAW Bytes that are required bye the                     */
   /* Data_Element_Sequence Field.  The first parameter to this MACRO   */
   /* is the size (in Bytes) of the Data_Element_Sequence Field (NOT    */
   /* including the SDP Data Element Type and SDP Data_Element_Sequence */
   /* Length).                                                          */
#define SDP_DATA_ELEMENT_SEQUENCE_2_BYTES_LENGTH_SIZE(_x) (sizeof(SDP_Data_Element_Sequence_2_Bytes_Length_t) - sizeof(Byte_t) + (unsigned int)(_x))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_Data_Element_Sequence_4_Bytes_Length_t
{
   Byte_t            SDP_Data_Element_Type;
   NonAlignedDWord_t Data_Element_Sequence_Length;
   Byte_t            Variable_Data[1];
} __PACKED_STRUCT_END__ SDP_Data_Element_Sequence_4_Bytes_Length_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an                */
   /* SDP Data_Element_Sequence Data Element Structure based upon the   */
   /* number of RAW Bytes that are required bye the                     */
   /* Data_Element_Sequence Field.  The first parameter to this MACRO   */
   /* is the size (in Bytes) of the Data_Element_Sequence Field (NOT    */
   /* including the SDP Data Element Type and SDP Data_Element_Sequence */
   /* Length).                                                          */
#define SDP_DATA_ELEMENT_SEQUENCE_4_BYTES_LENGTH_SIZE(_x) (sizeof(SDP_Data_Element_Sequence_4_Bytes_Length_t) - sizeof(Byte_t) + (unsigned int)(_x))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_Data_Element_Alternative_1_Byte_Length_t
{
   Byte_t SDP_Data_Element_Type;
   Byte_t Data_Element_Alternative_Length;
   Byte_t Variable_Data[1];
} __PACKED_STRUCT_END__ SDP_Data_Element_Alternative_1_Byte_Length_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an                */
   /* SDP Data_Element_Alternative Data Element Structure based upon the*/
   /* number of RAW Bytes that are required bye the                     */
   /* Data_Element_Alternative Field.  The first parameter to this MACRO*/
   /* is the size (in Bytes) of the Data_Element_Alternative Field (NOT */
   /* including the SDP Data Element Type and                           */
   /* SDP Data_Element_Alternative Length).                             */
#define SDP_DATA_ELEMENT_ALTERNATIVE_1_BYTE_LENGTH_SIZE(_x) (sizeof(SDP_Data_Element_Alternative_1_Byte_Length_t) - sizeof(Byte_t) + (unsigned int)(_x))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_Data_Element_Alternative_2_Bytes_Length_t
{
   Byte_t           SDP_Data_Element_Type;
   NonAlignedWord_t Data_Element_Alternative_Length;
   Byte_t           Variable_Data[1];
} __PACKED_STRUCT_END__ SDP_Data_Element_Alternative_2_Bytes_Length_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an                */
   /* SDP Data_Element_Alternative Data Element Structure based upon the*/
   /* number of RAW Bytes that are required bye the                     */
   /* Data_Element_Alternative Field.  The first parameter to this MACRO*/
   /* is the size (in Bytes) of the Data_Element_Alternative Field (NOT */
   /* including the SDP Data Element Type and                           */
   /* SDP Data_Element_Alternative Length).                             */
#define SDP_DATA_ELEMENT_ALTERNATIVE_2_BYTES_LENGTH_SIZE(_x) (sizeof(SDP_Data_Element_Alternative_2_Bytes_Length_t) - sizeof(Byte_t) + (unsigned int)(_x))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_Data_Element_Alternative_4_Bytes_Length_t
{
   Byte_t            SDP_Data_Element_Type;
   NonAlignedDWord_t Data_Element_Alternative_Length;
   Byte_t            Variable_Data[1];
} __PACKED_STRUCT_END__ SDP_Data_Element_Alternative_4_Bytes_Length_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an                */
   /* SDP Data_Element_Alternative Data Element Structure based upon the*/
   /* number of RAW Bytes that are required bye the                     */
   /* Data_Element_Alternative Field.  The first parameter to this MACRO*/
   /* is the size (in Bytes) of the Data_Element_Alternative Field (NOT */
   /* including the SDP Data Element Type and                           */
   /* SDP Data_Element_Alternative Length).                             */
#define SDP_DATA_ELEMENT_ALTERNATIVE_4_BYTES_LENGTH_SIZE(_x) (sizeof(SDP_Data_Element_Alternative_4_Bytes_Length_t) - sizeof(Byte_t) + (unsigned int)(_x))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_URL_1_Byte_Length_t
{
   Byte_t SDP_Data_Element_Type;
   Byte_t URL_Length;
   Byte_t Variable_Data[1];
} __PACKED_STRUCT_END__ SDP_URL_1_Byte_Length_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an                */
   /* SDP URL Data Element Structure based upon the number of           */
   /* characters that are required by the URL String.  The first        */
   /* parameter to this MACRO is the length (in bytes) of the URL       */
   /* String (NOT including the SDP Data Element Type and SDP URL       */
   /* Length).                                                          */
#define SDP_URL_1_BYTE_LENGTH_SIZE(_x)                  (sizeof(SDP_URL_1_Byte_Length_t) - sizeof(Byte_t) + (unsigned int)(_x))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_URL_2_Bytes_Length_t
{
   Byte_t           SDP_Data_Element_Type;
   NonAlignedWord_t URL_Length;
   Byte_t           Variable_Data[1];
} __PACKED_STRUCT_END__ SDP_URL_2_Bytes_Length_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an                */
   /* SDP URL Data Element Structure based upon the number of           */
   /* characters that are required by the URL String.  The first        */
   /* parameter to this MACRO is the length (in bytes) of the URL       */
   /* String (NOT including the SDP Data Element Type and SDP URL       */
   /* Length).                                                          */
#define SDP_URL_2_BYTES_LENGTH_SIZE(_x)                 (sizeof(SDP_URL_2_Bytes_Length_t) - sizeof(Byte_t) + (unsigned int)(_x))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_URL_4_Bytes_Length_t
{
   Byte_t            SDP_Data_Element_Type;
   NonAlignedDWord_t URL_Length;
   Byte_t            Variable_Data[1];
} __PACKED_STRUCT_END__ SDP_URL_4_Bytes_Length_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an                */
   /* SDP URL Data Element Structure based upon the number of           */
   /* characters that are required by the URL String.  The first        */
   /* parameter to this MACRO is the length (in bytes) of the URL       */
   /* String (NOT including the SDP Data Element Type and SDP URL       */
   /* Length).                                                          */
#define SDP_URL_4_BYTES_LENGTH_SIZE(_x)                 (sizeof(SDP_URL_4_Bytes_Length_t) - sizeof(Byte_t) + (unsigned int)(_x))

   /* SDP Protocol Data Unit Identifiers (PDU ID's).                    */
#define SDP_PDU_ID_ERROR_RESPONSE                                       0x01
#define SDP_PDU_ID_SERVICE_SEARCH_REQUEST                               0x02
#define SDP_PDU_ID_SERVICE_SEARCH_RESPONSE                              0x03
#define SDP_PDU_ID_SERVICE_ATTRIBUTE_REQUEST                            0x04
#define SDP_PDU_ID_SERVICE_ATTRIBUTE_RESPONSE                           0x05
#define SDP_PDU_ID_SERVICE_SEARCH_ATTRIBUTE_REQUEST                     0x06
#define SDP_PDU_ID_SERVICE_SEARCH_ATTRIBUTE_RESPONSE                    0x07

   /* SDP Protocol Data Unit Error Codes.                               */
#define SDP_ERROR_CODE_INVALID_UNSUPPORTED_SDP_VERSION                  0x0001
#define SDP_ERROR_CODE_INVALID_SERVICE_RECORD_HANDLE                    0x0002
#define SDP_ERROR_CODE_INVALID_REQUEST_SYNTAX                           0x0003
#define SDP_ERROR_CODE_INVALID_PDU_SIZE                                 0x0004
#define SDP_ERROR_CODE_INVALID_CONTINUATION_STATE                       0x0005
#define SDP_ERROR_CODE_INSUFFICIENT_RESOURCES                           0x0006

   /* The following type declaration represents the structure of the    */
   /* Header of an SDP Protocol Data Packet.  This Header Information is*/
   /* contained in Every Defined SDP Protocol Packet.  This structure   */
   /* forms the basis of additional defined SDP Protocol Packets.  Since*/
   /* this structure is present at the begining of Every Defined SDP    */
   /* Protocol Packet, this structure will be the first element of Every*/
   /* Defined SDP Protocol Packet in this file.                         */
   /* * NOTE * SDP is a Big Endian protocol.  The declarations below    */
   /*          are defined to be in the Host Machine Endian Format,     */
   /*          however the User is free to swap Byte ordering at will   */
   /*          as required for the current use.                         */
typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_Protocol_Data_Unit_Header_t
{
   Byte_t           PDU_ID;
   NonAlignedWord_t Transaction_ID;
   NonAlignedWord_t Parameter_Length;
} __PACKED_STRUCT_END__ SDP_Protocol_Data_Unit_Header_t;

#define SDP_PROTOCOL_DATA_UNIT_HEADER_SIZE                      (sizeof(SDP_Protocol_Data_Unit_Header_t))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_Continuation_State_t
{
   Byte_t Info_Length;
   Byte_t Variable_Data[1];
} __PACKED_STRUCT_END__ SDP_Continuation_State_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an SDP            */
   /* Continuation Statue Data Element structure based upon the length  */
   /* of bytes required to hold the continuation state data.  The first */
   /* parameter to this MACRO is the size (in Bytes) of the Continuation*/
   /* State Information Data that is required/present.                  */
#define SDP_CONTINUATION_STATE_SIZE(_x)                         (sizeof(SDP_Continuation_State_t) - sizeof(Byte_t) + (unsigned int)(_x))

   /* The following definition defines the Maximum Length (in Bytes) of */
   /* the actual Continuation Data that can be present in a Continuation*/
   /* State Data Element.                                               */
#define SDP_MAX_CONTINUATION_STATE_BYTES                                0x10

   /* The following definition provides the maximum size that the       */
   /* entire continuation state variable can occupy (including the      */
   /* Length field).                                                    */
#define SDP_MAX_CONTINUATION_STATE_SIZE                         (SDP_CONTINUATION_STATE_SIZE(SDP_MAX_CONTINUATION_STATE_BYTES))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_Error_Response_t
{
   SDP_Protocol_Data_Unit_Header_t SDP_Protocol_Data_Unit_Header;
   NonAlignedWord_t                Error_Code;
   Byte_t                          Variable_Data[1];
} __PACKED_STRUCT_END__ SDP_Error_Response_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an SDP Error      */
   /* Response Packet Data structure based upon the length of bytes     */
   /* that are required for the Error Info portion of the Error.  The   */
   /* first parameter to this MACRO is the size (in Bytes) of the       */
   /* Error Information Data that is part of the Error Response Packet. */
#define SDP_ERROR_RESPONSE_SIZE(_x)                             (sizeof(SDP_Error_Response_t) - sizeof(Byte_t) + (unsigned int)(_x))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_Service_Search_Request_t
{
   SDP_Protocol_Data_Unit_Header_t SDP_Protocol_Data_Unit_Header;
   Byte_t                          Variable_Data[1];
} __PACKED_STRUCT_END__ SDP_Service_Search_Request_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an SDP Service    */
   /* Search Request Packet Data structure based upon the length of     */
   /* bytes that are required for ALL of the Search Request Information.*/
   /* The first parameter to this MACRO is the size (in Bytes) of ALL   */
   /* the Service Search Request Information that is part of the SDP    */
   /* Service Search Request Packet.                                    */
#define SDP_SERVICE_SEARCH_REQUEST_SIZE(_x)                     (sizeof(SDP_Service_Search_Request_t) - sizeof(Byte_t) + (unsigned int)(_x))

#define SDP_SERVICE_SEARCH_REQUEST_MAX_UUID                             0x0C

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_Service_Search_Response_t
{
   SDP_Protocol_Data_Unit_Header_t SDP_Protocol_Data_Unit_Header;
   NonAlignedWord_t                Total_Service_Record_Count;
   NonAlignedWord_t                Current_Service_Record_Count;
   Byte_t                          Variable_Data[1];
} __PACKED_STRUCT_END__ SDP_Service_Search_Response_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an SDP Service    */
   /* Search Response Packet Data structure based upon the length of    */
   /* bytes that are required for the Service Record Handle List and    */
   /* Continuation State information.  The first parameter to this      */
   /* MACRO is the size (in Bytes) of ALL the Service Record Handles    */
   /* and the Continuation State Information that is part of the SDP    */
   /* Service Search Response Packet.                                   */
#define SDP_SERVICE_SEARCH_RESPONSE_SIZE(_x)                    (sizeof(SDP_Service_Search_Response_t) - sizeof(Byte_t) + (unsigned int)(_x))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_Service_Attribute_Request_t
{
   SDP_Protocol_Data_Unit_Header_t SDP_Protocol_Data_Unit_Header;
   NonAlignedDWord_t               Service_Record_Handle;
   NonAlignedWord_t                Maximum_Attribute_Byte_Count;
   Byte_t                          Variable_Data[1];
} __PACKED_STRUCT_END__ SDP_Service_Attribute_Request_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an SDP Service    */
   /* Attribute Request Packet Data structure based upon the length of  */
   /* bytes that are required to hold the entire Attribute List and the */
   /* Continuation State Information.  The first parameter to this MACRO*/
   /* is the size (in Bytes) required by the entire Attribute List and  */
   /* the Continuation State Information that is part of the SDP        */
   /* Service Attribute Request Packet.                                 */
#define SDP_SERVICE_ATTRIBUTE_REQUEST_SIZE(_x)                  (sizeof(SDP_Service_Attribute_Request_t) - sizeof(Byte_t) + (unsigned int)(_x))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_Service_Attribute_Response_t
{
   SDP_Protocol_Data_Unit_Header_t SDP_Protocol_Data_Unit_Header;
   NonAlignedWord_t                Attribute_List_Byte_Count;
   Byte_t                          Variable_Data[1];
} __PACKED_STRUCT_END__ SDP_Service_Attribute_Response_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an SDP Service    */
   /* Attribute Response  Packet Data structure based upon the length of*/
   /* bytes that are required to hold the entire Attribute List and the */
   /* Continuation State Information.  The first parameter to this MACRO*/
   /* is the size (in Bytes) required by the entire Attribute List and  */
   /* the Continuation State Information that is part of the SDP        */
   /* Service Attribute Response Packet.                                */
#define SDP_SERVICE_ATTRIBUTE_RESPONSE_SIZE(_x)                 (sizeof(SDP_Service_Attribute_Response_t) - sizeof(Byte_t) + (unsigned int)(_x))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_Service_Search_Attribute_Request_t
{
   SDP_Protocol_Data_Unit_Header_t SDP_Protocol_Data_Unit_Header;
   Byte_t                          Variable_Data[1];
} __PACKED_STRUCT_END__ SDP_Service_Search_Attribute_Request_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an SDP Service    */
   /* Search Attribute Request Packet Data structure based upon the     */
   /* length of ALL the bytes that are required for ALL of the Search   */
   /* Service Attribute Request Information.  The first parameter to    */
   /* this MACRO is the size (in Bytes) of ALL the Service Attribute    */
   /* Request Information that is part of the SDP Service Search        */
   /* Attribute Request Packet.                                         */
#define SDP_SERVICE_SEARCH_ATTRIBUTE_REQUEST_SIZE(_x)           (sizeof(SDP_Service_Search_Attribute_Request_t) - sizeof(Byte_t) + (unsigned int)(_x))

typedef __PACKED_STRUCT_BEGIN__ struct _tagSDP_Service_Search_Attribute_Response_t
{
   SDP_Protocol_Data_Unit_Header_t SDP_Protocol_Data_Unit_Header;
   NonAlignedWord_t                Attribute_Lists_Byte_Count;
   Byte_t                          Variable_Data[1];
} __PACKED_STRUCT_END__ SDP_Service_Search_Attribute_Response_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an SDP Service    */
   /* Search Attribute Response Packet Data structure based upon the    */
   /* length of bytes that are required for the Attribute List(s) Data  */
   /* and the Continuation State Information.  The first parameter to   */
   /* this MACRO is the size (in Bytes) of ALL the Attribute List(s)    */
   /* and the Continuation State Information that is part of the SDP    */
   /* Service Search Attribute Response Packet.                         */
#define SDP_SERVICE_SEARCH_ATTRIBUTE_RESPONSE_SIZE(_x)          (sizeof(SDP_Service_Search_Attribute_Response_t) - sizeof(Byte_t) + (unsigned int)(_x))

#endif
