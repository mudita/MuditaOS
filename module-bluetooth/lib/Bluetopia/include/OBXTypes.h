/*****< obxtypes.h >***********************************************************/
/*      Copyright 2000 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  OBXTYPES - Bluetooth OBEX Type Definitions/Constants.                     */
/*                                                                            */
/*  Author:  Damon Lange                                                      */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   09/11/00  D. Lange       Initial creation (in another file).             */
/*   06/11/09  D. Lange       Moved into seperate file.                       */
/******************************************************************************/
#ifndef __OBXTYPESH__
#define __OBXTYPESH__

#include "BTTypes.h"            /* Bluetooth Type Definitions.                */

   /* Supported version of OBEX that these types support.               */
#define OBEX_VERSION_1_0                                                0x10

   /* The following defines Bit Mask used with the OBEX Command Opcode  */
   /* and the OBEX Response Codes.  The Command and Response codes are  */
   /* contained in the lower 7 bits of the Command/Response values.  The*/
   /* most significant bit of the value contains the Final Bit.  The    */
   /* Final bit is used to denote the Last Command/Response of a request*/
   /* that may span multiple OBEX packets.                              */
#define OBEX_FINAL_BIT                                                  0x80
#define OBEX_OPCODE_RESPONSE_BIT_MASK                                   0x7F

   /* The following define the possible OBEX Command Opcodes.           */
#define OBEX_CONNECT_COMMAND                                            0x00
#define OBEX_DISCONNECT_COMMAND                                         0x01
#define OBEX_PUT_COMMAND                                                0x02
#define OBEX_GET_COMMAND                                                0x03
#define OBEX_SET_PATH_COMMAND                                           0x05
#define OBEX_RESERVED_COMMAND_START                                     0x06
#define OBEX_RESERVED_COMMAND_END                                       0x0F
#define OBEX_RESERVED_USER_DEFINED_START                                0x10
#define OBEX_RESERVED_USER_DEFINED_END                                  0x1F
#define OBEX_ABORT_COMMAND                                              0x7F

   /* The following defines the OBEX Request Packet Header format.      */
typedef __PACKED_STRUCT_BEGIN__ struct _tagOBEX_Request_Packet_Header_t
{
   Byte_t OpCode;
   Word_t PacketLength;
} __PACKED_STRUCT_END__ OBEX_Request_Packet_Header_t;

#define OBEX_REQUEST_PACKET_HEADER_DATA_SIZE             (sizeof(OBEX_Request_Packet_Header_t))

   /* The following defines the OBEX Response Packet Header format.     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagOBEX_Response_Packet_Header_t
{
   Byte_t ResponseCode;
   Word_t PacketLength;
} __PACKED_STRUCT_END__ OBEX_Response_Packet_Header_t;

#define OBEX_RESPONSE_PACKET_HEADER_DATA_SIZE            (sizeof(OBEX_Response_Packet_Header_t))

   /* The following define the possible Flags for a Set Path Action.    */
#define OBEX_SET_PATH_FLAGS_BACKUP_MASK                                 0x01
#define OBEX_SET_PATH_FLAGS_NO_CREATE_MASK                              0x02

   /* The following define the possible OBEX Connection Flags values    */
   /* for the OBEX_Connect_Confirmation_Data_t structure.               */
#define OBEX_CONNECTION_FLAGS_RESPONSE_MULTIPLE_IRLMP_CONNECTIONS       0x01

   /* The following define the possible OBEX Response codes.            */
   /* * NOTE * Response Codes less than 0x10 (OBEX_CONTINUE_RESPONSE)   */
   /*          are Reserved and CANNOT be used.                         */
