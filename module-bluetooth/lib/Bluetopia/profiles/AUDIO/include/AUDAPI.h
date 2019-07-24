/*
 * Copyright 2010 - 2014 Stonestreet One.
 * All Rights Reserved.
 * Author:  Damon Lange
 *** MODIFICATION HISTORY ****************************************************
 *
 *   mm/dd/yy  F. Lastname    Description of Modification
 *   --------  -----------    ------------------------------------------------
 *   07/25/10  D. Lange       Initial creation.
 ****************************************************************************
 */

/**
 *  @file AUDAPI.h
 *
 *  @brief  Stonestreet One Bluetooth Stack Audio Manager API Type
 *      Definitions, Constants, and Prototypes.
 *
 *  The audio profile sub-system (AUD) programming interface defines
 *  the protocols and procedures to be used to implement audio (SRC/SNK)
 *  and A/V remote control capabilities (Controller/Target).
 *
 *  @code
 *  #include "SS1BTAUD.h"
 *  @endcode
 *  ============================================================================
 */
#ifndef __AUDAPIH__
#define __AUDAPIH__

#include "SS1BTPS.h"            /*! Bluetooth Stack API Prototypes/Constants. */

#if BTPS_CONFIGURATION_AUD_SUPPORT_REMOTE_CONTROL

   #include "SS1BTAVR.h"        /*! Bluetooth AVRCP API Prototypes/Constants. */

#endif

   /*! Error Return Codes.
    * Error Codes that are smaller than these (less than -1000) are
    * related to the Bluetooth protocol stack itself (see BTERRORS.H).
    */
#define BTAUD_ERROR_INVALID_PARAMETER                             (-2000)
#define BTAUD_ERROR_NOT_INITIALIZED                               (-2001)
#define BTAUD_ERROR_INVALID_BLUETOOTH_STACK_ID                    (-2002)
#define BTAUD_ERROR_ALREADY_INITIALIZED                           (-2003)
#define BTAUD_ERROR_INSUFFICIENT_RESOURCES                        (-2004)
#define BTAUD_ERROR_INVALID_OPERATION                             (-2005)
#define BTAUD_ERROR_UNABLE_TO_INITIALIZE_GAVD                     (-2007)
#define BTAUD_ERROR_UNABLE_TO_INITIALIZE_AVCTP                    (-2008)
#define BTAUD_ERROR_STREAM_NOT_INITIALIZED                        (-2009)
#define BTAUD_ERROR_UNABLE_TO_CONNECT_REMOTE_STREAM               (-2010)
#define BTAUD_ERROR_STREAM_ALREADY_CONNECTED                      (-2011)
#define BTAUD_ERROR_STREAM_NOT_CONNECTED                          (-2012)
#define BTAUD_ERROR_STREAM_CONNECTION_IN_PROGRESS                 (-2013)
#define BTAUD_ERROR_STREAM_IS_ACTIVE                              (-2014)
#define BTAUD_ERROR_STREAM_IS_NOT_ACTIVE                          (-2015)
#define BTAUD_ERROR_STREAM_STATE_ALREADY_CURRENT                  (-2016)
#define BTAUD_ERROR_UNABLE_TO_CHANGE_STREAM_STATE                 (-2017)
#define BTAUD_ERROR_STREAM_STATE_CHANGE_IN_PROGRESS               (-2018)
#define BTAUD_ERROR_STREAM_FORMAT_CHANGE_IN_PROGRESS              (-2019)
#define BTAUD_ERROR_UNSUPPORTED_FORMAT                            (-2020)
#define BTAUD_ERROR_UNABLE_TO_CHANGE_STREAM_FORMAT                (-2021)
#define BTAUD_ERROR_UNABLE_TO_SEND_STREAM_DATA                    (-2022)
#define BTAUD_ERROR_UNABLE_TO_SEND_REMOTE_CONTROL_COMMAND         (-2023)
#define BTAUD_ERROR_UNABLE_TO_SEND_REMOTE_CONTROL_RESPONSE        (-2024)
#define BTAUD_ERROR_REMOTE_DEVICE_NOT_CONNECTED                   (-2025)
#define BTAUD_ERROR_REMOTE_CONTROL_NOT_CONNECTED                  (-2026)
#define BTAUD_ERROR_INVALID_REMOTE_CONTROL_DATA                   (-2027)
#define BTAUD_ERROR_REMOTE_CONTROL_ALREADY_CONNECTED              (-2028)
#define BTAUD_ERROR_REMOTE_CONTROL_CONNECTION_IN_PROGRESS         (-2029)
#define BTAUD_ERROR_REMOTE_CONTROL_NOT_INITIALIZED                (-2030)
#define BTAUD_ERROR_STREAM_CONNECTED                              (-2031)
#define BTAUD_ERROR_SAME_FORMAT                                   (-2032)
#define BTAUD_ERROR_REMOTE_CONTROL_ROLE_NOT_INITIALIZED           (-2033)

   /*! The following enumerated type is used to denote a specific Stream
    * Endpoint Type.
    */
typedef enum
{
   astSNK,
   astSRC
} AUD_Stream_Type_t;

   /*! The following enumerated type is used to specify the current state
    * of an established Stream (either SRC or SNK).
    */
typedef enum
{
   astStreamStopped,
   astStreamStarted
} AUD_Stream_State_t;

   /*! The following structure is used to denote the settings of Stream
    * Data.
    */
typedef struct _tagAUD_Stream_Format_t
{
   unsigned long SampleFrequency;
   unsigned int  NumberChannels;
   unsigned long FormatFlags;
} AUD_Stream_Format_t;

#define AUD_STREAM_FORMAT_SIZE                              (sizeof(AUD_Stream_Format_t))

   /*! The following constants represent the bit-mask values that are
    * used with the StreamFormatFlags member of the AUD_Stream_Format_t
    * to denote various capabilities that are supported by the stream
    * format.
    */
#define AUD_STREAM_FORMAT_FLAGS_CODEC_TYPE_BIT_MASK             0x0F000000

#define AUD_STREAM_FORMAT_FLAGS_SCMS_T_PROTECTION_SUPPORTED     0x80000000

   /*! The following values represent the defined Codec values that are
    * supported by the this module.  These values are used in
    * conjunction with the AUD_STREAM_FORMAT_FLAGS_CODEC_TYPE_BIT_MASK
    * to specify the Codec type that is supported.
    */
#define AUD_STREAM_FORMAT_FLAGS_CODEC_TYPE_SBC                  0x00000000
#define AUD_STREAM_FORMAT_FLAGS_CODEC_TYPE_AAC                  0x01000000
#define AUD_STREAM_FORMAT_FLAGS_CODEC_TYPE_MP3                  0x02000000

   /*! The following flags are specific to the SBC and MP3 Codec Types.
    */
#define AUD_STREAM_FORMAT_FLAGS_DUAL_CHANNEL_NOT_SUPPORTED      0x00000001
#define AUD_STREAM_FORMAT_FLAGS_JOINT_STEREO_NOT_SUPPORTED      0x00000002

   /*! The following flags are specific to the AAC Codec type format.
    */
#define AUD_STREAM_FORMAT_FLAGS_AAC_SUPPORT_MPEG4_LC            0x00000010
#define AUD_STREAM_FORMAT_FLAGS_AAC_SUPPORT_MPEG4_LTP           0x00000020
#define AUD_STREAM_FORMAT_FLAGS_AAC_SUPPORT_MPEG4_SCALABLE      0x00000040
#define AUD_STREAM_FORMAT_FLAGS_AAC_SUPPORT_VBR                 0x00000080

   /*! The following flags are specific to the MP3 Codec type format.
    */
#define AUD_STREAM_FORMAT_FLAGS_MP3_SUPPORT_LAYER_1             0x00000100
#define AUD_STREAM_FORMAT_FLAGS_MP3_SUPPORT_LAYER_2             0x00000200
#define AUD_STREAM_FORMAT_FLAGS_MP3_SUPPORT_LAYER_3             0x00000400
#define AUD_STREAM_FORMAT_FLAGS_MP3_SUPPORT_CRC_PROTECTION      0x00000800
#define AUD_STREAM_FORMAT_FLAGS_MP3_SUPPORT_PAYLOAD_FORMAT_2    0x00001000
#define AUD_STREAM_FORMAT_FLAGS_MP3_SUPPORT_VBR                 0x00002000

   /*! The following constant is used with the AUD_Stream_Configuration_t
    * structure to denote the largest supported Media Codec Information
    * Length that can be supported.
    */
#define AUD_MEDIA_CODEC_INFORMATION_SIZE_MAXIMUM_SIZE           32

   /*! The following structure is a container structure that is used with
    * the AUD_Query_Stream_Configuration() function to query the current
    * Stream Configuration.
    * \note The Media Codec members denote the low level A2DP
    *          information that is being used on the stream.
    * \note All fields of the FormatFlags member of the StreamFormat
    *          member will be cleared except for the Media Codec type
    *          and if SCMS-T Content Protection is currently configured,
    *          then the:
    *             AUD_STREAM_FORMAT_FLAGS_SCMS_T_PROTECTION_SUPPORTED
    *          bit will also be set.  If it is not currently configured
    *          (active) then the content protection bit will be zero.
    */
