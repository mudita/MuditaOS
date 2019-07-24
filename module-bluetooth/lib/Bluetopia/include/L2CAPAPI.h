/*
 * Copyright 2000 - 2014 Stonestreet One.
 * All Rights Reserved.
 * Author:  Tim Thomas
 *** MODIFICATION HISTORY ****************************************************
 *
 *   mm/dd/yy  F. Lastname    Description of Modification
 *   --------  -----------    ------------------------------------------------
 *   09/11/00  T. Thomas      Initial creation.
 *   09/18/08  T. Thomas      Updates for BT 2.1
 *   06/10/11  T. Thomas      Updates for BT 4.0.
 *   02/15/14  T. Cook        Updates for BT 4.1.
 ****************************************************************************
 */

/**
 *  @file L2CAPAPI.h
 *
 *  @brief Stonestreet One Bluetooth Stack L2CAP API Type Definitions,
 *      Constants, and Prototypes. To use any of the following API's,
 *      include the following declaration in your application.
 *
 *  @code
 *  #include "SS1BTPS.h"
 *  @endcode
 *  ============================================================================
 */
#ifndef __L2CAPAPIH__
#define __L2CAPAPIH__

#include "BTPSKRNL.h"           /*! BTPS Kernel Prototypes/Constants. */

#include "BTAPITyp.h"           /*! Bluetooth API Type Definitions. */
#include "L2CAPTyp.h"           /*! Bluetooth L2CAP Type Definitions/Constants. */

#include "BTPSCFG.h"            /*! BTPS Configuration Constants. */

#define L2CAP_MAXIMUM_SUPPORTED_STACK_MTU                          (BTPS_CONFIGURATION_L2CAP_MAXIMUM_SUPPORTED_STACK_MTU)

   /*! The following defines the amount of time in seconds that an L2CAP
    * signaling request can go unacknowledged before some action is
    * taken.
    */
#define L2CAP_RTX_TIMER_MINIMUM_VALUE                               1
#define L2CAP_RTX_TIMER_MAXIMUM_VALUE                              60
#define L2CAP_RTX_TIMER_DEFAULT_VALUE                              (BTPS_CONFIGURATION_L2CAP_DEFAULT_RTX_TIMER_TIMEOUT_S)

   /*! The following defines the amount of time in seconds that a Pending
    * L2CAP signaling request can go unacknowledged before some action
    * is taken.
    */
#define L2CAP_ERTX_TIMER_MINIMUM_VALUE                             60
#define L2CAP_ERTX_TIMER_MAXIMUM_VALUE                            300
#define L2CAP_ERTX_TIMER_DEFAULT_VALUE                             (BTPS_CONFIGURATION_L2CAP_DEFAULT_ERTX_TIMER_TIMEOUT_S)

   /*! The following defines the amount of time in seconds that a
    * connection is maintained with no active logical channels before
    * the connection is removed.
    */
#define L2CAP_IDLE_TIMER_MINIMUM_VALUE                              0
#define L2CAP_IDLE_TIMER_MAXIMUM_VALUE                             30
#define L2CAP_IDLE_TIMER_DEFAULT_VALUE                             (BTPS_CONFIGURATION_L2CAP_DEFAULT_IDLE_TIMER_TIMEOUT_S)

   /*! The following defines then amount of time in seconds that a
    * connection is allowed to be in the Config State.  If the CID can
    * not be negotiated in this amount of time, the channel is
    * disconnected.
    */
#define L2CAP_CONFIG_TIMER_MINIMUM_VALUE                            1
#define L2CAP_CONFIG_TIMER_MAXIMUM_VALUE                          120
#define L2CAP_CONFIG_TIMER_DEFAULT_VALUE                           (BTPS_CONFIGURATION_L2CAP_DEFAULT_CONFIG_TIMER_TIMEOUT_S)

   /*! The following defines the amount of time in seconds that a
    * multi-segmented L2CAP packet segment may be outstanding on the
    * receive end.  If the next segment is not received in the amount of
    * time specified, the partial received message will be discarded.
    */
#define L2CAP_RECEIVE_TIMER_MINIMUM_VALUE                           1
#define L2CAP_RECEIVE_TIMER_MAXIMUM_VALUE                          60
#define L2CAP_RECEIVE_TIMER_DEFAULT_VALUE                          (BTPS_CONFIGURATION_L2CAP_DEFAULT_RECEIVE_TIMER_TIMEOUT_S)

   /*! The following define declare the Bit Masks that are to be used
    * with the Option_Flags field of the Config Request Structure .
    */
#define L2CA_CONFIG_OPTION_FLAG_MTU                                (0x0001 <<  0)
#define L2CA_CONFIG_OPTION_FLAG_FLUSH_TIMEOUT                      (0x0001 <<  1)
#define L2CA_CONFIG_OPTION_FLAG_QOS                                (0x0001 <<  2)
#define L2CA_CONFIG_OPTION_FLAG_MODE_INFO                          (0x0001 <<  3)
#define L2CA_CONFIG_OPTION_FLAG_FCS_OPTION                         (0x0001 <<  4)
#define L2CA_CONFIG_OPTION_FLAG_EXTENDED_FLOW_SPEC                 (0x0001 <<  5)
#define L2CA_CONFIG_OPTION_FLAG_EXTENDED_WINDOW_SIZE               (0x0001 <<  6)
#define L2CA_CONFIG_OPTION_FLAG_CONTINUATION                       (0x0001 << 15)

#define L2CA_CONFIG_SUPPORTED_OPTIONS                              (L2CA_CONFIG_OPTION_FLAG_MTU | L2CA_CONFIG_OPTION_FLAG_FLUSH_TIMEOUT | L2CA_CONFIG_OPTION_FLAG_QOS | L2CA_CONFIG_OPTION_FLAG_MODE_INFO | L2CA_CONFIG_OPTION_FLAG_FCS_OPTION)

   /*! The following structure represents the structure of the L2CAP LE
    * Credit Based Channel Data Structure.
    * \note The Credit Threshold controls when credits will be
    *          granted back to the remote device.  When the number of
    *          credits that we MAY grant back to the remote device (i.e.
    *          the initial credit count minus the number of un-credited
    *          packets we have received) is greater than or equal to
    *          this number then we will grant credits back to the remote
    *          device (so a value of 2 says that we will send credits
    *          back to the remote device on every second packet).  Thus
    *          CreditThreshold MUST be less than or equal to
    *          InitialCredits.
    */
typedef struct _tagL2CA_LE_Channel_Parameters_t
{
   Word_t ChannelFlags;
   Word_t MaxSDUSize; /*!< The Max SDU Size denotes the maximum Service Data Unit that the local device is capable of receiving from the remote device. */
   Word_t MaxPDUSize; /*!< The Max PDU Size (MPS) denotes the maximum L2CAP Packet (LE-Frame) that the local device is capable of receiving from the remote device.  If the Max SDU Size is larger than the Max PDU Size then the SDU will be split into multiple LE-Frames before being sent to the remote device. */
   Word_t PDUQueueDepth; /*!< The PDUQueueLimit directly affects the throughput for this channel and for other channels.  With a large value for PDUQueueLimit the channel will have a larger share of the bandwidth at the expense of other channels. */
   Word_t MaximumCredits; /*!< The Maximum Credits denotes the maximum number of credits (in LE-Frames) to grant to the remote device.  The remote device will only be able to send this many LE-Frames (of at most MaxPDUSize bytes) without the local device granting credits back. */
} L2CA_LE_Channel_Parameters_t;

#define L2CA_LE_CHANNEL_PARAMETERS_SIZE                  (sizeof(L2CA_LE_Channel_Parameters_t))

   /*! The following flags define the valid flags that may be set in the
    * ChannelFlags member of the L2CA_LE_Channel_Parameters_t structure.
    */
#define L2CA_LE_CHANNEL_PARAMETER_FLAGS_MANUAL_CREDIT_MODE  0x0001

   /*! The following Type Declaration defines the L2CA Event Callback
    * Data Types.  These types are used with the L2CA_Event_Data_t
    * structure to determine the type of Data the L2CAP Event Callback
    * structure contains.
    */
typedef enum
{
   etConnect_Indication, /*!< Notify the host of a connection request from a remote device. */
   etConnect_Confirmation, /*!< Notify the host that a connection request has completed or is pending. */
   etConfig_Indication, /*!< Notify the host of a configuration request from a remote device. */
   etConfig_Confirmation, /*!< Notify the host that the configuration request has completed. */
   etDisconnect_Indication, /*!< Notify the host of a disconnection request from a remote device. */
   etDisconnect_Confirmation, /*!< Notify the host that the disconnection request has completed. */
   etTimeout_Indication, /*!< Notify the host that a response from a remote device has timed out. */
   etEcho_Confirmation, /*!< Notify the host that an L2CA Ping request has completed. */
   etInformation_Confirmation, /*!< Return the requested device information to the Host.*/
   etData_Indication, /*!< Notify the host of incoming L2CAP data. */
   etData_Error_Indication, /*!< Notify the host of incoming L2CAP data error. */
   etGroup_Data_Indication, /*!< Notify the host of incoming connectionless data. */
   etGroup_Member_Status, /*!< Notify the host that a member has been added to a group. */
   etChannel_Buffer_Empty_Indication, /*!< Notify the host that all buffered data has been sent to the device. */
   etConnection_Parameter_Update_Indication, /*!< Notify the host of a received connection parameter update request (fixed channel LE only). */
   etConnection_Parameter_Update_Confirmation, /*!< Notify the host of a received connection parameter update confirmation (fixed channel LE only). */
   etFixed_Channel_Connect_Indication, /*!< Notify the host that a fixed channel is now connected to a specific device (fixed channel only). */
   etFixed_Channel_Disconnect_Indication, /*!< Notify the host that a fixed channel is now disconnected from a specific device (fixed channel only). */
   etFixed_Channel_Data_Indication, /*!< Notify the host that data has been received on a fixed channel (fixed channel only). */
   etFixed_Channel_Buffer_Empty_Indication, /*!< Notify the host that all buffered data has been sent to the device for the specified fixed channel. */
   etLE_Connect_Indication, /*!< Notifies the application that a LE Connection Request has been made by a remote device. */
   etLE_Connect_Confirmation, /*!< Notifies the application that a LE Connect Request to a remote device has been responded to. */
   etLE_Disconnect_Indication, /*!< Notifies the application that the specified logical LE connection has disconnected. */
   etLE_Disconnect_Confirmation, /*!< Notifies the application that a LE disconnect request has been responded to. */
   etLE_Data_Indication, /*!< Notifies the application that data has been received on the specified logical LE connection. */
   etLE_Channel_Buffer_Empty_Indication /*!< Notifies the application that the specified logical LE channel’s outgoing queue may now have more data queued in it. */
} L2CA_Event_Type_t;

   /*! The following structure represents the structure of the L2CAP
    * Timers that user to control timeouts in the L2CAP layer.  This
    * structure is used when a L2CA_Set_Timer_Values of
    * L2CA_Get_Timer_Values function is called.  The timers that are
    * provided in this structure can be adjusted to provide appropriate
    * timing for the profile being implemented.  The timer values are
    * specified in seconds.  Timers RTXTimerVal and ERTXTimerVal are
    * defined in the L2CAP specifications.  Refer to the specification
    * for information on these timers.  The IdleTimerVal is added to
    * support the idea of Client and Server L2CAP connections.  L2CAP
    * connections are established by Clients to Servers.  At the time
    * that the ACL connection is to be terminated, the Client should be
    * the one to initiate the disconnection of the ACL link.  When an
    * L2CAP server denotes that no CIDs are open on an ACL link, a timer
    * of value IdleTimerVal is started to allow the Client time to
    * disconnect the ACL link.  If the Client fails to disconnect the
    * ACL link that the expiration of this timer, the server will then
    * perform the disconnection.  If this timer is set to a value of
    * Zero, then the Server will attempt to disconnect the ACL when the
    * last L2CAP channel is released.  The ConfigStateTimerVal is used
    * to control the amount of time that the stack is allowed to be in
    * the Config State.  If the configuration process is not complete at
    * the expiration of this timer, the connection will be terminated.
    * The ReceiveSegmentTimerVal is used to control the time that the
    * stack will wait for the next segmented data packet to arrive.  If
    * the stack is waiting on a continuation information during the
    * recombination of packets at the time this timer expires, the
    * collected data will be discarded and an Error Event will be
    * issues.  Refer to the defined constants for each timer for the
    * range of values that each timer may be set.
    */
typedef struct _tagL2CA_Timer_Values_t
{
   unsigned int RTXTimerVal;
   unsigned int ERTXTimerVal;
   unsigned int IdleTimerVal;
   unsigned int ConfigStateTimerVal;
   unsigned int ReceiveSegmentTimerVal;
} L2CA_Timer_Values_t;

#define L2CA_TIMER_VALUES_SIZE                          (sizeof(L2CA_Timer_Values_t))

   /*! The following enumerated type is used with the
    * L2CA_Get_Link_Connection_Configuration() and the
    * L2CA_Set_Link_Connection_Configuration() functions.  These types
    * dictate how L2CAP will Issue Connection Requests and Respond to
    * Connection Requests.  This functionality is NOT provided as part
    * of L2CAP in the specification and exists to allow Role Switching
    * to take place at the HCI Layer when L2CAP is controlling the HCI
    * Connection Establishment (which is the default).
    * The Connect Request Configuration Types control how L2CAP will
    * handle the Allow Role Switch parameter for the HCI Connection
    * when L2CAP physically issues the HCI_Connect_Request() function.
    * The Connect Response Configuration Types control how L2CAP will
    * handle responding to an HCI_Connect_Request_Event.  The Ignore
    * Request Response type should ONLY be used when some other entity
    * is watching Connection Requests and controlling them (some form
    * of connection manager).  In almost ALL circumstances, L2CAP can
    * handle all HCI ACL Connection Requests/Responses so it is not
    * envisioned that the Ignore Request will be used in practice.
    * \note All of the Connect Request/Response parameters below
    *          ONLY Apply to lower level HCI Role Switching parameters.
    *          These parameters are only needed in circumstances where
    *          the programmer needs Point to Multi-Point capability
    *          (either client or server).  The defaults for L2CAP are
    *          given by the constants:
    *             - L2CAP_DEFAULT_LINK_CONNECT_REQUEST_CONFIG
    *             - L2CAP_DEFAULT_LINK_CONNECT_RESPONSE_CONFIG
    *
    */
typedef enum
{
   cqNoRoleSwitch,
   cqAllowRoleSwitch
} L2CA_Link_Connect_Request_Config_t;

typedef enum
{
   csMaintainCurrentRole,
   csRequestRoleSwitch,
   csIgnoreConnectionRequest
} L2CA_Link_Connect_Response_Config_t;

   /*! The following structure represents the structure of the L2CAP
    * Connection Configuration Parameters.  This structure is used with
    * the L2CA_Get_Link_Connection_Configuration() and the
    * L2CA_Set_Link_Connection_Configuration() functions.
    */
