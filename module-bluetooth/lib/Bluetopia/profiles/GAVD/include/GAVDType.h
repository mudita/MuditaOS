/*****< gavdtype.h >***********************************************************/
/*      Copyright 2003 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  GAVDTYPE - Bluetooth Generic Audio/Video Distribution Protocol (GAVD)     */
/*             Type Definitions/Constants.                                    */
/*                                                                            */
/*  Author:  Damon Lange                                                      */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   01/13/03  D. Hill        Initial creation.                               */
/*   08/01/09  D. Lange       Moved into seperate file.                       */
/******************************************************************************/
#ifndef __GAVDTYPEH__
#define __GAVDTYPEH__

#include "BTTypes.h"            /* Bluetooth Type Definitions.                */

   /* AVDTP Protocol UUIDs.                                             */

   /* The following MACRO is a utility MACRO that assigns the AVDTP     */
   /* Protocol Bluetooth Universally Unique Identifier (SDP_UUID_16) to */
   /* the specified UUID_16_t variable.  This MACRO accepts one         */
   /* parameter which is the UUID_16_t variable that is to receive the  */
   /* AVDTP_PROTOCOL_UUID_16 Constant value.                            */
#define SDP_ASSIGN_AVDTP_PROTOCOL_UUID_16(_x)            ASSIGN_SDP_UUID_16((_x), 0x00, 0x19)

   /* The following MACRO is a utility MACRO that assigns the AVDTP     */
   /* Protocol Bluetooth Universally Unique Identifier (SDP_UUID_32) to */
   /* the specified UUID_32_t variable.  This MACRO accepts one         */
   /* parameter which is the UUID_32_t variable that is to receive the  */
   /* AVDTP_PROTOCOL_UUID_32 Constant value.                            */
#define SDP_ASSIGN_AVDTP_PROTOCOL_UUID_32(_x)            ASSIGN_SDP_UUID_32((_x), 0x00, 0x00, 0x00, 0x19)

   /* The following MACRO is a utility MACRO that assigns the AVDTP     */
   /* Protocol Bluetooth Universally Unique Identifier (SDP_UUID_128) to*/
   /* the specified UUID_128_t variable.  This MACRO accepts one        */
   /* parameter which is the UUID_128_t variable that is to receive the */
   /* AVDTP_PROTOCOL_UUID_128 Constant value.                           */
#define SDP_ASSIGN_AVDTP_PROTOCOL_UUID_128(_x)           ASSIGN_SDP_UUID_128((_x), 0x00, 0x00, 0x00, 0x19, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /* The following constant defines the Protocol Version Number used   */
   /* within the SDP Record for AVDTP Protocol Server/Clients (supported*/
   /* by this implementation).                                          */
#define AVDTP_PROTOCOL_VERSION                                            (0x0102)

   /* The following Constants represent the Service Discovery Protocol  */
   /* Protocol Service Multiplexor (PSM) values when GAVD/AVDTP runs    */
   /* over the L2CAP Protocol (which is the only defined mechanism).    */
#define AVDTP_PSM                                                         (0x19)

   /* The following Constants represent the Minimum and Maximum         */
   /* allowable LSEID values.  The LSEID value is a 6 bit value with the*/
   /* following Minimum and Maximum values.                             */
#define AVDTP_MINIMUM_LSEID                                               (0x01)
#define AVDTP_MAXIMUM_LSEID                                               (0x3E)

   /* The following definitions represent the Service Categories that   */
   /* Stream Endpoints may support.  These Service Categories are       */
   /* enumerated as type AVDTP_SEP_Service_Category_t.  The service     */
   /* categories appear in the Service Capabilities structure.          */
#define AVDTP_SERVICE_CATEGORY_MEDIA_TRANSPORT                            (0x01)
#define AVDTP_SERVICE_CATEGORY_REPORTING                                  (0x02)
#define AVDTP_SERVICE_CATEGORY_RECOVERY                                   (0x03)
#define AVDTP_SERVICE_CATEGORY_CONTENT_PROTECTION                         (0x04)
#define AVDTP_SERVICE_CATEGORY_HEADER_COMPRESSION                         (0x05)
#define AVDTP_SERVICE_CATEGORY_MULTIPLEXING                               (0x06)
#define AVDTP_SERVICE_CATEGORY_MEDIA_CODEC                                (0x07)
#define AVDTP_SERVICE_CATEGORY_UNKNOWN                                    (0x08)

   /* The following constants represent the AVDTP Message Identifiers   */
