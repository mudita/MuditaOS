/*****< avrtypes.h >***********************************************************/
/*      Copyright 2006 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  AVRTYPES - Stonestreet One Bluetooth Stack Audio/Video Remote Control     */
/*             Profile Type Definitions and Constants.                        */
/*                                                                            */
/*  Author:  Nikhil Deo                                                       */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname       Description of Modification                  */
/*   --------  -----------       ---------------------------------------------*/
/*   01/09/06  N. Deo            Initial creation.                            */
/******************************************************************************/
#ifndef __AVRTYPESH__
#define __AVRTYPESH__

#include "BTTypes.h"            /* Bluetooth Type Definitions.                */

   /* SDP Audio/Video Remote Control Profile Service Class UUID's.      */

   /* The following MACRO is a utility MACRO that assigns the Audio/    */
   /* Video Remote Control Profile Bluetooth Universally Unique         */
   /* Identifier (AUDIO_VIDEO_REMOTE_CONTROL_PROFILE_UUID_16) to the    */
   /* specified UUID_16_t variable.  This MACRO accepts one parameter   */
   /* which is the UUID_16_t variable that is to receive the            */
   /* AUDIO_VIDEO_REMOTE_CONTROL_PROFILE_UUID_16 Constant value.        */
#define SDP_ASSIGN_AUDIO_VIDEO_REMOTE_CONTROL_PROFILE_UUID_16(_x)    ASSIGN_SDP_UUID_16((_x), 0x11, 0x0E)

   /* The following MACRO is a utility MACRO that assigns the Audio/    */
   /* Video Remote Control Profile Bluetooth Universally Unique         */
   /* Identifier (AUDIO_VIDEO_REMOTE_CONTROL_PROFILE_UUID_32) to the    */
   /* specified UUID_32_t variable.  This MACRO accepts one parameter   */
   /* which is the UUID_32_t variable that is to receive the            */
   /* AUDIO_VIDEO_REMOTE_CONTROL_PROFILE_UUID_32 Constant value.        */
#define SDP_ASSIGN_AUDIO_VIDEO_REMOTE_CONTROL_PROFILE_UUID_32(_x)    ASSIGN_SDP_UUID_32((_x), 0x00, 0x00, 0x11, 0x0E)

   /* The following MACRO is a utility MACRO that assigns the Audio/    */
   /* Video Remote Control Profile Bluetooth Universally Unique         */
   /* Identifier (AUDIO_VIDEO_CONTROL_REMOTE_CONTROL_UUID_128) to the   */
   /* specified UUID_128_t variable.  This MACRO accepts one parameter  */
   /* which is the UUID_128_t variable that is to receive the           */
   /* AUDIO_VIDEO_CONTROL_REMOTE_CONTROL_UUID_128 Constant value.       */
#define SDP_ASSIGN_AUDIO_VIDEO_REMOTE_CONTROL_PROFILE_UUID_128(_x)   ASSIGN_SDP_UUID_128((_x), 0x00, 0x00, 0x11, 0x0E, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /* The following MACRO is a utility MACRO that assigns the Audio/    */
   /* Video Remote Control Target Service Class Bluetooth Universally   */
   /* Unique Identifier (AUDIO_VIDEO_REMOTE_CONTROL_TARGET_UUID_16) to  */
   /* the specified UUID_16_t variable.  This MACRO accepts one         */
   /* parameter which is the UUID_16_t variable that is to receive the  */
   /* AUDIO_VIDEO_REMOTE_CONTROL_TARGET_UUID_16 Constant value.         */
#define SDP_ASSIGN_AUDIO_VIDEO_REMOTE_CONTROL_TARGET_UUID_16(_x)     ASSIGN_SDP_UUID_16((_x), 0x11, 0x0C)

   /* The following MACRO is a utility MACRO that assigns the Audio/    */
   /* Video Remote Control Target Service Class Bluetooth Universally   */
   /* Unique Identifier (AUDIO_VIDEO_REMOTE_CONTROL_TARGET_UUID_32) to  */
   /* the specified UUID_32_t variable.  This MACRO accepts one         */
   /* parameter which is the UUID_32_t variable that is to receive the  */
   /* AUDIO_VIDEO_REMOTE_CONTROL_TARGET_UUID_32 Constant value.         */
#define SDP_ASSIGN_AUDIO_VIDEO_REMOTE_CONTROL_TARGET_UUID_32(_x)     ASSIGN_SDP_UUID_32((_x), 0x00, 0x00, 0x11, 0x0C)

   /* The following MACRO is a utility MACRO that assigns the Audio/    */
   /* Video Control Transport Protocol Remote Control Target Service    */
   /* Class Bluetooth Universally Unique Identifier                     */
   /* (AUDIO_VIDEO_REMOTE_CONTROL_TARGET_UUID_128) to the specified     */
   /* UUID_128_t variable.  This MACRO accepts one parameter which is   */
   /* the UUID_128_t variable that is to receive the                    */
   /* AUDIO_VIDEO_CONTROL_REMOTE_CONTROL_TARGET_UUID_128 Constant value.*/
#define SDP_ASSIGN_AUDIO_VIDEO_REMOTE_CONTROL_TARGET_UUID_128(_x)    ASSIGN_SDP_UUID_128((_x), 0x00, 0x00, 0x11, 0x0C, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /* The following MACRO is a utility MACRO that assigns the Audio/    */
   /* Video Remote Control Profile Bluetooth Universally Unique         */
   /* Identifier (AUDIO_VIDEO_REMOTE_CONTROL_PROFILE_UUID_16) to the    */
   /* specified UUID_16_t variable.  This MACRO accepts one parameter   */
   /* which is the UUID_16_t variable that is to receive the            */
   /* AUDIO_VIDEO_REMOTE_CONTROL_PROFILE_UUID_16 Constant value.        */
#define SDP_ASSIGN_AUDIO_VIDEO_REMOTE_CONTROL_CONTROLLER_UUID_16(_x)    ASSIGN_SDP_UUID_16((_x), 0x11, 0x0F)

   /* The following MACRO is a utility MACRO that assigns the Audio/    */
   /* Video Remote Control Profile Bluetooth Universally Unique         */
   /* Identifier (AUDIO_VIDEO_REMOTE_CONTROL_PROFILE_UUID_32) to the    */
   /* specified UUID_32_t variable.  This MACRO accepts one parameter   */
   /* which is the UUID_32_t variable that is to receive the            */
   /* AUDIO_VIDEO_REMOTE_CONTROL_PROFILE_UUID_32 Constant value.        */
#define SDP_ASSIGN_AUDIO_VIDEO_REMOTE_CONTROL_CONTROLLER_UUID_32(_x)    ASSIGN_SDP_UUID_32((_x), 0x00, 0x00, 0x11, 0x0F)

   /* The following MACRO is a utility MACRO that assigns the Audio/    */
   /* Video Remote Control Profile Bluetooth Universally Unique         */
   /* Identifier (AUDIO_VIDEO_CONTROL_REMOTE_CONTROL_UUID_128) to the   */
   /* specified UUID_128_t variable.  This MACRO accepts one parameter  */
   /* which is the UUID_128_t variable that is to receive the           */
   /* AUDIO_VIDEO_CONTROL_REMOTE_CONTROL_UUID_128 Constant value.       */
#define SDP_ASSIGN_AUDIO_VIDEO_REMOTE_CONTROL_CONTROLLER_UUID_128(_x)   ASSIGN_SDP_UUID_128((_x), 0x00, 0x00, 0x11, 0x0F, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /* The following definitions represent the Supported Features that   */
   /* can be supported by the AVRCP controller.  These SDP Definitions  */
   /* are used with the Supported Features Attribute.                   */
#define SDP_AVRCP_SUPPORTED_FEATURES_CONTROLLER_CATEGORY_1           (0x0001)
#define SDP_AVRCP_SUPPORTED_FEATURES_CONTROLLER_CATEGORY_2           (0x0002)
#define SDP_AVRCP_SUPPORTED_FEATURES_CONTROLLER_CATEGORY_3           (0x0004)
#define SDP_AVRCP_SUPPORTED_FEATURES_CONTROLLER_CATEGORY_4           (0x0008)
#define SDP_AVRCP_SUPPORTED_FEATURES_CONTROLLER_BROWSING             (0x0040)

   /* The following definitions represent the Supported Features that   */
   /* can be supported by the AVRCP target.  These SDP Definitions are  */
   /* used with the Supported Features Attribute.                       */
#define SDP_AVRCP_SUPPORTED_FEATURES_TARGET_CATEGORY_1               (0x0001)
#define SDP_AVRCP_SUPPORTED_FEATURES_TARGET_CATEGORY_2               (0x0002)
#define SDP_AVRCP_SUPPORTED_FEATURES_TARGET_CATEGORY_3               (0x0004)
#define SDP_AVRCP_SUPPORTED_FEATURES_TARGET_CATEGORY_4               (0x0008)
#define SDP_AVRCP_SUPPORTED_FEATURES_TARGET_PLAYER_SETTINGS          (0x0010)
#define SDP_AVRCP_SUPPORTED_FEATURES_TARGET_GROUP_NAVIGATION         (0x0020)
#define SDP_AVRCP_SUPPORTED_FEATURES_TARGET_BROWSING                 (0x0040)
#define SDP_AVRCP_SUPPORTED_FEATURES_TARGET_MULTIPLE_MEDIA_PLAYERS   (0x0080)

   /* The following constant defines the Profile Version Number that is */
   /* supported by this implementation (this is the value that is used  */
   /* in the SDP Records for the actual Profile Version).               */
#define AVRCP_PROFILE_VERSION                                        (0x0104)

   /* The following constants define the previous Profile Versions of   */
   /* the Audio/Video Remote Control Profile that are supported by this */
   /* implementation.  These values can be used in the SDP Records to   */
   /* specify a specific AVRCP Version.                                 */
#define AVRCP_PROFILE_VERSION_1_0                                    (0x0100)
#define AVRCP_PROFILE_VERSION_1_3                                    (0x0103)
#define AVRCP_PROFILE_VERSION_1_4                                    (0x0104)

   /* The following value represents the minimum supported MTU (Maximum */
   /* Transmission Unit) that is allowed when establishing a browsing   */
   /* channel.  This value is larger than the default L2CAP MTU to      */
   /* provide a 'reasonable' browsing experience.                       */
#define AVRCP_MINIMUM_BROWSING_CHANNEL_MTU                           (335)

   /* The following value represents the defined maximum size of an     */
   /* individual AVRCP message (for all channels except the Browsing    */
   /* Channel).                                                         */
   /* * NOTE * This value is defined in the IEEE 1394 "AV/C Digital     */
   /*          Interface Command Set" Specification.                    */
#define AVRCP_MAXIMUM_NON_BROWSING_MESSAGE_SIZE                      (512)

   /* The following structure represents the header of a AVRCP Command  */
   /* frame.  Byte0 contains CTYPE, Byte1 contains the SUBUNIT type and */
   /* SUBUNIT ID information and Opcode describes the procedure.        */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Command_Header_t
{
   Byte_t  Byte0;
   Byte_t  Byte1;
   Byte_t  Opcode;
} __PACKED_STRUCT_END__ AVRCP_Command_Header_t;

#define AVRCP_COMMAND_HEADER_SIZE                                    (sizeof(AVRCP_Command_Frame_t) - BYTE_SIZE)

   /* The following structure represents the structure of a AVRCP       */
   /* Command Frame.  Header is described above and command data points */
   /* to the operands for this command.                                 */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Command_Frame_t
{
   AVRCP_Command_Header_t AVRCP_Command_Header;
   Byte_t                 CommandData[1];
} __PACKED_STRUCT_END__ AVRCP_Command_Frame_t;

#define AVRCP_CALCULATE_COMMAND_FRAME_SIZE(_x)                       (AVRCP_COMMAND_HEADER_SIZE+(Byte_t)(_x))

   /* The following structure represents the header of a AVRCP Response */
   /* frame.  Byte0 contains RESPONSE type, Byte1 contains the SUBUNIT  */
   /* type and SUBUNIT ID information and Opcode describes the          */
   /* procedure.                                                        */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Response_Header_t
{
   Byte_t  Byte0;
   Byte_t  Byte1;
   Byte_t  Opcode;
} __PACKED_STRUCT_END__ AVRCP_Response_Header_t;

#define AVRCP_RESPONSE_HEADER_SIZE                                   (sizeof(AVRCP_Command_Frame_t) - BYTE_SIZE)

   /* The following structure represents the structure of a AVRCP       */
   /* Response Frame.  Header is described above and Response data      */
   /* points to the operands for this command.                          */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Response_Frame_t
{
   AVRCP_Response_Header_t AVRCP_Response_Header;
   Byte_t                  ResponseData[1];
} __PACKED_STRUCT_END__ AVRCP_Response_Frame_t;

#define AVRCP_CALCULATE_RESPONSE_FRAME_SIZE(_x)                      (AVRCP_COMMAND_HEADER_SIZE+(Byte_t)(_x))

   /* The following constants define the Mask and Shift values to       */
   /* extract or fill CTYPE values from/in Byte0 of AVRCP Command Frame.*/
   /* * NOTE * These values are defined in the IEEE 1394 "AV/C Digital  */
   /*          Interface Command Set" Specification.                    */
#define AVRCP_CTYPE_MASK                                             (0x0F)
#define AVRCP_CTYPE_SHIFT                                            (0x00)

   /* The following constants define the various CTYPE values that are  */
   /* used by AVRCP.                                                    */
   /* * NOTE * These values are defined in the IEEE 1394 "AV/C Digital  */
   /*          Interface Command Set" Specification.                    */
#define AVRCP_CTYPE_CONTROL                                          (0x00)
#define AVRCP_CTYPE_STATUS                                           (0x01)
#define AVRCP_CTYPE_SPECIFIC_INQUIRY                                 (0x02)
#define AVRCP_CTYPE_NOTIFY                                           (0x03)
#define AVRCP_CTYPE_GENERAL_INQUIRY                                  (0x04)

   /* The following constants define the Mask and Shift values to       */
   /* extract or fill Response code values from/in Byte0 of AVRCP       */
   /* Response Frame.                                                   */
   /* * NOTE * These values are defined in the IEEE 1394 "AV/C Digital  */
   /*          Interface Command Set" Specification.                    */
#define AVRCP_RESPONSE_MASK                                          (0x0F)
#define AVRCP_RESPONSE_SHIFT                                         (0x00)

   /* The following constants define the various RESPONSE values that   */
   /* are used by AVRCP.                                                */
   /* * NOTE * These values are defined in the IEEE 1394 "AV/C Digital  */
   /*          Interface Command Set" Specification.                    */
#define AVRCP_RESPONSE_NOT_IMPLEMENTED                               (0x08)
#define AVRCP_RESPONSE_ACCEPTED                                      (0x09)
#define AVRCP_RESPONSE_REJECTED                                      (0x0A)
#define AVRCP_RESPONSE_IN_TRANSITION                                 (0x0B)
#define AVRCP_RESPONSE_STABLE                                        (0x0C)
#define AVRCP_RESPONSE_CHANGED                                       (0x0D)
#define AVRCP_RESPONSE_INTERIM                                       (0x0F)

   /* The following constants define the Mask and shift values to       */
   /* extract or fill values from/in Byte1 of AVRCP Command or response */
   /* frame.                                                            */
   /* * NOTE * These values are defined in the IEEE 1394 "AV/C Digital  */
   /*          Interface Command Set" Specification.                    */
#define AVRCP_SUBUNIT_TYPE_MASK                                      (0x1F)
#define AVRCP_SUBUNIT_TYPE_SHIFT                                     (0x03)
#define AVRCP_SUBUNIT_ID_MASK                                        (0x07)
#define AVRCP_SUBUNIT_ID_SHIFT                                       (0x00)

   /* The following constants represent the supported SUBUNIT types.    */
   /* * NOTE * These values are defined in the IEEE 1394 "AV/C Digital  */
   /*          Interface Command Set" Specification.                    */
#define AVRCP_SUBUNIT_TYPE_VIDEO_MONITOR                             (0x00)
#define AVRCP_SUBUNIT_TYPE_DISC_RECORDER_PLAYER                      (0x03)
#define AVRCP_SUBUNIT_TYPE_TAPE_RECORDER_PLAYER                      (0x04)
#define AVRCP_SUBUNIT_TYPE_TUNER                                     (0x05)
#define AVRCP_SUBUNIT_TYPE_VIDEO_CAMERA                              (0x07)
#define AVRCP_SUBUNIT_TYPE_PANEL                                     (0x09)
#define AVRCP_SUBUNIT_TYPE_VENDOR_SPECIFIC                           (0x1C)
#define AVRCP_SUBUNIT_TYPE_EXTENDED                                  (0x1E)
#define AVRCP_SUBUNIT_TYPE_UNIT                                      (0x1F)

   /* The following constants represent the supported SUBUNIT           */
   /* identifiers.                                                      */
   /* * NOTE * These values are defined in the IEEE 1394 "AV/C Digital  */
   /*          Interface Command Set" Specification.                    */
#define AVRCP_SUBUNIT_ID_INSTANCE_0                                  (0x00)
#define AVRCP_SUBUNIT_ID_INSTANCE_1                                  (0x01)
#define AVRCP_SUBUNIT_ID_INSTANCE_2                                  (0x02)
#define AVRCP_SUBUNIT_ID_INSTANCE_3                                  (0x03)
#define AVRCP_SUBUNIT_ID_INSTANCE_4                                  (0x04)
#define AVRCP_SUBUNIT_ID_EXTENDED                                    (0x05)
#define AVRCP_SUBUNIT_ID_IGNORE                                      (0x07)

   /* The following constants represent the OPCODEs for the various     */
   /* procedures that are supported by AVRCP.                           */
#define AVRCP_OPCODE_UNIT_INFO                                       (0x30)
#define AVRCP_OPCODE_SUBUNIT_INFO                                    (0x31)
#define AVRCP_OPCODE_VENDOR_DEPENDENT                                (0x00)
#define AVRCP_OPCODE_PASS_THROUGH                                    (0x7C)

   /* This structure defines the Company ID to be used in the UNIT INFO */
   /* Response message.                                                 */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Company_ID_t
{
   Byte_t CompanyID0;
   Byte_t CompanyID1;
   Byte_t CompanyID2;
} __PACKED_STRUCT_END__ AVRCP_Company_ID_t;

#define AVRCP_COMPANY_ID_SIZE                                        (sizeof(AVRCP_Company_ID_t))

   /* This is a MACRO that formats a Company ID with the supplied three */
   /* bytes.  The parameter to this function is the name of the         */
   /* AVRCP_Company_ID_t variable that is to receive the specified      */
   /* Company ID bytes.                                                 */
#define ASSIGN_AVRCP_COMPANY_ID(_dest, _a, _b, _c) \
{                                                  \
   (_dest).CompanyID0 = (_a);                      \
   (_dest).CompanyID1 = (_b);                      \
   (_dest).CompanyID2 = (_c);                      \
}

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* Comparison of two AVRCP_Company_ID_t variables.  This MACRO only  */
   /* returns whether the two AVRCP_Company_ID_t variables are equal    */
   /* (MACRO returns boolean result) NOT less than/greater than.  The   */
   /* two parameters to this MACRO are both of type AVRCP_Company_ID_t  */
   /* and represent the AVRCP_Company_ID_t variables to compare.        */
#define COMPARE_AVRCP_COMPANY_ID(_x, _y) (((_x).CompanyID0 == (_y).CompanyID0) && ((_x).CompanyID1 == (_y).CompanyID1) && ((_x).CompanyID2 == (_y).CompanyID2))

   /* The following MACRO is responsible for assigning the Bluetooth SIG*/
   /* Company ID into an AVRCP_Compant_ID_t variable.  This function    */
   /* accepts as input the name of the AVRCP_Company_ID_t variable that */
   /* is to receive the A/V Bluetooth SIG Company ID.                   */
#define ASSIGN_AVRCP_BLUETOOTH_COMPANY_ID(_dest)                     ASSIGN_AVRCP_COMPANY_ID(_dest, 0x00, 0x19, 0x58)

   /* The following is a utility MACRO that exists to assign a CTYPE in */
   /* a command header while formating a Command Message.  The first    */
   /* parameter could be any command message structure (UNIT            */
   /* INFO/SUBUNIT INFO/PASS THROURH/VENDOR DEPENDENT).  The second     */
   /* parameter is a constant that specifies a CTYPE.                   */
#define AVRCP_ASSIGN_COMMAND_HEADER_CTYPE(_x, _y) \
   ((((AVRCP_Command_Header_t *)(_x))->Byte0) |= (Byte_t)(((Byte_t)(_y) & (AVRCP_CTYPE_MASK)) << (AVRCP_CTYPE_SHIFT)))

   /* The following is a utility MACRO that exists to assign a SUBUNIT  */
   /* Type in a command header while formating a Command Message.  The  */
   /* first parameter could be any command message structure (UNIT      */
   /* INFO/SUBUNIT INFO/PASS THROUGH/VENDOR DEPENDENT).  The second     */
   /* parameter is a constant that specifies a SUBUNIT type.            */
#define AVRCP_ASSIGN_COMMAND_HEADER_SUBUNIT_TYPE(_x, _y) \
   ((((AVRCP_Command_Header_t *)(_x))->Byte1) |= (Byte_t)(((Byte_t)(_y) & (AVRCP_SUBUNIT_TYPE_MASK)) << (AVRCP_SUBUNIT_TYPE_SHIFT)))

   /* The following is a utility MACRO that exists to assign a SUBUNIT  */
   /* ID in a command header while formating a Command Message.  The    */
   /* first parameter could be any command message structure (UNIT      */
   /* INFO/SUBUNIT INFO/PASS THROUGH/VENDOR DEPENDENT).  The second     */
   /* parameter is a constant that specifies a SUBUNIT ID.              */
#define AVRCP_ASSIGN_COMMAND_HEADER_SUBUNIT_ID(_x, _y) \
   ((((AVRCP_Command_Header_t *)(_x))->Byte1) |= (Byte_t)(((Byte_t)(_y) & (AVRCP_SUBUNIT_ID_MASK)) << (AVRCP_SUBUNIT_ID_SHIFT)))

   /* The following is a utility MACRO that exists to assign a Opcode in*/
   /* a command header while formating a Command Message.  The first    */
   /* parameter could be any command message structure (UNIT            */
   /* INFO/SUBUNIT INFO/PASS THROUGH/VENDOR DEPENDENT).  The second     */
   /* parameter is a constant that specifies an Opcode.                 */
#define AVRCP_ASSIGN_COMMAND_HEADER_OPCODE(_x, _y) \
   ((((AVRCP_Command_Header_t *)(_x))->Opcode) |= (Byte_t)(_y))

   /* The following is a utility MACRO to read the CTYPE value from the */
   /* first byte of a command message.  The input parameter can be any  */
   /* command message (UNIT INFO/SUBUNIT INFO/PASS THROUGH/VENDOR       */
   /* DEPENDENT).                                                       */
#define AVRCP_READ_COMMAND_HEADER_CTYPE(_x) \
   (((((AVRCP_Command_Header_t *)(_x))->Byte0) >> (AVRCP_CTYPE_SHIFT)) & (AVRCP_CTYPE_MASK))

   /* The following is a utility MACRO to read the SUBUNIT Type value   */
   /* from the second byte of a command message.  The input parameter   */
   /* can be any command message (UNIT INFO/SUBUNIT INFO/PASS           */
   /* THROUGH/VENDOR DEPENDENT).                                        */
#define AVRCP_READ_COMMAND_HEADER_SUBUNIT_TYPE(_x) \
   (((((AVRCP_Command_Header_t *)(_x))->Byte1) >> (AVRCP_SUBUNIT_TYPE_SHIFT)) & (AVRCP_SUBUNIT_TYPE_MASK))

   /* The following is a utility MACRO to read the SUBUNIT ID value from*/
   /* the second byte of a command message.  The input parameter can be */
   /* any command message (UNIT INFO/SUBUNIT INFO/PASS THROUGH/VENDOR   */
   /* DEPENDENT).                                                       */
#define AVRCP_READ_COMMAND_HEADER_SUBUNIT_ID(_x) \
   (((((AVRCP_Command_Header_t *)(_x))->Byte1) >> (AVRCP_SUBUNIT_ID_SHIFT)) & (AVRCP_SUBUNIT_ID_MASK))

   /* The following is a utility MACRO to read the Opcode value from the*/
   /* third byte of a command message.  The input parameter can be any  */
   /* command message (UNIT INFO/SUBUNIT INFO/PASS THROUGH/VENDOR       */
   /* DEPENDENT).                                                       */
