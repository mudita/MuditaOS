/*
 * Copyright 2005 - 2014 Stonestreet One.
 * All Rights Reserved.
 * Author:  Nikhil Deo
 *** MODIFICATION HISTORY ****************************************************
 *
 *   mm/dd/yy  F. Lastname    Description of Modification
 *   --------  -----------    ------------------------------------------------
 *   12/06/05  N. Deo         Initial creation.
 ****************************************************************************
 */

/**
 *  @file AVCTPAPI.h
 *
 *  @brief  Stonestreet One Bluetooth Stack Audio/Video Control Transport
 *      Protocol (AVCTP) API Type Definitions, Constants, and
 *      Prototypes.
 *
 *  @code
 *  #include "SS1BTAVC.h"
 *  @endcode
 *  ============================================================================
 */
#ifndef __AVCTPAPIH__
#define __AVCTPAPIH__

#include "SS1BTPS.h"       /*! Bluetooth Stack API Prototypes/Constants. */

#include "AVCTypes.h"      /*! Audio/Video Control Transport Type Definitions. */

    /*! Error Return Codes.
    * Error Codes that are smaller than these (less than -1000) are
    * related to the Bluetooth Protocol Stack itself (see BTERRORS.H).
    */
#define BTAVCTP_ERROR_INVALID_PARAMETER                                  (-1000)
#define BTAVCTP_ERROR_NOT_INITIALIZED                                    (-1001)
#define BTAVCTP_ERROR_INVALID_BLUETOOTH_STACK_ID                         (-1002)
#define BTAVCTP_ERROR_LIBRARY_INITIALIZATION_ERROR                       (-1003)
#define BTAVCTP_ERROR_INSUFFICIENT_RESOURCES                             (-1004)
#define BTAVCTP_ERROR_CONTEXT_ALREADY_EXISTS                             (-1005)
#define BTAVCTP_ERROR_PROFILE_ALREADY_REGISTERED                         (-1006)
#define BTAVCTP_ERROR_PROFILE_NOT_FOUND                                  (-1007)
#define BTAVCTP_ERROR_PROFILE_BUSY                                       (-1008)
#define BTAVCTP_ERROR_ALREADY_CONNECTED                                  (-1009)
#define BTAVCTP_ERROR_ALREADY_CONNECTING                                 (-1010)
#define BTAVCTP_ERROR_AWAITING_DISCONNECTION                             (-1011)
#define BTAVCTP_ERROR_INVALID_CONNECTION                                 (-1012)
#define BTAVCTP_ERROR_OPEN_STATUS_UNKNOWN_ERROR                          (-1013)
#define BTAVCTP_ERROR_MESSAGE_TOO_LONG                                   (-1014)
#define BTAVCTP_ERROR_AVCTP_CONNECTED                                    (-1015)
#define BTAVCTP_ERROR_L2CAP_CONNECT_FAILED                               (-1016)
#define BTAVCTP_ERROR_CONNECTION_NOT_INITIATED                           (-1017)
#define BTAVCTP_ERROR_PROFILES_REGISTERED                                (-1018)
#define BTAVCTP_ERROR_BROWSING_CHANNEL_MTU_EXCEEDED                      (-1019)
#define BTAVCTP_ERROR_REMOTE_BROWSING_CHANNEL_NOT_SUPPORTED              (-1020)

   /*! SDP Profile UUID's for the Audio/Video Control Transport Protocol.
    * SDP Audio/Video Control Transport Protocol UUID's.
    * The following MACRO is a utility MACRO that assigns the Audio/
    * Video Control Transport Protocol Bluetooth Universally Unique
    * Identifier (AUDIO_VIDEO_CONTROL_TRANSPORT_PROTOCOL_UUID_16) to the
    * specified UUID_16_t variable.  This MACRO accepts one parameter
    * which is the UUID_16_t variable that is to receive the
    * AUDIO_VIDEO_CONTROL_TRANSPORT_PROTOCOL_UUID_16 Constant value.
    */
#define SDP_ASSIGN_AUDIO_VIDEO_CONTROL_TRANSPORT_PROTOCOL_UUID_16(_x)   ASSIGN_SDP_UUID_16((_x), 0x00, 0x17)

   /*! The following MACRO is a utility MACRO that assigns the Audio/
    * Video Control Transport Protocol Bluetooth Universally Unique
    * Identifier (AUDIO_VIDEO_CONTROL_TRANSPORT_PROTOCOL_UUID_32) to the
    * specified UUID_32_t variable.  This MACRO accepts one parameter
    * which is the UUID_32_t variable that is to receive the
    * AUDIO_VIDEO_CONTROL_TRANSPORT_PROTOCOL_UUID_16 Constant value.
    */
