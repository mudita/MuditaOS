/*****< btvsapi.h >************************************************************/
/*      Copyright 2011 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  BTVSAPI - Vendor specific functions/definitions/constants used to define  */
/*            a set of vendor specific functions supported for a specific     */
/*            hardware platform.                                              */
/*                                                                            */
/*  Author:  Damon Lange                                                      */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   05/20/11  D. Lange       Initial creation.                               */
/******************************************************************************/
#ifndef __BTVSAPIH__
#define __BTVSAPIH__

#include "SS1BTPS.h"            /* Bluetopia API Prototypes/Constants.        */

   /* The following enumerated type represents the different modulation */
   /* types that may be specified in VS_Enable_FCC_Test_Mode.           */
typedef enum
{
   mtContinuousWave = 0x00,
   mtGFSK           = 0x01,
   mtEDR2           = 0x02,
   mtEDR3           = 0x03
} VS_Modulation_Type_t;

   /* The following types represent the different Test Patterns that may*/
   /* be specified in VS_Enable_FCC_Test_Mode.                          */
#define VS_TEST_PATTERN_PN9                              0x00
#define VS_TEST_PATTERN_PN15                             0x01
#define VS_TEST_PATTERN_Z0Z0                             0x02
#define VS_TEST_PATTERN_ALL_ONE                          0x03
#define VS_TEST_PATTERN_ALL_ZERO                         0x04
#define VS_TEST_PATTERN_F0F0                             0x05
#define VS_TEST_PATTERN_FF00                             0x06
#define VS_TEST_PATTERN_USER_DEFINED                     0x07

   /* The following types represent the Minimum and Maxium BT Frequency */
   /* Channel that may be specified in VS_Enable_FCC_Test_Mode.         */
#define VS_MINIMUM_BT_FREQUENCY_CHANNEL                  0
#define VS_MAXIMUM_BT_FREQUENCY_CHANNEL                  78

   /* The following types represent the Minimum and Maximum Power Levels*/
   /* that may be specified in VS_Enable_FCC_Test_Mode.                 */
#define VS_MINIMUM_POWER_LEVEL                           0
#define VS_MAXIMUM_POWER_LEVEL                           15

   /* The following define the value A3DP Roles to the VS_AVPR_Enable() */
   /* function.                                                         */
#define AVRP_A3DP_SOURCE_ROLE                            0
#define AVRP_A3DP_SINK_ROLE                              1

   /* The following defines the list of valid values for the sampling   */
   /* frequencies, channel mode, and alloc method for the codec config  */
   /* parameters used in VS_A3DP_CODEC_Configure().                     */
#define AVRP_SOURCE_PCM                                  0x00
#define AVRP_SOURCE_FM                                   0x01

#define AVRP_AUDIO_FORMAT_PCM_SAMPLE_RATE_MASK           0x0F
#define AVRP_AUDIO_FORMAT_PCM_SAMPLE_RATE_8K             0x01
#define AVRP_AUDIO_FORMAT_PCM_SAMPLE_RATE_11K            0x02
#define AVRP_AUDIO_FORMAT_PCM_SAMPLE_RATE_12K            0x03
#define AVRP_AUDIO_FORMAT_PCM_SAMPLE_RATE_16K            0x04
#define AVRP_AUDIO_FORMAT_PCM_SAMPLE_RATE_22K            0x05
#define AVRP_AUDIO_FORMAT_PCM_SAMPLE_RATE_24K            0x06
#define AVRP_AUDIO_FORMAT_PCM_SAMPLE_RATE_32K            0x07
#define AVRP_AUDIO_FORMAT_PCM_SAMPLE_RATE_44K1           0x08
#define AVRP_AUDIO_FORMAT_PCM_SAMPLE_RATE_48K            0x09

#define AVRP_AUDIO_FORMAT_SBC_SAMPLE_RATE_MASK           0x30
#define AVRP_AUDIO_FORMAT_SBC_SAMPLE_RATE_16K            0x00
#define AVRP_AUDIO_FORMAT_SBC_SAMPLE_RATE_32K            0x10
#define AVRP_AUDIO_FORMAT_SBC_SAMPLE_RATE_44K1           0x20
#define AVRP_AUDIO_FORMAT_SBC_SAMPLE_RATE_48K            0x30