#define AVRCP_READ_COMMAND_HEADER_OPCODE(_x) \
   (((AVRCP_Command_Header_t *)(_x))->Opcode)

   /* The following is a utility MACRO that exists to assign a RESPONSE */
   /* code in a response header while formating a Response Message.  The*/
   /* first parameter could be any response message structure(UNIT      */
   /* INFO/SUBUNIT INFO/PASS THROUGH/VENDOR DEPENDENT).  The second     */
   /* parameter is a constant that specifies a RESPONSE Code.           */
#define AVRCP_ASSIGN_RESPONSE_HEADER_RESPONSE_TYPE(_x, _y) \
   ((((AVRCP_Response_Header_t *)(_x))->Byte0) |= (Byte_t)(((Byte_t)(_y) & (AVRCP_RESPONSE_MASK)) << (AVRCP_RESPONSE_SHIFT)))

   /* The following is a utility MACRO that exists to assign a SUBUNIT  */
   /* Type in a response header while formating a Response Message.  The*/
   /* first parameter could be any response message structure (UNIT     */
   /* INFO/SUBUNIT INFO/PASS THROUGH/VENDOR DEPENDENT).  The second     */
   /* parameter is a constant that specifies a SUBUNIT type.            */
#define AVRCP_ASSIGN_RESPONSE_HEADER_SUBUNIT_TYPE(_x, _y) \
   ((((AVRCP_Response_Header_t *)(_x))->Byte1) |= (Byte_t)(((Byte_t)(_y) & (AVRCP_SUBUNIT_TYPE_MASK)) << (AVRCP_SUBUNIT_TYPE_SHIFT)))

   /* The following is a utility MACRO that exists to assign a SUBUNIT  */
   /* ID in a response header while formating a Response Message.  The  */
   /* first parameter could be any response message structure (UNIT     */
   /* INFO/SUBUNIT INFO/PASS THROUGH/VENDOR DEPENDENT).  The second     */
   /* parameter is a constant that specifies a SUBUNIT ID.              */
#define AVRCP_ASSIGN_RESPONSE_HEADER_SUBUNIT_ID(_x, _y) \
   ((((AVRCP_Response_Header_t *)(_x))->Byte1) |= (Byte_t)(((Byte_t)(_y) & (AVRCP_SUBUNIT_ID_MASK)) << (AVRCP_SUBUNIT_ID_SHIFT)))

   /* The following is a utility MACRO that exists to assign a Opcode in*/
   /* a response header while formating a Response Message.  The first  */
   /* parameter could be any Response message structure (UNIT           */
   /* INFO/SUBUNIT INFO/PASS THROUGH/VENDOR DEPENDENT).  The second     */
   /* parameter is a constant that specifies an Opcode.                 */
#define AVRCP_ASSIGN_RESPONSE_HEADER_OPCODE(_x, _y) \
   ((((AVRCP_Response_Header_t *)(_x))->Opcode) |= (Byte_t)(_y))

   /* The following is a utility MACRO to read the RESPONSE type value  */
   /* from the first byte of a response message.  The input parameter   */
   /* can be any response message (UNIT INFO/SUBUNIT INFO/PASS          */
   /* THROUGH/VENDOR DEPENDENT).                                        */
#define AVRCP_READ_RESPONSE_HEADER_RESPONSE_TYPE(_x) \
   (((((AVRCP_Response_Header_t *)(_x))->Byte0) >> (AVRCP_RESPONSE_SHIFT)) & (AVRCP_RESPONSE_MASK))

   /* The following is a utility MACRO to read the SUBUNIT Type value   */
   /* from the second byte of a response message.  The input parameter  */
   /* can be any response message (UNIT INFO/SUBUNIT INFO/PASS          */
   /* THROUGH/VENDOR DEPENDENT).                                        */
#define AVRCP_READ_RESPONSE_HEADER_SUBUNIT_TYPE(_x) \
   (((((AVRCP_Response_Header_t *)(_x))->Byte1) >> (AVRCP_SUBUNIT_TYPE_SHIFT)) & (AVRCP_SUBUNIT_TYPE_MASK))

   /* The following is a utility MACRO to read the SUBUNIT ID value from*/
   /* the second byte of a response message.  The input parameter can be*/
   /* any response message (UNIT INFO/SUBUNIT INFO/PASS THROUGH/VENDOR  */
   /* DEPENDENT).                                                       */
#define AVRCP_READ_RESPONSE_HEADER_SUBUNIT_ID(_x) \
   (((((AVRCP_Response_Header_t *)(_x))->Byte1) >> (AVRCP_SUBUNIT_ID_SHIFT)) & (AVRCP_SUBUNIT_ID_MASK))

   /* The following is a utility MACRO to read the Opcode value from the*/
   /* third byte of a response message.  The input parameter can be any */
   /* response message (UNIT INFO/SUBUNIT INFO/PASS THROUGH/VENDOR      */
   /* DEPENDENT).                                                       */
#define AVRCP_READ_RESPONSE_HEADER_OPCODE(_x) \
   (((AVRCP_Response_Header_t *)(_x))->Opcode)

   /* The following structure defines a UNIT INFO Command message.  This*/
   /* command message has a command header followed by 5 single byte    */
   /* operands.  For a command, all of these 5 operands have a value of */
   /* 0xFF.                                                             */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Unit_Info_Command_t
{
   AVRCP_Command_Header_t AVRCP_Command_Header;
   Byte_t                 Operand0;  /* 0xFF as Padding.                */
   Byte_t                 Operand1;  /* 0xFF as Padding.                */
   Byte_t                 Operand2;  /* 0xFF as Padding.                */
   Byte_t                 Operand3;  /* 0xFF as Padding.                */
   Byte_t                 Operand4;  /* 0xFF as Padding.                */
} __PACKED_STRUCT_END__ AVRCP_Unit_Info_Command_t;

#define AVRCP_UNIT_INFO_COMMAND_SIZE                                 (sizeof(AVRCP_Unit_Info_Command_t))

   /* This is a utility MACRO that allows the user to fill all the 5    */
   /* "padding" operands to be filled with 0xFF in a UNIT INFO Command  */
   /* structure specified by the input parameter.  This is used before  */
   /* sending a command to a peer.                                      */
   /* * NOTE * No MACRO is provided to read these bytes from a command  */
   /*          message as they are to be ignored when a command is      */
   /*          received.                                                */
#define AVRCP_ASSIGN_UNIT_INFO_COMMAND_OPERAND_PADDING(_x) \
{                                                          \
   ((AVRCP_Unit_Info_Command_t *)(_x))->Operand0 = 0xFF;   \
   ((AVRCP_Unit_Info_Command_t *)(_x))->Operand1 = 0xFF;   \
   ((AVRCP_Unit_Info_Command_t *)(_x))->Operand2 = 0xFF;   \
   ((AVRCP_Unit_Info_Command_t *)(_x))->Operand3 = 0xFF;   \
   ((AVRCP_Unit_Info_Command_t *)(_x))->Operand4 = 0xFF;   \
}

   /* The following structure defines a UNIT INFO Response message.     */
   /* This response message has a message header followed by two single */
   /* byte operands, the first one has a value of 0x07 and the second   */
   /* one has the SUBUNIT type and SUBUNIT ID.  The last operand is a   */
   /* three byte Company Identifier.                                    */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Unit_Info_Response_t
{
   AVRCP_Response_Header_t AVRCP_Response_Header;
   Byte_t                  Operand0;   /* Value is always 0x07.         */
   Byte_t                  Operand1;   /* UNIT INFO and UNIT ID.        */
   AVRCP_Company_ID_t      OperandCompanyID; /* Company ID.             */
} __PACKED_STRUCT_END__ AVRCP_Unit_Info_Response_t;

#define AVRCP_UNIT_INFO_RESPONSE_SIZE                                (sizeof(AVRCP_Unit_Info_Response_t))

   /* The following is a utility MACRO to fill in the 0x07 value in the */
   /* first operand of a UNIT INFO Response structure specified by the  */
   /* only input parameter.                                             */
#define AVRCP_ASSIGN_UNIT_INFO_RESPONSE_OPERAND_0(_x) \
   (((AVRCP_Unit_Info_Response_t *)(_x))->Operand0 = 0x07)

   /* The following is a utility MACRO to fill the UNIT Type in the     */
   /* second operand of a Unit Info Response structure specified by the */
   /* only input parameter.                                             */
#define AVRCP_ASSIGN_UNIT_INFO_RESPONSE_UNIT_TYPE(_x, _y) \
   ((((AVRCP_Unit_Info_Response_t *)(_x))->Operand1) |= (Byte_t)(((Byte_t)(_y) & (AVRCP_SUBUNIT_TYPE_MASK)) << (AVRCP_SUBUNIT_TYPE_SHIFT)))

   /* The following is a utility MACRO to fill the UNIT value in the    */
   /* second operand of a Unit Info Response structure specified by the */
   /* only input parameter.  The specification says that the value of   */
   /* this "UNIT" field is not defined.  So it is up to the user to use */
   /* this MACRO.  It could be used when the specs do define the meaning*/
   /* of this field.                                                    */
#define AVRCP_ASSIGN_UNIT_INFO_RESPONSE_UNIT(_x, _y) \
   ((((AVRCP_Unit_Info_Response_t *)(_x))->Operand1) |= (Byte_t)(((Byte_t)(_y) & (AVRCP_SUBUNIT_ID_MASK)) << (AVRCP_SUBUNIT_ID_SHIFT)))

   /* The following is a utility MACRO that assigns a pre-formated      */
   /* Company ID, specified by the second parameter in a UNIT INFO      */
   /* response structure specified by the first parameter.              */
#define AVRCP_ASSIGN_UNIT_INFO_RESPONSE_COMPANY_ID(_x, _y) \
   ((((AVRCP_Unit_Info_Response_t *)(_x))->OperandCompanyID) = (_y))

   /* The following is a utility MACRO that reads the first operand of a*/
   /* UNIT INFO response message specified by the only input parameter. */
#define AVRCP_READ_UNIT_INFO_RESPONSE_OPERAND_0(_x) \
   (((AVRCP_Unit_Info_Response_t *)(_x))->Operand0)

   /* The following is a utility MACRO that reads the UNIT type from a  */
   /* UNIT INFO response message specified by the input parameter.      */
#define AVRCP_READ_UNIT_INFO_RESPONSE_UNIT_TYPE(_x) \
   (((((AVRCP_Unit_Info_Response_t *)(_x))->Operand1) >> (AVRCP_SUBUNIT_TYPE_SHIFT)) & (AVRCP_SUBUNIT_TYPE_MASK))

   /* The following is a utility MACRO that reads the UNIT from a UNIT  */
   /* INFO response messsage specified by the input parameter.          */
#define AVRCP_READ_UNIT_INFO_RESPONSE_UNIT(_x) \
   (((((AVRCP_Unit_Info_Response_t *)(_x))->Operand1) >> (AVRCP_SUBUNIT_ID_SHIFT)) & (AVRCP_SUBUNIT_ID_MASK))

   /* The following is a utility MACRO that reads the Company ID of type*/
   /* AVRCP_Company_ID_t from a UNIT INFO response message specified by */
   /* the input parameter.                                              */
#define AVRCP_READ_UNIT_INFO_RESPONSE_COMPANY_ID(_x) \
   (((AVRCP_Unit_Info_Response_t *)(_x))->OperandCompanyID)

   /* The following constants define the Mask and shift values to       */
   /* extract the Page number from a SUBUNIT INFO command or response   */
   /* frame.  These are part of the Operand0 Byte.                      */
#define AVRCP_PAGE_MASK                                              (0x07)
#define AVRCP_PAGE_SHIFT                                             (0x04)
#define AVRCP_EXTENSION_CODE_MASK                                    (0x07)
#define AVRCP_EXTENSION_CODE_SHIFT                                   (0x00)

   /* The following structure defines a SUBUNIT INFO Command message.   */
   /* It has a Command Header followed by 5 single byte operands.  Only */
   /* the first operand has any useful information.  It contains the    */
   /* page number queried and some extension code bits that are         */
   /* constant.  The rest 4 operands are padding and have value 0xFF.   */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Subunit_Info_Command_t
{
   AVRCP_Command_Header_t AVRCP_Command_Header;
   Byte_t                 Operand0;  /* Page and Extension Code Information.  */
   Byte_t                 Operand1;  /* 0xFF as Padding.                      */
   Byte_t                 Operand2;  /* 0xFF as Padding.                      */
   Byte_t                 Operand3;  /* 0xFF as Padding.                      */
   Byte_t                 Operand4;  /* 0xFF as Padding.                      */
} __PACKED_STRUCT_END__ AVRCP_Subunit_Info_Command_t;

#define AVRCP_SUBUNIT_INFO_COMMAND_SIZE                              (sizeof(AVRCP_Subunit_Info_Command_t))

   /* The following is a utility MACRO to assign PAGE value in a SUBUNIT*/
   /* INFO command.  The first parameter specifies the SUBUNIT command  */
   /* info structure and the second one is the page value to be         */
   /* assigned.                                                         */
#define AVRCP_ASSIGN_SUBUNIT_INFO_COMMAND_PAGE(_x, _y) \
   ((((AVRCP_Subunit_Info_Command_t *)(_x))->Operand0) |= (Byte_t)(((Byte_t)(_y) & (AVRCP_PAGE_MASK)) << (AVRCP_PAGE_SHIFT)))

   /* The following is a utility MACRO to assign extension code in a    */
   /* Subunit Info command structure specified by the input parameter.  */
#define AVRCP_ASSIGN_SUBUNIT_INFO_COMMAND_EXTN_CODE(_x) \
   ((((AVRCP_Subunit_Info_Command_t *)(_x))->Operand0) |= 0x07)

   /* The following is a utility MACRO to assign 0xFF in the four single*/
   /* byte operands used as padding in a SUBUNIT INFO command structure */
   /* specified by the input parameter.                                 */
#define AVRCP_ASSIGN_SUBUNIT_INFO_COMMAND_OPERAND_PADDING(_x) \
{                                                             \
   ((AVRCP_Subunit_Info_Command_t *)(_x))->Operand1 = 0xFF;   \
   ((AVRCP_Subunit_Info_Command_t *)(_x))->Operand2 = 0xFF;   \
   ((AVRCP_Subunit_Info_Command_t *)(_x))->Operand3 = 0xFF;   \
   ((AVRCP_Subunit_Info_Command_t *)(_x))->Operand4 = 0xFF;   \
}

   /* The following is a utility MACRO to read Page value from a SUBUNIT*/
   /* INFO command message specified by the input parameter.            */
#define AVRCP_READ_SUBUNIT_INFO_COMMAND_PAGE(_x) \
   (((((AVRCP_Subunit_Info_Command_t *)(_x))->Operand0) >> (AVRCP_PAGE_SHIFT)) & (AVRCP_PAGE_MASK))

   /* The following is a utility MACRO to read the extension code from a*/
   /* SUBUNIT INFO command message specified by the input parameter.  It*/
   /* is a constant value and can be ignored but this MACRO allows the  */
   /* application easy access to this value if it wants to read it.     */
#define AVRCP_READ_SUBUNIT_INFO_COMMAND_EXTN_CODE(_x) \
   (((((AVRCP_Subunit_Info_Command_t *)(_x))->Operand0) >> (AVRCP_EXTENSION_CODE_SHIFT)) & (AVRCP_EXTENSION_CODE_MASK))

   /* The following constant represents the maximum number of Pages that*/
   /* are supported by the SUBUNIT INFO Command.                        */
#define AVRCP_SUBUNIT_INFO_MAXIMUM_NUMBER_PAGES                      (8)

   /* The following structure defines a SUBUNIT INFO response message.  */
   /* It has a Response Header followed by 5 single byte operands.  The */
   /* first operand specifies the Page queried and the constant         */
   /* extension code.  There can be at most 4 SUBUNITs at this page     */
   /* number.  The rest 4 single byte operands specify the SUBUNIT type */
   /* and ID of one SUBUNIT each that is on this page.  If there are    */
   /* less than 4 SUBUNITs at this page, the remaining Operands containg*/
   /* the value 0xFF.  constant.                                        */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Subunit_Info_Response_t
{
   AVRCP_Response_Header_t AVRCP_Response_Header;
   Byte_t                  Operand0;  /* Page and Extension Code Information.                    */
   Byte_t                  Operand1;  /* Possible Subunit Type and ID, 0xFF if no subunits here. */
   Byte_t                  Operand2;  /* Possible Subunit Type and ID, 0xFF if no subunits here. */
   Byte_t                  Operand3;  /* Possible Subunit Type and ID, 0xFF if no subunits here. */
   Byte_t                  Operand4;  /* Possible Subunit Type and ID, 0xFF if no subunits here. */
} __PACKED_STRUCT_END__ AVRCP_Subunit_Info_Response_t;

#define AVRCP_SUBUNIT_INFO_RESPONSE_SIZE                             (sizeof(AVRCP_Subunit_Info_Response_t))

   /* The following constant defines the maximum possible numbr of      */
   /* SUBUNIT Table Entries that can be present in the SUBUNIT INFO     */
   /* Response.                                                         */
#define AVRCP_SUBUNIT_INFO_RESPONSE_MAXIMUM_SUBUNIT_TABLE_ENTRIES    0x04

   /* This is a utility MACRO that assigns the value of the Page queried*/
   /* in a SUBUNIT INFO response structure specified by the first       */
   /* parameter.  The second parameter is the actual value of the       */
   /* queried page.                                                     */
#define AVRCP_ASSIGN_SUBUNIT_INFO_RESPONSE_PAGE(_x, _y) \
   ((((AVRCP_Subunit_Info_Response_t *)(_x))->Operand0) |= (Byte_t)(((Byte_t)(_y) & (AVRCP_PAGE_MASK)) << (AVRCP_PAGE_SHIFT)))

   /* This is a utility MACRO that assigns the value of the Page queried*/
   /* in a SUBUNIT INFO response structure specified by the first       */
   /* parameter.  It is a constant value so there is no second          */
   /* parameter.                                                        */
#define AVRCP_ASSIGN_SUBUNIT_INFO_RESPONSE_EXTN_CODE(_x) \
   ((((AVRCP_Subunit_Info_Response_t *)(_x))->Operand0) |= 0x07)

   /* The following MACROs pad the operands that are unused in a SUBUNIT*/
   /* INFO response.  It is the application's responsibility to use the */
   /* appopriate MACROs depending on what SUBUNITs are on that page.    */
#define AVRCP_ASSIGN_SUBUNIT_INFO_RESPONSE_PAD_OPERAND_1(_x) \
   ((AVRCP_Subunit_Info_Response_t *)(_x))->Operand1 = 0xFF;

#define AVRCP_ASSIGN_SUBUNIT_INFO_RESPONSE_PAD_OPERAND_2(_x) \
   ((AVRCP_Subunit_Info_Response_t *)(_x))->Operand2 = 0xFF;

#define AVRCP_ASSIGN_SUBUNIT_INFO_RESPONSE_PAD_OPERAND_3(_x) \
   ((AVRCP_Subunit_Info_Response_t *)(_x))->Operand3 = 0xFF;

#define AVRCP_ASSIGN_SUBUNIT_INFO_RESPONSE_PAD_OPERAND_4(_x) \
   ((AVRCP_Subunit_Info_Response_t *)(_x))->Operand4 = 0xFF;

   /* This is a utility MACRO that assigns the SUBUNIT Type of the first*/
   /* SUBUNIT at the page specified in a command message.  It assigns   */
   /* the value in the approprate operand of a SUBUNIT INFO response    */
   /* structure specified by the first parameter.  The second parameter */
   /* is the actual SUBUNIT Type.                                       */
#define AVRCP_ASSIGN_SUBUNIT_INFO_RESPONSE_SUBUNIT_1_TYPE(_x,_y) \
   ((((AVRCP_Subunit_Info_Response_t *)(_x))->Operand1) |= (Byte_t)(((Byte_t)(_y) & (AVRCP_SUBUNIT_TYPE_MASK)) << (AVRCP_SUBUNIT_TYPE_SHIFT)))

   /* This is a utility MACRO that assigns the SUBUNIT ID of the first  */
   /* SUBUNIT at the page specified in a command message.  It assigns   */
   /* the value in the approprate operand of a SUBUNIT INFO response    */
   /* structure specified by the first parameter.  The second parameter */
   /* is the actual SUBUNIT ID.                                         */
#define AVRCP_ASSIGN_SUBUNIT_INFO_RESPONSE_SUBUNIT_1_ID(_x,_y) \
   ((((AVRCP_Subunit_Info_Response_t *)(_x))->Operand1) |= (Byte_t)(((Byte_t)(_y) & (AVRCP_SUBUNIT_ID_MASK)) << (AVRCP_SUBUNIT_ID_SHIFT)))

   /* This is a utility MACRO that assigns the SUBUNIT Type of the      */
   /* second SUBUNIT at the page specified in a command message.  It    */
   /* assigns the value in the approprate operand of a SUBUNIT INFO     */
   /* response structure specified by the first parameter.  The second  */
   /* parameter is the actual SUBUNIT Type.                             */
#define AVRCP_ASSIGN_SUBUNIT_INFO_RESPONSE_SUBUNIT_2_TYPE(_x,_y) \
   ((((AVRCP_Subunit_Info_Response_t *)(_x))->Operand2) |= (Byte_t)(((Byte_t)(_y) & (AVRCP_SUBUNIT_TYPE_MASK)) << (AVRCP_SUBUNIT_TYPE_SHIFT)))

   /* This is a utility MACRO that assigns the SUBUNIT ID of the second */
   /* SUBUNIT at the page specified in a command message.  It assigns   */
   /* the value in the approprate operand of a SUBUNIT INFO response    */
   /* structure specified by the first parameter.  The second parameter */
   /* is the actual SUBUNIT ID.                                         */
#define AVRCP_ASSIGN_SUBUNIT_INFO_RESPONSE_SUBUNIT_2_ID(_x,_y) \
   ((((AVRCP_Subunit_Info_Response_t *)(_x))->Operand2) |= (Byte_t)(((Byte_t)(_y) & (AVRCP_SUBUNIT_ID_MASK)) << (AVRCP_SUBUNIT_ID_SHIFT)))

   /* This is a utility MACRO that assigns the SUBUNIT Type of the third*/
   /* SUBUNIT at the page specified in a command message.  It assigns   */
   /* the value in the approprate operand of a SUBUNIT INFO response    */
   /* structure specified by the first parameter.  The second parameter */
   /* is the actual SUBUNIT Type.                                       */
#define AVRCP_ASSIGN_SUBUNIT_INFO_RESPONSE_SUBUNIT_3_TYPE(_x,_y) \
   ((((AVRCP_Subunit_Info_Response_t *)(_x))->Operand3) |= (Byte_t)(((Byte_t)(_y) & (AVRCP_SUBUNIT_TYPE_MASK)) << (AVRCP_SUBUNIT_TYPE_SHIFT)))

   /* This is a utility MACRO that assigns the SUBUNIT ID of the third  */
   /* SUBUNIT at the page specified in a command message.  It assigns   */
   /* the value in the approprate operand of a SUBUNIT INFO response    */
   /* structure specified by the first parameter.  The second parameter */
   /* is the actual SUBUNIT ID.                                         */
#define AVRCP_ASSIGN_SUBUNIT_INFO_RESPONSE_SUBUNIT_3_ID(_x,_y) \
   ((((AVRCP_Subunit_Info_Response_t *)(_x))->Operand3) |= (Byte_t)(((Byte_t)(_y) & (AVRCP_SUBUNIT_ID_MASK)) << (AVRCP_SUBUNIT_ID_SHIFT)))

   /* This is a utility MACRO that assigns the SUBUNIT Type of the      */
   /* fourth SUBUNIT at the page specified in a command message.  It    */
   /* assigns the value in the approprate operand of a SUBUNIT INFO     */
   /* response structure specified by the first parameter.  The second  */
   /* parameter is the actual SUBUNIT Type.                             */
#define AVRCP_ASSIGN_SUBUNIT_INFO_RESPONSE_SUBUNIT_4_TYPE(_x,_y) \
   ((((AVRCP_Subunit_Info_Response_t *)(_x))->Operand4) |= (Byte_t)(((Byte_t)(_y) & (AVRCP_SUBUNIT_TYPE_MASK)) << (AVRCP_SUBUNIT_TYPE_SHIFT)))

   /* This is a utility MACRO that assigns the SUBUNIT ID of the fourth */
   /* SUBUNIT at the page specified in a command message.  It assigns   */
   /* the value in the approprate operand of a SUBUNIT INFO response    */
   /* structure specified by the first parameter.  The second parameter */
   /* is the actual SUBUNIT ID.                                         */
