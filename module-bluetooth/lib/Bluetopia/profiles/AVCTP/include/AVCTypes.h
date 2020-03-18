/*****< avctypes.h >***********************************************************/
/*      Copyright 2005 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  AVCTYPES - Bluetooth Audio/Video Control Transport Type                   */
/*             Definitions/Constants.                                         */
/*                                                                            */
/*  Author:  Nikhil Deo                                                       */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   12/08/05  N. Deo         Initial creation.                               */
/*   08/01/09  D. Lange       Moved into seperate file.                       */
/******************************************************************************/
#ifndef __AVCTYPESH__
#define __AVCTYPESH__

#include "BTTypes.h"            /* Bluetooth Type Definitions.                */

   /* AVCTP Message Header definitions for the first Byte in the        */
   /* Signalling Packet Header.                                         */
#define AVCTP_TRANSACTION_ID_MASK                                        (0x0F)
#define AVCTP_TRANSACTION_ID_SHIFT                                       (4)

#define AVCTP_PACKET_TYPE_SINGLE                                         (0x00)
#define AVCTP_PACKET_TYPE_START                                          (0x01)
#define AVCTP_PACKET_TYPE_CONTINUE                                       (0x02)
#define AVCTP_PACKET_TYPE_END                                            (0x03)
#define AVCTP_PACKET_TYPE_MASK                                           (0x03)
#define AVCTP_PACKET_TYPE_SHIFT                                          (2)

#define AVCTP_MESSAGE_TYPE_MASK                                          (0x01)
#define AVCTP_MESSAGE_TYPE_SHIFT                                         (1)
#define AVCTP_MESSAGE_TYPE_COMMAND                                       (0x0)
#define AVCTP_MESSAGE_TYPE_RESPONSE                                      (0x1)

#define AVCTP_IPID_BIT_MASK                                              (0x01)

#define AVCTP_MINIMUM_MESSAGE_HEADER_LENGTH                              (2)

   /* The following constants represent the Service Discovery Protocol  */
   /* Protocol Service Multiplexor (PSM) values that AVCTP uses over the*/
   /* L2CAP Protocol (which is the only defined mechanism).             */
#define AVCTP_PSM                                                        (0x0017)
#define AVCTP_BROWSING_CHANNEL_PSM                                       (0x001B)

   /* This structure defines the signalling header for a single,        */
   /* unfragmented signalling message.  MessageHeader contains the      */
   /* Transaction Label, Packet Type, and IPID (see defined constants   */
   /* above).  ProfileUUID holds the UUID of the intended recepient     */
   /* profile.                                                          */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVCTP_Single_Packet_Header_t
{
   Byte_t    MessageHeader;
   UUID_16_t ProfileUUID;
} __PACKED_STRUCT_END__ AVCTP_Single_Packet_Header_t;

#define AVCTP_SINGLE_PACKET_HEADER_SIZE                 (sizeof(AVCTP_Single_Packet_Header_t))

   /* This structure defines the signalling header for a start packet of*/
   /* a signalling message that is fragmented over multiple packets.    */
   /* MessageHeader contains the Transaction Label, Packet Type, and    */
   /* Message Type (see defined constants above).  NumberOfPackets is   */
   /* the number of packets (including this one) that the message is    */
   /* fragmented into.  ProfileUUID holds the UUID of the intended      */
   /* recepient profile.                                                */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVCTP_Start_Packet_Header_t
{
   Byte_t    MessageHeader;
   Byte_t    NumberOfPackets;
   UUID_16_t ProfileUUID;
} __PACKED_STRUCT_END__ AVCTP_Start_Packet_Header_t;

#define AVCTP_START_PACKET_HEADER_SIZE                  (sizeof(AVCTP_Start_Packet_Header_t))

   /* This structure defines the signalling header for a continuing     */
   /* packet for a signalling message that is fragmented across multiple*/
   /* packets.  MessageHeader contains the Transaction Label, Packet    */
   /* Type, and Message Type (see defined constants above).             */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVCTP_Continue_Packet_Header_t
{
   Byte_t MessageHeader;
} __PACKED_STRUCT_END__ AVCTP_Continue_Packet_Header_t;

#define AVCTP_CONTINUE_PACKET_HEADER_SIZE               (sizeof(AVCTP_Continue_Packet_Header_t))

   /* This structure defines the signalling header for an end packet    */
   /* (the last packet) for a signalling message that is fragmented     */
   /* across multiple packets.  MessageHeader contains the Transaction  */
   /* Label, Packet Type, and Message Type (see defined constants       */
   /* above).                                                           */
typedef __PACKED_STRUCT_BEGIN__ struct _tagAVCTP_End_Packet_Header_t
{
   Byte_t MessageHeader;
} __PACKED_STRUCT_END__ AVCTP_End_Packet_Header_t;

#define AVCTP_END_PACKET_HEADER_SIZE                    (sizeof(AVCTP_End_Packet_Header_t))

#endif
