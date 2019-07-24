/*
 * Copyright 2009 - 2014 Stonestreet One.
 * All Rights Reserved.
 * Author:  Tim Thomas
 *** MODIFICATION HISTORY ****************************************************
 *
 *   mm/dd/yy  F. Lastname    Description of Modification
 *   --------  -----------    ------------------------------------------------
 *   04/23/09  T. Thomas      Initial creation.
 ****************************************************************************
 */

/**
 *  @file MAPAPI.h
 *
 *  @brief Stonestreet One Bluetooth Message Access Profile (MAP) API Type
 *      Definitions, Constants, and Prototypes.
 *
 *  @code
 *  #include "SS1BTMAP.h"
 *  @endcode
 *  ============================================================================
 */
#ifndef __MAPAPIH__
#define __MAPAPIH__

#include "SS1BTPS.h"            /*! Bluetooth Stack API Prototypes/Constants. */

   /*! Error Return Codes.
    * Error Codes that are smaller than these (less than -1000) are
    * related to the Bluetooth Protocol Stack itself (see BTERRORS.H).
    */
#define BTMAP_ERROR_INVALID_PARAMETER                           (-1000)
#define BTMAP_ERROR_NOT_INITIALIZED                             (-1001)
#define BTMAP_ERROR_INVALID_BLUETOOTH_STACK_ID                  (-1002)
#define BTMAP_ERROR_DLL_INITIALIZATION_ERROR                    (-1003)
#define BTMAP_ERROR_INSUFFICIENT_RESOURCES                      (-1004)
#define BTMAP_ERROR_REQUEST_ALREADY_OUTSTANDING                 (-1005)
#define BTMAP_ERROR_ACTION_NOT_ALLOWED                          (-1006)
#define BTMAP_ERROR_INSUFFICIENT_PACKET_LENGTH                  (-1007)

   /*! MAP SDP Profile UUID's for the Message Access Profile.
    * The following MACRO is a utility MACRO that assigns the MAP
    * Message Access Profile Universally Unique Identifier to the
    * specified UUID_16_t variable.  This MACRO accepts one parameter
    * which is the UUID_16_t variable that is to receive the constant
    * value.  This value is defined in the MAP specification.
    */
#define SDP_ASSIGN_MESSAGE_ACCESS_PROFILE_UUID_16(_x)           ASSIGN_SDP_UUID_16((_x), 0x11, 0x34)

   /*! The following MACRO is a utility MACRO that assigns the MAP
    * Message Access Profile Universally Unique Identifier to the
    * specified UUID_32_t variable.  This MACRO accepts one parameter
    * which is the UUID_16_t variable that is to receive the constant
    * value.  This value is defined in the MAP specification.
    */
#define SDP_ASSIGN_MESSAGE_ACCESS_PROFILE_UUID_32(_x)           ASSIGN_SDP_UUID_32((_x), 0x00, 0x00, 0x11, 0x34)

   /*! The following MACRO is a utility MACRO that assigns the MAP
    * Message Access Profile Universally Unique Identifier to the
    * specified UUID_128_t variable.  This MACRO accepts one parameter
    * which is the UUID_128_t variable that is to receive the constant
    * value.  This value is defined in the MAP specification.
    */
#define SDP_ASSIGN_MESSAGE_ACCESS_PROFILE_UUID_128(_x)          ASSIGN_SDP_UUID_128((_x), 0x00, 0x00, 0x11, 0x34, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /*! MAP SDP UUID Service Classes for the Message Access Profile.
    * The following MACRO is a utility MACRO that assigns the MAP
    * Message Access Server Service Class value Universally Unique
    * Identifier to the specified UUID_16_t variable.  This MACRO
    * accepts one parameter which is the UUID_16_t variable that is to
    * receive the constant value.  This value is defined in the MAP
    * specification.
    */
#define SDP_ASSIGN_MESSAGE_ACCESS_SERVER_UUID_16(_x)            ASSIGN_SDP_UUID_16((_x), 0x11, 0x32)

   /*! The following MACRO is a utility MACRO that assigns the MAP
    * Message Access Server Service Class value Universally Unique
    * Identifier to the specified UUID_32_t variable.  This MACRO
    * accepts one parameter which is the UUID_32_t variable that is to
    * receive the constant value.  This value is defined in the MAP
    * specification.
    */
#define SDP_ASSIGN_MESSAGE_ACCESS_SERVER_UUID_32(_x)            ASSIGN_SDP_UUID_32((_x), 0x00, 0x00, 0x11, 0x32)

   /*! The following MACRO is a utility MACRO that assigns the MAP
    * Message Access Server Service Class value Universally Unique
    * Identifier to the specified UUID_128_t variable.  This MACRO
    * accepts one parameter which is the UUID_128_t variable that is to
    * receive the constant value.  This value is defined in the MAP
    * specification.
    */
#define SDP_ASSIGN_MESSAGE_ACCESS_SERVER_UUID_128(_x)           ASSIGN_SDP_UUID_128((_x), 0x00, 0x00, 0x11, 0x32, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /*! The following MACRO is a utility MACRO that assigns the MAP
    * Message Notification Server Service Class value Universally Unique
    * Identifier to the specified UUID_16_t variable.  This MACRO
    * accepts one parameter which is the UUID_16_t variable that is to
    * receive the constant value.  This value is defined in the MAP
    * specification.
    */
#define SDP_ASSIGN_MESSAGE_NOTIFICATION_SERVER_UUID_16(_x)      ASSIGN_SDP_UUID_16((_x), 0x11, 0x33)

   /*! The following MACRO is a utility MACRO that assigns the MAP
    * Message Notification Server Service Class value Universally Unique
    * Identifier to the specified UUID_32_t variable.  This MACRO
    * accepts one parameter which is the UUID_32_t variable that is to
    * receive the constant value.  This value is defined in the MAP
    * specification.
    */
#define SDP_ASSIGN_MESSAGE_NOTIFICATION_SERVER_UUID_32(_x)      ASSIGN_SDP_UUID_32((_x), 0x00, 0x00, 0x11, 0x33)

   /*! The following MACRO is a utility MACRO that assigns the MAP
    * Message Notification Server Service Class value Universally Unique
    * Identifier to the specified UUID_128_t variable.  This MACRO
    * accepts one parameter which is the UUID_128_t variable that is to
    * receive the constant value.  This value is defined in the MAP
    * specification.
    */
#define SDP_ASSIGN_MESSAGE_NOTIFICATION_SERVER_UUID_128(_x)     ASSIGN_SDP_UUID_128((_x), 0x00, 0x00, 0x11, 0x33, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /*! Defines the Profile Version Number used within the SDP Record for
    * Message Access Profile Servers.
    */
#define MAP_PROFILE_VERSION                                     (0x0100)

   /*! The following constants represent the Minimum and Maximum Port
    * Numbers that can be opened (both locally and remotely).  These
    * constants specify the range for the Port Number parameters in the
    * Open Functions.
    */
#define MAP_PORT_NUMBER_MINIMUM                                 (SPP_PORT_NUMBER_MINIMUM)
#define MAP_PORT_NUMBER_MAXIMUM                                 (SPP_PORT_NUMBER_MAXIMUM)

   /*! The following constants are the OBEX response codes that are
    * specifically allowed in the MAP specification.  The error codes
    * for Not Found, Service Unavailable, Bad Request, and Not
    * Implemented are all specifically mandatory.  All others listed
    * here are optional.  Other OBEX error codes not listed here are
    * specifically DISALLOWED by the specification.
    */
#define MAP_OBEX_RESPONSE_CONTINUE                              (OBEX_CONTINUE_RESPONSE)
#define MAP_OBEX_RESPONSE_OK                                    (OBEX_OK_RESPONSE)
#define MAP_OBEX_RESPONSE_NOT_FOUND                             (OBEX_NOT_FOUND_RESPONSE)
#define MAP_OBEX_RESPONSE_SERVICE_UNAVAILABLE                   (OBEX_SERVICE_UNAVAILABLE_RESPONSE)
#define MAP_OBEX_RESPONSE_BAD_REQUEST                           (OBEX_BAD_REQUEST_RESPONSE)
#define MAP_OBEX_RESPONSE_NOT_IMPLEMENTED                       (OBEX_NOT_IMPLEMENTED_RESPONSE)
#define MAP_OBEX_RESPONSE_UNAUTHORIZED                          (OBEX_UNAUTHORIZED_RESPONSE)
#define MAP_OBEX_RESPONSE_PRECONDITION_FAILED                   (OBEX_PRECONDITION_FAILED_RESPONSE)
#define MAP_OBEX_RESPONSE_NOT_ACCEPTABLE                        (OBEX_NOT_ACCEPTABLE_RESPONSE)
#define MAP_OBEX_RESPONSE_FORBIDDEN                             (OBEX_FORBIDDEN_RESPONSE)
#define MAP_OBEX_RESPONSE_SERVER_ERROR                          (OBEX_INTERNAL_SERVER_ERROR_RESPONSE)

   /*! The following define the OBEX Response Final Bit to be appended to
    * the response codes passed into function of this module.
    */
#define MAP_OBEX_RESPONSE_FINAL_BIT                             (OBEX_FINAL_BIT)

   /*! The following definition defines the maximum MAP Message Handle
    * Length.  A Message Handle is the following ASCII digits long
    * (where each digit is ASCII Hexadecimal ('0' through 'F').
    */
#define MAP_MESSAGE_HANDLE_LENGTH                                16

   /*! The following constants represent the Maximum List Count minimum,
    * maximum, and default values.
    */
#define MAP_MAX_LIST_COUNT_MINIMUM_VALUE                        (0)
#define MAP_MAX_LIST_COUNT_MAXIMUM_VALUE                        (65535)
#define MAP_MAX_LIST_COUNT_DEFAULT_VALUE                        (1024)

   /*! The following constants represent the List Start Offset minimum,
    * maximum, and default values.
    */
#define MAP_LIST_START_OFFSET_MINIMUM_VALUE                     (0)
#define MAP_LIST_START_OFFSET_MAXIMUM_VALUE                     (65535)
#define MAP_LIST_START_OFFSET_DEFAULT_VALUE                     (0)

   /*! The following constants represent the Subject Length minimum and
    * maximum values.
    */
#define MAP_SUBJECT_LENGTH_MINIMUM_VALUE                        (1)
#define MAP_SUBJECT_LENGTH_MAXIMUM_VALUE                        (255)

   /*! The following BIT definitions are used to denote the possible MAP
    * Profile Server Modes that can be applied to a MAP Profile Client
    * Connection.  These BIT definitions are used with the
    * MAP_Set_Server_Mode() and MAP_Get_Server_Mode() mode functions.
    */
#define MAP_SERVER_MODE_AUTOMATIC_ACCEPT_CONNECTION             (0x00000000)
#define MAP_SERVER_MODE_MANUAL_ACCEPT_CONNECTION                (0x00000001)

   /*! The following BIT MASK is used to mask the Server Mode Accept
    * Connection settings from other (undefined) Server Mode bits.
    */
#define MAP_SERVER_MODE_ACCEPT_CONNECTION_MASK                  (0x00000001)

   /*! The following BIT definitions are used to denote the supported
    * messages types of the Message Server.  These bit values are used
    * in the Server's SDP Record.
    */