#define AVRCP_ASSIGN_SUBUNIT_INFO_RESPONSE_SUBUNIT_4_ID(_x,_y) \
   ((((AVRCP_Subunit_Info_Response_t *)(_x))->Operand4) |= (Byte_t)(((Byte_t)(_y) & (AVRCP_SUBUNIT_ID_MASK)) << (AVRCP_SUBUNIT_ID_SHIFT)))

   /* This is a utilty MACRO to read Page value from a SUBUNIT INFO     */
   /* response structure specified by the input parameter.              */
#define AVRCP_READ_SUBUNIT_INFO_RESPONSE_PAGE(_x) \
   (((((AVRCP_Subunit_Info_Response_t *)(_x))->Operand0) >> (AVRCP_PAGE_SHIFT)) & (AVRCP_PAGE_MASK))

    /* This is a utilty MACRO to read Extension code from a SUBUNIT INFO*/
    /* response structure specified by the input parameter.             */
#define AVRCP_READ_SUBUNIT_INFO_RESPONSE_EXTN_CODE(_x) \
   (((((AVRCP_Subunit_Info_Response_t *)(_x))->Operand0) >> (AVRCP_EXTENSION_CODE_SHIFT)) & (AVRCP_EXTENSION_CODE_MASK))

   /* This is a utility MACRO that will be TRUE only if the operand 1   */
   /* contains something other than 0xFF.  We are assuming that the peer*/
   /* profile reported a valid SUBUNIT in this operand.                 */
#define AVRCP_READ_SUBUNIT_INFO_RESPONSE_SUBUNIT_1_VALID(_x) \
   ((((AVRCP_Subunit_Info_Response_t *)(_x))->Operand1 == 0xFF)?FALSE:TRUE)

   /* This is a utility MACRO to read SUBUNIT type of the first SUBUNIT */
   /* from a SUBUNIT info response structure specified by the input     */
   /* parameter.  This should be used if the application thinks that it */
   /* has a valid first SUBUNIT.                                        */
#define AVRCP_READ_SUBUNIT_INFO_RESPONSE_SUBUNIT_1_TYPE(_x) \
   (((((AVRCP_Subunit_Info_Response_t *)(_x))->Operand1) >> (AVRCP_SUBUNIT_TYPE_SHIFT)) & (AVRCP_SUBUNIT_TYPE_MASK))

   /* This is a utility MACRO to read SUBUNIT ID of the first SUBUNIT   */
   /* from a SUBUNIT INFO response structure specified by the input     */
   /* parameter.  This should be used if the application thinks that it */
   /* has a valid first SUBUNIT.                                        */
#define AVRCP_READ_SUBUNIT_INFO_RESPONSE_SUBUNIT_1_ID(_x) \
   (((((AVRCP_Subunit_Info_Response_t *)(_x))->Operand1) >> (AVRCP_SUBUNIT_ID_SHIFT)) & (AVRCP_SUBUNIT_ID_MASK))

   /* This is a utility MACRO that will be TRUE only if the operand 4   */
   /* contains something other than 0xFF.  We are assuming that the peer*/
   /* profile reported a valid SUBUNIT in this operand.                 */
#define AVRCP_READ_SUBUNIT_INFO_RESPONSE_SUBUNIT_2_VALID(_x) \
   ((((AVRCP_Subunit_Info_Response_t *)(_x))->Operand1 == 0xFF)?FALSE:TRUE)

   /* This is a utility MACRO to read SUBUNIT type of the second SUBUNIT*/
   /* from a SUBUNIT INFO response structure specified by the input     */
   /* parameter.  This should be used if the application thinks that it */
   /* has a valid second SUBUNIT.                                       */
#define AVRCP_READ_SUBUNIT_INFO_RESPONSE_SUBUNIT_2_TYPE(_x) \
   (((((AVRCP_Subunit_Info_Response_t *)(_x))->Operand2) >> (AVRCP_SUBUNIT_TYPE_SHIFT)) & (AVRCP_SUBUNIT_TYPE_MASK))

   /* This is a utility MACRO to read SUBUNIT ID of the second SUBUNIT  */
   /* from a SUBUNIT INFO response structure specified by the input     */
   /* parameter.  This should be used if the application thinks that it */
   /* has a valid second SUBUNIT.                                       */
#define AVRCP_READ_SUBUNIT_INFO_RESPONSE_SUBUNIT_2_ID(_x) \
   (((((AVRCP_Subunit_Info_Response_t *)(_x))->Operand2) >> (AVRCP_SUBUNIT_ID_SHIFT)) & (AVRCP_SUBUNIT_ID_MASK))

   /* This is a utility MACRO that will be TRUE only if the operand 3   */
   /* contains something other than 0xFF.  We are assuming that the peer*/
   /* profile reported a valid SUBUNIT in this operand.                 */
#define AVRCP_READ_SUBUNIT_INFO_RESPONSE_SUBUNIT_3_VALID(_x) \
   ((((AVRCP_Subunit_Info_Response_t *)(_x))->Operand1 == 0xFF)?FALSE:TRUE)

   /* This is a utility MACRO to read SUBUNIT type of the third SUBUNIT */
   /* from a SUBUNIT INFO response structure specified by the input     */
   /* parameter.  This should be used if the application thinks that it */
   /* has a valid third SUBUNIT.                                        */
#define AVRCP_READ_SUBUNIT_INFO_RESPONSE_SUBUNIT_3_TYPE(_x) \
   (((((AVRCP_Subunit_Info_Response_t *)(_x))->Operand3) >> (AVRCP_SUBUNIT_TYPE_SHIFT)) & (AVRCP_SUBUNIT_TYPE_MASK))

   /* This is a utility MACRO to read SUBUNIT ID of the third SUBUNIT   */
   /* from a SUBUNIT INFO response structure specified by the input     */
   /* parameter.  This should be used if the application thinks that it */
   /* has a valid third SUBUNIT.                                        */
#define AVRCP_READ_SUBUNIT_INFO_RESPONSE_SUBUNIT_3_ID(_x) \
   (((((AVRCP_Subunit_Info_Response_t *)(_x))->Operand3) >> (AVRCP_SUBUNIT_ID_SHIFT)) & (AVRCP_SUBUNIT_ID_MASK))

   /* This is a utility MACRO that will be TRUE only if the operand 4   */
   /* contains something other than 0xFF.  We are assuming that the peer*/
   /* profile reported a valid SUBUNIT in this operand.                 */
#define AVRCP_READ_SUBUNIT_INFO_RESPONSE_SUBUNIT_4_VALID(_x) \
   ((((AVRCP_Subunit_Info_Response_t *)(_x))->Operand1 == 0xFF)?FALSE:TRUE)

   /* This is a utility MACRO to read SUBUNIT type of the fourth SUBUNIT*/
   /* from a SUBUNIT INFO response structure specified by the input     */
   /* parameter.  This should be used if the application thinks that it */
   /* has a valid fourth SUBUNIT.                                       */
#define AVRCP_READ_SUBUNIT_INFO_RESPONSE_SUBUNIT_4_TYPE(_x) \
   (((((AVRCP_Subunit_Info_Response_t *)(_x))->Operand4) >> (AVRCP_SUBUNIT_TYPE_SHIFT)) & (AVRCP_SUBUNIT_TYPE_MASK))

   /* This is a utility MACRO to read SUBUNIT ID of the fourth SUBUNIT  */
   /* from a SUBUNIT INFO response structure specified by the input     */
   /* parameter.  This should be used if the application thinks that it */
   /* has a valid fourth SUBUNIT.                                       */
#define AVRCP_READ_SUBUNIT_INFO_RESPONSE_SUBUNIT_4_ID(_x) \
   (((((AVRCP_Subunit_Info_Response_t *)(_x))->Operand4) >> (AVRCP_SUBUNIT_ID_SHIFT)) & (AVRCP_SUBUNIT_ID_MASK))

   /* The following structure defines a Pass through Command Header.  It*/
   /* contains the header information that will be present for EVERY    */
   /* Pass-through message.                                             */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Pass_Through_Command_Header_t
{
   AVRCP_Command_Header_t  AVRCP_Command_Header;
   Byte_t                  OperationID;
   Byte_t                  OperationDataLength;
} __PACKED_STRUCT_END__ AVRCP_Pass_Through_Command_Header_t;

#define AVRCP_CALCULATE_PASS_THROUGH_COMMAND_HEADER_SIZE             (sizeof(AVRCP_Pass_Through_Command_Header_t))

   /* The following structure defines a Pass through Response Header.   */
   /* It contains the header information that will be present for EVERY */
   /* Pass-through message.                                             */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Pass_Through_Response_Header_t
{
   AVRCP_Command_Header_t  AVRCP_Command_Header;
   Byte_t                  OperationID;
   Byte_t                  OperationDataLength;
} __PACKED_STRUCT_END__ AVRCP_Pass_Through_Response_Header_t;

#define AVRCP_CALCULATE_PASS_THROUGH_RESPONSE_HEADER_SIZE            (sizeof(AVRCP_Pass_Through_Response_Header_t))

   /* The following structure defines a Pass through Command message.   */
   /* It contains a command message header followed by a single Byte    */
   /* operation ID (defined as constants below) and the length of the   */
   /* operation field data.  If the length is non-zero, it has a member */
   /* "data" that can be used to access the operation data field.       */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Pass_Through_Command_t
{
   AVRCP_Command_Header_t  AVRCP_Command_Header;
   Byte_t                  OperationID;
   Byte_t                  OperationDataLength;
   Byte_t                  Data[1];
} __PACKED_STRUCT_END__ AVRCP_Pass_Through_Command_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an outgoing Pass through command     */
   /* frame.  The input parameter is a constant (defined below) that    */
   /* specifies the length of the data field for the Operation ID used. */
#define AVRCP_CALCULATE_PASS_THROUGH_COMMAND_SIZE(_x)                (sizeof(AVRCP_Pass_Through_Command_t) - BYTE_SIZE + (Byte_t)(_x))

   /* The following structure defines a Pass Through Response message.  */
   /* It contains a response message header followed by a single Byte   */
   /* operation ID (defined as constants below) and the length of the   */
   /* operation field data.  If the length is non-zero, it has a member */
   /* "data" that can be used to access the operation data field.       */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Pass_Through_Response_t
{
   AVRCP_Response_Header_t AVRCP_Response_Header;
   Byte_t                  OperationID;
   Byte_t                  OperationDataLength;
   Byte_t                  Data[1];
} __PACKED_STRUCT_END__ AVRCP_Pass_Through_Response_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an outgoing Pass through response    */
   /* frame.  The input parameter is a constant (defined below) that    */
   /* specifies the length of the data field for the Operation ID used. */
#define AVRCP_CALCULATE_PASS_THROUGH_RESPONSE_SIZE(_x)               (sizeof(AVRCP_Pass_Through_Response_t) - BYTE_SIZE + (Byte_t)(_x))

   /* The following is a utility MACRO that assigns an Operation ID,    */
   /* specified by the second parameter at the appropriate position in  */
   /* the pass through command structure specified by the first         */
   /* parameter.                                                        */
#define AVRCP_ASSIGN_PASS_THROUGH_COMMAND_OPERATION_ID(_x, _y) \
   (((AVRCP_Pass_Through_Command_t *)(_x))->OperationID = (Byte_t)(((Byte_t)(_y)) & AVRCP_PASS_THROUGH_ID_OPERATION_ID_MASK))

   /* The following is a utility MACRO that assigns the specified       */
   /* operation ID State Flag (second parameter) to a pass through      */
   /* command structure specified by the first parameter.               */
#define AVRCP_ASSIGN_PASS_THROUGH_COMMAND_OPERATION_ID_STATE_FLAG(_x, _y)                                                        \
{                                                                                                                                \
   ((AVRCP_Pass_Through_Command_t *)(_x))->OperationID &= (Byte_t)(AVRCP_PASS_THROUGH_ID_OPERATION_ID_MASK);                     \
   ((AVRCP_Pass_Through_Command_t *)(_x))->OperationID |= (Byte_t)((((Boolean_t)(_y))?AVRCP_PASS_THROUGH_ID_STATE_FLAG_MASK:0)); \
}

   /* The following is a utility MACRO that assigns an Operation data   */
   /* length, specified by the second parameter at the appropriate      */
   /* position in the pass through command structure specified by the   */
   /* first parameter.                                                  */
#define AVRCP_ASSIGN_PASS_THROUGH_COMMAND_OPERATION_DATA_LENGTH(_x, _y) \
   (((AVRCP_Pass_Through_Command_t *)(_x))->OperationDataLength = (Byte_t)(_y))

#define AVRCP_ASSIGN_PASS_THROUGH_COMMAND_OPERATION_DATA_LEN(_x, _y) \
   (((AVRCP_Pass_Through_Command_t *)(_x))->OperationDataLength = (Byte_t)(_y))

   /* The following is a utility MACRO that reads an operation ID from a*/
   /* pass through command structure specified by the first parameter.  */
#define AVRCP_READ_PASS_THROUGH_COMMAND_OPERATION_ID(_x) \
   (((AVRCP_Pass_Through_Command_t *)(_x))->OperationID) & ((Byte_t)AVRCP_PASS_THROUGH_ID_OPERATION_ID_MASK)

   /* The following is a utility MACRO that reads an operation ID State */
   /* Flag from a pass through command structure specified by the first */
   /* parameter.                                                        */
#define AVRCP_READ_PASS_THROUGH_COMMAND_OPERATION_ID_STATE_FLAG(_x) \
   ((((AVRCP_Pass_Through_Command_t *)(_x))->OperationID) & ((Byte_t)AVRCP_PASS_THROUGH_ID_STATE_FLAG_MASK))

   /* The following is a utility MACRO that reads an operation data     */
   /* length from a pass through command structure specified by the     */
   /* first parameter.                                                  */
#define AVRCP_READ_PASS_THROUGH_COMMAND_OPERATION_DATA_LENGTH(_x) \
   (((AVRCP_Pass_Through_Command_t *)(_x))->OperationDataLength)

#define AVRCP_READ_PASS_THROUGH_COMMAND_OPERATION_DATA_LEN(_x) \
   (((AVRCP_Pass_Through_Command_t *)(_x))->OperationDataLength)

   /* The following is a utility MACRO that assigns an Operation ID,    */
   /* specified by the second parameter at the appropriate position in  */
   /* the pass through response structure specified by the first        */
   /* parameter.                                                        */
#define AVRCP_ASSIGN_PASS_THROUGH_RESPONSE_OPERATION_ID(_x, _y) \
   (((AVRCP_Pass_Through_Response_t *)(_x))->OperationID = (Byte_t)((Byte_t)(_y) & AVRCP_PASS_THROUGH_ID_OPERATION_ID_MASK))

   /* The following is a utility MACRO that assigns the specified       */
   /* operation ID State Flag (second parameter) to a pass through      */
   /* command structure specified by the first parameter.               */
#define AVRCP_ASSIGN_PASS_THROUGH_RESPONSE_OPERATION_ID_STATE_FLAG(_x, _y)                                                        \
{                                                                                                                                 \
   ((AVRCP_Pass_Through_Response_t *)(_x))->OperationID &= (Byte_t)(AVRCP_PASS_THROUGH_ID_OPERATION_ID_MASK);                     \
   ((AVRCP_Pass_Through_Response_t *)(_x))->OperationID |= (Byte_t)((((Boolean_t)(_y))?AVRCP_PASS_THROUGH_ID_STATE_FLAG_MASK:0)); \
}

   /* The following is a utility MACRO that reads an operation ID State */
   /* Flag from a pass through response structure specified by the first*/
   /* parameter.                                                        */
#define AVRCP_READ_PASS_THROUGH_RESPONSE_OPERATION_ID_STATE_FLAG(_x) \
   ((((AVRCP_Pass_Through_Response_t *)(_x))->OperationID) & ((Byte_t)AVRCP_PASS_THROUGH_ID_STATE_FLAG_MASK))

   /* The following is a utility MACRO that assigns an Operation data   */
   /* length, specified by the second parameter at the appropriate      */
   /* position in the pass through response structure specified by the  */
   /* first parameter.                                                  */
#define AVRCP_ASSIGN_PASS_THROUGH_RESPONSE_OPERATION_DATA_LENGTH(_x, _y) \
   (((AVRCP_Pass_Through_Response_t *)(_x))->OperationDataLength = (Byte_t)(_y))

#define AVRCP_ASSIGN_PASS_THROUGH_RESPONSE_OPERATION_DATA_LEN        AVRCP_ASSIGN_PASS_THROUGH_RESPONSE_OPERATION_DATA_LENGTH

   /* The following is a utility MACRO that reads an operation ID from a*/
   /* pass through response structure specified by the first parameter. */
#define AVRCP_READ_PASS_THROUGH_RESPONSE_OPERATION_ID(_x) \
   (((AVRCP_Pass_Through_Response_t *)(_x))->OperationID) & ((Byte_t)AVRCP_PASS_THROUGH_ID_OPERATION_ID_MASK)

   /* The following is a utility MACRO that reads an operation data     */
   /* length from a pass through response structure specified by the    */
   /* first parameter.                                                  */
#define AVRCP_READ_PASS_THROUGH_RESPONSE_OPERATION_DATA_LENGTH(_x) \
   (((AVRCP_Pass_Through_Response_t *)(_x))->OperationDataLength)

#define AVRCP_READ_PASS_THROUGH_RESPONSE_OPERATION_DATA_LEN          AVRCP_READ_PASS_THROUGH_RESPONSE_OPERATION_DATA_LENGTH

   /* The following constant represents the maximum allowable data      */
   /* length that can be sent (Operational Payload) in a Pass Through   */
   /* Command/Response.                                                 */
#define AVRCP_MAXIMUM_PASS_THROUGH_DATA_SIZE                         (0xFF)

   /* The following constant represents the Bit Mask that is applied to */
   /* the OperationID member of a Pass through Command/Response frame to*/
   /* yield the actual Operation ID.                                    */
#define AVRCP_PASS_THROUGH_ID_OPERATION_ID_MASK                      (0x7F)

   /* The following constants represent the Bit Masks (and values) that */
   /* are applied to an AVRCP Pass Through Operation ID to determine if */
   /* the button has been pressed (down) or released (up).              */
   /* * NOTE * These values are OR'ed with the AVRCP Pass Through       */
   /*          Operation ID.                                            */
#define AVRCP_PASS_THROUGH_ID_STATE_FLAG_MASK                        (0x80)

#define AVRCP_PASS_THROUGH_ID_STATE_FLAG_BUTTON_DOWN_MASK            (0x00)
#define AVRCP_PASS_THROUGH_ID_STATE_FLAG_BUTTON_UP_MASK              (0x80)

   /* The following constants define the various Operation IDs.         */
   /* * NOTE * A value of 0xFF is invalid and is a placeholder until the*/
   /*          correct value is known for that operation ID.            */
   /* * NOTE * These values are defined in the IEEE 1394 "AV/C Digital  */
   /*          Interface Command Set" Specification.                    */
#define AVRCP_PASS_THROUGH_ID_SELECT                                 (0x00)
#define AVRCP_PASS_THROUGH_ID_UP                                     (0x01)
#define AVRCP_PASS_THROUGH_ID_DOWN                                   (0x02)
#define AVRCP_PASS_THROUGH_ID_LEFT                                   (0x03)
#define AVRCP_PASS_THROUGH_ID_RIGHT                                  (0x04)
#define AVRCP_PASS_THROUGH_ID_RIGHT_UP                               (0x05)
#define AVRCP_PASS_THROUGH_ID_RIGHT_DOWN                             (0x06)
#define AVRCP_PASS_THROUGH_ID_LEFT_UP                                (0x07)
#define AVRCP_PASS_THROUGH_ID_LEFT_DOWN                              (0x08)
#define AVRCP_PASS_THROUGH_ID_ROOT_MENU                              (0x09)
#define AVRCP_PASS_THROUGH_ID_SETUP_MENU                             (0x0A)
#define AVRCP_PASS_THROUGH_ID_CONTENTS_MENU                          (0x0B)
#define AVRCP_PASS_THROUGH_ID_FAVORITE_MENU                          (0x0C)
#define AVRCP_PASS_THROUGH_ID_EXIT                                   (0x0D)
#define AVRCP_PASS_THROUGH_ID_0                                      (0x20)
#define AVRCP_PASS_THROUGH_ID_1                                      (0x21)
#define AVRCP_PASS_THROUGH_ID_2                                      (0x22)
#define AVRCP_PASS_THROUGH_ID_3                                      (0x23)
#define AVRCP_PASS_THROUGH_ID_4                                      (0x24)
#define AVRCP_PASS_THROUGH_ID_5                                      (0x25)
#define AVRCP_PASS_THROUGH_ID_6                                      (0x26)
#define AVRCP_PASS_THROUGH_ID_7                                      (0x27)
#define AVRCP_PASS_THROUGH_ID_8                                      (0x28)
#define AVRCP_PASS_THROUGH_ID_9                                      (0x29)
#define AVRCP_PASS_THROUGH_ID_DOT                                    (0x2A)
#define AVRCP_PASS_THROUGH_ID_ENTER                                  (0x2B)
#define AVRCP_PASS_THROUGH_ID_CLEAR                                  (0x2C)
#define AVRCP_PASS_THROUGH_ID_CHANNEL_UP                             (0x30)
#define AVRCP_PASS_THROUGH_ID_CHANNEL_DOWN                           (0x31)
#define AVRCP_PASS_THROUGH_ID_PREVIOUS_CHANNEL                       (0x32)
#define AVRCP_PASS_THROUGH_ID_SOUND_SELECT                           (0x33)
#define AVRCP_PASS_THROUGH_ID_INPUT_SELECT                           (0x34)
#define AVRCP_PASS_THROUGH_ID_DISPLAY_INFORMATION                    (0x35)
#define AVRCP_PASS_THROUGH_ID_HELP                                   (0x36)
#define AVRCP_PASS_THROUGH_ID_PAGE_UP                                (0x37)
#define AVRCP_PASS_THROUGH_ID_PAGE_DOWN                              (0x38)
#define AVRCP_PASS_THROUGH_ID_POWER                                  (0x40)
#define AVRCP_PASS_THROUGH_ID_VOLUME_UP                              (0x41)
#define AVRCP_PASS_THROUGH_ID_VOLUME_DOWN                            (0x42)
#define AVRCP_PASS_THROUGH_ID_MUTE                                   (0x43)
#define AVRCP_PASS_THROUGH_ID_PLAY                                   (0x44)
#define AVRCP_PASS_THROUGH_ID_STOP                                   (0x45)
#define AVRCP_PASS_THROUGH_ID_PAUSE                                  (0x46)
#define AVRCP_PASS_THROUGH_ID_RECORD                                 (0x47)
#define AVRCP_PASS_THROUGH_ID_REWIND                                 (0x48)
#define AVRCP_PASS_THROUGH_ID_FAST_FORWARD                           (0x49)
#define AVRCP_PASS_THROUGH_ID_EJECT                                  (0x4A)
#define AVRCP_PASS_THROUGH_ID_FORWARD                                (0x4B)
#define AVRCP_PASS_THROUGH_ID_BACKWARD                               (0x4C)
#define AVRCP_PASS_THROUGH_ID_ANGLE                                  (0x50)
#define AVRCP_PASS_THROUGH_ID_SUBPICTURE                             (0x51)
#define AVRCP_PASS_THROUGH_ID_F1                                     (0xFF)
#define AVRCP_PASS_THROUGH_ID_F2                                     (0xFF)
#define AVRCP_PASS_THROUGH_ID_F3                                     (0xFF)
#define AVRCP_PASS_THROUGH_ID_F4                                     (0xFF)
#define AVRCP_PASS_THROUGH_ID_F5                                     (0xFF)

#define AVRCP_PASS_THROUGH_ID_VENDOR_UNIQUE                          (0x7E)

   /* The following constants define the lengths associated with various*/
   /* Operation IDs.                                                    */
   /* * NOTE * All AVRCP Pass Through commands have an operational Data */
   /*          Field length of zero.                                    */
   /* * NOTE * These values are defined in the IEEE 1394 "AV/C Digital  */
   /*          Interface Command Set" Specification.                    */
#define AVRCP_PASS_THROUGH_ID_OPERATION_DATA_FIELD_LENGTH            (0x00)

