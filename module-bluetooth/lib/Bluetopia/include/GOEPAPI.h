/*
 * Copyright 2000 - 2014 Stonestreet One.
 * All Rights Reserved.
 * Author:  Tim Thomas
 *** MODIFICATION HISTORY ****************************************************
 *
 *   mm/dd/yy  F. Lastname    Description of Modification
 *   --------  -----------    ------------------------------------------------
 *   06/06/01  T. Thomas      Initial creation.
 ****************************************************************************
 */

/**
 *  @file GOEPAPI.h
 *
 *  @brief Stonestreet One Bluetooth Stack GOEP API Type Definitions,
 *  	Constants, and Prototypes. To use any of the following API's,
 *      include the following declaration in your application.
 *
 *  @code
 *  #include "SS1BTPS.h"
 *  @endcode
 *  ============================================================================
 */
#ifndef __GOEPAPIH__
#define __GOEPAPIH__

#include "BTPSKRNL.h"           /*! BTPS Kernel Prototypes/Constants. */

#include "BTAPITyp.h"           /*! Bluetooth API Type Definitions. */
#include "BTTypes.h"            /*! Bluetooth Type Definitions/Constants. */
#include "OBXTypes.h"           /*! Bluetooth OBEX Type Definitions/Constants. */
#include "SDPAPI.h"             /*! Bluetooth SDP API Definitions/Constants. */
#include "SPPAPI.h"             /*! Bluetooth SPP API Definitions/Constants. */

#include "BTPSCFG.h"            /*! BTPS Configuration Constants. */

   /*! The following structure is used with the
    * GOEP_Register_SDP_Record() function. This structure contains
    * additional SDP Service Information that will be added to the SDP
    * OBEX Service Record Entry.  The first member of this structure
    * specifies the Number of Service Class UUID's that are present in
    * the SDPUUIDEntries Array.  This member must be at least one, and
    * the SDPUUIDEntries member must point to an array of SDP UUID
    * Entries that contains (at least) as many entries specified by the
    * NumberServiceClassUUID member.  The ProtocolList member is an SDP
    * Data Element Sequence that contains a list of Protocol Information
    * that will be added to the generic OBEX SDP Service Record.
    */
typedef struct _tagGOEP_SDP_Service_Record_t
{
   unsigned int        NumberServiceClassUUID;
   SDP_UUID_Entry_t   *SDPUUIDEntries;
   SDP_Data_Element_t *ProtocolList;
} GOEP_SDP_Service_Record_t;

#define GOEP_SDP_SERVICE_RECORD_SIZE                    (sizeof(GOEP_SDP_Service_Record_t))

   /*! The following structure is used with the
    * GOEP_Register_Raw_SDP_Record() function.  This structure contains
    * additional SDP Service Information that will be added to the SDP
    * OBEX Service Record Entry.  The first member of this structure
    * specifies the Number of Service Class UUID's that are present in
    * the SDPUUIDEntries Array.  This member must be at least one, and
    * the SDPUUIDEntries member must point to an array of SDP UUID
    * Entries that contains (at least) as many entries specified by the
    * NumberServiceClassUUID member.  The
    * NumberOfProtocolDataListUUIDOffsets and
    * ProtocolDataListUUIDOffsets specify the offsets of the UUIDs in
    * the specified ProtocolDataList data (if any UUIDs).  The
    * ProtocolDataListLength and ProtocolDataList members must contain
    * (if specified) a formatted SDP Data Element Sequence that contains
    * a list of Protocol Information that will be added to the generic
    * OBEX SDP Service Record.
    */
typedef struct _tagGOEP_SDP_Raw_Service_Record_t
{
   unsigned int      NumberServiceClassUUID;
   SDP_UUID_Entry_t *SDPUUIDEntries;
   unsigned int      NumberOfProtocolDataListUUIDOffsets;
   Word_t           *ProtocolDataListUUIDOffsets;
   unsigned int      ProtocolDataListLength;
   Byte_t           *ProtocolDataList;
} GOEP_SDP_Raw_Service_Record_t;

#define GOEP_SDP_RAW_SERVICE_RECORD_SIZE                 (sizeof(GOEP_SDP_Raw_Service_Record_t))

   /*! The following constants represent the Port Open Status Values that
    * are possible in the GOEP Open Port Confirmation Event Data
    * Information.
    */
#define GOEP_OPEN_PORT_STATUS_SUCCESS                                   0x00
#define GOEP_OPEN_PORT_STATUS_CONNECTION_TIMEOUT                        0x01
#define GOEP_OPEN_PORT_STATUS_CONNECTION_REFUSED                        0x02
#define GOEP_OPEN_PORT_STATUS_UNKNOWN_ERROR                             0x03

   /*! The following enumerated type represents all of the allowable
    * States of a Port, either Remote or Server.
    */
typedef enum
{
   psIdle,
   psPortOpening,
   psPortOpened,
   psConnecting,
   psConnected,
   psDisconnecting
} PortState_t;

   /*! The following enumerated type represents all the allowable OBEX
    * Header ID Types that can be used with the OBEX API.
    */
typedef enum
{
   hidCount,
   hidName,
   hidType,
   hidLength,
   hidTime,
   hidDescription,
   hidTarget,
   hidHTTP,
   hidBody,
   hidEndOfBody,
   hidWho,
   hidConnectionID,
   hidApplicationParameters,
   hidAuthenticationChallenge,
   hidAuthenticationResponse,
   hidObjectClass
} OBEX_Header_ID_t;

   /*! The following enumerated type represents all the allowable OBEX
    * Header Format Types that can be used with the OBEX API.
    */
typedef enum
{
   htUnsignedInteger1Byte,
   htUnsignedInteger4Byte,
   htNullTerminatedUnicodeText,
   htByteSequence
} OBEX_Header_Type_t;

   /*! The following enumerated type represents the events that can be
    * generated by the GOEP layer to the upper layers.
    */
