/*****< rfcommt.h >************************************************************/
/*      Copyright 2000 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  RFCOMMT - Bluetooth RFCOMM Type Definitions/Constants.                    */
/*                                                                            */
/*  Author:  Damon Lange                                                      */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   09/11/00  D. Lange       Initial creation.                               */
/******************************************************************************/
#ifndef __RFCOMMTH__
#define __RFCOMMTH__

#include "BTTypes.h"
#include "L2CAPTyp.h"

   /*              Bluetooth Type Definitions.                          */

   /* The following Constant represents the RFCOMM Protocol, Protocol   */
   /* Service Multiplexor (PSM) value when RFCOMM runs over the L2CAP   */
   /* Protocol (which is the only defined mechanism).                   */
#define RFCOMM_PROTOCOL_SERVICE_MULTIPLEXOR                                0x03

   /* The following Constants represent the defined Bluetooth RFCOMM    */
   /* Server Channel ID Minimum and Maximum Values.                     */
#define RFCOMM_MINIMUM_SERVER_CHANNEL_ID                                   0x01
#define RFCOMM_MAXIMUM_SERVER_CHANNEL_ID                                   0x1E

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability of testing whether or not a specified Server Channel  */
   /* ID Value is valid.  The first parameter to this MACRO is the      */
   /* Server Channel ID Value to verify.  This MACRO returns a boolean  */
   /* value based upon whether or not the specified Server Channel ID   */
   /* Value is valid.  This MACRO returns a boolean TRUE if the         */
   /* specified Server Channel ID is valid, or a boolean FALSE if the   */
   /* specified Server Channel ID Value is invalid.                     */
#define RFCOMM_VALID_SERVER_CHANNEL_ID(_x)                      (((_x) >= RFCOMM_MINIMUM_SERVER_CHANNEL_ID) && ((_x) <= RFCOMM_MAXIMUM_SERVER_CHANNEL_ID))

   /* The following Constants represent the defined Bluetooth RFCOMM    */
   /* Server Channel ID Minimum and Maximum Values.                     */
#define RFCOMM_MINIMUM_DLCI             (RFCOMM_MINIMUM_SERVER_CHANNEL_ID << 1)
#define RFCOMM_MAXIMUM_DLCI             (RFCOMM_MAXIMUM_SERVER_CHANNEL_ID << 1)

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability of testing whether or not a specified DLCI Value is   */
   /* valid.  The first parameter to this MACRO is the DLCI value to    */
   /* verify.  This MACRO returns a boolean value based upon whether or */
   /* not the specified DLCI Value is valid.  This MACRO returns a      */
   /* boolean TRUE if the specified DLCI is valid, or a boolean FALSE   */
   /* if the specified DLCI Value is invalid.                           */
   /* * NOTE * This MACRO does NOT consider the Control DLCI to be a    */
   /*          valid DLCI Value !!!!                                    */
#define RFCOMM_VALID_DLCI(_x)                           (((_x) >= RFCOMM_MINIMUM_DLCI) && ((_x) <= RFCOMM_MAXIMUM_DLCI))

   /* The following specified the DLCI on which the Control Channel     */
   /* resides.                                                          */
#define RFCOMM_CONTROL_DLCI                                                0x00
#define RFCOMM_CONTROL_CHANNEL                                             0x00
#define RFCOMM_PENDING_CHANNEL                                             0xFF

#define RFCOMM_SESSION_INITIATOR                                           0x01

   /* The following constants represent the RFCOMM Frame types that are */
   /* supported.  These are the base values of each command.  Bit 3 of  */
   /* each command will have the P/F bit inserted prior to transmission.*/
   /* The P/F bit is used to indicate direction.                        */
#define RFCOMM_FRAME_TYPE_DM                                               0x0F
#define RFCOMM_FRAME_TYPE_SABM                                             0x2F
#define RFCOMM_FRAME_TYPE_DISC                                             0x43
#define RFCOMM_FRAME_TYPE_UA                                               0x63
#define RFCOMM_FRAME_TYPE_UIH                                              0xEF

   /* The following constants represent the Minimum, Maximum, and       */
   /* Default values for RFCOMM Frame Sizes (in Bytes).                 */