#define AVRCP_PASS_THROUGH_ID_SELECT_LENGTH                          (0x00)
#define AVRCP_PASS_THROUGH_ID_UP_LENGTH                              (0x00)
#define AVRCP_PASS_THROUGH_ID_DOWN_LENGTH                            (0x00)
#define AVRCP_PASS_THROUGH_ID_LEFT_LENGTH                            (0x00)
#define AVRCP_PASS_THROUGH_ID_RIGHT_LENGTH                           (0x00)
#define AVRCP_PASS_THROUGH_ID_RIGHT_UP_LENGTH                        (0x00)
#define AVRCP_PASS_THROUGH_ID_RIGHT_DOWN_LENGTH                      (0x00)
#define AVRCP_PASS_THROUGH_ID_LEFT_UP_LENGTH                         (0x00)
#define AVRCP_PASS_THROUGH_ID_LEFT_DOWN_LENGTH                       (0x00)
#define AVRCP_PASS_THROUGH_ID_ROOT_MENU_LENGTH                       (0x00)
#define AVRCP_PASS_THROUGH_ID_SETUP_MENU_LENGTH                      (0x00)
#define AVRCP_PASS_THROUGH_ID_CONTENTS_MENU_LENGTH                   (0x00)
#define AVRCP_PASS_THROUGH_ID_FAVORITE_MENU_LENGTH                   (0x00)
#define AVRCP_PASS_THROUGH_ID_EXIT_LENGTH                            (0x00)
#define AVRCP_PASS_THROUGH_ID_0_LENGTH                               (0x00)
#define AVRCP_PASS_THROUGH_ID_1_LENGTH                               (0x00)
#define AVRCP_PASS_THROUGH_ID_2_LENGTH                               (0x00)
#define AVRCP_PASS_THROUGH_ID_3_LENGTH                               (0x00)
#define AVRCP_PASS_THROUGH_ID_4_LENGTH                               (0x00)
#define AVRCP_PASS_THROUGH_ID_5_LENGTH                               (0x00)
#define AVRCP_PASS_THROUGH_ID_6_LENGTH                               (0x00)
#define AVRCP_PASS_THROUGH_ID_7_LENGTH                               (0x00)
#define AVRCP_PASS_THROUGH_ID_8_LENGTH                               (0x00)
#define AVRCP_PASS_THROUGH_ID_9_LENGTH                               (0x00)
#define AVRCP_PASS_THROUGH_ID_DOT_LENGTH                             (0x00)
#define AVRCP_PASS_THROUGH_ID_ENTER_LENGTH                           (0x00)
#define AVRCP_PASS_THROUGH_ID_CLEAR_LENGTH                           (0x00)
#define AVRCP_PASS_THROUGH_ID_CHANNEL_UP_LENGTH                      (0x00)
#define AVRCP_PASS_THROUGH_ID_CHANNEL_DOWN_LENGTH                    (0x00)
#define AVRCP_PASS_THROUGH_ID_PREVIOUS_CHANNEL_LENGTH                (0x00)
#define AVRCP_PASS_THROUGH_ID_SOUND_SELECT_LENGTH                    (0x00)
#define AVRCP_PASS_THROUGH_ID_INPUT_SELECT_LENGTH                    (0x00)
#define AVRCP_PASS_THROUGH_ID_DISPLAY_INFORMATION_LENGTH             (0x00)
#define AVRCP_PASS_THROUGH_ID_HELP_LENGTH                            (0x00)
#define AVRCP_PASS_THROUGH_ID_PAGE_UP_LENGTH                         (0x00)
#define AVRCP_PASS_THROUGH_ID_PAGE_DOWN_LENGTH                       (0x00)
#define AVRCP_PASS_THROUGH_ID_POWER_LENGTH                           (0x00)
#define AVRCP_PASS_THROUGH_ID_VOLUME_UP_LENGTH                       (0x00)
#define AVRCP_PASS_THROUGH_ID_VOLUME_DOWN_LENGTH                     (0x00)
#define AVRCP_PASS_THROUGH_ID_MUTE_LENGTH                            (0x00)
#define AVRCP_PASS_THROUGH_ID_PLAY_LENGTH                            (0x00)
#define AVRCP_PASS_THROUGH_ID_STOP_LENGTH                            (0x00)
#define AVRCP_PASS_THROUGH_ID_PAUSE_LENGTH                           (0x00)
#define AVRCP_PASS_THROUGH_ID_RECORD_LENGTH                          (0x00)
#define AVRCP_PASS_THROUGH_ID_REWIND_LENGTH                          (0x00)
#define AVRCP_PASS_THROUGH_ID_FAST_FORWARD_LENGTH                    (0x00)
#define AVRCP_PASS_THROUGH_ID_EJECT_LENGTH                           (0x00)
#define AVRCP_PASS_THROUGH_ID_FORWARD_LENGTH                         (0x00)
#define AVRCP_PASS_THROUGH_ID_BACKWARD_LENGTH                        (0x00)
#define AVRCP_PASS_THROUGH_ID_ANGLE_LENGTH                           (0x00)
#define AVRCP_PASS_THROUGH_ID_SUBPICTURE_LENGTH                      (0x00)
#define AVRCP_PASS_THROUGH_ID_F1_LENGTH                              (0x00)
#define AVRCP_PASS_THROUGH_ID_F2_LENGTH                              (0x00)
#define AVRCP_PASS_THROUGH_ID_F3_LENGTH                              (0x00)
#define AVRCP_PASS_THROUGH_ID_F4_LENGTH                              (0x00)
#define AVRCP_PASS_THROUGH_ID_F5_LENGTH                              (0x00)

   /* The following types are the defined AVRCP Pass Through Data Field */
   /* formats for the AVRCP Pass Through Group Comand/Responses.        */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Pass_Through_Group_Navigation_Command_Data_t
{
   AVRCP_Pass_Through_Command_Header_t AVRCP_Pass_Through_Command_Header;
   AVRCP_Company_ID_t                  CompanyID;
   NonAlignedWord_t                    VendorUniqueID;
} __PACKED_STRUCT_END__ AVRCP_Pass_Through_Group_Navigation_Command_Data_t;

#define AVRCP_PASS_THROUGH_GROUP_NAVIGATION_COMMAND_DATA_SIZE        (sizeof(AVRCP_Pass_Through_Group_Navigation_Command_Data_t))

typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Pass_Through_Group_Navigation_Response_Data_t
{
   AVRCP_Pass_Through_Response_Header_t AVRCP_Pass_Through_Response_Header;
   AVRCP_Company_ID_t                   CompanyID;
   NonAlignedWord_t                     VendorUniqueID;
} __PACKED_STRUCT_END__ AVRCP_Pass_Through_Group_Navigation_Response_Data_t;

#define AVRCP_PASS_THROUGH_GROUP_NAVIGATION_RESPONSE_DATA_SIZE       (sizeof(AVRCP_Pass_Through_Group_Navigation_Response_Data_t))

   /* The following constants define the Vendor Unique ID Pass-through  */
   /* AVRCP values.                                                     */
#define AVRCP_PASS_THROUGH_VENDOR_UNIQUE_ID_NEXT_GROUP               (0x0001)
#define AVRCP_PASS_THROUGH_VENDOR_UNIQUE_ID_PREVIOUS_GROUP           (0x0002)

   /* The following structure defines a Vendor Dependent Command        */
   /* message.  It contains a command message header followed by the    */
   /* Vendor Company ID.  The Data member is a variable length array of */
   /* data that is zero or more bytes that holds the Vendor Dependent   */
   /* Command Data.                                                     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Vendor_Dependent_Command_t
{
   AVRCP_Command_Header_t AVRCP_Command_Header;
   AVRCP_Company_ID_t     CompanyID;
   Byte_t                 Data[1];
} __PACKED_STRUCT_END__ AVRCP_Vendor_Dependent_Command_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an outgoing Vendor Dependent command */
   /* frame.  The input parameter specifies the length actual Vendor    */
   /* Dependent data bytes that are part of the command.  This MACRO    */
   /* calculates the total size required (in bytes) to hold the data    */
   /* PLUS the AVRCP Vendor Dependent Command Header.                   */
#define AVRCP_CALCULATE_VENDOR_DEPENDENT_COMMAND_SIZE(_x)            (sizeof(AVRCP_Vendor_Dependent_Command_t) - BYTE_SIZE + (unsigned int)(_x))

   /* The following is a utility MACRO that assigns a Company ID,       */
   /* specified by the second parameter at the appropriate position in  */
   /* the Vendor Dependent command structure specified by the first     */
   /* parameter (a pointer to the Vendor Dependent Command structure).  */
#define AVRCP_ASSIGN_VENDOR_DEPENDENT_COMMAND_COMPANY_ID(_x, _y) \
   ASSIGN_UNALIGNED_GENERIC_TYPE((&(((AVRCP_Vendor_Dependent_Command_t *)(_x))->CompanyID)), (*((AVRCP_Company_ID_t *)(&(_y)))))

   /* The following is a utility MACRO that reads a Company ID from the */
   /* Vendor Dependent command structure specified as the first         */
   /* parameter (a pointer to the Vendor Dependent Command structure).  */
#define AVRCP_READ_VENDOR_DEPENDENT_COMMAND_COMPANY_ID(_x) \
   READ_UNALIGNED_GENERIC_TYPE(&(((AVRCP_Vendor_Dependent_Command_t *)(_x))->CompanyID))

   /* The following structure defines a Vendor Dependent Response       */
   /* message.  It contains a response message header followed by the   */
   /* Vendor Company ID.  The Data member is a variable length array of */
   /* data that is zero or more bytes that holds the Vendor Dependent   */
   /* response Data.                                                    */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Vendor_Dependent_Response_t
{
   AVRCP_Response_Header_t AVRCP_Response_Header;
   AVRCP_Company_ID_t      CompanyID;
   Byte_t                  Data[1];
} __PACKED_STRUCT_END__ AVRCP_Vendor_Dependent_Response_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an outgoing Vendor Dependent response*/
   /* frame.  The input parameter specifies the length actual Vendor    */
   /* Dependent data bytes that are part of the response.  This MACRO   */
   /* calculates the total size required (in bytes) to hold the data    */
   /* PLUS the AVRCP Vendor Dependent Response Header.                  */
#define AVRCP_CALCULATE_VENDOR_DEPENDENT_RESPONSE_SIZE(_x)           (sizeof(AVRCP_Vendor_Dependent_Response_t) - BYTE_SIZE + (unsigned int)(_x))

   /* The following is a utility MACRO that assigns a Company ID,       */
   /* specified by the second parameter at the appropriate position in  */
   /* the Vendor Dependent response structure specified by the first    */
   /* parameter (a pointer to the Vendor Dependent Response structure). */
#define AVRCP_ASSIGN_VENDOR_DEPENDENT_RESPONSE_COMPANY_ID(_x, _y) \
   ASSIGN_UNALIGNED_GENERIC_TYPE((&((AVRCP_Vendor_Dependent_Response_t *)(_x))->CompanyID), (*((AVRCP_Company_ID_t *)(&(_y)))))

   /* The following is a utility MACRO that reads a Company ID from the */
   /* Vendor Dependent response structure specified as the first        */
   /* parameter (a pointer to the Vendor Dependent Response structure). */
#define AVRCP_READ_VENDOR_DEPENDENT_RESPONSE_COMPANY_ID(_x) \
   READ_UNALIGNED_GENERIC_TYPE(&(((AVRCP_Vendor_Dependent_Response_t *)(_x))->CompanyID))

   /* The following section defines the format and values of AVRCP      */
   /* Vendor Dependent Commands (AVRCP Specific AV/C commands).         */

   /* The following structure defines the format of an AVRCP Vendor     */
   /* Dependent Command (AVRCP Specific AV/C command) Header.           */
   /* * NOTE * This message is simply a Vendor Dependent Command with a */
   /*          specially formatted Vendor Dependent Payload.  Since this*/
   /*          message type is used so heavily in AVRCP, it will get    */
   /*          it's own definition to aid code readability/maintenance. */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Specific_AV_C_Command_Header_t
{
   AVRCP_Command_Header_t AVRCP_Command_Header;
   AVRCP_Company_ID_t     CompanyID;
   Byte_t                 PDU_ID;
   Byte_t                 PacketType;
   NonAlignedWord_t       ParameterLength;
} __PACKED_STRUCT_END__ AVRCP_Specific_AV_C_Command_Header_t;

#define AVRCP_SPECIFIC_AV_C_COMMAND_HEADER_SIZE                      (sizeof(AVRCP_Specific_AV_C_Command_Header_t))

   /* The following structure defines the format of an AVRCP Vendor     */
   /* Dependent Command (AVRCP Specific AV/C command) message.          */
   /* * NOTE * This message is simply a Vendor Dependent Command with a */
   /*          specially formatted Vendor Dependent Payload.  Since this*/
   /*          message type is used so heavily in AVRCP, it will get    */
   /*          it's own definition to aid code readability/maintenance. */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Specific_AV_C_Command_t
{
   AVRCP_Command_Header_t AVRCP_Command_Header;
   AVRCP_Company_ID_t     CompanyID;
   Byte_t                 PDU_ID;
   Byte_t                 PacketType;
   NonAlignedWord_t       ParameterLength;
   Byte_t                 ParameterData[1];
} __PACKED_STRUCT_END__ AVRCP_Specific_AV_C_Command_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an outgoing AVRCP Specific AV/C      */
   /* Command frame.  The input parameter specifies the length actual   */
   /* AVRCP Specific AV/C Parameter bytes that are part of the command. */
   /* This MACRO calculates the total size required (in bytes) to hold  */
   /* the data PLUS the AVRCP Specific AV/C Command Header.             */
#define AVRCP_CALCULATE_AVRCP_SPECIFIC_AV_C_COMMAND_SIZE(_x)         (sizeof(AVRCP_Specific_AV_C_Command_t) - BYTE_SIZE + (unsigned int)(_x))

   /* The following structure defines the format of an AVRCP Vendor     */
   /* Dependent Response (AVRCP Specific AV/C command) Header.          */
   /* * NOTE * This message is simply a Vendor Dependent Response with a*/
   /*          specially formatted Vendor Dependent Payload.  Since this*/
   /*          message type is used so heavily in AVRCP, it will get    */
   /*          it's own definition to aid code readability/maintenance. */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Specific_AV_C_Response_Header_t
{
   AVRCP_Response_Header_t AVRCP_Response_Header;
   AVRCP_Company_ID_t      CompanyID;
   Byte_t                  PDU_ID;
   Byte_t                  PacketType;
   NonAlignedWord_t        ParameterLength;
} __PACKED_STRUCT_END__ AVRCP_Specific_AV_C_Response_Header_t;

#define AVRCP_SPECIFIC_AV_C_RESPONSE_HEADER_SIZE                     (sizeof(AVRCP_Specific_AV_C_Response_Header_t))

   /* The following structure defines the format of an AVRCP Vendor     */
   /* Dependent Response (AVRCP Specific AV/C response) message.        */
   /* * NOTE * This message is simply a Vendor Dependent Response with a*/
   /*          specially formatted Vendor Dependent Payload.  Since this*/
   /*          message type is used so heavily in AVRCP, it will get    */
   /*          it's own definition to aid code readability/maintenance. */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Specific_AV_C_Response_t
{
   AVRCP_Response_Header_t AVRCP_Response_Header;
   AVRCP_Company_ID_t      CompanyID;
   Byte_t                  PDU_ID;
   Byte_t                  PacketType;
   NonAlignedWord_t        ParameterLength;
   Byte_t                  ParameterData[1];
} __PACKED_STRUCT_END__ AVRCP_Specific_AV_C_Response_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an outgoing AVRCP Specific AV/C      */
   /* Response frame.  The input parameter specifies the length actual  */
   /* AVRCP Specific AV/C Parameter bytes that are part of the response.*/
   /* This MACRO calculates the total size required (in bytes) to hold  */
   /* the data PLUS the AVRCP Specific AV/C Response Header.            */
#define AVRCP_CALCULATE_AVRCP_SPECIFIC_AV_C_RESPONSE_SIZE(_x)        (sizeof(AVRCP_Specific_AV_C_Response_t) - BYTE_SIZE + (unsigned int)(_x))

   /* The following structure defines the format of an AVRCP AV/C       */
   /* Command Reject Response (AVRCP Specific AV/C command).            */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Specific_AV_C_Command_Reject_Response_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Response_Header;
   Byte_t                                ErrorCode;
} __PACKED_STRUCT_END__ AVRCP_Specific_AV_C_Command_Reject_Response_t;

#define AVRCP_SPECIFIC_AV_C_COMMAND_REJECT_RESPONSE_SIZE             (sizeof(AVRCP_Specific_AV_C_Command_Reject_Response_t))

   /* The following constants define the AVRCP Vendor Dependent Command */
   /* PDU ID's for AVRCP Specific AV/C commands.                        */
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_GET_CAPABILITIES                         (0x10)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_LIST_PLAYER_SETTING_ATTRIBUTES           (0x11)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_LIST_PLAYER_SETTING_VALUES               (0x12)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_GET_CURRENT_PLAYER_SETTING_VALUE         (0x13)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_SET_PLAYER_SETTING_VALUE                 (0x14)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_GET_PLAYER_SETTING_ATTRIBUTE_TEXT        (0x15)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_GET_PLAYER_SETTING_VALUE_TEXT            (0x16)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_INFORM_DISPLAYABLE_CHAR_SET              (0x17)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_INFORM_BATTERY_STATUS_OF_CT              (0x18)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_GET_ELEMENT_ATTRIBUTES                   (0x20)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_GET_PLAY_STATUS                          (0x30)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_REGISTER_NOTIFICATION                    (0x31)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_EVENT_PLAYBACK_STATUS_CHANGED            (0x31)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_EVENT_TRACK_CHANGED                      (0x31)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_EVENT_TRACK_REACHED_END                  (0x31)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_EVENT_TRACK_REACHED_START                (0x31)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_EVENT_PLAYBACK_POS_CHANGED               (0x31)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_EVENT_BATT_STATUS_CHANGED                (0x31)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_EVENT_SYSTEM_STATUS_CHANGED              (0x31)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_EVENT_PLAYER_SETTING_CHANGED             (0x31)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_REQUEST_CONTINUING_RESPONSE              (0x40)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_ABORT_CONTINUING_RESPONSE                (0x41)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_SET_ABSOLUTE_VOLUME                      (0x50)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_EVENT_VOLUME_CHANGED                     (0x31)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_SET_ADDRESSED_PLAYER                     (0x60)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_GET_FOLDER_ITEMS                         (0x71)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_GET_FOLDER_ITEMS_MEDIA_PLAYER_LIST       (0x71)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_EVENT_ADDRESSED_PLAYER_CHANGED           (0x31)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_EVENT_NOTIFY_AVAILABLE_PLAYERS_CHANGED   (0x31)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_SET_BROWSED_PLAYER                       (0x70)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_GET_FOLDER_ITEMS_FILE_SYSTEM             (0x71)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_CHANGE_PATH                              (0x72)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_GET_ITEM_ATTRIBUTES                      (0x73)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_PLAY_ITEM                                (0x74)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_PLAY_ITEM_FILE_SYSTEM                    (0x74)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_EVENT_UIDS_CHANGED                       (0x31)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_SEARCH                                   (0x80)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_GET_FOLDER_ITEMS_SEARCH_RESULT_LIST      (0x71)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_PLAY_ITEM_SEARCH_RESULT_LIST             (0x74)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_GET_FOLDER_ITEMS_NOW_PLAYING_LIST        (0x71)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_PLAY_ITEM_NOW_PLAYING_LIST               (0x74)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_ADD_TO_NOW_PLAYING                       (0x90)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_EVENT_NOW_PLAYING_CONTENT_CHANGED        (0x31)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PDU_ID_GENERAL_REJECT                           (0xA0)

   /* The following constants define the AVRCP Vendor Dependent Command */
   /* Packet Types for AVRCP Specific AV/C commands.                    */
#define AVRCP_SPECIFIC_AV_C_COMMAND_PACKET_TYPE_MASK                 (0x00)

#define AVRCP_SPECIFIC_AV_C_COMMAND_PACKET_TYPE_UNFRAGMENTED         (0x00)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PACKET_TYPE_START                (0x01)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PACKET_TYPE_CONTINUE             (0x02)
#define AVRCP_SPECIFIC_AV_C_COMMAND_PACKET_TYPE_END                  (0x03)

   /* Bluetooth AVRCP Specific AV/C Command/Response PDU type           */
   /* definitions/constants follow.                                     */

   /* The following type definition defines the structure of a Get      */
   /* Capabilities Command PDU (AVRCP Specific AV/C Command).           */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Get_Capabilities_Command_PDU_t
{
   AVRCP_Specific_AV_C_Command_Header_t AVRCP_Specific_Command_Header;
   Byte_t                               CapabilityID;
} __PACKED_STRUCT_END__ AVRCP_Get_Capabilities_Command_PDU_t;

#define AVRCP_GET_CAPABILITIES_COMMAND_PDU_SIZE                      (sizeof(AVRCP_Get_Capabilities_Command_PDU_t))

   /* The following type definition defines the structure of a Get      */
   /* Capabilities Response PDU (AVRCP Specific AV/C Response).         */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Get_Capabilities_Response_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Response_Header;
   Byte_t                                CapabilityID;
   Byte_t                                CapabilityCount;
   Byte_t                                CapabilityData[1];
} __PACKED_STRUCT_END__ AVRCP_Get_Capabilities_Response_PDU_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an outgoing AVRCP Specific AV/C Get  */
   /* Capababilities Response PDU.  The input parameter specifies the   */
   /* length of the actual AVRCP Specific AV/C Capabilitity bytes that  */
   /* are part of the response.  This MACRO calculates the total size   */
   /* required (in bytes) to hold the data PLUS the AVRCP Specific AV/C */
   /* Get Capabilities Response PDU Header.                             */
#define AVRCP_GET_CAPABILITIES_RESPONSE_PDU_SIZE(_x)                 (sizeof(AVRCP_Get_Capabilities_Response_PDU_t) - BYTE_SIZE + (unsigned int)(_x))

   /* The following constants represent the defined values for the      */
   /* CapabilityID member of the AVRCP Specific AV/C Get Capabilities   */
   /* Command/Response PDU.                                             */
#define AVRCP_GET_CAPABILITIES_CAPABILITY_ID_COMPANY_ID              (0x02)
#define AVRCP_GET_CAPABILITIES_CAPABILITY_ID_EVENTS_SUPPORTED        (0x03)

   /* The following constants define the minimum and maximum number of  */
   /* supported capabilities for a device (respectively).               */
#define AVRCP_GET_CAPABILITIES_MINIMUM_SUPPORTED_COMPANY_ID_COUNT    (0x01)
#define AVRCP_GET_CAPABILITIES_MAXIMUM_SUPPORTED_COMPANY_ID_COUNT    (0xFF)

   /* The following constants define the minimum and maximum number of  */
   /* supported Events for a device (respectively).                     */
#define AVRCP_MINIMUM_SUPPORTED_EVENT_COUNT                          (0x02)
#define AVRCP_MAXIMUM_SUPPORTED_EVENT_COUNT                          (0xFF)

   /* The following constants represent the supported Notification      */
   /* Events.                                                           */
#define AVRCP_EVENT_PLAYBACK_STATUS_CHANGED                          (0x01)
#define AVRCP_EVENT_TRACK_CHANGED                                    (0x02)
#define AVRCP_EVENT_TRACK_REACHED_END                                (0x03)
#define AVRCP_EVENT_TRACK_REACHED_START                              (0x04)
#define AVRCP_EVENT_PLAYBACK_POS_CHANGED                             (0x05)
#define AVRCP_EVENT_BATT_STATUS_CHANGED                              (0x06)
#define AVRCP_EVENT_SYSTEM_STATUS_CHANGED                            (0x07)
#define AVRCP_EVENT_PLAYER_APPLICATION_SETTING_CHANGED               (0x08)
#define AVRCP_EVENT_NOW_PLAYING_CONTENT_CHANGED                      (0x09)
#define AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED                        (0x0A)
#define AVRCP_EVENT_ADDRESSED_PLAYER_CHANGED                         (0x0B)
#define AVRCP_EVENT_UIDS_CHANGED                                     (0x0C)
#define AVRCP_EVENT_VOLUME_CHANGED                                   (0x0D)

   /* The following type definition defines the structure of a List     */
   /* Player Application Setting Attributes Command PDU (AVRCP Specific */
   /* AV/C Command).                                                    */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_List_Player_Setting_Attributes_Command_PDU_t
{
   AVRCP_Specific_AV_C_Command_Header_t AVRCP_Specific_Command_Header;
} __PACKED_STRUCT_END__ AVRCP_List_Player_Setting_Attributes_Command_PDU_t;

