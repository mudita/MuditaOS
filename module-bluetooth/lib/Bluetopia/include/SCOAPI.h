/*
 * Copyright 2000 - 2014 Stonestreet One.
 * All Rights Reserved.
 * Author:  Damon Lange
 *** MODIFICATION HISTORY ****************************************************
 *
 *   mm/dd/yy  F. Lastname    Description of Modification
 *   --------  -----------    ------------------------------------------------
 *   10/10/00  D. Lange       Initial creation.
 *   06/25/08  D. Wooldridge  Merged D. Franck's eSCO API additions.
 ****************************************************************************
 */

/**
 *  @file SCOAPI.h
 *
 *  @brief Stonestreet One Bluetooth Stack SCO API Type Definitions,
 *      Constants, and Prototypes. To use any of the following API's,
 *      include the following declaration in your application.
 *
 *  @code
 *  #include "SS1BTPS.h"
 *  @endcode
 *  ============================================================================
 */
#ifndef __SCOAPIH__
#define __SCOAPIH__

#include "BTPSKRNL.h"           /*! BTPS Kernel Prototypes/Constants. */

#include "BTAPITyp.h"           /*! Bluetooth API Type Definitions. */
#include "BTTypes.h"            /*! Bluetooth Type Definitions/Constants. */

#include "BTPSCFG.h"            /*! BTPS Configuration Constants. */

   /*! The following structure is used with the
    * SCO_Query_Packet_Information() and SCO_Change_Packet_Information()
    * function to retrieve/change the supported SCO Buffer Information
    * that is supported by the Local Bluetooth Module.
    * \note This information is returned straight from the Bluetooth
    *          Module and is only provided so that SCO Applications can
    *          be written without having to sift through HCI API
    *          functions to determine the required information.
    * \note Special care should be used when using this structure
    *          with the SCO_Change_Packet_Information() function since
    *          the default values that are typically used are determined
    *          by read the information directly from the Bluetooth
    *          Device.
    */
typedef struct _tagSCO_Packet_Information_t
{
   unsigned int MaximumOutstandingSCOPackets;
   unsigned int MaximumSCOPacketSize;
} SCO_Packet_Information_t;

#define SCO_PACKET_INFORMATION_SIZE                     (sizeof(SCO_Packet_Information_t))

   /*! The following structure and these flags are used to control that
    * handling of data packets at the SCO Layer.  The QueueLimit value
    * indicates the upper limit (defined in number of packets) at which
    * the connection is considered full.  The LowThreshold is the limit
    * (defined in number of packets) at which the connection is unlocked
    * and data can once again be sent on the connection.
    */
typedef struct _tagSCO_Queueing_Parameters_t
{
   Word_t QueueLimit;
   Word_t LowThreshold;
} SCO_Queueing_Parameters_t;

#define SCO_QUEUEING_PARAMETERS_SIZE                    (sizeof(SCO_Queueing_Parameters_t))

   /*! The following enumerated type specifies the various forms of
    * Encoded Data that can be processed by the Bluetooth Device.  This
    * value specifies the input/output encoding type of data that is
    * sent/received from the Bluetooth Device.
    * \note It's possible that the Bluetooth Hardware does not
    *          support some of the specified types.
    */
typedef enum
{
   deLinearPCM,
   deuLaw,
   deALaw
} SCO_Data_Encoding_Type_t;

   /*! The following enumerated type specifies the various format of
    * Encoded Data that can be processed by the Bluetooth Device.  This
    * value specifies the input/output format of data that is
    * sent/received from the Bluetooth Device.
    * \note It's possible that the Bluetooth Hardware does not
    *          support some of the specified formats.
    */
typedef enum
{
   ef1sComplement,
   ef2sComplement,
   efSignMagnitude,
   efUnsigned
} SCO_Data_Encoding_Format_t;

   /*! The following enumerated type specifies the encoding size of
    * Linear PCM Encoded Data that can be processed by the Bluetooth
    * Device.  This value specifies the input/output format of data that
    * is sent/received from the Bluetooth Device.
    * \note This parameter is only applicable when Linear PCM is the
    *          type of encoded data being used.
    * \note It's possible that the Bluetooth Hardware does not
    *          support some of the specified sizes.
    */
typedef enum
{
   ds8Bit,
   ds16Bit
} SCO_PCM_Data_Sample_Size_t;

   /*! The following enumerated type specifies the encoding type that
    * is to be used when transmitting SCO Data over the air.  This
    * value specifies the input/output encoding type that is to be
    * used for transmission.
    * \note It's possible that the Bluetooth Hardware does not
    *          support some of the specified types.
    */
typedef enum
{
   aeCVSD,
   aeuLaw,
   aeALaw,
   aemSBC,
   aeNone
} SCO_Air_Encoding_Type_t;

   /*! The following structure is used with the SCO_Query_Data_Format()
    * and SCO_Change_Data_Format() functions to Query or Change
    * (respectively) the SCO Data Format.  This information specifies
    * the Input/Output Encoding type as well as the Air Encoding Type.
    * \note Since not all Bluetooth Hardware supports all of the
    *          various encoding options and there is not standard
    *          defined, default, option, the current Data Format should
    *          be queried and set to the desired options.
    */
typedef struct _tagSCO_Data_Format_t
{
   SCO_Data_Encoding_Type_t   SCO_Data_Encoding_Type;
   SCO_Data_Encoding_Format_t SCO_Data_Encoding_Format;
   SCO_PCM_Data_Sample_Size_t SCO_PCM_Data_Sample_Size;
   SCO_Air_Encoding_Type_t    SCO_Air_Encoding_Type;
} SCO_Data_Format_t;

   /*! The following enumerated type specifies the retransmission modes
    * for a eSCO connection.
    * \note It's possible that the Bluetooth Hardware does not
    *          support some of the specified types.
    */
typedef enum
{
   reNoRetransmissions,
   reRetransmitOptimizePowerConsumption,
   reRetransmitOptimizeLinkQuality,
   reDontCare
} SCO_Retransmission_Effort_t;

   /*! The following enumerated type specifies the supported link types.
    */
typedef enum
{
   ltSCO,
   ltESCO
} SCO_Link_Type_t;

   /* The default packet type masks that are used for eSCO links.  The
    * default is to enable all Packet types.  These values are used with
    * the PacketTypes parameter of the PacketTypes member of the
    * SCO_Synchronous_Connection_Info_t structure.
    */