typedef struct _tagL2CA_Link_Connect_Params_t
{
   L2CA_Link_Connect_Request_Config_t  L2CA_Link_Connect_Request_Config;
   L2CA_Link_Connect_Response_Config_t L2CA_Link_Connect_Response_Config;
} L2CA_Link_Connect_Params_t;

#define L2CA_LINK_CONNECT_PARAMS_SIZE                   (sizeof(L2CA_Link_Connect_Params_t))

   /*! The following constants represent the default Link Connect
    * Parameters that is used by L2CAP.
    */
#define L2CAP_DEFAULT_LINK_CONNECT_REQUEST_CONFIG       (BTPS_CONFIGURATION_L2CAP_DEFAULT_LINK_CONNECT_REQUEST_CONFIG)
#define L2CAP_DEFAULT_LINK_CONNECT_RESPONSE_CONFIG      (BTPS_CONFIGURATION_L2CAP_DEFAULT_LINK_CONNECT_RESPONSE_CONFIG)

   /*! The following structure and these flags are used to control that
    * handling of data packets at the L2CAP Layer.  The Flags parameter
    * specifies options that can be used when sending data.  The
    * QueueLimit value indicates the upper limit at which the channel is
    * considered full.  The LowThreshold is the limit at which the
    * channel is unlocked and data can once again be sent on the
    * channel.  The units of QueueLimit and LowThreshold will depend on
    * the Flags that are set in the Flags parameter.  If
    * L2CA_QUEUEING_FLAG_LIMIT_BY_PACKETS is indicated, then the units
    * are in Packets.  If L2CA_QUEUEING_FLAG_LIMIT_BY_SIZE is indicated
    * then the units are in Bytes.
    * \note The L2CA_QUEUEING_FLAG_DISCARD_OLDEST Flag is only valid
    *          when L2CA_QUEUEING_FLAG_LIMIT_BY_PACKETS is indicated.
    */
typedef struct _tagL2CA_Queueing_Parameters_t
{
   DWord_t Flags;
   DWord_t QueueLimit;
   DWord_t LowThreshold;
} L2CA_Queueing_Parameters_t;

#define L2CA_QUEUEING_PARAMETERS_SIZE                    (sizeof(L2CA_Queueing_Parameters_t))

   /*! The following defines the Bit Flags that are used with the Flags
    * parameter of the L2CA_Queueing_Parmeters_t structure.
    * \note L2CA_QUEUEING_FLAG_LIMIT_BY_SIZE is no longer supported
    *          but is left here for backwards compatibility.
    */
#define L2CA_QUEUEING_FLAG_LIMIT_BY_PACKETS              0x00000000L
#define L2CA_QUEUEING_FLAG_LIMIT_BY_SIZE                 0x00000001L
#define L2CA_QUEUEING_FLAG_DISCARD_OLDEST                0x00000002L

   /*! The following structure is used with the
    * L2CA_Get_Channel_Queue_Threshold() and the
    * L2CA_Set_Channel_Queue_Threshold() functions to change the L2CAP
    * Channel Queue Thresholds.  These functions allow the caller a
    * mechanism to artificially limit the amount of data that L2CAP
    * will buffer internally (per L2CAP Channel).  This mechanism can
    * be used to instruct L2CAP to return a Memory Allocation Error
    * when the specified Threshold is reached (as opposed to only
    * returning the Memory Allocation Error when all the memory is
    * exhausted).  See the L2CA_Get_Channel_Queue_Threshold() and the
    * L2CA_Set_Channel_Queue_Threshold() functions for more information.
    * \note This mechanism has been DEPRECATED AND IS NO LONGER
    *          SUPPORTED!
    * \note This functionality has been SUPERCEDED by the
    *          L2CA_Enhanced_Data_Write() function!
    */
typedef struct _tagL2CA_Channel_Queue_Threshold_t
{
   unsigned int SizeThreshold;
   unsigned int DepthThreshold;
   unsigned int LowQueueThreshold;
} L2CA_Channel_Queue_Threshold_t;

#define L2CA_CHANNEL_QUEUE_THRESHOLD_SIZE               (sizeof(L2CA_Channel_Queue_Threshold_t))

   /*! The following type represents the various Controller Types that
    * are supported.
    */
typedef enum
{
   ctBR_EDR,
   ctLE
} L2CA_Controller_Type_t;

   /*! The following enumerated type is used with the
    * L2CA_Get_Link_Connection_State() function and denotes various link
    * states that a Bluetooth device is currently operating (with
    * regards to the L2CAP framework).
    */
typedef enum
{
   lcsDisconnected,
   lcsConnecting,
   lcsConnected,
   lcsDisconnecting
} L2CA_Link_Connection_State_t;

   /*! The following structure represents the structure of the L2CAP
    * Connect Indication Event Data.  This structure is used when an
    * L2CAP Connect Request is received.  The Upper Layer is provided
    * the Connect Request information in the L2CA_Connect_Indication
    * Callback Function. The LCID is the Local CID that should be used
    * to reference this connection.  The Board address is provided as
    * Extra information about the device that is requesting the
    * connection.
    */
typedef struct _tagL2CA_Connect_Indication_t
{
  Word_t                        PSM;  /*!< Protocol/Service Multiplexer value to which this callback is to be registered. */
  Word_t                        LCID; /*!< Local channel identifier. */
  Byte_t                        Identifier; /*!< Requestor’s identifier used to match up responses */
  BD_ADDR_t                     BD_ADDR; /*!< Address of the Bluetooth device requesting the connection. */
  L2CAP_Extended_Feature_Mask_t ExtendedFeatures; /*!< The extended features of the device that is attempting to connect.
                                                       L2CAP_EXTENDED_FEATURE_FLOW_CONTROL_BIT_NUMBER
                                                       L2CAP_EXTENDED_FEATURE_RETRANSMIT_MODE_BIT_NUMBER
                                                       L2CAP_EXTENDED_FEATURE_BI_DIRECTIONAL_QOS_BIT_NUMBER
                                                       L2CAP_EXTENDED_FEATURE_ENHANCED_RETRANSMISSION_MODE_BIT_NUMBER
                                                       L2CAP_EXTENDED_FEATURE_STREAMING_MODE_BIT_NUMBER
                                                       L2CAP_EXTENDED_FEATURE_FCS_OPTION_BIT_NUMBER
                                                       L2CAP_EXTENDED_FEATURE_ENHANCED_FLOW_SPEC_BIT_NUMBER
                                                       L2CAP_EXTENDED_FEATURE_FIXED_CHANNELS_BIT_NUMBER
                                                       L2CAP_EXTENDED_FEATURE_EXTENDED_WINDOW_SIZE_BIT_NUMBER
                                                       L2CAP_EXTENDED_FEATURE_UNICAST_DATA_RECEPTION_BIT_NUMBER
                                                       L2CAP_EXTENDED_FEATURE_MASK_EXTENSION_BIT_NUMBER */

} L2CA_Connect_Indication_t;

#define L2CA_CONNECT_INDICATION_SIZE                    (sizeof(L2CA_Connect_Indication_t))

   /*! The following structure represents the structure of the L2CAP
    * Connect Confirmation Event Data. The LCID is the Local CID that
    * is used to reference this connection.
    * \note  This structure is also used as a Connection Pending.
    *           When supplying a connection Pending Indication the
    *           Result is used to denote the Pending Status.
    */
typedef struct _tagL2CA_Connect_Confirmation_t
{
  Word_t                        LCID; /*!< Local channel identifier. */
  Word_t                        Result; /*!< Result of the connection attempt. Possible values are:
                                             L2CAP_CONNECT_RESULT_CONNECTION_SUCCESSFUL
                                             L2CAP_CONNECT_RESULT_CONNECTION_PENDING
                                             L2CAP_CONNECT_RESULT_CONNECTION_REFUSED_PSM_NOT_REGISTERED
                                             L2CAP_CONNECT_RESULT_CONNECTION_REFUSED_SECURITY_RELATED
                                             L2CAP_CONNECT_RESULT_CONNECTION_TIMEOUT */
  Word_t                        Status;  /*!< If the Result indicates connection Pending, then this field contains the reason for the hold up.  Possible values are:
                                              L2CAP_CONNECT_STATUS_NO_FURTHER_INFORMATION
                                              L2CAP_CONNECT_STATUS_AUTHENTICATION_PENDING
                                              L2CAP_CONNECT_STATUS_AUTHORIZATION_PENDING */
  L2CAP_Extended_Feature_Mask_t ExtendedFeatures; /*!< The extended features of the device whose connection is pending. Access should be made using the following bit masks:
                                             L2CAP_EXTENDED_FEATURE_FLOW_CONTROL_BIT_NUMBER
                                             L2CAP_EXTENDED_FEATURE_RETRANSMIT_MODE_BIT_NUMBER
                                             L2CAP_EXTENDED_FEATURE_BI_DIRECTIONAL_QOS_BIT_NUMBER
                                             L2CAP_EXTENDED_FEATURE_ENHANCED_RETRANSMISSION_MODE_BIT_NUMBER
                                             L2CAP_EXTENDED_FEATURE_STREAMING_MODE_BIT_NUMBER
                                             L2CAP_EXTENDED_FEATURE_FCS_OPTION_BIT_NUMBER
                                             L2CAP_EXTENDED_FEATURE_ENHANCED_FLOW_SPEC_BIT_NUMBER
                                             L2CAP_EXTENDED_FEATURE_FIXED_CHANNELS_BIT_NUMBER
                                             L2CAP_EXTENDED_FEATURE_EXTENDED_WINDOW_SIZE_BIT_NUMBER
                                             L2CAP_EXTENDED_FEATURE_UNICAST_DATA_RECEPTION_BIT_NUMBER
                                             L2CAP_EXTENDED_FEATURE_MASK_EXTENSION_BIT_NUMBER */

} L2CA_Connect_Confirmation_t;

#define L2CA_CONNECT_CONFIRMATION_SIZE                  (sizeof(L2CA_Connect_Confirmation_t))

   /*! The following structure represents the L2CA QoS Flow Spec
    * parameters.
    */
typedef struct _tagL2CA_Flow_Spec_t
{
   Byte_t   Flags;
   Byte_t   ServiceType;
   DWord_t  TokenRate;
   DWord_t  TokenBucketSize;
   DWord_t  PeakBandwidth;
   DWord_t  Latency;
   DWord_t  DelayVariation;
} L2CA_Flow_Spec_t;

#define L2CA_FLOW_SPEC_SIZE                             (sizeof(L2CA_Flow_Spec_t))

   /*! The following structure represents the L2CA Retransmission and
    * Flow Control Mode parameters.
    */
typedef struct _tagL2CA_Mode_Info_t
{
   Byte_t Mode;
   Byte_t TxWindowSize;
   Byte_t MaxTransmitAttempts;
   Word_t RetransmissionTimeout;
   Word_t MonitorTimeout;
   Word_t MaxPDUSize;
} L2CA_Mode_Info_t;

#define L2CA_MODE_INFO_SIZE                             (sizeof(L2CA_Mode_Info_t))

   /*! The following structure represents the L2CA Extended Flow Spec
    * parameters.
    */
typedef struct _tagL2CA_Extended_Flow_Spec_t
{
   Byte_t  Identifier;
   Byte_t  ServiceType;
   Word_t  MaxSDU;
   DWord_t SDUInterArrivalTime;
   DWord_t AccessLatency;
   DWord_t FlushTimeout;
} L2CA_Extended_Flow_Spec_t;

#define L2CA_EXTENDED_FLOW_SPEC_SIZE                    (sizeof(L2CA_Extended_Flow_Spec_t))

   /*! The following structure represents the structure of the L2CAP
    * Config Request Data Structure.
    */
typedef struct _tagL2CA_Config_Request_t
{
  Word_t                    Option_Flags;
  Word_t                    InMTU;
  Word_t                    OutFlushTO;
  L2CA_Flow_Spec_t          OutFlow;
  L2CA_Mode_Info_t          ModeInfo;
  Byte_t                    FCS_Option;
  L2CA_Extended_Flow_Spec_t ExtendedFlowSpec;
  Word_t                    ExtendedWindowSize;
} L2CA_Config_Request_t;

#define L2CA_CONFIG_REQUEST_SIZE                        (sizeof(L2CA_Config_Request_t))

   /*! The following structure represents the structure of the L2CAP
    * Config Confirmation Event Data.
    */
typedef struct _tagL2CA_Config_Confirmation_t
{
  Word_t                    LCID; /*!< Local channel identifier. */
  Word_t                    Result; /*!< Outcome of the configuration operation.  Possible values are:
                                         L2CAP_CONFIGURE_RESPONSE_RESULT_SUCCESS
                                         L2CAP_CONFIGURE_RESPONSE_RESULT_FAILURE_UNACCEPTABLE_PARAMETERS
                                         L2CAP_CONFIGURE_RESPONSE_RESULT_FAILURE_REJECTED_NO_REASON
                                         L2CAP_CONFIGURE_RESPONSE_RESULT_FAILURE_UNKNOWN_OPTIONS
                                         L2CAP_CONFIGURE_RESPONSE_RESULT_TIMEOUT */
  Word_t                    Option_Flags; /*!< A bit list.  Possible bit values are:
                                               L2CA_CONFIG_OPTION_FLAG_MTU
                                               L2CA_CONFIG_OPTION_FLAG_FLUSH_TIMEOUT
                                               L2CA_CONFIG_OPTION_FLAG_QOS
                                               L2CA_CONFIG_OPTION_FLAG_MODE_INFO
                                               L2CA_CONFIG_OPTION_FLAG_FCS_OPTION
                                               L2CA_CONFIG_OPTION_FLAG_EXTENDED_FLOW_SPEC
                                               L2CA_CONFIG_OPTION_FLAG_EXTENDED_WINDOW_SIZE
                                               L2CA_CONFIG_OPTION_FLAG_CONTINUATION */
  Word_t                    InMTU; /*!< Maximum transmission unit that the remote unit will send across this channel (maybe less or equal to the InMTU input parameter). */
  Word_t                    OutFlushTO; /*!< Number of milliseconds before an L2CAP packet that cannot be acknowl-edged at the physical layer is dropped.  This value is indicates the actual value that will be used for outgoing packets and may be less than or equal to the OutFlushTO parameter given as input. */
  L2CA_Flow_Spec_t          OutFlow; /*!< Quality of service parameters dealing with the traffic characteristics of the agreed-upon outgoing data flow. */
  L2CA_Mode_Info_t          ModeInfo; /*!< Quality of service parameters dealing with the traffic characteristics of the agreed-upon outgoing data flow. */
  Byte_t                    FCS_Option; /*!< Specifies the requested operating FCS mode of the L2CAP channel. */
  L2CA_Extended_Flow_Spec_t ExtendedFlowSpec; /*!< Specifies the requested extended Flow Specification.   */
  Word_t                    ExtendedWindowSize; /*!< Specifies the requested extended window size (ERTM modes). */
} L2CA_Config_Confirmation_t;

