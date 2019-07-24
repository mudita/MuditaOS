/*
 * Copyright 2007 - 2014 Stonestreet One.
 * All Rights Reserved.
 * Author: Josh Toole
 *** MODIFICATION HISTORY ****************************************************
 *
 *   mm/dd/yy  F. Lastname    Description of Modification
 *   --------  -----------    ------------------------------------------------
 *   12/18/07  J. Toole       Initial creation.
 ****************************************************************************
 */

/**
 *  @file PBAPAPI.h
 *
 *  @brief Stonestreet One Bluetooth Phone Book Access Profile (Version 1.0) API Type
 *            Definitions, Constants, and Prototypes.
 *
 *  The PBAP interface provides APIs, data types,
 *  and macros. The header file should be included in an application as
 *  follows:
 *
 *  @code
 *  #include "SS1BTPBA.h"
 *  @endcode
 *
 *  The Phone Book Access Profile, PBAP, programming interface defines the
 *  protocols and procedures to be used to implement PBAP capabilities
 *  for both Server and Client services.
 *
 *  ============================================================================
 */
#ifndef __PBAPAPIH__
#define __PBAPAPIH__

#include "SS1BTPS.h"

   /*! Bluetooth Stack API Prototypes/Constants. */
   /*! Error Return Codes.
    * Error Codes that are smaller than these (less than -1000) are
    * related to the Bluetooth Protocol Stack itself (see BTERRORS.H).
    */
#define BTPBAP_ERROR_INVALID_PARAMETER                           (-1000)
#define BTPBAP_ERROR_NOT_INITIALIZED                             (-1001)
#define BTPBAP_ERROR_INVALID_BLUETOOTH_STACK_ID                  (-1002)
#define BTPBAP_ERROR_LIBRARY_INITIALIZATION_ERROR                (-1003)
#define BTPBAP_ERROR_INSUFFICIENT_RESOURCES                      (-1004)
#define BTPBAP_ERROR_REQUEST_ALREADY_OUTSTANDING                 (-1005)
#define BTPBAP_ERROR_ACTION_NOT_ALLOWED                          (-1006)

   /* SDP Service Classes for the Phonebook Access Profile.
    * The following MACRO is a utility MACRO that assigns the PBAP
    * Profile ID Bluetooth Universally Unique Identifier to the
    * specified UUID_16_t variable.  This MACRO accepts one parameter
    * which is the UUID_16_t variable that is to receive the Constant
    * value.
    */
#define SDP_ASSIGN_PBAP_PROFILE_UUID_16(_x)             ASSIGN_SDP_UUID_16((_x), 0x11, 0x30)

   /*! The following MACRO is a utility MACRO that assigns the PBAP
    * Profile ID Bluetooth Universally Unique Identifier to the
    * specified UUID_32_t variable.  This MACRO accepts one parameter
    * which is the UUID_32_t variable that is to receive the Constant
    * value.
    */
#define SDP_ASSIGN_PBAP_PROFILE_UUID_32(_x)             ASSIGN_SDP_UUID_32((_x), 0x00, 0x00, 0x11, 0x30)

   /*! The following MACRO is a utility MACRO that assigns the PBAP
    * Profile ID Bluetooth Universally Unique Identifier to the
    * specified UUID_128_t variable.  This MACRO accepts one parameter
    * which is the UUID_128_t variable that is to receive the Constant
    * value.
    */
#define SDP_ASSIGN_PBAP_PROFILE_UUID_128(_x)            ASSIGN_SDP_UUID_128((_x), 0x00, 0x00, 0x11, 0x30, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /* SDP Service Classes for the Phonebook Access Profile.
    * The following MACRO is a utility MACRO that assigns the PBAP
    * Profile Client Bluetooth Universally Unique Identifier to the
    * specified UUID_16_t variable.  This MACRO accepts one parameter
    * which is the UUID_16_t variable that is to receive the Constant
    * value.
    */
#define SDP_ASSIGN_PBAP_CLIENT_UUID_16(_x)              ASSIGN_SDP_UUID_16((_x), 0x11, 0x2E)

   /*! The following MACRO is a utility MACRO that assigns the PBAP
    * Profile Client Bluetooth Universally Unique Identifier to the
    * specified UUID_32_t variable.  This MACRO accepts one parameter
    * which is the UUID_32_t variable that is to receive the Constant
    * value.
    */
#define SDP_ASSIGN_PBAP_CLIENT_UUID_32(_x)              ASSIGN_SDP_UUID_32((_x), 0x00, 0x00, 0x11, 0x2E)

   /*! The following MACRO is a utility MACRO that assigns the PBAP
    * Profile Client Bluetooth Universally Unique Identifier to the
    * specified UUID_128_t variable.  This MACRO accepts one parameter
    * which is the UUID_128_t variable that is to receive the Constant
    * value.
    */
#define SDP_ASSIGN_PBAP_CLIENT_UUID_128(_x)             ASSIGN_SDP_UUID_128((_x), 0x00, 0x00, 0x11, 0x2E, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /*! The following MACRO is a utility MACRO that assigns the PBAP
    * Profile Server Bluetooth Universally Unique Identifier to the
    * specified UUID_16_t variable.  This MACRO accepts one parameter
    * which is the UUID_16_t variable that is to receive the Constant
    * value.
    */
#define SDP_ASSIGN_PBAP_SERVER_UUID_16(_x)              ASSIGN_SDP_UUID_16((_x), 0x11, 0x2F)

   /*! The following MACRO is a utility MACRO that assigns the PBAP
    * Profile Server Bluetooth Universally Unique Identifier to the
    * specified UUID_32_t variable.  This MACRO accepts one parameter
    * which is the UUID_32_t variable that is to receive the Constant
    * value.
    */
#define SDP_ASSIGN_PBAP_SERVER_UUID_32(_x)              ASSIGN_SDP_UUID_32((_x), 0x00, 0x00, 0x11, 0x2F)

   /*! The following MACRO is a utility MACRO that assigns the PBAP
    * Profile Server Bluetooth Universally Unique Identifier to the
    * specified UUID_128_t variable.  This MACRO accepts one parameter
    * which is the UUID_128_t variable that is to receive the Constant
    * value.
    */
#define SDP_ASSIGN_PBAP_SERVER_UUID_128(_x)             ASSIGN_SDP_UUID_128((_x), 0x00, 0x00, 0x11, 0x2F, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /*! The following definitions represent the defined Supported
    * Repositories that can be supported by a PBAP Server.  These SDP
    * Definitions are used with the Bluetooth Supported Repositories
    * Attribute.
    */
#define SDP_PBAP_SUPPORTED_REPOSITORIES_LOCAL_PHONEBOOK           0x01
#define SDP_PBAP_SUPPORTED_REPOSITORIES_SIM_CARD_PHONEBOOK        0x02

   /*! The following constants represent the Minimum and Maximum Port
    * Numbers that can be opened (both locally and remotely).  These
    * constants specify the range for the Port Number parameters in the
    * Open Functions.
    */
#define PBAP_PORT_NUMBER_MINIMUM                                 (SPP_PORT_NUMBER_MINIMUM)
#define PBAP_PORT_NUMBER_MAXIMUM                                 (SPP_PORT_NUMBER_MAXIMUM)

   /*! The following constants are the OBEX response codes that are
    * specifically allowed in the PBAP specification.  The error codes
    * for Not Found, Service Unavailable, Bad Request, and Not
    * Implemented are all specifically mandatory.  All others listed
    * here are optional.  Other OBEX error codes not listed here are
    * specifically DISALLOWED by the specification.
    */
#define PBAP_OBEX_RESPONSE_CONTINUE                              (OBEX_CONTINUE_RESPONSE)
#define PBAP_OBEX_RESPONSE_OK                                    (OBEX_OK_RESPONSE)
#define PBAP_OBEX_RESPONSE_NOT_FOUND                             (OBEX_NOT_FOUND_RESPONSE)
#define PBAP_OBEX_RESPONSE_SERVICE_UNAVAILABLE                   (OBEX_SERVICE_UNAVAILABLE_RESPONSE)
#define PBAP_OBEX_RESPONSE_BAD_REQUEST                           (OBEX_BAD_REQUEST_RESPONSE)
#define PBAP_OBEX_RESPONSE_NOT_IMPLEMENTED                       (OBEX_NOT_IMPLEMENTED_RESPONSE)
#define PBAP_OBEX_RESPONSE_UNAUTHORIZED                          (OBEX_UNAUTHORIZED_RESPONSE)
#define PBAP_OBEX_RESPONSE_PRECONDITION_FAILED                   (OBEX_PRECONDITION_FAILED_RESPONSE)
#define PBAP_OBEX_RESPONSE_NOT_ACCEPTABLE                        (OBEX_NOT_ACCEPTABLE_RESPONSE)
#define PBAP_OBEX_RESPONSE_FORBIDDEN                             (OBEX_FORBIDDEN_RESPONSE)

   /*! The following define the OBEX Response Final Bit to be appended to
    * the response codes passed into function of this module.
    */
