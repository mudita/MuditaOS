/*
 * Copyright 2000 - 2014 Stonestreet One.
 * All Rights Reserved.
 * Author:  Damon Lange
 *** MODIFICATION HISTORY ****************************************************
 *
 *   mm/dd/yy  F. Lastname    Description of Modification
 *   --------  -----------    ------------------------------------------------
 *   03/15/01  D. Lange       Initial creation.
 ****************************************************************************
 */

/**
 *  @file SPPAPI.h
 *
 *  @brief Stonestreet One Bluetooth Stack SPP API Type Definitions,
 *      Constants, and Prototypes. To use any of the following API's,
 *      include the following declaration in your application.
 *
 *  @code
 *  #include "SS1BTPS.h"
 *  @endcode
 *  ============================================================================
 */


#ifndef __SPPAPIH__
#define __SPPAPIH__

#include "BTPSKRNL.h"          /*! BTPS Kernel Prototypes/Constants. */

#include "BTAPITyp.h"          /*! Bluetooth API Type Definitions. */
#include "BTTypes.h"           /*! Bluetooth Type Definitions/Constants. */
#include "RFCOMMT.h"           /*! Bluetooth RFCOMM Type Definitions/Constants. */
#include "SDPAPI.h"            /*! Bluetooth SDP API Definitions/Constants. */

#include "BTPSCFG.h"            /*! BTPS Configuration Constants. */

   /*! The following Constants represent the Minimum and Maximum Port
    * Numbers that can be opened (both locally and remotely).  These
    * constants specify the range for the Port Number parameters in the
    * Open Port Functions.
    */
#define SPP_PORT_NUMBER_MINIMUM           (RFCOMM_MINIMUM_SERVER_CHANNEL_ID)
#define SPP_PORT_NUMBER_MAXIMUM           (RFCOMM_MAXIMUM_SERVER_CHANNEL_ID)

   /*! The following MACRO is a utility MACRO that exists to aid code
    * readability of testing whether or not a specified SPP Port Number
    * Value is valid. The first parameter to this MACRO is the SPP
    * Port Number Value to verify.  This MACRO returns a boolean value
    * based upon whether or not the specified SPP Port Number value
    * is valid.  This MACRO returns a boolean TRUE if the specified
    * SPP Port Number value is valid, or a boolean FALSE if the
    * specified SPP Port Number value is invalid.
    */
#define SPP_VALID_PORT_NUMBER(_x)                               (((_x) >= SPP_PORT_NUMBER_MINIMUM) && ((_x) <= SPP_PORT_NUMBER_MAXIMUM))

   /*! SDP Service Classes for the Serial Port Profile.
    * The following MACRO is a utility MACRO that assigns the Serial
    * Port Service Class Bluetooth Universally Unique Identifier
    * (SERIAL_PORT_PROFILE_UUID_16) to the specified UUID_16_t variable.
    * This MACRO accepts one parameter which is the UUID_16_t variable
    * that is to receive the SERIAL_PORT_PROFILE_UUID_16 Constant value.
    */
#define SDP_ASSIGN_SERIAL_PORT_PROFILE_UUID_16(_x)  ASSIGN_SDP_UUID_16((_x), 0x11, 0x01)

   /*! The following MACRO is a utility MACRO that assigns the Serial
    * Port Service Class Bluetooth Universally Unique Identifier
    * (SERIAL_PORT_PROFILE_UUID_16) to the specified UUID_32_t variable.
    * This MACRO accepts one parameter which is the UUID_32_t variable
    * that is to receive the SERIAL_PORT_PROFILE_UUID_32 Constant value.
    */
#define SDP_ASSIGN_SERIAL_PORT_PROFILE_UUID_32(_x)  ASSIGN_SDP_UUID_32((_x), 0x00, 0x00, 0x11, 0x01)

   /*! The following MACRO is a utility MACRO that assigns the Serial
    * Port Service Class Bluetooth Universally Unique Identifier
    * (SERIAL_PORT_PROFILE_UUID_128) to the specified UUID_128_t
    * variable.  This MACRO accepts one parameter which is the
    * UUID_128_t variable that is to receive the
    * SERIAL_PORT_PROFILE_UUID_128 Constant value.
    */
#define SDP_ASSIGN_SERIAL_PORT_PROFILE_UUID_128(_x) ASSIGN_SDP_UUID_128((_x), 0x00, 0x00, 0x11, 0x01, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /*! The following constants represent the Port Open Status Values
    * that are possible in the SPP Open Port Confirmation Event Data
    * Information.
    */
#define SPP_OPEN_PORT_STATUS_SUCCESS                                    0x00
#define SPP_OPEN_PORT_STATUS_CONNECTION_TIMEOUT                         0x01
#define SPP_OPEN_PORT_STATUS_CONNECTION_REFUSED                         0x02
#define SPP_OPEN_PORT_STATUS_UNKNOWN_ERROR                              0x03

   /*! The following Constants represent the allowable Minimum and
    * Maximum Baud Rate (Bits/Second) that are supported by the Serial
    * Port Profile.
    */
#define SPP_BAUD_RATE_MINIMUM                                           2400
#define SPP_BAUD_RATE_MAXIMUM                                         230400

   /*! The following Constants represent the ONLY allowable Baud Rates
    * (Bits/Second) that are supported by the Serial Port Profile.
    */
#define SPP_BAUD_RATE_2400                                              2400
#define SPP_BAUD_RATE_4800                                              4800
#define SPP_BAUD_RATE_7200                                              7200
#define SPP_BAUD_RATE_9600                                              9600
#define SPP_BAUD_RATE_19200                                            19200
#define SPP_BAUD_RATE_38400                                            38400
#define SPP_BAUD_RATE_57600                                            57600
#define SPP_BAUD_RATE_115200                                          115200
#define SPP_BAUD_RATE_230400                                          230400

   /*! The following Constants represent the allowable Minimum and
    * Maximum Number of Data bits that are supported by the Serial
    * Port Profile.
    */
#define SPP_DATA_BITS_MINIMUM                                           0x05
#define SPP_DATA_BITS_MAXIMUM                                           0x08

#define SPP_DATA_BITS_5                                                 0x05
#define SPP_DATA_BITS_6                                                 0x06
#define SPP_DATA_BITS_7                                                 0x07
#define SPP_DATA_BITS_8                                                 0x08

   /*! The following enumerated type represents the supported Stop Bit
    * types for the Serial Port Profile.
    */
typedef enum
{
   sbOneStopBit,
   sbOneOneHalfStopBit
} SPP_Stop_Bits_t;

   /*! The following enumerated type represents the supported Parity
    * types for the Serial Port Profile.
    */
typedef enum
{
   ptNone,
   ptOdd,
   ptEven,
   ptMark,
   ptSpace
} SPP_Parity_t;

   /*! The following Bit Definitions represent the defined Bit Maps that
    * specify how many fields (if any) of the
    * SPP_Port_Open_Information_t are present.
    */
#define SPP_PORT_INFORMATION_NONE_VALUE                         0x00000000
#define SPP_PORT_INFORMATION_BAUD_RATE_BIT                      0x00000001
#define SPP_PORT_INFORMATION_DATA_BITS_BIT                      0x00000002
#define SPP_PORT_INFORMATION_STOP_BITS_BIT                      0x00000004
#define SPP_PORT_INFORMATION_PARITY_BIT                         0x00000008
#define SPP_PORT_INFORMATION_XON_CHARACTER_BIT                  0x00000010
#define SPP_PORT_INFORMATION_XOFF_CHARACTER_BIT                 0x00000020
#define SPP_PORT_INFORMATION_FLOW_CONTROL_BIT                   0x00000040

   /*! The following Bit Definitions represent the defined Flow Control
    * Bit Masks that are supported by the Serial Port Profile.
    */