typedef struct _tagAUD_Stream_Configuration_t
{
   unsigned int        MediaMTU;
   AUD_Stream_Format_t StreamFormat;
   unsigned int        MediaCodecType;
   unsigned int        MediaCodecInfoLength;
   unsigned char       MediaCodecInformation[AUD_MEDIA_CODEC_INFORMATION_SIZE_MAXIMUM_SIZE];
} AUD_Stream_Configuration_t;

#define AUD_STREAM_CONFIGURATION_SIZE                       (sizeof(AUD_Stream_Configuration_t))

   /*! The following structure represents the structure of the Stream
    * Channel Information.  This structure is used with the
    * AUD_Query_Stream_Channel_Info() function.
    */
typedef struct _tagAUD_Stream_Channel_Info_t
{
   Word_t InMTU;
   Word_t OutMTU;
   Word_t LocalCID;
   Word_t RemoteCID;
} AUD_Stream_Channel_Info_t;

#define AUD_STREAM_CHANNEL_INFO_SIZE                        (sizeof(AUD_Stream_Channel_Info_t))

   /*! The following constant represents the maximum number of Stream
    * Formats that are supported by this module (for each Endpoint).
    */
#define AUD_STREAM_FORMAT_MAXIMUM_NUMBER_FORMATS            16

   /*! The following enumerated type is used with the
    * AUD_Open_Request_Indication_Data_t event to denote the type of
    * connection that is being requested.
    */
typedef enum
{
   acrStream,
   acrRemoteControl
} AUD_Connection_Request_Type_t;

   /*! The following enumerated type is used to identify a reason for a
    * Disconnect.
    */
typedef enum
{
   adrRemoteDeviceDisconnect,
   adrRemoteDeviceLinkLoss,
   adrRemoteDeviceTimeout
} AUD_Disconnect_Reason_t;

   /*! The following structure is used with the AUD_Initialize() function
    * to inform the Audio Manager of the supported Capabilities and
    * SDP information.  This structure is used to specify settings for
    * either a SRC or SNK.
    * \note The Stream Formats should be listed in order of
    *          preference (when specifying more than one codec type).
    * \note The Stream Formats should be grouped by Codec type to
    *          conserve memory/endpoints.  For instance all SBC formats
    *          should be listed, followed by all AAC formats (for a
    *          device supporting SBC and AAC and specifying AAC has a
    *          higher priority Codec (when connecting out).
    */
typedef struct _tagAUD_Stream_Initialization_Info_t
{
   unsigned long        InitializationFlags;
   char                *EndpointSDPDescription;
   unsigned int         NumberSupportedStreamFormats;
   AUD_Stream_Format_t  StreamFormat[AUD_STREAM_FORMAT_MAXIMUM_NUMBER_FORMATS];
   unsigned int         NumberConcurrentStreams;
} AUD_Stream_Initialization_Info_t;

#define AUD_STREAM_INITIALIZATION_INFO_SIZE                 (sizeof(AUD_Stream_Initialization_Info_t))

   /*! The following values are used with the InitializationFlags member
    * of the AUD_Stream_Initialization_Info_t structure to specify
    * various initialization/configuration options (for the stream).
    */
#define AUD_STREAM_INITIALIZATION_FLAGS_NUMBER_CONCURRENT_STREAMS_PRESENT  0x80000000
#define AUD_STREAM_INITIALIZATION_FLAGS_GROUP_CONCURRENT_STREAMS           0x40000000

#if BTPS_CONFIGURATION_AUD_SUPPORT_REMOTE_CONTROL

   /*! The following structure specifies the details of an AVRCP Role,
    * either Controller or Target.  The possible values for the
    * SupportedFeaturesFlags are defined in the AVRCP API.
    * \note If AVRCP is to be used, then the AVRCP categories must
    *          be correctly specified when populating this information.
    */
typedef struct _tagAUD_Remote_Control_Role_Info_t
{
   Word_t  SupportedFeaturesFlags;
   char   *ProviderName;
   char   *ServiceName;
} AUD_Remote_Control_Role_Info_t;

#define AUD_REMOTE_CONTROL_ROLE_INFO_SIZE                   (sizeof(AUD_Remote_Control_Role_Info_t))

   /*! The following structure is a container structure that is used to
    * initialize/configure the AVRCP usage.
    */
typedef struct _tagAUD_Remote_Control_Initialization_Info_t
{
   unsigned long                   InitializationFlags;
   AVRCP_Version_t                 SupportedVersion;
   AUD_Remote_Control_Role_Info_t *ControllerRoleInfo;
   AUD_Remote_Control_Role_Info_t *TargetRoleInfo;
} AUD_Remote_Control_Initialization_Info_t;

#define AUD_REMOTE_CONTROL_INITIALIZATION_INFO_SIZE         (sizeof(AUD_Remote_Control_Initialization_Info_t))

#else

#define AUD_Remote_Control_Initialization_Info_t            void

#endif

   /*! The following structure is a container structure that is used to
    * specify the initialization information used to configure this
    * module.  This structure is passed to the AUD_Initialize()
    * function.
    * \note If the AUD_INITIALIZATION_INFO_FLAGS_OVERRIDE_MEDIA_MTU
    *          flag is set in the InitializationFlags then the MediaMTU
    *          member must be set to a valid MTU for the Media channel.
    */
typedef struct _tagAUD_Initialization_Info_t
{
   unsigned long                             InitializationFlags;
   AUD_Stream_Initialization_Info_t         *SRCInitializationInfo;
   AUD_Stream_Initialization_Info_t         *SNKInitializationInfo;
   AUD_Remote_Control_Initialization_Info_t *RemoteControlInitializationInfo;
   Word_t                                    MediaMTU;
} AUD_Initialization_Info_t;

#define AUD_INITIALIZATION_INFO_SIZE                        (sizeof(AUD_Initialization_Info_t))

#define AUD_INITIALIZATION_INFO_FLAGS_OVERRIDE_MEDIA_MTU    0x00000001

   /*! The following enumerated type represents the supported Server
    * Connection Modes supported by the Audio Server.
    */
typedef enum
{
   ausAutomaticAccept,
   ausAutomaticReject,
   ausManualAccept
} AUD_Server_Connection_Mode_t;

   /*! The following structures specifies the RTP Header Information of
    * an encoded audio data packet.
    */
typedef struct _tagAUD_RTP_Header_Info_t
{
   Word_t    SequenceNumber;
   DWord_t   TimeStamp;
   Byte_t    PayloadType;
   Boolean_t Marker;
} AUD_RTP_Header_Info_t;

#define AUD_RTP_HEADER_INFO_SIZE                            (sizeof(AUD_RTP_Header_Info_t))

#if BTPS_CONFIGURATION_AUD_SUPPORT_REMOTE_CONTROL

   /*! The following structure is used with the
    * AUD_Send_Remote_Control_Command() and specifies the data that is
    * to be sent to the remote device.
    * \note This module does NOT support all of the AVRCP Messages.
    *          If there is not a member in the union for a specific
    *          Command type, then it is not currently supported by this
    *          module.
    */
typedef struct _tagAUD_Remote_Control_Command_Data_t
{
   AVRCP_Message_Type_t MessageType;
   union
   {
      AVRCP_Unit_Info_Command_Data_t                                     UnitInfoCommandData;
      AVRCP_Subunit_Info_Command_Data_t                                  SubunitInfoCommandData;
      AVRCP_Pass_Through_Command_Data_t                                  PassThroughCommandData;
      AVRCP_Vendor_Dependent_Generic_Command_Data_t                      VendorDependentGenericCommandData;
      AVRCP_Browsing_Channel_Generic_Message_Data_t                      BrowsingChannelGenericMessageData;
      AVRCP_Group_Navigation_Command_Data_t                              GroupNavigationCommandData;
      AVRCP_Get_Capabilities_Command_Data_t                              GetCapabilitiesCommandData;
      AVRCP_List_Player_Application_Setting_Values_Command_Data_t        ListPlayerApplicationSettingValuesCommandData;
      AVRCP_Get_Current_Player_Application_Setting_Value_Command_Data_t  GetCurrentPlayerApplicationSettingValueCommandData;
      AVRCP_Set_Player_Application_Setting_Value_Command_Data_t          SetPlayerApplicationSettingValueCommandData;
      AVRCP_Get_Player_Application_Setting_Attribute_Text_Command_Data_t GetPlayerApplicationSettingAttributeTextCommandData;
      AVRCP_Get_Player_Application_Setting_Value_Text_Command_Data_t     GetPlayerApplicationSettingValueTextCommandData;
      AVRCP_Inform_Displayable_Character_Set_Command_Data_t              InformDisplayableCharacterSetCommandData;
      AVRCP_Inform_Battery_Status_Of_CT_Command_Data_t                   InformBatteryStatusOfCTCommandData;
      AVRCP_Get_Element_Attributes_Command_Data_t                        GetElementAttributesCommandData;
      AVRCP_Abort_Continuing_Response_Command_Data_t                     AbortContinuingResponseCommandData;
      AVRCP_Register_Notification_Command_Data_t                         RegisterNotificationCommandData;
      AVRCP_Set_Absolute_Volume_Command_Data_t                           SetAbsoluteVolumeCommandData;
      AVRCP_Set_Addressed_Player_Command_Data_t                          SetAddressedPlayerCommandData;
      AVRCP_Play_Item_Command_Data_t                                     PlayItemCommandData;
      AVRCP_Add_To_Now_Playing_Command_Data_t                            AddToNowPlayingCommandData;
      AVRCP_Set_Browsed_Player_Command_Data_t                            SetBrowsedPlayerCommandData;
      AVRCP_Change_Path_Command_Data_t                                   ChangePathCommandData;
      AVRCP_Get_Item_Attributes_Command_Data_t                           GetItemAttributesCommandData;
      AVRCP_Search_Command_Data_t                                        SearchCommandData;
      AVRCP_Get_Folder_Items_Command_Data_t                              GetFolderItemsCommandData;
   } MessageData;
} AUD_Remote_Control_Command_Data_t;

