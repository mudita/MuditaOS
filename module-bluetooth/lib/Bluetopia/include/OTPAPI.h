/*
 * Copyright 2000 - 2014 Stonestreet One.
 * All Rights Reserved.
 * Author:  Tim Thomas
 *** MODIFICATION HISTORY ****************************************************
 *
 *   mm/dd/yy  F. Lastname    Description of Modification
 *   --------  -----------    ------------------------------------------------
 *   08/02/01  T. Thomas      Initial creation.
 ****************************************************************************
 */

/**
 *  @file OTPAPI.h
 *
 *  @brief Stonestreet One Bluetooth Stack OTP API Type Definitions,
 *      Constants, and Prototypes. To use any of the following API's,
 *      include the following declaration in your application.
 *
 *  @code
 *  #include "SS1BTPS.h"
 *  @endcode
 *  ============================================================================
 */
#ifndef __OTPAPIH__
#define __OTPAPIH__

#include "BTPSKRNL.h"           /*! BTPS Kernel Prototypes/Constants. */

#include "BTTypes.h"            /*! Bluetooth Type Definitions/Constants. */
#include "BTAPITyp.h"           /*! Bluetooth API Type Definitions. */
#include "SDPAPI.h"             /*! Bluetooth SDP API Definitions/Constants. */
#include "SPPAPI.h"             /*! Bluetooth SPP API Definitions/Constants. */

#include "BTPSCFG.h"            /*! BTPS Configuration Constants. */


   /*! The following constants represent the Minimum and Maximum Packet
    * Length values for OTP Packets (in Bytes).
    */
#define OTP_PACKET_LENGTH_MINIMUM                               OBEX_PACKET_LENGTH_MINIMUM
#define OTP_PACKET_LENGTH_MAXIMUM                               OBEX_PACKET_LENGTH_MAXIMUM

   /*! The following constants represent the OBEX Open Status Values
    * that are possible in the OTP Port Open Confirmation Event Data
    * Information.
    */
#define OTP_OPEN_PORT_STATUS_SUCCESS                            GOEP_OPEN_PORT_STATUS_SUCCESS
#define OTP_OPEN_PORT_STATUS_CONNECTION_TIMEOUT                 GOEP_OPEN_PORT_STATUS_CONNECTION_TIMEOUT
#define OTP_OPEN_PORT_STATUS_CONNECTION_REFUSED                 GOEP_OPEN_PORT_STATUS_CONNECTION_REFUSED
#define OTP_OPEN_PORT_STATUS_UNKNOWN_ERROR                      GOEP_OPEN_PORT_STATUS_UNKNOWN_ERROR

   /*! The following define the possible OBEX/OTP Response codes that
    * can be sent in OTP Response Messages.
    * \note These error codes map DIRECTLY to the OBEX Error Codes
    *          that are used by the underlying GOEP Tranport Layer.
    *          Because of this, the user is free to use either the
    *          GOEP or OTP defined error codes (because both are
    *          identical).
    * \note Response Codes less than 0x10 (OTP_CONTINUE_RESPONSE)
    *          are Reserved and CANNOT be used.
    */
#define OTP_CONTINUE_RESPONSE                                   OBEX_CONTINUE_RESPONSE
#define OTP_OK_RESPONSE                                         OBEX_OK_RESPONSE
#define OTP_CREATED_RESPONSE                                    OBEX_CREATED_RESPONSE
#define OTP_ACCEPTED_RESPONSE                                   OBEX_ACCEPTED_RESPONSE
#define OTP_NON_AUTHORITATIVE_INFORMATION_RESPONSE              OBEX_NON_AUTHORITATIVE_INFORMATION_RESPONSE
#define OTP_NO_CONTENT_RESPONSE                                 OBEX_NO_CONTENT_RESPONSE
#define OTP_RESET_CONTENT_RESPONSE                              OBEX_RESET_CONTENT_RESPONSE
#define OTP_PARTIAL_CONTENT_RESPONSE                            OBEX_PARTIAL_CONTENT_RESPONSE
#define OTP_MULTIPLE_CHOICES_RESPONSE                           OBEX_MULTIPLE_CHOICES_RESPONSE
#define OTP_MOVED_PERMANETLY_RESPONSE                           OBEX_MOVED_PERMANETLY_RESPONSE
#define OTP_MOVED_TEMPORARILY_RESPONSE                          OBEX_MOVED_TEMPORARILY_RESPONSE
#define OTP_SEE_OTHER_RESPONSE                                  OBEX_SEE_OTHER_RESPONSE
#define OTP_NOT_MODIFIED_RESPONSE                               OBEX_NOT_MODIFIED_RESPONSE
#define OTP_USE_PROXY_RESPONSE                                  OBEX_USE_PROXY_RESPONSE
#define OTP_BAD_REQUEST_RESPONSE                                OBEX_BAD_REQUEST_RESPONSE
#define OTP_UNAUTHORIZED_RESPONSE                               OBEX_UNAUTHORIZED_RESPONSE
#define OTP_PAYMENT_REQUIRED_RESPONSE                           OBEX_PAYMENT_REQUIRED_RESPONSE
#define OTP_FORBIDDEN_RESPONSE                                  OBEX_FORBIDDEN_RESPONSE
#define OTP_NOT_FOUND_RESPONSE                                  OBEX_NOT_FOUND_RESPONSE
#define OTP_METHOD_NOT_ALLOWED_RESPONSE                         OBEX_METHOD_NOT_ALLOWED_RESPONSE
#define OTP_NOT_ACCEPTABLE_RESPONSE                             OBEX_NOT_ACCEPTABLE_RESPONSE
#define OTP_PROXY_AUTHENTICATION_REQUIRED_RESPONSE              OBEX_PROXY_AUTHENTICATION_REQUIRED_RESPONSE
#define OTP_REQUEST_TIMEOUT_RESPONSE                            OBEX_REQUEST_TIMEOUT_RESPONSE
#define OTP_CONFLICT_RESPONSE                                   OBEX_CONFLICT_RESPONSE
#define OTP_GONE_RESPONSE                                       OBEX_GONE_RESPONSE
#define OTP_LENGTH_REQUIRED_RESPONSE                            OBEX_LENGTH_REQUIRED_RESPONSE
#define OTP_PRECONDITION_FAILED_RESPONSE                        OBEX_PRECONDITION_FAILED_RESPONSE
#define OTP_REQUESTED_ENTITY_TOO_LARGE_RESPONSE                 OBEX_REQUESTED_ENTITY_TOO_LARGE_RESPONSE
#define OTP_REQUESTED_URL_TOO_LARGE_RESPONSE                    OBEX_REQUESTED_URL_TOO_LARGE_RESPONSE
#define OTP_UNSUPPORTED_MEDIA_TYPE_RESPONSE                     OBEX_UNSUPPORTED_MEDIA_TYPE_RESPONSE
#define OTP_INTERNAL_SERVER_ERROR_RESPONSE                      OBEX_INTERNAL_SERVER_ERROR_RESPONSE
#define OTP_NOT_IMPLEMENTED_RESPONSE                            OBEX_NOT_IMPLEMENTED_RESPONSE
#define OTP_BAD_GATEWAY_RESPONSE                                OBEX_BAD_GATEWAY_RESPONSE
#define OTP_SERVICE_UNAVAILABLE_RESPONSE                        OBEX_SERVICE_UNAVAILABLE_RESPONSE
#define OTP_GATEWAY_TIMEOUT_RESPONSE                            OBEX_GATEWAY_TIMEOUT_RESPONSE
#define OTP_HTTP_VERSION_NOT_SUPPORTED_RESPONSE                 OBEX_HTTP_VERSION_NOT_SUPPORTED_RESPONSE
#define OTP_DATABASE_FULL_RESPONSE                              OBEX_DATABASE_FULL_RESPONSE
#define OTP_DATABASE_LOCKED_RESPONSE                            OBEX_DATABASE_LOCKED_RESPONSE

   /*! The following MACRO is a utility MACRO that is provided to
    * simplify verifying if a Response Code Number is valid or not.
    */
#define OTP_VALID_RESPONSE_CODE_VALUE(_x)               (OBEX_VALID_RESPONSE_CODE_VALUE(_x))

   /*! The following defines are used to indicate the phase of an
    * operation that requires multiple transactions.  If an operation
    * can be completed in a single operation, the OBJECT_PHASE_LAST is
    * used for that transaction.
    */
#define OTP_OBJECT_PHASE_FIRST                                  0x01
#define OTP_OBJECT_PHASE_LAST                                   0x02
#define OTP_OBJECT_PHASE_CONTINUE                               0x04

   /*! The following defines the bit assignments for the FieldMask
    * parameter in the ObjectInfo_t structure.  The appropriate bit is
    * set in the FieldMask parameter to denote the presence of a
    * parameter in the structure.
    */
#define OTP_OBJECT_INFO_MASK_CLEAR                              0x0000
#define OTP_OBJECT_INFO_MASK_NAME                               0x0001
#define OTP_OBJECT_INFO_MASK_SIZE                               0x0002
#define OTP_OBJECT_INFO_MASK_TYPE                               0x0004
#define OTP_OBJECT_INFO_MASK_MODIFIED                           0x0008
#define OTP_OBJECT_INFO_MASK_CREATED                            0x0010
#define OTP_OBJECT_INFO_MASK_ACCESSED                           0x0020
#define OTP_OBJECT_INFO_MASK_USER_PERMISSION                    0x0040
#define OTP_OBJECT_INFO_MASK_GROUP_PERMISSION                   0x0080
#define OTP_OBJECT_INFO_MASK_OTHER_PERMISSION                   0x0100
#define OTP_OBJECT_INFO_MASK_OWNER                              0x0200
#define OTP_OBJECT_INFO_MASK_GROUP                              0x0400
#define OTP_OBJECT_INFO_MASK_EXTENDED_NAME                      0x0800

   /*! The following defines the basic bit assignments for the permission
    * parameter used in the ObjectInfo_t structure.
    */
#define OTP_PERMISSION_READ_MASK                                0x0001
#define OTP_PERMISSION_WRITE_MASK                               0x0002
#define OTP_PERMISSION_DELETE_MASK                              0x0004

   /*! The following defines the bit assignments for the permission
    * parameters used in the ObjectInfo_t structure.  Each specific
    * permission is based on the basic bit assignment for the
    * permission.
    */