#define SPP_FLOW_CONTROL_DISABLED_VALUE                         0x00000000
#define SPP_FLOW_CONTROL_XON_XOFF_INPUT_ENABLED_BIT             0x00000001
#define SPP_FLOW_CONTROL_XON_XOFF_OUTPUT_ENABLED_BIT            0x00000002
#define SPP_FLOW_CONTROL_CTS_INPUT_ENABLED_BIT                  0x00000004
#define SPP_FLOW_CONTROL_RTS_OUTPUT_ENABLED_BIT                 0x00000008
#define SPP_FLOW_CONTROL_DSR_INPUT_ENABLED_BIT                  0x00000010
#define SPP_FLOW_CONTROL_DTR_OUTPUT_ENABLED_BIT                 0x00000020

   /*! The following structure represents the Port Information that can
    * be specified when opening a Port.  This information is optional
    * because the Serial Port Profile does NOT need this information to
    * virtualize a Serial Port over Bluetooth.  This information would
    * only be of value for an application that needed to open a Serial
    * Port (physical) to pass data through.  There also exist API's to
    * query and set the this information about a Serial Port.
    */
typedef struct _tagSPP_Port_Information_t
{
   unsigned int    PortInformationMask;
   unsigned long   BaudRate;
   unsigned int    DataBits;
   SPP_Stop_Bits_t StopBits;
   SPP_Parity_t    Parity;
   Byte_t          XOnCharacter;
   Byte_t          XOffCharacter;
   unsigned int    FlowControlMask;
} SPP_Port_Information_t;

#define SPP_PORT_INFORMATION_SIZE                       (sizeof(SPP_Port_Information_t))

   /*! The following structure is used with the SPP_Register_SDP_Record()
    * function.  This structure (when specified) contains additional
    * SDP Service Information that will be added to the SDP SPP Service
    * Record Entry.  The first member of this strucuture specifies the
    * Number of Service Class UUID's that are present in the
    * SDPUUIDEntries Array.  This member must be at least one, and
    * the SDPUUIDEntries member must point to an array of SDP UUID
    * Entries that contains (at least) as many entries specified by
    * the NumberServiceClassUUID member.  The ProtocolList member is
    * an SDP Data Element Sequence that contains a list of Protocol
    * Information that will be added to the generic SDP Service
    * Record.
    */
typedef struct _tagSPP_SDP_Service_Record_t
{
   unsigned int        NumberServiceClassUUID;
   SDP_UUID_Entry_t   *SDPUUIDEntries;
   SDP_Data_Element_t *ProtocolList;
} SPP_SDP_Service_Record_t;

#define SPP_SDP_SERVICE_RECORD_SIZE                     (sizeof(SPP_SDP_Service_Record_t))

   /*! The following structure is used with the
    * SPP_Register_Raw_SDP_Record() function.  This structure (when
    * specified) contains additional SDP Service Information that will
    * be added to the SDP SPP Service Record Entry.  The first member of
    * this strucuture specifies the Number of Service Class UUID's that
    * are present in the SDPUUIDEntries Array.  This member must be at
    * least one, and the SDPUUIDEntries member must point to an array of
    * SDP UUID Entries that contains (at least) as many entries
    * specified by the NumberServiceClassUUID member.  The
    * NumberOfProtocolDataListUUIDOffsets and
    * ProtocolDataListUUIDOffsets specify the offsets of the UUIDs in
    * the specified ProtocolDataList data (if any UUIDs).  The
    * ProtocolDataListLength and ProtocolDataList members must contain
    * (if specified) a formatted SDP Data Element Sequence that contains
    * a list of Protocol Information that will be added to the generic
    * SDP Service Record.
    */
typedef struct _tagSPP_SDP_Raw_Service_Record_t
{
   unsigned int      NumberServiceClassUUID;
   SDP_UUID_Entry_t *SDPUUIDEntries;
   unsigned int      NumberOfProtocolDataListUUIDOffsets;
   Word_t           *ProtocolDataListUUIDOffsets;
   unsigned int      ProtocolDataListLength;
   Byte_t           *ProtocolDataList;
} SPP_SDP_Raw_Service_Record_t;

#define SPP_SDP_RAW_SERVICE_RECORD_SIZE                  (sizeof(SPP_SDP_Raw_Service_Record_t))

   /*! The following Constants represent the Minimum, Maximum and
    * Default Buffer Sizes that are supported for Transmit/Receive
    * Buffers.  These Constants can be used with the
    * SPP_Change_Buffer_Size(), SPP_Set_Configuration_Parameters(), and
    * the SPP_Get_Configuration_Parameters() functions.  The special
    * Constant SPP_BUFFER_SIZE_CURRENT is used with the
    * SPP_Change_Buffer_Size() and the
    * SPP_Set_Configuration_Parameters() function to inform the function
    * NOT to change the Buffer Size.
    */
#define SPP_BUFFER_SIZE_MINIMUM                                        (BTPS_CONFIGURATION_SPP_MINIMUM_SUPPORTED_STACK_BUFFER_SIZE)
#define SPP_BUFFER_SIZE_MAXIMUM                                        (BTPS_CONFIGURATION_SPP_MAXIMUM_SUPPORTED_STACK_BUFFER_SIZE)
#define SPP_BUFFER_SIZE_DEFAULT_TRANSMIT                               (BTPS_CONFIGURATION_SPP_DEFAULT_TRANSMIT_BUFFER_SIZE)
#define SPP_BUFFER_SIZE_DEFAULT_RECEIVE                                (BTPS_CONFIGURATION_SPP_DEFAULT_RECEIVE_BUFFER_SIZE)
#define SPP_BUFFER_SIZE_CURRENT                                         0

   /*! The following Constants represent the Minimum, Maximum and Default
    * RFCOMM Frame Sizes that are supported by the SPP Module.  These
    * Constants can be used with the SPP_Set_Configuration_Parameters()
    * and the SPP_Get_Configuration_Parameters() functions.  The special
    * Constant SPP_FRAME_SIZE_CURRENT is used with the
    * SPP_Set_Configuration_Parameters() function to inform the function
    * NOT to change the Default Frame Size.
    */
#define SPP_FRAME_SIZE_MINIMUM                                         (RFCOMM_FRAME_SIZE_MINIMUM_VALUE)
#define SPP_FRAME_SIZE_MAXIMUM                                         (RFCOMM_MAXIMUM_SUPPORTED_STACK_FRAME_SIZE)
#define SPP_FRAME_SIZE_DEFAULT                                         (BTPS_CONFIGURATION_SPP_DEFAULT_FRAME_SIZE)
#define SPP_FRAME_SIZE_CURRENT                                          0

   /*! The following structure represents the structure of the User
    * configurable parameter structure that can be used to change the
    * default behaviour of SPP Clients and Servers.  This structure is
    * used with the SPP_Get_Configuration_Parameters() and the
    * SPP_Set_Configuration_Parameters() functions.
    */
typedef struct _tagSPP_Configuration_Params_t
{
   Word_t       MaximumFrameSize;
   unsigned int TransmitBufferSize;
   unsigned int ReceiveBufferSize;
} SPP_Configuration_Params_t;

#define SPP_CONFIGURATION_PARAMS_SIZE                   (sizeof(SPP_Configuration_Params_t))

   /*! The following Bit Definitions represent the allowable Flags for an
    * SPP_Purge_Buffer() operation.  Either Transmit or Receive can be
    * specified simultaneously.  Abort and Flush may not be used
    * simultaneously.
    */
#define SPP_PURGE_MASK_TRANSMIT_ABORT_BIT                       0x00000001
#define SPP_PURGE_MASK_RECEIVE_ABORT_BIT                        0x00000002
#define SPP_PURGE_MASK_TRANSMIT_FLUSH_BIT                       0x00000004

   /*! The following enumerated type represents the Status of the Break
    * Signal for a specified Serial Port.
    */
typedef enum
{
   bsBreakCleared,
   bsBreakReceived
} SPP_Break_Status_t;

   /*! The following Constants represent the Bit Mask that specifies
    * the value of an individual Modem/Port Control Signal.  If the
    * specified Bit has a binary value of '1', then the Signal is
    * considered to be set, else it is considered NOT set (clear).
    * This Bit Mask is used with the SPP_Port_Status() function.
    */
#define SPP_PORT_STATUS_CLEAR_VALUE                             0x00000000
#define SPP_PORT_STATUS_RTS_CTS_BIT                             0x00000001
#define SPP_PORT_STATUS_DTR_DSR_BIT                             0x00000002
#define SPP_PORT_STATUS_RING_INDICATOR_BIT                      0x00000004
#define SPP_PORT_STATUS_CARRIER_DETECT_BIT                      0x00000008

   /*! The following constants represent the valid SPP Break Interval
    * values that can be used with the SPP_Send_Break() function.
    */