typedef enum
{
   etOBEX_Port_Open_Indication,
   etOBEX_Port_Open_Confirmation,
   etOBEX_Port_Close_Indication,
   etOBEX_Connect_Indication,
   etOBEX_Connect_Confirmation,
   etOBEX_Disconnect_Indication,
   etOBEX_Disconnect_Confirmation,
   etOBEX_Put_Indication,
   etOBEX_Put_Confirmation,
   etOBEX_Get_Indication,
   etOBEX_Get_Confirmation,
   etOBEX_Set_Path_Indication,
   etOBEX_Set_Path_Confirmation,
   etOBEX_Abort_Indication,
   etOBEX_Abort_Confirmation,
   etOBEX_Port_Open_Request_Indication
} OBEX_Event_Data_Type_t;

   /*! The following structure represents that format of OBEX Header
    * Values that are varying in length.  This will be used to represent
    * the format of the Value data that represent a Byte Sequence.
    */
typedef struct _tagOBEX_Byte_Sequence_t
{
   Word_t  DataLength;
   Byte_t *ValuePointer;
} OBEX_Byte_Sequence_t;

#define OBEX_BYTE_SEQUENCE_SIZE                         (sizeof(OBEX_Byte_Sequence_t))

   /*! The following structure represents that format of OBEX Header
    * Values that are varying in length.  This will be used to represent
    * the format of the Value data that represent a Null Terminated
    * UNICODE Text string.
    */
typedef struct _tagOBEX_Word_Sequence_t
{
   Word_t  DataLength;
   Word_t *ValuePointer;
} OBEX_Word_Sequence_t;

#define OBEX_WORD_SEQUENCE_SIZE                         (sizeof(OBEX_Word_Sequence_t))

   /*! The following structure us used to define format to be used for
    * all header information.  The Header ID identified the header type
    * that is contained in the structure.  Header data is represented in
    * 1 of 4 different formats.  The Header Type field identifies the
    * format of the header information.  The Header value contains the
    * header value for all fixed length values and pointers to values
    * that are varying in length.
    */
typedef struct _tagOBEX_Header_t
{
   OBEX_Header_ID_t   OBEX_Header_ID;
   OBEX_Header_Type_t OBEX_Header_Type;
   union
   {
      Byte_t               OneByteValue;
      DWord_t              FourByteValue;
      OBEX_Byte_Sequence_t ByteSequence;
      OBEX_Word_Sequence_t UnicodeText;
   } Header_Value;
} OBEX_Header_t;

#define OBEX_HEADER_SIZE                                (sizeof(OBEX_Header_t))

   /*! The following structure is used to contain all Header tags and
    * header values that are associated with an OBEX command.  Since
    * there may be a number of optional headers included in a request or
    * response, the Number of Headers field is provided to assist in
    * locating the end of the information.
    */
typedef struct _tagOBEX_Headers_List_t
{
   Byte_t         NumberOfHeaders;
   OBEX_Header_t *Headers;
} OBEX_Header_List_t;

   /*! The following MACRO is a utility MACRO that exists to aid code
    * readability to Determine the size (in Bytes) of an
    * OBEX Header List Data Structure based upon the number of OBEX
    * Header that are present in the OBEX Header List.
    * @param _x The first parameter to this MACRO is the number of OBEX Headers that are to
    * be part of the OBEX Header List.
    */
#define OBEX_HEADER_LIST_SIZE(_x)                       (sizeof(OBEX_Header_List_t) + ((unsigned int)(_x) * sizeof(OBEX_Header_t)))

   /*! The following definitions represent the maximum size (in Bytes)
    * of the specified fields in the OBEX_Digest_Challenge_t and
    * OBEX_Digest_Response_t structures.  This Implementation of OBEX
    * has a limition on the length of certain OBEX fields.  These
    * lengths are listed below.
    */
#define OBEX_DIGEST_MAXIMUM_REQUEST_DIGEST_LENGTH                16
#define OBEX_DIGEST_MAXIMUM_NONCE_LENGTH                         16

   /*! The following structure is used to pass information about a
    * connection that has been made by the lower layer to a server that
    * has registered services.  The GOEP_ID identifies the server that
    * has been connected to.  The BD_ADDR member denotes the Remote
    * Bluetooth Board Address of the Bluetooth Device that has
    * connected to the local OBEX Server.
    */
typedef struct _tagOBEX_Port_Open_Indication_Data_t
{
   unsigned int GOEP_ID;
   BD_ADDR_t    BD_ADDR;
} OBEX_Port_Open_Indication_Data_t;

#define OBEX_PORT_OPEN_INDICATION_DATA_SIZE             (sizeof(OBEX_Port_Open_Indication_Data_t))

   /*! The following structure is used to pass information about the
    * status of the connect process to the upper layer.  This dispatched
    * when the result from an Open Remote Port command is received.  The
    * parameter PortOpenStatus indicates the success/failure of the
    * connection attempt.
    */
typedef struct _tagOBEX_Port_Open_Confirmation_Data_t
{
   unsigned int GOEP_ID;
   unsigned int PortOpenStatus;
} OBEX_Port_Open_Confirmation_Data_t;

#define OBEX_PORT_OPEN_CONFIRMATION_DATA_SIZE           (sizeof(OBEX_Port_Open_Confirmation_Data_t))

   /*! The following structure is used to pass information about the
    * termination of a connection to the upper layer.  This dispatched
    * when an indication is received from the lower layers that a
    * previously established connection has been terminated.  The
    * parameter GOEP_ID identifies the connection that was closed.
    */
typedef struct _tagOBEX_Port_Close_Indication_Data_t
{
   unsigned int GOEP_ID;
} OBEX_Port_Close_Indication_Data_t;

#define OBEX_PORT_CLOSE_INDICATION_DATA_SIZE            (sizeof(OBEX_Port_Close_Indication_Data_t))

   /*! The following structure is used to pass information received from
    * a connect request up to the upper layers.  The Version number
    * identified the Version of OBEX used by the requester.  The Max
    * Packet Length denoted the maximum size of a packet that can be
    * handled by the requester.  This is not a negotiated value and the
    * Max Packet Length of the responder may be a different size.  Each
    * side must be able to support a Packet Length of at least 255
    * bytes.  The Max Packet Length can not be greater then 64K-1 bytes.
    * All header information, including the Body is enumerated in the
    * Header List field.
    */
typedef struct _tagOBEX_Connect_Indication_Data_t
{
   unsigned int       GOEP_ID;
   Byte_t             Version_Number;
   Word_t             Max_Packet_Length;
   OBEX_Header_List_t Header_List;
} OBEX_Connect_Indication_Data_t;