#define AVDTP_DISCOVER                                                    (0x01)
#define AVDTP_GET_CAPABILITIES                                            (0x02)
#define AVDTP_SET_CONFIGURATION                                           (0x03)
#define AVDTP_GET_CONFIGURATION                                           (0x04)
#define AVDTP_RECONFIGURE                                                 (0x05)
#define AVDTP_OPEN                                                        (0x06)
#define AVDTP_START                                                       (0x07)
#define AVDTP_CLOSE                                                       (0x08)
#define AVDTP_SUSPEND                                                     (0x09)
#define AVDTP_ABORT                                                       (0x0A)
#define AVDTP_SECURITY_CONTROL                                            (0x0B)

   /* AVDTP Message Header definitions for the first Byte in the        */
   /* Signalling Packet Header.                                         */
#define AVDTP_TRANSACTION_ID_MASK                                         (0x0F)
#define AVDTP_TRANSACTION_ID_SHIFT                                           (4)

#define AVDTP_PACKET_TYPE_SINGLE                                          (0x00)
#define AVDTP_PACKET_TYPE_START                                           (0x01)
#define AVDTP_PACKET_TYPE_CONTINUE                                        (0x02)
#define AVDTP_PACKET_TYPE_END                                             (0x03)
#define AVDTP_PACKET_TYPE_MASK                                            (0x03)
#define AVDTP_PACKET_TYPE_SHIFT                                              (2)

#define AVDTP_MESSAGE_TYPE_COMMAND                                        (0x00)
#define AVDTP_MESSAGE_TYPE_RESPONSE_ACCEPT                                (0x02)
#define AVDTP_MESSAGE_TYPE_RESPONSE_REJECT                                (0x03)
#define AVDTP_MESSAGE_TYPE_MASK                                           (0x03)

#define AVDTP_MESSAGE_ID_MASK                                             (0x3F)

#define AVDTP_SEID_MASK                                                   (0x3F)
#define AVDTP_SEID_SHIFT                                                     (2)
#define AVDTP_MAX_NUMBER_OF_SEIDS                                           (62)

#define AVDTP_DISCOVER_RESPONSE_IN_USE                                    (0x02)
#define AVDTP_MEDIA_TYPE_AUDIO                                            (0x00)
#define AVDTP_MEDIA_TYPE_VIDEO                                            (0x01)
#define AVDTP_MEDIA_TYPE_MULTIMEDIA                                       (0x02)
#define AVDTP_MEDIA_TYPE_SHIFT                                               (4)
#define AVDTP_DISCOVER_RESPONSE_TSEP_SRC                                  (0x00)
#define AVDTP_DISCOVER_RESPONSE_TSEP_SNK                                  (0x08)

#define AVDTP_CAPABILITIES_RECOVERY_HEADER_COMPRESSION                    (0x20)
#define AVDTP_CAPABILITIES_MEDIA_HEADER_COMPRESSION                       (0x40)
#define AVDTP_CAPABILITIES_BACK_CHANNEL_SUPPORTED                         (0x80)
#define AVDTP_CAPABILITIES_ADAPTATION_LAYER_FRAGMENTATION                 (0x80)

#define MAX_NUMBER_SERVICE_CAPABILITIES_ELEMENTS_IN_COMMANDS                 (7)

#define ADAPTATION_LAYER_TSID_MASK                                        (0x1F)
#define ADAPTATION_LAYER_TSID_SHIFT                                          (3)
#define ADAPTATION_LAYER_FRAGMENTATION_BIT                                (0x04)
#define ADAPTATION_LAYER_FRAGMENTATION_MASK                               (0x01)
#define ADAPTATION_LAYER_FRAGMENTATION_SHIFT                                 (2)
#define ADAPTATION_LAYER_LCODE_MASK                                       (0x03)
#define ADAPTATION_LAYER_LCODE_NONE                                       (0x00)
#define ADAPTATION_LAYER_LCODE_16_BIT                                     (0x01)
#define ADAPTATION_LAYER_LCODE_9_BIT_MSB_0                                (0x02)
#define ADAPTATION_LAYER_LCODE_9_BIT_MSB_1                                (0x03)