#define MAP_SERVER_SUPPORTED_MESSAGE_TYPE_EMAIL                 (1 << 0)
#define MAP_SERVER_SUPPORTED_MESSAGE_TYPE_SMS_GSM               (1 << 1)
#define MAP_SERVER_SUPPORTED_MESSAGE_TYPE_SMS_CDMA              (1 << 2)
#define MAP_SERVER_SUPPORTED_MESSAGE_TYPE_MMS                   (1 << 3)

   /*! The following constants represent the MAP Profile Status Values
    * that are possible in the MAP Profile Connect Confirmation Event.
    */
#define MAP_OPEN_STATUS_SUCCESS                                 (GOEP_OPEN_PORT_STATUS_SUCCESS)
#define MAP_OPEN_STATUS_CONNECTION_TIMEOUT                      (GOEP_OPEN_PORT_STATUS_CONNECTION_TIMEOUT)
#define MAP_OPEN_STATUS_CONNECTION_REFUSED                      (GOEP_OPEN_PORT_STATUS_CONNECTION_REFUSED)
#define MAP_OPEN_STATUS_UNKNOWN_ERROR                           (GOEP_OPEN_PORT_STATUS_UNKNOWN_ERROR)

   /*! The following define sets the MAP Profile version included in the
    * SDP record for the MAP profile, as specified in the relevant
    * specification.
    */
#define MAP_PROFILE_VERSION                                     (0x0100)

   /*! The following defines are for the bits used when creating a MAP
    * server to indicate the types of repositories supported.  This
    * information is also populated in the SDP record when created.
    */
#define MAP_SUPPORTED_REPOSITORIES_LOCAL_PHONEBOOK              (0x01)
#define MAP_SUPPORTED_REPOSITORIES_SIM_CARD                     (0x02)

   /*! The following defines specify the bit locations for the filter
    * field specified by the MAP Profile specification.  Each of these
    * bit families can be ORed together to form a filter mask that
    * should be specified for a single filter parameter.
    */
#define MAP_FILTER_MESSAGE_TYPE_SMS_GSM                         (1 << 0)
#define MAP_FILTER_MESSAGE_TYPE_SMS_CDMA                        (1 << 1)
#define MAP_FILTER_MESSAGE_TYPE_EMAIL                           (1 << 2)
#define MAP_FILTER_MESSAGE_TYPE_MMS                             (1 << 3)

#define MAP_FILTER_READ_STATUS_NO_FILTERING                          (0)
#define MAP_FILTER_READ_STATUS_GET_UNREAD_ONLY                  (1 << 0)
#define MAP_FILTER_READ_STATUS_GET_READ_ONLY                    (1 << 1)

#define MAP_FILTER_PRIORITY_NO_FILTERING                             (0)
#define MAP_FILTER_PRIORITY_HIGH_ONLY                           (1 << 0)
#define MAP_FILTER_PRIORITY_NON_HIGH_ONLY                       (1 << 1)

   /*! The following defines specify the bit locations for the 16-bit
    * OptionsMask field of the MAP_Message_Listing_Info_t structure.
    * Each bit can be ORed together to form a OptionMask that should be
    * passed in the Message Listing request.
    */
#define MAP_MESSAGE_LISTING_INFO_OPTION_MASK_SUBJECT_LENGTH_PRESENT       (1 << 0)
#define MAP_MESSAGE_LISTING_INFO_OPTION_MASK_PARAMETER_MASK_PRESENT       (1 << 1)
#define MAP_MESSAGE_LISTING_INFO_OPTION_MASK_FILTER_MESSAGE_TYPE_PRESENT  (1 << 2)
#define MAP_MESSAGE_LISTING_INFO_OPTION_MASK_FILTER_PERIOD_BEGIN_PRESENT  (1 << 3)
#define MAP_MESSAGE_LISTING_INFO_OPTION_MASK_FILTER_PERIOD_END_PRESENT    (1 << 4)
#define MAP_MESSAGE_LISTING_INFO_OPTION_MASK_FILTER_READ_STATUS_PRESENT   (1 << 5)
#define MAP_MESSAGE_LISTING_INFO_OPTION_MASK_FILTER_RECIPIENT_PRESENT     (1 << 6)
#define MAP_MESSAGE_LISTING_INFO_OPTION_MASK_FILTER_ORIGINATOR_PRESENT    (1 << 7)
#define MAP_MESSAGE_LISTING_INFO_OPTION_MASK_FILTER_PRIORITY_PRESENT      (1 << 8)

   /*! The following defines specify the bit locations for the 64-bit
    * Parameter Mask field used for Message Listing Objects.  Each bit
    * can be ORed together to form a Parameter Mask that should be
    * passed in the Message Listing request.
    */
#define MAP_MESSAGE_LISTING_PARAMETER_MASK_SUBJECT              (1 << 0)
#define MAP_MESSAGE_LISTING_PARAMETER_MASK_DATE_TIME            (1 << 1)
#define MAP_MESSAGE_LISTING_PARAMETER_MASK_SENDER_NAME          (1 << 2)
#define MAP_MESSAGE_LISTING_PARAMETER_MASK_SENDER_ADDRESS       (1 << 3)
#define MAP_MESSAGE_LISTING_PARAMETER_MASK_RECIPIENT_NAME       (1 << 4)
#define MAP_MESSAGE_LISTING_PARAMETER_MASK_RECIPIENT_ADDRESS    (1 << 5)
#define MAP_MESSAGE_LISTING_PARAMETER_MASK_TYPE                 (1 << 6)
#define MAP_MESSAGE_LISTING_PARAMETER_MASK_SIZE                 (1 << 7)
#define MAP_MESSAGE_LISTING_PARAMETER_MASK_RECEPTION_STATUS     (1 << 8)
#define MAP_MESSAGE_LISTING_PARAMETER_MASK_TEXT                 (1 << 9)
#define MAP_MESSAGE_LISTING_PARAMETER_MASK_ATTACHMENT_SIZE      (1 << 10)
#define MAP_MESSAGE_LISTING_PARAMETER_MASK_PRIORITY             (1 << 11)
#define MAP_MESSAGE_LISTING_PARAMETER_MASK_READ                 (1 << 12)
#define MAP_MESSAGE_LISTING_PARAMETER_MASK_SENT                 (1 << 13)
#define MAP_MESSAGE_LISTING_PARAMETER_MASK_PROTECTED            (1 << 14)
#define MAP_MESSAGE_LISTING_PARAMETER_MASK_REPLY_TO_ADDRESS     (1 << 15)

#define MAP_MESSAGE_LISTING_PARAMETER_MASK_FILTER_NONE          (0)
#define MAP_MESSAGE_LISTING_PARAMETER_MASK_FILTER_ALL           ((1 << 16)-1)

   /*! The following define is the value that is used for the
    * MaxListCount header to indicate that there is not a restriction of
    * the number of items that can be returned.  This is also the
    * default value if this header is not present.
    */
#define MAP_MAX_LIST_COUNT_NOT_RESTRICTED                       (65535)

   /*! The following enumeration is a list of the valid operations that
    * can be performed using the MAP_SetFolder operation.  This
    * correspond to flag settings for the OBEX Set Path command.
    */
typedef enum _tagMAP_Set_Folder_Options_t
{
   sfRoot,
   sfDown,
   sfUp
} MAP_Set_Folder_Option_t;

   /*! The following enumeration is a list of valid Char Set parameters
    * used when specifying the character set the is to be used.
    */
typedef enum
{
   csNative,
   csUTF8
} MAP_CharSet_t;

   /*! The following enumeration is a list of valid Fragmentation Type
    * parameters used when specifying the Fractional Type.
    */
typedef enum
{
   ftUnfragmented,
   ftFirst,
   ftNext,
   ftMore,
   ftLast
} MAP_Fractional_Type_t;

   /*! The following enumeration is used to define the valid Status
    * indicators.
    */
typedef enum
{
   siReadStatus,
   siDeletedStatus
} MAP_Status_Indicator_t;

   /*! The following structure is used to represent a Time/Data
    * associated with a file when performing file transfer.  Since no
    * AM/PM field is provided, the Time is represented in 24 hour time.
    * The time data in this structure always represents the local time.
    * The UTC_Time field is used to indicate whether the UTC_Offset
    * value is valid.  If UTC time is specified, then UTC_Time is true
    * and UTC_Offset identifies the number of minutes that the current
    * time differs from UTC time.  This may be a positive or negative
    * number.
    * \note If no information about UTC time is known, then UTC_Time
    *          is set to false and local time is assumed.
    */
typedef struct _tagMAP_TimeDate_t
{
   Word_t    Year;
   Word_t    Month;
   Word_t    Day;
   Word_t    Hour;
   Word_t    Minute;
   Word_t    Second;
   Boolean_t UTC_Time;
   int       UTC_Offset;
} MAP_TimeDate_t;

#define MAP_TIME_DATE_SIZE                              (sizeof(MAP_TimeDate_t))

   /*! The following structure is used to specify the filtering options
    * that can be used to filter the Message Listing.  The OptionMask
    * parameter is a bit mask that identifies which filter parameter in
    * the structure is valid.  The bit masks are defined above and all
    * masks start with MAP_MESSAGE_LISTING_INFO_OPTION_MASK_xxx.
    */
typedef struct _tagMAP_Message_Listing_Info_t
{
   Word_t          OptionMask;
   Byte_t          SubjectLength;
   DWord_t         ParameterMask;
   Byte_t          FilterMessageType;
   MAP_TimeDate_t  FilterPeriodBegin;
   MAP_TimeDate_t  FilterPeriodEnd;
   Byte_t          FilterReadStatus;
   char           *FilterRecipient;
   char           *FilterOriginator;
   Byte_t          FilterPriority;
} MAP_Message_Listing_Info_t;

#define MAP_MESSAGE_LISTING_INFO_DATA_SIZE(_x, _y) (sizeof(MAP_Message_Listing_Info_t) + (_x) + (_y))

   /*! The following enumeration covers all the events generated by the
    * MAP Profile.  These are used to determine the type of each event
    * generated, and to ensure the proper union element is accessed for
    * the MAP_Event_Data_t structure.
    */