#define PBAP_OBEX_RESPONSE_FINAL_BIT                             (OBEX_FINAL_BIT)

   /*! The following BIT definitions are used to denote the possible PBAP
    * Profile Server Modes that can be applied to a PBAP Profile Client
    * Connection.  These BIT definitions are used with the
    * PBAP_Set_Server_Mode() and PBAP_Get_Server_Mode() mode functions.
    */
#define PBAP_SERVER_MODE_AUTOMATIC_ACCEPT_CONNECTION             (0x00000000)
#define PBAP_SERVER_MODE_MANUAL_ACCEPT_CONNECTION                (0x00000001)

   /*! The following BIT MASK is used to mask the Server Mode Accept
    * Connection settings from other (undefined) Server Mode bits.
    */
#define PBAP_SERVER_MODE_CONNECTION_MASK                         (0x00000001)

   /*! The following constants represent the PBAP Profile Status Values
    * that are possible in the PBAP Profile Connect Confirmation Event.
    */
#define PBAP_OPEN_STATUS_SUCCESS                                 (0x00)
#define PBAP_OPEN_STATUS_CONNECTION_TIMEOUT                      (0x01)
#define PBAP_OPEN_STATUS_CONNECTION_REFUSED                      (0x02)
#define PBAP_OPEN_STATUS_UNKNOWN_ERROR                           (0x04)

   /*! The following constant defines the PBAP Profile version included
    * in the SDP record for the PBAP profile, as specified in the
    * relevant specification.
    */
#define PBAP_PROFILE_VERSION                                     (0x0100)

   /*! The following constants define the Supported Repositories that are
    * able to be supported by this profile.  These values are used with
    * the PBAP_Open_Server_Port() function.
    */
#define PBAP_SUPPORTED_REPOSITORIES_LOCAL_PHONEBOOK              (SDP_PBAP_SUPPORTED_REPOSITORIES_LOCAL_PHONEBOOK)
#define PBAP_SUPPORTED_REPOSITORIES_SIM_CARD_PHONEBOOK           (SDP_PBAP_SUPPORTED_REPOSITORIES_SIM_CARD_PHONEBOOK)

   /*! The following defines specify the bit locations for the 64-bit
    * filter field specified by the PBAP Profile specification.  Each of
    * these bits can be OR'ed together to form a filter mask that should
    * be passed in the FilterLow parameter where a filter is required.
    * The FilterHigh portion contains proprietary filter settings, if
    * these are not used it should be set to zero.  Refer to the PBAP
    * specification for more information.
    */
#define PBAP_FILTER_VERSION                  (1 << 0)
#define PBAP_FILTER_FN                       (1 << 1)
#define PBAP_FILTER_N                        (1 << 2)
#define PBAP_FILTER_PHOTO                    (1 << 3)
#define PBAP_FILTER_BDAY                     (1 << 4)
#define PBAP_FILTER_ADR                      (1 << 5)
#define PBAP_FILTER_LABEL                    (1 << 6)
#define PBAP_FILTER_TEL                      (1 << 7)
#define PBAP_FILTER_EMAIL                    (1 << 8)
#define PBAP_FILTER_MAILER                   (1 << 9)
#define PBAP_FILTER_TZ                       (1 << 10)
#define PBAP_FILTER_GEO                      (1 << 11)
#define PBAP_FILTER_TITLE                    (1 << 12)
#define PBAP_FILTER_ROLE                     (1 << 13)
#define PBAP_FILTER_LOGO                     (1 << 14)
#define PBAP_FILTER_AGENT                    (1 << 15)
#define PBAP_FILTER_ORG                      (1 << 16)
#define PBAP_FILTER_NOTE                     (1 << 17)
#define PBAP_FILTER_REV                      (1 << 18)
#define PBAP_FILTER_SOUND                    (1 << 19)
#define PBAP_FILTER_URL                      (1 << 20)
#define PBAP_FILTER_UID                      (1 << 21)
#define PBAP_FILTER_KEY                      (1 << 22)
#define PBAP_FILTER_NICKNAME                 (1 << 23)
#define PBAP_FILTER_CATEGORIES               (1 << 24)
#define PBAP_FILTER_PROID                    (1 << 25)
#define PBAP_FILTER_CLASS                    (1 << 26)
#define PBAP_FILTER_SORT_STRING              (1 << 27)
#define PBAP_FILTER_X_IRMC_CALL_DATETIME     (1 << 28)

#define PBAP_FILTER_NONE                     (0)
#define PBAP_FILTER_ALL                      ((1 << 29)-1)

#define PBAP_FILTER_VCARD21_MINIMUM          (PBAP_FILTER_VERSION | PBAP_FILTER_N | PBAP_FILTER_TEL)
#define PBAP_FILTER_VCARD30_MINIMUM          (PBAP_FILTER_VERSION | PBAP_FILTER_N | PBAP_FILTER_FN | PBAP_FILTER_TEL)

   /*! The following define specifies the bit location for the
    * Proprietary Filter bit for the 64-bit Filter bit mask.  Because
    * this implementation handles this 64-bit field as two 32-bit
    * fields, this bit position has been shifted for use in the
    * FilterHigh parameter only.  If a Proprietary Filter is NOT used,
    * the entire FilterHigh parameter should be set to ZERO.  Bits 40-63
    * (shifted by 32 for the FilterHigh parameter) are available for
    * defining proprietary filter bits.  Refer to the PBAP specification
    * for more information.
    */
#define PBAP_FILTER_PROPRIETARY_FILTER_HIGH  (1 << (39-32))

   /*! The following define is the value that is used for the
    * MaxListCount header to indicate that there is not a restriction of
    * the number of items that can be returned.  This is also the
    * default value if this header is not present.
    */
#define PBAP_MAX_LIST_COUNT_NOT_RESTRICTED   (65535)

   /*! The following definition defines the delimiter character that is
    * used to delimit the individual sub-paths in a fully qualified path
    * name.
    */
#define PBAP_PATH_DELIMETER_CHARACTER        '/'

   /*! The following definition defines the path immediately off of the
    * root either (main root or SIM root) that contains the PBAP
    * Information Store.
    */
#define PBAP_TELECOM_PATH_NAME               "telecom"

   /*! The following constant presents the path (off of main/absolute
    * root) that contains the PBAP Information Store that is present on
    * an external SIM Card (as opposed to stored on the phone directly).
    */
#define PBAP_SIM_PATH_NAME                   "SIM"

   /*! The following constants present the path names of all required
    * paths under the PBAP_TELECOM_PATH_NAME that hold the various PBAP
    * Information Stores.
    * \note These path names can also be used as a prefix to fetch
    *          an entire store.  For example, if the caller was
    *          currently operating in the PBAP_TELECOM_PATH_NAME path
    *          (/telecom), the fetching the object
    *          PBAP_PHONEBOOK_PATH_NAME appended with ".vcf" (pb.vcf)
    *          would fetch the entire phonebook.
    */
#define PBAP_PHONEBOOK_PATH_NAME             "pb"
#define PBAP_INCOMING_CALL_HISTORY_PATH_NAME "ich"
#define PBAP_OUTGOING_CALL_HISTORY_PATH_NAME "och"
#define PBAP_MISSED_CALL_HISTORY_PATH_NAME   "mch"
#define PBAP_COMBINED_CALL_HISTORY_PATH_NAME "cch"

   /*! The following enumeration is a list of the valid operations that
    * can be performed using the PBAP_Set_Phonebook operation.  This
    * correspond to flag settings for the OBEX SetPath command.
    */
typedef enum
{
   spRoot,
   spDown,
   spUp
} PBAP_Set_Path_Option_t;

   /*! The following enumeration is a list of valid format values that
    * can be used when pulling vCards from a remote PBAP server.
    */
typedef enum
{
   pfvCard21,
   pfvCard30,
   pfDefault
} PBAP_Format_t;

   /*! The following enumeration is a list of the valid Listing order
    * types that can be sent/received in the Pull vCard Listing request.
    */
typedef enum
{
   loIndexed,
   loAlphabetical,
   loPhonetical,
   loDefault
} PBAP_List_Order_t;

   /*! The following enumeration is a list of the valid search attributes
    * that can be specified in a Pull vCard Listing request.
    */
typedef enum
{
   saName,
   saNumber,
   saSound,
   saDefault
} PBAP_Search_Attribute_t;

   /*! The following enumeration covers all the events generated by the
    * PBAP Profile.  These are used to determine the type of each event
    * generated, and to ensure the proper union element is accessed for
    * the PBAP_Event_Data_t structure.
    */