#define MIN_TRANSPORT_ID_VALUE                                            (0x01)
#define MAX_TRANSPORT_ID_VALUE                                            (0x1E)

   /* RTP Header definitions.  See RFC 1889 and RFC 2733.               */

   /* RTP Version 2 headers are used in AVDTP.                          */
#define RTP_VERSION                                                       (0x80)
#define RTP_PADDING_MASK                                                  (0x01)
#define RTP_PADDING_SHIFT                                                    (5)
#define RTP_MARKER_BIT                                                    (0x80)
#define RTP_PAYLOAD_TYPE_MASK                                             (0x7F)

#define RTCP_REPORT_COUNT_MASK                                            (0x1F)
#define RTCP_SOURCE_COUNT_MASK                                            (0x1F)
#define RTCP_SENDER_REPORT_PACKET_TYPE                                     (200)
#define RTCP_RECEIVER_REPORT_PACKET_TYPE                                   (201)
#define RTCP_SDES_PACKET_TYPE                                              (202)

#define RTP_FEC_PAYLOAD_TYPE_RECOVERY_MASK                                (0x1F)

   /* This structure defines the signalling header for a single,        */
   /* unfragmented signalling message.  MessageHeader contains the      */
   /* Transaction Label, Packet Type, and Message Type (see defined     */
   /* constants above).  MessageID holds the defined AVDTP command byte.*/
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVDTP_Single_Packet_Header_t
{
   Byte_t MessageHeader;
   Byte_t MessageID;
} __PACKED_STRUCT_END__ AVDTP_Single_Packet_Header_t;

#define AVDTP_SINGLE_PACKET_HEADER_SIZE                 (sizeof(AVDTP_Single_Packet_Header_t))

   /* This structure defines the signalling header for a start packet of*/
   /* a signalling message that is fragmented over multiple packets.    */
   /* MessageHeader contains the Transaction Label, Packet Type, and    */
   /* Message Type (see defined constants above).  NumberOfSignalPackets*/
   /* is the number of packets (including this one) that the signalling */
   /* message is fragmented into.  MessageID holds the defined AVDTP    */
   /* command byte.                                                     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVDTP_Start_Packet_Header_t
{
   Byte_t MessageHeader;
   Byte_t NumberSignalPackets;
   Byte_t MessageID;
} __PACKED_STRUCT_END__ AVDTP_Start_Packet_Header_t;

#define AVDTP_START_PACKET_HEADER_SIZE                  (sizeof(AVDTP_Start_Packet_Header_t))

   /* This structure defines the signalling header for a continuing     */
   /* packet for a signalling message that is fragmented across multiple*/
   /* packets.  MessageHeader contains the Transaction Label, Packet    */
   /* Type, and Message Type (see defined constants above).             */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVDTP_Continue_Packet_Header_t
{
   Byte_t MessageHeader;
} __PACKED_STRUCT_END__ AVDTP_Continue_Packet_Header_t;

#define AVDTP_CONTINUE_PACKET_HEADER_SIZE               (sizeof(AVDTP_Continue_Packet_Header_t))

   /* This structure defines the signalling header for an end packet    */
   /* (the last packet) for a signalling message that is fragmented     */
   /* across multiple packets.  MessageHeader contains the Transaction  */
   /* Label, Packet Type, and Message Type (see defined constants       */
   /* above).                                                           */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVDTP_End_Packet_Header_t
{
   Byte_t MessageHeader;
} __PACKED_STRUCT_END__ AVDTP_End_Packet_Header_t;

#define AVDTP_END_PACKET_HEADER_SIZE                    (sizeof(AVDTP_End_Packet_Header_t))

   /* This union specifies the generic signalling header used in all    */
   /* signalling packets.  Signalling headers may be Single Packet      */
   /* Headers, Start Packet Headers, Continue Packet Headers, or End    */
   /* Packet Headers.                                                   */
typedef __PACKED_STRUCT_BEGIN__ union _tagAVDTP_Signalling_Packet_Header_t
{
   AVDTP_Single_Packet_Header_t   AVDTP_Single_Packet_Header;
   AVDTP_Start_Packet_Header_t    AVDTP_Start_Packet_Header;
   AVDTP_Continue_Packet_Header_t AVDTP_Continue_Packet_Header;
   AVDTP_End_Packet_Header_t      AVDTP_End_Packet_Header;
} __PACKED_STRUCT_END__ AVDTP_Signalling_Packet_Header_t;