typedef enum
{
   etMAP_Open_Request_Indication, /*!< Dispatched when a remote client requests to connect to a local server. */
   etMAP_Open_Port_Indication, /*!< Dispatched when a remote Client connects to a Message Access Server or a Message Notification Server. */
   etMAP_Open_Port_Confirmation, /*!< Dispatched to the Initiator to indicate the success or failure of a previously submitted Connection Attempt to a remote Server Port. */
   etMAP_Close_Port_Indication, /*!< Dispatched when the Servers and Clients when a service connection is disconnected. */
   etMAP_Send_Event_Indication, /*!< Dispatched to the Message Notification Server when a Message Notification Client send a Send Event Request. */
   etMAP_Send_Event_Confirmation, /*!< Dispatched to the Message Notification Client when a Message Notification Server sends a Send Event Response. */
   etMAP_Notification_Registration_Indication, /*!< Dispatched to the Message Access Server when the Message Access Client send a Notification Registration Request. */
   etMAP_Notification_Registration_Confirmation, /*!< Dispatched to the Message Access Client when the Message Access Server send a Notification Registration Response. */
   etMAP_Get_Folder_Listing_Indication, /*!< Dispatched to the Message Access Server when the Client sends a Get Folder Listing Request. */
   etMAP_Get_Folder_Listing_Confirmation, /*!< Dispatched to the Message Access Client when the Message Access Server sends a Get Folder Listing Response. */
   etMAP_Get_Message_Listing_Indication, /*!< Dispatched to the Message Access Server when the Client sends a Get Message Listing Request. */
   etMAP_Get_Message_Listing_Confirmation, /*!< Dispatched to the Message Access Client when the Message Access Server sends a Get Message Listing Response. */
   etMAP_Get_Message_Indication, /*!< Dispatched to the Message Access Server when the Client sends a Get Message Request. */
   etMAP_Get_Message_Confirmation, /*!< Dispatched to the Message Access Client when the Message Access Server sends a Get Message Response. */
   etMAP_Set_Message_Status_Indication, /*!< Dispatched to the Message Access Server when the Client sends a Set Message Status Request. */
   etMAP_Set_Message_Status_Confirmation, /*!< Dispatched to the Message Access Client when the Message Access Server sends a Set Message Status Response. */
   etMAP_Push_Message_Indication, /*!< Dispatched to the Message Access Server when the Client sends a Push Message Request. */
   etMAP_Push_Message_Confirmation, /*!< Dipatched to the Message Access Client when the Message Access Server sends a Push Message Response. */
   etMAP_Update_Inbox_Indication, /*!< Dispatched to the Message Access Server when the Client sends an Update Inbox Request. */
   etMAP_Update_Inbox_Confirmation, /*!< Dipatched to the Message Access Client when the Message Access Server sends an Update Inbox Response. */
   etMAP_Set_Folder_Indication, /*!< Dispatched to the Message Access Server when the Client sends a Set Folder Request. */
   etMAP_Set_Folder_Confirmation, /*!< Dipatched to the Message Access Client when the Message Access Server sends a Set Folder Response. */
   etMAP_Abort_Indication, /*!< Dispatched to the Client/Server when the Server/Client sends an Abort Request. */
   etMAP_Abort_Confirmation /*!< Dipatched to the Client/Server when the Server/Client sends an Abort Response. */
} MAP_Event_Type_t;

   /*! The following event structure is used when a remote client
    * requests a connection to a local server.  The MAPID member
    * specifies the identifier of the Local Message Access Profile
    * Server being connect with. The ServerPort member specifies the
    * server port of that Local Message Access Profile Server being
    * connected with.  The BD_ADDR member specifies the address of the
    * Remote Client requesting the connection to the Local Server.
    * \note This event is only dispatched to servers that are in
    *            Manual Accept Mode.
    * \note This event must be responded to with the
    *            MAP_Open_Request_Response() function in order to accept
    *            or reject the outstanding Open Request.
    */
typedef struct _tagMAP_Open_Request_Indication_Data_t
{
   unsigned int MAPID;
   BD_ADDR_t    BD_ADDR;
} MAP_Open_Request_Indication_Data_t;

#define MAP_OPEN_REQUEST_INDICATION_DATA_SIZE           (sizeof(MAP_Open_Request_Indication_Data_t))

   /*! The following event structure is used when a MAP Profile Client
    * Connects to a local MAP Profile Server.  The MAP ID member
    * specifies the local Server that has been connected to and the
    * BD_ADDR member specifies the Client Bluetooth Device that has
    * connected to the specified Server.
    */
typedef struct _tagMAP_Open_Port_Indication_Data_t
{
   unsigned int MAPID;
   BD_ADDR_t    BD_ADDR;
} MAP_Open_Port_Indication_Data_t;

#define MAP_OPEN_PORT_INDICATION_DATA_SIZE              (sizeof(MAP_Open_Port_Indication_Data_t))

   /*! The following event structure is used when a MAP Client receives
    * the Connection Response from a remote MAP Profile Server to which
    * it has previously attempted to connect.  The MAPID member
    * specifies the local Client MAP ID that was returned when the
    * connection request was made.  The OpenStatus represents the
    * Connection Status of the Request.
    */
typedef struct _tagMAP_Open_Port_Confirmation_Data_t
{
   unsigned int MAPID;
   unsigned int OpenStatus;
} MAP_Open_Port_Confirmation_Data_t;

#define MAP_OPEN_PORT_CONFIRMATION_DATA_SIZE            (sizeof(MAP_Open_Port_Confirmation_Data_t))

   /*! The following event structure is used when a MAP Profile Server or
    * Client connection is disconnected.  The MAPID member specifies the
    * local connection from which the remote device has disconnected.
    */
typedef struct _tagMAP_Close_Port_Indication_Data_t
{
   unsigned int MAPID;
} MAP_Close_Port_Indication_Data_t;

#define MAP_CLOSE_PORT_INDICATION_DATA_SIZE             (sizeof(MAP_Close_Port_Indication_Data_t))

   /*! The following event structure is used when a Message Access Server
    * receives a request from the client to modify a connection to the
    * clients Message Notification Server.  The MAPID identifies the MAP
    * Server on which the request was received.  The NotificationStatus
    * parameter indicates whether the connection should be established
    * or disconnected.  If the value is 0, then a disconnect of the
    * connection is being requested, else the connection should be
    * established.
    */
typedef struct _tagMAP_Notification_Registration_Indication_Data_t
{
   unsigned int  MAPID;
   BD_ADDR_t     BD_ADDR;
   Boolean_t     NotificationStatus;
} MAP_Notification_Registration_Indication_Data_t;

#define MAP_NOTIFICATION_REGISTRATION_INDICATION_DATA_SIZE (sizeof(MAP_Notification_Registration_Indication_Data_t))

   /*! The following event structure is used when a response to the
    * Notification Registration request have been received by the
    * Client.  The ResponseCode indicates the success of the request.
    */
typedef struct _tagMAP_Notification_Registration_Confirmation_Data_t
{
   unsigned int  MAPID;
   Byte_t        ResponseCode;
} MAP_Notification_Registration_Confirmation_Data_t;

#define MAP_NOTIFICATION_REGISTRATION_CONFIRMATION_DATA_SIZE (sizeof(MAP_Notification_Registration_Confirmation_Data_t))

   /*! The following event structure is used when an Event Object is
    * received from the Notification Client.  The MAPID identifies the
    * Server that received the event.  The MASInstance ID identifies the
    * Instance that originated the event.  The DataLength and DataBuffer
    * members are used to access the Event Object.  The Final Flag is
    * used to indicate whether this is the last segment of the object.
    */
typedef struct _tagMAP_Send_Event_Indication_Data_t
{
   unsigned int  MAPID;
   Byte_t        MASInstanceID;
   unsigned int  DataLength;
   Byte_t       *DataBuffer;
   Boolean_t     Final;
} MAP_Send_Event_Indication_Data_t;

#define MAP_SEND_EVENT_INDICATION_DATA_SIZE             (sizeof(MAP_Send_Event_Indication_Data_t))

   /*! The following event structure is used when a Message Notification
    * Client receives an acknowledgment from the Notification Server for
    * a previously sent event.  The MAPID identifies the Client that
    * received the response.  The ResponseCode indicates the success of
    * the event.
    */
typedef struct _tagMAP_Send_Event_Confirmation_Data_t
{
   unsigned int MAPID;
   Byte_t       ResponseCode;
} MAP_Send_Event_Confirmation_Data_t;

#define MAP_SEND_EVENT_CONFIRMATION_DATA_SIZE           (sizeof(MAP_Send_Event_Confirmation_Data_t))

   /*! The following event structure is used when a Message Access Server
    * receives a request from a client to return a Folder Listing
    * Object.  The MAPID identifies the server that received the
    * request.  MaxListCount indicates the maximum number of entries
    * that can be received by the client.  If MaxListCount is Zero, then
    * the client is requesting a response that identifies the number of
    * folder in the current directory.  If MaxListCount is non-Zero,
    * then ListStartOffset indicates the list index that should be used
    * as the first entry to return.
    */
typedef struct _tagMAP_Get_Folder_Listing_Indication_Data_t
{
   unsigned int MAPID;
   Word_t       MaxListCount;
   Word_t       ListStartOffset;
} MAP_Get_Folder_Listing_Indication_Data_t;

#define MAP_GET_FOLDER_LISTING_INDICATION_DATA_SIZE    (sizeof(MAP_Get_Folder_Listing_Indication_Data_t))

   /*! The following event structure is used when a Message Access client
    * receives a Folder Listing from the remote server.  The MAPID
    * identifies the client that generated the event.  The ResponseCode
    * indicates the success of the request.  If the request was made
    * with MaxListCount set to Zero, then the NumberOfFolders indicates
    * the number of folders that are in the current directory.  The
    * DataLength and DataBuffer members are used to access the Folder
    * Listing Object.
    */
typedef struct _tagMAP_Get_Folder_Listing_Confirmation_Data_t
{
   unsigned int   MAPID;
   Byte_t         ResponseCode;
   Word_t         NumberOfFolders;
   unsigned int   DataLength;
   Byte_t        *DataBuffer;
} MAP_Get_Folder_Listing_Confirmation_Data_t;

#define MAP_GET_FOLDER_LISTING_CONFIRMATION_DATA_SIZE  (sizeof(MAP_Get_Folder_Listing_Confirmation_Data_t))

   /*! The following event structure is used when a Message Access Server
    * receives a request from a client to return a Message Listing
    * Object.  The MAPID identifies the server that received the
    * request.  The Folder Name identifies the folder where the message
    * list is to be generated.  If FolderName is NULL, then the current
    * folder is assumed.  The FolderName must be formatted as a NULL
    * terminated UNICODE string.  MaxListCount indicates the maximum
    * number of message entries that can be received by the client.  If
    * MaxListCount is Zero, then the client is requesting a response
    * that identifies the number of messages in the specified directory.
    * If MaxListCount is non-Zero, then ListStartOffset indicates the
    * list index that should be used as the first entry to return.  The
    * ListingInfo structure contains a bitmask to identify the requested
    * filtering options and the data that pertains to the selected
    * options.
    */
typedef struct _tagMAP_Get_Message_Listing_Indication_Data_t
{
   unsigned int                MAPID;
   Word_t                     *FolderName;
   Word_t                      MaxListCount;
   Word_t                      ListStartOffset;
   MAP_Message_Listing_Info_t  ListingInfo;
} MAP_Get_Message_Listing_Indication_Data_t;

#define MAP_GET_MESSAGE_LISTING_INDICATION_DATA_SIZE   (sizeof(MAP_Get_Message_Listing_Indication_Data_t))

   /*! The following event structure is used when a Message Access client
    * receives a Message Listing from the remote server.  The MAPID
    * identifies the client that generated the event.  The ResponseCode
    * indicates the success of the request.  The NewMessage parameter
    * indicates whether the listing contains any new message received.
    * If the request was made with MaxListCount set to Zero, then the
    * NumberOfMessages indicates the number of Messages that are in the
    * current directory.  The DataLength and DataBuffer members are used
    * to access the Message Listing Object.
    */
typedef struct _tagMAP_Get_Message_Listing_Confirmation_Data_t
{
   unsigned int    MAPID;
   Byte_t          ResponseCode;
   Boolean_t       NewMessage;
   Word_t          NumberOfMessages;
   unsigned int    DataLength;
   Byte_t         *DataBuffer;
   MAP_TimeDate_t  MSETime;
} MAP_Get_Message_Listing_Confirmation_Data_t;

#define MAP_GET_MESSAGE_LISTING_CONFIRMATION_DATA_SIZE (sizeof(MAP_Get_Message_Listing_Confirmation_Data_t))

   /*! The following event structure is used when a Message Access Server
    * receives a request from a client to return a Message Object.  The
    * MAPID identifies the server that received the request.  The
    * Attachment Flag indicates whether any attachments should be
    * included in the Message Object.  The CharSet parameter identifies
    * the character set that should be used when formatting the Message
    * Object.  The FractionalType identifies a portion of the message
    * that is being requested when supported by the server.  The Message
    * Handle is a NULL terminated 16 character hexadecimal string that
    * identifies the message that is being requested.  The Message
    * Handle value is obtained from the Message Listing Object.
    */
