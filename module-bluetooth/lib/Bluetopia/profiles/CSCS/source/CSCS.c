/*****< cscs.c >***************************************************************/
/*																			  */
/* Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/ 	  */
/* All Rights Reserved.  													  */
/*																			  */
/*  CSCS - Bluetooth Stack Cycle Speed and Cadence Service Service (GATT      */
/*         Based).											                  */
/*																			  */
/*  Author:  Dan Horowitz                                                     */
/*																			  */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   11/30/14  D. Horowitz    Initial creation.                               */
/******************************************************************************/
#include "SS1BTPS.h"        /* Bluetooth Stack API Prototypes/Constants.      */
#include "SS1BTGAT.h"       /* Bluetooth Stack GATT API Prototypes/Constants. */
#include "SS1BTCSCS.h"      /* Bluetooth CSCS API Prototypes/Constants.       */

#include "BTPSKRNL.h"       /* BTPS Kernel Prototypes/Constants.              */
#include "CSCS.h"           /* Bluetooth CSCS Prototypes/Constants.           */

   /* The following controls the number of supported CSCS instances.     */
#define CSCS_MAXIMUM_SUPPORTED_INSTANCES                    (BTPS_CONFIGURATION_CSCS_MAXIMUM_SUPPORTED_INSTANCES)

   /* The following defines the CSCS Instance Data, that contains data   */
   /* is unique for each CSCS Service Instance.                          */
typedef __PACKED_STRUCT_BEGIN__ struct _tagCSCS_Instance_Data_t
{

   NonAlignedWord_t CSC_Features_Length;
   NonAlignedWord_t CSC_Features;

   NonAlignedWord_t Sensor_Location_Length;
   NonAlignedByte_t Sensor_Location;

} __PACKED_STRUCT_END__ CSCS_Instance_Data_t;

#define CSCS_INSTANCE_DATA_SIZE                             (sizeof(CSCS_Instance_Data_t))

#define CSCS_CSC_FEATURE_INSTANCE_TAG                       (BTPS_STRUCTURE_OFFSET(CSCS_Instance_Data_t, CSC_Features_Length))

#define CSCS_SENSOR_LOCATION_INSTANCE_TAG                   (BTPS_STRUCTURE_OFFSET(CSCS_Instance_Data_t, Sensor_Location_Length))


   /*********************************************************************/
   /**              Cycle Speed and Cadence Service Table              **/
   /*********************************************************************/

   /* The Cycle Speed and Cadence Service Declaration UUID.                      */
static BTPSCONST GATT_Primary_Service_16_Entry_t CSCS_Service_UUID =
{
   CSCS_SERVICE_BLUETOOTH_UUID_CONSTANT
};

/********************************** First Characteristic *************************************/
   /* The Cycle Speed and Cadence Measurement Characteristic Declaration.        */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t CSCS_Measurement_Declaration =
{
   GATT_CHARACTERISTIC_PROPERTIES_NOTIFY,
   CSCS_MEASUREMENT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The Cycle Speed and Cadence Measurement Characteristic Value.              */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t  CSCS_Measurement_Value =
{
   CSCS_MEASUREMENT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   0,
   NULL
};
/********************************** First Characteristic *************************************/

/********************************** Second Characteristic ************************************/
   /* The CSCS Feature Characteristic Declaration.                               */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t CSCS_Feature_Declaration =
{
   GATT_CHARACTERISTIC_PROPERTIES_READ,
   CSCS_FEATURE_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The CSCS Feature Characteristic Value.                                     */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t  CSCS_Feature_Value =
{
   CSCS_FEATURE_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   CSCS_CSC_FEATURE_INSTANCE_TAG,
   NULL
};
/********************************** Second Characteristic ************************************/

/*********************************** Third Characteristic ************************************/

   /* The Sensor Location Characteristic Declaration.                            */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t CSCS_Sensor_Location_Declaration =
{
   GATT_CHARACTERISTIC_PROPERTIES_READ,
   CSCS_SENSOR_LOCATION_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The Sensor Location Characteristic Value.                                  */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t  CSCS_Sensor_Location_Value =
{
   CSCS_SENSOR_LOCATION_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   CSCS_SENSOR_LOCATION_INSTANCE_TAG,
   NULL
};

/*********************************** Third Characteristic ************************************/

/*********************************** fourth Characteristic ***********************************/
#if (BTPS_CONFIGURATION_CSCS_SUPPORTED_SC_CONTROL_POINT)
   /* The SC Control Point Characteristic Declaration.                           */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t CSCS_SC_Control_Point_Declaration =
{

   (GATT_CHARACTERISTIC_PROPERTIES_WRITE|GATT_CHARACTERISTIC_PROPERTIES_INDICATE),
   CSCS_SC_CONTROL_POINT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The SC Control Point Characteristic Value.                                 */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t  CSCS_SC_Control_Point_Value =
{
   CSCS_SC_CONTROL_POINT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   0,
   NULL
};
#endif
/*********************************** fourth Characteristic ***********************************/

/******************************* Client Characteristic Configuration *******************************/

   /* Client Characteristic Configuration Descriptor.                            */
static GATT_Characteristic_Descriptor_16_Entry_t Client_Characteristic_Configuration =
{
   GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_BLUETOOTH_UUID_CONSTANT,
   GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_LENGTH,
   NULL
};

/******************************* Client Characteristic Configuration *******************************/

   /* The following defines the Cycling Speed and Cadence service that  */
   /* is registered with the GATT_Register_Service function call.       */
   /* * NOTE * This array will be registered with GATT in the call to   */
   /*          GATT_Register_Service.                                   */
BTPSCONST GATT_Service_Attribute_Entry_t Cycling_Speed_and_Cadence_Service[] =
{

   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetPrimaryService16,            (Byte_t *)&CSCS_Service_UUID},

/********************************** First Characteristic *************************************/
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetCharacteristicDeclaration16, (Byte_t *)&CSCS_Measurement_Declaration},
   {0,                                      aetCharacteristicValue16,       (Byte_t *)&CSCS_Measurement_Value},
   {GATT_ATTRIBUTE_FLAGS_READABLE_WRITABLE, aetCharacteristicDescriptor16,  (Byte_t *)&Client_Characteristic_Configuration},
/********************************** First Characteristic *************************************/

/********************************** Second Characteristic ************************************/
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetCharacteristicDeclaration16, (Byte_t *)&CSCS_Feature_Declaration},
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetCharacteristicValue16,       (Byte_t *)&CSCS_Feature_Value},
/********************************** Second Characteristic ************************************/

/*********************************** Third Characteristic ************************************/
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetCharacteristicDeclaration16, (Byte_t *)&CSCS_Sensor_Location_Declaration},
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetCharacteristicValue16,       (Byte_t *)&CSCS_Sensor_Location_Value},
/*********************************** Third Characteristic ************************************/


#if (BTPS_CONFIGURATION_CSCS_SUPPORTED_SC_CONTROL_POINT)   
/*********************************** fourth Characteristic ***********************************/
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetCharacteristicDeclaration16, (Byte_t *)&CSCS_SC_Control_Point_Declaration},
   {GATT_ATTRIBUTE_FLAGS_WRITABLE,          aetCharacteristicValue16,       (Byte_t *)&CSCS_SC_Control_Point_Value},
   {GATT_ATTRIBUTE_FLAGS_READABLE_WRITABLE, aetCharacteristicDescriptor16,  (Byte_t *)&Client_Characteristic_Configuration},
/*********************************** fourth Characteristic ***********************************/
#endif
};

#define CYCLING_SPEED_AND_CADENCE_SERVICE_ATTRIBUTE_COUNT               	(sizeof(Cycling_Speed_and_Cadence_Service)/sizeof(GATT_Service_Attribute_Entry_t))

#define CSCS_MEASUREMENT_CHARACTERISTIC_OFFSET             					2
#define CSCS_MEASUREMENT_CLIENT_CHARACTERISTIC_CONFIGURATION_OFFSET         3
#define CSCS_FEATURE_CHARACTERISTIC_OFFSET                 					5
#define CSCS_SENSOR_LOCATION_CHARACTERISTIC_OFFSET         					7
#if (BTPS_CONFIGURATION_CSCS_SUPPORTED_SC_CONTROL_POINT)
#define CSCS_SC_CONTROL_POINT_CHARACTERISTIC_OFFSET        					9
#define CSCS_SC_CONTROL_POINT_CHARACTERISTIC_CONFIGURATION_OFFSET    		10
#endif


   /*********************************************************************/
   /**                    END OF SERVICE TABLE                         **/
   /*********************************************************************/

   /* The following type defines a union large enough to hold all events*/
   /* dispatched by this module.                                        */
typedef union
{
   CSCS_Read_Client_Configuration_Data_t   Read_Client_Data;
   CSCS_Client_Configuration_Update_Data_t Client_Configuration_Update_Data;
   CSCS_Control_Point_Data_t               CSCS_SC_Control_Command_Data;
   CSCS_Confirmation_Data_t                CSCS_Confirmation_Data;
} CSCS_Event_Data_Buffer_t;

#define CSCS_EVENT_DATA_BUFFER_SIZE                       (sizeof(CSCS_Event_Data_Buffer_t))


   /* The following structure represents the information that will be   */
   /* stored during an outstanding GATT indication.                     */
typedef struct _tagIndicatonInfo_t_t
{
   unsigned int ConnectionID;
   int          TransactionID;
} IndicatonInfo_t;

   /* CSCS Service Instance Block.  This structure contains All         */
   /* information associated with a specific Bluetooth Stack ID (member */
   /* is present in this structure).                                    */
typedef struct _tagCSCSServerInstance_t
{
   unsigned int             BluetoothStackID;
   unsigned int             ServiceID;
   CSCS_Event_Callback_t    EventCallback;
   unsigned long            CallbackParameter;
   IndicatonInfo_t          CSControlPointInfo;
} CSCSServerInstance_t;

#define CSCS_SERVER_INSTANCE_DATA_SIZE                    (sizeof(CSCSServerInstance_t))

   /* Internal Variables to this Module (Remember that all variables    */
   /* declared static are initialized to 0 automatically by the         */
   /* compiler as part of standard C/C++).                              */


static CSCS_Instance_Data_t InstanceData[CSCS_MAXIMUM_SUPPORTED_INSTANCES];
                                            /* Variable which holds all */
                                            /* data that is unique for  */
                                            /* each service instance.   */


static CSCSServerInstance_t InstanceList[CSCS_MAXIMUM_SUPPORTED_INSTANCES];
                                            /* Variable which holds the */
                                            /* service instance data.   */

static Boolean_t            InstanceListInitialized;   
                                            /* Variable that flags that */
                                            /* is used to denote that   */
                                            /* this module has been     */
                                            /* successfully initialized.*/

static Word_t               SensorLocationBitMaskTable;   
                                            /* Variable that holds the  */
                                            /* Bitmask of the supported */
                                            /* Sensor list.             */