#define AUD_REMOTE_CONTROL_COMMAND_DATA_SIZE                (sizeof(AUD_Remote_Control_Command_Data_t))

   /*! The following structure is used with the
    * AUD_Send_Remote_Control_Response() and specifies the data that is
    * to be sent to the remote device.
    * \note This module does NOT support all of the AVRCP Messages.
    *          If there is not a member in the union for a specific
    *          Response type, then it is not currently supported by this
    *          module.
    */
typedef struct _tagAUD_Remote_Control_Response_Data_t
{
   AVRCP_Message_Type_t MessageType;
   union
   {
      AVRCP_Unit_Info_Response_Data_t                                     UnitInfoResponseData;
      AVRCP_Subunit_Info_Response_Data_t                                  SubunitInfoResponseData;
      AVRCP_Pass_Through_Response_Data_t                                  PassThroughResponseData;
      AVRCP_Vendor_Dependent_Generic_Response_Data_t                      VendorDependentGenericResponseData;
      AVRCP_Browsing_Channel_Generic_Message_Data_t                       BrowsingChannelGenericMessageData;
      AVRCP_Group_Navigation_Response_Data_t                              GroupNavigationResponseData;
      AVRCP_Get_Capabilities_Response_Data_t                              GetCapabilitiesResponseData;
      AVRCP_List_Player_Application_Setting_Attributes_Response_Data_t    ListPlayerApplicationSettingAttributesResponseData;
      AVRCP_List_Player_Application_Setting_Values_Response_Data_t        ListPlayerApplicationSettingValuesResponseData;
      AVRCP_Get_Current_Player_Application_Setting_Value_Response_Data_t  GetCurrentPlayerApplicationSettingValueResponseData;
      AVRCP_Set_Player_Application_Setting_Value_Response_Data_t          SetPlayerApplicationSettingValueResponseData;
      AVRCP_Get_Player_Application_Setting_Attribute_Text_Response_Data_t GetPlayerApplicationSettingAttributeTextResponseData;
      AVRCP_Get_Player_Application_Setting_Value_Text_Response_Data_t     GetPlayerApplicationSettingValueTextResponseData;
      AVRCP_Inform_Displayable_Character_Set_Response_Data_t              InformDisplayableCharacterSetResponseData;
      AVRCP_Inform_Battery_Status_Of_CT_Response_Data_t                   InformBatteryStatusOfCTResponseData;
      AVRCP_Get_Element_Attributes_Response_Data_t                        GetElementAttributesResponseData;
      AVRCP_Get_Play_Status_Response_Data_t                               GetPlayStatusResponseData;
      AVRCP_Register_Notification_Response_Data_t                         RegisterNotificationResponseData;
      AVRCP_Abort_Continuing_Response_Response_Data_t                     AbortContinuingResponseResponseData;
      AVRCP_Set_Absolute_Volume_Response_Data_t                           SetAbsoluteVolumeResponseData;
      AVRCP_Set_Addressed_Player_Response_Data_t                          SetAddressedPlayerResponseData;
      AVRCP_Play_Item_Response_Data_t                                     PlayItemResponseData;
      AVRCP_Add_To_Now_Playing_Response_Data_t                            AddToNowPlayingResponseData;
      AVRCP_Command_Reject_Response_Data_t                                CommandRejectResponseData;
      AVRCP_Set_Browsed_Player_Response_Data_t                            SetBrowsedPlayerResponseData;
      AVRCP_Change_Path_Response_Data_t                                   ChangePathResponseData;
      AVRCP_Get_Item_Attributes_Response_Data_t                           GetItemAttributesResponseData;
      AVRCP_Search_Response_Data_t                                        SearchResponseData;
      AVRCP_Get_Folder_Items_Response_Data_t                              GetFolderItemsResponseData;
      AVRCP_General_Reject_Response_Data_t                                GeneralRejectResponseData;
   } MessageData;
} AUD_Remote_Control_Response_Data_t;

#define AUD_REMOTE_CONTROL_RESPONSE_DATA_SIZE               (sizeof(AUD_Remote_Control_Response_Data_t))

#else

#define AUD_Remote_Control_Command_Data_t       unsigned int
#define AUD_Remote_Control_Response_Data_t      unsigned int

#endif

   /*! Audio Manager Event API Types.
    */
typedef enum
{
   etAUD_Open_Request_Indication, /*!< Event that signals a remote audio stream or remote control profile is attempting a connection. */
   etAUD_Stream_Open_Indication, /*!< Event that signals that the local audio stream (SRC/SNK) now has an active connection. */
   etAUD_Stream_Open_Confirmation, /*!< Event that signals the result of a local audio stream (SRC/SNK) connection to a remote device. */
   etAUD_Stream_Close_Indication, /*!< Event that signals that a currently active audio stream (SRC/SNK) is no longer active. */
   etAUD_Stream_State_Change_Indication, /*!< Event that signals that streaming state of a local audio stream (SRC/SNK) has changed. */
   etAUD_Stream_State_Change_Confirmation, /*!< Event that signals the result of a local audio stream (SRC/SNK) state change request. */
   etAUD_Stream_Format_Change_Indication, /*!< Event that signals that stream format of a local audio stream (SRC/SNK) has changed. */
   etAUD_Stream_Format_Change_Confirmation, /*!< Event that signals the result of a local audio stream (SRC/SNK) format change request. */
   etAUD_Encoded_Audio_Data_Indication, /*!< Event that signals that encoded audio data has been received on the local audio SNK from the currently connected remote audio SRC. */
   etAUD_Remote_Control_Command_Indication, /*!< Event that signals that a AVRCP remote control command has been received by a remote device. */
   etAUD_Remote_Control_Command_Confirmation, /*!< Event that signals the result of a local remote control command request. */
   etAUD_Remote_Control_Open_Indication, /*!< Event that signals that a remote control connection has been established from a specific remote device. */
   etAUD_Remote_Control_Open_Confirmation, /*!< Event that signals the result of an outgoing remote control connection. */
   etAUD_Remote_Control_Close_Indication, /*!< Event that signal that a remote control connection is no longer established to a specific remote device. */
   etAUD_Signalling_Channel_Open_Indication, /*!< Event that signals that a signalling channel has been established to/from a specific remote device. */
   etAUD_Signalling_Channel_Close_Indication, /*!< Event that signals that a signaling channel is no longer established to a specific remote device. */
   etAUD_Browsing_Channel_Open_Indication, /*!< Event that signals that a remote Browsing service is connected to the local Browsing service. */
   etAUD_Browsing_Channel_Open_Confirmation, /*!< Event that signals that an attempt to connect to a remote AUD Browsing Channel is complete. */
   etAUD_Browsing_Channel_Close_Indication /*!< Event that signals that remote device disconnects the Browsing Channel from the Local service. */
} AUD_Event_Type_t;

   /*! The following event is dispatched when a remote service is
    * requesting a connection to the local Audio Manager service.  The
    * BD_ADDR specifies the Bluetooth address of the Remote device that
    * is connecting.
    * \note This event is only dispatched to servers that are in
    *            Manual Accept Mode.
    * \note This event must be responded to with the
    *            AUD_Open_Request_Response() function in order to
    *            accept or reject the outstanding Open Request.
    */
typedef struct _tagAUD_Open_Request_Indication_Data_t
{
   BD_ADDR_t                     BD_ADDR;
   AUD_Connection_Request_Type_t ConnectionRequestType;
} AUD_Open_Request_Indication_Data_t;