typedef struct _tagMAP_Get_Message_Indication_Data_t
{
   unsigned int           MAPID;
   Boolean_t              Attachment;
   MAP_CharSet_t          CharSet;
   MAP_Fractional_Type_t  FractionalType;
   char                  *MessageHandle;
} MAP_Get_Message_Indication_Data_t;

#define MAP_GET_MESSAGE_INDICATION_DATA_SIZE                (sizeof(MAP_Get_Message_Indication_Data_t))

   /*! The following event structure is used when a Message Access client
    * receives a Get Message response from the remote server.  The MAPID
    * identifies the client that generated the event.  The ResponseCode
    * indicates the success of the request.  The NewMessage parameter
    * indicates whether the listing contains any new message received.
    * The FractionalType identifies the segment of the message that is
    * being received.  The DataLength and DataBuffer members are used to
    * access the Message Object.
    */
typedef struct _tagMAP_Get_Message_Confirmation_Data_t
{
   unsigned int           MAPID;
   Byte_t                 ResponseCode;
   MAP_Fractional_Type_t  FractionalType;
   unsigned int           DataLength;
   Byte_t                *DataBuffer;
} MAP_Get_Message_Confirmation_Data_t;

#define MAP_GET_MESSAGE_CONFIRMATION_DATA_SIZE          (sizeof(MAP_Get_Message_Confirmation_Data_t))

   /*! The following event structure is used when a Message Access Server
    * receives a request from a client to return change the status of a
    * Message.  The MAPID identifies the server that received the
    * request.  The Message Handle is a NULL terminated 16 character
    * hexadecimal string that identifies the message whose status is to
    * be changed.  The StatusIndicator identifies the status type to be
    * modified and the StatusValue specifies the new state.
    */
typedef struct _tagMAP_Set_Message_Status_Indication_Data_t
{
   unsigned int            MAPID;
   char                   *MessageHandle;
   MAP_Status_Indicator_t  StatusIndicator;
   Boolean_t               StatusValue;
} MAP_Set_Message_Status_Indication_Data_t;

#define MAP_SET_MESSAGE_STATUS_INDICATION_DATA_SIZE         (sizeof(MAP_Set_Message_Status_Indication_Data_t))

   /*! The following event structure is used when a Message Access client
    * receives a Set Message Status response from the remote server.
    * The MAPID identifies the client that generated the event.  The
    * ResponseCode indicates the success of the request.
    */
typedef struct _tagMAP_Set_Message_Status_Confirmation_Data_t
{
   unsigned int  MAPID;
   unsigned char ResponseCode;
} MAP_Set_Message_Status_Confirmation_Data_t;

#define MAP_SET_MESSAGE_STATUS_CONFIRMATION_DATA_SIZE     (sizeof(MAP_Set_Message_Status_Confirmation_Data_t))

   /*! The following event structure is used when a Message Access Server
    * receives a request from a client to send a Message.  The MAPID
    * identifies the server that received the request.  The Folder Name
    * identifies the folder where the message list is to be generated.
    * If FolderName is NULL, then the current folder is assumed.  The
    * FolderName must be formatted as a NULL terminated UNICODE string.
    * The Transparent Flag, when TRUE, instructs that server to not
    * place a copy of the message in the Sent Folder.  The Retry Flag
    * indicates whether the server should retry the sending of the
    * message when a previous attempt fails.  The CharSet identifies the
    * character set that the Message Object is formatted in.  DataLength
    * and DataBuffer members are used to access the Message Object.  The
    * Final Flag is used to indicate whether this is the final segment
    * of the Object.
    */
typedef struct _tagMAP_Push_Message_Indication_Data_t
{
   unsigned int   MAPID;
   Word_t        *FolderName;
   Boolean_t      Transparent;
   Boolean_t      Retry;
   MAP_CharSet_t  CharSet;
   unsigned int   DataLength;
   Byte_t        *DataBuffer;
   Boolean_t      Final;
} MAP_Push_Message_Indication_Data_t;

#define MAP_PUSH_MESSAGE_INDICATION_DATA_SIZE              (sizeof(MAP_Push_Message_Indication_Data_t))

   /*! The following event structure is used when a Message Access client
    * receives a Push Message response from the remote server.  The
    * MAPID identifies the client that generated the event.  The
    * ResponseCode indicates the success of the request.  The
    * MessageHandle is a NULL terminated 16 character hexadecimal string
    * that identifies the message.
    */
typedef struct _tagMAP_Push_Message_Confirmation_Data_t
{
   unsigned int   MAPID;
   Byte_t         ResponseCode;
   char          *MessageHandle;
} MAP_Push_Message_Confirmation_Data_t;

#define MAP_PUSH_MESSAGE_CONFIRMATION_DATA_SIZE             (sizeof(MAP_Push_Message_Confirmation_Data_t))

   /*! The following event structure is used when a Message Access Server
    * receives a request from a client to scan for new messages.  The
    * MAPID identifies the server that received the request.
    */
typedef struct _tagMAP_Update_Inbox_Indication_Data_t
{
   unsigned int MAPID;
} MAP_Update_Inbox_Indication_Data_t;

#define MAP_UPDATE_INBOX_INDICATION_DATA_SIZE               (sizeof(MAP_Update_Inbox_Indication_Data_t))

   /*! The following event structure is used when a Message Access client
    * receives an UpdateInbox response from the remote server.  The
    * MAPID identifies the client that generated the event.  The
    * ResponseCode indicates the success of the request.
    */
typedef struct _tagMAP_Update_Inbox_Confirmation_Data_t
{
   unsigned int   MAPID;
   Byte_t         ResponseCode;
} MAP_Update_Inbox_Confirmation_Data_t;

#define MAP_UPDATE_INBOX_CONFIRMATION_DATA_SIZE             (sizeof(MAP_Update_Inbox_Confirmation_Data_t))

   /*! The following event structure is used when a Message Access Server
    * receives a request to change the current directory.  The MAPID
    * identifies the server that received the request.  The PathOption
    * identifies the location of the new directory with respect to the
    * current directory.  FolderName along with the PathOption
    * identifies the folder where the path should be set.  The
    * FolderName must be formatted as a NULL terminated UNICODE string.
    */
typedef struct _tagMAP_Set_Folder_Indication_Data_t
{
   unsigned int             MAPID;
   MAP_Set_Folder_Option_t  PathOption;
   Word_t                  *FolderName;
} MAP_Set_Folder_Indication_Data_t;

#define MAP_SET_FOLDER_INDICATION_DATA_SIZE            (sizeof(MAP_Set_Folder_Indication_Data_t))

   /*! The following event structure is used when a Message Access client
    * receives an Set Folder response from the remote server.  The MAPID
    * identifies the client that generated the event.  The ResponseCode
    * indicates the success of the request.
    */
typedef struct _tagMAP_Set_Folder_Confirmation_Data_t
{
   unsigned int  MAPID;
   Byte_t        ResponseCode;
} MAP_Set_Folder_Confirmation_Data_t;

#define MAP_SET_FOLDER_CONFIRMATION_DATA_SIZE          (sizeof(MAP_Set_Folder_Confirmation_Data_t))

   /*! The following MAP Profile Event is dispatched when a MAP Profile
    * Abort Request is received from a remote MAP Profile Client.  The
    * MAP ID member specifies the local Server that has received the
    * request.
    * \note When receiving this indication, the MAP profile has
    *          already automatically dispatched a response to the remote
    *          client.
    */
typedef struct _tagMAP_Abort_Indication_Data_t
{
   unsigned int MAPID;
} MAP_Abort_Indication_Data_t;

#define MAP_ABORT_INDICATION_DATA_SIZE                  (sizeof(MAP_Abort_Indication_Data_t))

   /*! The following MAP Profile Event is dispatched when a MAP Profile
    * Abort Response is received from a remote MAP Profile Server. The
    * MAP ID member specifies the local Client that has received the
    * response.
    */
typedef struct _tagMAP_Abort_Confirmation_Data_t
{
   unsigned int MAPID;
} MAP_Abort_Confirmation_Data_t;

#define MAP_ABORT_CONFIRMATION_DATA_SIZE                (sizeof(MAP_Abort_Confirmation_Data_t))

   /*! The following structure represents the container structure for
    * holding all MAP Profile Event Data.  This structure is received
    * for each event generated.  The Event_Data_Type member is used to
    * determine the appropriate union member element to access the
    * contained data.  The Event_Data_Size member contains the total
    * size of the data contained in this event.
    */
typedef struct _tagMAP_Event_Data_t
{
   MAP_Event_Type_t Event_Data_Type;
   Word_t           Event_Data_Size;
   union
   {
      MAP_Open_Request_Indication_Data_t                     *MAP_Open_Request_Indication_Data;
      MAP_Open_Port_Indication_Data_t                        *MAP_Open_Port_Indication_Data;
      MAP_Open_Port_Confirmation_Data_t                      *MAP_Open_Port_Confirmation_Data;
      MAP_Close_Port_Indication_Data_t                       *MAP_Close_Port_Indication_Data;
      MAP_Notification_Registration_Indication_Data_t        *MAP_Notification_Registration_Indication_Data;
      MAP_Notification_Registration_Confirmation_Data_t      *MAP_Notification_Registration_Confirmation_Data;
      MAP_Send_Event_Indication_Data_t                       *MAP_Send_Event_Indication_Data;
      MAP_Send_Event_Confirmation_Data_t                     *MAP_Send_Event_Confirmation_Data;
      MAP_Get_Folder_Listing_Indication_Data_t               *MAP_Get_Folder_Listing_Indication_Data;
      MAP_Get_Folder_Listing_Confirmation_Data_t             *MAP_Get_Folder_Listing_Confirmation_Data;
      MAP_Get_Message_Listing_Indication_Data_t              *MAP_Get_Message_Listing_Indication_Data;
      MAP_Get_Message_Listing_Confirmation_Data_t            *MAP_Get_Message_Listing_Confirmation_Data;
      MAP_Get_Message_Indication_Data_t                      *MAP_Get_Message_Indication_Data;
      MAP_Get_Message_Confirmation_Data_t                    *MAP_Get_Message_Confirmation_Data;
      MAP_Set_Message_Status_Indication_Data_t               *MAP_Set_Message_Status_Indication_Data;
      MAP_Set_Message_Status_Confirmation_Data_t             *MAP_Set_Message_Status_Confirmation_Data;
      MAP_Push_Message_Indication_Data_t                     *MAP_Push_Message_Indication_Data;
      MAP_Push_Message_Confirmation_Data_t                   *MAP_Push_Message_Confirmation_Data;
      MAP_Update_Inbox_Indication_Data_t                     *MAP_Update_Inbox_Indication_Data;
      MAP_Update_Inbox_Confirmation_Data_t                   *MAP_Update_Inbox_Confirmation_Data;
      MAP_Set_Folder_Indication_Data_t                       *MAP_Set_Folder_Indication_Data;
      MAP_Set_Folder_Confirmation_Data_t                     *MAP_Set_Folder_Confirmation_Data;
      MAP_Abort_Indication_Data_t                            *MAP_Abort_Indication_Data;
      MAP_Abort_Confirmation_Data_t                          *MAP_Abort_Confirmation_Data;
   } Event_Data;
} MAP_Event_Data_t;