#define RFCOMM_FRAME_SIZE_MINIMUM_VALUE                                      23
#define RFCOMM_FRAME_SIZE_MAXIMUM_VALUE                                   32767
#define RFCOMM_FRAME_SIZE_DEFAULT_VALUE                                     127

   /* The following constants represent RFCOMM Open Status.  These      */
   /* constants represent the result of a previous Open Request.        */
#define RFCOMM_CONNECT_RESULT_CONNECTION_SUCCESSFUL                        0x00
#define RFCOMM_CONNECT_RESULT_CONNECTION_TIMEOUT                           0x01
#define RFCOMM_CONNECT_RESULT_CONNECTION_REFUSED                           0x02

   /* The following constants represent RFCOMM control commands.  These */
   /* commands are transfered on DCLI 0, the Control Channel, to        */
   /* negotiate and control the operation of a DLCI                     */
#define RFCOMM_CONTROL_MESSAGE_NSC                                         (0x04 << 2)
#define RFCOMM_CONTROL_MESSAGE_TEST                                        (0x08 << 2)
#define RFCOMM_CONTROL_MESSAGE_RLS                                         (0x14 << 2)
#define RFCOMM_CONTROL_MESSAGE_FCOFF                                       (0x18 << 2)
#define RFCOMM_CONTROL_MESSAGE_PN                                          (0x20 << 2)
#define RFCOMM_CONTROL_MESSAGE_RPN                                         (0x24 << 2)
#define RFCOMM_CONTROL_MESSAGE_FCON                                        (0x28 << 2)
#define RFCOMM_CONTROL_MESSAGE_MSC                                         (0x38 << 2)

   /* The following constants represent RFCOMM parametes supplied during*/
   /* a port negotiation.  These represent those paramters that have    */
   /* fixed values.                                                     */
#define RFCOMM_PORT_NEGOTIATION_PARAMETER_USE_HIU_FRAMES                   0x00
#define RFCOMM_PORT_NEGOTIATION_PARAMETER_CFC_NOT_SUPPORTED                0x00
#define RFCOMM_PORT_NEGOTIATION_PARAMETER_CFC_MASK                         0xF0
#define RFCOMM_PORT_NEGOTIATION_PARAMETER_CFC_SUPPORTED_REQUEST            0xF0
#define RFCOMM_PORT_NEGOTIATION_PARAMETER_CFC_SUPPORTED_RESPONSE           0xE0
#define RFCOMM_PORT_NEGOTIATION_PARAMETER_ACKNOWLEDGMENT_TIMER             0x00
#define RFCOMM_PORT_NEGOTIATION_PARAMETER_NUMBER_OF_RETRANSMISSIONS        0x00
#define RFCOMM_PORT_NEGOTIATION_PARAMETER_INITIAL_CREDIT_MASK              0x07

   /* The following constants reprsent the bit pattern to be used in the*/
   /* Port Negotiation command.  Note: The binary pattern for some      */
   /* parameters may be packed into a byte.  The constants provided for */
   /* packed parameters here represents the bit pattern that has been   */
   /* shifted to its proper position in the packed byte.                */
#define RFCOMM_RPN_PARAMETER_BAUD_2400                                     0x00
#define RFCOMM_RPN_PARAMETER_BAUD_4800                                     0x01
#define RFCOMM_RPN_PARAMETER_BAUD_7200                                     0x02
#define RFCOMM_RPN_PARAMETER_BAUD_9600                                     0x03
#define RFCOMM_RPN_PARAMETER_BAUD_19200                                    0x04
#define RFCOMM_RPN_PARAMETER_BAUD_38400                                    0x05
#define RFCOMM_RPN_PARAMETER_BAUD_57600                                    0x06
#define RFCOMM_RPN_PARAMETER_BAUD_115200                                   0x07
#define RFCOMM_RPN_PARAMETER_BAUD_230400                                   0x08