#define L2CA_CONFIG_CONFIRMATION_SIZE                   (sizeof(L2CA_Config_Confirmation_t))

   /*! The following structure represents the structure of the L2CAP
    * Config Indication Event Data.
    */
typedef struct _tagL2CA_Config_Indication_t
{
  Word_t                    LCID; /*!< Local channel identifier. */
  Word_t                    Option_Flags; /*!< A bit list.  Possible bit values are:
                                               L2CA_CONFIG_OPTION_FLAG_MTU
                                               L2CA_CONFIG_OPTION_FLAG_FLUSH_TIMEOUT
                                               L2CA_CONFIG_OPTION_FLAG_QOS
                                               L2CA_CONFIG_OPTION_FLAG_MODE_INFO
                                               L2CA_CONFIG_OPTION_FLAG_FCS_OPTION
                                               L2CA_CONFIG_OPTION_FLAG_EXTENDED_FLOW_SPEC
                                               L2CA_CONFIG_OPTION_FLAG_EXTENDED_WINDOW_SIZE
                                               L2CA_CONFIG_OPTION_FLAG_CONTINUATION */
  Word_t                    OutMTU; /*!< Maximum transmission unit that the remote unit will send across this channel (maybe less or equal to the InMTU input parameter). */
  Word_t                    InFlushTO; /*!< Number of milliseconds before an L2CAP packet that cannot be acknowl-edged at the physical layer is dropped.  This value is indicates the actual value that will be used for outgoing packets and may be less than or equal to the OutFlushTO parameter given as input. */
  L2CA_Flow_Spec_t          InFlow; /*!< Quality of service parameters dealing with the traffic characteristics of the agreed-upon outgoing data flow.*/
  L2CA_Mode_Info_t          ModeInfo; /*!< Specifies the requested operating mode of the L2CAP channel. */
  Byte_t                    FCS_Option; /*!< Specifies the requested operating FCS mode of the L2CAP channel. */
  L2CA_Extended_Flow_Spec_t ExtendedFlowSpec; /*!< Specifies the requested extended Flow Specification. */
  Word_t                    ExtendedWindowSize; /*!< Specifies the requested extended window size (ERTM modes). */
} L2CA_Config_Indication_t;

#define L2CA_CONFIG_INDICATION_SIZE                     (sizeof(L2CA_Config_Indication_t))

   /*! The following structure represents the structure of the L2CAP
    * Config Response Event Data.
    */
typedef struct _tagL2CA_Config_Response_t
{
  Word_t                    Option_Flags; /*!< A bit list.  Possible bit values are:
                                               L2CA_CONFIG_OPTION_FLAG_MTU
                                               L2CA_CONFIG_OPTION_FLAG_FLUSH_TIMEOUT
                                               L2CA_CONFIG_OPTION_FLAG_QOS
                                               L2CA_CONFIG_OPTION_FLAG_MODE_INFO
                                               L2CA_CONFIG_OPTION_FLAG_FCS_OPTION
                                               L2CA_CONFIG_OPTION_FLAG_EXTENDED_FLOW_SPEC
                                               L2CA_CONFIG_OPTION_FLAG_EXTENDED_WINDOW_SIZE
                                               L2CA_CONFIG_OPTION_FLAG_CONTINUATION */
  Word_t                    OutMTU; /*!< Maximum transmission unit that the remote unit will send across this channel (maybe less or equal to the InMTU input parameter). */
  Word_t                    InFlushTO; /*!< Number of milliseconds before an L2CAP packet that cannot be acknowl-edged at the physical layer is dropped.  This value is indicates the actual value that will be used for outgoing packets and may be less than or equal to the OutFlushTO parameter given as input. */
  L2CA_Flow_Spec_t          InFlow; /*!< Quality of service parameters dealing with the traffic characteristics of the agreed-upon outgoing data flow.*/
  L2CA_Mode_Info_t          ModeInfo; /*!< Specifies the requested operating mode of the L2CAP channel. */
  Byte_t                    FCS_Option; /*!< Specifies the requested operating FCS mode of the L2CAP channel. */
  L2CA_Extended_Flow_Spec_t ExtendedFlowSpec; /*!< Specifies the requested extended Flow Specification. */
  Word_t                    ExtendedWindowSize; /*!< Specifies the requested extended window size (ERTM modes). */
} L2CA_Config_Response_t;

#define L2CA_CONFIG_RESPONSE_SIZE                       (sizeof(L2CA_Config_Response_t))

   /*! The following structure represents the structure of the L2CAP
    * Config Parameters.  This structure is used with the
    * L2CA_Get_Current_Channel_Configuration() function.
    */
typedef struct _tagL2CA_Config_Params_t
{
  Word_t           OutMTU;
  Word_t           InFlushTO;
  Word_t           OutFlushTO;
  L2CA_Flow_Spec_t InFlow;
  Word_t           RemoteCID;
} L2CA_Config_Params_t;

#define L2CA_CONFIG_PARAMS_SIZE                         (sizeof(L2CA_Config_Params_t))

   /*! The following structure represents the structure of the L2CAP
    * Disconnect Indication Event Data.
    */
typedef struct _tagL2CA_Disconnect_Indication_t
{
  Word_t LCID; /*!< Local channel identifier. */
  Byte_t Reason; /*!< Reason for disconnection */
} L2CA_Disconnect_Indication_t;

#define L2CA_DISCONNECT_INDICATION_SIZE                 (sizeof(L2CA_Disconnect_Indication_t))

   /*! The following structure represents the structure of the L2CAP
    * Disconnect Confirmation Event Data.
    */
typedef struct _tagL2CA_Disconnect_Confirmation_t
{
  Word_t Result; /*!< Disconnection action result.  Possible values are:
                      L2CAP_DISCONNECT_RESPONSE_RESULT_SUCCESS
                      L2CAP_DISCONNECT_RESPONSE_RESULT_TIMEOUT */
  Word_t LCID; /*!< Local channel identifier. */
} L2CA_Disconnect_Confirmation_t;

#define L2CA_DISCONNECT_CONFIRMATION_SIZE               (sizeof(L2CA_Disconnect_Confirmation_t))

   /*! The following structure represents the structure of the L2CAP
    * Timeout Indication Event Data.
    */
typedef struct _tagL2CA_Timeout_Indication_t
{
  Word_t LCID; /*!< Local channel identifier. */
} L2CA_Timeout_Indication_t;

#define L2CA_TIMEOUT_INDICATION_SIZE                    (sizeof(L2CA_Timeout_Indication_t))

   /*! The following structure represents the structure of the L2CAP Echo
    * Confirmation Event Data.
    */
typedef struct _tagL2CA_Echo_Confirmation_t
{
   BD_ADDR_t  BD_ADDR; /*!< Bluetooth address of the remote device that participated in the L2CAP Ping request. */
   Word_t     Result; /*!< Outcome of the Ping operation.  Possible values are:
                           L2CAP_ECHO_REQUEST_RESULT_RESPONSE_RECEIVED
                           L2CAP_ECHO_REQUEST_RESULT_RESPONSE_TIMEOUT */
   Word_t     Echo_Data_Length; /*!<  Number of bytes in the response, Variable_Data, array */
   Byte_t    *Variable_Data; /*!< Echo response data. */
} L2CA_Echo_Confirmation_t;

#define L2CA_ECHO_CONFIRMATION_SIZE                      (sizeof(L2CA_Echo_Confirmation_t))

   /*! The following structure represents the structure of the L2CAP
    * Information Confirmation Event Data.
    */
typedef struct _tagL2CA_Information_Confirmation_t
{
   BD_ADDR_t  BD_ADDR; /*!< Bluetooth device address whose device information if being returned. */
   Word_t     InfoType; /*!< Type of information returned.  Possible values are:
                            L2CAP_INFORMATION_REQUEST_INFOTYPE_CONNECTIONLESS_MTU
                            L2CAP_INFORMATION_REQUEST_INFOTYPE_EXTENDED_FEATURE_MASK */
   Word_t     Result; /*!< Outcome of this operation.  Possible values are:
                          L2CAP_INFORMATION_RESPONSE_RESULT_SUCCESS
                          L2CAP_INFORMATION_RESPONSE_RESULT_NOT_SUPPORTED
                          L2CAP_INFORMATION_RESPONSE_RESULT_PDU_REJECTED
                          L2CAP_INFORMATION_RESPONSE_RESULT_TIMEOUT */
   Byte_t    *Variable_Data; /*!< Returned device information. */
} L2CA_Information_Confirmation_t;

#define L2CA_INFORMATION_CONFIRMATION_SIZE               (sizeof(L2CA_Information_Confirmation_t))

   /*! The following MACRO is a utility MACRO that exists to aid code
    * readability to Read a specified Connectionless MTU Value in the
    * L2CA Information Confirmation Data. @param _x The first parameter is a
    * pointer to a Data Buffer that is a
    * L2CA_Information_Confirmation_t.
    * \note No Check of any of the input data is performed !
    */
#define L2CA_INFORMATION_CONFIRMATION_READ_CONNECTIONLESS_MTU_DATA(_x)      (((Word_t *)(((L2CA_Information_Confirmation_t *)(_x))->Variable_Data))[0])

   /*! The following MACRO is a utility MACRO that exists to aid code
    * readability to Set a specified Connectionless MTU Value in the
    * L2CA Information Confirmation Data.  @param _x The first parameter is a
    * pointer to a Data Buffer that is a
    * L2CA_Information_Confirmation_t.  @param _y The second parameter is the
    * value of the Connectionless MTU to set in the data structure.
    * \note No Check of any of the input data is performed !
    */
#define L2CA_INFORMATION_CONFIRMATION_SET_CONNECTIONLESS_MTU_DATA(_x, _y)   (((Word_t *)(((L2CA_Information_Confirmation_t *)(_x))->Variable_Data))[0] = (Word_t)(_y))

   /*! The following structure represents the structure of the L2CAP
    * Connectionless Data Indication Event Data.
    */
typedef struct _tagL2CA_Group_Data_Indication_t
{
   Word_t     PSM; /*!< Protocol/Service Multiplexer value to which this callback is to be registered. */
   Word_t     GroupCID; /*!< Channel identifier that uniquely identifies the group. */
   BD_ADDR_t  BD_ADDR; /*!< Address of the Bluetooth device. */
   Word_t     Data_Length; /*!< Number of bytes read in, i.e., in Variable_Data. */
   Byte_t    *Variable_Data; /*!< Data read in. */
} L2CA_Group_Data_Indication_t;

#define L2CA_GROUP_DATA_INDICATION_SIZE                  (sizeof(L2CA_Group_Data_Indication_t))

   /*! The following structure represents the structure of the L2CAP Data
    * Indication Event Data.
    */
typedef struct _tagL2CA_Data_Indication_t
{
   Word_t  CID; /*!< Channel identifier. */
   Word_t  Data_Length; /*!< Number of bytes read in, i.e., in Variable_Data. */
   Byte_t *Variable_Data; /*!< Data read in. */
} L2CA_Data_Indication_t;

#define L2CA_DATA_INDICATION_SIZE                        (sizeof(L2CA_Data_Indication_t))

   /*! The following structure represents the structure of the L2CAP Data
    * Error Indication Event.  The Data Error Event is issued when an
    * inconsistency is detected in the reception of data on a channel
    * that is configured for reliable operation.
    */
typedef struct _tagL2CA_Data_Error_Indication_t
{
   Word_t Result; /*!< Outcome of this operation.  Possible values are:
                       L2CAP_DATA_READ_RESULT_SUCCESS
                       L2CAP_DATA_READ_RESULT_ERROR */
   Word_t Status; /*!< If Result was an error, what the cause of the error was.  Possible values are:
                       L2CAP_DATA_READ_STATUS_MTU_EXCEEDED
                       L2CAP_DATA_READ_STATUS_RECEIVE_TIMEOUT
                       L2CAP_DATA_READ_STATUS_SIZE_ERROR */
   Word_t CID; /*!< Channel identifier */
} L2CA_Data_Error_Indication_t;

   /*! The following MACRO is a utility MACRO that exists to aid code
    * readability to Determine the size (in Bytes) of an
    * L2CA Data Error Indication Structure.
    */
#define L2CA_DATA_ERROR_INDICATION_SIZE                 (sizeof(L2CA_Data_Error_Indication_t))

   /*! The following structure represents the structure of the L2CA Group
    * Member Status Event Data.  Upon the successful addition of a
    * member to the group, the information in the structure will be
    * dispatched, via the Callback function that was supplied when the
    * Group was created, to indicate the Connection State of the remote
    * device.  There will be a Callback made for each Connection and
    * Disconnection of the remote device.  The BD_ADDR indicates the
    * Board Address of the member that was attempted to be added and the
    * CID references the Group in which the Member was to be added.
    */
typedef struct _tagL2CA_Group_Member_Status_t
{
   Word_t    PSM; /*!< Registered PSM associated with the group. */
   Word_t    GroupCID; /*!< Channel identifier that uniquely identifies the group. */
   BD_ADDR_t BD_ADDR; /*!< Address of the Bluetooth device. */
   Boolean_t Connected; /*!< Address of the Bluetooth device. */
} L2CA_Group_Member_Status_t;

#define L2CA_GROUP_MEMBER_STATUS_SIZE                   (sizeof(L2CA_Group_Member_Status_t))

   /*! The following structure represents the event that is dispatched
    * when the L2CAP Channel Buffers are Empty.  This event is ONLY
    * dispatched when the caller has tried to write data and the
    * Channel Buffers are full.  See the L2CAP_Data_Write() and the
    * L2CAP_Group_Data_Write() functions for more information.
    */
typedef struct _tagL2CA_Channel_Buffer_Empty_Indication_t
{
   Word_t CID; /*!< Channel identifier which has no longer had any data available for transmitting. */
} L2CA_Channel_Buffer_Empty_Indication_t;

#define L2CA_CHANNEL_BUFFER_EMPTY_INDICATION_SIZE       (sizeof(L2CA_Channel_Buffer_Empty_Indication_t))

   /*! The following structure represents the event that is dispatched
    * when the L2CAP Connection Parameter Update request is received.
    */