#define MAP_EVENT_DATA_SIZE                       (sizeof(MAP_Event_Data_t))

   /*! The following declared type represents the Prototype Function for
    * a MAP Profile Event Receive Data Callback.  This function will be
    * called whenever an MAP Profile Event occurs that is associated
    * with the specified Bluetooth Stack ID.  This function passes to
    * the caller the Bluetooth Stack ID, the MAP Event Data that
    * occurred and the MAP Profile Event Callback Parameter that was
    * specified when this Callback was installed.  The caller is free to
    * use the contents of the MAP Profile Event Data ONLY in the context
    * of this callback.  If the caller requires the Data for a longer
    * period of time, then the callback function MUST copy the data into
    * another Data Buffer This function is guaranteed NOT to be invoked
    * more than once simultaneously for the specified installed callback
    * (i.e. this function DOES NOT have be re-entrant).  It needs to be
    * noted however, that if the same Callback is installed more than
    * once, then the callbacks will be called serially.  Because of
    * this, the processing in this function should be as efficient as
    * possible.  It should also be noted that this function is called in
    * the Thread Context of a Thread that the User does NOT own.
    * Therefore, processing in this function should be as efficient as
    * possible (this argument holds anyway because another MAP Profile
    * Event will not be processed while this function call is
    * outstanding).
    * \note This function MUST NOT Block and wait for events that
    *            can only be satisfied by Receiving MAP Profile Event
    *            Packets.  A Deadlock WILL occur because NO MAP Event
    *            Callbacks will be issued while this function is
    *            currently outstanding.
    */
typedef void (BTPSAPI *MAP_Event_Callback_t)(unsigned int BluetoothStackID, MAP_Event_Data_t *MAP_Event_Data, unsigned long CallbackParameter);

   /*! @brief The following function is responsible for opening a local MAP
    * Server.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID on which to
    * open the server.  @param MessageAccessServiceServerPort The second parameter is the Port on which to
    * open this server, and *MUST* be between MAP_PORT_NUMBER_MINIMUM
    * and MAP_PORT_NUMBER_MAXIMUM.  @param EventCallback The third parameter is the Callback
    * function to call when an event occurs on this Server Port.
    * @param CallbackParameter The first parameter is a user-defined callback parameter that will be
    * passed to the callback function with each event.
    * @return This function returns a positive, non zero value if successful or a negative
    * return error code if an error occurs.  A successful return code
    * will be a MAP Profile ID that can be used to reference the Opened
    * MAP Profile Server Port in ALL other MAP Server functions in this
    * module.  Once an MAP Profile Server is opened, it can only be
    * Un-Registered via a call to the MAP_Close_Server() function
    * (passing the return value from this function).
    */
BTPSAPI_DECLARATION int BTPSAPI MAP_Open_Message_Access_Server(unsigned int BluetoothStackID, unsigned int MessageAccessServiceServerPort, MAP_Event_Callback_t EventCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_MAP_Open_Message_Access_Server_t)(unsigned int BluetoothStackID, unsigned int MessageAccessServiceServerPort, MAP_Event_Callback_t EventCallback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is responsible for opening a local MAP
    * Server.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID on which to
    * open the server.  @param MessageNotificationServiceServerPort The second parameter is the Port on which to
    * open this server, and *MUST* be between MAP_PORT_NUMBER_MINIMUM
    * and MAP_PORT_NUMBER_MAXIMUM.  @param MAS_MAPID The third parameter is the MAPID for
    * the Message Access Client that the server is being associated
    * with. @param EventCallback The fourth paramerer is a Callback function to call when an
    * event occurs on this Server Port.  @param CallbackParameter The first parameter is a
    * user-defined callback parameter that will be passed to the
    * callback function with each event.  @return This function returns a
    * positive, non zero value if successful or a negative return error
    * code if an error occurs.  A successful return code will be a MAP
    * Profile ID that can be used to reference the Opened MAP Profile
    * Server Port in ALL other MAP Server functions in this module.
    * Once an MAP Profile Server is opened, it can only be Un-Registered
    * via a call to the MAP_Close_Server() function (passing the return
    * value from this function).
    */
BTPSAPI_DECLARATION int BTPSAPI MAP_Open_Message_Notification_Server(unsigned int BluetoothStackID, unsigned int MessageNotificationServiceServerPort, unsigned int MAS_MAPID, MAP_Event_Callback_t EventCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_MAP_Open_Message_Notification_Server_t)(unsigned int BluetoothStackID, unsigned int MessageNotificationServiceServerPort, unsigned int MAS_MAPID, MAP_Event_Callback_t EventCallback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is responsible for responding to an
    * individual request to connect to a local Server.
    * @param BluetoothStackID The first parameter to this function is the Bluetooth Stack ID of the
    * Bluetooth Stack associated with the Message Access Profile Server
    * that is responding to the request.  @param MAPID The second parameter to this
    * function is the MAP ID of the Server for which a connection
    * request was received.  @param AcceptConnection The final parameter to this function
    * specifies whether to accept the pending connection request (or to
    * reject the request).  @return This function returns zero if successful, or
    * a negative return error code if an error occurred.
    * \note The connection to the server is not established until a
    *            etMAP_Open_Service_Port_Indication event has occurred.
    */
BTPSAPI_DECLARATION int BTPSAPI MAP_Open_Request_Response(unsigned int BluetoothStackID, unsigned int MAPID, Boolean_t AcceptConnection);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_MAP_Open_Request_Response_t)(unsigned int BluetoothStackID, unsigned int MAPID, Boolean_t AcceptConnection);
#endif

   /*! @brief The following function adds a MAP Server (MSE) Service Record to
    * the SDP Database.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID
    * of the stack on which to register this SDP record.
    * @param MAPID The second parameter is the MAP ID that was returned by a previous call to
    * MAP_Open_Message_Access_Server().  @param ServiceName The third parameter is a
    * pointer to ASCII, NULL terminated string containing the Service
    * Name to include within the SDP Record.  @param MASInstance The fourth parameter is a
    * user defined MAS Instance value. @param SupportedMessageTypes The fifth parameter is the
    * Supported Message Type Bitmask value.  @param SDPServiceRecordHandle The final parameter is a
    * pointer to a DWord_t which receives the SDP Service Record Handle
    * if this function successfully creates an SDP Service Record.  If
    * @return This function returns zero, then the SDPServiceRecordHandle entry
    * will contain the Service Record Handle of the added SDP Service
    * Record.  If this function fails, a negative return error code will
    * be returned (see BTERRORS.H) and the SDPServiceRecordHandle value
    * will be undefined.
    * \note This function should only be called with the MAP ID that
    *          was returned from the MAP_Open_Server_Port() function.
    *          This function should NEVER be used with MAP ID returned
    *          from the MAP_Open_Remote_Server_Port() function.
    * \note The Service Record Handle that is returned from this
    *          function will remain in the SDP Record Database until it
    *          is deleted by calling the MAP_Un_Register_SDP_Record()
    *          function.
    * \note A MACRO is provided to Delete the Service Record from the
    *          SDP Data Base.  This MACRO maps the
    *          MAP_Un_Register_SDP_Record() to the
    *          SDP_Delete_Service_Record() function.
    * \note The Service Name is always added at Attribute ID 0x0100.
    *          A Language Base Attribute ID List is created that
    *          specifies that 0x0100 is UTF-8 Encoded, English Language.
    */
BTPSAPI_DECLARATION int BTPSAPI MAP_Register_Message_Access_Server_SDP_Record(unsigned int BluetoothStackID, unsigned int MAPID, char *ServiceName, Byte_t MASInstance, Byte_t SupportedMessageTypes, DWord_t *SDPServiceRecordHandle);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_MAP_Register_Message_Access_Server_SDP_Record_t)(unsigned int BluetoothStackID, unsigned int MAPID, char *ServiceName, Byte_t MASInstance, Byte_t SupportedMessageTypes, DWord_t *SDPServiceRecordHandle);
#endif

   /*! @brief The following function adds a MAP Client (PCE) Service Record to
    * the SDP Database.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID
    * of the stack on which to register this SDP record.
    * @param MAPID The second parameter is the MAP ID that was returned by a previous call to
    * MAP_Open_Message_Notification_Server().  @param ServiceName The third parameter is a
    * pointer to ASCII, NULL terminated string containing the Service
    * Name to include within the SDP Record.  @param SDPServiceRecordHandle The final parameter is a
    * pointer to a DWord_t which receives the SDP Service Record Handle
    * if this function successfully creates an SDP Service Record.  If
    * @return This function returns zero, then the SDPServiceRecordHandle entry
    * will contain the Service Record Handle of the added SDP Service
    * Record.  If this function fails, a negative return error code will
    * be returned (see BTERRORS.H) and the SDPServiceRecordHandle value
    * will be undefined.
    * \note The Service Record Handle that is returned from this
    *          function will remain in the SDP Record Database until it
    *          is deleted by calling the MAP_Un_Register_SDP_Record()
    *          function.
    * \note A MACRO is provided to Delete the Service Record from the
    *          SDP Data Base.  This MACRO maps the
    *          MAP_Un_Register_SDP_Record() to the
    *          SDP_Delete_Service_Record() function.
    * \note The Service Name is always added at Attribute ID 0x0100.
    *          A Language Base Attribute ID List is created that
    *          specifies that 0x0100 is UTF-8 Encoded, English Language.
    */
BTPSAPI_DECLARATION int BTPSAPI MAP_Register_Message_Notification_Server_SDP_Record(unsigned int BluetoothStackID, unsigned int MAPID, char *ServiceName, DWord_t *SDPServiceRecordHandle);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_MAP_Register_Message_Notification_Server_SDP_Record_t)(unsigned int BluetoothStackID, unsigned int MAPID, char *ServiceName, DWord_t *SDPServiceRecordHandle);
#endif


   /*! The following MACRO is a utility MACRO that deletes the MAP
    * Profile Server SDP Service Record (specified by the third parameter) from the SDP Database.  This MACRO maps to the
    * SDP_Delete_Service_Record() function.  This MACRO is only provided
    * so that the caller doesn't have to sift through the SDP API for
    * very simplistic applications.  This function accepts as input the
    * Bluetooth Stack ID of the Bluetooth Protocol Stack that the
    * Service Record exists on, the MAP ID (returned from a successful
    * call to open a server port), and the SDP Service Record Handle.
    * The SDP Service Record Handle was returned via a successful call
    * to the MAP_Register_Server_SDP_Record() function.  See the
    * MAP_Register_Server_SDP_Record() function for more information.
    * This MACRO returns the result of the SDP_Delete_Service_Record()
    * function, which is zero for success or a negative return error
    * code (see BTERRORS.H).  If using this function to remove a Client
    * SDP record, simply set MAPID to zero.
    */
#define MAP_Un_Register_SDP_Record(__BluetoothStackID, __MAPID, __SDPRecordHandle) (SDP_Delete_Service_Record(__BluetoothStackID, __SDPRecordHandle))

   /*! @brief The following function is responsible for opening a connection to
    * a remote Message Access Server.  @param BluetoothStackID The first parameter is the
    * Bluetooth Stack ID of the local Bluetooth stack.
    * @param BD_ADDR The second parameter is the remote Bluetooth Device Address of the Bluetooth
    * MAP Profile Server with which to connect. @param ServerPort The third parameter
    * specifies the remote server port with which to connect.
    * @param EventCallback The fourth parameter specifies the MAP Profile Event Callback Function.
    * @param CallbackParameter The final parameter specifies the Callback
    * Parameter to associate with this MAP Profile Client.
    * The ServerPort parameter *MUST* be between MAP_PORT_NUMBER_MINIMUM
    * and MAP_PORT_NUMBER_MAXIMUM.  @return This function returns a positive,
    * non zero, value if successful or a negative return error code if
    * an error occurs.  A successful return code will be a MAP ID that
    * can be used to reference the remote opened MAP Profile Server in
    * ALL other MAP Profile Client functions in this module.  Once a
    * remote server is opened, it can only be closed via a call to the
    * MAP_Close_Connection() function (passing the return value from
    * this function).
    */