#define SCO_PACKET_SYNCHRONOUS_CONNECTION_TYPE_DEFAULT            ( HCI_PACKET_SYNCHRONOUS_CONNECTION_TYPE_HV1                   |   \
                                                                    HCI_PACKET_SYNCHRONOUS_CONNECTION_TYPE_HV2                   |   \
                                                                    HCI_PACKET_SYNCHRONOUS_CONNECTION_TYPE_HV3                   |   \
                                                                    HCI_PACKET_SYNCHRONOUS_CONNECTION_TYPE_EV3                   |   \
                                                                    HCI_PACKET_SYNCHRONOUS_CONNECTION_TYPE_EV4                   |   \
                                                                    HCI_PACKET_SYNCHRONOUS_CONNECTION_TYPE_EV5                   |   \
                                                                   !HCI_PACKET_SYNCHRONOUS_CONNECTION_TYPE_2_EV3_MAY_NOT_BE_USED |   \
                                                                   !HCI_PACKET_SYNCHRONOUS_CONNECTION_TYPE_3_EV3_MAY_NOT_BE_USED |   \
                                                                   !HCI_PACKET_SYNCHRONOUS_CONNECTION_TYPE_2_EV5_MAY_NOT_BE_USED |   \
                                                                   !HCI_PACKET_SYNCHRONOUS_CONNECTION_TYPE_3_EV5_MAY_NOT_BE_USED   )

   /*! The following structure is used to define eSCO/SCO connections.
    * The TransmitBandwidth and ReceiveBandwidth are in octets per
    * second.  The MaxLatency is the value in milliseconds representing
    * the upper limit of the sum of the synchronous interval, and must
    * be greater than 3.  The VoiceSettings is defined in the
    * SCO_Data_Format_t structure.  Retransmission_Effort is defined in
    * SCO SCO_Retransmission_Effort_t.
    */
typedef struct _tagSCO_Synchronous_Connection_Info_t
{
   SCO_Link_Type_t              LinkType;
   DWord_t                      TransmitBandwidth;
   DWord_t                      ReceiveBandwidth;
   Word_t                       MaxLatency;
   SCO_Data_Format_t            DataFormat;
   SCO_Retransmission_Effort_t  RetransmissionEffort;
   Word_t                       PacketTypes;
} SCO_Synchronous_Connection_Info_t;

#define SCO_SYNCHRONOUS_CONNECTION_INFO_SIZE            (sizeof(SCO_Synchronous_Connection_Info_t))

   /*! The following structure defines the the enhanced audio connection
    * parameters.  Please refer to the core specification for the
    * meaning of all parameters except for the mSBCDataPathHCI
    * parameter.
    * \note The mSBCDataPathHCI parameter only applies when the
    *          DataPath is set to HCI and mSBC is being used
    *          over-the-air.  In this scenario if mSBCDataPathHCI is
    *          true then mSBC is carried over HCI, if it is false then
    *          the data is decoded by the controller and transported
    *          over HCI as PCM data.  If any other encoding is used over
    *          the air then PCM data is transported over HCI regardless
    *          of the value of this parameter.
    */
typedef struct _tagSCO_Enhanced_Transport_Parameters_t
{
   Byte_t    DataPath;
   Boolean_t mSBCDataPathHCI;
   Byte_t    PCMSamplePayloadMSBPosition;
   Byte_t    TransportUnitSize;
} SCO_Enhanced_Transport_Parameters_t;

   /*! The following Constants represent the Minimum, Maximum and
    * Default Buffer Sizes that are supported for the SCO Transmit
    * Buffers.  These Constants can be used with the
    * SCO_Change_Buffer_Size() function.
    */
#define SCO_BUFFER_SIZE_MINIMUM                                 0
#define SCO_BUFFER_SIZE_MAXIMUM                                 (1024*1024)
#define SCO_BUFFER_SIZE_DEFAULT                                 (BTPS_CONFIGURATION_SCO_DEFAULT_BUFFER_SIZE)

   /*! The following enumerated type represents the supported SCO
    * Connection Modes supported by the SCO module.
    */
typedef enum
{
   scmDisableConnections,
   scmEnableConnections
} SCO_Connection_Mode_t;

#define SCO_CONNECTION_MODE_DEFAULT                             (BTPS_CONFIGURATION_SCO_DEFAULT_CONNECTION_MODE)

   /*! The following enumerated type represents the supported SCO
    * Transport Modes.  This is used to notify the SCO layer which
    * physical transport the SCO Data will occupy.  This type is used
    * with the SCO_Set_Physical_Transport() function.
    */
typedef enum
{
   sptCodec,
   sptHCI
} SCO_Physical_Transport_t;

   /*! The following constant represents the default SCO Physical
    * Transport Layer that the SCO layer will utilize (unless changed by
    * a call to the SCO_Set_Physical_Transport()) function.
    */
#define SCO_PHYSICAL_TRANSPORT_DEFAULT                          (BTPS_CONFIGURATION_SCO_DEFAULT_PHYSICAL_TRANSPORT)

   /*! The following Bit Definitions represent the allowable Flags for
    * an SCO_Purge_Buffer() operation.  Currently the Transmit Buffer
    * can only be specified for purging.
    */
#define SCO_PURGE_MASK_TRANSMIT_ABORT_BIT                       0x00000001

   /*! The following Type Declaration represents the Data that is
    * associated with the SCO Connection Request Callback.  The BD_ADDR
    * member represents the Bluetooth Board Address of the Bluetooth
    * Device Requesting the SCO Connection.  The Class_of_Device member
    * represents the Class of Device of the Bluetooth Device requesting
    * the SCO Connection.  The SCO_Connection_ID member is assigned by
    * the Bluetooth Protocol Stack, and can be used as the SCO
    * Connection ID for functions that require it.  If the caller wants
    * to accept or reject the SCO Connection then the caller *MUST* call
    * the SCO_Accept_Connection() function passing the SCO_Connection_ID
    * member of this structure.
    * \note The Connection can ONLY be accepted/rejected during the
    *            SCO Connection Request Callback.  If the
    *            SCO_Accept_Connection() function is not called during
    *            this callback then there is NO way to Accept/Reject the
    *            SCO Connection Request and the Connection Request will
    *            timeout (on the originators end).
    */
typedef struct _tagSCO_Connect_Request_Data_t
{
   BD_ADDR_t         BD_ADDR;
   Class_of_Device_t Class_of_Device;
   unsigned int      SCO_Connection_ID;
   SCO_Link_Type_t   LinkType;
} SCO_Connect_Request_Data_t;