static Byte_t               SensorLocationTableLength;   
                                            /* Variable that holds the  */
                                            /* length of the Sensor list*/
                                            /* by counting the bits that*/
                                            /* are set in               */
                                            /*SensorLocationBitMaskTable*/ 
                                            /* variable.                */

                                            



   /* The following are the prototypes of local functions.              */
static Boolean_t InitializeModule(void);
static void CleanupModule(void);

static int FormatCycleSpeedandCadenceMeasurements(unsigned int InstanceID, unsigned int BufferLength, Byte_t *Buffer, CSCS_Measurements_Data_t *CSCS_Measurement);
static int DecodeClientConfigurationValue(unsigned int BufferLength, Byte_t *Buffer, Word_t *ClientConfiguration, CSCS_Characteristic_Type_t ClientConfigurationType);

static CSCS_Event_Data_t *FormatEventHeader(unsigned int BufferLength, Byte_t *Buffer, CSCS_Event_Type_t EventType, unsigned int InstanceID, unsigned int ConnectionID, unsigned int *TransactionID, GATT_Connection_Type_t ConnectionType, BD_ADDR_t *BD_ADDR);
static Boolean_t InstanceRegisteredByStackID(unsigned int BluetoothStackID);
static CSCSServerInstance_t *AcquireServiceInstance(unsigned int BluetoothStackID, unsigned int *InstanceID);
static int CSCSRegisterService(unsigned int BluetoothStackID, CSCS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange);

#if (BTPS_CONFIGURATION_CSCS_SUPPORTED_SC_CONTROL_POINT)

static int FormatSensorLocationList(unsigned int BufferLength, Byte_t *Buffer, CSCS_Control_Point_Data_t *CSCS_Control_Point);
static int DecodeControlPointCommand(unsigned int InstanceID, Word_t ValueLength, Byte_t *Value, CSCS_Control_Point_Data_t *CSCS_Control_Point);

#endif


   /* Bluetooth Event Callbacks.                                        */
static void BTPSAPI GATT_ServerEventCallback(unsigned int BluetoothStackID, GATT_Server_Event_Data_t *GATT_ServerEventData, unsigned long CallbackParameter);

/************************************************************************************************************************************************/

   /* The following function is a utility function that is used to      */
   /* reduce the ifdef blocks that are needed to handle the difference  */
   /* between module initialization for Threaded and NonThreaded stacks.*/
static Boolean_t InitializeModule(void)
{
   /* All we need to do is flag that we are initialized.                */
   if(InstanceListInitialized == 0)
   {
      InstanceListInitialized = TRUE;

      BTPS_MemInitialize(InstanceList, 0, sizeof(InstanceList));
   }

   return(TRUE);
}


   /* The following function is a utility function that exists to       */
   /* perform stack specific (threaded versus nonthreaded) cleanup.     */
static void CleanupModule(void)
{
   /* Flag that we are no longer initialized.                           */
   InstanceListInitialized = FALSE;
}
   

   /* The following function is used to format a Cycling Speed and      */
   /* cadence Measurement into a specified buffer.                      */
static int FormatCycleSpeedandCadenceMeasurements(unsigned int InstanceID, unsigned int BufferLength, Byte_t *Buffer, CSCS_Measurements_Data_t *CSCS_Measurement)
{
   int     ret_val = 0;
   int     SupportedFeatures;
   Byte_t *tempPtr;
   
   /* Make sure the parameters passed to us are semi-valid.             */
   if((CSCS_Measurement) && (CSCS_DATA_PRESENT_VALID(CSCS_Measurement->Flags)))
   {
		/* Copy the flags into the measurement Flags member.			*/
        tempPtr = Buffer;
		ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(tempPtr, CSCS_Measurement->Flags);
        tempPtr += CSCS_MEASUREMENTS_NOTIFICATION_DATA_SIZE;
        SupportedFeatures = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(InstanceData[(InstanceID)-1].CSC_Features));	 

		/* Checking Which Flags are set, So we will be able to   		*/
		/* format the appropriate fields                           		*/
		 
#if (BTPS_CONFIGURATION_CSCS_SUPPORTED_WHEEL_REVOLUTION_DATA)
		/* Wheel Data Section											*/
		if(CSCS_Measurement->Flags & SupportedFeatures & CSCS_CYCLING_SPEED_AND_CADENCE_MEASUREMENT_FLAGS_WHEEL_REVOLUTION_DATA_PRESENT)
		{
			/* Format the notification for Wheel Measurements values.   */
			ASSIGN_HOST_DWORD_TO_LITTLE_ENDIAN_UNALIGNED_DWORD((NonAlignedDWord_t*)tempPtr, CSCS_Measurement->Wheel_Data->Cumulative_Wheel_Revolutions);
            tempPtr += DWORD_SIZE;
			ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD((NonAlignedWord_t *)tempPtr, CSCS_Measurement->Wheel_Data->Last_Wheel_Event_Time);
            tempPtr += WORD_SIZE;
		}
#endif        
#if (BTPS_CONFIGURATION_CSCS_SUPPORTED_CRANK_REVOLUTION_DATA)
		/* Crank Data Section											*/
		if(CSCS_Measurement->Flags & SupportedFeatures & CSCS_CYCLING_SPEED_AND_CADENCE_MEASUREMENT_FLAGS_CRANK_REVOLUTION_DATA_PRESENT)
		{	
			/* Format the notification for Crank Measurements values.   */
			ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD((NonAlignedWord_t *)tempPtr, CSCS_Measurement->Crank_Data->Cumulative_Crank_Revolutions);
            tempPtr += WORD_SIZE;
    		ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD((NonAlignedWord_t *)tempPtr, CSCS_Measurement->Crank_Data->Last_Crank_Event_Time);
            tempPtr += WORD_SIZE;
		}       
#endif        
	}
	else
	{
		ret_val = CSCS_ERROR_INVALID_PARAMETER;
	}

		/* Finally return the result to the caller.                     */
   return(ret_val);
}		 


   /* The following function is a utility function that exists to decode*/
   /* an Client Configuration value into a user specified Bit Mask value*/
   /* . This function returns the zero if successful or a negative error*/
   /* code.                                                             */
 static int DecodeClientConfigurationValue(unsigned int BufferLength, Byte_t *Buffer, Word_t *ClientConfiguration, CSCS_Characteristic_Type_t ClientConfigurationType)
{
   int ret_val = CSCS_ERROR_MALFORMATTED_DATA;


#if (BTPS_CONFIGURATION_CSCS_SUPPORTED_SC_CONTROL_POINT)  
   /* Verify that the input parameters are valid.                       */
   /* This if checks if the size of BufferLength is valid, if Buffer    */
   /* isn't a NULL pointer and checks if we receive notification or     */
   /* indication configuration to its supported characteristic          */
   /* the characterisitcs are ctCyclingSpeedandCadenceMeasurement or    */
   /* ctSCControlPoint                                                  */
   if(((BufferLength == NON_ALIGNED_BYTE_SIZE) || 
    (BufferLength == GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_LENGTH)) && 
    (Buffer) && (ClientConfiguration && 
    ((*ClientConfiguration == 0) ||
    ((*ClientConfiguration == GATT_CLIENT_CONFIGURATION_CHARACTERISTIC_NOTIFY_ENABLE) && 
     (ClientConfigurationType == ctCyclingSpeedandCadenceMeasurement)) ||
    ((*ClientConfiguration == GATT_CLIENT_CONFIGURATION_CHARACTERISTIC_INDICATE_ENABLE) && 
     (ClientConfigurationType == ctSCControlPoint)))))
#else
   /* Verify that the input parameters are valid.                       */
   /* This if checks if the size of BufferLength is valid, if Buffer    */
   /* isn't a NULL pointer and checks if we receive notification or     */
   /* indication configuration to its supported characteristic          */
   /* the characterisitcs is ctCyclingSpeedandCadenceMeasurement        */
   if(((BufferLength == NON_ALIGNED_BYTE_SIZE) || 
    (BufferLength == GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_LENGTH)) && 
    (Buffer) && (ClientConfiguration && 
    ((*ClientConfiguration == 0) ||
    ((*ClientConfiguration == GATT_CLIENT_CONFIGURATION_CHARACTERISTIC_NOTIFY_ENABLE) && 
     (ClientConfigurationType == ctCyclingSpeedandCadenceMeasurement)))))
#endif
   {
      /* Read the requested Client Configuration.                       */
      if(BufferLength == NON_ALIGNED_BYTE_SIZE)
      {
         *ClientConfiguration = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(Buffer);
      }
      else
      {
         *ClientConfiguration = READ_UNALIGNED_WORD_LITTLE_ENDIAN(Buffer);
      }

      ret_val = 0;
   }
   else
   {
      if(BufferLength == GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_LENGTH)
      {
         ret_val = CSCS_ERROR_INVALID_PARAMETER;
      }
   }

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

#if (BTPS_CONFIGURATION_CSCS_SUPPORTED_SC_CONTROL_POINT)
   /* The following function is used to format Sensor Location list     */
   /* Into a specified buffer.                      					*/
static int FormatSensorLocationList(unsigned int BufferLength, Byte_t *Buffer, CSCS_Control_Point_Data_t *CSCS_Control_Point)
{
   int     ret_val = 0;
   Byte_t *tempPtr;
   Byte_t  Index = 0;
   Word_t  tempSensorLocationBitMaskTable;
  
   /* Make sure the parameters passed to us are semi-valid.             */
   if((CSCS_Control_Point) && (BufferLength) && (Buffer))
   {
		/* Verify that the buffer is big enough to hold the list.  		*/
		if(BufferLength >= CSCS_CONTROL_POINT_INDICATION_LIST_VALUE_SIZE(SensorLocationTableLength))
		{
    		/* Set a temporary pointer past the Response Parameter		   	*/
    		/* of the Control_Point_Response_Data.                       	*/
   			tempPtr = ((CSCS_Control_Point_Value_t *)Buffer)->Command_Data_Buffer.Indication.Response_Parameter;
   			/* Format all of the List Locations into the Buffer       		*/
            tempSensorLocationBitMaskTable = SensorLocationBitMaskTable;
   			while(tempSensorLocationBitMaskTable > 0)
   			{
                /* Change from bitmask to integer value                     */
                /* This while change the value by shifting right and checks */
                /* if the LSB bit is 0, if so it will increase the Index so */
                /* we will receive the integer value of each Bit that is set*/
                /* in this bitmask.                                         */
                if ((tempSensorLocationBitMaskTable & 0x01) != 0)
                {
                    /* Assign the next Sensor Location into the Buffer.     */
                    ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(tempPtr, Index);
   				    tempPtr += CSCS_SENSOR_LOCATION_VALUE_LENGTH;
                }

                tempSensorLocationBitMaskTable >>= 1;
                Index++;
    		}

    		/* Return success to the caller.                               	*/
    		ret_val = 0;
		
		}
		else
		{
			ret_val = CSCS_ERROR_MALFORMATTED_DATA;
		}
	}
	else
	{
		ret_val = CSCS_ERROR_INVALID_PARAMETER;
	}
	return(ret_val);
}


   /* The following function is a utility function that exists to aid in*/
   /* decoding a value read from a client, and interpreting it as a		*/
   /* Control Point command and value.  This function returns the zero  */
   /* if successful or a negative error code.                           */
static int DecodeControlPointCommand(unsigned int InstanceID, Word_t ValueLength, Byte_t *Value, CSCS_Control_Point_Data_t *CSCS_Control_Point)
{
   int     ret_val = 0;
   int     SupportedFeatures;
    
   /* Make sure the parameters passed to us are semi-valid.             */
   if((CSCS_Control_Point) && (ValueLength) && (Value))
   {
		/* Verify that the Value is big enough to hold Correct values.  */
		if(ValueLength >= CSCS_CONTROL_POINT_MINIMUM_VALUE_SIZE)		
		{
			CSCS_Control_Point->Op_Code = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(((CSCS_Control_Point_Value_t *)Value)->Op_Code));
            SupportedFeatures = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(InstanceData[(InstanceID)-1].CSC_Features));
            switch (CSCS_Control_Point->Op_Code)
            {
                case CSCS_CONTROL_POINT_OP_CODE_SET_CUMULATIVE_VALUE:
                     if (IS_WHEEL_REVOLUTION_DATA_SUPPORTED(SupportedFeatures))
                     {
			            CSCS_Control_Point->Command_data_Buffer.Cumulative_Value = READ_UNALIGNED_DWORD_LITTLE_ENDIAN(&(((CSCS_Control_Point_Value_t *)Value)->Command_Data_Buffer.Cumulative_Value));
                     }
                     else
                     {
                        CSCS_Control_Point->Command_data_Buffer.Indication.Response_Value = CSCS_CONTROL_POINT_RESPONSE_OP_CODE_NOT_SUPPORTED;
                     }
                     break;
                case CSCS_CONTROL_POINT_OP_CODE_UPDATE_SENSOR_LOCATION:
                     if(IS_MULTIPLE_SENSOR_LOCATION_SUPPORTED(SupportedFeatures))
                     {
                        CSCS_Control_Point->Command_data_Buffer.Sensor_Location_Value = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(((CSCS_Control_Point_Value_t *)Value)->Command_Data_Buffer.Sensor_Location_Value));
         				if (CSCS_SENSOR_LOCATION_VALID_TYPE(CSCS_Control_Point->Command_data_Buffer.Sensor_Location_Value) && (SensorLocationBitMaskTable & (1 << CSCS_Control_Point->Command_data_Buffer.Sensor_Location_Value)))
                        {
                           ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(InstanceData[InstanceID-1].Sensor_Location), CSCS_Control_Point->Command_data_Buffer.Sensor_Location_Value);
         		    	   CSCS_Control_Point->Command_data_Buffer.Indication.Response_Value = CSCS_CONTROL_POINT_RESPONSE_SUCCESS;
                        }
                     	else
                     	{
         	    			CSCS_Control_Point->Command_data_Buffer.Indication.Response_Value = CSCS_CONTROL_POINT_RESPONSE_INVALID_PARAMETER;
                     	}
                     }
                     else
                     {
                         CSCS_Control_Point->Command_data_Buffer.Indication.Response_Value = CSCS_CONTROL_POINT_RESPONSE_OP_CODE_NOT_SUPPORTED;
                     }
                     break;
                case CSCS_CONTROL_POINT_OP_CODE_REQUEST_SUPPORTED_SENSOR_LOCATION:
                     if(IS_MULTIPLE_SENSOR_LOCATION_SUPPORTED(SupportedFeatures))
                     {
           		    	    CSCS_Control_Point->Command_data_Buffer.Indication.Response_Value = CSCS_CONTROL_POINT_RESPONSE_SUCCESS;         
                     }
                     else
                     {
                        CSCS_Control_Point->Command_data_Buffer.Indication.Response_Value = CSCS_CONTROL_POINT_RESPONSE_OP_CODE_NOT_SUPPORTED;
                     }
                     break;
                default:
                        CSCS_Control_Point->Command_data_Buffer.Indication.Response_Value = CSCS_CONTROL_POINT_RESPONSE_OP_CODE_NOT_SUPPORTED;
                     break;
            }
		}
		else
		{
			ret_val = CSCS_ERROR_MALFORMATTED_DATA;
		}
	}
	else
	{
		ret_val = CSCS_ERROR_INVALID_PARAMETER;
	}
	return(ret_val);
}
#endif

   
   /* The following function is a utility function that exists to format*/
   /* a CSCS Event into the specified buffer.                            */
   /* * NOTE * TransactionID is optional and may be set to NULL.        */
   /* * NOTE * BD_ADDR is NOT optional and may NOT be set to NULL.      */