#define RFCOMM_RPN_PARAMETER_DATA_FORMAT_DATA_BITS_MASK                    0x03
#define RFCOMM_RPN_PARAMETER_DATA_BITS_5                                   0x00
#define RFCOMM_RPN_PARAMETER_DATA_BITS_6                                   0x02
#define RFCOMM_RPN_PARAMETER_DATA_BITS_7                                   0x01
#define RFCOMM_RPN_PARAMETER_DATA_BITS_8                                   0x03

#define RFCOMM_RPN_PARAMETER_DATA_FORMAT_STOP_BITS_MASK                    0x04
#define RFCOMM_RPN_PARAMETER_STOP_BITS_1                                   0x00
#define RFCOMM_RPN_PARAMETER_STOP_BITS_1_5                                 0x04

#define RFCOMM_RPN_PARAMETER_DATA_FORMAT_PARITY_MASK                       0x38
#define RFCOMM_RPN_PARAMETER_PARITY_DISABLED                               0x00
#define RFCOMM_RPN_PARAMETER_PARITY_ODD                                    0x08
#define RFCOMM_RPN_PARAMETER_PARITY_EVEN                                   0x28
#define RFCOMM_RPN_PARAMETER_PARITY_MARK                                   0x18
#define RFCOMM_RPN_PARAMETER_PARITY_SPACE                                  0x38

#define RFCOMM_RPN_PARAMETER_FLOW_CONTROL_MASK                             0x3F
#define RFCOMM_RPN_PARAMETER_FLOW_CONTROL_DISABLED                         0x00
#define RFCOMM_RPN_PARAMETER_FLOW_CONTROL_XON_XOFF_ON_INPUT                0x01
#define RFCOMM_RPN_PARAMETER_FLOW_CONTROL_XON_XOFF_ON_OUTPUT               0x02
#define RFCOMM_RPN_PARAMETER_FLOW_CONTROL_RTR_ON_INPUT                     0x04
#define RFCOMM_RPN_PARAMETER_FLOW_CONTROL_RTR_ON_OUTPUT                    0x08
#define RFCOMM_RPN_PARAMETER_FLOW_CONTROL_RTC_ON_INPUT                     0x10
#define RFCOMM_RPN_PARAMETER_FLOW_CONTROL_RTC_ON_OUTPUT                    0x20

#define RFCOMM_RPN_PARAMETER_DEFAULT_XON_CHARACTER                         0x11

#define RFCOMM_RPN_PARAMETER_DEFAULT_XOFF_CHARACTER                        0x13

#define RFCOMM_RPN_PARAMETER_PARAMETER_MASK_NEGOTIATE_BIT_RATE             0x0001
#define RFCOMM_RPN_PARAMETER_PARAMETER_MASK_NEGOTIATE_DATA_BITS            0x0002
#define RFCOMM_RPN_PARAMETER_PARAMETER_MASK_NEGOTIATE_STOP_BITS            0x0004
#define RFCOMM_RPN_PARAMETER_PARAMETER_MASK_NEGOTIATE_PARITY               0x0008
#define RFCOMM_RPN_PARAMETER_PARAMETER_MASK_NEGOTIATE_PARITY_TYPE          0x0010
#define RFCOMM_RPN_PARAMETER_PARAMETER_MASK_NEGOTIATE_XON_CHARACTER        0x0020
#define RFCOMM_RPN_PARAMETER_PARAMETER_MASK_NEGOTIATE_XOFF_CHARACTER       0x0040
#define RFCOMM_RPN_PARAMETER_PARAMETER_MASK_NEGOTIATE_XON_XOFF_ON_INPUT    0x0100
#define RFCOMM_RPN_PARAMETER_PARAMETER_MASK_NEGOTIATE_XON_XOFF_ON_OUTPUT   0x0200
#define RFCOMM_RPN_PARAMETER_PARAMETER_MASK_NEGOTIATE_RTR_ON_INPUT         0x0400
#define RFCOMM_RPN_PARAMETER_PARAMETER_MASK_NEGOTIATE_RTR_ON_OUTPUT        0x0800
#define RFCOMM_RPN_PARAMETER_PARAMETER_MASK_NEGOTIATE_RTC_ON_INPUT         0x1000
#define RFCOMM_RPN_PARAMETER_PARAMETER_MASK_NEGOTIATE_RTC_ON_OUTPUT        0x2000

   /* The following constants reprsent the bit pattern to be used in the*/
   /* Remote Line Status command.  Note: The binary pattern for some    */
   /* parameters may be packed into a byte.  The constants provided for */
   /* packed parameters here represents the bit pattern that has been   */
   /* shifted to its proper position in the packed byte.                */