#define SDP_ASSIGN_AUDIO_VIDEO_CONTROL_TRANSPORT_PROTOCOL_UUID_32(_x)   ASSIGN_SDP_UUID_32((_x), 0x00, 0x00, 0x00, 0x17)

   /*! The following MACRO is a utility MACRO that assigns the Audio/
    * Video Control Transport Protocol Bluetooth Universally Unique
    * Identifier (AUDIO_VIDEO_CONTROL_TRANSPORT_PROTOCOL_UUID_128) to
    * the specified UUID_128_t variable.  This MACRO accepts one
    * parameter which is the UUID_128_t variable that is to receive the
    * AUDIO_VIDEO_CONTROL_TRANSPORT_PROTOCOL_UUID_128 Constant value.
    */
#define SDP_ASSIGN_AUDIO_VIDEO_CONTROL_TRANSPORT_PROTOCOL_UUID_128(_x)  ASSIGN_SDP_UUID_128((_x), 0x00, 0x00, 0x00, 0x17, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /*! The following constant defines the Protocol Version Number used
    * within the SDP Record for AVCTP Profile Server/Clients (supported
    * by this implementation).
    */
#define AVCTP_PROTOCOL_VERSION                                           (0x0103)

   /*! The following constants represent the Open Status Values that are
    * possible in the Open Confirmation Event.
    */
#define AVCTP_OPEN_STATUS_SUCCESS                                        (0x00)
#define AVCTP_OPEN_STATUS_CONNECTION_TIMEOUT                             (0x01)
#define AVCTP_OPEN_STATUS_CONNECTION_REFUSED                             (0x02)
#define AVCTP_OPEN_STATUS_UNKNOWN_ERROR                                  (0x03)
#define AVCTP_OPEN_STATUS_UNSUPPORTED                                    (0x04)

   /*! The following enumerated type represents the supported Server
    * Connection Modes supported by the AVCTP Server.
    */
typedef enum
{
   asmAutomaticAccept,
   asmAutomaticReject,
   asmManualAccept
} AVCTP_Server_Connection_Mode_t;

   /*! The following structure is used with the
    * AVCTP_Register_SDP_Record() function.  This structure (when
    * specified) contains additional SDP Service Information that will
    * be added to the SDP AVCTP Service Record Entry.  The first member
    * of this structure specifies the Number of Service Class UUID's
    * that are present in the SDPUUIDEntries Array.  This member must be
    * at least one, and the SDPUUIDEntries member must point to an array
    * of SDP UUID Entries that contains (at least) as many entries
    * specified by the NumberServiceClassUUID member.  The ProtocolList
    * member is an SDP Data Element Sequence that contains a list of
    * Protocol Information that will be added to the generic SDP Service
    * Record.  The ProtocolList SDP Data Element must be a Data Element
    * Sequence containing the Protocol List information to add (in
    * addition to the AVCTP Protocol Information).  This element is
    * optional and can be NULL (signifying no additional Protocol
    * Information).  The ProfileList SDP Data Element must be a Data
    * Element Sequence containing the Profile List information to add.
    * This element is optional and can be NULL (signifying that no
    * Profile Information is to be added).
    */
typedef struct _tagAVCTP_SDP_Service_Record_t
{
   unsigned int        NumberServiceClassUUID;
   SDP_UUID_Entry_t   *SDPUUIDEntries;
   SDP_Data_Element_t *ProtocolList;
   SDP_Data_Element_t *ProfileList;
} AVCTP_SDP_Service_Record_t;

#define AVCTP_SDP_SERVICE_RECORD_SIZE                                    (sizeof(AVCTP_SDP_Service_Record_t))

   /*! Audio/Video Control Transport Protocol Event API Types.
    */
typedef enum
{
   etAVCTP_Connect_Indication, /*!< A remote device has connected to local AVCTP instance. */
   etAVCTP_Connect_Confirmation, /*!< Confirms that the connection attempt to a remote profile that was initiated by a local profile has ended and informs if it was successful or not. */
   etAVCTP_Disconnect_Indication, /*!< A remote device that was connected to a local AVCTP profile has disconnected. */
   etAVCTP_Message_Indication, /*!< A local registered and connected profile has received a message/response from a remote device. */
   etAVCTP_Connect_Request_Indication, /*!< A remote service is requesting a connection to the local service. */
   etAVCTP_Browsing_Channel_Connect_Indication, /*!< A remote Browsing service has connected to the local Browsing service. */
   etAVCTP_Browsing_Channel_Connect_Confirmation, /*!< A previously outstanding attempt to connect to a remote AVCTP Browsing Channel is complete. */
   etAVCTP_Browsing_Channel_Disconnect_Indication, /*!< A remote device has disconnected from the Browsing Channel of the local service. */
   etAVCTP_Browsing_Channel_Message_Indication /*!< The local Browsing Service received data from a remote Browsing Service that is connected to the local device. */
} AVCTP_Event_Type_t;

   /*! The following event is dispatched when a remote service is
    * requesting a connection to the local service.  The BD_ADDR
    * specifies the Bluetooth Address of the Remote Device that is
    * connecting.
    * \note This event is only dispatched to servers that are in
    *            Manual Accept Mode.
    * \note This event must be responded to with the
    *            AVCTP_Connect_Request_Response() function in order to
    *            accept or reject the outstanding Connect Request.
    */