static CSCS_Event_Data_t *FormatEventHeader(unsigned int BufferLength, Byte_t *Buffer, CSCS_Event_Type_t EventType, unsigned int InstanceID, unsigned int ConnectionID, unsigned int *TransactionID, GATT_Connection_Type_t ConnectionType, BD_ADDR_t *BD_ADDR)
{
   CSCS_Event_Data_t *EventData = NULL;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BufferLength >= (CSCS_EVENT_DATA_SIZE + CSCS_EVENT_DATA_BUFFER_SIZE)) && (Buffer) && (BD_ADDR))
   {
      /* Format the header of the event, that is data that is common to */
      /* all events.                                                    */
      BTPS_MemInitialize(Buffer, 0, BufferLength);

      EventData                                                                                       = (CSCS_Event_Data_t *)Buffer;
      EventData->Event_Data_Type                                                                      = EventType;
      EventData->Event_Data.CSCS_Read_Client_Configuration_Data                                       = (CSCS_Read_Client_Configuration_Data_t *)(((Byte_t *)EventData) + CSCS_EVENT_DATA_SIZE);

      EventData->Event_Data.CSCS_Read_Client_Configuration_Data->ConnectionHeader.InstanceID          = InstanceID;
      EventData->Event_Data.CSCS_Read_Client_Configuration_Data->ConnectionHeader.ConnectionID        = ConnectionID;

      if(TransactionID)
      {
         EventData->Event_Data.CSCS_Read_Client_Configuration_Data->TransactionID                     = *TransactionID;
         EventData->Event_Data.CSCS_Read_Client_Configuration_Data->ConnectionHeader.ConnectionType   = ConnectionType;
         EventData->Event_Data.CSCS_Read_Client_Configuration_Data->ConnectionHeader.RemoteDevice     = *BD_ADDR;
      }
      else
      {
         EventData->Event_Data.CSCS_Client_Configuration_Update_Data->ConnectionHeader.ConnectionType = ConnectionType;
         EventData->Event_Data.CSCS_Client_Configuration_Update_Data->ConnectionHeader.RemoteDevice   = *BD_ADDR;
      }
   }

   /* Finally return the result to the caller.                          */
   return(EventData);
}
  

   /* The following function is a utility function that exists to check */
   /* to see if an instance has already been registered for a specified */
   /* Bluetooth Stack ID.                                               */
   /* * NOTE * Since this is an internal function no check is done on   */
   /*          the input parameters.                                    */
static Boolean_t InstanceRegisteredByStackID(unsigned int BluetoothStackID)
{
   Boolean_t    ret_val = FALSE;
   unsigned int Index;

   for(Index=0;Index<CSCS_MAXIMUM_SUPPORTED_INSTANCES;Index++)
   {
      if((InstanceList[Index].BluetoothStackID == BluetoothStackID) && (InstanceList[Index].ServiceID))
      {
         ret_val = TRUE;
         break;
      }
   }

   /* Finally return the result to the caller.                          */
   return(ret_val);
} 
 

   /* The following function is a utility function that exists to       */
   /* acquire a specified service instance.                             */
   /* * NOTE * Since this is an internal function no check is done on   */
   /*          the input parameters.                                    */
   /* * NOTE * If InstanceID is set to 0, this function will return the */
   /*          next free instance.                                      */
static CSCSServerInstance_t *AcquireServiceInstance(unsigned int BluetoothStackID, unsigned int *InstanceID)
{
   unsigned int          LocalInstanceID;
   unsigned int          Index;
   CSCSServerInstance_t *ret_val = NULL;

   /* Lock the Bluetooth Stack to gain exclusive access to this         */
   /* Bluetooth Protocol Stack.                                         */
   if(BSC_LockBluetoothStack(BluetoothStackID) == 0)
   {
      /* Acquire the BSC List Lock while we are searching the instance  */
      /* list.                                                          */
      if(BSC_AcquireListLock())
      {
         /* Store a copy of the passed in InstanceID locally.           */
         LocalInstanceID = *InstanceID;

         /* Verify that the Instance ID is valid.                       */
         if((LocalInstanceID) && (LocalInstanceID <= CSCS_MAXIMUM_SUPPORTED_INSTANCES))
         {
            /* Decrement the LocalInstanceID (to access the InstanceList*/
            /* which is 0 based).                                       */
            --LocalInstanceID;

            /* Verify that this Instance is registered and valid.       */
            if((InstanceList[LocalInstanceID].BluetoothStackID == BluetoothStackID) && (InstanceList[LocalInstanceID].ServiceID))
            {
               /* Return a pointer to this instance.                    */
               ret_val = &InstanceList[LocalInstanceID];
            }
         }
         else
         {
            /* Verify that we have been requested to find the next free */
            /* instance.                                                */
            if(LocalInstanceID == 0)
            {
               /* Try to find a free instance.                          */
               for(Index=0;Index<CSCS_MAXIMUM_SUPPORTED_INSTANCES;Index++)
               {
                  /* Check to see if this instance is being used.       */
                  if((InstanceList[Index].ServiceID) == 0)
                  {
                     /* Return the InstanceID AND a pointer to the      */
                     /* instance.                                       */
                     *InstanceID = Index+1;
                     ret_val     = &InstanceList[Index];
                     break;
                  }
               }
            }
         }

         /* Release the previously acquired list lock.                  */
         BSC_ReleaseListLock();
      }

      /* If we failed to acquire the instance then we should un-lock the*/
      /* previously acquired Bluetooth Stack.                           */
      if(ret_val == 0)
      {
         BSC_UnLockBluetoothStack(BluetoothStackID);
      }
   }

   /* Finally return the result to the caller.                          */
   return(ret_val);
}


   /* The following function is a utility function which is used to     */
   /* register an CSCS Service.  This function returns the positive,     */
   /* non-zero, Instance ID on success or a negative error code.        */