BTPSAPI_DECLARATION int BTPSAPI MAP_Open_Remote_Message_Access_Server_Port(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, unsigned int ServerPort, MAP_Event_Callback_t EventCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_MAP_Open_Remote_Message_Access_Server_Port_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, unsigned int ServerPort, MAP_Event_Callback_t EventCallback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is responsible for opening a connection to
    * a remote Message Notification Server.  @param BluetoothStackID The first parameter is the
    * Bluetooth Stack ID of the local Bluetooth stack.
    * @param LocalMAPID The second parameter is the MAP ID of the local Message Access Server making
    * the connection.  @param ServerPort The third parameter specifies the remote server
    * port with which to connect.
    * @param EventCallback The fourth parameter specifies the MAP Profile Event Callback Function.
    * @param CallbackParameter The final parameter specifies the Callback Parameter to
    * associate with this MAP Profile Client.  The ServerPort parameter
    * *MUST* be between MAP_PORT_NUMBER_MINIMUM and
    * MAP_PORT_NUMBER_MAXIMUM.  @return This function returns a positive, non
    * zero, value if successful or a negative return error code if an
    * error occurs.  A successful return code will be a MAP ID that can
    * be used to reference the remote opened MAP Profile Server in ALL
    * other MAP Notification Client functions in this module.  Once a
    * remote server is opened, it can only be closed via a call to the
    * MAP_Close_Connection() function (passing the return value from
    * this function).
    */
BTPSAPI_DECLARATION int BTPSAPI MAP_Open_Remote_Message_Notification_Server_Port(unsigned int BluetoothStackID, unsigned int LocalMAPID, unsigned int ServerPort, MAP_Event_Callback_t EventCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_MAP_Open_Remote_Message_Notification_Server_Port_t)(unsigned int BluetoothStackID, unsigned int LocalMAPID, unsigned int ServerPort, MAP_Event_Callback_t EventCallback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is responsible for closing a currently
    * open/registered Message Access Profile server.  This function is
    * capable of closing servers opened via a call to
    * MAP_Open_Message_Access_Server() and
    * MAP_Open_Message_Notification_Server().  @param BluetoothStackID The first parameter to
    * this function is the Bluetooth Stack ID of the Bluetooth Protocol
    * Stack Instance that is associated with the Message Access Profile
    * Server being closed.  @param MAPID The second parameter to this function is the
    * MAP ID of the Profile Server to be closed.  @return This function returns
    * zero if successful, or a negative return value if there was an
    * error.
    * \note This function only closes/un-registers servers it does
    *            NOT delete any SDP Service Record Handles that are
    *            registered for the specified server..
    */
BTPSAPI_DECLARATION int BTPSAPI MAP_Close_Server(unsigned int BluetoothStackID, unsigned int MAPID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_MAP_Close_Server_t)(unsigned int BluetoothStackID, unsigned int MAPID);
#endif

   /*! @brief The following function is responsible for closing a currently
    * ongoing MAP Profile connection.  @param BluetoothStackID The first parameter is the
    * Bluetooth Stack ID of the Bluetooth Protocol Stack Instance that
    * is associated with the MAP Profile connection being closed.
    * @param MAPID The second parameter to this function is the MAP ID of the MAP Profile
    * connection to be closed.  @return This function returns zero if
    * successful, or a negative return value if there was an error.
    * \note If this function is called with a Server MAP ID (a value
    *          returned from a call to MAP_Open_Server_Port()) any
    *          clients currently connected to this server will be
    *          terminated, but the server will remained open and
    *          registered.  If this function is called using a Client
    *          MAP ID (a value returned from a call to
    *          MAP_Open_Remote_Server_Port()), the client connection
    *          will be terminated/closed entirely.
    */
BTPSAPI_DECLARATION int BTPSAPI MAP_Close_Connection(unsigned int BluetoothStackID, unsigned int MAPID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_MAP_Close_Connection_t)(unsigned int BluetoothStackID, unsigned int MAPID);
#endif

   /*! @brief The following function is responsible for sending an Disconnect
    * Request to the remote device/entity.  @param BluetoothStackID The first parameter to this
    * function is the Bluetooth Stack ID of the Bluetooth Protocol Stack
    * Instance that is associated with the MAP Profile connection being
    * disconnected.  @param MAPID The second parameter to this function is the MAP ID
    * of the MAP Profile connection to be disconnected (that was
    * returned by a call to MAP_Open_Remote_Server_Port()).
    * @return This function returns zero if successful, or a negative return value if
    * there was an error.
    * \note Use of this function is optional and is provided for full
    *          compliance with some OBEX applications.  Calling the
    *          MAP_Close_Connection() function will achieve the same
    *          results without sending the OBEX disconnect packet
    *          beforehand.  If is also possible to call this function
    *          and then immediately call MAP_Close_Connection() without
    *          waiting for a confirmation because a Disconnect Request
    *          cannot be failed.  Calling this function by itself and
    *          waiting for a response will cause the underlying
    *          connection to automatically be closed once the response
    *          is received.  This will generate a Close Port Indication.
    */
BTPSAPI_DECLARATION int BTPSAPI MAP_Disconnect_Request(unsigned int BluetoothStackID, unsigned int MAPID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_MAP_Disconnect_Request_t)(unsigned int BluetoothStackID, unsigned int MAPID);
#endif

   /*! @brief The following function is responsible for providing a mechanism to
    * query the current Message Access Profile Server Mode.
    * @param BluetoothStackID The first parameter to this function is the Bluetooth Stack ID of the
    * Bluetooth Protocol Stack Instance associated with the requested
    * servers Server Mode.  @param MAPID The second parameter to this function is the
    * MAP ID of the Profile Server in which to get the current Server
    * Mode Mask.  @param ServerModeMask The final parameter to this function is a pointer to a
    * variable which will receive the current Server Mode Mask.
    * @return This function returns zero if successful, or a negative return value if
    * there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI MAP_Get_Server_Mode(unsigned int BluetoothStackID, unsigned int MAPID, unsigned long *ServerModeMask);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_MAP_Get_Server_Mode_t)(unsigned int BluetoothStackID, unsigned int MAPID, unsigned long *ServerModeMask);
#endif

   /*! @brief The following function is responsible for providing a mechanism to
    * change the current Message Access Server Mode.
    * @param BluetoothStackID The first parameter to this function is the Bluetooth Stack ID of the
    * Bluetooth Protocol Stack Instance associated with the requested
    * servers Server Mode.
    * @param MAPID The second parameter to this function is the
    * MAP ID of the Profile Server in which to set the current Server
    * Mode Mask.  @param ServerModeMask The final parameter to this function is the new Server
    * Mode Mask to use.  @return This function returns zero if successful, or a
    * negative return value if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI MAP_Set_Server_Mode(unsigned int BluetoothStackID, unsigned int MAPID, unsigned long ServerModeMask);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_MAP_Set_Server_Mode_t)(unsigned int BluetoothStackID, unsigned int MAPID, unsigned long ServerModeMask);
#endif

   /*! @brief The following function is responsible for sending an Abort Request
    * to the remote Server.  @param BluetoothStackID The first parameter is the ID of
    * the Bluetooth Stack that is associated with this MAP Client. @param MAPID The
    * second parameter specifies the MAP ID for the local MAP Client.
    * @return This function returns zero if successful, or a negative return
    * value if there was an error.
    * \note Upon the reception of the Abort Confirmation Event it
    *            may be assumed that the currently on going transaction
    *            has been successfully aborted and new requests may be
    *            submitted.
    */
BTPSAPI_DECLARATION int BTPSAPI MAP_Abort_Request(unsigned int BluetoothStackID, unsigned int MAPID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_MAP_Abort_Request_t)(unsigned int BluetoothStackID, unsigned int MAPID);
#endif

   /*! @brief The following function is responsible for providing a mechanism to
    * Enable or Disable Notification messages from the remote Message
    * Access Server.  @param BluetoothStackID The first parameter to this function is the
    * Bluetooth Stack ID.  @param MAPID The second parameter to this function is the
    * MAP ID of the Service Client making this call.
    * @param Enabled The third parameter specifies if the Notifications should be Enabled or
    * Disabled.  @return This function returns zero if successful, or a negative
    * return value if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI MAP_Set_Notification_Registration_Request(unsigned int BluetoothStackID, unsigned int MAPID, Boolean_t Enabled);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_MAP_Set_Notification_Registration_Request_t)(unsigned int BluetoothStackID, unsigned int MAPID, Boolean_t Enabled);
#endif

   /*! @brief The following function is responsible for sending a Notification
    * Registration Response to the remote Client.  @param BluetoothStackID The first parameter
    * to this function is the Bluetooth Stack ID.  @param MAPID The second parameter
    * to this function is the MAP ID of the Server making this call.
    * @param ResponseCode The final parameter to this function is the Response Code to be
    * associated with this response.  @return This function returns zero if
    * successful, or a negative return value if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI MAP_Set_Notification_Registration_Response(unsigned int BluetoothStackID, unsigned int MAPID, Byte_t ResponseCode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_MAP_Set_Notification_Registration_Response_t)(unsigned int BluetoothStackID, unsigned int MAPID, Byte_t ResponseCode);
#endif

   /*! @brief The following function is responsible for providing a mechanism
    * for Message Notification Clients to dispatch an event to the
    * remote Server.
    * @param BluetoothStackID The first parameter to this function is the
    * Bluetooth Stack ID of the Bluetooth Protocol Stack Instance.
    * @param MAPID The second parameter to this function is the MAP ID of the Service
    * Client making this call.
    * @param DataLength The third parameter specifies the number
    * of bytes that are present in the object segment.
    * @param DataBuffer The fourth parameter in a pointer to the segment of the event object
    * data.
    * @param AmountSent The fifth parameter is a pointer to a variable used to return to the caller the
    * amount of data pointed to by the ObjectSegment parameter actually successfully sent in the request.
    * @param Final The final parameter to this function is a Boolean Flag
    * indicating if this is to be the final segment of the Event Object.
    * @return This function returns zero if successful, or a negative return
    * value if there was an error.
    * \note The EventObject is a "x-bt/MAP-event-report" character
    *          stream that is formatted as defined in the Message Access
    *          Profile Specification.
    */
BTPSAPI_DECLARATION int BTPSAPI MAP_Send_Event_Request(unsigned int BluetoothStackID, unsigned int MAPID, unsigned int DataLength, Byte_t *DataBuffer, unsigned int *AmountSent, Boolean_t Final);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_MAP_Send_Event_Request_t)(unsigned int BluetoothStackID, unsigned int MAPID, unsigned int DataLength, Byte_t *DataBuffer, unsigned int *AmountSent, Boolean_t Final);
#endif

   /*! @brief The following function is responsible for sending a Send Event
    * Response to the remote Client.  @param BluetoothStackID The first parameter to this
    * function is the Bluetooth Stack ID.  @param MAPID The second parameter to this
    * function is the MAP ID of the Server making this call.
    * @param ResponseCode The final parameter to this function is the Response Code to be associated
    * with this response. @return This function returns zero if successful, or
    * a negative return value if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI MAP_Send_Event_Response(unsigned int BluetoothStackID, unsigned int MAPID, Byte_t ResponseCode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_MAP_Send_Event_Response_t)(unsigned int BluetoothStackID, unsigned int MAPID, Byte_t ResponseCode);
#endif

   /*! @brief The following function generates a MAP Get Folder Listing Request
    * to the specified remote MAP Server. @param BluetoothStackID Te first parameter
    * is the ID of the Bluetooth Stack that is associated with
    * this MAP Client. @param MAPID The second parameter specifies the MAP ID for the
    * local MAP Client (returned from a successful call to the
    * MAP_Connect_Remote_Server_Port() function). @param MaxListCount The third
    * parameter is an unsigned integer that specifies the maximum number
    * of list entries the client can handle.  A MaxListCount of ZERO (0)
    * indicates that this is a request for the number accessible folders
    * in the current folder. @param ListStartOffset The final parameter specifies
    * the index requested by the Client in this Folder Listing.
    * @return This function returns zero if successful or a negative return error
    * code if there was an error.
    * \note A successful return code does not mean that the remote
    *          MAP Profile Server successfully processed the command.
    *          The caller needs to check the confirmation result to
    *          determine if the remote MAP Profile Server successfully
    *          executed the Request.
    * \note There can only be one outstanding MAP Profile Request
    *          active at any one time.  Because of this, another MAP
    *          Profile Request cannot be issued until either the current
    *          request is Aborted (by calling the MAP_Abort_Request()
    *          function) or the current Request is completed (this is
    *          signified by receiving a Confirmation Event in the MAP
    *          Profile Event Callback that was registered when the MAP
    *          Profile Port was opened).
    */
BTPSAPI_DECLARATION int BTPSAPI MAP_Get_Folder_Listing_Request(unsigned int BluetoothStackID, unsigned int MAPID, Word_t MaxListCount, Word_t ListStartOffset);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_MAP_Get_Folder_Listing_Request_t)(unsigned int BluetoothStackID, unsigned int MAPID, Word_t MaxListCount, Word_t ListStartOffset);
#endif

   /*! @brief The following function sends a MAP Get Folder Listing Response to
    * the specified remote MAP Client.  This is used for responding to a
    * MAP Get Folder Listing Indication. @param BluetoothStackID The first parameter
    * is the ID of the Bluetooth Stack that is associated with this MAP
    * Server. @param MAPID The second parameter specifies the MAP ID of the MAP
    * Server responding to the request. @param ResponseCode The third parameter is
    * the OBEX response code to include in the response. @param  FolderCount
    * The fourth parameter is a pointer to a variable that can
    * optionally contain the number of accessible folder entries in the
    * current folder.  This should ONLY be used if the received
    * indication indicated a request for Folder Listing by indicating a
    * MaxListCount = ZERO (0).  In all other instances this parameter
    * must be set to NULL. @param DataLength The fifth parameter indicates the
    * number of bytes that are contained in the object segment
    * (DataBuffer). @param DataBuffer The sixth parameter is a pointer to a byte
    * buffer containing a segment of the Folder Listing Object. @param AmountSent The final
    * parameter is a pointer to variable which will be
    * written with the actual amount of data that was able to be
    * included in the packet.  @return This function returns zero if successful
    * or a negative return error code if there was an error.
    * \note The FolderListingObject is a "x-obex/folder-listing"
    *          character stream that is formatted as defined in the
    *          IrOBEX Specification.
    * \note If FolderCount is not NULL, then the remaining parameters
    *          are ignored.
    * \note Including a DataBuffer pointer and setting DataLength > 0
    *          will cause a Body or End-of-Body header to be added to
    *          the packet, either on the first or subsequent packets.
    *          If the stack cannot include all the requested object
    *          (DataLength) in the current packet, a Body header will
    *          be used and AmountSent will reflect that not all of the
    *          data was sent.  If all data is included, an End-of-Body
    *          header will be used.
    * \note If AmountSent returns an amount smaller than the
    *          specified DataLength, not all the data was able to be
    *          sent.  This function should be called again with an
    *          adjusted DataLength and DataBuffer pointer to account
    *          for the data that was successfully sent.
    */