#define AVRCP_LIST_PLAYER_SETTING_ATTRIBUTES_COMMAND_PDU_SIZE        (sizeof(AVRCP_List_Player_Setting_Attributes_Command_PDU_t))

   /* The following type definition defines the structure of a List     */
   /* Player Application Setting Attributes Response PDU (AVRCP Specific*/
   /* AV/C Response).                                                   */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_List_Player_Setting_Attributes_Response_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Response_Header;
   Byte_t                                NumberAttributeIDs;
   Byte_t                                AttributeIDList[1];
} __PACKED_STRUCT_END__ AVRCP_List_Player_Setting_Attributes_Response_PDU_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an outgoing AVRCP Specific AV/C List */
   /* Player Application Attributes Setting Response PDU.  The input    */
   /* parameter specifies the length of the actual AVRCP Specific AV/C  */
   /* Attribute ID List bytes that are part of the response.  This MACRO*/
   /* calculates the total size required (in bytes) to hold the data    */
   /* PLUS the AVRCP Specific AV/C List Application Setting Attributes  */
   /* Response PDU Header.                                              */
#define AVRCP_LIST_PLAYER_SETTING_ATTRIBUTES_RESPONSE_PDU_SIZE(_x)   (sizeof(AVRCP_List_Player_Setting_Attributes_Response_PDU_t) - BYTE_SIZE + (unsigned int)(_x))

   /* The following constants define the minimum and maximum number of  */
   /* supported Player Setting Attribute IDs for a device               */
   /* (respectively).                                                   */
#define AVRCP_LIST_PLAYER_SETTING_ATTRIBUTES_MINIMUM_SUPPORTED_ATTRIBUTES_ID_COUNT (0x00)
#define AVRCP_LIST_PLAYER_SETTING_ATTRIBUTES_MAXIMUM_SUPPORTED_ATTRIBUTES_ID_COUNT (0xFF)

   /* The following constants represent the defined Player Application  */
   /* Setting Attributes IDs.                                           */
#define AVRCP_PLAYER_SETTING_ATTRIBUTE_ID_ILLEGAL                                  (0x00)
#define AVRCP_PLAYER_SETTING_ATTRIBUTE_ID_EQUALIZER_ON_OFF_STATUS                  (0x01)
#define AVRCP_PLAYER_SETTING_ATTRIBUTE_ID_REPEAT_MODE_STATUS                       (0x02)
#define AVRCP_PLAYER_SETTING_ATTRIBUTE_ID_SHUFFLE_ON_OFF_STATUS                    (0x03)
#define AVRCP_PLAYER_SETTING_ATTRIBUTE_ID_SCAN_ON_OFF_STATUS                       (0x04)

#define AVRCP_PLAYER_SETTING_ATTRIBUTE_ID_TARGET_MEDIA_PLAYER_MENU_EXTENSION_START (0x80)
#define AVRCP_PLAYER_SETTING_ATTRIBUTE_ID_TARGET_MEDIA_PLAYER_MENU_EXTENSION_END   (0xFF)

   /* The following type definition defines the structure of a List     */
   /* Player Application Setting Values Command PDU (AVRCP Specific     */
   /* AV/C Command).                                                    */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_List_Player_Setting_Values_Command_PDU_t
{
   AVRCP_Specific_AV_C_Command_Header_t AVRCP_Specific_Command_Header;
   Byte_t                               AttributeID;
} __PACKED_STRUCT_END__ AVRCP_List_Player_Setting_Values_Command_PDU_t;

#define AVRCP_LIST_PLAYER_SETTING_VALUES_COMMAND_PDU_SIZE         (sizeof(AVRCP_List_Player_Setting_Values_Command_PDU_t))

   /* The following type definition defines the structure of a List     */
   /* Player Application Setting Values Response PDU (AVRCP Specific    */
   /* AV/C Response).                                                   */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_List_Player_Setting_Values_Response_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Response_Header;
   Byte_t                                NumberValueIDs;
   Byte_t                                ValueIDList[1];
} __PACKED_STRUCT_END__ AVRCP_List_Player_Setting_Values_Response_PDU_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an outgoing AVRCP Specific AV/C List */
   /* Player Application Setting Value Response PDU.  The input         */
   /* parameter specifies the length of the actual AVRCP Specific AV/C  */
   /* Values List bytes that are part of the response.  This MACRO      */
   /* calculates the total size required (in bytes) to hold the data    */
   /* PLUS the AVRCP Specific AV/C List Application Setting Value       */
   /* Response PDU Header.                                              */
#define AVRCP_LIST_PLAYER_SETTING_VALUES_RESPONSE_PDU_SIZE(_x)       (sizeof(AVRCP_List_Player_Setting_Values_Response_PDU_t) - BYTE_SIZE + (unsigned int)(_x))

   /* The following constants define the minimum and maximum number of  */
   /* supported Player Settings Value IDs for a device (respectively).  */
#define AVRCP_LIST_PLAYER_SETTINGS_ATTRIBUTES_MINIMUM_SUPPORTED_VALUE_ID_COUNT (0x01)
#define AVRCP_LIST_PLAYER_SETTINGS_ATTRIBUTES_MAXIMUM_SUPPORTED_VALUE_ID_COUNT (0xFF)

   /* The following constants represent the defined Player Application  */
   /* Settings Value IDs (corresponding to Player Application Settings  */
   /* Attributes).                                                      */
#define AVRCP_PLAYER_SETTING_ATTRIBUTE_VALUE_ID_EQUALIZER_STATUS_OFF  (0x01)
#define AVRCP_PLAYER_SETTING_ATTRIBUTE_VALUE_ID_EQUALIZER_STATUS_ON   (0x02)

#define AVRCP_PLAYER_SETTING_VALUE_ID_REPEAT_MODE_STATUS_OFF          (0x01)
#define AVRCP_PLAYER_SETTING_VALUE_ID_REPEAT_MODE_STATUS_SINGLE_TRACK (0x02)
#define AVRCP_PLAYER_SETTING_VALUE_ID_REPEAT_MODE_STATUS_ALL_TRACKS   (0x03)
#define AVRCP_PLAYER_SETTING_VALUE_ID_REPEAT_MODE_STATUS_GROUP        (0x04)

#define AVRCP_PLAYER_SETTING_VALUE_ID_SHUFFLE_STATUS_OFF              (0x01)
#define AVRCP_PLAYER_SETTING_VALUE_ID_SHUFFLE_STATUS_ALL_TRACKS       (0x02)
#define AVRCP_PLAYER_SETTING_VALUE_ID_SHUFFLE_STATUS_GROUP            (0x03)

#define AVRCP_PLAYER_SETTING_VALUE_ID_SCAN_STATUS_OFF                 (0x01)
#define AVRCP_PLAYER_SETTING_VALUE_ID_SCAN_STATUS_ALL_TRACKS          (0x02)
#define AVRCP_PLAYER_SETTING_VALUE_ID_SCAN_STATUS_GROUP               (0x03)

   /* The following type definition defines the structure of a Get      */
   /* Current Player Application Setting Value Command PDU (AVRCP       */
   /* Specific AV/C Command).                                           */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Get_Current_Player_Setting_Value_Command_PDU_t
{
   AVRCP_Specific_AV_C_Command_Header_t AVRCP_Specific_Command_Header;
   Byte_t                               NumberAttributeIDs;
   Byte_t                               AttributeIDList[1];
} __PACKED_STRUCT_END__ AVRCP_Get_Current_Player_Setting_Value_Command_PDU_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an outgoing AVRCP Specific AV/C Get  */
   /* Current Player Application Setting Value Command PDU.  The input  */
   /* parameter specifies the length of the actual AVRCP Specific AV/C  */
   /* Attribute ID List bytes that are part of the command.  This MACRO */
   /* calculates the total size required (in bytes) to hold the data    */
   /* PLUS the AVRCP Specific AV/C Get Current Player Application       */
   /* Setting Value Command PDU Header.                                 */
#define AVRCP_GET_CURRENT_PLAYER_SETTING_VALUE_COMMAND_PDU_SIZE(_x)  (sizeof(AVRCP_Get_Current_Player_Setting_Value_Command_PDU_t) - BYTE_SIZE + (unsigned int)(_x))

   /* The following type definition is used with the                    */
   /* AVRCP_Get_Current_Player_Setting_Value_Response_PDU_t and         */
   /* AVRCP_Set_Player_Setting_Value_Command_PDU_t PDU's to specify an  */
   /* Attribute/Value ID pair.                                          */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Attribute_Value_ID_Pair_t
{
   Byte_t AttributeID;
   Byte_t ValueID;
} __PACKED_STRUCT_END__ AVRCP_Attribute_Value_ID_Pair_t;

#define AVRCP_ATTRIBUTE_VALUE_ID_PAIR_SIZE                           (sizeof(AVRCP_Attribute_Value_ID_Pair_t))

   /* The following type definition defines the structure of a Get      */
   /* Current Player Application Setting Value Response PDU (AVRCP      */
   /* Specific AV/C Response).                                          */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Get_Current_Player_Setting_Value_Response_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Response_Header;
   Byte_t                                NumberAttributeValueIDs;
   AVRCP_Attribute_Value_ID_Pair_t       AttributeValueIDList[1];
} __PACKED_STRUCT_END__ AVRCP_Get_Current_Player_Setting_Value_Response_PDU_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an outgoing AVRCP Specific AV/C Get  */
   /* Current Player Application Setting Value Response PDU.  The input */
   /* parameter specifies the the number of AVRCP Specific AV/C Current */
   /* Player Application Setting Attribute/Value Pairs that are part of */
   /* the response (not the actual number of bytes).  This MACRO        */
   /* calculates the total size required (in bytes) to hold the data    */
   /* PLUS the AVRCP Specific AV/C Get Current Player Application       */
   /* Setting Value Response PDU Header.                                */
#define AVRCP_GET_CURRENT_PLAYER_SETTING_VALUE_RESPONSE_PDU_SIZE(_x)    (sizeof(AVRCP_Get_Current_Player_Setting_Value_Response_PDU_t) - AVRCP_ATTRIBUTE_VALUE_ID_PAIR_SIZE + ((unsigned int)(_x)*AVRCP_ATTRIBUTE_VALUE_ID_PAIR_SIZE))

   /* The following type definition defines the structure of a Set      */
   /* Player Application Setting Value Command PDU (AVRCP Specific AV/C */
   /* Command).                                                         */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Set_Player_Setting_Value_Command_PDU_t
{
   AVRCP_Specific_AV_C_Command_Header_t AVRCP_Specific_Command_Header;
   Byte_t                               NumberAttributeValueIDs;
   AVRCP_Attribute_Value_ID_Pair_t      AttributeValueIDList[1];
} __PACKED_STRUCT_END__ AVRCP_Set_Player_Setting_Value_Command_PDU_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an outgoing AVRCP Specific AV/C Set  */
   /* Player Application Setting Value Command PDU.  The input parameter*/
   /* specifies the the number of AVRCP Specific AV/C Player Application*/
   /* Setting Attribute/Value Pairs that are part of the command (not   */
   /* the actual number of bytes).  This MACRO calculates the total size*/
   /* required (in bytes) to hold the data PLUS the AVRCP Specific AV/C */
   /* Set Player Application Setting Value Command PDU Header.          */
#define AVRCP_SET_PLAYER_SETTING_VALUE_COMMAND_PDU_SIZE(_x)          (sizeof(AVRCP_Set_Player_Setting_Value_Command_PDU_t) - AVRCP_ATTRIBUTE_VALUE_ID_PAIR_SIZE + ((unsigned int)(_x)*AVRCP_ATTRIBUTE_VALUE_ID_PAIR_SIZE))

   /* The following type definition defines the structure of a Set      */
   /* Player Application Setting Value Response PDU (AVRCP Specific AV/C*/
   /* Response).                                                        */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Set_Player_Setting_Value_Response_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Response_Header;
} __PACKED_STRUCT_END__ AVRCP_Set_Player_Setting_Value_Response_PDU_t;

#define AVRCP_SET_PLAYER_SETTING_VALUE_RESPONSE_PDU_SIZE             (sizeof(AVRCP_Set_Player_Setting_Value_Response_PDU_t))

   /* The following type definition defines the structure of a Get      */
   /* Player Application Setting Attribute Text Command PDU (AVRCP      */
   /* Specific AV/C Command).                                           */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Get_Player_Setting_Attribute_Text_Command_PDU_t
{
   AVRCP_Specific_AV_C_Command_Header_t AVRCP_Specific_Command_Header;
   Byte_t                               NumberAttributeIDs;
   Byte_t                               AttributeIDList[1];
} __PACKED_STRUCT_END__ AVRCP_Get_Player_Setting_Attribute_Text_Command_PDU_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an outgoing AVRCP Specific AV/C Get  */
   /* Player Application Setting Attribute Text Command PDU.  The input */
   /* parameter specifies the length of the actual AVRCP Specific AV/C  */
   /* Attribute ID List bytes that are part of the command.  This MACRO */
   /* calculates the total size required (in bytes) to hold the data    */
   /* PLUS the AVRCP Specific AV/C Get Player Application Setting       */
   /* Attribute Text Command PDU Header.                                */
#define AVRCP_GET_PLAYER_SETTING_ATTRIBUTE_TEXT_COMMAND_PDU_SIZE(_x) (sizeof(AVRCP_Get_Player_Setting_Attribute_Text_Command_PDU_t) - BYTE_SIZE + (unsigned int)(_x))

   /* The following type definition is used with the                    */
   /* AVRCP_Get_Player_Setting_Attribute_Text_Response_PDU_t PDU to     */
   /* specify a Attribute Text Information.                             */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Attribute_Text_Entry_t
{
   Byte_t           AttributeID;
   NonAlignedWord_t CharacterSet;
   Byte_t           AttributeStringLength;
   Byte_t           AttributeString[1];
} __PACKED_STRUCT_END__ AVRCP_Attribute_Text_Entry_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an Attribute Text Entry.  The input  */
   /* parameter specifies the the length (in bytes) of the actual Text  */
   /* String that is part of the Entry (the actual number of bytes).    */
   /* This MACRO calculates the total size required (in bytes) to hold  */
   /* the Text String PLUS the Attribute Information to describe the    */
   /* Text string.                                                      */
#define AVRCP_ATTRIBUTE_TEXT_ENTRY_SIZE(_x)                          (sizeof(AVRCP_Attribute_Text_Entry_t) - BYTE_SIZE + (unsigned int)(_x))

   /* The following type definition defines the structure of a Get      */
   /* Player Application Setting Attribute Text Response PDU (AVRCP     */
   /* Specific AV/C Response).                                          */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Get_Player_Setting_Attribute_Text_Response_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Response_Header;
   Byte_t                                NumberAttributeTextEntries;
   AVRCP_Attribute_Text_Entry_t          AttributeTextData[1];
} __PACKED_STRUCT_END__ AVRCP_Get_Player_Setting_Attribute_Text_Response_PDU_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an outgoing AVRCP Specific AV/C Get  */
   /* Player Application Setting Attribute Text Response PDU.  The input*/
   /* parameter specifies the the number of bytes of actual AVRCP       */
   /* Specific AV/C Attribute Text Data that is part of the response    */
   /* (the actual number of bytes).  This MACRO calculates the total    */
   /* size required (in bytes) to hold the data PLUS the AVRCP Specific */
   /* AV/C Set Player Application Setting Attribute Text Response PDU   */
   /* Header.                                                           */
#define AVRCP_GET_PLAYER_SETTING_ATTRIBUTE_TEXT_RESPONSE_PDU_SIZE(_x) (sizeof(AVRCP_Get_Player_Setting_Attribute_Text_Response_PDU_t) - sizeof(AVRCP_Attribute_Text_Entry_t) + (unsigned int)(_x))

   /* The following type definition defines the structure of a Get      */
   /* Player Application Setting Value Text Command PDU (AVRCP Specific */
   /* AV/C Command).                                                    */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Get_Player_Setting_Value_Text_Command_PDU_t
{
   AVRCP_Specific_AV_C_Command_Header_t AVRCP_Specific_Command_Header;
   Byte_t                               AttributeID;
   Byte_t                               NumberValueIDs;
   Byte_t                               ValueIDList[1];
} __PACKED_STRUCT_END__ AVRCP_Get_Player_Setting_Value_Text_Command_PDU_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an outgoing AVRCP Specific AV/C Get  */
   /* Player Application Setting Value Text Command PDU.  The input     */
   /* parameter specifies the number of Value ID's that are part of the */
   /* command.  This MACRO calculates the total size required (in bytes)*/
   /* to hold the Value ID List PLUS the AVRCP Specific AV/C Get Player */
   /* Application Setting Value Text Command PDU Header.                */
#define AVRCP_GET_PLAYER_SETTING_VALUE_TEXT_COMMAND_PDU_SIZE(_x)     (sizeof(AVRCP_Get_Player_Setting_Value_Text_Command_PDU_t) - BYTE_SIZE + (unsigned int)(_x))

   /* The following type definition is used with the                    */
   /* AVRCP_Get_Player_Setting_Value_Text_Response_PDU_t PDU to specify */
   /* a Value Text Information.                                         */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Value_Text_Entry_t
{
   Byte_t           ValueID;
   NonAlignedWord_t CharacterSet;
   Byte_t           ValueStringLength;
   Byte_t           ValueString[1];
} __PACKED_STRUCT_END__ AVRCP_Value_Text_Entry_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an Value Text Entry.  The input      */
   /* parameter specifies the the length (in bytes) of the actual Text  */
   /* String that is part of the Entry (the actual number of bytes).    */
   /* This MACRO calculates the total size required (in bytes) to hold  */
   /* the Text String PLUS the Value Information to describe the Text   */
   /* string.                                                           */
#define AVRCP_VALUE_TEXT_ENTRY_SIZE(_x)                              (sizeof(AVRCP_Value_Text_Entry_t) - BYTE_SIZE + (unsigned int)(_x))

   /* The following type definition defines the structure of a Get      */
   /* Player Application Setting Value Text Response PDU (AVRCP Specific*/
   /* AV/C Response).                                                   */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Get_Player_Setting_Value_Text_Response_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Response_Header;
   Byte_t                                NumberValueTextEntries;
   AVRCP_Value_Text_Entry_t              ValueTextData[1];
} __PACKED_STRUCT_END__ AVRCP_Get_Player_Setting_Value_Text_Response_PDU_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an outgoing AVRCP Specific AV/C Get  */
   /* Player Application Setting Value Text Response PDU.  The input    */
   /* parameter specifies the the number of bytes of actual AVRCP       */
   /* Specific AV/C Value Text Data that is part of the response (the   */
   /* actual number of bytes).  This MACRO calculates the total size    */
   /* required (in bytes) to hold the data PLUS the AVRCP Specific AV/C */
   /* Set Player Application Setting Value Text Response PDU Header.    */
#define AVRCP_GET_PLAYER_SETTING_VALUE_TEXT_RESPONSE_PDU_SIZE(_x)    (sizeof(AVRCP_Get_Player_Setting_Value_Text_Response_PDU_t) - sizeof(AVRCP_Value_Text_Entry_t) + (unsigned int)(_x))

   /* The following type definition defines the structure of an Inform  */
   /* Displayable Character Set Command PDU (AVRCP Specific AV/C        */
   /* Command).                                                         */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Inform_Displayable_Character_Set_Command_PDU_t
{
   AVRCP_Specific_AV_C_Command_Header_t AVRCP_Specific_Command_Header;
   Byte_t                               NumberDisplayableCharacterSets;
   NonAlignedWord_t                     CharacterSetID[1];
} __PACKED_STRUCT_END__ AVRCP_Inform_Displayable_Character_Set_Command_PDU_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an Inform Displayable Character Set  */
   /* Command PDU.  The input parameter specifies the the number of     */
   /* Character Set entries (NOT individual bytes) that are to part of  */
   /* the actual PDU.  This MACRO calculates the total size required (in*/
   /* bytes) to hold the Inform Displayable Character Set Command PDU   */
   /* with the specified number of Character Set values.                */
#define AVRCP_INFORM_DISPLAYABLE_CHARACTER_SET_COMMAND_PDU_SIZE(_x)  (sizeof(AVRCP_Inform_Displayable_Character_Set_Command_PDU_t) - sizeof(NonAlignedWord_t) + (unsigned int)((_x)*sizeof(NonAlignedWord_t)))

   /* The following type definition defines the structure of an Inform  */
   /* Displayable Character Set Response PDU (AVRCP Specific AV/C       */
   /* Response).                                                        */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Inform_Displayable_Character_Set_Response_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Response_Header;
} __PACKED_STRUCT_END__ AVRCP_Inform_Displayable_Character_Set_Response_PDU_t;

#define AVRCP_INFORM_DISPLAYABLE_CHARACTER_SET_RESPONSE_PDU_SIZE     (sizeof(AVRCP_Inform_Displayable_Character_Set_Response_PDU_t))

   /* The following type definition defines the structure of an Inform  */
   /* Battery Status of Controller Command PDU (AVRCP Specific AV/C     */
   /* Command).                                                         */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Inform_Battery_Status_Of_CT_Command_PDU_t
{
   AVRCP_Specific_AV_C_Command_Header_t AVRCP_Specific_Command_Header;
   Byte_t                               BatteryStatus;
} __PACKED_STRUCT_END__ AVRCP_Inform_Battery_Status_Of_CT_Command_PDU_t;

#define AVRCP_INFORM_BATTERY_STATUS_OF_CT_COMMAND_PDU_SIZE           (sizeof(AVRCP_Inform_Battery_Status_Of_CT_Command_PDU_t))

   /* The following constants define the AVRCP Battery Status values    */
   /* that are used with the BatteryStatus member of the                */
   /* AVRCP_Inform_Battery_Status_Of_CT_Command_PDU_t PDU.              */
#define AVRCP_BATTERY_STATUS_VALUE_NORMAL                            (0x00)
#define AVRCP_BATTERY_STATUS_VALUE_WARNING                           (0x01)
#define AVRCP_BATTERY_STATUS_VALUE_CRITICAL                          (0x02)
#define AVRCP_BATTERY_STATUS_VALUE_EXTERNAL                          (0x03)
#define AVRCP_BATTERY_STATUS_VALUE_FULL_CHARGE                       (0x04)

   /* The following type definition defines the structure of an Inform  */
   /* Battery Status of Controller Response PDU (AVRCP Specific AV/C    */
   /* Response).                                                        */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Inform_Battery_Status_Of_CT_Response_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Response_Header;
} __PACKED_STRUCT_END__ AVRCP_Inform_Battery_Status_Of_CT_Response_PDU_t;

#define AVRCP_INFORM_BATTERY_STATUS_OF_CT_RESPONSE_PDU_SIZE          (sizeof(AVRCP_Inform_Battery_Status_Of_CT_Response_PDU_t))

   /* The following type definition defines the structure of a Get      */
   /* Element Attributes Command PDU (AVRCP Specific AV/C Command).     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Get_Element_Attributes_Command_PDU_t
{
   AVRCP_Specific_AV_C_Command_Header_t AVRCP_Specific_Command_Header;
   NonAlignedQWord_t                    Identifier;
   Byte_t                               NumberAttributes;
   NonAlignedDWord_t                    AttributeIDList[1];
} __PACKED_STRUCT_END__ AVRCP_Get_Element_Attributes_Command_PDU_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an outgoing AVRCP Specific AV/C Set  */
   /* Player Application Setting Value Command PDU.  The input parameter*/
   /* specifies the the number of AVRCP Specific AV/C Player Application*/
   /* Setting Attribute/Value Pairs that are part of the command (not   */
   /* the actual number of bytes).  This MACRO calculates the total size*/
   /* required (in bytes) to hold the data PLUS the AVRCP Specific AV/C */
   /* Set Player Application Setting Value Command PDU Header.          */
#define AVRCP_GET_ELEMENT_ATTRIBUTES_COMMAND_PDU_SIZE(_x)            (sizeof(AVRCP_Get_Element_Attributes_Command_PDU_t) - DWORD_SIZE + ((unsigned int)(_x)*DWORD_SIZE))

   /* The following constants represent the defined Element Identifiers.*/
#define AVRCP_ELEMENT_IDENTIFIER_PLAYING                             (0)

   /* The following constants represent the defined Media Attributes    */
   /* that are used with the AVRCP_Get_Element_Attributes_Command_PDU_t */
   /* and AVRCP_Get_Element_Attributes_Response_PDU_t PDU's.            */