#define OBEX_CONNECT_INDICATION_DATA_SIZE               (sizeof(OBEX_Connect_Indication_Data_t))

   /*! The following structure is used to pass information received from
    * the response to a Connect Request to the upper layers.  The
    * Version Number identified the OBEX version used by the responder
    * of the request.  The Flags field is used to indicate if the Server
    * can support multiple, simultaneous OBEX Connections to the same
    * SAP-SEL.  The Max Packet Length indicates the largest packet that
    * can be received by the responder.  All header information,
    * including the Body is enumerated in the Header List Field.
    */
typedef struct _tagOBEX_Connect_Confirmation_Data_t
{
   unsigned int       GOEP_ID;
   Byte_t             Response_Code;
   Byte_t             Version_Number;
   Byte_t             Flags;
   Word_t             Max_Packet_Length;
   OBEX_Header_List_t Header_List;
} OBEX_Connect_Confirmation_Data_t;

#define OBEX_CONNECT_CONFIRMATION_DATA_SIZE             (sizeof(OBEX_Connect_Confirmation_Data_t))

   /*! The following structure is used to pass information received from
    * a Disconnect Request to the upper layers.  Any supplied headers
    * will be listed in the Header List Field.  Header information is
    * not required for a disconnect function.
    */
typedef struct _tagOBEX_Disconnect_Indication_Data_t
{
   unsigned int       GOEP_ID;
   OBEX_Header_List_t Header_List;
} OBEX_Disconnect_Indication_Data_t;

#define OBEX_DISCONNECT_INDICATION_DATA_SIZE            (sizeof(OBEX_Disconnect_Indication_Data_t))

   /*! The following structure is used to pass information received from
    * the remote server as a response to a Disconnect Request to the
    * upper layers.  The Response Code contains the response sent from
    * the remote server.  Any additional header information is contained
    * in the Header List.
    */
typedef struct _tagOBEX_Disconnect_Confirmation_Data_t
{
   unsigned int       GOEP_ID;
   Byte_t             Response_Code;
   OBEX_Header_List_t Header_List;
} OBEX_Disconnect_Confirmation_Data_t;

#define OBEX_DISCONNECT_CONFIRMATION_DATA_SIZE          (sizeof(OBEX_Disconnect_Confirmation_Data_t))

   /*! The following structure is used to pass a Put Request received
    * from a remote client to the upper layers.  All header information,
    * including the Body is contained in the Header List field.
    */
typedef struct _tagOBEX_Put_Indication_Data_t
{
   unsigned int       GOEP_ID;
   Boolean_t          Final_Flag;
   OBEX_Header_List_t Header_List;
} OBEX_Put_Indication_Data_t;

#define OBEX_PUT_INDICATION_DATA_SIZE                   (sizeof(OBEX_Put_Indication_Data_t))

   /*! The following structure is used to pass information received from
    * the remote server as a response to a Put Request to the upper
    * layers.  The Response Code contains the response sent from the
    * remote server.  Any additional header information is contained in
    * the Header List.
    */
typedef struct _tagOBEX_Put_Confirmation_Data_t
{
   unsigned int       GOEP_ID;
   Byte_t             Response_Code;
   OBEX_Header_List_t Header_List;
} OBEX_Put_Confirmation_Data_t;

#define OBEX_PUT_CONFIRMATION_DATA_SIZE                 (sizeof(OBEX_Put_Confirmation_Data_t))

   /*! The following structure is used to pass a Get Request received
    * from a remote client to the upper layers.  All header information,
    * including the Body is contained in the Header List field.
    */
typedef struct _tagOBEX_Get_Indication_Data_t
{
   unsigned int       GOEP_ID;
   Boolean_t          Final_Flag;
   OBEX_Header_List_t Header_List;
} OBEX_Get_Indication_Data_t;

#define OBEX_GET_INDICATION_DATA_SIZE                   (sizeof(OBEX_Get_Indication_Data_t))

   /*! The following structure is used to pass information received from
    * the remote server as a response to a Get Request to the upper
    * layers.  The Response Code contains the response sent from the
    * remote server.  Any additional header information is contained in
    * the Header List.
    */
typedef struct _tagOBEX_Get_Confirmation_Data_t
{
   unsigned int       GOEP_ID;
   Byte_t             Response_Code;
   OBEX_Header_List_t Header_List;
} OBEX_Get_Confirmation_Data_t;

#define OBEX_GET_CONFIRMATION_DATA_SIZE                 (sizeof(OBEX_Get_Confirmation_Data_t))

   /*! The following structure is used to pass a Set Path Request
    * received from a remote client to the upper layers.  All header
    * information, including the Body is contained in the Header List
    * field.
    */
typedef struct _tagOBEX_Set_Path_Indication_Data_t
{
   unsigned int       GOEP_ID;
   Boolean_t          CreateDirectory;
   Boolean_t          Backup;
   OBEX_Header_List_t Header_List;
} OBEX_Set_Path_Indication_Data_t;

#define OBEX_SET_PATH_INDICATION_DATA_SIZE              (sizeof(OBEX_Set_Path_Indication_Data_t))

   /*! The following structure is used to pass information received from
    * the remote server as a response to a Set Path Request to the upper
    * layers.  The Response Code contains the response sent from the
    * remote server.  Any additional header information is contained in
    * the Header List.
    */
typedef struct _tagOBEX_Set_Path_Confirmation_Data_t
{
   unsigned int       GOEP_ID;
   Byte_t             Response_Code;
   OBEX_Header_List_t Header_List;
} OBEX_Set_Path_Confirmation_Data_t;

#define OBEX_SET_PATH_CONFIRMATION_DATA_SIZE            (sizeof(OBEX_Set_Path_Confirmation_Data_t))

   /*! The following structure is used to pass a Abort Request received
    * from a remote client to the upper layers.  All header information,
    * including the Body is contained in the Header List field.
    */
typedef struct _tagOBEX_Abort_Indication_Data_t
{
   unsigned int       GOEP_ID;
   OBEX_Header_List_t Header_List;
} OBEX_Abort_Indication_Data_t;

#define OBEX_ABORT_INDICATION_DATA_SIZE                 (sizeof(OBEX_Abort_Indication_Data_t))

   /*! The following structure is used to pass information received from
    * the remote server as a response to a Abort Request to the upper
    * layers.  The Response Code contains the response sent from the
    * remote server.  Any additional header information is contained in
    * the Header List.
    */