static int CSCSRegisterService(unsigned int BluetoothStackID, CSCS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange)
{
   int                  ret_val;
   unsigned int         InstanceID;
   CSCSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (EventCallback) && (ServiceID))
   {

      /* utility function that initialize InstanceListInitialized to 0  */
      CleanupModule();
      
      /* making sure that the Bluetooth Stack CSCS Module is Initialized*/
      /* correctly.                                                     */
      InitializeCSCSModule();
      
      /* Verify that no instance is registered to this Bluetooth Stack. */
      if(InstanceRegisteredByStackID(BluetoothStackID) == 0)
      {
         /* Acquire a free CSCS Instance.                                */
         InstanceID = 0;
         if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
         {
            /* Call GATT to register the CSCS service.                   */
            ret_val = GATT_Register_Service(BluetoothStackID, CSCS_SERVICE_FLAGS_LE_CONNECTION, CYCLING_SPEED_AND_CADENCE_SERVICE_ATTRIBUTE_COUNT, (GATT_Service_Attribute_Entry_t *)Cycling_Speed_and_Cadence_Service, ServiceHandleRange, GATT_ServerEventCallback, InstanceID);
            if(ret_val > 0)
            {
               /* Save the Instance information.                        */
               ServiceInstance->BluetoothStackID  = BluetoothStackID;
               ServiceInstance->ServiceID         = (unsigned int)ret_val;
               ServiceInstance->EventCallback     = EventCallback;
               ServiceInstance->CallbackParameter = CallbackParameter;
               *ServiceID                         = (unsigned int)ret_val;

               /* Intilize the Instance Data for this instance.         */
               BTPS_MemInitialize(&InstanceData[InstanceID-1], 0, CSCS_INSTANCE_DATA_SIZE);

               ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(InstanceData[InstanceID-1].CSC_Features_Length), CSCS_CSC_FEATURE_VALUE_LENGTH);

               ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(InstanceData[InstanceID-1].Sensor_Location_Length), CSCS_SENSOR_LOCATION_VALUE_LENGTH);

               /* Initilize the Sensor Features for this instance.      */
               ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(InstanceData[InstanceID-1].CSC_Features), 0);
               
               /* Initilize the Sensor Location for this instance.      */
               ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(InstanceData[InstanceID-1].Sensor_Location), CSC_SENSOR_LOCATION_OTHER);

               /* Return the CSCS Instance ID.                           */
               ret_val                                        = (int)InstanceID;
            }

            /* UnLock the previously locked Bluetooth Stack.            */
            BSC_UnLockBluetoothStack(BluetoothStackID);
         }
         else
         {
            ret_val = CSCS_ERROR_INSUFFICIENT_RESOURCES;
         }
      }
      else
      {
         ret_val = CSCS_ERROR_SERVICE_ALREADY_REGISTERED;
      }
   }
   else
   {
      ret_val = CSCS_ERROR_INVALID_PARAMETER;
   }

   /* Finally return the result to the caller.                          */
   return(ret_val);
}


   /* The following function is the GATT Server Event Callback that     */
   /* handles all requests made to the CSCS Service for all registered  */
   /* instances.                                                        */
static void BTPSAPI GATT_ServerEventCallback(unsigned int BluetoothStackID, GATT_Server_Event_Data_t *GATT_ServerEventData, unsigned long CallbackParameter)
{
   Word_t                      AttributeOffset;
   Word_t                      InstanceTag;
   Word_t                      ValueLength;
   Byte_t                     *Value;
   Word_t                      temp;
   Byte_t                      Event_Buffer[CSCS_EVENT_DATA_SIZE + CSCS_EVENT_DATA_BUFFER_SIZE];
   unsigned int                TransactionID;
   unsigned int                InstanceID;
   CSCS_Event_Data_t          *EventData;
   CSCSServerInstance_t       *ServiceInstance;

    
#if (BTPS_CONFIGURATION_CSCS_SUPPORTED_SC_CONTROL_POINT)  
   CSCS_Characteristic_Type_t  Characteristic_Type;
#endif

   /* Verify that all parameters to this callback are Semi-Valid.       */
   if((BluetoothStackID) && (GATT_ServerEventData) && (CallbackParameter))
   {
      /* The Instance ID is always registered as the callback parameter.*/
      InstanceID = (unsigned int)CallbackParameter;

      /* Acquire the Service Instance for the specified service.        */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         switch(GATT_ServerEventData->Event_Data_Type)
         {
            case etGATT_Server_Read_Request:
               /* Verify that the Event Data is semi valid.             */
               if(GATT_ServerEventData->Event_Data.GATT_Read_Request_Data)
               {
                  AttributeOffset = GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->AttributeOffset;
                  TransactionID   = GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->TransactionID;

                  /* Verify that they are not trying to write with an   */
                  /* offset or using preprared writes.                  */
                  if((GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->AttributeValueOffset) == 0)
                  {
                     if(Cycling_Speed_and_Cadence_Service[AttributeOffset].Attribute_Entry_Type == aetCharacteristicDescriptor16)
                     {
                        EventData = FormatEventHeader(sizeof(Event_Buffer), Event_Buffer, etCSCS_Server_Read_Client_Configuration_Request, InstanceID, GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->ConnectionID, &TransactionID, GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->ConnectionType, &(GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->RemoteDevice));
                        if(EventData)
                        {
                           /* Determine the Correct Client Configuration*/
                           /* Type.                                     */
#if (BTPS_CONFIGURATION_CSCS_SUPPORTED_SC_CONTROL_POINT)                               
                           if((AttributeOffset == CSCS_MEASUREMENT_CLIENT_CHARACTERISTIC_CONFIGURATION_OFFSET) 
                          || (AttributeOffset == CSCS_SC_CONTROL_POINT_CHARACTERISTIC_CONFIGURATION_OFFSET))
#else
                           if(AttributeOffset == CSCS_MEASUREMENT_CLIENT_CHARACTERISTIC_CONFIGURATION_OFFSET)
#endif 
                           {
                               /* Format the rest of the event.             */
                               EventData->Event_Data_Size = CSCS_CLIENT_CONFIGURATION_UPDATE_DATA_SIZE;
                               if (AttributeOffset == CSCS_MEASUREMENT_CLIENT_CHARACTERISTIC_CONFIGURATION_OFFSET)
                               {
                                   EventData->Event_Data.CSCS_Read_Client_Configuration_Data->ClientConfigurationType = ctCyclingSpeedandCadenceMeasurement;
                               }
#if (BTPS_CONFIGURATION_CSCS_SUPPORTED_SC_CONTROL_POINT)                               
                               else if (AttributeOffset == CSCS_SC_CONTROL_POINT_CHARACTERISTIC_CONFIGURATION_OFFSET)
                               {
                                   EventData->Event_Data.CSCS_Read_Client_Configuration_Data->ClientConfigurationType = ctSCControlPoint;
                               }
#endif                               
                               /* Dispatch the event.                       */
                               __BTPSTRY
                               {
                                  (*ServiceInstance->EventCallback)(ServiceInstance->BluetoothStackID, EventData, ServiceInstance->CallbackParameter);
                               }
                               __BTPSEXCEPT(1)
                               {
                                  /* Do Nothing.                            */
                               }
                           }
                           else
                           {
                               GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR);
                           }
                        }
                        else
                        {
                           GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR);
                        }
                  }
                  else
                  {
                     if(Cycling_Speed_and_Cadence_Service[AttributeOffset].Attribute_Entry_Type == aetCharacteristicValue16)
                     {
                        InstanceTag = (Word_t)(((GATT_Characteristic_Value_16_Entry_t *)Cycling_Speed_and_Cadence_Service[AttributeOffset].Attribute_Value)->Characteristic_Value_Length);
                        ValueLength = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(((Byte_t *)(&InstanceData[InstanceID-1]))[InstanceTag]));
                        if(AttributeOffset == CSCS_FEATURE_CHARACTERISTIC_OFFSET)
                        {
                             temp = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(InstanceData[(InstanceID)-1].CSC_Features));
                             GATT_Read_Response(BluetoothStackID, TransactionID, (unsigned int)ValueLength, (Byte_t *)(&temp));
                        } 
                        else
                        {
                            if(AttributeOffset == CSCS_SENSOR_LOCATION_CHARACTERISTIC_OFFSET)
                            {
                                 temp = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(InstanceData[(InstanceID)-1].Sensor_Location));
                                 GATT_Read_Response(BluetoothStackID, TransactionID, (unsigned int)ValueLength, (Byte_t *)(&temp));
                            }
                        }
                     }
                  }
               }
               else
               {
                     GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, ATT_PROTOCOL_ERROR_CODE_ATTRIBUTE_NOT_LONG);
               }
               break;
               }
            case etGATT_Server_Write_Request:
               /* Verify that the Event Data is semi valid.             */
               if(GATT_ServerEventData->Event_Data.GATT_Write_Request_Data)
               {
                  AttributeOffset = GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->AttributeOffset;
                  TransactionID   = GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->TransactionID;
                  ValueLength     = GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->AttributeValueLength;
                  Value           = GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->AttributeValue;

                  if(((GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->AttributeValueOffset) == 0) && ((GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->DelayWrite) == 0))
                  {
                     if(Cycling_Speed_and_Cadence_Service[AttributeOffset].Attribute_Entry_Type == aetCharacteristicDescriptor16)
                     {
                        /* Begin formatting the Client Configuration    */
                        /* Update event.                                */
                        EventData = FormatEventHeader(sizeof(Event_Buffer), Event_Buffer, etCSCS_Server_Client_Configuration_Update, InstanceID, GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->ConnectionID, NULL, GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->ConnectionType, &(GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->RemoteDevice));
                        if(EventData)
                        {
                           /* Format the rest of the event.             */
                           EventData->Event_Data_Size = CSCS_CLIENT_CONFIGURATION_UPDATE_DATA_SIZE;

                           /* Determine the Correct Client Configuration*/
                           /* Type.                                     */
#if (BTPS_CONFIGURATION_CSCS_SUPPORTED_SC_CONTROL_POINT)                               
                           if((AttributeOffset == CSCS_MEASUREMENT_CLIENT_CHARACTERISTIC_CONFIGURATION_OFFSET) 
                          || (AttributeOffset == CSCS_SC_CONTROL_POINT_CHARACTERISTIC_CONFIGURATION_OFFSET))
#else
                           if(AttributeOffset == CSCS_MEASUREMENT_CLIENT_CHARACTERISTIC_CONFIGURATION_OFFSET)
#endif 
                           {
                               if (AttributeOffset == CSCS_MEASUREMENT_CLIENT_CHARACTERISTIC_CONFIGURATION_OFFSET)
                               {
                                   EventData->Event_Data.CSCS_Client_Configuration_Update_Data->ClientConfigurationType = ctCyclingSpeedandCadenceMeasurement;
                               }
#if (BTPS_CONFIGURATION_CSCS_SUPPORTED_SC_CONTROL_POINT)                               
                               else if (AttributeOffset == CSCS_SC_CONTROL_POINT_CHARACTERISTIC_CONFIGURATION_OFFSET)
                               {
                                   EventData->Event_Data.CSCS_Client_Configuration_Update_Data->ClientConfigurationType = ctSCControlPoint;
                               }
#endif                          
                               /* Attempt to decode the request Client      */
                               /* Configuration.                            */
                               if(DecodeClientConfigurationValue(ValueLength, Value, &(EventData->Event_Data.CSCS_Client_Configuration_Update_Data->ClientConfiguration), EventData->Event_Data.CSCS_Client_Configuration_Update_Data->ClientConfigurationType) == 0)
                               {
                                  /* Go ahead and accept the write request  */
                                  /* since we have decoded the Client       */
                                  /* Configuration Value successfully.      */
                                  GATT_Write_Response(BluetoothStackID, TransactionID);
    
                                  /* Dispatch the event.                    */
                                  __BTPSTRY
                                  {
                                     (*ServiceInstance->EventCallback)(ServiceInstance->BluetoothStackID, EventData, ServiceInstance->CallbackParameter);
                                  }
                                  __BTPSEXCEPT(1)
                                  {
                                     /* Do Nothing.                         */
                                  }
                               }
                               else
                               {
                                   GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, CSCS_ERROR_CODE_CLIENT_CCD_IMPROPERLY_CONFIGURED);
                               }
                           }
                           else
                           {
                              GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, ATT_PROTOCOL_ERROR_CODE_REQUEST_NOT_SUPPORTED);
                           }
                        }
                        else
                        {
                           GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR);
                        }
                     }
