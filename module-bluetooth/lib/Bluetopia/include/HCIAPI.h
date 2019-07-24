/*
 * Copyright 2000 - 2014 Stonestreet One.
 * All Rights Reserved.
 * Author:  Damon Lange
 *** MODIFICATION HISTORY ****************************************************
 *
 *   mm/dd/yy  F. Lastname    Description of Modification
 *   --------  -----------    ------------------------------------------------
 *   09/11/00  D. Lange       Initial creation.
 *   12/07/07  D. Mason       Changes for BT 2.1
 *   05/20/11  T. Cook        Changes for BT 4.0
 *   01/02/14  T. Cook        Changes for BT 4.1
 ****************************************************************************
 */

/**
 *  @file HCIAPI.h
 *
 *  @brief Stonestreet One Bluetooth Stack HCI API Type Definitions,
 *      Constants, and Prototypes.
 *      To use any of the following API's,
 *      include the following declaration in your application.
 *
 *  @code
 *  #include "SS1BTPS.h"
 *  @endcode
 *  ============================================================================
 */
#ifndef __HCIAPIH__
#define __HCIAPIH__

#include "BTPSKRNL.h"           /*! BTPS Kernel Prototypes/Constants. */

#include "BTAPITyp.h"           /*! Bluetooth API Type Definitions. */
#include "HCITypes.h"           /*! Bluetooth HCI Type Definitions/Constants. */

#include "BTPSCFG.h"            /*! BTPS Configuration Constants. */


   /*! The following enumerated type is used with the
    * HCI_Version_Supported() function.  These values represent the
    * HCI Layer Specification that the HCI API Layer supports.  Notice
    * that the Version definition for later versions of the Bluetooth
    * HCI Specification are defined as values having a larger
    * numerical value.  This allows simple comparison logic
    * (greater than/less than/equal to/etc. logic to be performed).
    * \note If a Bluetooth HCI Specification value is NOT listed in
    *          this list then it is NOT supported by the HCI API
    *          present in this file.
    */
typedef enum
{
   hvSpecification_1_0B,                /*! HCI Specification 1.0B. */
   hvSpecification_1_1,                 /*! HCI Specification 1.1. */
   hvSpecification_1_2,                 /*! HCI Specification 1.2. */
   hvSpecification_2_0,                 /*! HCI Specification 2.0. */
   hvSpecification_2_1,                 /*! HCI Specification 2.1 + EDR. */
   hvSpecification_3_0,                 /*! HCI Specification 3.0 + HS. */
   hvSpecification_4_0,                 /*! HCI Specification 4.0. */
   hvSpecification_4_1,                 /*! HCI Specification 4.1. */
   hvSpecification_4_2                  /*! HCI Specification 4.2. */
} HCI_Version_t;

   /*! The following structure is used with the
    * HCI_Write_Stored_Link_Key() function, and represents a single
    * BD_ADDR/Link Key Pair that is required by this function.
    */
typedef struct _tagHCI_Stored_Link_Key_Info_t
{
   BD_ADDR_t  BD_ADDR;
   Link_Key_t Link_Key;
} HCI_Stored_Link_Key_Info_t;

#define HCI_STORED_LINK_KEY_INFO_SIZE                       (sizeof(HCI_Stored_Link_Key_Info_t))

   /*! The following structure is used with the
    * HCI_Host_Number_Of_Completed_Packets() function, and represents
    * a single Connection Handle/Host Number of Completed Packets Pair
    * that is required by this function.
    */
typedef struct _tagHCI_Host_Completed_Packets_Info_t
{
   Word_t Connection_Handle;
   Word_t Host_Num_Of_Completed_Packets;
} HCI_Host_Completed_Packets_Info_t;

#define HCI_HOST_COMPLETED_PACKETS_INFO_SIZE                (sizeof(HCI_Host_Completed_Packets_Info_t))

   /*! The following structure is used with
    * HCI_Read_Local_Supported_Codecs() function and respresents a
    * single Vendor Specific Codec pair that is returned from this
    * function.
    */
typedef struct _tagHCI_Vendor_Specific_Codec_Info_t
{
   Word_t CompanyID;
   Word_t VendorCodecID;
} HCI_Vendor_Specific_Codec_Info_t;

#define HCI_VENDOR_SPECIFIC_CODEC_INFO_SIZE                 (sizeof(HCI_Vendor_Specific_Codec_Info_t))

   /*! The following structure is used with
    * HCI_Read_Local_Supported_Codecs() function and respresents the
    * Local Supported Codec information (Bluetooth and Vendor Specific)
    * that is returned from this function.
    */
typedef struct _tagHCI_Local_Supported_Codec_Info_t
{
   Byte_t                            NumberOfSupportedCodecs;
   Byte_t                           *SupportedCodecs;
   Byte_t                            NumberOfVendorSpecificCodecs;
   HCI_Vendor_Specific_Codec_Info_t *VendorSpecificCodecs;
} HCI_Local_Supported_Codec_Info_t;

#define HCI_LOCAL_SUPPORTED_CODEC_INFO_SIZE                 (sizeof(HCI_Local_Supported_Codec_Info_t))

   /*! The following structure is used with the the
    * HCI_Enhanced_Setup_Synchronous_Connection()/
    * HCI_Enhanced_Accept_Synchronous_Connection_Request() commands for
    * the HCI SCO Coding Format.
    * \note The VendorCodecInfo member is ignored unless the
    *          CodingFormat member is set to
    *          HCI_ENHANCED_SYNCHRONOUS_CODING_FORMAT_VENDOR_SPECIFIC.
    */
typedef struct _tagHCI_Coding_Format_Info_t
{
   Byte_t                           CodingFormat;
   HCI_Vendor_Specific_Codec_Info_t VendorCodecInfo;
} HCI_Coding_Format_Info_t;

#define HCI_CODING_FORMAT_INFO_SIZE                         (sizeof(HCI_Coding_Format_Info_t))

   /*! The following structure contains the informat that is passed into
    * the HCI_Enhanced_Setup_Synchronous_Connection()/
    * HCI_Enhanced_Accept_Synchronous_Connection_Request() commands for
    * the Synchronous Connection Information.
    */
typedef struct _tagHCI_Enhanced_Synchronous_Connection_Info_t
{
   DWord_t                  TransmitBandwidth;
   DWord_t                  ReceiveBandwidth;
   HCI_Coding_Format_Info_t TransmitCodingFormat;
   HCI_Coding_Format_Info_t ReceiveCodingFormat;
   Word_t                   TransmitCodecFrameSize;
   Word_t                   ReceiveCodecFrameSize;
   DWord_t                  InputBandwidth;
   DWord_t                  OutputBandwidth;
   HCI_Coding_Format_Info_t InputCodingFormat;
   HCI_Coding_Format_Info_t OutputCodingFormat;
   Word_t                   InputCodedDataSize;
   Word_t                   OutputCodedDataSize;
   Byte_t                   InputPCMDataFormat;
   Byte_t                   OutputPCMDataFormat;
   Byte_t                   InputPCMSamplePayloadMSBPosition;
   Byte_t                   OutputPCMSamplePayloadMSBPosition;
   Byte_t                   InputDataPath;
   Byte_t                   OutputDataPath;
   Byte_t                   InputTransportUnitSize;
   Byte_t                   OutputTransportUnitSize;
   Word_t                   MaxLatency;
   Word_t                   PacketType;
   Byte_t                   RetransmissionEffort;
} HCI_Enhanced_Synchronous_Connection_Info_t;

#define HCI_ENHANCED_SYNCHRONOUS_CONNECTION_INFO_SIZE       (sizeof(HCI_Enhanced_Synchronous_Connection_Info_t))

   /*! The following structure is used to holder information on a
    * specified MWS Transport.
    * \note The To_MWS_Baud_Rate and From_MWS_Baud_Rate members are
    *          arrays of NumberOfBaudRates entries.
    */
typedef struct _tagHCI_MWS_Transport_Info_t
{
   Byte_t   TransportLayer;
   Byte_t   NumberOfBaudRates;
   DWord_t *To_MWS_Baud_Rate;
   DWord_t *From_MWS_Baud_Rate;
} HCI_MWS_Transport_Info_t;

#define HCI_MWS_TRANSPORT_INFO_SIZE                         (sizeof(HCI_MWS_Transport_Info_t))

   /*! The following structure is used with the the
    * HCI_Get_MWS_Transport_Layer_Configuration command for the HCI
    * return result.
    * \note The TransportLayerInfo parameter is an array of
    *          NumberOfTransports entries.
    */
typedef struct _tagHCI_MWS_Transport_Layer_Configuration_Info_t
{
   Byte_t                    NumberOfTransports;
   HCI_MWS_Transport_Info_t *TransportLayerInfo;
} HCI_MWS_Transport_Layer_Configuration_Info_t;

#define HCI_MWS_TRANSPORT_LAYER_CONFIGURATION_INFO_SIZE     (sizeof(HCI_MWS_Transport_Layer_Configuration_Info_t))

   /*! The following structure is used with the HCI_Set_MWS_Signaling()
    * command for the command parameters.
    */
typedef struct _tagHCI_Set_MWS_Signaling_Parameters_t
{
   Word_t MWS_RX_Assert_Offset;
   Word_t MWS_RX_Assert_Jitter;
   Word_t MWS_RX_Deassert_Offset;
   Word_t MWS_RX_Deassert_Jitter;
   Word_t MWS_TX_Assert_Offset;
   Word_t MWS_TX_Assert_Jitter;
   Word_t MWS_TX_Deassert_Offset;
   Word_t MWS_TX_Deassert_Jitter;
   Word_t MWS_Pattern_Assert_Offset;
   Word_t MWS_Pattern_Assert_Jitter;
   Word_t MWS_Inactivity_Duration_Assert_Offset;
   Word_t MWS_Inactivity_Duration_Assert_Jitter;
   Word_t MWS_Scan_Frequency_Assert_Offset;
   Word_t MWS_Scan_Frequency_Assert_Jitter;
   Word_t MWS_Priority_Assert_Offset_Request;
} HCI_Set_MWS_Signaling_Parameters_t;

#define HCI_SET_MWS_SIGNALING_PARAMETERS_SIZE               (sizeof(HCI_Set_MWS_Signaling_Parameters_t))

   /*! The following structure is used with the HCI_Set_MWS_Signaling()
    * command for the command Result.
    */
typedef struct _tagHCI_Set_MWS_Signaling_Result_t
{
   Word_t Bluetooth_RX_Priority_Assert_Offset;
   Word_t Bluetooth_RX_Priority_Assert_Jitter;
   Word_t Bluetooth_RX_Priority_Deassert_Offset;
   Word_t Bluetooth_RX_Priority_Deassert_Jitter;
   Word_t _802_RX_Priority_Assert_Offset;
   Word_t _802_RX_Priority_Assert_Jitter;
   Word_t _802_RX_Priority_Deassert_Offset;
   Word_t _802_RX_Priority_Deassert_Jitter;
   Word_t Bluetooth_TX_On_Assert_Offset;
   Word_t Bluetooth_TX_On_Assert_Jitter;
   Word_t Bluetooth_TX_On_Deassert_Offset;
   Word_t Bluetooth_TX_On_Deassert_Jitter;
   Word_t _802_TX_On_Assert_Offset;
   Word_t _802_TX_On_Assert_Jitter;
   Word_t _802_TX_On_Deassert_Offset;
   Word_t _802_TX_On_Deassert_Jitter;
} HCI_Set_MWS_Signaling_Result_t;

#define HCI_SET_MWS_SIGNALING_RESULT_SIZE                   (sizeof(HCI_Set_MWS_Signaling_Result_t))

   /* HCI Event API Types.
    */
typedef enum
{
   etInquiry_Complete_Event, /*!< Indicates that the Inquiry is finished. */
   etInquiry_Result_Event, /*!< Indicates that one or more Bluetooth devices have responded so far during the current Inquiry process. */
   etConnection_Complete_Event, /*!< Indicates to both of the Hosts forming the connection that a new connection has been established.*/
   etConnection_Request_Event, /*!< Indicates that a new incoming connection is trying to be established. */
   etDisconnection_Complete_Event, /*!< Indicates that a connection has been terminated. */
   etAuthentication_Complete_Event, /*!< Indicates that the authentication has been completed for the specified connection. */
   etRemote_Name_Request_Complete_Event, /*!< Indicates that a remote name request has been completed. */
   etEncryption_Change_Event, /*!< Indicates that the change in the encryption has been completed for a connection. */
   etChange_Connection_Link_Key_Complete_Event, /*!< Indicates that the change in the Link Key for the connection has been completed. */
   etMaster_Link_Key_Complete_Event, /*!< Indicates that the change in the temporary Link Key or in the semi-permanent link keys on the Bluetooth master side has been completed. */
   etRead_Remote_Supported_Features_Complete_Event, /*!<Indicates the completion of the process of obtaining the supported features of the remote Bluetooth device. */
   etRead_Remote_Version_Information_Complete_Event, /*!< Indicates the completion of the process of obtaining the version information of the remote Bluetooth device.*/
   etQoS_Setup_Complete_Event, /*!< Indicates the completion of the process of setting up QoS with the remote Bluetooth device. */
   etHardware_Error_Event, /*!< Indicates some type of hardware failure for the Bluetooth device. */
   etFlush_Occurred_Event, /*!< Indicates that, for the specified connection, the current user data to be transmitted has been removed. */
   etRole_Change_Event, /*!< Indicates that the current Bluetooth role related to the particular connection has been changed. */
   etNumber_Of_Completed_Packets_Event, /*!< Indicates to the Host how many HCI Data Packets have been completed for each Connection Handle since the previous Number Of Completed Packets Event was sent.  (part of flow control) */
   etMode_Change_Event, /*!< Indicates when the device associated with a connection changes between Active, Hold, Sniff and Park modes. */
   etReturn_Link_Keys_Event, /*!< Returns stored link keys after a Read_Stored_Link_Key command is used.*/
   etPIN_Code_Request_Event, /*!< Indicates that a PIN code is required to create a new link key for a connection. */
   etLink_Key_Request_Event, /*!< Indicates that a Link Key is required for the connection with the device specified. */
   etLink_Key_Notification_Event, /*!< Indicates to the Host that a new Link Key has been created for the connection with a device. */
   etLoopback_Command_Event, /*!< Returns most commands that the Host sends to the Host Controller while in loopback testing mode. */
   etData_Buffer_Overflow_Event, /*!< Indicates that the Host Controller's data buffers have overflowed, because the Host has sent more packets than allowed. */
   etMax_Slots_Change_Event, /*!< Notifies the Host about the LMP_Max_Slots parameter when the value of this parameter changes. */
   etRead_Clock_Offset_Complete_Event, /*!< Indicates the completion of the process of obtaining the Clock offset information. */
   etConnection_Packet_Type_Changed_Event, /*!< Indicates the completion of the process of changing the Packet Types used for the specified connection. */
   etQoS_Violation_Event, /*!< Indicates that the Link Manager is unable to provide the current QoS requirement for the connection. */
   etPage_Scan_Mode_Change_Event, /*!< Indicates that the connected remote Bluetooth device has successfully changed the Page_Scan_Mode. */
   etPage_Scan_Repetition_Mode_Change_Event, /*!< Indicates that the connected remote Bluetooth device has successfully changed the Page_Scan_Repetition_Mode (SR). */
   etBluetooth_Logo_Testing_Event, /*!< Reserved for Bluetooth Logo Testing Events. */
   etVendor_Specific_Debug_Event, /*!< Reserved for Bluetooth Logo Testing Events. */
   etDevice_Reset_Event, /*!< Indicates that the local Bluetooth device has been reset. */
   etFlow_Specification_Complete_Event, /*!< Indicates the Quality of Service for the ACL Connection the Controller is able to support. */
   etInquiry_Result_With_RSSI_Event, /*!< Indicates that one or more Bluetooth devices have responded so far during the current Inquiry process. */
   etRead_Remote_Extended_Features_Complete_Event, /*!< Indicates the completion of the process of the Link Manager obtaining the remote extended LMP features of the remote device. */
   etSynchronous_Connection_Complete_Event, /*!< Indicates to both the Hosts that a new Synchronous connection has been established. */
   etSynchronous_Connection_Changed_Event, /*!< Indicates to the Host that an existing Synchronous connection has been reconfigured. */
   etSniff_Subrating_Event, /*!< Indicates that specified device has had a sniff subrating enabled or the parameters have been changed. */
   etExtended_Inquiry_Result_Event, /*!< Indicates that controller has responded during inquiry process  with extended inquiry response data. */
   etEncryption_Key_Refresh_Complete_Event, /*!< Indicates that encryption key was refreshed on a given connection handle. */
   etIO_Capability_Request_Event, /*!< Indicates that IO capabilities of the host are required for simple pairing process. */
   etIO_Capability_Response_Event, /*!< Indicates that IO capabilities of remote host have been received. */
   etUser_Confirmation_Request_Event, /*!< Indicates that user confirmation of a numeric value is needed. */
   etUser_Passkey_Request_Event, /*!< Indicates that passkey is required as part of Simple Pairing process. */
   etRemote_OOB_Data_Request_Event, /*!< Indicates that Simple Pairing Hash C and Simple Pairing Randomizer R is required for the Secure Simple Pairing process. */
   etSimple_Pairing_Complete_Event, /*!< Indicates that Simple Pairing process has completed. */
   etLink_Supervision_Timeout_Changed_Event, /*!< Indicates to slave’s host that Link Supervision Timeout parameter has changed in the slave controller. */
   etEnhanced_Flush_Complete_Event, /*!< Indicates that an Enhanced Flush is complete for specified handle. */
   etUser_Passkey_Notification_Event, /*!< Used to provide a passkey to display to user as required by Simple Pairing process. */
   etKeypress_Notification_Event, /*!< Sent to the host after a passkey notification has been received by Link Manager on specified device. */
   etRemote_Host_Supported_Features_Notification_Event, /*!< Used to return LMP extended features page which contains Host features. */
   etPhysical_Link_Complete_Event, /*!< Indicates that a new physical link has been established. */
   etChannel_Selected_Event, /*!< Indicates that link information data is available to be read. */
   etDisconnection_Physical_Link_Complete_Event, /*!< Indicates a physical link was terminated. */
   etPhysical_Link_Loss_Early_Warning_Event, /*!< Occurs when physical link has indications that it may be disrupted. */
   etPhysical_Link_Recovery_Event, /*!< Indicates that whatever caused etPhysical_Link_Loss_Early_Warning_Event has been cleared. */
   etLogical_Link_Complete_Event, /*!< Indicates to host that a new logical link has been successfully established. */
   etDisconnection_Logical_Link_Complete_Event, /*!< Occurs when logical link is terminated on local controller. */
   etFlow_Spec_Modify_Complete_Event, /*!< Indicates that Flow Spec Modify command has completed. */
   etNumber_Of_Completed_Data_Blocks_Event, /*!< Indicates how many ACL data packets have been completed and how many data block buffers freed. */
   etShort_Range_Mode_Change_Complete_Event, /*!< Indicates that a controller was asked to enable or disable the Short Range Mode for a specified physical link. */
   etAMP_Status_Change_Event, /*!< Indicates that a change has occurred to AMP status. */
   etAMP_Start_Test_Event, /*!< Indicates that HCI_AMP_Test_Command has completed. */
   etAMP_Test_End_Event, /*!< Indicates AMP has transmitted or received number of frames/bursts configured for a test. */
   etAMP_Receiver_Report_Event, /*!< Indicates number of frames received for a test. */
   etLE_Meta_Event, /*!< Indicates Bluetooth Low Energy event has occurred. */
   etPlatform_Specific_Event, /*!< Indicates a platform specific event has occurred. */
   etTriggered_Clock_Capture_Event, /*!< Indicates the Data that is associated with the HCI Triggered Clock Capture Event Event. */
   etSynchronization_Train_Complete_Event, /*!< Indicates the Data that is associated with the HCI Synchronization Train Complete Event Event. */
   etSynchronization_Train_Received_Event, /*!< Indicates the Data that is associated with the HCI Synchronization Train Received Event Event. */
   etConnectionless_Slave_Broadcast_Receive_Event, /*!< Indicates the Data that is associated with the HCI Connectionless Slave Broadcast Receive Event Event. */
   etConnectionless_Slave_Broadcast_Timeout_Event, /*!< Indicates the Data that is associated with the HCI Connectionless Slave Broadcast Timeout Event. */
   etTruncated_Page_Complete_Event, /*!< Indicates the Data that is associated with the HCI Truncated Page Complete Event. */
   etSlave_Page_Response_Timeout_Event, /*!< Indicates that a slave page response timeout has occurred. */
   etConnectionless_Slave_Broadcast_Channel_Map_Change_Event, /*!< Indicates the Data that is associated with the HCI Connectionless Slave Broadcast Channel Map Change Event. */
   etInquiry_Response_Notification_Event, /*!< Indicates the Data that is associated with the HCI Inquiry Response Notification Event. */
   etAuthenticated_Payload_Timeout_Expired_Event /*!< Indicates the Data that is associated with the HCI Authenticated Payload Timeout Expired Event.  */
} HCI_Event_Type_t;

   /* LE Meta Event Subevent Types.  (Version 4.0 + LE).
    */
typedef enum
{
   meConnection_Complete_Event, /*!< Indicates that a new connection has been created. */
   meAdvertising_Report_Event, /*!< Indicates that a Bluetooth device or multiple devices have responded to an active scan or received some information during a passive scan. */
   meConnection_Update_Complete_Event, /*!< Indicates that the controller has updated the connection parameters. */
   meRead_Remote_Used_Features_Complete_Event, /*!< Indicates the result of a Remote used feature request to a remote Bluetooth device. */
   meLong_Term_Key_Request_Event, /*!< Indicates master device is trying to encrypt or re-encrypt the link and is requesting the long term key from the host. */
   meRemote_Connection_Parameter_Request_Event /*!< Indicates the remote device is requesting that the connection parameters be updated. */
} HCI_LE_Meta_Event_Type_t;

   /*! The following structure represents the data that provided by a
    * HCI Inquiry Complete Event.
    * \note The Num_Responses field is ONLY valid if the Bluetooth
    *          Device is using HCI Specification 1.0B, and is NOT
    *          valid if the Bluetooth Device is using HCI Specification
    *          Version 1.1 (or greater).  The caller can issue the
    *          HCI_Version_Supported() function to determine if the
    *          Num_Responses field is valid (Version 1.1 or greater).
    */
typedef struct _tagHCI_Inquiry_Complete_Event_Data_t
{
   Byte_t Status;
   Byte_t Num_Responses;
} HCI_Inquiry_Complete_Event_Data_t;

#define HCI_INQUIRY_COMPLETE_EVENT_DATA_SIZE            (sizeof(HCI_Inquiry_Complete_Event_Data_t))

   /*! The following structure represents the data that is returned in
    * a HCI Inquiry Result Event.  This structure represents an
    * individual entry, and the HCI Inquiry Result Event structure will
    * contains an array of these structures (based upon the number of
    * responses).
    */
typedef struct _tagHCI_Inquiry_Result_Data_t
{
   BD_ADDR_t         BD_ADDR;
   Byte_t            Page_Scan_Repetition_Mode;
   Byte_t            Page_Scan_Period_Mode;
   Byte_t            Page_Scan_Mode;
   Class_of_Device_t Class_of_Device;
   Word_t            Clock_Offset;
} HCI_Inquiry_Result_Data_t;

#define HCI_INQUIRY_RESULT_DATA_SIZE                    (sizeof(HCI_Inquiry_Result_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Inquiry Result Event.  The HCI_Inquiry_Result_Data
    * member represents a variable array that can contain 0 or more
    * entries.  The Number of Entries in this array is given by the
    * Num_Responses member.  A utility MACRO is provided to aid in the
    * calculation of the Number of Bytes that are occupied by the
    * structure given a specified number of Responses.
    */
typedef struct _tagHCI_Inquiry_Result_Event_Data_t
{
   Byte_t                    Num_Responses;
   HCI_Inquiry_Result_Data_t HCI_Inquiry_Result_Data[1];
} HCI_Inquiry_Result_Event_Data_t;

   /*! The following MACRO is a utility MACRO that exists to aid code
    * readability to Determine the size (in Bytes) of an
    * HCI Inquiry Result Event Data Structure based upon the number of
    * HCI Inquiry Result Entries associated with the Event.
    * @param _x The first parameter to this MACRO is the number of HCI Inquiry Result
    * Entries.
    */
#define HCI_INQUIRY_RESULT_EVENT_DATA_SIZE(_x)          ((sizeof(HCI_Inquiry_Result_Event_Data_t) - sizeof(HCI_Inquiry_Result_Data_t)) + (((Byte_t)(_x))*sizeof(HCI_Inquiry_Result_Data_t)))

#define HCI_INQUIRY_RESULT_EVENT_MAX_DATA_ENTRIES       ((((sizeof(Byte_t))*256)-HCI_INQUIRY_RESULT_EVENT_DATA_SIZE(0))/sizeof(HCI_Inquiry_Result_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Connection Complete Event.
    */
typedef struct _tagHCI_Connection_Complete_Event_Data_t
{
   Byte_t    Status;
   Word_t    Connection_Handle;
   BD_ADDR_t BD_ADDR;
   Byte_t    Link_Type;
   Byte_t    Encryption_Mode;
} HCI_Connection_Complete_Event_Data_t;

#define HCI_CONNECTION_COMPLETE_EVENT_DATA_SIZE         (sizeof(HCI_Connection_Complete_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Connection Request Event.
    */
typedef struct _tagHCI_Connection_Request_Event_Data_t
{
   BD_ADDR_t         BD_ADDR;
   Class_of_Device_t Class_of_Device;
   Byte_t            Link_Type;
} HCI_Connection_Request_Event_Data_t;

#define HCI_CONNECTION_REQUEST_EVENT_DATA_SIZE          (sizeof(HCI_Connection_Request_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Disconnection Complete Event.
    */
typedef struct _tagHCI_Disconnection_Complete_Event_Data_t
{
   Byte_t Status;
   Word_t Connection_Handle;
   Byte_t Reason;
} HCI_Disconnection_Complete_Event_Data_t;

#define HCI_DISCONNECTION_COMPLETE_EVENT_DATA_SIZE      (sizeof(HCI_Disconnection_Complete_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Authentication Complete Event.
    */
typedef struct _tagHCI_Authentication_Complete_Event_Data_t
{
   Byte_t Status;
   Word_t Connection_Handle;
} HCI_Authentication_Complete_Event_Data_t;

#define HCI_AUTHENTICATION_COMPLETE_EVENT_DATA_SIZE     (sizeof(HCI_Authentication_Complete_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Remote Name Request Complete Event.  The Remote_Name
    * member is a variable length field that represents a NULL
    * terminated Name.  The member is guaranteed to have at LEAST one
    * Byte of storage (empty string, single NULL Terminator), and NOT
    * more than MAX_NAME_LENGTH+1 (Maximum Name Length plus one byte for
    * NULL terminator).  A utility MACRO is provided to aid in the
    * calculation of the number of bytes required based upon the
    * number of bytes that the Remote Name String occupies.
    */
typedef struct _tagHCI_Remote_Name_Request_Complete_Event_Data_t
{
   Byte_t    Status;
   BD_ADDR_t BD_ADDR;
   char      Remote_Name[1];
} HCI_Remote_Name_Request_Complete_Event_Data_t;

   /*! The following MACRO is a utility MACRO that exists to aid code
    * readability to Determine the size (in Bytes) of an
    * HCI Remote Name Request Complete Event Data Structure based upon
    * the length (in bytes, including the terminating NULL character)
    * of the Remote Name. The first parameter to this MACRO is the
    * number of bytes required for the remote name (including
    * terminating NULL character).  The size of the Remote name
    * (including NULL character) will be maximum (MAX_NAME_LENGTH+1)
    * Bytes.
    */
#define HCI_REMOTE_NAME_REQUEST_COMPLETE_EVENT_DATA_SIZE(_x)    ((BTPS_STRUCTURE_OFFSET(HCI_Remote_Name_Request_Complete_Event_Data_t, Remote_Name)) + (((Byte_t)(_x))*sizeof(Byte_t)))

   /*! The following structure represents the Data that is associated
    * with the HCI Encryption Change Event.
    */
typedef struct _tagHCI_Encryption_Change_Event_Data_t
{
   Byte_t Status;
   Word_t Connection_Handle;
   Byte_t Encryption_Enable;
} HCI_Encryption_Change_Event_Data_t;

#define HCI_ENCRYPTION_CHANGE_EVENT_DATA_SIZE           (sizeof(HCI_Encryption_Change_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Change Connection Link Key Complete Event.
    */
typedef struct _tagHCI_Change_Connection_Link_Key_Complete_Event_Data_t
{
   Byte_t Status;
   Word_t Connection_Handle;
} HCI_Change_Connection_Link_Key_Complete_Event_Data_t;

#define HCI_CHANGE_CONNECTION_LINK_KEY_COMPLETE_EVENT_DATA_SIZE (sizeof(HCI_Change_Connection_Link_Key_Complete_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Master Link Complete Event.
    */
typedef struct _tagHCI_Master_Link_Key_Complete_Event_Data_t
{
   Byte_t Status;
   Word_t Connection_Handle;
   Byte_t Key_Flag;
} HCI_Master_Link_Key_Complete_Event_Data_t;

#define HCI_MASTER_LINK_KEY_COMPLETE_EVENT_DATA_SIZE    (sizeof(HCI_Master_Link_Key_Complete_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Read Remote Supported Features Complete Event.
    */
typedef struct _tagHCI_Read_Remote_Supported_Features_Complete_Event_Data_t
{
   Byte_t         Status;
   Word_t         Connection_Handle;
   LMP_Features_t LMP_Features;
} HCI_Read_Remote_Supported_Features_Complete_Event_Data_t;

#define HCI_READ_REMOTE_SUPPORTED_FEATURES_COMPLETE_EVENT_DATA_SIZE (sizeof(HCI_Read_Remote_Supported_Features_Complete_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Read Remote Version Information Complete Event.
    */
typedef struct _tagHCI_Read_Remote_Version_Information_Complete_Event_Data_t
{
   Byte_t Status;
   Word_t Connection_Handle;
   Byte_t LMP_Version;
   Word_t Manufacturer_Name;
   Word_t LMP_Subversion;
} HCI_Read_Remote_Version_Information_Complete_Event_Data_t;

#define HCI_READ_REMOTE_VERSION_INFORMATION_COMPLETE_EVENT_DATA_SIZE (sizeof(HCI_Read_Remote_Version_Information_Complete_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI QoS Setup Complete Event.
    */
typedef struct _tagHCI_QoS_Setup_Complete_Event_Data_t
{
   Byte_t  Status;
   Word_t  Connection_Handle;
   Byte_t  Flags;
   Byte_t  Service_Type;
   DWord_t Token_Rate;
   DWord_t Peak_Bandwidth;
   DWord_t Latency;
   DWord_t Delay_Variation;
} HCI_QoS_Setup_Complete_Event_Data_t;

#define HCI_QOS_SETUP_COMPLETE_EVENT_DATA_SIZE          (sizeof(HCI_QoS_Setup_Complete_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Hardware Error Event.
    */
typedef struct _tagHCI_Hardware_Error_Event_Data_t
{
   Byte_t Hardware_Code;
} HCI_Hardware_Error_Event_Data_t;

#define HCI_HARDWARE_ERROR_EVENT_DATA_SIZE              (sizeof(HCI_Hardware_Error_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Flush Occurred Event.
    */
typedef struct _tagHCI_Flush_Occurred_Event_Data_t
{
   Word_t Connection_Handle;
} HCI_Flush_Occurred_Event_Data_t;

#define HCI_FLUSH_OCCURRED_EVENT_DATA_SIZE              (sizeof(HCI_Flush_Occurred_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Role Change Event.
    */
typedef struct _tagHCI_Role_Change_Event_Data_t
{
   Byte_t    Status;
   BD_ADDR_t BD_ADDR;
   Byte_t    New_Role;
} HCI_Role_Change_Event_Data_t;

#define HCI_ROLE_CHANGE_EVENT_DATA_SIZE                 (sizeof(HCI_Role_Change_Event_Data_t))

   /*! The following structure is used with the HCI Number of Completed
    * Packets Event, and represents a single Connection Handle/Host
    * Number of Completed Packets Pair that is required by this Event.
    * This structure represents an individual entry and a HCI Number of
    * Completed Packets Event will contain a variable number of these
    * structures.
    */
typedef struct _tagHCI_Number_Of_Completed_Packets_Data_t
{
   Word_t Connection_Handle;
   Word_t HC_Num_Of_Completed_Packets;
} HCI_Number_Of_Completed_Packets_Data_t;

#define HCI_NUMBER_OF_COMPLETED_PACKETS_DATA_SIZE       (sizeof(HCI_Number_Of_Completed_Packets_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Number Of Completed Packets Event.  The
    * HCI_Number_Of_Completed_Packets_Data member represents a
    * variable array that can contain 0 or more entries.  The number of
    * Entries in this array is given by the Number_of_Handles member.
    * A utility MACRO is provided to aid in the calculation of the
    * Number of Bytes that are occupied by the structure given a
    * specified number of Handles.
    */
typedef struct _tagHCI_Number_Of_Completed_Packets_Event_Data_t
{
   Byte_t                                 Number_of_Handles;
   HCI_Number_Of_Completed_Packets_Data_t HCI_Number_Of_Completed_Packets_Data[1];
} HCI_Number_Of_Completed_Packets_Event_Data_t;

   /*! The following MACRO is a utility MACRO that exists to aid code
    * readability to Determine the size (in Bytes) of an
    * HCI Number of Completed Packets Event Data Structure based upon
    * the number of HCI Number of Completed Packets Event Data Entries
    * associated with the Event. The first parameter to this MACRO is
    * the number of HCI Number of Completed Packets Entries.
    */
#define HCI_NUMBER_OF_COMPLETED_PACKETS_EVENT_DATA_SIZE(_x)     ((sizeof(HCI_Number_Of_Completed_Packets_Event_Data_t) - sizeof(HCI_Number_Of_Completed_Packets_Data_t)) + (((Byte_t)(_x))*sizeof(HCI_Number_Of_Completed_Packets_Data_t)))

#define HCI_NUMBER_OF_COMPLETED_PACKETS_EVENT_MAX_DATA_ENTRIES  ((((sizeof(Byte_t))*256)-HCI_NUMBER_OF_COMPLETED_PACKETS_EVENT_DATA_SIZE(0))/sizeof(HCI_Number_Of_Completed_Packets_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Mode Change Event.
    */
typedef struct _tagHCI_Mode_Change_Event_Data_t
{
   Byte_t Status;
   Word_t Connection_Handle;
   Byte_t Current_Mode;
   Word_t Interval;
} HCI_Mode_Change_Event_Data_t;

#define HCI_MODE_CHANGE_EVENT_DATA_SIZE                 (sizeof(HCI_Mode_Change_Event_Data_t))

   /*! The following structure is used with the HCI Return Link Keys
    * Event, and represents a single BD_ADDR/Link Key Pair that is
    * required by this Event.  This structure represents an individual
    * entry and a HCI Return Link Keys Event will contain a variable
    * number of these structures.
    */
typedef struct _tagHCI_Return_Link_Keys_Data_t
{
   BD_ADDR_t  BD_ADDR;
   Link_Key_t Link_Key;
} HCI_Return_Link_Keys_Data_t;

#define HCI_RETURN_LINK_KEYS_DATA_SIZE                  (sizeof(HCI_Return_Link_Keys_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Return Link Keys Event.  The
    * HCI_Return_Link_Keys_Data member represents a variable array that
    * that can contain 0 or more entries.  The number of Entries in this
    * array is given by the Num_Keys member.  A utility MACRO is
    * provided to aid in the calculation of the Number of Bytes that
    * are occupied by the structure given a specified Number of Keys.
    */
typedef struct _tagHCI_Return_Link_Keys_Event_Data_t
{
   Byte_t                      Num_Keys;
   HCI_Return_Link_Keys_Data_t HCI_Return_Link_Keys_Data[1];
} HCI_Return_Link_Keys_Event_Data_t;

   /*! The following MACRO is a utility MACRO that exists to aid code
    * readability to Determine the size (in Bytes) of an
    * HCI Return Link Keys Event Data Structure based upon the number of
    * HCI Return Link Key Data Entries associated with the Event.
    * @param _x The first parameter to this MACRO is the number of HCI Return Link
    * Key Entries.
    */
#define HCI_RETURN_LINK_KEYS_EVENT_DATA_SIZE(_x)        ((sizeof(HCI_Return_Link_Keys_Event_Data_t) - sizeof(HCI_Return_Link_Keys_Data_t)) + (((Byte_t)(_x))*sizeof(HCI_Return_Link_Keys_Data_t)))

#define HCI_RETURN_LINK_KEYS_EVENT_MAX_DATA_ENTRIES     ((((sizeof(Byte_t))*256)-HCI_RETURN_LINK_KEYS_EVENT_DATA_SIZE(0))/sizeof(HCI_Return_Link_Keys_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI PIN Code Request Event.
    */
typedef struct _tagHCI_PIN_Code_Request_Event_Data_t
{
   BD_ADDR_t BD_ADDR;
} HCI_PIN_Code_Request_Event_Data_t;

#define HCI_PIN_CODE_REQUEST_EVENT_DATA_SIZE            (sizeof(HCI_PIN_Code_Request_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Link Key Request Event.
    */
typedef struct _tagHCI_Link_Key_Request_Event_Data_t
{
   BD_ADDR_t BD_ADDR;
} HCI_Link_Key_Request_Event_Data_t;

#define HCI_LINK_KEY_REQUEST_EVENT_DATA_SIZE            (sizeof(HCI_Link_Key_Request_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Link Key Notification Event.
    * \note The Key_Type field is ONLY valid if the Bluetooth Device
    *          is using HCI Specification 1.1 (or greater), and is NOT
    *          valid if the Bluetooth Device is using HCI Specification
    *          Version 1.0B.  The caller can issue the
    *          HCI_Version_Supported() function to determine if the
    *          Key_Type field is valid (Version 1.1 or greater).
    */
typedef struct _tagHCI_Link_Key_Notification_Event_Data_t
{
   BD_ADDR_t  BD_ADDR;
   Link_Key_t Link_Key;
   Byte_t     Key_Type;
} HCI_Link_Key_Notification_Event_Data_t;

#define HCI_LINK_KEY_NOTIFICATION_EVENT_DATA_SIZE       (sizeof(HCI_Link_Key_Notification_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Loopback Command Event.  The HCI_Command_Packet_Data
    * member represents a variable array that can contain 0 or more
    * bytes.  The number of bytes specified in this member is given by
    * the HCI_Command_Packet_Length member.  A utility MACRO is
    * provided to aid in the calculation of the Number of Bytes that
    * are occupied by the structure given a specified HCI Command
    * Packet Length (in bytes).
    */
typedef struct _tagHCI_Loopback_Command_Event_Data_t
{
   Word_t HCI_Command_Packet_Length;
   Byte_t HCI_Command_Packet_Data[1];
} HCI_Loopback_Command_Event_Data_t;

   /*! The following MACRO is a utility MACRO that exists to aid code
    * readability to Determine the size (in Bytes) of an
    * HCI Loopeback Command Event Data Structure based upon the size
    * (in bytes) of the HCI Command Packet Length associated with the
    * Event. The first parameter to this MACRO is the length (in bytes)
    * of the HCI Command Packet Data.
    */
#define HCI_LOOPBACK_COMMAND_EVENT_DATA_SIZE(_x)        ((BTPS_STRUCTURE_OFFSET(HCI_Loopback_Command_Event_Data_t, HCI_Command_Packet_Data)) + (((Byte_t)(_x))*sizeof(Byte_t)))

#define HCI_LOOPBACK_COMMAND_EVENT_MAX_DATA_LENGTH      (HCI_LOOPBACK_COMMAND_EVENT_DATA_SIZE(HCI_COMMAND_MAX_SIZE))

   /*! The following structure represents the Data that is associated
    * with the HCI Data Buffer Overflow Event.
    */
typedef struct _tagHCI_Data_Buffer_Overflow_Event_Data_t
{
   Byte_t Link_Type;
} HCI_Data_Buffer_Overflow_Event_Data_t;

#define HCI_DATA_BUFFER_OVERFLOW_EVENT_DATA_SIZE        (sizeof(HCI_Data_Buffer_Overflow_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Max Slots Change Event.
    */
typedef struct _tagHCI_Max_Slots_Change_Event_Data_t
{
   Word_t Connection_Handle;
   Byte_t LMP_Max_Slots;
} HCI_Max_Slots_Change_Event_Data_t;

#define HCI_MAX_SLOTS_CHANGE_EVENT_DATA_SIZE            (sizeof(HCI_Max_Slots_Change_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Read Clock Offset Complete Event.
    */
typedef struct _tagHCI_Read_Clock_Offset_Complete_Event_Data_t
{
   Byte_t Status;
   Word_t Connection_Handle;
   Word_t Clock_Offset;
} HCI_Read_Clock_Offset_Complete_Event_Data_t;

#define HCI_READ_CLOCK_OFFSET_COMPLETE_EVENT_DATA_SIZE  (sizeof(HCI_Read_Clock_Offset_Complete_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Connection Packet Type Changed Event.
    */
typedef struct _tagHCI_Connection_Packet_Type_Changed_Event_Data_t
{
   Byte_t Status;
   Word_t Connection_Handle;
   Word_t Packet_Type;
} HCI_Connection_Packet_Type_Changed_Event_Data_t;

#define HCI_CONNECTION_PACKET_TYPE_CHANGED_EVENT_DATA_SIZE (sizeof(HCI_Connection_Packet_Type_Changed_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI QoS Violation Event.
    */
typedef struct _tagHCI_QoS_Violation_Event_Data_t
{
   Word_t Connection_Handle;
} HCI_QoS_Violation_Event_Data_t;

#define HCI_QOS_VIOLATION_EVENT_DATA_SIZE               (sizeof(HCI_QoS_Violation_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Page Scan Mode Change Event.
    */
typedef struct _tagHCI_Page_Scan_Mode_Change_Event_Data_t
{
   BD_ADDR_t BD_ADDR;
   Byte_t    Page_Scan_Mode;
} HCI_Page_Scan_Mode_Change_Event_Data_t;

#define HCI_PAGE_SCAN_MODE_CHANGE_EVENT_DATA_SIZE       (sizeof(HCI_Page_Scan_Mode_Change_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Page Scan Repetition Mode Change Event.
    */
typedef struct _tagHCI_Page_Scan_Repetition_Mode_Change_Event_Data_t
{
   BD_ADDR_t BD_ADDR;
   Byte_t    Page_Scan_Repetition_Mode;
} HCI_Page_Scan_Repetition_Mode_Change_Event_Data_t;

#define HCI_PAGE_SCAN_REPETITION_MODE_CHANGE_EVENT_DATA_SIZE    (sizeof(HCI_Page_Scan_Repetition_Mode_Change_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Flow Specification Complete Event (Version 1.2).
    */
typedef struct _tagHCI_Flow_Specification_Complete_Event_Data_t
{
   Byte_t  Status;
   Word_t  Connection_Handle;
   Byte_t  Flags;
   Byte_t  Flow_Direction;
   Byte_t  Service_Type;
   DWord_t Token_Rate;
   DWord_t Token_Bucket_Size;
   DWord_t Peak_Bandwidth;
   DWord_t Access_Latency;
} HCI_Flow_Specification_Complete_Event_Data_t;

#define HCI_FLOW_SPECIFICATION_COMPLETE_EVENT_DATA_SIZE (sizeof(HCI_Flow_Specification_Complete_Event_Data_t))

   /*! The following structure represents the data that is returned in a
    * HCI Inquiry Result With RSSI Event.  This structure represents an
    * individual entry, and the HCI Inquiry Result With RSSI Event
    * structure will contain an array of these structures (based upon
    * the number of responses).  (Version 1.2)
    */
typedef struct _tagHCI_Inquiry_Result_With_RSSI_Data_t
{
   BD_ADDR_t         BD_ADDR;
   Byte_t            Page_Scan_Repetition_Mode;
   Byte_t            Page_Scan_Period_Mode;
   Class_of_Device_t Class_of_Device;
   Word_t            Clock_Offset;
   SByte_t           RSSI;
} HCI_Inquiry_Result_With_RSSI_Data_t;

#define HCI_INQUIRY_RESULT_WITH_RSSI_DATA_SIZE          (sizeof(HCI_Inquiry_Result_With_RSSI_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Inquiry Result With RSSI Event.  The
    * HCI_Inquiry_Result_With_RSSI_Data member represents a variable
    * array that can contain 0 or more entries.  The Number of Entries
    * in this array is given by the Num_Responses member.  A utility
    * MACRO is provided to aid in the calculation of the Number of Bytes
    * that are occupied by the structure given a specified number of
    * Responses. (Version 1.2)
    */
typedef struct _tagHCI_Inquiry_Result_With_RSSI_Event_Data_t
{
   Byte_t                              Num_Responses;
   HCI_Inquiry_Result_With_RSSI_Data_t HCI_Inquiry_Result_With_RSSI_Data[1];
} HCI_Inquiry_Result_With_RSSI_Event_Data_t;

   /*! The following MACRO is a utility MACRO that exists to aid code
    * readability to Determine the size (in Bytes) of an HCI Inquiry
    * Result With RSSI Event Data Structure based upon the number of HCI
    * Inquiry Result With RSSI Entries associated with the Event.
    * The first parameter to this MACRO is the number of HCI Inquiry Result
    * With RSSI Entries (Version 1.2).
    */
#define HCI_INQUIRY_RESULT_WITH_RSSI_EVENT_DATA_SIZE(_x)     ((sizeof(HCI_Inquiry_Result_With_RSSI_Event_Data_t) - sizeof(HCI_Inquiry_Result_With_RSSI_Data_t)) + (((Byte_t)(_x))*sizeof(HCI_Inquiry_Result_With_RSSI_Data_t)))

#define HCI_INQUIRY_RESULT_WITH_RSSI_EVENT_MAX_DATA_ENTRIES  ((((sizeof(Byte_t))*256)-HCI_INQUIRY_RESULT_WITH_RSSI_EVENT_DATA_SIZE(0))/sizeof(HCI_Inquiry_Result_With_RSSI_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Read Remote Extended Features Complete Event (Version
    * 1.2).
    */
typedef struct _tagHCI_Read_Remote_Extended_Features_Complete_Event_Data_t
{
   Byte_t         Status;
   Word_t         Connection_Handle;
   Byte_t         Page_Number;
   Byte_t         Maximum_Page_Number;
   LMP_Features_t Extended_LMP_Features;
} HCI_Read_Remote_Extended_Features_Complete_Event_Data_t;

#define HCI_READ_REMOTE_EXTENDED_FEATURES_COMPLETE_EVENT_DATA_SIZE (sizeof(HCI_Read_Remote_Extended_Features_Complete_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Synchronous Connection Complete Event (Version 1.2).
    */
typedef struct _tagHCI_Synchronous_Connection_Complete_Event_Data_t
{
   Byte_t    Status;
   Word_t    Connection_Handle;
   BD_ADDR_t BD_ADDR;
   Byte_t    Link_Type;
   Byte_t    Transmission_Interval;
   Byte_t    Retransmission_Window;
   Word_t    Rx_Packet_Length;
   Word_t    Tx_Packet_Length;
   Byte_t    Air_Mode;
} HCI_Synchronous_Connection_Complete_Event_Data_t;

#define HCI_SYNCHRONOUS_CONNECTION_COMPLETE_EVENT_DATA_SIZE (sizeof(HCI_Synchronous_Connection_Complete_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Synchronous Connection Changed Event (Version 1.2).
    */
typedef struct _tagHCI_Synchronous_Connection_Changed_Event_Data_t
{
   Byte_t Status;
   Word_t Connection_Handle;
   Byte_t Transmission_Interval;
   Byte_t Retransmission_Window;
   Word_t Rx_Packet_Length;
   Word_t Tx_Packet_Length;
} HCI_Synchronous_Connection_Changed_Event_Data_t;

#define HCI_SYNCHRONOUS_CONNECTION_CHANGED_EVENT_DATA_SIZE           (sizeof(HCI_Synchronous_Connection_Changed_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Sniff Subrating Event (Version 2.1 + EDR).
    */
typedef struct _tagHCI_Sniff_Subrating_Event_Data_t
{
   Byte_t Status;
   Word_t Connection_Handle;
   Word_t Maximum_Transmit_Latency;
   Word_t Maximum_Receive_Latency;
   Word_t Minimum_Remote_Timeout;
   Word_t Minimum_Local_Timeout;
} HCI_Sniff_Subrating_Event_Data_t;

#define HCI_SNIFF_SUBRATING_EVENT_DATA_SIZE                          (sizeof(HCI_Sniff_Subrating_Event_Data_t))

   /*! The following structure represents the Extended Inquiry Result
    * Format that is associated with Inquiry Mode set to
    *    HCI_INQUIRY_MODE_INQUIRY_RESULT_WITH_RSSI_OR_EXTENDED_RESULT
    * (Version 2.1 + EDR).
    */
typedef struct _tagHCI_Extended_Inquiry_Result_Data_t
{
   BD_ADDR_t                        BD_ADDR;
   Byte_t                           Page_Scan_Repetition_Mode;
   Byte_t                           Reserved;
   Class_of_Device_t                Class_of_Device;
   Word_t                           Clock_Offset;
   SByte_t                          RSSI;
   Extended_Inquiry_Response_Data_t Extended_Inquiry_Response;
} HCI_Extended_Inquiry_Result_Data_t;

#define HCI_EXTENDED_INQUIRY_RESULT_DATA_SIZE                        (sizeof(HCI_Extended_Inquiry_Result_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Extended Inquiry Result Event (Version 2.1 + EDR).
    * \note The Num_Responses field is always 1 for extended data.
    */
typedef struct _tagHCI_Extended_Inquiry_Result_Event_Data_t
{
   Byte_t                              Num_Responses;
   HCI_Extended_Inquiry_Result_Data_t  HCI_Inquiry_Result_Data;
} HCI_Extended_Inquiry_Result_Event_Data_t;

#define HCI_EXTENDED_INQUIRY_RESULT_EVENT_DATA_SIZE                  (sizeof(HCI_Extended_Inquiry_Result_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Encryption Key Refresh Event (Version 2.1 + EDR).
    */
typedef struct _tagHCI_Encryption_Key_Refresh_Complete_Event_Data_t
{
   Byte_t Status;
   Word_t Connection_Handle;
} HCI_Encryption_Key_Refresh_Complete_Event_Data_t;

#define HCI_ENCRYPTION_KEY_REFRESH_COMPLETE_EVENT_DATA_SIZE          (sizeof(HCI_Encryption_Key_Refresh_Complete_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI I/O Capability Request Event (Version 2.1 + EDR).
    */
typedef struct _tagHCI_IO_Capability_Request_Event_Data_t
{
   BD_ADDR_t BD_ADDR;
} HCI_IO_Capability_Request_Event_Data_t;

#define HCI_IO_CAPABILITY_REQUEST_EVENT_DATA_SIZE                    (sizeof(HCI_IO_Capability_Request_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI I/O Capability Response Event (Version 2.1 + EDR).
    */
typedef struct _tagHCI_IO_Capability_Response_Event_Data_t
{
   BD_ADDR_t BD_ADDR;
   Byte_t    IO_Capability;
   Byte_t    OOB_Data_Present;
   Byte_t    Authentication_Requirements;
} HCI_IO_Capability_Response_Event_Data_t;

#define HCI_IO_CAPABILITY_RESPONSE_EVENT_DATA_SIZE                   (sizeof(HCI_IO_Capability_Response_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI User Confirmation Request Event (Version 2.1 + EDR).
    */
typedef struct _tagHCI_User_Confirmation_Request_Event_Data_t
{
   BD_ADDR_t BD_ADDR;
   DWord_t   Numeric_Value;
} HCI_User_Confirmation_Request_Event_Data_t;

#define HCI_USER_CONFIRMATION_REQUEST_EVENT_DATA_SIZE                (sizeof(HCI_User_Confirmation_Request_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI User Passkey Request Event (Version 2.1 + EDR).
    */
typedef struct _tagHCI_User_Passkey_Request_Event_Data_t
{
   BD_ADDR_t BD_ADDR;
} HCI_User_Passkey_Request_Event_Data_t;

#define HCI_USER_PASSKEY_REQUEST_EVENT_DATA_SIZE                     (sizeof(HCI_User_Passkey_Request_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Remote OOB Data Request Event (Version 2.1 + EDR).
    */
typedef struct _tagHCI_Remote_OOB_Data_Request_Event_Data_t
{
   BD_ADDR_t BD_ADDR;
} HCI_Remote_OOB_Data_Request_Event_Data_t;

#define HCI_REMOTE_OOB_DATA_REQUEST_EVENT_DATA_SIZE                  (sizeof(HCI_Remote_OOB_Data_Request_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Simple Pairing Complete Event (Version 2.1 + EDR).
    */
typedef struct _tagHCI_Simple_Pairing_Complete_Event_Data_t
{
   Byte_t    Status;
   BD_ADDR_t BD_ADDR;
} HCI_Simple_Pairing_Complete_Event_Data_t;

#define HCI_SIMPLE_PAIRING_COMPLETE_EVENT_DATA_SIZE                  (sizeof(HCI_Simple_Pairing_Complete_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Link Supervision Timeout Changed Event (Version 2.1 +
    * EDR).
    */
typedef struct _tagHCI_Link_Supervision_Timeout_Changed_Event_Data_t
{
   Word_t Connection_Handle;
   Word_t Link_Supervision_Timeout;
} HCI_Link_Supervision_Timeout_Changed_Event_Data_t;

#define HCI_LINK_SUPERVISION_TIMEOUT_CHANGED_EVENT_DATA_SIZE         (sizeof(HCI_Link_Supervision_Timeout_Changed_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Enhanced Flush Complete Event (Version 2.1 + EDR).
    */
typedef struct _tagHCI_Enhanced_Flush_Complete_Event_Data_t
{
   Word_t Connection_Handle;
} HCI_Enhanced_Flush_Complete_Event_Data_t;

#define HCI_ENHANCED_FLUSH_COMPLETE_EVENT_DATA_SIZE                  (sizeof(HCI_Enhanced_Flush_Complete_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI User Passkey Notification Event (Version 2.1 + EDR).
    */
typedef struct _tagHCI_User_Passkey_Notification_Event_Data_t
{
   BD_ADDR_t BD_ADDR;
   DWord_t   Passkey;
} HCI_User_Passkey_Notification_Event_Data_t;

#define HCI_USER_PASSKEY_NOTIFICATION_EVENT_DATA_SIZE                (sizeof(HCI_User_Passkey_Notification_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Keypress Notification Event (Version 2.1 + EDR).
    */
typedef struct _tagHCI_Keypress_Notification_Event_Data_t
{
   BD_ADDR_t BD_ADDR;
   Byte_t    Notification_Type;
} HCI_Keypress_Notification_Event_Data_t;

#define HCI_KEYPRESS_NOTIFICATION_EVENT_DATA_SIZE                    (sizeof(HCI_Keypress_Notification_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Remote Host Supported Features Notification Event
    * (Version 2.1 + EDR).
    */
typedef struct _tagHCI_Remote_Host_Supported_Features_Notification_Event_Data_t
{
   BD_ADDR_t      BD_ADDR;
   LMP_Features_t Host_Supported_Features;
} HCI_Remote_Host_Supported_Features_Notification_Event_Data_t;

#define HCI_REMOTE_HOST_SUPPORTED_FEATURES_NOTIFICATION_EVENT_DATA_SIZE    (sizeof(HCI_Remote_Host_Supported_Features_Notification_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Physical Link Complete Event (Version 3.0 + HS).
    */
typedef struct _tagHCI_Physical_Link_Complete_Event_Data_t
{
   Byte_t Status;
   Byte_t Physical_Link_Handle;
} HCI_Physical_Link_Complete_Event_Data_t;

#define HCI_PHYSICAL_LINK_COMPLETE_EVENT_DATA_SIZE                         (sizeof(HCI_Physical_Link_Complete_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Channel Selected Event (Version 3.0 + HS).
    */
typedef struct _tagHCI_Channel_Selected_Event_Data_t
{
   Byte_t Physical_Link_Handle;
} HCI_Channel_Selected_Event_Data_t;

#define HCI_CHANNEL_SELECTED_EVENT_DATA_SIZE                               (sizeof(HCI_Channel_Selected_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Disconnection Physical Link Complete Event (Version
    * 3.0 + HS).
    */
typedef struct _tagHCI_Disconnection_Physical_Link_Complete_Event_Data_t
{
   Byte_t Status;
   Byte_t Physical_Link_Handle;
   Byte_t Reason;
} HCI_Disconnection_Physical_Link_Complete_Event_Data_t;

#define HCI_DISCONNECTION_PHYSICAL_LINK_COMPLETE_EVENT_DATA_SIZE           (sizeof(HCI_Disconnection_Physical_Link_Complete_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Physical Link Loss Early Warning Event (Version 3.0 +
    * HS).
    */
typedef struct _tagHCI_Physical_Link_Loss_Early_Warning_Event_Data_t
{
   Byte_t Physical_Link_Handle;
   Byte_t Link_Loss_Reason;
} HCI_Physical_Link_Loss_Early_Warning_Event_Data_t;

#define HCI_PHYSICAL_LINK_LOSS_EARLY_WARNING_EVENT_DATA_SIZE               (sizeof(HCI_Physical_Link_Loss_Early_Warning_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Physical Link Recovery Event (Version 3.0 + HS).
    */
typedef struct _tagHCI_Physical_Link_Recovery_Event_Data_t
{
   Byte_t Physical_Link_Handle;
} HCI_Physical_Link_Recovery_Event_Data_t;

#define HCI_PHYSICAL_LINK_RECOVERY_EVENT_DATA_SIZE                         (sizeof(HCI_Physical_Link_Recovery_Event_Data_t))

   /*! The following structure represents the format of the defined
    * Extended Flow Spec information.  This is used with various HCI
    * Commands/Events that deal with Extended Flow Specifications
    * (Version 3.0 + HS).
    */
typedef struct _tagHCI_Extended_Flow_Spec_Data_t
{
   Byte_t  Identifier;
   Byte_t  Service_Type;
   Word_t  Maximum_SDU_Size;
   DWord_t SDU_Inter_Arrival_Time;
   DWord_t Access_Latency;
   DWord_t Flush_Timeout;
} HCI_Extended_Flow_Spec_Data_t;

#define HCI_EXTENDED_FLOW_SPEC_DATA_SIZE                                   (sizeof(HCI_Extended_Flow_Spec_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Logical Link Complete Event (Version 3.0 + HS).
    */
typedef struct _tagHCI_Logical_Link_Complete_Event_Data_t
{
   Byte_t Status;
   Word_t Logical_Link_Handle;
   Byte_t Physical_Link_Handle;
   Byte_t Tx_Flow_Spec_ID;
} HCI_Logical_Link_Complete_Event_Data_t;

#define HCI_LOGICAL_LINK_COMPLETE_EVENT_DATA_SIZE                          (sizeof(HCI_Logical_Link_Complete_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Disconnection Logical Link Complete Event (Version
    * 3.0 + HS).
    */
typedef struct _tagHCI_Disconnection_Logical_Link_Complete_Event_Data_t
{
   Byte_t Status;
   Word_t Logical_Link_Handle;
   Byte_t Reason;
} HCI_Disconnection_Logical_Link_Complete_Event_Data_t;

#define HCI_DISCONNECTION_LOGICAL_LINK_COMPLETE_EVENT_DATA_SIZE            (sizeof(HCI_Disconnection_Logical_Link_Complete_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Flow Spec Modify Complete Event (Version 3.0 + HS).
    */
typedef struct _tagHCI_Flow_Spec_Modify_Complete_Event_Data_t
{
   Byte_t Status;
   Word_t Handle;
} HCI_Flow_Spec_Modify_Complete_Event_Data_t;

#define HCI_FLOW_SPEC_MODIFY_COMPLETE_EVENT_DATA_SIZE                      (sizeof(HCI_Flow_Spec_Modify_Complete_Event_Data_t))

   /*! The following structure is used with the HCI Number of Completed
    * Data Blocks Event, and represents a single Connection Handle/Host
    * Number of Completed Packets/Number of Data Blocks triplet that is
    * required by this Event.  This structure represents an individual
    * entry and a HCI Number of Completed Data Blocks Event will contain
    * a variable number of these structures (Version 3.0 + HS).
    */
typedef struct _tagHCI_Number_Of_Completed_Data_Blocks_Data_t
{
   Word_t Handle;
   Word_t Num_Of_Completed_Packets;
   Word_t Num_Of_Completed_Blocks;
} HCI_Number_Of_Completed_Data_Blocks_Data_t;

#define HCI_NUMBER_OF_COMPLETED_DATA_BLOCKS_DATA_SIZE                      (sizeof(HCI_Number_Of_Completed_Data_Blocks_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Number Of Completed Data Blocks Event (Version 3.0 +
    * HS).
    * The following structure represents the Data that is associated
    * with the HCI Number Of Completed Data Blocks Event (Version 3.0 +
    * HS).  The HCI_Number_Of_Completed_Data_Blocks_Data member
    * represents a variable array that can contain 0 or more entries.
    * The number of Entries in this array is given by the
    * Number_of_Handles member.  A utility MACRO is provided to aid in
    * the calculation of the Number of Bytes that are occupied by the
    * structure given a specified number of Handles.
    */
typedef struct _tagHCI_Number_Of_Completed_Data_Blocks_Event_Data_t
{
   Word_t                                     Total_Num_Data_Blocks;
   Byte_t                                     Number_of_Handles;
   HCI_Number_Of_Completed_Data_Blocks_Data_t HCI_Number_Of_Completed_Data_Blocks_Data[1];
} HCI_Number_Of_Completed_Data_Blocks_Event_Data_t;

   /*! The following MACRO is a utility MACRO that exists to aid code
    * readability to Determine the size (in Bytes) of an HCI Number of
    * Completed Data Blocks Event Data Structure based upon the number
    * of HCI Number of Completed Data Blocks Event Data Entries
    * associated with the Event. The first parameter to this MACRO is
    * the number of HCI Number of Completed Data Blocks Entries.
    */
#define HCI_NUMBER_OF_COMPLETED_DATA_BLOCKS_EVENT_DATA_SIZE(_x)            ((sizeof(HCI_Number_Of_Completed_Data_Blocks_Event_Data_t) - sizeof(HCI_Number_Of_Completed_Data_Blocks_Data_t)) + (((Byte_t)(_x))*sizeof(HCI_Number_Of_Completed_Data_Blocks_Data_t)))

#define HCI_NUMBER_OF_COMPLETED_DATA_BLOCKS_EVENT_MAX_DATA_ENTRIES         ((((sizeof(Byte_t))*256)-HCI_NUMBER_OF_COMPLETED_DATA_BLOCKS_EVENT_DATA_SIZE(0))/sizeof(HCI_Number_Of_Completed_Data_Blocks_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Number Completed Data Blocks Event (Version 3.0 +
    * HS).
    */
typedef struct _tagHCI_Short_Range_Mode_Change_Complete_Event_Data_t
{
   Byte_t Status;
   Byte_t Physical_Link_Handle;
   Byte_t Short_Range_Mode_State;
} HCI_Short_Range_Mode_Change_Complete_Event_Data_t;

#define HCI_SHORT_RANGE_MODE_CHANGE_COMPLETE_EVENT_DATA_SIZE               (sizeof(HCI_Short_Range_Mode_Change_Complete_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI AMP Status Change Event (Version 3.0 + HS).
    */
typedef struct _tagHCI_AMP_Status_Change_Event_Data_t
{
   Byte_t Status;
   Byte_t AMP_Status;
} HCI_AMP_Status_Change_Event_Data_t;

#define HCI_AMP_STATUS_CHANGE_EVENT_DATA_SIZE                              (sizeof(HCI_AMP_Status_Change_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI AMP Start Test Event (Version 3.0 + HS).
    */
typedef struct _tagHCI_AMP_Start_Test_Event_Data_t
{
   Byte_t Status;
   Byte_t Test_Scenario;
} HCI_AMP_Start_Test_Event_Data_t;

#define HCI_AMP_START_TEST_EVENT_DATA_SIZE                                 (sizeof(HCI_AMP_Start_Test_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI AMP Test End Event (Version 3.0 + HS).
    */
typedef struct _tagHCI_AMP_Test_End_Event_Data_t
{
   Byte_t Status;
   Byte_t Test_Scenario;
} HCI_AMP_Test_End_Event_Data_t;

#define HCI_AMP_TEST_END_EVENT_DATA_SIZE                                   (sizeof(HCI_AMP_Test_End_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI AMP Receiver Report Event (Version 3.0 + HS).
    */
typedef struct _tagHCI_AMP_Receiver_Report_Event_Data_t
{
   Byte_t  Controller_Type;
   Byte_t  Reason;
   DWord_t Event_Type;
   Word_t  Number_Of_Frames;
   Word_t  Number_Of_Error_Frames;
   DWord_t Number_Of_Bits;
   DWord_t Number_Of_Error_Bits;
} HCI_AMP_Receiver_Report_Event_Data_t;

#define HCI_AMP_RECEIVER_REPORT_EVENT_DATA_SIZE                            (sizeof(HCI_AMP_Receiver_Report_Event_Data_t))

   /*! The following structures represent the Data that is associated
    * with the LE Meta Event. (Version 4.0 + LE).
    * The following structure represents the data that is returned in a
    * HCI LE Meta Event with Subevent LE Connection Complete Event.
    * (Version 4.0 + LE).
    */
typedef struct _tagHCI_LE_Connection_Complete_Event_Data_t
{
   Byte_t    Status;
   Word_t    Connection_Handle;
   Byte_t    Role;
   Byte_t    Peer_Address_Type;
   BD_ADDR_t Peer_Address;
   Word_t    Conn_Interval;
   Word_t    Conn_Latency;
   Word_t    Supervision_Timeout;
   Byte_t    Master_Clock_Accuracy;
} HCI_LE_Connection_Complete_Event_Data_t;

#define HCI_LE_CONNECTION_COMPLETE_EVENT_DATA_SIZE                         (sizeof(HCI_LE_Connection_Complete_Event_Data_t))

   /*! The following structure represents the data that is returned in a
    * HCI LE Meta Event with Subevent LE Avertising Report Event.  This
    * structure represents an individual entry, and the HCI LE
    * Advertising Report Event structure will contain an array of
    * these structures (based upon the number of responses).  (Version
    * 4.0 + LE).
    */
typedef struct _tagHCI_LE_Advertising_Report_Data_t
{
   Byte_t             Event_Type;
   Byte_t             Address_Type;
   BD_ADDR_t          Address;
   Byte_t             Data_Length;
   Advertising_Data_t Data;
   SByte_t            RSSI;
} HCI_LE_Advertising_Report_Data_t;

#define HCI_LE_ADVERTISING_REPORT_DATA_SIZE                                (sizeof(HCI_LE_Advertising_Report_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI LE Meta Event with Subevent LE Avertising Report
    * Event.  The HCI_LE_Advertising_Report_Data member represents a
    * variable array that can contain 0 or more entries.  The Number of
    * Entries in this array is given by the Num_Responses member.  A
    * utility MACRO is provided to aid in the calculation of the Number
    * of Bytes that are occupied by the structure given a specified
    * number of Responses.  (Version 4.0 + LE).
    */
typedef struct _tagHCI_LE_Advertising_Report_Event_Data_t
{
   Byte_t                           Num_Responses;
   HCI_LE_Advertising_Report_Data_t HCI_LE_Advertising_Report_Data[1];
} HCI_LE_Advertising_Report_Event_Data_t;

   /*! The following MACRO is a utility MACRO that exists to aid code
    * readability to Determine the size (in Bytes) of an HCI LE
    * Advertising Report Event Data Structure that must be allocated to
    * store the number of HCI LE Advertising Report Event Entries
    * associated with the Event. The first parameter to this MACRO is
    * the number of HCI LE Advertising Event Entries (Version 4.0 + LE).
    */
#define HCI_LE_ADVERTISING_REPORT_EVENT_DATA_SIZE(_x)                      ((sizeof(HCI_LE_Advertising_Report_Event_Data_t) - sizeof(HCI_LE_Advertising_Report_Data_t)) + (((unsigned int)(_x))*sizeof(HCI_LE_Advertising_Report_Data_t)))

#define HCI_LE_ADVERTISING_REPORT_EVENT_MAX_DATA_ENTRIES                   ((((sizeof(Byte_t))*256)-HCI_LE_ADVERTISING_REPORT_EVENT_DATA_SIZE(0))/sizeof(HCI_LE_Advertising_Report_Data_t))

   /*! The following structure represents the data that is returned in a
    * HCI LE Meta Event with Subevent LE Connection Update Complete
    * Event (Vesion 4.0 + LE).
    */
typedef struct _tagHCI_LE_Connection_Update_Complete_Event_Data_t
{
   Byte_t Status;
   Word_t Connection_Handle;
   Word_t Conn_Interval;
   Word_t Conn_Latency;
   Word_t Supervision_Timeout;
} HCI_LE_Connection_Update_Complete_Event_Data_t;

#define HCI_LE_CONNECTION_UPDATE_COMPLETE_EVENT_DATA_SIZE                  (sizeof(HCI_LE_Connection_Update_Complete_Event_Data_t))

   /*! The following structure represents the data that is returned in a
    * HCI LE Meta Event with Subevent LE Read Remote Used Features
    * Complete Event (Version 4.0 + LE).
    */
typedef struct _tagHCI_LE_Read_Remote_Used_Features_Complete_Event_Data_t
{
   Byte_t        Status;
   Word_t        Connection_Handle;
   LE_Features_t LE_Features;
} HCI_LE_Read_Remote_Used_Features_Complete_Event_Data_t;

#define HCI_LE_READ_REMOTE_USED_FEATURES_COMPLETE_EVENT_DATA_SIZE          (sizeof(HCI_LE_Read_Remote_Used_Features_Complete_Event_Data_t))

   /*! The following structure represents the data that is returned in a
    * HCI LE Meta Event with Subevent LE Long Term Key Request Event
    * (Vesion 4.0 + LE).
    */
typedef struct _tagHCI_LE_Long_Term_Key_Request_Event_Data_t
{
   Word_t          Connection_Handle;
   Random_Number_t Random_Number;
   Word_t          Encrypted_Diversifier;
} HCI_LE_Long_Term_Key_Request_Event_Data_t;

#define HCI_LE_LONG_TERM_KEY_REQUEST_EVENT_DATA_SIZE                       (sizeof(HCI_LE_Long_Term_Key_Request_Event_Data_t))

   /*! The following structure represents the data that is returned in a
    * HCI LE Meta Event with Subevent LE Remote Connection Parameter
    * Request Event (Vesion 4.1).
    */
typedef struct _tagHCI_LE_Remote_Connection_Parameter_Request_Event_Data_t
{
   Word_t Connection_Handle;
   Word_t Conn_Interval_Min;
   Word_t Conn_Interval_Max;
   Word_t Conn_Latency;
   Word_t Supervision_Timeout;
} HCI_LE_Remote_Connection_Parameter_Request_Event_Data_t;

#define HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_EVENT_DATA_SIZE         (sizeof(HCI_LE_Remote_Connection_Parameter_Request_Event_Data_t))

   /*! The following represents the structure of an LE Meta Event.
    */
typedef struct _tagHCI_LE_Meta_Event_Data_t
{
   HCI_LE_Meta_Event_Type_t LE_Event_Data_Type;
   union
   {
      HCI_LE_Connection_Complete_Event_Data_t                  HCI_LE_Connection_Complete_Event_Data;
      HCI_LE_Advertising_Report_Event_Data_t                   HCI_LE_Advertising_Report_Event_Data;
      HCI_LE_Connection_Update_Complete_Event_Data_t           HCI_LE_Connection_Update_Complete_Event_Data;
      HCI_LE_Read_Remote_Used_Features_Complete_Event_Data_t   HCI_LE_Read_Remote_Used_Features_Complete_Event_Data;
      HCI_LE_Long_Term_Key_Request_Event_Data_t                HCI_LE_Long_Term_Key_Request_Event_Data;
      HCI_LE_Remote_Connection_Parameter_Request_Event_Data_t  HCI_LE_Remote_Connection_Parameter_Request_Event_Data;
   } Event_Data;
} HCI_LE_Meta_Event_Data_t;

#define HCI_LE_META_EVENT_DATA_SIZE                                        (sizeof(HCI_LE_Meta_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Platform Specific Event.
    */
typedef struct _tagHCI_Platform_Specific_Event_Data_t
{
   DWord_t  Platform_Event_Type;
   void    *Platform_Event_Data;
} HCI_Platform_Specific_Event_Data_t;

#define HCI_PLATFORM_SPECIFIC_EVENT_DATA_SIZE                              (sizeof(HCI_Platform_Specific_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Triggered Clock Capture Event.
    */
typedef struct _tagHCI_Triggered_Clock_Capture_Event_Data_t
{
   Word_t  Connection_Handle;
   Byte_t  Which_Clock;
   DWord_t Clock;
   Word_t  Slot_Offset;
} HCI_Triggered_Clock_Capture_Event_Data_t;

#define HCI_TRIGGERED_CLOCK_CAPTURE_EVENT_DATA_SIZE                        (sizeof(HCI_Triggered_Clock_Capture_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Synchronization Train Received Event.
    */
typedef struct _tagHCI_Synchronization_Train_Complete_Event_Data_t
{
   Byte_t Status;
} HCI_Synchronization_Train_Complete_Event_Data_t;

#define HCI_SYNCHRONIZATION_TRAIN_COMPLETE_EVENT_DATA_SIZE                 (sizeof(HCI_Synchronization_Train_Complete_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Synchronization Train Received Event.
    */
typedef struct _tagHCI_Synchronization_Train_Received_Event_Data_t
{
   Byte_t            Status;
   BD_ADDR_t         BD_ADDR;
   DWord_t           Clock_Offset;
   AFH_Channel_Map_t AFH_Channel_Map;
   Byte_t            LT_ADDR;
   DWord_t           Next_Broadcast_Instant;
   Word_t            Connectionless_Slave_Broadcast_Interval;
   Byte_t            Service_Data;
} HCI_Synchronization_Train_Received_Event_Data_t;

#define HCI_SYNCHRONIZATION_TRAIN_RECEIVED_EVENT_DATA_SIZE                 (sizeof(HCI_Synchronization_Train_Received_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Connectionless Slave Broadcast Receive Event.
    */
typedef struct _tagHCI_Connectionless_Slave_Broadcast_Receive_Event_Data_t
{
   BD_ADDR_t BD_ADDR;
   Byte_t    LT_ADDR;
   DWord_t   CLK;
   DWord_t   Offset;
   Byte_t    Receive_Status;
   Byte_t    Fragment;
   Byte_t    Data_Length;
   Byte_t    Data[1];
} HCI_Connectionless_Slave_Broadcast_Receive_Event_Data_t;

   /*! The following MACRO is a utility MACRO that exists to aid code
    * readability to Determine the size (in Bytes) of an
    * HCI Inquiry Result Event Data Structure based upon the number of
    * HCI Inquiry Result Entries associated with the Event.
    * @param _x The first parameter to this MACRO is the number of HCI Inquiry Result
    * Entries.
    */
#define HCI_CONNECTIONLESS_SLAVE_BROADCAST_RECEIVE_EVENT_DATA_SIZE(_x)     ((sizeof(HCI_Connectionless_Slave_Broadcast_Receive_Event_Data_t) - sizeof(Byte_t)) + (((Byte_t)(_x))*sizeof(Byte_t)))

   /*! The following structure represents the Data that is associated
    * with the HCI Connectionless Slave Broadcast Timeout Event.
    */
typedef struct _tagHCI_Connectionless_Slave_Broadcast_Timeout_Event_Data_t
{
   BD_ADDR_t BD_ADDR;
   Byte_t    LT_ADDR;
} HCI_Connectionless_Slave_Broadcast_Timeout_Event_Data_t;

#define HCI_CONNECTIONLESS_SLAVE_BROADCAST_TIMEOUT_EVENT_DATA_SIZE         (sizeof(HCI_Connectionless_Slave_Broadcast_Timeout_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Truncated Page Complete Timeout Event.
    */
typedef struct _tagHCI_Truncated_Page_Complete_Event_Data_t
{
   Byte_t    Status;
   BD_ADDR_t BD_ADDR;
} HCI_Truncated_Page_Complete_Event_Data_t;

#define HCI_TRUNCATED_PAGE_COMPLETE_EVENT_DATA_SIZE                        (sizeof(HCI_Truncated_Page_Complete_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Connectionless Slave Broadcast Channel Map Change
    * Event.
    */
typedef struct _tagHCI_Connectionless_Slave_Broadcast_Channel_Map_Change_Event_Data_t
{
   AFH_Channel_Map_t Channel_Map;
} HCI_Connectionless_Slave_Broadcast_Channel_Map_Change_Event_Data_t;

#define HCI_CONNECTIONLESS_SLAVE_BROADCAST_CHANNEL_MAP_CHANGE_EVENT_DATA_SIZE (sizeof(HCI_Connectionless_Slave_Broadcast_Channel_Map_Change_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Inquiry Response Notification Event.
    */
typedef struct _tagHCI_Inquiry_Response_Notification_Event_Data_t
{
   LAP_t  LAP;
   Byte_t RSSI;
} HCI_Inquiry_Response_Notification_Event_Data_t;

#define HCI_INQUIRY_RESPONSE_NOTIFICATION_EVENT_DATA_SIZE                  (sizeof(HCI_Inquiry_Response_Notification_Event_Data_t))

   /*! The following structure represents the Data that is associated
    * with the HCI Authenticated Payload Timeout Expired Event.
    */
typedef struct _tagHCI_Authenticated_Payload_Timeout_Expired_Event_Data_t
{
   Word_t Connection_Handle;
} HCI_Authenticated_Payload_Timeout_Expired_Event_Data_t;

#define HCI_AUTHENTICATED_PAYLOAD_TIMEOUT_EXPIRED_EVENT_DATA_SIZE          (sizeof(HCI_Authenticated_Payload_Timeout_Expired_Event_Data_t))

   /*! The following structure represents the container structure for
    * Holding all HCI Event Data Data.
    */
typedef struct _tagHCI_Event_Data_t
{
   HCI_Event_Type_t Event_Data_Type;
   Word_t           Event_Data_Size;
   union
   {
      HCI_Inquiry_Complete_Event_Data_t                                  *HCI_Inquiry_Complete_Event_Data;
      HCI_Inquiry_Result_Event_Data_t                                    *HCI_Inquiry_Result_Event_Data;
      HCI_Connection_Complete_Event_Data_t                               *HCI_Connection_Complete_Event_Data;
      HCI_Connection_Request_Event_Data_t                                *HCI_Connection_Request_Event_Data;
      HCI_Disconnection_Complete_Event_Data_t                            *HCI_Disconnection_Complete_Event_Data;
      HCI_Authentication_Complete_Event_Data_t                           *HCI_Authentication_Complete_Event_Data;
      HCI_Remote_Name_Request_Complete_Event_Data_t                      *HCI_Remote_Name_Request_Complete_Event_Data;
      HCI_Encryption_Change_Event_Data_t                                 *HCI_Encryption_Change_Event_Data;
      HCI_Change_Connection_Link_Key_Complete_Event_Data_t               *HCI_Change_Connection_Link_Key_Complete_Event_Data;
      HCI_Master_Link_Key_Complete_Event_Data_t                          *HCI_Master_Link_Key_Complete_Event_Data;
      HCI_Read_Remote_Supported_Features_Complete_Event_Data_t           *HCI_Read_Remote_Supported_Features_Complete_Event_Data;
      HCI_Read_Remote_Version_Information_Complete_Event_Data_t          *HCI_Read_Remote_Version_Information_Complete_Event_Data;
      HCI_QoS_Setup_Complete_Event_Data_t                                *HCI_QoS_Setup_Complete_Event_Data;
      HCI_Hardware_Error_Event_Data_t                                    *HCI_Hardware_Error_Event_Data;
      HCI_Flush_Occurred_Event_Data_t                                    *HCI_Flush_Occurred_Event_Data;
      HCI_Role_Change_Event_Data_t                                       *HCI_Role_Change_Event_Data;
      HCI_Number_Of_Completed_Packets_Event_Data_t                       *HCI_Number_Of_Completed_Packets_Event_Data;
      HCI_Mode_Change_Event_Data_t                                       *HCI_Mode_Change_Event_Data;
      HCI_Return_Link_Keys_Event_Data_t                                  *HCI_Return_Link_Keys_Event_Data;
      HCI_PIN_Code_Request_Event_Data_t                                  *HCI_PIN_Code_Request_Event_Data;
      HCI_Link_Key_Request_Event_Data_t                                  *HCI_Link_Key_Request_Event_Data;
      HCI_Link_Key_Notification_Event_Data_t                             *HCI_Link_Key_Notification_Event_Data;
      HCI_Loopback_Command_Event_Data_t                                  *HCI_Loopback_Command_Event_Data;
      HCI_Data_Buffer_Overflow_Event_Data_t                              *HCI_Data_Buffer_Overflow_Event_Data;
      HCI_Max_Slots_Change_Event_Data_t                                  *HCI_Max_Slots_Change_Event_Data;
      HCI_Read_Clock_Offset_Complete_Event_Data_t                        *HCI_Read_Clock_Offset_Complete_Event_Data;
      HCI_Connection_Packet_Type_Changed_Event_Data_t                    *HCI_Connection_Packet_Type_Changed_Event_Data;
      HCI_QoS_Violation_Event_Data_t                                     *HCI_QoS_Violation_Event_Data;
      HCI_Page_Scan_Repetition_Mode_Change_Event_Data_t                  *HCI_Page_Scan_Repetition_Mode_Change_Event_Data;
      HCI_Page_Scan_Mode_Change_Event_Data_t                             *HCI_Page_Scan_Mode_Change_Event_Data;
      HCI_Flow_Specification_Complete_Event_Data_t                       *HCI_Flow_Specification_Complete_Event_Data;
      HCI_Inquiry_Result_With_RSSI_Event_Data_t                          *HCI_Inquiry_Result_With_RSSI_Event_Data;
      HCI_Read_Remote_Extended_Features_Complete_Event_Data_t            *HCI_Read_Remote_Extended_Features_Complete_Event_Data;
      HCI_Synchronous_Connection_Complete_Event_Data_t                   *HCI_Synchronous_Connection_Complete_Event_Data;
      HCI_Synchronous_Connection_Changed_Event_Data_t                    *HCI_Synchronous_Connection_Changed_Event_Data;
      HCI_Sniff_Subrating_Event_Data_t                                   *HCI_Sniff_Subrating_Event_Data;
      HCI_Extended_Inquiry_Result_Event_Data_t                           *HCI_Extended_Inquiry_Result_Event_Data;
      HCI_Encryption_Key_Refresh_Complete_Event_Data_t                   *HCI_Encryption_Key_Refresh_Complete_Event_Data;
      HCI_IO_Capability_Request_Event_Data_t                             *HCI_IO_Capability_Request_Event_Data;
      HCI_IO_Capability_Response_Event_Data_t                            *HCI_IO_Capability_Response_Event_Data;
      HCI_User_Confirmation_Request_Event_Data_t                         *HCI_User_Confirmation_Request_Event_Data;
      HCI_User_Passkey_Request_Event_Data_t                              *HCI_User_Passkey_Request_Event_Data;
      HCI_Remote_OOB_Data_Request_Event_Data_t                           *HCI_Remote_OOB_Data_Request_Event_Data;
      HCI_Simple_Pairing_Complete_Event_Data_t                           *HCI_Simple_Pairing_Complete_Event_Data;
      HCI_Link_Supervision_Timeout_Changed_Event_Data_t                  *HCI_Link_Supervision_Timeout_Changed_Event_Data;
      HCI_Enhanced_Flush_Complete_Event_Data_t                           *HCI_Enhanced_Flush_Complete_Event_Data;
      HCI_User_Passkey_Notification_Event_Data_t                         *HCI_User_Passkey_Notification_Event_Data;
      HCI_Keypress_Notification_Event_Data_t                             *HCI_Keypress_Notification_Event_Data;
      HCI_Remote_Host_Supported_Features_Notification_Event_Data_t       *HCI_Remote_Host_Supported_Features_Notification_Event_Data;
      HCI_Physical_Link_Complete_Event_Data_t                            *HCI_Physical_Link_Complete_Event_Data;
      HCI_Channel_Selected_Event_Data_t                                  *HCI_Channel_Selected_Event_Data;
      HCI_Disconnection_Physical_Link_Complete_Event_Data_t              *HCI_Disconnection_Physical_Link_Complete_Event_Data;
      HCI_Physical_Link_Loss_Early_Warning_Event_Data_t                  *HCI_Physical_Link_Loss_Early_Warning_Event_Data;
      HCI_Physical_Link_Recovery_Event_Data_t                            *HCI_Physical_Link_Recovery_Event_Data;
      HCI_Logical_Link_Complete_Event_Data_t                             *HCI_Logical_Link_Complete_Event_Data;
      HCI_Disconnection_Logical_Link_Complete_Event_Data_t               *HCI_Disconnection_Logical_Link_Complete_Event_Data;
      HCI_Flow_Spec_Modify_Complete_Event_Data_t                         *HCI_Flow_Spec_Modify_Complete_Event_Data;
      HCI_Number_Of_Completed_Data_Blocks_Event_Data_t                   *HCI_Number_Of_Completed_Data_Blocks_Event_Data;
      HCI_Short_Range_Mode_Change_Complete_Event_Data_t                  *HCI_Short_Range_Mode_Change_Complete_Event_Data;
      HCI_AMP_Status_Change_Event_Data_t                                 *HCI_AMP_Status_Change_Event_Data;
      HCI_AMP_Start_Test_Event_Data_t                                    *HCI_AMP_Start_Test_Event_Data;
      HCI_AMP_Test_End_Event_Data_t                                      *HCI_AMP_Test_End_Event_Data;
      HCI_AMP_Receiver_Report_Event_Data_t                               *HCI_AMP_Receiver_Report_Event_Data;
      HCI_LE_Meta_Event_Data_t                                           *HCI_LE_Meta_Event_Data;
      HCI_Triggered_Clock_Capture_Event_Data_t                           *HCI_Triggered_Clock_Capture_Event_Data;
      HCI_Synchronization_Train_Complete_Event_Data_t                    *HCI_Synchronization_Train_Complete_Event_Data;
      HCI_Synchronization_Train_Received_Event_Data_t                    *HCI_Synchronization_Train_Received_Event_Data;
      HCI_Connectionless_Slave_Broadcast_Receive_Event_Data_t            *HCI_Connectionless_Slave_Broadcast_Receive_Event_Data;
      HCI_Connectionless_Slave_Broadcast_Timeout_Event_Data_t            *HCI_Connectionless_Slave_Broadcast_Timeout_Event_Data;
      HCI_Truncated_Page_Complete_Event_Data_t                           *HCI_Truncated_Page_Complete_Event_Data;
      HCI_Connectionless_Slave_Broadcast_Channel_Map_Change_Event_Data_t *HCI_Connectionless_Slave_Broadcast_Channel_Map_Change_Event_Data;
      HCI_Inquiry_Response_Notification_Event_Data_t                     *HCI_Inquiry_Response_Notification_Event_Data;
      HCI_Authenticated_Payload_Timeout_Expired_Event_Data_t             *HCI_Authenticated_Payload_Timeout_Expired_Event_Data;
      HCI_Platform_Specific_Event_Data_t                                 *HCI_Platform_Specific_Event_Data;
      void                                                               *HCI_Unknown_Event_Data;
   } Event_Data;
} HCI_Event_Data_t;

#define HCI_EVENT_DATA_SIZE                                                (sizeof(HCI_Event_Data_t))

   /*! The following declared type represents the Prototype Function for
    * an HCI Event Receive Data Callback.  This function will be called
    * whenever a complete HCI Event Packet has been received by the
    * HCI Layer that is associated with the specified Bluetooth Stack
    * ID. @param BluetoothStackID This function passes to the caller the Bluetooth Stack ID.
    * @param HCI_Event_Data The HCI Event Data that was received.
    * @param CallbackParameter The HCI Event Callback
    * Parameter that was specified when this Callback was installed.
    * The caller is free to use the contents of the HCI Event Data ONLY
    * in the context of this callback.  If the caller requires the Data
    * for a longer period of time, then the callback function MUST copy
    * the data into another Data Buffer.  This function is guaranteed
    * NOT to be invoked more than once simultaneously for the specified
    * installed callback (i.e. this function DOES NOT have be
    * reentrant).  It Needs to be noted however, that if the same
    * Callback is installed more than once, then the callbacks will be
    * called serially.  Because of this, the processing in this function
    * should be as efficient as possible.  It should also be noted that
    * this function is called in the Thread Context of a Thread that
    * the User does NOT own.  Therefore, processing in this function
    * should be as efficient as possible (this argument holds anyway
    * because another HCI Event Packet will not be processed while this
    * function call is outstanding).
    * \note This function MUST NOT Block and wait for events that
    *            can only be satisfied by Receiving HCI Event Packets.
    *            A Deadlock WILL occur because NO HCI Event Receive
    *            Callbacks will be issued while this function is
    *            currently outstanding.
    */
typedef void (BTPSAPI *HCI_Event_Callback_t)(unsigned int BluetoothStackID, HCI_Event_Data_t *HCI_Event_Data, unsigned long CallbackParameter);

   /*! The following declared type represents the Prototype Function for
    * an ACL Data Receive Data Callback.  This function will be called
    * whenever a complete ACL Data Packet has been received by the HCI
    * Layer that is associated with the specified Bluetooth Stack ID.
    * @param BluetoothStackID The first parameter to this
    * function is the Bluetooth Stack ID.
    * @param Connection_Handle This function accepts as it's second argument,
    * the ACL Data that was received.
    * @param Flags The HCI ACL Flags to specify with the Connection Handle
    * when building the ACL Packet.
    * @param ACLDataLength The fourth specifies the Length of the ACL Data
    * which specifies the number of Bytes in the Buffer.
    * @param ACLData The fifth parameter is the physical ACL Data to send.
    * @param CallbackParameter The HCI ACL Data Callback Parameter that was
    * specified when this Callback was installed.
    * The caller is free to use the ACL Data Contents ONLY in the context of this
    * callback. If the caller requires the Data for a longer period of
    * time, then the callback function MUST copy the data into another
    * Data Buffer.  This function is guaranteed NOT to be invoked more
    * than once simultaneously for the specified installed callback
    * (i.e. this function DOES NOT have be reentrant).  It needs to be
    * noted however, that if the same Callback is installed more than
    * once, then the callbacks will be called serially.  Because of
    * this, the processing in this function should be as efficient as
    * possible.  It should also be noted that this function is called
    * in the Thread Context of a Thread that the User does NOT own.
    * Therefore, processing in this function should be as efficient as
    * possible (this argument holds anyway because another ACL Data
    * Packet will not be processed while this function call is
    * outstanding).
    * \note This function MUST NOT Block and wait for events that
    *            can only be satisfied by Receiving HCI Event Packets.
    *            A Deadlock WILL occur because NO HCI Event Receive
    *            Callbacks will be issued while this function is
    *            currently outstanding.
    */
typedef void (BTPSAPI *HCI_ACL_Data_Callback_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Word_t Flags, Word_t ACLDataLength, Byte_t *ACLData, unsigned long CallbackParameter);

   /*! The following declared type represents the Prototype Function for
    * an SCO Data Receive Data Callback.  This function will be called
    * whenever a complete SCO Data Packet has been received by the HCI
    * Layer that is associated with the specified Bluetooth Stack ID.
    * @param BluetoothStackID The first parameter to this
    * function is the Bluetooth Stack ID.
    * @param Connection_Handle This function accepts as it's second argument,
    * the ACL Data that was received.
    * @param Flags Any SCO Flags that were returned.
    * @param SCODataLength The fourth specifies the Length of the SCO Data.
    * @param SCOData The SCO Data that was received.
    * @param CallbackParameter The Callback Parameter that was specified when this
    * Callback was installed.
    * The caller is free to use the SCO Data
    * Contents ONLY in the context of this callback.  If the caller
    * requires the Data for a longer period of time, then the callback
    * function MUST copy the data into another Data Buffer.  This
    * function is guaranteed NOT to be invoked more than once
    * simultaneously for the specified installed callback (i.e. this
    * function DOES NOT have be reentrant).  It needs to be noted
    * however, that if the same Callback is installed more than once,
    * then the callbacks will be called serially.  Because of this, the
    * processing in this function should be as efficient as possible.
    * It should also be noted that this function is called in the
    * Thread Context of a Thread that the User does NOT own.  Therefore,
    * processing in this function should be as efficient as possible
    * (this argument holds anyway because another SCO Data Packet will
    * not be processed while this function call is outstanding).
    * \note This function MUST NOT Block and wait for events that
    *            can only be satisfied by Receiving HCI Event Packets.
    *            A Deadlock WILL occur because NO HCI Event Receive
    *            Callbacks will be issued while this function is
    *            currently outstanding.
    */
typedef void (BTPSAPI *HCI_SCO_Data_Callback_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Word_t Flags, Byte_t SCODataLength, Byte_t *SCOData, unsigned long CallbackParameter);

   /* HCI API's.
    * @brief The following function exists to allow the caller a simple
    * mechanism for determining the HCI Version that is supported by
    * the HCI API Layer present for the specified Bluetooth Protocol
    * Stack. @param BluetoothStackID The first parameter to this function is the Bluetooth
    * Protocol Stack ID of the Bluetooth Protocol Stack that was
    * successfully opened. @param HCI_Version The second parameter is a pointer to a
    * Variable that will receive the HCI Version Information.  This
    * value will only be valid If this function returns zero (which
    * indicates a successful call to the function). This function returns
    * a negative error code as the return value if the call to
    * this function failed.
    * \note This function should be called instead of issuing an
    *          HCI_Read_Local_Version_Information() function call
    *          because this call will NOT contact the Bluetooth Device
    *          to query the version.  This mechanism is provided to
    *          provide a more efficient, abstracted mechanism to
    *          determine the HCI Specification Version supported.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Version_Supported(unsigned int BluetoothStackID, HCI_Version_t *HCI_Version);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Version_Supported_t)(unsigned int BluetoothStackID, HCI_Version_t *HCI_Version);
#endif

   /*! @brief The following function exists to allow the caller a simple
    * mechanism for determining if a specific HCI function is supported
    * by the HCI API Layer present for the specified Bluetooth Protocol
    * Stack.  @param BluetoothStackID The first parameter to this function is the Bluetooth
    * Protocol Stack ID of the Bluetooth Protocol Stack that was
    * successfully opened.
    * @param SupportedCommandBitNumber The second parameter is the supported HCI
    * Command Bit Number (defined in HCITypes.h) for the specified HCI
    * Command that is to be tested. @return This function returns a negative
    * error code as the return value if the call to this function
    * failed, zero if the command is NOT supported, or a positive,
    * non-zero, value if the HCI command is supported.
    * \note This function should be called instead of issuing an
    *          HCI_Read_Local_Supported_Commands() function call
    *          because this call will NOT contact the Bluetooth Device
    *          to query the version (it is already cached in the
    *          HCI layer).  Also, this function will return the correct
    *          value for Bluetooth Devices that do not support the
    *          above call (i.e. Pre Bluetooth 1.2 devices).  This
    *          mechanism is provided to provide a more efficient,
    *          abstracted mechanism to determine if a specified HCI
    *          command is supported by the local Bluetooth Device.
    * \note  The final parameter to this function should be a contant
    *          from HCITypes.h for the supported HCI Commands (i.e.
    *          of the format HCI_SUPPORTED_COMMAND_xxx_BIT_NUMBER, where
    *          "xxx" varies depending upon the command being tested).
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Command_Supported(unsigned int BluetoothStackID, unsigned int SupportedCommandBitNumber);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Command_Supported_t)(unsigned int BluetoothStackID, unsigned int SupportedCommandBitNumber);
#endif

   /*! @brief The following function is responsible for registering the
    * specified HCI Event Callback Function with the Bluetooth Protocol
    * Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID The first parameter to this function is the Bluetooth
    * Protocol Stack ID of the Bluetooth Protocol Stack that was
    * successfully opened.
    * @param HCI_EventCallback This parameter is a pointer to the Callback Function
    * and IS NOT optional.
    * @param CallbackParameter This parameter is a
    * parameter that the caller can specify which will be provided as
    * a function argument when the Callback Function is called.
    * @return This function returns a positive, non-zero value if the specified
    * HCI Event Callback was registered, or a negative value if the
    * callback was unable to be registered.  Upon success, this return
    * value can be used as an argument to HCI_UnRegisterCallback to
    * Remove the Registered Callback from the associated Bluetooth
    * Stack.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Register_Event_Callback(unsigned int BluetoothStackID, HCI_Event_Callback_t HCI_EventCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Register_Event_Callback_t)(unsigned int BluetoothStackID, HCI_Event_Callback_t HCI_EventCallback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is responsible for registering the
    * specified HCI ACL Data Callback Function with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID The first parameter to this function is the Bluetooth
    * Protocol Stack ID of the Bluetooth Protocol Stack that was
    * successfully opened.
    * @param HCI_ACLDataCallback This parameter is a pointer to the Callback Function
    * and IS NOT optional. @param CallbackParameter This parameter is a
    * parameter that the caller can specify which will be provided as
    * a function argument when the Callback Function is called.
    * @return This function returns a positive, non-zero value if the specified
    * HCI Event Callback was registered, or a negative value if the
    * callback was unable to be registered.  Upon success, this return
    * value can be used as an argument to HCI_UnRegisterCallback to
    * Remove the Registered Callback from the associated Bluetooth
    * Stack.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Register_ACL_Data_Callback(unsigned int BluetoothStackID, HCI_ACL_Data_Callback_t HCI_ACLDataCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Register_ACL_Data_Callback_t)(unsigned int BluetoothStackID, HCI_ACL_Data_Callback_t HCI_ACLDataCallback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is responsible for registering the
    * specified HCI SCO Data Callback Function with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID The first parameter to this function is the Bluetooth
    * Protocol Stack ID of the Bluetooth Protocol Stack that was
    * successfully opened.
    * @param HCI_SCODataCallback This parameter is a pointer to the Callback Function
    * and IS NOT optional. @param CallbackParameter This parameter is a
    * parameter that the caller can specify which will be provided as
    * a function argument when the Callback Function is called.
    * @return This function returns a positive, non-zero value if the specified
    * HCI Event Callback was registered, or a negative value if the
    * callback was unable to be registered.  Upon success, this return
    * value can be used as an argument to HCI_UnRegisterCallback to
    * Remove the Registered Callback from the associated Bluetooth
    * Stack.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Register_SCO_Data_Callback(unsigned int BluetoothStackID, HCI_SCO_Data_Callback_t HCI_SCODataCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Register_SCO_Data_Callback_t)(unsigned int BluetoothStackID, HCI_SCO_Data_Callback_t HCI_SCODataCallback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is responsible for removing a previously
    * installed HCI Callback (either HCI Event, HCI ACL Data, OR HCI
    * SCO Data) for the Bluetooth Protocol Stack specified by the
    * BluetoothStackID parameter.
    * @param BluetoothStackID The first parameter to this function is the Bluetooth
    * Protocol Stack ID of the Bluetooth Protocol Stack that was
    * successfully opened.
    * @param CallbackID The second parameter to this
    * function is the Callback ID and is obtained via a successful call
    * to either the HCI_Register_Event_Callback() function, the
    * HCI_Register_ACL_Data_Callback() function, OR the
    * HCI_Register_SCO_Data_Callback() function.  @return This function returns
    * zero if the specified callback was removed, or a negative return
    * value if the callback was unable to be removed.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Un_Register_Callback(unsigned int BluetoothStackID, unsigned int CallbackID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Un_Register_Callback_t)(unsigned int BluetoothStackID, unsigned int CallbackID);
#endif

   /*! @brief The following function is responsible for sending HCI ACL Packets
    * to the Bluetooth Device specifed by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID The first parameter to this
    * function is the Bluetooth Stack ID.
    * @param Connection_Handle This function accepts as it's second argument, a
    * Connection Handle to send the ACL Data Packet to.
    * @param Flags The HCI ACL Flags to specify with the Connection
    * Handle when building the ACL Packet.
    * @param ACLDataLength The fourth specifies the Length of the
    * ACL Data which specifies the number of Bytes in the Buffer.
    * @param ACLData The fifth parameter is the physical ACL Data to send.
    * The caller IS NOT responsible for submitting
    * an HCI ACL Data Packet.  This function will format an HCI ACL Data
    * Packet in the correct format and submit it to the Bluetooth Device
    * that is specified.
    * @return This function returns zero if the Packet was
    * successfully sent, otherwise this function returns a negative
    * error code.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Send_ACL_Data(unsigned int BluetoothStackID, Word_t Connection_Handle, Word_t Flags, Word_t ACLDataLength, Byte_t *ACLData);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Send_ACL_Data_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Word_t Flags, Word_t ACLDataLength, Byte_t *ACLData);
#endif

   /*! @brief The following function is responsible for sending HCI SCO Packets
    * to the Bluetooth Device specifed by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID The first parameter to this function is the Bluetooth
    * Protocol Stack ID of the Bluetooth Protocol Stack that was
    * successfully opened.
    * @param Connection_Handle The second parameter is the connection handle
    * send the SCO Data packet to.
    * @param Flags The third parameter is currently NOT used in the existing
    * Bluetooth Specification.
    * However, this function will pass off whatever Flag
    * values you pass here.
    * @param SCODataLength The number of
    * Bytes in the Buffer pointed to by SCOData to send.
    * @param SCOData The physical SCO data to send.
    * The caller IS
    * NOT responsible for submitting an HCI SCO Data Packet. This
    * function will format an HCI SCO Data Packet in the correct format
    * and submit it to the Bluetooth Device that is specified.
    * @return This function returns zero if the Packet was successfully sent,
    * otherwise this function returns a negative error code.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Send_SCO_Data(unsigned int BluetoothStackID, Word_t Connection_Handle, Word_t Flags, Byte_t SCODataLength, Byte_t *SCOData);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Send_SCO_Data_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Word_t Flags, Byte_t SCODataLength, Byte_t *SCOData);
#endif

   /*! HCI Command API's (Generic). */
   /*! @brief The following function issues a Raw HCI Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID The first parameter to this function is the Bluetooth
    * Protocol Stack ID of the Bluetooth Protocol Stack that was
    * successfully opened.
    * @param Command_OGF The Command_OGF parameter specifies the Bluetooth Command OGF
    * of the Command.
    * @param Command_OCF The Command_OCF specifies the Bluetooth Command OCF.
    * @param Command_Length The Command_Length parameter specifies the number
    * of bytes to send from the Buffer specified by the Command_Data
    * parameter.
    * @param Command_Data The Command_Data parameter is ignored if Command_Length is zero.
    * @param StatusResult The StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * @param LengthResult The LengthResult variable will contain
    * the Number of Bytes that are valid in the BufferResult variable.
    * @param BufferResult The current buffer result.
    * @param WaitForResponse The final parameter specifies whether or not this function
    * is to wait for a response from the Bluetooth Device. This
    * parameter is provided because the Bluetooth Device may not issue
    * any response for this HCI Command.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.
    * If you (the caller know that
    * the Bluetooth Device does NOT return data, then set this
    * parameter to FALSE.  If you want to be absolutely sure, you can
    * wait for the Timeout, however the calling therad will block until
    * an invalid response OR the Timeout occurs (valid data).
    * \note The LengthResult and BufferResult
    * parameters are ignored if the WaitForResponse Flag is FALSE.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Send_Raw_Command(unsigned int BluetoothStackID, Byte_t Command_OGF, Word_t Command_OCF, Byte_t Command_Length, Byte_t Command_Data[], Byte_t *StatusResult, Byte_t *LengthResult, Byte_t *BufferResult, Boolean_t WaitForResponse);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Send_Raw_Command_t)(unsigned int BluetoothStackID, Byte_t Command_OGF, Word_t Command_OCF, Byte_t Command_Length, Byte_t Command_Data[], Byte_t *StatusResult, Byte_t *LengthResult, Byte_t *BufferResult, Boolean_t WaitForResponse);
#endif

   /*! @brief The following function issues the appropriate call to an HCI
    * driver to get SCO data delivery via the HCI channel enabled or
    * disabled.
    * @param BluetoothStackID The first parameter to this function is the Bluetooth
    * Protocol Stack ID of the Bluetooth Protocol Stack that was
    * successfully opened.
    * @param SCOConfiguration SCO configuration data.
    * @return This function returns zero if successful, or a non-zero
    * value if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Change_SCO_Configuration(unsigned int BluetoothStackID, HCI_SCOConfiguration_t SCOConfiguration);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Change_SCO_Configuration_t) (unsigned int BluetoothStackID, HCI_SCOConfiguration_t SCOConfiguration);
#endif

   /*! @brief The following function issues the appropriate calls to an HCI
    * driver to set the current list of SCO connection handles.
    * @param BluetoothStackID The first parameter to this function is the Bluetooth
    * Protocol Stack ID of the Bluetooth Protocol Stack that was
    * successfully opened.
    * @param NumberHandles Number of SCO connection handles.
    * @param ConnectionHandleList Current list of SCO connection handles.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Change_SCO_Configuration_Handles(unsigned int BluetoothStackID, unsigned int NumberHandles, Word_t *ConnectionHandleList);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
    typedef int (BTPSAPI *PFN_HCI_Change_SCO_Configuration_Handles_t)(unsigned int BluetoothStackID, unsigned int NumberHandles, Word_t *ConnectionHandleList);
#endif

   /*! @brief The following function issues the appropriate call to an HCI
    * driver to perform a reconfiguration operation with the specified
    * reconfiguration information.
    * @param BluetoothStackID The first parameter to this function is the Bluetooth
    * Protocol Stack ID of the Bluetooth Protocol Stack that was
    * successfully opened.
    * @param ResetStateMachines Boolean variable dictating whether or not to reset the state machine.
    * @param DriverReconfigureData Pointer to tructure containing reconfiguration data
    * to be sent to the lower level HCI driver.
    * @return This function returns a zero or
    * positive value if successful, or a negative value if there was an
    * error.
    * \note This function should ONLY be used if the caller has
    *          explicit knowledge of the reconfiguration process being
    *          requested.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Reconfigure_Driver(unsigned int BluetoothStackID, Boolean_t ResetStateMachines, HCI_Driver_Reconfigure_Data_t *DriverReconfigureData);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Reconfigure_Driver_t)(unsigned int BluetoothStackID, Boolean_t ResetStateMachines, HCI_Driver_Reconfigure_Data_t *DriverReconfigureData);
#endif

   /*! @brief The following function is responsible for enabling and disabling
    * handling of Controller to Host Flow Control by this module.
    * @param BluetoothStackID The first parameter to this function is the Bluetooth
    * Protocol Stack ID of the Bluetooth Protocol Stack that was
    * successfully opened.
    * @param NumberOfACLPackets The number of ACL data packets that the controller
    * can send to the Host without the Host sending credits back to the controller.
    * @param NumberOfSCOPackets The number of SCO Data Packets that the controller
    * can send to the Host without the Host sending credits back to the controller.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.
    * \note If NumberOfACLPackets and NumberOfSCOPackets are both
    *          ZERO, then Controller to Host Flow Control will be
    *          disabled.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Set_Host_Flow_Control(unsigned int BluetoothStackID, Word_t NumberOfACLPackets, Word_t NumberOfSCOPackets);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Set_Host_Flow_Control_t)(unsigned int BluetoothStackID, Word_t NumberOfACLPackets, Word_t NumberOfSCOPackets);
#endif

   /*! @brief The following function is responsible for querying the Controller
    * to Host Flow Control Configuration of this Module.
    * @param BluetoothStackID The first parameter to this function is the Bluetooth
    * Protocol Stack ID of the Bluetooth Protocol Stack that was
    * successfully opened.
    * @param NumberOfACLPackets The number of ACL data packets that the controller
    * can send to the Host without the Host sending credits back to the controller.
    * @param NumberOfSCOPackets The number of SCO Data Packets that the controller
    * can send to the Host without the Host sending credits back to the controller.
    * @return This function returns zero if successful, or a non-zero value if
    * there was an error.
    * \note If NumberOfACLPackets and NumberOfSCOPackets are both set
    *          to ZERO on a successfull call to this function, then
    *          Controller to Host Flow Control will be disabled.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Query_Host_Flow_Control(unsigned int BluetoothStackID, Word_t *NumberOfACLPackets, Word_t *NumberOfSCOPackets);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Query_Host_Flow_Control_t)(unsigned int BluetoothStackID, Word_t *NumberOfACLPackets, Word_t *NumberOfSCOPackets);
#endif

   /* HCI Command API's (Link Control).                                 */

   /*! @brief The following function issues the HCI_Inquiry Command to the
    * Bluetooth Device that is associated with the Bluetooth Protocol
    * Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID The first parameter to this function is the Bluetooth
    * Protocol Stack ID of the Bluetooth Protocol Stack that was
    * successfully opened.
    * @param LAP Lower Address Part structure type.
    * @param Inquiry_Length Length of Inquiry in seconds.
    * @param Num_Responses Number of inquiry responses to be returned.
    * @param StatusResult The Status Result returned from
    * the Bluetooth Device.
    * @return This function returns zero if successful,
    *  or a non-zero value if there was an
    * error.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Inquiry(unsigned int BluetoothStackID, LAP_t LAP, Byte_t Inquiry_Length, Byte_t Num_Responses, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Inquiry_t)(unsigned int BluetoothStackID, LAP_t LAP, Byte_t Inquiry_Length, Byte_t Num_Responses, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Inquiry_Cancel Command to
    * the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error. If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Inquiry_Cancel(unsigned int BluetoothStackID, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Inquiry_Cancel_t)(unsigned int BluetoothStackID, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Periodic_Inquiry_Mode
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param Max_Period_Length Upper bound on random interval between inquiries.
    * Values are in increments of .28 seconds, with a range of 3.84 sec. (0x03)
    * to ~23.3 hrs. (0xFFFE)
    * @param Min_Period_Length Lower bound on random interval between inquiries.
    * Values are in increments of .28 seconds, with a range of 2.56 sec. (0x02)
    * to ~23.3 hrs. (0xFFFE)
    * @param LAP Lower address part of the Bluetooth device address.
    * Range: 0x9E8B00–0x9E8B3F
    * @param Inquiry_Length Amount of time before each inquiry is halted.
    * Values are in increments of .28 seconds, with a range of .28 sec. (0x0)
    * to 6.44 sec. (0x30).
    * @param Num_Responses Maximum number of Bluetooth devices to find before
    * each inquiry is halted.  A value of zero (0) means unlimited.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.
    * If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Periodic_Inquiry_Mode(unsigned int BluetoothStackID, Word_t Max_Period_Length, Word_t Min_Period_Length, LAP_t LAP, Byte_t Inquiry_Length, Byte_t Num_Responses, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Periodic_Inquiry_Mode_t)(unsigned int BluetoothStackID, Word_t Max_Period_Length, Word_t Min_Period_Length, LAP_t LAP, Byte_t Inquiry_Length, Byte_t Num_Responses, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Exit_Periodic_Inquiry_Mode
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Exit_Periodic_Inquiry_Mode(unsigned int BluetoothStackID, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Exit_Periodic_Inquiry_Mode_t)(unsigned int BluetoothStackID, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Create_Connection Command to
    * the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param BD_ADDR Bluetooth device address to connect to.
    * @param Packet_Type Which packet types the Link Manager shall use for the ACL link.
    * This can be an ORing of multiple packet types.
    * @param Page_Scan_Repetition_Mode Part of the supported Page Scan Modes that the device
    * being connected to supports.  This information is discovered during the Inquiry mode.
    * @param Page_Scan_Mode The other part of the supported Page Scan Modes that the device
    * being connected to supports.  This information is discovered during the Inquiry mode
    * @param Clock_Offset Bits 6 to 2 of the difference between the master and slave device
    * clocks, mapped to bits 4 to 0 of this parameter (i.e., computed from
    * ((clock_slave – clock_master) ShiftRight 2).  Bit 5 (MSB) is the
    * Clock_Offset_Valid flag which is  if the offset value is valid.
    * @param Allow_Role_Switch Whether the local device will accept a role switch and become
    * a slave device or not.  The currently defined values are:
    * HCI_ROLE_SWITCH_LOCAL_MASTER_NO_ROLE_SWITCH
    * HCI_ROLE_SWITCH_LOCAL_MASTER_ACCEPT_ROLE_SWITCH
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Create_Connection(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t Packet_Type, Byte_t Page_Scan_Repetition_Mode, Byte_t Page_Scan_Mode, Word_t Clock_Offset, Byte_t Allow_Role_Switch, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Create_Connection_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t Packet_Type, Byte_t Page_Scan_Repetition_Mode, Byte_t Page_Scan_Mode, Word_t Clock_Offset, Byte_t Allow_Role_Switch, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Disconnect Command to the
    * Bluetooth Device that is associated with the Bluetooth Protocol
    * Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Connection_Handle Unique identifier for the connection returned
    * in the Connection Complete event associated with the HCI_Create_Connection
    * command.
    * @param Reason The reason for ending the connection.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful,
    * or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Disconnect(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t Reason, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Disconnect_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t Reason, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Add_SCO_Connection Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Connection_Handle Handle for the ACL connection from which to
    * base the SCO link to the same remote device.
    * @param Packet_Type Which packet types the Link Manager shall use for
    * the SCO connection.  This can be an ORing of multiple packet types.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero
    * value if there was an error.
    * If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Add_SCO_Connection(unsigned int BluetoothStackID, Word_t Connection_Handle, Word_t Packet_Type, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Add_SCO_Connection_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Word_t Packet_Type, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Accept_Connection_Request
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param BD_ADDR Bluetooth device address for the device to connect to.
    * @param Role Designate the master-slave role to take on in this connection.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Accept_Connection_Request(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t Role, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Accept_Connection_Request_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t Role, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Reject_Connection_Request
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param BD_ADDR Bluetooth device address for the device to connect to.
    * @param Reason The reason for the refusal.
    * @param StatusResult Returned HCI status code
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error. If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Reject_Connection_Request(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t Reason, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Reject_Connection_Request_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t Reason, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Link_Key_Request_Reply
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param BD_ADDR Bluetooth device that the link key is for.
    * @param Link_Key 6-Byte Link Key to use to make the connection.
    * @param StatusResult Returned HCI status code.
    * @param BD_ADDRResult Pointer for return value of Bluetooth device
    * for which the link key request reply was completed.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * BD_ADDRResult variable will contain the BD_ADDR Result returned
    * from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Link_Key_Request_Reply(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Link_Key_t Link_Key, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Link_Key_Request_Reply_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Link_Key_t Link_Key, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);
#endif

   /*! @brief The following function issues the
    * HCI_Link_Key_Request_Negative_Reply Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param BD_ADDR Bluetooth device that the link key is for.
    * @param StatusResult Returned HCI status code.
    * @param BD_ADDRResult Pointer for return value of Bluetooth
    * device for which the link key request negative reply was completed.
    * @return This function returns zero if successful, or a non-zero value
    * if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned
    * from the Bluetooth Device AND the BD_ADDRResult variable will
    * contain the BD_ADDR Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Link_Key_Request_Negative_Reply(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Link_Key_Request_Negative_Reply_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);
#endif

   /*! @brief The following function issues the HCI_PIN_Code_Request_Reply
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param BD_ADDR Bluetooth device which the PIN Code is for.
    * @param PIN_Code_Length The length in bytes of the PIN Code in the
    * range of 0x0 to 0x0.
    * @param PIN_Code The PIN Code for the device being connected, with
    * the MSB in byte zero.
    * @param StatusResult Returned HCI status code.
    * @param BD_ADDRResult Pointer for return value of Bluetooth device
    * for which the PIN Code request reply was completed.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * BD_ADDRResult variable will contain the BD_ADDR Result returned
    * from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_PIN_Code_Request_Reply(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t PIN_Code_Length, PIN_Code_t PIN_Code, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_PIN_Code_Request_Reply_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t PIN_Code_Length, PIN_Code_t PIN_Code, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);
#endif

   /*! @brief The following function issues the
    * HCI_PIN_Code_Request_Negative_Reply Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param BD_ADDR Bluetooth device which the PIN Code is for.
    * @param StatusResult Returned HCI status code.
    * @param BD_ADDRResult Pointer for return value of Bluetooth
    * device for which the PIN Code request negative reply was completed.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned
    * from the Bluetooth Device AND the BD_ADDRResult variable will
    * contain the BD_ADDR Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_PIN_Code_Request_Negative_Reply(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_PIN_Code_Request_Negative_Reply_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);
#endif

   /*! @brief The following function issues the
    * HCI_Change_Connection_Packet_Type Command to the Bluetooth Device
    * that is associated with the Bluetooth Protocol Stack specified by
    * the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Connection_Handle Handle for the connection returned in the
    * Connection Complete event associated with the HCI_Create_Connection command.
    * @param Packet_Type Which packet types the Link Manager shall use for
    * the ACL link.  This can be an ORing of multiple packet types.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if
    * successful, or a non-zero value if there was an error.
    * If this function returns zero (success) then the StatusResult variable
    * will contain the Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Change_Connection_Packet_Type(unsigned int BluetoothStackID, Word_t Connection_Handle, Word_t Packet_Type, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Change_Connection_Packet_Type_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Word_t Packet_Type, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Authentication_Requested
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Connection_Handle Unique identifier for the connection returned
    * in the Connection Complete event associated with the HCI_Create_Connection command.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error. If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Authentication_Requested(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Authentication_Requested_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Set_Connection_Encryption
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Connection_Handle Unique identifier for the connection returned
    * in the Connection Complete event associated with the HCI_Create_Connection
    * command.
    * @param Encryption_Enable Flag indicating whether the encryption should
    * be turned on or off.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error. If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Set_Connection_Encryption(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t Encryption_Enable, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Set_Connection_Encryption_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t Encryption_Enable, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Change_Connection_Link_Key
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Connection_Handle Unique identifier for the connection
    * returned in the Connection Complete event associated with
    * the HCI_Create_Connection command.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Change_Connection_Link_Key(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Change_Connection_Link_Key_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Master_Link_Key Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param Key_Flag Indicator of which link key to change to.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error. If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Master_Link_Key(unsigned int BluetoothStackID, Byte_t Key_Flag, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Master_Link_Key_t)(unsigned int BluetoothStackID, Byte_t Key_Flag, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Remote_Name_Request Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param BD_ADDR Address of the remote Bluetooth device.
    * @param Page_Scan_Repetition_Mode Part of the supported Page Scan Modes
    * that the device being connected to supports.  This information is discovered
    * during the Inquiry mode.
    * @param Page_Scan_Mode The other part of the supported Page Scan Modes
    * that the device being connected to supports.  This information is
    * discovered during the Inquiry mode.
    * @param Clock_Offset Bits 6 to 2 of the difference between the master
    * and slave device clocks, mapped to bits 4 to 0 of this parameter
    * (i.e., computed from ((clock_slave – clock_master) ShiftRight 2).
    * Bit 5 (MSB) is the Clock_Offset_Valid flag which is  if the
    * offset value is valid.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Remote_Name_Request(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t Page_Scan_Repetition_Mode, Byte_t Page_Scan_Mode, Word_t Clock_Offset, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Remote_Name_Request_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t Page_Scan_Repetition_Mode, Byte_t Page_Scan_Mode, Word_t Clock_Offset, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the
    * HCI_Read_Remote_Supported_Features Command to the Bluetooth Device
    * that is associated with the Bluetooth Protocol Stack specified by
    * the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Connection_Handle Handle for the ACL connection.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if
    * successful, or a non-zero value if there was an error.
    * If this function returns zero (success) then the StatusResult variable
    * will contain the Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Remote_Supported_Features(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Remote_Supported_Features_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the
    * HCI_Read_Remote_Version_Information Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Connection_Handle Unique identifier for the connection returned
    * in the Connection Complete event associated with the HCI_Create_Connection
    * command.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Remote_Version_Information(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Remote_Version_Information_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Read_Clock_Offset Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Connection_Handle Handle for the ACL connection to the remote device.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Clock_Offset(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Clock_Offset_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult);
#endif

   /* HCI Command API's (Link Control - Version 1.2).
    * @brief The following function issues the HCI_Create_Connection_Cancel
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param BD_ADDR Address of the remote Bluetooth device.
    * @param StatusResult Returned HCI status code.
    * @param BD_ADDRResult Pointer for return value of Bluetooth device for
    * which the create connection cancel reply was completed.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error. If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * BD_ADDRResult variable will contain the BD_ADDR Result returned
    * from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Create_Connection_Cancel(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Create_Connection_Cancel_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);
#endif

   /*! @brief The following function issues the HCI_Remote_Name_Request_Cancel
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param BD_ADDR Address of the remote Bluetooth device.
    * @param StatusResult Returned HCI status code.
    * @param BD_ADDRResult Pointer for return value of Bluetooth device
    * for which the create connection cancel reply was completed.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error. If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * BD_ADDRResult variable will contain the BD_ADDR Result returned
    * from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Remote_Name_Request_Cancel(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Remote_Name_Request_Cancel_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);
#endif

   /*! @brief The following function issues the
    * HCI_Read_Remote_Extended_Features Command to the Bluetooth Device
    * that is associated with the Bluetooth Protocol Stack specified by
    * the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Connection_Handle Handle for the ACL connection to the remote
    * device.
    * @param Page_Number The Page Number of the Extended Features Mask that
    * is to be returned.  Passing zero for this parameter returns the normal
    * LMP features mask.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if
    * successful, or a non-zero value if there was an error. If this function returns zero (success) then the StatusResult variable
    * will contain the Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Remote_Extended_Features(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t Page_Number, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Remote_Extended_Features_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t Page_Number, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Read_LMP_Handle Command to
    * the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Connection_Handle Handle for the ACL connection to the remote device.
    * @param StatusResult Returned HCI status code.
    * @param Connection_HandleResult Unique identifier for the connection
    * handle for which the read LMPhandle was done.
    * @param LMP_HandleResult LMP handle from the remote device.
    * @param ReservedResult Reserved result from the remote device.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error. If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the Connection_HandleResult variable will
    * contain the Connection Handle Result returned from the Bluetooth
    * Device AND the LMP_HandleResult variable will contain the LMP
    * Handle Result returned from the Device AND the ReservedResult
    * variable will contain the Reserved Result returned from the
    * Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_LMP_Handle(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult, Byte_t *LMP_HandleResult, DWord_t *ReservedResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_LMP_Handle_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult, Byte_t *LMP_HandleResult, DWord_t *ReservedResult);
#endif

   /*! @brief The following function issues the HCI_Setup_Sychronous_Connection
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param Connection_Handle Handle for the ACL connection to the remote device.
    * @param Transmit_Bandwidth Amount of bandwidth available for transmit.
    * @param Receive_Bandwidth Amount of bandwidth available for receive.
    * @param Max_Latency Upper limit of the time (in milliseconds) between the eSCO
    * (or SCO) instants, plus the size of the retransmission window, plus the length
    * of the reserved synchronous slots for this logical transport.
    * @param Voice_Setting Indicates if this connection is for voice or transparent data.
    * @param Retransmission_Effort The extra resources that are allocated to this
    * connection if a packet needs to be retransmitted.  The Retransmission_Effort
    * parameter shall be set to indicate the required behaviour, or to don't care.
    * @param Packet_Type A bitmask specifying which packet types the LM shall
    * accept in the negotiation of the link parameters.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error. If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Setup_Synchronous_Connection(unsigned int BluetoothStackID, Word_t Connection_Handle, DWord_t Transmit_Bandwidth, DWord_t Receive_Bandwidth, Word_t Max_Latency, Word_t Voice_Setting, Byte_t Retransmission_Effort, Word_t Packet_Type, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Setup_Synchronous_Connection_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, DWord_t Transmit_Bandwidth, DWord_t Receive_Bandwidth, Word_t Max_Latency, Word_t Voice_Setting, Byte_t Retransmission_Effort, Word_t Packet_Type, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the
    * HCI_Accept_Sychronous_Connection_Request Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param BD_ADDR Address of the remote Bluetooth device.
    * @param Transmit_Bandwidth Amount of bandwidth available for transmit.
    * @param Receive_Bandwidth Amount of bandwidth available for receive.
    * @param Max_Latency Upper limit of the time (in milliseconds) between the eSCO
    * (or SCO) instants, plus the size of the retransmission window, plus the length
    * of the reserved synchronous slots for this logical transport.
    * @param Content_Format Indicates if this connection is for voice or transparent data.
    * @param Retransmission_Effort Specifies the extra resources that are allocated
    * to this connection if a packet may need to be retransmitted.  The Retransmission
    * _Effort parameter shall be set to indicate the required behaviour, or to don't care.
    * @param Packet_Type A bitmask specifying which packet types the LM shall accept
    * in the negotiation of the link parameters.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Accept_Synchronous_Connection_Request(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, DWord_t Transmit_Bandwidth, DWord_t Receive_Bandwidth, Word_t Max_Latency, Word_t Content_Format, Byte_t Retransmission_Effort, Word_t Packet_Type, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Accept_Synchronous_Connection_Request_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, DWord_t Transmit_Bandwidth, DWord_t Receive_Bandwidth, Word_t Max_Latency, Word_t Content_Format, Byte_t Retransmission_Effort, Word_t Packet_Type, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the
    * HCI_Reject_Sychronous_Connection_Request Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param BD_ADDR Address of the remote Bluetooth device.
    * @param Reason Host reject error code returned to the initiating host in the
    * Status parameter of the Synchronous connection complete event on the remote side.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Reject_Synchronous_Connection_Request(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t Reason, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Reject_Synchronous_Connection_Request_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t Reason, Byte_t *StatusResult);
#endif

   /* HCI Command API's (Link Control - Version 2.1 + EDR).
    * @brief The following function issues the HCI_IO_Capability_Request_Reply
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param BD_ADDR Address of the remote Bluetooth device.
    * @param IO_Capability IO Capabilities of the local device.
    * @param OOB_Data_Present Specifies whether or not OOB Data for the remote
    * Bluetooth device is present (zero signifies not present).
    * @param Authentication_Requirements  Authentication Requirements of the
    * local device.
    * @param StatusResult If function returns zero (success) this variable will
    * contain the Status Result returned from the Bluetooth device
    * @param BD_ADDRResult If function returns zero (success) this variable will
    * contain the BD_ADDR Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * BD_ADDRResult variable will contain the BD_ADDR Result returned
    * from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_IO_Capability_Request_Reply(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t IO_Capability, Byte_t OOB_Data_Present, Byte_t Authentication_Requirements, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_IO_Capability_Request_Reply_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t IO_Capability, Byte_t OOB_Data_Present, Byte_t Authentication_Requirements, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);
#endif

   /*! @brief The following function issues the
    * HCI_User_Confirmation_Request_Reply Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param BD_ADDR Address of the remote Bluetooth device.
    * @param StatusResult If function returns zero (success) this variable will
    * contain the Status Result returned from the Bluetooth device
    * @param BD_ADDRResult If function returns zero (success) this variable will
    * contain the BD_ADDR Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the BD_ADDRResult variable will contain
    * the BD_ADDR Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_User_Confirmation_Request_Reply(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_User_Confirmation_Request_Reply_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);
#endif

   /*! @brief The following function issues the
    * HCI_User_Confirmation_Request_Negative_Reply Command to the
    * Bluetooth Device that is associated with the Bluetooth Protocol
    * Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param BD_ADDR Address of the remote Bluetooth device.
    * @param StatusResult If function returns zero (success) this variable will
    * contain the Status Result returned from the Bluetooth device
    * @param BD_ADDRResult If function returns zero (success) this variable will
    * contain the BD_ADDR Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the BD_ADDRResult variable will contain
    * the BD_ADDR Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_User_Confirmation_Request_Negative_Reply(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_User_Confirmation_Request_Negative_Reply_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);
#endif

   /*! @brief The following function issues the HCI_User_Passkey_Request_Reply
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param BD_ADDR Address of the remote Bluetooth device.
    * @param Numeric_Value Actual passkey value.  This value must be
    * between 0 and 999999.
    * @param StatusResult If function returns zero (success) this
    * variable will contain the Status Result returned from the Bluetooth device
    * @param BD_ADDRResult If function returns zero (success) this
    * variable will contain the BD_ADDR Result returned from the Bluetooth device.
    *  @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * BD_ADDRResult variable will contain the BD_ADDR Result returned
    * from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_User_Passkey_Request_Reply(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, DWord_t Numeric_Value, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_User_Passkey_Request_Reply_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, DWord_t Numeric_Value, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);
#endif

   /*! @brief The following function issues the
    * HCI_User_Passkey_Request_Negative_Reply Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param BD_ADDR Address of the remote Bluetooth device.
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device
    * @param BD_ADDRResult If function returns zero (success) this variable
    * will contain the BD_ADDR Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the BD_ADDRResult variable will contain
    * the BD_ADDR Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_User_Passkey_Request_Negative_Reply(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_User_Passkey_Request_Negative_Reply_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);
#endif

   /*! @brief The following function issues the
    * HCI_Remote_OOB_Data_Request_Reply Command to the Bluetooth Device
    * that is associated with the Bluetooth Protocol Stack specified by
    * the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param BD_ADDR Address of the remote Bluetooth device.
    * @param Simple_Pairing_Hash Simple pairing of the OOB data that was received
    * for the remote device (C).
    * @param Simple_Pairing_Randomizer Simple pairing randomizer of the OOB data
    * that was received for the remote device (R)
    * @param StatusResult If function returns zero (success) this variable will
    * contain the Status Result returned from the Bluetooth device
    * @param BD_ADDRResult If function returns zero (success) this variable will
    * contain the BD_ADDR Result returned from the Bluetooth device.
    * @return This function returns zero if
    * successful, or a non-zero value if there was an error.  If this function returns zero (success) then the StatusResult variable
    * will contain the Status Result returned from the Bluetooth Device
    * AND the BD_ADDRResult variable will contain the BD_ADDR Result
    * returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Remote_OOB_Data_Request_Reply(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Simple_Pairing_Hash_t Simple_Pairing_Hash, Simple_Pairing_Randomizer_t Simple_Pairing_Randomizer, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Remote_OOB_Data_Request_Reply_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Simple_Pairing_Hash_t Simple_Pairing_Hash, Simple_Pairing_Randomizer_t Simple_Pairing_Randomizer, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);
#endif

   /*! @brief The following function issues the
    * HCI_Remote_OOB_Data_Request_Negative_Reply Command to the
    * Bluetooth Device that is associated with the Bluetooth Protocol
    * Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param BD_ADDR Address of the remote Bluetooth device.
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device
    * @param BD_ADDRResult If function returns zero (success) this variable
    * will contain the BD_ADDR Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the BD_ADDRResult variable will contain
    * the BD_ADDR Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Remote_OOB_Data_Request_Negative_Reply(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Remote_OOB_Data_Request_Negative_Reply_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);
#endif

   /*! @brief The following function issues the
    * HCI_IO_Capability_Request_Negative_Reply Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param BD_ADDR Address of the remote Bluetooth device.
    * @param Reason Reason code for the IO Capability rejection.  Possible values
    * are the HCI Status Codes.
    * @param StatusResult If function returns zero (success) this variable will
    * contain the Status Result returned from the Bluetooth device
    * @param BD_ADDRResult If function returns zero (success) this variable will
    * contain the BD_ADDR Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the BD_ADDRResult variable will contain
    * the BD_ADDR Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_IO_Capability_Request_Negative_Reply(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t Reason, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_IO_Capability_Request_Negative_Reply_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t Reason, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);
#endif

   /* HCI Command API's (Link Control - Version 3.0 + HS).
    * @brief The following function issues the HCI_Create_Physical_Link Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param Physical_Link_Handle Physical Link Handle indentifing the physical link
    * to be created.
    * @param Dedicated_AMP_Key_Length The number of valid octets (bytes) in the
    * Dedicated_AMP_Key parameter.
    * @param Dedicated_AMP_Key_Type Indicates the type of key that the parameter
    * Dedicated_AMP_Key[] is.
    * @param Dedicated_AMP_Key[] Byte array with Dedicated_AMP_Key_Length valid
    * bytes that will be used to generate a session key in order to encrypt all
    * data on the physical link specified by Physical_Link_Handle.
    * @param StatusResult If this function returns zero (success) then variable
    * pointed to by StatusResult will contain the status result returned from
    * the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Create_Physical_Link(unsigned int BluetoothStackID, Byte_t Physical_Link_Handle, Byte_t Dedicated_AMP_Key_Length, Byte_t Dedicated_AMP_Key_Type, Byte_t Dedicated_AMP_Key[], Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Create_Physical_Link_t)(unsigned int BluetoothStackID, Byte_t Physical_Link_Handle, Byte_t Dedicated_AMP_Key_Length, Byte_t Dedicated_AMP_Key_Type, Byte_t Dedicated_AMP_Key[], Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Accept_Physical_Link_Request
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Physical_Link_Handle The second parameter is the physical link handle.
    * @param Dedicated_AMP_Key_Length The number of valid octets (bytes) in
    * the Dedicated_AMP_Key parameter.
    * @param Dedicated_AMP_Key_Type  Indicates the type of key that the
    * parameter Dedicated_AMP_Key[] is.
    * @param Dedicated_AMP_Key[] Byte array with Dedicated_AMP_Key_Length
    * valid bytes that will be used to generate a session key in order
    * to encrypt all data on the physical link specified by Physical_Link_Handle.
    * @param StatusResult If this function returns zero (success) then
    * variable pointed to by StatusResult will contain the status
    * result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Accept_Physical_Link_Request(unsigned int BluetoothStackID, Byte_t Physical_Link_Handle, Byte_t Dedicated_AMP_Key_Length, Byte_t Dedicated_AMP_Key_Type, Byte_t Dedicated_AMP_Key[], Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Accept_Physical_Link_Request_t)(unsigned int BluetoothStackID, Byte_t Physical_Link_Handle, Byte_t Dedicated_AMP_Key_Length, Byte_t Dedicated_AMP_Key_Type, Byte_t Dedicated_AMP_Key[], Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Disconnect_Physical_Link
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param Physical_Link_Handle Physical Link Handle identifying the physical
    * link which has been created.
    * @param Reason Byte value indicating the reason that the specified physical
    * link is being disconnected. The remote controller will receive this parameter
    * in the etDisconnection_Physical_Link_Complete_Event event.
    * @param StatusResult If this function returns zero (success) then variable
    * pointed to by StatusResult will contain the status result returned from the
    * Bluetooth device.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Disconnect_Physical_Link(unsigned int BluetoothStackID, Byte_t Physical_Link_Handle, Byte_t Reason, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Disconnect_Physical_Link_t)(unsigned int BluetoothStackID, Byte_t Physical_Link_Handle, Byte_t Reason, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Create_Logical_Link Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param Physical_Link_Handle Handle of the physical link over which the
    * logical link will be created.
    * @param Tx_Flow_Spec Extended flow specification value that defines the
    * transmitted traffic.
    * @param Rx_Flow_Spec Extended flow specification value that defines the
    * received traffic.
    * @param StatusResult If this function returns zero (success) then variable
    * pointed to by StatusResult will contain the status result returned from
    * the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Create_Logical_Link(unsigned int BluetoothStackID, Byte_t Physical_Link_Handle, HCI_Extended_Flow_Spec_Data_t *Tx_Flow_Spec, HCI_Extended_Flow_Spec_Data_t *Rx_Flow_Spec, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Create_Logical_Link_t)(unsigned int BluetoothStackID, Byte_t Physical_Link_Handle, HCI_Extended_Flow_Spec_Data_t *Tx_Flow_Spec, HCI_Extended_Flow_Spec_Data_t *Rx_Flow_Spec, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Accept_Logical_Link Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Physical_Link_Handle Handle of the physical link over which
    * the logical link will be created.
    * @param Tx_Flow_Spec Extended flow specification value that defines
    * the transmitted traffic.
    * @param Rx_Flow_Spec Extended flow specification value that defines
    * the received traffic.
    * @param StatusResult If this function returns zero (success) then
    * variable pointed to by StatusResult will contain the status result
    * returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Accept_Logical_Link(unsigned int BluetoothStackID, Byte_t Physical_Link_Handle, HCI_Extended_Flow_Spec_Data_t *Tx_Flow_Spec, HCI_Extended_Flow_Spec_Data_t *Rx_Flow_Spec, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Accept_Logical_Link_t)(unsigned int BluetoothStackID, Byte_t Physical_Link_Handle, HCI_Extended_Flow_Spec_Data_t *Tx_Flow_Spec, HCI_Extended_Flow_Spec_Data_t *Rx_Flow_Spec, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Disconnect_Logical_Link
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Logical_Link_Handle Handle of the logical link that is to be
    * disconnected.
    * @param StatusResult If this function returns zero (success) then
    * variable pointed to by StatusResult will contain the status result
    * returned from the Bluetooth device.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Disconnect_Logical_Link(unsigned int BluetoothStackID, Word_t Logical_Link_Handle, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Disconnect_Logical_Link_t)(unsigned int BluetoothStackID, Word_t Logical_Link_Handle, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Logical_Link_Cancel Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Physical_Link_Handle Physical link handle for the physical link
    * over which the logical link was being established.
    * @param Tx_Flow_Spec_ID Flow Spec ID identifying th logical link whose
    * creation is being cancelled.
    * @param StatusResult If this function returns zero (success) then variable
    * pointed to by StatusResult will contain the status result returned from the
    * Bluetooth device.
    * @param Physical_Link_HandleResult  If this function returns zero (success)
    * then the variable pointed to by Physical_Link_HandleResult will contain the
    *  Physical Link Handle returned from the Bluetooth device.
    * @param Tx_Flow_Spec_IDResult If this function returns zero (success) then
    * the variable pointed to by Tx_Flow_Spec_IDResult will contain the Tx
    * Flow Spec ID returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the Physical_Link_HandleResult will
    * contain the Physical Link Handle returned from the Bluetooth
    * Device AND the Tx_Flow_Spec_IDResult will return the Tx Flow Spec
    * ID returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Logical_Link_Cancel(unsigned int BluetoothStackID, Byte_t Physical_Link_Handle, Byte_t Tx_Flow_Spec_ID, Byte_t *StatusResult, Byte_t *Physical_Link_HandleResult, Byte_t *Tx_Flow_Spec_IDResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Logical_Link_Cancel_t)(unsigned int BluetoothStackID, Byte_t Physical_Link_Handle, Byte_t Tx_Flow_Spec_ID, Byte_t *StatusResult, Byte_t *Physical_Link_HandleResult, Byte_t *Flow_Spec_IDResult);
#endif

   /*! @brief The following function issues the HCI_Flow_Spec_Modify Command to
    * the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param Handle  Logical Handle of the logical connection whose Flow Spec
    * will be modified.
    * @param Tx_Flow_Spec Extended flow specification value that defines the
    * transmitted traffic.
    * @param Rx_Flow_Spec Extended flow specification value that defines the
    * received traffic.
    * @param StatusResult If this function returns zero (success) then variable
    * pointed to by StatusResult will contain the status result returned from the
    * Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Flow_Spec_Modify(unsigned int BluetoothStackID, Word_t Handle, HCI_Extended_Flow_Spec_Data_t *Tx_Flow_Spec, HCI_Extended_Flow_Spec_Data_t *Rx_Flow_Spec, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Flow_Spec_Modify_t)(unsigned int BluetoothStackID, Word_t Handle, HCI_Extended_Flow_Spec_Data_t *Tx_Flow_Spec, HCI_Extended_Flow_Spec_Data_t *Rx_Flow_Spec, Byte_t *StatusResult);
#endif

   /*! HCI Command API's (Link Control - Version 4.0 + CSA2).
    * @brief The following function issues the
    * HCI_Enhanced_Setup_Synchronous_Connection Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param Connection_Handle Unique identifier for the connection returned in the
    * Connection Complete event associated with the HCI_Create_Connection command.
    * @param SynchronousConnectionInfo Pointer to a structure containing the Synchronous connection info.
    * @param StatusResult If this function returns zero (success) then variable
    * pointed to by StatusResult will contain the status result returned from the
    * Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Enhanced_Setup_Synchronous_Connection(unsigned int BluetoothStackID, Word_t Connection_Handle, HCI_Enhanced_Synchronous_Connection_Info_t *SynchronousConnectionInfo, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Enhanced_Setup_Synchronous_Connection_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, HCI_Enhanced_Synchronous_Connection_Info_t *SynchronousConnectionInfo, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the
    * HCI_Enhanced_Setup_Synchronous_Connection Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack
    * via a call to BSC_Initialize.
    * @param BD_ADDR The address of the bluetooth device.
    * @param SynchronousConnectionInfo Pointer to a structure containing the Synchronous connection info.
    * @param StatusResult If this function returns zero (success) then variable
    * pointed to by StatusResult will contain the status result returned from the
    * Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Enhanced_Accept_Setup_Synchronous_Connection_Request(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, HCI_Enhanced_Synchronous_Connection_Info_t *SynchronousConnectionInfo, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Enhanced_Accept_Setup_Synchronous_Connection_Request_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, HCI_Enhanced_Synchronous_Connection_Info_t *SynchronousConnectionInfo, Byte_t *StatusResult);
#endif

   /*! HCI Command API's (Link Control - Version 4.0 + CSA4). */


   /* @brief The following function issues the HCI_Truncated_Page Command to
    * the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR BD_ADDR of remote device from which the C and R values were received.
    * @param Page_Scan_Repetition_Mode Specifies the page scan repetition mode supported by the remote device with the BD_ADDR.
	*	HCI_PAGE_SCAN_REPETITION_MODE_R0
	*	HCI_PAGE_SCAN_REPETITION_MODE_R1
	*	HCI_PAGE_SCAN_REPETITION_MODE_R2
    * @param Clock_Offset Difference between its own clock and the clock of the remote device with BD_ADDR.
    * @param StatusResult If function returns zero (success) this variable will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error. If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Truncated_Page(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t Page_Scan_Repetition_Mode, Word_t Clock_Offset, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Truncated_Page_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t Page_Scan_Repetition_Mode, Word_t Clock_Offset, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Truncated_Page_Cancel
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR BD_ADDR of remote device from which the C and R values were received.
    * @param StatusResult If function returns zero (success) this variable will contain the Status Result returned from the Bluetooth device.
    * @param BD_ADDRResult Will contain the BD_ADDR returned from the Bluetooth Device.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * BD_ADDRResult variable will contain the BD_ADDR returned from the
    * Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Truncated_Page_Cancel(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Truncated_Page_Cancel_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);
#endif

   /*! @brief The following function issues the
    * HCI_Set_Connectionless_Slave_Broadcast Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param Enable
    * @param LT_ADDR Allows the Host to request that theBR/EDR Controller reserve a specific LT_ADDR for Connectionless Slave Broadcast.
    * @param LPO_Allowed Informs the BR/EDR Controller whether it is allowed to sleep.
    * @param Packet_Type Packets identified by Packet_Type that are currently pending for transmission in the Controller for the specified Handle.
    * @param Interval_Min Contains the minimum connection’s interval.
    * @param Interval_Max Contains the maximum connection’s interval.
    * @param CSB_Supervision_Timeout Duration in slots to continue listening for Connectionless Slave Broadcast packets after the last successfully received Connectionless Slave Broadcast packet.
    * @param StatusResult Thr status result returned from the Bluetooth Device.
    * @param LT_ADDRResult The LT_ADDR returned from the Bluetooth Device
    * @param IntervalResult The Interval returned from the Bluetooth Device.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.
    * If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the LT_ADDRResult variable will contain
    * the LT_ADDR returned from the Bluetooth Device AND the
    * IntervalResult variable will contain the Interval returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Set_Connectionless_Slave_Broadcast(unsigned int BluetoothStackID, Byte_t Enable, Byte_t LT_ADDR, Byte_t LPO_Allowed, Word_t Packet_Type, Word_t Interval_Min, Word_t Interval_Max, Word_t CSB_Supervision_Timeout, Byte_t *StatusResult, Byte_t *LT_ADDRResult, Word_t *IntervalResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Set_Connectionless_Slave_Broadcast_t)(unsigned int BluetoothStackID, Byte_t Enable, Byte_t LT_ADDR, Byte_t LPO_Allowed, Word_t Packet_Type, Word_t Interval_Min, Word_t Interval_Max, Word_t CSB_Supervision_Timeout, Byte_t *StatusResult, Byte_t *LT_ADDRResult, Word_t *IntervalResult);
#endif

   /*! @brief The following function issues the
    * HCI_Set_Connectionless_Slave_Broadcast_Receive Command to the
    * Bluetooth Device that is associated with the Bluetooth Protocol
    * Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param Enable If the Enable parameter is set to Enabled, the BR/EDR Controller starts attempting to receive Connectionless Slave Broadcast packets on the specified LT_ADDR.
    * @param LT_ADDR Allows the Host to request that theBR/EDR Controller reserve a specific LT_ADDR for Connectionless Slave Broadcast.
    * @param BD_ADDR BD_ADDR of remote device from which the C and R values were received.
    * @param Interval Will contain the BD_ADDR returned from the Bluetooth Device.
    * @param Clock_Offset CLKNslave – CLK) modulo 228.
    * @param Next_CSB_Clock CLK for next Connectionless Slave Broadcast instant.
    * @param CSB_Supervision_Timeout Duration in slots to continue listening for Connectionless Slave Broadcast packets after the last successfully received Connectionless Slave Broadcast packet.
    * @param Remote_Timing_Accuracy Timing accuracy of the master in ppm. Typical values are 20ppm and 250ppm.
    * @param Skip Number of Connectionless Slave Broadcast instants to skip after successfully receiving a Broadcast packet.
    * @param Packet_Type Packets identified by Packet_Type that are currently pending for transmission in the Controller for the specified Handle.
    * @param AFH_Channel_Map Used by the master for the PBD logical link and is obtained by the slave’s host.
    * @param StatusResult If function returns zero (success) this variable will contain the Status Result returned from the Bluetooth device.
    * @param BD_ADDRResult Will contain the BD_ADDR returned from the Bluetooth Device.
    * @param LT_ADDRResult Will contain the LT_ADDR returned from the Bluetooth Device.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the BD_ADDRResult variable will contain
    * the BD_ADDR returned from the Bluetooth Device AND the
    * LT_ADDRResult variable will contain the LT_ADDR returned from the
    * Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Set_Connectionless_Slave_Broadcast_Receive(unsigned int BluetoothStackID, Byte_t Enable, BD_ADDR_t BD_ADDR, Byte_t LT_ADDR, Word_t Interval, DWord_t Clock_Offset, DWord_t Next_CSB_Clock, Word_t CSB_Supervision_Timeout, Byte_t Remote_Timing_Accuracy, Byte_t Skip, Word_t Packet_Type, AFH_Channel_Map_t *AFH_Channel_Map, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult, Word_t *LT_ADDRResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Set_Connectionless_Slave_Broadcast_Receive_t)(unsigned int BluetoothStackID, Byte_t Enable, BD_ADDR_t BD_ADDR, Byte_t LT_ADDR, Word_t Interval, DWord_t Clock_Offset, DWord_t Next_CSB_Clock, Word_t CSB_Supervision_Timeout, Byte_t Remote_Timing_Accuracy, Byte_t Skip, Word_t Packet_Type, AFH_Channel_Map_t *AFH_Channel_Map, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult, Word_t *LT_ADDRResult);
#endif

   /*! @brief The following function issues the HCI_Start_Synchronization_Train
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Start_Synchronization_Train(unsigned int BluetoothStackID, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Start_Synchronization_Train_t)(unsigned int BluetoothStackID, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Start_Synchronization_Train
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR BD_ADDR of remote device from which the C and R values were received.
    * @param Sync_Scan_Timeout Specifies the interval between the start of consecutive scan windows.
    * @param Sync_Scan_Window Specifies the duration of each scan
    * @param Sync_Scan_Interval Specifies the interval between the start of consecutive scan windows.
    * @param StatusResult Will contain the Status Result returned from the Bluetooth Device.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Receive_Synchronization_Train(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t Sync_Scan_Timeout, Word_t Sync_Scan_Window, Word_t Sync_Scan_Interval, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Receive_Synchronization_Train_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t Sync_Scan_Timeout, Word_t Sync_Scan_Window, Word_t Sync_Scan_Interval, Byte_t *StatusResult);
#endif

   /*! HCI Command API's (Link Control - Version 4.1). */
    /*! @brief The following function issues the
    * HCI_Remote_OOB_Extended_Data_Request_Reply Command to the
    * Bluetooth Device that is associated with the Bluetooth Protocol
    * Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR BD_ADDR of remote device from which the C and R values were received.
    * @param BD_ADDRResult Will contain the BD_ADDR returned from the Bluetooth Device.
    * @param C_192 Simple Pairing Hash C derived from the P-192 public key.
    * @param R_192 Simple Pairing Randomizer associated with the P-192 public key.
    * @param C_256 Simple Pairing Hash C derived from the P-256 public key.
    * @param R_256 Simple Pairing Randomizer associated with the P-256 public key.
    * @param StatusResult will contain the Status Result returned from the Bluetooth Device.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If @return This function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the BD_ADDRResult variable will contain
    * the BD_ADDR returned from the Bluetooth device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Remote_OOB_Extended_Data_Request_Reply(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Simple_Pairing_Hash_t *C_192, Simple_Pairing_Randomizer_t *R_192, Simple_Pairing_Hash_t *C_256, Simple_Pairing_Randomizer_t *R_256, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Remote_OOB_Extended_Data_Request_Reply_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Simple_Pairing_Hash_t *C_192, Simple_Pairing_Randomizer_t *R_192, Simple_Pairing_Hash_t *C_256, Simple_Pairing_Randomizer_t *R_256, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);
#endif

    /*! HCI Command API's (Link Policy). */
    /*! @brief The following function issues the HCI_Hold_Mode Command to the
    * Bluetooth Device that is associated with the Bluetooth Protocol
    * Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param Connection_Handle Unique identifier for the connection returned in the Connection Complete event associated with the HCI_Create_Connection command.
    * @param Hold_Mode_Max_Interval Maximum time to stay in Hold Mode. Values are number of baseband slots (0.625 msec), with a range of 0.625 msec (0x0001) to 40.9 sec (0xFFFF).
    * @param Hold_Mode_Min_Interval Minimum time to stay in Hold Mode. Values are number of baseband slots (0.625 msec), with a range of 0.625 msec (0x0001) to 40.9 sec (0xFFFF).
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error. If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Hold_Mode(unsigned int BluetoothStackID, Word_t Connection_Handle, Word_t Hold_Mode_Max_Interval, Word_t Hold_Mode_Min_Interval, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Hold_Mode_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Word_t Hold_Mode_Max_Interval, Word_t Hold_Mode_Min_Interval, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Sniff_Mode Command to the
    * Bluetooth Device that is associated with the Bluetooth Protocol
    * Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param Connection_Handle Unique identifier for the connection returned in
    * the Connection Complete event associated with the HCI_Create_Connection command.
    * @param Sniff_Max_Interval Maximum time between each sniff period.
    *  Values are number of baseband slots (0.625 msec), with a range of
    * 0.625 msec (0x000) to 40.9 sec (0xFFFF).
    * @param Sniff_Min_Interval Minimum time between each sniff period.
    *  Values are number of baseband slots (0.625 msec), with a range
    * of 0.625 msec (0x000) to 40.9 sec (0xFFFF).
    * @param Sniff_Attempt Amount of time for each sniff attempt.
    * Values are number of baseband slots (0.625 msec), with a range
    * of 0.625 msec (0x000) to 40.9 sec (0xFFFF).
    * @param Sniff_Timeout Amount of time for sniff timeout.
    * Values are number of baseband slots (0.625 msec), with a range
    * of 0.625 msec (0x000) to 40.9 sec (0xFFFF).
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Sniff_Mode(unsigned int BluetoothStackID, Word_t Connection_Handle, Word_t Sniff_Max_Interval, Word_t Sniff_Min_Interval, Word_t Sniff_Attempt, Word_t Sniff_Timeout, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Sniff_Mode_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Word_t Sniff_Max_Interval, Word_t Sniff_Min_Interval, Word_t Sniff_Attempt, Word_t Sniff_Timeout, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Exit_Sniff_Mode Command to
    * the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Connection_Handle Unique identifier for the connection
    * returned in the Connection Complete event associated with the HCI_Create_Connection command.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Exit_Sniff_Mode(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Exit_Sniff_Mode_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Park_Mode Command to the
    * Bluetooth Device that is associated with the Bluetooth Protocol
    * Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param Connection_Handle Unique identifier for the connection
    * returned in the Connection Complete event associated with the
    * HCI_Create_Connection command.
    * @param Beacon_Max_Interval Maximum time between consecutive
    * beacons. Values are number of baseband slots (0.625 msec),
    * with a range of 0.625 msec (0x000) to 40.9 sec (0xFFFF).
    * @param Beacon_Min_Interval Minimum time between consecutive
    * beacons. Values are number of baseband slots (0.625 msec),
    * with a range of 0.625 msec (0x000) to 40.9 sec (0xFFFF).
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Park_Mode(unsigned int BluetoothStackID, Word_t Connection_Handle, Word_t Beacon_Max_Interval, Word_t Beacon_Min_Interval, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Park_Mode_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Word_t Beacon_Max_Interval, Word_t Beacon_Min_Interval, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Exit_Park_Mode Command to
    * the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to
    * this Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param Connection_Handle Unique identifier for the
    * connection returned in the Connection Complete event
    * associated with the HCI_Create_Connection command.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Exit_Park_Mode(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Exit_Park_Mode_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_QoS_Setup Command to the
    * Bluetooth Device that is associated with the Bluetooth Protocol
    * Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param Connection_Handle Unique identifier for the connection returned in the
    * Connection Complete event associated with the HCI_Create_Connection command.
    * @param Flags  (reserved for future use)
    * @param Service_Type The type of service to establish.  Possible values are:
    * HCI_QOS_SERVICE_TYPE_NO_TRAFFIC
    * HCI_QOS_SERVICE_TYPE_BEST_EFFORT
    * HCI_QOS_SERVICE_TYPE_GUARANTEED
    * @param Token_Rate Token Rate in bytes per second.
    * @param Peak_Bandwidth  Peak Bandwidth in bytes per second.
    * @param Latency Latency in microseconds.
    * @param Delay_Variation Delay Variation in microseconds.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_QoS_Setup(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t Flags, Byte_t Service_Type, DWord_t Token_Rate, DWord_t Peak_Bandwidth, DWord_t Latency, DWord_t Delay_Variation, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_QoS_Setup_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t Flags, Byte_t Service_Type, DWord_t Token_Rate, DWord_t Peak_Bandwidth, DWord_t Latency, DWord_t Delay_Variation, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Role_Discovery Command to
    * the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param Connection_Handle Unique identifier for the connection returned in the
    * Connection Complete event associated with the HCI_Create_Connection command.
    * @param StatusResult Returned HCI status code.
    * @param Connection_HandleResult Unique identifier for the connection handle for
    * which the role discovery was done.
    * @param Current_RoleResult  The current role for the Connection_HandleResult.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the Connection_HandleResult will contain
    * the Connection_Handle Result returned from the Bluetooth Device
    * AND Current_RoleResult will contain the Current Role Result
    * returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Role_Discovery(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult, Byte_t *Current_RoleResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Role_Discovery_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult, Byte_t *Current_RoleResult);
#endif

   /*! @brief The following function issues the HCI_Switch_Role Command to the
    * Bluetooth Device that is associated with the Bluetooth Protocol
    * Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param BD_ADDR Address of the Bluetooth device.
    * @param Role Role for this device to take on.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Switch_Role(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t Role, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Switch_Role_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t Role, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Read_Link_Policy_Settings
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Connection_Handle Unique identifier for the connection returned
    * in the Connection Complete event associated with the HCI_Create_Connection command.
    * @param StatusResult Returned HCI status code.
    * @param Connection_HandleResult Unique identifier for the connection
    * handle for which the policy reading was done.
    * @param Link_Policy_SettingsResult The current link policy settings
    * for the Connection_HandleResult connection
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * Connection_HandleResult will contain the Connection_Handle Result
    * returned from the Bluetooth Device AND Link_Policy_SettingsResult
    * will contain the Link Policy Settings Result returned from the
    * Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Link_Policy_Settings(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult, Word_t *Link_Policy_SettingsResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Link_Policy_Settings_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult, Word_t *Link_Policy_SettingsResult);
#endif

   /*! @brief The following function issues the HCI_Write_Link_Policy_Settings
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Connection_Handle Unique identifier for the connection returned in
    * the Connection Complete event associated with the HCI_Create_Connection command.
    * @param Link_Policy_Settings The link policy settings for the
    * Connection_HandleResult connection to write.
    * @param StatusResult Returned HCI status code.
    * @param Connection_HandleResult Unique identifier for the connection handle
    * for which the policy writing was done.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * Connection_HandleResult will contain the Connection_Handle Result
    * returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Link_Policy_Settings(unsigned int BluetoothStackID, Word_t Connection_Handle, Word_t Link_Policy_Settings, Byte_t *StatusResult, Word_t *Connection_HandleResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Link_Policy_Settings_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Word_t Link_Policy_Settings, Byte_t *StatusResult, Word_t *Connection_HandleResult);
#endif

   /* HCI Command API's (Link Policy - Version 1.2).
    * @brief The following function issues the
    * HCI_Read_Default_Link_Policy_Settings Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @param Link_Policy_SettingsResult The current default link policy settings
    * for all new connections.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the Link_Policy_SettingsResult will
    * contain the Link Policy Settings Result returned from the
    * Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Default_Link_Policy_Settings(unsigned int BluetoothStackID, Byte_t *StatusResult, Word_t *Link_Policy_SettingsResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Default_Link_Policy_Settings_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Word_t *Link_Policy_SettingsResult);
#endif

   /*! @brief The following function issues the
    * HCI_Write_Default_Link_Policy_Settings Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Link_Policy_Settings The updated default link policy settings
    * for all new connections.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Default_Link_Policy_Settings(unsigned int BluetoothStackID, Word_t Link_Policy_Settings, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Default_Link_Policy_Settings_t)(unsigned int BluetoothStackID, Word_t Link_Policy_Settings, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Flow_Specification Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param Connection_Handle Unique identifier for the connection
    * returned in the Connection Complete event associated with the
    * HCI_Create_Connection command.
    * @param Flags   Reserved for future use and shall be set to 0
    * and ignored by the receiver.
    * @param Flow_Direction Determines if the parameters refer to the
    * outgoing or incoming traffic of the ACL link.
    * @param Service_Type Indicates the level of service required.
    * @param Token_Rate  The average data rate with which the application
    * transmits data.
    * @param Token_Bucket_Size Specifies a limit on the 'burstiness'
    * with which the application may transmit data.
    * @param Peak_Bandwidth Limits how fast packets from applications
    * may be sent back-to-back.
    * @param Access_Latency The maximum acceptable delay of an L2CAP
    * packet to the air-interface.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Flow_Specification(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t Flags, Byte_t Flow_Direction, Byte_t Service_Type, DWord_t Token_Rate, DWord_t Token_Bucket_Size, DWord_t Peak_Bandwidth, DWord_t Access_Latency, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Flow_Specification_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t Flags, Byte_t Flow_Direction, Byte_t Service_Type, DWord_t Token_Rate, DWord_t Token_Bucket_Size, DWord_t Peak_Bandwidth, DWord_t Access_Latency, Byte_t *StatusResult);
#endif

   /*! HCI Command API's (Link Policy - Version 2.1 + EDR). */

   /*! @brief The following function issues the HCI_Sniff_Subrating Command to
    * the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param Connection_Handle Unique identifier for the connection returned in the
    * Connection Complete event associated with the HCI_Create_Connection command.
    * @param Maximum_Latency Used to calculate the maximum sniff subrate that the
    * remote device may use. Values are number of baseband slots (0.625 msec), with
    * a range of 0.625 msec (0x000) to 40.9 sec (0xFFFE)
    * @param Minimum_Remote_Timeout Minimum base sniff subrate timeout that the
    * remote device may use. Values are number of baseband slots (0.625 msec),
    * with a range of 0.625 msec (0x000) to 40.9 sec (0xFFFE)
    * @param Minimum_Local_Timeout Minimum base sniff subrate timeout that the
    * local device may use. Values are number of baseband slots (0.625 msec),
    * with a range of 0.625 msec (0x000) to 40.9 sec (0xFFFE)
    * @param StatusResult If function returns zero (success) this variable will
    * contain the Status Result returned from the Bluetooth device
    * @param Connection_HandleResult If function returns zero (success) this
    * variable will contain the Connection_Handle Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the Connection_HandleResult will contain
    * the Connection_Handle Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Sniff_Subrating(unsigned int BluetoothStackID, Word_t Connection_Handle, Word_t Maximum_Latency, Word_t Minimum_Remote_Timeout, Word_t Minimum_Local_Timeout, Byte_t *StatusResult, Word_t *Connection_HandleResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Sniff_Subrating_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Word_t Maximum_Latency, Word_t Minimum_Remote_Timeout, Word_t Minimum_Local_Timeout, Byte_t *StatusResult, Word_t *Connection_HandleResult);
#endif

   /*! HCI Command API's (Host Controller and Baseband). */

   /*! @brief The following function issues the HCI_Set_Event_Mask Command to
    * the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param Event_Mask  Eight-byte bit mask of events to allow.
    * Setting a bit to one enables the corresponding event.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Set_Event_Mask(unsigned int BluetoothStackID, Event_Mask_t Event_Mask, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Set_Event_Mask_t)(unsigned int BluetoothStackID, Event_Mask_t Event_Mask, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Reset Command to the
    * Bluetooth Device that is associated with the Bluetooth Protocol
    * Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Reset(unsigned int BluetoothStackID, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Reset_t)(unsigned int BluetoothStackID, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Set_Event_Filter Command to
    * the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param Filter_Type The type of filter that the condition is being set for.
    * @param Filter_Condition_Type The filter condition to be set for the specified
    * Filter_Type.  This field is ignored for the Clear type.
    * @param Condition This is a overlayed structure which permits specifying the
    * filter condition for the later two Condition Types for each Filter Type.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Set_Event_Filter(unsigned int BluetoothStackID, Byte_t Filter_Type,  Byte_t Filter_Condition_Type, Condition_t Condition, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Set_Event_Filter_t)(unsigned int BluetoothStackID, Byte_t Filter_Type,  Byte_t Filter_Condition_Type, Condition_t Condition, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Flush Command to the
    * Bluetooth Device that is associated with the Bluetooth Protocol
    * Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param Connection_Handle Unique identifier for the connection returned in the
    * Connection Complete event associated with the HCI_Create_Connection command.
    * @param StatusResult Returned HCI status code.
    * @param Connection_HandleResult Unique identifier for the connection handle
    * for which the operation was done.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the Connection_HandleResult variable
    * will contain the Connection Handle Result returned from the
    * Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Flush(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Flush_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult);
#endif

   /*! @brief The following function issues the HCI_Read_PIN_Type Command to the
    * Bluetooth Device that is associated with the Bluetooth Protocol
    * Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to
    * this Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @param PIN_TypeResult The type of PIN supported by the Host.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the PIN_TypeResultResult variable will
    * contain the PIN Type Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_PIN_Type(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *PIN_TypeResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_PIN_Type_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *PIN_TypeResult);
#endif

   /*! @brief The following function issues the HCI_Write_PIN_Type Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param PIN_Type The type of PIN supported by the Host.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_PIN_Type(unsigned int BluetoothStackID, Byte_t PIN_Type, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_PIN_Type_t)(unsigned int BluetoothStackID, Byte_t PIN_Type, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Create_New_Unit_Key Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Create_New_Unit_Key(unsigned int BluetoothStackID, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Create_New_Unit_Key_t)(unsigned int BluetoothStackID, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Read_Stored_Link_Key Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param BD_ADDR Address of the Bluetooth device.
    * @param Read_All_Flag Flag to indicate whether only the Link Key for the
    * specified Bluetooth device should be returned or all Link Keys.
    * @param StatusResult Returned HCI status code.
    * @param Max_Num_KeysResult  Maximum number of Link Keys that can be stored
    * in the Host Controller.
    * @param Num_Keys_ReadResult Number of Link Keys being read. The Link Keys
    * will be returned in this number of etReturn_Link_Keys_Event events.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the Max_Num_KeysResult variable will
    * contain the Maximum Number of Keys Result returned from the
    * Bluetooth Device AND the Num_Keys_ReadResult variable will
    * contain the Number of Keys Read Result returned from the
    * Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Stored_Link_Key(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t Read_All_Flag, Byte_t *StatusResult, Word_t *Max_Num_KeysResult, Word_t *Num_Keys_ReadResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Stored_Link_Key_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t Read_All_Flag, Byte_t *StatusResult, Word_t *Max_Num_KeysResult, Word_t *Num_Keys_ReadResult);
#endif

   /*! @brief The following function issues the
    * HCI_Write_Stored_Link_Key Command to the Bluetooth Device that is
    * associated with the Bluetooth Protocol Stack specified by the
    * BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Num_Keys_To_Write Number of Keys in the array to be written.
    * @param HCI_Stored_Link_Key_Info Array of structures which pair up
    * Bluetooth devices and Link Keys.
    * @param StatusResult Returned HCI status code.
    * @param Num_Keys_Written Number of Link Keys actually written.
    * @return This function returns zero if
    * successful, or a non-zero value if there was an error. If this function returns zero (success) then the StatusResult variable
    * will contain the Status Result returned from the Bluetooth Device
    * AND the Num_Keys_WrittenResult variable will contain the Number
    * of Keys Written Result returned from the Bluetooth Device.  The
    * Num_Keys_To_Write parameter specifies the
    * Number of HCI_Stored_Link_Key_Info_t entries that are pointed to
    * by the HCI_Stored_Link_Key_Info parameter.  The Num_Keys_To_Write
    * parameter CANNOT be zero, and the HCI_Stored_Link_Key_Info
    * parameter MUST point to Num_Keys_To_Write HCI_Stored_Link_Key_Info
    * structures.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Stored_Link_Key(unsigned int BluetoothStackID, Byte_t Num_Keys_To_Write, HCI_Stored_Link_Key_Info_t HCI_Stored_Link_Key_Info[], Byte_t *StatusResult, Byte_t *Num_Keys_Written);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Stored_Link_Key_t)(unsigned int BluetoothStackID, Byte_t Num_Keys_To_Write, HCI_Stored_Link_Key_Info_t HCI_Stored_Link_Key_Info[], Byte_t *StatusResult, Byte_t *Num_Keys_Written);
#endif

   /*! @brief The following function issues the
    * HCI_Delete_Stored_Link_Key Command to the Bluetooth Device that is
    * associated with the Bluetooth Protocol Stack specified by the
    * BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param BD_ADDR Address of the Bluetooth device. This field is ignored, if
    * the Delete_All_Flag is set to indicate deleting all.
    * @param Delete_All_Flag A flag to indicate whether all the stored Link Keys
    * should be deleted or not.
    * @param StatusResult Returned HCI status code.
    * @param Num_Keys_DeletedResult Returned number of Link Keys deleted
    * @return This function returns zero if
    * successful, or a non-zero value if there was an error.
    * If this function returns zero (success) then the StatusResult variable
    * will contain the Status Result returned from the Bluetooth Device
    * AND the Num_Keys_DeletedResult variable will contain the Number
    * of Keys Deleted Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Delete_Stored_Link_Key(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t Delete_All_Flag, Byte_t *StatusResult, Word_t *Num_Keys_DeletedResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Delete_Stored_Link_Key_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t Delete_All_Flag, Byte_t *StatusResult, Word_t *Num_Keys_DeletedResult);
#endif

   /*! @brief The following function issues the HCI_Change_Local_Name Command to
    * the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param Name Pointer to null-terminated name (up to 249 bytes including the NULL character)
    * @param StatusResult Returned HCI status code.
    * @param BluetoothStackID   Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Name Pointer to null-terminated name (up to 249 bytes including
    * the NULL character)
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.  The Name parameter is a NULL terminated
    * String that represents the Name parameter of this HCI function.
    * This name can be up to MAX_NAME_LENGTH Bytes (not counting the
    * NULL terminating character).
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Change_Local_Name(unsigned int BluetoothStackID, char *Name, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Change_Local_Name_t)(unsigned int BluetoothStackID, char *Name, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Read_Local_Name Command to
    * the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @param NameResult Returned NULL-terminated character string, up to 249 bytes.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the NameResult variable will contain a
    * NULL terminated String which represents the Name Result that was
    * returned from the Bluetooth Device.  This function requires that
    * NameResult be a pointer to a Buffer that is at least
    * MAX_NAME_LENGTH+1 to hold the largest possible name that can be
    * assigned to a Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Local_Name(unsigned int BluetoothStackID, Byte_t *StatusResult, char *NameResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Local_Name_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, char *NameResult);
#endif

   /*! @brief The following function issues the
    * HCI_Read_Connection_Accept_Timeout Command to the Bluetooth Device
    * that is associated with the Bluetooth Protocol Stack specified by
    * the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @param Conn_Accept_TimeoutResult Current timeout value.Values are number
    * of baseband slots (0.625 msec), with a range of 0.625 msec (0x000) to 40.9 sec (0xFFFF).
    * @return This function returns zero if
    * successful, or a non-zero value if there was an error.
    * If this function returns zero (success) then the StatusResult variable
    * will contain the Status Result returned from the Bluetooth Device
    * AND the Conn_Accept_TimeoutResult variable will contain the
    * Connection Accept Timeout Result returned from the Bluetooth
    * Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Connection_Accept_Timeout(unsigned int BluetoothStackID, Byte_t *StatusResult, Word_t *Conn_Accept_TimeoutResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Connection_Accept_Timeout_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Word_t *Conn_Accept_TimeoutResult);
#endif

   /*! @brief The following function issues the
    * HCI_Write_Connection_Accept_Timeout Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Conn_Accept_Timeout New Timeout value.
    * Values are number of baseband slots (0.625 msec), with a range
    * of 0.625 msec (0x000) to 40.9 sec (0xFFFF).
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error. If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned
    * from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Connection_Accept_Timeout(unsigned int BluetoothStackID, Word_t Conn_Accept_Timeout, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Connection_Accept_Timeout_t)(unsigned int BluetoothStackID, Word_t Conn_Accept_Timeout, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Read_Page_Timeout Command to
    * the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @param Page_TimeoutResult Current timeout value.  Values are number of
    * baseband slots (0.625 msec), with a range of 0.625 msec (0x000) to 40.9 sec (0xFFFF).
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the Page_TimeoutResult variable will
    * contain the Page Timeout Result returned from the Bluetooth
    * Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Page_Timeout(unsigned int BluetoothStackID, Byte_t *StatusResult, Word_t *Page_TimeoutResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Page_Timeout_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Word_t *Page_TimeoutResult);
#endif

   /*! @brief The following function issues the HCI_Write_Page_Timeout Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Page_Timeout New timeout value.  Values are number of baseband
    * slots (0.625 msec), with a range of 0.625 msec (0x000) to 40.9 sec (0xFFFF).
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned
    * from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Page_Timeout(unsigned int BluetoothStackID, Word_t Page_Timeout, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Page_Timeout_t)(unsigned int BluetoothStackID, Word_t Page_Timeout, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Read_Scan_Enable Command to
    * the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @param Scan_EnableResult Current setting of this parameter.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the Scan_EnableResult variable will
    * contain the Scan Enable Result returned from the Bluetooth
    * Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Scan_Enable(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Scan_EnableResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Scan_Enable_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Scan_EnableResult);
#endif

   /*! @brief The following function issues the HCI_Write_Scan_Enable Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Scan_Enable Desired setting of this parameter.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned
    * from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Scan_Enable(unsigned int BluetoothStackID, Byte_t Scan_Enable, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Scan_Enable_t)(unsigned int BluetoothStackID, Byte_t Scan_Enable, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Read_Page_Scan_Activity
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @param Page_Scan_IntervalResult Amount of time between consecutive page scans.
    *  Values are number of baseband slots (0.625 msec), with a range of .25 msec
    * (0x002) to 2560 msec (0x000).
    * @param Page_Scan_WindowResult Amount of time for the duration of the page scan.
    * This parameter will be less than or equal to the Page_Scan_Interval.
    * Values are number of baseband slots (0.625 msec), with a range of
    * .25 msec (0x002) to 2560 msec (0x000).
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * Page_Scan_IntervalResult variable will contain the Page Scan
    * Interval Result returned from the Bluetooth Device AND the
    * Page_Scan_WindowResult variable will contain the Page Scan
    * Window Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Page_Scan_Activity(unsigned int BluetoothStackID, Byte_t *StatusResult, Word_t *Page_Scan_IntervalResult, Word_t *Page_Scan_WindowResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Page_Scan_Activity_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Word_t *Page_Scan_IntervalResult, Word_t *Page_Scan_WindowResult);
#endif

   /*! @brief The following function issues the HCI_Write_Page_Scan_Activity
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param Page_Scan_Interval Defines the amount of time between consecutive page
    * scans.  Values are number of baseband slots (0.625 msec), with a range of
    * .25 msec (0x002) to 2560 msec (0x000).  Default value is .28 sec (0x0800).
    * @param Page_Scan_Window Defines the amount of time for the duration of the
    * page scan. This parameter must be less than or equal to the Page_Scan_Interval.
    * Values are number of baseband slots (0.625 msec), with a range of .25 msec
    * (0x002) to 2560 msec (0x000).  Default value is .25 msec (0x002).
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Page_Scan_Activity(unsigned int BluetoothStackID, Word_t Page_Scan_Interval, Word_t Page_Scan_Window, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Page_Scan_Activity_t)(unsigned int BluetoothStackID, Word_t Page_Scan_Interval, Word_t Page_Scan_Window, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Read_Inquiry_Scan_Activity
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @param Inquiry_Scan_IntervalResult Amount of time between consecutive
    * inquiry scans.  Values are number of baseband slots (0.625 msec),
    * with a range of .25 msec (0x002) to 2560 msec (0x000).
    * @param Inquiry_Scan_WindowResult Amount of time for the duration of
    * the inquiry scan. This parameter will be less than or equal to the
    * Inquiry_Scan_Interval.  Values are number of baseband slots (0.625 msec),
    * with a range of .25 msec (0x002) to 2560 msec (0x000).
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * Inquiry_Scan_IntervalResult variable will contain the Inquiry Scan
    * Interval Result returned from the Bluetooth Device AND the
    * Inquiry_Scan_WindowResult variable will contain the Inquiry Scan
    * Window Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Inquiry_Scan_Activity(unsigned int BluetoothStackID, Byte_t *StatusResult, Word_t *Inquiry_Scan_IntervalResult, Word_t *Inquiry_Scan_WindowResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Inquiry_Scan_Activity_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Word_t *Inquiry_Scan_IntervalResult, Word_t *Inquiry_Scan_WindowResult);
#endif

   /*! @brief The following function issues the HCI_Write_Inquiry_Scan_Activity
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Inquiry_Scan_Interval Defines the amount of time between
    * consecutive inquiry scans.  Values are number of baseband slots (0.625 msec),
    * with a range of .25 msec (0x002) to 2560 msec (0x000).  Default value is .28
    * sec (0x0800).
    * @param Inquiry_Scan_Window Defines the amount of time for the
    * duration of the inquiry scan. This parameter must be less than or equal to
    * the Inquiry_Scan_Interval. Values are number of baseband slots (0.625 msec),
    * with a range of .25 msec (0x002) to 2560 msec (0x000).  Default value is
    * .25 msec (0x002).
    * @param StatusResult Returned HCI status code.

    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Inquiry_Scan_Activity(unsigned int BluetoothStackID, Word_t Inquiry_Scan_Interval, Word_t Inquiry_Scan_Window, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Inquiry_Scan_Activity_t)(unsigned int BluetoothStackID, Word_t Inquiry_Scan_Interval, Word_t Inquiry_Scan_Window, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Read_Authentication_Enable
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @param Authentication_EnableResult Current value of this parameter.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * Authentication_EnableResult variable will contain the
    * Authentication Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Authentication_Enable(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Authentication_EnableResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Authentication_Enable_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Authentication_EnableResult);
#endif

   /*! @brief The following function issues the HCI_Write_Authentication_Enable
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack
    * via a call to BSC_Initialize
    * @param Authentication_Enable Desired value of this parameter.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Authentication_Enable(unsigned int BluetoothStackID, Byte_t Authentication_Enable, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Authentication_Enable_t)(unsigned int BluetoothStackID, Byte_t Authentication_Enable, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Read_Encryption_Mode
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @param Encryption_ModeResult Current value of this parameter.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * Encryption_ModeResult variable will contain the Encryption Mode
    * Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Encryption_Mode(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Encryption_ModeResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Encryption_Mode_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Encryption_ModeResult);
#endif

   /*! @brief The following function issues the HCI_Write_Encryption_Mode
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Encryption_Mode Desired value of this parameter.
    * @param StatusResult Returned HCI status code.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param Encryption_Mode Desired value of this parameter.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Encryption_Mode(unsigned int BluetoothStackID, Byte_t Encryption_Mode, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Encryption_Mode_t)(unsigned int BluetoothStackID, Byte_t Encryption_Mode, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Read_Class_of_Device
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @param Class_of_DeviceResult Bit mask list of features that determine
    * the class of device for this Bluetooth device.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * Class_of_DeviceResult variable will contain the Class of Device
    * Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Class_of_Device(unsigned int BluetoothStackID, Byte_t *StatusResult, Class_of_Device_t *Class_of_DeviceResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Class_of_Device_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Class_of_Device_t *Class_of_DeviceResult);
#endif

   /*! @brief The following function issues the HCI_Write_Class_of_Device
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param Class_of_Device Bit mask list of features that determine
    * the class of device for this Bluetooth device.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Class_of_Device(unsigned int BluetoothStackID, Class_of_Device_t Class_of_Device, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Class_of_Device_t)(unsigned int BluetoothStackID, Class_of_Device_t Class_of_Device, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Read_Voice_Setting Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @param Voice_SettingResult Current voice settings.
    * To test these bits, the result must first be masked with one of the following masks.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the Voice_SettingResult variable will
    * contain the Voice Setting Result returned from the Bluetooth
    * Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Voice_Setting(unsigned int BluetoothStackID, Byte_t *StatusResult, Word_t *Voice_SettingResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Voice_Setting_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Word_t *Voice_SettingResult);
#endif

   /*! @brief The following function issues the HCI_Write_Voice_Setting Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Voice_Setting Desired voice settings.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Voice_Setting(unsigned int BluetoothStackID, Word_t Voice_Setting, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Voice_Setting_t)(unsigned int BluetoothStackID, Word_t Voice_Setting, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Read_Automatic_Flush_Timeout
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Connection_Handle Unique identifier for the connection
    * returned in the Connection Complete event associated with the
    * HCI_Create_Connection command.
    * @param StatusResult Returned HCI status code.
    * @param Connection_HandleResult Unique identifier for the connection
    * handle for which the operation was done.
    * @param Flush_TimeoutResult Current timeout value. A zero indicates
    * that there is no timeout defined (or infinite timeout).
    * Values are number of baseband slots (0.625 msec), with a range
    * of 0.625 msec (0x000) to almost .28 sec (0x07FF).
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * Connection_HandleResult variable will contain the Connection
    * Handle Result returned from the Bluetooth Device AND the
    * Flush_TimeoutResult variable will contain the Flush Timeout
    * Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Automatic_Flush_Timeout(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult, Word_t *Flush_TimeoutResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Automatic_Flush_Timeout_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult, Word_t *Flush_TimeoutResult);
#endif

   /*! @brief The following function issues the
    * HCI_Write_Automatic_Flush_Timeout Command to the Bluetooth Device
    * that is associated with the Bluetooth Protocol Stack specified by
    * the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param Connection_Handle Unique identifier for the connection returned in the
    * Connection Complete event associated with the HCI_Create_Connection command.
    * @param Flush_Timeout Current timeout value.  A zero indicates that there is
    * no timeout defined (or infinite timeout).  Values are number of baseband slots (0.625 msec), with a range of 0.625 msec (0x000) to almost .28 sec (0x07FF).
    * @param StatusResult Returned HCI status code.
    * @param Connection_HandleResult Unique identifier for the connection handle
    * for which the operation was done.
    * @return This function returns zero if
    * successful, or a non-zero value if there was an error.
    * If this function returns zero (success) then the StatusResult variable
    * will contain the Status Result returned from the Bluetooth Device
    * AND the Connection_HandleResult variable will contain the
    * Connection Handle Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Automatic_Flush_Timeout(unsigned int BluetoothStackID, Word_t Connection_Handle, Word_t Flush_Timeout, Byte_t *StatusResult, Word_t *Connection_HandleResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Automatic_Flush_Timeout_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Word_t Flush_Timeout, Byte_t *StatusResult, Word_t *Connection_HandleResult);
#endif

   /*! @brief The following function issues the
    * HCI_Read_Num_Broadcast_Retransmissions Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @param Num_Broadcast_RetranResult  Current parameter value
    * in the range of 0x00 to 0xFF.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned
    * from the Bluetooth Device AND the Num_Broadcast_RetranResult
    * variable will contain the Number of Broadcast Retransmissions
    * Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Num_Broadcast_Retransmissions(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Num_Broadcast_RetranResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Num_Broadcast_Retransmissions_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Num_Broadcast_RetranResult);
#endif

   /*! @brief The following function issues the
    * HCI_Write_Num_Broadcast_Retransmissions Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param Num_Broadcast_Retran Desired parameter value in
    * the range of 0x00 to 0xFF.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned
    * from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Num_Broadcast_Retransmissions(unsigned int BluetoothStackID, Byte_t Num_Broadcast_Retran, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Num_Broadcast_Retransmissions_t)(unsigned int BluetoothStackID, Byte_t Num_Broadcast_Retran, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Read_Hold_Mode_Activity
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @param Hold_Mode_ActivityResult Current parameter value.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * Hold_Mode_ActivityResult variable will contain the Hold Mode
    * Activity Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Hold_Mode_Activity(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Hold_Mode_ActivityResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Hold_Mode_Activity_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Hold_Mode_ActivityResult);
#endif

   /*! @brief The following function issues the HCI_Write_Hold_Mode_Activity
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param Hold_Mode_Activity Current parameter value.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Hold_Mode_Activity(unsigned int BluetoothStackID, Byte_t Hold_Mode_Activity, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Hold_Mode_Activity_t)(unsigned int BluetoothStackID, Byte_t Hold_Mode_Activity, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Read_Transmit_Power_Level
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
     Stack via a call to BSC_Initialize
    * @param Connection_Handle Unique identifier for the connection returned
    * in the Connection Complete event associated with the HCI_Create_Connection command.
    * @param Type Flag to indicate whether to read the current or maximum
    * power level.
    * @param StatusResult Returned HCI status code.
    * @param Connection_HandleResult Unique identifier for the connection
    * handle for which the operation was done.
    * @param Transmit_Power_LevelResult  The current/maximum power level
    * in the range of -30 dBm to +20 dBm.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * Connection_HandleResult variable will contain the Connection
    * Handle Result returned from the Bluetooth Device AND the
    * Transmit_Power_LevelResult variable will contain the Transmit
    * Power Level Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Transmit_Power_Level(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t Type, Byte_t *StatusResult, Word_t *Connection_HandleResult, SByte_t *Transmit_Power_LevelResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Transmit_Power_Level_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t Type, Byte_t *StatusResult, Word_t *Connection_HandleResult, Byte_t *Transmit_Power_LevelResult);
#endif

   /*! @brief The following function issues the HCI_Read_SCO_Flow_Control_Enable
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @param SCO_Flow_Control_EnableResult Current parameter setting.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * SCO_Flow_Control_EnableResult variable will contain the SCO Flow
    * Control Enable Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_SCO_Flow_Control_Enable(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *SCO_Flow_Control_EnableResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_SCO_Flow_Control_Enable_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *SCO_Flow_Control_EnableResult);
#endif

   /*! @brief The following function issues the
    * HCI_Write_SCO_Flow_Control_Enable Command to the Bluetooth Device
    * that is associated with the Bluetooth Protocol Stack specified by
    * the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param SCO_Flow_Control_Enable Current parameter setting.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if
    * successful, or a non-zero value if there was an error.  If this function returns zero (success) then the StatusResult variable
    * will contain the Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_SCO_Flow_Control_Enable(unsigned int BluetoothStackID, Byte_t SCO_Flow_Control_Enable, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_SCO_Flow_Control_Enable_t)(unsigned int BluetoothStackID, Byte_t SCO_Flow_Control_Enable, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the
    * HCI_Set_Host_Controller_To_Host_Flow_Control Command to the
    * Bluetooth Device that is associated with the Bluetooth Protocol
    * Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to
    * this Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param Flow_Control_Enable Desired setting of this parameter.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned
    * from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Set_Host_Controller_To_Host_Flow_Control(unsigned int BluetoothStackID, Byte_t Flow_Control_Enable, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Set_Host_Controller_To_Host_Flow_Control_t)(unsigned int BluetoothStackID, Byte_t Flow_Control_Enable, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Host_Buffer_Size Command to
    * the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Host_ACL_Data_Packet_Length Maximum length of ACL data packets,
    * up to 0xFFFF
    * @param Host_SCO_Data_Packet_Length Maximum length of SCO data packets,
    * up to 0xFF
    * @param Host_Total_Num_ACL_Data_Packets Maximum number of ACL packets
    * that can be held in the host, up to 0xFFFF.
    * @param Host_Total_Num_SCO_Data_Packets Maximum number of SCO packets
    * that can be held in the host, up to 0xFFFF.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned
    * from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Host_Buffer_Size(unsigned int BluetoothStackID, Word_t Host_ACL_Data_Packet_Length, Byte_t Host_SCO_Data_Packet_Length, Word_t Host_Total_Num_ACL_Data_Packets, Word_t Host_Total_Num_SCO_Data_Packets, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Host_Buffer_Size_t)(unsigned int BluetoothStackID, Word_t Host_ACL_Data_Packet_Length, Byte_t Host_SCO_Data_Packet_Length, Word_t Host_Total_Num_ACL_Data_Packets, Word_t Host_Total_Num_SCO_Data_Packets, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the
    * HCI_Host_Number_Of_Completed_Packets Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Number_Of_Handles Number of packets in the provided array.
    *  Must not be zero.
    * @param HCI_Host_Completed_Packets_Info Array of structures which
    * pair up a connection handle and the number of packets which have been
    * completed for that handle.
    * @param WaitForResponse Boolean flag indicating whether this
    * function call should wait until it gets a response from the Host Controller.
    * \note, there is no response unless there is invalid data.
    * Therefore, when the data is good this function will stall until the timeout is reached.
    * If the Host knows it is passing good data, it should probably set this flag to FALSE.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned
    * from the Bluetooth Device.  The Number_Of_Handles parameter
    * specifies the Number of HCI_Host_Completed_Packets_Info_t
    * entries that are pointed to by the HCI_Host_Completed_Packets_Info
    * parameter.  The Number_Of_Handles parameter CANNOT be zero, and
    * the HCI_Host_Completed_Packets_Info parameter MUST point to
    * Number_Of_Handles HCI_Host_Completed_Packets_Info structures.
    * The WaitForResponse flag, specifies whether or not this function
    * is to wait for a response from the Bluetooth Device.  This
    * parameter is provided because the Bluetooth Device DOES NOT ISSUE
    * ANY RESPONSE for this HCI Command UNLESS the HCI Command has
    * invalid data.  If you (the caller) KNOW that the data you
    * specified is correct, then set this parameter to FALSE.  If you
    * want to be absolutely sure, you can wait for the Timeout, however
    * the calling thread will block until either an invalid response
    * OR the Timeout occurs (valid data).
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Host_Number_Of_Completed_Packets(unsigned int BluetoothStackID, Byte_t Number_Of_Handles, HCI_Host_Completed_Packets_Info_t HCI_Host_Completed_Packets_Info[], Byte_t WaitForResponse, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Host_Number_Of_Completed_Packets_t)(unsigned int BluetoothStackID, Byte_t Number_Of_Handles, HCI_Host_Completed_Packets_Info_t HCI_Host_Completed_Packets_Info[], Byte_t WaitForResponse, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the
    * HCI_Read_Link_Supervision_Timeout Command to the Bluetooth Device
    * that is associated with the Bluetooth Protocol Stack specified by
    * the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param Connection_Handle Unique identifier for the connection
    * returned in the Connection Complete event associated with the
    * HCI_Create_Connection command.
    * @param StatusResult Returned HCI status code.
    * @param Connection_HandleResult Unique identifier for the
    * connection handle for which the operation was done.
    * @param Link_Supervision_TimeoutResult Current timeout value.
    * Values are number of baseband slots (0.625 msec), with a range
    * of 0.625 msec (0x000) to 40.9 sec (0xFFFF).
    * @return This function returns zero if
    * successful, or a non-zero value if there was an error.
    * If this function returns zero (success) then the StatusResult variable
    * will contain the Status Result returned from the Bluetooth Device
    * AND the Connection_HandleResult variable will contain the
    * Connection Handle Result returned from the Bluetooth Device AND
    * the Link_Supervision_TimeoutResult variable will contain the Link
    * Supervision Timeout Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Link_Supervision_Timeout(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult, Word_t *Link_Supervision_TimeoutResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Link_Supervision_Timeout_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult, Word_t *Link_Supervision_TimeoutResult);
#endif

   /*! @brief The following function issues the
    * HCI_Write_Link_Supervision_Timeout Command to the Bluetooth Device
    * that is associated with the Bluetooth Protocol Stack specified by
    * the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param Connection_Handle Unique identifier for the connection returned
    * in the Connection Complete event associated with the HCI_Create_Connection command.
    * @param Link_Supervision_Timeout Current timeout value.
    * A value of zero disables this timeout. Values are number
    * of baseband slots (0.625 msec), with a range of 0.625 msec (0x000) to 40.9 sec (0xFFFF).
    * @param StatusResult Returned HCI status code.
    * @param Connection_HandleResult Unique identifier for the
    * connection handle for which the operation was done.
    * @return This function returns zero if
    * successful, or a non-zero value if there was an error.
    * If this function returns zero (success) then the StatusResult variable
    * will contain the Status Result returned from the Bluetooth Device
    * AND the Connection_HandleResult variable will contain the
    * Connection Handle Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Link_Supervision_Timeout(unsigned int BluetoothStackID, Word_t Connection_Handle, Word_t Link_Supervision_Timeout, Byte_t *StatusResult, Word_t *Connection_HandleResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Link_Supervision_Timeout_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Word_t Link_Supervision_Timeout, Byte_t *StatusResult, Word_t *Connection_HandleResult);
#endif

   /*! @brief The following function issues the HCI_Read_Number_Of_Supported_IAC
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @param Num_Support_IACResult Current setting in the range of 0x0 to 0x40.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * Num_Support_IACResult variable will contain the Number of
    * Supported IAC's Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Number_Of_Supported_IAC(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Num_Support_IACResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Number_Of_Supported_IAC_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Num_Support_IACResult);
#endif

   /*! @brief The following function issues the HCI_Read_Current_IAC_LAP Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @param Num_Current_IACResult   Number of IACs currently in use
    * by the local Bluetooth device.
    * @param IAC_LAPResult Array of LAPs (3-Byte structures) for
    * in-use IACs.
    * @return This function returns zero if successful, or a non-zero value
    * if there was an error.  If this function returns zero (success)
    * then the StatusResult variable will contain the Status Result
    * returned from the Bluetooth Device AND the Num_Current_IACResult
    * variable will contain the Number of LAP_t entries that have been
    * copied into the IAC_LAPResult variable.  This function requires
    * that IAC_LAPResult be a pointer to a Buffer that is at least
    * HCI_READ_CURRENT_IAC_LAP_COMMAND_MAX_IAC_LAPS to holde the maximum
    * number of LAP_t entries taht can be returned by the Bluetooth
    * Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Current_IAC_LAP(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Num_Current_IACResult, LAP_t *IAC_LAPResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Current_IAC_LAP_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Num_Current_IACResult, LAP_t *IAC_LAPResult);
#endif

   /*! @brief The following function issues the HCI_Write_Current_IAC_LAP
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to
    * this Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param Num_Current_IAC Number of IAC LAPs provided in
    * this command.
    * @param IAC_LAP Array of LAPs (3-Byte structures)
    * for in-use IACs.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.  The
    * Num_Current_IAC parameter specifies the Number of LAP_t entries
    * that are pointed to by the IAC_LAP parameter.  The
    * Num_Current_IAC parameter CANNOT be zero, and the IAC_LAP
    * parameter MUST point to Num_Current_IAC LAP_t entries.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Current_IAC_LAP(unsigned int BluetoothStackID, Byte_t Num_Current_IAC, LAP_t IAC_LAP[], Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Current_IAC_LAP_t)(unsigned int BluetoothStackID, Byte_t Num_Current_IAC, LAP_t IAC_LAP[], Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Read_Page_Scan_Period_Mode
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @param Page_Scan_Period_ModeResult Current setting of this parameter.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * Page_Scan_Period_ModeResult variable will contain the Page Scan
    * Period Mode Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Page_Scan_Period_Mode(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Page_Scan_Period_ModeResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Page_Scan_Period_Mode_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Page_Scan_Period_ModeResult);
#endif

   /*! @brief The following function issues the HCI_Write_Page_Scan_Period_Mode
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Page_Scan_Period_Mode Current setting of this parameter.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Page_Scan_Period_Mode(unsigned int BluetoothStackID, Byte_t Page_Scan_Period_Mode, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Page_Scan_Period_Mode_t)(unsigned int BluetoothStackID, Byte_t Page_Scan_Period_Mode, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Read_Page_Scan_Mode Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @param Page_Scan_ModeResult Current parameter setting.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the Page_Scan_ModeResult variable will
    * contain the Page Scan Mode Result returned from the Bluetooth
    * Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Page_Scan_Mode(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Page_Scan_ModeResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Page_Scan_Mode_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Page_Scan_ModeResult);
#endif

   /*! @brief The following function issues the HCI_Write_Page_Scan_Mode
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Page_Scan_Mode Current parameter setting.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Page_Scan_Mode(unsigned int BluetoothStackID, Byte_t Page_Scan_Mode, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Page_Scan_Mode_t)(unsigned int BluetoothStackID, Byte_t Page_Scan_Mode, Byte_t *StatusResult);
#endif

   /* HCI Command API's (Host Controller and Baseband - Version 1.2).
    * @brief The following function issues the
    * HCI_Set_AFH_Host_Channel_Classification Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param AFH_Host_Channel_Classification Host channel classification.
    * This is simply a bitmask of the available channels (numbered 0 through 79).
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error. If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Set_AFH_Host_Channel_Classification(unsigned int BluetoothStackID, AFH_Channel_Map_t AFH_Host_Channel_Classification, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Set_AFH_Host_Channel_Classification_t)(unsigned int BluetoothStackID, AFH_Channel_Map_t AFH_Host_Channel_Classification, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Read_Inquiry_Scan_Type
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @param Inquiry_Scan_TypeResult Returned inquiry scan type
    * of the local device.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * Inquiry_Scan_TypeResult variable will contain the Inquiry Scan
    * Type Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Inquiry_Scan_Type(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Inquiry_Scan_TypeResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Inquiry_Scan_Type_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Inquiry_Scan_TypeResult);
#endif

   /*! @brief The following function issues the HCI_Write_Inquiry_Scan_Type
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param Scan_Type Indicates standard scan or interlaced scan.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Inquiry_Scan_Type(unsigned int BluetoothStackID, Byte_t Scan_Type, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Inquiry_Scan_Type_t)(unsigned int BluetoothStackID, Byte_t Scan_Type, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Read_Inquiry_Mode Command
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @param Inquiry_ModeResult Returned inquiry mode setting.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * Inquiry_ModeResult variable will contain the Inquiry Mode Result
    * returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Inquiry_Mode(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Inquiry_ModeResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Inquiry_Mode_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Inquiry_ModeResult);
#endif

   /*! @brief The following function issues the HCI_Write_Inquiry_Mode Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Inquiry_Mode Indicates standard inquiry result mode or inquiry
    * result with RSSI mode.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Inquiry_Mode(unsigned int BluetoothStackID, Byte_t Inquiry_Mode, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Inquiry_Mode_t)(unsigned int BluetoothStackID, Byte_t Inquiry_Mode, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Read_Page_Scan_Type Command
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @param Page_Scan_TypeResult Returned page scan type setting.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * Page_Scan_TypeResult variable will contain the Page Scan Type
    * Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Page_Scan_Type(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Page_Scan_TypeResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Page_Scan_Type_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Page_Scan_TypeResult);
#endif

   /*! @brief The following function issues the HCI_Write_Page_Scan_Type Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param Page_Scan_Type Indicates standard scan or interlaced scan.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Page_Scan_Type(unsigned int BluetoothStackID, Byte_t Page_Scan_Type, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Page_Scan_Type_t)(unsigned int BluetoothStackID, Byte_t Page_Scan_Type, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the
    * HCI_Read_AFH_Channel_Assessment_Mode Command Command to the
    * Bluetooth Device that is associated with the Bluetooth Protocol
    * Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @param AFH_Channel_Assessment_ModeResult Returned AFH channel assessment mode setting.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error. If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the AFH_Channel_Assessment_ModeResult
    * variable will contain the AFH Channel Assessment Mode Result
    * returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_AFH_Channel_Assessment_Mode(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *AFH_Channel_Assessment_ModeResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_AFH_Channel_Assessment_Mode_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *AFH_Channel_Assessment_ModeResult);
#endif

   /*! @brief The following function issues the
    * HCI_Write_AFH_Channel_Assessment_Mode Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param AFH_Channel_Assessment_Mode Indicates whether the controller
    * channel assessment is enabled or disabled.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_AFH_Channel_Assessment_Mode(unsigned int BluetoothStackID, Byte_t AFH_Channel_Assessment_Mode, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_AFH_Channel_Assessment_Mode_t)(unsigned int BluetoothStackID, Byte_t AFH_Channel_Assessment_Mode, Byte_t *StatusResult);
#endif

   /* HCI Command API's (Host Controller and Baseband - Version 2.1 +
    * EDR).
    * @brief The following function issues the
    * HCI_Read_Extended_Inquiry_Response Command to the Bluetooth Device
    * that is associated with the Bluetooth Protocol Stack specified by
    * the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param StatusResult If function returns zero (success) this variable will
    * contain the Status Result returned from the Bluetooth device
    * @param FEC_RequiredResult If function returns zero (success) this variable
    * will contain the FEC Required parameter returned from the Bluetooth device
    * @param Extended_Inquiry_Response_DataResult If function returns zero
    * (success) this variable variable will the contain the Extended Inquiry
    * Response Result returned from the Bluetooth device.
    * @return This function returns zero if
    * successful, or a non-zero value if there was an error.  If this function
    * returns zero (success) then the StatusResult variable
    * will contain the Status Result returned from the Bluetooth Device
    * AND the FEC_RequiredResult variable will contain the FEC Required
    * parameter returned from the Bluetooth Device AND the
    * Extended_Inquiry_Response_DataResult variable will the contain the
    * Extended Inquiry Response Result returned from the Bluetooth
    * Device.  Note the buffer passed in MUST be at least
    * EXTENDED_INQUIRY_RESPONSE_DATA_MAXIMUM_SIZE in length.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Extended_Inquiry_Response(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *FEC_RequiredResult, Extended_Inquiry_Response_Data_t *Extended_Inquiry_Response_DataResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Extended_Inquiry_Response_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *FEC_RequiredResult, Extended_Inquiry_Response_Data_t *Extended_Inquiry_Response_DataResult);
#endif

   /*! @brief The following function issues the
    * HCI_Write_Extended_Inquiry_Response Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param FEC_Required Specifies whether FEC is required
    * @param Extended_Inquiry_Response_Data   Pointer to the actual formatted
    * Extended Inquiry  Response Data (must be 240 bytes in length).
    * @param StatusResult The final parameter is the status.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error. If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Extended_Inquiry_Response(unsigned int BluetoothStackID, Byte_t FEC_Required, Extended_Inquiry_Response_Data_t *Extended_Inquiry_Response_Data, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Extended_Inquiry_Response_t)(unsigned int BluetoothStackID, Byte_t FEC_Required, Extended_Inquiry_Response_Data_t *Extended_Inquiry_Response_Data, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Refresh_Encryption_Key
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Connection_Handle Unique identifier for the connection
    * returned in the Connection Complete event associated with
    * the HCI_Create_Connection command.
    * @param StatusResult If function returns zero (success) this
    * variable will contain the Status Result returned from the Bluetooth device
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Refresh_Encryption_Key(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Refresh_Encryption_Key_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Read_Simple_Pairing_Mode
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device
    * @param Simple_Pairing_ModeResult If function returns zero (success)
    * this variable will contain the Simple Pairing Mode returned from the
    * Bluetooth device.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * Simple_Pairing_ModeResult variable will contain the Simple Pairing
    * Mode returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Simple_Pairing_Mode(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Simple_Pairing_ModeResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Simple_Pairing_Mode_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Simple_Pairing_ModeResult);
#endif

   /*! @brief The following function issues the HCI_Write_Simple_Pairing_Mode
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param Simple_Pairing_Mode Flags whether not simple pairing mode is enabled.
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Simple_Pairing_Mode(unsigned int BluetoothStackID, Byte_t Simple_Pairing_Mode, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Simple_Pairing_Mode_t)(unsigned int BluetoothStackID, Byte_t Simple_Pairing_Mode, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Read_Local_OOB_Data Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device
    * @param Simple_Pairing_HashResult If function returns zero (success)
    * this variable will contain the Simple_Pairing_HashResult returned from
    * the Bluetooth device
    * @param Simple_Pairing_RandomizerResult If function returns zero
    * (success) this variable will contain the Simple_Pairing_RandomizerResult
    * returned from the Bluetooth device
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the Simple_Pairing_HashResult variable
    * will contain the Simple Pairing Hash Result returned from the
    * Bluetooth Device AND the Simple_Pairing_RandomizerResult variable
    * will contain the Simple Pairing Randomizer Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Local_OOB_Data(unsigned int BluetoothStackID, Byte_t *StatusResult, Simple_Pairing_Hash_t *Simple_Pairing_HashResult, Simple_Pairing_Randomizer_t *Simple_Pairing_RandomizerResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Local_OOB_Data_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Simple_Pairing_Hash_t *Simple_Pairing_HashResult, Simple_Pairing_Randomizer_t *Simple_Pairing_RandomizerResult);
#endif

   /*! @brief The following function issues the
    * HCI_Read_Inquiry_Response_Transmit_Power_Level Command to the
    * Bluetooth Device that is associated with the Bluetooth Protocol
    * Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device
    * @param TX_PowerResult If function returns zero (success) this variable
    * will contain the TX_PowerResult returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the TX_PowerResult variable will contain
    * the TX Power Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Inquiry_Response_Transmit_Power_Level(unsigned int BluetoothStackID, Byte_t *StatusResult, SByte_t *TX_PowerResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Inquiry_Response_Transmit_Power_Level_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, SByte_t *TX_PowerResult);
#endif

   /*! @brief The following function issues the
    * HCI_Write_Inquiry_Transmit_Power_Level Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param TX_Power Transmit power level.  This is a signed value that
    * specifies dBm.  Range must be between -70 dB and 20 dBm.
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Inquiry_Transmit_Power_Level(unsigned int BluetoothStackID, SByte_t TX_Power, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Inquiry_Transmit_Power_Level_t)(unsigned int BluetoothStackID, SByte_t TX_Power, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Send_Keypress_Notification
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param BD_ADDR Bluetooth device address of the remote Bluetooth
    * device to receive the Keypress notification.
    * @param KeyPress Keypress Notification value.
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device
    * @param BD_ADDRResult If function returns zero (success) this variable
    * will contain the BD_ADDRResult returned from the Bluetooth device.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * BD_ADDRResult variable will contain the BD_ADDR Result returned
    * from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Send_Keypress_Notification(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t KeyPress, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Send_Keypress_Notification_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t KeyPress, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);
#endif

   /*! @brief The following function issues the
    * HCI_Read_Default_Erroneous_Data_Reporting Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device
    * @param Erroneous_Data_ReportingResult  If function returns zero (success)
    * this variable will contain the Connection_Handle Result returned from
    * the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the Erroneous_Data_ReportingResult
    * variable will contain the Erroneous Data Reporting Result returned
    * from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Default_Erroneous_Data_Reporting(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Erroneous_Data_ReportingResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Default_Erroneous_Data_Reporting_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Erroneous_Data_ReportingResult);
#endif

   /*! @brief The following function issues the
    * HCI_Write_Default_Erroneous_Data_Reporting Command to the
    * Bluetooth Device that is associated with the Bluetooth Protocol
    * Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Erroneous_Data_Reporting Specifies whether Erroneous Data
    * Repoirting is enabled.
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Default_Erroneous_Data_Reporting(unsigned int BluetoothStackID, Byte_t Erroneous_Data_Reporting, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Default_Erroneous_Data_Reporting_t)(unsigned int BluetoothStackID, Byte_t Erroneous_Data_Reporting, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Enhanced_Flush Command to
    * the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Connection_Handle Unique identifier for the connection
    * returned in the Connection Complete event associated with the
    * HCI_Create_Connection command.
    * @param Packet_Type The third parameter is the packet type.
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Enhanced_Flush(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t Packet_Type, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Enhanced_Flush_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t Packet_Type, Byte_t *StatusResult);
#endif

   /* HCI Command API's (Host Controller and Baseband - Version 3.0 +
    * HS).
    * @brief The following function issues the
    * HCI_Read_Logical_Link_Accept_Timeout Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device
    * @param Logical_Link_Accept_TimeoutResult If function returns zero
    * (success) the variable pointed to by this parameter will contain
    * the Logical Link Accept Timeout returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the Logical_Link_Accept_TimeoutResult
    * parameter will contain the Logical Link Accept Timeout Result
    * returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Logical_Link_Accept_Timeout(unsigned int BluetoothStackID, Byte_t *StatusResult, Word_t *Logical_Link_Accept_TimeoutResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Logical_Link_Accept_Timeout_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Word_t *Logical_Link_Accept_TimeoutResult);
#endif

   /*! @brief The following function issues the
    * HCI_Write_Logical_Link_Accept_Timeout Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Logical_Link_Accept_Timeout Contains the Logical Link Accept
    * Timeout that will be written to the Logical Link Accept Timeout configuration parameter.
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Logical_Link_Accept_Timeout(unsigned int BluetoothStackID, Word_t Logical_Link_Accept_Timeout, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Logical_Link_Accept_Timeout_t)(unsigned int BluetoothStackID, Word_t Logical_Link_Accept_Timeout, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Set_Event_Mask_Page_2
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Event_Mask Eight-byte bit mask of events to allow.  Setting a
    * bit to one enables the corresponding event.
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Set_Event_Mask_Page_2(unsigned int BluetoothStackID, Event_Mask_t Event_Mask, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Set_Event_Mask_Page_2_t)(unsigned int BluetoothStackID, Event_Mask_t Event_Mask, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Read_Location_Data Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device
    * @param Location_Domain_AwareResult If function returns zero (success)
    * the variable pointed to by this parameter will contain the Location
    * Domain Aware Result returned from the device.
    * @param Location_DomainResult If function returns zero (success) the variable
    * pointed to by this parameter will contain the Location Domain Result
    * returned from the device.
    * @param Location_Domain_OptionsResult If function returns zero (success)
    * the variable pointed to by this parameter will contain the Location
    * Domain Options result returned from the device.
    * @param Location_OptionsResult If function returns zero (success) the
    * variable pointed to by this parameter will contain the Location Options
    * Result returned from the device.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the Location_Domain_AwareResult variable
    * will contain the Location Domain Aware Result returned from the
    * Bluetooth Device AND the Location_DomainResult variable will
    * contain the Location_Domain Result returned from the Bluetooth
    * Device AND the Location_Domain_OptionsResult variable will contain
    * the Location Domain Options Result AND the Location_OptionsResult
    * will contain the Location Options Result returned from the
    * Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Location_Data(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Location_Domain_AwareResult, Word_t *Location_DomainResult, Byte_t *Location_Domain_OptionsResult, Byte_t *Location_OptionsResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Location_Data_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Location_Domain_AwareResult, Word_t *Location_DomainResult, Byte_t *Location_Domain_OptionsResult, Byte_t *Location_OptionsResult);
#endif

   /*! @brief The following function issues the HCI_Write_Location_Data Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Location_Domain_Aware Location Domain Aware result to write.
    * @param Location_Domain Location Domain result to write.
    * @param Location_Domain_Options Location Domain Options to write.
    * @param Location_Options Location Options to write.
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Location_Data(unsigned int BluetoothStackID, Byte_t Location_Domain_Aware, Word_t Location_Domain, Byte_t Location_Domain_Options, Byte_t Location_Options, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Location_Data_t)(unsigned int BluetoothStackID, Byte_t Location_Domain_Aware, Word_t Location_Domain, Byte_t Location_Domain_Options, Byte_t Location_Options, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Read_Flow_Control_Mode
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device
    * @param Flow_Control_ModeResult If function returns zero (success)
    * the variable pointed to by this parameter will contain the
    * Flow Control Mode Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * Flow_Control_ModeResult variable will contain the Flow Control
    * Mode Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Flow_Control_Mode(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Flow_Control_ModeResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Flow_Control_Mode_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Flow_Control_ModeResult);
#endif

   /*! @brief The following function issues the HCI_Write_Flow_Control_Mode
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Flow_Control_Mode Flow Control Mode to write to Flow_Control_Mode
    * configuration parameter.
    * @param StatusResult If function returns zero (success) this variable will
    * contain the Status Result returned from the Bluetooth device
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Flow_Control_Mode(unsigned int BluetoothStackID, Byte_t Flow_Control_Mode, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Flow_Control_Mode_t)(unsigned int BluetoothStackID, Byte_t Flow_Control_Mode, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the
    * HCI_Read_Enhanced_Transmit_Power_Level Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param Connection_Handle Connection handle used to identify the connection to
    * be used, must be a Connection_Handle for an ACL connection.
    * @param StatusResult If function returns zero (success) this variable will
    * contain the Status Result returned from the Bluetooth device
    * @param Connection_HandleResult If function returns zero (success) the
    * variable pointed to by this parameter will contain the Connection Handle
    * Result returned from the Bluetooth device.
    * @param Transmit_Power_Level_GFSKResult If function returns zero (success)
    * the variable pointed to by this parameter will contain the GFSK Transmit
    * Power level returned from the Bluetooth device
    * @param Transmit_Power_Level_DQPSKResult If function returns zero (success)
    * the variable pointed to by this parameter will contain DQPSK Transmit Power
    * level returned from the Bluetooth device.
    * @param Transmit_Power_Level_8DPSKResult If function returns zero (success)
    * the variable pointed to by this parameter will contain the 8DQPSK Transmit
    * Power Level returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if
    * there was an
    * error. If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the Connection_HandleResult Connection
    * Handle Result returned from the Bluetooth Device AND the
    * Transmit_Power_Level_GFSKResult Transmit Power Level (GFSK) Result
    * returned from the Bluetooth Device AND the
    * Transmit_Power_Level_DQPSKResult Transmit Power Level (DQPSK)
    * Result returned from the Bluetooth Device AND the
    * Transmit_Power_Level_8DPSKResult Transmit Power Level (8DPSK)
    * Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Enhanced_Transmit_Power_Level(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult, SByte_t *Transmit_Power_Level_GFSKResult, SByte_t *Transmit_Power_Level_DQPSKResult, SByte_t *Transmit_Power_Level_8DPSKResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Enhanced_Transmit_Power_Level_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult, SByte_t *Transmit_Power_Level_GFSKResult, SByte_t *Transmit_Power_Level_DQPSKResult, SByte_t *Transmit_Power_Level_8DPSKResult);
#endif

   /*! @brief The following function issues the
    * HCI_Read_Best_Effort_Flush_Timeout Command to the Bluetooth Device
    * that is associated with the Bluetooth Protocol Stack specified by
    * the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param Logical_Link_Handle Handle of Logical Link to which the command applies.
    * @param StatusResult If function returns zero (success) this variable will
    * contain the Status Result returned from the Bluetooth device
    * @param Best_Effort_Flush_TimeoutResult If function returns zero (success)
    * the variable pointed to by this parameter will contain the Best Effort
    * Flush Timeout read from the Bluetooth device.
    * @return This function returns zero if
    * successful, or a non-zero value if there was an error.  If this function returns zero (success) then the StatusResult variable
    * will contain the Status Result returned from the Bluetooth Device
    * AND the Best_Effort_Flush_TimeoutResult variable will contain the
    * Best Effort Flush Timeout Result returned from the Bluetooth
    * Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Best_Effort_Flush_Timeout(unsigned int BluetoothStackID, Word_t Logical_Link_Handle, Byte_t *StatusResult, DWord_t *Best_Effort_Flush_TimeoutResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Best_Effort_Flush_Timeout_t)(unsigned int BluetoothStackID, Word_t Logical_Link_Handle, Byte_t *StatusResult, DWord_t *Best_Effort_Flush_TimeoutResult);
#endif

   /*! @brief The following function issues the
    * HCI_Write_Best_Effort_Flush_Timeout Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Logical_Link_Handle Handle of Logical Link to which the command
    * applies.
    * @param Best_Effort_Flush_Timeout Value to write to the Best Effort Flush
    * Timeout of the specified logical link.
    * @param StatusResult If function returns zero (success) this variable will
    * contain the Status Result returned from the Bluetooth device
    * @return This function returns zero if successful, or a non-zero value if
    * there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Best_Effort_Flush_Timeout(unsigned int BluetoothStackID, Word_t Logical_Link_Handle, DWord_t Best_Effort_Flush_Timeout, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Best_Effort_Flush_Timeout_t)(unsigned int BluetoothStackID, Word_t Logical_Link_Handle, DWord_t Best_Effort_Flush_Timeout, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Short_Range_Mode Command to
    * the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Physical_Link_Handle Handle of the physical link to which the
    * command applies.
    * @param Short_Range_Mode Configuration setting of Short Range Mode
    * configuration parameter.
    * @param StatusResult If function returns zero (success) this
    * variable will contain the Status Result returned from the Bluetooth device
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Short_Range_Mode(unsigned int BluetoothStackID, Byte_t Physical_Link_Handle, Byte_t Short_Range_Mode, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Short_Range_Mode_t)(unsigned int BluetoothStackID, Byte_t Physical_Link_Handle, Byte_t Short_Range_Mode, Byte_t *StatusResult);
#endif

   /*! HCI Command API's (Host Controller and Baseband - Version 4.0 + LE). */
   /*! @brief The following function issues the HCI_Read_LE_Host_Supported
    * command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device
    * @param LE_Supported_HostResult If function is successful, this will
    * contain the LE supported host result.
    * @param Simultaneous_LE_HostResult  If function is successful, this
    * will contain the simultaneous LE host result.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * LE_Supported_HostResult variable will contain the LE Supported
    * Host Result returned from the Bluetooth Device AND the
    * Simultaneous_LE_HostResult will contain the Simultaneous LE Host
    * Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_LE_Host_Supported(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *LE_Supported_HostResult, Byte_t *Simultaneous_LE_HostResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_LE_Host_Supported_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *LE_Supported_HostResult, Byte_t *Simultaneous_LE_HostResult);
#endif

   /*! @brief The following function issues the HCI_Read_LE_Write_Supported
    * command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter followed by the Host supported LE parameters.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param LE_Supported_Host Used to set the LE supported feature
    * bit on the host device.
    * @param Simultaneous_LE_Host Used to set the simultaneous
    * LE and BR/EDR to same device capable feature bit on the host device.
    * @param StatusResult If function returns zero (success) this
    * variable will contain the Status Result returned from the Bluetooth device
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_LE_Host_Supported(unsigned int BluetoothStackID, Byte_t LE_Supported_Host, Byte_t Simultaneous_LE_Host, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_LE_Host_Supported_t)(unsigned int BluetoothStackID, Byte_t LE_Supported_Host, Byte_t Simultaneous_LE_Host, Byte_t *StatusResult);
#endif

   /*! HCI Command API's (Host Controller and Baseband - Version 4.0 + CSA3). */
    /*! @brief The following function issues the HCI_Set_MWS_Channel_Parameters
    * command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param MWS_Channel_Enable Used to set the LE supported feature bit on the host device. Possible values are
    * HCI_MWS_CHANNEL_ENABLE_CHANNEL_DISABLE
    * HCI_MWS_CHANNEL_ENABLE_CHANNEL_ENABLED
    * @param MWS_RX_Center_Frequency Used to indicate the center frequency of the MWS device’s uplink (TX) and downlink (RX) channels.
    * @param MWS_TX_Center_Frequency Used to indicate the center frequency of the MWS device’s uplink (TX) and downlink (RX) channels.
    * @param MWS_RX_Channel_Bandwidth Used to indicate the bandwidth, in kHz, of the MWS device’s uplink and downlink channels.
    * @param MWS_TX_Channel_Bandwidth Used to indicate the bandwidth, in kHz, of the MWS device’s uplink and downlink channels.
    * @param MWS_Channel_Type Describes the type of channel.
    * @param StatusResult If function returns zero (success) this variable will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Set_MWS_Channel_Parameters(unsigned int BluetoothStackID, Byte_t MWS_Channel_Enable, Word_t MWS_RX_Center_Frequency, Word_t MWS_TX_Center_Frequency, Word_t MWS_RX_Channel_Bandwidth, Word_t MWS_TX_Channel_Bandwidth, Byte_t MWS_Channel_Type, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Set_MWS_Channel_Parameters_t)(unsigned int BluetoothStackID, Byte_t MWS_Channel_Enable, Word_t MWS_RX_Center_Frequency, Word_t MWS_TX_Center_Frequency, Word_t MWS_RX_Channel_Bandwidth, Word_t MWS_TX_Channel_Bandwidth, Byte_t MWS_Channel_Type, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the
    * HCI_Set_External_Frame_Configuration command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter followed by the Host
    * supported LE parameters.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param Ext_Frame_Duration The sum of all Period_Duration[i] parameters shall be equal to the Ext_Frame_Duration parameter.
    * @param Ext_Frame_Sync_Assert_Offset The start of the external frame structure is defined as an offset from an external frame synchronization signal. This offset is defined by the Ext_Frame_Sync_Assert_Offset parameter.
    * @param Ext_Frame_Sync_Assert_Jitter External frame sync jitter in microseconds.
    * @param Ext_Num_Periods Has arrays of Period_Duration and Period_Type.
    * @param Period_Duration Array of Ext_Num_Periods.
    * @param Period_Type Array of Ext_Num_Periods.
    * HCI_MWS_PERIOD_TYPE_DOWNLINK
    * HCI_MWS_PERIOD_TYPE_UPLINK
    * HCI_MWS_PERIOD_TYPE_BIDIRECTIONAL
    * HCI_MWS_PERIOD_TYPE_GUARD_PERIOD
    * @param StatusResult If function returns zero (success) this variable will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if
    * successful, or a non-zero value if there was an error. If this function returns zero (success) then the StatusResult variable
    * will contain the Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    * \note Both Period_Duration and Period_Type must be arrays of
    *          Ext_Num_Periods entries.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Set_External_Frame_Configuration(unsigned int BluetoothStackID, Word_t Ext_Frame_Duration, Word_t Ext_Frame_Sync_Assert_Offset, Word_t Ext_Frame_Sync_Assert_Jitter, Byte_t Ext_Num_Periods, Word_t *Period_Duration, Byte_t *Period_Type, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Set_External_Frame_Configuration_t)(unsigned int BluetoothStackID, Word_t Ext_Frame_Duration, Word_t Ext_Frame_Sync_Assert_Offset, Word_t Ext_Frame_Sync_Assert_Jitter, Byte_t Ext_Num_Periods, Word_t *Period_Duration, Byte_t *Period_Type, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Set_MWS_Signaling command to
    * the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param Set_MWS_Signaling_Parameters Will contain parameters for MWS_Signaling.
    * @param Set_MWS_Signaling_Result Will contain the return values returned from the Bluetooth device.
    * @param StatusResult If function returns zero (success) this variable will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device and teh Set_MWS_Signaling_Result variable
    * will contain the return values returned from the Bluetooth device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Set_MWS_Signaling(unsigned int BluetoothStackID, HCI_Set_MWS_Signaling_Parameters_t *Set_MWS_Signaling_Parameters, Byte_t *StatusResult, HCI_Set_MWS_Signaling_Result_t *Set_MWS_Signaling_Result);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Set_MWS_Signaling_t)(unsigned int BluetoothStackID, HCI_Set_MWS_Signaling_Parameters_t *Set_MWS_Signaling_Parameters, Byte_t *StatusResult, HCI_Set_MWS_Signaling_Result_t *Set_MWS_Signaling_Result);
#endif

   /*! @brief The following function issues the HCI_Set_MWS_Transport_Layer
    * command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param Transport_Layer Layer through which commands are sent.
    * @param To_MWS_Baud_Rate Baud rate in the Bluetooth to MWS direction in Baud.
    * @param From_MWS_Baud_Rate Baud rate in the MWS to Bluetooth direction in Baud.
    * @param StatusResult If function returns zero (success) this variable will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Set_MWS_Transport_Layer(unsigned int BluetoothStackID, Byte_t Transport_Layer, DWord_t To_MWS_Baud_Rate, DWord_t From_MWS_Baud_Rate, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Set_MWS_Transport_Layer_t)(unsigned int BluetoothStackID, Byte_t Transport_Layer, DWord_t To_MWS_Baud_Rate, DWord_t From_MWS_Baud_Rate, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Set_MWS_Scan_Frequency_Table
    * command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param Num_Scan_Frequencies Both Scan_Frequency_Low and Scan_Frequency_High are arrays of Num_Scan_Frequencies entries.
    * @param Scan_Frequency_Low	Array of Num_Scan_Frequencies.
    * @param Scan_Frequency_High Array of Num_Scan_Frequencies.
    * @param StatusResult If function returns zero (success) this variable will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error. If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    * \note Both Scan_Frequency_Low and Scan_Frequency_High are
    *          arrays of Num_Scan_Frequencies entries.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Set_MWS_Scan_Frequency_Table(unsigned int BluetoothStackID, Byte_t Num_Scan_Frequencies, Word_t *Scan_Frequency_Low, Word_t *Scan_Frequency_High, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Set_MWS_Scan_Frequency_Table_t)(unsigned int BluetoothStackID, Byte_t Num_Scan_Frequencies, Word_t *Scan_Frequency_Low, Word_t *Scan_Frequency_High, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the
    * HCI_Set_MWS_PATTERN_Configuration command to the Bluetooth Device
    * that is associated with the Bluetooth Protocol Stack specified by
    * the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param MWS_PATTERN_Index Index of the MWS_PATTERN instance to be configured.Range is 0-2.
    * @param MWS_PATTERN_NumIntervals Both MWS_PATTERN_IntervalDuration and MWS_PATTERN_IntervalType are arrays of MWS_PATTERN_NumIntervals entries.
    * @param MWS_PATTERN_IntervalDuration Array of MWS_PATTERN_NumIntervals.
    * @param MWS_PATTERN_IntervalType Array of MWS_PATTERN_NumIntervals
    * HCI_MWS_PATTERN_INTERVAL_TYPE_TX_RX_NOT_ALLOWED
    * HCI_MWS_PATTERN_INTERVAL_TYPE_TX_ALLOWED
    * HCI_MWS_PATTERN_INTERVAL_TYPE_RX_ALLOWED
    * HCI_MWS_PATTERN_INTERVAL_TYPE_TX_RX_ALLOWED
    * HCI_MWS_PATTERN_INTERVAL_TYPE_INTERVAL_FOR_MWS_FRAME
    * @param StatusResult If function returns zero (success) this variable will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if
    * successful, or a non-zero value if there was an error.
    * If this function returns zero (success) then the StatusResult variable
    * will contain the Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    * \note Both MWS_PATTERN_IntervalDuration and
    *          MWS_PATTERN_IntervalType are arrays of
    *          MWS_PATTERN_NumIntervals entries.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Set_MWS_PATTERN_Configuration(unsigned int BluetoothStackID, Byte_t MWS_PATTERN_Index, Byte_t MWS_PATTERN_NumIntervals, Word_t *MWS_PATTERN_IntervalDuration, Byte_t *MWS_PATTERN_IntervalType, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Set_MWS_PATTERN_Configuration_t)(unsigned int BluetoothStackID, Byte_t MWS_PATTERN_Index, Byte_t MWS_PATTERN_NumIntervals, Word_t *MWS_PATTERN_IntervalDuration, Byte_t *MWS_PATTERN_IntervalType, Byte_t *StatusResult);
#endif

   /* HCI Command API's (Host Controller and Baseband - Version 4.0 +
    * CSA4).
    * @brief The following function issues the HCI_Set_Reserved_LT_ADDR Command
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param LT_ADDR Allows the Host to request that theBR/EDR Controller reserve a specific LT_ADDR for Connectionless Slave Broadcast.
    * @param LT_ADDRResult It contains the LT_ADDR returned from the Bluetooth Device.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error. If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * LT_ADDRResult variable will contain the LT_ADDR returned from the
    * Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Set_Reserved_LT_ADDR(unsigned int BluetoothStackID, Byte_t LT_ADDR, Byte_t *StatusResult, Byte_t *LT_ADDRResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Set_Reserved_LT_ADDR_t)(unsigned int BluetoothStackID, Byte_t LT_ADDR, Byte_t *StatusResult, Byte_t *LT_ADDRResult);
#endif

   /*! @brief The following function issues the HCI_Delete_Reserved_LT_ADDR
    * Command Command to the Bluetooth Device that is associated with
    * the Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param LT_ADDR Allows the Host to request that theBR/EDR Controller reserve a specific LT_ADDR for Connectionless Slave Broadcast.
    * @param StatusResult	If function returns zero (success) this variable will contain the Status Result returned from the Bluetooth device.
    * @param LT_ADDRResult It contains the LT_ADDR returned from the Bluetooth Device.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * LT_ADDRResult variable will contain the LT_ADDR returned from the
    * Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Delete_Reserved_LT_ADDR(unsigned int BluetoothStackID, Byte_t LT_ADDR, Byte_t *StatusResult, Byte_t *LT_ADDRResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Delete_Reserved_LT_ADDR_t)(unsigned int BluetoothStackID, Byte_t LT_ADDR, Byte_t *StatusResult, Byte_t *LT_ADDRResult);
#endif

   /*! @brief The following function issues the
    * HCI_Set_Connectionless_Slave_Broadcast_Data Command Command to the
    * Bluetooth Device that is associated with the Bluetooth Protocol
    * Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param LT_ADDR Allows the Host to request that theBR/EDR Controller reserve a specific LT_ADDR for Connectionless Slave Broadcast.
    * @param LT_ADDRResult It contains the LT_ADDR returned from the Bluetooth Device.
    * @param Fragment Part of the data.
    * @param Data_Length Length of the data.
    * @param Data Data to be send.
    * @param StatusResult If function returns zero (success) this variable will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error. If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the LT_ADDRResult variable will contain
    * the LT_ADDR returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Set_Connectionless_Slave_Broadcast_Data(unsigned int BluetoothStackID, Byte_t LT_ADDR, Byte_t Fragment, Byte_t Data_Length, Byte_t *Data, Byte_t *StatusResult, Byte_t *LT_ADDRResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Set_Connectionless_Slave_Broadcast_Data_t)(unsigned int BluetoothStackID, Byte_t LT_ADDR, Byte_t Fragment, Byte_t Data_Length, Byte_t *Data, Byte_t *StatusResult, Byte_t *LT_ADDRResult);
#endif

   /*! @brief The following function issues the
    * HCI_Read_Synchronization_Train_Parameters Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param Sync_Train_IntervalResult Will contain the Synchronization Train Interval returned from the Bluetooth Device.
    * @param Sync_Train_TimeoutResult Will contain the Synchronization Train Timeout returned from the Bluetooth Device.
    * @param Service_DataResult	Will contain the Service Data returned from the Bluetooth Device.
    * @param StatusResult If function returns zero (success) this variable will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the Sync_Train_IntervalResult variable
    * will contain the Synchronization Train Interval returned from the
    * Bluetooth Device AND the Sync_Train_TimeoutResult variable will
    * contain the Synchronization Train Timeout returned from the
    * Bluetooth Device AND the Service_DataResult variable will contain
    * the Service Data returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Synchronization_Train_Parameters(unsigned int BluetoothStackID, Byte_t *StatusResult, Word_t *Sync_Train_IntervalResult, DWord_t *Sync_Train_TimeoutResult, Byte_t *Service_DataResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Synchronization_Train_Parameters_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Word_t *Sync_Train_IntervalResult, DWord_t *Sync_Train_TimeoutResult, Byte_t *Service_DataResult);
#endif

   /*! @brief The following function issues the
    * HCI_Write_Synchronization_Train_Parameters Command Command to the
    * Bluetooth Device that is associated with the Bluetooth Protocol
    * Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param Sync_Train_IntervalResult will contain the Synchronization Train Interval returned from the Bluetooth Device.
    * @param Sync_Train_Timeout will contain the Synchronization Train Timeout returned from the Bluetooth Device.
    * @param Interval_Min Contains the minimum connection’s interval.
    * @param Interval_Max Contains the maximum connection’s interval.
    * @param Service_Data Contains the service data.
    * @param StatusResult If function returns zero (success) this variable will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error. If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the Sync_Train_IntervalResult variable
    * will contain the Synchronization Train Interval returned from the
    * Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Synchronization_Train_Parameters(unsigned int BluetoothStackID, Word_t Interval_Min, Word_t Interval_Max, DWord_t Sync_Train_Timeout, Byte_t Service_Data, Byte_t *StatusResult, Word_t *Sync_Train_IntervalResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Synchronization_Train_Parameters_t)(unsigned int BluetoothStackID, Word_t Interval_Min, Word_t Interval_Max, DWord_t Sync_Train_Timeout, Byte_t Service_Data, Byte_t *StatusResult, Word_t *Sync_Train_IntervalResult);
#endif

   /* HCI Command API's (Host Controller and Baseband - Version 4.1).
    * @brief The following function issues the
    * HCI_Read_Secure_Connections_Host_Support Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param Secure_Connections_Host_SupportResult will contain the Secure Connections Host Support returned from the Bluetooth Device.
    * @param StatusResult If function returns zero (success) this variable will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If @return This function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the Secure_Connections_Host_SupportResult
    * variable will contain the Secure Connections Host Support returned
    * from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Secure_Connections_Host_Support(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Secure_Connections_Host_SupportResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Secure_Connections_Host_Support_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Secure_Connections_Host_SupportResult);
#endif

   /*! @brief The following function issues the
    * HCI_Write_Secure_Connections_Host_Support Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param Secure_Connections_Host_Support Returned from the Bluetooth device.
    * @param StatusResult If function returns zero (success) this variable will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error. If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the Secure_Connections_Host_SupportResult
    * variable will contain the Secure Connections Host Support returned
    * from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Secure_Connections_Host_Support(unsigned int BluetoothStackID, Byte_t Secure_Connections_Host_Support, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Secure_Connections_Host_Support_t)(unsigned int BluetoothStackID, Byte_t Secure_Connections_Host_Support, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the
    * HCI_Read_Authenticated_Payload_Timeout Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param Connection_Handle Returned from the Bluetooth device.
    * @param Connection_HandleResult will contain the Connection Handle returned from the Bluetooth Device.
    * @param Authenticated_Payload_TimeoutResult will contain the Authenticated Payload Timeout returned from the Bluetooth Device.
    * @param StatusResult If function returns zero (success) this variable will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error. If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the Connection_HandleResult variable will
    * contain the Connection Handle returned from the Bluetooth Device
    * AND the Authenticated_Payload_TimeoutResult variable will contain
    * the Authenticated Payload Timeout returned from the Bluetooth
    * Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Authenticated_Payload_Timeout(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult, Word_t *Authenticated_Payload_TimeoutResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Authenticated_Payload_Timeout_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult, Word_t *Authenticated_Payload_TimeoutResult);
#endif

   /*! @brief The following function issues the
    * HCI_Write_Authenticated_Payload_Timeout Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param Connection_Handle Returned from the Bluetooth device.
    * @param Connection_HandleResult Will contain the Connection Handle returned from the Bluetooth Device.
    * @param Authenticated_Payload_Timeout Returned from the Bluetooth Device.
    * @param StatusResult If function returns zero (success) this variable will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the Connection_HandleResult variable will
    * contain the Connection Handle returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Authenticated_Payload_Timeout(unsigned int BluetoothStackID, Word_t Connection_Handle, Word_t Authenticated_Payload_Timeout, Byte_t *StatusResult, Word_t *Connection_HandleResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Authenticated_Payload_Timeout_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Word_t Authenticated_Payload_Timeout, Byte_t *StatusResult, Word_t *Connection_HandleResult);
#endif

   /*! @brief The following function issues the HCI_Read_Local_OOB_Extended_Data
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param C_192Result Simple Pairing Hash C derived from the P-192 public key.
    * @param R_192Result Simple Pairing Randomizer associated with the P-192 public key.
    * @param C_256Result Simple Pairing Hash C derived from the P-256 public key.
    * @param R_256Result Simple Pairing Randomizer associated with the P-256 public key.
    * @param StatusResult If function returns zero (success) this variable will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * C_192Result variable will contain the Simple Pairing Hash derived
    * from the P-192 public key returned from the Bluetooth Device AND
    * the R_192Result variable will contain the Simple Pairing
    * Randomizer associated with the P-192 public key returned from the
    * Bluetooth Device AND the C_256Result variable will contain the
    * Simple Pairing Hash derived from the P-256 public key returned
    * from the Bluetooth Device AND the R_256Result variable will
    * contain the Simple Pairing Randomizer associated with the P-256
    * public key returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Local_OOB_Extended_Data(unsigned int BluetoothStackID, Byte_t *StatusResult, Simple_Pairing_Hash_t *C_192Result, Simple_Pairing_Randomizer_t *R_192Result, Simple_Pairing_Hash_t *C_256Result, Simple_Pairing_Randomizer_t *R_256Result);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Local_OOB_Extended_Data_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Simple_Pairing_Hash_t *C_192Result, Simple_Pairing_Randomizer_t *R_192Result, Simple_Pairing_Hash_t *C_256Result, Simple_Pairing_Randomizer_t *R_256Result);
#endif

   /*! @brief The following function issues the HCI_Read_Extended_Page_Timeout
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param StatusResult If function returns zero (success) this variable will contain the Status Result returned from the Bluetooth device.
    * @param Extended_Page_TimeoutResult will contain the Extended Page Timeout returned from the Bluetooth Device.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If @return This function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * Extended_Page_TimeoutResult variable will contain the Extended
    * Page Timeout returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Extended_Page_Timeout(unsigned int BluetoothStackID, Byte_t *StatusResult, Word_t *Extended_Page_TimeoutResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Extended_Page_Timeout_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Word_t *Extended_Page_TimeoutResult);
#endif

   /*! @brief The following function issues the HCI_Write_Extended_Page_Timeout
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param Extended_Page_Timeout will contain the Extended Page Timeout value.
    * @param StatusResult If function returns zero (success) this variable will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If @return This function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Extended_Page_Timeout(unsigned int BluetoothStackID, Word_t Extended_Page_Timeout, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Extended_Page_Timeout_t)(unsigned int BluetoothStackID, Word_t Extended_Page_Timeout, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_Read_Extended_Inquiry_Length
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param Extended_Inquiry_LengthResult Will contain the Extended Inquiry Length returned from the Bluetooth Device.
    * @param StatusResult If function returns zero (success) this variable will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If @return This function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * Extended_Inquiry_LengthResult variable will contain the Extended
    * Inquiry Length returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Extended_Inquiry_Length(unsigned int BluetoothStackID, Byte_t *StatusResult, Word_t *Extended_Inquiry_LengthResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Extended_Inquiry_Length_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Word_t *Extended_Inquiry_LengthResult);
#endif

   /*! @brief The following function issues the
    * HCI_Write_Extended_Inquiry_Length Command to the Bluetooth Device
    * that is associated with the Bluetooth Protocol Stack specified by
    * the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param Extended_Inquiry_Length will contain the Extended Inquiry Length returned from the Bluetooth Device.
    * @param StatusResult If function returns zero (success) this variable will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if
    * successful, or a non-zero value if there was an error.
    * If this function returns zero (success) then the StatusResult variable
    * will contain the Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Extended_Inquiry_Length(unsigned int BluetoothStackID, Word_t Extended_Inquiry_Length, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Extended_Inquiry_Length_t)(unsigned int BluetoothStackID, Word_t Extended_Inquiry_Length, Byte_t *StatusResult);
#endif

   /*! HCI Command API's (Informational Parameters). */

    /*! @brief The following function issues the
    * HCI_Read_Local_Version_Information Command to the Bluetooth Device
    * that is associated with the Bluetooth Protocol Stack specified by
    * the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param StatusResult Returned HCI status code.
    * @param HCI_VersionResult Major version for the Bluetooth hardware. Corresponds to changes in the released specifications only. Possible values are:
    * HCI_VERSION_SPECIFICATION_1_0B
    * HCI_VERSION_SPECIFICATION_1_1
    * HCI_VERSION_SPECIFICATION_1_2
    * HCI_VERSION_SPECIFICATION_2_0
    * HCI_VERSION_SPECIFICATION_2_1
    * HCI_VERSION_SPECIFICATION_3_0
    * HCI_VERSION_SPECIFICATION_4_0
    * @param HCI_RevisionResult The HCI revision number.
    * @param LMP_VersionResult The Link Manager Protocol version number. Possible values are:
    * HCI_LMP_VERSION_BLUETOOTH_1_0
    * HCI_LMP_VERSION_BLUETOOTH_1_1
    * HCI_LMP_VERSION_BLUETOOTH_1_2
    * HCI_LMP_VERSION_BLUETOOTH_2_0
    * HCI_LMP_VERSION_BLUETOOTH_2_1
    * HCI_LMP_VERSION_BLUETOOTH_3_0
    * HCI_LMP_VERSION_BLUETOOTH_4_0
    * @param Manufacturer_NameResult Manufacturer code. Possible values are:
    * HCI_LMP_COMPID_MANUFACTURER_NAME_ERICSSON_MOBILE_COMMUNICATIONS
    * HCI_LMP_COMPID_MANUFACTURER_NAME_NOKIA_MOBILE_PHONES
    * HCI_LMP_COMPID_MANUFACTURER_NAME_INTEL_CORPORATION
    * HCI_LMP_COMPID_MANUFACTURER_NAME_IBM_CORPORATION
    * HCI_LMP_COMPID_MANUFACTURER_NAME_TOSHIBA_CORPORATION
    * HCI_LMP_COMPID_MANUFACTURER_NAME_3COM
    * HCI_LMP_COMPID_MANUFACTURER_NAME_MICROSOFT
    * HCI_LMP_COMPID_MANUFACTURER_NAME_LUCENT
    * HCI_LMP_COMPID_MANUFACTURER_NAME_MOTOROLA
    * HCI_LMP_COMPID_MANUFACTURER_NAME_INFINEON_TECHNOLOGIES_AG
    * HCI_LMP_COMPID_MANUFACTURER_NAME_CAMBRIDGE_SILICON_RADIO
    * HCI_LMP_COMPID_MANUFACTURER_NAME_SILICON_WAVE
    * HCI_LMP_COMPID_MANUFACTURER_NAME_DIGIANSWER
    * HCI_LMP_COMPID_MANUFACTURER_NAME_TEXAS_INSTRUMENTS
    * HCI_LMP_COMPID_MANUFACTURER_NAME_PARTHUS_TECHNOLOGIES
    * HCI_LMP_COMPID_MANUFACTURER_NAME_BROADCOM
    * HCI_LMP_COMPID_MANUFACTURER_NAME_MITEL_SEMICONDUCTOR
    * HCI_LMP_COMPID_MANUFACTURER_NAME_WIDCOMM
    * HCI_LMP_COMPID_MANUFACTURER_NAME_TELENCOMM
    * HCI_LMP_COMPID_MANUFACTURER_NAME_ATMEL
    * HCI_LMP_COMPID_MANUFACTURER_NAME_MITSUBISHI
    * HCI_LMP_COMPID_MANUFACTURER_NAME_RTX_TELECOM
    * HCI_LMP_COMPID_MANUFACTURER_NAME_KC_TECHNOLOGY
    * HCI_LMP_COMPID_MANUFACTURER_NAME_NEWLOGIC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_TRANSILICA
    * HCI_LMP_COMPID_MANUFACTURER_NAME_ROHDE_AND_SCHWARTZ
    * HCI_LMP_COMPID_MANUFACTURER_NAME_TTPCOM
    * HCI_LMP_COMPID_MANUFACTURER_NAME_SIGNIA_TECHNOLOGIES
    * HCI_LMP_COMPID_MANUFACTURER_NAME_CONEXANT_SYSTEMS
    * HCI_LMP_COMPID_MANUFACTURER_NAME_QUALCOMM
    * HCI_LMP_COMPID_MANUFACTURER_NAME_INVENTEL
    * HCI_LMP_COMPID_MANUFACTURER_NAME_AVM_BERLIN
    * HCI_LMP_COMPID_MANUFACTURER_NAME_BANDSPEED
    * HCI_LMP_COMPID_MANUFACTURER_NAME_MANSELLA
    * HCI_LMP_COMPID_MANUFACTURER_NAME_NEC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_WAVEPLUS_TECHNOLOGY
    * HCI_LMP_COMPID_MANUFACTURER_NAME_ALCATEL
    * HCI_LMP_COMPID_MANUFACTURER_NAME_PHILIPS_SEMICONDUCTORS
    * HCI_LMP_COMPID_MANUFACTURER_NAME_C_TECHNOLOGIES
    * HCI_LMP_COMPID_MANUFACTURER_NAME_OPEN_INTERFACE
    * HCI_LMP_COMPID_MANUFACTURER_NAME_RF_MICRO_DEVICES
    * HCI_LMP_COMPID_MANUFACTURER_NAME_HITACHI
    * HCI_LMP_COMPID_MANUFACTURER_NAME_SYMBOL_TECHNOLOGIES
    * HCI_LMP_COMPID_MANUFACTURER_NAME_TENOVIS
    * HCI_LMP_COMPID_MANUFACTURER_NAME_MACRONIX_INTERNATIONAL
    * HCI_LMP_COMPID_MANUFACTURER_NAME_GCT_SEMICONDUCTOR
    * HCI_LMP_COMPID_MANUFACTURER_NAME_NORWOOD_SYSTEMS
    * HCI_LMP_COMPID_MANUFACTURER_NAME_MEWTEL_TECHNOLOGY
    * HCI_LMP_COMPID_MANUFACTURER_NAME_ST_MICROELECTRONICS
    * HCI_LMP_COMPID_MANUFACTURER_NAME_SYNOPSYS
    * HCI_LMP_COMPID_MANUFACTURER_NAME_RED_M_COMMUNICATIONS
    * HCI_LMP_COMPID_MANUFACTURER_NAME_COMMIL_LTD
    * HCI_LMP_COMPID_MANUFACTURER_NAME_CATC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_ECLIPSE_SL
    * HCI_LMP_COMPID_MANUFACTURER_NAME_RENESAS_TECHNOLOGY_CORP
    * HCI_LMP_COMPID_MANUFACTURER_NAME_MOBILIAN_CORPORATION
    * HCI_LMP_COMPID_MANUFACTURER_NAME_TERAX
    * HCI_LMP_COMPID_MANUFACTURER_NAME_INTEGRATED_SYSTEM_SOLUTION
    * HCI_LMP_COMPID_MANUFACTURER_NAME_MATSUSHITA
    * HCI_LMP_COMPID_MANUFACTURER_NAME_GENNUM_CORPORATION
    * HCI_LMP_COMPID_MANUFACTURER_NAME_RESEARCH_IN_MOTION
    * HCI_LMP_COMPID_MANUFACTURER_NAME_IPEXTREME
    * HCI_LMP_COMPID_MANUFACTURER_NAME_SYSTEMS_AND_CHIPS
    * HCI_LMP_COMPID_MANUFACTURER_NAME_BLUETOOTH_SIG
    * HCI_LMP_COMPID_MANUFACTURER_NAME_SEIKO_EPSON_CORPORATION
    * HCI_LMP_COMPID_MANUFACTURER_NAME_INTEGRATED_SILICON_SOLUTION
    * HCI_LMP_COMPID_MANUFACTURER_NAME_CONWISE_TECHNOLOGY_CORPORATION
    * HCI_LMP_COMPID_MANUFACTURER_NAME_PARROT_SA
    * HCI_LMP_COMPID_MANUFACTURER_NAME_SOCKET_MOBILE
    * HCI_LMP_COMPID_MANUFACTURER_NAME_ATHEROS_COMMUNICATIONS
    * HCI_LMP_COMPID_MANUFACTURER_NAME_MEDIATEK_INCORPORATED
    * HCI_LMP_COMPID_MANUFACTURER_NAME_BLUEGIGA
    * HCI_LMP_COMPID_MANUFACTURER_NAME_MARVELL_TECHNOLOGY_GROUP
    * HCI_LMP_COMPID_MANUFACTURER_NAME_3DSP_CORPORATION
    * HCI_LMP_COMPID_MANUFACTURER_NAME_ACCEL_SEMICONDUCTOR
    * HCI_LMP_COMPID_MANUFACTURER_NAME_CONTINENTAL_AUTOMOTIVE_SYSTEMS
    * HCI_LMP_COMPID_MANUFACTURER_NAME_APPLE_INCORPORATED
    * HCI_LMP_COMPID_MANUFACTURER_NAME_STACCATO_COMMUNICATIONS
    * HCI_LMP_COMPID_MANUFACTURER_NAME_AVAGO_TECHONOLOGIES
    * HCI_LMP_COMPID_MANUFACTURER_NAME_APT_LIMITED
    * HCI_LMP_COMPID_MANUFACTURER_NAME_SIRF_TECHONOLIGY
    * HCI_LMP_COMPID_MANUFACTURER_NAME_TZERO_TECHNOLOGIES
    * HCI_LMP_COMPID_MANUFACTURER_NAME_J_AND_M_CORPORATION
    * HCI_LMP_COMPID_MANUFACTURER_NAME_FREE2MOVE_AB
    * HCI_LMP_COMPID_MANUFACTURER_NAME_3DIJOY_CORPORATION
    * HCI_LMP_COMPID_MANUFACTURER_NAME_PLANTRONICS_INCORPORATED
    * HCI_LMP_COMPID_MANUFACTURER_NAME_SONY_ERICSSON_MOBILE_COMM
    * HCI_LMP_COMPID_MANUFACTURER_NAME_HARMAN_INTERNATIONAL_IND
    * HCI_LMP_COMPID_MANUFACTURER_NAME_VIZIO_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_NORDIC_SEMICONDUCTOR_ASA
    * HCI_LMP_COMPID_MANUFACTURER_NAME_EM_MICROELECTRONIC_MARIN_SA
    * HCI_LMP_COMPID_MANUFACTURER_NAME_RALINK_TECHNOLOGY_CORPORATION
    * HCI_LMP_COMPID_MANUFACTURER_NAME_BELKIN_INTERNATIONAL_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_REALTEK_SEMICONDUCTOR_CORP
    * HCI_LMP_COMPID_MANUFACTURER_NAME_STONESTREET_ONE_LLC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_WICENTRIC_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_RIVIERA_WAVES_SAS
    * HCI_LMP_COMPID_MANUFACTURER_NAME_RDA_MICROELECTRONICS
    * HCI_LMP_COMPID_MANUFACTURER_NAME_GIBSON_GUITARS
    * HCI_LMP_COMPID_MANUFACTURER_NAME_MICOMMAND_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_BAND_XI_INTERNATIONAL_LLC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_HEWLETT_PACKARD_COMPANY
    * HCI_LMP_COMPID_MANUFACTURER_NAME_9SOLUTIONS_OY
    * HCI_LMP_COMPID_MANUFACTURER_NAME_GN_NETCOM_AS
    * HCI_LMP_COMPID_MANUFACTURER_NAME_GENERAL_MOTORS
    * HCI_LMP_COMPID_MANUFACTURER_NAME_A_AND_D_ENGINEERING_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_MINDTREE_LTD
    * HCI_LMP_COMPID_MANUFACTURER_NAME_POLAR_ELECTRO_OY
    * HCI_LMP_COMPID_MANUFACTURER_NAME_BEAUTIFUL_ENTERPRISE_COMPANY
    * HCI_LMP_COMPID_MANUFACTURER_NAME_BRIARTEK_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_SUMMIT_DATA_COMMUNICATIONS_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_SOUND_ID
    * HCI_LMP_COMPID_MANUFACTURER_NAME_MONSTER_LLC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_CONNECT_BLUE_AB
    * HCI_LMP_COMPID_MANUFACTURER_NAME_SHANGHAI_SUPER_SMART_ELECTRON
    * HCI_LMP_COMPID_MANUFACTURER_NAME_GROUP_SENSE_LTD
    * HCI_LMP_COMPID_MANUFACTURER_NAME_ZOMM_LLC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_SAMSUNG_ELECTRONICS_CO_LTD
    * HCI_LMP_COMPID_MANUFACTURER_NAME_CREATIVE_TECHNOLOGY_LTD
    * HCI_LMP_COMPID_MANUFACTURER_NAME_LAIRD_TECHNOLOGIES
    * HCI_LMP_COMPID_MANUFACTURER_NAME_NIKE_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_LESSWIRE_AG
    * HCI_LMP_COMPID_MANUFACTURER_NAME_MSTAR_SEMICONDUCTOR_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_HANLYNN_TECHNOLOGIES
    * HCI_LMP_COMPID_MANUFACTURER_NAME_A_AND_R_CAMBRIDGE
    * HCI_LMP_COMPID_MANUFACTURER_NAME_SEERS_TECHNOLOGY_CO_LTD
    * HCI_LMP_COMPID_MANUFACTURER_NAME_SPORTS_TRACKING_TECHNOLOGIES
    * HCI_LMP_COMPID_MANUFACTURER_NAME_AUTONET_MOBILE
    * HCI_LMP_COMPID_MANUFACTURER_NAME_DELORME_PUBLISHING_COMPANY_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_WUXI_VIMICRO
    * HCI_LMP_COMPID_MANUFACTURER_NAME_SENNHEISER_COMMUNICATIONS_AS
    * HCI_LMP_COMPID_MANUFACTURER_NAME_TIMEKEEPING_SYSTEMS_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_LUDUS_HELSINKI_LTD
    * HCI_LMP_COMPID_MANUFACTURER_NAME_BLUERADIOS_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_EQUINUX_AG
    * HCI_LMP_COMPID_MANUFACTURER_NAME_GARMIN_INTERNATIONAL_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_ECOTEST
    * HCI_LMP_COMPID_MANUFACTURER_NAME_GN_RESOUND_AS
    * HCI_LMP_COMPID_MANUFACTURER_NAME_JAWBONE
    * HCI_LMP_COMPID_MANUFACTURER_NAME_TOPCON_POSITIONING_SYSTEMS_LLC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_QUALCOMM_LABS_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_ZSCAN_SOFTWARE
    * HCI_LMP_COMPID_MANUFACTURER_NAME_QUINTIC_CORP
    * HCI_LMP_COMPID_MANUFACTURER_NAME_STOLLMANN_E_V_GMBH
    * HCI_LMP_COMPID_MANUFACTURER_NAME_FUNAI_ELECTRIC_COMPANY_LTD
    * HCI_LMP_COMPID_MANUFACTURER_NAME_ADVANCED_PANMOBIL_SYSTEMS_GMBH
    * HCI_LMP_COMPID_MANUFACTURER_NAME_THINKOPTICS_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_UNIVERSAL_ELECTRONICS_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_AIROHA_TECHNOLOGY_CORP
    * HCI_LMP_COMPID_MANUFACTURER_NAME_NEC_LIGHTING_LTD
    * HCI_LMP_COMPID_MANUFACTURER_NAME_ODM_TECHNOLOGY_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_BLUETREK_TECHNOLOGIES_LIMITED
    * HCI_LMP_COMPID_MANUFACTURER_NAME_ZERO_1_TV_GMBH
    * HCI_LMP_COMPID_MANUFACTURER_NAME_I_TECH_DYNAMIC_GLOBAL_DIST_LTD
    * HCI_LMP_COMPID_MANUFACTURER_NAME_ALPWISE
    * HCI_LMP_COMPID_MANUFACTURER_NAME_JIANGSU_TOPPOWER_AUTOMOTIVE
    * HCI_LMP_COMPID_MANUFACTURER_NAME_COLORFY_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_GEOFORCE_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_BOSE_CORPORATION
    * HCI_LMP_COMPID_MANUFACTURER_NAME_SUUNTO_OY
    * HCI_LMP_COMPID_MANUFACTURER_NAME_KENSINGTON_COMPUTER_PROD_GROUP
    * HCI_LMP_COMPID_MANUFACTURER_NAME_SR_MEDIZINELEKTRONIK
    * HCI_LMP_COMPID_MANUFACTURER_NAME_VERTU_CORPORATION_LIMITED
    * HCI_LMP_COMPID_MANUFACTURER_NAME_META_WATCH_LTD
    * HCI_LMP_COMPID_MANUFACTURER_NAME_LINAK_A_S
    * HCI_LMP_COMPID_MANUFACTURER_NAME_OTL_DYNAMICS_LLC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_PANDA_OCEAN_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_VISTEON_CORPORATION
    * HCI_LMP_COMPID_MANUFACTURER_NAME_ARP_DEVICES_LIMITED
    * HCI_LMP_COMPID_MANUFACTURER_NAME_MAGNETI_MARELLI_S_P_A
    * HCI_LMP_COMPID_MANUFACTURER_NAME_CAEN_RFID_SRL
    * HCI_LMP_COMPID_MANUFACTURER_NAME_INGENIEUR_SYSTEMGRUPPE_ZAHN
    * HCI_LMP_COMPID_MANUFACTURER_NAME_GREEN_THROTTLE_GAMES
    * HCI_LMP_COMPID_MANUFACTURER_NAME_PETER SYSTEMTECHNIK_GMBH
    * HCI_LMP_COMPID_MANUFACTURER_NAME_OMEGAWAVE_OY
    * HCI_LMP_COMPID_MANUFACTURER_NAME_CINETIX
    * HCI_LMP_COMPID_MANUFACTURER_NAME_PASSIF_SEMICONDUCTOR_CORP
    * HCI_LMP_COMPID_MANUFACTURER_NAME_SARIS_CYCLING_GROUP_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_BEKEY_AS
    * HCI_LMP_COMPID_MANUFACTURER_NAME_CLARINOX_TECHNOLOGIES_PTY_LTD
    * HCI_LMP_COMPID_MANUFACTURER_NAME_BDE_TECHNOLOGY_CO_LTD
    * HCI_LMP_COMPID_MANUFACTURER_NAME_SWIRL_NETWORKS
    * HCI_LMP_COMPID_MANUFACTURER_NAME_MESO_INTERNATIONAL
    * HCI_LMP_COMPID_MANUFACTURER_NAME_TRELAB_LTD
    * HCI_LMP_COMPID_MANUFACTURER_NAME_QUALCOMM_INNOVATION_CENTER_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_JOHNSON_CONTROLS_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_STARKEY_LABORATORIES_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_S_POWER_ELECTRONICS_LIMITED
    * HCI_LMP_COMPID_MANUFACTURER_NAME_ACE_SENSOR_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_APLIX_CORPORATION
    * HCI_LMP_COMPID_MANUFACTURER_NAME_AAMP_OF_AMERICA
    * HCI_LMP_COMPID_MANUFACTURER_NAME_STALMART_TECHNOLOGY_LIMITED
    * HCI_LMP_COMPID_MANUFACTURER_NAME_AMICCOM_ELECTRONICS_CORP
    * HCI_LMP_COMPID_MANUFACTURER_NAME_SHENZHEN_EXCELSECU_DATA_TECH
    * HCI_LMP_COMPID_MANUFACTURER_NAME_GENEQ_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_ADIDAS_AG
    * HCI_LMP_COMPID_MANUFACTURER_NAME_LG_ELECTRONICS
    * HCI_LMP_COMPID_MANUFACTURER_NAME_ONSET_COMPUTER_CORPORATION
    * HCI_LMP_COMPID_MANUFACTURER_NAME_SELFLY_BV
    * HCI_LMP_COMPID_MANUFACTURER_NAME_QUUPPA_OY
    * HCI_LMP_COMPID_MANUFACTURER_NAME_GELO_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_EVLUMA
    * HCI_LMP_COMPID_MANUFACTURER_NAME_MC10
    * HCI_LMP_COMPID_MANUFACTURER_NAME_BINAURIC_SE
    * HCI_LMP_COMPID_MANUFACTURER_NAME_BEATS_ELECTRONICS
    * HCI_LMP_COMPID_MANUFACTURER_NAME_MICROCHIP_TECHNOLOGY_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_ELGATO_SYSTEMS_GMBH
    * HCI_LMP_COMPID_MANUFACTURER_NAME_ARCHOS_SA
    * HCI_LMP_COMPID_MANUFACTURER_NAME_DEXCOM_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_POLAR_ELECTRO_EUROPE_BV
    * HCI_LMP_COMPID_MANUFACTURER_NAME_DIALOG_SEMICONDUCTOR_BV
    * HCI_LMP_COMPID_MANUFACTURER_NAME_TAIXINGBANG_TECHNOLOGY_HK_CO
    * HCI_LMP_COMPID_MANUFACTURER_NAME_KAWANTECH
    * HCI_LMP_COMPID_MANUFACTURER_NAME_AUSTCO_COMMUNICATION_SYSTEMS
    * HCI_LMP_COMPID_MANUFACTURER_NAME_TIMEX_GROUP_USA_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_QUALCOMM_TECHNOLOGIES_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_QUALCOMM_CONNECTED_EXPERIENCES
    * HCI_LMP_COMPID_MANUFACTURER_NAME_VOYETRA_TURTLE_BEACH
    * HCI_LMP_COMPID_MANUFACTURER_NAME_TXTR_GMBH
    * HCI_LMP_COMPID_MANUFACTURER_NAME_BIOSENTRONICS
    * HCI_LMP_COMPID_MANUFACTURER_NAME_PROCTER_AND_GAMBLE
    * HCI_LMP_COMPID_MANUFACTURER_NAME_HOSIDEN_CORPORATION
    * HCI_LMP_COMPID_MANUFACTURER_NAME_MUZIK_LLC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_MISFIT_WEARABLES_CORP
    * HCI_LMP_COMPID_MANUFACTURER_NAME_GOOGLE
    * HCI_LMP_COMPID_MANUFACTURER_NAME_DANLERS_LTD
    * HCI_LMP_COMPID_MANUFACTURER_NAME_SEMILINK_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_INMUSIC_BRANDS_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_LS_RESEARCH_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_EDEN_SOFTWARE_CONSULTANTS_LTD
    * HCI_LMP_COMPID_MANUFACTURER_NAME_FRESHTEMP
    * HCI_LMP_COMPID_MANUFACTURER_NAME_KS_TECHNOLOGIES
    * HCI_LMP_COMPID_MANUFACTURER_NAME_ACTS_TECHNOLOGIES
    * HCI_LMP_COMPID_MANUFACTURER_NAME_VTRACK_SYSTEMS
    * HCI_LMP_COMPID_MANUFACTURER_NAME_NIELSEN_KELLERMAN_COMPANY
    * HCI_LMP_COMPID_MANUFACTURER_NAME_SERVER_TECHNOLOGY_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_BIORESEARCH_ASSOCIATES
    * HCI_LMP_COMPID_MANUFACTURER_NAME_JOLLY_LOGIC_LLC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_ABOVE_AVERAGE_OUTCOMES_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_BITSPLITTERS_GMBH
    * HCI_LMP_COMPID_MANUFACTURER_NAME_PAYPAL_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_WITRON_TECHNOLOGY_LIMITED
    * HCI_LMP_COMPID_MANUFACTURER_NAME_MORSE_PROJECT_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_KENT_DISPLAYS_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_NAUTILUS_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_SMARTIFIER_OY
    * HCI_LMP_COMPID_MANUFACTURER_NAME_ELCOMETER_LIMITED
    * HCI_LMP_COMPID_MANUFACTURER_NAME_VSN_TECHNOLOGIES_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_ACEUNI_CORP_LTD
    * HCI_LMP_COMPID_MANUFACTURER_NAME_STICKNFIND
    * HCI_LMP_COMPID_MANUFACTURER_NAME_CRYSTAL_CODE_AB
    * HCI_LMP_COMPID_MANUFACTURER_NAME_KOUKAAM_AS
    * HCI_LMP_COMPID_MANUFACTURER_NAME_DELPHI_CORPORATION
    * HCI_LMP_COMPID_MANUFACTURER_NAME_VALENCETECH_LIMITED
    * HCI_LMP_COMPID_MANUFACTURER_NAME_RESERVED
    * HCI_LMP_COMPID_MANUFACTURER_NAME_TYPO_PRODUCTS_LLC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_TOMTOM_INTERNATIONAL_BV
    * HCI_LMP_COMPID_MANUFACTURER_NAME_FUGOO_INC
    * HCI_LMP_COMPID_MANUFACTURER_NAME_KEISER_CORPORATION
    * HCI_LMP_COMPID_MANUFACTURER_NAME_BANG_AND_OLUFSEN_AS
    * HCI_LMP_COMPID_MANUFACTURER_NAME_PLUS_LOCATIONS_SYSTEMS_PTY_LTD
    * HCI_LMP_COMPID_MANUFACTURER_NAME_UBIQUITOUS_COMPUTING_TECH_CORP
    * HCI_LMP_COMPID_MANUFACTURER_NAME_INNOVATIVE_YACHTTER_SOLUTIONS
    * @param LMP_SubversionResult The LMP sub-version number. These are defined by each manufacturer.
    * @return This function returns zero if
    * successful, or a non-zero value if there was an error.
    * If this function returns zero (success) then the StatusResult variable
    * will contain the Status Result returned from the Bluetooth Device
    * AND the HCI_VersionResult variable will contain the HCI Version
    * Result returned from the Bluetooth Device AND the
    * HCI_RevisionResult variable will contain the HCI Revision Result
    * returned from the Bluetooth Device AND the LMP_VersionResult
    * variable will contain the LMP Version Result returned from the
    * Bluetooth Device AND the Manufacturer_NameResult variable will
    * contain the Manufacturer Name Result returned from the Bluetooth
    * Device AND the LMP_SubversionResult variable will contain the
    * LMP Subversion Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Local_Version_Information(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *HCI_VersionResult, Word_t *HCI_RevisionResult, Byte_t *LMP_VersionResult, Word_t *Manufacturer_NameResult, Word_t *LMP_SubversionResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Local_Version_Information_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *HCI_VersionResult, Word_t *HCI_RevisionResult, Byte_t *LMP_VersionResult, Word_t *Manufacturer_NameResult, Word_t *LMP_SubversionResult);
#endif

   /*! @brief The following function issues the
    * HCI_Read_Local_Supported_Features Command to the Bluetooth Device
    * that is associated with the Bluetooth Protocol Stack specified by
    * the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @param LMP_FeaturesResult Bit mask list of supported features.
    * @return This function returns zero if
    * successful, or a non-zero value if there was an error.  If this function returns zero (success) then the StatusResult variable
    * will contain the Status Result returned from the Bluetooth Device
    * AND the LMP_FeaturesResult variable will contain the LMP Features
    * Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Local_Supported_Features(unsigned int BluetoothStackID, Byte_t *StatusResult, LMP_Features_t *LMP_FeaturesResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Local_Supported_Features_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, LMP_Features_t *LMP_FeaturesResult);
#endif

   /*! @brief The following function issues the HCI_Read_Buffer_Size Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @param HC_ACL_Data_Packet_Length Maximum length (in bytes) of the data portion
    * of each HCI ACL Data Packet passed to the Host Controller.
    * @param HC_SCO_Data_Packet_Length Maximum length (in bytes) of the data portion
    * of each HCI SCO Data Packet passed to the Host Controller.
    * @param HC_Total_Num_ACL_Data_Packets Maximum number of ACL Data Packets that
    * can be stored in the Host Controller.
    * @param HC_Total_Num_SCO_Data_Packets Maximum number of SCO Data Packets that
    * can be stored in the Host Controller.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error. If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the HC_ACL_Data_Packet_LengthResult
    * variable will contain the HCI ACL Data Packet Length Result
    * returned from the Bluetooth Device AND the
    * HC_SCO_Data_Packet_LengthResult variable will contain the HCI SCO
    * Data Packet Length Result returned from the Bluetooth Device AND
    * the HC_Total_Num_ACL_Data_PacketsResult variable will contain the
    * HCI Total Number ACL Data Packets Result returned from the
    * Bluetooth Device AND the HC_Total_Num_SCO_Data_PacketsResult
    * variable will contain the HCI Total Number SCO Data Packets
    * Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Buffer_Size(unsigned int BluetoothStackID, Byte_t *StatusResult, Word_t *HC_ACL_Data_Packet_Length, Byte_t *HC_SCO_Data_Packet_Length, Word_t *HC_Total_Num_ACL_Data_Packets, Word_t *HC_Total_Num_SCO_Data_Packets);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Buffer_Size_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Word_t *HC_ACL_Data_Packet_Length, Byte_t *HC_SCO_Data_Packet_Length, Word_t *HC_Total_Num_ACL_Data_Packets, Word_t *HC_Total_Num_SCO_Data_Packets);
#endif

   /*! @brief The following function issues the HCI_Read_Country_Code Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @param Country_CodeResult Returned Country Code.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the Country_CodeResult variable will
    * contain the Country Code Result returned from the Bluetooth
    * Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Country_Code(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Country_CodeResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Country_Code_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Country_CodeResult);
#endif

   /*! @brief The following function issues the HCI_Read_BD_ADDR Command to the
    * Bluetooth Device that is associated with the Bluetooth Protocol
    * Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @param BD_ADDRResult The local device’s address/identifier.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error. If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the BD_ADDRResult variable will contain
    * the BD_ADDR Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_BD_ADDR(unsigned int BluetoothStackID, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_BD_ADDR_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, BD_ADDR_t *BD_ADDRResult);
#endif

   /* HCI Command API's (Informational Parameters - Version 1.2).
    * @brief The following function issues the
    * HCI_Read_Local_Supported_Commands Command to the Bluetooth Device
    * that is associated with the Bluetooth Protocol Stack specified by
    * the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @param Supported_CommandsResult Bit mask for each HCI command.
    * @return This function returns zero if
    * successful, or a non-zero value if there was an error.
    * If this function returns zero (success) then the StatusResult variable
    * will contain the Status Result returned from the Bluetooth Device
    * AND the Supported_CommandsResult variable will contain the
    * Supported Commands Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Local_Supported_Commands(unsigned int BluetoothStackID, Byte_t *StatusResult, Supported_Commands_t *Supported_CommandsResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Local_Supported_Commands_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Supported_Commands_t *Supported_CommandsResult);
#endif

   /*! @brief The following function issues the HCI_Read_Local_Extended_Features
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Page_Number Requests the normal LMP features as returned by
    * HCI_Read_Local_Supported_Features (if 0) or the corresponding page of features (non-zero).
    * @param StatusResult Returned HCI status code.
    * @param Page_NumberResult The page number result.
    * @param Maximum_Page_NumberResult The highest features page number which
    * contains non-zero bits for the local device.
    * @param Extended_LMP_FeaturesResult Bit map of requested page of LMP
    * features.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * Page_NumberResult variable will contain the Page Number Result
    * returned from the Bluetooth Device AND the
    * Maximum_Page_NumberResult variable will contain the Maximum Page
    * Number Result returned from the Bluetooth Device AND the
    * Extended_LMP_FeaturesResult variable will contain the Extended LMP
    * Features Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Local_Extended_Features(unsigned int BluetoothStackID, Byte_t Page_Number, Byte_t *StatusResult, Byte_t *Page_NumberResult, Byte_t *Maximum_Page_NumberResult, LMP_Features_t *Extended_LMP_FeaturesResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Local_Extended_Features_t)(unsigned int BluetoothStackID, Byte_t Page_Number, Byte_t *StatusResult, Byte_t *Page_NumberResult, Byte_t *Maximum_Page_NumberResult, LMP_Features_t *Extended_LMP_FeaturesResult);
#endif

   /*! HCI Command API's (Informational Parameters - Version 3.0 + HS). */

   /*! @brief The following function issues the HCI_Read_Data_Block_Size Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param StatusResult If this function returns zero (success)
    * then variable pointed to by StatusResult will contain the status result
    * returned from the Bluetooth device.
    * @param Max_ACL_Data_Packet_LengthResult If this function returns zero
    * (success) then variable pointed to by this parameter will contain the
    * Max ACL Data Packet Length returned from the Bluetooth device.
    * @param Data_Block_LengthResult If this function returns zero
    * (success) then variable pointed to by this parameter will contain the
    * Data Block Length returned from the Bluetooth device.
    * @param Total_Num_Data_BlocksResult If this function returns
    * zero (success) then variable pointed to by this parameter will contain
    * the Total Number Data Blocks returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the Max_ACL_Data_Packet_LengthResult
    * variable will contain the Maximum ACL Data Packet Length Result
    * returned from the Bluetooth Device AND the Data_Block_LengthResult
    * variable will contain the Data Block Length Result returned from
    * the Bluetooth Device AND the Total_Num_Data_BlocksResult variable
    * will contain the Total Number Data Blocks Result returned from the
    * Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Data_Block_Size(unsigned int BluetoothStackID, Byte_t *StatusResult, Word_t *Max_ACL_Data_Packet_LengthResult, Word_t *Data_Block_LengthResult, Word_t *Total_Num_Data_BlocksResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Data_Block_Size_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Word_t *Max_ACL_Data_Packet_LengthResult, Word_t *Data_Block_LengthResult, Word_t *Total_Num_Data_BlocksResult);
#endif

   /*! HCI Command API's (Informational Parameters - Version 4.0 + CSA2). */

   /*! @brief The following function issues the HCI_Read_Local_Supported_Codecs
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param StatusResult If this function returns zero (success)
    * then variable pointed to by StatusResult will contain the status result
    * returned from the Bluetooth device.
    * @param LocalSupportedCodecs LocalSupportedCodecs variable will contain information on the
    * codecs supported by the local Bluetooth controller.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * LocalSupportedCodecs variable will contain information on the
    * codecs supported by the local Bluetooth controller.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    * \note If this function returns successfully then the
    *          HCI_Free_Local_Supported_Codec_Info() MUST be called to
    *          free the information that is returned by this function.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Local_Supported_Codecs(unsigned int BluetoothStackID, Byte_t *StatusResult, HCI_Local_Supported_Codec_Info_t *LocalSupportedCodecs);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Local_Supported_Codecs_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, HCI_Local_Supported_Codec_Info_t *LocalSupportedCodecs);
#endif

   /*! @brief The following function is used to free the Local Supported Codec
    * information returned from a successful call to
    * HCI_Read_Local_Supported_Codecs().
    * @param LocalSupportedCodecs LocalSupportedCodecs variable will contain information on the
    * codecs supported by the local Bluetooth controller.
    */
BTPSAPI_DECLARATION void BTPSAPI HCI_Free_Local_Supported_Codec_Info(HCI_Local_Supported_Codec_Info_t *LocalSupportedCodecs);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (BTPSAPI *PFN_HCI_Free_Local_Supported_Codec_Info_t)(HCI_Local_Supported_Codec_Info_t *LocalSupportedCodecs);
#endif

   /*! HCI Command API's (Status Parameters). */

   /*! @brief The following function issues the HCI_Read_Failed_Contact_Counter
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Connection_Handle Unique identifier for the connection
    * returned in the Connection Complete event associated with the HCI_Create_Connection command.
    * @param StatusResult Returned HCI status code (see table in HCI introduction).
    * @param Connection_HandleResult Unique identifier for the connection handle
    * for which the operation was done.
    * @param Failed_Contact_CounterResult Number of consecutive failed contacts
    * for this connection.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error. If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * Connection_HandleResult variable will contain the Connection
    * Handle Result returned from the Bluetooth Device AND the
    * Failed_Contact_CounterResult variable will contain the Failed
    * Contact Counter Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Failed_Contact_Counter(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult, Word_t *Failed_Contact_CounterResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Failed_Contact_Counter_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult, Word_t *Failed_Contact_CounterResult);
#endif

   /*! @brief The following function issues the HCI_Reset_Failed_Contact_Counter
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Connection_Handle Unique identifier for the connection
    * returned in the Connection Complete event associated with the
    * HCI_Create_Connection command.
    * @param StatusResult Returned HCI status code.
    * @param Connection_HandleResult Unique identifier for the connection
    * handle for which the operation was done.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * Connection_HandleResult variable will contain the Connection
    * Handle Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Reset_Failed_Contact_Counter(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Reset_Failed_Contact_Counter_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult);
#endif

   /*! @brief The following function issues the HCI_Get_Link_Quality Command to
    * the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param Connection_Handle Unique identifier for the connection
    * returned in the Connection Complete event associated with
    * the HCI_Create_Connection command.
    * @param StatusResult Returned HCI status code.
    * @param Connection_HandleResult Unique identifier for the
    * connection handle for which the operation was done.
    * @param Link_QualityResult The current quality of the link
    * between the local and remote devices, range 0 to 255, where higher is better.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the Connection_HandleResult variable will
    * contain the Connection Handle Result returned from the Bluetooth
    * Device AND the Link_QualityResult variable will contain the Link
    * Quality Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Get_Link_Quality(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult, Byte_t *Link_QualityResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Get_Link_Quality_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult, Byte_t *Link_QualityResult);
#endif

   /*! @brief The following function issues the HCI_Read_RSSI Command to the
    * Bluetooth Device that is associated with the Bluetooth Protocol
    * Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Connection_Handle Unique identifier for the connection returned
    * in the Connection Complete event associated with the HCI_Create_Connection command.
    * @param StatusResult Returned HCI status code.
    * @param Connection_HandleResult Unique identifier for the connection
    * handle for which the operation was done.
    * @param RSSIResult Difference between the measured RSSI and the limits
    * of the Golden Receive Power Range.  This value may range from
    * -28 to +27 dB.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the Connection_HandleResult variable will
    * contain the Connection Handle Result returned from the Bluetooth
    * Device AND the RSSIResult variable will contain the RSSI Result
    * returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_RSSI(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult, SByte_t *RSSIResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_RSSI_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult, SByte_t *RSSIResult);
#endif

   /*! HCI Command API's (Status Parameters - Version 1.2). */

   /*! @brief The following function issues the HCI_Read_AFH_Channel_Map Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Connection_Handle Unique identifier for the connection
    * returned in the Connection Complete event associated with the
    * HCI_Create_Connection command.
    * @param StatusResult Returned HCI status code.
    * @param Connection_HandleResult Unique identifier for the
    * connection handle for which the operation was done.
    * @param AFH_ModeResult Valued returned for AFH enabled or
    * disabled.
    * @param AFH_Channel_MapResult AFH Channel Map result.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the Connection_HandleResult variable will
    * contain the Connection Handle Result returned from the Bluetooth
    * Device AND the AFH_ModeResult variable will contain the AFH Mode
    * Result returned from the Bluetooth Device AND the
    * AFH_Channel_MapResult variable will contain the AFH Channel Map
    * Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_AFH_Channel_Map(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult, Byte_t *AFH_ModeResult, AFH_Channel_Map_t *AFH_Channel_MapResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_AFH_Channel_Map_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult, Byte_t *AFH_ModeResult, AFH_Channel_Map_t *AFH_Channel_MapResult);
#endif

   /*! @brief The following function issues the HCI_Read_Clock Command to the
    * Bluetooth Device that is associated with the Bluetooth Protocol
    * Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Which_Clock Determines if the local clock or the piconet clock
    * is returned.
    * @param Connection_Handle Unique identifier for the connection returned
    * in the Connection Complete event associated with the HCI_Create_Connection command.
    * @param StatusResult Returned HCI status code.
    * @param Connection_HandleResult Unique identifier for the connection
    * handle for which the operation was done.
    * @param ClockResult Bluetooth clock of the device requested.
    * @param AccuracyResult Bluetooth clock error.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the Connection_HandleResult variable will
    * contain the Connection Handle Result returned from the Bluetooth
    * Device AND the ClockResult variable will contain the Clock Result
    * returned from the Bluetooth Device AND the AccuracyResult variable
    * will contain the Accuracy Result returned from the Bluetooth
    * Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Clock(unsigned int BluetoothStackID, Byte_t Which_Clock, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult, DWord_t *ClockResult, Word_t *AccuracyResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Clock_t)(unsigned int BluetoothStackID, Byte_t Which_Clock, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult, DWord_t *ClockResult, Word_t *AccuracyResult);
#endif

   /*! HCI Command API's (Status Parameters - Version 3.0 + HS). */

   /*! @brief The following function issues the HCI_Read_Encryption_Key_Size
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Connection_Handle Handle of connection that the encryption
    * key size will be read from. This should be for an active ACL connection.
    * @param StatusResult If this function returns zero (success) then
    * variable pointed to by StatusResult will contain the status result
    * returned from the Bluetooth device.
    * @param Connection_HandleResult If this function returns zero
    * (success) then variable pointed to by this parameter will contain
    * the Connection Handle returned from the device.
    * @param Key_SizeResult If this function returns zero (success)
    * then variable pointed to by this parameter will contain the
    * Encryption Key Size read from the device.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * Connection_HandleResult variable will contain the Connection
    * Handle Result returned from the Bluetooth Device AND the
    * Key_SizeResult variable will contain the Key Size Result returned
    * from the Bluetooth Device .
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Encryption_Key_Size(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult, Byte_t *Key_SizeResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Encryption_Key_Size_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult, Byte_t *Key_SizeResult);
#endif

   /*! @brief The following function issues the HCI_Read_Local_AMP_Info Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param StatusResult If this function returns zero (success) then
    * variable pointed to by StatusResult will contain the status result
    * returned from the Bluetooth device.
    * @param AMP_StatusResult If this function returns zero (success) then
    * variable pointed to by this parameter will contain the AMP Status returned
    * from the Bluetooth device. Valid values are 0x00 – 0x06. Consult the Host
    * Controller Interface Function specifications for a full description of the
    * possible meanings for each value. Stack via a call to BSC_Initialize
    * @param StatusResult If this function returns zero (success) then
    * variable pointed to by StatusResult will contain the status result
    * returned from the Bluetooth device.
    * @param AMP_StatusResult If this function returns zero (success) then
    * variable pointed to by this parameter will contain the AMP Status returned
    * from the Bluetooth device. Valid values are 0x00 – 0x06. Consult the Host
    * Controller Interface Function specifications for a full description of the
    * possible meanings for each value.
    * @param Total_BandwidthResult If this function returns zero (success)
    * then variable pointed to by this parameter will contain the Total Bandwidth
    * returned from the device. This is an upper bound on the data rate that can be
    * achieved over HCI and accounts for the total bandwidth achieved over the HCI
    * transport. Expressed in kbps.
    * @param Max_Guaranteed_BandwidthResult  If this function returns zero
    * (success) then variable pointed to by this parameter will contain the Max
    * Guaranteed Bandwidth returned from the Bluetooth device. This is the maximum
    * bandwidth the AMP controller can quarantee for a single logical link over HCI.
    * Expressed in kbps.
    * @param Min_LatencyResult If this function returns zero (success) then
    * variable pointed to by this parameter will contain the Min Latency returned
    * from the device. This is the minimum latency, in microsenconds, that the AMP
    * controller can quarantee for a logical channel.
    * @param Max_PDU_SizeResult If this function returns zero (success) then
    * variable pointed to by this parameter will contain the Max PDU Size returned
    * from the Bluetooth device. This is the maximum size of an L2CAP PDU that the
    * AMP will accept.
    * @param Controller_TypeResult If this function returns zero (success)
    * then variable pointed to by this parameter will contain the Controller Type
    * returned from the Bluetooth device.
    * @param PAL_CapabilitiesResult If this function returns zero (success)
    * then variable pointed to by this parameter will contain the PAL Capabilities
    * returned from the Bluetooth device.
    * @param Max_AMP_ASSOC_LengthResult If this function returns zero (success)
    * then variable pointed to by this parameter will contain the MAX Amp ASSOC
    * Length returned from the Bluetooth device.
    * @param Max_Flush_TimeoutResult If this function returns zero (success)
    * then variable pointed to by this parameter will contain the Max Flush Timeout
    * returned from the Bluetooth device.
    * @param Best_Effort_Flush_TimeoutResult If this function returns zero (success)
    * then variable pointed to by this parameter will contain the Max Flush Timeout
    * returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the AMP_StatusResult variable will
    * contain the AMP Status Result returned from the Bluetooth Device
    * AND the Total_BandwidthResult variable will contain the Total
    * Bandwidth Result returned from the Bluetooth Device AND the
    * Max_Guaranteed_BandwidthResult variable will contain the Maximum
    * Guaranteed Bandwidth Result returned from the Bluetooth Device AND
    * the Min_LatencyResult variable will contain the Minimum Latency
    * Result returned from the Bluetooth Device AND the
    * Max_PDU_SizeResult variable will contain the Maximum PDU Size
    * Result returned from the Bluetooth Device AND the
    * Controller_TypeResult variable will contain the Controller Type
    * Result returned from the Bluetooth Device AND the
    * PAL_CapabilitiesResult variable will contain the PAL Capabilities
    * Result returned from the Bluetooth Device AND the
    * Max_AMP_ASSOC_LengthResult variable will contain the Maximum
    * AMP ASSOC Length Result returned from the Bluetooth Device AND the
    * Max_Flush_TimeoutResult variable will contain the Maximum Flush
    * Timeout Result returned from the Bluetooth Device AND the
    * Best_Effort_Flush_TimeoutResult variable will contain the Best
    * Effort Flush Timeout Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Local_AMP_Info(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *AMP_StatusResult, DWord_t *Total_BandwidthResult, DWord_t *Max_Guaranteed_BandwidthResult, DWord_t *Min_LatencyResult, DWord_t *Max_PDU_SizeResult, Byte_t *Controller_TypeResult, Word_t *PAL_CapabilitiesResult, Word_t *Max_AMP_ASSOC_LengthResult, DWord_t *Max_Flush_TimeoutResult, DWord_t *Best_Effort_Flush_TimeoutResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Local_AMP_Info_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *AMP_StatusResult, DWord_t *Total_BandwidthResult, DWord_t *Max_Guaranteed_BandwidthResult, DWord_t *Min_LatencyResult, DWord_t *Max_PDU_SizeResult, Byte_t *Controller_TypeResult, Word_t *PAL_CapabilitiesResult, Word_t *Max_AMP_ASSOC_LengthResult, DWord_t *Max_Flush_TimeoutResult, DWord_t *Best_Effort_Flush_TimeoutResult);
#endif

   /*! @brief The following function issues the HCI_Read_Local_AMP_ASSOC Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param Physical_Link_Handle AMP physical link handle, may be set to 0x00 if
    * command is called outside of physical link creation context.
    * @param Length_So_Far 0 for the first AMP_ASSOC fragment, should be incremented
    * by the length of the previous fragment for each call.
    * @param Max_Remote_AMP_ASSOC_Length Max length in octects allowed by host for
    * AMP_ASSOC.
    * @param AMP_ASSOC_Fragment_Buffer_Length Defines the size of the buffer that
    * AMP_ASSOC_FragmentResult points to. This size MUST be at least:
    * HCI_AMP_ASSOC_FRAGMENT_SIZE_MAXIMUM_FRAGMENT_SIZE
    * bytes long when the calculated remaining length is greater than that value.
    * @param StatusResult If this function returns zero (success) then variable
    * pointed to by StatusResult will contain the status result returned from
    * the Bluetooth device.
    * @param Physical_Link_HandleResult If this function returns zero (success)
    * then variable pointed to by this parameter will contain the Physical
    * Link Handle returned by the device.
    * @param AMP_ASSOC_Remaining_LengthResult If this function returns zero
    * (success) then variable pointed to by this parameter will contain the
    * length in octets of the remainder of AMP_ASSOC structure including this fragment.
    * @param AMP_ASSOC_FragmentLengthResult If this function returns zero
    * (success) then variable pointed to by this parameter will contain the
    * AMP_ASSOC_FragmentLength returned from the Bluetooth device.
    * @param AMP_ASSOC_FragmentResult If this function returns zero
    * (success) then variable pointed to by this parameter will contain a
    * fragment of the AMP_ASSOC structure.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the Physical_Link_HandleResult variable
    * will contain the Physical Link Handle Result returned from the
    * Bluetooth Device AND the AMP_ASSOC_Remaining_LengthResult variable
    * will contain the AMP Association Remaining Length Result returned
    * from the Bluetooth Device AND the AMP_ASSOC_FragmentLengthResult
    * variable will contain the number of bytes of the AMP ASSOC
    * Fragment returned from the Bluetooth Device AND the
    * AMP_ASSOC_FragmentResult variable will contain the AMP Association
    * Fragrment Result returned from the Bluetooth Device
    * \note The fifth parameter (AMP_ASSOC_Fragment_Buffer_Length)
    *          defines the size of the buffer the final parameter
    *          (AMP_ASSOC_FragmentResult) points to.  This size *MUST*
    *          be at least
    *          HCI_AMP_ASSOC_FRAGMENT_SIZE_MAXIMUM_FRAGMENT_SIZE bytes
    *          long when the calculated remaining length is greater than
    *          that value.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Local_AMP_ASSOC(unsigned int BluetoothStackID, Byte_t Physical_Link_Handle, Word_t Length_So_Far, Word_t Max_Remote_AMP_ASSOC_Length, Byte_t AMP_ASSOC_Fragment_Buffer_Length, Byte_t *StatusResult, Byte_t *Physical_Link_HandleResult, Word_t *AMP_ASSOC_Remaining_LengthResult, Byte_t *AMP_ASSOC_FragmentLengthResult, Byte_t *AMP_ASSOC_FragmentResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Local_AMP_ASSOC_t)(unsigned int BluetoothStackID, Byte_t Physical_Link_Handle, Word_t Length_So_Far, Word_t Max_Remote_AMP_ASSOC_Length, Byte_t AMP_ASSOC_Fragment_Buffer_Length, Byte_t *StatusResult, Byte_t *Physical_Link_HandleResult, Word_t *AMP_ASSOC_Remaining_LengthResult, Byte_t *AMP_ASSOC_FragmentLengthResult, Byte_t *AMP_ASSOC_FragmentResult);
#endif

   /*! @brief The following function issues the HCI_Write_Remote_AMP_ASSOC
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param Physical_Link_Handle Handle of physical link that
    * identifies the physical link to be created with associated AMP_ASSOC.
    * @param Length_So_Far 0 for the first AMP_ASSOC fragment, should
    * be incremented by the length of the previous fragment for each call.
    * @param AMP_ASSOC_Remaining_Length Length in octets of remainder
    * of AMP_ASSOC including this fragment.
    * @param AMP_ASSOC_Fragment_Length Size of buffer pointed to by
    * AMP_ASSOC_Fragment. This is the fragment size that will be written by this command.
    * @param AMP_ASSOC_Fragment  AMP_ASSOC fragment buffer that will
    * be written by this command.
    * @param StatusResult If this function returns zero (success) then
    * variable pointed to by StatusResult will contain the status result
    * returned from the Bluetooth device.
    * @param Physical_Link_HandleResult If this function returns zero
    * (success) then variable pointed to by this parameter will contain
    * the Physical Link Handle returned by the device.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * Physical_Link_HandleResult variable will contain the Physical Link
    * Handle Result returned from the Bluetooth Device.
    * \note The fifth paramter (AMP_ASSOC_Fragment_Length) specifies
    *          the size of the buffer that is pointed to by the sixth
    *          parameter (AMP_ASSOC_Fragment).  This is the size of the
    *          fragment that will be written by this command.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Remote_AMP_ASSOC(unsigned int BluetoothStackID, Byte_t Physical_Link_Handle, Word_t Length_So_Far, Word_t AMP_ASSOC_Remaining_Length, Byte_t AMP_ASSOC_Fragment_Length, Byte_t *AMP_ASSOC_Fragment, Byte_t *StatusResult, Byte_t *Physical_Link_HandleResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Remote_AMP_ASSOC_t)(unsigned int BluetoothStackID, Byte_t Physical_Link_Handle, Word_t Length_So_Far, Word_t AMP_ASSOC_Remaining_Length, Byte_t AMP_ASSOC_Fragment_Length, Byte_t *AMP_ASSOC_Fragment, Byte_t *StatusResult, Byte_t *Physical_Link_HandleResult);
#endif

    /*! HCI Command API's (Status Parameters - Version 4.0 + CSA3). */
    /*! @brief The following function issues the
    * HCI_Get_MWS_Transport_Layer_Configuration Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param Transport_Layer_ConfigurationResult Will contain the Transport Layer Configuration retruned from the Bluetooth device.
    * @param StatusResult If this function returns zero (success) then variables pointed to by StatusResult will contain the status result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error. If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device and the Transport_Layer_ConfigurationResult
    * variable will contain the Transport Layer Configuration retruned
    * from the Bluetooth device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    * \note The HCI_Free_MWS_Transport_Layer_Configuration() function
    *          MUST be called after a successful return from this
    *          function to free the memory allocated by this call.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Get_MWS_Transport_Layer_Configuration(unsigned int BluetoothStackID, Byte_t *StatusResult, HCI_MWS_Transport_Layer_Configuration_Info_t *Transport_Layer_ConfigurationResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Get_MWS_Transport_Layer_Configuration_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, HCI_MWS_Transport_Layer_Configuration_Info_t *Transport_Layer_ConfigurationResult);
#endif

   /*! @brief The following function frees the memory allocated by a successful
    * call to the HCI_Get_MWS_Transport_Layer_Configuration() function.
    */
BTPSAPI_DECLARATION void BTPSAPI HCI_Free_MWS_Transport_Layer_Configuration(HCI_MWS_Transport_Layer_Configuration_Info_t *Transport_Layer_Configuration);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (BTPSAPI *PFN_HCI_Free_MWS_Transport_Layer_Configuration_t)(HCI_MWS_Transport_Layer_Configuration_Info_t *Transport_Layer_Configuration);
#endif

   /* HCI Command API's (Status Parameters - Version 4.0 + CSA4).
    * @brief The following function issues the HCI_Set_Triggered_Clock_Capture
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param Connection_Handle Returned from Bluetooth device.
    * @param Enable Disable triggered clock capturing on the specified Connection Handle.
    * @param Which_Clock Value is 0, then the Connection_Handle shall be ignored,the local Bluetooth Clock value shall be returned and the accuracy parameter shall be set to 0.
    * @param LPO_Allowed Informs the BR/EDR Controller whether it is allowed to sleep.
    * @param StatusResult If function returns zero (success) this variable will contain the Status Result returned from the Bluetooth device.
    * @param Num_Clock_Captures_To_Filter Used to filter triggered clock captures between sending Triggered Clock Capture events to the Host.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If @return This function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Set_Triggered_Clock_Capture(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t Enable, Byte_t Which_Clock, Byte_t LPO_Allowed, Byte_t Num_Clock_Captures_To_Filter, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Set_Triggered_Clock_Capture_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t Enable, Byte_t Which_Clock, Byte_t LPO_Allowed, Byte_t Num_Clock_Captures_To_Filter, Byte_t *StatusResult);
#endif

   /* HCI Command API's (Testing Commands).
    * @brief The following function issues the HCI_Read_Loopback_Mode Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @param Loopback_ModeResult Current setting of this parameter. Possible values are:
    * HCI_LOOPBACK_MODE_NO_LOOPBACK_MODE
    * HCI_LOOPBACK_MODE_ENABLE_LOCAL_LOOPBACK
    * HCI_LOOPBACK_MODE_ENABLE_REMOTE_LOOPBACK
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the Loopback_ModeResult variable will
    * contain the Connection Handle Result returned from the Bluetooth
    * Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Read_Loopback_Mode(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Loopback_ModeResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Read_Loopback_Mode_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Loopback_ModeResult);
#endif

   /*! @brief The following function issues the HCI_Write_Loopback_Mode Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Loopback_Mode Current setting of this parameter.
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned
    * from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Loopback_Mode(unsigned int BluetoothStackID, Byte_t Loopback_Mode, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Loopback_Mode_t)(unsigned int BluetoothStackID, Byte_t Loopback_Mode, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the
    * HCI_Enable_Device_Under_Test_Mode Command to the Bluetooth Device
    * that is associated with the Bluetooth Protocol Stack specified by
    * the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param StatusResult Returned HCI status code.
    * @return This function returns zero if
    * successful, or a non-zero value if there was an error.
    * If this function returns zero (success) then the StatusResult variable
    * will contain the Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Enable_Device_Under_Test_Mode(unsigned int BluetoothStackID, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Enable_Device_Under_Test_Mode_t)(unsigned int BluetoothStackID, Byte_t *StatusResult);
#endif

   /*! HCI Command API's (Testing Commands - Version 2.1 + EDR). */

   /*! @brief The following function issues the
    * HCI_Write_Simple_Pairing_Debug_Mode Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Debug_Mode Specifies whether to enable (0x0) or disable (0x00)
    * Simple Pairing debug mode.
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Simple_Pairing_Debug_Mode(unsigned int BluetoothStackID, Byte_t Debug_Mode, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Simple_Pairing_Debug_Mode_t)(unsigned int BluetoothStackID, Byte_t Debug_Mode, Byte_t *StatusResult);
#endif

   /*! HCI Command API's (Testing Commands - Version 3.0 + HS). */

   /*! @brief The following function issues the HCI_Enable_AMP_Receiver_Reports
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param Enable Specifies whether to enable (0x0) or disable
    * (0x00) the reporting of frames sent.
    * @param Interval The third parameter is the interval.
    * @param StatusResult If function returns zero (success)
    * this variable will contain the Status Result returned from the Bluetooth device
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Enable_AMP_Receiver_Reports(unsigned int BluetoothStackID, Byte_t Enable, Byte_t Interval, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Enable_AMP_Receiver_Reports_t)(unsigned int BluetoothStackID, Byte_t Enable, Byte_t Interval, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_AMP_Test_End Command to the
    * Bluetooth Device that is associated with the Bluetooth Protocol
    * Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to
    * this Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param StatusResult If function returns zero (success)
    * this variable will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_AMP_Test_End(unsigned int BluetoothStackID, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_AMP_Test_End_t)(unsigned int BluetoothStackID, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_AMP_Test_Command Command to
    * the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Parameter_Length Number of bytes to send from buffer specified
    * by Parameter_Data parameter
    * @param Parameter_Data[] Byte buffer containing the bytes to be sent.
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.  The Parameter_Length parameter specifies
    * the number of bytes to send from the Buffer specified by the
    * Parameter_Data parameter.  The Parameter_Data parameter is ignored
    * if Parameter_Length is zero.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_AMP_Test_Command(unsigned int BluetoothStackID, Byte_t Parameter_Length, Byte_t Parameter_Data[], Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_AMP_Test_Command_t)(unsigned int BluetoothStackID, Byte_t Parameter_Length, Byte_t Parameter_Data[], Byte_t *StatusResult);
#endif

   /*! HCI Command API's (Testing Commands - Version 4.1). */
   /*! @brief The following function issues the
    * HCI_Write_Secure_Connections_Test_Mode Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param Connection_Handle Returned from Bluetooth device.
    * @param DM1_ACLU_Mode enables and disables the use of DM1 packets for transmitting ACL-U data.
    * @param ESCO_Loopback_Mode	enables and disables the loopback of received eSCO payloads.
    * @param Connection_HandleResult will contain the Connection Handle result returned from the Bluetooth Device.
    * @param StatusResult If function returns zero (success) this variable will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the Connection_HandleResult variable will
    * contain the Connection Handle result returned from the Bluetooth
    * Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_Write_Secure_Connections_Test_Mode(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t DM1_ACLU_Mode, Byte_t ESCO_Loopback_Mode, Byte_t *StatusResult, Word_t *Connection_HandleResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_Write_Secure_Connections_Test_Mode_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t DM1_ACLU_Mode, Byte_t ESCO_Loopback_Mode, Byte_t *StatusResult, Word_t *Connection_HandleResult);
#endif

   /*! HCI Command API's (LE Commands - Version 4.0 + LE). */
   /*! @brief The following function issues the HCI_LE_Set_Event_Mask Command to
    * the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param LE_Event_Mask Event mask to set for the Host. The bit mask is constructed via the following API macros:
    * SET_EVENT_MASK_BIT(Mask, BitNumber)
    * RESET_EVENT_MASK_BIT(Mask, BitNumber)
    * TEST_EVENT_MASK_BIT(Mask, BitNumber)
    * HCI_ENABLE_ALL_LE_EVENTS_IN_EVENT_MASK(Mask)
    * HCI_DISABLE_ALL_LE_EVENTS_IN_EVENT_MASK(Mask)
    * The bit number constants defined in the API for use with these macros are:
    * HCI_LE_EVENT_MASK_CONNECTION_COMPLETE_* BIT_NUMBER
    * HCI_LE_EVENT_MASK_ADVERTISING_REPORT_BIT_* NUMBER
    * HCI_LE_EVENT_MASK_CONNECTION_UPDATE_* COMPLETE_BIT_NUMBER
    * HCI_LE_EVENT_MASK_READ_REMOTE_USED_* FEATURES_COMPLETE_BIT_NUMBER
    * HCI_LE_EVENT_MASK_LONG_TERM_KEY_REQUEST_* BIT_NUMBER
    * @param StatusResult If function returns zero (success) this variable will contain the Status Result returned from the Bluetooth device.
    * followed by the LE Event Mask to set.  @return This function returns zero
    * if successful, or a non-zero value if there was an error.  If @return This function returns zero (success) then the StatusResult variable
    * will contain the Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Set_Event_Mask(unsigned int BluetoothStackID, Event_Mask_t LE_Event_Mask, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Set_Event_Mask_t)(unsigned int BluetoothStackID, Event_Mask_t LE_Event_Mask, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_LE_Read_Buffer_Size Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize,
    * @param StatusResult If function returns zero (success) this
    * variable will contain the Status Result returned from the Bluetooth device.
    * @param HC_LE_ACL_Data_Packet_Length Contains the returned maximum
    * length of ACL data packet.
    * @param HC_Total_Num_LE_ACL_Data_Packets Contains the returned total
    * number of data packets the can be stored in the buffers.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device AND the HC_LE_ACL_Data_Packet_LengthResult
    * variable will contain the HCI LE ACL Data Packet Length Result
    * returned from the Bluetooth Device AND the
    * HC_Total_Num_LE_ACL_Data_PacketsResult variable will contain the
    * HCI Total Number LE ACL Data Packets Result returned from the
    * Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Read_Buffer_Size(unsigned int BluetoothStackID, Byte_t *StatusResult, Word_t *HC_LE_ACL_Data_Packet_Length, Byte_t *HC_Total_Num_LE_ACL_Data_Packets);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Read_Buffer_Size_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Word_t *HC_LE_ACL_Data_Packet_Length, Byte_t *HC_Total_Num_LE_ACL_Data_Packets);
#endif

   /*! @brief The following function issues the
    * HCI_LE_Read_Local_Supported_Features Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param StatusResult If function returns zero (success) this
    * variable will contain the Status Result returned from the Bluetooth device.
    * @param LE_FeaturesResult Bit mask list of supported features.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned
    * from the Bluetooth Device AND the LE_FeaturesResult variable will
    * contain the LE Features Result returned from the Bluetooth
    * Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Read_Local_Supported_Features(unsigned int BluetoothStackID, Byte_t *StatusResult, LE_Features_t *LE_FeaturesResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Read_Local_Supported_Features_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, LE_Features_t *LE_FeaturesResult);
#endif

   /*! @brief The following function issues the HCI_LE_Set_Random_Address
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Random address to use.
    * @param StatusResult If function returns zero (success) this
    * variable will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error. If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Set_Random_Address(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Set_Random_Address_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the
    * HCI_LE_Set_Advertising_Parameters to the Bluetooth Device that is
    * associated with the Bluetooth Protocol Stack specified by the
    * BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param Advertising_Interval_Min Mininum interval to
    * advertise.
    * @param Advertising_Interval_Max Maximum interval to
    * advertise.
    * @param Advertising_Type Type of advertising to use.
    * @param Own_Address_Type Address type of local device’s
    * address.
    * @param Direct_Address_Type Address type of directed
    * address (if directed advertising).
    * @param Direct_Address Address of directed device
    * (if directed advertising).
    * @param Advertising_Channel_Map Indicates which advertising
    * channels to use.
    * @param Advertising_Filter_Policy Policy of which devices to allow
    * requests from.
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device
    * @return This function returns zero if
    * successfull, or a non-zero value if there was an error. If this function returns zero (success) then the StatusResult variable
    * will contain the Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Set_Advertising_Parameters(unsigned int BluetoothStackID, Word_t Advertising_Interval_Min, Word_t Advertising_Interval_Max, Byte_t Advertising_Type, Byte_t Own_Address_Type, Byte_t Direct_Address_Type, BD_ADDR_t Direct_Address, Byte_t Advertising_Channel_Map, Byte_t Advertising_Filter_Policy, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Set_Advertising_Parameters_t)(unsigned int BluetoothStackID, Word_t Advertising_Interval_Min, Word_t Advertising_Interval_Max, Byte_t Advertising_Type, Byte_t Own_Address_Type, Byte_t Direct_Address_Type, BD_ADDR_t Direct_Address, Byte_t Advertising_Channel_Map, Byte_t Advertising_Filter_Policy, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the
    * HCI_LE_Read_Advertising_Channel_Tx_Power to the Bluetooth Device
    * that isassociated with the Bluetooth Protocol Stack specified by
    * the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device.
    * @param Transmit_Power_LevelResult  Contains the returned
    * transmit power level.
    * @return This function returns zero if
    * successfull, or a non-zero value if there was an error. If this function returns zero (success) then the StatusResult variable
    * will contain the Status Result returned from the Bluetooth Device
    * AND the Transmit_Power_LevelResult variable will contain the
    * Transmit Power Level returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Read_Advertising_Channel_Tx_Power(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Transmit_Power_LevelResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Read_Advertising_Channel_Tx_Power_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *Transmit_Power_LevelResult);
#endif

   /*! @brief The following function issues the HCI_LE_Set_Advertising_Data
    * to the Bluetooth Device that isassociated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param Advertising_Data_Length Length of advertising data.
    * @param Advertising_Data Actual advertising data.
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successfull, or a non-zero value if
    * there was an error. If this function returns zero (success) then
    * the StatusResult variable will contain the Status Result returned
    * from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Set_Advertising_Data(unsigned int BluetoothStackID, Byte_t Advertising_Data_Length, Advertising_Data_t *Advertising_Data, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Set_Advertising_Data_t)(unsigned int BluetoothStackID, Byte_t Advertising_Data_Length, Advertising_Data_t *Advertising_Data, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_LE_Set_Scan_Response_Data
    * to the Bluetooth Device that isassociated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param Scan_Response_Data_Length Length of scan response data.
    * @param Scan_Response_Data  Actual scan response data.
    * @param StatusResult If function returns zero (success) this
    * variable will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successfull, or a non-zero value if
    * there was an error. If this function returns zero (success) then
    * the StatusResult variable will contain the Status Result returned
    * from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Set_Scan_Response_Data(unsigned int BluetoothStackID, Byte_t Scan_Response_Data_Length, Scan_Response_Data_t *Scan_Response_Data, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Set_Scan_Response_Data_t)(unsigned int BluetoothStackID, Byte_t Scan_Response_Data_Length, Scan_Response_Data_t *Scan_Response_Data, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_LE_Set_Advertise_Enable
    * to the Bluetooth Device that isassociated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param Advertising_Enable Desired value to set.
    * @param StatusResult If function returns zero (success)
    * this variable will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successfull, or a non-zero value if
    * there was an error. If this function returns zero (success) then
    * the StatusResult variable will contain the Status Result returned
    * from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Set_Advertise_Enable(unsigned int BluetoothStackID, Byte_t Advertising_Enable, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Set_Advertise_Enable_t)(unsigned int BluetoothStackID, Byte_t Advertising_Enable, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_LE_Set_Scan_Parameters
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param LE_Scan_Type Type of scan to perform.
    * @param LE_Scan_Interval Interval to set between LE scans. Defined as
    * number of baseband slots (0.625 msec).
    * @param LE_Scan_Window Value to set duration of an LE scan. Should be
    * defined as number of baseband slots (00625msec), less than or equal
    * to scan window, and within the range as scan window.
    * Both intervals follow the rule:
    * Time = N * 0.625msec
    * @param Own_Address_Type Type of local device’s address.
    * @param Scanning_Filter_Policy Determines which advertising
    * packets to accept.
    * @param StatusResult If function returns zero (success) this
    * variable will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successfull, or a non-zero value if
    * there was an error. If this function returns zero (success) then
    * the StatusResult variable will contain the Status Result returned
    * from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Set_Scan_Parameters(unsigned int BluetoothStackID, Byte_t LE_Scan_Type, Word_t LE_Scan_Interval, Word_t LE_Scan_Window, Byte_t Own_Address_Type, Byte_t Scanning_Filter_Policy, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Set_Scan_Parameters_t)(unsigned int BluetoothStackID, Byte_t LE_Scan_Type, Word_t LE_Scan_Interval, Word_t LE_Scan_Window, Byte_t Own_Address_Type, Byte_t Scanning_Filter_Policy, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_LE_Set_Scan_Enable
    * to the Bluetooth Device that isassociated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param LE_Scan_Enable Enable or disable scanning.
    * @param Filter_Duplicates Specifies whether duplicate reports should
    * be filtered out.
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successfull, or a non-zero value if
    * there was an error. If this function returns zero (success) then
    * the StatusResult variable will contain the Status Result returned
    * from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Set_Scan_Enable(unsigned int BluetoothStackID, Byte_t LE_Scan_Enable, Byte_t Filter_Duplicates, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Set_Scan_Enable_t)(unsigned int BluetoothStackID, Byte_t LE_Scan_Enable, Byte_t Filter_Duplicates, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_LE_Create_Connection Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID    Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param LE_Scan_Interval    Interval to delay between LE scans. Defined
    * as number of baseband slots (0.625 msec). Should be within the range:
    * HCI_LE_SCAN_INTERVAL_MINIMUM  to HCI_LE_SCAN_INTERVAL_MAXIMUM
    * @param LE_Scan_Window  Value to use for the duration of an LE scan.
    * Should be defined as number of baseband slots (0.625 msec), less
    * than or equal to scan window, and within the range as scan window.
    * @param Initiator_Filter_Policy Determines whether to use a white list.
    * Possible values are:
    * HCI_LE_INITIATOR_FILTER_POLICY_WHITE_LIST_NOT_USED or
    * HCI_LE_INITIATOR_FILTER_POLICY_WHITE_LIST_IS_USED
    * @param Peer_Address_Type   Type of peer address. Possible values are:
    * HCI_LE_ADDRESS_TYPE_PUBLIC or HCI_LE_ADDRESS_TYPE_RANDOM
    * @param Peer_Address    Address of advertiser to connect if white list is not enabled.
    * @param Own_Address_Type    Type of local device address. Possible values are:
    * HCI_LE_ADDRESS_TYPE_PUBLIC or HCI_LE_ADDRESS_TYPE_RANDOM
    * @param Conn_Interval_Min   Minimum value for the the connection interval.
    *  This should fall within the range:
    * HCI_LE_CONNECTION_INTERVAL_MINIMUM and HCI_LE_CONNECTION_INTERVAL_MAXIMUM
    * @param Conn_Interval_Max   This should be greater than or equal to
    * Conn_Interval_Min and shall fall within the range:
    * HCI_LE_CONNECTION_INTERVAL_MINIMUM and HCI_LE_CONNECTION_INTERVAL_MAXIMUM
    * Both intervals follow the rule:
    * Time = N * .25 msec
    * @param Conn_Latency Slave latency for connection.  This should be in range:
    * HCI_LE_CONNECTION_LATENCY_MINIMUM and HCI_LE_CONNECTION_LATENCY_MAXIMUM
    * @param Supervision_Timeout Supervision timeout for LE link.  This should be in range:
    * HCI_LE_SUPERVISION_TIMEOUT_MINIMUM and HCI_LE_SUPERVISION_TIMEOUT_MAXIMUM
    * The Supervision_Timeout follows the rule:
    * Time = N * 0 msec
    * @param Minimum_CE_Length   Information about minimum length of LE connection.
    *  This should be in range:
    * HCI_LE_LENGTH_OF_CONNECTION_MINIMUM and HCI_LE_LENGTH_OF_CONNECTION_MAXIMUM
    * @param Maximum_CE_Length   Information about maximum length of LE connection.
    * Should be in range
    * HCI_LE_LENGTH_OF_CONNECTION_MINIMUM and HCI_LE_LENGTH_OF_CONNECTION_MAXIMUM
    * Both CE_Lengths follow the rule:
    * Time = N * 0.625 msec
    * @param StatusResult If function returns zero (success) this variable will
    * contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Create_Connection(unsigned int BluetoothStackID, Word_t LE_Scan_Interval, Word_t LE_Scan_Window, Byte_t Initiator_Filter_Policy, Byte_t Peer_Address_Type, BD_ADDR_t Peer_Address, Byte_t Own_Address_Type, Word_t Conn_Interval_Min, Word_t Conn_Interval_Max, Word_t Conn_Latency, Word_t Supervision_Timeout, Word_t Minimum_CE_Length, Word_t Maximum_CE_Length, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Create_Connection_t)(unsigned int BluetoothStackID, Word_t LE_Scan_Interval, Word_t LE_Scan_Window, Byte_t Initiator_Filter_Policy, Byte_t Peer_Address_Type, BD_ADDR_t Peer_Address, Byte_t Own_Address_Type, Word_t Conn_Interval_Min, Word_t Conn_Interval_Max, Word_t Conn_Latency, Word_t Supervision_Timeout, Word_t Minimum_CE_Length, Word_t Maximum_CE_Length, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_LE_Create_Connection_Cancel
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Create_Connection_Cancel(unsigned int BluetoothStackID, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Create_Connection_Cancel_t)(unsigned int BluetoothStackID, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_LE_Read_White_List_Size
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device.
    * @param White_List_SizeResult Contains the returned size of the white
    * list (specified in number of devices).
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * White_List_SizeResult variable will contain the White List Size
    * returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Read_White_List_Size(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *White_List_SizeResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Read_White_List_Size_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Byte_t *White_List_SizeResult);
#endif

   /*! @brief The following function issues the HCI_LE_Clear_White_List
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param StatusResult If function returns zero (success) this variable will
    * contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Clear_White_List(unsigned int BluetoothStackID, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Clear_White_List_t)(unsigned int BluetoothStackID, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_LE_Add_Device_To_White_List
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param Address_Type Type of address being added.
    * @param Address Address to of device to add to the white list.
    * @param StatusResult If function returns zero (success) this variable will contain
    * the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Add_Device_To_White_List(unsigned int BluetoothStackID, Byte_t Address_Type, BD_ADDR_t Address, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Add_Device_To_White_List_t)(unsigned int BluetoothStackID, Byte_t Address_Type, BD_ADDR_t Address, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the
    * HCI_LE_Remove_Device_From_White_List Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param Address_Type Type of address being added.
    * @param Address Address to of device to remove from the white list.
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned
    * from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Remove_Device_From_White_List(unsigned int BluetoothStackID, Byte_t Address_Type, BD_ADDR_t Address, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Remove_Device_From_White_List_t)(unsigned int BluetoothStackID, Byte_t Address_Type, BD_ADDR_t Address, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_LE_Connection_Update Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param Connection_Handle Handle to the connection desired to be updated.
    * @param Conn_Interval_Min Minimum value for the the connection interval.
    * This should fall within the range:
    * HCI_LE_CONNECTION_INTERVAL_MINIMUM and HCI_LE_CONNECTION_INTERVAL_MAXIMUM
    * @param Conn_Interval_Max This should be greater than or equal
    *to Conn_Interval_Min and shall fall within the range:
    * HCI_LE_CONNECTION_INTERVAL_MINIMUM and HCI_LE_CONNECTION_INTERVAL_MAXIMUM
    * Both intervals follow the rule:
    * Time = N * .25 msec
    * @param Conn_Latency Slave latency for connection. This should be in range:
    * HCI_LE_CONNECTION_LATENCY_MINIMUM and HCI_LE_CONNECTION_LATENCY_MAXIMUM
    * @param Supervision_Timeout Supervision timeout for LE link.  This should be in range:
    * HCI_LE_SUPERVISION_TIMEOUT_MINIMUM and HCI_LE_SUPERVISION_TIMEOUT_MAXIMUM
    The Supervision_Timeout follows the rule:
    * Time = N * 0 msec
    * @param Minimum_CE_Length Information about minimum length of LE
    * connection.  This should be in range:
    * HCI_LE_LENGTH_OF_CONNECTION_MINIMUM and HCI_LE_LENGTH_OF_CONNECTION_MAXIMUM
    * @param Maximum_CE_Length Information about maximum length of LE connection
    * Should be in range.
    * HCI_LE_LENGTH_OF_CONNECTION_MINIMUM and HCI_LE_LENGTH_OF_CONNECTION_MAXIMUM
    * Both CE_Lengths follow the rule:
    * Time = N * 0.625 msec
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Connection_Update(unsigned int BluetoothStackID, Word_t Connection_Handle, Word_t Conn_Interval_Min, Word_t Conn_Interval_Max, Word_t Conn_Latency, Word_t Supervision_Timeout, Word_t Minimum_CE_Length, Word_t Maximum_CE_Length, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Connection_Update_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Word_t Conn_Interval_Min, Word_t Conn_Interval_Max, Word_t Conn_Latency, Word_t Supervision_Timeout, Word_t Minimum_CE_Length, Word_t Maximum_CE_Length, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the
    * HCI_LE_Set_Host_Channel_Classification Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID    Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param Channel_Map New channel map to set.  It is a 37-bit field where
    * the nth bit represents channel index n. A value of 0 represents the channel
    * is bad (not used).  A value of 1 represents the channel is unkown.
    * At least one channel should be marked as unkown.
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value
    * if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned
    * from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Set_Host_Channel_Classification(unsigned int BluetoothStackID, LE_Channel_Map_t Channel_Map, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Set_Host_Channel_Classification_t)(unsigned int BluetoothStackID, LE_Channel_Map_t Channel_Map, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_LE_Read_Channel_Map
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param Connection_Handle Handle that identifies the desired connection.
    * @param StatusResult If function returns zero (success) this variable will
    * contain the Status Result returned from the Bluetooth device.
    * @param Connection_HandleResult Connection handle returned from Bluetooth
    * device.
    * @param Channel_MapResult Returned channel map.  It is a 37-bit field where
    * the nth bit represents channel index n. A value of 0 represents the channel
    * is bad (not used).  A value of 1 represents the channel is unkown.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * Connection_HandleResult variable will contain the Connection
    * Handle returned from the Bluetooth Device AND the
    * LE_Channel_MapResult variable will contain the LE Channel Map
    * returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Read_Channel_Map(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult, LE_Channel_Map_t *Channel_MapResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Read_Channel_Map_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult, LE_Channel_Map_t *Channel_MapResult);
#endif

   /*! @brief The following function issues the HCI_LE_Read_Remote_Used_Features
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param Connection_Handle Handle that identifies the desired connection.
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Read_Remote_Used_Features(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Read_Remote_Used_Features_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_LE_Encrypt Command to the
    * Bluetooth Device that is associated with the Bluetooth Protocol
    * Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param Key 28 bit encryption key.
    * @param Plain_Text_Data 28 bit data block to be encrypted.
    * @param StatusResult If function returns zero (success) this
    * variable will contain the Status Result returned from the Bluetooth device.
    * @param Encrypted_DataResult 28 bit encrypted data block.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned
    * from the Bluetooth Device AND the  Encrypted_DataResult variable
    * will contain the Encrypted Data returned from the Bluetooth
    * Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Encrypt(unsigned int BluetoothStackID, Encryption_Key_t Key, Plain_Text_Data_t Plain_Text_Data, Byte_t *StatusResult, Encrypted_Data_t *Encrypted_DataResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Encrypt_t)(unsigned int BluetoothStackID, Encryption_Key_t Key, Plain_Text_Data_t Plain_Text_Data, Byte_t *StatusResult, Encrypted_Data_t *Encrypted_DataResult);
#endif

   /*! @brief The following function issues the HCI_LE_Rand Command to the
    * Bluetooth Device that is associated with the Bluetooth Protocol
    * Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param StatusResult If function returns zero (success) this
    * variable will contain the Status Result returned from the Bluetooth device.
    * @param Random_NumberResult 64-bit random number generated
    * from the controller.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error. If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned
    * from the Bluetooth Device AND the Random_NumberResult variable
    * will contain the Random Number returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Rand(unsigned int BluetoothStackID, Byte_t *StatusResult, Random_Number_t *Random_NumberResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Rand_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Random_Number_t *Random_NumberResult);
#endif

   /*! @brief The following function issues the HCI_LE_Start_Encryption
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param Connection_Handle Handle used to identify the desired
    * connection.
    * @param Random_Number 64 bit random number to use during the
    * encryption process.
    * @param Encrypted_Diversifier 6-bit encrypted diversifier.
    * @param Long_Term_Key 28-bit long term key.
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Start_Encryption(unsigned int BluetoothStackID, Word_t Connection_Handle, Random_Number_t Random_Number, Word_t Encrypted_Diversifier, Long_Term_Key_t Long_Term_Key, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Start_Encryption_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Random_Number_t Random_Number, Word_t Encrypted_Diversifier, Long_Term_Key_t Long_Term_Key, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the
    * HCI_LE_Long_Term_Key_Request_Reply Command to the Bluetooth
    * Device that is associated with the Bluetooth Protocol Stack
    * specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param Connection_Handle Handle used ot identify the desired connection.
    * @param Long_Term_Key 28-bit long term key.
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device.
    * @param Connection_HandleResult Returned connection handle.
    * @return This function returns zero if successful, or a non-zero value if there was
    * an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned
    * from the Bluetooth Device AND the Connection_HandleResult
    * variable will contain the Connection Handle returned from the
    * Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Long_Term_Key_Request_Reply(unsigned int BluetoothStackID, Word_t Connection_Handle, Long_Term_Key_t Long_Term_Key, Byte_t *StatusResult, Word_t *Connection_HandleResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Long_Term_Key_Request_Reply_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Long_Term_Key_t Long_Term_Key, Byte_t *StatusResult, Word_t *Connection_HandleResult);
#endif

   /*! @brief The following function issues the
    * HCI_LE_Long_Term_Key_Request_Negative_Reply Command to the
    * Bluetooth Device that is associated with the Bluetooth Protocol
    * Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param Connection_Handle Handle used to identify the desired
    * connection.
    * @param StatusResult If function returns zero (success) this
    * variable will contain the Status Result returned from the Bluetooth device.
    * @param Connection_HandleResult Returned connection handle.
    * @return This function returns zero if successful, or a non-zero value if there was
    * an error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned
    * from the Bluetooth Device AND the Connection_HandleResult
    * variable will contain the Connection Handle returned from the
    * Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Long_Term_Key_Request_Negative_Reply(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Long_Term_Key_Request_Negative_Reply_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t *StatusResult, Word_t *Connection_HandleResult);
#endif

   /*! @brief The following function issues the HCI_LE_Read_Supported_States
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device.
    * @param LE_StatesResult Returned supported LE states. These states are
    * represented as a bit mask.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device AND the
    * LE_StatesResult variable will contain the LE States returned
    * from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Read_Supported_States(unsigned int BluetoothStackID, Byte_t *StatusResult, LE_States_t *LE_StatesResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Read_Supported_States_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, LE_States_t *LE_StatesResult);
#endif

   /*! @brief The following function issues the HCI_LE_Receiver_Test
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param RX_Frequency Frequency to receive packets, Where
    * N (RX_Frequency) = (F -2402) / 2. This value should be in the range:
    * HCI_LE_RECEIVER_TRANSMITTER_TEST_FREQUENCY_MINIMUM and
    * HCI_LE_RECEIVER_TRANSMITTER_TEST_FREQUENCY_MAXIMUM
    * @param StatusResult If function returns zero (success)
    * this variable will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Receiver_Test(unsigned int BluetoothStackID, Byte_t RX_Frequency, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Receiver_Test_t)(unsigned int BluetoothStackID, Byte_t RX_Frequency, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_LE_Transmitter_Test
    * Command to the Bluetooth Device that is associated with the
    * Bluetooth Protocol Stack specified by the BluetoothStackID
    * parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param TX_Frequency Frequency to receive packets, Where N(TX_Frequency) = (F -2402) / 2.  This value should be in the range:
    * HCI_LE_RECEIVER_TRANSMITTER_TEST_FREQUENCY_MINIMUM and
    * HCI_LE_RECEIVER_TRANSMITTER_TEST_FREQUENCY_MAXIMUM
    * @param Length_Of_Test_Data Length in bytes of payload data in each packet.
    * @param Packet_Payload Description of the transmitted test pattern.
    * @param StatusResult If function returns zero (success) this variable
    * will contain the Status Result returned from the Bluetooth device.
    * @return This function returns zero if successful, or a
    * non-zero value if there was an error.  If this function returns
    * zero (success) then the StatusResult variable will contain the
    * Status Result returned from the Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Transmitter_Test(unsigned int BluetoothStackID, Byte_t TX_Frequency, Byte_t Length_Of_Test_Data, Byte_t Packet_Payload, Byte_t *StatusResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Transmitter_Test_t)(unsigned int BluetoothStackID, Byte_t TX_Frequency, Byte_t Length_Of_Test_Data, Byte_t Packet_Payload, Byte_t *StatusResult);
#endif

   /*! @brief The following function issues the HCI_LE_Test_End Command to the
    * Bluetooth Device that is associated with the Bluetooth Protocol
    * Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param StatusResult The second parameter is the status result.
    * @param Number_Of_PacketsResult Number of packets received
    * (0x0000 for a transmitter test).
    * @return This function returns zero if successful, or a non-zero
    * value if there was an
    * error. If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned
    * from the Bluetooth Device AND the Number_Of_PacketsResult
    * variable will contain the Number Of Packets returned from the
    * Bluetooth Device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Test_End(unsigned int BluetoothStackID, Byte_t *StatusResult, Word_t *Number_Of_PacketsResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Test_End_t)(unsigned int BluetoothStackID, Byte_t *StatusResult, Word_t *Number_Of_PacketsResult);
#endif

   /*! HCI Command API's (LE Commands - Version 4.1). */
   /*! @brief The following function issues the
    * HCI_LE_Remote_Connection_Parameter_Request_Reply Command to the
    * Bluetooth Device that is associated with the Bluetooth Protocol
    * Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param Connection_Handle Returned from Bluetooth device.
    * @param Conn_Interval_Min Contains the minimum connection’s interval.
    * @param Conn_Interval_Max Contains the minimum connection’s interval.
    * @param Conn_Latency Contains the current connection’s latency.
    * @param Supervision_Timeout Contains the current connection’s supervision timeout.
    * @param Minimum_CE_Length Contains the minimum CE length.
    * @param Maximum_CE_Length Contains the maximum CE length.
    * @param StatusResult If function returns zero (success) this variable will contain the Status Result returned from the Bluetooth device.
    * @param Connection_HandleResult Will contain the Connection Handle returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there was an
    * error.  If this function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device and the Connection_HandleResult variable will
    * contain the Connection Handle returned from the Bluetooth device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Remote_Connection_Parameter_Request_Reply(unsigned int BluetoothStackID, Word_t Connection_Handle, Word_t Conn_Interval_Min, Word_t Conn_Interval_Max, Word_t Conn_Latency, Word_t Supervision_Timeout, Word_t Minimum_CE_Length, Word_t Maximum_CE_Length, Byte_t *StatusResult, Word_t *Connection_HandleResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Remote_Connection_Parameter_Request_Reply_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Word_t Conn_Interval_Min, Word_t Conn_Interval_Max, Word_t Conn_Latency, Word_t Supervision_Timeout, Word_t Minimum_CE_Length, Word_t Maximum_CE_Length, Byte_t *StatusResult, Word_t *Connection_HandleResult);
#endif

   /*! @brief The following function issues the
    * HCI_LE_Remote_Connection_Parameter_Request_Negative_Reply Command
    * to the Bluetooth Device that is associated with the Bluetooth
    * Protocol Stack specified by the BluetoothStackID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param Connection_Handle Returned from Bluetooth device.
    * @param Reason	Reason that the connection parameter request was rejected.
    * @param StatusResult If function returns zero (success) this variable will contain the Status Result returned from the Bluetooth device.
    * @param Connection_HandleResult Will contain the Connection Handle returned from the Bluetooth device.
    * @return This function returns zero if successful, or a non-zero value if there
    * was an error.  If @return This function returns zero (success) then the
    * StatusResult variable will contain the Status Result returned from
    * the Bluetooth Device and the Connection_HandleResult variable will
    * contain the Connection Handle returned from the Bluetooth device.
    * \note This function blocks until either a result is returned
    *          from the Bluetooth Device OR the function Times Out
    *          Waiting for a response from the Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI HCI_LE_Remote_Connection_Parameter_Request_Negative_Reply(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t Reason, Byte_t *StatusResult, Word_t *Connection_HandleResult);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_LE_Remote_Connection_Parameter_Request_Negative_Reply_t)(unsigned int BluetoothStackID, Word_t Connection_Handle, Byte_t Reason, Byte_t *StatusResult, Word_t *Connection_HandleResult);
#endif

#endif