typedef struct _tagOBEX_Abort_Confirmation_Data_t
{
   unsigned int       GOEP_ID;
   Byte_t             Response_Code;
   OBEX_Header_List_t Header_List;
} OBEX_Abort_Confirmation_Data_t;

#define OBEX_ABORT_CONFIRMATION_DATA_SIZE               (sizeof(OBEX_Abort_Confirmation_Data_t))

   /*! The following structure is used to pass information about a
    * connection request that has been made by the lower layer to a
    * server that has registered services.  The GOEP_ID identifies the
    * server that has been requested to be connected to.  The BD_ADDR
    * member denotes the Remote Bluetooth Board Address of the Bluetooth
    * Device that has requested to connect to the local OBEX Server.
    */
typedef struct _tagOBEX_Port_Open_Request_Indication_Data_t
{
   unsigned int GOEP_ID;
   BD_ADDR_t    BD_ADDR;
} OBEX_Port_Open_Request_Indication_Data_t;

#define OBEX_PORT_OPEN_REQUEST_INDICATION_DATA_SIZE     (sizeof(OBEX_Port_Open_Request_Indication_Data_t))

   /*! The following structure is used to package event information that
    * is dispatched to the upper layers for processing.  The Event Data
    * Type is identified the type of information that is contained in
    * the structure.
    */
typedef struct _tagGOEP_Event_Data_t
{
   OBEX_Event_Data_Type_t  Event_Data_Type;
   Word_t                  Event_Data_Size;
   union
   {
      OBEX_Port_Open_Indication_Data_t         *OBEX_Port_Open_Indication_Data;
      OBEX_Port_Open_Confirmation_Data_t       *OBEX_Port_Open_Confirmation_Data;
      OBEX_Port_Close_Indication_Data_t        *OBEX_Port_Close_Indication_Data;
      OBEX_Connect_Indication_Data_t           *OBEX_Connect_Indication_Data;
      OBEX_Connect_Confirmation_Data_t         *OBEX_Connect_Confirmation_Data;
      OBEX_Disconnect_Indication_Data_t        *OBEX_Disconnect_Indication_Data;
      OBEX_Disconnect_Confirmation_Data_t      *OBEX_Disconnect_Confirmation_Data;
      OBEX_Put_Indication_Data_t               *OBEX_Put_Indication_Data;
      OBEX_Put_Confirmation_Data_t             *OBEX_Put_Confirmation_Data;
      OBEX_Get_Indication_Data_t               *OBEX_Get_Indication_Data;
      OBEX_Get_Confirmation_Data_t             *OBEX_Get_Confirmation_Data;
      OBEX_Set_Path_Indication_Data_t          *OBEX_Set_Path_Indication_Data;
      OBEX_Set_Path_Confirmation_Data_t        *OBEX_Set_Path_Confirmation_Data;
      OBEX_Abort_Indication_Data_t             *OBEX_Abort_Indication_Data;
      OBEX_Abort_Confirmation_Data_t           *OBEX_Abort_Confirmation_Data;
      OBEX_Port_Open_Request_Indication_Data_t *OBEX_Port_Open_Request_Indication_Data;
   } Event_Data;
} GOEP_Event_Data_t;

#define GOEP_EVENT_DATA_SIZE                            (sizeof(GOEP_Event_Data_t))

   /*! The following declared type represents the Prototype Function for
    * an OBEX Event Callback. This function will be called whenever a
    * defined OBEX Action occurs within the Bluetooth Protocol Stack
    * that is specified with the specified Bluetooth Stack ID.
	* @param BluetoothStackID Unique identifier assigned to this Bluetooth
	* Protocol Stack via a call to BSC_Initialize
	* @param GOEP_Event_Data Data describing the event for which the callback
	* function is called.
    * The caller is free to use the contents of the OBEX
    * Event Data ONLY in the context of this callback.  If the caller
    * requires the Data for a longer period of time, then the callback
    * function MUST copy the data into another Data Buffer.  This
    * function is guaranteed NOT to be invoked more than once
    * simultaneously for the specified installed callback (i.e.  this
    * function DOES NOT have be reentrant).  The processing in this
    * function should be as efficient as possible.  It should be noted
    * that this function is called in the Thread Context of a Thread
    * that the User does NOT own.  Therefore, processing in this
    * function should be as small as possible.
    * \note This function MUST NOT Block and wait for events that
    *            can only be satisfied by Receiving other Bluetooth
    *            Stack Events.  A Deadlock WILL occur because other
    *            Callbacks might not be issued while this function is
    *            currently outstanding.
    */
typedef void (BTPSAPI *GOEP_Event_Callback_t)(unsigned int BluetoothStackID, GOEP_Event_Data_t *GOEP_Event_Data, unsigned long CallbackParameter);

   /*! @brief The following function is responsible for establishing a GOEP Port
    * Server (will wait for a connection to occur on the port
    * established by this function).
	* @param BluetoothStackID Unique identifier assigned to this Bluetooth
	* Protocol Stack via a call to BSC_Initialize
	* @param ServerPort	Port number to use.  This must fall in the range defined
	* by the following constants:
	* SPP_PORT_NUMBER_MINIMUM and SPP_PORT_NUMBER_MAXIMUM
	* @param MaxPacketLength Max packet length that will be accepted by this server.
	* @param GOEP_Event_Callback Function to call when events occur on this port.
	* @param CallbackParameter A user-defined parameter (e.g., a tag value)
	* that will be passed back to the user in the callback function with each packet.
    * @return This function returns a non-zero, positive, number on
    * success or a negative return error code if an error occurred (see
    * BTERRORS.H).  A successful return code will be a GOEP Port ID
    * that can be used to reference the Opened GOEP Port in ALL other
    * functions in this module (except the GOEP_Open_Remote_Port()
    * function).  Once a Server GOEP Port is opened, it can only be
    * Un-Registered via a call to the GOEP_Close_Server_Port() function
    * (passing the return value from this function).  The
    * GOEP_Close_Port() function can be used to Disconnect a Client from
    * the Server Port (if one is connected, it will NOT Un-Register the
    * Server Port however.
    */