typedef enum
{
   etPBAP_Open_Port_Indication, /*!< Dispatched when a PBAP client connects to a local PBAP Server. */
   etPBAP_Open_Port_Confirmation, /*!< Dispatched when a PBAP client receives a connection response from a remote PBAP Server to which it previously attempted to connect. */
   etPBAP_Open_Port_Request_Indication, /*!< Dispatched when a PBAP client requests a connection to a local PBAP Server.  This event only occurs if the local server is in Manual Accept mode. */
   etPBAP_Close_Port_Indication, /*!< Dispatched when a PBAP Server or Client connection has been disconnected from either end. */
   etPBAP_Abort_Indication, /*!< Dispatched when the local PBAP Server receives an OBEX Abort Request from the remote Client. */
   etPBAP_Abort_Confirmation, /*!< Dispatched when the local PBAP Client receives an OBEX Abort Response from the remote Server. */
   etPBAP_Pull_Phonebook_Indication, /*!< Dispatched when the local PBAP Server receives a Pull Phonebook Request. */
   etPBAP_Pull_Phonebook_Confirmation, /*!< Dispatched when the local PBAP Client receives a Pull Phonebook Response. */
   etPBAP_Set_Phonebook_Indication, /*!< Dispatched when the local PBAP Server receives a Set Phonebook Request. */
   etPBAP_Set_Phonebook_Confirmation, /*!< Dispatched when the local PBAP Client receives a Set Phonebook Response. */
   etPBAP_Pull_vCard_Listing_Indication, /*!< Dispatched when the local PBAP Server receives a Pull vCard Listing Request. */
   etPBAP_Pull_vCard_Listing_Confirmation, /*!< Dispatched when the local PBAP Client receives a Pull vCard LIsting Response. */
   etPBAP_Pull_vCard_Entry_Indication, /*!< Dispatched when the local PBAP Server receives a Pull vCard Entry Request. */
   etPBAP_Pull_vCard_Entry_Confirmation /*!< Dispatched when the local PBAP Client receives a Pull vCard Entry Response. */
} PBAP_Event_Type_t;

   /*! The following PBAP Profile Event is dispatched when a PBAP Profile
    * Client Connects to a local PBAP Profile Server.  The PBAP ID
    * member specifies the local Server that has been connected to and
    * the BD_ADDR member specifies the Client Bluetooth Device that has
    * connected to the specified Server.
    */
typedef struct _tagPBAP_Open_Port_Indication_Data_t
{
   unsigned int PBAPID;
   BD_ADDR_t    BD_ADDR;
} PBAP_Open_Port_Indication_Data_t;

#define PBAP_OPEN_PORT_INDICATION_DATA_SIZE        (sizeof(PBAP_Open_Port_Indication_Data_t))

   /*! The following PBAP Profile Event is dispatched when a PBAP Client
    * receives the Connection Response from a remote PBAP Profile Server
    * to which it has previously attempted to connect.  The PBAP Profile
    * ID member specifies the local Client that requested the
    * connection, the PBAP Profile Connect Status represents the
    * Connection Status of the Request, and the BD_ADDR member specifies
    * the Bluetooth Device Address of the remote Bluetooth PBAP Profile
    * Server.
    */
typedef struct _tagPBAP_Open_Port_Confirmation_Data_t
{
   unsigned int PBAPID;
   unsigned int PBAPConnectStatus;
   BD_ADDR_t    BD_ADDR;
} PBAP_Open_Port_Confirmation_Data_t;

#define PBAP_OPEN_PORT_CONFIRMATION_DATA_SIZE      (sizeof(PBAP_Open_Port_Confirmation_Data_t))

   /*! The following PBAP Profile Event is dispatched when a PBAP Profile
    * Client requests a connection to a local PBAP Profile Server.  The
    * PBAP ID member specifies the local Server that has received the
    * request to connect and the BD_ADDR member specifies the Client
    * Bluetooth Device that is requesting to connect to the specified
    * Server.
    * \note This event is only dispatched to servers that are in
    *            Manual Accept Mode.
    * \note This event must be responded to with the
    *            PBAP_Open_Request_Response() function in order to
    *            accept or reject the outstanding Open Request.
    */
typedef struct _tagPBAP_Open_Port_Request_Indication_Data_t
{
   unsigned int PBAPID;
   unsigned int ServerPort;
   BD_ADDR_t    BD_ADDR;
} PBAP_Open_Port_Request_Indication_Data_t;

#define PBAP_OPEN_PORT_REQUEST_INDICATION_DATA_SIZE (sizeof(PBAP_Open_Port_Request_Indication_Data_t))

   /*! The following PBAP Profile Event is dispatched when a PBAP Profile
    * Server or Client connection is disconnected.  The PBAP ID member
    * specifies the local connection from which the remote device has
    * disconnected.
    */
typedef struct _tagPBAP_Close_Port_Indication_Data_t
{
   unsigned int PBAPID;
} PBAP_Close_Port_Indication_Data_t;

#define PBAP_CLOSE_PORT_INDICATION_DATA_SIZE       (sizeof(PBAP_Close_Port_Indication_Data_t))

   /*! The following PBAP Profile Event is dispatched when a PBAP Profile
    * Abort Request is received from a remote PBAP Profile Client.  The
    * PBAP ID member specifies the local Server that has received the
    * request.
    * \note When receiving this indication, the PBAP profile has
    *          already automatically dispatched a response to the remote
    *          client.
    */
typedef struct _tagPBAP_Abort_Indication_Data_t
{
   unsigned int PBAPID;
} PBAP_Abort_Indication_Data_t;

#define PBAP_ABORT_INDICATION_DATA_SIZE            (sizeof(PBAP_Abort_Indication_Data_t))

   /*! The following PBAP Profile Event is dispatched when a PBAP Profile
    * Abort Response is received from a remote PBAP Profile Server.  The
    * PBAP ID member specifies the local Client that has received the
    * response.
    */
typedef struct _tagPBAP_Abort_Confirmation_Data_t
{
   unsigned int PBAPID;
} PBAP_Abort_Confirmation_Data_t;

#define PBAP_ABORT_CONFIRMATION_DATA_SIZE          (sizeof(PBAP_Abort_Confirmation_Data_t))

   /*! The following PBAP Profile Event is dispatched when an PBAP Server
    * receives a Pull Phonebook Request from a remote PBAP Client.  The
    * PBAP ID member specifies the local Server that has received the
    * request.  The ObjectName member contains the Name/Path of the
    * Phonebook being requested by this Pull Phonebook operation.  The
    * FilterLow parameter contains the lower 32 bits of the 64-bit
    * filter attribute.  The FilterHigh parameter contains the higher 32
    * bits of the 64-bit filter attribute.  The Format parameter is an
    * enumeration which specifies the vCard format requested in this
    * Pull Phonebook request.  The MaxListCount parameter is an unsigned
    * integer that specifies the maximum number of entries the
    * requesting client can handle.  A value of 65535 means that the
    * number of entries is not restricted.  A MaxListCount of ZERO (0)
    * indicates that the Client is requesting the number of used indexes
    * in the Phonebook specified by the ObjectName parameter.  In this
    * case the response should return the PhonebookSize and other
    * parameters in this event can be ignored.  If MaxListCount is
    * non-zero then the ListStartOffset parameter specifies the index
    * requested by the Client in this PullPhonebookRequest.  This value
    * defaults to zero if not provided by the Client.
    */
typedef struct _tagPBAP_Pull_Phonebook_Indication_Data_t
{
   unsigned int   PBAPID;
   char          *ObjectName;
   DWord_t        FilterLow;
   DWord_t        FilterHigh;
   PBAP_Format_t  Format;
   Word_t         MaxListCount;
   Word_t         ListStartOffset;
} PBAP_Pull_Phonebook_Indication_Data_t;

#define PBAP_PULL_PHONEBOOK_INDICATION_DATA_SIZE   (sizeof(PBAP_Pull_Phonebook_Indication_Data_t))

   /*! The following PBAP Profile Event is dispatched when an PBAP Client
    * receives a Pull Phonebook Response from a remote PBAP Server.  The
    * PBAP ID member specifies the local client that has received the
    * response.  The ResponseCode member contains the response code
    * included in the response packet.  The BufferSize and Buffer
    * members contain the size and pointer (respectively) for the vCard
    * data included in this Pull Phonebook Response.  The Final member
    * indicates if this is the last block of data that will be sent in
    * the Pull Phonebook transaction (Final = TRUE if Server sent
    * EndOfBody header).  The PhonebookSize parameter is only valid if
    * the original request was for the PhonebookSize.  In this case,
    * this parameter will contain the Phonebook size returned by the
    * PBAP server.  Otherwise this parameter will be set to zero.  The
    * NewMissedCalls parameter is only valid if the "mch" Phonebook was
    * requested.  In this case, this parameter contains the number of
    * new missed calls which have not been checked on the server.
    * Otherwise this parameter will default to zero.
    */
typedef struct _tagPBAP_Pull_Phonebook_Confirmation_Data_t
{
   unsigned int  PBAPID;
   Byte_t        ResponseCode;
   unsigned int  BufferSize;
   Byte_t       *Buffer;
   Boolean_t     Final;
   Word_t        PhonebookSize;
   Byte_t        NewMissedCalls;
} PBAP_Pull_Phonebook_Confirmation_Data_t;