#define AUD_OPEN_REQUEST_INDICATION_DATA_SIZE               (sizeof(AUD_Open_Request_Indication_Data_t))

   /*! The following Event is dispatched when a remote device connects to
    * the local device.  The BD_ADDR member specifies the address of the
    * remote device which is connecting to the local device.  The
    * StreamType member defines whether or not the remote device has
    * connected to the SRC endpoint or the SNK endpoint.
    * \note All fields of the FormatFlags member of the StreamFormat
    *          member will be cleared except for the Media Codec type
    *          and if SCMS-T Content Protection is currently configured,
    *          then the:
    *             AUD_STREAM_FORMAT_FLAGS_SCMS_T_PROTECTION_SUPPORTED
    *          bit will also be set.  If it is not currently configured
    *          (active) then the content protection bit will be zero.
    */
typedef struct _tagAUD_Stream_Open_Indication_Data_t
{
   BD_ADDR_t           BD_ADDR;
   unsigned int        MediaMTU;
   AUD_Stream_Type_t   StreamType;
   AUD_Stream_Format_t StreamFormat;
} AUD_Stream_Open_Indication_Data_t;

#define AUD_STREAM_OPEN_INDICATION_DATA_SIZE                (sizeof(AUD_Stream_Open_Indication_Data_t))

   /*! The following Event is dispatched to indicate success or failure
    * of a previously submitted connection request.  The OpenStatus
    * member specifies the status of the connection attempt.  Possible
    * values for the open status can be found below.
    * \note All fields of the FormatFlags member of the StreamFormat
    *          member will be cleared except for the Media Codec type
    *          and if SCMS-T Content Protection is currently configured,
    *          then the:
    *             AUD_STREAM_FORMAT_FLAGS_SCMS_T_PROTECTION_SUPPORTED
    *          bit will also be set.  If it is not currently configured
    *          (active) then the content protection bit will be zero.
    */
typedef struct _tagAUD_Stream_Open_Confirmation_Data_t
{
   BD_ADDR_t           BD_ADDR;
   unsigned int        OpenStatus;
   unsigned int        MediaMTU;
   AUD_Stream_Type_t   StreamType;
   AUD_Stream_Format_t StreamFormat;
} AUD_Stream_Open_Confirmation_Data_t;

#define AUD_STREAM_OPEN_CONFIRMATION_DATA_SIZE              (sizeof(AUD_Stream_Open_Confirmation_Data_t))

   /*! The following constants represent the status values for the
    * OpenStatus member of the AUD_Stream_Open_Confirmation_Data_t
    * event.
    */
#define AUD_STREAM_OPEN_CONFIRMATION_STATUS_SUCCESS            0x00000000
#define AUD_STREAM_OPEN_CONFIRMATION_STATUS_CONNECTION_TIMEOUT 0x00000001
#define AUD_STREAM_OPEN_CONFIRMATION_STATUS_CONNECTION_REFUSED 0x00000002
#define AUD_STREAM_OPEN_CONFIRMATION_STATUS_UNKNOWN_ERROR      0x00000003

   /*! The following event is dispatched when a remote device disconnects
    * from the local device.  The BD_ADDR member specifies the Bluetooth
    * device address of the device that is disconnecting.  This Event is
    * NOT dispatched in response to the local host requesting the
    * disconnection.  This Event is dispatched only when the remote
    * device terminates the Connection (and/or Bluetooth Link
    * terminates).
    */
typedef struct _tagAUD_Stream_Close_Indication_Data_t
{
   BD_ADDR_t               BD_ADDR;
   AUD_Stream_Type_t       StreamType;
   AUD_Disconnect_Reason_t DisconnectReason;
} AUD_Stream_Close_Indication_Data_t;

#define AUD_STREAM_CLOSE_INDICATION_DATA_SIZE               (sizeof(AUD_Stream_Close_Indication_Data_t))

   /*! The following event is dispatched when the Stream Endpoint State
    * changes.  The BD_ADDR member specifies the Bluetooth device
    * address of the device that caused the Stream State Change.  The
    * StreamType and StreamState members represent the type of Stream
    * Endpoint and the State of the Stream, respectively.
    */
typedef struct _tagAUD_Stream_State_Change_Indication_Data_t
{
   BD_ADDR_t          BD_ADDR;
   AUD_Stream_Type_t  StreamType;
   AUD_Stream_State_t StreamState;
} AUD_Stream_State_Change_Indication_Data_t;

#define AUD_STREAM_STATE_CHANGE_INDICATION_DATA_SIZE        (sizeof(AUD_Stream_State_Change_Indication_Data_t))

   /*! The following event is dispatched when the Stream Endpoint State
    * changes.  The BD_ADDR member specifies the Bluetooth device
    * address of the device that caused the Stream State Change.  The
    * StreamType and StreamState members represent the type of Stream
    * Endpoint and the State of the Stream, respectively.
    */
typedef struct _tagAUD_Stream_State_Change_Confirmation_Data_t
{
   BD_ADDR_t          BD_ADDR;
   Boolean_t          Successful;
   AUD_Stream_Type_t  StreamType;
   AUD_Stream_State_t StreamState;
} AUD_Stream_State_Change_Confirmation_Data_t;

#define AUD_STREAM_STATE_CHANGE_CONFIRMATION_DATA_SIZE      (sizeof(AUD_Stream_State_Change_Confirmation_Data_t))

   /*! The following event is dispatched when the Stream Endpoint Format
    * changes.  The BD_ADDR member specifies the Bluetooth device
    * address of the device that caused the Stream Format Change.  The
    * StreamType and StreamFormat members represent the type of Stream
    * Endpoint and the Format of the Stream, respectively.
    * \note All fields of the FormatFlags member of the StreamFormat
    *          member will be cleared except for the Media Codec type
    *          and if SCMS-T Content Protection is currently configured,
    *          then the:
    *             AUD_STREAM_FORMAT_FLAGS_SCMS_T_PROTECTION_SUPPORTED
    *          bit will also be set.  If it is not currently configured
    *          (active) then the content protection bit will be zero.
    */
typedef struct _tagAUD_Stream_Format_Change_Indication_Data_t
{
   BD_ADDR_t           BD_ADDR;
   AUD_Stream_Type_t   StreamType;
   AUD_Stream_Format_t StreamFormat;
} AUD_Stream_Format_Change_Indication_Data_t;

#define AUD_STREAM_FORMAT_CHANGE_INDICATION_DATA_SIZE       (sizeof(AUD_Stream_Format_Change_Indication_Data_t))

   /*! The following event is dispatched when the Stream Endpoint Format
    * changes.  The BD_ADDR member specifies the Bluetooth device
    * address of the device that caused the Stream Format Change.  The
    * StreamType and StreamFormat members represent the type of Stream
    * Endpoint and the Format of the Stream, respectively.
    * \note All fields of the FormatFlags member of the StreamFormat
    *          member will be cleared except for the Media Codec type
    *          and if SCMS-T Content Protection is currently configured,
    *          then the:
    *             AUD_STREAM_FORMAT_FLAGS_SCMS_T_PROTECTION_SUPPORTED
    *          bit will also be set.  If it is not currently configured
    *          (active) then the content protection bit will be zero.
    */
typedef struct _tagAUD_Stream_Format_Change_Confirmation_Data_t
{
   BD_ADDR_t           BD_ADDR;
   Boolean_t           Successful;
   AUD_Stream_Type_t   StreamType;
   AUD_Stream_Format_t StreamFormat;
} AUD_Stream_Format_Change_Confirmation_Data_t;

#define AUD_STREAM_FORMAT_CHANGE_CONFIRMATION_DATA_SIZE     (sizeof(AUD_Stream_Format_Change_Confirmation_Data_t))

   /*! The following event is dispatched when Data is received on a SNK
    * Stream Endpoint.  This indication informs how many raw encoded
    * bytes were received.  It is the event handlers responsibility to
    * process and decode the data into caller supplied buffers.  Failure
    * to do this will cause the data to be thrown away.
    */
typedef struct _tagAUD_Encoded_Audio_Data_Indication_Data_t
{
   BD_ADDR_t              BD_ADDR;
   unsigned int           RawAudioDataFrameLength;
   unsigned char         *RawAudioDataFrame;
   Word_t                 SequenceNumber;
   AUD_RTP_Header_Info_t *RTPHeaderInfo;
} AUD_Encoded_Audio_Data_Indication_Data_t;

#define AUD_ENCODED_AUDIO_DATA_INDICATION_DATA_SIZE         (sizeof(AUD_Encoded_Audio_Data_Indication_Data_t))

   /*! The following event is dispatched when a Remote Control Event is
    * received to control a SRC Stream Endpoint.  This indication
    * informs of the type of Remote Control Event that occurred as well
    * as providing the data for the event itself.
    */
typedef struct _tagAUD_Remote_Control_Command_Indication_Data_t
{
   BD_ADDR_t                         BD_ADDR;
   unsigned int                      TransactionID;
   AUD_Remote_Control_Command_Data_t RemoteControlCommandData;
} AUD_Remote_Control_Command_Indication_Data_t;

#define AUD_REMOTE_CONTROL_COMMAND_INDICATION_DATA_SIZE     (sizeof(AUD_Remote_Control_Command_Indication_Data_t))

   /*! The following event is dispatched when a Remote Control
    * Confirmation Event is received from a remote device (in response
    * to a Remote Control message being sent).  This confirmation
    * informs of the type of Remote Control Event that occurred as well
    * as providing the data for the event itself.
    */