typedef struct _tagAVCTP_Connect_Request_Indication_Data_t
{
   BD_ADDR_t BD_ADDR;
} AVCTP_Connect_Request_Indication_Data_t;

#define AVCTP_CONNECT_REQUEST_INDICATION_DATA_SIZE                       (sizeof(AVCTP_Connect_Request_Indication_Data_t))

   /*! The following event is dispatched when a remote service gets
    * connected to the local service.  The BD_ADDR specifies the
    * Bluetooth Address of the Remote Device that is connected.
    */
typedef struct _tagAVCTP_Connect_Indication_Data_t
{
   unsigned int AVCTPProfileID;
   BD_ADDR_t    BD_ADDR;
} AVCTP_Connect_Indication_Data_t;

#define AVCTP_CONNECT_INDICATION_DATA_SIZE                               (sizeof(AVCTP_Connect_Indication_Data_t))

   /*! The following event is dispatched to the application when an
    * attempt to connect to a remote AVCTP is complete.  The BD_ADDR
    * specifies the Bluetooth Address of the Remote Device that the
    * connection was attempted.  The Status parameter is zero if
    * successful or a negative error code in case of failure.
    */
typedef struct _tagAVCTP_Connect_Confirmation_Data_t
{
   unsigned int AVCTPProfileID;
   BD_ADDR_t    BD_ADDR;
   int          Status;
   int          Result;
} AVCTP_Connect_Confirmation_Data_t;

#define AVCTP_CONNECT_CONFIRMATION_DATA_SIZE                             (sizeof(AVCTP_Connect_Confirmation_Data_t))

   /*! The following event is dispatched to the application when the
    * remote device disconnects from the Local service.  The BD_ADDR
    * specifies the Bluetooth Address of the Remote Device that we got
    * disconnected from. The Reason is the zero if the connection
    * disconnected cleanly or the HCI disconnect reason otherwise.
    */
typedef struct _tagAVCTP_Disconnect_Indication_Data_t
{
   unsigned int AVCTPProfileID;
   BD_ADDR_t    BD_ADDR;
   Byte_t       Reason;
} AVCTP_Disconnect_Indication_Data_t;

#define AVCTP_DISCONNECT_INDICATION_DATA_SIZE                            (sizeof(AVCTP_Disconnect_Indication_Data_t))

   /*! The following event is dispatched to the application when the
    * local service receives data from a remote service that is
    * connected.  The BD_ADDR specifies the Bluetooth Address of the
    * Remote Device that sent this message.  Message Type is the type of
    * message and is described above.  Transaction ID is the identifier
    * provided by the remote service.  InvalidProfileID is set if the
    * local application tried to send a message to a profile that was
    * not registered with remote AVCTP.  This would result in local
    * AVCTP receiving a message with the IPID bit set.  This will be
    * conveyed to the local application through InvalidProfileID.  If
    * InvalidProfileID is set to TRUE, Datalength would be zero and
    * Databuffer would point to NULL.  DataLength specifies the Length
    * of data in buffer pointed to by DataBuffer.
    */
typedef struct _tagAVCTP_Message_Indication_Data_t
{
   unsigned int  AVCTPProfileID;
   BD_ADDR_t     BD_ADDR;
   Byte_t        MessageType;
   Byte_t        TransactionID;
   Boolean_t     InvalidProfileID;
   unsigned int  DataLength;
   Byte_t       *DataBuffer;
} AVCTP_Message_Indication_Data_t;

#define AVCTP_MESSAGE_INDICATION_DATA_SIZE                               (sizeof(AVCTP_Message_Indication_Data_t))

   /*! The following event is dispatched when a remote Browsing service
    * gets connected to the local Browsing service.  The BD_ADDR
    * specifies the Bluetooth Address of the Remote Device that is
    * connected.  The MTU parameter specifies the maximum allowable
    * packet data payload that can be sent (Maximum Transmission Unit).
    * The MTU is required because the Browsing Channel does not support
    * AVCTP Message Fragmentation (i.e. each individual message *MUST*
    * fit within this size).
    */
typedef struct _tagAVCTP_Browsing_Channel_Connect_Indication_Data_t
{
   unsigned int AVCTPProfileID;
   BD_ADDR_t    BD_ADDR;
   Word_t       MTU;
} AVCTP_Browsing_Channel_Connect_Indication_Data_t;

#define AVCTP_BROWSING_CHANNEL_CONNECT_INDICATION_DATA_SIZE             (sizeof(AVCTP_Browsing_Channel_Connect_Indication_Data_t))

   /*! The following event is dispatched to the application when an
    * attempt to connect to a remote AVCTP Browsing Channel is complete.
    * The BD_ADDR specifies the Bluetooth Address of the Remote Device
    * that the connection was attempted.  The Status parameter is zero
    * if successful or a negative error code in case of failure.  The
    * MTU parameter specifies the maximum allowable packet data payload
    * that can be sent (Maximum Transmission Unit).  The MTU is required
    * because the Browsing Channel does not support AVCTP Message
    * Fragmentation (i.e. each individual message *MUST* fit within
    * this size).
    */