#define PBAP_PULL_PHONEBOOK_CONFIRMATION_DATA_SIZE (sizeof(PBAP_Pull_Phonebook_Confirmation_Data_t))

   /*! The following PBAP Profile Event is dispatched when an PBAP Server
    * receives a Set Phonebook Request from a remote PBAP Client.  The
    * PBAP ID member specifies the local Server that has received the
    * request.  The PathOption parameter contains an enumerated value
    * that indicates the type of path change being requested.  The
    * ObjectName member contains the Name/Path of the Phonebook being
    * requested by this Set Phonebook operation.
    */
typedef struct _tagPBAP_Set_Phonebook_Indication_Data_t
{
   unsigned int            PBAPID;
   PBAP_Set_Path_Option_t  PathOption;
   char                   *ObjectName;
} PBAP_Set_Phonebook_Indication_Data_t;

#define PBAP_SET_PHONEBOOK_INDICATION_DATA_SIZE    (sizeof(PBAP_Set_Phonebook_Indication_Data_t))

   /*! The following PBAP Profile Event is dispatched when an PBAP Client
    * receives a Set Phonebook Response from a remote PBAP Server.  The
    * PBAP ID member specifies the local client that has received the
    * response.  The ResponseCode member contains the response code
    * included in the response packet.
    */
typedef struct _tagPBAP_Set_Phonebook_Confirmation_Data_t
{
   unsigned int PBAPID;
   Byte_t       ResponseCode;
} PBAP_Set_Phonebook_Confirmation_Data_t;

#define PBAP_SET_PHONEBOOK_CONFIRMATION_DATA_SIZE  (sizeof(PBAP_Set_Phonebook_Confirmation_Data_t))

   /*! The following PBAP Profile Event is dispatched when an PBAP Server
    * receives a Pull vCard Listing Request from a remote PBAP Client.
    * The PBAP ID member specifies the local Server that has received
    * the request.  The ObjectName member contains the folder being
    * requested by this Pull vCard Listing operation.  If this value is
    * NULL the current directory should be retrieved.  The ListOrder
    * parameter is an enumerated type that determines the desired order
    * of the requested listing.  The SearchAttribute is an enumerated
    * type that determines the requested attribute used to filter this
    * request.  The SearchValue parameter contains an ASCII,
    * Null-terminated character string that contains the string request
    * for search/filter.  If this parameter is NULL, no search/filter
    * was specified in the request.  The MaxListCount parameter is an
    * unsigned integer that specifies the maximum number of entries the
    * requesting client can handle.  A value of 65535 means that the
    * number of entries is not restricted.  A MaxListCount of ZERO (0)
    * indicates that the Client is requesting the number of used indexes
    * in the folder specified by the ObjectName parameter.  In this case
    * the response should return the PhonebookSize and other parameters
    * in this event can be ignored.  If MaxListCount is non-zero then
    * the ListStartOffset parameter specifies the index requested by the
    * Client in this Pull vCard Listing.  This value defaults to zero if
    * not provided by the Client.
    */
typedef struct _tagPBAP_Pull_vCard_Listing_Indication_Data_t
{
   unsigned int             PBAPID;
   char                    *ObjectName;
   PBAP_List_Order_t        ListOrder;
   PBAP_Search_Attribute_t  SearchAttribute;
   char                    *SearchValue;
   Word_t                   MaxListCount;
   Word_t                   ListStartOffset;
} PBAP_Pull_vCard_Listing_Indication_Data_t;

#define PBAP_PULL_VCARD_LISTING_INDICATION_DATA_SIZE  (sizeof(PBAP_Pull_vCard_Listing_Indication_Data_t))

   /*! The following PBAP Profile Event is dispatched when an PBAP Client
    * receives a Pull vCard Listing Response from a remote PBAP Server.
    * The PBAP ID member specifies the local client that has received
    * the response.  The ResponseCode member contains the response code
    * included in the response packet.  The BufferSize and Buffer
    * members contain the size and pointer (respectively) for the vCard
    * data included in this Pull vCard Listing Response.  The Final
    * member indicates if this is the last block of data that will be
    * sent in the Pull vCard Listing transaction (Final = TRUE if Server
    * sent EndOfBody header).  The PhonebookSize parameter is only valid
    * if the original request was for the PhonebookSize.  In this case,
    * this parameter will contain the Phonebook size returned by the
    * PBAP server.  Otherwise this parameter will be set to zero.  The
    * NewMissedCalls parameter is only valid if the "mch" Phonebook was
    * requested.  In this case, this parameter contains the number of
    * new missed calls which have not been checked on the server.
    * Otherwise this parameter will default to zero.
    */
typedef struct _tagPBAP_Pull_vCard_Listing_Confirmation_Data_t
{
   unsigned int  PBAPID;
   Byte_t        ResponseCode;
   unsigned int  BufferSize;
   Byte_t       *Buffer;
   Boolean_t     Final;
   Word_t        PhonebookSize;
   Byte_t        NewMissedCalls;
} PBAP_Pull_vCard_Listing_Confirmation_Data_t;

#define PBAP_PULL_VCARD_LISTING_CONFIRMATION_DATA_SIZE   (sizeof(PBAP_Pull_vCard_Listing_Confirmation_Data_t))

   /*! The following PBAP Profile Event is dispatched when an PBAP Server
    * receives a Pull vCard Entry Request from a remote PBAP Client.
    * The PBAP ID member specifies the local Server that has received
    * the request.  The ObjectName member contains the entry of the
    * Phonebook being requested by this Pull vCard Entry operation.  The
    * FilterLow parameter contains the lower 32 bits of the 64-bit
    * filter attribute.  The FilterHigh parameter contains the higher 32
    * bits of the 64-bit filter attribute.  The Format parameter is an
    * enumeration which specifies the vCard format requested in this
    * Pull vCard Entry request.
    */
typedef struct _tagPBAP_Pull_vCard_Entry_Indication_Data_t
{
   unsigned int   PBAPID;
   char          *ObjectName;
   DWord_t        FilterLow;
   DWord_t        FilterHigh;
   PBAP_Format_t  Format;
} PBAP_Pull_vCard_Entry_Indication_Data_t;

#define PBAP_PULL_VCARD_ENTRY_INDICATION_DATA_SIZE    (sizeof(PBAP_Pull_vCard_Entry_Indication_Data_t))

   /*! The following PBAP Profile Event is dispatched when an PBAP Client
    * receives a Pull vCard Entry Response from a remote PBAP Server.
    * The PBAP ID member specifies the local client that has received
    * the response.  The ResponseCode member contains the response code
    * included in the response packet.  The BufferSize and Buffer
    * members contain the size and pointer (respectively) for the vCard
    * data included in this Pull vCard Entry Response.  The Final member
    * indicates if this is the last block of data that will be sent in
    * the Pull vCard Entry transaction (Final = TRUE if Server sent
    * EndOfBody header).
    */
typedef struct _tagPBAP_Pull_vCard_Entry_Confirmation_Data_t
{
   unsigned int  PBAPID;
   Byte_t        ResponseCode;
   unsigned int  BufferSize;
   Byte_t       *Buffer;
   Boolean_t     Final;
} PBAP_Pull_vCard_Entry_Confirmation_Data_t;

#define PBAP_PULL_VCARD_ENTRY_CONFIRMATION_DATA_SIZE  (sizeof(PBAP_Pull_vCard_Entry_Confirmation_Data_t))

   /*! The following structure represents the container structure for
    * holding all PBAP Profile Event Data.  This structure is received
    * for each event generated.  The Event_Data_Type member is used to
    * determine the appropriate union member element to access the
    * contained data.  The Event_Data_Size member contains the total
    * size of the data contained in this event.
    */
typedef struct _tagPBAP_Event_Data_t
{
   PBAP_Event_Type_t Event_Data_Type;
   Word_t            Event_Data_Size;
   union
   {
      PBAP_Open_Port_Indication_Data_t            *PBAP_Open_Port_Indication_Data;
      PBAP_Open_Port_Confirmation_Data_t          *PBAP_Open_Port_Confirmation_Data;
      PBAP_Open_Port_Request_Indication_Data_t    *PBAP_Open_Port_Request_Indication_Data;
      PBAP_Close_Port_Indication_Data_t           *PBAP_Close_Port_Indication_Data;
      PBAP_Abort_Indication_Data_t                *PBAP_Abort_Indication_Data;
      PBAP_Abort_Confirmation_Data_t              *PBAP_Abort_Confirmation_Data;
      PBAP_Pull_Phonebook_Indication_Data_t       *PBAP_Pull_Phonebook_Indication_Data;
      PBAP_Pull_Phonebook_Confirmation_Data_t     *PBAP_Pull_Phonebook_Confirmation_Data;
      PBAP_Set_Phonebook_Indication_Data_t        *PBAP_Set_Phonebook_Indication_Data;
      PBAP_Set_Phonebook_Confirmation_Data_t      *PBAP_Set_Phonebook_Confirmation_Data;
      PBAP_Pull_vCard_Listing_Indication_Data_t   *PBAP_Pull_vCard_Listing_Indication_Data;
      PBAP_Pull_vCard_Listing_Confirmation_Data_t *PBAP_Pull_vCard_Listing_Confirmation_Data;
      PBAP_Pull_vCard_Entry_Indication_Data_t     *PBAP_Pull_vCard_Entry_Indication_Data;
      PBAP_Pull_vCard_Entry_Confirmation_Data_t   *PBAP_Pull_vCard_Entry_Confirmation_Data;
   } Event_Data;
} PBAP_Event_Data_t;