BTPSAPI_DECLARATION int BTPSAPI GOEP_Open_Server_Port(unsigned int BluetoothStackID, unsigned int ServerPort, Word_t MaxPacketLength, GOEP_Event_Callback_t GOEP_Event_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GOEP_Open_Server_Port_t)(unsigned int BluetoothStackID, unsigned int ServerPort, Word_t MaxPacketLength, GOEP_Event_Callback_t GOEP_Event_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is responsible for Un-Registering an OBEX
    * Port Server (which was Registered by a successful call to the
    * GOEP_Open_Server_Port() function).
	* @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
	* Stack via a call to BSC_Initialize
	* @param GOEP_ID The port to close. This is the value that was returned from the
	* GOEP_Open_Server_Port() function.
    * @return This function returns zero if successful, or a negative return error
    * code if an error occurred (see BTERRORS.H).  Note that this
    * function does NOT delete any SDP Service Record Handles.
    */
BTPSAPI_DECLARATION int BTPSAPI GOEP_Close_Server_Port(unsigned int BluetoothStackID, unsigned int GOEP_ID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GOEP_Close_Server_Port_t)(unsigned int BluetoothStackID, unsigned int GOEP_ID);
#endif

   /*! @brief The following function is responsible for responding to requests
    * to connect to an OBEX Port Server.
	* @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
	* Stack via a call to BSC_Initialize
	* @param GOEP_ID The port this command applies to. This is the value that was
	* returned from the GOEP_Open_Server_Port( ) function.
	* @param AcceptConnection Boolean indicating if the pending connection should
	* be accepted.
    * @return This function returns
    * zero if successful, or a negative return value if there was an
    * error.
    */
BTPSAPI_DECLARATION int BTPSAPI GOEP_Open_Port_Request_Response(unsigned int BluetoothStackID, unsigned int GOEP_ID, Boolean_t AcceptConnection);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GOEP_Open_Port_Request_Response_t)(unsigned int BluetoothStackID, unsigned int GOEP_ID, Boolean_t AcceptConnection);
#endif

   /*! @brief The following function is provided to allow a means to add a
    * Generic OBEX Service Record to the SDP Database.

	* @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
	* Stack via a call to BSC_Initialize
	* @param GOEP_ID The port this command applies to. This is the value that was
	* returned from the GOEP_Open_Server_Port( ) function.
	* @param SDPServiceRecord Any additional Service Discovery Protocol information
	* to be added to the record for this serial port server.
	* @param ServiceName Name to appear in the SDP Database for this service.
	* @param SDPServiceRecordHandle Returned handle to the SDP Database entry
	* which may be used to remove the entry at a later time.
    * @return If this function zero, then the
    * SDPServiceRecordHandle entry will contain the Service Record
    * Handle of the added SDP Service Record.  If this function fails,
    * a negative return error code will be returned (see BTERRORS.H)
    * and the SDPServiceRecordHandle value will be undefined.
    * \note This function should only be called with the GOEP Port ID
    *          that was returned from the GOEP_Open_Server_Port()
    *          function.  This function should NEVER be used with the
    *          GOEP Port ID returned from the GOEP_Open_Remote_Port()
    *          function.
    * \note The Service Record Handle that is returned from this
    *          function will remain in the SDP Record Database until
    *          it is deleted by calling the SDP_Delete_Service_Record()
    *          function.
    * \note A MACRO is provided to Delete the Service Record from
    *          the SDP Data Base.  This MACRO maps the
    *          GOEP_Un_Register_SDP_Record() to
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
BTPSAPI_DECLARATION int BTPSAPI GOEP_Register_SDP_Record(unsigned int BluetoothStackID, unsigned int GOEP_ID, GOEP_SDP_Service_Record_t *SDPServiceRecord, char *ServiceName, DWord_t *SDPServiceRecordHandle);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GOEP_Register_SDP_Record_t)(unsigned int BluetoothStackID, unsigned int GOEP_ID, GOEP_SDP_Service_Record_t *SDPServiceRecord, char *ServiceName, DWord_t *SDPServiceRecordHandle);
#endif

   /*! @brief The following function is provided to allow a means to add a
    * Generic Raw OBEX Service Record to the SDP Database.
	* @param BluetoothStackID Unique identifier assigned to this Bluetooth
	* Protocol Stack via a call to BSC_Initialize
	* @param GOEP_ID The port this command applies to. This is the value
	* that was returned from the GOEP_Open_Server_Port() function.
	* @param SDPServiceRecord Contains any additional Service Discovery
	* Protocol information to be added to the record for this serial port server.
	* @param ServiceName Name to appear in the SDP Database for this service.
	* @param SDPServiceRecordHandle Returned handle to the SDP Database
	* entry which may be used to remove the entry at a later time.
    * @return If this function zero, then the
    * SDPServiceRecordHandle entry will contain the Service Record
    * Handle of the added SDP Service Record.  If this function fails, a
    * negative return error code will be returned (see BTERRORS.H) and
    * the SDPServiceRecordHandle value will be undefined.
    * \note This function should only be called with the GOEP Port ID
    *          that was returned from the GOEP_Open_Server_Port()
    *          function.  This function should NEVER be used with the
    *          GOEP Port ID returned from the GOEP_Open_Remote_Port()
    *          function.
    * \note The Service Record Handle that is returned from this
    *          function will remain in the SDP Record Database until it
    *          is deleted by calling the SDP_Delete_Service_Record()
    *          function.
    * \note A MACRO is provided to Delete the Service Record from the
    *          SDP Data Base.  This MACRO maps the
    *          GOEP_Un_Register_SDP_Record() to
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
BTPSAPI_DECLARATION int BTPSAPI GOEP_Register_Raw_SDP_Record(unsigned int BluetoothStackID, unsigned int GOEP_ID, GOEP_SDP_Raw_Service_Record_t *SDPServiceRecord, char *ServiceName, DWord_t *SDPServiceRecordHandle);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GOEP_Register_Raw_SDP_Record_t)(unsigned int BluetoothStackID, unsigned int GOEP_ID, GOEP_SDP_Raw_Service_Record_t *SDPServiceRecord, char *ServiceName, DWord_t *SDPServiceRecordHandle);
#endif

   /*! The following MACRO is a utility MACRO that simply deletes the
    * GOEP SDP Service Record (specified by the third parameter) from
    * SDP Database.  This MACRO simply maps to the
    * SDP_Delete_Service_Record() function.  This MACRO is only
    * provided so that the caller doesn't have to sift through the SDP
    * API for very simplistic applications.  This function accepts as
    * input the Bluetooth Stack ID of the Bluetooth Protocol Stack that
    * the Service Record exists on, the GOEP Port ID (returned from a
    * successful call to the GOEP_Open_Server_Port() function), and the
    * SDP Service Record Handle.  The SDP Service Record Handle was
    * returned via a succesful call to the GOEP_Register_SDP_Record()
    * function.  See the GOEP_Register_SDP_Record() function for more
    * information.  This MACRO returns the result of the
    * SDP_Delete_Service_Record() function, which is zero for success
    * or a negative return error code (see BTERRORS.H).
    */