#define OTP_USER_PERMISSION_READ                                (OTP_PERMISSION_READ_MASK   << 0)
#define OTP_USER_PERMISSION_WRITE                               (OTP_PERMISSION_WRITE_MASK  << 0)
#define OTP_USER_PERMISSION_DELETE                              (OTP_PERMISSION_DELETE_MASK << 0)
#define OTP_GROUP_PERMISSION_READ                               (OTP_PERMISSION_READ_MASK   << 4)
#define OTP_GROUP_PERMISSION_WRITE                              (OTP_PERMISSION_WRITE_MASK  << 4)
#define OTP_GROUP_PERMISSION_DELETE                             (OTP_PERMISSION_DELETE_MASK << 4)
#define OTP_OTHER_PERMISSION_READ                               (OTP_PERMISSION_READ_MASK   << 8)
#define OTP_OTHER_PERMISSION_WRITE                              (OTP_PERMISSION_WRITE_MASK  << 8)
#define OTP_OTHER_PERMISSION_DELETE                             (OTP_PERMISSION_DELETE_MASK << 8)

   /*! The following defines the Tag values that are used with parameters
    * represented in the Tag-Length-Value format.  This format is used
    * with the Authentication headers.
    */
#define OTP_DIGEST_CHALLENGE_TAG_NONCE                          0x00
#define OTP_DIGEST_CHALLENGE_TAG_OPTIONS                        0x01
#define OTP_DIGEST_CHALLENGE_TAG_REALM                          0x02

   /*! The following defines the bit masks that are used with the Digest
    * Challenge and Digest Response structures.  If a bit is set, the
    * structure contains the optional infomration that is specified by
    * the bit.
    */
#define OTP_DIGEST_CHALLENGE_OPTIONAL_PARAMETERS_MASK_OPTIONS   0x01
#define OTP_DIGEST_CHALLENGE_OPTIONAL_PARAMETERS_MASK_REALM     0x02

   /*! The following defines the values that can be used in the options
    * parameter in the DigestChallenge_t structure.
    */
#define OTP_DIGEST_CHALLENGE_OPTIONS_USER_ID_IN_RESPONSE_BIT    0x01
#define OTP_DIGEST_CHALLENGE_OPTIONS_ACCESS_MODE_READ_ONLY_BIT  0x02

   /*! The following defines the Tag values that are used with parameters
    * represented in the Tag-Length-Value format.  This format is used
    * with the Authentication headers.
    */
#define OTP_DIGEST_RESPONSE_TAG_REQUEST_DIGEST                  0x00
#define OTP_DIGEST_RESPONSE_TAG_USER_ID                         0x01
#define OTP_DIGEST_RESPONSE_TAG_NONCE                           0x02

   /*! The following defines the values that can be used in the options
    * parameter in the DigestResponse_t structure.
    */
#define OTP_DIGEST_RESPONSE_OPTIONAL_PARAMETERS_MASK_USER_ID    0x01
#define OTP_DIGEST_RESPONSE_OPTIONAL_PARAMETERS_MASK_NONCE      0x02

   /*! The following defines the possible values for the character set
    * used in the DigentChallange_t structure.
    */
#define OTP_REALM_CHARACTER_SET_ASCII                           0x00
#define OTP_REALM_CHARACTER_SET_ISO88591                        0x01
#define OTP_REALM_CHARACTER_SET_ISO88592                        0x02
#define OTP_REALM_CHARACTER_SET_ISO88593                        0x03
#define OTP_REALM_CHARACTER_SET_ISO88594                        0x04
#define OTP_REALM_CHARACTER_SET_ISO88595                        0x05
#define OTP_REALM_CHARACTER_SET_ISO88596                        0x06
#define OTP_REALM_CHARACTER_SET_ISO88597                        0x07
#define OTP_REALM_CHARACTER_SET_ISO88598                        0x08
#define OTP_REALM_CHARACTER_SET_ISO88599                        0x09
#define OTP_REALM_CHARACTER_SET_UNICODE                         0xFF

   /*! The following defines the Tag values that are used with parameters
    * represented in the Tag-Length-Value format.  This format is used
    * with the Application Parameters used in Sync Mode.
    */
#define OTP_SYNC_REQUEST_TAG_EXPECTED_CHANGE_COUNTER            0x11
#define OTP_SYNC_REQUEST_TAG_HARD_DELETE                        0x12

   /*! The following defines the Tag values that are used with parameters
    * represented in the Tag-Length-Value format.  This format is used
    * with the Application Parameters used in Sync Mode.
    */
#define OTP_SYNC_RESPONSE_TAG_LUID                              0x01
#define OTP_SYNC_RESPONSE_TAG_CHANGE_COUNTER                    0x02
#define OTP_SYNC_RESPONSE_TAG_TIMESTAMP                         0x03

   /*! The following structure is used with the OTP_Register_SDP_Record()
    * function.  This structure contains additional SDP Service
    * Information that will be added to the SDP OBEX Service Record
    * Entry.  The first member of this strucuture specifies the Number
    * of Service Class UUID's that are present in the SDPUUIDEntries
    * Array.  This member must be at least one, and the SDPUUIDEntries
    * member must point to an array of SDP UUID Entries that contains
    * (at least) as many entries specified by the NumberServiceClassUUID
    * member.  The ProtocolList member is an SDP Data Element Sequence
    * that contains a list of Protocol Information that will be added to
    * the generic OBEX SDP Service Record.
    */
typedef struct _tagOTP_SDP_Service_Record_t
{
   unsigned int        NumberServiceClassUUID;
   SDP_UUID_Entry_t   *SDPUUIDEntries;
   SDP_Data_Element_t *ProtocolList;
} OTP_SDP_Service_Record_t;

#define OTP_SDP_SERVICE_RECORD_SIZE                     (sizeof(OTP_SDP_Service_Record_t))

   /*! The following structure is used with the OTP_Register_SDP_Record()
    * function.  This structure contains additional SDP Service
    * Information that will be added to the SDP OBEX Service Record
    * Entry.  The first member of this strucuture specifies the Number
    * of Service Class UUID's that are present in the SDPUUIDEntries
    * Array.  This member must be at least one, and the SDPUUIDEntries
    * member must point to an array of SDP UUID Entries that contains
    * (at least) as many entries specified by the NumberServiceClassUUID
    * member.  The NumberOfProtocolDataListUUIDOffsets and
    * ProtocolDataListUUIDOffsets specify the offsets of the UUIDs in
    * the specified ProtocolDataList data (if any UUIDs).  The
    * ProtocolDataListLength and ProtocolDataList members must contain
    * (if specified) a formatted SDP Data Element Sequence that contains
    * a list of Protocol Information that will be added to the generic
    * OBEX SDP Service Record.
    */
typedef struct _tagOTP_SDP_Raw_Service_Record_t
{
   unsigned int      NumberServiceClassUUID;
   SDP_UUID_Entry_t *SDPUUIDEntries;
   unsigned int      NumberOfProtocolDataListUUIDOffsets;
   Word_t           *ProtocolDataListUUIDOffsets;
   unsigned int      ProtocolDataListLength;
   Byte_t           *ProtocolDataList;
} OTP_SDP_Raw_Service_Record_t;

#define OTP_SDP_RAW_SERVICE_RECORD_SIZE                  (sizeof(OTP_SDP_Raw_Service_Record_t))

   /*! The following enumerates the Type of objects that are known by OTP
    * layer.  These types are used to define the type of object that is
    * described in the OTP_ObjectInfo_t structure.
    */
typedef enum
{
   otUnknown,
   otFolder,
   otFile,
   otObject,
   otFileFolder
} OTP_ObjectType_t;

   /*! The following enumerates the valid endpoints that a Client can
    * connect to.  These types are used internal to the OTP layer.
    */
typedef enum
{
   tUnknown,
   tInbox,
   tFileBrowser,
   tIRSync
} OTP_Target_t;

   /*! The following structure is used to represent a Time/Data
    * associated with a file when performing file transfer.  Since no
    * AM/PM field is provided, the Time is represented in 24 hour time.
    * The UTC_Time field is used to indicate whether the time is
    * represented in UTC time ot Local Time.
    */
typedef struct _tagOTP_TimeDate_t
{
   Word_t    Year;
   Word_t    Month;
   Word_t    Day;
   Word_t    Hour;
   Word_t    Minute;
   Word_t    Second;
   Boolean_t UTC_Time;
} OTP_TimeDate_t;

#define OTP_TIME_DATE_SIZE                              (sizeof(OTP_TimeDate_t))

   /*! The following definitions represent the maximum size (in Bytes)
    * of the specified fields in the OTP_ObjectInfo_t structure.  This
    * Implementation of OBEX/OTP has a limition on the length of
    * certain OBEX fields.  These lengths are listed below.
    */
#define OTP_OBJECT_INFO_MAXIMUM_NAME_LENGTH             (BTPS_CONFIGURATION_OTP_OBJECT_INFO_MAXIMUM_NAME_LENGTH )
#define OTP_OBJECT_INFO_MAXIMUM_TYPE_LENGTH             (BTPS_CONFIGURATION_OTP_OBJECT_INFO_MAXIMUM_TYPE_LENGTH )
#define OTP_OBJECT_INFO_MAXIMUM_OWNER_LENGTH            (BTPS_CONFIGURATION_OTP_OBJECT_INFO_MAXIMUM_OWNER_LENGTH)
#define OTP_OBJECT_INFO_MAXIMUM_GROUP_LENGTH            (BTPS_CONFIGURATION_OTP_OBJECT_INFO_MAXIMUM_GROUP_LENGTH)

   /*! The following structure is used to hold descriptive information
    * about an object that is being processed.  The object can be a
    * generic object, directory entry, or other.  The type of object is
    * defined in the Object Type parameter.  Also, the Name, Type, Owner
    * and Group fields are not limited in size per the OBEX or Bluetooth
    * specification.  This poses a problem when working on systems with
    * limited memory resources.  Therefor, these fields have a self
    * imposed limit to their size and any received values that exceed
    * this limit will be truncated to fit.
    * \note The Name, Type, Owner, and Group members are formatted
    *          as ASCII strings with UTF-8 encoding.
    */
typedef struct _tagOTP_ObjectInfo_t
{
   OTP_ObjectType_t ObjectType;
   Word_t           FieldMask;
   unsigned int     NameLength;
   char             Name[OTP_OBJECT_INFO_MAXIMUM_NAME_LENGTH];
   unsigned long    Size;
   unsigned int     TypeLength;
   char             Type[OTP_OBJECT_INFO_MAXIMUM_TYPE_LENGTH];
   OTP_TimeDate_t   Modified;
   OTP_TimeDate_t   Created;
   OTP_TimeDate_t   Accessed;
   Word_t           Permission;
   unsigned int     OwnerLength;
   char             Owner[OTP_OBJECT_INFO_MAXIMUM_OWNER_LENGTH];
   unsigned int     GroupLength;
   char             Group[OTP_OBJECT_INFO_MAXIMUM_GROUP_LENGTH];
} OTP_ObjectInfo_t;