#define SPP_BREAK_SIGNAL_DETECTED           (RFCOMM_BREAK_SIGNAL_DETECTED)
#define SPP_BREAK_SIGNAL_MINIMUM            (RFCOMM_BREAK_TIMEOUT_INTERVAL*RFCOMM_BREAK_SIGNAL_MINIMUM)
#define SPP_BREAK_SIGNAL_MAXIMUM            (RFCOMM_BREAK_TIMEOUT_INTERVAL*RFCOMM_BREAK_SIGNAL_MAXIMUM)

   /*! The following Constants represent the Bit Mask that specifies
    * the value of an individual Modem/Port Line Status Signal.  If the
    * specified Bit has a binary value of '1', then the Signal is
    * considered to be set, else it is considered NOT set (clear).
    * This Bit Mask is used with the SPP_Line_Status() function.
    */
#define SPP_LINE_STATUS_NO_ERROR_VALUE                                  0x00
#define SPP_LINE_STATUS_OVERRUN_ERROR_BIT_MASK                          0x01
#define SPP_LINE_STATUS_PARITY_ERROR_BIT_MASK                           0x02
#define SPP_LINE_STATUS_FRAMING_ERROR_BIT_MASK                          0x04

   /*! The following enumerated type represents the supported Server
    * Connection Modes supported by a specified Serial Port Server.
    */
typedef enum
{
   smAutomaticAccept,
   smAutomaticReject,
   smManualAccept
} SPP_Server_Connection_Mode_t;

#define SPP_DEFAULT_SERVER_CONNECTION_MODE                              (BTPS_CONFIGURATION_SPP_DEFAULT_SERVER_CONNECTION_MODE)

   /*! The following enumerated type is used with the
    * SPP_Get_Port_Connection_State() function to determine the current
    * SPP/RFCOMM Connection state of a specific Client/Server Port.
    */
typedef enum
{
   csPortNotPresent,
   csPortBusy,
   csPortDisconnecting,
   csPortReady
} SPP_Port_Connection_State_t;

   /*! SPP Event API Types.
    */
typedef enum
{
   etPort_Open_Indication, /*!< Indicate that a Remote Port Open connection has been made. */
   etPort_Open_Confirmation, /*!< Confirm that a Port Open request has been responded to or errored out. */
   etPort_Close_Port_Indication, /*!< Indicate that a port has been closed (unregistered). */
   etPort_Status_Indication, /*!< Indicate that a change in port status has been received. */
   etPort_Data_Indication, /*!< Indicate that data has arrived on a port. */
   etPort_Transmit_Buffer_Empty_Indication, /*!< Indicate when the Transmit Buffer is Empty (only if the Transmit Buffer was completely full or the SPP_Purge_Buffer() function was called with the option to flush the transmit buffer). */
   etPort_Line_Status_Indication, /*!< Indicate that a change in line status has been received. */
   etPort_Send_Port_Information_Indication, /*!< Indicate that a remote device’s port parameters have been received (start of negotiation of parameters). */
   etPort_Send_Port_Information_Confirmation, /*!< Confirm that a response has been received to a send port parameters command.*/
   etPort_Query_Port_Information_Indication, /*!< Indicate that a request to send current port parameters has been received. */
   etPort_Query_Port_Information_Confirmation, /*!< Confirm that a response has been received to a request to send current port parameters. */
   etPort_Open_Request_Indication /*!< Indicate that a Remote Port Open request has been received. */
} SPP_Event_Type_t;

typedef struct _tagSPP_Open_Port_Indication_Data_t
{
   unsigned int SerialPortID; /*!< The port this event applies to. */
   BD_ADDR_t    BD_ADDR; /*!< Address of the Bluetooth device. */
} SPP_Open_Port_Indication_Data_t;

#define SPP_OPEN_PORT_INDICATION_DATA_SIZE              (sizeof(SPP_Open_Port_Indication_Data_t))

typedef struct _tagSPP_Open_Port_Confirmation_Data_t
{
   unsigned int SerialPortID; /*!< The port this event applies to. */
   unsigned int PortOpenStatus; /*!< Status of the open request, one of the following values:
                                     SPP_OPEN_PORT_STATUS_SUCCESS
                                     SPP_OPEN_PORT_STATUS_CONNECTION_TIMEOUT
                                     SPP_OPEN_PORT_STATUS_CONNECTION_REFUSED
                                     SPP_OPEN_PORT_STATUS_UNKNOWN_ERROR
                                 */
} SPP_Open_Port_Confirmation_Data_t;

#define SPP_OPEN_PORT_CONFIRMATION_DATA_SIZE            (sizeof(SPP_Open_Port_Confirmation_Data_t))

typedef struct _tagSPP_Close_Port_Indication_Data_t
{
   unsigned int SerialPortID; /*!< SerialPortID The port this event applies to. */
} SPP_Close_Port_Indication_Data_t;

#define SPP_CLOSE_PORT_INDICATION_DATA_SIZE             (sizeof(SPP_Close_Port_Indication_Data_t))

typedef struct _tagSPP_Port_Status_Indication_Data_t
{
   unsigned int       SerialPortID; /*!< The port this event applies to.  */
   unsigned int       PortStatus; /*!< The current status of the port sent from the remote side;  a bit mask that may contain one or more of the following bits:
                                       SPP_PORT_STATUS_RTS_CTS_BIT
                                       SPP_PORT_STATUS_DTR_DSR_BIT
                                       SPP_PORT_STATUS_RING_INDICATOR_BIT
                                       SPP_PORT_STATUS_CARRIER_DETECT_BIT
                                   */
   SPP_Break_Status_t BreakStatus; /*!< One of the following values:
                                        bsBreakCleared
                                        bsBreakReceived
                                    */
   unsigned int       BreakTimeout; /*!< Value of the Break Timeout, in seconds, if BreakStatus is set to bsBreakReceived. */
} SPP_Port_Status_Indication_Data_t;

#define SPP_PORT_STATUS_INDICATION_DATA_SIZE           (sizeof(SPP_Port_Status_Indication_Data_t))

typedef struct _tagSPP_Data_Indication_Data_t
{
   unsigned int  SerialPortID; /*!< The port this event applies to. */
   Word_t        DataLength; /*!< Length of the data which is waiting to be read. */
} SPP_Data_Indication_Data_t;

#define SPP_DATA_INDICATION_DATA_SIZE                   (sizeof(SPP_Data_Indication_Data_t))

typedef struct _tagSPP_Transmit_Buffer_Empty_Indication_Data_t
{
   unsigned int  SerialPortID; /*!< The port this event applies to. */
} SPP_Transmit_Buffer_Empty_Indication_Data_t;

#define SPP_TRANSMIT_BUFFER_EMPTY_INDICATION_DATA_SIZE  (sizeof(SPP_Transmit_Buffer_Empty_Indication_Data_t))

typedef struct _tagSPP_Line_Status_Indication_Data_t
{
   unsigned int      SerialPortID; /*!< The port this event applies to. */
   unsigned int      SPPLineStatusMask; /*!< Status bits, which may contain one or more of the following bit mask values:
                                             SPP_LINE_STATUS_OVERRUN_ERROR_BIT_MASK
                                             SPP_LINE_STATUS_PARITY_ERROR_BIT_MASK
                                             SPP_LINE_STATUS_FRAMING_ERROR_BIT_MASK
                                             Or one may the following value:
                                             SPP_LINE_STATUS_NO_ERROR_VALUE
                                         */
} SPP_Line_Status_Indication_Data_t;

#define SPP_LINE_STATUS_INDICATION_DATA_SIZE            (sizeof(SPP_Line_Status_Indication_Data_t))

typedef struct _tagSPP_Send_Port_Information_Indication_Data_t
{
   unsigned int           SerialPortID; /*!< The port this event applies to. */
   SPP_Port_Information_t SPPPortInformation; /*!< The port parameters from the remote side */
} SPP_Send_Port_Information_Indication_Data_t;