#define SCO_CONNECT_REQUEST_DATA_SIZE                   (sizeof(SCO_Connect_Request_Data_t))

   /*! The following Type Declaration represents all available SCO
    * Event Types that are applicable to installed SCO Connection
    * Callbacks.  Each SCO Event will have a corresponding data
    * structure that will hold the applicable data for the specified
    * action.
    */
typedef enum
{
   etSCO_Connect_Result, /*!< Indicate that SCO connection result data is available. */
   etSCO_Disconnect, /*!< Indicate that SCO disconnection result data is available. */
   etSCO_Data_Indication, /*!< Indicate that SCO data is available. */
   etSCO_Transmit_Buffer_Empty_Indication, /*!< Indicate that the Transmit Queue is empty. */
   etSCO_Synchronous_Connection_Changed /*!< Indicate that the SCO connection has changed. */
} SCO_Event_Type_t;

   /*! The following Type Declaration represents the Data that is
    * associated with the etSCO_Connect_Result SCO Event.  The SCO
    * Connection ID represents the SCO Connection ID that the SCO
    * Connect Result Data is valid for.  The BD_ADDR member represents
    * the Bluetooth Board Address of the Bluetooth Device that is trying
    * to Connect to the Local Bluetooth Device OR that the Local
    * Bluetooth Device is trying to Connect with.  The Connection_Status
    * member represents the Status (zero for successful connection or
    * non-zero error value - defined in HCI Event Error Codes).  The
    * SCO_Connection_ID member represents the SCO Connection ID
    * associated with the connection and is used with SCO API functions
    * that require a Connection ID.
    */
typedef struct _tagSCO_Connect_Result_Event_t
{
   unsigned int    SCO_Connection_ID;
   BD_ADDR_t       BD_ADDR;
   unsigned int    Connection_Status;
   SCO_Link_Type_t LinkType;
   Byte_t          Transmission_Interval;
   Byte_t          Retransmission_Window;
   Word_t          Rx_Packet_Length;
   Word_t          Tx_Packet_Length;
   Byte_t          Air_Mode;
   Word_t          Connection_Handle;
} SCO_Connect_Result_Event_t;

#define SCO_CONNECT_RESULT_EVENT_SIZE                   (sizeof(SCO_Connect_Result_Event_t))

   /*! The following Type Declaration represents the Data that is
    * associated with the etSCO_Disconnect SCO Event.  The SCO
    * Connection ID represents the SCO Connection ID that the SCO
    * Disconnect Event is valid for.  The BD_ADDR member represents the
    * Bluetooth Board Address of the Bluetooth Device that was
    * disconnected (specified SCO Link ONLY).  The SCO Disconnection
    * Status member represents the Status (defined in HCI Event Error
    * Codes) of the Disconnection, and the SCO_Connection_ID member
    * represents the SCO Connection ID of the SCO Connection that has
    * been terminated.
    */
typedef struct _tagSCO_Disconnect_Event_t
{
   unsigned int SCO_Connection_ID;
   BD_ADDR_t    BD_ADDR;
   unsigned int Disconnection_Status;
} SCO_Disconnect_Event_t;

#define SCO_DISCONNECT_EVENT_SIZE                       (sizeof(SCO_Disconnect_Event_t))

   /*! The following Type Declaration represents the Data that is
    * associated with the etSCO_Data_Indication SCO Event.  The BD_ADDR
    * member represents the Bluetooth Board Address of the Bluetooth
    * Device that has received the data (specified SCO Link ONLY).  The
    * Data Length member represents the size of the DataBuffer contained
    * as the next parameter.  The DataBuffer member is a pointer to the
    * actual data.  @param PARAMFINAL The final parameter, PacketStatus defines the
    * status (as reported by the baseband) of the packet.  Valid values
    * are one of the following:
    *
    *    HCI_SCO_FLAGS_PACKET_STATUS_MASK_CORRECTLY_RECEIVED_DATA
    *    HCI_SCO_FLAGS_PACKET_STATUS_MASK_POSSIBLY_INVALID_DATA
    *    HCI_SCO_FLAGS_PACKET_STATUS_MASK_NO_DATA_RECEIVED
    *    HCI_SCO_FLAGS_PACKET_STATUS_MASK_DATA_PARTIALLY_LOST
    *
    * See the Bluetooth HCI specification for more information on these
    * flags (Bluetooth 2.1 + EDR specification or greater).
    */
typedef struct _tagSCO_Data_Indication_Event_t
{
   unsigned int  SCO_Connection_ID;
   BD_ADDR_t     BD_ADDR;
   Byte_t        DataLength;
   Byte_t       *DataBuffer;
   Word_t        PacketStatus;
} SCO_Data_Indication_Event_t;

#define SCO_DATA_INDICATION_EVENT_SIZE                  (sizeof(SCO_Data_Indication_Event_t))

   /*! The following type Declaration represents the Data that is
    * associated with the SCO Transmit Data Empty Event.  This Event is
    * dispatched when the Transmit Queue is empty, but ONLY after the
    * Transmit Queue was COMPLETELY full.  See the SCO_Queue_Data() or
    * SCO_Send_Data() API (only 1 should be used by the application)
    * function for more information.
    */
typedef struct _tagSCO_Transmit_Buffer_Empty_Event_t
{
   unsigned int SCO_Connection_ID;
   BD_ADDR_t    BD_ADDR;
} SCO_Transmit_Buffer_Empty_Event_t;

#define SCO_TRANSMIT_BUFFER_EMPTY_EVENT_SIZE            (sizeof(SCO_Transmit_Buffer_Empty_Event_t))

   /*! The following Type Declaration represents the Data that is
    * associated with the etSCO_Synchronous_Connection_Changed Event.
    * The SCO Connection ID represents the SCO Connection ID that the
    * SCO Changed Event is valid for.  The Status member represents the
    * Status (defined in HCI Event Error Codes) of the Change, and the
    * Transmission_Interval, Retransmission_Window, Rx_Packet_Length,
    * Tx_Packet_Length represents SCO parameters of the Connection that
    * has been changed.
    */
typedef struct _tagSCO_Synchronous_Connection_Changed_Event_t
{
   unsigned int SCO_Connection_ID;
   Byte_t       Status;
   Byte_t       Transmission_Interval;
   Byte_t       Retransmission_Window;
   Word_t       Rx_Packet_Length;
   Word_t       Tx_Packet_Length;
} SCO_Synchronous_Connection_Changed_Event_t;