#define OTP_OBJECT_INFO_SIZE                            (sizeof(OTP_ObjectInfo_t))

   /*! The following MACRO may be used to aid in reading the extended
    * name from the OTP_ObjectInfo_t structure Name member when an
    * extended name exists in the Name member.  This may be determine by
    * the FieldMask having the OTP_OBJECT_INFO_MASK_EXTENDED_NAME bit
    * set.
    */
#define READ_OBJECT_INFO_EXTENDED_NAME(_x)              READ_UNALIGNED_DWORD_LITTLE_ENDIAN(&(((char *)(_x))[OTP_OBJECT_INFO_MAXIMUM_NAME_LENGTH - sizeof(NonAlignedDWord_t)]))

   /*! The following structure is used when receiving and building
    * directory information when connected to the File Browser service.
    * The NumberEntries field specifies how many OTP Object Information
    * Entries are contained in the array pointed to by the last member.
    * It is very important that the ObjectInfo member contain at least
    * as many entries as the NumberEntries parameter.  The
    * ParentDirectory member specifies whether or not the directory
    * specified has a Parent Directory above it.  When this member is
    * TRUE, then the Client is allowed to issue a Set Path Command to
    * the Parent of this directory.  When this member is FALSE, then
    * the client is notified that this is a Root Directory and as such,
    * cannot issue a Set Path to reach a Parent Directory.
    */
typedef struct _tagOTP_DirectoryInfo_t
{
   Boolean_t         ParentDirectory;
   unsigned int      NumberEntries;
   OTP_ObjectInfo_t *ObjectInfo;
} OTP_DirectoryInfo_t;

   /*! The following Macro is used as a helper function when calculating
    * the amount of memory required to hold a specified number of
    * OTP_ObjectInfo_t Structure entries.  @param _x The first parameter to this
    * MACRO is the number of entries that are present in the
    * ObjectInfo member of the OTP_DirectoryInfo_t structure.
    */
#define OTP_DIRECTORY_INFO_SIZE(_x)                     (sizeof(OTP_DirectoryInfo_t)+((_x)*sizeof(OTP_ObjectInfo_t)))

   /*! The following structure is used to map data that is presented in
    * the OBEX Tag-Length-Value format.  The Value parameter is used as
    * a place holder for the start of the Value field.  The actual
    * number of bytes that are contained in the Value parameter is
    * specified in the Length parameter.
    */
typedef struct _tagOTP_Tag_Length_Value_t
{
   Byte_t Tag;
   Byte_t Length;
   Byte_t Value[1];
} OTP_Tag_Length_Value_t;

   /*! The following Macro is used as a helper function when calculating
    * the amount of memory required to hold the entire Tag-Length-Value
    * information.  The parameter is the number of bytes that make up
    * the Value parameter.
    */
#define OTP_TAG_LENGTH_VALUE_SIZE(_x)                   (BTPS_STRUCTURE_OFFSET(OTP_Tag_Length_Value_t, Value) + (unsigned int)(_x))

   /*! The following definitions represent the maximum size (in Bytes)
    * of the specified fields in the OTP_Digest_Challenge_t and
    * OTP_Digest_Response_t structures.  This Implementation of OBEX/OTP
    * has a limition on the length of certain OBEX fields.  These
    * lengths are listed below.
    */
#define OTP_DIGEST_MAXIMUM_REQUEST_DIGEST_LENGTH                16
#define OTP_DIGEST_MAXIMUM_NONCE_LENGTH                         16
#define OTP_DIGEST_MAXIMUM_USER_ID_LENGTH                       20
#define OTP_DIGEST_MAXIMUM_REALM_LENGTH                         50

   /*! The following structure is used to represent the information that
    * is required when performing an Authentication Challenge.  The
    * Nonce field is manditory for this structure and must be 16 bytes
    * in length.  The OptionalParameterMask is a bit mask that denotes
    * the optional parameters that are supplied in the structure.  The
    * Realm value has been limited to 50 bytes in this implementation.
    */
typedef struct _tagOTP_Digest_Challenge_t
{
   Byte_t       Nonce[OTP_DIGEST_MAXIMUM_NONCE_LENGTH];
   Byte_t       OptionalParametersMask;
   Byte_t       Options;
   unsigned int RealmLength;
   Byte_t       RealmCharacterSet;
   char         Realm[OTP_DIGEST_MAXIMUM_REALM_LENGTH];
} OTP_Digest_Challenge_t;

#define OTP_DIGEST_CHALLENGE_SIZE                       (sizeof(OTP_Digest_Challenge_t))

   /*! The following structure is used to represent the information that
    * is required when performing an Authentication Response.  The
    * RequestDigest field is manditory for this structure and must be
    * 16 bytes in length.  The OptionalParameterMask is a bit mask that
    * denotes the optional parameters that are supplied in the
    * structure.  The User ID has been limited in size in this
    * implementation.
    */
typedef struct _tagOTP_Digest_Response_t
{
   Byte_t       RequestDigest[OTP_DIGEST_MAXIMUM_REQUEST_DIGEST_LENGTH];
   Byte_t       OptionalParametersMask;
   unsigned int UserIDLength;
   Byte_t       UserID[OTP_DIGEST_MAXIMUM_USER_ID_LENGTH];
   Byte_t       Nonce[OTP_DIGEST_MAXIMUM_NONCE_LENGTH];
} OTP_Digest_Response_t;

#define OTP_DIGEST_RESPONSE_SIZE                        (sizeof(OTP_Digest_Response_t))

   /*! The following structure is used to provide information about the
    * Sync Anchor(s) that are used for the sync process.  At least one
    * Sync Anchor must be used.  The profile does allow the use of both
    * types during a sync operation, but if both are available, the use
    * of the Change Counter only is recommended.
    */
typedef struct _tagSyncAnchor_t
{
   Boolean_t      TimestampUsed;
   OTP_TimeDate_t Timestamp;
   Boolean_t      ChangeCountUsed;
   DWord_t        ChangeCount;
} SyncAnchor_t;

#define OTP_SYNCANCHOR_SIZE                           (sizeof(SyncAnchor_t))

   /*! The following structure is used to represent the information that
    * is required when performing a Put Sync Request.
    */
typedef struct _tagOTP_Sync_Request_Params_t
{
   Boolean_t    HardDelete;
   SyncAnchor_t SyncAnchor;
} OTP_Sync_Request_Params_t;

#define OTP_SYNC_REQUEST_PARAMS_SIZE                    (sizeof(OTP_Sync_Request_Params_t))

   /*! The following definitions represent the maximum size (in Bytes) of
    * the UID/LUID field in the OTP_Sync_Response_Params_t structures.
    * This Implementation of OBEX/OTP has a limition on the length of
    * certain OBEX fields.  These lengths are listed below.
    */
#define OTP_SYNC_UID_MAXIMUM_LENGTH                             40

   /*! The following structure is used to represent the information that
    * is required when performing a Put Sync Response.  The UID has been
    * limited in size in this implementation.
    */
typedef struct _tagOTP_Sync_Response_Params_t
{
   SyncAnchor_t SyncAnchor;
   Byte_t       UID[OTP_SYNC_UID_MAXIMUM_LENGTH];
} OTP_Sync_Response_Params_t;

#define OTP_SYNC_RESPONSE_PARAMS_SIZE                   (sizeof(OTP_Sync_Response_Params_t))

   /*! The following enumerated type represents the events that can be
    * generated by the OTP layer to the upper layers.
    */
typedef enum
{
   etOTP_Port_Open_Indication,
   etOTP_Port_Open_Confirmation,
   etOTP_Port_Close_Indication,
   etOTP_Connect_Request,
   etOTP_Connect_Response,
   etOTP_Disconnect_Request,
   etOTP_Disconnect_Response,
   etOTP_Set_Path_Request,
   etOTP_Set_Path_Response,
   etOTP_Abort_Request,
   etOTP_Abort_Response,
   etOTP_Delete_Object_Request,
   etOTP_Delete_Object_Response,
   etOTP_Get_Directory_Request,
   etOTP_Get_Directory_Response,
   etOTP_Get_Object_Request,
   etOTP_Get_Object_Response,
   etOTP_Put_Object_Request,
   etOTP_Put_Object_Response,
   etOTP_Free_Directory_Information,
   etOTP_Delete_Sync_Object_Request,
   etOTP_Delete_Sync_Object_Response,
   etOTP_Put_Sync_Object_Request,
   etOTP_Put_Sync_Object_Response,
   etOTP_Port_Open_Request_Indication
} OTP_Event_Data_Type_t;

   /*! The following structure is used to pass information to the upper
    * layers upon the reception of the Open Indication.
    */
typedef struct _tagOTP_Port_Open_Indication_Data_t
{
   unsigned int OTP_ID;
   BD_ADDR_t    BD_ADDR;
} OTP_Port_Open_Indication_Data_t;

#define OTP_PORT_OPEN_INDICATION_DATA_SIZE              (sizeof(OTP_Port_Open_Indication_Data_t))

   /*! The following structure is used to pass information to the upper
    * layers upon the reception of the Open Confirmation.
    */
typedef struct _tagOTP_Port_Open_Confirmation_Data_t
{
   unsigned int OTP_ID;
   unsigned int PortOpenStatus;
} OTP_Port_Open_Confirmation_Data_t;

#define OTP_PORT_OPEN_CONFIRMATION_DATA_SIZE            (sizeof(OTP_Port_Open_Confirmation_Data_t))

   /*! The following structure is used to pass information to the upper
    * layers upon the reception of the Close Indication.
    */
typedef struct _tagOTP_Port_Close_Indication_Data_t
{
   unsigned int  OTP_ID;
   unsigned long UserInfo;
} OTP_Port_Close_Indication_Data_t;

#define OTP_PORT_CLOSE_INDICATION_DATA_SIZE             (sizeof(OTP_Port_Close_Indication_Data_t))

   /*! The following structure is used to pass information received from
    * a connect request up to the upper layers.
    */
typedef struct _tagOTP_Connect_Request_Data_t
{
   unsigned int            OTP_ID;
   OTP_Target_t            Target;
   OTP_Digest_Challenge_t *DigestChallenge;
   OTP_Digest_Response_t  *DigestResponse;
} OTP_Connect_Request_Data_t;

#define OTP_CONNECT_REQUEST_DATA_SIZE                   (sizeof(OTP_Connect_Request_Data_t))

   /*! The following structure is used to pass information received from
    * the response to a Connect Request to the upper layers.
    */
typedef struct _tagOTP_Connect_Response_Data_t
{
   unsigned int            OTP_ID;
   Byte_t                  ResponseCode;
   OTP_Target_t            Target;
   OTP_Digest_Challenge_t *DigestChallenge;
   OTP_Digest_Response_t  *DigestResponse;
} OTP_Connect_Response_Data_t;