typedef struct _tagL2CA_Connection_Parameter_Update_Indication_t
{
   Word_t    FCID; /*!< Fixed channel identifier which the connection parameter update indication request was received. */
   BD_ADDR_t BD_ADDR; /*!< Bluetooth device address of the device that has requested the connection parameter update. */
   Word_t    IntervalMin; /*!< Minimum value for the the connection interval.  This should fall within the range:
                               HCI_LE_CONNECTION_INTERVAL_MINIMUM
                               HCI_LE_CONNECTION_INTERVAL_MAXIMUM */
   Word_t    IntervalMax; /*!< This should be greater than or equal to Conn_Interval_Min and shall fall within the range:
                               HCI_LE_CONNECTION_INTERVAL_MINIMUM
                               HCI_LE_CONNECTION_INTERVAL_MAXIMUM
                               Both intervals follow the rule:
                               Time = N * 1.25 msec  */
   Word_t    SlaveLatency; /*!< Slave latency for connection.  This should be in range:
                                HCI_LE_CONNECTION_LATENCY_MINIMUM
                                HCI_LE_CONNECTION_LATENCY_MAXIMUM */
   Word_t    TimeoutMultiplier; /*!< Supervision timeout multiplier for LE link.  This should be in range:
                                     HCI_LE_SUPERVISION_TIMEOUT_MINIMUM
                                     HCI_LE_SUPERVISION_TIMEOUT_MAXIMUM
                                         The Supervision_Timeout follows the rule:
                                     Time = N * 10 msec  */
} L2CA_Connection_Parameter_Update_Indication_t;

#define L2CA_CONNECTION_PARAMETER_UPDATE_INDICATION_SIZE   (sizeof(L2CA_Connection_Parameter_Update_Indication_t))

   /*! The following structure represents the event that is dispatched
    * when the L2CAP Connection Parameter Update response is received.
    */
typedef struct _tagL2CA_Connection_Parameter_Update_Confirmation_t
{
   Word_t    FCID; /*!< Fixed channel identifier which the connection parameter update indication response (confirmation) was received. */
   BD_ADDR_t BD_ADDR; /*!< Bluetooth device address of the device that has responsed to the connection parameter update request. */
   Word_t    Result; /*!< Result of the connection parameter update request.  This will be one of the following values:
                          L2CAP_CONNECTION_PARAMETER_UPDATE_RESPONSE_RESULT_ACCEPTED
                          L2CAP_CONNECTION_PARAMETER_UPDATE_RESPONSE_RESULT_REJECTED */
} L2CA_Connection_Parameter_Update_Confirmation_t;

#define L2CA_CONNECTION_PARAMETER_UPDATE_CONFIRMATION_SIZE (sizeof(L2CA_Connection_Parameter_Update_Confirmation_t))

   /*! The following structure represents the structure of the L2CAP
    * Fixed Channel Connect Indication Event Data.
    */
typedef struct _tagL2CA_Fixed_Channel_Connect_Indication_t
{
   Word_t                 FCID; /*!< Fixed channel identifier which the connection event was received. */
   BD_ADDR_t              BD_ADDR; /*!< Bluetooth device address of the device that has connected to the local device on the corresponding fixed channel. */
   L2CA_Controller_Type_t ControllerType; /*!< Value that specifies the controller type of the fixed channel connection.  This will be one of the following:
                                               ctBR_EDR
                                               ctLE */
} L2CA_Fixed_Channel_Connect_Indication_t;

#define L2CA_FIXED_CHANNEL_CONNECT_INDICATION_SIZE         (sizeof(L2CA_Fixed_Channel_Connect_Indication_t))

   /*! The following structure represents the structure of the L2CAP
    * Fixed Channel Disconnect Indication Event Data.
    */
typedef struct _tagL2CA_Fixed_Channel_Disconnect_Indication_t
{
  Word_t                 FCID; /*!< Fixed channel identifier which the connection event was received. */
  BD_ADDR_t              BD_ADDR; /*!< Bluetooth device address of the device that has disconnected from the local device on the corresponding fixed channel. */
  L2CA_Controller_Type_t ControllerType; /*!< Value that specifies the controller type of the fixed channel connection.  This will be one of the following:
                                               ctBR_EDR
                                               ctLE */
} L2CA_Fixed_Channel_Disconnect_Indication_t;

#define L2CA_FIXED_CHANNEL_DISCONNECT_INDICATION_SIZE      (sizeof(L2CA_Fixed_Channel_Disconnect_Indication_t))

   /*! The following structure represents the structure of the L2CAP
    * Fixed Channel Data Indication Event Data.
    */
typedef struct _tagL2CA_Fixed_Channel_Data_Indication_t
{
   Word_t     FCID; /*!< Fixed channel identifier which the data was received. */
   BD_ADDR_t  BD_ADDR; /*!< Bluetooth device address of the device that has sent the data to the local device on the corresponding fixed channel. */
   Word_t     Data_Length; /*!< Number of bytes read in, i.e., in Variable_Data. */
   Byte_t    *Variable_Data; /*!< Data read in. */
} L2CA_Fixed_Channel_Data_Indication_t;

#define L2CA_FIXED_CHANNEL_DATA_INDICATION_SIZE          (sizeof(L2CA_Fixed_Channel_Data_Indication_t))

   /*! The following structure represents the event that is dispatched
    * when a specific fixed channel connection's buffers are empty.
    * This event is ONLY dispatched when the caller has tried to write
    * data and the Fixed Channel Buffers are full.  See the
    * L2CAP_Enhanced_Fixed_Channel_Data_Write() function for more
    * information.
    */
typedef struct _tagL2CA_Fixed_Channel_Buffer_Empty_Indication_t
{
   Word_t    FCID; /*!< Fixed channel identifier which the data was received. */
   BD_ADDR_t BD_ADDR; /*!< Bluetooth device address of the device that has sent the data to the local device on the corresponding fixed channel. */
} L2CA_Fixed_Channel_Buffer_Empty_Indication_t;

#define L2CA_FIXED_CHANNEL_BUFFER_EMPTY_INDICATION_SIZE  (sizeof(L2CA_Fixed_Channel_Buffer_Empty_Indication_t))

   /*! The following structure represents the structure of the L2CAP LE
    * Connect Indication Event Data (etLE_Connect_Indication).  This
    * structure is used when an L2CAP LE Connect Request is received.
    * The Upper Layer is provided the LE Connect Request information in
    * the Callback Function.  The LCID is the Local CID that should be
    * used to reference this connection.  The Board address is provided
    * as Extra information about the device that is requesting the
    * connection.
    */
typedef struct _tagL2CA_LE_Connect_Indication_t
{
  Word_t    PSM; /*!< LE Protocol/Service Multiplexer value to which this callback is to be registered. */
  Word_t    LCID; /*!< Address of the Bluetooth device requesting the connection. */
  Byte_t    Identifier; /*!< Requestor’s identifier used to match up responses. */
  BD_ADDR_t BD_ADDR; /*!< Address of the Bluetooth device requesting the connection. */
  Word_t    MaxSDUSize; /*!< Maximum Service Data Unit  (MTU) that may be sent by the local device to the remote device. */
  Word_t    MaxPDUSize; /*!< Maximum Packet Data Unit (MPS) that may be sent by the local device to the remote.  Service Data Units bigger than this value will be fragmented into packets that are a multiple of the MPS. */
  Word_t    InitialCredits; /*!< The number of initial LE-Frames (of at most MPS bytes) that may be sent by the local device to the remote before the remote grants credits back. */
} L2CA_LE_Connect_Indication_t;

#define L2CA_LE_CONNECT_INDICATION_SIZE                  (sizeof(L2CA_LE_Connect_Indication_t))

   /*! The following structure represents the structure of the L2CAP LE
    * Connect Confirmation Event Data (etLE_Connect_Confirmation).  This
    * structure is used when an L2CAP LE Connect Response is received.
    * The Upper Layer is provided the LE Connect Response information in
    * the Callback Function.  The LCID is the Local CID that should be
    * used to reference this connection.
    */
typedef struct _tagL2CA_LE_Connect_Confirmation_t
{
   Word_t LCID; /*!< Local channel identifier. */
   Word_t Result; /*!< Contains the result of the connection request. */
   Word_t MaxSDUSize; /*!< Maximum Service Data Unit  (MTU) that may be sent by the local device to the remote device. */
   Word_t MaxPDUSize; /*!< Maximum Packet Data Unit (MPS) that may be sent by the local device to the remote.  Service Data Units bigger than this value will be fragmented into packets that are a multiple of the MPS. */
   Word_t InitialCredits; /*!< The number of initial LE-Frames (of at most MPS bytes) that may be sent by the local device to the remote before the remote grants credits back. */
} L2CA_LE_Connect_Confirmation_t;

#define L2CA_LE_CONNECT_CONFIRMATION_SIZE                (sizeof(L2CA_LE_Connect_Confirmation_t))

   /*! The following structure represents the structure of the L2CAP LE
    * Disconnect Indication Event Data.
    */
typedef struct _tagL2CA_LE_Disconnect_Indication_t
{
  Word_t LCID; /*!< Local channel identifier. */
  Byte_t Reason; /*!< Contains the reason code that tells the application the reason the connection was disconnected.  */
} L2CA_LE_Disconnect_Indication_t;

#define L2CA_LE_DISCONNECT_INDICATION_SIZE               (sizeof(L2CA_LE_Disconnect_Indication_t))

   /*! The following structure represents the structure of the L2CAP LE
    * Disconnect Confirmation Event Data.
    */
typedef struct _tagL2CA_LE_Disconnect_Confirmation_t
{
  Word_t LCID; /*!< Local channel identifier. */
  Word_t Result; /*!< Contains the reason code that tells the application the reason the connection was disconnected. */
} L2CA_LE_Disconnect_Confirmation_t;

#define L2CA_LE_DISCONNECT_CONFIRMATION_SIZE             (sizeof(L2CA_LE_Disconnect_Confirmation_t))

   /*! The following structure represents the structure of the L2CAP LE
    * Data Indication Event Data.
    * \note
    * \note This CreditsConsumed member is additive so the
    *          application may add the CreditsConsumed from multiple
    *          events into one call to the L2CA_LE_Grant_Credits().
    *          However the application should grant credits as soon as a
    *          SDU is consumed rather than waiting on multiple data
    *          indication events.
    */
typedef struct _tagL2CA_LE_Data_Indication_t
{
   Word_t  CID; /*!< Identifies the channel on which the data is currently queued. */
   Word_t  CreditsConsumed; /*!< The CreditsConsumed member of this event indicates the
                             * number of credits that were consumed by the remote device
                             * to send this packet.  When in manual-credit mode (i.e.
                             * the L2CA_LE_CHANNEL_PARAMETER_FLAGS_MANUAL_CREDIT_MODE
                             * flag was set in the ChannelFlags member of the
                             * L2CA_LE_Channel_Parameters_t structure that was passed to
                             * either the L2CA_LE_Connect_Request() or
                             * L2CA_LE_Connect_Response() API calls that created or
                             * accepted this LE channel respectively) then the
                             * application should call the L2CA_LE_Grant_Credits()
                             * passing the CreditsConsumed member to inform the L2CA
                             * Layer that the SDU has been consumed and we can grant
                             * credits back. */
   Word_t  Data_Length; /*!< Identifies the channel on which the data is currently queued. */
   Byte_t *Variable_Data; /*!< The data received on the logical LE channel. */
} L2CA_LE_Data_Indication_t;

#define L2CA_LE_DATA_INDICATION_SIZE                     (sizeof(L2CA_LE_Data_Indication_t))

   /*! The following structure represents the event that is dispatched
    * when the L2CAP LE Channel Buffers are Empty.  This event is ONLY
    * dispatched when the caller has tried to write data and the Channel
    * Buffers are full.  See the
    * L2CA_Enhanced_Dynamic_Channel_Data_Write() function for more
    * information.
    */
typedef struct _tagL2CA_LE_Channel_Buffer_Empty_Indication_t
{
   Word_t CID; /*!< Identifies the channel on which the data is currently queued. */
} L2CA_LE_Channel_Buffer_Empty_Indication_t;

#define L2CA_LE_CHANNEL_BUFFER_EMPTY_INDICATION_SIZE     (sizeof(L2CA_LE_Channel_Buffer_Empty_Indication_t))

typedef struct _tagL2CA_Event_Data_t
{
   L2CA_Event_Type_t L2CA_Event_Type;
   DWord_t           Event_Data_Length;
   union
   {
      L2CA_Connect_Indication_t                       *L2CA_Connect_Indication;
      L2CA_Connect_Confirmation_t                     *L2CA_Connect_Confirmation;
      L2CA_Config_Indication_t                        *L2CA_Config_Indication;
      L2CA_Config_Confirmation_t                      *L2CA_Config_Confirmation;
      L2CA_Disconnect_Indication_t                    *L2CA_Disconnect_Indication;
      L2CA_Disconnect_Confirmation_t                  *L2CA_Disconnect_Confirmation;
      L2CA_Echo_Confirmation_t                        *L2CA_Echo_Confirmation;
      L2CA_Information_Confirmation_t                 *L2CA_Information_Confirmation;
      L2CA_Timeout_Indication_t                       *L2CA_Timeout_Indication;
      L2CA_Data_Indication_t                          *L2CA_Data_Indication;
      L2CA_Data_Error_Indication_t                    *L2CA_Data_Error_Indication;
      L2CA_Group_Data_Indication_t                    *L2CA_Group_Data_Indication;
      L2CA_Group_Member_Status_t                      *L2CA_Group_Member_Status;
      L2CA_Channel_Buffer_Empty_Indication_t          *L2CA_Channel_Buffer_Empty_Indication;
      L2CA_Connection_Parameter_Update_Indication_t   *L2CA_Connection_Parameter_Update_Indication;
      L2CA_Connection_Parameter_Update_Confirmation_t *L2CA_Connection_Parameter_Update_Confirmation;
      L2CA_Fixed_Channel_Connect_Indication_t         *L2CA_Fixed_Channel_Connect_Indication;
      L2CA_Fixed_Channel_Disconnect_Indication_t      *L2CA_Fixed_Channel_Disconnect_Indication;
      L2CA_Fixed_Channel_Data_Indication_t            *L2CA_Fixed_Channel_Data_Indication;
      L2CA_Fixed_Channel_Buffer_Empty_Indication_t    *L2CA_Fixed_Channel_Buffer_Empty_Indication;
      L2CA_LE_Connect_Indication_t                    *L2CA_LE_Connect_Indication;
      L2CA_LE_Connect_Confirmation_t                  *L2CA_LE_Connect_Confirmation;
      L2CA_LE_Disconnect_Indication_t                 *L2CA_LE_Disconnect_Indication;
      L2CA_LE_Disconnect_Confirmation_t               *L2CA_LE_Disconnect_Confirmation;
      L2CA_LE_Data_Indication_t                       *L2CA_LE_Data_Indication;
      L2CA_LE_Channel_Buffer_Empty_Indication_t       *L2CA_LE_Channel_Buffer_Empty_Indication;
   } Event_Data;
} L2CA_Event_Data_t;

   /*! The following Constant represents the actual size of the L2CA
    * Event Data Structure.  This Constant is to be used instead of
    * simply using sizeof(L2CA_Event_Data_t) to get the size.  The
    * reason for this was explained above, and is primarily to aid in
    * code readability and efficiency.
    */