#if (BTPS_CONFIGURATION_CSCS_SUPPORTED_SC_CONTROL_POINT)                               
                     else
                     {
                        /* Format and Dispatch the event.               */
                        EventData = FormatEventHeader(sizeof(Event_Buffer), Event_Buffer, etCSCS_Server_SC_Control_Point_Command, InstanceID, GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->ConnectionID, &TransactionID, GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->ConnectionType, &(GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->RemoteDevice));
                        if(EventData)
                        {
                           if(ServiceInstance->CSControlPointInfo.ConnectionID == 0)
                           {

                               /* Format the rest of the event.             */
                               EventData->Event_Data_Size = ValueLength;
                               EventData->Event_Data.CSCS_Control_Point_Data->AttributeOffset = AttributeOffset;

                               /* This function Decode the cumulative value */
                               /* for the application, and formats the      */
                               /* Update location value to the service      */
                               if(DecodeControlPointCommand(InstanceID, ValueLength, Value, (EventData->Event_Data.CSCS_Control_Point_Data)) == 0)
                               {
                                  /* Dispatch the event.                    */
                                  __BTPSTRY
                                  {
                                     (*ServiceInstance->EventCallback)(ServiceInstance->BluetoothStackID, EventData, ServiceInstance->CallbackParameter);
                                  }
                                  __BTPSEXCEPT(1)
                                  {
                                     /* Do Nothing.                         */
                                  }
                               }
                               else
                               {
                                  GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, CSCS_ERROR_CODE_CLIENT_CCD_IMPROPERLY_CONFIGURED);
                               }
                           }
                           else
                           {
                               GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, CSCS_ERROR_CODE_PROCEDURE_ALREADY_IN_PROGRESS);
                           }
                        }
                        else
                        {
                           GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR);
                        }
                     }
#endif                     

                  }
                  else
                  {
                     GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, ATT_PROTOCOL_ERROR_CODE_REQUEST_NOT_SUPPORTED);
                  }
               }
               break;
#if (BTPS_CONFIGURATION_CSCS_SUPPORTED_SC_CONTROL_POINT)                               
            case etGATT_Server_Confirmation_Response:
               /* Verify that the Event Data is semi valid.             */
               if(GATT_ServerEventData->Event_Data.GATT_Confirmation_Data)
               {
                  /* Determine the type of the indication that has been */
                  /* confirmed.                                         */
                  if((ServiceInstance->CSControlPointInfo.ConnectionID == GATT_ServerEventData->Event_Data.GATT_Confirmation_Data->ConnectionID) && ((unsigned int)ServiceInstance->CSControlPointInfo.TransactionID == GATT_ServerEventData->Event_Data.GATT_Confirmation_Data->TransactionID))
                  {
                     ValueLength                                                         = 1;
                     Characteristic_Type                                                 = ctSCControlPoint;
                     ServiceInstance->CSControlPointInfo.ConnectionID                    = 0;
                  }
                  else
                  {
                     ValueLength = 0;
                  }

                  if(ValueLength)
                  {
                     /* Format the event.                               */
                     EventData = FormatEventHeader(sizeof(Event_Buffer), Event_Buffer, etCSCS_Confirmation_Response, InstanceID, GATT_ServerEventData->Event_Data.GATT_Confirmation_Data->ConnectionID, NULL, GATT_ServerEventData->Event_Data.GATT_Confirmation_Data->ConnectionType, &(GATT_ServerEventData->Event_Data.GATT_Confirmation_Data->RemoteDevice));
                     if(EventData)
                     {
                        /* Format the rest of the event.                */
                        EventData->Event_Data_Size                                        = CSCS_CONFIRMATION_DATA_SIZE;
                        EventData->Event_Data.CSCS_Confirmation_Data->Characteristic_Type = Characteristic_Type;
                        EventData->Event_Data.CSCS_Confirmation_Data->Status              = GATT_ServerEventData->Event_Data.GATT_Confirmation_Data->Status;

                        /* Dispatch the event.                          */
                        __BTPSTRY
                        {
                           (*ServiceInstance->EventCallback)(ServiceInstance->BluetoothStackID, EventData, ServiceInstance->CallbackParameter);
                        }
                        __BTPSEXCEPT(1)
                        {
                           /* Do Nothing.                               */
                        }
                     }
                  }
               }
               break;
#endif
            case etGATT_Server_Device_Disconnection:
               /* Verify that the Event Data is semi valid.             */
               if(GATT_ServerEventData->Event_Data.GATT_Device_Disconnection_Data)
               {
                  /* Check for an outstanding SC Control Point          */
                  /* indication.                                        */
                  if(ServiceInstance->CSControlPointInfo.ConnectionID == GATT_ServerEventData->Event_Data.GATT_Device_Disconnection_Data->ConnectionID)
                  {
                     ServiceInstance->CSControlPointInfo.ConnectionID = 0;
                  }
               }
               break;
            default:
               /* Do nothing, as this is just here to get rid of        */
               /* warnings that some compilers flag when not all cases  */
               /* are handled in a switch off of a enumerated value.    */
               break;
         }

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
   }
}


   /* The following function is responsible for making sure that the    */
   /* Bluetooth Stack CSCS Module is Initialized correctly.  This       */
   /* function *MUST* be called before ANY other Bluetooth Stack CSCS   */
   /* function can be called.  This function returns non-zero if the    */
   /* Module was initialized correctly, or a zero value if there was an */
   /* error.                                                            */
   /* * NOTE * Internally, this module will make sure that this function*/
   /*          has been called at least once so that the module will    */
   /*          function.  Calling this function from an external        */
   /*          location is not necessary.                               */
int InitializeCSCSModule(void)
{
   return((int)InitializeModule());
}


   /* The following function is responsible for instructing the         */
   /* Bluetooth Stack CSCS Module to clean up any resources that it has */
   /* allocated.  Once this function has completed, NO other Bluetooth  */
   /* Stack CSCS Functions can be called until a successful call to the */
   /* InitializeCSCSModule() function is made.  The parameter to this   */
   /* function specifies the context in which this function is being    */
   /* called.  If the specified parameter is TRUE, then the module will */
   /* make sure that NO functions that would require waiting/blocking on*/
   /* Mutexes/Events are called.  This parameter would be set to TRUE if*/
   /* this function was called in a context where threads would not be  */
   /* allowed to run.  If this function is called in the context where  */
   /* threads are allowed to run then this parameter should be set to   */
   /* FALSE.                                                            */
void CleanupCSCSModule(Boolean_t ForceCleanup)
{
   /* Check to make sure that this module has been initialized.         */
   if(InstanceListInitialized)
   {
      /* Wait for access to the CSCS Context List.                      */
      if((ForceCleanup) || ((ForceCleanup == 0) && (BSC_AcquireListLock())))
      {
         /* Cleanup the Instance List.                                  */
         BTPS_MemInitialize(InstanceList, 0, sizeof(InstanceList));

         if(ForceCleanup == 0)
         {
            BSC_ReleaseListLock();
         }
      }

      /* Cleanup the module.                                            */
      CleanupModule();
   }
}


   /* The following function is responsible for opening a CSCS Server.   */
   /* The first parameter is the Bluetooth Stack ID on which to open the*/
   /* server.  The second parameter is the Callback function to call    */
   /* when an event occurs on this Server Port.  The third parameter is */
   /* a user-defined callback parameter that will be passed to the      */
   /* callback function with each event.  The final parameter is a      */
   /* pointer to store the GATT Service ID of the registered CSCS        */
   /* service.  This can be used to include the service registered by   */
   /* this call.  This function returns the positive, non-zero, Instance*/
   /* ID or a negative error code.                                      */
   /* * NOTE * Only 1 CSCS Server may be open at a time, per Bluetooth   */
   /*          Stack ID.                                                */
   /* * NOTE * All Client Requests will be dispatch to the EventCallback*/
   /*          function that is specified by the second parameter to    */
   /*          this function.                                           */
int BTPSAPI CSCS_Initialize_Service(unsigned int BluetoothStackID, CSCS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID)
{
   GATT_Attribute_Handle_Group_t ServiceHandleRange;

    /* Initialize the Service Handle Group to 0.                        */
   ServiceHandleRange.Starting_Handle = 0;
   ServiceHandleRange.Ending_Handle   = 0;

   return(CSCSRegisterService(BluetoothStackID, EventCallback, CallbackParameter, ServiceID, &ServiceHandleRange));
}


   /* The following function is responsible for opening a CSCS Server.  */
   /* The first parameter is the Bluetooth Stack ID on which to open the*/
   /* server.  The second parameter is the Callback function to call    */
   /* when an event occurs on this Server Port.  The third parameter is */
   /* a user-defined callback parameter that will be passed to the      */
   /* callback function with each event.  The fourth parameter is a     */
   /* pointer to store the GATT Service ID of the registered CSCS       */
   /* service.  This can be used to include the service registered by   */
   /* this call.  The final parameter is a pointer, that on input can be*/
   /* used to control the location of the service in the GATT database, */
   /* and on ouput to store the service handle range.  This function    */
   /* returns the positive, non-zero, Instance ID or a negative error   */
   /* code.                                                             */
   /* * NOTE * Only 1 CSCS Server may be open at a time, per Bluetooth  */
   /*          Stack ID.                                                */
   /* * NOTE * All Client Requests will be dispatch to the EventCallback*/
   /*          function that is specified by the second parameter to    */
   /*          this function.                                           */