#define SCO_SYNCHRONOUS_CONNECTION_CHANGED_EVENT_SIZE   (sizeof(SCO_Synchronous_Connection_Changed_Event_t))

   /*! The following Type Declaration represents the Data that is
    * associated with all SCO Events that are defined for all SCO
    * Connection Callbacks.  This Data Structure encapsulates all
    * possible SCO Events, such that a single SCO Callback function
    * needs to be installed to monitor SCO Connections.  This data
    * structure is passed to the caller when any registered SCO
    * Callbacks are called.  The SCO_Event_Type member allows the
    * programmer a means to determine the correct Data structure member
    * to use when this Data Structure is received.
    */
typedef struct _tagSCO_Event_Data_t
{
   SCO_Event_Type_t SCO_Event_Type;
   Word_t           SCO_Event_Data_Size;
   union
   {
      SCO_Connect_Result_Event_t                 *SCO_Connect_Result_Event;
      SCO_Disconnect_Event_t                     *SCO_Disconnect_Event;
      SCO_Data_Indication_Event_t                *SCO_Data_Indication_Event;
      SCO_Transmit_Buffer_Empty_Event_t          *SCO_Transmit_Buffer_Empty_Event;
      SCO_Synchronous_Connection_Changed_Event_t *SCO_Synchronous_Connection_Changed_Event;
   } SCO_Event_Data;
} SCO_Event_Data_t;

#define SCO_EVENT_DATA_SIZE                             (sizeof(SCO_Event_Data_t))

   /*! The following declared type represents the Prototype Function for
    * a SCO Connection Request Callback. This function will be called
    * whenever a SCO Connection Request occurs within the Bluetooth
    * Protocol Stack that is specified with the specified Bluetooth
    * Stack ID.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param SCO_Connect_Request_Data  Data associated with this connection request.
    * @param CallbackParameter User-defined parameter (e.g., tag value) that
    * was defined in the callback registration.
    * The caller is free to use the contents of the SCO Connection Request Data
    * ONLY in the context of this callback.  If the caller requires the
    * Data for a longer period of time, then the callback function MUST
    * copy the data into another Data Buffer(s).  This function is
    * guaranteed NOT to be invoked more than once simultaneously for
    * the specified installed callback (i.e. this  function DOES NOT
    * have be reentrant).  It Needs to be noted however, that if the
    * same Callback is installed more than once, then the callbacks
    * will be called serially.  Because of this, the processing in this
    * function should be as efficient as possible.  It should also be
    * noted that this function is called in the Thread Context of a
    * Thread that the User does NOT own.  Therefore, processing in this
    * function should be as efficient as possible (this argument holds
    * anyway because another SCO Event will not be processed while this
    * function call is outstanding).
    * \note A Connection can ONLY be accepted/rejected in the
    *            context of this callback function.  If the
    *            SCO_Accept_Connection() function is NOT called during
    *            this callback (to accept or reject the connection) then
    *            there is NO way to Accept/Reject the SCO Connection
    *            Request, and the SCO Connection Request will timeout
    *            on the originators end (fail).
    * \note This function MUST NOT Block and wait for events that
    *            can only be satisfied by Receiving other Bluetooth
    *            Stack Events.  A Deadlock WILL occur because other
    *            Callbacks might not be issued while this function is
    *            currently outstanding.
    */
typedef void (BTPSAPI *SCO_Connect_Request_Callback_t)(unsigned int BluetoothStackID, SCO_Connect_Request_Data_t *SCO_Connect_Request_Data, unsigned long CallbackParameter);

   /*! The following declared type represents the Prototype Function for
    * a SCO Connection Callback.  This function will be called whenever
    * a defined SCO Connection Event occurs for the specified SCO
    * Connection within the Bluetooth Protocol Stack that is specified
    * with the specified Bluetooth Stack ID.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param SCO_Event_Data Event associated with this SCO Connection.
    * @param CallbackParameter User-defined parameter (e.g., tag value)
    * that was defined in the callback registration.
    * The caller is free to use the contents of the SCO Event Data ONLY in the
    * context of this callback.  If the caller requires the Data for a
    * longer period of time, then the callback function MUST copy the
    * into another Data Buffer(s).  This function is guaranteed NOT to
    * be invoked more than once simultaneously for the specified
    * installed callback (i.e. this  function DOES NOT have be
    * reentrant).  It Needs to be noted however, that if the same
    * Callback is installed more than once, then the callbacks will be
    * called serially.  Because of this, the processing in this function
    * should be as efficient as possible.  It should also be noted that
    * this function is called in the Thread Context of a Thread that
    * the User does NOT own.  Therefore, processing in this function
    * should be as efficient as possible (this argument holds anyway
    * because another SCO Event will not be processed while this
    * function call is outstanding).
    * \note This function MUST NOT Block and wait for events that
    *            can only be satisfied by Receiving other Bluetooth
    *            Stack Events.  A Deadlock WILL occur because other
    *            Callbacks might not be issued while this function is
    *            currently outstanding.
    */
typedef void (BTPSAPI *SCO_Connection_Callback_t)(unsigned int BluetoothStackID, SCO_Event_Data_t *SCO_Event_Data, unsigned long CallbackParameter);

   /*! @brief The following function is responsible for registering a eSCO and
    * SCO Connection Request Callback with the specified Bluetooth
    * Protocol Stack (specified via the BluetoothStackID parameter).
    * Once this Callback is installed, the caller will be notified of
    * Asynchronous eSCO and SCO Connection Requests when they occur.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param SCO_Connect_Request_Callback Callback function that is to be registered.
    * A Connection can ONLY be accepted/rejected in the context of this callback
    * function  This function MUST NOT Block and wait for events that  can only be
    * satisfied by Receiving other Bluetooth Stack Events.  A Deadlock WILL occur because
    * other Callbacks might not be issued while this function is currently outstanding.
    * @param CallbackParameter User defined parameter that will be passed to the callback
    * function when invoked.
    * @return This function returns the SCOCallbackID which is a positive,
    * non-zero, return value if the specified Callback Function was
    * installed, or a negative error code if the specified Callback
    * Function was unable to be installed.  If a Callback was able to be
    * installed successfully, the caller can remove the eSCO and SCO
    * Connection Request Callback from the Bluetooth Protocol Stack by
    * calling the SCO_Un_Register_Callback() function and passing the
    * return value from this function as the SCOCallbackID parameter.
    */