typedef struct _tagAVCTP_Browsing_Channel_Connect_Confirmation_Data_t
{
   unsigned int AVCTPProfileID;
   BD_ADDR_t    BD_ADDR;
   int          Status;
   int          Result;
   Word_t       MTU;
} AVCTP_Browsing_Channel_Connect_Confirmation_Data_t;

#define AVCTP_BROWSING_CHANNEL_CONNECT_CONFIRMATION_DATA_SIZE           (sizeof(AVCTP_Browsing_Channel_Connect_Confirmation_Data_t))

   /*! The following event is dispatched to the application when the
    * remote device disconnects the Browsing Channel from the Local
    * service.  The BD_ADDR specifies the Bluetooth Address of the
    * Remote Device that has disconnected the Browsing Channel.
    */
typedef struct _tagAVCTP_Browsing_Channel_Disconnect_Indication_Data_t
{
   unsigned int AVCTPProfileID;
   BD_ADDR_t    BD_ADDR;
} AVCTP_Browsing_Channel_Disconnect_Indication_Data_t;

#define AVCTP_BROWSING_CHANNEL_DISCONNECT_INDICATION_DATA_SIZE          (sizeof(AVCTP_Browsing_Channel_Disconnect_Indication_Data_t))

   /*! The following event is dispatched to the application when the
    * local Browsing service receives data from a remote Browsing
    * service that is connected.  The BD_ADDR member specifies the
    * Bluetooth Address of the Remote Device that sent the message.  The
    * Message Type member specifies the type of message and is described
    * above.  The Transaction ID member specifies the identifier
    * provided by the remote service.  The InvalidProfileID member is
    * set if the local application tried to send a message to a profile
    * that was not registered with remote AVCTP.  The DataLength member
    * specifies the Length of data in buffer pointed to by DataBuffer
    * member.  If InvalidProfileID is set to TRUE, Datalength will be
    * zero and Databuffer will be NULL.
    */
typedef struct _tagAVCTP_Browsing_Channel_Message_Indication_Data_t
{
   unsigned int  AVCTPProfileID;
   BD_ADDR_t     BD_ADDR;
   Byte_t        MessageType;
   Byte_t        TransactionID;
   Boolean_t     InvalidProfileID;
   unsigned int  DataLength;
   Byte_t       *DataBuffer;
} AVCTP_Browsing_Channel_Message_Indication_Data_t;

#define AVCTP_BROWSING_CHANNEL_MESSAGE_INDICATION_DATA_SIZE             (sizeof(AVCTP_Browsing_Channel_Message_Indication_Data_t))

   /*! The following structure represents the container structure for
    * Holding all AVCTP Event Data Data.
    */
typedef struct _tagAVCTP_Event_Data_t
{
   AVCTP_Event_Type_t Event_Data_Type;
   Word_t             Event_Data_Size;
   union
   {
      AVCTP_Connect_Request_Indication_Data_t             *AVCTP_Connect_Request_Indication_Data;
      AVCTP_Connect_Indication_Data_t                     *AVCTP_Connect_Indication_Data;
      AVCTP_Connect_Confirmation_Data_t                   *AVCTP_Connect_Confirmation_Data;
      AVCTP_Disconnect_Indication_Data_t                  *AVCTP_Disconnect_Indication_Data;
      AVCTP_Message_Indication_Data_t                     *AVCTP_Message_Indication_Data;
      AVCTP_Browsing_Channel_Connect_Indication_Data_t    *AVCTP_Browsing_Channel_Connect_Indication_Data;
      AVCTP_Browsing_Channel_Connect_Confirmation_Data_t  *AVCTP_Browsing_Channel_Connect_Confirmation_Data;
      AVCTP_Browsing_Channel_Disconnect_Indication_Data_t *AVCTP_Browsing_Channel_Disconnect_Indication_Data;
      AVCTP_Browsing_Channel_Message_Indication_Data_t    *AVCTP_Browsing_Channel_Message_Indication_Data;
   } Event_Data;
} AVCTP_Event_Data_t;

#define AVCTP_EVENT_DATA_SIZE                                            (sizeof(AVCTP_Event_Data_t))

   /*! The following declared type represents the Prototype Function for
    * a Audio/Video Control Transport Protocol Event Receive Data
    * Callback.  This function will be called whenever an AVCTP Event
    * occurs that is associated with the specified Bluetooth Stack ID.
    * This function passes to the caller the Bluetooth Stack ID, the
    * AVCTP Event Data that occurred and the AVCTP Event Callback
    * Parameter that was specified when this Callback was installed.
    * The caller is free to use the contents of the AVCTP Event Data
    * ONLY in the context of this callback.  If the caller requires the
    * Data for a longer period of time, then the callback function MUST
    * copy the data into another Data Buffer.  This function is
    * guaranteed NOT to be invoked more than once simultaneously for the
    * specified installed callback (i.e. this function DOES NOT have be
    * reentrant).  It needs to be noted however, that if the same
    * Callback is installed more than once, then the callbacks will be
    * called serially.  Because of this, the processing in this function
    * should be as efficient as possible.  It should also be noted that
    * this function is called in the Thread Context of a Thread that the
    * User does NOT own.  Therefore, processing in this function should
    * be as efficient as possible (this argument holds anyway because
    * another Audio/Video Control Transport Protocol Event will not be
    * processed while this function call is outstanding).
    * \note This function MUST NOT Block and wait for events that
    *            can only be satisfied by Receiving AVCTP Events.  A
    *            Deadlock WILL occur because NO AVCTP Event Callbacks
    *            will be issued while this function is currently
    *            outstanding.
    */