#define OBEX_CONTINUE_RESPONSE                                          0x10
#define OBEX_OK_RESPONSE                                                0x20
#define OBEX_CREATED_RESPONSE                                           0x21
#define OBEX_ACCEPTED_RESPONSE                                          0x22
#define OBEX_NON_AUTHORITATIVE_INFORMATION_RESPONSE                     0x23
#define OBEX_NO_CONTENT_RESPONSE                                        0x24
#define OBEX_RESET_CONTENT_RESPONSE                                     0x25
#define OBEX_PARTIAL_CONTENT_RESPONSE                                   0x26
#define OBEX_MULTIPLE_CHOICES_RESPONSE                                  0x30
#define OBEX_MOVED_PERMANETLY_RESPONSE                                  0x31
#define OBEX_MOVED_TEMPORARILY_RESPONSE                                 0x32
#define OBEX_SEE_OTHER_RESPONSE                                         0x33
#define OBEX_NOT_MODIFIED_RESPONSE                                      0x34
#define OBEX_USE_PROXY_RESPONSE                                         0x35
#define OBEX_BAD_REQUEST_RESPONSE                                       0x40
#define OBEX_UNAUTHORIZED_RESPONSE                                      0x41
#define OBEX_PAYMENT_REQUIRED_RESPONSE                                  0x42
#define OBEX_FORBIDDEN_RESPONSE                                         0x43
#define OBEX_NOT_FOUND_RESPONSE                                         0x44
#define OBEX_METHOD_NOT_ALLOWED_RESPONSE                                0x45
#define OBEX_NOT_ACCEPTABLE_RESPONSE                                    0x46
#define OBEX_PROXY_AUTHENTICATION_REQUIRED_RESPONSE                     0x47
#define OBEX_REQUEST_TIMEOUT_RESPONSE                                   0x48
#define OBEX_CONFLICT_RESPONSE                                          0x49
#define OBEX_GONE_RESPONSE                                              0x4A
#define OBEX_LENGTH_REQUIRED_RESPONSE                                   0x4B
#define OBEX_PRECONDITION_FAILED_RESPONSE                               0x4C
#define OBEX_REQUESTED_ENTITY_TOO_LARGE_RESPONSE                        0x4D
#define OBEX_REQUESTED_URL_TOO_LARGE_RESPONSE                           0x4E
#define OBEX_UNSUPPORTED_MEDIA_TYPE_RESPONSE                            0x4F
#define OBEX_INTERNAL_SERVER_ERROR_RESPONSE                             0x50
#define OBEX_NOT_IMPLEMENTED_RESPONSE                                   0x51
#define OBEX_BAD_GATEWAY_RESPONSE                                       0x52
#define OBEX_SERVICE_UNAVAILABLE_RESPONSE                               0x53
#define OBEX_GATEWAY_TIMEOUT_RESPONSE                                   0x54
#define OBEX_HTTP_VERSION_NOT_SUPPORTED_RESPONSE                        0x55
#define OBEX_DATABASE_FULL_RESPONSE                                     0x60
#define OBEX_DATABASE_LOCKED_RESPONSE                                   0x61

   /* The following MACRO is a utility MACRO that is provided to        */
   /* simplify verifying if a Response Code Number is valid or not.     */
#define OBEX_VALID_RESPONSE_CODE_VALUE(_x)              ((_x) >= OBEX_CONTINUE_RESPONSE)

   /* The following definitions represent the Minimum and Maximum       */
   /* values of the OBEX Packet Size.                                   */
#define OBEX_PACKET_LENGTH_MINIMUM                                       255
#define OBEX_PACKET_LENGTH_MAXIMUM                                     65535

   /* OBEX SDP Protocol UUID's.                                         */

   /* The following MACRO is a utility MACRO that assigns the OBEX      */
   /* Bluetooth Universally Unique Identifier (OBEX_UUID_16) to the     */
   /* specified UUID_16_t variable.  This MACRO accepts one parameter   */
   /* which is the UUID_16_t variable that is receive the OBEX_UUID_16  */
   /* Constant value.                                                   */
#define SDP_ASSIGN_OBEX_UUID_16(_x)             ASSIGN_SDP_UUID_16((_x), 0x00, 0x08)

   /* The following MACRO is a utility MACRO that assigns the OBEX      */
   /* Bluetooth Universally Unique Identifier (OBEX_UUID_32) to the     */
   /* specified UUID_32_t variable.  This MACRO accepts one parameter   */
   /* which is the UUID_32_t variable that is receive the OBEX_UUID_32  */
   /* Constant value.                                                   */