#define SPP_SEND_PORT_INFORMATION_INDICATION_DATA_SIZE (sizeof(SPP_Send_Port_Information_Indication_Data_t))

typedef struct _tagSPP_Send_Port_Information_Confirmation_Data_t
{
   unsigned int           SerialPortID; /*!< The port this event applies to. */
   SPP_Port_Information_t SPPPortInformation; /*!< Port parameters.  See etPort_Send_Port_Information_Indication event for a complete listing of this structure. */
} SPP_Send_Port_Information_Confirmation_Data_t;

#define SPP_SEND_PORT_INFORMATION_CONFIRMATION_DATA_SIZE (sizeof(SPP_Send_Port_Information_Confirmation_Data_t))

typedef struct _tagSPP_Query_Port_Information_Indication_Data_t
{
   unsigned int  SerialPortID; /*!< The port this event applies to. */
} SPP_Query_Port_Information_Indication_Data_t;

#define SPP_QUERY_PORT_INFORMATION_INDICATION_DATA_SIZE (sizeof(SPP_Query_Port_Information_Indication_Data_t))

typedef struct _tagSPP_Query_Port_Information_Confirmation_Data_t
{
   unsigned int           SerialPortID; /*!< The port this event applies to. */
   SPP_Port_Information_t SPPPortInformation; /*!< Port parameters.  See etPort_Send_Port_Information_Indication event for a complete listing of this structure. */
} SPP_Query_Port_Information_Confirmation_Data_t;

#define SPP_QUERY_PORT_INFORMATION_CONFIRMATION_DATA_SIZE (sizeof(SPP_Query_Port_Information_Confirmation_Data_t))

typedef struct _tagSPP_Open_Port_Request_Indication_Data_t
{
   unsigned int SerialPortID; /*!< The port this event applies to. */
   BD_ADDR_t    BD_ADDR; /*!< Address of the Bluetooth device. */
} SPP_Open_Port_Request_Indication_Data_t;

#define SPP_OPEN_PORT_REQUEST_INDICATION_DATA_SIZE      (sizeof(SPP_Open_Port_Request_Indication_Data_t))

   /*! The following structure represents the container structure for
    * Holding all SPP Event Data Data.
    */
typedef struct _tagSPP_Event_Data_t
{
   SPP_Event_Type_t Event_Data_Type;
   Word_t           Event_Data_Size;
   union
   {
      SPP_Open_Port_Indication_Data_t                *SPP_Open_Port_Indication_Data;
      SPP_Open_Port_Confirmation_Data_t              *SPP_Open_Port_Confirmation_Data;
      SPP_Close_Port_Indication_Data_t               *SPP_Close_Port_Indication_Data;
      SPP_Port_Status_Indication_Data_t              *SPP_Port_Status_Indication_Data;
      SPP_Data_Indication_Data_t                     *SPP_Data_Indication_Data;
      SPP_Transmit_Buffer_Empty_Indication_Data_t    *SPP_Transmit_Buffer_Empty_Indication_Data;
      SPP_Line_Status_Indication_Data_t              *SPP_Line_Status_Indication_Data;
      SPP_Send_Port_Information_Indication_Data_t    *SPP_Send_Port_Information_Indication_Data;
      SPP_Send_Port_Information_Confirmation_Data_t  *SPP_Send_Port_Information_Confirmation_Data;
      SPP_Query_Port_Information_Indication_Data_t   *SPP_Query_Port_Information_Indication_Data;
      SPP_Query_Port_Information_Confirmation_Data_t *SPP_Query_Port_Information_Confirmation_Data;
      SPP_Open_Port_Request_Indication_Data_t        *SPP_Open_Port_Request_Indication_Data;
   } Event_Data;
} SPP_Event_Data_t;

#define SPP_EVENT_DATA_SIZE                             (sizeof(SPP_Event_Data_t))

   /*! The following declared type represents the Prototype Function for
    * an SPP Event Receive Data Callback. This function will be called
    * whenever a SPP Event occurs that is associated with the specified
    * Bluetooth Stack ID. @param BluetoothStackID This function passes to the caller the
    * Bluetooth Stack ID. @param SPP_Event_Data The SPP Event Data that occurred.
    * @param CallbackParameter The SPP
    * Event Callback Parameter that was specified when this Callback
    * was installed.  The caller is free to use the contents of the SPP
    * SPP Event Data ONLY in the context of this callback.  If the
    * caller requires the Data for a longer period of time, then the
    * callback function MUST copy the data into another Data Buffer.
    * This function is guaranteed NOT to be invoked more than once
    * simultaneously for the specified installed callback (i.e. this
    * function DOES NOT have be reentrant).  It Needs to be noted
    * however, that if the same Callback is installed more than once,
    * then the callbacks will be called serially.  Because of this, the
    * processing in this function should be as efficient as possible.
    * It should also be noted that this function is called in the Thread
    * Context of a Thread that the User does NOT own.  Therefore,
    * processing in this function should be as efficient as possible
    * (this argument holds anyway because another SPP Event will not be
    * processed while this function call is outstanding).
    * \note This function MUST NOT Block and wait for events that
    *            can only be satisfied by Receiving SPP Event Packets.
    *            A Deadlock WILL occur because NO SPP Event Callbacks
    *            will be issued while this function is currently
    *            outstanding.
    */
typedef void (BTPSAPI *SPP_Event_Callback_t)(unsigned int BluetoothStackID, SPP_Event_Data_t *SPP_Event_Data, unsigned long CallbackParameter);

   /*! @brief The following function is responsible for establishing a Serial
    * Port Server (will wait for a connection to occur on the port
    * established by this function). @param BluetoothStackID This function accepts as input the
    * Bluetooth Stack ID of the Bluetooth Protocol Stack that this
    * Serial Port is to be established with.
    * @param ServerPort The second parameter is
    * the Port Number to establish. This number *MUST* be between
    * SPP_PORT_NUMBER_MINIMUM and SPP_PORT_NUMBER_MAXIMUM. @param SPP_Event_Callback
    * SPP Event Callback function that will be called with SPP Events that
    * occur on the specified Serial Port. @param CallbackParameter Callback
    * Parameter. @return This function returns a
    * non-zero, positive, number on success or a negative return error
    * code if an error occurred (see BTERRORS.H).  A successful return
    * code will be a Serial Port ID that can be used to reference the
    * Opened Serial Port in ALL other functions in this module (except
    * the SPP_Open_Remote_Port() function).  Once a Server Serial Port
    * is opened, it can only be Un-Registered via a call to the
    * SPP_Close_Server_Port() function (passing the return value from
    * this function).  The SPP_Close_Port() function can be used to
    * Disconnect a Client from the Server Port (if one is connected, it
    * will NOT Un-Register the Server Port however.
    */