typedef void (BTPSAPI *AVCTP_Event_Callback_t)(unsigned int BluetoothStackID, AVCTP_Event_Data_t *AVCTP_Event_Data, unsigned long CallbackParameter);

   /*! @brief The following function is responsible for initializing the AVCTP.
    * This function MUST be called before any other AVCTP function. @param BluetoothStackID
    * The first parameter to this function is the Bluetooth Stack
    * ID of the Bluetooth Stack to be used by this AVCTP instance.
    * @return This function returns zero if successful, or a negative return value if
    * there was an error.
    * \note This function must be called once for a given Bluetooth
    *            Stack ID.
    */
BTPSAPI_DECLARATION int BTPSAPI AVCTP_Initialize(unsigned int BluetoothStackID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVCTP_Initialize_t)(unsigned int BluetoothStackID);
#endif

   /*! @brief The following function is responsible for cleaning up a previously
    * initialized AVCTP instance. After calling this function,
    * AVCTP_Initialize() must be called to initialize AVCTP using
    * specified Bluetooth Stack again before any other AVCTP Profile
    * function can be called. @param BluetoothStackID This function accepts the Bluetooth Stack
    * ID of the Bluetooth Stack used by AVCTP Instance being cleaned up.
    * @return This function returns zero if successful, or a negative return
    * value if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI AVCTP_Cleanup(unsigned int BluetoothStackID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVCTP_Cleanup_t)(unsigned int BluetoothStackID);
#endif

   /*! @brief The following function is responsible for instructing the AVCTP
    * module that it is to support the Browsing Channel.  This function
    * can only be called *AFTER* a successful call to the
    * AVCTP_Initialize() function and *BEFORE* an profiles are
    * registered. @param BluetoothStackID This function accepts as input, the Bluetooth Stack
    * ID of the Bluetooth Stack that supports an AVCTP intstance
    * (initialized via a call to the AVCTP_Initialize() function.
    * @return This function returns zero if successful or a negative return error
    * code if there was an error.
    * \note Once Browsing Channel support is enabled, it cannot be
    *          disabled.
    */
BTPSAPI_DECLARATION int BTPSAPI AVCTP_Enable_Browsing_Channel_Support(unsigned int BluetoothStackID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVCTP_Enable_Browsing_Channel_Support_t)(unsigned int BluetoothStackID);
#endif

   /*! @brief The following function is responsible for responding to an
    * individual request to connect to a local AVCTP Server.
    * @param BluetoothStackID The first parameter to this function is the Bluetooth Stack ID of the
    * Bluetooth Stack associated with the AVCTP Server that is
    * responding to the request.  @param BD_ADDR The second parameter to this function
    * is the Bluetooth Device Address of the AVCTP Connection for which
    * a connection request was received.  @param AcceptConnection The final parameter to this
    * function specifies whether to accept the pending connection
    * request (or to reject the request).  @return This function returns zero if
    * successful, or a negative return error code if an error occurred.
    * \note The connection to the server is not established until
    *            a etAVCTP_Connect_Indication event has occurred.
    * \note This event will only be dispatched if the server mode
    *            was explicitly set to asmManualAccept via the
    *            AVCTP_Set_Profile_Server_Connection_Mode() function.
    *            If this mode is set, ONLY the callback that was
    *            specified with the
    *            AVCTP_Set_Profile_Server_Connection_Mode() function
    *            will receive this event.
    */