#define PBAP_EVENT_DATA_SIZE                       (sizeof(PBAP_Event_Data_t))

   /*! The following declared type represents the Prototype Function for
    * a PBAP Profile Event Receive Data Callback.  This function will be
    * called whenever an PBAP Profile Event occurs that is associated
    * with the specified Bluetooth Stack ID.  This function passes to
    * the caller the Bluetooth Stack ID, the PBAP Event Data that
    * occurred and the PBAP Profile Event Callback Parameter that was
    * specified when this Callback was installed.  The caller is free to
    * use the contents of the PBAP Profile Event Data ONLY in the
    * context of this callback.  If the caller requires the Data for a
    * longer period of time, then the callback function MUST copy the
    * data into another Data Buffer This function is guaranteed NOT to
    * be invoked more than once simultaneously for the specified
    * installed callback (i.e. this function DOES NOT have be
    * re-entrant).  It needs to be noted however, that if the same
    * Callback is installed more than once, then the callbacks will be
    * called serially.  Because of this, the processing in this function
    * should be as efficient as possible.  It should also be noted that
    * this function is called in the Thread Context of a Thread that the
    * User does NOT own.  Therefore, processing in this function should
    * be as efficient as possible (this argument holds anyway because
    * another PBAP Profile Event will not be processed while this
    * function call is outstanding).
    * \note This function MUST NOT Block and wait for events that
    *            can only be satisfied by Receiving PBAP Profile Event
    *            Packets.  A Deadlock WILL occur because NO PBAP Event
    *            Callbacks will be issued while this function is
    *            currently outstanding.
    */
typedef void (BTPSAPI *PBAP_Event_Callback_t)(unsigned int BluetoothStackID, PBAP_Event_Data_t *PBAP_Event_Data, unsigned long CallbackParameter);

   /*! @brief The following function is responsible for opening a local PBAP
    * Server.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID on which to
    * open the server.  @param ServerPort The second parameter is the Port on which to
    * open this server, and *MUST* be between PBAP_PORT_NUMBER_MINIMUM
    * and PBAP_PORT_NUMBER_MAXIMUM.  @param SupportedRepositories The third parameter is a bitmask
    * which determines which repositories are supported by this server
    * instance.  @param EventCallback The fourth parameter is the Callback function to call
    * when an event occurs on this Server Port.  @param CallbackParameter The final parameter is
    * a user-defined callback parameter that will be passed to the
    * callback function with each event.  @return This function returns a
    * positive, non zero value if successful or a negative return error
    * code if an error occurs.  A successful return code will be a PBAP
    * Profile ID that can be used to reference the Opened PBAP Profile
    * Server Port in ALL other PBAP Server functions in this module.
    * Once an PBAP Profile Server is opened, it can only be
    * Un-Registered via a call to the PBAP_Close_Server() function
    * (passing the return value from this function).
    */
BTPSAPI_DECLARATION int BTPSAPI PBAP_Open_Server_Port(unsigned int BluetoothStackID, unsigned int ServerPort, Byte_t SupportedRepositories, PBAP_Event_Callback_t EventCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_PBAP_Open_Server_Port_t)(unsigned int BluetoothStackID, unsigned int ServerPort, Byte_t SupportedRepositories, PBAP_Event_Callback_t EventCallback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is responsible for closing a PBAP Profile
    * Server (which was opened by a successful call to the
    * PBAP_Open_Server_Port() function).  @param BluetoothStackID The first parameter is the
    * Bluetooth Stack ID of the previously opened server port.  @param PBAPID The second parameter is the PBAP ID returned from the previous call to
    * PBAP_Open_Server_Port().  @return This function returns zero if
    * successful, or a negative return error code if an error occurred
    * (see BTERRORS.H).  Note that this function does NOT delete any SDP
    * Service Record Handles.
    */
BTPSAPI_DECLARATION int BTPSAPI PBAP_Close_Server_Port(unsigned int BluetoothStackID, unsigned int PBAPID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_PBAP_Close_Server_Port_t)(unsigned int BluetoothStackID, unsigned int PBAPID);
#endif

   /*! @brief The following function adds a PBAP Server (PSE) Service Record to
    * the SDP Database.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID
    * of the stack on which to register this SDP record.  @param PBAPID The second parameter is the PBAP ID that was returned by a previous call to
    * PBAP_Open_Server_Port().  @param ServiceName The third parameter is a pointer to
    * ASCII, NULL terminated string containing the Service Name to
    * include within the SDP Record.  @param SDPServiceRecordHandle The final parameter is a pointer
    * to a DWord_t which receives the SDP Service Record Handle if this
    * function successfully creates an SDP Service Record.  If @return This function returns zero, then the SDPServiceRecordHandle entry will
    * contain the Service Record Handle of the added SDP Service Record.
    * If this function fails, a negative return error code will be
    * returned (see BTERRORS.H) and the SDPServiceRecordHandle value
    * will be undefined.
    * \note This function should only be called with the PBAP ID that
    *          was returned from the PBAP_Open_Server_Port() function.
    *          This function should NEVER be used with PBAP ID returned
    *          from the PBAP_Open_Remote_Server_Port() function.
    * \note The Service Record Handle that is returned from this
    *          function will remain in the SDP Record Database until it
    *          is deleted by calling the PBAP_Un_Register_SDP_Record()
    *          function.
    * \note A MACRO is provided to Delete the Service Record from the
    *          SDP Data Base.  This MACRO maps the
    *          PBAP_Un_Register_SDP_Record() to the
    *          SDP_Delete_Service_Record() function.
    * \note The Service Name is always added at Attribute ID 0x0100.
    *          A Language Base Attribute ID List is created that
    *          specifies that 0x0100 is UTF-8 Encoded, English Language.
    */
BTPSAPI_DECLARATION int BTPSAPI PBAP_Register_Server_SDP_Record(unsigned int BluetoothStackID, unsigned int PBAPID, char *ServiceName, DWord_t *SDPServiceRecordHandle);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_PBAP_Register_Server_SDP_Record_t)(unsigned int BluetoothStackID, unsigned int PBAPID, char *ServiceName, DWord_t *SDPServiceRecordHandle);
#endif

   /*! @brief The following function adds a PBAP Client (PCE) Service Record to
    * the SDP Database.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID
    * of the stack on which to register this SDP record.  @param ServiceName The second parameter is a pointer to ASCII, NULL terminated string containing
    * the Service Name to include within the SDP Record.  @param SDPServiceRecordHandle The final parameter is a pointer to a DWord_t which receives the SDP Service
    * Record Handle if this function successfully creates an SDP Service
    * Record. @return If this function returns zero, then the
    * SDPServiceRecordHandle entry will contain the Service Record
    * Handle of the added SDP Service Record.  If this function fails, a
    * negative return error code will be returned (see BTERRORS.H) and
    * the SDPServiceRecordHandle value will be undefined.
    * \note The Service Record Handle that is returned from this
    *          function will remain in the SDP Record Database until it
    *          is deleted by calling the PBAP_Un_Register_SDP_Record()
    *          function.
    * \note A MACRO is provided to Delete the Service Record from the
    *          SDP Data Base.  This MACRO maps the
    *          PBAP_Un_Register_SDP_Record() to the
    *          SDP_Delete_Service_Record() function.
    * \note The Service Name is always added at Attribute ID 0x0100.
    *          A Language Base Attribute ID List is created that
    *          specifies that 0x0100 is UTF-8 Encoded, English Language.
    */
BTPSAPI_DECLARATION int BTPSAPI PBAP_Register_Client_SDP_Record(unsigned int BluetoothStackID, char *ServiceName, DWord_t *SDPServiceRecordHandle);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_PBAP_Register_Client_SDP_Record_t)(unsigned int BluetoothStackID, char *ServiceName, DWord_t *SDPServiceRecordHandle);
#endif

   /*! The following MACRO is a utility MACRO that deletes the PBAP
    * Profile Server SDP Service Record (specified by the third parameter) from the SDP Database.  This MACRO maps to the
    * SDP_Delete_Service_Record() function.  This MACRO is only provided
    * so that the caller doesn't have to sift through the SDP API for
    * very simplistic applications.  This function accepts as input the
    * Bluetooth Stack ID of the Bluetooth Protocol Stack that the
    * Service Record exists on, the PBAP ID (returned from a successful
    * call to the PBAP_Open_Server_Port() function), and the SDP Service
    * Record Handle.  The SDP Service Record Handle was returned via a
    * successful call to the PBAP_Register_Server_SDP_Record() function.
    * See the PBAP_Register_Server_SDP_Record() function for more
    * information.  This MACRO returns the result of the
    * SDP_Delete_Service_Record() function, which is zero for success or
    * a negative return error code (see BTERRORS.H).  If using this
    * function to remove a Client SDP record, simply set PBAPID to zero.
    */