#define OTP_CONNECT_RESPONSE_DATA_SIZE                  (sizeof(OTP_Connect_Response_Data_t))

   /*! The following structure is used to pass information received from
    * a Disconnect Request to the upper layers.  The UserInfo member
    * specifies the UserInfo parameter that could have been specified
    * for the current action that is executing (user definable).
    */
typedef struct _tagOTP_Disconnect_Request_Data_t
{
   unsigned int  OTP_ID;
   unsigned long UserInfo;
} OTP_Disconnect_Request_Data_t;

#define OTP_DISCONNECT_REQUEST_DATA_SIZE                (sizeof(OTP_Disconnect_Request_Data_t))

   /*! The following structure is used to pass information received from
    * the remote server as a response to a Disconnect Request to the
    * upper layers.  The Response Code contains the response sent from
    * the remote server.
    */
typedef struct _tagOTP_Disconnect_Response_Data_t
{
   unsigned int OTP_ID;
   Byte_t       ResponseCode;
} OTP_Disconnect_Response_Data_t;

#define OTP_DISCONNECT_RESPONSE_DATA_SIZE               (sizeof(OTP_Disconnect_Response_Data_t))

   /*! The following structure is used to pass a Set Path Request
    * received from a remote client to the upper layers.
    * \note The Folder member is formatted as a NULL terminated
    *          ASCII string with UTF-8 encoding.
    */
typedef struct _tagOTP_Set_Path_Request_Data_t
{
   unsigned int OTP_ID;
   Boolean_t    Backup;
   Boolean_t    Create;
   char        *Folder;
} OTP_Set_Path_Request_Data_t;

#define OTP_SET_PATH_REQUEST_DATA_SIZE                  (sizeof(OTP_Set_Path_Request_Data_t))

   /*! The following structure is used to pass information received from
    * the remote server as a response to a Set Path Request to the upper
    * layers.  The Response Code contains the response sent from the
    * remote server.
    */
typedef struct _tagOTP_Set_Path_Response_Data_t
{
   unsigned int OTP_ID;
   Byte_t       ResponseCode;
} OTP_Set_Path_Response_Data_t;

#define OTP_SET_PATH_RESPONSE_DATA_SIZE                 (sizeof(OTP_Set_Path_Response_Data_t))

   /*! The following structure is used to pass a Abort Request received
    * from a remote client to the upper layers.  The UserInfo member
    * specifies the UserInfo parameter that could have been specified
    * for the current action that is executing (user definable).
    */
typedef struct _tagOTP_Abort_Request_Data_t
{
   unsigned int  OTP_ID;
   unsigned long UserInfo;
} OTP_Abort_Request_Data_t;

#define OTP_ABORT_REQUEST_DATA_SIZE                     (sizeof(OTP_Abort_Request_Data_t))

   /*! The following structure is used to pass information received from
    * the remote server as a response to a Abort Request to the upper
    * layers.  The Response Code contains the response sent from the
    * remote server.
    */
typedef struct _tagOTP_Abort_Response_Data_t
{
   unsigned int OTP_ID;
   Byte_t       ResponseCode;
} OTP_Abort_Response_Data_t;

#define OTP_ABORT_RESPONSE_DATA_SIZE                    (sizeof(OTP_Abort_Response_Data_t))

   /*! The following structure is used to pass a Delete Object Request
    * received from a remote client to the upper layers.  The
    * ObjectInfo_t structure provides information as to the object that
    * is to be deleted.
    */
typedef struct _tagOTP_Delete_Object_Request_Data_t
{
   unsigned int     OTP_ID;
   OTP_ObjectInfo_t ObjectInfo;
} OTP_Delete_Object_Request_Data_t;

#define OTP_DELETE_OBJECT_REQUEST_DATA_SIZE             (sizeof(OTP_Delete_Object_Request_Data_t))

   /*! The following structure is used to pass information received from
    * the remote server as a response to a Delete Object Request to the
    * upper layers.  The Response Code contains the response sent from
    * the remote server.
    */
typedef struct _tagOTP_Delete_Object_Response_Data_t
{
   unsigned int  OTP_ID;
   Byte_t        ResponseCode;
} OTP_Delete_Object_Response_Data_t;

#define OTP_DELETE_OBJECT_RESPONSE_DATA_SIZE            (sizeof(OTP_Delete_Object_Response_Data_t))

   /*! The following structure is used to pass a Delete Sync Object
    * Request received from a remote client to the upper layers.  The
    * ObjectInfo_t structure provides information as to the object that
    * is to be deleted.
    */
typedef struct _tagOTP_Delete_Sync_Object_Request_Data_t
{
   unsigned int              OTP_ID;
   OTP_Sync_Request_Params_t SyncParams;
   OTP_ObjectInfo_t          ObjectInfo;
} OTP_Delete_Sync_Object_Request_Data_t;

#define OTP_DELETE_SYNC_OBJECT_REQUEST_DATA_SIZE        (sizeof(OTP_Delete_Sync_Object_Request_Data_t))

   /*! The following structure is used to pass information received from
    * the remote server as a response to a Delete Sync Object Request to
    * the upper layers.  The Response Code contains the response sent
    * from the remote server.
    */
typedef struct _tagOTP_Delete_Sync_Object_Response_Data_t
{
   unsigned int               OTP_ID;
   OTP_Sync_Response_Params_t SyncParams;
   Byte_t                     ResponseCode;
} OTP_Delete_Sync_Object_Response_Data_t;

#define OTP_DELETE_SYNC_OBJECT_RESPONSE_DATA_SIZE       (sizeof(OTP_Delete_Sync_Object_Response_Data_t))

   /*! The following structure is used to pass a Put Request received
    * from a remote client to the upper layers.
    */
typedef struct _tagOTP_Put_Object_Request_Data_t
{
   unsigned int      OTP_ID;
   Byte_t            Phase;
   OTP_ObjectInfo_t  ObjectInfo;
   unsigned int      DataLength;
   Byte_t           *DataPtr;
   unsigned long     UserInfo;
} OTP_Put_Object_Request_Data_t;

#define OTP_PUT_OBJECT_REQUEST_DATA_SIZE                (sizeof(OTP_Put_Object_Request_Data_t))

   /*! The following structure is used to pass a Put Request received
    * from a remote client to the upper layers.
    */
typedef struct _tagOTP_Put_Object_Response_Data_t
{
   unsigned int   OTP_ID;
   Byte_t         ResponseCode;
   unsigned int   BufferSize;
   unsigned long  UserInfo;
} OTP_Put_Object_Response_Data_t;

#define OTP_PUT_OBJECT_RESPONSE_DATA_SIZE               (sizeof(OTP_Put_Object_Response_Data_t))

   /*! The following structure is used to pass a Put Sync Request
    * received from a remote client to the upper layers.
    */
typedef struct _tagOTP_Put_Sync_Object_Request_Data_t
{
   unsigned int               OTP_ID;
   Byte_t                     Phase;
   OTP_ObjectInfo_t           ObjectInfo;
   OTP_Sync_Request_Params_t  SyncParams;
   unsigned int               DataLength;
   Byte_t                    *DataPtr;
   unsigned long              UserInfo;
} OTP_Put_Sync_Object_Request_Data_t;

#define OTP_PUT_SYNC_OBJECT_REQUEST_DATA_SIZE           (sizeof(OTP_Put_Sync_Object_Request_Data_t))

   /*! The following structure is used to pass a Put Request received
    * from a remote client to the upper layers.
    */
typedef struct _tagOTP_Put_Sync_Object_Response_Data_t
{
   unsigned int               OTP_ID;
   OTP_Sync_Response_Params_t SyncParams;
   Byte_t                     ResponseCode;
   unsigned int               BufferSize;
   unsigned long              UserInfo;
} OTP_Put_Sync_Object_Response_Data_t;

#define OTP_PUT_SYNC_OBJECT_RESPONSE_DATA_SIZE          (sizeof(OTP_Put_Sync_Object_Response_Data_t))

   /*! The following structure is used to pass a Get Object Request
    * received from a remote client to the upper layers.  The Phase
    * parameter indicates whether this is the first request or if this
    * is a continuation.  The ObjectInfo parameter provides information
    * about the Object that is being retreived.  The BufferSize and
    * BufferPtr are passed to the upper layer to provide a Buffer and
    * Length that is available for a response.  The UserInfo parameter
    * is a User Definable parameter that is obtained from the parameter
    * that is supplied in the previous Get Object Response.
    */
typedef struct _tagOTP_Get_Object_Request_Data_t
{
   unsigned int      OTP_ID;
   Byte_t            Phase;
   OTP_ObjectInfo_t  ObjectInfo;
   unsigned int      BufferSize;
   Byte_t           *BufferPtr;
   unsigned long     UserInfo;
} OTP_Get_Object_Request_Data_t;

#define OTP_GET_OBJECT_REQUEST_DATA_SIZE                (sizeof(OTP_Get_Object_Request_Data_t))

   /*! The following structure is used to pass a Get Object Response
    * received from a remote server to the upper layers.  The
    * ResponseCode contains the response to the request from the remote
    * server.  The Phase parameter indicates whether this is the first
    * response or if this is a continuation.  The ObjectInfo parameter
    * provides information about the Object that is being retreived.
    * The BufferSize and BufferPtr are used to passed to the upper layer
    * the data that is present in the response.  The UserInfo parameter
    * is a User Definable parameter that is obtained from the parameter
    * that is supplied in the prevoius Get Object Request.
    */
typedef struct _tagOTP_Get_Object_Response_Data_t
{
   unsigned int      OTP_ID;
   Byte_t            ResponseCode;
   Byte_t            Phase;
   OTP_ObjectInfo_t  ObjectInfo;
   unsigned int      BufferSize;
   Byte_t           *BufferPtr;
   unsigned long     UserInfo;
} OTP_Get_Object_Response_Data_t;

#define OTP_GET_OBJECT_RESPONSE_DATA_SIZE               (sizeof(OTP_Get_Object_Response_Data_t))

   /*! The following structure is used to pass a Get Directory Resquest
    * received from a remote client to the upper layers.  The
    * name_length parameter identifies the number of bytes that are
    * present the the Name field.  The Name parameter is a pointer to a
    * string of characters that denote the directory Name that is being
    * requested.
    * \note The Name member is formatted as an ASCII string with
    *          UTF-8 encoding.
    */
typedef struct _tagOTP_Get_Directory_Request_Data_t
{
   unsigned int OTP_ID;
   unsigned int NameLength;
   char        *Name;
} OTP_Get_Directory_Request_Data_t;