BTPSAPI_DECLARATION int BTPSAPI SPP_Open_Server_Port(unsigned int BluetoothStackID, unsigned int ServerPort, SPP_Event_Callback_t SPP_Event_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SPP_Open_Server_Port_t)(unsigned int BluetoothStackID, unsigned int ServerPort, SPP_Event_Callback_t SPP_Event_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is responsible for Un-Registering a Serial
    * Port Server (which was Registered by a successful call to the
    * SPP_Open_Server_Port() function). @param BluetoothStackID
    * This function accepts as input the Bluetooth Stack ID of
    * the Bluetooth Protocol Stack.
    * @param SerialPortID The second parameter is Serial Port.
    * @return This function returns zero if successful, or a negative return
    * error code if an error occurred (see BTERRORS.H).  Note that this
    * function does NOT delete any SDP Service Record Handles.
    */
BTPSAPI_DECLARATION int BTPSAPI SPP_Close_Server_Port(unsigned int BluetoothStackID, unsigned int SerialPortID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SPP_Close_Server_Port_t)(unsigned int BluetoothStackID, unsigned int SerialPortID);
#endif

   /*! @brief The following function is responsible for responding to requests
    * to connect to a Serial Port Server. @param BluetoothStackID This function accepts as
    * input the Bluetooth Stack ID of the Local Bluetooth Protocol
    * Stack.
    * @param SerialPortID The Serial Port ID (which *MUST* have been obtained by
    * calling the SPP_Open_Server_Port() function).
    * @param AcceptConnection The first parameter whether to accept the pending connection request.
    * @return This function returns zero if successful, or a negative return value if
    * there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI SPP_Open_Port_Request_Response(unsigned int BluetoothStackID, unsigned int SerialPortID, Boolean_t AcceptConnection);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SPP_Open_Port_Request_Response_t)(unsigned int BluetoothStackID, unsigned int SerialPortID, Boolean_t AcceptConnection);
#endif

   /*! @brief The following function is provided to allow a means to add a
    * Generic SDP Service Record to the SDP Database.
    * @param BluetoothStackID This function
    * takes as input the Bluetooth Stack ID of the Local Bluetooth
    * Protocol Stack. @param SerialPortID The Serial Port ID (which *MUST* have been
    * obtained by calling the SPP_Open_Server_Port() function.
    * @param SDPServiceRecord The third parameter (if specified) specifies any additional SDP
    * Information to add to the record.  @param ServiceName The fourth parameter specifies
    * the Service Name to associate with the SDP Record.
    * @param SDPServiceRecordHandle The final parameter is a pointer to a DWord_t which receives the SDP
    * Service Record Handle if this function successfully creates an
    * SDP Service Record. @return If this function returns zero, then the
    * SDPServiceRecordHandle entry will contain the Service Record
    * Handle of the added SDP Service Record.  If this function fails,
    * a negative return error code will be returned (see BTERRORS.H)
    * and the SDPServiceRecordHandle value will be undefined.
    * \note This function should only be called with the SerialPortID
    *          that was returned from the SPP_Open_Server_Port()
    *          function.  This function should NEVER be used with the
    *          Serial Port ID returned from the SPP_Open_Remote_Port()
    *          function.
    * \note The Service Record Handle that is returned from this
    *          function will remain in the SDP Record Database until
    *          it is deleted by calling the SDP_Delete_Service_Record()
    *          function.
    * \note A MACRO is provided to Delete the Service Record from
    *          the SDP Data Base.  This MACRO maps the
    *          SPP_Un_Register_SDP_Record() to
    *          SDP_Delete_Service_Record().
    * \note If NO UUID Information is specified in the
    *          SDPServiceRecord Parameter, then the default SPP Service
    *          Class's are added.  Any Protocol Information that is
    *          specified (if any) will be added in the Protocol
    *          Attribute AFTER the default SPP Protocol List (L2CAP
    *          and RFCOMM).
    * \note The Service Name is always added at Attribute ID 0x0100.
    *          A Language Base Attribute ID List is created that
    *          specifies that 0x0100 is UTF-8 Encoded, English Language.
    */
BTPSAPI_DECLARATION int BTPSAPI SPP_Register_SDP_Record(unsigned int BluetoothStackID, unsigned int SerialPortID, SPP_SDP_Service_Record_t *SDPServiceRecord, char *ServiceName, DWord_t *SDPServiceRecordHandle);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SPP_Register_SDP_Record_t)(unsigned int BluetoothStackID, unsigned int SerialPortID, SPP_SDP_Service_Record_t *SDPServiceRecord, char *ServiceName, DWord_t *SDPServiceRecordHandle);
#endif

   /*! The following MACRO is a utility MACRO that simply registers a
    * generic SPP SDP Service Record.  This MACRO simply maps to the
    * SPP_Register_Raw_SDP_Record() function.  This MACRO is only
    * provided so that the caller doesn't have to sift through the SDP
    * API for very simplistic applications.  This function accepts as
    * input the Bluetooth Stack ID of the Bluetooth Protocol Stack that
    * the Service Record exists on, the Serial Port ID (returned from a
    * successful call to the SPP_Open_Server_Port() function), the
    * Service Name and a pointer to return the SDP Service Record
    * Handle.  See the SPP_Register_Raw_SDP_Record() function for more
    * information.  This MACRO returns the result of the
    * SPP_Register_Raw_SDP_Record() function, which is zero for success
    * or a negative return error code (see BTERRORS.H).
    */
#define SPP_Register_Generic_SDP_Record(__BluetoothStackID, __SerialPortID, __ServiceName, __SDPServiceRecordHandle) \
   (SPP_Register_Raw_SDP_Record(__BluetoothStackID, __SerialPortID, NULL, __ServiceName, __SDPServiceRecordHandle))

   /*! @brief The following function is provided to allow a means to add a
    * Generic raw SDP Service Record to the SDP Database.
    * @param BluetoothStackID This function
    * takes as input the Bluetooth Stack ID of the Local Bluetooth
    * Protocol Stack. @param SerialPortID The Serial Port ID (which *MUST* have been
    * obtained by calling the SPP_Open_Server_Port() function.
    * @param SDPServiceRecord The third parameter (if specified) specifies any additional SDP
    * Information to add to the record.  @param ServiceName The fourth parameter specifies
    * the Service Name to associate with the SDP Record.
    * @param SDPServiceRecordHandle The final parameter
    * is a pointer to a DWord_t which receives the SDP Service
    * Record Handle if this function successfully creates an SDP Service
    * Record. @return If this function returns zero, then the
    * SDPServiceRecordHandle entry will contain the Service Record
    * Handle of the added SDP Service Record.  If this function fails, a
    * negative return error code will be returned (see BTERRORS.H) and
    * the SDPServiceRecordHandle value will be undefined.
    * \note This function should only be called with the SerialPortID
    *          that was returned from the SPP_Open_Server_Port()
    *          function.  This function should NEVER be used with the
    *          Serial Port ID returned from the SPP_Open_Remote_Port()
    *          function.
    * \note The Service Record Handle that is returned from this
    *          function will remain in the SDP Record Database until it
    *          is deleted by calling the SDP_Delete_Service_Record()
    *          function.
    * \note A MACRO is provided to Delete the Service Record from the
    *          SDP Data Base.  This MACRO maps the
    *          SPP_Un_Register_SDP_Record() to
    *          SDP_Delete_Service_Record().
    * \note If NO UUID Information is specified in the
    *          SDPServiceRecord Parameter, then the default SPP Service
    *          Class's are added.  Any Protocol Information that is
    *          specified (if any) will be added in the Protocol
    *          Attribute AFTER the default SPP Protocol List (L2CAP and
    *          RFCOMM).
    * \note The Service Name is always added at Attribute ID 0x0100.
    *          A Language Base Attribute ID List is created that
    *          specifies that 0x0100 is UTF-8 Encoded, English Language.
    */
BTPSAPI_DECLARATION int BTPSAPI SPP_Register_Raw_SDP_Record(unsigned int BluetoothStackID, unsigned int SerialPortID, SPP_SDP_Raw_Service_Record_t *SDPServiceRecord, char *ServiceName, DWord_t *SDPServiceRecordHandle);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SPP_Register_Raw_SDP_Record_t)(unsigned int BluetoothStackID, unsigned int SerialPortID, SPP_SDP_Raw_Service_Record_t *SDPServiceRecord, char *ServiceName, DWord_t *SDPServiceRecordHandle);
#endif

   /*! The following MACRO is a utility MACRO that simply deletes the
    * SPP SDP Service Record (specified by the third parameter) from
    * SDP Database.  This MACRO simply maps to the
    * SDP_Delete_Service_Record() function This MACRO is only
    * provided so that the caller doesn't have to sift through the SDP
    * API for very simplistic applications.  This function accepts as
    * input the Bluetooth Stack ID of the Bluetooth Protocol Stack that
    * the Service Record exists on, the Serial Port ID (returned from
    * a successful call to the SPP_Open_Server_Port() function), and the
    * SDP Service Record Handle.  The SDP Service Record Handle was
    * returned via a succesful call to the SPP_Register_SDP_Record()
    * function.  See the SPP_Register_SDP_Record() function for more
    * information.  This MACRO returns the result of the
    * SDP_Delete_Service_Record() function, which is zero for success
    * or a negative return error code (see BTERRORS.H).
    */