#define GOEP_Un_Register_SDP_Record(__BluetoothStackID, __GOEPID, __SDPRecordHandle) (SDP_Delete_Service_Record(__BluetoothStackID, __SDPRecordHandle))

   /*! @brief The following function is responsible for Opening a Remote GOEP
    * Port on the specified Remote Device.
	* @param BluetoothStackID Unique identifier assigned
	* to this Bluetooth Protocol Stack via a call to BSC_Initialize
	* @param BD_ADDR Address of the Bluetooth device to
	* connect with.
	* @param ServerPort The remote device’s server port ID to connect with.
	* @param MaxPacketLength The largest packet that will be
	* sent on this connection. Each side must support a minimum of 255 bytes,
	* and cannot have a packet size greater than 64K- bytes.
	* @param GOEP_Event_Callback Function to call when events occur on this port.
	* @param CallbackParameter A user-defined parameter (e.g., a tag value)
	* that will be passed back to the user in the callback function with each packet.
    * @return This function returns a
    * non-zero, positive, value if successful, or a negative return
    * error code if this function is unsuccessful.  If this function is
    * successful, the return value will represent the GOEP Port ID that
    * can be passed to all other functions that require it.  Once a
    * GOEP Port is opened, it can only be closed via a call to the
    * GOEP_Close_Port() function (passing the return value from this
    * function).
    */