#define RFCOMM_RLS_PARAMETER_NO_ERROR                                      0x00
#define RFCOMM_RLS_PARAMETER_OVERRUN_ERROR                                 0x03
#define RFCOMM_RLS_PARAMETER_PARITY_ERROR                                  0x05
#define RFCOMM_RLS_PARAMETER_FRAMING_ERROR                                 0x09

   /* The following constants reprsent the bit masks to be used in the  */
   /* Modem Status command.  Note: The binary pattern for some          */
   /* parameters may be packed into a byte.  The constants provided for */
   /* packed parameters here represents the bit pattern that has been   */
   /* shifted to its proper position in the packed byte.                */
#define RFCOMM_MSC_PARAMETER_FC_BIT                                        0x02
#define RFCOMM_MSC_PARAMETER_RTC_BIT                                       0x04
#define RFCOMM_MSC_PARAMETER_RTR_BIT                                       0x08
#define RFCOMM_MSC_PARAMETER_IC_BIT                                        0x40
#define RFCOMM_MSC_PARAMETER_DV_BIT                                        0x80
#define RFCOMM_MSC_PARAMETER_BREAK_SIGNAL_PRESENT                          0x02

   /* The following constants represent the RFCOMM Break Signal Timeout */
   /* Interval, Maximum, and Minimum values.                            */
   /* * NOTE * The Break Timeout Internval is specified in Milliseconds.*/
#define RFCOMM_BREAK_TIMEOUT_INTERVAL                                      0xC8

#define RFCOMM_BREAK_SIGNAL_DETECTED                                       0x00
#define RFCOMM_BREAK_SIGNAL_MINIMUM                                        0x00
#define RFCOMM_BREAK_SIGNAL_MAXIMUM                                        0x0F

   /* RFCOMM SDP Protocol UUID's.                                       */

   /* The following MACRO is a utility MACRO that assigns the RFCOMM    */
   /* Bluetooth Universally Unique Identifier (RFCOMM_UUID_16) to the   */
   /* specified UUID_16_t variable.  This MACRO accepts one parameter   */
   /* which is the UUID_16_t variable that is receive the RFCOMM_UUID_16*/
   /* Constant value.                                                   */
#define SDP_ASSIGN_RFCOMM_UUID_16(_x)           ASSIGN_SDP_UUID_16((_x), 0x00, 0x03)

   /* The following MACRO is a utility MACRO that assigns the RFCOMM    */
   /* Bluetooth Universally Unique Identifier (RFCOMM_UUID_32) to the   */
   /* specified UUID_32_t variable.  This MACRO accepts one parameter   */
   /* which is the UUID_32_t variable that is receive the RFCOMM_UUID_32*/
   /* Constant value.                                                   */
#define SDP_ASSIGN_RFCOMM_UUID_32(_x)           ASSIGN_SDP_UUID_32((_x), 0x00, 0x00, 0x00, 0x03)

   /* The following MACRO is a utility MACRO that assigns the RFCOMM    */
   /* Bluetooth Universally Unique Identifier (RFCOMM_UID_128) to the   */
   /* specified UUID_128_t variable.  This MACRO accepts one parameter  */
   /* which is the UUID_128_t variable that is receive the              */
   /* RFCOMM_UUID_128 Constant value.                                   */