#define SDP_ASSIGN_OBEX_UUID_32(_x)             ASSIGN_SDP_UUID_32((_x), 0x00, 0x00, 0x00, 0x08)

   /* The following MACRO is a utility MACRO that assigns the OBEX      */
   /* Bluetooth Universally Unique Identifier (OBEX_UID_128) to the     */
   /* specified UUID_128_t variable.  This MACRO accepts one parameter  */
   /* which is the UUID_128_t variable that is receive the              */
   /* OBEX_UUID_128 Constant value.                                     */
#define SDP_ASSIGN_OBEX_UUID_128(_x)            ASSIGN_SDP_UUID_128((_x), 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /* OBEX Header Definitions/Constants.                                */

#define OBEX_HEADER_ID_BIT_MASK                                         0x3F

#define OBEX_HEADER_ID_COUNT_VALUE                                      0x00
#define OBEX_HEADER_ID_NAME_VALUE                                       0x01
#define OBEX_HEADER_ID_TYPE_VALUE                                       0x02
#define OBEX_HEADER_ID_LENGTH_VALUE                                     0x03
#define OBEX_HEADER_ID_TIME_VALUE                                       0x04
#define OBEX_HEADER_ID_DESCRIPTION_VALUE                                0x05
#define OBEX_HEADER_ID_TARGET_VALUE                                     0x06
#define OBEX_HEADER_ID_HTTP_VALUE                                       0x07
#define OBEX_HEADER_ID_BODY_VALUE                                       0x08
#define OBEX_HEADER_ID_END_OF_BODY_VALUE                                0x09
#define OBEX_HEADER_ID_WHO_VALUE                                        0x0A
#define OBEX_HEADER_ID_CONNECTION_ID_VALUE                              0x0B
#define OBEX_HEADER_ID_APPLICATION_PARAMETERS_VALUE                     0x0C
#define OBEX_HEADER_ID_AUTHENTICATION_CHALLENGE_VALUE                   0x0D
#define OBEX_HEADER_ID_AUTHENTICATION_RESPONSE_VALUE                    0x0E
#define OBEX_HEADER_ID_OBJECT_CLASS_VALUE                               0x0F
#define OBEX_HEADER_ID_RESERVED_START_VALUE                             0x10
#define OBEX_HEADER_ID_RESERVED_END_VALUE                               0x2F
#define OBEX_HEADER_ID_USER_DEFINED_START_VALUE                         0x30
#define OBEX_HEADER_ID_USER_DEFINED_END_VALUE                           0x3F

#define OBEX_HEADER_TYPE_BIT_MASK                                       0xC0

#define OBEX_HEADER_TYPE_NULL_TERMINATED_UNICODE_TEXT_VALUE             0x00
#define OBEX_HEADER_TYPE_BYTE_SEQUENCE_VALUE                            0x40
#define OBEX_HEADER_TYPE_SINGLE_BYTE_VALUE                              0x80
#define OBEX_HEADER_TYPE_FOUR_BYTE_VALUE                                0xC0

   /* The following defines the amount of data consumed by specific OBEX*/
   /* Header.  This define includes the non-payload portion of a header,*/
   /* the Header ID and any length values.                              */
#define OBEX_BYTE_VALUE_HEADER_SIZE                                     (1)
#define OBEX_DWORD_VALUE_HEADER_SIZE                                    (1)
#define OBEX_BYTE_SEQUENCE_HEADER_SIZE                                  (3)
#define OBEX_UNICODE_HEADER_SIZE                                        (3)

   /* The following define the size of a header field in bytes.  This   */
   /* includes the header IDs and values.  This is used in calculating  */
   /* space consumed by a specific header in an OBEX packet.            */
#define OBEX_BYTE_VALUE_FIELD_SIZE                                     (OBEX_BYTE_VALUE_HEADER_SIZE + sizeof(Byte_t))
#define OBEX_DWORD_VALUE_FIELD_SIZE                                    (OBEX_DWORD_VALUE_HEADER_SIZE + sizeof(NonAlignedDWord_t))
#define OBEX_BYTE_SEQUENCE_FIELD_SIZE(_x)                              (OBEX_BYTE_SEQUENCE_HEADER_SIZE + (_x))
#define OBEX_UNICODE_FIELD_SIZE(_x)                                    (OBEX_UNICODE_HEADER_SIZE + ((_x) << 1))

   /* Application Parameters are passed in an OBEX Header as a Byte     */
   /* sequence.  The Byte sequence consist of a varying number of       */
   /* parameters that are organized in a Tag/Length/Value format.  The  */
   /* Value Parameter can be 0 to 255 bytes in length.                  */