BTPSAPI_DECLARATION int BTPSAPI MAP_Get_Folder_Listing_Response(unsigned int BluetoothStackID, unsigned int MAPID, Byte_t ResponseCode, Word_t *FolderCount, unsigned int DataLength, Byte_t *DataBuffer, unsigned int *AmountSent);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_MAP_Get_Folder_Listing_Response_t)(unsigned int BluetoothStackID, unsigned int MAPID, Byte_t ResponseCode, Word_t *FolderCount, unsigned int DataLength, Byte_t *DataBuffer, unsigned int *AmountSent);
#endif

   /*! @brief The following function generates a MAP Get Message Listing Request
    * to the specified remote MAP Server. @param BluetoothStackID
    * The first parameter is the ID of the Bluetooth Stack that is associated with
    * this MAP Client. @param MAPID The second parameter specifies the MAP ID for the
    * local MAP Client (returned from a successful call to the
    * MAP_Connect_Remote_Server_Port() function). @param FolderName
    * The third parameter specifies the Folder from which the Message Listing is to be
    * retrieved.  If the parameter is NULL, the listing is made from the
    * current directory. @param MaxListCount The fourth parameter is an unsigned
    * integer that specifies the maximum number of list entries the
    * client can handle.  A MaxListCount of ZERO (0) indicates that this
    * is a request for the number of messages in the specified folder.
    * @param ListStartOffset The fifth parameter specifies the index requested by the
    * Client in this Listing. @param ListingInfo The final parameter is a
    * structure is used to specify
    * a number of filters and options that should be applied to the
    * request.  @return This function returns zero if successful or a negative
    * return error code if there was an error.
    * \note A successful return code does not mean that the remote
    *          MAP Profile Server successfully processed the command.
    *          The caller needs to check the confirmation result to
    *          determine if the remote MAP Profile Server successfully
    *          executed the Request.
    */
BTPSAPI_DECLARATION int BTPSAPI MAP_Get_Message_Listing_Request(unsigned int BluetoothStackID, unsigned int MAPID, Word_t *FolderName, Word_t MaxListCount, Word_t ListStartOffset, MAP_Message_Listing_Info_t *ListingInfo);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_MAP_Get_Message_Listing_Request_t)(unsigned int BluetoothStackID, unsigned int MAPID, Word_t *FolderName, Word_t MaxListCount, Word_t ListStartOffset, MAP_Message_Listing_Info_t *ListingInfo);
#endif

   /*! @brief The following function sends a MAP Get Message Listing Response to
    * the specified remote MAP Client.  This is used for responding to a
    * MAP Get Message Listing Indication. @param BluetoothStackID
    * The first parameter is the ID of the Bluetooth Stack that is associated with
    * this MAP Server. @param MAPID The second parameter specifies the MAP ID of the
    * MAP Server responding to the request. @param ResponseCode The third parameter
    * is the OBEX response code to include in the response.
    * @param MessageCount The fourth parameter contains the number of accessible message
    * entries. @param NewMessage The fifth parameter indicates if new messages have
    * arrived since last checked. @param CurrentTime The sixth parameter indicates
    * the time at which the response is being sent. @param DataLength
    * The seventh parameter defines the number of bytes that are included in the
    * object segment (DataBuffer). @param DataBuffer The eighth parameter is a
    * pointer to a byte buffer containing a segment of the Message
    * Listing Object. @param AmountSent The final parameter is a pointer to variable
    * which will be written with the actual amount of data that was able
    * to be included in the packet.  @return This function returns zero if
    * successful or a negative return error code if there was an error.
    * \note The MessageListingObject is a "x-bt/MAP-msg-listing"
    *          character stream that is formatted as defined in the
    *          Message Access Profile Specification.
    * \note If MessageCount is not NULL, then the remaining
    *          parameters are ignored.
    * \note CurrentTime is used by the receiver of this response to
    *          correlate the timestamps in the listing with the current
    *          time of the server.
    * \note Including a DataBuffer pointer and setting DataLength > 0
    *          will cause a Body or End-of-Body header to be added to
    *          the packet, either on the first or subsequent packets.
    *          If the stack cannot include all the requested object
    *          (DataLength) in the current packet, a Body header will be
    *          used and AmountSent will reflect that not all of the data
    *          was sent.  If all data is included, an End-of-Body header
    *          will be used.
    * \note If AmountSent returns an amount smaller than the
    *          specified DataLength, not all the data was able to be
    *          sent.  This function should be called again with an
    *          adjusted DataLength and DataBuffer pointer to account for
    *          the data that was successfully sent.
    */
BTPSAPI_DECLARATION int BTPSAPI MAP_Get_Message_Listing_Response(unsigned int BluetoothStackID, unsigned int MAPID, Byte_t ResponseCode, Word_t *MessageCount, Boolean_t NewMessage, MAP_TimeDate_t *CurrentTime, unsigned int DataLength, Byte_t *DataBuffer, unsigned int *AmountSent);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_MAP_Get_Message_Listing_Response_t)(unsigned int BluetoothStackID, unsigned int MAPID, Byte_t ResponseCode, Word_t *MessageCount, Boolean_t NewMessage, MAP_TimeDate_t *CurrentTime, unsigned int DataLength, Byte_t *DataBuffer, unsigned int *AmountSent);
#endif

   /*! @brief The following function generates a MAP Get Message Request to the
    * specified remote MAP Server. @param BluetoothStackID The first parameter is the
    * ID of the Bluetooth Stack that is associated with this MAP Client.
    * @param MAPID The second parameter specifies the MAP ID for the local MAP Client.
    * @param MessageHandle The third parameter is a pointer to a 16 byte Null
    * terminated Unicode Text string that identifies the message.
    * @param Attachment The fourth parameter is used to indicate if any existing
    * attachments to the specified message are to be included in the
    * response. @param CharSet The fifth parameter specifies the
    * format of the response message.
    * @param FractionalType The final parameter also specifies the format of the response message.
    * @return This function returns zero if
    * successful or a negative return error code if there was an error.
    * \note Specifying the FractionalType as ftUnfragmented causes
    *          no FractionalType Header to be added to the OBEX Header
    *          List.  This is the value that should be specified for a
    *          a message that is non-fragmented.
    */