#define PBAP_Un_Register_SDP_Record(__BluetoothStackID, __PBAPID, __SDPRecordHandle) \
        (SDP_Delete_Service_Record(__BluetoothStackID, __SDPRecordHandle))

   /*! @brief The following function is responsible for opening a connection to
    * a remote PBAP Server.  @param BluetoothStackID The first parameter is the Bluetooth Stack
    * ID of the local Bluetooth stack.  @param BD_ADDR The second parameter is the
    * remote Bluetooth Device Address of the Bluetooth PBAP Profile
    * Server with which to connect.  @param ServerPort The third parameter specifies the
    * remote server port with which to connect. @param EventCallback The fourth parameter
    * specifies the PBAP Profile Event Callback Function. @param CallbackParameter The final parameter is
    * the Callback associated with this PBAP Profile Client.
    * \note The ServerPort parameter MUST be between
    * PBAP_PORT_NUMBER_MINIMUM and PBAP_PORT_NUMBER_MAXIMUM.
    * @return This function returns a positive, non zero, value if successful or a
    * negative return error code if an error occurs.  A successful
    * return code will be a PBAP ID that can be used to reference the
    * remote opened PBAP Profile Server in ALL other PBAP Profile Client
    * functions in this module.  Once a remote server is opened, it can
    * only be closed via a call to the PBAP_Close_Connection() function
    * (passing the return value from this function).
    */
BTPSAPI_DECLARATION int BTPSAPI PBAP_Open_Remote_Server_Port(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, unsigned int ServerPort, PBAP_Event_Callback_t EventCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_PBAP_Open_Remote_Server_Port_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, unsigned int ServerPort, PBAP_Event_Callback_t EventCallback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to allow a means to respond to
    * a request to connect to the local PBAP Profile Server.  @param BluetoothStackID
    * The first parameter is the Bluetooth Stack ID of the Bluetooth Stack on
    * which this server resides.  @param PBAPID The second parameter is the PBAP ID
    * that was returned from a previous PBAP_Open_Server_Port() function
    * for this server.  @param AcceptConnection The final parameter to this function is a
    * Boolean_t that indicates whether to accept the pending connection.
    * @return This function returns zero if successful, or a negative return
    * value if there was an error.
    * \note The connection to the server is not established until a
    *            etPBAP_Open_Port_Indication event has occurred.
    */
BTPSAPI_DECLARATION int BTPSAPI PBAP_Open_Request_Response(unsigned int BluetoothStackID, unsigned int PBAPID, Boolean_t AcceptConnection);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_PBAP_Open_Request_Response_t)(unsigned int BluetoothStackID, unsigned int PBAPID, Boolean_t AcceptConnection);
#endif

   /*! @brief The following function is responsible for closing a currently
    * ongoing PBAP Profile connection.  @param BluetoothStackID The first parameter is the
    * Bluetooth Stack ID of the Bluetooth Protocol Stack Instance that
    * is associated with the PBAP Profile connection being closed.
    * @param PBAPID The second parameter to this function is the PBAP ID of the PBAP
    * Profile connection to be closed.  @return This function returns zero if
    * successful, or a negative return value if there was an error.
    * \note If this function is called with a Server PBAP ID (a value
    *          returned from a call to PBAP_Open_Server_Port()) any
    *          clients currently connected to this server will be
    *          terminated, but the server will remained open and
    *          registered.  If this function is called using a Client
    *          PBAP ID (a value returned from a call to
    *          PBAP_Open_Remote_Server_Port()), the client connection
    *          will be terminated/closed entirely.
    */
BTPSAPI_DECLARATION int BTPSAPI PBAP_Close_Connection(unsigned int BluetoothStackID, unsigned int PBAPID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_PBAP_Close_Connection_t)(unsigned int BluetoothStackID, unsigned int PBAPID);
#endif

   /*! @brief The following function is responsible for sending an Disconnect
    * Request to the remote device/entity.  @param BluetoothStackID The first parameter to this
    * function is the Bluetooth Stack ID of the Bluetooth Protocol Stack
    * Instance that is associated with the PBAP Profile connection being
    * disconnected.  @param PBAPID The second parameter to this function is the PBAP
    * ID of the PBAP Profile connection to be disconnected (that was
    * returned by a call to PBAP_Open_Remote_Server_Port()).
    * @return This function returns zero if successful, or a negative return value if
    * there was an error.
    * \note Use of this function is optional and is provided for full
    *          compliance with some OBEX applications.  Calling the
    *          PBAP_Close_Connection() function will achieve the same
    *          results without sending the OBEX disconnect packet
    *          beforehand.  If is also possible to call this function
    *          and then immediately call PBAP_Close_Connection() without
    *          waiting for a confirmation because a Disconnect Request
    *          cannot be failed.  Calling this function by itself and
    *          waiting for a response will cause the underlying
    *          connection to automatically be closed once the response
    *          is received.  This will generate a Close Port Indication.
    */
BTPSAPI_DECLARATION int BTPSAPI PBAP_Disconnect_Request(unsigned int BluetoothStackID, unsigned int PBAPID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_PBAP_Disconnect_Request_t)(unsigned int BluetoothStackID, unsigned int PBAPID);
#endif

   /*! @brief The following function is responsible for Aborting ANY currently
    * outstanding PBAP Profile Client Request.  @param BluetoothStackID The first parameter is
    * the Bluetooth Stack ID of the Bluetooth Stack for which the PBAP
    * Profile Client is valid. @param PBAPID The second parameter to this function
    * specifies the PBAP ID (returned from a successful call to the
    * PBAP_Open_Remote_Server_Port() function). @return This function returns
    * zero if successful, or a negative return error code if there was
    * an error.
    * \note There can only be one outstanding PBAP Profile Request
    *          active at any one time.  Because of this, another PBAP
    *          Profile Request cannot be issued until either the current
    *          request is Aborted (by calling the PBAP_Abort_Request()
    *          function) or the current Request is completed (this is
    *          signified by receiving a Confirmation Event in the PBAP
    *          Profile Event Callback that was registered when the PBAP
    *          Profile Port was opened).
    * \note Because of transmission latencies, it may be possible
    *          that a PBAP Profile Client Request that is to be aborted
    *          may have completed before the server was able to Abort
    *          the request.  In either case, the caller will be notified
    *          via PBAP Profile Callback of the status of the previous
    *          Request.
    */
BTPSAPI_DECLARATION int BTPSAPI PBAP_Abort_Request(unsigned int BluetoothStackID, unsigned int PBAPID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_PBAP_Abort_Request_t)(unsigned int BluetoothStackID, unsigned int PBAPID);
#endif

   /*! @brief The following function generates a PBAP Pull Phonebook Request to
    * the specified remote PBAP Server. @param BluetoothStackID The first parameter
    * the ID of the Bluetooth Stack that is associated with this PBAP
    * Client.  @param PBAPID The second parameter specifies the PBAP ID for the local
    * PBAP Client (returned from a successful call to the
    * PBAP_Connect_Remote_Server_Port() function). @param ObjectName The third
    * parameter contains the Name/Path of the Phonebook being requested
    * by this Pull Phonebook operation. @param FilterLow The fourth parameter
    * contains the lower 32 bits of the 64-bit filter attribute. @param
    * FilterHigh The fifth parameter contains the higher 32 bits of the 64-bit
    * filter attribute. @param Format The sixth parameter is an enumeration which
    * specifies the vCard format requested in this Pull Phonebook
    * request.  If pfDefault is specified then the format will not be
    * included in the request (note that the server will default to
    * pfvCard21 in this case). @param MaxListCount The seventh parameter is an
    * unsigned integer that specifies the maximum number of entries the
    * client can handle.  A value of 65535 means that the number of
    * entries is not restricted.  A MaxListCount of ZERO (0) indicates
    * that this is a request for the number of used indexes in the
    * Phonebook specified by the ObjectName parameter. @param
    * ListStartOffset The final parameter specifies the index requested by the
    * Client in this PullPhonebookRequest.  @return This function returns zero
    * if successful or a negative return error code if there was an
    * error.
    * \note A successful return code does not mean that the remote
    *          PBAP Profile Server successfully processed the command.
    *          The caller needs to check the confirmation result to
    *          determine if the remote PBAP Profile Server successfully
    *          executed the Request.
    * \note There can only be one outstanding PBAP Profile Request
    *          active at any one time.  Because of this, another PBAP
    *          Profile Request cannot be issued until either the current
    *          request is Aborted (by calling the PBAP_Abort_Request()
    *          function) or the current Request is completed (this is
    *          signified by receiving a Confirmation Event in the PBAP
    *          Profile Event Callback that was registered when the PBAP
    *          Profile Port was opened).
    */