BTPSAPI_DECLARATION int BTPSAPI AVCTP_Connect_Request_Response(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Boolean_t AcceptConnection);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVCTP_Connect_Request_Response_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Boolean_t AcceptConnection);
#endif

   /*! @brief This function will register a Local Profile so that remote
    * Profiles/Applications can connect to us. @param BluetoothStackID The BluetoothStackID
    * parameter identifies the Bluetooth Stack that is used by the
    * AVCTP. @param ProfileUUID The second parameter is the Profile Identifier of the profile that
    * wishes to register. AVCTPServiceType is described above and
    * identifies the role as a target or a controller. @param EventCallback The third parameter is
    * the callback function to be invoked by AVCTP with the
    * CallbackParameter whenver there are any events of interest for
    * this profile. @param CallbackParameter The final paramter is the a user-defined parameter (e.g. a tag value)
    * that will be passed back to the user in the callback function with each event callback.
    * @return This function returns a positive AVCTPProfileID if
    * successful, or a negative return value if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI AVCTP_Register_Profile(unsigned int BluetoothStackID, UUID_16_t ProfileUUID, AVCTP_Event_Callback_t EventCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVCTP_Register_Profile_t)(unsigned int BluetoothStackID, UUID_16_t ProfileUUID, AVCTP_Event_Callback_t EventCallback, unsigned long CallbackParameter);
#endif

   /*! @brief This function will un-register a Local Profile so that remote
    * Profiles/Applications can connect to us. @param BluetoothStackID
    * The first parameter identifies the Bluetooth Stack that is used by the
    * AVCTP. @param AVCTPProfileID The second paramter is the Profile Identifier
    * of the profile that wishes to unregister. @return This function returns zero if
    * successful, or a negative return value if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI AVCTP_UnRegister_Profile(unsigned int BluetoothStackID, unsigned int AVCTPProfileID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVCTP_UnRegister_Profile_t)(unsigned int BluetoothStackID, unsigned int AVCTPProfileID);
#endif

   /*! @brief The following function is provided to allow a means to add a
    * Generic SDP Service Record to the SDP Database. @param BluetoothStackID This function
    * takes as input the Bluetooth Stack ID of the Local Bluetooth
    * Protocol Stack. @param SDPServiceRecord The second parameter (if specified) specifies any
    * additional SDP Information to add to the record.
    * @param ServiceName The third parameter specifies the Service Name to associate with the SDP
    * Record and @param ProviderName The fourth parameter specifies the provider name to
    * associate with the SDP record.  @param SDPServiceRecordHandle The final parameter is a pointer
    * to a DWord_t which receives the SDP Service Record Handle if this
    * function successfully creates an SDP Service Record.
    * @return If this function returns zero, then the SDPServiceRecordHandle entry will
    * contain the Service Record Handle of the added SDP Service Record.
    * If this function fails, a negative return error code will be
    * returned (see BTERRORS.H) and the SDPServiceRecordHandle value
    * will be undefined.
    * \note The Service Record Handle that is returned from this
    *            function will remain in the SDP Record Database until
    *            it is deleted by calling the
    *            SDP_Delete_Service_Record() function.
    * \note A MACRO is provided to Delete the Service Record from
    *            the SDP Data Base.  This MACRO maps the
    *            AVCTP_Unregister_SDP_Record() to
    *            SDP_Delete_Service_Record().
    * \note Any Protocol Information that is specified will be
    *            added in the Protocol Attribute AFTER the default
    *            Protocol List (L2CAP and AVDTP).
    * \note The Service Name is always added at Attribute ID
    *            0x0100.  A Language Base Attribute ID List is created
    *            that specifies that 0x0100 is UTF-8 Encoded, English
    *            Language.
    */
BTPSAPI_DECLARATION int BTPSAPI AVCTP_Register_Profile_SDP_Record(unsigned int BluetoothStackID, AVCTP_SDP_Service_Record_t *SDPServiceRecord, char *ServiceName, char *ProviderName, DWord_t *SDPServiceRecordHandle);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVCTP_Register_Profile_SDP_Record_t)(unsigned int BluetoothStackID, AVCTP_SDP_Service_Record_t *SDPServiceRecord, char *ServiceName, char *ProviderName, DWord_t *SDPServiceRecordHandle);
#endif

   /*! The following MACRO is a utility MACRO that simply deletes the
    * AVCTP SDP Service Record (specified by the next parameter) from
    * SDP Database.  This MACRO simply maps to the
    * SDP_Delete_Service_Record() function.  This MACRO is only provided
    * so that the caller doesn't have to sift through the SDP API for
    * very simplistic applications.  This function accepts as input the
    * Bluetooth Stack ID of the Bluetooth Protocol Stack that the
    * Service Record exists on and the SDP Service Record Handle.  The
    * SDP Service Record Handle was returned via a successful call to
    * the AVCTP_Register_SDP_Record() function.  See the
    * AVCTP_Register_SDP_Record() function for more information.  This
    * MACRO returns the result of the SDP_Delete_Service_Record()
    * function, which is zero for success or a negative return error
    * code (see BTERRORS.H).
    */
#define AVCTP_UnRegister_Profile_SDP_Record(__BluetoothStackID, __SDPRecordHandle) \
        (SDP_Delete_Service_Record(__BluetoothStackID, __SDPRecordHandle))

   /*! @brief This function is responsible for initiating a connection to a
    * remote device. It will try to establish an L2CAP channel if no
    * channel exists to the remote device. @param BluetoothStackID It takes the
    * BluetoothStackID associated with the AVCTP contenx this profile is
    * registered with. @param AVCTPProfileID The second parameter allows for the
    * AVCTP to maintain information about who initiated this connection.
    * @param RemoteBD_ADDR The final parameter is the Bluetooth Address of the remote device this
    * profile wants to connect to.
    */