typedef __PACKED_STRUCT_BEGIN__ struct _tagOBEX_Application_Parameters_t
{
   Byte_t Tag;
   Byte_t Length;
   Byte_t Value[1];
} __PACKED_STRUCT_END__ OBEX_Application_Parameters_t;

   /* The following Macro is a helper function that is used to calculate*/
   /* the amount of memory that is required for the OBEX Applicaiton    */
   /* Parameters structure.  The Macro takes one parameter which        */
   /* represents the number of bytes that is contained in the Value     */
   /* Buffer of the structure.                                          */
#define OBEX_APPLICATION_PARAMETERS_SIZE(_x)             ((sizeof(OBEX_Application_Parameters_t) - sizeof(Byte_t)) + (unsigned int)(_x))

   /* The following structure is used to map data that is presented in  */
   /* the OBEX Tag-Length-Value format.  The Value parameter is used as */
   /* a place holder for the start of the Value field.  The actual      */
   /* number of bytes that are contained in the Value parameter is      */
   /* specified in the Length parameter.                                */
typedef __PACKED_STRUCT_BEGIN__ struct _tagOBEX_Tag_Length_Value_t
{
   Byte_t Tag;
   Byte_t Length;
   Byte_t Value[1];
} __PACKED_STRUCT_END__ OBEX_Tag_Length_Value_t;

   /* The following Macro is used as a helper function when calculating */
   /* the amount of memory required to hold the entire Tag-Length-Value */
   /* information.  The parameter is the number of bytes that make up   */
   /* the Value parameter.                                              */
#define OBEX_TAG_LENGTH_VALUE_SIZE(_x)                   (sizeof(OBEX_Tag_Length_Value_t) - sizeof(Byte_t) + (unsigned int)(_x))

   /* OBEX Authentication Constants.                                    */

   /* The following constants represent the size (in bytes) of the      */
   /* Digest Challenge and Digest Response Nonce sizes.                 */
#define OBEX_DIGEST_CHALLENGE_RESPONSE_NONCE_SIZE                       0x10

   /* The following structure is a container structure for an OBEX      */
   /* Digest Challenge/Response Nonce.  Note that this is the same size */
   /* as the MD5 Hash output.                                           */
typedef __PACKED_STRUCT_BEGIN__ struct _tagOBEX_Nonce_t
{
   Byte_t Nonce[OBEX_DIGEST_CHALLENGE_RESPONSE_NONCE_SIZE];
} __PACKED_STRUCT_END__ OBEX_Nonce_t;

#define OBEX_NONCE_SIZE                                  (sizeof(OBEX_Nonce_t))

   /* The following constants represent the 8-bit delimeter character   */
   /* that is used when caculating the MD5 Hash of the Nonce and Private*/
   /* Key/Password.  For example, the MD5 Hash of the Digest Challenge  */
   /* is defined as follows:                                            */
   /*                                                                   */
   /*    - Time Stamp (or some other random number)                     */
   /*         There is no limit to the length of this value.            */
   /*    - Delimeter character (this constant)                          */
   /*         OBEX_DIGEST_CHALLENGE_RESPONSE_NONCE_MD5_DELIMETER_BYTE   */
   /*    - Private Key                                                  */
   /*         There is no limit to the length of this value.            */
   /*                                                                   */
   /* The MD5 Hash of the Digest Response is defined as follows:        */
   /*                                                                   */
   /*   - Nonce                                                         */
   /*       This is OBEX_DIGEST_CHALLENGE_RESPONSE_NONCE_SIZE bytes     */
   /*   - Delimeter character (this constant)                           */
   /*       OBEX_DIGEST_CHALLENGE_RESPONSE_NONCE_MD5_DELIMETER_BYTE     */
   /*   - Password                                                      */
   /*         There is no limit to the length of this value.            */