int BTPSAPI CSCS_Initialize_Service_Handle_Range(unsigned int BluetoothStackID, CSCS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange)
{
   return(CSCSRegisterService(BluetoothStackID, EventCallback, CallbackParameter, ServiceID, ServiceHandleRange));
}


   /* The following function is responsible for closing a previously    */
   /* opened CSCS Server.  The first parameter is the Bluetooth Stack ID */
   /* on which to close the server.  The second parameter is the        */
   /* InstanceID that was returned from a successful call to            */
   /* CSCS_Initialize_Service().  This function returns a zero if        */
   /* successful or a negative return error code if an error occurs.    */
int BTPSAPI CSCS_Cleanup_Service(unsigned int BluetoothStackID, unsigned int InstanceID)
{
   int                   ret_val;
   CSCSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID))
   {
      /* Acquire the specified CSCS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Verify that the service is actually registered.             */
         if(ServiceInstance->ServiceID)
         {
            /* Call GATT to un-register the service.                    */
            GATT_Un_Register_Service(BluetoothStackID, ServiceInstance->ServiceID);

            /* mark the instance entry as being free.                   */
            BTPS_MemInitialize(ServiceInstance, 0, CSCS_SERVER_INSTANCE_DATA_SIZE);

            /* Instructing the Bluetooth Stack CSCS Module to clean up  */
            /* any resources that it has allocated.                     */
            CleanupCSCSModule (TRUE);

            /* return success to the caller.                            */
            ret_val = 0;
         }
         else
         {
            ret_val = CSCS_ERROR_INVALID_PARAMETER;
         }

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(BluetoothStackID);
      }
      else
      {
         ret_val = CSCS_ERROR_INVALID_INSTANCE_ID;
      }
   }
   else
   {
      ret_val = CSCS_ERROR_INVALID_PARAMETER;
   }

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   
   /* The following function is responsible for querying the number of  */
   /* attributes that are contained in the CSCS Service that is         */
   /* function registered with a call to CSCS_Initialize_Service().   	*/
   /* This returns the non-zero number of attributes that are contained */
   /* in a CSCS Server or zero on failure.                              */
unsigned int BTPSAPI CSCS_Query_Number_Attributes(void)
{
   /* Simply return the number of attributes that are contained in a 	*/
   /* CSCS service.                                                     */
   return((unsigned int)(CYCLING_SPEED_AND_CADENCE_SERVICE_ATTRIBUTE_COUNT));
}

 


   
   /* The following function is responsible for querying the current    */
   /* Location of the sensor on the specified CSCS Instance.  The first */
   /* parameter is the Bluetooth Stack ID of the Bluetooth Device.  The */
   /* second parameter is the InstanceID returned from a successful call*/
   /* to CSCS_Initialize_Server().  The final parameter is a pointer to */
   /* return the current Sensor Location for the specified CSCS         */
   /* Instance.  This function returns a zero if successful or a        */
   /* negative return error code if an error occurs.                    */
int BTPSAPI CSCS_Query_Sensor_Location(unsigned int BluetoothStackID, unsigned int InstanceID, Byte_t *Sensor_Location)
{
   int                   ret_val = 0;
   CSCSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID))
   {
      /* Acquire the specified CSCS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Assign the measurement interval for the specified instance. */
         *Sensor_Location = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(InstanceData[InstanceID-1].Sensor_Location));

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
      {
         ret_val = CSCS_ERROR_INVALID_INSTANCE_ID;
      }
   }
   else
   {
      ret_val = CSCS_ERROR_INVALID_PARAMETER;
   }

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

		 
   /* The following function is responsible for responding to a CSCS    */
   /* Read Client Configuration Request.  The first parameter is the    */
   /* Bluetooth Stack ID of the Bluetooth Device.  The second parameter */
   /* is the InstanceID returned from a successful call to              */
   /* CSCS_Initialize_Server().  The third is the Transaction ID of the */
   /* request.  The final parameter contains the Client Configuration to*/
   /* send to the remote device.  This function returns a zero if       */
   /* successful or a negative return error code if an error occurs.    */
int BTPSAPI CSCS_Read_Response_For_Read_Client_Configuration(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int TransactionID, Word_t Client_Configuration)
{
   int                  ret_val;
   Word_t               ValueLength;
   NonAlignedWord_t     ClientConfiguration;
   CSCSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (TransactionID))
   {
      /* Acquire the specified CSCS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Format the Read Response.                                   */
         ValueLength = GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_LENGTH;
         ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&ClientConfiguration, Client_Configuration);

         /* Send the response.                                          */
         ret_val = GATT_Read_Response(ServiceInstance->BluetoothStackID, TransactionID, (unsigned int)ValueLength, (Byte_t *)&ClientConfiguration);

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
      {
         ret_val = CSCS_ERROR_INVALID_INSTANCE_ID;
      }
   }
   else
   {
      ret_val = CSCS_ERROR_INVALID_PARAMETER;
   }

   /* Finally return the result to the caller.                          */
   return(ret_val);
}


   /* The following function is responsible for sending a Measurement   */
   /* notification to a specified remote device.  The first parameter is*/
   /* the Bluetooth Stack ID of the Bluetooth Device.  The second       */
   /* parameter is the InstanceID returned from a successful call to    */
   /* CSCS_Initialize_Server().  The third parameter is the ConnectionID*/
   /* of the remote device to send the notification to.  The final      */
   /* parameter is the measurement data to notify.  This function  		*/
   /* returns a zero if successful or a negative return error code if an*/
   /* error occurs.                                                     */
   /* * Note *                                                          */
   /* Mandatory: At least one flag needs to be set in order to send the */
   /* Notification, otherwise an error will be returend to the caller.  */
   /* When flag WHEEL_REVOLUTION_DATA_PRESENT is set in the flags of    */
   /* the CSCS_Measurements parameter, then Cumulative Wheel Revolutions*/
   /* and Last Wheel Event Time fields are present                      */
   /* When flag CRANK_REVOLUTION_DATA_PRESENT is set in the flags of    */
   /* the CSCS_Measurements parameter, then Cumulative Crank Revolutions*/
   /* and Last Crank Event Time fields are present                      */
int BTPSAPI CSCS_Measurements_Notification(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int ConnectionID, CSCS_Measurements_Data_t *CSCS_Measurement)
{
   int                           	ret_val;
   CSCSServerInstance_t          	*ServiceInstance;
   Byte_t                           *NotificationData;
   unsigned int                     Notification_Data_Size;
   Word_t                           SupportedFeatures;

   /* Make sure the parameters passed to us are semi-valid.            	*/
   if((BluetoothStackID) && (InstanceID) && (ConnectionID) && (CSCS_Measurement))
   {
      /* Acquire the specified CSCS Instance.                           */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         SupportedFeatures = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(InstanceData[(InstanceID)-1].CSC_Features));	 
         Notification_Data_Size = CSCS_MEASUREMENTS_NOTIFICATION_DATA_SIZE;
#if (BTPS_CONFIGURATION_CSCS_SUPPORTED_WHEEL_REVOLUTION_DATA)
         if (CSCS_Measurement->Flags & SupportedFeatures & CSCS_CSC_FEATURE_BIT_MASK_WHEEL_REVOLUTION_DATA_SUPPORTED)
         {
             Notification_Data_Size += CSCS_WHEEL_VALUE_SIZE;
         }
#endif

#if (BTPS_CONFIGURATION_CSCS_SUPPORTED_CRANK_REVOLUTION_DATA)
         if (CSCS_Measurement->Flags & SupportedFeatures & CSCS_CSC_FEATURE_BIT_MASK_CRANK_REVOLUTION_DATA_SUPPORTED)
         {
             Notification_Data_Size += CSCS_CRANK_VALUE_SIZE;
         }
#endif

         /* Allocate a buffer for the notification.                     */
         if((NotificationData = BTPS_AllocateMemory(Notification_Data_Size)) != NULL)
         {
			/* Format the notification.                                 */
            if((ret_val = FormatCycleSpeedandCadenceMeasurements(InstanceID, Notification_Data_Size, NotificationData, CSCS_Measurement)) == 0)
            {
      			/* Attempt to send the notification.                    */
				ret_val = GATT_Handle_Value_Notification(ServiceInstance->BluetoothStackID, ServiceInstance->ServiceID, ConnectionID, CSCS_MEASUREMENT_CHARACTERISTIC_OFFSET, Notification_Data_Size, (Byte_t *)NotificationData);
				if(ret_val > 0)
				{
				   ret_val = 0;
				}

				/* Free the memory allocated for the Notification Data. */
				BTPS_FreeMemory(NotificationData);
			}
			else
			{
				ret_val = CSCS_ERROR_MALFORMATTED_DATA;
			}
         }
         else
         {
            ret_val = CSCS_ERROR_INSUFFICIENT_RESOURCES;
         }

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
      {
         ret_val = CSCS_ERROR_INVALID_INSTANCE_ID;
      }
   }
   else
   {
      ret_val = CSCS_ERROR_INVALID_PARAMETER;
   }

   /* Finally return the result to the caller.                          */
   return(ret_val);
}		
 

   /* The following function is responsible for setting Supported      	*/
   /* features on the specified CSCS Instance.  The first parameter is  */
   /* the Bluetooth Stack ID of the Bluetooth Device.  The second  		*/
   /* parameter is the InstanceID returned from a successful call to 	*/
   /* CSCS_Initialize_Server(). The final parameter is the Supported	*/
   /* Feature bit mask to set the supported Features for the specified  */
   /* CSCS Instance.  This function returns a zero if successful or a   */
   /* negative return error code if an error occurs.	                */
   /* * NOTE * The SupportedFeaturesMask is a bit mask that is made up	*/
   /*          of bit masks of the form CSCS_CSC_FEATURE_BIT_MASK_XXX.  */