#define L2CA_EVENT_DATA_SIZE                            (sizeof(L2CA_Event_Data_t))

   /*! The following declared type represents the Prototype Function for
    * an L2CAP Event Callback.  This function will be called whenever a
    * define L2CAP Action occurs within the Bluetooth Protocol Stack
    * that is specified with the specified Bluetooth Stack ID.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack on which the event occurred.
    * @param L2CA_Event_Data Pointer to a structure that contains information
    * about the event that has occurred.
    * @param CallbackParameter User defined value to was supplied as an
    * input parameter from a prior L2CAP request.
    * The caller is free to use the contents of the L2CAP
    * Event Data ONLY in the context of this callback.  If the caller
    * requires the Data for a longer period of time, then the callback
    * function MUST copy the data into another Data Buffer.  This
    * function is guaranteed NOT to be invoked more than once
    * simultaneously for the specified installed callback (i.e. this
    * function DOES NOT have be reentrant).  It Needs to be noted
    * however, that if the same Callback is installed more than once,
    * then the callbacks will be called serially.  Because of this, the
    * processing in this function should be as efficient as possible.
    * It should also be noted that this function is called in the
    * Thread Context of a Thread that the User does NOT own.  Therefore,
    * processing in this function should be as efficient as possible
    * (this argument holds anyway because another L2CAP Event will not
    * be processed while this function call is outstanding).
    * \note This function MUST NOT Block and wait for events that
    *            can only be satisfied by Receiving other Bluetooth
    *            Stack Events.  A Deadlock WILL occur because other
    *            Callbacks might not be issued while this function is
    *            currently outstanding.
    */
typedef void (BTPSAPI *L2CA_Event_Callback_t)(unsigned int BluetoothStackID, L2CA_Event_Data_t *L2CA_Event_Data, unsigned long CallbackParameter);

   /*! @brief The following function is responsible for setting Timer Values
    * that are used to control the operation of the Stack.  Refer to the
    * documentation for the L2CA_Timer_Values_t structure for
    * information on the available timers and their function.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param L2CA_Timer_Values Stack control timer values.
    * @return If the new values are successfully set, the function will return a
    * value of Zero.  If any error occurs while settings these values, a
    * negative value will be returned.  Once set, these timer values
    * will be used for all future L2CAP connections.  Connections that
    * currently exist will not be affected.
    * \note Prior to setting any timer with a new value, each of the
    *           values will be tested to ensure that the value is
    *           within the valid range for that timer.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Set_Timer_Values(unsigned int BluetoothStackID, L2CA_Timer_Values_t *L2CA_Timer_Values);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Set_Timer_Values_t)(unsigned int BluetoothStackID, L2CA_Timer_Values_t *L2CA_Timer_Values);
#endif

   /*! @brief The following function is responsible for retrieving the current
    * Timer Values that are used to control the operation of the Stack.
    * Refer to the documentation for the L2CA_Timer_Values_t structure
    * for information on the available timers and their function.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param L2CA_Timer_Values Stack control timer values.
    * @return If the values are successfully retrieved, the
    * function will return a value of Zero.  If any error occurs while
    * obtaining these values, a negative value will be returned.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Get_Timer_Values(unsigned int BluetoothStackID, L2CA_Timer_Values_t *L2CA_Timer_Values);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Get_Timer_Values_t)(unsigned int BluetoothStackID, L2CA_Timer_Values_t *L2CA_Timer_Values);
#endif

   /*! @brief The following function is responsible for Registering an L2CAP
    * Callback function, with the L2CAP Layer associated with the
    * specified Bluetooth Stack ID, to handle incoming L2CAP Events
    * destined for the specified PSM Number.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * protocol stack via a call to BSC_Initialize.
    * @param PSM Protocol/Service Multiplexer value to which this callback
    * is to be registered.
    * @param L2CA_Event_Callback Function pointer to be used by the L2CAP
    * layer to notify higher layers of L2CAP events.
    * @param CallbackParameter User defined value to be supplied as an
    * input parameter for all event callbacks.
    * @return This function returns a non-zero, positive return value which
    * represents the L2CAP PSM Callback ID if successful, or a
    * negative return code if the function is unsuccessful.  The caller
    * can use the return value from this function (if successful) as
    * the L2CAP_PSMID parameter for the L2CA_Un_Register_PSM()
    * function when the caller wants to Un-Register the PSM from the
    * L2CAP Layer associated with the Bluetooth Stack specified by the
    * Bluetooth Stack ID parameter.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Register_PSM(unsigned int BluetoothStackID, Word_t PSM, L2CA_Event_Callback_t L2CA_Event_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Register_PSM_t)(unsigned int BluetoothStackID, Word_t PSM, L2CA_Event_Callback_t L2CA_Event_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is responsible for Un-Registering an L2CAP
    * Callback function that was previously registered via a successful
    * call to the L2CA_Register_PSM() function.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth protocol stack via a call to BSC_Initialize.
    * @param L2CAP_PSMID PSMID value that uniquely identifies the
    * callback function for a PSM value. The L2CAP_PSMID supplied is
    * the return value of a successful call to the L2CA_Register_PSM function.
    * @return This function returns zero if the
    * L2CAP PSM Callback was successfully removed from the Bluetooth
    * Protocol Stack or a negative return value if the operation was
    * unsuccessful.  If this function completes successfully, then the
    * specified Callback will NO longer be called when an L2CAP Event
    * occurs for the PSM Number that was associated with the specified
    * L2CAP Callback function.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Un_Register_PSM(unsigned int BluetoothStackID, unsigned int L2CAP_PSMID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Un_Register_PSM_t)(unsigned int BluetoothStackID, unsigned int L2CAP_PSMID);
#endif

   /*! @brief The following function is responsible for Registering an L2CAP
    * Callback function, with the L2CAP Layer associated with the
    * specified Bluetooth Stack ID, to handle incoming L2CAP Events
    * destined for the specified LE PSM Number.  @param BluetoothStackID The first parameter to
    * this function is the Bluetooth Stack ID that specifies the
    * Bluetooth Protocol Stack to install the L2CAP Callback with.
    * @param LE_PSM The second parameter is the LE PSM Number to register the L2CAP
    * Callback for. @param L2CA_Event_Callback The third parameter specifies the Callback
    * Function. @param CallbackParameter The fourth parameter is the Callback Parameter that will be passed to the
    * callback function when an Event occurs for the specified LE PSM.
    * @return This function returns a non-zero, positive return value which
    * represents the L2CAP LE PSM Callback ID if successful, or a
    * negative return code if the function is unsuccessful.  The caller
    * can use the return value from this function (if successful) as the
    * L2CAP_PSMID parameter for the L2CA_Un_Register_LE_PSM() function
    * when the caller wants to Un-Register the LE PSM from the L2CAP
    * Layer associated with the Bluetooth Stack specified by the
    * Bluetooth Stack ID parameter.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Register_LE_PSM(unsigned int BluetoothStackID, Word_t LE_PSM, L2CA_Event_Callback_t L2CA_Event_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Register_LE_PSM_t)(unsigned int BluetoothStackID, Word_t LE_PSM, L2CA_Event_Callback_t L2CA_Event_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is responsible for Un-Registering an L2CAP
    * Callback function that was previously registered via a successful
    * call to the L2CA_Register_LE_PSM() function.  @param BluetoothStackID The first parameter
    * to this function is the Bluetooth Stack ID that specifies the
    * Bluetooth Protocol Stack that the L2CAP PSM Callback Function was
    * previously installed. @param L2CAP_PSMID The second parameter is the L2CAP LE PSM
    * Callback ID.  This ID is the return value from the
    * L2CA_Register_LE_PSM() function. @return This function returns zero if
    * the L2CAP LE PSM Callback was successfully removed from the
    * Bluetooth Protocol Stack or a negative return value if the
    * operation was unsuccessful.  If this function completes
    * successfully, then the specified Callback will NO longer be called
    * when an L2CAP Event occurs for the PSM Number that was associated
    * with the specified L2CAP Callback function.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Un_Register_LE_PSM(unsigned int BluetoothStackID, unsigned int L2CAP_PSMID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Un_Register_LE_PSM_t)(unsigned int BluetoothStackID, unsigned int L2CAP_PSMID);
#endif

   /*! @brief The following function is responsible for Registering an L2CAP
    * Callback function, with the L2CAP Layer associated with the
    * specified Bluetooth Stack ID, to handle incoming L2CAP Events
    * destined for the specified Fixed Channel.  @param BluetoothStackID The first parameter to
    * this function is the Bluetooth Stack ID that specifies the
    * Bluetooth Protocol Stack to install the L2CAP Callback with.
    * @param FixedChannel The second parameter is the Fixed Channel to register the L2CAP
    * Callback for. @param ChannelParameters The third parameter is a pointer to channel specific
    * information that may be needed to support the fixed channel. @param L2CA_Event_Callback The
    * fourth parameter is the Callback Function. @param CallbackParameter The fifth parameter is the Callback
    * Parameter that will be passed to the callback function when an
    * Event occurs for the specified channel.  @return This function returns a
    * non-zero, positive return value which represents the registered
    * Fixed channel ID (FCID) if successful, or a negative return code
    * if the function is unsuccessful.  The caller can use the return
    * value from this function (if successful) as the L2CAP_FCID
    * parameter for the L2CA_Un_Register_Fixed_Channel() function when
    * the caller wants to Un-Register the Channel from the L2CAP Layer
    * associated with the Bluetooth Stack specified by the Bluetooth
    * Stack ID parameter.
    * \note The ChannelParameters parameter is a placeholder for
    *          channel Specific information.  At this time there are no
    *          specific parameters assigned for any fixed channel.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Register_Fixed_Channel(unsigned int BluetoothStackID, Word_t FixedChannel, void *ChannelParameters, L2CA_Event_Callback_t L2CA_Event_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Register_Fixed_Channel_t)(unsigned int BluetoothStackID, Word_t FixedChannel, void *ChannelParameters, L2CA_Event_Callback_t L2CA_Event_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is responsible for un-registering an L2CAP
    * callback function that was previously registered via a successful
    * call to the L2CA_Register_Fixed_Channel() function.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param FCID Fixed channel ID value that uniquely identifies
    * the callback function for a fixed channel. The FCID supplied is
    * the return value of a successful call to the L2CA_Register_Fixed_Channel function.
    * @return This function returns
    * zero if the Callback was successfully removed from the Bluetooth
    * Protocol Stack or a negative return value if the operation was
    * unsuccessful.  If this function completes successfully, then the
    * specified Callback will NO longer be called when an L2CAP Event
    * occurs for the Channel that was associated with the specified
    * L2CAP Callback function.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Un_Register_Fixed_Channel(unsigned int BluetoothStackID, Word_t FCID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Un_Register_Fixed_Channel_t)(unsigned int BluetoothStackID, Word_t FCID);
#endif

   /*! @brief The following function is responsible for requesting the creation
    * of a Logical L2CAP Connection with the specified Bluetooth Board
    * Address.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Device address of the Bluetooth device to which an L2CAP
    * logical channel is to be established.
    * @param PSM Protocol/Service Multiplexer identifier of the remote device
    * to which the logical channel connection is to be made.
    * @param L2CA_Event_Callback Pointer to a callback function to be used by
    * the L2CAP layer to dispatch L2CAP Event information for this connection.
    * @param CallbackParameter User defined value to be used by the L2CAP layer
    * as an input parameter for all callbacks.
    * @return This function returns a positive,
    * non-zero Local Channel Identifier (LCID) if the L2CAP Connection
    * Request was issued successfully, or a negative, return error code
    * indicating an error.
    * \note If this function is successful, then all further Status
    *          and Event Notifications will occur through the Callback
    *          Function that is specified with this function.
    * \note A Positive return value (LCID) does NOT mean that a
    *          connection already exists, only that the Connection
    *          Request has been successfully submitted.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Connect_Request(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t PSM, L2CA_Event_Callback_t L2CA_Event_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Connect_Request_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t PSM, L2CA_Event_Callback_t L2CA_Event_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is responsible for Responding to a L2CAP
    * Connect Indication Event.  This function allows the recipient of
    * the L2CAP Event the option of Responding to the Connection
    * Indication with a Positive Accept, a Negative Accept, or a
    * Pending Accept.  This function should be called ONLY upon receipt
    * of an L2CAP Connect Indication Event Callback.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Device address of the Bluetooth device to which an L2CAP
    * logical channel is to be established.  The BD_ADDR is obtained from
    * the L2CA_Connect_Indication event.
    * @param Identifier L2CAP assigned number used to match requests with
    * responses.  The Identifier value is obtained from the L2CA_Connect_Indication event.
    * @param LCID Local CID value used by the L2CAP layer to reference the
    * logical channel being requested.  The LCID value is obtained from the
    * L2CA_Connect_Indication event.
    * @param Response User supplied response to the connection request.
    * The connection is accepted, rejected or pended by the value of this parameter.
    * @param Status The Status parameter only has significance when the
    * Connection Pending response is provided and is used to provide extra
    * information about the status of the connection.
    * @return This function returns zero if the L2CAP Connection
    * Response was successfully submitted, or a negative return error
    * code if the Response was NOT successfully submitted.
    * \note A successful return value (zero) does NOT mean that the
    *          connection response was sent, only that the Connection
    *          Response has been successfully submitted to the L2CAP
    *          Layer associated with the Local Bluetooth Protocol Stack.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Connect_Response(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t Identifier, Word_t LCID, Word_t Response, Word_t Status);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Connect_Response_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t Identifier, Word_t LCID, Word_t Response, Word_t Status);
#endif

   /*! @brief The following function is used to issue an Configuration Request
    * to the other peer.  The configuration data is passed to this
    * function in a structure.  Since those options that the user does
    * not want to negotiate, but assume the default, are not included in
    * the structure.  In order to determine which options are present in
    * the structure, the structure holds an Option_Flags field the is a
    * bit mask of the options that are included in the structure.
    * Option fields that are not marked in the Option_Flags field are to
    * be ignored.  NOTE: This function will scan the options record and
    * the Option_Flags bit mask in order to build a Options String that
    * is compatible with the L2CAP specification. Options string is
    * variable string of options that begin with an Option Flag, Option
    * length and Option Data for each option in the list.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param LCID Local CID value referencing the logical channel being
    * configures.
    * @param LinkTO Suggested Baseband Link Timeout value to be used for
    * the connection.
    * @param ConfigRequest Structure containing the configuration parameters
    * to be negotiated.
    * @return Zero (0) if successful submitting the Connect Response.
    * This does not mean that the connect response has been delivered,
    * but that the response was successfully submitted for delivery.
    * Negative if an Error occurred and the Response was not submitted.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Config_Request(unsigned int BluetoothStackID, Word_t LCID, Word_t LinkTO, L2CA_Config_Request_t *ConfigRequest);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Config_Request_t)(unsigned int BluetoothStackID, Word_t LCID, Word_t LinkTO, L2CA_Config_Request_t *ConfigRequest);
#endif

   /*! @brief The following function is used to issue an Configuration Response
    * to the other peer.  The configuration data is passed to this
    * function in a structure. Those options that the user will accept
    * in the request are not included in the structure.  In order to
    * determine which options are present in the structure, the
    * structure holds an Option_Flags field the is a bit mask of the
    * options that are included in the structure.  Option fields that
    * are not marked in the Option_Flags field are accepted by the local
    * user.  NOTE: This function will scan the options record and the
    * Option_Flags bit mask in order to build a Options String that is
    * compatible with the L2CAP specification.  The Options string is
    * variable string of options that begin with an Option Flag, Option
    * length and Option Data for each option in the list.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param LCID Local CID value referencing the logical channel being
    * configured.
    * @param Result Parameter that indicates the result of the
    * Configuration Request.
    * @param ConfigResponse Structure containing the configuration
    * parameter being negotiated.
    * @return Zero (0) if successful submitting the Configuration
    * Response. This does not mean that the configuration response has been
    * delivered, but that the response was successfully submitted for delivery.
    * Negative if an Error occurred and the Response was not submitted.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Config_Response(unsigned int BluetoothStackID, Word_t LCID, Word_t Result, L2CA_Config_Response_t *ConfigResponse);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Config_Response_t)(unsigned int BluetoothStackID, Word_t LCID, Word_t Result, L2CA_Config_Response_t *ConfigResponse);
#endif

   /*! @brief The following function is responsible for requesting the
    * Disconnection of a logical L2CAP Connection that has previously
    * been established (either created or accepted).
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param LCID Local CID value referencing the logical channel to be disconnected.
    * @return This function returns a zero value if the
    * L2CAP Connection was terminated, or a negative, return error
    * code if unsuccessful.  If this function completes successfully
    * then an L2CAP Disconnect Indication will be sent to the L2CAP
    * Callback Function that was handling the L2CAP Events for this
    * connection.
    * \note It should be noted that after this function is called
    *          (if it completes successfully), the LCID is NO Longer
    *          able to be used (i.e. Logical Connection is terminated).
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Disconnect_Request(unsigned int BluetoothStackID, Word_t LCID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Disconnect_Request_t)(unsigned int BluetoothStackID, Word_t LCID);
#endif

   /*! @brief The following function is responsible for Responding to a L2CAP
    * Disconnect Indication Event.  This function allows the recipient
    * of the L2CAP Event the option of Responding to the Disconnection
    * Indication.  There is no option provided to reject or deny the
    * request. This function should be called ONLY upon receipt of an
    * L2CAP Disconnect Indication Event Callback.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param LCID Local CID value used by the L2CAP layer to reference
    * the logical channel to disconnect.
    * @return This function returns
    * zero if the L2CAP Disconnection Response was successfully
    * submitted, or a negative return error code if the Response was NOT
    * successfully submitted.
    * \note A successful return value (zero) does NOT mean that the
    *          disconnection response was sent, only that the
    *          Disconnection Response has been successfully submitted to
    *          the L2CAP Layer associated with the Local Bluetooth
    *          Protocol Stack.  Any further use of the specified CID
    *          will be rejected by the stack.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Disconnect_Response(unsigned int BluetoothStackID, Word_t LCID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Disconnect_Response_t)(unsigned int BluetoothStackID, Word_t LCID);
#endif

   /*! @brief The following function is responsible for requesting the creation
    * of a Dynamic LE L2CAP Channel with the specified Bluetooth Board
    * Address. @param BluetoothStackID The first parameter is the Bluetooth Stack ID
    * of the Bluetooth Stack to create the L2CAP Connection.
    * @param BD_ADDR The second parameter to this function is the Bluetooth Board Address of the
    * Bluetooth Device to establish the L2CAP Connection with.
    * @param LE_PSM The third parameter is the LE_PSM Number of the Protocol to request
    * connection with (on the remote side). @param ChannelParameters The fourth parameter contains
    * the local device's channel parameters to use with the channel.
    * @param L2CA_Event_Callback The fifth parameter is the L2CAP Callback Function.
    * @param CallbackParameter The final parameter is the Callback Parameter that will be associated with ALL L2CAP Events
    * that occur related to the requested Connection.
    * @return This function returns a positive, non-zero Local Channel Identifier (LCID) if
    * the L2CAP Connection Request was issued successfully, or a
    * negative, return error code indicating an error.
    * \note If this function is successful, then all further Status
    *          and Event Notifications will occur through the Callback
    *          Function that is specified with this function.
    * \note A Positive return value (LCID) does NOT mean that a
    *          connection already exists, only that the Connection
    *          Request has been successfully submitted.
    * \note The ACL connection to the remote device must already
    *          exist before calling this function.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_LE_Connect_Request(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t LE_PSM, L2CA_LE_Channel_Parameters_t *ChannelParameters, L2CA_Event_Callback_t L2CA_Event_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_LE_Connect_Request_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t LE_PSM, L2CA_LE_Channel_Parameters_t *ChannelParameters, L2CA_Event_Callback_t L2CA_Event_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is responsible for Responding to a L2CAP LE
    * Connect Indication Event.  This function allows the recipient of
    * the L2CAP Event the option of Responding to the Connection
    * Indication with a Positive Accept, or a Negative Accept.  This
    * function should be called ONLY upon receipt of an L2CAP LE Connect
    * Indication Event Callback. @param BluetoothStackID The input parameter to this function
    * are the Bluetooth Protocol Stack ID. @param BD_ADDR The second parameter is the Bluetooth Board Address
    * of the Bluetooth Device Requesting the L2CAP Connection. @param Identifier
    * The third parameter is the L2CAP
    * Identifier (this value was passed to the caller in the L2CAP
    * Connect Indication Event).
    * @param LCID The fourth parameter is the Local CID value used by the L2CAP layer to reference
    * the logical channel to disconnect.
    * @param Result The fifth parameter is the L2CAP LE Connection Accept
    * Result (Accept, Pending, or Rejection Reason).
    * @param ChannelParameters The final parameter is the
    * local device's channel parameters to use with the channel (if the
    * connection request is being accepted).  @return This function returns zero
    * if the L2CAP Connection Response was successfully submitted, or a
    * negative return error code if the Response was NOT successfully
    * submitted.
    * \note A successful return value (zero) does NOT mean that the
    *          connection response was sent, only that the Connection
    *          Response has been successfully submitted to the L2CAP
    *          Layer associated with the Local Bluetooth Protocol Stack.
    * \note The ChannelParameters parameter is only required when the
    *          Result is set to
    *          L2CAP_LE_CONNECT_RESULT_CONNECTION_SUCCESSFUL (i.e the
    *          connection is being accepted).
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_LE_Connect_Response(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t Identifier, Word_t LCID, Word_t Result, L2CA_LE_Channel_Parameters_t *ChannelParameters);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_LE_Connect_Response_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t Identifier, Word_t LCID, Word_t Result, L2CA_LE_Channel_Parameters_t *ChannelParameters);
#endif

   /*! @brief The following function is responsible for requesting the
    * Disconnection of a logical L2CAP LE Connection that has previously
    * been established (either created or accepted). @param BluetoothStackID The first
    * parameter to this function is the Bluetooth stack ID of the
    * Bluetooth Stack that is maintaining the L2CAP LE Connection.
    * @param LCID The second parameter is the Local Channel Identifier (LCID) of the Logical L2CAP LE
    * Connection to terminate. @return This function returns a zero value if
    * the L2CAP LE Connection was terminated, or a negative, return
    * error code if unsuccessful.  If this function completes
    * successfully then an L2CAP LE Disconnect Indication will be sent
    * to the L2CAP Callback Function that was handling the L2CAP Events
    * for this connection.
    * \note It should be noted that after this function is called (if
    *          it completes successfully), the LCID is NO Longer able to
    *          be used (i.e.  Logical Connection is terminated).
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_LE_Disconnect_Request(unsigned int BluetoothStackID, Word_t LCID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_LE_Disconnect_Request_t)(unsigned int BluetoothStackID, Word_t LCID);
#endif

   /*! @brief The following function is responsible for Responding to a L2CAP LE
    * Disconnect Indication Event.  This function allows the recipient
    * of the L2CAP Event the option of Responding to the Disconnection
    * Indication. There is no option provided to reject or deny the
    * request. This function should be called ONLY upon receipt of an
    * L2CAP Disconnect Indication Event Callback. @param BluetoothStackID The first
    * parameter to this function are the Bluetooth Protocol Stack ID.
    * @param LCID The second parameter is the L2CAP Identifier (this value was passed to the caller in the
    * caller in the L2CAP LE Disconnect Indication Event).
    * @return This function returns zero if the L2CAP LE Disconnection Response was
    * successfully submitted, or a negative return error code if the
    * Response was NOT successfully submitted.
    * \note A successful return value (zero) does NOT mean that the
    *          disconnection response was sent, only that the
    *          Disconnection Response has been successfully submitted to
    *          the L2CAP Layer associated with the Local Bluetooth
    *          Protocol Stack.  Any further use of the specified CID
    *          will be rejected by the stack.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_LE_Disconnect_Response(unsigned int BluetoothStackID, Word_t LCID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_LE_Disconnect_Response_t)(unsigned int BluetoothStackID, Word_t LCID);
#endif

   /*! @brief The following function is responsible for Sending L2CAP Data to
    * the specified L2CAP Connection.  The L2CAP Connection ID (LCID)
    * that is passed to this function MUST have been established
    * by either Accepting an L2CAP Connection or Initiating an L2CAP
    * Connection (and having the other side accept the Connection).
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param LCID Local CID value used by the L2CAP layer to reference the
    * logical channel on which to send the data.
    * @param Data_Length Number of characters to be sent over the channel.
    * @param Data Pointer to a buffer of data to be sent over the channel.
    * @return This function returns a zero if the data was successfully sent, or
    * a negative return error code if unsuccessful.
    * \note @return If this function returns the Error Code:
    *
    *             BTPS_ERROR_INSUFFICIENT_BUFFER_SPACE
    *
    *          then this is a signal to the caller that the requested
    *          data could NOT be sent because the requested data could
    *          not be queued in the Outgoing L2CAP Queue.  The caller
    *          then, must wait for the
    *
    *             etChannel_Buffer_Empty_Indication
    *
    *          Event before trying to send any more data.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Data_Write(unsigned int BluetoothStackID, Word_t LCID, Word_t Data_Length, Byte_t *Data);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Data_Write_t)(unsigned int BluetoothStackID, Word_t LCID, Word_t Data_Length, Byte_t *Data);
#endif

   /*! @brief The following function is responsible for Sending L2CAP Data to
    * the specified L2CAP Connection.  This function provides additional
    * functionallity to control the amount of buffer usage for each data
    * channel .The L2CAP Connection ID (LCID) that is passed to this
    * function MUST have been established by either Accepting an L2CAP
    * Connection or Initiating an L2CAP Connection (and having the other
    * side accept the Connection).
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param LCID Local CID value used by the L2CAP layer to reference the
    * logical channel on which to send the data.
    * @param QueueingParameters Optional pointer to a structure which describes
    * the parameters that dictate how the packet is queued.
    * @param Data_Length Number of characters to be sent over the channel.
    * @param Data Pointer to a buffer of data to be sent over the channel.
    * @return This function returns a
    * zero if the data was successfully sent, or a negative return error
    * code if unsuccessful.
    * \note @return If this function returns the Error Code:
    *
    *             BTPS_ERROR_INSUFFICIENT_BUFFER_SPACE
    *
    *          then this is a signal to the caller that the requested
    *          data could NOT be sent because the requested data could
    *          not be queued in the Outgoing L2CAP Queue.  The caller
    *          then, must wait for the
    *
    *             etChannel_Buffer_Empty_Indication
    *
    *          Event before trying to send any more data.
    * \note If this function is called with QueueingParameters set to
    *          NULL, then this function operates like the legacy
    *          L2CA_Data_Write function.
    * \note If this function is called with a non-NULL
    *          QueueingParameters then the data is queued conditionally.
    *          If successful, the return value will indicate the number
    *          of packets/bytes that are currently queued on the
    *          specified channel at the time the function returns.
    */