#define OTP_GET_DIRECTORY_REQUEST_DATA_SIZE             (sizeof(OTP_Get_Directory_Request_Data_t))

   /*! The following structure is used to pass a Get Directory Response
    * received from a remote server to the upper layers.  The
    * ResponseCode contains the response to the request from the remote
    * server.  The Phase parameter indicates whether this is the first
    * response or if this is a continuation.  The DirInfo parameter
    * contains all the information about the files and sub-directories
    * that are contained in the directory that was requested.
    */
typedef struct _tagOTP_Get_Directory_Response_Data_t
{
   unsigned int        OTP_ID;
   Byte_t              ResponseCode;
   Byte_t              Phase;
   OTP_DirectoryInfo_t DirInfo;
} OTP_Get_Directory_Response_Data_t;

#define OTP_GET_DIRECTORY_RESPONSE_DATA_SIZE            (sizeof(OTP_Get_Directory_Response_Data_t))

   /*! The following structure is used to inform the upper layer that the
    * memory that was allocated to contain the directory information
    * from a previous Get Directory Request, is no longer needed by the
    * lower layers and can not be freed.
    */
typedef struct _tagOTP_Free_Directory_Information_Data_t
{
   unsigned int        OTP_ID;
   OTP_DirectoryInfo_t DirectoryInfo;
} OTP_Free_Directory_Information_Data_t;

#define OTP_FREE_DIRECTORY_INFORMATION_DATA_SIZE        (sizeof(OTP_Free_Directory_Information_Data_t))

   /*! The following structure is used to pass information to the upper
    * layers upon the reception of the Open Request Indication.
    */
typedef struct _tagOTP_Port_Open_Request_Indication_Data_t
{
   unsigned int OTP_ID;
   BD_ADDR_t    BD_ADDR;
} OTP_Port_Open_Request_Indication_Data_t;

#define OTP_PORT_OPEN_REQUEST_INDICATION_DATA_SIZE      (sizeof(OTP_Port_Open_Request_Indication_Data_t))

   /*! The following structure is used to package event information that
    * is dispatched to the upper layers for processing.  The Event Data
    * Type is identified the type of information that is contained in
    * the structure.
    */
typedef struct _tagOTP_Event_Data_t
{
   OTP_Event_Data_Type_t  Event_Data_Type;
   Word_t                 Event_Data_Size;
   union
   {
      OTP_Port_Open_Indication_Data_t         *OTP_Port_Open_Indication_Data;
      OTP_Port_Open_Confirmation_Data_t       *OTP_Port_Open_Confirmation_Data;
      OTP_Port_Close_Indication_Data_t        *OTP_Port_Close_Indication_Data;
      OTP_Connect_Request_Data_t              *OTP_Connect_Request_Data;
      OTP_Connect_Response_Data_t             *OTP_Connect_Response_Data;
      OTP_Disconnect_Request_Data_t           *OTP_Disconnect_Request_Data;
      OTP_Disconnect_Response_Data_t          *OTP_Disconnect_Response_Data;
      OTP_Set_Path_Request_Data_t             *OTP_Set_Path_Request_Data;
      OTP_Set_Path_Response_Data_t            *OTP_Set_Path_Response_Data;
      OTP_Abort_Request_Data_t                *OTP_Abort_Request_Data;
      OTP_Abort_Response_Data_t               *OTP_Abort_Response_Data;
      OTP_Get_Directory_Request_Data_t        *OTP_Get_Directory_Request_Data;
      OTP_Get_Directory_Response_Data_t       *OTP_Get_Directory_Response_Data;
      OTP_Put_Object_Request_Data_t           *OTP_Put_Object_Request_Data;
      OTP_Put_Object_Response_Data_t          *OTP_Put_Object_Response_Data;
      OTP_Put_Sync_Object_Request_Data_t      *OTP_Put_Sync_Object_Request_Data;
      OTP_Put_Sync_Object_Response_Data_t     *OTP_Put_Sync_Object_Response_Data;
      OTP_Get_Object_Request_Data_t           *OTP_Get_Object_Request_Data;
      OTP_Get_Object_Response_Data_t          *OTP_Get_Object_Response_Data;
      OTP_Delete_Object_Request_Data_t        *OTP_Delete_Object_Request_Data;
      OTP_Delete_Object_Response_Data_t       *OTP_Delete_Object_Response_Data;
      OTP_Delete_Sync_Object_Request_Data_t   *OTP_Delete_Sync_Object_Request_Data;
      OTP_Delete_Sync_Object_Response_Data_t  *OTP_Delete_Sync_Object_Response_Data;
      OTP_Free_Directory_Information_Data_t   *OTP_Free_Directory_Information_Data;
      OTP_Port_Open_Request_Indication_Data_t *OTP_Port_Open_Request_Indication_Data;
   } Event_Data;
} OTP_Event_Data_t;

#define OTP_EVENT_DATA_SIZE                             (sizeof(OTP_Event_Data_t))

   /*! The following declared type represents the Prototype Function for
    * an OTP Event Callback.  This function will be called whenever a
    * defined OTP Action occurs within the Bluetooth Protocol Stack that
    * is specified with the specified Bluetooth Stack ID.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param OTP_Event_Data Data describing the event for which the callback
    * function is called.
    * @param CallbackParameter User-defined parameter (e.g., tag value) that
    * was defined in the callback registration.
    * The caller is free to use the contents of the OTP Event Data ONLY
    * in the context of this callback.  If the caller requires the Data
    * for a longer period of time, then the callback function MUST copy
    * the data into another Data Buffer.  This function is guaranteed
    * NOT to be invoked more than once simultaneously for the specified
    * installed callback (i.e.  this function DOES NOT have be
    * reentrant).
    */
typedef void (BTPSAPI *OTP_Event_Callback_t)(unsigned int BluetoothStackID, OTP_Event_Data_t *OTP_Event_Data, unsigned long CallbackParameter);

   /*! @brief The following function is responsible for establishing an OTP Port
    * Server (will wait for a connection to occur on the port
    * established by this function).
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param ServerPort Port number to use. This must fall in the range
    * defined by the following constants:
    * SPP_PORT_NUMBER_MINIMUM and SPP_PORT_NUMBER_MAXIMUM
    * @param Target The service on the remote server to which the connection
    * is targeted.
    * @param MaxPacketLength The largest packet that will be sent/received
    * on this connection. Each side must support a minimum of 255 bytes,
    * and cannot have a packet size greater than 64K- bytes.
    * @param OTP_Event_Callback Function to call when events occur on this
    * port.
    * @param CallbackParameter A user-defined parameter (e.g., a tag value)
    * that will be passed back to the user in the callback function with each packet.
    * @return This function returns a non-zero, positive, number on
    * success or a negative return error code if an error occurred (see
    * BTERRORS.H).  A successful return code will be a Port ID that can
    * be used to reference the Opened OTP Port in ALL other functions in
    * this module (except the OTP_Open_Remote_Port() function).  Once a
    * Server Port is opened, it can only be Un-Registered via a call to
    * the OTP_Close_Server_Port() function (passing the return value
    * from this function).  The OTP_Close_Port() function can be used to
    * Disconnect a Client from the Server Port (if one is connected, it
    * will NOT Un-Register the Server Port however).
    */
BTPSAPI_DECLARATION int BTPSAPI OTP_Open_Server_Port(unsigned int BluetoothStackID, unsigned int ServerPort, OTP_Target_t Target, Word_t MaxPacketLength, OTP_Event_Callback_t OTP_Event_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_OTP_Open_Server_Port_t)(unsigned int BluetoothStackID, unsigned int ServerPort, OTP_Target_t Target, Word_t MaxPacketLength, OTP_Event_Callback_t OTP_Event_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is responsible for Un-Registering an OTP
    * Port Server (which was Registered by a successful call to the
    * OTP_Open_Server_Port() function).
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param OTP_ID The port to close. This is the value that was returned
    * from the OTP_Open_Server_Port( ) function.
    * @return This function returns zero if successful, or a
    * negative return error code if an error occurred (see BTERRORS.H).
    * Note that this function does NOT delete any SDP Service Record
    * Handles.
    */
BTPSAPI_DECLARATION int BTPSAPI OTP_Close_Server_Port(unsigned int BluetoothStackID, unsigned int OTP_ID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_OTP_Close_Server_Port_t)(unsigned int BluetoothStackID, unsigned int OTP_ID);
#endif

   /*! @brief The following function is responsible for responding to requests
    * to connect to an OTP Port Server.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param OTP_ID The port this command applies to.  This is the value
    * that was returned from the OTP_Open_Server_Port( ) function.
    * @param AcceptConnection Boolean indicating if the pending connection
    * should be accepted.
    * @return This function returns zero if successful, or a negative return value if there was an
    * error.
    */
BTPSAPI_DECLARATION int BTPSAPI OTP_Open_Port_Request_Response(unsigned int BluetoothStackID, unsigned int OTP_ID, Boolean_t AcceptConnection);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_OTP_Open_Port_Request_Response_t)(unsigned int BluetoothStackID, unsigned int OTP_ID, Boolean_t AcceptConnection);
#endif

   /*! @brief The following function is provided to allow a means to add a
    * Generic OBEX Service Record to the SDP Database.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param OTP_ID The port this command applies to.  This is the value
    * that was returned from the OTP_Open_Server_Port() function.
    * @param SDPServiceRecord Any additional Service Discovery Protocol
    * information to be added to the record for this serial port server.
    * @param ServiceName Name to appear in the SDP Database for this service.
    * @param SDPServiceRecordHandle Returned handle to the SDP Database
    * entry which may be used to remove the entry at a later time.
    * @return If this function returns zero, then the
    * SDPServiceRecordHandle entry will contain the Service Record
    * Handle of the added SDP Service Record.  If this function fails,
    * a negative return error code will be returned (see BTERRORS.H)
    * and the SDPServiceRecordHandle value will be undefined.
    * \note This function should only be called with the OTP Port ID
    *          that was returned from the OTP_Open_Server_Port()
    *          function.  This function should NEVER be used with the
    *          OTP Port ID returned from the OTP_Open_Remote_Port()
    *          function.
    * \note The Service Record Handle that is returned from this
    *          function will remain in the SDP Record Database until
    *          it is deleted by calling the SDP_Delete_Service_Record()
    *          function.
    * \note A MACRO is provided to Delete the Service Record from
    *          the SDP Data Base.  This MACRO maps the
    *          OTP_Un_Register_SDP_Record() to
    *          SDP_Delete_Service_Record().
    * \note There must be UUID Information specified in the
    *          SDPServiceRecord Parameter, however protocol information
    *          is completely optional.  Any Protocol Information that is
    *          specified (if any) will be added in the Protocol
    *          Attribute AFTER the default OBEX Protocol List (L2CAP,
    *          RFCOMM, and OBEX).
    * \note The Service Name is always added at Attribute ID 0x0100.
    *          A Language Base Attribute ID List is created that
    *          specifies that 0x0100 is UTF-8 Encoded, English Language.
    */