#define SPP_Un_Register_SDP_Record(__BluetoothStackID, __SerialPortID, __SDPRecordHandle) \
        (SDP_Delete_Service_Record(__BluetoothStackID, __SDPRecordHandle))

   /*! @brief The following function is responsible for Opening a Remote Serial
    * Port on the specified Remote Device. @param BluetoothStackID This function accepts the
    * Bluetooth Stack ID of the Bluetooth Stack.
    * @param BD_ADDR The second parameter
    * specifies the Board Address (NON NULL) of the Remote Bluetooth
    * Device to connect with. @param ServerPort The next parameter specifies the Remote
    * Server Channel ID to connect. @param SPP_Event_Callback
    * The SPP Event Callback function of the SPP Event Callback that is to process any
    * further interaction with the specified Remote Port (Opening
    * Status, Data Writes, etc).
    * @param CallbackParameter The SPP Event callback parameter.
    * @return This function returns a non-zero,
    * positive, value if successful, or a negative return error code if
    * this function is unsuccessful.  If this function is successful,
    * the return value will represent the Serial Port ID that can be
    * passed to all other functions that require it.  Once a Serial Port
    * is opened, it can only be closed via a call to the
    * SPP_Close_Port() function (passing the return value from this
    * function).
    */
BTPSAPI_DECLARATION int BTPSAPI SPP_Open_Remote_Port(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, unsigned int ServerPort, SPP_Event_Callback_t SPP_Event_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SPP_Open_Remote_Port_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, unsigned int ServerPort, SPP_Event_Callback_t SPP_Event_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function exists to close a Serial Port that was
    * previously opened with the SPP_Open_Server_Port() function OR the
    * SPP_Open_Remote_Port() function. @param BluetoothStackID This function accepts as input
    * the Bluetooth Stack ID of the Bluetooth Stack which the Open
    * Serial Port resides. @param SerialPortID The Serial Port ID (return value from one
    * of the above mentioned Open functions) of the Port to Close.
    * @return This function returns zero if successful, or a negative return value
    * if there was an error.  This function does NOT Un-Register a SPP
    * Server Port from the system, it ONLY disconnects any connection
    * that is currently active on the Server Port.  The
    * SPP_Close_Server_Port() function can be used to Un-Register the
    * SPP Server Port.
    */
BTPSAPI_DECLARATION int BTPSAPI SPP_Close_Port(unsigned int BluetoothStackID, unsigned int SerialPortID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SPP_Close_Port_t)(unsigned int BluetoothStackID, unsigned int SerialPortID);
#endif

   /*! @brief The following function is responsible for Reading Serial Data from
    * the specified Serial Connection.  The SerialPortID that is passed
    * to this function MUST have been established by either Accepting
    * a Serial Port Connection (callback from the SPP_Open_Server_Port()
    * function) or by initiating a Serial Port Connection (via calling
    * the SPP_Open_Remote_Port() function and having the remote side
    * accept the Connection). @param BluetoothStackID The input parameters to this function are
    * the Bluetooth Stack ID of the Bluetooth Stack that the second parameter
    * is valid for. @param SerialPortID Serial Port Identifier.
    * @param DataBufferSize The Size of the
    * Data Buffer to be used for reading. @param DataBuffer A pointer to the Data
    * Buffer. @return This function returns the number of data bytes that were
    * successfully read (zero if there were no Data Bytes ready to be
    * read), or a negative return error code if unsuccessful.
    */
BTPSAPI_DECLARATION int BTPSAPI SPP_Data_Read(unsigned int BluetoothStackID, unsigned int SerialPortID, Word_t DataBufferSize, Byte_t *DataBuffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SPP_Data_Read_t)(unsigned int BluetoothStackID, unsigned int SerialPortID, Word_t DataBufferSize, Byte_t *DataBuffer);
#endif

   /*! @brief The following function is responsible for Sending Serial Data to
    * the specified Serial Connection.  The SerialPortID that is passed
    * to this function MUST have been established by either Accepting
    * a Serial Port Connection (callback from the SPP_Open_Server_Port()
    * function) or by initiating a Serial Port Connection (via calling
    * the SPP_Open_Remote_Port() function and having the remote side
    * accept the Connection). @param BluetoothStackID The input parameters to this function are
    * the Bluetooth Stack ID of the Bluetooth Stack that the second parameter is valid for.
    * @param SerialPortID Serial Port Identifier.
    * @param DataLength the Length of the Data to send.
    * @param DataBuffer A pointer to the Data Buffer to Send.
    * @return This function returns the number of data bytes that were successfully
    * sent, or a negative return error code if unsuccessful.
    * \note If this function is unable to send all of the data that
    *          was specified (via the DataLength parameter), this
    *          function will return the number of bytes that were
    *          actually sent (zero or more, but less than the DataLength
    *          parameter value).  When this happens (and ONLY when
    *          this happens), the user can expect to be notified when
    *          the Serial Port is able to send data again via the
    *          etPort_Transmit_Buffer_Empty_Indication SPP Event.  This
    *          will allow the user a mechanism to know when the Transmit
    *          Buffer is empty so that more data can be sent.
    */
BTPSAPI_DECLARATION int BTPSAPI SPP_Data_Write(unsigned int BluetoothStackID, unsigned int SerialPortID, Word_t DataLength, Byte_t *DataBuffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SPP_Data_Write_t)(unsigned int BluetoothStackID, unsigned int SerialPortID, Word_t DataLength, Byte_t *DataBuffer);
#endif

   /*! @brief The following function is provided to allow the programmer a
    * means to change the default Transmit and Receive Buffer Sizes.
    * @param BluetoothStackID This function accepts as input the Bluetooth Stack ID of the
    * Bluetooth Protocol Stack that the specified Serial Port has been
    * previously opened (second parameter).
    * @param SerialPortID Serial Port Identifier.
    * @param ReceiveBufferSize The requested Buffer size to change the Receive buffer.
    * @param TransmitBufferSize The requested Buffer size to change the Transmit buffer.
    * \note The special constant
    * SPP_BUFFER_SIZE_CURRENT can be used to specify that the requested
    * Buffer Size (either Transmit and/or Receive) NOT be changed.
    * @return This function returns zero if the specified Buffer Size(s) were
    * changed, or a negative return error code if there was an error.
    * \note This function causes ALL Data in each Buffer to be
    *          lost.  This function clears the each Data Buffer so that
    *          all the available data buffer is available to be used.
    */
BTPSAPI_DECLARATION int BTPSAPI SPP_Change_Buffer_Size(unsigned int BluetoothStackID, unsigned int SerialPortID, unsigned int ReceiveBufferSize, unsigned int TransmitBufferSize);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SPP_Change_Buffer_Size_t)(unsigned int BluetoothStackID, unsigned int SerialPortID, unsigned int ReceiveBufferSize, unsigned int TransmitBufferSize);
#endif

   /*! @brief The following function exists to allow the user a mechanism for
    * either aborting ALL Data present in either an Input or an Output
    * Buffer, or a means to wait until a ALL Data present in either an
    * Input or Output buffer has been removed.
    * @param BluetoothStackID This function accepts as input the Bluetooth Stack
    * ID of the Bluetooth Protocol Stack that the specified Serial Port
    * has been opened on.
    * @param SerialPortID The Serial Port that was opened previously.
    * @param PurgeBufferMask The final parameter is a BIT MASK that represents
    * type of operation to perform. @return This function returns zero if
    * successful, or a negative return error code if unsuccessful.
    * \note When using a PurgeBufferMask of
    *            SPP_PURGE_MASK_TRANSMIT_FLUSH_BIT, if the SPP Transmit
    *            Buffer is already empty this function will return
    *            BTPS_ERROR_SPP_BUFFER_EMPTY.
    */