int BTPSAPI CSCS_Set_Supported_Features(unsigned int BluetoothStackID, unsigned int InstanceID, Word_t SupportedFeaturesMask)
{
   int                   ret_val;
   CSCSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID))  
   {
      if ((SupportedFeaturesMask) && (CSCS_SENSOR_FEATURES_VALID_TYPE(SupportedFeaturesMask)))
      {
          /* Make sure that Only Supported Features are passed          */                             
          if (SupportedFeaturesMask &  ((BTPS_CONFIGURATION_CSCS_SUPPORTED_WHEEL_REVOLUTION_DATA) |
                                       (BTPS_CONFIGURATION_CSCS_SUPPORTED_CRANK_REVOLUTION_DATA << 1) |
                                       (BTPS_CONFIGURATION_CSCS_SUPPORTED_MULTIPLE_SENSOR_LOCATION << 2)))
          {
              /* Acquire the specified CSCS Instance.                           */
              if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
              {
                 /* Acquire the correct Feature Location.                		*/
                 /* The first byte of the Supported Features Mask is a direct 	*/
                 /* pass through of the LSB of the SupportedFeaturesMask      	*/
                 /* parameter.                                                	*/
                 ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(InstanceData[InstanceID-1].CSC_Features), SupportedFeaturesMask);
                 /* Return success to the caller.                               */
                 ret_val = 0;

                 /* UnLock the previously locked Bluetooth Stack.               */
                 BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
              }
              else
              {
                 ret_val = CSCS_ERROR_INVALID_INSTANCE_ID;
              }
          }
          else
          {
            switch (SupportedFeaturesMask &= ~((BTPS_CONFIGURATION_CSCS_SUPPORTED_WHEEL_REVOLUTION_DATA) |
                                              (BTPS_CONFIGURATION_CSCS_SUPPORTED_CRANK_REVOLUTION_DATA << 1) |
                                              (BTPS_CONFIGURATION_CSCS_SUPPORTED_MULTIPLE_SENSOR_LOCATION << 2)))
            {
                case 1:
                    ret_val = CSCS_ERROR_WHEEL_NOT_SUPPORTED;
                    break;
                case 2:
                    ret_val = CSCS_ERROR_CRANK_NOT_SUPPORTED;
                    break;
                case 3:
                    ret_val = CSCS_ERROR_WHEEL_AND_CRANK_NOT_SUPPORTED;
                    break;
                case 4:
                    ret_val = CSCS_ERROR_MULTIPLE_LOCATION_NOT_SUPPORTED;
                    break;
                case 5:
                    ret_val = CSCS_ERROR_WHEEL_AND_MULTIPLE_LOCATION_NOT_SUPPORTED;
                    break;
                case 6:
                    ret_val = CSCS_ERROR_CRANK_AND_MULTIPLE_LOCATION_NOT_SUPPORTED;
                    break;
                case 7:
                    ret_val = CSCS_ERROR_WHEEL_AND_CRANK_AND_MULTIPLE_LOCATION_NOT_SUPPORTED;
                    break;
                default:
                    ret_val = CSCS_ERROR_MALFORMATTED_DATA;
                    break;
            }
          }
      }
      else
      {
         ret_val = CSCS_ERROR_MALFORMATTED_DATA;
      }
   }
   else
   {
      ret_val = CSCS_ERROR_INVALID_PARAMETER;
   }

   /* Finally return the result to the caller.                          */
   return(ret_val);
}


   /* The following function is responsible for querying Supported      */
   /* features on the specified CSCS Instance.  The first parameter is  */
   /* the Bluetooth Stack ID of the Bluetooth Device.  The second  		*/
   /* parameter is the InstanceID returned from a successful call to 	*/
   /* CSCS_Initialize_Server(). The final parameter is a pointer to		*/
   /* store the Feature bit mask for the specified CSCS Instance.	    */
   /* This function returns the BitMask of the supported Features if  	*/
   /* successful or a negative return error code if an error occurs.    */
   /* * NOTE * The Features is a pointer to a bit mask that will  	    */
   /*          be made up of bit masks of the form                      */
   /*          CSCS_CSC_FEATURE_BIT_MASK_XXX, if this function returns  */
   /*          success.                                                 */
int BTPSAPI CSCS_Query_Supported_Features(unsigned int BluetoothStackID, unsigned int InstanceID, Word_t *Features)
{
   int                   ret_val = 0;
   CSCSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.				*/
   if((BluetoothStackID) && (InstanceID) && (Features))
   {
      /* Acquire the specified CSCS Instance.							*/
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Extract the Supported Features value.						*/
         *Features = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(InstanceData[InstanceID-1].CSC_Features));
         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
      {
         ret_val = CSCS_ERROR_INVALID_INSTANCE_ID;
      }
   }
   else
   {
      ret_val = CSCS_ERROR_INVALID_PARAMETER;
   }

   /* Finally return the result to the caller.                          */
   return(ret_val);
}


   /* The following function is responsible for sending a SC Control    */
   /* Point indication to a specified remote device.  The first         */
   /* is the Bluetooth Stack ID of the Bluetooth Device.  The second    */
   /* parameter is the InstanceID returned from a successful call to    */
   /* CSCS_Initialize_Server().  The third parameter is the ConnectionID*/
   /* of the remote device to send the indication to. The fourth        */
   /* parameter is the Op_Code_Response, the structure who store the    */
   /* data for the indication.                                          */              
   /* This function returns a zero if successful or a negative return   */
   /* error code if an error occurs.                                    */
int BTPSAPI CSCS_SC_Control_Point_Indication(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int ConnectionID, CSCS_Control_Point_Data_t *Op_Code_Response)
{
#if (BTPS_CONFIGURATION_CSCS_SUPPORTED_SC_CONTROL_POINT)                               
   int                   ret_val = 0;
   Byte_t               *IndicationData;
   unsigned int          IndicationDataLength;
   CSCSServerInstance_t *ServiceInstance;
   Word_t                Supportfeatures;



   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (ConnectionID) && (Op_Code_Response))
   {
      /* Acquire the specified CSCS Instance.                           */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Verify that no CS Control Point Indication is outstanding.  */
         if((ServiceInstance->CSControlPointInfo.ConnectionID) == 0)
         {
            Supportfeatures = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(InstanceData[InstanceID-1].CSC_Features));
            if (IS_SC_CONTROL_POINT_SUPPORTED(Supportfeatures))
            {
                    /* Calculate the required length for the CS Control Point   */
                    /* Sensor list.                                             */
                if ((Op_Code_Response->Op_Code == 
                    CSCS_CONTROL_POINT_OP_CODE_REQUEST_SUPPORTED_SENSOR_LOCATION)
                    && IS_MULTIPLE_SENSOR_LOCATION_SUPPORTED(Supportfeatures)
                    && (Op_Code_Response->Command_data_Buffer.Indication.Response_Value == 
                    CSCS_CONTROL_POINT_RESPONSE_SUCCESS))
                {
                    IndicationDataLength = CSCS_CONTROL_POINT_INDICATION_LIST_VALUE_SIZE(SensorLocationTableLength);
                }
                else
                {
                    /* Calculate the required length for the CS Control Point   */
                    /* Response.                                                */
                    IndicationDataLength = CSCS_CONTROL_POINT_RESPONSE_VALUE_SIZE;
                }
                if ((Op_Code_Response->Command_data_Buffer.Indication.Response_Value != CSCS_CONTROL_POINT_RESPONSE_OP_CODE_NOT_SUPPORTED) &&
                   ((Op_Code_Response->Op_Code == CSCS_CONTROL_POINT_OP_CODE_SET_CUMULATIVE_VALUE) && 
                     (IS_WHEEL_REVOLUTION_DATA_SUPPORTED(Supportfeatures))))
                {
                    Op_Code_Response->Command_data_Buffer.Indication.Response_Value = CSCS_CONTROL_POINT_RESPONSE_SUCCESS;
                }
                /* Allocate a buffer for the Indication.                        */
                if(((IndicationData = BTPS_AllocateMemory(IndicationDataLength)) != NULL) && (ret_val == 0))
                {
                    BTPS_MemInitialize(IndicationData, 0, IndicationDataLength);
                    ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE
                        (&(((CSCS_Control_Point_Value_t *)IndicationData)->Op_Code), 
                        (Byte_t)CSCS_CONTROL_POINT_OP_CODE_RESPONSE_CODE);
                    ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE
                        (&(((CSCS_Control_Point_Value_t *)IndicationData)->Command_Data_Buffer.Indication.Request_Op_Code), 
                        Op_Code_Response->Op_Code);
                    ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE
                        (&(((CSCS_Control_Point_Value_t *)IndicationData)->Command_Data_Buffer.Indication.Response_Value), 
                        Op_Code_Response->Command_data_Buffer.Indication.Response_Value);
                    if (IndicationDataLength != CSCS_CONTROL_POINT_RESPONSE_VALUE_SIZE)
                    {
                        /* Format the notification.                              */ 
                       ret_val = FormatSensorLocationList(IndicationDataLength, IndicationData, Op_Code_Response);
                    }
                    /* Attempt to send the Indication.                           */
                    if (ret_val == 0)
                    {
                        ret_val = GATT_Handle_Value_Indication(ServiceInstance->BluetoothStackID, ServiceInstance->ServiceID, ConnectionID, CSCS_SC_CONTROL_POINT_CHARACTERISTIC_OFFSET, (Word_t)IndicationDataLength, (Byte_t *)IndicationData);
                    }
                    if(ret_val > 0)
                    {
                         /* Save the Transaction ID of the Indication.      */
                         ServiceInstance->CSControlPointInfo.ConnectionID  = ConnectionID;
                         ServiceInstance->CSControlPointInfo.TransactionID = (unsigned int)ret_val;
                         ret_val                                           = 0;
                    }    
                   /* Free the memory allocated for the Indication Data.    */
                   BTPS_FreeMemory(IndicationData);
                }
                else
                {
                   ret_val = CSCS_ERROR_INSUFFICIENT_RESOURCES;
                }
            }
            else
            {
                ret_val = CSCS_ERROR_WHEEL_AND_MULTIPLE_LOCATION_NOT_SUPPORTED;
            }
         }
         else
         {
            ret_val = CSCS_ERROR_INDICATION_IN_PROGRESS;
         }

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);

      }
      else
      {
         ret_val = CSCS_ERROR_INVALID_INSTANCE_ID;
      }
   }
   else
   {
      ret_val = CSCS_ERROR_INVALID_PARAMETER;
   }
   /* Finally return the result to the caller.                          */
   return(ret_val);
#else
    return (BTPS_ERROR_FEATURE_NOT_AVAILABLE);
#endif
}


   /* The following function is responsible for setting the supported 	*/
   /* sensor location list on the Sensor.  The first parameter is the   */
   /* Bluetooth Stack ID of the Bluetooth Device.  The second parameter */
   /* is the InstanceID returned from a successful call to 	            */
   /* CSCS_Initialize_Server(). The final parameter is the Supported	*/
   /* sensor location list bit mask to set on the sensor.               */              
   /* This function returns a zero if successful or a                   */  
   /* negative return error code if an error occurs.	                */
   /* * NOTE * The SensorListBitMask is a bit mask that is made up	    */
   /*          of bit masks that needs to be smaller then 0x7fff.       */
int BTPSAPI CSCS_Set_Sensor_Location_List(unsigned int BluetoothStackID, unsigned int InstanceID, Word_t SensorListBitMask)
{
#if (BTPS_CONFIGURATION_CSCS_SUPPORTED_SC_CONTROL_POINT)                               
   int      ret_val = 0;
   Word_t   Supportfeatures;
   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (CSCS_SENSOR_LOCATION_BITMASK_VALID_TYPE(SensorListBitMask)))  
   {
       Supportfeatures = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(InstanceData[InstanceID-1].CSC_Features));
       if (IS_MULTIPLE_SENSOR_LOCATION_SUPPORTED(Supportfeatures))
       {
           SensorLocationTableLength = 0;
           SensorLocationBitMaskTable = SensorListBitMask;

           /* This while will retreive us the some of the ones in the  */
           /* Bit mask, which is the length of the Sensor location list*/          
           while (SensorListBitMask > 0)
           {
              if (SensorListBitMask & 0x01)
              {
                  SensorLocationTableLength++;
              }
              SensorListBitMask = SensorListBitMask >> 1;
           }
       }
       else
       {
           ret_val = CSCS_ERROR_CRANK_AND_MULTIPLE_LOCATION_NOT_SUPPORTED;
       }

       
   }
   else
   {
      ret_val = CSCS_ERROR_INVALID_PARAMETER;
   }
   /* Finally return the result to the caller.                          */
   return(ret_val);