BTPSAPI_DECLARATION int BTPSAPI OTP_Register_SDP_Record(unsigned int BluetoothStackID, unsigned int OTP_ID, OTP_SDP_Service_Record_t *SDPServiceRecord, char *ServiceName, DWord_t *SDPServiceRecordHandle);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_OTP_Register_SDP_Record_t)(unsigned int BluetoothStackID, unsigned int OTP_ID, OTP_SDP_Service_Record_t *SDPServiceRecord, char *ServiceName, DWord_t *SDPServiceRecordHandle);
#endif

   /*! @brief The following function is provided to allow a means to add a
    * Generic raw OBEX Service Record to the SDP Database.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param OTP_ID The port this command applies to.  This is the value
    * that was returned from the OTP_Open_Server_Port( ) function.
    * @param SDPServiceRecord Any additional Service Discovery Protocol
    * information to be added to the record for this serial port server.
    * @param ServiceName Name to appear in the SDP Database for this service.
    * @param SDPServiceRecordHandle Returned handle to the SDP Database
    * entry which may be used to remove the entry at a later time.
    * @return If this function returns zero, then the
    * SDPServiceRecordHandle entry will contain the Service Record
    * Handle of the added SDP Service Record.  If this function fails, a
    * negative return error code will be returned (see BTERRORS.H) and
    * the SDPServiceRecordHandle value will be undefined.
    * \note This function should only be called with the OTP Port ID
    *          that was returned from the OTP_Open_Server_Port()
    *          function.  This function should NEVER be used with the
    *          OTP Port ID returned from the OTP_Open_Remote_Port()
    *          function.
    * \note The Service Record Handle that is returned from this
    *          function will remain in the SDP Record Database until it
    *          is deleted by calling the SDP_Delete_Service_Record()
    *          function.
    * \note A MACRO is provided to Delete the Service Record from the
    *          SDP Data Base.  This MACRO maps the
    *          OTP_Un_Register_SDP_Record() to
    *          SDP_Delete_Service_Record().
    * \note There must be UUID Information specified in the
    *          SDPServiceRecord Parameter, however protocol information
    *          is completely optional.  Any Protocol Information that is
    *          specified (if any) will be added in the Protocol
    *          Attribute AFTER the default OBEX Protocol List (L2CAP,
    *          RFCOMM, and OBEX).
    * \note The Service Name is always added at Attribute ID 0x0100.
    *          A Language Base Attribute ID List is created that
    *          specifies that 0x0100 is UTF-8 Encoded, English Language.
    */
BTPSAPI_DECLARATION int BTPSAPI OTP_Register_Raw_SDP_Record(unsigned int BluetoothStackID, unsigned int OTP_ID, OTP_SDP_Raw_Service_Record_t *SDPServiceRecord, char *ServiceName, DWord_t *SDPServiceRecordHandle);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_OTP_Register_Raw_SDP_Record_t)(unsigned int BluetoothStackID, unsigned int OTP_ID, OTP_SDP_Raw_Service_Record_t *SDPServiceRecord, char *ServiceName, DWord_t *SDPServiceRecordHandle);
#endif

   /*! The following MACRO is a utility MACRO that simply deletes the
    * OTP SDP Service Record (specified by the third parameter) from
    * SDP Database.  This MACRO simply maps to the
    * SDP_Delete_Service_Record() function.  This MACRO is only
    * provided so that the caller doesn't have to sift through the SDP
    * API for very simplistic applications.  This function accepts as
    * input the Bluetooth Stack ID of the Bluetooth Protocol Stack that
    * the Service Record exists on, the OTP Port ID (returned from a
    * successful call to the OTP_Open_Server_Port() function), and the
    * SDP Service Record Handle.  The SDP Service Record Handle was
    * returned via a succesful call to the OTPP_Register_SDP_Record()
    * function.  See the OTP_Register_SDP_Record() function for more
    * information.  This MACRO returns the result of the
    * SDP_Delete_Service_Record() function, which is zero for success
    * or a negative return error code (see BTERRORS.H).
    */
#define OTP_Un_Register_SDP_Record(__BluetoothStackID, __OTPID, __SDPRecordHandle) \
        (SDP_Delete_Service_Record(__BluetoothStackID, __SDPRecordHandle))

   /*! @brief The following function is responsible for Opening a port to a
    * remote Server on the specified Server Port.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param BD_ADDR Address of the Bluetooth device to connect with.
    * @param ServerPort The remote device’s server port ID to connect with.
    * @param MaxPacketLength The largest packet that will be sent/received
    * on this connection. Each side must support a minimum of 255 bytes, and cannot have a packet size greater than 64K- bytes.  These constraints are defined as the constants:
    * OTP_PACKET_LENGTH_MINIMUM and OTP_PACKET_LENGTH_MAXIMUM
    * @param OTP_Event_Callback Function to call when events occur on this port.
    * @param CallbackParameter A user-defined parameter (e.g., a tag value)
    * that will be passed back to the user in the callback function with each packet.
    * @return This function returns a non-zero, positive, value if successful, or a negative
    * return error code if this function is unsuccessful.  If this
    * function is successful, the return value will represent the OTP ID
    * that can be passed to all other functions that require it.  Once a
    * remote server is opened, it can only be closed via a call to the
    * OTP_Close_Port() function (passing the return value from this
    * function).
    */
BTPSAPI_DECLARATION int BTPSAPI OTP_Open_Remote_Port(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, unsigned int ServerPort, Word_t MaxPacketLength, OTP_Event_Callback_t OTP_Event_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_OTP_Open_Remote_Port_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, unsigned int ServerPort, Word_t MaxPacketLength, OTP_Event_Callback_t OTP_Event_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is used to terminate a possible connection
    * to a remote server or client.  If this is called by a Server, the
    * connection to the client will be terminated, but the Server will
    * remain registered.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param OTP_ID The port to close. This is the value that was returned
    * from the OTP_Open_Server_Port( ) or OTP_Open_Remote_Port( ) function.
    * @return This function returns zero if
    * successful, or a negative return value if there was an error.
    * This function does NOT Un-Register a Server Port from the system,
    * it ONLY disconnects any connection that is currently active on the
    * Server Port.  The OTP_Close_Server_Port() function can be used to
    * Un-Register the Server.
    */
BTPSAPI_DECLARATION int BTPSAPI OTP_Close_Port(unsigned int BluetoothStackID, unsigned int OTP_ID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_OTP_Close_Port_t)(unsigned int BluetoothStackID, unsigned int OTP_ID);
#endif

   /*! @brief The following function is used to Send an OBEX Connect Request to
    * the Remote Server.  The remote server is referenced by the OTP ID
    * that was returned from an OTP_Open_Remote_Port.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param OTP_ID The port to use.
    * @param Target The service on the remote server to which the
    * connection is targeted.
    * @param DigestChallenge Used along with DigestResponse to pass
    * Authentication Request and Response information between Server and Clients.
    * These parameters should be set to NULL if authentication is not in use.
    * @param DigestResponse This is defined by the following structure:
    * typedef struct
    * {
    *    Byte_t        RequestDigest[OTP_DIGEST_MAXIMUM_REQUEST_DIGEST_LENGTH];
    *    Byte_t        OptionalParametersMask;
    *    unsigned int  UserIDLength;
    *    Byte_t        UserID[OTP_DIGEST_MAXIMUM_USER_ID_LENGTH];
    *    Byte_t        Nonce[OTP_DIGEST_MAXIMUM_NONCE_LENGTH];
    * } OTP_Digest_Response_t;
    * The RequestDigest field is mandatory and must be 6 bytes and, similarly,
    * the UserID has been limited in size in this implementation (as defined by the constants shown).
    * @return This function returns zero if successful, or a negative return value if there
    * was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI OTP_Client_Connect(unsigned int BluetoothStackID, unsigned int OTP_ID, OTP_Target_t Target, OTP_Digest_Challenge_t *DigestChallenge, OTP_Digest_Response_t *DigestResponse);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_OTP_Client_Connect_t)(unsigned int BluetoothStackID, unsigned int OTP_ID, OTP_Target_t Target, OTP_Digest_Challenge_t *DigestChallenge, OTP_Digest_Response_t *DigestResponse);
#endif

   /*! @brief The following function is used to Disconnect an OBEX connection.
    * This function will disconnect from an OBEX service on the remote
    * OBEX server without releasing the connection to the Server.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param OTP_ID The port on which to close the connection. This is the
    * value that was returned from either the OTP_Open_Remote_Port( ).
    * @return This function returns zero if successful, or a
    * negative return value if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI OTP_Client_Disconnect(unsigned int BluetoothStackID, unsigned int OTP_ID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_OTP_Client_Disconnect_t)(unsigned int BluetoothStackID, unsigned int OTP_ID);
#endif

   /*! @brief The following function is used to request a directory listing from
    * a remote OBEX File Browsing Server.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param OTP_ID The OBEX connection on which to issue the request.
    * This is the value that was returned from the OTP_Open_Remote_Port() function.
    * @param Name A pointer to a ASCIIZ string that identifies the name of
    * the directory that is to be retreived.  When specifying the Name, No path
    * information is allowed.  When retreiving a directory listing, the SETPATH
    * function should be used to set the current directory.  This function is then
    * called with the Name parameter set to NULL to pull the current directory.
    * If the Name parameter is not NULL, then Name must point to a ASCIIZ string
    * of the name of a sub-directory that exists off the current directory.
    * It must also be noted that when the Name parameter is used, a sub-directory
    * listing will be returned for the directory specified, however, the current
    * directory will remain the same and will not be changed to the sub-directory specified.
    * @return This function returns zero if successful, or a negative return value if there
    * was an error.
    * \note The Name parameter should be formatted as a NULL
    *          terminated ASCII string with UTF-8 encoding.
    */
BTPSAPI_DECLARATION int BTPSAPI OTP_Client_Get_Directory(unsigned int BluetoothStackID, unsigned int OTP_ID, char *Name);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_OTP_Client_Get_Directory_t)(unsigned int BluetoothStackID, unsigned int OTP_ID, char *Name);
#endif

   /*! @brief The following function is used to Pull and Object from a Remote
    * OBEX Server.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param OTP_ID The OBEX connection on which to issue the request.  This is the
    * value that was returned from the OTP_Open_Remote_Port() function.
    * @param Type A pointer to a NULL terminated string that describes the type of
    * object to be retreived
    * @param Name A pointer to a NULL terminated string that specifies the Name of
    * the Object that is to be retreived.
    * \note When connected to an OBEX File Browser Service, the Type parameter is
    * optional.  When connected to the OBEX Inbox, the Name parameter is optional.
    * @param UserInfo A user-defined parameter that will be passed back in the
    * event callback.
    * @return This function returns zero if successful, or a negative
    * return value if there was an error.  It should be noted that when
    * connected to an OBEX File Browser Service, the Type parameter is
    * optional.  When connected to the OBEX Inbox, the Name parameter is
    * optional.
    * \note The Type and Name parameters should be formatted as NULL
    *          terminated ASCII strings with UTF-8 encoding.
    */