#define AVDTP_SINGALLING_PACKET_HEADER_SIZE             (sizeof(AVDTP_Signalling_Packet_Header_t))

   /* This structure defines the AVDTP Discover Response SEID Data.     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVDTP_Discover_Response_End_Point_Data_t
{
   Byte_t EndPointInfo;
   Byte_t EndPointType;
} __PACKED_STRUCT_END__ AVDTP_Discover_Response_End_Point_Data_t;

#define AVDTP_DISCOVER_RESPONSE_END_POINT_DATA_SIZE     (sizeof(AVDTP_Discover_Response_End_Point_Data_t))

   /* This structure defines the AVDTP Set Configuration Command.       */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVDTP_Set_Configuration_Command_t
{
   Byte_t RSEID;
   Byte_t LSEID;
   Byte_t VariableData[1];
} __PACKED_STRUCT_END__ AVDTP_Set_Configuration_Command_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of a Set             */
   /* Configuration command based upon the length of the Variable Data  */
   /* containing the Service Capabilities data in the configuration.    */
   /* The first parameter to this MACRO is the length of the Variable   */
   /* Data element of the structure.                                    */
#define AVDTP_SET_CONFIGURATION_COMMAND_SIZE(_x)        (sizeof(AVDTP_Set_Configuration_Command_t) - sizeof(Byte_t) + (unsigned int)(_x))

   /* This structure defines the AVDTP Set Configuration Reject         */
   /* Response.                                                         */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVDTP_Set_Configuration_Reject_Response_t
{
   Byte_t FirstFailingServiceCategory;
   Byte_t ErrorCode;
} __PACKED_STRUCT_END__ AVDTP_Set_Configuration_Reject_Response_t;

#define AVDTP_SET_CONFIGURATION_REJECT_RESPONSE_SIZE    (sizeof(AVDTP_Set_Configuration_Reject_Response_t))

   /* This structure defines the AVDTP Reconfigure Command.             */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVDTP_Reconfigure_Command_t
{
   Byte_t RSEID;
   Byte_t VariableData[1];
} __PACKED_STRUCT_END__ AVDTP_Reconfigure_Command_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of a Reconfigure     */
   /* command based upon the length of the Variable Data containing the */
   /* Service Capabilities data in the Reconfiguration.  The first      */
   /* parameter to this MACRO is the length of the Variable Data element*/
   /* of the structure.                                                 */
#define AVDTP_RECONFIGURE_COMMAND_SIZE(_x)              (sizeof(AVDTP_Reconfigure_Command_t) - sizeof(Byte_t) + (unsigned int)(_x))

   /* This structure defines the AVDTP Reconfigure Reject Response.     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVDTP_Reconfigure_Reject_Response_t
{
   Byte_t FirstFailingServiceCategory;
   Byte_t ErrorCode;
} __PACKED_STRUCT_END__ AVDTP_Reconfigure_Reject_Response_t;

#define AVDTP_RECONFIGURE_REJECT_RESPONSE_SIZE          (sizeof(AVDTP_Reconfigure_Reject_Response_t))

   /* This structure defines the AVDTP Start Stream Reject Response.    */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVDTP_Start_Stream_Reject_Response_t
{
   Byte_t FirstFailingLSEID;
   Byte_t ErrorCode;
} __PACKED_STRUCT_END__ AVDTP_Start_Stream_Reject_Response_t;

#define AVDTP_START_STREAM_REJECT_RESPONSE_SIZE         (sizeof(AVDTP_Start_Stream_Reject_Response_t))

   /* This structure defines the AVDTP Suspend Stream Reject Response.  */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVDTP_Suspend_Stream_Reject_Response_t
{
   Byte_t FirstFailingLSEID;
   Byte_t ErrorCode;
} __PACKED_STRUCT_END__ AVDTP_Suspend_Stream_Reject_Response_t;

#define AVDTP_SUSPEND_STREAM_REJECT_RESPONSE_SIZE       (sizeof(AVDTP_Suspend_Stream_Reject_Response_t))

   /* This structure defines the AVDTP Security Control Command.        */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVDTP_Security_Control_Command_t
{
   Byte_t RSEID;
   Byte_t VariableData[1];
} __PACKED_STRUCT_END__ AVDTP_Security_Control_Command_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of a Security Control*/
   /* command based upon the length of the Variable Data element.  The  */
   /* first parameter to this MACRO is the length of the Variable Data  */
   /* element of the structure.                                         */