#define AVRP_AUDIO_FORMAT_SBC_MODE_MASK                  0xC0
#define AVRP_AUDIO_FORMAT_SBC_MODE_MONO                  0x00
#define AVRP_AUDIO_FORMAT_SBC_MODE_DUAL_CHANNEL          0x40
#define AVRP_AUDIO_FORMAT_SBC_MODE_STEREO                0x80
#define AVRP_AUDIO_FORMAT_SBC_MODE_JOINT_STEREO          0xC0

#define AVRP_SBC_FORMAT_BLOCK_LENGTH_MASK                0x03
#define AVRP_SBC_FORMAT_BLOCK_LENGTH_4                   0x00
#define AVRP_SBC_FORMAT_BLOCK_LENGTH_8                   0x01
#define AVRP_SBC_FORMAT_BLOCK_LENGTH_12                  0x02
#define AVRP_SBC_FORMAT_BLOCK_LENGTH_16                  0x03

#define AVRP_SBC_FORMAT_ALLOCATION_METHOD_MASK           0x04
#define AVRP_SBC_FORMAT_ALLOCATION_METHOD_LOUDNESS       0x00
#define AVRP_SBC_FORMAT_ALLOCATION_METHOD_SNL            0x04

#define STOP_STREAM_FLAG_FLUSH_DATA                      0x01
#define STOP_STREAM_FLAG_GENERATE_STOP_EVENT             0x02

   /* The following function prototype represents the vendor specific   */
   /* function which is used to change the Bluetooth UART for the Local */
   /* Bluetooth Device specified by the Bluetooth Protocol Stack that   */
   /* is specified by the Bluetooth Protocol Stack ID. The second       */
   /* parameter specifies the new baud rate to set.  This change        */
   /* encompasses both changing the speed of the Bluetooth chip (by     */
   /* issuing the correct commands) and then, if successful, informing  */
   /* the HCI Driver of the change (so the driver can communicate with  */
   /* the Bluetooth device at the new baud rate).  This function returns*/
   /* zero if successful or a negative return error code if there was   */
   /* an error.                                                         */
BTPSAPI_DECLARATION int BTPSAPI VS_Update_UART_Baud_Rate(unsigned int BluetoothStackID, DWord_t BaudRate);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_VS_Update_UART_Baud_Rate_t)(unsigned int BluetoothStackID, DWord_t BaudRate);
#endif

   /* The following function prototype represents the vendor specific   */
   /* function which is used to change the HCILL parameters that are    */
   /* used for the HCILL Low Power protocol for the Local Bluetooth     */
   /* Device specified by the Bluetooth Protocol Stack that is specified*/
   /* by the Bluetooth Protocol Stack ID.  The second is the            */
   /* InactivityTimeout on the Uart in ms.  If no traffic on Uart lines */
   /* after this time the Controller sends a Sleep Indication.  The     */
   /* third is the RetransmitTimeout (ms) for the Sleep Indication if no*/
   /* Sleep Acknowledgement (from the Host) is received.  The fourth is */
   /* the Controller RTS pulse width during Controller wakeup (specified*/
   /* in us).  This function returns zero if successful or a negative   */
   /* return error code if there was an error.                          */
BTPSAPI_DECLARATION int BTPSAPI VS_Update_HCILL_Parameters(unsigned int BluetoothStackID, Word_t InactivityTimeout, Word_t RetransmitTimeout, Byte_t CTSPulseWidth);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_VS_Update_HCILL_Parameters_t)(unsigned int BluetoothStackID, Word_t InactivityTimeout, Word_t RetransmitTimeout, Byte_t CTSPulseWidth);
#endif

   /* The following function prototype represents the vendor specific   */
   /* function which is used to enable/disable the HCILL low power      */
   /* protocol for the Local Bluetooth Device specified by the Bluetooth*/
   /* Protocol Stack that is specified by the Bluetooth Protocol Stack  */
   /* ID.  The second parameter specifies whether to enable (TRUE) or   */
   /* disable (FALSE) the HCILL low power protocol.  This function      */
   /* returns zero if successful or a negative return error code if     */
   /* there was an error.                                               */