BTPSAPI_DECLARATION int BTPSAPI AVCTP_Connect_Device(unsigned int BluetoothStackID, unsigned int AVCTPProfileID, BD_ADDR_t RemoteBD_ADDR);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVCTP_Connect_Device_t)(unsigned int BluetoothStackID, unsigned int AVCTPProfileID, BD_ADDR_t RemoteBD_ADDR);
#endif

   /*! @brief This function is responsible for initiating a Browsing Channel
    * connection to a remote device.  It will try to establish an L2CAP
    * channel if no channel exists to the remote device. @param BluetoothStackID
    * It takes the BluetoothStackID associated with the AVCTP contenx this profile is
    * registered with. @param AVCTPProfileID The second parameter is used such that the
    * AVCTP can maintain information who initiated this connection. @param RemoteBD_ADDR The
    * third parameter is the Bluetooth Address of the remote
    * device this profile wants to connect to. @param MTUSize The final parameter
    * specifies the MTU (Maximum Transmission Unit) to use for the
    * browsing channel.  @return This function returns zero if successful or a
    * negative return error code if unsuccessful.
    * \note A Browsing Channel can *ONLY* be added if there already
    *          exists an on-going AVCTP connection between the local
    *          device and the remote device already.
    * \note The Browsing Channel cannot exist without a
    *          corresponding AVCTP connection.  This means that if the
    *          AVCTP connection is terminated, the Browsing Channel
    *          connection will be terminated as well.
    */
BTPSAPI_DECLARATION int BTPSAPI AVCTP_Connect_Browsing_Channel(unsigned int BluetoothStackID, unsigned int AVCTPProfileID, BD_ADDR_t RemoteBD_ADDR, Word_t MTUSize);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVCTP_Connect_Browsing_Channel_t)(unsigned int BluetoothStackID, unsigned int AVCTPProfileID, BD_ADDR_t RemoteBD_ADDR, Word_t MTUSize);
#endif

   /*! @brief This function is responsible for disconnecting a connection to a
    * remote device.  The L2CAP channel is disconnected only if this
    * profile has initiated this connection. @param BluetoothStackID The first
    * parameter is the Stack ID corresponding to the AVCTP context that
    * we need to use. @param AVCTPProfileID The second parameter is the
    * ID of the profile wishes to disconnect the connection.
    * This is the ID that was returned when this profile was registered.
    * @param RemoteBD_ADDR The final parameter is the Bluetooth Device Address of the
    * remote device to disconnect.
    * @return The function returns zero if successful and a negative return code on failure.
    * \note If there is a browsing channel established between the
    *          the local device and the specified remote device, it will
    *          be disconnected as well (a Browsing Channel requires a
    *          valid AVCTP connection at all times).
    */
BTPSAPI_DECLARATION int BTPSAPI AVCTP_Close_Connection(unsigned int BluetoothStackID, unsigned int AVCTPProfileID, BD_ADDR_t RemoteBD_ADDR);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVCTP_Close_Connection_t)(unsigned int BluetoothStackID, unsigned int AVCTPProfileID, BD_ADDR_t RemoteBD_ADDR);
#endif

   /*! @brief This function is responsible for disconnecting any connected
    * Browsing channel to the specified remote device.  The L2CAP
    * channel is disconnected only if this profile has initiated this
    * connection. @param BluetoothStackID The first parameter is the Stack ID
    * corresponding to the AVCTP context that we need to use. @param AVCTPProfileID
    * The second parameter is the the ID of the profile wishes to disconnect the connection.
    * This is the ID that was returned when this profile was registered. @param RemoteBD_ADDR
    * The third parameter is the Bluetooth Device Address of the remote device to disconnect.
    * @return The function returns zero if successful and a negative return code on failure.
    */
BTPSAPI_DECLARATION int BTPSAPI AVCTP_Close_Browsing_Channel(unsigned int BluetoothStackID, unsigned int AVCTPProfileID, BD_ADDR_t RemoteBD_ADDR);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVCTP_Close_Browsing_Channel_t)(unsigned int BluetoothStackID, unsigned int AVCTPProfileID, BD_ADDR_t RemoteBD_ADDR);
#endif

   /*! @brief This function is used by a profile to send a message to a remote
    * profile. @param BluetoothStackID This function takes Bluetooth Stack Identifier of the
    * AVCTP instance to use. @param AVCTPProfileID The second parameter is the
    * AVCTP Profile Identifier for this profile.
    * @param RemoteBD_ADDR The third paramter is the Bluetooth Address of the intended recepient remote
    * device. @param TransactionID The fourth parameter is a flag that states if this a response message or a
    * command. @param ResponseMessage The fifth paramter is the data to send.
    * @param DataLength The sixth parameter is the length of the actual Message Data.
    * @param DataBuffer The final paramter is a pointer to the Message Data itself
    * (this buffer must point to memory that contains (at least) the number of bytes specified by
    * the DataLength parameter).  @return This function returns zero if data was
    * sent successfully or a negative error code on failure.
    */