BTPSAPI_DECLARATION int BTPSAPI SCO_Register_Synchronous_Connect_Request_Callback(unsigned int BluetoothStackID, SCO_Connect_Request_Callback_t SCO_Connect_Request_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SCO_Register_Synchronous_Connect_Request_Callback_t)(unsigned int BluetoothStackID, SCO_Connect_Request_Callback_t SCO_Connect_Request_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is responsible for registering ONLY SCO
    * Connection Request Callback with the specified Bluetooth Protocol
    * Stack (specified via the BluetoothStackID parameter).  Once this
    * Callback is installed, the caller will be notified of Asynchronous
    * SCO Connection Requests when they occur.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param SCO_Connect_Request_Callback User-supplied callback function.
    * @param CallbackParameter A user-defined parameter (e.g., a tag value)
    * that will be passed back to the user in the callback function with each packet.
    * @return This function returns
    * the SCOCallbackID which is a positive, non-zero, return value if
    * the specified Callback Function was installed, or a negative error
    * code if the specified Callback Function was unable to be
    * installed.  If a Callback was able to be installed successfully,
    * the caller can remove the SCO Connection Request Callback from the
    * Bluetooth Protocol Stack by calling the SCO_Un_Register_Callback()
    * function and passing the return value from this function as the
    * SCOCallbackID parameter.
    * \note This function is deprecated, as it only supports SCO
    *            connections, use
    *            SCO_Register_Synchronous_Connect_Request_Callback for
    *            eSCO and SCO connection callbacks.
    */
BTPSAPI_DECLARATION int BTPSAPI SCO_Register_Connect_Request_Callback(unsigned int BluetoothStackID, SCO_Connect_Request_Callback_t SCO_Connect_Request_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SCO_Register_Connect_Request_Callback_t)(unsigned int BluetoothStackID, SCO_Connect_Request_Callback_t SCO_Connect_Request_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is responsible for removing a previously
    * installed SCO Callback for the Bluetooth Protocol Stack specified
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param SCOCallbackID Identifier returned from a successful callback
    * registration.
    * @return This function returns zero if the specified callback was removed, or a
    * negative return value if the callback was unable to be removed.
    */
BTPSAPI_DECLARATION int BTPSAPI SCO_Un_Register_Callback(unsigned int BluetoothStackID, unsigned int SCOCallbackID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SCO_Un_Register_Callback_t)(unsigned int BluetoothStackID, unsigned int SCOCallbackID);
#endif

   /*! @brief The following function is responsible for adding an eSCO and SCO
    * Connection to the specified Bluetooth Board Address (for the
    * specified Bluetooth Device).
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param BD_ADDR Bluetooth device address of the remote device to setup SCO/eSCO
    * connection to.
    * @param SynchronousConnectionInfo The connection parameters for the connection.
    * To use defaults this parameter may be set to NULL.
    * @param SCO_Connection_Callback Callback function to be installed for this connection.
    * This is called when a SCO/eSCO event occurs on the specified SCO/eSCO connection.
    * @param CallbackParameter Parameter that is passed to the callback function when a
    * SCO/eSCO event occurs.
    * @return If this function returns a negative
    * value then a SCO/eSCO Connection was NOT able to be established
    * with the specified Bluetooth Device (based upon the BD_ADDR_t
    * specified.  The caller can close the SCO/eSCO Connection when
    * finished by passing the return value from this function (if
    * successful) to the SCO_Close_Connection() function.
    * If this function returns a positive value, then a SCO/eSCO Connection has
    * been requested with the specified Board Address.  Note that this
    * does NOT mean that the SCO/eSCO Connection has been established,
    * this information is returned in the specified Connection Callback
    * with the Connection Result.
    * \note If the SCO_Set_Enhanced_Transport_Parameters() is called
    *          before this function then then the
    *          SCO_Data_Encoding_Type, SCO_PCM_Data_Sample_Size, and
    *          SCO_PCM_Data_Sample_Size members of
    *          SynchronousConnectionInfo->DataFormat will be ignored and
    *          the parameters specified in
    *          SCO_Set_Enhanced_Transport_Parameters() will be used
    *          instead.
    */
BTPSAPI_DECLARATION int BTPSAPI SCO_Setup_Synchronous_Connection(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, SCO_Synchronous_Connection_Info_t *SynchronousConnectionInfo, SCO_Connection_Callback_t SCO_Connection_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SCO_Setup_Synchronous_Connection_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, SCO_Synchronous_Connection_Info_t *SynchronousConnectionInfo, SCO_Connection_Callback_t SCO_Connection_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is responsible for adding an SCO Connection
    * to the specified Bluetooth Board Address (for the specified
    * Bluetooth Device).
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param BD_ADDR Address of the Bluetooth device to make the connection to.
    * @param SCO_Connection_Callback Function to call to report connection status/actions to.
    * @param CallbackParameter A user-defined parameter (e.g., a tag value)
    * that will be passed back to the user in the callback function with each packet.
    * @return If this function returns a negative value then a SCO Connection was NOT
    * able to be established with the specified Bluetooth Device (based
    * upon the BD_ADDR_t specified.  The caller can close the SCO
    * Connection when finished by passing the return value from this
    * function (if successful) to the SCO_Close_Connection() function.
    * If this function returns a positive value, then a SCO Connection
    * has been requested with the specified Board Address.  Note that
    * this does NOT mean that the SCO Connection has been established,
    * this information is returned in the specified SCO Connection
    * Callback with the Connection Result.
    * \note This function is deprecated, as it only supports SCO
    *            connections, use SCO_Setup_Synchronous_Connection for
    *            eSCO and SCO connection.
    */
BTPSAPI_DECLARATION int BTPSAPI SCO_Add_Connection(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, SCO_Connection_Callback_t SCO_Connection_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SCO_Add_Connection_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, SCO_Connection_Callback_t SCO_Connection_Callback, unsigned long CallbackParameter);
#endif

   /*! This function is responsible for Closing a SCO/eSCO Connection */
    /*! @brief Link that was established via successful call to the
    * SCO_Add_Connection() or SCO_Setup_Synchronous_Connection()
    * function.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param SCOConnectionID The identifier for this connection which was
    * returned from a successful call to SCO_Add_Connection.
    * @param Disconnect_Status The reason for the disconnection, which is
    * one of the HCI Error Codes.
    * @return This function returns a zero if successful, or a negative return
    * code if unsuccessful.  Once this function is called for the
    * specified SCO/eSCO Connection ID, that SCO/eSCO Connection is NO
    * Longer valid (if established) and the SCO/eSCO Connection Callback
    * that was registered with the Connection will NO Longer be called.
    */
BTPSAPI_DECLARATION int BTPSAPI SCO_Close_Connection(unsigned int BluetoothStackID, unsigned int SCOConnectionID, unsigned int Disconnect_Status);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SCO_Close_Connection_t)(unsigned int BluetoothStackID, unsigned int SCOConnectionID, unsigned int Disconnect_Status);
#endif

   /*! @brief The following function is responsible for Accepting or Rejecting a
    * SCO/eSCO Connection Request.  This function CAN ONLY be called in
    * the Context of a SCO/eSCO Connection Request Callback.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param SCOConnectionID Obtained via the SCO_Connection_ID member of
    * the SCO_Connect_Request Data member of the SCO Connect Request Event Data.
    * This Data is specified in a SCO/eSCO Callback, so the caller will only be
    * able to issue this function if a SCO/eSCO Callback has been installed.
    * @param SynchronousConnectionInfo  Required parameters of the connection,
    * if set to NULL default values will be used.
    * @param RejectReason Specifies whether or not the caller has Accepted or
    * Rejected the SCO Connection Request.  If this parameter is zero, then the SCO
    * Request will be accepted, else this parameter represents the Rejection Reason
    * (defined in the  Bluetooth HCI specification Error Codes).
    * @param SCO_Connection_Callback Callback function that is to be installed
    * for the accepted SCO/eSCO connection. Ignored if the connection is being rejected,
    * MUST be valid if the connection is being accepted. This Callback Function
    * (and  specified Callback Parameter) will be used when any SCO/eSCO Event
    * occurs on the accepted SCO/eSCO Connection (if accepted).
    * @param CallbackParameter Parameter to the callback function. Will be ignored
    * if the connection is being reject, otherwise must be valid.
    * @return If this function returns zero, then the SCO/eSCO Connection has been
    * accepted or rejected.  Note that this does NOT mean that the
    * SCO/eSCO Connection has been established in the case of a SCO/eSCO
    * Accept.  This information is returned in specified SCO/eSCO
    * Connection Callback with the Connection Result.
    * \note This function can ONLY be called in the context of a
    *            SCO/eSCO Connection Request Callback.  If this function
    *            is called from any other context it will fail.  This
    *            means that a SCO/eSCO Connection can ONLY be
    *            accepted/rejected during a SCO/eSCO Connection Request
    *            Callback.
    * \note The Connection can ONLY be accepted/rejected during the
    *            SCO/eSCO Connection Request Callback.  If the
    *            SCO_Accept_Synchronous_Connection() function is not
    *            called during this callback then there is NO way to
    *            Accept/Reject the SCO/eSCO Connection Request and the
    *            Connection Request will timeout (on the originators
    *            end).
    */
BTPSAPI_DECLARATION int BTPSAPI SCO_Accept_Synchronous_Connection(unsigned int BluetoothStackID, unsigned int SCOConnectionID, SCO_Synchronous_Connection_Info_t *SynchronousConnectionInfo, unsigned int RejectReason, SCO_Connection_Callback_t SCO_Connection_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SCO_Accept_Synchronous_Connection_t)(unsigned int BluetoothStackID, unsigned int SCOConnectionID, SCO_Synchronous_Connection_Info_t *SynchronousConnectionInfo, unsigned int RejectReason, SCO_Connection_Callback_t SCO_Connection_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is responsible for Accepting or Rejecting a
    * SCO Connection Request.  This function CAN ONLY be called in the
    * Context of a SCO Connection Request Callback.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param SCOConnectionID The unique identifier for this SCO Connection. This is
    * provided to the SCO Connection Request Callback function.
    * @param RejectReason If the connection is being accepted, this parameter is set
    * to zero.  If the connection is being rejected, this parameter is set to one of
    * the HCI Error Codes.
    * @param SCO_Connection_Callback Function to call to report connection status/actions to.
    * @param CallbackParameter A user-defined parameter (e.g., a tag value) that will
    * be passed back to the user in the callback function with each packet.
    * @return If this function returns zero, then
    * the SCO Connection has been accepted or rejected.  Note that this
    * does NOT mean that the SCO Connection has been established in the
    * case of a SCO Accept.  This information is returned in specified
    * SCO Connection Callback with the Connection Result.
    * \note This function can ONLY be called in the context of a
    *            SCO Connection Request Callback.  If this function is
    *            called from any other context it will fail.  This means
    *            that a SCO Connection can ONLY be accepted/rejected
    *            during a SCO Connection Request Callback.
    * \note The Connection can ONLY be accepted/rejected during the
    *            SCO Connection Request Callback.  If the
    *            SCO_Accept_Connection() function is not called during
    *            this callback then there is NO way to Accept/Reject the
    *            SCO Connection Request and the Connection Request will
    *            timeout (on the originators end).
    * \note This function is deprecated, as it only supports SCO
    *            connections, use SCO_Accept_Synchronous_Connection for
    *            eSCO and SCO connection.
    */
BTPSAPI_DECLARATION int BTPSAPI SCO_Accept_Connection(unsigned int BluetoothStackID, unsigned int SCOConnectionID, unsigned int RejectReason, SCO_Connection_Callback_t SCO_Connection_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SCO_Accept_Connection_t)(unsigned int BluetoothStackID, unsigned int SCOConnectionID, unsigned int RejectReason, SCO_Connection_Callback_t SCO_Connection_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is used to modify an existing synchronous
    * connection.

    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param SCOConnectionID Connection ID obtained via a successful call
    * to SCO_Setup_Synchronous_Connection().
    * @param MaxLatency The value in milliseconds representing the upper
    * limit of the sum of the synchronous interval.
    * @param RetransmissionEffort The ReTransmissionEffort modes for a eSCO connection.
    * @return This function will return a non-zero,
    * positive value on success of sending the command to the device.
    * The actual success of failure for changing the connection will be
    * in the status of etSynchronous_Connection_Changed_Event returned
    * from the SCO_Connection_Callback passed in during
    * SCO_Setup_Synchronous_Connection().
    * \note Only eSCO connections can be modified
    */
BTPSAPI_DECLARATION int BTPSAPI SCO_Modify_Synchronous_Connection(unsigned int BluetoothStackID, unsigned int SCOConnectionID, Word_t MaxLatency, SCO_Retransmission_Effort_t RetransmissionEffort);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_Modify_Synchronous_Connection_t)(unsigned int BluetoothStackID, unsigned int SCOConnectionID, Word_t MaxLatency, SCO_Retransmission_Effort_t RetransmissionEffort);
#endif

   /*! @brief The following function is responsible for sending SCO/eSCO Data to
    * the specified SCO/eSCO Connection.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param SCOConnectionID The unique identifier for this SCO Connection.
    * This is provided to the SCO Connection Request Callback function.
    * @param SCODataLength Length of the Data referenece by SCOData.
    * @param SCOData Pointer to the data to send.
    * @return This function returns zero if the SCO/eSCO Data
    * was successfully sent, or a negative return error code if the
    * SCO/eSCO Data was not able to be sent.
    * \note If this function returns the Error Code:
    *          BTPS_ERROR_INSUFFICIENT_BUFFER_SPACE then this is a
    *          signal to the caller that the requested data could NOT be
    *          sent because the requested data could not be queued in
    *          the Outgoing SCO Queue.  The caller then, must wait for
    *          the etSCO_Transmit_Buffer_Empty_Indication Event before
    *          trying to send any more data.
    */
BTPSAPI_DECLARATION int BTPSAPI SCO_Send_Data(unsigned int BluetoothStackID, unsigned int SCOConnectionID, Byte_t SCODataLength, Byte_t *SCOData);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SCO_Send_Data_t)(unsigned int BluetoothStackID, unsigned int SCOConnectionID, Byte_t SCODataLength, Byte_t *SCOData);
#endif

   /*! @brief The following function is responsible for changing the current SCO
    * Queuing Threshold information for the Bluetooth Stack L2CAP
    * Module.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param QueueingParameters Pointer to structure that contains the queue
    * threshold information to set.
    * @return This function returns zero if the information
    * was successfully changed, or a negative return error code if there
    * was an error.
    * \note The QueueLimit member of the SCO_Queueing_Parameters_t
    *          must be less than the MaximumOutstandingSCOPackets member
    *          of the SCO_Packet_Information_t that is returned by the
    *          SCO_Query_Packet_Information() API.
    */
BTPSAPI_DECLARATION int BTPSAPI SCO_Set_Queue_Threshold(unsigned int BluetoothStackID, SCO_Queueing_Parameters_t *QueueingParameters);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SCO_Set_Queue_Threshold_t)(unsigned int BluetoothStackID, SCO_Queueing_Parameters_t *QueueingParameters);
#endif

   /*! @brief The following function is responsible for getting the current SCO
    * Queuing Threshold information for the Bluetooth Stack L2CAP
    * Module.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param QueueingParameters Pointer to structure to return the current
    * SCO queue threshold information.
    * @return This function returns
    * zero if successful, or a negative return error code if there was
    * an error.
    */
BTPSAPI_DECLARATION int BTPSAPI SCO_Get_Queue_Threshold(unsigned int BluetoothStackID, SCO_Queueing_Parameters_t *QueueingParameters);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SCO_Get_Queue_Threshold_t)(unsigned int BluetoothStackID, SCO_Queueing_Parameters_t *QueueingParameters);
#endif

   /*! @brief The following function is provided to allow a mechanism for
    * determining the Maximum SCO/eSCO Buffer sizes that are supported
    * by the Bluetooth Device that is associated with the Bluetooth
    * Stack ID that is passed to this function.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param SCO_Packet_Information Pointer to buffer that is to receive the
    * current SCO Packet information.
    * @return This function returns zero if successful or a
    * negative return error code if there was an error.
    * \note This information is returned straight from the Bluetooth
    *          Module and is only provided so that SCO/eSCO Applications
    *          can be written without having to sift through HCI API
    *          functions to determine the required information.
    * \note The information returned from this function is valid for
    *          EVERY SCO Channel that is active.  Note that the maximum
    *          number of outstanding SCO/eSCO Packets represents the
    *          maximum number of outstanding SCO/eSCO Packets per
    *          individual SCO/eSCO Channel, NOT all SCO/eSCO Channels.
    */
BTPSAPI_DECLARATION int BTPSAPI SCO_Query_Packet_Information(unsigned int BluetoothStackID, SCO_Packet_Information_t *SCO_Packet_Information);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SCO_Query_Packet_Information_t)(unsigned int BluetoothStackID, SCO_Packet_Information_t *SCO_Packet_Information);
#endif

   /*! @brief The following function is provided to allow a mechanism for
    * determining the current SCO/eSCO Data Encoding Format Information
    * for the Bluetooth Device that is associated with the specified
    * Bluetooth Stack ID.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param SCO_Data_Format Pointer to buffer that is to receive the current
    * SCO data format information.
    * @return This function returns zero if successful, or
    * a negative return error code if there was an error.
    * \note This information is applicable to ALL SCO/eSCO Channels,
    *          and NOT individual SCO Channels.  Settings for individual
    *          SCO Channels is not supported by the current Bluetooth
    *          specification.
    * \note There is no defined standard for the default SCO/eSCO
    *          Data Format.  For this reason the current settings can be
    *          queried and/or changed if a specific setting is required.
    */