#define AVRCP_MEDIA_ATTRIBUTE_ID_ILLEGAL                             (0x00000000)
#define AVRCP_MEDIA_ATTRIBUTE_ID_TITLE_OF_MEDIA                      (0x00000001)
#define AVRCP_MEDIA_ATTRIBUTE_ID_NAME_OF_ARTIST                      (0x00000002)
#define AVRCP_MEDIA_ATTRIBUTE_ID_NAME_OF_ALBUM                       (0x00000003)
#define AVRCP_MEDIA_ATTRIBUTE_ID_NUMBER_OF_MEDIA                     (0x00000004)
#define AVRCP_MEDIA_ATTRIBUTE_ID_TOTAL_NUMBER_OF_MEDIA               (0x00000005)
#define AVRCP_MEDIA_ATTRIBUTE_ID_GENRE                               (0x00000006)
#define AVRCP_MEDIA_ATTRIBUTE_ID_PLAYING_TIME_MS                     (0x00000007)

   /* The following type definition is used with the                    */
   /* AVRCP_Get_Element_Attributes_Response_PDU_t PDU to specify an     */
   /* individual Element Attribute.                                     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Element_Attribute_Entry_t
{
   NonAlignedDWord_t AttributeID;
   NonAlignedWord_t  CharacterSetID;
   NonAlignedWord_t  AttributeValueLength;
   Byte_t            AttributeValueData[1];
} __PACKED_STRUCT_END__ AVRCP_Element_Attribute_Entry_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an Element Attribute Entry.  The     */
   /* input parameter specifies the the length (in bytes) of the actual */
   /* Attribute Value Data that is part of the Entry (the actual number */
   /* of bytes).  This MACRO calculates the total size required (in     */
   /* bytes) to hold the Element Attribute Data PLUS the Attribute      */
   /* Information to describe the Element Attribute.                    */
#define AVRCP_ELEMENT_ATTRIBUTE_ENTRY_SIZE(_x)                       (sizeof(AVRCP_Element_Attribute_Entry_t) - BYTE_SIZE + (unsigned int)(_x))

   /* The following type definition defines the structure of a Get      */
   /* Element Attributes Response PDU (AVRCP Specific AV/C Response).   */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Get_Element_Attributes_Response_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Response_Header;
   Byte_t                                NumberAttributes;
   Byte_t                                AttributeData[1];
} __PACKED_STRUCT_END__ AVRCP_Get_Element_Attributes_Response_PDU_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an outgoing AVRCP Specific AV/C Get  */
   /* Element Attributes Response PDU.  The input parameter specifies   */
   /* the length of the actual AVRCP Specific AV/C Element Attribute    */
   /* List bytes that are part of the response.  This MACRO calculates  */
   /* the total size required (in bytes) to hold the data PLUS the AVRCP*/
   /* Specific AV/C Get Element Attributes Response PDU Header.         */
#define AVRCP_GET_ELEMENT_ATTRIBUTES_RESPONSE_PDU_SIZE(_x)           (sizeof(AVRCP_Get_Element_Attributes_Response_PDU_t) - BYTE_SIZE + (unsigned int)(_x))

   /* The following type definition defines the structure of a Get Play */
   /* Status Command PDU (AVRCP Specific AV/C Command).                 */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Get_Play_Status_Command_PDU_t
{
   AVRCP_Specific_AV_C_Command_Header_t AVRCP_Specific_Command_Header;
} __PACKED_STRUCT_END__ AVRCP_Get_Play_Status_Command_PDU_t;

#define AVRCP_GET_PLAY_STATUS_COMMAND_PDU_SIZE                       (sizeof(AVRCP_Get_Play_Status_Command_PDU_t))

   /* The following type definition defines the structure of a Get Play */
   /* Status Response PDU (AVRCP Specific AV/C Response).               */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Get_Play_Status_Response_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Response_Header;
   NonAlignedDWord_t                     SongLength;
   NonAlignedDWord_t                     SongPosition;
   Byte_t                                PlayStatus;
} __PACKED_STRUCT_END__ AVRCP_Get_Play_Status_Response_PDU_t;

#define AVRCP_GET_PLAY_STATUS_RESPONSE_PDU_SIZE                      (sizeof(AVRCP_Get_Play_Status_Response_PDU_t))

  /* The following constants are used with the PlayStatus member of the */
  /* AVRCP_Get_Play_Status_Response_PDU_t PDU to represent the current  */
  /* Play Status.                                                       */
#define AVRCP_PLAY_STATUS_STATUS_STOPPED                             (0x00)
#define AVRCP_PLAY_STATUS_STATUS_PLAYING                             (0x01)
#define AVRCP_PLAY_STATUS_STATUS_PAUSED                              (0x02)
#define AVRCP_PLAY_STATUS_STATUS_FWD_SEEK                            (0x03)
#define AVRCP_PLAY_STATUS_STATUS_REV_SEEK                            (0x04)
#define AVRCP_PLAY_STATUS_STATUS_ERROR                               (0xFF)

  /* The following constants are used with the SongLength and           */
  /* SongPosition members (respectively) of the                         */
  /* AVRCP_Get_Play_Status_Response_PDU_t PDU to represent that the     */
  /* individual feature is not supported.                               */
#define AVRCP_PLAY_STATUS_SONG_LENGTH_UNSUPPORTED                    (0xFFFFFFFF)
#define AVRCP_PLAY_STATUS_SONG_POSITION_UNSUPPORTED                  (0xFFFFFFFF)

   /* The following type definition defines the structure of a Register */
   /* Notification Command PDU (AVRCP Specific AV/C Command).           */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Register_Notification_Command_PDU_t
{
   AVRCP_Specific_AV_C_Command_Header_t AVRCP_Specific_Command_Header;
   Byte_t                               EventID;
   NonAlignedDWord_t                    PlaybackInterval;
} __PACKED_STRUCT_END__ AVRCP_Register_Notification_Command_PDU_t;

#define AVRCP_REGISTER_NOTIFICATION_COMMAND_PDU_SIZE                 (sizeof(AVRCP_Register_Notification_Command_PDU_t))

   /* The following constant represents the minimum allowable value (in */
   /* seconds) that can be specified for the PlaybackInterval member of */
   /* the AVRCP_Register_Notification_Command_PDU_t PDU.                */
#define AVRCP_NOTIFICATION_PLAYBACK_INTERVAL_MINIMUM_VALUE           (1)

   /* The following type definition defines the structure of a Register */
   /* Notification Response PDU (AVRCP Specific AV/C Response).         */
   /* * NOTE * This response is an empty response that should only be   */
   /*          used to send an error response.  All other responses     */
   /*          should use the explicitly defined Event Responses defined*/
   /*          below.                                                   */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Register_Notification_Response_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Response_Header;
   Byte_t                                EventID;
} __PACKED_STRUCT_END__ AVRCP_Register_Notification_Response_PDU_t;

#define AVRCP_REGISTER_NOTIFICATION_RESPONSE_PDU_SIZE                (sizeof(AVRCP_Register_Notification_Response_PDU_t))

   /* The following Responses are the explicitly defined responses for  */
   /* the AVRCP_Register_Notification_Command_PDU_t PDU.  These         */
   /* responses are different based on the Event ID that was registered.*/

   /* Event ID - EVENT_PLAYBACK_STATUS_CHANGED Register Notification    */
   /* Response PDU.                                                     */

   /* The following type definition defines the structure of a Register */
   /* Notification Response PDU (AVRCP Specific AV/C Response) for an   */
   /* Event Playback Status Changed.                                    */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Register_Notification_Playback_Status_Changed_Response_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Response_Header;
   Byte_t                                EventID;
   Byte_t                                PlayStatus;
} __PACKED_STRUCT_END__ AVRCP_Register_Notification_Playback_Status_Changed_Response_PDU_t;

#define AVRCP_REGISTER_NOTIFICATION_PLAYBACK_STATUS_CHANGED_RESPONSE_PDU_SIZE  (sizeof(AVRCP_Register_Notification_Playback_Status_Changed_Response_PDU_t))

   /* Event ID - EVENT_TRACK_CHANGED Register Notification Response PDU.*/

   /* The following type definition defines the structure of a Register */
   /* Notification Response PDU (AVRCP Specific AV/C Response) for an   */
   /* Event Track Changed.                                              */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Register_Notification_Track_Changed_Response_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Response_Header;
   Byte_t                                EventID;
   NonAlignedQWord_t                     Identifier;
} __PACKED_STRUCT_END__ AVRCP_Register_Notification_Track_Changed_Response_PDU_t;

#define AVRCP_REGISTER_NOTIFICATION_TRACK_CHANGED_RESPONSE_PDU_SIZE  (sizeof(AVRCP_Register_Notification_Track_Changed_Response_PDU_t))

   /* The following constant represents the value that is to be returned*/
   /* in the Identifier member of the                                   */
   /* AVRCP_Register_Notification_Track_Changed_Response_PDU_t PDU if   */
   /* there is no track selected.                                       */
#define AVRCP_TRACK_CHANGED_IDENTIFIER_NO_TRACK_SELECTED             (0xFFFFFFFFFFFFFFFF)

   /* Event ID - EVENT_TRACK_REACHED_END Register Notification Response */
   /* PDU.                                                              */

   /* The following type definition defines the structure of a Register */
   /* Notification Response PDU (AVRCP Specific AV/C Response) for an   */
   /* Event Track Reached End.                                          */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Register_Notification_Track_Reached_End_Response_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Response_Header;
   Byte_t                                EventID;
} __PACKED_STRUCT_END__ AVRCP_Register_Notification_Track_Reached_End_Response_PDU_t;

#define AVRCP_REGISTER_NOTIFICATION_TRACK_REACHED_END_RESPONSE_PDU_SIZE (sizeof(AVRCP_Register_Notification_Track_Reached_End_Response_PDU_t))

   /* Event ID - EVENT_TRACK_REACHED_START Register Notification        */
   /* Response PDU.                                                     */

   /* The following type definition defines the structure of a Register */
   /* Notification Response PDU (AVRCP Specific AV/C Response) for an   */
   /* Event Track Reached Start.                                        */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Register_Notification_Track_Reached_Start_Response_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Response_Header;
   Byte_t                                EventID;
} __PACKED_STRUCT_END__ AVRCP_Register_Notification_Track_Reached_Start_Response_PDU_t;

#define AVRCP_REGISTER_NOTIFICATION_TRACK_REACHED_START_RESPONSE_PDU_SIZE  (sizeof(AVRCP_Register_Notification_Track_Reached_Start_Response_PDU_t))

   /* Event ID - EVENT_PLAYBACK_POS_CHANGED Register Notification       */
   /* Response PDU.                                                     */

   /* The following type definition defines the structure of a Register */
   /* Notification Response PDU (AVRCP Specific AV/C Response) for an   */
   /* Event Playback Position Changed.                                  */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Register_Notification_Playback_Pos_Changed_Response_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Response_Header;
   Byte_t                                EventID;
   NonAlignedDWord_t                     PlaybackPosition;
} __PACKED_STRUCT_END__ AVRCP_Register_Notification_Playback_Pos_Changed_Response_PDU_t;

#define AVRCP_REGISTER_NOTIFICATION_PLAYBACK_POS_CHANGED_RESPONSE_PDU_SIZE (sizeof(AVRCP_Register_Notification_Playback_Pos_Changed_Response_PDU_t))

   /* The following constant represents the value that is to be returned*/
   /* in the Playback Position member of the                            */
   /* AVRCP_Register_Notification_Playback_Pos_Changed_Response_PDU_t   */
   /* PDU if there is no track selected.                                */
#define AVRCP_PLAYBACK_POS_CHANGED_POSITION_NO_TRACK_SELECTED        (0xFFFFFFFF)

   /* Event ID - EVENT_BATT_STATUS_CHANGED Register Notification        */
   /* Response PDU.                                                     */

   /* The following type definition defines the structure of a Register */
   /* Notification Response PDU (AVRCP Specific AV/C Response) for an   */
   /* Event Battery Status Changed.                                     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Register_Notification_Batt_Status_Changed_Response_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Response_Header;
   Byte_t                                EventID;
   Byte_t                                BatteryStatus;
} __PACKED_STRUCT_END__ AVRCP_Register_Notification_Batt_Status_Changed_Response_PDU_t;

#define AVRCP_REGISTER_NOTIFICATION_BATT_STATUS_CHANGED_RESPONSE_PDU_SIZE  (sizeof(AVRCP_Register_Notification_Batt_Status_Changed_Response_PDU_t))

   /* The following constants represent the defined values for the      */
   /* BatteryStatus member of the                                       */
   /* AVRCP_Register_Notification_Batt_Status_Changed_Response_PDU_t    */
   /* PDU.                                                              */
#define AVRCP_BATTERY_STATUS_CHANGED_NORMAL                          (0x00)
#define AVRCP_BATTERY_STATUS_CHANGED_WARNING                         (0x01)
#define AVRCP_BATTERY_STATUS_CHANGED_CRITICAL                        (0x02)
#define AVRCP_BATTERY_STATUS_CHANGED_EXTERNAL                        (0x03)
#define AVRCP_BATTERY_STATUS_CHANGED_FULL_CHARGE                     (0x04)

   /* Event ID - EVENT_SYSTEM_STATUS_CHANGED Register Notification      */
   /* Response PDU.                                                     */

   /* The following type definition defines the structure of a Register */
   /* Notification Response PDU (AVRCP Specific AV/C Response) for an   */
   /* Event System Status Changed.                                      */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Register_Notification_System_Status_Changed_Response_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Response_Header;
   Byte_t                                EventID;
   Byte_t                                SystemStatus;
} __PACKED_STRUCT_END__ AVRCP_Register_Notification_System_Status_Changed_Response_PDU_t;

#define AVRCP_REGISTER_NOTIFICATION_SYSTEM_STATUS_CHANGED_RESPONSE_PDU_SIZE   (sizeof(AVRCP_Register_Notification_System_Status_Changed_Response_PDU_t))

   /* The following constants represent the defined values for the      */
   /* SystemStatus member of the                                        */
   /* AVRCP_Register_Notification_System_Status_Changed_Response_PDU_t  */
   /* PDU.                                                              */
#define AVRCP_SYSTEM_STATUS_CHANGED_POWER_ON                         (0x00)
#define AVRCP_SYSTEM_STATUS_CHANGED_POWER_OFF                        (0x01)
#define AVRCP_SYSTEM_STATUS_CHANGED_POWER_UNPLUGGED                  (0x02)

   /* Event ID - EVENT_PLAYER_APPLICATION_SETTING_CHANGED Register      */
   /* Notification Response PDU.                                        */

   /* The following type definition defines the structure of a Register */
   /* Notification Response PDU (AVRCP Specific AV/C Response) for an   */
   /* Event Player Application Setting Changed.                         */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Register_Notification_Player_Setting_Changed_Response_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Response_Header;
   Byte_t                                EventID;
   Byte_t                                NumberAttributeValueIDs;
   AVRCP_Attribute_Value_ID_Pair_t       AttributeValueIDList[1];
} __PACKED_STRUCT_END__ AVRCP_Register_Notification_Player_Setting_Changed_Response_PDU_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an outgoing AVRCP Specific AV/C      */
   /* Register Notification Player Application Setting Changed Response */
   /* PDU.  The input parameter specifies the the number of AVRCP       */
   /* Specific AV/C Player Application Setting Attribute/Value Pairs    */
   /* that are part of the response (not the actual number of bytes).   */
   /* This MACRO calculates the total size required (in bytes) to hold  */
   /* the data PLUS the AVRCP Specific AV/C Register Notification Player*/
   /* Application Setting Response PDU Header.                          */
#define AVRCP_REGISTER_NOTIFICATION_PLAYER_SETTING_CHANGED_RESPONSE_PDU_SIZE(_x) (sizeof(AVRCP_Register_Notification_Player_Setting_Changed_Response_PDU_t) - AVRCP_ATTRIBUTE_VALUE_ID_PAIR_SIZE + ((unsigned int)(_x)*AVRCP_ATTRIBUTE_VALUE_ID_PAIR_SIZE))

   /* Event ID - EVENT_VOLUME_CHANGED Register Notification Response    */
   /* PDU.                                                              */

   /* The following type definition defines the structure of a Register */
   /* Notification Response PDU (AVRCP Specific AV/C Response) for an   */
   /* Event Volume Changed.                                             */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Register_Notification_Volume_Changed_Response_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Response_Header;
   Byte_t                                EventID;
   Byte_t                                AbsoluteVolume;
} __PACKED_STRUCT_END__ AVRCP_Register_Notification_Volume_Changed_Response_PDU_t;

#define AVRCP_REGISTER_NOTIFICATION_VOLUME_CHANGED_RESPONSE_PDU_SIZE (sizeof(AVRCP_Register_Notification_Volume_Changed_Response_PDU_t))

   /* Event ID - EVENT_ADDRESSED_PLAYER_CHANGED Register Notification   */
   /* Response PDU.                                                     */

   /* The following type definition defines the structure of a Register */
   /* Notification Response PDU (AVRCP Specific AV/C Response) for an   */
   /* Event Addressed Player Changed.                                   */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Register_Notification_Addressed_Player_Changed_Response_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Response_Header;
   Byte_t                                EventID;
   NonAlignedWord_t                      MediaPlayerID;
   NonAlignedWord_t                      UIDCounter;
} __PACKED_STRUCT_END__ AVRCP_Register_Notification_Addressed_Player_Changed_Response_PDU_t;

#define AVRCP_REGISTER_NOTIFICATION_ADDRESSED_PLAYER_CHANGED_RESPONSE_PDU_SIZE (sizeof(AVRCP_Register_Notification_Addressed_Player_Changed_Response_PDU_t))

   /* The following constants represent the minimum and maximum values  */
   /* that a UID Counter can have (respectively).                       */
#define AVRCP_UID_COUNTER_MINIMUM_VALUE                              (0x0001)
#define AVRCP_UID_COUNTER_MAXIMUM_VALUE                              (0xFFFF)

   /* Event ID - EVENT_NOTIFY_AVAILABLE_PLAYERS_CHANGED Register        */
   /* Notification Response PDU.                                        */

   /* The following type definition defines the structure of a Register */
   /* Notification Response PDU (AVRCP Specific AV/C Response) for an   */
   /* Event Notify Available Players Changed.                           */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Register_Notification_Notify_Available_Players_Changed_Response_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Response_Header;
   Byte_t                                EventID;
} __PACKED_STRUCT_END__ AVRCP_Register_Notification_Notify_Available_Players_Changed_Response_PDU_t;

#define AVRCP_REGISTER_NOTIFICATION_NOTIFY_AVAILABLE_PLAYERS_CHANGED_RESPONSE_PDU_SIZE (sizeof(AVRCP_Register_Notification_Notify_Available_Players_Changed_Response_PDU_t))

   /* Event ID - EVENT_UIDS_CHANGED Register Notification Response PDU. */

   /* The following type definition defines the structure of a Register */
   /* Notification Response PDU (AVRCP Specific AV/C Response) for an   */
   /* Event UIDs Changed.                                               */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Register_Notification_UIDs_Changed_Response_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Response_Header;
   Byte_t                                EventID;
   NonAlignedWord_t                      UIDCounter;
} __PACKED_STRUCT_END__ AVRCP_Register_Notification_UIDs_Changed_Response_PDU_t;

#define AVRCP_REGISTER_NOTIFICATION_UIDS_CHANGED_RESPONSE_PDU_SIZE   (sizeof(AVRCP_Register_Notification_UIDs_Changed_Response_PDU_t))

   /* Event ID - EVENT_NOW_PLAYING_CONTENT_CHANGED Register Notification*/
   /* Response PDU.                                                     */

   /* The following type definition defines the structure of a Register */
   /* Notification Response PDU (AVRCP Specific AV/C Response) for an   */
   /* Event Now Playing Changed.                                        */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Register_Notification_Now_Playing_Content_Changed_Response_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Response_Header;
   Byte_t                                EventID;
} __PACKED_STRUCT_END__ AVRCP_Register_Notification_Now_Playing_Content_Changed_Response_PDU_t;

#define AVRCP_REGISTER_NOTIFICATION_NOW_PLAYING_CONTENT_CHANGED_RESPONSE_PDU_SIZE   (sizeof(AVRCP_Register_Notification_Now_Playing_Content_Changed_Response_PDU_t))

   /* The following type definition defines the structure of a Request  */
   /* Continuing Response Command PDU (AVRCP Specific AV/C Command).    */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Request_Continuing_Response_Command_PDU_t
{
   AVRCP_Specific_AV_C_Command_Header_t AVRCP_Specific_Command_Header;
   Byte_t                               PDU_ID;
} __PACKED_STRUCT_END__ AVRCP_Request_Continuing_Response_Command_PDU_t;

#define AVRCP_REQUEST_CONTINUING_RESPONSE_COMMAND_PDU_SIZE           (sizeof(AVRCP_Request_Continuing_Response_Command_PDU_t))

   /* The following type definition defines the structure of a Abort    */
   /* Continuing Response Command PDU (AVRCP Specific AV/C Command).    */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Abort_Continuing_Response_Command_PDU_t
{
   AVRCP_Specific_AV_C_Command_Header_t AVRCP_Specific_Command_Header;
   Byte_t                               PDU_ID;
} __PACKED_STRUCT_END__ AVRCP_Abort_Continuing_Response_Command_PDU_t;

#define AVRCP_ABORT_CONTINUING_RESPONSE_COMMAND_PDU_SIZE             (sizeof(AVRCP_Abort_Continuing_Response_Command_PDU_t))

   /* The following type definition defines the structure of a Abort    */
   /* Continuing Response Command PDU (AVRCP Specific AV/C Command).    */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Abort_Continuing_Response_Response_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Response_Header;
} __PACKED_STRUCT_END__ AVRCP_Abort_Continuing_Response_Response_PDU_t;

#define AVRCP_ABORT_CONTINUING_RESPONSE_RESPONSE_PDU_SIZE            (sizeof(AVRCP_Abort_Continuing_Response_Response_PDU_t))

   /* The following type definition defines the structure of a Set      */
   /* Absolute Volume Command PDU (AVRCP Specific AV/C Command).        */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Set_Absolute_Volume_Command_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Command_Header;
   Byte_t                                AbsoluteVolume;
} __PACKED_STRUCT_END__ AVRCP_Set_Absolute_Volume_Command_PDU_t;

#define AVRCP_SET_ABSOLUTE_VOLUME_COMMAND_PDU_SIZE                   (sizeof(AVRCP_Set_Absolute_Volume_Command_PDU_t))

   /* The following type definition defines the structure of a Set      */
   /* Absolute Volume Response PDU (AVRCP Specific AV/C Response).      */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Set_Absolute_Volume_Response_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Response_Header;
   Byte_t                                AbsoluteVolume;
} __PACKED_STRUCT_END__ AVRCP_Set_Absolute_Volume_Response_PDU_t;

#define AVRCP_SET_ABSOLUTE_VOLUME_RESPONSE_PDU_SIZE                  (sizeof(AVRCP_Set_Absolute_Volume_Response_PDU_t))

   /* The following type definition defines the structure of a Set      */
   /* Addressed Player Command PDU (AVRCP Specific AV/C Command).       */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Set_Addressed_Player_Command_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Command_Header;
   NonAlignedWord_t                      MediaPlayerID;
} __PACKED_STRUCT_END__ AVRCP_Set_Addressed_Player_Command_PDU_t;

#define AVRCP_SET_ADDRESSED_PLAYER_COMMAND_PDU_SIZE                  (sizeof(AVRCP_Set_Addressed_Player_Command_PDU_t))

   /* The following type definition defines the structure of a Set      */
   /* Addressed Player Response PDU (AVRCP Specific AV/C Response).     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Set_Addressed_Player_Response_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Response_Header;
   Byte_t                                Status;
} __PACKED_STRUCT_END__ AVRCP_Set_Addressed_Player_Response_PDU_t;

#define AVRCP_SET_ADDRESSED_PLAYER_RESPONSE_PDU_SIZE                 (sizeof(AVRCP_Set_Addressed_Player_Response_PDU_t))

   /* The following type definition defines the structure of a Play Item*/
   /* Command PDU (AVRCP Specific AV/C Command).                        */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Play_Item_Command_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Command_Header;
   Byte_t                                Scope;
   NonAlignedQWord_t                     UID;
   NonAlignedWord_t                      UIDCounter;
} __PACKED_STRUCT_END__ AVRCP_Play_Item_Command_PDU_t;

#define AVRCP_PLAY_ITEM_COMMAND_PDU_SIZE                             (sizeof(AVRCP_Play_Item_Command_PDU_t))

   /* The following constants define the values of the AVRCP Media      */
   /* Content Navigation Scopes.                                        */
#define AVRCP_NAVIGATION_SCOPE_MEDIA_PLAYER_LIST                     (0x00)
#define AVRCP_NAVIGATION_SCOPE_MEDIA_PLAYER_VIRTUAL_FILESYSTEM       (0x01)
#define AVRCP_NAVIGATION_SCOPE_SEARCH                                (0x02)
#define AVRCP_NAVIGATION_SCOPE_NOW_PLAYING                           (0x03)

   /* The following type definition defines the structure of a Play Item*/
   /* Response PDU (AVRCP Specific AV/C Response).                      */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Play_Item_Response_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Response_Header;
   Byte_t                                Status;
} __PACKED_STRUCT_END__ AVRCP_Play_Item_Response_PDU_t;