BTPSAPI_DECLARATION int BTPSAPI AVCTP_Send_Message(unsigned int BluetoothStackID, unsigned int AVCTPProfileID, BD_ADDR_t RemoteBD_ADDR, Byte_t TransactionID, Boolean_t ResponseMessage, unsigned int DataLength, Byte_t *DataBuffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVCTP_Send_Message_t)(unsigned int BluetoothStackID, unsigned int AVCTPProfileID, BD_ADDR_t RemoteBD_ADDR, Byte_t TransactionID, Boolean_t ResponseMessage, unsigned int DataLength, Byte_t *DataBuffer);
#endif

   /*! @brief This function is used by a profile to send a message to a remote
    * profile over an established Browsing Channel. @param BluetoothStackID This function takes
    * the Bluetooth Stack Identifier of the AVCTP instance to use. @param AVCTPProfileID
    * @param AVCTPProfileID The second paramter is the profile identifier for this profile.
    * @param RemoteBD_ADDR The third parameter is the Bluetooth Address of the
    * intended recepient remote device. @param TransactionID The fourth parameter is
    * a flag that states if this a response message or a command. @param ResponseMessage The fifth parameter
    * is a Flag indicating if this is a response message or not. @param DataLength The sixth paramter
    * is the length of the actual Message Data.
    * @param DataBuffer The final parameter is a pointer to the Message Data itself (this
    * buffer must point to memory that contains (at least) the number of
    * bytes specified by the DataLength parameter).
    * @return This function returns zero if data was sent successfully or a negative error
    * code on failure.
    */
BTPSAPI_DECLARATION int BTPSAPI AVCTP_Send_Browsing_Channel_Message(unsigned int BluetoothStackID, unsigned int AVCTPProfileID, BD_ADDR_t RemoteBD_ADDR, Byte_t TransactionID, Boolean_t ResponseMessage, unsigned int DataLength, Byte_t *DataBuffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVCTP_Send_Browsing_Channel_Message_t)(unsigned int BluetoothStackID, unsigned int AVCTPProfileID, BD_ADDR_t RemoteBD_ADDR, Byte_t TransactionID, Boolean_t ResponseMessage, unsigned int DataLength, Byte_t *DataBuffer);
#endif

   /*! @brief The following function is responsible for retrieving the current
    * AVCTP Server Connection Mode. @param BluetoothStackID This function accepts as its first
    * parameter the Bluetooth Stack ID of the Bluetooth Stack on which
    * the server exists.  @param ServerConnectionMode The final parameter to this function is a
    * pointer to a Server Connection Mode variable which will receive
    * the current Server Connection Mode. @return This function returns zero if
    * successful, or a negative return error code if an error occurred.
    * \note The Default Server Connection Mode is
    *            asmAutomaticAccept.
    * \note This function is used for AVCTP Servers which use
    *            Bluetooth Security Mode 2.
    */
BTPSAPI_DECLARATION int BTPSAPI AVCTP_Get_Profile_Server_Connection_Mode(unsigned int BluetoothStackID, AVCTP_Server_Connection_Mode_t *ServerConnectionMode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVCTP_Get_Profile_Server_Connection_Mode_t)(unsigned int BluetoothStackID, AVCTP_Server_Connection_Mode_t *ServerConnectionMode);
#endif

   /*! @brief The following function is responsible for setting the AVCTP Server
    * Connection Mode. @param BluetoothStackID This function accepts as its first parameter the
    * Bluetooth Stack ID of the Bluetooth Stack in which the server
    * exists.  @param ServerConnectionMode The second parameter to this function is the new Server
    * Connection Mode to set the Server to use. @param EventCallback The third parameter
    * An AVCTP Event Callback which will receive notifications of a Bluetooth Connection Request.
    * @param CallbackParameter The final parameter is the callback parameter
    * A user-defined parameter (e.g. a tag value) that will be passed back to
    * the user in the callback function with each event callback.
    * These connection requests will not be dispatched unless the Server Mode
    * (second parameter) is set to asmManualAccept.  In this case the Callback
    * will be invoked whenever a Remote Bluetooth Device attempts to
    * connect to the Local Device.  If the Server Mode (second
    * parameter) is set to anything other than asmManualAccept then the
    * final two parameters are ignored.  Note that only Connection
    * Request events will be dispatched to the specified callback.  No
    * other events will be dispatched.  @return This function returns zero if
    * successful, or a negative return error code if an error occurred.
    * \note The Default Server Connection Mode is
    *            asmAutomaticAccept.
    * \note This function is used for AVCTP Servers which use
    *            Bluetooth Security Mode 2.
    */
BTPSAPI_DECLARATION int BTPSAPI AVCTP_Set_Profile_Server_Connection_Mode(unsigned int BluetoothStackID, AVCTP_Server_Connection_Mode_t ServerConnectionMode, AVCTP_Event_Callback_t EventCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVCTP_Set_Profile_Server_Connection_Mode_t)(unsigned int BluetoothStackID, AVCTP_Server_Connection_Mode_t ServerConnectionMode, AVCTP_Event_Callback_t EventCallback, unsigned long CallbackParameter);
#endif

#endif