BTPSAPI_DECLARATION int BTPSAPI SCO_Query_Data_Format(unsigned int BluetoothStackID, SCO_Data_Format_t *SCO_Data_Format);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SCO_Query_Data_Format_t)(unsigned int BluetoothStackID, SCO_Data_Format_t *SCO_Data_Format);
#endif

   /*! @brief The following function is provided to allow a mechanism for
    * changing the current SCO Data Encoding Format Information for the
    * Bluetooth Device that is associated with the specified Bluetooth
    * Stack ID.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param SCO_Data_Format Pointer to buffer that specifies the new SCO data
    * format information.
    * @return This function returns zero if successful, or a
    * negative return error code if there was an error.
    * \note This information is applicable to ALL SCO Channels, and
    *          NOT individual SCO Channels.  Settings for individual SCO
    *          Channels is not supported by the current Bluetooth
    *          specification.
    * \note There is no defined standard for the default SCO Data
    *          Format.  For this reason the current settings can be
    *          queried and/or changed if a specific setting is required.
    */
BTPSAPI_DECLARATION int BTPSAPI SCO_Change_Data_Format(unsigned int BluetoothStackID, SCO_Data_Format_t *SCO_Data_Format);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SCO_Change_Data_Format_t)(unsigned int BluetoothStackID, SCO_Data_Format_t *SCO_Data_Format);
#endif

   /*! @brief The following function is provided to allow the programmer a means
    * to change the default Transmit Buffer Size.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param SCOConnectionID The unique identifier for this SCO Connection.
    * This is provided to the SCO Connection Request Callback function.
    * @param TransmitBufferSize Size (in bytes) to change the SCO Output
    * Buffer (Queue) size to.
    * @return This function returns zero if the specified Buffer Size was changed, or a
    * negative return error code if there was an error.
    * \note This function causes ALL Data in the Transmit Buffer to
    *          be lost.  This function clears the Data Buffer so that
    *          all the available data buffer is available to be used.
    */