#define SDP_ASSIGN_RFCOMM_UUID_128(_x)          ASSIGN_SDP_UUID_128((_x), 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /* The following definition is the assigned Bluetooth RFCOMM Server  */
   /* Protocol Name.                                                    */
#define RFCOMM_SERVER_PROTOCOL_NAME                     "com.bt"

   /* The following Constants represent the defined Bluetooth RFCOMM    */
   /* Option Type Designator Values.                                    */
#define RFCOMM_OPEN_PARAMS_OPTION_TYPE_MAX_FRAME_SIZE                      0x01
#define RFCOMM_OPEN_PARAMS_OPTION_TYPE_INITIAL_CREDITS                     0x02

typedef __PACKED_STRUCT_BEGIN__ struct _tagRFCOMM_Frame_Header_t
{
   NonAlignedByte_t Address;
   NonAlignedByte_t Control;
} __PACKED_STRUCT_END__ RFCOMM_Frame_Header_t;

#define RFCOMM_FRAME_HEADER_SIZE                        (sizeof(RFCOMM_Frame_Header_t))

typedef __PACKED_STRUCT_BEGIN__ struct _tagRFCOMM_Control_Frame_t
{
   RFCOMM_Frame_Header_t FrameHeader;
   NonAlignedByte_t      Length;
   NonAlignedByte_t      FCS;
} __PACKED_STRUCT_END__ RFCOMM_Control_Frame_t;

#define RFCOMM_CONTROL_FRAME_SIZE                       (sizeof(RFCOMM_Control_Frame_t))

typedef __PACKED_STRUCT_BEGIN__ struct _tagRFCOMM_Control_Frame_2_Byte_Length_t
{
   RFCOMM_Frame_Header_t FrameHeader;
   NonAlignedByte_t      FrameLengthLow;
   NonAlignedByte_t      FrameLengthHigh;
   NonAlignedByte_t      FCS;
} __PACKED_STRUCT_END__ RFCOMM_Control_Frame_2_Byte_Length_t;

#define RFCOMM_CONTROL_FRAME_2_BYTE_LENGTH_SIZE         (sizeof(RFCOMM_Control_Frame_2_Byte_Length_t))

   /* A UIH Frame can have a varying amount of data.  The UIH Frame must*/
   /* not exceed the Maximum Frame Size that was set when the           */
   /* Connection was negotiated.                                        */
   /* * NOTE * The FCS Member in this structure should NOT be used to   */
   /*          access the FCS value.  It's place in this structure is   */
   /*          ONLY to show the programmer that the FCS Member follows  */
   /*          the Variable Data Member which is of varying length.     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagRFCOMM_UIH_Frame_Header_t
{
   RFCOMM_Frame_Header_t FrameHeader;
   NonAlignedByte_t      FrameLengthLow;
   NonAlignedByte_t      FrameLengthHigh;
   NonAlignedByte_t      Variable_Data[1];
   NonAlignedByte_t      FCS;
} __PACKED_STRUCT_END__ RFCOMM_UIH_Frame_Header_t;

   /* A UIH Frame can have a varying amount of data.  The UIH frame must*/
   /* not exceed the Maximum Frame Size that was set when the connection*/
   /* was negotiated.                                                   */
   /* * NOTE * The FCS Member in this structure should NOT be used to   */
   /*          access the FCS value.  It's place in this structure is   */
   /*          ONLY to show the programmer that the FCS Member follows  */
   /*          the Variable Data Member which is of varying length.     */
#define RFCOMM_UIH_FRAME_SIZE(_x)                       (sizeof(RFCOMM_UIH_Frame_Header_t)-sizeof(NonAlignedByte_t)+(unsigned int)(_x))

   /* A UIH Frame can have a varying amount of data.  The UIH frame must*/
   /* not exceed the Maximum Frame Size that was set when the connection*/
   /* was negotiated.  This MACRO is a utility MACRO that returns the   */
   /* size of the UIH Frame Header that goes before the UIH Frame Data. */