#define OBEX_DIGEST_CHALLENGE_RESPONSE_NONCE_MD5_DELIMETER_BYTE         ':'

   /* Digest Challenge Constants.                                       */
#define OBEX_DIGEST_CHALLENGE_TAG_NONCE                                 0x00
#define OBEX_DIGEST_CHALLENGE_TAG_OPTIONS                               0x01
#define OBEX_DIGEST_CHALLENGE_TAG_REALM                                 0x02

   /* The following bit mask values are used with the Digest Challenge  */
   /* Options Tag.                                                      */
#define OBEX_DIGEST_CHALLENGE_TAG_OPTIONS_USER_ID_NOT_REQUIRED          0x00
#define OBEX_DIGEST_CHALLENGE_TAG_OPTIONS_USER_ID_REQUIRED              0x01
#define OBEX_DIGEST_CHALLENGE_TAG_OPTIONS_FULL_ACCESS_MODE              0x00
#define OBEX_DIGEST_CHALLENGE_TAG_OPTIONS_READ_ONLY_ACCESS_MODE         0x02

   /* The following values represent the defined Character Set values   */
   /* used with the Digest Challenge Realm Tag.                         */
#define OBEX_DIGEST_CHALLENGE_TAG_REALM_CHARACTER_SET_ASCII             0x00
#define OBEX_DIGEST_CHALLENGE_TAG_REALM_CHARACTER_SET_ISO_8859_1        0x01
#define OBEX_DIGEST_CHALLENGE_TAG_REALM_CHARACTER_SET_ISO_8859_2        0x02
#define OBEX_DIGEST_CHALLENGE_TAG_REALM_CHARACTER_SET_ISO_8859_3        0x03
#define OBEX_DIGEST_CHALLENGE_TAG_REALM_CHARACTER_SET_ISO_8859_4        0x04
#define OBEX_DIGEST_CHALLENGE_TAG_REALM_CHARACTER_SET_ISO_8859_5        0x05
#define OBEX_DIGEST_CHALLENGE_TAG_REALM_CHARACTER_SET_ISO_8859_6        0x06
#define OBEX_DIGEST_CHALLENGE_TAG_REALM_CHARACTER_SET_ISO_8859_7        0x07
#define OBEX_DIGEST_CHALLENGE_TAG_REALM_CHARACTER_SET_ISO_8859_8        0x08
#define OBEX_DIGEST_CHALLENGE_TAG_REALM_CHARACTER_SET_ISO_8859_9        0x09
#define OBEX_DIGEST_CHALLENGE_TAG_REALM_CHARACTER_SET_UNICODE           0xFF

   /* The following structure is used to map data that is presented in  */
   /* the OBEX Tag-Length-Value format.  The Value parameter is used as */
   /* a place holder for the start of the Value field.  The actual      */
   /* number of bytes that are contained in the Value parameter is      */
   /* specified in the Length parameter.                                */
typedef __PACKED_STRUCT_BEGIN__ struct _tagOBEX_Realm_t
{
   Byte_t CharacterSet;
   Byte_t Value[1];
} __PACKED_STRUCT_END__ OBEX_Realm_Value_t;

   /* The following Macro is used as a helper function when calculating */
   /* the amount of memory required to hold the entire OBEX Real Value  */
   /* information.  The parameter is the number of bytes that make up   */
   /* the actual displayable text string (NOT counting the character set*/
   /* identifier).                                                      */
#define OBEX_REALM_VALUE_SIZE(_x)                        (sizeof(OBEX_Real_Value_t) - sizeof(Byte_t) + (unsigned int)(_x))

   /* Digest Response Constants.                                        */
#define OBEX_DIGEST_RESPONSE_TAG_REQUEST_DIGEST                         0x00
#define OBEX_DIGEST_RESPONSE_TAG_USER_ID                                0x01
#define OBEX_DIGEST_RESPONSE_TAG_NONCE                                  0x02

   /* The following constant represents the maximum size (in bytes) of  */
   /* the User ID value for the Digest Response User ID Tag.            */
#define OBEX_DIGEST_RESPONSE_TAG_USER_ID_MAXIMUM_SIZE                   0x14

#endif