BTPSAPI_DECLARATION int BTPSAPI GOEP_Open_Remote_Port(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, unsigned int ServerPort, Word_t MaxPacketLength, GOEP_Event_Callback_t GOEP_Event_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GOEP_Open_Remote_Port_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, unsigned int ServerPort, Word_t MaxPacketLength, GOEP_Event_Callback_t GOEP_Event_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function exists to close an OBEX Port that was
    * previously opened with the GOEP_Open_Server_Port() function OR the
    * GOEP_Open_Remote_Port() function.
	* @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
	* Stack via a call to BSC_Initialize
	* @param GOEP_ID The port to close. This is the value that was returned from
	* the GOEP_Open_Server_Port() or GOEP_Open_Remote_Port() function.
    * @return This function returns zero if successful, or a negative return value if there
    * was an error.  This function does NOT Un-Register a OBEX Server
    * Port from the system, it ONLY disconnects any connection that is
    * currently active on the Server Port.  The OBEX_Close_Server_Port()
    * function can be used to Un-Register the OBEX Server Port.
    */
BTPSAPI_DECLARATION int BTPSAPI GOEP_Close_Port(unsigned int BluetoothStackID, unsigned int GOEP_ID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GOEP_Close_Port_t)(unsigned int BluetoothStackID, unsigned int GOEP_ID);
#endif

   /*! @brief The following function exists to Send an OBEX Connect Request to
    * the Remote OBEX entity.  The OBEX entity is referenced by the
    * GOEP ID that was returned from an OBEX_Open_Remote_Port or and
    * OBEX_Open_Server_Port.
	* @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
	* Stack via a call to BSC_Initialize.
	* @param GOEP_ID The port to use for the connection.
	* @param Header_List A pointer to an array of optional headers.
    * @return This function returns zero if
    * successful, or a negative return value if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI GOEP_Connect_Request(unsigned int BluetoothStackID, unsigned int GOEP_ID, OBEX_Header_List_t *Header_List);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GOEP_Connect_Request_t)(unsigned int BluetoothStackID, unsigned int GOEP_ID, OBEX_Header_List_t *Header_List);
#endif

   /*! @brief The following function exists to Send an OBEX Disconnect Request
    * to the Remote OBEX entity.  The OBEX entity is referenced by the
    * GOEP_ID that was returned from an OBEX_Open_Remote_Port or and
    * OBEX_Open_Server_Port.
	* @param BluetoothStackID Unique identifier assigned to this Bluetooth
	* Protocol Stack via a call to BSC_Initialize
	* @param GOEP_ID The port to close the connection on. This is the
	* value that was returned from either the GOEP_Open_Remote_Port() or
	* GOEP_Open_Server_Port() function.
	* @param Header_List A pointer to an array of optional headers.
	* See GOEP_Connect_Request() for a description of the headers.
    * @return This function returns zero if
    * successful, or a negative return value if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI GOEP_Disconnect_Request(unsigned int BluetoothStackID, unsigned int GOEP_ID, OBEX_Header_List_t *Header_List);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GOEP_Disconnect_Request_t)(unsigned int BluetoothStackID, unsigned int GOEP_ID, OBEX_Header_List_t *Header_List);
#endif

   /*! @brief The following function exists to Send an OBEX Put Request to the
    * Remote OBEX entity.  The OBEX entity is referenced by the GOEP_ID
    * that was returned from an OBEX_Open_Remote_Port or and
    * OBEX_Open_Server_Port.
	* @param BluetoothStackID Unique identifier assigned to this Bluetooth
	* Protocol Stack via a call to BSC_Initialize
	* @param GOEP_ID The port to send the Put Request to. This is the value
	* that was returned from the GOEP_Open_Remote_Port() function.
	* @param Final Flag which indicates if this is the last packet of the
	* Put sequence or not.
	* @param Header_List A pointer to an array of OBEX headers. This is
	* the data to send. See GOEP_Connect_Request() for a description of the headers.
    * @return This function returns zero if
    * successful, or a negative return value if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI GOEP_Put_Request(unsigned int BluetoothStackID, unsigned int GOEP_ID, Boolean_t Final, OBEX_Header_List_t *Header_List);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GOEP_Put_Request_t)(unsigned int BluetoothStackID, unsigned int GOEP_ID, Boolean_t Final, OBEX_Header_List_t *Header_List);
#endif

   /*! @brief The following function exists to Send an OBEX Get Request to the
    * Remote OBEX entity.  The OBEX entity is referenced by the GOEP_ID
    * that was returned from an OBEX_Open_Remote_Port or and
    * OBEX_Open_Server_Port.
	* @param BluetoothStackID Unique identifier assigned to this Bluetooth
	* Protocol Stack via a call to BSC_Initialize
	* @param GOEP_ID The port to send the Get Request to.  This is the
	* value that was returned from the GOEP_Open_Remote_Port() function.
	* @param Final Flag which indicates when all the headers have been
	* sent over and the Server should start sending the object data.
	* @param Header_List A pointer to an optional array of OBEX headers.
	* This is the data to be retrieved, and is only optional on the final call.
	* See GOEP_Connect_Request() for a description of the headers.
    * @return This function returns zero if
    * successful, or a negative return value if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI GOEP_Get_Request(unsigned int BluetoothStackID, unsigned int GOEP_ID, Boolean_t Final, OBEX_Header_List_t *Header_List);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GOEP_Get_Request_t)(unsigned int BluetoothStackID, unsigned int GOEP_ID, Boolean_t Final, OBEX_Header_List_t *Header_List);
#endif

   /*! @brief The following function exists to Send an OBEX Set Path Request to
    * the Remote OBEX entity.  The OBEX entity is referenced by the
    * GOEP_ID that was returned from an OBEX_Open_Remote_Port or and
    * OBEX_Open_Server_Port.
	* @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
	* Stack via a call to BSC_Initialize
	* @param GOEP_ID The port to send the Set Path Request to. This is
	* the value that was returned from the GOEP_Open_Remote_Port() function.
	* @param Flags Flags to control folder navigation and creation.
	* Possible values are:
	* OBEX_SET_PATH_FLAGS_BACKUP_MASK or OBEX_SET_PATH_FLAGS_NO_CREATE_MASK
	* @param Header_List A pointer to an array of OBEX headers.
	* The path to change to should be provided in a hidName type header.
	* See GOEP_Connect_Request() for a description of the headers.
    * @return This function returns zero if successful, or a negative return
    * value if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI GOEP_Set_Path_Request(unsigned int BluetoothStackID, unsigned int GOEP_ID, Byte_t Flags, OBEX_Header_List_t *Header_List);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GOEP_Set_Path_Request_t)(unsigned int BluetoothStackID, unsigned int GOEP_ID, Byte_t Flags, OBEX_Header_List_t *Header_List);
#endif

   /*! @brief The following function exists to Send an OBEX Abort Request to the
    * Remote OBEX entity.  The OBEX entity is referenced by the GOEP_ID
    * that was returned from an OBEX_Open_Remote_Port or and
    * OBEX_Open_Server_Port.
	* @param BluetoothStackID Unique identifier assigned to
	* this Bluetooth Protocol Stack via a call to BSC_Initialize
	* @param GOEP_ID The port to send the Abort Request to.
	* This is the value that was returned from either the
	* GOEP_Open_Remote_Port() or the GOEP_Open_Server_Port() function.
	* @param Header_List A pointer to an array of OBEX headers.
	* See GOEP_Connect_Request() for a description of the headers.
    * @return This function returns zero if successful,
    * or a negative return value if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI GOEP_Abort_Request(unsigned int BluetoothStackID, unsigned int GOEP_ID, OBEX_Header_List_t *Header_List);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GOEP_Abort_Request_t)(unsigned int BluetoothStackID, unsigned int GOEP_ID, OBEX_Header_List_t *Header_List);
#endif

   /*! @brief The following function exists to Send an OBEX Command Response to
    * the Remote OBEX entity.  The OBEX entity is referenced by the
    * GOEP ID that was returned from an OBEX_Open_Remote_Port or and
    * OBEX_Open_Server_Port.
	* @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize
	* @param GOEP_ID The port to send the Comamnd Response to.  This is the value that was provided in the event being responded to.
	* @param ResponseCode Response code to return to the requester.
	* This code is a logical ORing of the Final status flag
	* (0x80 or the constant: OBEX_FINAL_BIT) with one of the following possible status values
	* (all less than 0x7F).
	* OBEX_CONTINUE_RESPONSE
	* OBEX_OK_RESPONSE
	* OBEX_CREATED_RESPONSE
	* OBEX_ACCEPTED_RESPONSE
	* OBEX_NON_AUTHORITATIVE_INFORMATION_RESPONSE
	* OBEX_NO_CONTENT_RESPONSE
	* OBEX_RESET_CONTENT_RESPONSE
	* OBEX_PARTIAL_CONTENT_RESPONSE
	* OBEX_MULTIPLE_CHOICES_RESPONSE
	* OBEX_MOVED_PERMANETLY_RESPONSE
	* OBEX_MOVED_TEMPORARILY_RESPONSE
	* OBEX_SEE_OTHER_RESPONSE
	* OBEX_NOT_MODIFIED_RESPONSE
	* OBEX_USE_PROXY_RESPONSE
	* OBEX_BAD_REQUEST_RESPONSE
	* OBEX_UNAUTHORIZED_RESPONSE
	* OBEX_PAYMENT_REQUIRED_RESPONSE
	* OBEX_FORBIDDEN_RESPONSE
	* OBEX_NOT_FOUND_RESPONSE
	* OBEX_METHOD_NOT_ALLOWED_RESPONSE
	* OBEX_NOT_ACCEPTABLE_RESPONSE
	* OBEX_PROXY_AUTHENTICATION_REQUIRED_RESPONSE
	* OBEX_REQUEST_TIMEOUT_RESPONSE
	* OBEX_CONFLICT_RESPONSE
	* OBEX_GONE_RESPONSE
	* OBEX_LENGTH_REQUIRED_RESPONSE
	* OBEX_PRECONDITION_FAILED_RESPONSE
	* OBEX_REQUESTED_ENTITY_TOO_LARGE_RESPONSE
	* OBEX_REQUESTED_URL_TOO_LARGE_RESPONSE
	* OBEX_UNSUPORTED_MEDIA_TYPE_RESPONSE
	* OBEX_INTERNAL_SERVER_ERROR_RESPONSE
	* OBEX_NOT_IMPLEMENTED_RESPONSE
	* OBEX_BAD_GATEWAY_RESPONSE
	* OBEX_SERVICE_UNAVAILABLE_RESPONSE
	* OBEX_GATEWAY_TIMEOUT_RESPONSE
	* OBEX_HTTP_VERSION_NOT_SUPPORTED_RESPONSE
	* OBEX_DATABASE_FULL_RESPONSE
	* OBEX_DATABASE_LOCKED_RESPONSE
	* @param Header_List Optional list of headers to be passed with the command response
	* (e.g., return data object requested).
    * @return This function returns zero if successful,
    * or a negative return value if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI GOEP_Command_Response(unsigned int BluetoothStackID, unsigned int GOEP_ID, Byte_t ResponseCode, OBEX_Header_List_t *Header_List);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GOEP_Command_Response_t)(unsigned int BluetoothStackID, unsigned int GOEP_ID, Byte_t ResponseCode, OBEX_Header_List_t *Header_List);
#endif

   /*! @brief The following function is responsible for allowing a mechanism to
    * query the OBEX Port Server Connection Mode.
	* @param BluetoothStackID Unique identifier assigned to this Bluetooth
	* Protocol Stack via a call to BSC_Initialize
	* @param GOEP_ID The port this command applies to.This is the value
	* that was returned from the GOEP_Open_Server_Port() function.
	* @param ServerConnectionMode Pointer to a variable to receive the
	* current Server Connection Mode.
    * @return This function returns zero if successful, or a negative return value if there
    * was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI GOEP_Get_Server_Connection_Mode(unsigned int BluetoothStackID, unsigned int GOEP_ID, SPP_Server_Connection_Mode_t *ServerConnectionMode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GOEP_Get_Server_Connection_Mode_t)(unsigned int BluetoothStackID, unsigned int GOEP_ID, SPP_Server_Connection_Mode_t *ServerConnectionMode);
#endif

   /*! @brief The following function is responsible for allowing a mechanism to
    * change the current OBEX Port Server Connection Mode.
	* @param BluetoothStackID Unique identifier assigned to this Bluetooth
	* Protocol Stack via a call to BSC_Initialize
	* @param GOEP_ID The port this command applies to.  This is the value
	* that was returned from the GOEP_Open_Server_Port() function.
	* @param ServerConnectionMode The new Server Connection Mode being set.
    * @return This function returns zero if successful, or a negative return value if
    * there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI GOEP_Set_Server_Connection_Mode(unsigned int BluetoothStackID, unsigned int GOEP_ID, SPP_Server_Connection_Mode_t ServerConnectionMode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GOEP_Set_Server_Connection_Mode_t)(unsigned int BluetoothStackID, unsigned int GOEP_ID, SPP_Server_Connection_Mode_t ServerConnectionMode);
#endif

   /* Given a pointer to a list of headers this function will traverse
    * the hidApplicationParameters Header types and match the Tag ID
    * to one of the Triplets. If no match is found throughout the list
    * a NULL is returned, which is feasible for optional parameters.
	* @param HeaderListPtr Pointer to list of OBEX Headers
	* @param TagID The Tag ID to attempt to match in the header.
	* @return If successful, pointer to the OBEX Application
	* Parameters structure which was matched.
    */
BTPSAPI_DECLARATION OBEX_Application_Parameters_t *BTPSAPI GOEP_Find_Application_Parameter_Header_By_Tag_ID(OBEX_Header_List_t *HeaderList, Byte_t TagID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef OBEX_Application_Parameters_t *(BTPSAPI *PFN_GOEP_Find_Application_Parameter_Header_By_Tag_ID_t)(OBEX_Header_List_t *HeaderList, Byte_t TagID);
#endif

   /*! @brief The following function is used to scan through an array of headers
    * for the header ID type that was specified.
	* @param HeaderID Header ID to search for.
	* @param HeaderList Pointer to header list to search for HeaderID.
    * @return If found, the index of
    * the header in the list is returned.  If the header was not
    * located, the function returns a negative value.
    */
BTPSAPI_DECLARATION int BTPSAPI GOEP_Find_Header(OBEX_Header_ID_t HeaderID, OBEX_Header_List_t *HeaderList);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GOEP_Find_Header_t)(OBEX_Header_ID_t HeaderID, OBEX_Header_List_t *HeaderList);
#endif

   /*! @brief The following function is used to generate the MD5 Hash of the two
    * pieces required for OBEX Authentication.  The two pieces refer to
    * the first part of the data to be MD5 hashed before the OBEX
    * Delimeter and the second part of the data to be MD5 hashed after
    * the OBEX Delimeter.  The OBEX Delimeter used by this function is
    * defined as:
    *
    *    OBEX_DIGEST_CHALLENGE_RESPONSE_NONCE_MD5_DELIMETER_BYTE
    *
    * The first and second parts *MUST* be specified and cannot be
    * of zero length.  The MD5 Hash is returned (as an OBEX_Nonce_t)
    * in the buffer passed as the final parameter to this function (this
    * parameter also *MUST* be specified and cannot be NULL.
	* @param PreDelimeterLength Number of bytes in the byte array pointed
	* to by PreDelimeterData.
	* @param PreDelimeterData The byte array buffer that holds the piece
	* that will MD5 hashed before the OBEX Delimeter.
	* @param PostDelimeterLength Number of bytes in the byte array pointed
	* to by PostDelimeterData.
	* @param PostDelimeterData The byte array buffer that holds the piece
	* that will MD5 hashed afer the OBEX Delimeter.
	* @param OutputNonce Buffer to hold the returned MD5 hash. Must not be
	* NULL.
    * @return This function returns zero if successful, or a negative return error
    * code if there was an error.
    * \note As an example (using simple ASCII strings):
    *
    *             GOEP_Generate_Digest_Nonce(4, "ABCD", 5, "WXYZ", &N);
    *
    *          would calculate the MD5 Hash of the following 9 bytes:
    *
    *             ABCD:WXYZ
    *
    *          and return this in the buffer pointed to by N.  Note
    *          that the ':' character is assumed to be the Delimeter
    *          constant mentioned above.
    */
BTPSAPI_DECLARATION int BTPSAPI GOEP_Generate_Digest_Nonce(unsigned int PreDelimeterLength, Byte_t *PreDelimeterData, unsigned int PostDelimeterLength, Byte_t *PostDelimeterData, OBEX_Nonce_t *OutputNonce);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GOEP_Generate_Digest_Nonce_t)(unsigned int PreDelimeterLength, Byte_t *PreDelimeterData, unsigned int PostDelimeterLength, Byte_t *PostDelimeterData, OBEX_Nonce_t *OutputNonce);
#endif

#endif