BTPSAPI_DECLARATION int BTPSAPI VS_Enable_Sleep_Mode(unsigned int BluetoothStackID, Boolean_t Enable);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_VS_Enable_Sleep_Mode_t)(unsigned int BluetoothStackID, Boolean_t Enable);
#endif

   /* The following function prototype represents the vendor specific   */
   /* function which is used to change the public Bluetooth Address     */
   /* (also known as a MAC address) for the local Bluetooth Device      */
   /* specified by the Bluetooth Protocol Stack ID.  The second         */
   /* parameter contains the Bluetooth Address to set.  This function   */
   /* returns zero if successful or a negative return error code if     */
   /* there was an error.                                               */
BTPSAPI_DECLARATION int BTPSAPI VS_Set_Bluetooth_Address(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_VS_Set_Bluetooth_Address_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR);
#endif

   /* The following function prototype represents the vendor specific   */
   /* function which is used to change the output power for the Local   */
   /* Bluetooth Device specified by the Bluetooth Protocol Stack that   */
   /* is specified by the Bluetooth Protocol Stack ID. The second       */
   /* parameter is the max output power to set. This function returns   */
   /* zero if successful or a negative return error code if there was   */
   /* an error.                                                         */
   /* * NOTE * The maximum output power is specified from 0 to 12 and   */
   /*          it specifies 4 dBm steps.                                */
BTPSAPI_DECLARATION int BTPSAPI VS_Set_Max_Output_Power(unsigned int BluetoothStackID, Byte_t MaxPower);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_VS_Set_Max_Output_Power_t)(unsigned int BluetoothStackID, Byte_t MaxPower);
#endif

   /* The following function prototype represents the function which is */
   /* used to put the Local Bluetooth Device specified by the Bluetooth */
   /* Protocol Stack that is specified by the Bluetooth Protocol Stack  */
   /* ID into RF Signal Test Mode.  This function returns zero if       */
   /* successful or a negative return error code if there was an error. */
   /* * NOTE * Once the Local Bluetooth Device is in RF Signal Test Mode*/
   /*          it will remain in this mode until an HCI Reset is issued.*/
BTPSAPI_DECLARATION int BTPSAPI VS_Enable_RF_SIG_Test_Mode(unsigned int BluetoothStackID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_VS_Enable_RF_SIG_Test_Mode_t)(unsigned int BluetoothStackID);
#endif

   /* The following function prototype represents the vendor function   */
   /* which is used to put the specified by the Bluetooth Protocol Stack*/
   /* that is specified by the Bluetooth Protocol Stack ID into FCC Test*/
   /* Mode.  The second parameter specifies the Modulation Type to use  */
   /* in the test mode, the third parameter specifies the Test Pattern  */
   /* to transmit in the test mode, the fourth parameter specifies the  */
   /* Frequency Channel to trasmit on, and the fifth parameter specifies*/
   /* the Power Level to use while transmitting.  The final two         */
   /* parameters are only used when the Test_Pattern parameter is set to*/
   /* VS_TEST_PATTERN_USER_DEFINED.  This function returns zero if      */
   /* successful or a negative return error code if there was an error. */
BTPSAPI_DECLARATION int BTPSAPI VS_Enable_FCC_Test_Mode(unsigned int BluetoothStackID, VS_Modulation_Type_t Modulation_Type, Byte_t Test_Pattern, Byte_t Frequency_Channel, Byte_t Power_Level, DWord_t Generator_Init_Value, DWord_t EDR_Generator_Mask);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_VS_Enable_FCC_Test_Mode_t)(unsigned int BluetoothStackID, VS_Modulation_Type_t Modulation_Type, Byte_t Test_Pattern, Byte_t Frequency_Channel, Byte_t Power_Level, DWord_t Generator_Init_Value, DWord_t EDR_Generator_Mask);
#endif

   /* The following function is used to setup to use BLE.			    */
   /* In order to start the BLE the command must be sent to the 		*/
   /* Bluetooth chip.													*/
   /* This function returns zero if successful or a negative return     */
   /* error code if there was an error.                                 */