BTPSAPI_DECLARATION int BTPSAPI PBAP_Pull_Phonebook_Request(unsigned int BluetoothStackID, unsigned int PBAPID, char *ObjectName, DWord_t FilterLow, DWord_t FilterHigh, PBAP_Format_t Format, Word_t MaxListCount, Word_t ListStartOffset);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_PBAP_Pull_Phonebook_Request_t)(unsigned int BluetoothStackID, unsigned int PBAPID, char *ObjectName, DWord_t FilterLow, DWord_t FilterHigh, PBAP_Format_t Format, Word_t MaxListCount, Word_t ListStartOffset);
#endif

   /*! @brief The following function sends a PBAP Pull Phonebook Response to the
    * specified remote PBAP Client.  This is used for responding to a
    * PBAP Pull Phonebook Indication. @param BluetoothStackID The
    * ID of the Bluetooth Stack that is associated with this PBAP
    * Server. @param PBAPID  The PBAP ID of the PBAP
    * Server responding to the request. @param ResponseCode
    * The OBEX response code to include in the response. @param
    * PhonebookSize Pointer to a variable that can
    * optionally contain a Phonebook Size value to return in this
    * request.  This should be done if the received indication indicated
    * a request for PhonebookSize by indicating a MaxListCount = ZERO
    * (0).  If this value is to be included in the response the Buffer
    * parameter should be set to NULL and the BufferSize to ZERO.  If
    * this value is NOT to be used in the response, this parameter
    * should be set to NULL. @param NewMissedCalls Pointer
    * to a variable that can optionally contain the number of new missed
    * calls which have not been checked on this server.  This should
    * only be included on requests for the 'mch' phonebook type.  If
    * this value is to be included in the response the Buffer parameter
    * should be set to NULL and the BufferSize to ZERO.  If this value
    * is NOT to be used in the response, this parameter should be set to
    * NULL. @param BufferSize The size in bytes of the data
    * included in the specified Buffer. @param Buffer
    * Pointer to a byte buffer containing the Phonebook data to be
    * included in this response packet. @param AmountWritten
    * Pointer to variable which will be written with the actual amount
    * of data that was able to be included in the packet.
    * @return This function returns zero if successful or a negative return error code if
    * there was an error.
    * \note Including a Buffer pointer and setting BufferSize > 0
    *          will cause a Body or End-of-Body header to be added to
    *          the packet, either on the first or subsequent packets.
    *          If the stack cannot include all the requested object data
    *          (BufferSize) in the current packet, a Body header will be
    *          used and AmountWritten will reflect that not all data was
    *          sent.  If all data is included an End-of-Body header will
    *          be used.
    * \note If AmountWritten returns an amount smaller than the
    *          specified BufferSize, not all the data was able to be
    *          sent.  This function should be called again with an
    *          adjusted BufferSize and Buffer pointer to account for the
    *          data that was successfully sent.
    */
BTPSAPI_DECLARATION int BTPSAPI PBAP_Pull_Phonebook_Response(unsigned int BluetoothStackID, unsigned int PBAPID, Byte_t ResponseCode, Word_t *PhonebookSize, Byte_t *NewMissedCalls, unsigned int BufferSize, Byte_t *Buffer, unsigned int *AmountWritten);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_PBAP_Pull_Phonebook_Response_t)(unsigned int BluetoothStackID, unsigned int PBAPID, Byte_t ResponseCode, Word_t *PhonebookSize, Byte_t *NewMissedCalls, unsigned int BufferSize, Byte_t *Buffer, unsigned int *AmountWritten);
#endif

   /*! @brief The following function generates a PBAP Set Phonebook Request to
    * the specified remote PBAP Server.  @param BluetoothStackID
    * The ID of the Bluetooth Stack that is associated with this PBAP
    * Client.  @param PBAPID  Specifies the PBAP ID for the local
    * PBAP Client (returned from a successful call to the
    * PBAP_Connect_Remote_Server_Port() function). @param PathOption
    * An enumerated value that indicates the type of
    * path change to request. @param ObjectName The
    * folder name to include with this Set Phonebook request. This
    * value can be NULL if no name is required for the selected
    * PathOption. See the PBAP specification for more information.
    * @return This function returns zero if successful or a negative return
    * error code if there was an error.
    * \note A successful return code does not mean that the remote
    *          PBAP Profile Server successfully processed the command.
    *          The caller needs to check the confirmation result to
    *          determine if the remote PBAP Profile Server successfully
    *          executed the Request.
    * \note There can only be one outstanding PBAP Profile Request
    *          active at any one time.  Because of this, another PBAP
    *          Profile Request cannot be issued until either the current
    *          request is Aborted (by calling the PBAP_Abort_Request()
    *          function) or the current Request is completed (this is
    *          signified by receiving a Confirmation Event in the PBAP
    *          Profile Event Callback that was registered when the PBAP
    *          Profile Port was opened).
    */
BTPSAPI_DECLARATION int BTPSAPI PBAP_Set_Phonebook_Request(unsigned int BluetoothStackID, unsigned int PBAPID, PBAP_Set_Path_Option_t PathOption, char *ObjectName);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_PBAP_Set_Phonebook_Request_t)(unsigned int BluetoothStackID, unsigned int PBAPID, PBAP_Set_Path_Option_t PathOption, char *ObjectName);
#endif

   /*! @brief The following function sends a PBAP Set Phonebook Response to the
    * specified remote PBAP Client.  This is used for responding to a
    * PBAP Set Phonebook Indication.  @param BluetoothStackID
    * The ID of the Bluetooth Stack that is associated with this PBAP
    * Server. @param PBAPID Specifies the PBAP ID of the PBAP
    * Server responding to the request. @param ResponseCode
    * The OBEX response code to include in the response.
    * @return This function returns zero if successful or a negative return error code if
    * there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI PBAP_Set_Phonebook_Response(unsigned int BluetoothStackID, unsigned int PBAPID, Byte_t ResponseCode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_PBAP_Set_Phonebook_Response_t)(unsigned int BluetoothStackID, unsigned int PBAPID, Byte_t ResponseCode);
#endif

   /*! @brief The following function generates a PBAP Pull vCard Listing Request
    * to the specified remote PBAP Server. @param BluetoothStackID
    * The ID of the Bluetooth Stack that is associated with
    * this PBAP Client. @param PBAPID Specifies the PBAP ID for
    * the local PBAP Client (returned from a successful call to the
    * PBAP_Connect_Remote_Server_Port() function). @param ObjectName
    * Contains the folder of the Phonebook being requested by
    * this Pull vCard Listing operation.  This value can be NULL if a
    * PhonebookSize request is being performed. @param ListOrder
    * An enumerated type that determines the optionally requested
    * order of the listing.  Using the 'loDefault' value for this
    * parameter will prevent this field from being added to the request
    * (note that the server will default to loIndexed in this case).
    * @param SearchAttribute An enumerated type that determines the
    * optionally requested attribute used to filter this request. Using
    * the 'saDefault' value for this parameter will prevent this
    * field from being added to the request (note that the server will
    * default to saIndexed in this case). @param SearchValue
    * Contains an optional ASCII, Null-terminated character string that
    * contains the string requested for search/filter. If this
    * parameter is NULL, this field will be excluded from the request.
    * @param MaxListCount An unsigned integer that specifies
    * the maximum number of list entries the client can handle.  A value
    * of 65535 means that the number of entries is not restricted.  A
    * MaxListCount of ZERO (0) indicates that this is a request for the
    * number of used indexes in the Phonebook specified by the
    * ObjectName parameter. @param ListStartOffset Specifies the
    * index requested by the Client in this Pull vCard Listing.
    * @return This function returns zero if successful or a negative return error
    * code if there was an error.
    * \note A successful return code does not mean that the remote
    *          PBAP Profile Server successfully processed the command.
    *          The caller needs to check the confirmation result to
    *          determine if the remote PBAP Profile Server successfully
    *          executed the Request.
    * \note There can only be one outstanding PBAP Profile Request
    *          active at any one time.  Because of this, another PBAP
    *          Profile Request cannot be issued until either the current
    *          request is Aborted (by calling the PBAP_Abort_Request()
    *          function) or the current Request is completed (this is
    *          signified by receiving a Confirmation Event in the PBAP
    *          Profile Event Callback that was registered when the PBAP
    *          Profile Port was opened).
    */