typedef struct _tagAUD_Remote_Control_Command_Confirmation_Data_t
{
   BD_ADDR_t                          BD_ADDR;
   unsigned int                       TransactionID;
   unsigned int                       ConfirmationStatus;
   AUD_Remote_Control_Response_Data_t RemoteControlResponseData;
} AUD_Remote_Control_Command_Confirmation_Data_t;

#define AUD_REMOTE_CONTROL_COMMAND_CONFIRMATION_DATA_SIZE   (sizeof(AUD_Remote_Control_Command_Confirmation_Data_t))

   /*! The following constants represent the status values for the
    * ConfirmationStatus member of the
    * AUD_Remote_Control_Command_Confirmation_Data_t event.
    */
#define AUD_REMOTE_CONTROL_COMMAND_CONFIRMATION_STATUS_SUCCESS          0x00000000
#define AUD_REMOTE_CONTROL_COMMAND_CONFIRMATION_STATUS_TIMEOUT          0x00000001
#define AUD_REMOTE_CONTROL_COMMAND_CONFIRMATION_STATUS_UNKNOWN_ERROR    0x00000002

   /*! The following Event is dispatched when a remote device connects to
    * the local device.  The BD_ADDR member specifies the address of the
    * remote device which is connecting to the local device.
    */
typedef struct _tagAUD_Remote_Control_Open_Indication_Data_t
{
   BD_ADDR_t BD_ADDR;
} AUD_Remote_Control_Open_Indication_Data_t;

#define AUD_REMOTE_CONTROL_OPEN_INDICATION_DATA_SIZE        (sizeof(AUD_Remote_Control_Open_Indication_Data_t))

   /*! The following Event is dispatched to indicate success or failure
    * of a previously submitted remote control connection request.  The
    * OpenStatus member specifies the status of the connection attempt.
    * Possible values for the open status can be found below.
    */
typedef struct _tagAUD_Remote_Control_Open_Confirmation_Data_t
{
   BD_ADDR_t    BD_ADDR;
   unsigned int OpenStatus;
} AUD_Remote_Control_Open_Confirmation_Data_t;

#define AUD_REMOTE_CONTROL_OPEN_CONFIRMATION_DATA_SIZE      (sizeof(AUD_Remote_Control_Open_Confirmation_Data_t))

   /*! The following constants represent the status values for the
    * OpenStatus member of the
    * AUD_Remote_Control_Open_Confirmation_Data_t event.
    */
#define AUD_REMOTE_CONTROL_OPEN_CONFIRMATION_STATUS_SUCCESS             0x00000000
#define AUD_REMOTE_CONTROL_OPEN_CONFIRMATION_STATUS_CONNECTION_TIMEOUT  0x00000001
#define AUD_REMOTE_CONTROL_OPEN_CONFIRMATION_STATUS_CONNECTION_REFUSED  0x00000002
#define AUD_REMOTE_CONTROL_OPEN_CONFIRMATION_STATUS_UNKNOWN_ERROR       0x00000003

   /*! The following event is dispatched when a remote device disconnects
    * from the local device.  The BD_ADDR member specifies the Bluetooth
    * device address of the device that is disconnecting.  This Event is
    * NOT dispatched in response to the local host requesting the
    * disconnection.  This Event is dispatched only when the remote
    * device terminates the remote control connection (and/or Bluetooth
    * Link terminates).
    */
typedef struct _tagAUD_Remote_Control_Close_Indication_Data_t
{
   BD_ADDR_t               BD_ADDR;
   AUD_Disconnect_Reason_t DisconnectReason;
} AUD_Remote_Control_Close_Indication_Data_t;

#define AUD_REMOTE_CONTROL_CLOSE_INDICATION_DATA_SIZE       (sizeof(AUD_Remote_Control_Close_Indication_Data_t))

   /*! The following Event is dispatched when the AVDTP/GAVD signalling
    * channel is established to the specified remote device.  The
    * BD_ADDR member specifies the address of the remote device which
    * has connected the AVDTP/GAVD signalling channel to the local
    * device.
    */
typedef struct _tagAUD_Signalling_Channel_Open_Indication_Data_t
{
   BD_ADDR_t BD_ADDR;
} AUD_Signalling_Channel_Open_Indication_Data_t;

#define AUD_SIGNALLING_CHANNEL_OPEN_INDICATION_DATA_SIZE    (sizeof(AUD_Signalling_Channel_Open_Indication_Data_t))

   /*! The following Event is dispatched when the AVDTP/GAVD signalling
    * channel is disconnected from the specified remote device.  The
    * BD_ADDR member specifies the address of the remote device which
    * has disconnected the AVDTP/GAVD signalling channel from the local
    * device.
    */
typedef struct _tagAUD_Signalling_Channel_Close_Indication_Data_t
{
   BD_ADDR_t               BD_ADDR;
   AUD_Disconnect_Reason_t DisconnectReason;
} AUD_Signalling_Channel_Close_Indication_Data_t;

#define AUD_SIGNALLING_CHANNEL_CLOSE_INDICATION_DATA_SIZE   (sizeof(AUD_Signalling_Channel_Close_Indication_Data_t))

   /*! The following event is dispatched when a remote Browsing service
    * gets connected to the local Browsing service.  The BD_ADDR
    * specifies the Bluetooth Address of the Remote Device that is
    * connected.  The MTU parameter specifies the maximum allowable
    * packet data payload that can be sent (Maximum Transmission Unit).
    * The MTU is required because the Browsing Channel does not support
    * AUD Message Fragmentation (i.e.  each individual message *MUST*
    * fit within this size).
    */
typedef struct _tagAUD_Browsing_Channel_Open_Indication_Data_t
{
   BD_ADDR_t    BD_ADDR;
   Word_t       MTU;
} AUD_Browsing_Channel_Open_Indication_Data_t;

#define AUD_BROWSING_CHANNEL_OPEN_INDICATION_DATA_SIZE             (sizeof(AUD_Browsing_Channel_Open_Indication_Data_t))

   /*! The following event is dispatched to the application when an
    * attempt to connect to a remote AUD Browsing Channel is complete.
    * The BD_ADDR specifies the Bluetooth Address of the Remote Device
    * that the connection was attempted.  The OpenStatus parameter is
    * zero if successful or a negative error code in case of failure.
    * The MTU parameter specifies the maximum allowable packet data
    * payload that can be sent (Maximum Transmission Unit).  The MTU is
    * required because the Browsing Channel does not support AUD Message
    * Fragmentation (i.e.  each individual message *MUST* fit within
    * this size).
    */
typedef struct _tagAUD_Browsing_Channel_Open_Confirmation_Data_t
{
   BD_ADDR_t    BD_ADDR;
   unsigned int OpenStatus;
   Word_t       MTU;
} AUD_Browsing_Channel_Open_Confirmation_Data_t;

#define AUD_BROWSING_CHANNEL_OPEN_CONFIRMATION_DATA_SIZE           (sizeof(AUD_Browsing_Channel_Open_Confirmation_Data_t))

   /*! The following constants represent the status values for the
    * OpenStatus member of the
    * AUD_Remote_Control_Open_Confirmation_Data_t event.
    */
#define AUD_BROWSE_CHANNEL_OPEN_CONFIRMATION_STATUS_SUCCESS             0x00000000
#define AUD_BROWSE_CHANNEL_OPEN_CONFIRMATION_STATUS_CONNECTION_TIMEOUT  0x00000001
#define AUD_BROWSE_CHANNEL_OPEN_CONFIRMATION_STATUS_CONNECTION_REFUSED  0x00000002
#define AUD_BROWSE_CHANNEL_OPEN_CONFIRMATION_STATUS_UNKNOWN_ERROR       0x00000003
#define AUD_BROWSE_CHANNEL_OPEN_CONFIRMATION_STATUS_UNSUPPORTED         0x00000004

   /*! The following event is dispatched to the application when the
    * remote device disconnects the Browsing Channel from the Local
    * service.  The BD_ADDR specifies the Bluetooth Address of the
    * Remote Device that has disconnected the Browsing Channel.
    */
typedef struct _tagAUD_Browsing_Channel_Close_Indication_Data_t
{
   BD_ADDR_t    BD_ADDR;
} AUD_Browsing_Channel_Close_Indication_Data_t;

#define AUD_BROWSING_CHANNEL_CLOSE_INDICATION_DATA_SIZE          (sizeof(AUD_Browsing_Channel_Close_Indication_Data_t))

   /*! The following structure represents the container structure for
    * Holding all Audio Manager Event Data Data.
    */