BTPSAPI_DECLARATION int BTPSAPI SPP_Purge_Buffer(unsigned int BluetoothStackID, unsigned int SerialPortID, unsigned int PurgeBufferMask);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SPP_Purge_Buffer_t)(unsigned int BluetoothStackID, unsigned int SerialPortID, unsigned int PurgeBufferMask);
#endif

   /*! @brief The following function is provided to allow the programmer a
    * means to notify the remote side of the Serial Connection of a
    * Break Condition.
    * @param BluetoothStackID This function accepts as input the Bluetooth Stack
    * ID of the Bluetooth Protocol Stack that the specified Serial Port
    * has been opened on.
    * @param SerialPortID The Serial Port that was opened previously.
    * @param BreakTimeout The first parameter (if specified specifies the length of time that the
    * Break was detected. @return This function returns zero if successful, or
    * a negative return value if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI SPP_Send_Break(unsigned int BluetoothStackID, unsigned int SerialPortID, unsigned int BreakTimeout);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SPP_Send_Break_t)(unsigned int BluetoothStackID, unsigned int SerialPortID, unsigned int BreakTimeout);
#endif

   /*! @brief The following function is provided to allow the programmer a
    * method to send the existing state of the Line Status to the
    * remote side.
    * @param BluetoothStackID This function accepts as input the Bluetooth Stack
    * ID of the Bluetooth Protocol Stack that the specified Serial Port
    * has been opened on.
    * @param SerialPortID The Serial Port that was opened previously.
    * @param SPPLineStatusMask The Current Line Status State.
    * @return This function returns zero if
    * successful, or a negative return value if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI SPP_Line_Status(unsigned int BluetoothStackID, unsigned int SerialPortID, unsigned int SPPLineStatusMask);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SPP_Line_Status_t)(unsigned int BluetoothStackID, unsigned int SerialPortID, unsigned int SPPLineStatusMask);
#endif

   /*! @brief The following function is provided to allow the programmer a
    * method to send the existing state of ALL Modem/Port Control
    * Signals to the remote side.
    * @param BluetoothStackID This function accepts as input the Bluetooth Stack
    * ID of the Bluetooth Protocol Stack that the specified Serial Port
    * has been opened on.
    * @param SerialPortID The Serial Port that was opened previously.
    * @param PortStatus The Current State of all the Modem Control
    * Signals. @return This function returns zero if successful, or a negative
    * return value if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI SPP_Port_Status(unsigned int BluetoothStackID, unsigned int SerialPortID, unsigned int PortStatus);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SPP_Port_Status_t)(unsigned int BluetoothStackID, unsigned int SerialPortID, unsigned int PortStatus);
#endif

   /*! @brief The following function is provided to allow the programmer a
    * method to inform the remote Side of the Serial Port Parameters
    * that are to be used.
    * @param BluetoothStackID This function accepts as input the Bluetooth Stack
    * ID of the Bluetooth Protocol Stack that the specified Serial Port
    * has been opened on.
    * @param SerialPortID The Serial Port that was opened previously.
    * @param SPPPortInformation The final parameter to this function is the
    * Requested Serial Port Information and cannot be NULL.  @return This function returns zero if successful, or a negative return value
    * if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI SPP_Send_Port_Information(unsigned int BluetoothStackID, unsigned int SerialPortID, SPP_Port_Information_t *SPPPortInformation);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SPP_Send_Port_Information_t)(unsigned int BluetoothStackID, unsigned int SerialPortID, SPP_Port_Information_t *SPPPortInformation);
#endif

   /*! @brief The following function is provided to allow the programmer a
    * method to respond to a Serial Port Parameters Indication from the
    * remote side.
    * @param BluetoothStackID This function accepts as input the Bluetooth Stack
    * ID of the Bluetooth Protocol Stack that the specified Serial Port
    * has been opened on.
    * @param SerialPortID The Serial Port that was opened previously.
    * @param SPPPortInformation The status fo the specified Port Information (acceptable or
    * unacceptable).  @return This function returns zero if successful, or a
    * negative return value if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI SPP_Respond_Port_Information(unsigned int BluetoothStackID, unsigned int SerialPortID, SPP_Port_Information_t *SPPPortInformation);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SPP_Respond_Port_Information_t)(unsigned int BluetoothStackID, unsigned int SerialPortID, SPP_Port_Information_t *SPPPortInformation);
#endif

   /*! @brief The following function is provided to allow the programmer a
    * method to query the existing Serial Port Parameters from the
    * remote side.
    * @param BluetoothStackID This function accepts as input the Bluetooth Stack
    * ID of the Bluetooth Protocol Stack that the specified Serial Port
    * has been opened on.
    * @param SerialPortID The Serial Port that was opened previously.
    * @return This function returns zero if successful, or a negative return
    * value if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI SPP_Query_Remote_Port_Information(unsigned int BluetoothStackID, unsigned int SerialPortID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SPP_Query_Remote_Port_Information_t)(unsigned int BluetoothStackID, unsigned int SerialPortID);
#endif

   /*! @brief The following function is provided to allow the programmer a
    * method to respond to a Serial Port Parameters Request from the
    * remote side.
    * @param BluetoothStackID This function accepts as input the Bluetooth Stack
    * ID of the Bluetooth Protocol Stack that the specified Serial Port
    * has been opened on.
    * @param SerialPortID The Serial Port that was opened previously.
    * @param SPPPortInformation The Current Local Port Information.
    * @return This function returns zero
    * if successful, or a negative return value if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI SPP_Respond_Query_Port_Information(unsigned int BluetoothStackID, unsigned int SerialPortID, SPP_Port_Information_t *SPPPortInformation);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SPP_Respond_Query_Port_Information_t)(unsigned int BluetoothStackID, unsigned int SerialPortID, SPP_Port_Information_t *SPPPortInformation);
#endif

   /*! @brief The following function is responsible for allowing a mechanism to
    * query the current SPP Configuration Parameters.  These parameters
    * are the parameters that control the RFCOMM Frame Size (and the
    * default Transmit and Receive Buffer sizes) that SPP will use when
    * opening/accepting SPP connections.  @param BluetoothStackID The first parameter to this
    * function is the Bluetooth Stack ID of the Bluetooth Stack which
    * this function is to query the SPP Configuration of.
    * @param SPPConfigurationParams The second parameter
    * is a pointer to structure that will receive the current
    * SPP Configuration Information that is in use.
    * @return This function returns zero if successful or a negative return error code if an
    * error occurs.
    * \note These parameters are set globally for the entire SPP
    *          entity (per Bluetooth Stack Instance).  These values
    *          can only be changed when NO SPP Clients or Servers are
    *          open.
    * \note The Transmit and Receive Buffer sizes *MUST* be AT LEAST
    *          the size of the Maximum Frame Size that is being set.
    */
BTPSAPI_DECLARATION int BTPSAPI SPP_Get_Configuration_Parameters(unsigned int BluetoothStackID, SPP_Configuration_Params_t *SPPConfigurationParams);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SPP_Get_Configuration_Parameters_t)(unsigned int BluetoothStackID, SPP_Configuration_Params_t *SPPConfigurationParams);
#endif

   /*! @brief The following function is responsible for allowing a mechanism
    * to change the default SPP Configuration Parameters.  These
    * parameters are the parameters that control the RFCOMM Frame Size
    * (and the default Transmit and Receive Buffer sizes) that SPP will
    * use when opening/accepting SPP connections.  @param BluetoothStackID The first parameter
    * to this function is the Bluetooth Stack ID of the Bluetooth Stack
    * which this function is to change the SPP Configuration of.
    * @param SPPConfigurationParams The second parameter is a
    * pointer to structure that contains the new
    * SPP Configuration Information to use. @return This function returns zero
    * if successful or a negative return error code if an error occurs.
    * \note These parameters are set globally for the entire SPP
    *          entity (per Bluetooth Stack Instance).  These values
    *          can only be changed when NO SPP Clients or Servers are
    *          open (i.e. this function can only be called when no
    *          SPP Clients or Servers are active).
    * \note The Transmit and Receive Buffer sizes *MUST* be AT LEAST
    *          the size of the Maximum Frame Size that is being set.
    */