BTPSAPI_DECLARATION int BTPSAPI PBAP_Pull_vCard_Listing_Request(unsigned int BluetoothStackID, unsigned int PBAPID, char *ObjectName, PBAP_List_Order_t ListOrder, PBAP_Search_Attribute_t SearchAttribute, char *SearchValue, Word_t MaxListCount, Word_t ListStartOffset);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_PBAP_Pull_vCard_Listing_Request_t)(unsigned int BluetoothStackID, unsigned int PBAPID, char *ObjectName, PBAP_List_Order_t ListOrder, PBAP_Search_Attribute_t SearchAttribute, char *SearchValue, Word_t MaxListCount, Word_t ListStartOffset);
#endif

   /*! @brief The following function sends a PBAP Pull vCard Listing Response to
    * the specified remote PBAP Client.  This is used for responding to
    * a PBAP Pull vCard Listing Indication. @param BluetoothStackID
    * The ID of the Bluetooth Stack that is associated with
    * this PBAP Server. @param PBAPID Specifies the PBAP ID of
    * the PBAP Server responding to the request. @param ResponseCode
    * parameter is the OBEX response code to include in the response.
    * @param PhonebookSize A pointer to a variable that can
    * optionally contain a Phonebook Size value to return in this
    * request.  This should be done if the received indication indicated
    * a request for PhonebookSize by indicating a MaxListCount = ZERO
    * (0).  If this value is to be included in the response the Buffer
    * parameter should be set to NULL and the BufferSize to ZERO.  If
    * this value is NOT to be used in the response, this parameter
    * should be set to NULL. @param NewMissedCalls A pointer
    * to a variable that can optionally contain the number of new missed
    * calls which have not been checked on this server.  This should
    * only be included on requests for the 'mch' phonebook type.  If
    * this value is to be included in the response the Buffer parameter
    * should be set to NULL and the BufferSize to ZERO.  If this value
    * is NOT to be used in the response, this parameter should be set to
    * NULL. @param BufferSize The size in bytes of the data
    * included in the specified Buffer. @param Buffer A
    * pointer to a byte buffer containing the Phonebook listing data to
    * be included in this response packet. @param AmountWritten
    * A pointer to variable which will be written with the actual
    * amount of data that was able to be included in the packet.
    * @return This function returns zero if successful or a negative return error
    * code if there was an error.
    * \note Including a Buffer pointer and setting BufferSize > 0
    *          will cause a Body or End-of-Body header to be added to
    *          the packet, either on the first or subsequent packets.
    *          If the stack cannot include all the requested object data
    *          (BufferSize) in the current packet, a Body header will be
    *          used and AmountWritten will reflect that not all data was
    *          sent.  If all data is included an End-of-Body header will
    *          be used.
    * \note If AmountWritten returns an amount smaller than the
    *          specified BufferSize, not all the data was able to be
    *          sent.  This function should be called again with an
    *          adjusted BufferSize and Buffer pointer to account for the
    *          data that was successfully sent.
    */
BTPSAPI_DECLARATION int BTPSAPI PBAP_Pull_vCard_Listing_Response(unsigned int BluetoothStackID, unsigned int PBAPID, Byte_t ResponseCode, Word_t *PhonebookSize, Byte_t *NewMissedCalls, unsigned int BufferSize, Byte_t *Buffer, unsigned int *AmountWritten);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_PBAP_Pull_vCard_Listing_Response_t)(unsigned int BluetoothStackID, unsigned int PBAPID, Byte_t ResponseCode, Word_t *PhonebookSize, Byte_t *NewMissedCalls, unsigned int BufferSize, Byte_t *Buffer, unsigned int *AmountWritten);
#endif

   /*! @brief The following function generates a PBAP Pull vCard Entry Request
    * to the specified remote PBAP Server. @param BluetoothStackID
    * The ID of the Bluetooth Stack that is associated with
    * this PBAP Client. @param PBAPID Specifies the PBAP ID for
    * the local PBAP Client (returned from a successful call to the
    * PBAP_Connect_Remote_Server_Port() function). @param ObjectName
    * Contains the name of the Phonebook entry being requested
    * by this Pull vCard Entry operation. @param FilterLow
    * Contains the lower 32 bits of the 64-bit filter attribute. @param
    * FilterHigh Contains the higher 32 bits of the 64-bit
    * filter attribute. @param Format An enumeration which
    * specifies the vCard format requested in this Pull vCard Entry
    * request.  If pfDefault is specified then the format will not be
    * included in the request (note that in this case the server will
    * default to pfvCard21 in this case). @return This function returns zero if
    * successful or a negative return error code if there was an error.
    * \note A successful return code does not mean that the remote
    *          PBAP Profile Server successfully processed the command.
    *          The caller needs to check the confirmation result to
    *          determine if the remote PBAP Profile Server successfully
    *          executed the Request.
    * \note There can only be one outstanding PBAP Profile Request
    *          active at any one time.  Because of this, another PBAP
    *          Profile Request cannot be issued until either the current
    *          request is Aborted (by calling the PBAP_Abort_Request()
    *          function) or the current Request is completed (this is
    *          signified by receiving a Confirmation Event in the PBAP
    *          Profile Event Callback that was registered when the PBAP
    *          Profile Port was opened).
    */
BTPSAPI_DECLARATION int BTPSAPI PBAP_Pull_vCard_Entry_Request(unsigned int BluetoothStackID, unsigned int PBAPID, char *ObjectName, DWord_t FilterLow, DWord_t FilterHigh, PBAP_Format_t Format);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_PBAP_Pull_vCard_Entry_Request_t)(unsigned int BluetoothStackID, unsigned int PBAPID, char *ObjectName, DWord_t FilterLow, DWord_t FilterHigh, PBAP_Format_t Format);
#endif

   /*! @brief The following function sends a PBAP Pull vCard Entry Response to
    * the specified remote PBAP Client.  This is used for responding to
    * a PBAP Pull vCard Entry Indication. @param BluetoothStackID
    * The ID of the Bluetooth Stack that is associated with
    * this PBAP Server.  @param PBAPID Specifies the PBAP ID of
    * the PBAP Server responding to the request. @param ResponseCode
    * The OBEX response code to include in the response.
    * @param BufferSize The size in bytes of the data included
    * in the specified Buffer. @param Buffer A pointer to a
    * byte buffer containing the Phonebook entry data to be included in
    * this response packet. @param AmountWritten Apointer to
    * variable which will be written with the actual amount of data that
    * was able to be included in the packet. @return This function returns zero
    * if successful or a negative return error code if there was an
    * error.
    * \note Including a Buffer pointer and setting BufferSize > 0
    *          will cause a Body or End-of-Body header to be added to
    *          the packet, either on the first or subsequent packets.
    *          If the stack cannot include all the requested object data
    *          (BufferSize) in the current packet, a Body header will be
    *          used and AmountWritten will reflect that not all data was
    *          sent.  If all data is included an End-of-Body header will
    *          be used.
    * \note If AmountWritten returns an amount smaller than the
    *          specified BufferSize, not all the data was able to be
    *          sent.  This function should be called again with an
    *          adjusted BufferSize and Buffer pointer to account for the
    *          data that was successfully sent.
    */
BTPSAPI_DECLARATION int BTPSAPI PBAP_Pull_vCard_Entry_Response(unsigned int BluetoothStackID, unsigned int PBAPID, Byte_t ResponseCode, unsigned int BufferSize, Byte_t *Buffer, unsigned int *AmountWritten);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_PBAP_Pull_vCard_Entry_Response_t)(unsigned int BluetoothStackID, unsigned int PBAPID, Byte_t ResponseCode, unsigned int BufferSize, Byte_t *Buffer, unsigned int *AmountWritten);
#endif

   /*! @brief The following function is responsible for providing a mechanism to
    * query the current PBAP Profile Server Mode.  @param BluetoothStackID The first parameter
    * is the Bluetooth Stack ID of the Bluetooth Stack of the local PBAP
    * Profile Server.  @param PBAPID The second parameter is the PBAP ID that was
    * returned from the PBAP_Open_Server_Port() function.
    * @param ServerModeMask The final parameter is a pointer to a variable which will receive the
    * current Server Mode Mask.  @return This function returns zero if
    * successful, or a negative return value if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI PBAP_Get_Server_Mode(unsigned int BluetoothStackID, unsigned int PBAPID, unsigned long *ServerModeMask);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_PBAP_Get_Server_Mode_t)(unsigned int BluetoothStackID, unsigned int PBAPID, unsigned long *ServerModeMask);
#endif

   /*! @brief The following function is responsible for providing a mechanism to
    * change the current PBAP Profile Server Mode.  @param BluetoothStackID The first parameter
    * is the Bluetooth Stack ID of the Bluetooth Stack of the local PBAP
    * Profile Server.  @param PBAPID The second parameter is the PBAP ID that was
    * returned from the PBAP_Open_Server_Port() function.
    * @param ServerModeMask The final parameter is the new Server Mode Mask to use.
    * @return This function returns zero if successful, or a negative return value if there
    * was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI PBAP_Set_Server_Mode(unsigned int BluetoothStackID, unsigned int PBAPID, unsigned long ServerModeMask);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_PBAP_Set_Server_Mode_t)(unsigned int BluetoothStackID, unsigned int PBAPID, unsigned long ServerModeMask);
#endif

#endif