BTPSAPI_DECLARATION int BTPSAPI OTP_Client_Get_Object(unsigned int BluetoothStackID, unsigned int OTP_ID, char *Type, char *Name, unsigned long UserInfo);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_OTP_Client_Get_Object_t)(unsigned int BluetoothStackID, unsigned int OTP_ID, char *Type, char *Name, unsigned long UserInfo);
#endif

   /*! @brief The following function is used to send a request to an OBEX Server
    * to save or create an object on the Server.'
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param OTP_ID The OBEX connection on which to issue the request.
    * This is the value that was returned from the OTP_Open_Remote_Port() function.
    * @param CreateOnly Specifies whether or not this request is being
    * made as an introduction to putting an object (CreateOnly equals FALSE), or to simply create an object of zero length (CreateOnly equals TRUE).
    * @param Length The Length (in Bytes) of the actual Object that is
    * to be placed on the Remote Server.
    * @param Type A pointer to a NULL terminated string that describes
    * the type of object to be retreived.  This is NULL for files or a
    * string that defines the Object Type (for example “text/x-vCard” to put a vCard Object).
    * This field is only used if the Target is not a File Browser.
    * @param Name A pointer to a NULL terminated string that specifies
    * the Name of the Object that is to be sent.
    * @param UserInfo A user-defined parameter that will be passed back
    * in the event callback.
    * @return This function returns zero
    * if successful, or a negative return value if there was an error.
    * It should be noted that when connected to an OBEX File Browser
    * Service, the Type parameter is optional.  When connected to the
    * OBEX Inbox, the Name parameter is optional.
    * \note The Type and Name parameters should be formatted as NULL
    *          terminated ASCII strings with UTF-8 encoding.
    */
BTPSAPI_DECLARATION int BTPSAPI OTP_Client_Put_Object_Request(unsigned int BluetoothStackID, unsigned int OTP_ID, Boolean_t CreateOnly, unsigned int Length, char *Type, char *Name, unsigned long UserInfo);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_OTP_Client_Put_Object_Request_t)(unsigned int BluetoothStackID, unsigned int OTP_ID, Boolean_t CreateOnly, unsigned int Length, char *Type, char *Name, unsigned long UserInfo);
#endif

   /*! @brief The following function is used to send a request to an OBEX Sync
    * Server to modify or create an object on the Server.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param OTP_ID The OBEX connection on which to issue the request. This is the
    * value that was returned from the OTP_Open_Remote_Port() function.
    * @param Length The Length (in Bytes) of the actual Object that is to be
    * placed on the Remote Server.
    * @param Name A pointer to a NULL terminated string that specifies the
    * Name of the Object that is to be sent.
    * @param SyncAnchor A pointer to structure that contains the Synchronization
    * Anchor information for the Object.
    * @param UserInfo A user-defined parameter that will be passed back in the
    * event callback.
    * @return This function returns zero if
    * successful, or a negative return value if there was an error.
    * \note The Name parameter should be formatted as a NULL
    *          terminated ASCII string with UTF-8 encoding.
    */
BTPSAPI_DECLARATION int BTPSAPI OTP_Client_Put_Sync_Object_Request(unsigned int BluetoothStackID, unsigned int OTP_ID, unsigned int Length, char *Name, SyncAnchor_t *SyncAnchor, unsigned long UserInfo);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_OTP_Client_Put_Sync_Object_Request_t)(unsigned int BluetoothStackID, unsigned int OTP_ID, unsigned int Length, char *Name, SyncAnchor_t *SyncAnchor, unsigned long UserInfo);
#endif

   /*! @brief This function is used to send an object to a remote OBEX Server.
    * This function must be used after an acceptable response is
    * received from the OTP_Client_Put_Object_Request and is used to
    * transfer the object itself.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param OTP_ID The port to send the Put Request to.  This is the
    * value that was returned from the OTP_Open_Remote_Port( ) function.
    * @param DataLength The number of bytes being passed in this call
    * in the Data parameter.
    * @param Data Data to be sent for this object in this call.
    * @param Final Flag which indicates if this is the last packet
    * of the Put sequence or not.
    * @param UserInfo A user-defined parameter that will be passed
    * back in the event callback.
    * @return This function returns zero if successful, or a negative return value if
    * there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI OTP_Client_Put_Object(unsigned int BluetoothStackID, unsigned int OTP_ID, unsigned int DataLength, Byte_t *Data, Boolean_t Final, unsigned long UserInfo);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_OTP_Client_Put_Object_t)(unsigned int BluetoothStackID, unsigned int OTP_ID, unsigned int DataLength, Byte_t *Data, Boolean_t Final, unsigned long UserInfo);
#endif

   /*! @brief The following function is used to create, delete or set the
    * current directory, of remote OBEX server supplying File Browsing
    * Services.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param OTP_ID The port to send the Get Request to.
    * This is the value that was returned from the OTP_Open_Remote_Port( ) function.
    * @param Name A pointer to a NULL terminated string of
    * the path to the sub-directory referenced from the current directory.
    * @param Backup Go back up one level in the directory
    * structure.  When this is set to TRUE, it takes priority over
    * the Name parameter which is ignored in this situation.
    * @param Create Whether or not to create the directory
    * if it does not already exist. The Name parameter must be supplied if TRUE.
    * @return This function returns zero if successful, or a negative return
    * value if there was an error.  Note that the Backup flag has the
    * highest priority and The Name parameter will be ignored when
    * Backup is set TRUE.  Also, when the Create parameter is TRUE the
    * Name parameter must also be specified.
    * \note The Name parameter should be formatted as a NULL
    *          terminated ASCII string with UTF-8 encoding.
    */
BTPSAPI_DECLARATION int BTPSAPI OTP_Client_Set_Path(unsigned int BluetoothStackID, unsigned int OTP_ID, char *Name, Boolean_t Backup, Boolean_t Create);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_OTP_Client_Set_Path_t)(unsigned int BluetoothStackID, unsigned int OTP_ID, char *Name, Boolean_t Backup, Boolean_t Create);
#endif

   /*! @brief The following function is used to send a request to an OBEX Server
    * to delete an object on the Server.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param OTP_ID The OBEX connection on which to issue the request.
    * This is the value that was returned from the OTP_Open_Remote_Port() function.
    * @param Name A pointer to a NULL terminated string that indicates
    * the object to be deleted.
    * @return This function returns
    * zero if successful, or a negative return value if there was an
    * error.
    * \note The Name parameter should be formatted as a NULL
    *          terminated ASCII string with UTF-8 encoding.
    */
BTPSAPI_DECLARATION int BTPSAPI OTP_Client_Delete_Object_Request(unsigned int BluetoothStackID, unsigned int OTP_ID, char *Name);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_OTP_Client_Delete_Object_Request_t)(unsigned int BluetoothStackID, unsigned int OTP_ID, char *Name);
#endif

   /*! @brief The following function is used to send a request to an OBEX Sync
    * Server to delete an object from the Servers Object Store.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param OTP_ID The OBEX connection on which to issue the request.
    * This is the value that was returned from the OTP_Open_Remote_Port() function.
    * @param Name A pointer to a NULL terminated string that indicates
    * the object to be deleted.
    * @param SyncAnchor A pointer to a Syncronization Anchor to use.
    * This member only has meaning if the SyncAnchor type is of type Change Counter.
    * This action then allows the remote OBEX Sync entity the ability to
    * allow/reject the delete based on the remote OBEX Sync servers Current Change
    * Count for the Object.  This value should be the expected Change Count of
    * the Object AFTER the delete is successful (i.e. not the current Change Count value).
    * @param HardDelete A Boolean_t flag which specifies whether the
    * delete is Hard Delete (TRUE) or Soft Delete (FALSE).
    * @return This function returns zero if successful, or a negative return value if there
    * was an error.
    * \note The Name parameter should be formatted as a NULL
    *          terminated ASCII string with UTF-8 encoding.
    */
BTPSAPI_DECLARATION int BTPSAPI OTP_Client_Delete_Sync_Object_Request(unsigned int BluetoothStackID, unsigned int OTP_ID, char *Name, SyncAnchor_t *SyncAnchor, Boolean_t HardDelete);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_OTP_Client_Delete_Sync_Object_Request_t)(unsigned int BluetoothStackID, unsigned int OTP_ID, char *Name, SyncAnchor_t *SyncAnchor, Boolean_t HardDelete);
#endif

   /*! @brief The following function is used to Abort a request to the remote
    * server that is outstanding.
    * BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * OTP_ID The OBEX connection on which to issue the request. This is the
    * value that was returned from the OTP_Open_Remote_Port() function.
    * @return This function returns zero if successful, or a negative return value if there
    * was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI OTP_Client_Abort_Request(unsigned int BluetoothStackID, unsigned int OTP_ID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_OTP_Client_Abort_Request_t)(unsigned int BluetoothStackID, unsigned int OTP_ID);
#endif

   /*! @brief The following function is used to send a response to a remote
    * client due to a request for connection.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param OTP_ID The OBEX connection on which to issue the request.
    * This is the value that was returned from the OTP_Open_Remote_Port() function.
    * @param Accept Whether to accept the connection or not.
    * @param DigestChallenge With DigestResponse are used for authentification.
    * If authentification is not being used, both parameters are set to NULL.
    * See The OTP_Client_Connect() command for information the data structure of this parameter.
    * @param DigestResponse See The OTP_Client_Connect() command for information
    * the data structure of this parameter.
    * @return This function returns
    * zero if successful, or a negative return value if there was an
    * error.
    */