#define AVRCP_PLAY_ITEM_RESPONSE_PDU_SIZE                            (sizeof(AVRCP_Play_Item_Response_PDU_t))

   /* The following constants define the valid AVRCP Command            */
   /* Error/Status Codes.                                               */
#define AVRCP_COMMAND_ERROR_STATUS_CODE_INVALID_COMMAND              (0x00)
#define AVRCP_COMMAND_ERROR_STATUS_CODE_INVALID_PARAMETER            (0x01)
#define AVRCP_COMMAND_ERROR_STATUS_CODE_PARAMETER_NOT_FOUND          (0x02)
#define AVRCP_COMMAND_ERROR_STATUS_CODE_INTERNAL_ERROR               (0x03)
#define AVRCP_COMMAND_ERROR_STATUS_CODE_COMPLETE_NO_ERROR            (0x04)
#define AVRCP_COMMAND_ERROR_STATUS_CODE_UID_CHANGED                  (0x05)
#define AVRCP_COMMAND_ERROR_STATUS_CODE_INVALID_DIRECTION            (0x07)
#define AVRCP_COMMAND_ERROR_STATUS_CODE_NOT_A_DIRECTORY              (0x08)
#define AVRCP_COMMAND_ERROR_STATUS_CODE_DOES_NOT_EXIST               (0x09)
#define AVRCP_COMMAND_ERROR_STATUS_CODE_INVALID_SCOPE                (0x0A)
#define AVRCP_COMMAND_ERROR_STATUS_CODE_RANGE_OUT_OF_BOUNDS          (0x0B)
#define AVRCP_COMMAND_ERROR_STATUS_CODE_UID_IS_A_DIRECTORY           (0x0C)
#define AVRCP_COMMAND_ERROR_STATUS_CODE_MEDIA_IN_USE                 (0x0D)
#define AVRCP_COMMAND_ERROR_STATUS_CODE_NOW_PLAYING_LIST_FULL        (0x0E)
#define AVRCP_COMMAND_ERROR_STATUS_CODE_SEARCH_NOT_SUPPORTED         (0x0F)
#define AVRCP_COMMAND_ERROR_STATUS_CODE_SEARCH_IN_PROGRESS           (0x10)
#define AVRCP_COMMAND_ERROR_STATUS_CODE_INVALID_PLAYER_ID            (0x11)
#define AVRCP_COMMAND_ERROR_STATUS_CODE_PLAYER_NOT_BROWSABLE         (0x12)
#define AVRCP_COMMAND_ERROR_STATUS_CODE_PLAYER_NOT_ADDRESSED         (0x13)
#define AVRCP_COMMAND_ERROR_STATUS_CODE_NO_VALID_SEARCH_RESULTS      (0x14)
#define AVRCP_COMMAND_ERROR_STATUS_CODE_NO_AVAILABLE_PLAYERS         (0x15)
#define AVRCP_COMMAND_ERROR_STATUS_CODE_ADDRESSED_PLAYER_CHANGED     (0x16)

   /* The following type definition defines the structure of an Add to  */
   /* Now Playing Command PDU (AVRCP Specific AV/C Command).            */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Add_To_Now_Playing_Command_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Command_Header;
   Byte_t                                Scope;
   NonAlignedQWord_t                     UID;
   NonAlignedWord_t                      UIDCounter;
} __PACKED_STRUCT_END__ AVRCP_Add_To_Now_Playing_Command_PDU_t;

#define AVRCP_ADD_TO_NOW_PLAYING_COMMAND_PDU_SIZE                    (sizeof(AVRCP_Add_To_Now_Playing_Command_PDU_t))

   /* The following type definition defines the structure of an Add to  */
   /* Now Playing Response PDU (AVRCP Specific AV/C Response).          */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Add_To_Now_Playing_Response_PDU_t
{
   AVRCP_Specific_AV_C_Response_Header_t AVRCP_Specific_Response_Header;
   Byte_t                                Status;
} __PACKED_STRUCT_END__ AVRCP_Add_To_Now_Playing_Response_PDU_t;

#define AVRCP_ADD_TO_NOW_PLAYING_RESPONSE_PDU_SIZE                   (sizeof(AVRCP_Add_To_Now_Playing_Response_PDU_t))

   /* The following definitions are for the Command/Response format for */
   /* the AVRCP Browsing Channel.                                       */

   /* The following structure defines the format of an AVRCP Specific   */
   /* Command or Response (Message) (AVRCP Specific AV/C command) Header*/
   /* (for the Browsing Channel).                                       */
   /* * NOTE * The Browsing Channel does not have any IEEE 1394 Header  */
   /*          Information.  It is simply the header defined below.     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Specific_AV_C_Browsing_Message_Header_t
{
   Byte_t           PDU_ID;
   NonAlignedWord_t ParameterLength;
} __PACKED_STRUCT_END__ AVRCP_Specific_AV_C_Browsing_Message_Header_t;

#define AVRCP_SPECIFIC_AV_C_BROWSING_MESSAGE_HEADER_SIZE             (sizeof(AVRCP_Specific_AV_C_Browsing_Message_Header_t))

   /* The following structure defines the format of an AVRCP Specific   */
   /* Command or Response (Message) (AVRCP Specific AV/C command)       */
   /* message for the Browsing Channel.                                 */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Specific_AV_C_Browsing_Message_t
{
   Byte_t           PDU_ID;
   NonAlignedWord_t ParameterLength;
   Byte_t           ParameterData[1];
} __PACKED_STRUCT_END__ AVRCP_Specific_AV_C_Browsing_Message_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an outgoing AVRCP Specific AV/C      */
   /* Browsing Message frame.  The input parameter specifies the length */
   /* actual AVRCP Specific AV/C Parameter bytes that are part of the   */
   /* message.  This MACRO calculates the total size required (in bytes)*/
   /* to hold the data PLUS the AVRCP Specific AV/C Browsing Message    */
   /* Header.                                                           */
#define AVRCP_CALCULATE_AVRCP_SPECIFIC_AV_C_BROWSING_MESSAGE_SIZE(_x)   (sizeof(AVRCP_Specific_AV_C_Browsing_Message_t) - BYTE_SIZE + (unsigned int)(_x))

   /* The following structure defines the format of an AVRCP Specific   */
   /* Command (AVRCP Specific AV/C command) Header (for the Browsing    */
   /* Channel).                                                         */
   /* * NOTE * The Browsing Channel does not have any IEEE 1394 Header  */
   /*          Information.  It is simply the header defined below.     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Specific_AV_C_Browsing_Command_Header_t
{
   Byte_t           PDU_ID;
   NonAlignedWord_t ParameterLength;
} __PACKED_STRUCT_END__ AVRCP_Specific_AV_C_Browsing_Command_Header_t;

#define AVRCP_SPECIFIC_AV_C_BROWSING_COMMAND_HEADER_SIZE             (sizeof(AVRCP_Specific_AV_C_Browsing_Command_Header_t))

   /* The following structure defines the format of an AVRCP Specific   */
   /* Command (AVRCP Specific AV/C command) message for the Browsing    */
   /* Channel.                                                          */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Specific_AV_C_Browsing_Command_t
{
   Byte_t           PDU_ID;
   NonAlignedWord_t ParameterLength;
   Byte_t           ParameterData[1];
} __PACKED_STRUCT_END__ AVRCP_Specific_AV_C_Browsing_Command_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an outgoing AVRCP Specific AV/C      */
   /* Browsing Command frame.  The input parameter specifies the length */
   /* actual AVRCP Specific AV/C Parameter bytes that are part of the   */
   /* command.  This MACRO calculates the total size required (in bytes)*/
   /* to hold the data PLUS the AVRCP Specific AV/C Browsing Command    */
   /* Header.                                                           */
#define AVRCP_CALCULATE_AVRCP_SPECIFIC_AV_C_BROWSING_COMMAND_SIZE(_x)   (sizeof(AVRCP_Specific_AV_C_Browsing_Command_t) - BYTE_SIZE + (unsigned int)(_x))

   /* The following structure defines the format of an AVRCP Specific   */
   /* Response (AVRCP Specific AV/C command) Header (for the Browsing   */
   /* Channel).                                                         */
   /* * NOTE * The Browsing Channel does not have any IEEE 1394 Header  */
   /*          Information.  It is simply the header defined below.     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Specific_AV_C_Browsing_Response_Header_t
{
   Byte_t           PDU_ID;
   NonAlignedWord_t ParameterLength;
} __PACKED_STRUCT_END__ AVRCP_Specific_AV_C_Browsing_Response_Header_t;

#define AVRCP_SPECIFIC_AV_C_BROWSING_RESPONSE_HEADER_SIZE            (sizeof(AVRCP_Specific_AV_C_Browsing_Response_Header_t))

   /* The following structure defines the format of an AVRCP Specific   */
   /* Command (AVRCP Specific AV/C command) message for the Browsing    */
   /* Channel.                                                          */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Specific_AV_C_Browsing_Response_t
{
   Byte_t           PDU_ID;
   NonAlignedWord_t ParameterLength;
   Byte_t           ParameterData[1];
} __PACKED_STRUCT_END__ AVRCP_Specific_AV_C_Browsing_Response_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an outgoing AVRCP Specific AV/C      */
   /* Browsing Response frame.  The input parameter specifies the length*/
   /* actual AVRCP Specific AV/C Parameter bytes that are part of the   */
   /* response.  This MACRO calculates the total size required (in      */
   /* bytes) to hold the data PLUS the AVRCP Specific AV/C Browsing     */
   /* Response Header.                                                  */
#define AVRCP_CALCULATE_AVRCP_SPECIFIC_AV_C_BROWSING_RESPONSE_SIZE(_x)  (sizeof(AVRCP_Specific_AV_C_Browsing_Response_t) - BYTE_SIZE + (unsigned int)(_x))

   /* Bluetooth AVRCP Specific AV/C Browsing Command/Response PDU type  */
   /* definitions/constants follow.                                     */

   /* The following type definition defines the structure of a Get      */
   /* Folder Items Command PDU (AVRCP Specific AV/C Browsing Command).  */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Get_Folder_Items_Command_PDU_t
{
   AVRCP_Specific_AV_C_Browsing_Command_Header_t AVRCP_Specific_AV_C_Browsing_Command_Header;
   Byte_t                                        Scope;
   NonAlignedDWord_t                             StartItem;
   NonAlignedDWord_t                             EndItem;
   Byte_t                                        AttributeCount;
   NonAlignedDWord_t                             AttributeList[1];
} __PACKED_STRUCT_END__ AVRCP_Get_Folder_Items_Command_PDU_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an outgoing AVRCP Specific AV/C Get  */
   /* Folder Items Command PDU.  The input parameter specifies the the  */
   /* number of Attributes that are part of the command (not the actual */
   /* number of bytes).  This MACRO calculates the total size required  */
   /* (in bytes) to hold the data PLUS the AVRCP Specific AV/C Get      */
   /* Folder Items Command PDU Header.                                  */
#define AVRCP_GET_FOLDER_ITEMS_COMMAND_PDU_SIZE(_x)                  (sizeof(AVRCP_Get_Folder_Items_Command_PDU_t) - DWORD_SIZE + ((unsigned int)(_x)*DWORD_SIZE))

   /* The following constants represent special handling values for the */
   /* AttributeCount member of the AVRCP_Get_Folder_Items_Command_PDU_t */
   /* PDU.                                                              */
#define AVRCP_GET_FOLDER_ITEMS_ATTRIBUTE_COUNT_ALL_ATTRIBUTES        (0x00)
#define AVRCP_GET_FOLDER_ITEMS_ATTRIBUTE_COUNT_NO_ATTRIBUTES         (0xFF)

   /* The following constants define the supported AVRCP Browsable Item */
   /* Types (used with Get Folder Item Command/Response PDU's).         */
#define AVRCP_BROWSABLE_ITEM_TYPE_MEDIA_PLAYER_ITEM                  (0x01)
#define AVRCP_BROWSABLE_ITEM_TYPE_FOLDER_ITEM                        (0x02)
#define AVRCP_BROWSABLE_ITEM_TYPE_MEDIA_ELEMENT_ITEM                 (0x03)

   /* The following type definition defines the structure of a Generic  */
   /* Get Folder Items Response PDU (AVRCP Specific AV/C Browsing       */
   /* Response).                                                        */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Get_Folder_Items_Response_PDU_t
{
   AVRCP_Specific_AV_C_Browsing_Response_Header_t AVRCP_Specific_AV_C_Browsing_Response_Header;
   Byte_t                                         Status;
   NonAlignedWord_t                               UIDCounter;
   NonAlignedWord_t                               NumberItems;
   Byte_t                                         ItemList[1];
} __PACKED_STRUCT_END__ AVRCP_Get_Folder_Items_Response_PDU_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an outgoing AVRCP Specific AV/C Get  */
   /* Folder Items Response PDU.  The input parameter specifies the the */
   /* number of actual bytes that are part of the response.  This MACRO */
   /* calculates the total size required (in bytes) to hold the data    */
   /* PLUS the AVRCP Specific AV/C Get Folder Items Response PDU Header.*/
#define AVRCP_GET_FOLDER_ITEMS_RESPONSE_PDU_SIZE(_x)                 (sizeof(AVRCP_Get_Folder_Items_Response_PDU_t) - BYTE_SIZE + (unsigned int)(_x))

   /* The following define the various formats of the Get Folder Items  */
   /* AVRCP Specific AV/C Browsing Responses.  The same command is used */
   /* for each, however, the response format depends on the Folder Item */
   /* Scope.                                                            */

   /* Get Folder Items for Media Player List AVRCP Specific AV/C        */
   /* Browsing Response.                                                */

   /* The following type declaration represents the structure of an     */
   /* AVRCP Medai Player Item Features Mask Type.                       */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Media_Player_Item_Features_Mask_t
{
   Byte_t Features0;
   Byte_t Features1;
   Byte_t Features2;
   Byte_t Features3;
   Byte_t Features4;
   Byte_t Features5;
   Byte_t Features6;
   Byte_t Features7;
   Byte_t Features8;
   Byte_t Features9;
   Byte_t Features10;
   Byte_t Features11;
   Byte_t Features12;
   Byte_t Features13;
   Byte_t Features14;
   Byte_t Features15;
} __PACKED_STRUCT_END__ AVRCP_Media_Player_Item_Features_Mask_t;

#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_MASK_SIZE                   (sizeof(AVRCP_Media_Player_Item_Features_Mask_t))

   /* The following MACRO is a utility MACRO that exists to assign the  */
   /* individual Byte values into the specified                         */
   /* AVRCP_Media_Player_Item_Features_Mask variable.  The Bytes are NOT*/
   /* listed in little Endian Format, however, they are assigned to the */
   /* AVRCP_Media_Player_Item_Feature_Mask Variable in Little Endian    */
   /* Format (as per specification).  The first parameter is the        */
   /* AVRCP_Media_Player_Item_Features_Mask_t Variable (of type         */
   /* AVRCP_Media_Player_Item_Features_Mask_t) to assign, and the next  */
   /* sixteen parameters are the Individual                             */
   /* AVRCP_Media_Player_Item_Features_Mask Byte values to assign to the*/
   /* variable.                                                         */
#define ASSIGN_MEDIA_PLAYER_ITEM_FEATURES(_dest, _a, _b, _c, _d, _e, _f, _g, _h, _i, _j, _k, _l, _m, _n, _o, _p) \
{                                                                                                                \
   (_dest).Features0  = (_a);                                                                                    \
   (_dest).Features1  = (_b);                                                                                    \
   (_dest).Features2  = (_c);                                                                                    \
   (_dest).Features3  = (_d);                                                                                    \
   (_dest).Features4  = (_e);                                                                                    \
   (_dest).Features5  = (_f);                                                                                    \
   (_dest).Features6  = (_g);                                                                                    \
   (_dest).Features7  = (_h);                                                                                    \
   (_dest).Features8  = (_i);                                                                                    \
   (_dest).Features9  = (_j);                                                                                    \
   (_dest).Features10 = (_k);                                                                                    \
   (_dest).Features11 = (_l);                                                                                    \
   (_dest).Features12 = (_m);                                                                                    \
   (_dest).Features13 = (_n);                                                                                    \
   (_dest).Features14 = (_o);                                                                                    \
   (_dest).Features15 = (_p);                                                                                    \
}

   /* The following MACRO is a utility MACRO that exists to aid in the  */
   /* Comparison of two AVRCP_Media_Player_Item_Features_Mask_t         */
   /* variables.  This MACRO only returns whether the two               */
   /* AVRCP_Media_Player_Item_Features_Mask_t variables are equal (MACRO*/
   /* returns boolean result) NOT less than/greater than.  The two      */
   /* parameters to this MACRO are both of type                         */
   /* AVRCP_Media_Player_Item_Features_Mask_t and represent the         */
   /* AVRCP_Media_Player_Item_Features_Mask_t variables to compare.     */
#define COMPARE_MEDIA_PLAYER_ITEM_FEATURES(_x, _y)                                                                         \
(                                                                                                                          \
   ((_x).Features0  == (_y).Features0)  && ((_x).Features1  == (_y).Features1)  && ((_x).Features2  == (_y).Features2)  && \
   ((_x).Features3  == (_y).Features3)  && ((_x).Features4  == (_y).Features4)  && ((_x).Features5  == (_y).Features5)  && \
   ((_x).Features6  == (_y).Features6)  && ((_x).Features7  == (_y).Features7)  && ((_x).Features8  == (_y).Features8)  && \
   ((_x).Features9  == (_y).Features9)  && ((_x).Features10 == (_y).Features10) && ((_x).Features11 == (_y).Features11) && \
   ((_x).Features12 == (_y).Features12) && ((_x).Features13 == (_y).Features13) && ((_x).Features14 == (_y).Features14) && \
   ((_x).Features15 == (_y).Features15)                                                                                    \
)

   /* The following constants represent the Individual Feature Mask Bit */
   /* Numbers that can be passed to the                                 */
   /* SET_FEATURES_BIT()/RESET_FEATURES_BIT()/TEST_FEATURES_BIT()       */
   /* MACRO's.                                                          */
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_SELECT_PASSTHROUGH_SUPPORTED        (0x00)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_UP_PASSTHROUGH_SUPPORTED            (0x01)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_DOWN_PASSTHROUGH_SUPPORTED          (0x02)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_LEFT_PASSTHROUGH_SUPPORTED          (0x03)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_RIGHT_PASSTHROUGH_SUPPORTED         (0x04)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_RIGHT_UP_PASSTHROUGH_SUPPORTED      (0x05)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_RIGHT_DOWN_PASSTHROUGH_SUPPORTED    (0x06)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_LEFT_UP_PASSTHROUGH_SUPPORTED       (0x07)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_LEFT_DOWN_PASSTHROUGH_SUPPORTED     (0x08)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_ROOT_MENU_PASSTHROUGH_SUPPORTED     (0x09)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_SETUP_MENU_PASSTHROUGH_SUPPORTED    (0x0A)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_CONTENTS_MENU_PASSTHROUGH_SUPPORTED (0x0B)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_FAVORITE_MENU_PASSTHROUGH_SUPPORTED (0x0C)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_EXIT_PASSTHROUGH_SUPPORTED          (0x0D)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_0_PASSTHROUGH_SUPPORTED             (0x0E)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_1_PASSTHROUGH_SUPPORTED             (0x0F)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_2_PASSTHROUGH_SUPPORTED             (0x10)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_3_PASSTHROUGH_SUPPORTED             (0x11)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_4_PASSTHROUGH_SUPPORTED             (0x12)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_5_PASSTHROUGH_SUPPORTED             (0x13)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_6_PASSTHROUGH_SUPPORTED             (0x14)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_7_PASSTHROUGH_SUPPORTED             (0x15)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_8_PASSTHROUGH_SUPPORTED             (0x16)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_9_PASSTHROUGH_SUPPORTED             (0x17)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_DOT_PASSTHROUGH_SUPPORTED           (0x18)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_ENTER_PASSTHROUGH_SUPPORTED         (0x19)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_CLEAR_PASSTHROUGH_SUPPORTED         (0x1A)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_CHANNEL_UP_PASSTHROUGH_SUPPORTED    (0x1B)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_CHANNEL_DOWN_PASSTHROUGH_SUPPORTED  (0x1C)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_PREV_CHANNEL_PASSTHROUGH_SUPPORTED  (0x1D)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_SOUND_SELECT_PASSTHROUGH_SUPPORTED  (0x1E)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_INPUT_SELECT_PASSTHROUGH_SUPPORTED  (0x1F)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_DISPLAY_INFO_PASSTHROUGH_SUPPORTED  (0x20)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_HELP_PASSTHROUGH_SUPPORTED          (0x21)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_PAGE_UP_PASSTHROUGH_SUPPORTED       (0x22)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_PAGE_DOWN_PASSTHROUGH_SUPPORTED     (0x23)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_POWER_PASSTHROUGH_SUPPORTED         (0x24)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_VOLUME_UP_PASSTHROUGH_SUPPORTED     (0x25)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_VOLUME_DOWN_PASSTHROUGH_SUPPORTED   (0x26)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_MUTE_PASSTHROUGH_SUPPORTED          (0x27)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_PLAY_PASSTHROUGH_SUPPORTED          (0x28)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_STOP_PASSTHROUGH_SUPPORTED          (0x29)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_PAUSE_PASSTHROUGH_SUPPORTED         (0x2A)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_RECORD_PASSTHROUGH_SUPPORTED        (0x2B)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_REWIND_PASSTHROUGH_SUPPORTED        (0x2C)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_FAST_FORWARD_PASSTHROUGH_SUPPORTED  (0x2D)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_EJECT_PASSTHROUGH_SUPPORTED         (0x2E)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_FORWARD_PASSTHROUGH_SUPPORTED       (0x2F)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_BACKWARD_PASSTHROUGH_SUPPORTED      (0x30)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_ANGLE_PASSTHROUGH_SUPPORTED         (0x31)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_SUBPICTURE_PASSTHROUGH_SUPPORTED    (0x32)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_F1_PASSTHROUGH_SUPPORTED            (0x33)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_F2_PASSTHROUGH_SUPPORTED            (0x34)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_F3_PASSTHROUGH_SUPPORTED            (0x35)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_F4_PASSTHROUGH_SUPPORTED            (0x36)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_F5_PASSTHROUGH_SUPPORTED            (0x37)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_VENDOR_UNIQUE_PASSTHROUGH_SUPPORTED (0x38)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_BASIC_GROUP_NAVIGATION_SUPPORTED    (0x39)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_ADVANCED_PLAYER_CONTROL_SUPPORTED   (0x3A)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_BROWSING_SUPPORTED                  (0x3B)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_SEARCHING_SUPPORTED                 (0x3C)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_ADD_TO_NOW_PLAYING_SUPPORTED        (0x3D)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_UIDS_UNIQUE_BROWSE_TREE             (0x3E)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_ONLY_BROWSABLE_WHEN_ADDRESSED       (0x3F)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_ONLY_SEARCHABLE_WHEN_ADDRESSED      (0x40)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_NOW_PLAYING_SUPPORTED               (0x41)
#define AVRCP_MEDIA_PLAYER_ITEM_FEATURES_BIT_UID_PERSISTENCY_SUPPORTED           (0x42)

   /* The following structure represents an individual entry that is    */
   /* returned in the Get Folder Items for Media Player List PDU (AVRCP */
   /* Specific AV/C Browsing Response).                                 */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Folder_Item_Media_Player_List_Entry_t
{
   Byte_t                                  ItemType;
   NonAlignedWord_t                        ItemLength;
   NonAlignedWord_t                        PlayerID;
   Byte_t                                  MajorPlayerType;
   NonAlignedDWord_t                       PlayerSubType;
   Byte_t                                  PlayStatus;
   AVRCP_Media_Player_Item_Features_Mask_t FeaturesMask;
   NonAlignedWord_t                        CharacterSetID;
   NonAlignedWord_t                        DisplayableNameLength;
   Byte_t                                  DisplayableName[1];
} __PACKED_STRUCT_END__ AVRCP_Folder_Item_Media_Player_List_Entry_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an AVRCP Folder Item for a Media     */
   /* Player List Entry.  The input parameter specifies the length (in  */
   /* bytes) of the Displayable Text Name of the Item.  This MACRO      */
   /* calculates the total size required (in bytes) to hold the         */
   /* Displayable Text Name PLUS the Folder Item Media Player List Entry*/
   /* Header.                                                           */
#define AVRCP_FOLDER_ITEM_MEDIA_PLAYER_LIST_ENTRY_SIZE(_x)           (sizeof(AVRCP_Folder_Item_Media_Player_List_Entry_t) - BYTE_SIZE + (unsigned int)(_x))

   /* The following constants represent the Bit Mask values for the     */
   /* Media Player Item Major Player Type Bits.                         */