#define AVDTP_SECURITY_CONTROL_COMMAND_SIZE(_x)         (sizeof(AVDTP_Security_Control_Command_t) - sizeof(Byte_t) + (unsigned int)(_x))

   /* This structure defines the AVDTP Generic Capability Container.    */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVDTP_Capability_t
{
   Byte_t ServiceCategory;
   Byte_t ServiceCapabilityLength;
   Byte_t VariableData[1];
} __PACKED_STRUCT_END__ AVDTP_Capability_t;

   /* The following MACRO definition is provided to allow an easy to    */
   /* read mechanism for calculating the total size of an AVDTP Protocol*/
   /* Capability.  This MACRO accepts as input the size (in bytes) of   */
   /* the Capability specific data and returns the total size (in bytes)*/
   /* of the container required to hold the AVDTP Capability.           */
#define AVDTP_CAPABILITY_SIZE(_x)                       (sizeof(AVDTP_Capability_t) - sizeof(Byte_t) + (unsigned int)(_x))

   /* This structure defines the AVDTP Recovery Capability Data.        */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVDTP_Recovery_Capability_Data_t
{
   Byte_t RecoveryType;
   Byte_t MaxRecoveryWindowSize;
   Byte_t MaxNumberMediaPackets;
} __PACKED_STRUCT_END__ AVDTP_Recovery_Capability_Data_t;

#define AVDTP_RECOVERY_CAPABILITY_DATA_SIZE             (sizeof(AVDTP_Recovery_Capability_Data_t))

   /* This structure defines the AVDTP Media Codec Capability Data.     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVDTP_Media_Codec_Capability_Data_t
{
   Byte_t  MediaType;
   Byte_t  MediaCodecType;
   Byte_t  VariableData[1];
} __PACKED_STRUCT_END__ AVDTP_Media_Codec_Capability_Data_t;

   /* This size definition calculates the size of the Media Codec       */
   /* Capability element based upon the specified number of specific    */
   /* Capabilities (specified in bytes).                                */
#define AVDTP_MEDIA_CODEC_CAPABILITY_DATA_SIZE(_x)      (sizeof(AVDTP_Media_Codec_Capability_Data_t) - sizeof(Byte_t) + (unsigned int)(_x))

   /* This structure defines the AVDTP Content Protection Capability    */
   /* Data.                                                             */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVDTP_Content_Protection_Capability_Data_t
{
   NonAlignedWord_t  ContentProtectionType;
   Byte_t            VariableData[1];
} __PACKED_STRUCT_END__ AVDTP_Content_Protection_Capability_Data_t;

   /* This size definition calculates the size of the Content Protection*/
   /* Capability element based upon the specified number of specific    */
   /* Capabilities (specified in bytes).                                */
#define AVDTP_CONTENT_PROTECTION_CAPABILITY_DATA_SIZE(_x)  (sizeof(AVDTP_Content_Protection_Capability_Data_t) - sizeof(Byte_t) + (unsigned int)(_x))

   /* This structure defines the AVDTP Header Compression Capability    */
   /* Data.                                                             */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVDTP_Header_Compression_Capability_Data_t
{
   Byte_t Compression;
} __PACKED_STRUCT_END__ AVDTP_Header_Compression_Capability_Data_t;

#define AVDTP_HEADER_COMPRESSION_CAPABILITY_DATA_SIZE   (sizeof(AVDTP_Header_Compression_Capability_Data_t))

   /* This structure defines the AVDTP Multiplexing Capability Data.    */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVDTP_Multiplexing_Capability_Data_t
{
   Byte_t Fragment;
   Byte_t MediaTSID;
   Byte_t MediaTCID;
   Byte_t ReportingTSID;
   Byte_t ReportingTCID;
   Byte_t RecoveryTSID;
   Byte_t RecoveryTCID;
} __PACKED_STRUCT_END__ AVDTP_Multiplexing_Capability_Data_t;