BTPSAPI_DECLARATION int BTPSAPI SPP_Set_Configuration_Parameters(unsigned int BluetoothStackID, SPP_Configuration_Params_t *SPPConfigurationParams);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SPP_Set_Configuration_Parameters_t)(unsigned int BluetoothStackID, SPP_Configuration_Params_t *SPPConfigurationParams);
#endif

   /*! @brief The following function is responsible for allowing a mechanism to
    * query the SPP Server Connection Mode.
    * @param BluetoothStackID This function accepts as input the Bluetooth Stack
    * ID of the Bluetooth Protocol Stack that the specified Serial Port
    * has been opened on.
    * @param SerialPortID The Serial Port that was opened previously.
    * @param SPPServerConnectionMode The final parameter a pointer to
    * a Server Connection Mode variable which
    * will receive the current Server Connection Mode.
    * @return This function returns zero if successful, or a negative return value if there
    * was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI SPP_Get_Server_Connection_Mode(unsigned int BluetoothStackID, unsigned int SerialPortID, SPP_Server_Connection_Mode_t *SPPServerConnectionMode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SPP_Get_Server_Connection_Mode_t)(unsigned int BluetoothStackID, unsigned int SerialPortID, SPP_Server_Connection_Mode_t *SPPServerConnectionMode);
#endif

   /*! @brief The following function is responsible for allowing a mechanism to
    * change the current SPP Server Connection Mode.
    * @param BluetoothStackID This function accepts as input the Bluetooth Stack
    * ID of the Bluetooth Protocol Stack that the specified Serial Port
    * has been opened on.
    * @param SerialPortID The Serial Port that was opened previously.
    * @param SPPServerConnectionMode The first parameter the new Server Connection Mode to use.
    * @return This function returns zero if successful, or a negative return value if
    * there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI SPP_Set_Server_Connection_Mode(unsigned int BluetoothStackID, unsigned int SerialPortID, SPP_Server_Connection_Mode_t SPPServerConnectionMode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SPP_Set_Server_Connection_Mode_t)(unsigned int BluetoothStackID, unsigned int SerialPortID, SPP_Server_Connection_Mode_t SPPServerConnectionMode);
#endif

   /*! @brief The following function is a utility function that exists to
    * determine the current SPP (RFCOMM) Connection state for a specific
    * SPP/RFCOMM Connection. @param BluetoothStackID This function accepts as input, the
    * Bluetooth Stack ID of the Bluetooth Stack for which the request is
    * valid for.
    * @param BD_ADDR The second parameter is the Bluetooth Device Address of the
    * connection in question (required).
    * @param ServerPort The third parameter is the SPP/RFCOMM
    * Server Port Number of the connection in question.
    * @param LocalPort The local port flag.
    * @param SPP_Port_Connection_State The final parameter is a
    * flag that specifies whether or not the connection is to a Local
    * SPP Server (TRUE) or a Remote SPP Server (FALSE), followed by a
    * pointer to state variable that is to receive the state
    * information.  @return This function returns zero if successful, or a
    * negative return error code if there was an error.
    * \note This function will only supply a value in the SPP
    *          Port Connection State parameter if the return value from
    *          this function is success (0).  If this function returns
    *          an error, then the contents of this variable will be
    *          undefined.
    * \note The Bluetooth Address is a required parameter and is
    *          used to determine the TEI of the underlying RFCOMM
    *          Connection.
    * \note The ServerPort parameter must be one of the following:
    *             - 0 (queries if a connection to the remote device
    *               specifies is possible)
    *             - SPP_PORT_NUMBER_MINIMUM
    *             - SPP_PORT_NUMBER_MAXIMUM
    *          Note that the above values are NOT the SPP Port ID
    *          values returned from the SPP_Open_Server_Port() or the
    *          SPP_Open_Remote_Port() functions, but rather are the
    *          actual SPP/RFCOMM Port Channel Numbers !
    * \note This is a very low level function and exists solely to
    *          allow a mechanism to determine the current Control
    *          Message State for the underlying RFCOMM Transport.  This
    *          is needed in some circumstances when trying to connect or
    *          disconnect SPP Ports and the user has no way of knowning
    *          knowing the current Status (keep in mind that there can
    *          only be a single Control Message outstanding on any
    *          given RFCOMM Channel).
    */
BTPSAPI_DECLARATION int BTPSAPI SPP_Get_Port_Connection_State(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, unsigned int ServerPort, Boolean_t LocalPort, SPP_Port_Connection_State_t *SPP_Port_Connection_State);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SPP_Get_Port_Connection_State_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, unsigned int ServerPort, Boolean_t LocalPort, SPP_Port_Connection_State_t *SPP_Port_Connection_State);
#endif

   /*! @brief The following function is responsible for setting the lower level
    * queuing parameters.  These parameters are used to control aspects
    * of the amount of data packets that can be queued into the lower
    * level (per individual channel).  This mechanism allows for the
    * flexibility to limit the amount of RAM that is used for streaming
    * type applications (where the remote side has a large number of
    * credits that were granted). @param BluetoothStackID This function accepts as input the
    * Bluetooth Stack ID of the Bluetooth stack in which to set the
    * system wide queuing parameters. @param MaximumNumberDataPackets The second
    * parameter is the maximum number of
    * queued data packets (per DLCI). @param QueuedDataPacketsThreshold The final
    * parameter is the low threshold
    * (used be the lower layer to inform RFCOMM when it can send another
    * data packet).  @return This function returns zero if successful or a
    * negative return error code if there is an error.
    * \note This function can only be called when there are NO active
    *          connections.
    * \note Setting both parameters to zero will disable the queuing
    *          mechanism.  This means that the amount of queued packets
    *          will only be limited via the amount of available RAM.
    * \note These parameters do not affect the transmit and receive
    *          buffers and do not affect any frame sizes and/or credit
    *          logic.  These parameters ONLY affect the number of
    *          data packets queued into the lower level.
    */
BTPSAPI_DECLARATION int BTPSAPI SPP_Set_Queuing_Parameters(unsigned int BluetoothStackID, unsigned int MaximumNumberDataPackets, unsigned int QueuedDataPacketsThreshold);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SPP_Set_Queuing_Parameters_t)(unsigned int BluetoothStackID, unsigned int MaximumNumberDataPackets, unsigned int QueuedDataPacketsThreshold);
#endif

   /*! @brief The following function is responsible for setting the lower level
    * queuing parameters.  These parameters are used to control aspects
    * of the amount of data packets that can be queued into the lower
    * level (per individual channel).  This mechanism allows for the
    * flexibility to limit the amount of RAM that is used for streaming
    * type applications (where the remote side has a large number of
    * credits that were granted). @param BluetoothStackID This function accepts as input the
    * Bluetooth Stack ID of the Bluetooth stack in which to get the
    * lower level queuing parameters. @param MaximumNumberDataPackets A pointer to a
    * variable that is to receive the maximum number of queued data
    * packets (per channel). @param QueuedDataPacketsThreshold The low threshold (used be the
    * lowest layer to inform the lower layer when it can send another
    * data packet).  @return This function returns zero if successful or a
    * negative return error code if there is an error.
    */
BTPSAPI_DECLARATION int BTPSAPI SPP_Get_Queuing_Parameters(unsigned int BluetoothStackID, unsigned int *MaximumNumberDataPackets, unsigned int *QueuedDataPacketsThreshold);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SPP_Get_Queuing_Parameters_t)(unsigned int BluetoothStackID, unsigned int *MaximumNumberDataPackets, unsigned int *QueuedDataPacketsThreshold);
#endif

   /*! @brief The following function is provided to allow a mechanism to
    * determine if a SPP Server Port is open on the specified RFCOMM
    * Server Port. This allows application developers a means to
    * determine if there are free/active SPP servers on specific RFCOMM
    * ports. @param BluetoothStackID This function accepts the Bluetooth Stack ID.
    * @param ServerPort The SPP Server Port (RFCOMM Channel) number to query.
    * @param ServerPresent The final parameters is a pointer to a variable that will hold the value (on
    * success) for the server being present.  This value will be BOOLEAN
    * TRUE if a server is registered with the system, or FALSE if there
    * is not a server registered with the system.  @return This function returns
    * zero if successful, or a negative return error code if there was
    * an error.
    * \note The ServerPort parameter must be one of the following:
    *             - SPP_PORT_NUMBER_MINIMUM
    *             - SPP_PORT_NUMBER_MAXIMUM
    *          Note that the above values are NOT the SPP Port ID
    *          values returned from the SPP_Open_Server_Port() function,
    *          but rather are the actual SPP/RFCOMM Port Channel
    *          Numbers!
    */
BTPSAPI_DECLARATION int BTPSAPI SPP_Query_Server_Present(unsigned int BluetoothStackID, unsigned int ServerPort, Boolean_t *ServerPresent);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SPP_Query_Server_Present_t)(unsigned int BluetoothStackID, unsigned int ServerPort, Boolean_t *ServerPresent);
#endif

#endif