#else
   return (BTPS_ERROR_FEATURE_NOT_AVAILABLE);
#endif
}
   


/************************* CSCS Client API Utils function *****************************/

   /* The following function is responsible for formatting a Cycling    */
   /* Speed and Cadence SC Control Point Command into a user specified  */
   /* buffer. The first two parameters contain the length of the buffer,*/
   /* and the buffer,to format the command into. The final parameter    */
   /* is the command to format. This function returns a zero if         */
   /* successful or a negative return error code if an error occurs.    */
   /* * NOTE * The BufferLength and Buffer parameter must point to a    */
   /*          buffer of at least CSCS_CONTROL_POINT_MINIMUM_VALUE_SIZE */
   /*          in size.                                                 */        
int BTPSAPI CSCS_Format_Control_Point_Command_Util(unsigned int BufferLength, Byte_t *Buffer, CSCS_Control_Point_Data_t *CSCS_Control_Point)
{
#if (BTPS_CONFIGURATION_CSCS_SUPPORTED_SC_CONTROL_POINT)                               
   int     ret_val = 0;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((CSCS_Control_Point) && (BufferLength) && (Buffer))
   {
		/* Verify that the buffer is big enough to hold the list.  		*/
		if(BufferLength >= CSCS_CONTROL_POINT_MINIMUM_VALUE_SIZE)		
		{

			ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(((CSCS_Control_Point_Value_t *)Buffer)->Op_Code), CSCS_Control_Point->Op_Code);
		
			switch (CSCS_Control_Point->Op_Code)
			{
				case CSCS_CONTROL_POINT_OP_CODE_SET_CUMULATIVE_VALUE:
						ASSIGN_HOST_DWORD_TO_LITTLE_ENDIAN_UNALIGNED_DWORD(&(((CSCS_Control_Point_Value_t *)Buffer)->Command_Data_Buffer.Cumulative_Value), CSCS_Control_Point->Command_data_Buffer.Cumulative_Value);
						break; 
				case CSCS_CONTROL_POINT_OP_CODE_UPDATE_SENSOR_LOCATION:
						ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(((CSCS_Control_Point_Value_t *)Buffer)->Command_Data_Buffer.Sensor_Location_Value), CSCS_Control_Point->Command_data_Buffer.Sensor_Location_Value);
						break;
				case CSCS_CONTROL_POINT_OP_CODE_REQUEST_SUPPORTED_SENSOR_LOCATION:
                        break;
				default:
						break;
			}
		}
		else
		{
			ret_val = CSCS_ERROR_MALFORMATTED_DATA;
		}
	}
	else
	{
		ret_val = CSCS_ERROR_INVALID_PARAMETER;
	}

	return(ret_val);
#else
    return (BTPS_ERROR_FEATURE_NOT_AVAILABLE);
#endif
}


   /* The following function is responsible for parsing a measurement   */
   /* notification received from a remote CSCS Server.  The first       */
   /* parameter is the length of the value returned by the remote CSCS  */
   /* Server.  The second parameter is a pointer to the data returned by*/
   /* the remote CSCS Server.  This function returns a pointer to the   */
   /* decode measurement data or NULL if an error occurred.             */
int BTPSAPI CSCS_Decode_Cycle_Speed_and_Cadence_Measurements_Util(unsigned int ValueLength, Byte_t *Value, CSCS_Measurements_Data_t *CSCS_Measurement)
{
   int          ret_val;
   Byte_t      *tempPtr;

   /* Verify that the input parameters appear semi-valid.               */
   if((ValueLength) && (Value) && (CSCS_Measurement))
   {
		/* Verify that the Temperature Measurement is a valid length.   */
		if(ValueLength >= (BYTE_SIZE + CSCS_CRANK_VALUE_SIZE))
		{
            tempPtr = Value;
			/* Read the Flags from the Temperature Measurement.         */
 			CSCS_Measurement->Flags	= READ_UNALIGNED_BYTE_LITTLE_ENDIAN(tempPtr);
            tempPtr += CSCS_MEASUREMENTS_NOTIFICATION_DATA_SIZE;
			/* Verify that the received data holds at least the expected*/
			/* data.                                                  	*/		 
			if (CSCS_DATA_PRESENT_VALID(CSCS_Measurement->Flags) || (CSCS_Measurement->Flags == 0))
			{
				/* Initialize the return value to success.              */
				ret_val = 0;

#if (BTPS_CONFIGURATION_CSCS_SUPPORTED_WHEEL_REVOLUTION_DATA)	
				/* Checking if Wheel Data flag is set					*/
				if(CSCS_Measurement->Flags & CSCS_CYCLING_SPEED_AND_CADENCE_MEASUREMENT_FLAGS_WHEEL_REVOLUTION_DATA_PRESENT)
				{
					CSCS_Measurement->Wheel_Data->Cumulative_Wheel_Revolutions = 	READ_UNALIGNED_DWORD_LITTLE_ENDIAN(tempPtr);
                    tempPtr += DWORD_SIZE;
					CSCS_Measurement->Wheel_Data->Last_Wheel_Event_Time        =    READ_UNALIGNED_WORD_LITTLE_ENDIAN(tempPtr);
                    tempPtr += WORD_SIZE;
				}
#endif
#if (BTPS_CONFIGURATION_CSCS_SUPPORTED_CRANK_REVOLUTION_DATA)
				/* Checking if Crank Data flag is set					*/
				if(CSCS_Measurement->Flags & CSCS_CYCLING_SPEED_AND_CADENCE_MEASUREMENT_FLAGS_CRANK_REVOLUTION_DATA_PRESENT)
				{
					CSCS_Measurement->Crank_Data->Cumulative_Crank_Revolutions =    READ_UNALIGNED_WORD_LITTLE_ENDIAN(tempPtr);
                    tempPtr += WORD_SIZE;
					CSCS_Measurement->Crank_Data->Last_Crank_Event_Time        =    READ_UNALIGNED_WORD_LITTLE_ENDIAN(tempPtr);
                    tempPtr += WORD_SIZE;                    
				}
#endif
			}
            else
            {
                ret_val = CSCS_ERROR_MALFORMATTED_DATA;
            }
		}
		else
		{
			ret_val = CSCS_ERROR_MALFORMATTED_DATA;
		}
	}
	else
	{
		ret_val = CSCS_ERROR_INVALID_PARAMETER;
	}
   return(ret_val);
}  


   /* The following function is responsible for parsing a value received*/
   /* from a remote CSCS Server interpreting it as a SC Control point   */
   /* response characteristic.  The first parameter is the length of    */
   /* the value returned by the remote CSCS Server.  The second         */
   /* parameter is a pointer to the data returned by the remote CSCS    */
   /* Server.  The final parameter is a pointer to store the parsed     */
   /* Response value.  This function returns a zero if successful or    */
   /* a negative return error code if an error occurs.                  */
   /* * NOTE * On INPUT the Number_Of_Parameters member of the          */
   /*          CSCS_Control_Point parameter must contain number of      */
   /*          entries if it is greater then one, and the               */
   /*          Request_Op_Code is REQUEST_SUPPORTED_SENSOR_LOCATION, it */
   /*          represent the size of the Response_Parameter array.  On  */
   /*          RETURN this will contain the Op code, value, and the     */
   /*          actual number of Sensor locations available if asked.    */
int BTPSAPI CSCS_Decode_SC_Control_Point_Indication_Util(Word_t ValueLength, Byte_t *Value, CSCS_Control_Point_Data_t *CSCS_Control_Point, Byte_t * LocationList)
{
#if (BTPS_CONFIGURATION_CSCS_SUPPORTED_SC_CONTROL_POINT)                               
   int           ret_val = 0;
   Byte_t       *tempPtr;
   Word_t        Index;

	/* Make sure the parameters passed to us are semi-valid.            */
	if((CSCS_Control_Point) && (ValueLength) && (Value))
	{
	
		/* Verify that the Value buffer is big enough to hold Correct.  */
		/* Values														*/
		if(ValueLength >= CSCS_CONTROL_POINT_RESPONSE_VALUE_SIZE)		
		{
			/* Read the Op_Code Response and Value from the Value Buffer*/
			CSCS_Control_Point->Op_Code = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(((CSCS_Control_Point_Value_t *)Value)->Op_Code));
    		CSCS_Control_Point->Command_data_Buffer.Indication.Request_Op_Code = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(((CSCS_Control_Point_Value_t *)Value)->Command_Data_Buffer.Indication.Request_Op_Code));
    		CSCS_Control_Point->Command_data_Buffer.Indication.Response_Value = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(((CSCS_Control_Point_Value_t *)Value)->Command_Data_Buffer.Indication.Response_Value));

            if (CSCS_Control_Point->Command_data_Buffer.Indication.Request_Op_Code == CSCS_CONTROL_POINT_OP_CODE_REQUEST_SUPPORTED_SENSOR_LOCATION)
            {
			    CSCS_Control_Point->Command_data_Buffer.Indication.Number_Of_Parameters = ValueLength - CSCS_CONTROL_POINT_RESPONSE_VALUE_SIZE;
    			tempPtr = ((CSCS_Control_Point_Value_t *)Value)->Command_Data_Buffer.Indication.Response_Parameter;
		
    			/* Read The Number of parameters from the Value buffer		*/
    			Index = 0;
                if (CSCS_Control_Point->Command_data_Buffer.Indication.Number_Of_Parameters >= CSCS_SENSOR_LOCATION_MAX_VALID_LOCATIONS)
                {
                    CSCS_Control_Point->Command_data_Buffer.Indication.Number_Of_Parameters = 0;
                    ret_val = CSCS_ERROR_LOCATION_LIST_IS_GREATER_THEN_MAXIMUM_SIZE;
                }
                    
    			while ((Index < CSCS_Control_Point->Command_data_Buffer.Indication.Number_Of_Parameters))
    			{
    			/* Read the next Sensor Location from the Value Buffer.     */
    				LocationList[Index] = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(tempPtr);
    				tempPtr += CSCS_SENSOR_LOCATION_VALUE_LENGTH;
    			/* Increment the index.                                     	*/
    				Index++;
    			}
            }
		}
		else
		{
			ret_val = CSCS_ERROR_MALFORMATTED_DATA;
		}
	}
	else
	{
		ret_val = CSCS_ERROR_INVALID_PARAMETER;
	}

	return(ret_val);
#else
    return (BTPS_ERROR_FEATURE_NOT_AVAILABLE);
#endif    
}