BTPSAPI_DECLARATION int BTPSAPI VS_EnableBLE(unsigned int BluetoothStackID);

   /* The following function sends the VS DDIP command in order to      */
   /* change ACL priority over scans. (Data During Inquiry Page)        */
   /* Send the command when A2DP is streaming for better quality        */
   /* The two parameters are the Best-effort precentage and the         */
   /* Guarenteed percentage when doing scans with ACL trafic            */
   /* Inputs: 1- The BluetoothStackID.                                  */
   /* 2- Best-effort percentage. 3- Guaranteed percentage.              */
   /* This function returns zero if successful or a negative return     */
   /* error code if there was an error.                                 */
BTPSAPI_DECLARATION void BTPSAPI VS_Send_DDIP(unsigned int BluetoothStackID, unsigned int bestEffortPer, unsigned int GuaranteedPer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_VS_EnableBLE_t)(unsigned int BluetoothStackID);
#endif

   /* The following function is used to setup to disable the use of BLE.*/
   /* In order to stop the BLE the command must be sent to the 	    	*/
   /* Bluetooth chip.													*/
   /* This function returns zero if successful or a negative return     */
   /* error code if there was an error.                                 */
   /* Row data:															*/
   /* 0x01, 0x5B, 0xFD, 0x02, 0x00, 0x00  								*/
BTPSAPI_DECLARATION int BTPSAPI VS_DisableBLE(unsigned int BluetoothStackID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_VS_DisableBLE_t)(unsigned int BluetoothStackID);
#endif

   /* The following function is used to setup to use Wide Band Speech.  */
   /* To Setup for WBS, the Codec settings must be changed to support   */
   /* 16KHz samples.  The Voice settings need to be set to transparent  */
   /* and the WBS_Associate command must be sent to the Bluetooth chip. */
   /* This function returns zero if successful or a negative return     */
   /* error code if there was an error.                                 */
BTPSAPI_DECLARATION int BTPSAPI VS_EnableWBS(unsigned int BluetoothStackID, Word_t ConnectionHandle);
   
#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_VS_EnableWBS_t)(unsigned int BluetoothStackID, Word_t ConnectionHandle);
#endif

	/* Enable the WBS without configur the CC256xx PCM interface, the CODEC setting */
	/* Send Write_Voice_Setting and WBS associate                                   */
BTPSAPI_DECLARATION int BTPSAPI VS_EnableWBS_No_PCM(unsigned int BluetoothStackID, Word_t ConnectionHandle);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
	typedef int (BTPSAPI *PFN_VS_EnableWBS_No_PCM_t)(unsigned int BluetoothStackID, Word_t ConnectionHandle);
#endif


   /* The following function is used to disable the use Wide Band       */
   /* Speech.  To revert from WBS, the Codec settings must be changed to*/
   /* support 8KHz samples.  The Voice settings need to be set to CVSD  */
   /* and the WBS_Disassociate command must be sent to the Bluetooth    */
   /* chip.  This function returns zero if successful or a negative     */
   /* return error code if there was an error.                          */
BTPSAPI_DECLARATION int BTPSAPI VS_DisableWBS(unsigned int BluetoothStackID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_VS_DisableWBS_t)(unsigned int BluetoothStackID);
#endif

   /* The following function is used to write the codec interface       */
   /* parameters and the PCM and Frame Sync frequencies.  This function */
   /* returns zero if successful or a negative return error code if     */
   /* there was an error.                                               */
   /* * NOTE * PCMFreq is in kHz, whereas FSyncFreq is in Hz.           */
BTPSAPI_DECLARATION int BTPSAPI VS_PCM_Codec_Config(unsigned int BluetoothStackID, Word_t PCMFreq, DWord_t FSyncFreq);
	
#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_VS_PCM_Codec_Config_t)(unsigned int BluetoothStackID, Word_t PCMFreq, DWord_t FSyncFreq);
#endif