BTPSAPI_DECLARATION int BTPSAPI OTP_Connect_Response(unsigned int BluetoothStackID, unsigned int OTP_ID, Boolean_t Accept, OTP_Digest_Challenge_t *DigestChallenge, OTP_Digest_Response_t *DigestResponse);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_OTP_Connect_Response_t)(unsigned int BluetoothStackID, unsigned int OTP_ID, Boolean_t Accept, OTP_Digest_Challenge_t *DigestChallenge, OTP_Digest_Response_t *DigestResponse);
#endif

   /*! @brief The following function is used to send a response to a remote
    * client due to a request for a Directory listing.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param OTP_ID The OBEX connection on which to issue the request.
    * This is the value that was returned from the OTP_Open_Remote_Port() function.
    * @param DirInfo The parameter DirEntry is a pointer to an array
    * of directory entrystructures.  Each entry in the array contains
    * information about a file or directory entry that is to be sent in response to the request.  It is important to note that the stack receives the directory information as an array of structures, and will convert this information into XML format prior to sending to information  to the remote client.  The process of converting the data to XML and sending all of the information to the remote client may require multiple requests and responses from the client and server.  The lower layer stack will handle all of these additionaltransactions without any further interaction from the application.  Since the directory transfer process may take some time to complete, the data pointed to by the parameter DirInfo must be preserved until the transfer process is complete.  When the DirInfo information is no longer needed by the lower stack, a Callback will be generated with the etOTP_Free_Directory_Information event to inform the application that the directory transfer process is complete and the data can be freed.
    * @param ResponseCode The parameter ResponseCode is used to notify
    * the remote client of its ability to satisfy the request.
    * If the ResponseCode value is non-Zero, then the information pointed
    * to by the DirInfo parameter is considered invalid and the ResponseCode
    * value represents the OBEX result code that identifies the reason why
    * the request was not processed.
    * The possible ResponseCode values are listed earlier in this section
    * (before the first _Response) function.
    * @return This function returns zero if
    * successful, or a negative return value if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI OTP_Get_Directory_Request_Response(unsigned int BluetoothStackID, unsigned int OTP_ID, OTP_DirectoryInfo_t *DirInfo, Byte_t ResponseCode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_OTP_Get_Directory_Request_Response_t)(unsigned int BluetoothStackID, unsigned int OTP_ID, OTP_DirectoryInfo_t *DirInfo, Byte_t ResponseCode);
#endif

   /*! @brief The following function is used to respond to a request from a
    * remote Client to Change to a new directory, Create, or Delete a
    * directory.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param OTP_ID The OBEX connection on which to issue the request.
    * This is the value that was returned from the OTP_Open_Remote_Port() function.
    * @param ResponseCode The parameter ResponseCode is used to notify
    * the remote client of its ability to satisfy the request.
    * The possible ResponseCode values are listed earlier in this
    * section (before the first _Response) function.
    * @return This function returns zero if successful, or a
    * negative return value if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI OTP_Set_Path_Response(unsigned int BluetoothStackID, unsigned int OTP_ID, Byte_t ResponseCode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_OTP_Set_Path_Response_t)(unsigned int BluetoothStackID, unsigned int OTP_ID, Byte_t ResponseCode);
#endif

   /*! @brief The following function is used to respond to a request from a
    * remote Client to Abort an operation.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param OTP_ID The OBEX connection on which to issue the request. This is
    * the value that was returned from the OTP_Open_Remote_Port() function.
    * @return This function returns zero if successful, or a negative
    * return value if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI OTP_Abort_Response(unsigned int BluetoothStackID, unsigned int OTP_ID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_OTP_Abort_Response_t)(unsigned int BluetoothStackID, unsigned int OTP_ID);
#endif

   /*! @brief The following function is used to respond to a request from a
    * remote Client to Get an Object.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param OTP_ID The OBEX connection on which to issue the request. This
    * is the value that was returned from the OTP_Open_Remote_Port() function.
    * @param BytesToSend When the request was made, the Server received a
    * Get Request event which included a pointer to a buffer where the
    * data was to be loaded.  This buffer was referenced in the structure OTP_Info_t.
    * The number of bytes that was loaded into this buffer is what is placed into BytestoSend.
    * @param ResponseCode The parameter ResponseCode is used to notify the
    * remote client of its ability to satisfy the request. If
    * the ResponseCode value is non-Zero, then any other information
    * is considered invalid and the ResponseCode value represents the OBEX
    * result code that identifies the reason why the request was not processed.
    * The possible ResponseCode values are listed earlier in this section
    * (before the first _Response) function.
    * @param UserInfo A user-defined parameter that will be passed back in
    * the next Get Request event.
    * @return This function returns zero if successful, or a
    * negative return value if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI OTP_Get_Object_Response(unsigned int BluetoothStackID, unsigned int OTP_ID, unsigned int BytesToSend, unsigned int ResponseCode, unsigned long UserInfo);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_OTP_Get_Object_Response_t)(unsigned int BluetoothStackID, unsigned int OTP_ID, unsigned int BytesToSend, unsigned int ResponseCode, unsigned long UserInfo);
#endif

   /*! @brief The following function is used to respond to a request from a
    * remote Client to Delete an Object.

    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param OTP_ID The OBEX connection on which to issue the request.
    * This is the value that was returned from the OTP_Open_Remote_Port() function.
    * @param ResponseCode The parameter ResponseCode is used to notify
    * the remote client of its ability to satisfy the request.
    *If the ResponseCode value is non-Zero, then any other information
    * is considered invalid and the ResponseCode value represents the OBEX
    * result code that identifies the reason why the request was not processed.
    * The possible ResponseCode values are listed earlier in this section
    * (before the first _Response) function.
    * @return This function returns zero if successful, or a negative return value
    * if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI OTP_Delete_Object_Response(unsigned int BluetoothStackID, unsigned int OTP_ID, Byte_t ResponseCode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_OTP_Delete_Object_Response_t)(unsigned int BluetoothStackID, unsigned int OTP_ID, Byte_t ResponseCode);
#endif

   /*! @brief The following function is used to respond to a request from a
    * remote Sync Client to Delete an Object.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param OTP_ID The OBEX connection on which to issue the request.
    *  This is the value that was returned from the OTP_Open_Remote_Port() function.
    * @param ResponseCode The parameter ResponseCode is used to notify
    * the remote client of its ability to satisfy the request.
    * If the ResponseCode value is non-Zero, then any other information
    * is considered invalid and the ResponseCode value represents the OBEX
    * result code that identifies the reason why the request was not processed.
    * The possible ResponseCode values are listed earlier in this section
    * (before the first _Response) function.
    * @param UID A pointer to a NULL terminated ASCII string that
    * specifies the local UID of the Object that was deleted.
    * @param SyncAnchor A pointer to the SyncAnchor to return in
    * the delete response (either Change Count or Timestamp) .
    * @return This function returns zero if successful, or a negative return value if
    * there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI OTP_Delete_Sync_Object_Response(unsigned int BluetoothStackID, unsigned int OTP_ID, Byte_t ResponseCode, char *UID, SyncAnchor_t *SyncAnchor);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_OTP_Delete_Sync_Object_Response_t)(unsigned int BluetoothStackID, unsigned int OTP_ID, Byte_t ResponseCode, char *UID, SyncAnchor_t *SyncAnchor);
#endif

   /*! @brief The following function is used to respond to a request from a
    * remote Client to Put an Object.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param OTP_ID The OBEX connection on which to issue the request.
    * This is the value that was returned from the OTP_Open_Remote_Port() function.
    * @param ResponseCode The parameter ResponseCode is used to notify
    * the remote client of its ability to satisfy the request.
    * If the ResponseCode value is non-Zero, then any other information
    * is considered invalid and the ResponseCode value represents the OBEX
    * result code that identifies the reason why the request was not processed.
    * The possible ResponseCode values are listed earlier in this section
    * (before the first _Response) function.
    * @return This function returns zero if
    * successful, or a negative return value if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI OTP_Put_Object_Response(unsigned int BluetoothStackID, unsigned int OTP_ID, Byte_t ResponseCode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_OTP_Put_Object_Response_t)(unsigned int BluetoothStackID, unsigned int OTP_ID, Byte_t ResponseCode);
#endif

   /*! @brief The following function is used to respond to a request from a
    * remote Sync Client to Put an Object.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param OTP_ID The OBEX connection on which to issue the request.
    * This is the value that was returned from the OTP_Open_Remote_Port() function.
    * @param ResponseCode The parameter ResponseCode is used to notify
    * the remote client of its ability to satisfy the request.
    * If the ResponseCode value is non-Zero, then any other information
    * is considered invalid and the ResponseCode value represents the
    * OBEX result code that identifies the reason why the request was not processed.
    * The possible ResponseCode values are listed earlier in this section
    * (before the first _Response) function.
    * @param UID A pointer to a NULL terminated ASCII string that specifies
    * the local UID of the Object that was deleted.
    * @param SyncAnchor A pointer to the SyncAnchor to return in the delete
    * response (either Change Count or Timestamp).
    * @return This function returns zero if successful, or a negative return value if
    * there was an error.
    * \note The UID parameter should be formatted as a NULL
    *          terminated ASCII string with UTF-8 encoding.
    */
BTPSAPI_DECLARATION int BTPSAPI OTP_Put_Sync_Object_Response(unsigned int BluetoothStackID, unsigned int OTP_ID, Byte_t ResponseCode, char *UID, SyncAnchor_t *SyncAnchor);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_OTP_Put_Sync_Object_Response_t)(unsigned int BluetoothStackID, unsigned int OTP_ID, Byte_t ResponseCode, char *UID, SyncAnchor_t *SyncAnchor);
#endif

   /*! @brief The following function is responsible for allowing a mechanism to
    * query the OTP Port Server Connection Mode.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param OTP_ID The port this command applies to. This is the value that was
    * returned from the OTP_Open_Server_Port( ) function.
    * @param ServerConnectionMode Pointer to a variable to receive the current
    * Server Connection Mode.
    * @return This function returns zero if successful, or a negative return value if there
    * was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI OTP_Get_Server_Connection_Mode(unsigned int BluetoothStackID, unsigned int OTP_ID, SPP_Server_Connection_Mode_t *ServerConnectionMode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_OTP_Get_Server_Connection_Mode_t)(unsigned int BluetoothStackID, unsigned int OTP_ID, SPP_Server_Connection_Mode_t *ServerConnectionMode);
#endif

   /*! @brief The following function is responsible for allowing a mechanism to
    * change the current OTP Port Server Connection Mode.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param OTP_ID The port this command applies to. This is the
    * value that was returned from the OTP_Open_Server_Port() function.
    * @param ServerConnectionMode The new Server Connection Mode being set.
    * @return This function returns zero if successful, or a negative return value if there
    * was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI OTP_Set_Server_Connection_Mode(unsigned int BluetoothStackID, unsigned int OTP_ID, SPP_Server_Connection_Mode_t ServerConnectionMode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_OTP_Set_Server_Connection_Mode_t)(unsigned int BluetoothStackID, unsigned int OTP_ID, SPP_Server_Connection_Mode_t ServerConnectionMode);
#endif

#endif