typedef struct _tagAUD_Event_Data_t
{
   AUD_Event_Type_t Event_Data_Type;
   Word_t           Event_Data_Size;
   union
   {
      AUD_Open_Request_Indication_Data_t              *AUD_Open_Request_Indication_Data;
      AUD_Stream_Open_Indication_Data_t               *AUD_Stream_Open_Indication_Data;
      AUD_Stream_Open_Confirmation_Data_t             *AUD_Stream_Open_Confirmation_Data;
      AUD_Stream_Close_Indication_Data_t              *AUD_Stream_Close_Indication_Data;
      AUD_Stream_State_Change_Indication_Data_t       *AUD_Stream_State_Change_Indication_Data;
      AUD_Stream_State_Change_Confirmation_Data_t     *AUD_Stream_State_Change_Confirmation_Data;
      AUD_Stream_Format_Change_Indication_Data_t      *AUD_Stream_Format_Change_Indication_Data;
      AUD_Stream_Format_Change_Confirmation_Data_t    *AUD_Stream_Format_Change_Confirmation_Data;
      AUD_Encoded_Audio_Data_Indication_Data_t        *AUD_Encoded_Audio_Data_Indication_Data;
      AUD_Remote_Control_Command_Indication_Data_t    *AUD_Remote_Control_Command_Indication_Data;
      AUD_Remote_Control_Command_Confirmation_Data_t  *AUD_Remote_Control_Command_Confirmation_Data;
      AUD_Remote_Control_Open_Indication_Data_t       *AUD_Remote_Control_Open_Indication_Data;
      AUD_Remote_Control_Open_Confirmation_Data_t     *AUD_Remote_Control_Open_Confirmation_Data;
      AUD_Remote_Control_Close_Indication_Data_t      *AUD_Remote_Control_Close_Indication_Data;
      AUD_Signalling_Channel_Open_Indication_Data_t   *AUD_Signalling_Channel_Open_Indication_Data;
      AUD_Signalling_Channel_Close_Indication_Data_t  *AUD_Signalling_Channel_Close_Indication_Data;
      AUD_Browsing_Channel_Open_Indication_Data_t     *AUD_Browsing_Channel_Open_Indication_Data;
      AUD_Browsing_Channel_Open_Confirmation_Data_t   *AUD_Browsing_Channel_Open_Confirmation_Data;
      AUD_Browsing_Channel_Close_Indication_Data_t    *AUD_Browsing_Channel_Close_Indication_Data;
   } Event_Data;
} AUD_Event_Data_t;

#define AUD_EVENT_DATA_SIZE                              (sizeof(AUD_Event_Data_t))

   /*! The following declared type represents the Prototype Function for
    * an Audio Manager Event Callback.  This function will be called
    * whenever an Audio Manager Event occurs that is associated with the
    * specified Bluetooth stack ID.  This function passes to the caller
    * the Bluetooth Stack ID, the Audio Event Data that occurred and the
    * Audio Manager Event Callback Parameter that was specified when
    * this Callback was installed.  The caller is free to use the
    * contents of the Audio Manager Event Data ONLY in the context of
    * this callback.  If the caller requires the Data for a longer
    * period of time, then the callback function MUST copy the data into
    * another Data Buffer.  This function is guaranteed NOT to be
    * invoked more than once simultaneously for the specified installed
    * callback (i.e. this function DOES NOT have be reentrant).  It
    * Needs to be noted however, that if the same Callback is installed
    * more than once, then the callbacks will be called serially.
    * Because of this, the processing in this function should be as
    * efficient as possible.  It should also be noted that this function
    * is called in the Thread Context of a Thread that the User does NOT
    * own.  Therefore, processing in this function should be as
    * efficient as possible (this argument holds anyway because another
    * Audio Manager Event will not be processed while this function call
    * is outstanding).
    * \note This function MUST NOT Block and wait for events that
    *            can only be satisfied by Receiving Audio Events.  A
    *            Deadlock WILL occur because NO Audio Event Callbacks
    *            will be issued while this function is currently
    *            outstanding.
    */
typedef void (BTPSAPI *AUD_Event_Callback_t)(unsigned int BluetoothStackID, AUD_Event_Data_t *AUD_Event_Data, unsigned long CallbackParameter);

   /*! @brief The following function is responsible for Registering an Audio
    * Manager.  Note that only one Audio Manager can be Registered for
    * each Bluetooth stack. @param BluetoothStackID The first parameter is the Bluetooth stack
    * ID of the Bluetooth stack which this Server is to be associated
    * with.  @param InitializationInfo The second parameter to this function is the Audio Manager
    * Configuration Specification. @param EventCallback The third parameter is the
    * Audio Manager Event Callback function. @param CallbackParameter The fourth parameter is the Callback parameter,
    * of the Audio Manager Event Callback that is to process any further events
    * associated with this Audio Manager.
    * @return This function returns zero if successful, or a negative return
    * error code if an error occurred (see BTERRORS.H).
    */
BTPSAPI_DECLARATION int BTPSAPI AUD_Initialize(unsigned int BluetoothStackID, AUD_Initialization_Info_t *InitializationInfo, AUD_Event_Callback_t EventCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AUD_Initialize_t)(unsigned int BluetoothStackID, AUD_Initialization_Info_t *InitializationInfo, AUD_Event_Callback_t EventCallback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is responsible for Unregistering an Audio
    * Manager (which was Registered by a successful call to either the
    * AUD_Initialize() function. @param BluetoothStackID This function accepts as input the
    * Bluetooth stack ID of the Bluetooth protocol stack that the Audio
    * Manager was registered for.  @return This function returns zero if
    * successful, or a negative return error code if an error occurred
    * (see BTERRORS.H).
    */
BTPSAPI_DECLARATION int BTPSAPI AUD_Un_Initialize(unsigned int BluetoothStackID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AUD_Un_Initialize_t)(unsigned int BluetoothStackID);
#endif

   /*! @brief The following function is a utility function that exists to allow
    * the caller the ability to change the supported Media Codec
    * capabilities that are used by the module. @param BluetoothStackID The first
    * parameter is the Bluetooth Stack ID of the Audio Manager.
    * @param StreamType The second parameter is the Audio stream type
    * of the stream to update the SBC media codec capabilities.
    * \note Values for this parameter can either be astSNK or astSRC.
    * @param MinimumBitPool The third parameter specifies the new minimum
    * bit pool value that is to be used for the SBC Media Codec Capabilities
    * for the stream endpoint.
    * @param MaximumBitPool The fourth parameter specifies the new maximum bit pool
    * value that is to be used for the SBC Media Codec Capabilities for the stream endpoint.
    * @return This function returns zero if successful or a negative return
    * error code if there was an error.
    * \note This function can only be called when there is NO active
    *          Stream connections.
    */
BTPSAPI_DECLARATION int BTPSAPI AUD_Change_Media_Codec_Parameters(unsigned int BluetoothStackID, AUD_Stream_Type_t StreamType, unsigned int MinimumBitPool, unsigned int MaximumBitPool);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AUD_Change_Media_Codec_Parameters_t)(unsigned int BluetoothStackID, AUD_Stream_Type_t StreamType, unsigned int MinimumBitPool, unsigned int MaximumBitPool);
#endif

   /*! @brief The following function is responsible for responding to an
    * individual request to connect to any portion of the Audio Manager
    * (AVRCP or GAVD).  @param BluetoothStackID The first parameter to this function is the
    * Bluetooth stack ID of the Bluetooth stack associated with the
    * Audio Manager that is responding to the request.
    * @param BD_ADDR The second parameter specifies the Bluetooth device address of the device
    * that is connecting. @param ConnectionRequestType The third parameter specifies the connection
    * request that is being responded. \note Valid values for this parameter are
    * defined as either acrStream or acrRemoteControl.
    * @param AcceptConnection The final parameter to this function
    * specifies whether to accept the pending connection request (or to
    * reject the request).  @return This function returns zero if successful, or
    * a negative return error code if an error occurred.
    * \note The connection to the server is not established until
    *            an etAUD_Open_Indication event has occurred.
    * \note This event will only be dispatched if the server mode
    *            was explicitly set to ausManualAccept via the
    *            AUD_Set_Server_Connection_Mode() function.
    */
BTPSAPI_DECLARATION int BTPSAPI AUD_Open_Request_Response(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, AUD_Connection_Request_Type_t ConnectionRequestType, Boolean_t AcceptConnection);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AUD_Open_Request_Response_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, AUD_Connection_Request_Type_t ConnectionRequestType, Boolean_t AcceptConnection);
#endif

   /*! @brief The following function is responsible for opening a remote
    * streaming endpoint on the specified remote device. @param BluetoothStackID This function
    * accepts as input the Bluetooth stack ID of the Bluetooth protocol
    * stack that the requested Audio Manager is present. @param BD_ADDR The second parameter
    * is the remote  Bluetooth device address. @param StreamType The final parameter is the local Stream type.
    * @return This function returns zero if successful or a negative return error code if
    * there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI AUD_Open_Remote_Stream(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, AUD_Stream_Type_t StreamType);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AUD_Open_Remote_Stream_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, AUD_Stream_Type_t StreamType);
#endif

   /*! @brief The following function is responsible for Closing a currently open
    * stream endpoint on the local device. @param BluetoothStackID This function accepts as
    * input the Bluetooth stack ID of the Bluetooth protocol stack that
    * the requested Audio Manager is present. @param BD_ADDR The second parameter is the remote
    * device address of the connected stream. @param StreamType The final parameter is the stream
    * endpoint type to close (local Stream Endpoint).
    * @return This function returns zero if successful or a negative return error code if
    * there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI AUD_Close_Stream(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, AUD_Stream_Type_t StreamType);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AUD_Close_Stream_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, AUD_Stream_Type_t StreamType);
#endif

   /*! @brief The following function is responsible for opening a remote control
    * connection to the specified remote device. @param BluetoothStackID This function accepts
    * as input the Bluetooth stack ID of the Bluetooth protocol stack
    * that the requested Audio Manager is present. @param BD_ADDR The final paramter is the
    * remote Bluetooth device address of the device to connect with.
    * @return This function returns zero if successful or a negative return
    * error code if there was an error.
    * \note This function should **ONLY** be used if the caller
    *          wishes to create a Remote Control connection to the
    *          remote device but *DOES NOT* want to create a stream
    *          connection. The reason is that this module will
    *          automatically create a remote control connection to the
    *          remote device when a stream connection is made (if the
    *          remote device supports a remote control connection).
    * \note This function does NOT open the audio stream, it ONLY
    *          makes an remote control (if one is not already
    *          established).
    * \note If this function is called to establish a remote control
    *          connection (and the connection is successful), the
    *          caller *MUST* call the AUD_Close_Remote_Control()
    *          function to disconnect the remote control connection.
    */