BTPSAPI_DECLARATION int BTPSAPI SCO_Change_Buffer_Size(unsigned int BluetoothStackID, unsigned int SCOConnectionID, unsigned int TransmitBufferSize);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SCO_Change_Buffer_Size_t)(unsigned int BluetoothStackID, unsigned int SCOConnectionID, unsigned int TransmitBufferSize);
#endif

   /*! @brief The following function exists to allow the user a mechanism for
    * aborting ALL Data present in an Output Buffer.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param SCOConnectionID The unique identifier for this SCO Connection.
    * This is provided to the SCO Connection Request Callback function.
    * @param PurgeBufferMask Mechanism with which to flush the Output buffer.
    * Currently the following values are supported:
    * SCO_PURGE_MASK_TRANSMIT_ABORT_BIT
    * @return This function returns zero if successful, or a negative return error code if
    * unsuccessful.
    */
BTPSAPI_DECLARATION int BTPSAPI SCO_Purge_Buffer(unsigned int BluetoothStackID, unsigned int SCOConnectionID, unsigned int PurgeBufferMask);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SCO_Purge_Buffer_t)(unsigned int BluetoothStackID, unsigned int SCOConnectionID, unsigned int PurgeBufferMask);
#endif

   /*! @brief The following function is responsible for queuing SCO Data to send
    * to the specified SCO/eSCO Connection.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param SCOConnectionID The unique identifier for this SCO Connection.
    * This is provided to the SCO Connection Request Callback function.
    * @param SCODataLength The number of data bytes to queue
    * @param SCOData The data buffer that contains the data to queue
    * @return This function returns the number of bytes of
    * SCO/eSCO Data that was successfully queued (possibly zero), or a
    * negative return error code if there was an error.
    * \note If this function is unable to send all of the data that
    *          was specified (via the SCODataLength parameter), this
    *          function will return the number of bytes that were
    *          actually sent (zero or more, but less than the
    *          SCODataLength parameter value).  When this happens (and
    *          ONLY when this happens), the user can expect to be
    *          notified when the SCO/eSCO Connection is able to queue
    *          data again via the etSCO_Transmit_Buffer_Empty_Indication
    *          SCO/eSCO Event.  This will allow the user a mechanism to
    *          know when the Transmit Buffer is empty so that more data
    *          can be queued for sending.
    */