/* Configure the CC256x PCM interface to be the slave on the I2S lines for Audio on the STM3240G-EVAL board */
BTPSAPI_DECLARATION int BTPSAPI VS_PCM_Codec_Config_Slave_I2S(unsigned int BluetoothStackID, Word_t PCMFreq, DWord_t FSyncFreq);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
	 typedef int (BTPSAPI *PFN_VS_PCM_Codec_Config_Slave_I2S_t)(unsigned int BluetoothStackID, Word_t PCMFreq, DWord_t FSyncFreq);
#endif

  /* The following function is used to start/stop the CC256x PCM interface */
  /* in loopback mode, The input "int enable" is TRUE for loopback on.	   */
BTPSAPI_DECLARATION int BTPSAPI VS_Set_Pcm_Loopback(unsigned int BluetoothStackID, int enable);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
	  typedef int (BTPSAPI *PFN_VS_Set_Pcm_Loopback_t)(unsigned int BluetoothStackID, int enable);
#endif


   /* The following function is used to enable or disable the AVPR      */
   /* features.  This function returns zero if successful or a negative */
   /* return error code if there was an error.                          */
BTPSAPI_DECLARATION int BTPSAPI VS_AVPR_Enable(unsigned int BluetoothStackID, Boolean_t AVPREnable, Boolean_t LoadCode, Byte_t A3DPRole);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_VS_AVPR_Enable_t)(unsigned int BluetoothStackID, Boolean_t AVPREnable, Boolean_t LoadCode, Byte_t A3DPRole);
#endif

   /* The following function is called when an A2DP connection is in the*/
   /* open state to tell the controller the L2CAP parameters of the     */
   /* channel.  This function returns zero if successful or a negative  */
   /* return error code if there was an error.                          */
BTPSAPI_DECLARATION int BTPSAPI VS_A3DP_Open_Stream(unsigned int BluetoothStackID, Byte_t Connection_Handle, Word_t CID, Word_t MTU);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_VS_A3DP_Open_Stream_t)(unsigned int BluetoothStackID, Byte_t Connection_Handle, Word_t CID, Word_t MTU);
#endif

   /* The following function is used to inform the controller when A2DP */
   /* connection closes.  This function returns zero if successful or a */
   /* negative return error code if there was an error.                 */
BTPSAPI_DECLARATION int BTPSAPI VS_A3DP_Close_Stream(unsigned int BluetoothStackID, Byte_t Connection_Handle);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_VS_A3DP_Close_Stream_t)(unsigned int BluetoothStackID, Byte_t Connection_Handle);
#endif

   /* The following function is used to configure the SBC Encoder or    */
   /* Decoder parameters.  This function returns zero if successful or a*/
   /* negative return error code if there was an error.                 */
BTPSAPI_DECLARATION int BTPSAPI VS_A3DP_Codec_Configuration(unsigned int BluetoothStackID, Byte_t AudioFormat, Byte_t SBCFormat, Byte_t BitPoolSize);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_VS_A3DP_Codec_Configuration_t)(unsigned int BluetoothStackID, Byte_t AudioFormat, Byte_t SBCFormat, Byte_t BitPoolSize);
#endif

   /* The following function is used to start assisted A2DP streaming.  */
   /* This function returns zero if successful or a negative return     */
   /* error code if there was an error.                                 */
BTPSAPI_DECLARATION int BTPSAPI VS_A3DP_Start_Stream(unsigned int BluetoothStackID, Byte_t Connection_Handle);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_VS_A3DP_Start_Stream_t)(unsigned int BluetoothStackID, Byte_t Connection_Handle);
#endif

   /* The following function is used to stop assisted A2DP streaming.   */
   /* This function returns zero if successful or a negative return     */
   /* error code if there was an error.                                 */
BTPSAPI_DECLARATION int BTPSAPI VS_A3DP_Stop_Stream(unsigned int BluetoothStackID, Byte_t Connection_Handle, Byte_t Flags);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_VS_A3DP_Stop_Stream_t)(unsigned int BluetoothStackID, Byte_t Connection_Handle, Byte_t Flags);
#endif

   /* The following function is used to open a stream as an A2DP SNK    */
   /* device. This function returns zero if successful or a negative    */
   /* return error code if there was an error.                          */