BTPSAPI_DECLARATION int BTPSAPI AUD_Open_Remote_Control(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AUD_Open_Remote_Control_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR);
#endif

   /*! @brief The following function is responsible for closing a currently open
    * remote control connection on the local device. @param BluetoothStackID This function
    * accepts as input the Bluetooth stack ID of the Bluetooth protocol
    * stack that the requested Audio Manager is present. @param BD_ADDR The final parameter is the
    * device address of the remote control connection close.
    * @return This function returns zero if successful or a negative return error
    * code if there was an error.
    * \note This function should only be called if the local device
    *          previously issued a call to the AUD_Open_Remote_Control()
    *          function and a remote control connection was successfully
    *          established.
    */
BTPSAPI_DECLARATION int BTPSAPI AUD_Close_Remote_Control(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AUD_Close_Remote_Control_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR);
#endif

   /*! @brief This function is responsible for initiating a Browsing Channel
    * connection to a remote device.  It will try to establish an
    * L2CAP channel if no channel exists to the remote device. @param BluetoothStackID This
    * function accepts as input the Bluetooth stack ID of the Bluetooth
    * protocol stack that the requested Audio Manager is present. @param BD_ADDR The
    * second parameter is the Bluetooth Address of the remote device
    * this profile wants to Open to. @return This function returns zero if
    * successful or a negative return error code if unsuccessful.
    * \note A Browsing Channel can *ONLY* be added if there already
    *          exists an on-going AVCTP connection between the local
    *          device and the remote device already.
    * \note The Browsing Channel cannot exist without a
    *          corresponding AVCTP connection.  This means that if the
    *          AVCTP connection is terminated, the Browsing Channel
    *          connection will be terminated as well.
    */
BTPSAPI_DECLARATION int BTPSAPI AUD_Open_Browsing_Channel(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AUD_Open_Browsing_Channel_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR);
#endif

   /*! @brief This function is responsible for disconnecting any Opened Browsing
    * channel to the specified remote device. @param BluetoothStackID The first
    * parameter is the Stack ID corresponding to the AUD context that we
    * need to use. @param BD_ADDR The final paramter is the Bluetooth Address of the remote
    * device to disconnect the browsing channel from. @return The function
    * returns zero if successful and a negative return code on failure.
    */
BTPSAPI_DECLARATION int BTPSAPI AUD_Close_Browsing_Channel(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AUD_Close_Browsing_Channel_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR);
#endif

   /*! @brief The following function is responsible for Changing the Stream
    * State of a currently opened stream endpoint on the local device.
    * @param BluetoothStackID This function accepts as input the Bluetooth stack ID of the
    * Bluetooth protocol stack that the requested Audio Manager is
    * present. @param BD_ADDR The second parameter is the remote device address of the connected
    * stream. @param StreamType The third parameter is the stream endpoint type to change the state
    * of (local Stream Endpoint). @param StreamState The final parameter  is the new Stream Endpoint
    * state.  @return This function returns zero if successful or a negative
    * return error code if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI AUD_Change_Stream_State(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, AUD_Stream_Type_t StreamType, AUD_Stream_State_t StreamState);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AUD_Change_Stream_State_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, AUD_Stream_Type_t StreamType, AUD_Stream_State_t StreamState);
#endif

   /*! @brief The following function is responsible for querying the current
    * Stream State of a currently opened stream endpoint on the local
    * device. @param BluetoothStackID This function accepts as input the Bluetooth stack ID of
    * the Bluetooth protocol stack that the requested Audio Manager is
    * present. @param BD_ADDR The second paramter is the remote device address of the connected
    * stream. @param StreamType The third parameter is the stream endpoint type to query the state of
    * (local Stream Endpoint). @param StreamState The final paramter is a pointer to a buffer that is
    * receive the current Stream Endpoint state.  @return This function returns
    * zero if successful or a negative return error code if there was an
    * error.
    */
BTPSAPI_DECLARATION int BTPSAPI AUD_Query_Stream_State(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, AUD_Stream_Type_t StreamType, AUD_Stream_State_t *StreamState);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AUD_Query_Stream_State_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, AUD_Stream_Type_t StreamType, AUD_Stream_State_t *StreamState);
#endif

   /*! @brief The following function is responsible for Changing the Stream
    * Format of a currently opened stream endpoint on the local device.
    * @param BluetoothStackID This function accepts as input the Bluetooth stack ID of the
    * Bluetooth protocol stack that the requested Audio Manager is
    * present. @param BD_ADDR The second parameter is the remote device address of the connected
    * stream. @param StreamType The third paramter is the stream endpoint type to change the format
    * of (local Stream Endpoint). @param StreamFormat The fourth paramter is the new Stream Endpoint
    * format. @return This function returns zero if successful or a negative
    * return error code if there was an error.
    * \note The Stream Format can ONLY be changed when the stream
    *          state is stopped.
    * \note The stream codec type cannot be changed.  If a stream
    *          has been configured for a specific codec type, this
    *          function cannot be used to change the codec type.  The
    *          stream must be disconnected and reconnected and the
    *          alternate codec type must be selected.  In fact, the
    *          codec type bit mask value is ignored by this function.
    */
BTPSAPI_DECLARATION int BTPSAPI AUD_Change_Stream_Format(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, AUD_Stream_Type_t StreamType, AUD_Stream_Format_t *StreamFormat);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AUD_Change_Stream_Format_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, AUD_Stream_Type_t StreamType, AUD_Stream_Format_t *StreamFormat);
#endif

   /*! @brief The following function is responsible for querying the current
    * Stream Format of a currently opened stream endpoint on the local
    * device. @param BluetoothStackID This function accepts as input the Bluetooth stack ID of
    * the Bluetooth protocol stack that the requested Audio Manager is
    * present. @param BD_ADDR The second parameter is the remote device address of the connected
    * stream. @param StreamType The third paramter is the stream endpoint type to query the format
    * of (local Stream Endpoint). @param StreamFormat The final paramter is a pointer to a buffer that
    * is receive the current Stream Endpoint format.
    * @return This function returns zero if successful or a negative return error code if
    * there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI AUD_Query_Stream_Format(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, AUD_Stream_Type_t StreamType, AUD_Stream_Format_t *StreamFormat);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AUD_Query_Stream_Format_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, AUD_Stream_Type_t StreamType, AUD_Stream_Format_t *StreamFormat);
#endif

   /*! @brief The following function is responsible for querying the current
    * Stream Configuration of a currently opened stream endpoint on the
    * local device.  This function accepts as input the Bluetooth stack
    * ID of the Bluetooth protocol stack that the requested Audio
    * Manager is present, followed by the remote device address of the
    * connected stream, followed by the stream endpoint type to query
    * the configuration of (local Stream Endpoint), followed by a
    * pointer to a buffer that is receive the current Stream Endpoint
    * configuration.  @return This function returns zero if successful or a
    * negative return error code if there was an error.
    * \note This function is used to query the low level A2DP
    *          configuration that is currently active for the specified
    *          stream.
    */