BTPSAPI_DECLARATION int BTPSAPI SCO_Queue_Data(unsigned int BluetoothStackID, unsigned int SCOConnectionID, unsigned int SCODataLength, Byte_t *SCOData);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SCO_Queue_Data_t)(unsigned int BluetoothStackID, unsigned int SCOConnectionID, unsigned int SCODataLength, Byte_t *SCOData);
#endif

   /*! @brief The following function is provided to allow a mechanism for
    * changing the Maximum SCO/eSCO Buffer sizes.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param SCO_Packet_Information Pointer to buffer that contains the new
    * SCO Packet information.
    * @return This function returns zero if successful or a
    * negative return error code if there was an error.
    * \note The information returned from this function is valid for
    *          EVERY SCO/eSCO Channel that is active.  Note that the
    *          maximum number of outstanding SCO/eSCO Packets represents
    *          the maximum number of outstanding SCO/eSCO Packets per
    *          individual SCO/eSCO Channel, NOT all SCO/eSCO Channels.
    * \note Special care should be used when calling this function
    *          since the default values that are typically used are
    *          determined by read the information directly from the
    *          Bluetooth Device.  This function exists as a work around
    *          for devices that return incorrect or invalid information
    *          to the HCI_Read_Buffer_Size() request.
    */
BTPSAPI_DECLARATION int BTPSAPI SCO_Change_Packet_Information(unsigned int BluetoothStackID, SCO_Packet_Information_t *SCO_Packet_Information);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SCO_Change_Packet_Information_t)(unsigned int BluetoothStackID, SCO_Packet_Information_t *SCO_Packet_Information);
#endif

   /*! @brief The following function is responsible for setting the SCO
    * Connection Mode.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param ConnectionMode The second parameter is the SCO Connection Mode to set.
    * Specifying scmDisableConnections as the Connection Mode shall
    * disconnect all currently on going connections and disallow all new connection requests.
    * @return This function returns zero if successful, or a negative
    * return error code if unsuccessful.
    * \note Specifying scmDisableConnections as the Connection Mode
    *            shall disconnect all currently on going connections and
    *            disallow all new connection requests.
    */
BTPSAPI_DECLARATION int BTPSAPI SCO_Set_Connection_Mode(unsigned int BluetoothStackID, SCO_Connection_Mode_t ConnectionMode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SCO_Set_Connection_Mode_t)(unsigned int BluetoothStackID, SCO_Connection_Mode_t ConnectionMode);
#endif

   /*! @brief The following function is responsible for instructing the SCO
    * module about the type of Physical Transport that will be used for
    * SCO Data.  There is no defined way in the Bluetooth specification
    * to determine this data as it depends on the physical Bluetooth
    * Hardware configuration.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param PhysicalTransport The Physical Transport value to set.
    * @return This function returns zero if successful, or a negative return error
    * code if unsuccessful.
    * \note The Physical Transport can ONLY be changed if there are
    *          NO active SCO connections.  Calling this function with
    *          active connections will cause the function to return an
    *          error code (and to not set the physical transport mode).
    */
BTPSAPI_DECLARATION int BTPSAPI SCO_Set_Physical_Transport(unsigned int BluetoothStackID, SCO_Physical_Transport_t PhysicalTransport);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SCO_Set_Physical_Transport_t)(unsigned int BluetoothStackID, SCO_Physical_Transport_t PhysicalTransport);
#endif

   /*! @brief The following function is responsible for configuring the enhanced
    * audio connection parameters.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param EnhancedTransportParameters The enhanced transport parameters to be use.
    * @return This function returns zero if successful,
    * or a negative return error code if unsuccessful.
    * \note The parameters can only be changed if there are no active
    *          SCO connections.  Calling this function with active
    *          connections will cause the function to return an error
    *          and the parameters will not be changed.
    */
BTPSAPI_DECLARATION int BTPSAPI SCO_Set_Enhanced_Transport_Parameters(unsigned int BluetoothStackID, SCO_Enhanced_Transport_Parameters_t *EnhancedTransportParameters);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SCO_Set_Enhanced_Transport_Parameters_t)(unsigned int BluetoothStackID, SCO_Enhanced_Transport_Parameters_t *EnhancedTransportParameters);
#endif

#endif