#define AVDTP_MULTIPLEXING_CAPABILITY_DATA_SIZE         (sizeof(AVDTP_Multiplexing_Capability_Data_t))

   /* RTP/RTCP Header Definitions.                                      */

   /* The following structure defines the RTP header for media packets. */
   /* See RFC 1889.                                                     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagRTP_Header_t
{
   Byte_t            VPXCC;
   Byte_t            MPT;
   NonAlignedWord_t  SequenceNumber;
   NonAlignedDWord_t TimeStamp;
   NonAlignedDWord_t SyncSource;
   Byte_t            VariableData[1];
} __PACKED_STRUCT_END__ RTP_Header_t;

#define RTP_HEADER_SIZE                                 (sizeof(RTP_Header_t) - sizeof(Byte_t))

   /* The following structure defines the header for Sender Reporting   */
   /* (SR) and Receiver Reporting (RR) RTCP reporting packets.  See RFC */
   /* 1889.                                                             */
typedef __PACKED_STRUCT_BEGIN__ struct _tagSR_RR_Header_t
{
   Byte_t            VPRC;
   Byte_t            PacketType;
   NonAlignedWord_t  Length;
   NonAlignedDWord_t SyncSource;
} __PACKED_STRUCT_END__ SR_RR_Header_t;

#define SR_RR_HEADER_SIZE                               (sizeof(SR_RR_Header_t))

   /* The following structure defines the Sender Information portion for*/
   /* Sender Reporting (SR) RTCP reporting packets.  See RFC 1889.      */
typedef __PACKED_STRUCT_BEGIN__ struct _tagSender_Info_t
{
   NonAlignedDWord_t NTPTimeStampMSW;
   NonAlignedDWord_t NTPTimeStampLSW;
   NonAlignedDWord_t RTPTimeStamp;
   NonAlignedDWord_t PacketCount;
   NonAlignedDWord_t OctetCount;
} __PACKED_STRUCT_END__ Sender_Info_t;

#define SENDER_INFO_SIZE                                (sizeof(Sender_Info_t))

   /* The following structure defines the Report Block portion for      */
   /* Sender Reporting (SR) and Receiver Reporting (RR) RTCP reporting  */
   /* packets.  See RFC 1889.                                           */
typedef __PACKED_STRUCT_BEGIN__ struct _tagReport_Block_t
{
   NonAlignedDWord_t SyncSource;
   Byte_t            FractionLost;
   Byte_t            TotalPacketsLost[3];
   NonAlignedWord_t  HighestSeqNumReceivedCycleCount;
   NonAlignedWord_t  HighestSeqNumReceived;
   NonAlignedDWord_t IntervalJitter;
   NonAlignedDWord_t LastReport;
   NonAlignedDWord_t DelaySinceLastReport;
} __PACKED_STRUCT_END__ Report_Block_t;

#define REPORT_BLOCK_SIZE                               (sizeof(Report_Block_t))

   /* The following structure defines the header for Source Description */
   /* (SDES) RTCP reporting packets.  See RFC 1889.                     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagSDES_Header_t
{
   Byte_t           VPSC;
   Byte_t           PacketType;
   NonAlignedWord_t Length;
} __PACKED_STRUCT_END__ SDES_Header_t;

#define SDES_HEADER_SIZE                                (sizeof(SDES_Header_t))

   /* The following structure defines the Item Block for Source         */
   /* Description (SDES) RTCP reporting packets.  See RFC 1889.         */
typedef __PACKED_STRUCT_BEGIN__ struct _tagSDES_Item_t
{
   Byte_t ItemDescriptor;
   Byte_t ItemLength;
   Byte_t ItemData[1];
} __PACKED_STRUCT_END__ SDES_Item_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an SDES Item      */
   /* structure based upon the length of the Item Data associated with  */
   /* the structure.  The first parameter to this MACRO is the          */
   /* ItemLength element of the structure.                              */
#define SDES_ITEM_SIZE(_x)                              (sizeof(SDES_Item_t) - sizeof(Byte_t) + (unsigned int)(_x))

   /* The following structure defines the header for RTP Forward Error  */
   /* Correction (FEC) header packets.  See RFC 2733.                   */
typedef __PACKED_STRUCT_BEGIN__ struct _tagFEC_Header_t
{
   NonAlignedWord_t  SequenceNumberBase;
   NonAlignedWord_t  LengthRecovery;
   Byte_t            PayloadTypeRecovery;
   Byte_t            Mask[3];
   NonAlignedDWord_t TimeStampRecovery;
} __PACKED_STRUCT_END__ FEC_Header_t;

#define FEC_HEADER_SIZE                                 (sizeof(FEC_Header_t))

#endif