BTPSAPI_DECLARATION int BTPSAPI VS_A3DP_Sink_Open_Stream(unsigned int BluetoothStackID, Byte_t Connection_Handle, Word_t CID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_VS_A3DP_Sink_Open_Stream_t)(unsigned int BluetoothStackID, Byte_t Connection_Handle, Word_t CID);
#endif

   /* The following function is used to close a SNK stream previously   */
   /* opened using VS_A3DP_Sink_Open_Stream. This function returns zero */
   /* if successful or a negative return error code if there was an     */
   /* error.                                                            */
BTPSAPI_DECLARATION int BTPSAPI VS_A3DP_Sink_Close_Stream(unsigned int BluetoothStackID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_VS_A3DP_Sink_Close_Stream_t)(unsigned int BluetoothStackID);
#endif

   /* The following function is used to configure an A3DP device as an  */
   /* A2DP SNK, giving it PCM and SBC parameters. This configuration    */
   /* should be performed after VS_PCM_CodecConfig and VS_AVPR_Enable.  */
   /* This function returns zero if successful or a negative return     */
   /* error code if there was an error.                                 */
BTPSAPI_DECLARATION int BTPSAPI VS_A3DP_Sink_Codec_Configuration(unsigned int BluetoothStackID, Byte_t AudioFormat, Byte_t SBCFormat);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_VS_A3DP_Sink_Codec_Configuration_t)(unsigned int BluetoothStackID, Byte_t AudioFormat, Byte_t SBCFormat);
#endif

   /* The following function is used to change a stream previously      */
   /* opened via VS_A3DP_Sink_Open_Stream to the "Playing" state. This  */
   /* function returns zero if successful or a negative return error    */
   /* code if there was an error.                                       */
BTPSAPI_DECLARATION int BTPSAPI VS_A3DP_Sink_Start_Stream(unsigned int BluetoothStackID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_VS_A3DP_Sink_Start_Stream_t)(unsigned int BluetoothStackID);
#endif

   /* The following function is used to change a stream perviously      */
   /* opened via VS_A3DP_Sink_Open_stream to the "Stopped" state. This  */
   /* function returns zero if successful or a negative return error    */
   /* code if there was ane error.                                      */
BTPSAPI_DECLARATION int BTPSAPI VS_A3DP_Sink_Stop_Stream(unsigned int BluetoothStackID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_VS_A3DP_Sink_Stop_Stream_t)(unsigned int BluetoothStackID);
#endif

BTPSAPI_DECLARATION int BTPSAPI VS_A3DP_Sink_Start_Audio(unsigned int BluetoothStackID, Byte_t AudioFormat, Byte_t SBCFormat, 
                                     Byte_t Connection_Handle, Byte_t SPK_Connection_Handle, Word_t CID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_VS_A3DP_Sink_Start_Audio_t)(unsigned int BluetoothStackID, Byte_t AudioFormat, Byte_t SBCFormat, 
                                     Byte_t Connection_Handle, Byte_t SPK_Connection_Handle, Word_t CID);
#endif

   /* The following function is used to enable debug messages from the  */
   /* AVPR coprocessor used in A3DP and WBS. These messages are sent on */
   /* the existing TX Debug line for the CC256X radio.                  */
   /* * NOTE * Once enabled, the radio must be reset to disable the     */
   /*          debug messages.                                          */
BTPSAPI_DECLARATION int BTPSAPI VS_AVPR_Debug(unsigned int BluetoothStackID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_VS_AVPR_Debug_t)(unsigned int BluetoothStackID);
#endif

   /* The following function is used to enable multi-room support for   */
   /* the CC2564B A3DP feature.                                         */
   /* * NOTE * This feature will ONLY work specifically with the A3DP   */
   /*          Multiroom SNK demo!                                      */
BTPSAPI_DECLARATION int BTPSAPI VS_A3DP_Multi_Room_Mode(unsigned int BluetoothStackID, Byte_t Enable, Byte_t Connection_Handle);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_VS_A3DP_Multi_Room_Mode_t)(unsigned int BluetoothStackID, Byte_t Enable, Byte_t Connection_Handle);
#endif


#endif