BTPSAPI_DECLARATION int BTPSAPI AUD_Query_Stream_Configuration(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, AUD_Stream_Type_t StreamType, AUD_Stream_Configuration_t *StreamConfiguration);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AUD_Query_Stream_Configuration_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, AUD_Stream_Type_t StreamType, AUD_Stream_Configuration_t *StreamConfiguration);
#endif

   /*! @brief The following function is responsible for querying the channel
    * information for the Media channel that is associated with the
    * specified stream type.  This function accepts as input the
    * Bluetooth Stack ID of the Bluetooth protocol stack that the
    * requested Audio Manager is present, followed by the remote device
    * address of the connected stream, followed by the stream endpoint
    * type to query the information of (local Stream Endpoint), followed
    * by a pointer to a variable to receive the channel Information.
    * @return This function returns zero if successful or a negative return
    * error code if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI AUD_Query_Stream_Channel_Information(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, AUD_Stream_Type_t StreamType, AUD_Stream_Channel_Info_t *ChannelInformation);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AUD_Query_Stream_Channel_Information_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, AUD_Stream_Type_t StreamType, AUD_Stream_Channel_Info_t *ChannelInformation);
#endif

   /*! @brief The following function is responsible for sending the specified
    * Remote Control Command to the remote device. @param BluetoothStackID This function
    * accepts as input the Bluetooth stack ID of the Bluetooth protocol
    * stack that the requested Audio Manager is present. @param BD_ADDR The second parameter is the
    * Bluetooth device address of the device that is to receive the
    * command. @param RemoteControlCommandData The third parameter is the Remote Control information itself.
    * @param ResponseTimeout The final parameter represents the Timeout value (in milliseconds) to
    * wait for a response (confirmation) from the remote device.
    * @return This function returns a positive (non-zero) return value if successful
    * which represents the internal Transaction ID of the Remote Control
    * Command.  This ID can be used to match responses (confirmations)
    * with outgoing commands.  @return This function returns a negative return
    * error code if there was an error.
    * \note Currently this function is ONLY applicable for local SRC
    *          devices (i.e. if there is a SNK connected and no SRC
    *          connected then this call will fail).
    * \note Specifying a Timeout of zero will cause the Audio Manager
    *          to not track the command (and thus, not dispatch a
    *          response/confirmation event).
    */
BTPSAPI_DECLARATION int BTPSAPI AUD_Send_Remote_Control_Command(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, AUD_Remote_Control_Command_Data_t *RemoteControlCommandData, unsigned long ResponseTimeout);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AUD_Send_Remote_Control_Command_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, AUD_Remote_Control_Command_Data_t *RemoteControlCommandData, unsigned long ResponseTimeout);
#endif

   /*! @brief The following function is responsible for sending the specified
    * Remote Control Response to the remote device that originated the
    * command). @param BluetoothStackID This function accepts as input the Bluetooth stack ID
    * of the Bluetooth protocol stack that the requested Audio Manager
    * is present. @param BD_ADDR the second parameter is the Bluetooth device address of the remote
    * device that sent the Remote Control command. @param TransactionID The third parameter is the
    * Transaction ID of the original Remote Control Transaction. @param RemoteControlResponseData
    * The fourth parameter is the Remote Control information itself.
    * @return This function returns a zero if successful, or a negative error code if there
    * was an error.
    * \note Currently this function is ONLY applicable for local SNK
    *          devices (i.e. if there is no SNK present then this call
    *          will fail - namely because it is was not possible to
    *          receive the command in the first place).
    */
BTPSAPI_DECLARATION int BTPSAPI AUD_Send_Remote_Control_Response(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, unsigned int TransactionID, AUD_Remote_Control_Response_Data_t *RemoteControlResponseData);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AUD_Send_Remote_Control_Response_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, unsigned int TransactionID, AUD_Remote_Control_Response_Data_t *RemoteControlResponseData);
#endif

   /*! @brief The following function is responsible for sending the specified
    * Encoded Audio Data to the remote SNK. @param BluetoothStackID This function accepts as
    * input the Bluetooth stack ID of the Bluetooth protocol stack that
    * the requested Audio Manager is present. @param BD_ADDR The second parameter is the remote
    * device address of the connected audio stream. @param RawAudioDataFrameLength The third
    * paramter is the number of bytes of raw, encoded, audio frame information.
    * @param RawAudioDataFrame The fourth paramter is the raw, encoded, Audio Data to send.
    * @return This function returns zero if successful or a negative return error code if there was an
    * error.
    * \note This is a low level function that exists for applications
    *          that would like to encode the Audio Data themselves (as
    *          opposed to having this module encode and send the data).
    *          The caller can determine the current configuration of the
    *          stream by calling the AUD_Query_Stream_Configuration()
    *          function.
    * \note The data that is sent *MUST* contain the AVDTP Header
    *          Information (i.e. the first byte of the data *MUST* be a
    *          valid AVDTP Header byte).
    * \note This function assumes the specified data is being sent at
    *          real time pacing, and the data is queued to be sent
    *          immediately.
    */
BTPSAPI_DECLARATION int BTPSAPI AUD_Send_Encoded_Audio_Data(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, unsigned int RawAudioDataFrameLength, unsigned char *RawAudioDataFrame);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AUD_Send_Encoded_Audio_Data_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, unsigned int RawAudioDataFrameLength, unsigned char *RawAudioDataFrame);
#endif

   /*! @brief The following function is responsible for sending the specified
    * Encoded Audio Data to the remote SNK. @param BluetoothStackID This function accepts as
    * input the Bluetooth stack ID of the Bluetooth protocol stack that
    * the requested Audio Manager is present. @param BD_ADDR The second paramter is the remote
    * device address of the remote connected device. @param RawAudioDataFrameLength
    * The third paramter is the number of bytes of raw, encoded, audio frame information.
    * @param RawAudioDataFrame The fourth paramter is the raw, encoded, Audio Data to send.
    * @param Flags The fifth paramter contains flags that the specify the format of the data.
    * @param RTPHeaderInfo The final parameter specifies the RTP Header Information that will added to
    * the outgoing packet. @return This function returns zero if successful or
    * a negative return error code if there was an error.
    * \note This is a low level function that exists for applications
    *          that would like to encode the Audio Data themselves (as
    *          opposed to having this module encode and send the data).
    *          The caller can determine the current configuration of the
    *          stream by calling the AUD_Query_Stream_Configuration()
    *          function.
    * \note The data that is sent *MUST* contain the AVDTP Header
    *          Information (i.e. the first byte of the data *MUST* be a
    *          valid AVDTP Header byte).
    * \note This function assumes the specified data is being sent at
    *          real time pacing, and the data is queued to be sent
    *          immediately.
    * \note This is a low level function and allows the user to
    *          specify the RTP Header Information for the outgoing data
    *          packet.  To use the default values for the RTP Header
    *          Information use AUD_Send_Encoded_Audio_Data() instead.
    */
BTPSAPI_DECLARATION int BTPSAPI AUD_Send_RTP_Encoded_Audio_Data(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, unsigned int RawAudioDataFrameLength, unsigned char *RawAudioDataFrame, unsigned long Flags, AUD_RTP_Header_Info_t *RTPHeaderInfo);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AUD_Send_RTP_Encoded_Audio_Data_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, unsigned int RawAudioDataFrameLength, unsigned char *RawAudioDataFrame, unsigned long Flags, AUD_RTP_Header_Info_t *RTPHeaderInfo);
#endif

   /*! @brief The following function is responsible for retrieving the current
    * Audio Manager Connection Mode. @param BluetoothStackID This function accepts as its first
    * parameter the Bluetooth stack ID of the Bluetooth stack on which
    * the server exists.  @param ServerConnectionMode The final parameter to this function is a
    * pointer to a Server Connection Mode variable which will receive
    * the current Server Connection Mode.  @return This function returns zero if
    * successful, or a negative return error code if an error occurred.
    * \note The Default Server Connection Mode is
    *            ausAutomaticAccept.
    * \note This function is used for Bluetooth devices operating
    *            in Bluetooth Security Mode 2.
    */
BTPSAPI_DECLARATION int BTPSAPI AUD_Get_Server_Connection_Mode(unsigned int BluetoothStackID, AUD_Server_Connection_Mode_t *ServerConnectionMode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AUD_Get_Server_Connection_Mode_t)(unsigned int BluetoothStackID, AUD_Server_Connection_Mode_t *ServerConnectionMode);
#endif

   /*! @brief The following function is responsible for setting the Audio
    * Manager Server Connection Mode. @param BluetoothStackID This function accepts as its
    * first parameter the Bluetooth stack ID of the Bluetooth stack in
    * which the server exists.  @param ServerConnectionMode The second parameter to this function is
    * the new Server Connection Mode to set the Server to use.
    * Connection requests will not be dispatched unless the Server Mode
    * (second parameter) is set to ausManualAccept. In this case the
    * Callback that was registered with the server will be invoked
    * whenever a Remote Bluetooth device attempts to connect to the
    * local device.  If the Server Mode (second parameter) is set to
    * anything other than ausManualAccept then no open request
    * indication events will be dispatched.  @return This function returns zero
    * if successful, or a negative return error code if an error
    * occurred.
    * \note The Default Server Connection Mode is
    *            ausAutomaticAccept.
    * \note This function is used for Bluetooth devices operating
    *            in Bluetooth Security Mode 2.
    */
BTPSAPI_DECLARATION int BTPSAPI AUD_Set_Server_Connection_Mode(unsigned int BluetoothStackID, AUD_Server_Connection_Mode_t ServerConnectionMode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AUD_Set_Server_Connection_Mode_t)(unsigned int BluetoothStackID, AUD_Server_Connection_Mode_t ServerConnectionMode);
#endif

#endif