BTPSAPI_DECLARATION int BTPSAPI MAP_Get_Message_Request(unsigned int BluetoothStackID, unsigned int MAPID, char *MessageHandle, Boolean_t Attachment, MAP_CharSet_t CharSet, MAP_Fractional_Type_t FractionalType);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_MAP_Get_Message_Request_t)(unsigned int BluetoothStackID, unsigned int MAPID, char *MessageHandle, Boolean_t Attachment, MAP_CharSet_t CharSet, MAP_Fractional_Type_t FractionalType);
#endif

   /*! @brief The following function sends a MAP Get Message Response to the
    * specified remote MAP Client.  This is used for responding to a MAP
    * Get Message Indication. @param BluetoothStackID The first parameter is the ID
    * of the Bluetooth Stack that is associated with this MAP Server.
    * @param MAPID The second parameter specifies the MAP ID of the MAP Server
    * responding to the request. @param ResponseCode The third parameter is the OBEX
    * response code to include in the response. @param FractionalType
    * The fourth parameter indicates the type of the object segment and can specify
    * ftUnfragmented, ftMore or ftLast. @param DataLength The fifth parameter
    * defines the number of bytes that are included in the object
    * segment (DataBuffer). @param DataBuffer The sixth parameter is a pointer to a
    * byte buffer containing the Message Listing Object segment.
    * @param AmountSent The final parameter is a pointer to variable which will be
    * written with the actual amount of data that was able to be
    * included in the packet.  @return This function returns zero if successful
    * or a negative return error code if there was an error.
    * \note The MessageObject is a "x-bt/message" character stream
    *          that is formatted as defined in the Message Access
    *          Profile Specification.
    * \note Including a DataBuffer pointer and setting DataLength > 0
    *          will cause a Body or End-of-Body header to be added to
    *          the packet, either on the first or subsequent packets.
    *          If the stack cannot include all the requested object
    *          (DataLength) in the current packet, a Body header will
    *          be used and AmountSent will reflect that not all of the
    *          data was sent.  If all data is included, an End-of-Body
    *          header will be used.
    * \note If AmountSent returns an amount smaller than the
    *          specified DataLength, not all the data was able to be
    *          sent.  This function should be called again with an
    *          adjusted DataLength and DataBuffer pointer to account
    *          for the data that was successfully sent.
    * \note Specifying the FractionalType as ftUnfragmented causes
    *          no FractionalType Header to be added to the OBEX Header
    *          List.  This is the value that should be specified for a
    *          a message that is non-fragmented.  Note that if the
    *          Get Message Indication specified a non-fragmented
    *          FractionalType then you must respond with the correct
    *          non-fragmented FractionalType (i.e. ftMore or ftLast).
    */
BTPSAPI_DECLARATION int BTPSAPI MAP_Get_Message_Response(unsigned int BluetoothStackID, unsigned int MAPID, Byte_t ResponseCode, MAP_Fractional_Type_t FractionalType, unsigned int DataLength, Byte_t *DataBuffer, unsigned int *AmountSent);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_MAP_Get_Message_Response_t)(unsigned int BluetoothStackID, unsigned int MAPID, Byte_t ResponseCode, MAP_Fractional_Type_t FractionalType, unsigned int DataLength, Byte_t *DataBuffer, unsigned int *AmountSent);
#endif

   /*! @brief The following function generates a MAP Set Message Status Request
    * to the specified remote MAP Server. @param BluetoothStackID
    * The first parameter is the ID of the Bluetooth Stack that is associated with
    * this MAP Client. @param MAPID The second parameter specifies the MAP ID for the
    * local MAP Client. @param MessageHandle The third parameter is a pointer to a
    * 16 byte Null terminated Unicode Text string that identifies the
    * message. @param StatusIndicator The fourth parameter identifies the Status indicator to
    * set. @param StatusValue The final parameter indicates the new state of the indicator.
    * @return This function returns zero if successful or a negative return
    * error code if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI MAP_Set_Message_Status_Request(unsigned int BluetoothStackID, unsigned int MAPID, char *MessageHandle, MAP_Status_Indicator_t StatusIndicator, Boolean_t StatusValue);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_MAP_Set_Message_Status_Request_t)(unsigned int BluetoothStackID, unsigned int MAPID, char *MessageHandle, MAP_Status_Indicator_t StatusIndicator, Boolean_t StatusValue);
#endif

   /*! @brief The following function sends a MAP Set Message Status Response to
    * the specified remote MAP Client.  This is used for responding to a
    * MAP Set Message Status Indication. @param BluetoothStackID The first parameter
    * the ID of the Bluetooth Stack that is associated with this MAP
    * Client. @param MAPID The second parameter specifies the MAP ID for the local
    * MAP Client.  @param ResponseCode The third parameter is the OBEX response code
    * to include in the response. @return This function returns zero if
    * successful or a negative return error code if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI MAP_Set_Message_Status_Response(unsigned int BluetoothStackID, unsigned int MAPID, Byte_t ResponseCode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_MAP_Set_Message_Staus_Response_t)(unsigned int BluetoothStackID, unsigned int MAPID, Byte_t ResponseCode);
#endif

   /*! @brief The following function generates a MAP Push Message Request to the
    * specified remote MAP Server.  @param BluetoothStackID The firstparameter the
    * ID of the Bluetooth Stack that is associated with this MAP Client.
    * @param MAPID The second parameter specifies the MAP ID for the local MAP Client.
    * @param FolderName The third parameter specifies the destination location for
    * the message. If this parameter is NULL, the current directory is
    * used. @param Transparent The fourth parameter is used to indicate that no copy
    * of the message should be placed in the Sent Folder. @param Retry
    * The fifth parameter is used to indicate if any attempts to retry the send if
    * the previous attempt fails. @param CharSet The sixth parameter specifies the
    * format of the message.. @param DataLength The seventh parameter defines the
    * number of bytes that are included in the object segment
    * (DataBuffer). @param DataBuffer The eighth parameter is a pointer to a byte
    * buffer containing the Message Listing Object segment.
    * @param AmountSent The ninth parameter is a pointer to variable which will be
    * written with the actual amount of data that was able to be
    * included in the packet.  @param Final The final parameter to this function is a
    * Boolean Flag indicating if this is the last segment of the object.
    * @return This function returns zero if successful or a negative return
    * error code if there was an error.
    * \note The MessageObject is a "x-bt/message" character stream
    *          that is formatted as defined in the Message Access
    *          Profile Specification.
    * \note Including a DataBuffer pointer and setting DataLength > 0
    *          will cause a Body or End-of-Body header to be added to
    *          the packet, either on the first or subsequent packets.
    *          If the stack cannot include all the requested object
    *          (DataLength) in the current packet, a Body header will
    *          be used and AmountSent will reflect that not all of the
    *          data was sent.  If all data is included, an End-of-Body
    *          header will be used.
    * \note If AmountSent returns an amount smaller than the
    *          specified DataLength, not all the data was able to be
    *          sent.  This function should be called again with an
    *          adjusted DataLength and DataBuffer pointer to account
    *          for the data that was successfully sent.
    */
BTPSAPI_DECLARATION int BTPSAPI MAP_Push_Message_Request(unsigned int BluetoothStackID, unsigned int MAPID, Word_t *FolderName, Boolean_t Transparent, Boolean_t Retry, MAP_CharSet_t CharSet, unsigned int DataLength, Byte_t *DataBuffer, unsigned int *AmountSent, Boolean_t Final);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_MAP_Push_Message_Request_t)(unsigned int BluetoothStackID, unsigned int MAPID, Word_t *FolderName, Boolean_t Transparent, Boolean_t Retry, MAP_CharSet_t CharSet, unsigned int DataLength, Byte_t *DataBuffer, unsigned int *AmountSent, Boolean_t Final);
#endif

   /*! @brief The following function sends a MAP Push Message Response to the
    * specified remote MAP Client.  This is used for responding to a MAP
    * Push Message Indication.  @param BluetoothStackID The first parameter is the ID
    * of the Bluetooth Stack that is associated with this MAP Server.
    * @param MAPID The second parameter specifies the MAP ID of the MAP Server
    * responding to the request. @param ResponseCode The third parameter is the OBEX
    * response code to include in the response. @param MessageHandle
    * The final parameter points to an ASCII string that contains 16 hexadecimal
    * digits and represents the handle assigned to the message pushed.
    * \note The MessageHandle pointer must point to a valid string if
    *          the Response Code indicates MAP_OBEX_RESPONSE_OK,
    *          otherwise it may be NULL.
    */
BTPSAPI_DECLARATION int BTPSAPI MAP_Push_Message_Response(unsigned int BluetoothStackID, unsigned int MAPID, Byte_t ResponseCode, char *MessageHandle);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_MAP_Push_Message_Response_t)(unsigned int BluetoothStackID, unsigned int MAPID, Byte_t ResponseCode, char *MessageHandle);
#endif

   /*! @brief The following function generates a MAP Update Inbox Request to the
    * specified remote MAP Server. @param BluetoothStackID The first parameter is the
    * ID of the Bluetooth Stack that is associated with this MAP Client.
    * @param MAPID The secondparameter specifies the MAP ID for the local MAP Client.
    * @return This function returns zero if successful or a negative return
    * error code if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI MAP_Update_Inbox_Request(unsigned int BluetoothStackID, unsigned int MAPID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_MAP_Update_Inbox_Request_t)(unsigned int BluetoothStackID, unsigned int MAPID);
#endif

   /*! @brief The following function sends a MAP Update Inbox Response to the
    * specified remote MAP Client.  This is used for responding to a MAP
    * Set Message Status Indication. @param BluetoothStackID The first parameter is the
    * ID of the Bluetooth Stack that is associated with this MAP Client.
    * @param MAPID The second parameter specifies the MAP ID for the local MAP Client.
    * @param ResponseCode The third parameter is the OBEX response code to include in
    * the response.  @return This function returns zero if successful or a
    * negative return error code if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI MAP_Update_Inbox_Response(unsigned int BluetoothStackID, unsigned int MAPID, Byte_t ResponseCode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_MAP_Update_Inbox_Response_t)(unsigned int BluetoothStackID, unsigned int MAPID, Byte_t ResponseCode);
#endif

   /*! @brief The following function generates a MAP Set Folder Request to the
    * specified remote MAP Server. @param BluetoothStackID The first parameter is the ID
    * of the Bluetooth Stack that is associated with this MAP Client.
    * @param MAPID The second parameter specifies the MAP ID for the local MAP Client.
    * @param PathOption The third parameter contains an enumerated value that
    * indicates the type of path change to request. @param FolderName
    * The final parameter contains the folder name to include with this SetFolder
    * request. This value can be NULL if no name is required for the
    * selected PathOption.  See the MAP specification for more
    * information. @return This function returns zero if successful or a
    * negative return error code if there was an error.
    * \note A successful return code does not mean that the remote
    *          MAP Profile Server successfully processed the command.
    *          The caller needs to check the confirmation result to
    *          determine if the remote MAP Profile Server successfully
    *          executed the Request.
    * \note There can only be one outstanding MAP Profile Request
    *          active at any one time.  Because of this, another MAP
    *          Profile Request cannot be issued until either the current
    *          request is Aborted (by calling the MAP_Abort_Request()
    *          function) or the current Request is completed (this is
    *          signified by receiving a Confirmation Event in the MAP
    *          Profile Event Callback that was registered when the MAP
    *          Profile Port was opened).
    */
BTPSAPI_DECLARATION int BTPSAPI MAP_Set_Folder_Request(unsigned int BluetoothStackID, unsigned int MAPID, MAP_Set_Folder_Option_t PathOption, Word_t *FolderName);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_MAP_Set_Folder_Request_t)(unsigned int BluetoothStackID, unsigned int MAPID, MAP_Set_Folder_Option_t PathOption, Word_t *FolderName);
#endif

   /*! @brief The following function sends a MAP Set Folder Response to the
    * specified remote MAP Client.  This is used for responding to a MAP
    * Set Folder Indication. @param BluetoothStackID parameter the ID of
    * the Bluetooth Stack that is associated with this MAP Client.
    * @param MAPID The second parameter specifies the MAP ID for the local MAP Client.
    * @param ResponseCode The final parameter is the OBEX response code to include in
    * the response. @return This function returns zero if successful or a
    * negative return error code if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI MAP_Set_Folder_Response(unsigned int BluetoothStackID, unsigned int MAPID, Byte_t ResponseCode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_MAP_Set_Folder_Response_t)(unsigned int BluetoothStackID, unsigned int MAPID, Byte_t ResponseCode);
#endif

#endif