#define RFCOMM_UIH_FRAME_HEADER_SIZE                    (sizeof(RFCOMM_UIH_Frame_Header_t)-sizeof(NonAlignedByte_t)-sizeof(NonAlignedByte_t))

   /* The following structure is used when sending UIH Message over the */
   /* Control Channel.  This is different from the above structure in   */
   /* that the length of the data that is sent in this frame can be held*/
   /* in a single byte.  This also includes a Type field that is used to*/
   /* indicate the type of control information that is contained in the */
   /* message.                                                          */
   /* * NOTE * The FCS Member in this structure should NOT be used to   */
   /*          access the FCS value.  It's place in this structure is   */
   /*          ONLY to show the programmer that the FCS Member follows  */
   /*          the Variable Data Member which is of varying length.     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagRFCOMM_UIH_Message_Header_t
{
   RFCOMM_Frame_Header_t FrameHeader;
   NonAlignedByte_t      FrameLength;
   NonAlignedByte_t      Type;
   NonAlignedWord_t      Length;
   NonAlignedByte_t      Variable_Data[1];
   NonAlignedByte_t      FCS;
} __PACKED_STRUCT_END__ RFCOMM_UIH_Message_Header_t;

   /* A UIH Message have a varying amount of data.  The UIH message must*/
   /* not exceed the Maximum Frame Size that was set when the connection*/
   /* was negotiated.  NOTE: The FCS field was added to the structure to*/
   /* indiacte that this extra byte is contained in the frame.  However,*/
   /* the FCS fields should not be used to access the FCS byte since it */
   /* will be in a variable position within the frame due to that       */
   /* variable size of the data.                                        */
#define RFCOMM_UIH_MESSAGE_SIZE(_x)                     (sizeof(RFCOMM_UIH_Message_Header_t)-sizeof(NonAlignedByte_t)+(unsigned int)(_x))

   /* A UIH Message can have a varying amount of data.  The UIH Message */
   /* must not exceed the Maximum Frame Size that was set when the      */
   /* connection was negotiated.  This MACRO is a utility MACRO that    */
   /* returns the size of the UIH Message Header that goes before the   */
   /* UIH Message Data.                                                 */
#define RFCOMM_UIH_MESSAGE_HEADER_SIZE                  (sizeof(RFCOMM_UIH_Message_Header_t)-sizeof(NonAlignedByte_t)-sizeof(NonAlignedByte_t))

   /* The following structure contains the Remote Negotiation           */
   /* Parameters that are used with the DLCI Parameter Negotiation      */
   /* Command (PN).                                                     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagRFCOMM_PN_Message_Data_t
{
   NonAlignedByte_t DLCI;
   NonAlignedByte_t FlowType;
   NonAlignedByte_t Priority;
   NonAlignedByte_t AcknowledgementTimer;
   NonAlignedWord_t FrameSize;
   NonAlignedByte_t Retransmission;
   NonAlignedByte_t Credits;
} __PACKED_STRUCT_END__ RFCOMM_PN_Message_Data_t;

#define RFCOMM_PN_MESSAGE_DATA_SIZE                     (sizeof(RFCOMM_PN_Message_Data_t))

   /* The following structure contains the Remote Negotiation           */
   /* Parameters that are used with the Remote Port Negotiation Command */
   /* (RPN).                                                            */
typedef __PACKED_STRUCT_BEGIN__ struct _tagRFCOMM_RPN_Message_Data_t
{
   NonAlignedByte_t DLCI;
   NonAlignedByte_t BaudRate;
   NonAlignedByte_t DataFormat;
   NonAlignedByte_t FlowControl;
   NonAlignedByte_t XOnCharacter;
   NonAlignedByte_t XOffCharacter;
   NonAlignedWord_t ParameterMask;
} __PACKED_STRUCT_END__ RFCOMM_RPN_Message_Data_t;

#define RFCOMM_RPN_MESSAGE_DATA_SIZE                    (sizeof(RFCOMM_RPN_Message_Data_t))

#endif