BTPSAPI_DECLARATION long BTPSAPI L2CA_Enhanced_Data_Write(unsigned int BluetoothStackID, Word_t LCID, L2CA_Queueing_Parameters_t *QueueingParameters, Word_t Data_Length, Byte_t *Data);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef long (BTPSAPI *PFN_L2CA_Enhanced_Data_Write_t)(unsigned int BluetoothStackID, Word_t LCID, L2CA_Queueing_Parameters_t *QueueingParameters, Word_t Data_Length, Byte_t *Data);
#endif

   /*! @brief The following function is responsible for Sending L2CAP Data to
    * the specified L2CAP Connection over a specified Fixed Channel.
    * The L2CAP Connection ID (FCID) that is passed to this function
    * identifies the Fixed Channel on which the data is to be sent.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * protocol stack via a call to BSC_Initialize.
    * @param BD_ADDR Bluetooth device address of the device to send the specified
    * fixed channel data.
    * @param FCID Fixed channel ID that represents the fixed channel to send the
    * data. This value is not the actual fixed channel itself, rather
    * this a value that was returned from a successful call to the
    * L2CA_Register_Fixed_Channel function.
    * @param Data_Length Number of characters to be sent over the fixed channel.
    * @param Data Pointer to a buffer of data to be sent over the fixed channel.
    * @return This function returns a zero if the data
    * was successfully sent, or a negative return error code if
    * unsuccessful.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Fixed_Channel_Data_Write(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t FCID, Word_t Data_Length, Byte_t *Data);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Fixed_Channel_Data_Write_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t FCID, Word_t Data_Length, Byte_t *Data);
#endif

   /*! @brief The following function is responsible for Sending L2CAP Data to
    * the specified L2CAP Connection over a specified Fixed Channel.
    * The L2CAP Connection ID (FCID) that is passed to this function
    * identifies the Fixed Channel on which the data is to be sent.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Bluetooth device address of the device to send the
    * specified fixed channel data.
    * @param FCID Fixed channel ID that represents the fixed channel to
    * send the data.  This value is not the actual fixed channel itself,
    * rather this a value that was returned from a successful call to
    * the L2CA_Register_Fixed_Channel function.
    * @param QueueingParameters Optional pointer to a structure which
    * describes the parameters that dictate how the packet is queued.
    * @param Data_Length Number of characters to be sent over the fixed channel.
    * @param Data Pointer to a buffer of data to be sent over the fixed channel.
    * @return This function returns a zero if the
    * data was successfully sent, or a negative return error code if
    * unsuccessful.
    * \note If this function returns the Error Code:
    *
    *             BTPS_ERROR_INSUFFICIENT_BUFFER_SPACE
    *
    *          then this is a signal to the caller that the requested
    *          data could NOT be sent because the requested data could
    *          not be queued in the Outgoing L2CAP Queue.  The caller
    *          then, must wait for the
    *
    *             etFixed_Channel_Buffer_Empty_Indication
    *
    *          Event before trying to send any more data.
    * \note If this function is called with QueueingParameters set to
    *          NULL, then this function operates like the legacy
    *          L2CA_Fixed_Channel_Data_Write() function.
    * \note If this function is called with a non-NULL
    *          QueueingParameters then the data is queued conditionally.
    *          If successful, the return value will indicate the number
    *          of packets that are currently queued on the specified
    *          channel at the time the function returns.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Enhanced_Fixed_Channel_Data_Write(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t FCID, L2CA_Queueing_Parameters_t *QueueingParameters, Word_t Data_Length, Byte_t *Data);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Enhanced_Fixed_Channel_Data_Write_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t FCID, L2CA_Queueing_Parameters_t *QueueingParameters, Word_t Data_Length, Byte_t *Data);
#endif

   /*! @brief The following function is responsible for Sending L2CAP Data to
    * the specified L2CAP LE Connection Oriented channel.  This function
    * provides additional functionallity to control the amount of buffer
    * usage for each data channel. The L2CAP Connection ID (LCID) that
    * is passed to this function MUST have been established by either
    * Accepting an L2CAP LE Connection or Initiating an L2CAP LE
    * Connection (and having the other side accept the Connection). @param BluetoothStackID The
    * input parameters to this function are the Bluetooth Stack ID of
    * the Bluetooth Stack that @param LCID The second parameter is valid for (LCID -
    * Logical Connection Channel Identifier). @param QueueingParameters The third parameter
    * is a pointer to a structure that contains conditional queueing parameters.
    * @param Data_Length The fourth parameter is the Length of the
    * Data to send. @param Data The fifth parameter is a pointer to the Data Buffer to Send.
    * @return This function returns a zero if the data was successfully sent, or a
    * negative return error code if unsuccessful.
    * \note If @return This function returns the Error Code:
    *
    *             BTPS_ERROR_INSUFFICIENT_BUFFER_SPACE
    *
    *          then this is a signal to the caller that the requested
    *          data could NOT be sent because the requested data could
    *          not be queued in the Outgoing L2CAP Queue.  The caller
    *          then, must wait for the
    *
    *             etLE_Channel_Buffer_Empty_Indication
    *
    *          Event before trying to send any more data.
    * \note If this function is called with a non-NULL
    *          QueueingParameters then the data is queued conditionally.
    *          If successful, the return value will indicate the number
    *          of packets/bytes that are currently queued on the
    *          specified channel at the time the function returns.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Enhanced_Dynamic_Channel_Data_Write(unsigned int BluetoothStackID, Word_t LCID, L2CA_Queueing_Parameters_t *QueueingParameters, Word_t Data_Length, Byte_t *Data);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Enhanced_Dynamic_Channel_Data_Write_t)(unsigned int BluetoothStackID, Word_t LCID, L2CA_Queueing_Parameters_t *QueueingParameters, Word_t Data_Length, Byte_t *Data);
#endif

   /*! @brief The following function is responsible for Sending L2CAP Data to
    * the specified Group.  The L2CAP Connection ID (LCID) references
    * a Group and the members of the group.  A group need not have any
    * members, but if members are assigned to the group, a message is
    * formatted and sent to each member of the group.  The data is set
    * to each member on CID 0002, which is the Connectionless Data
    * channel.  Along with the data, the Destination PSM is included to
    * direct the connectionless data to the proper endpoint.  When a
    * member is added to the group, a connection to the BD_ADDR is
    * established.  However, the connection may have been terminated
    * some time after the member was added, and this function will make
    * no effort to verify that a connection still exists prior to
    * sending the data.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param LCID Local CID value used by the L2CAP layer to reference the
    * Group to which to send the data.  This values is obtained from a
    * successful call to L2CA_Group_Create.
    * @param Data_Length Number of characters to be sent to the group.
    * @param Data Pointer to a buffer of data to be sent to the group.
    * @return This function returns a zero if the data was
    * successfully sent, or a negative return error code if
    * unsuccessful.
    * \note  If this function returns the Error Code:
    *
    *             BTPS_ERROR_INSUFFICIENT_BUFFER_SPACE
    *
    *          then this is a signal to the caller that the requested
    *          data could NOT be sent because the requested data could
    *          not be queued in the Outgoing L2CAP Queue.  The caller
    *          then, must wait for the
    *
    *             etChannel_Buffer_Empty_Indication
    *
    *          Event before trying to send any more data.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Group_Data_Write(unsigned int BluetoothStackID, Word_t LCID, Word_t Data_Length, Byte_t *Data);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Group_Data_Write_t)(unsigned int BluetoothStackID, Word_t LCID, Word_t Data_Length, Byte_t *Data);
#endif

   /*! @brief The following function is responsible for Initiating an L2CAP
    * Echo Request to the specified Bluetooth Board Address.  This
    * function allows the caller to specify data (not required) that is
    * be sent to the specified Bluetooth Board Address and returned
    * (hence the name Echo Request).
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Device address of the Bluetooth device to which an
    * L2CAP logical channel is to be established.
    * @param Data_Length Number of characters to be sent with the Ping.
    * @param Data Pointer to a buffer of data to be sent with the Ping.
    * @param L2CA_Event_Callback Pointer to a callback function to be used
    * by the L2CAP layer to dispatch a reply to the Ping.
    * @param CallbackParameter User defined value to be used by the L2CAP
    * layer as an input parameter for the callbacks.
    * @return This function returns a positive, non-zero
    * return value if the L2CAP Echo Request was successfully submitted,
    * or a negative return error value if unsuccessful.
    * \note A successful return value (zero) does NOT mean that the
    *          L2CA Echo Request was sent, only that the L2CA Echo
    *          Request has been successfully submitted to the L2CAP
    *          Layer associated with the Local Bluetooth Protocol Stack.
    * \note Upon successful completion of this function, the caller
    *          will have to process the result of the Echo Request in
    *          the L2CA Event Callback that was specified as input to
    *          this function.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Ping(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t Data_Length, Byte_t *Data, L2CA_Event_Callback_t L2CA_Event_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Ping_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t Data_Length, Byte_t *Data, L2CA_Event_Callback_t L2CA_Event_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is responsible for Initiating an L2CAP Info
    * Request to the specified Bluetooth Board Address.  This function
    * allows the caller to specify the Info Parameter that is be sent to
    * the specified Bluetooth Board Address.  The return Information
    * will be received in an Information Confirmation Event.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Device address of the Bluetooth device to which an L2CAP
    * logical channel is to be established.
    * @param Info_Type Identifier of the information element to be retrieved.
    * @param L2CA_Event_Callback Pointer to a callback function to be used by
    * the L2CAP layer to dispatch a reply to the Info Request.
    * @param CallbackParameter User defined value to be used by the L2CAP layer
    * as an input parameter for the callbacks.
    * @return This function returns a positive,
    * non-zero return value if the L2CA Get Info Request was
    * successfully submitted or a negative return error value if
    * unsuccessful.
    * \note A successful return value (zero) does NOT mean that the
    *          L2CA Get Info Request was sent, only that the L2CA Get
    *          Info Request has been successfully submitted to the L2CAP
    *          Layer associated with the Local Bluetooth Protocol Stack.
    * \note Upon successful completion of this function, the caller
    *          will have to process the result of the Info Request in
    *          the L2CA Event Callback that was specified as input to
    *          this function.
    * \note If a connection to the specified Board Address does not
    *          exist at the time the request is made, the request will
    *          be suspended until a connection to the device has been
    *          established.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Get_Info(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t Info_Type, L2CA_Event_Callback_t L2CA_Event_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Get_Info_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t Info_Type, L2CA_Event_Callback_t L2CA_Event_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is responsible for Initiating a request to
    * Update Connection Parameters.  This function should only be called
    * from an LE Slave.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Device address of the Bluetooth device to which an L2CAP logical channel is already established.
    * @param IntervalMin Minimum value for the the connection interval.  This should fall within the range:
    * HCI_LE_CONNECTION_INTERVAL_MINIMUM and HCI_LE_CONNECTION_INTERVAL_MAXIMUM
    * @param IntervalMax This should be greater than or equal to Conn_Interval_Min and shall fall within the range:
    * HCI_LE_CONNECTION_INTERVAL_MINIMUM and HCI_LE_CONNECTION_INTERVAL_MAXIMUM
    * Both intervals follow the rule:
    * Time = N * .25 msec
    * @param SlaveLatency Slave latency for connection. This should be in range:
    * HCI_LE_CONNECTION_LATENCY_MINIMUM and  HCI_LE_CONNECTION_LATENCY_MAXIMUM
    * @param TimeoutMultiplier Supervision timeout multiplier for LE link.
    * This should be in range:
    * HCI_LE_SUPERVISION_TIMEOUT_MINIMUM and HCI_LE_SUPERVISION_TIMEOUT_MAXIMUM
    * The Supervision_Timeout follows the rule:
    * Time = N * 0 msec
    * @return This function returns a
    * positive, non-zero return value if the request was successfully
    * submitted or a negative return error value if unsuccessful.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Connection_Parameter_Update_Request(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t IntervalMin, Word_t IntervalMax, Word_t SlaveLatency, Word_t TimeoutMultiplier);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Connection_Parameter_Update_Request_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t IntervalMin, Word_t IntervalMax, Word_t SlaveLatency, Word_t TimeoutMultiplier);
#endif

   /*! @brief The following function is responsible for Initiating an L2CAP
    * response to an Update Connection Parameters request.  This
    * function should only be called from an LE Masters.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Device address of the Bluetooth device to which an
    * L2CAP logical channel is already established.
    * @param Result Result of the connection parameter update request.
    * @return This function returns a
    * positive, non-zero return value if the request was successfully
    * submitted or a negative return error value if unsuccessful.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Connection_Parameter_Update_Response(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t Result);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Connection_Parameter_Update_Response_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t Result);
#endif

   /*! @brief The following function is responsible for processing the L2CA
    * Group Create.  A Group is created in order to send and receive
    * Connectionless data from other devices.  When a Group is Created,
    * a PSM value is specified.  This value provides a filter for the
    * various Connectionless Data packets that arrive at the local
    * device.  Only those Group Messages that are destined for the PSM
    * specified during the group create will be sent up the stack.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param PSM Protocol/Service Multiplexer identifier of the Group messages to be received.
    * @param RxEnabled Flag to controls the state of the receiver a creation.
    * If this is TRUE, reception of the group messages is enabled.
    * If FALSE, group messages are disabled.
    * @param L2CA_Event_Callback Pointer to a callback function to be used
    * by the L2CAP layer to dispatch group messages.
    * @param CallbackParameter User defined value to be used by the L2CAP
    * layer as an input parameter for the callbacks.
    * @return A positive, non-Zero value is returned after successfully creating the group.
    * This value is the Group CID and is used to identify the group when
    * future modifications to the group are made.
    * Negative if an Error occurred and the group was not created.
    * \note A Group is created in order to receive group data.  No
    *          Group Members are required in order to receive messages.
    *          Group Members are requires to send data to a specified
    *          Group.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Group_Create(unsigned int BluetoothStackID, Word_t PSM, Boolean_t RxEnabled, L2CA_Event_Callback_t L2CA_Event_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Group_Create_t)(unsigned int BluetoothStackID, Word_t PSM, Boolean_t RxEnabled, L2CA_Event_Callback_t L2CA_Event_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is responsible for processing the L2CA
    * Group Close.  When a Group is closed, the Group Owner will no
    * longer be able to receive Group Messages for the Group PSM.  If
    * the Group contains members, each group member is removed from the
    * group and connections to each group member will be terminated if
    * the connection is not required for any other purpose.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param CID Connection Identifier that uniquely identifies the Group.
    * @return If the removal of the Group was successful, the return value will be
    * a non-negative value. A negative return value indicates a failure
    * to remove the Group.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Group_Close(unsigned int BluetoothStackID, Word_t CID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Group_Close_t)(unsigned int BluetoothStackID, Word_t CID);
#endif

   /*! @brief The following function is used to Add a Member to an existing
    * Group.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param CID Connection Identifier that uniquely identifies the Group.
    * @param BD_ADDR Device address of the Bluetooth device to be added to the group.
    * @return A negative return value from this
    * function indicates the Request to Add member failed to complete.
    * A non-negative result indicates the request was successfully
    * submitted.
    * \note A non-negative result does not indicate the member was
    *          added, but that the request to add the member was
    *          submitted.  Information about the success or failure of
    *          the addition of the member will be received later via
    *          the Callback function that was supplied when the Group
    *          was created.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Group_Add_Member(unsigned int BluetoothStackID, Word_t CID, BD_ADDR_t BD_ADDR);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Group_Add_Member_t)(unsigned int BluetoothStackID, Word_t CID, BD_ADDR_t BD_ADDR);
#endif

   /* This function is used to Remove a Group Member from the Member
    * List.  If a member is removed from the list and the connection to
    * the device is no longer needed, then the connection to the device
    * removed will be terminated.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param CID Connection Identifier that uniquely identifies the Group.
    * @param BD_ADDR Device address of the Bluetooth device to be removed
    * from the group.
    * @return Zero (0) if the member was successfully removed.
    * Negative if an Error occurred and the member was not added.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Group_Remove_Member(unsigned int BluetoothStackID, Word_t CID, BD_ADDR_t BD_ADDR);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Group_Remove_Member_t)(unsigned int BluetoothStackID, Word_t CID, BD_ADDR_t BD_ADDR);
#endif

   /*! @brief The following function is used to obtain a list of Members to an
    * existing Group.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack
    * via a call to BSC_Initialize.
    * @param CID Connection Identifier that uniquely identifies the Group.
    * @param Result Pointer to an integer to receive status information for the request.
    * The currently defined result values are:
    * L2CAP_GROUP_MEMBERSHIP_RESPONSE_RESULT_SUCCESS or L2CAP_GROUP_MEMBERSHIP_RESPONSE_RESULT_FAILURE
    * @param MemberCount Pointer to an integer to receive a count of the number of member
    * entries that were moved to the BD_ADDR array.
    * @param BufferSize Size in Bytes of the BD_ADDR buffer that will receive the array
    * of member addresses.
    * @param BD_ADDR Pointer to an array of type BD_ADDR_t.  The function will fill the
    * array with the device address of each member of the group.
    * @return A negative result indicates an error occurred while executing the request.
    * \note A non-negative result from the function does not indicate
    *          that the function was successful.  A Non-Negative result
    *          indicates that the parameters were acceptable to complete
    *          the request. The Result value should be examined to
    *          determine if the command was successfully completed.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Get_Group_Membership(unsigned int BluetoothStackID, Word_t CID, unsigned int *Result, unsigned int *MemberCount, unsigned int BufferSize, BD_ADDR_t *BD_ADDR);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Get_Group_Membership_t)(unsigned int BluetoothStackID, Word_t CID, unsigned int *Result, unsigned int *MemberCount, unsigned int BufferSize, BD_ADDR_t *BD_ADDR);
#endif

   /*! This function is used to Enable the reception of Group Messages.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param PSM Protocol/Service Multiplexer identifier of the Group PSM
    * message to be enabled.
    * @return A Non-Negative return value for the
    * function indicates the messages were successfully enabled.
    * \note A CID is used to reference a Group and a PSM is used to
    *          reference a Group Message.  Thus, there may be multiple
    *          Groups whom have registered for the particular PSM
    *          Message.  Thus, enabling a PSM Message may effect
    *          multiple Groups.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Enable_CLT(unsigned int BluetoothStackID, Word_t PSM);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Enable_CLT_t)(unsigned int BluetoothStackID, Word_t PSM);
#endif

   /*! This function is used to Disable the reception of Group Messages.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param PSM Protocol/Service Multiplexer identifier of the Group PSM message to be disabled.
    * @return A Non-Negative return value for the
    * function indicates the messages were successfully disabled.
    * \note A CID is used to reference a Group and a PSM is used to
    *          reference a Group Message.  Thus, there may be multiple
    *          Groups whom have registered for the particular PSM
    *          Message.  Thus, disabling a PSM Message may effect
    *          multiple Groups.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Disable_CLT(unsigned int BluetoothStackID, Word_t PSM);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Disable_CLT_t)(unsigned int BluetoothStackID, Word_t PSM);
#endif

   /*! @brief The following function is responsible for flushing all data
    * that is current queued for transmission for the specified CID.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param CID Local CID value referencing the logical channel to be flushed.
    * @return This function will return Zero if
    * successful, otherwise the function will return a negative error
    * code.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Flush_Channel_Data(unsigned int BluetoothStackID, Word_t CID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Flush_Channel_Data_t)(unsigned int BluetoothStackID, Word_t CID);
#endif

   /*! @brief The following function is responsible for flushing all data that
    * is current queued for transmission for the specified LE CID.
    * @param BluetoothStackID The first parameter is the Bluetooth Stack ID that
    * identifies the stack in which the data is currently queued.
    * @param CID The second parameter is the Local L2CAP LE CID that identifies the channel on which the
    * data is currently queued.  This function will return Zero if
    * successful, otherwise the function will return a negative error
    * code.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_LE_Flush_Channel_Data(unsigned int BluetoothStackID, Word_t CID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_LE_Flush_Channel_Data_t)(unsigned int BluetoothStackID, Word_t CID);
#endif

   /*! @brief The following function is responsible for allowing a mechanism of
    * granting the specified amount of credits for the specified LE
    * CID. @param BluetoothStackID The first parameter is the Bluetooth Stack
    * ID that identifies the stack in which the data is currently
    * queued. @param CID The second parameter is the Local L2CAP LE CID that identifies the channel on
    * which to grant credits. @param CreditsToGrant The third parameter is the total amount of credits to grant.
    * @return This function will return zero if successful, otherwise the
    * function will return a negative error code.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_LE_Grant_Credits(unsigned int BluetoothStackID, Word_t CID, Word_t CreditsToGrant);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_LE_Grant_Credits_t)(unsigned int BluetoothStackID, Word_t CID, Word_t CreditsToGrant);
#endif

   /*! @brief The following function is responsible for retrieving Config
    * information on a specified CID.  The call must supply a pointer to
    * memory for this routine to pass the configuration information.
    * When a channel is created, the configuration parameters are set to
    * default values, and are updated during the configuration process
    * to reflect non-default values that are negotiated. The config
    * information only applies to Normal Channel types. Other channel
    * type do not configuration parameters.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param CID Channel Identifier.
    * @param Channel_Config_Params Pointer to a structure to receive the
    * configuration information.
    * @return Zero (0) if the information was successfully transferred.
    * An error code if negative; one of the following values:
    * BTPS_ERROR_INVALID_BLUETOOTH_STACK_ID
    * BTPS_ERROR_L2CAP_NOT_INITIALIZED
    * BTPS_ERROR_INVALID_PARAMETER
    * BTPS_ERROR_CONNECTION_TO_DEVICE_LOST
    * BTPS_ERROR_INVALID_CID_TYPE
    * BTPS_ERROR_INVALID_CID
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Get_Current_Channel_Configuration(unsigned int BluetoothStackID, Word_t CID, L2CA_Config_Params_t *Channel_Config_Params);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Get_Current_Channel_Configuration_t)(unsigned int BluetoothStackID, Word_t CID, L2CA_Config_Params_t *Channel_Config_Params);
#endif

   /*! @brief The following function is provided to allow a means to query the
    * current Link Connection Request/Response Configuration.  The Link
    * Request/Response Configuration parameters mandate how L2CAP is to
    * issue Lower Link Requests and respond to Lower Link Requests
    * during connection setup.  These parameters primarily mandate how
    * Master/Slave Role Switching is to be supported in L2CAP (if L2CAP
    * is managing ALL Lower Level Connection Request/Acceptance (which
    * is the default).
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param L2CA_Link_Connect_Params Stack connection configuration values.
    * @return This function returns zero if successful or a
    * negative return value if an error occurred.
    * \note This function, and it's counterpart, the
    *          L2CAP_Set_Link_Connection_Configuration() function, are
    *          really only of use when Point to Multi-Point usage is
    *          required (either Local or Remote).  The default values
    *          are:
    *
    *             cqNoRoleSwitch
    *             csMaintainCurrentRole
    *
    *          which means that if a connection is requested by L2CAP
    *          the Local Device will NOT allow a Role switch and will
    *          remain the Master of the Connection.  If a connection is
    *          responded to by L2CAP then NO Role Switch will be
    *          attempted (the Local Device will remain the Slave).
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Get_Link_Connection_Configuration(unsigned int BluetoothStackID, L2CA_Link_Connect_Params_t *L2CA_Link_Connect_Params);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Get_Link_Connection_Configuration_t)(unsigned int BluetoothStackID, L2CA_Link_Connect_Params_t *L2CA_Link_Connect_Params);
#endif

   /*! @brief The following function is provided to allow a means to change the
    * current Link Connection Request/Response Configuration.  The Link
    * Request/Response Configuration parameters mandate how L2CAP is to
    * issue Lower Link Requests and respond to Lower Link Requests
    * during connection setup.  These parameters primarily mandate how
    * Master/Slave Role Switching is to be supported in L2CAP (if L2CAP
    * is managing ALL Lower Level Connection Request/Acceptance (which
    * is the default).
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param L2CA_Link_Connect_Params Stack connection configuration values.
    * @return This function returns zero if successful or a negative return
    * value if an error occurred.
    * \note After this function has completed, the parameters will
    *          ONLY be in effect when the next Lower Level Link Action
    *          occurs (i.e. for future Link Connections).  Any current
    *          L2CAP Links that are established are un-affected by the
    *          action of this function.
    * \note This function, and it's counterpart, the
    *          L2CAP_Get_Link_Connection_Configuration() function, are
    *          really only of use when Point to Multi-Point usage is
    *          required (either Local or Remote).  The default values
    *          are:
    *
    *             cqNoRoleSwitch
    *             csMaintainCurrentRole
    *
    *          which means that if a connection is requested by L2CAP
    *          the Local Device will NOT allow a Role switch and will
    *          remain the Master of the Connection.  If a connection is
    *          responded to by L2CAP then NO Role Switch will be
    *          attempted (the Local Device will remain the Slave).
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Set_Link_Connection_Configuration(unsigned int BluetoothStackID, L2CA_Link_Connect_Params_t *L2CA_Link_Connect_Params);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Set_Link_Connection_Configuration_t)(unsigned int BluetoothStackID, L2CA_Link_Connect_Params_t *L2CA_Link_Connect_Params);
#endif

   /*! @brief The following function is provided to allow a mechanism to
    * determine the current connection state of the specified link
    * (specified by device address).
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param BD_ADDR Bluetooth device address of the Bluetooth link to query
    * the current state.
    * @param L2CA_Link_Connection_State Link connection state.
    * @return This function returns zero if
    * successful (and fills in the specified Link Connection Status
    * parameter), or a negative return error code if there was an error.
    * \note This function should be called with the stack lock held
    *          to guard against the state being returned to be invalid
    *          (due to race condition/timing).
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Get_Link_Connection_State(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, L2CA_Link_Connection_State_t *L2CA_Link_Connection_State);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Get_Link_Connection_State_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, L2CA_Link_Connection_State_t *L2CA_Link_Connection_State);
#endif

   /*! @brief The following function is responsible for retrieving the current
    * L2CAP Channel Queing Threshold information for the Bluetooth
    * Stack L2CAP Module.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param L2CA_Channel_Queue_Threshold The retrieved Channel Queue Threshold.
    * The SizeThreshold is the minimum size in bytes of an individual L2CAP ACL Segment.
    * The DepthThreshold is the number of packets of SizeThreshold that are allowed.
    * A DepthThreshold of zero means that this functionality is disabled.
    * The LowQueueThreshold parameter specifies the lower threshold of the
    * number of packets in the queue that must be met before a Channel empty
    * indication event is dispatched.
    * @return This function returns the current L2CAP
    * Channel Queue Threshold Information.  This information is used by
    * the L2CAP Module to artificially limit the amount of Data that the
    * L2CAP Module will buffer internally (per L2CAP Channel).  This
    * will help alleviate the case where L2CAP always accepts data to
    * be written when memory is available, which can lead to complete
    * memory allocation usage (in the future).  The two parameters in
    * the Queue Threshold structure specify the Minimum Size (in Bytes)
    * of an individual L2CAP ACL Segment that should be used as a
    * check, and the Depth (the amount of these packet sizes) that is
    * allowed. This function returns the current L2CAP Queue Threshold
    * values. This function returns zero if successful (and fills in
    * the specified Threshold Information structure), or a negative
    * return error code if there was an error.
    * \note This mechanism has been DEPRECATED AND IS NO LONGER
    *          SUPPORTED!
    * \note This functionality has been SUPERCEDED by the
    *          L2CA_Enhanced_Data_Write() function!
    * \note An L2CAP ACL Queue Threshold Depth of zero means that
    *          NO Threshold limits are to be used (i.e. this
    *          functionality is disabled).  A non-zero value specifies
    *          the number of segments (greater than or equal to) the
    *          Threshold Size that can be queued per L2CAP Channel.
    * \note The L2CAP ACL Queue Threshold size is used to affect
    *          the Threshold Depth.  Only queued segments that are
    *          larger than the specified size are used to count the
    *          Threshold Depth.  This means that smaller packets can be
    *          allowed to always be sent, while larger ones can be
    *          guarded.
    * \note The Threshold values are set globally and reflect the
    *          Threshold that will be applied to each individual L2CAP
    *          Channel individually (not all channels).
    * \note This function exists so the caller can query the
    *          current Threshold values.  The default values for both
    *          of the Threshold settings is zero (disabled).  The
    *          caller must use the L2CA_Set_Channel_Queue_Threshold()
    *          API function to change this functionality.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Get_Channel_Queue_Threshold(unsigned int BluetoothStackID, L2CA_Channel_Queue_Threshold_t *L2CA_Channel_Queue_Threshold);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Get_Channel_Queue_Threshold_t)(unsigned int BluetoothStackID, L2CA_Channel_Queue_Threshold_t *L2CA_Channel_Queue_Threshold);
#endif

   /*! @brief The following function is responsible for changing the current
    * L2CAP Channel Queing Threshold information for the Bluetooth
    * Stack L2CAP Module.  This function changes the current L2CAP
    * Channel Queue Threshold Information to match the L2CAP Channel
    * Queue Threshold Information of the Threshold Information that is
    * passed to this function.  This information is used by the L2CAP
    * Module to artificially limit the amount of Data that the L2CAP
    * Module will buffer internally (per L2CAP Channel).  This helps
    * alleviate the case where L2CAP always accepts data to be written
    * (queued) when memory is available, which can lead to complete
    * memory allocation usage (in the future).
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param L2CA_Channel_Queue_Threshold The L2CAP Channel Queing Threshold
    * to change to. The SizeThreshold is the minimum size in bytes of an
    * individual L2CAP ACL Segment. The DepthThreshold is the number of packets
    * of SizeThreshold that are allowed. A DepthThreshold of zero means that this
    * functionality is disabled.  The LowQueueThreshold parameter specifies the lower
    * threshold of the number of packets in the queue that must be met before a Channel
    * empty indication event is dispatched.
    * @return This function returns zero if the information was
    * successfully changed, or a negative return error code if there
    * was an error.
    * \note This mechanism has been DEPRECATED AND IS NO LONGER
    *          SUPPORTED!
    * \note This functionality has been SUPERCEDED by the
    *          L2CA_Enhanced_Data_Write() function!
    * \note An L2CAP ACL Queue Threshold Depth of zero means that
    *          NO Threshold limits are to be used (i.e. this
    *          functionality is disabled).  A non-zero value specifies
    *          the number of segments (greater than or equal to) the
    *          Threshold Size that can be queued per L2CAP Channel.
    * \note The L2CAP ACL Queue Threshold size is used to affect
    *          the Threshold Depth.  Only queued segments that are
    *          larger than the specified size are used to count the
    *          Threshold Depth.  This means that smaller packets can be
    *          allowed to always be sent, while larger ones can be
    *          guarded.
    * \note The Threshold values are set globally and reflect the
    *          Threshold that will be applied to each individual L2CAP
    *          Channel individually (not all channels).
    * \note This function exists so the caller can change the
    *          current Threshold values.  The default values for both
    *          of the Threshold settings is zero (disabled).  The
    *          caller can use the L2CA_Get_Channel_Queue_Threshold()
    *          API function to query the current Threshold values.
    */
BTPSAPI_DECLARATION int BTPSAPI L2CA_Set_Channel_Queue_Threshold(unsigned int BluetoothStackID, L2CA_Channel_Queue_Threshold_t *L2CA_Channel_Queue_Threshold);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_L2CA_Set_Channel_Queue_Threshold_t)(unsigned int BluetoothStackID, L2CA_Channel_Queue_Threshold_t *L2CA_Channel_Queue_Threshold);
#endif

#endif