#define AVRCP_MEDIA_PLAYER_ITEM_MAJOR_PLAYER_TYPE_AUDIO_BIT_MASK              (0x01)
#define AVRCP_MEDIA_PLAYER_ITEM_MAJOR_PLAYER_TYPE_VIDEO_BIT_MASK              (0x02)
#define AVRCP_MEDIA_PLAYER_ITEM_MAJOR_PLAYER_TYPE_BROADCASTING_AUDIO_BIT_MASK (0x04)
#define AVRCP_MEDIA_PLAYER_ITEM_MAJOR_PLAYER_TYPE_BROADCASTING_VIDEO_BIT_MASK (0x08)

   /* The following constants represent the Bit Mask values for the     */
   /* Media Player Item Player Sub Type Bits.                           */
#define AVRCP_MEDIA_PLAYER_ITEM_PLAYER_SUB_TYPE_AUDIO_BOOK_BIT_MASK            (0x01)
#define AVRCP_MEDIA_PLAYER_ITEM_PLAYER_SUB_TYPE_PODCAST_BIT_MASK               (0x02)

   /* The following constants represent the values for the Media Player */
   /* Item Play Status member.                                          */
#define AVRCP_MEDIA_PLAYER_ITEM_PLAY_STATUS_STOPPED                            (0x00)
#define AVRCP_MEDIA_PLAYER_ITEM_PLAY_STATUS_PLAYING                            (0x01)
#define AVRCP_MEDIA_PLAYER_ITEM_PLAY_STATUS_PAUSED                             (0x02)
#define AVRCP_MEDIA_PLAYER_ITEM_PLAY_STATUS_FWD_SEEK                           (0x03)
#define AVRCP_MEDIA_PLAYER_ITEM_PLAY_STATUS_REV_SEEK                           (0x04)
#define AVRCP_MEDIA_PLAYER_ITEM_PLAY_STATUS_ERROR                              (0xFF)

   /* The following type definition defines the structure of a Get      */
   /* Folder Items for Media Play List Response PDU (AVRCP Specific AV/C*/
   /* Browsing Response).                                               */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Get_Folder_Items_Media_Player_List_Response_PDU_t
{
   AVRCP_Specific_AV_C_Browsing_Response_Header_t AVRCP_Specific_AV_C_Browsing_Response_Header;
   Byte_t                                         Status;
   NonAlignedWord_t                               UIDCounter;
   NonAlignedWord_t                               NumberItems;
   AVRCP_Folder_Item_Media_Player_List_Entry_t    ItemList[1];
} __PACKED_STRUCT_END__ AVRCP_Get_Folder_Items_Media_Player_List_Response_PDU_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an outgoing AVRCP Specific AV/C Get  */
   /* Folder Items for Media Play List Response PDU.  The input         */
   /* parameter specifies the the number of Attributes that are part of */
   /* the response (not the actual number of bytes).  This MACRO        */
   /* calculates the total size required (in bytes) to hold the data    */
   /* PLUS the AVRCP Specific AV/C Get Folder Items for Media Play List */
   /* Response PDU Header.                                              */
#define AVRCP_GET_FOLDER_ITEMS_MEDIA_PLAYER_LIST_RESPONSE_PDU_SIZE(_x)  (sizeof(AVRCP_Get_Folder_Items_Media_Player_List_Response_PDU_t) - AVRCP_FOLDER_ITEM_MEDIA_PLAYER_LIST_ENTRY_SIZE(1) + AVRCP_FOLDER_ITEM_MEDIA_PLAYER_LIST_ENTRY_SIZE((unsigned int)_x))

   /* Get Folder Items for File System AVRCP Specific AV/C Browsing     */
   /* Response.                                                         */

   /* The following structure represents an individual entry that is    */
   /* returned in the Get Folder Items for File System PDU (AVRCP       */
   /* Specific AV/C Browsing Response).                                 */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Folder_Item_File_System_Entry_t
{
   Byte_t               ItemType;
   NonAlignedWord_t     ItemLength;
   NonAlignedQWord_t    FolderUID;
   Byte_t               FolderType;
   Byte_t               IsPlayable;
   NonAlignedWord_t     CharacterSetID;
   NonAlignedWord_t     DisplayableNameLength;
   Byte_t               DisplayableName[1];
} __PACKED_STRUCT_END__ AVRCP_Folder_Item_File_System_Entry_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an AVRCP Folder Item for a File      */
   /* System Entry.  The input parameter specifies the length (in bytes)*/
   /* of the Displayable Text Name of the Item.  This MACRO calculates  */
   /* the total size required (in bytes) to hold the Displayable Text   */
   /* Name PLUS the Folder Item File System Entry Header.               */
#define AVRCP_FOLDER_ITEM_FILE_SYSTEM_ENTRY_SIZE(_x)                 (sizeof(AVRCP_Folder_Item_File_System_Entry_t) - BYTE_SIZE + (unsigned int)(_x))

   /* The following constants represent the defined values that are     */
   /* applicable for the FolderType member of the                       */
   /* AVRCP_Folder_Item_File_System_Entry_t structure.                  */
#define AVRCP_FOLDER_ITEM_FILE_SYSTEM_ENTRY_FOLDER_TYPE_MIXED        (0x00)
#define AVRCP_FOLDER_ITEM_FILE_SYSTEM_ENTRY_FOLDER_TYPE_TITLES       (0x01)
#define AVRCP_FOLDER_ITEM_FILE_SYSTEM_ENTRY_FOLDER_TYPE_ALBUMS       (0x02)
#define AVRCP_FOLDER_ITEM_FILE_SYSTEM_ENTRY_FOLDER_TYPE_ARTISTS      (0x03)
#define AVRCP_FOLDER_ITEM_FILE_SYSTEM_ENTRY_FOLDER_TYPE_GENRES       (0x04)
#define AVRCP_FOLDER_ITEM_FILE_SYSTEM_ENTRY_FOLDER_TYPE_PLAYLISTS    (0x05)
#define AVRCP_FOLDER_ITEM_FILE_SYSTEM_ENTRY_FOLDER_TYPE_YEARS        (0x06)

   /* The following constants represent the defined values that are     */
   /* applicable for the IsPlayable member of the                       */
   /* AVRCP_Folder_Item_File_System_Entry_t structure.                  */
#define AVRCP_FOLDER_ITEM_FILE_SYSTEM_ENTRY_IS_PLAYABLE_NO           (0x00)
#define AVRCP_FOLDER_ITEM_FILE_SYSTEM_ENTRY_IS_PLAYABLE_YES          (0x01)

   /* Get Folder Items for File System/Media Elements AVRCP Specific    */
   /* AV/C Browsing Response.                                           */

   /* The following type defines the structure of an individual entry in*/
   /* a Media Element Attribute Value List (used when Getting Folder    */
   /* Listing of Media Items).                                          */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Media_Element_Attribute_Value_Entry_t
{
   NonAlignedDWord_t AttributeID;
   NonAlignedWord_t  CharacterSetID;
   NonAlignedWord_t  AttributeValueLength;
   Byte_t            AttributeValue[1];
} __PACKED_STRUCT_END__ AVRCP_Media_Element_Attribute_Value_Entry_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an AVRCP Media Element Attribute     */
   /* Value Entry.  The input parameter specifies the length (in bytes) */
   /* of the entire Text Attribute Value of the attribute.  This MACRO  */
   /* calculates the total size required (in bytes) to hold the Text    */
   /* Attribute Value Name PLUS the Media Element Attribute Value Entry */
   /* Header.                                                           */
#define AVRCP_MEDIA_ELEMENT_ATTRIBUTE_VALUE_ENTRY_SIZE(_x)           (sizeof(AVRCP_Media_Element_Attribute_Value_Entry_t) - BYTE_SIZE + (unsigned int)(_x))

   /* The following type defines the structure of a Media Element       */
   /* Attribute Value List (used when Getting Folder Listing of Media   */
   /* Items).                                                           */
   /* * NOTE * AttributeValueData contains an array of variable length  */
   /*          AVRCP_Media_Element_Attribute_Value_Entry_t members.     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Media_Element_Attribute_Value_List_t
{
   Byte_t NumberAttributes;
   Byte_t AttributeValueData[1];
} __PACKED_STRUCT_END__ AVRCP_Media_Element_Attribute_Value_List_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an AVRCP Media Element Attribute     */
   /* Value List.  The input parameter specifies the length (in bytes)  */
   /* of the entire Text Attribute Value List of the all Attributes.    */
   /* This MACRO calculates the total size required (in bytes) to hold  */
   /* the all Attribute Value Entries PLUS the Media Element Attribute  */
   /* Value List Header.                                                */
#define AVRCP_MEDIA_ELEMENT_ATTRIBUTE_VALUE_LIST_SIZE(_x)            (sizeof(AVRCP_Media_Element_Attribute_Value_List_t) - BYTE_SIZE + (unsigned int)(_x))

   /* The following structure represents an individual entry that is    */
   /* returned in the Get Folder Items for Media Elements PDUs (AVRCP   */
   /* Specific AV/C Browsing Response for Search Result List and Now    */
   /* Playing List).                                                    */
   /* * NOTE * The final member, VariableData, contains the following:  */
   /*             - Displayable Text Name (variable length bytes)       */
   /*             - Media Element Attribute Value List (also variable   */
   /*               length bytes)                                       */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Folder_Item_Media_Element_Entry_t
{
   Byte_t               ItemType;
   NonAlignedWord_t     ItemLength;
   NonAlignedQWord_t    UID;
   Byte_t               MediaType;
   NonAlignedWord_t     CharacterSetID;
   NonAlignedWord_t     DisplayableNameLength;
   Byte_t               VariableData[1];
} __PACKED_STRUCT_END__ AVRCP_Folder_Item_Media_Element_Entry_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an AVRCP Folder Item for a Media     */
   /* Element Entry.  The input parameter specifies the length (in      */
   /* bytes) of the Displayable Text Name AND the Attribute Value List  */
   /* of the Item.  This MACRO calculates the total size required (in   */
   /* bytes) to hold the Displayable Text Name and Attribute Value List */
   /* PLUS the Folder Item Media Element Entry Header.                  */
#define AVRCP_FOLDER_ITEM_MEDIA_ELEMENT_ENTRY_SIZE(_x)               (sizeof(AVRCP_Folder_Item_Media_Element_Entry_t) - BYTE_SIZE + (unsigned int)(_x))

   /* The following constants represent the defined values that are     */
   /* applicable for the MediaType member of the                        */
   /* AVRCP_Folder_Item_Media_Element_Entry_t structure.                */
#define AVRCP_FOLDER_ITEM_MEDIA_ELEMENT_MEDIA_TYPE_AUDIO             (0x00)
#define AVRCP_FOLDER_ITEM_MEDIA_ELEMENT_MEDIA_TYPE_VIDEO             (0x01)

   /* The following type definition defines the structure of a Get      */
   /* Folder Items for File System Response PDU (AVRCP Specific AV/C    */
   /* Browsing Response).                                               */
   /* * NOTE * ItemList contains zero or more of the following items:   */
   /*             - AVRCP_Folder_Item_File_System_Entry_t               */
   /*             - AVRCP_Folder_Item_Media_Element_Entry_t             */
   /*          Both of which are variable length items.                 */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Get_Folder_Items_File_System_Response_PDU_t
{
   AVRCP_Specific_AV_C_Browsing_Response_Header_t AVRCP_Specific_AV_C_Browsing_Response_Header;
   Byte_t                                         Status;
   NonAlignedWord_t                               UIDCounter;
   NonAlignedWord_t                               NumberItems;
   Byte_t                                         ItemList[1];
} __PACKED_STRUCT_END__ AVRCP_Get_Folder_Items_File_System_Response_PDU_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an outgoing AVRCP Specific AV/C Get  */
   /* Folder Items for File System Response PDU.  The input parameter   */
   /* specifies the the total number Item List bytes that are part of   */
   /* the response.  This MACRO calculates the total size required (in  */
   /* bytes) to hold the data PLUS the AVRCP Specific AV/C Get Folder   */
   /* Items for File System Response PDU Header.                        */
#define AVRCP_GET_FOLDER_ITEMS_FILE_SYSTEM_RESPONSE_PDU_SIZE(_x)     (sizeof(AVRCP_Get_Folder_Items_File_System_Response_PDU_t) - BYTE_SIZE + ((unsigned int)_x))

   /* The following type definition defines the structure of a Get      */
   /* Folder Items for Search Result List Response PDU (AVRCP Specific  */
   /* AV/C Browsing Response).                                          */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Get_Folder_Items_Search_Result_List_Response_PDU_t
{
   AVRCP_Specific_AV_C_Browsing_Response_Header_t AVRCP_Specific_AV_C_Browsing_Response_Header;
   Byte_t                                         Status;
   NonAlignedWord_t                               UIDCounter;
   NonAlignedWord_t                               NumberItems;
   Byte_t                                         ItemListData[1];
} __PACKED_STRUCT_END__ AVRCP_Get_Folder_Items_Search_Result_List_Response_PDU_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an outgoing AVRCP Specific AV/C Get  */
   /* Folder Items for Search Result List Response PDU.  The input      */
   /* parameter specifies the the number of actual Item List bytes of   */
   /* the response.  This MACRO calculates the total size required (in  */
   /* bytes) to hold the Item List data PLUS the AVRCP Specific AV/C Get*/
   /* Folder Items for Search Result List Response PDU Header.          */
   /* * NOTE * The Item Data List contains elements of type:            */
   /*          AVRCP_Folder_Item_Media_Element_Entry_t.  Note that each */
   /*          of those elements is variable length.                    */
#define AVRCP_GET_FOLDER_ITEMS_SEARCH_RESULT_LIST_RESPONSE_PDU_SIZE(_x) (sizeof(AVRCP_Get_Folder_Items_Search_Result_List_Response_PDU_t) - BYTE_SIZE + ((unsigned int)_x))

   /* The following type definition defines the structure of a Get      */
   /* Folder Items for Now Playing List Response PDU (AVRCP Specific    */
   /* AV/C Browsing Response).                                          */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Get_Folder_Items_Now_Playing_List_Response_PDU_t
{
   AVRCP_Specific_AV_C_Browsing_Response_Header_t AVRCP_Specific_AV_C_Browsing_Response_Header;
   Byte_t                                         Status;
   NonAlignedWord_t                               UIDCounter;
   NonAlignedWord_t                               NumberItems;
   Byte_t                                         ItemListData[1];
} __PACKED_STRUCT_END__ AVRCP_Get_Folder_Items_Now_Playing_List_Response_PDU_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an outgoing AVRCP Specific AV/C Get  */
   /* Folder Items for Now Playing List Response PDU.  The input        */
   /* parameter specifies the the number of actual Item List bytes of   */
   /* the response.  This MACRO calculates the total size required (in  */
   /* bytes) to hold the Item List data PLUS the AVRCP Specific AV/C Get*/
   /* Folder Items for Now Playing List Response PDU Header.            */
   /* * NOTE * The Item Data List contains elements of type:            */
   /*          AVRCP_Folder_Item_Media_Element_Entry_t.  Note that each */
   /*          of those elements is variable length.                    */
#define AVRCP_GET_FOLDER_ITEMS_NOW_PLAYING_LIST_RESPONSE_PDU_SIZE(_x)   (sizeof(AVRCP_Get_Folder_Items_Now_Playing_List_Response_PDU_t) - BYTE_SIZE + ((unsigned int)_x))

   /* The following type definition defines the structure of a Set      */
   /* Browsed Player Command PDU (AVRCP Specific AV/C Browsing Command).*/
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Set_Browsed_Player_Command_PDU_t
{
   AVRCP_Specific_AV_C_Browsing_Command_Header_t AVRCP_Specific_AV_C_Browsing_Command_Header;
   NonAlignedWord_t                              PlayerID;
} __PACKED_STRUCT_END__ AVRCP_Set_Browsed_Player_Command_PDU_t;

#define AVRCP_SET_BROWSED_PLAYER_COMMAND_PDU_SIZE                    (sizeof(AVRCP_Set_Browsed_Player_Command_PDU_t))

   /* The following type definition defines an individual Folder Name   */
   /* entry that is present in the FolderName data portion of the       */
   /* AVRCP_Set_Browsed_Player_Response_PDU_t PDU.                      */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Folder_Name_Entry_t
{
   NonAlignedWord_t FolderNameLength;
   Byte_t           FolderName[1];
} __PACKED_STRUCT_END__ AVRCP_Folder_Name_Entry_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an individual AVRCP Folder Name      */
   /* Entry.  The input parameter specifies the length (in bytes) of the*/
   /* Folder Name itself.  This MACRO calculates the total size required*/
   /* (in bytes) to hold the Folder Name PLUS the Folder Name Length.   */
#define AVRCP_FOLDER_NAME_ENTRY_SIZE(_x)                             (sizeof(AVRCP_Folder_Name_Entry_t) - BYTE_SIZE + (unsigned int)(_x))

   /* The following type definition defines the structure of a Set      */
   /* Browsed Player Response PDU (AVRCP Specific AV/C Browsing         */
   /* Response).                                                        */
   /* * NOTE * VariableData contains zero or more                       */
   /*          AVRCP_Folder_Name_Entry_t structures which are also      */
   /*          variable length structures.                              */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Set_Browsed_Player_Response_PDU_t
{
   AVRCP_Specific_AV_C_Browsing_Response_Header_t AVRCP_Specific_AV_C_Browsing_Response_Header;
   Byte_t                                         Status;
   NonAlignedWord_t                               UIDCounter;
   NonAlignedDWord_t                              NumberItems;
   NonAlignedWord_t                               CharacterSetID;
   Byte_t                                         FolderDepth;
   Byte_t                                         VariableData[1];
} __PACKED_STRUCT_END__ AVRCP_Set_Browsed_Player_Response_PDU_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an AVRCP Set Browsed Player Response */
   /* PDU.  The input parameter specifies the length (in bytes) of all  */
   /* of the Folder Names.  This MACRO calculates the total size        */
   /* required (in bytes) to hold all of the Folder Names PLUS the Set  */
   /* Browsed Player Response PDU Header.                               */
#define AVRCP_SET_BROWSED_PLAYER_RESPONSE_PDU_SIZE(_x)               (sizeof(AVRCP_Set_Browsed_Player_Response_PDU_t) - BYTE_SIZE + (unsigned int)(_x))

   /* The following type definition defines the structure of a Change   */
   /* Path Command PDU (AVRCP Specific AV/C Browsing Command).          */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Change_Path_Command_PDU_t
{
   AVRCP_Specific_AV_C_Browsing_Command_Header_t AVRCP_Specific_AV_C_Browsing_Command_Header;
   NonAlignedWord_t                              UIDCounter;
   Byte_t                                        Direction;
   NonAlignedQWord_t                             FolderUID;
} __PACKED_STRUCT_END__ AVRCP_Change_Path_Command_PDU_t;

#define AVRCP_CHANGE_PATH_COMMAND_PDU_SIZE                           (sizeof(AVRCP_Change_Path_Command_PDU_t))

   /* The following type definition defines the structure of a Change   */
   /* Path Response PDU (AVRCP Specific AV/C Browsing Response).        */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Change_Path_Response_PDU_t
{
   AVRCP_Specific_AV_C_Browsing_Response_Header_t AVRCP_Specific_AV_C_Browsing_Response_Header;
   Byte_t                                         Status;
   NonAlignedDWord_t                              NumberItems;
} __PACKED_STRUCT_END__ AVRCP_Change_Path_Response_PDU_t;

#define AVRCP_CHANGE_PATH_RESPONSE_PDU_SIZE                          (sizeof(AVRCP_Change_Path_Response_PDU_t))

   /* The following type definition defines the structure of a Get Item */
   /* Attributes Command PDU (AVRCP Specific AV/C Browsing Command).    */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Get_Item_Attributes_Command_PDU_t
{
   AVRCP_Specific_AV_C_Browsing_Command_Header_t AVRCP_Specific_AV_C_Browsing_Command_Header;
   Byte_t                                        Scope;
   NonAlignedQWord_t                             UID;
   NonAlignedWord_t                              UIDCounter;
   Byte_t                                        NumberAttributes;
   NonAlignedDWord_t                             AttributeList[1];
} __PACKED_STRUCT_END__ AVRCP_Get_Item_Attributes_Command_PDU_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an outgoing AVRCP Specific AV/C Get  */
   /* Item Attributes Command PDU.  The input parameter specifies the   */
   /* the number of Attributes that are part of the command (not the    */
   /* actual number of bytes).  This MACRO calculates the total size    */
   /* required (in bytes) to hold the data PLUS the AVRCP Specific AV/C */
   /* Get Item Attributes Command PDU Header.                           */
#define AVRCP_GET_ITEM_ATTRIBUTES_COMMAND_PDU_SIZE(_x)               (sizeof(AVRCP_Get_Item_Attributes_Command_PDU_t) - DWORD_SIZE + ((unsigned int)(_x)*DWORD_SIZE))

   /* The following type definition defines the structure of a Get Item */
   /* Attributes Response PDU (AVRCP Specific AV/C Browsing Response).  */
   /* * NOTE * AttributeValueData contains an array of variable length  */
   /*          AVRCP_Media_Element_Attribute_Value_Entry_t members.     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Get_Item_Attributes_Response_PDU_t
{
   AVRCP_Specific_AV_C_Browsing_Response_Header_t AVRCP_Specific_AV_C_Browsing_Response_Header;
   Byte_t                                         Status;
   Byte_t                                         NumberAttributes;
   Byte_t                                         AttributeValueData[1];
} __PACKED_STRUCT_END__ AVRCP_Get_Item_Attributes_Response_PDU_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an AVRCP Get Item Attributes Response*/
   /* PDU.  The input parameter specifies the length (in bytes) of the  */
   /* entire Attribute Value List of the all Attributes.  This MACRO    */
   /* calculates the total size required (in bytes) to hold the all     */
   /* Attribute Value Entries PLUS the Get Item Attributes Response PDU */
   /* Header.                                                           */
#define AVRCP_GET_ITEM_ATTRIBUTES_RESPONSE_PDU_SIZE(_x)              (sizeof(AVRCP_Get_Item_Attributes_Response_PDU_t) - BYTE_SIZE + (unsigned int)(_x))

   /* The following type definition defines the structure of a Search   */
   /* Command PDU (AVRCP Specific AV/C Browsing Command).               */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Get_Search_Command_PDU_t
{
   AVRCP_Specific_AV_C_Browsing_Command_Header_t AVRCP_Specific_AV_C_Browsing_Command_Header;
   NonAlignedWord_t                              CharacterSetID;
   NonAlignedWord_t                              Length;
   Byte_t                                        SearchString[1];
} __PACKED_STRUCT_END__ AVRCP_Search_Command_PDU_t;

   /* The following is a utility MACRO provided to determine how much   */
   /* memory is required to format an outgoing AVRCP Specific AV/C      */
   /* Search Command PDU.  The input parameter specifies the length of  */
   /* the Search String (in bytes).  This MACRO calculates the total    */
   /* size required (in bytes) to hold the Search String PLUS the AVRCP */
   /* Specific AV/C Search Command PDU Header.                          */
#define AVRCP_SEARCH_COMMAND_PDU_SIZE(_x)                            (sizeof(AVRCP_Search_Command_PDU_t) - BYTE_SIZE + ((unsigned int)(_x)))

   /* The following constants define the currently supported Search AV/C*/
   /* Browsing Command character sets.                                  */
#define AVRCP_SEARCH_CHARACTER_SET_ID_UTF_8                          (0x006A)

   /* The following type definition defines the structure of a Search   */
   /* Response PDU (AVRCP Specific AV/C Browsing Response).             */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_Search_Response_PDU_t
{
   AVRCP_Specific_AV_C_Browsing_Response_Header_t AVRCP_Specific_AV_C_Browsing_Response_Header;
   Byte_t                                         Status;
   NonAlignedWord_t                               UIDCounter;
   NonAlignedDWord_t                              NumberItems;
} __PACKED_STRUCT_END__ AVRCP_Search_Response_PDU_t;

#define AVRCP_SEARCH_RESPONSE_PDU_SIZE                               (sizeof(AVRCP_Search_Response_PDU_t))

   /* The following type definition defines the structure of a General  */
   /* Reject Response PDU (AVRCP Specific AV/C Browsing Response).      */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVRCP_General_Reject_Response_PDU_t
{
   AVRCP_Specific_AV_C_Browsing_Response_Header_t AVRCP_Specific_AV_C_Browsing_Response_Header;
   Byte_t                                         RejectReason;
} __PACKED_STRUCT_END__ AVRCP_General_Reject_Response_PDU_t;

#define AVRCP_GENERAL_REJECT_RESPONSE_PDU_SIZE                       (sizeof(AVRCP_General_Reject_Response_PDU_t))

#endif
