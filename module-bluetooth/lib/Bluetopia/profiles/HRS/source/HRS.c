/*****< hrs.c >****************************************************************/
/*      Copyright 2011 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  HRS - Bluetooth Stack Heart Rate Service (GATT Based) for Stonestreet     */
/*        One Bluetooth Protocol Stack.                                       */
/*                                                                            */
/*  Author:  Tim Cook                                                         */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   11/28/11  T. Cook        Initial creation.                               */
/******************************************************************************/
#include "SS1BTPS.h"        /* Bluetooth Stack API Prototypes/Constants.      */
#include "SS1BTGAT.h"       /* Bluetooth Stack GATT API Prototypes/Constants. */
#include "SS1BTHRS.h"       /* Bluetooth HRS API Prototypes/Constants.        */

#include "BTPSKRNL.h"       /* BTPS Kernel Prototypes/Constants.              */
#include "HRS.h"            /* Bluetooth HRS Prototypes/Constants.            */

   /* The following controls the number of supported HRS instances.     */
#define HRS_MAXIMUM_SUPPORTED_INSTANCES                 (BTPS_CONFIGURATION_HRS_MAXIMUM_SUPPORTED_INSTANCES)

#if BTPS_CONFIGURATION_HRS_SUPPORT_BODY_SENSOR_LOCATION

   /* The following defines the HRS Instance Data, that contains data   */
   /* is unique for each HRS Service Instance.                          */
typedef __PACKED_STRUCT_BEGIN__ struct _tagHRS_Instance_Data_t
{
   NonAlignedWord_t Body_Sensor_Location_Length;
   NonAlignedByte_t Body_Sensor_Location;
} __PACKED_STRUCT_END__ HRS_Instance_Data_t;

#define HRS_INSTANCE_DATA_SIZE                           (sizeof(HRS_Instance_Data_t))

#define HRS_BODY_SENSOR_LOCATION_INSTANCE_TAG            (BTPS_STRUCTURE_OFFSET(HRS_Instance_Data_t, Body_Sensor_Location_Length))

#endif

   /*********************************************************************/
   /**                    Heart Rate Service Table                     **/
   /*********************************************************************/

   /* The Heart Rate Service Declaration UUID.                          */
static BTPSCONST GATT_Primary_Service_16_Entry_t HRS_Service_UUID =
{
   HRS_SERVICE_BLUETOOTH_UUID_CONSTANT
};

   /* The Heart Rate Measurement Characteristic Declaration.            */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t HRS_Heart_Rate_Measurement_Declaration =
{
   GATT_CHARACTERISTIC_PROPERTIES_NOTIFY,
   HRS_HEART_RATE_MEASUREMENT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The Heart Rate Measurement Characteristic Value.                  */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t  HRS_Heart_Rate_Measurement_Value =
{
   HRS_HEART_RATE_MEASUREMENT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   0,
   NULL
};

   /* Client Characteristic Configuration Descriptor.                   */
static GATT_Characteristic_Descriptor_16_Entry_t Client_Characteristic_Configuration =
{
   GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_BLUETOOTH_UUID_CONSTANT,
   GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_LENGTH,
   NULL
};

#if BTPS_CONFIGURATION_HRS_SUPPORT_BODY_SENSOR_LOCATION

   /* The Body Sensor Location Characteristic Declaration.              */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t HRS_Body_Sensor_Location_Declaration =
{
   GATT_CHARACTERISTIC_PROPERTIES_READ,
   HRS_BODY_SENSOR_LOCATION_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The Body Sensor Location Characteristic Value.                    */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t  HRS_Body_Sensor_Location_Value =
{
   HRS_BODY_SENSOR_LOCATION_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   HRS_BODY_SENSOR_LOCATION_INSTANCE_TAG,
   NULL
};

#endif

#if BTPS_CONFIGURATION_HRS_SUPPORT_ENERGY_EXPENDED_STATUS

   /* The Heart Rate Control Point Characteristic Declaration.          */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t HRS_Heart_Rate_Control_Point_Declaration =
{
   GATT_CHARACTERISTIC_PROPERTIES_WRITE,
   HRS_HEART_RATE_CONTROL_POINT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The Heart Rate Control Point Characteristic Value.                */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t  HRS_Heart_Rate_Control_Point_Value =
{
   HRS_HEART_RATE_CONTROL_POINT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   0,
   NULL
};

#endif

   /* The following defines the Heart Rate service that is registered   */
   /* with the GATT_Register_Service function call.                     */
   /* * NOTE * This array will be registered with GATT in the call to   */
   /*          GATT_Register_Service.                                   */
BTPSCONST GATT_Service_Attribute_Entry_t Heart_Rate_Service[] =
{
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetPrimaryService16,            (Byte_t *)&HRS_Service_UUID},
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetCharacteristicDeclaration16, (Byte_t *)&HRS_Heart_Rate_Measurement_Declaration},
   {0,                                      aetCharacteristicValue16,       (Byte_t *)&HRS_Heart_Rate_Measurement_Value},
   {GATT_ATTRIBUTE_FLAGS_READABLE_WRITABLE, aetCharacteristicDescriptor16,  (Byte_t *)&Client_Characteristic_Configuration},

#if BTPS_CONFIGURATION_HRS_SUPPORT_BODY_SENSOR_LOCATION

   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetCharacteristicDeclaration16, (Byte_t *)&HRS_Body_Sensor_Location_Declaration},
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetCharacteristicValue16,       (Byte_t *)&HRS_Body_Sensor_Location_Value},

#endif

#if BTPS_CONFIGURATION_HRS_SUPPORT_ENERGY_EXPENDED_STATUS

   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetCharacteristicDeclaration16, (Byte_t *)&HRS_Heart_Rate_Control_Point_Declaration},
   {GATT_ATTRIBUTE_FLAGS_WRITABLE,          aetCharacteristicValue16,       (Byte_t *)&HRS_Heart_Rate_Control_Point_Value},

#endif

};

#define HEART_RATE_SERVICE_ATTRIBUTE_COUNT               (sizeof(Heart_Rate_Service)/sizeof(GATT_Service_Attribute_Entry_t))

#define HRS_HEART_RATE_MEASUREMENT_ATTRIBUTE_OFFSET      2

   /*********************************************************************/
   /**                    END OF SERVICE TABLE                         **/
   /*********************************************************************/

   /* The following type defines a union large enough to hold all events*/
   /* dispatched by this module.                                        */
typedef union
{
   HRS_Read_Client_Configuration_Data_t   Read_Client_Data;
   HRS_Client_Configuration_Update_Data_t Client_Configuration_Update_Data;
   HRS_Heart_Rate_Control_Command_Data_t  HRS_Heart_Rate_Control_Command_Data;
} HRS_Event_Data_Buffer_t;

#define HRS_EVENT_DATA_BUFFER_SIZE                       (sizeof(HRS_Event_Data_Buffer_t))

   /* HRS Service Instance Block.  This structure contains All          */
   /* information associated with a specific Bluetooth Stack ID (member */
   /* is present in this structure).                                    */
typedef struct _tagHRSServerInstance_t
{
   unsigned int         BluetoothStackID;
   unsigned int         ServiceID;
   HRS_Event_Callback_t EventCallback;
   unsigned long        CallbackParameter;
   unsigned long        Supported_Commands;
} HRSServerInstance_t;

#define HRS_SERVER_INSTANCE_DATA_SIZE                    (sizeof(HRSServerInstance_t))

   /* Internal Variables to this Module (Remember that all variables    */
   /* declared static are initialized to 0 automatically by the         */
   /* compiler as part of standard C/C++).                              */

#if BTPS_CONFIGURATION_HRS_SUPPORT_BODY_SENSOR_LOCATION

static HRS_Instance_Data_t InstanceData[HRS_MAXIMUM_SUPPORTED_INSTANCES];
                                            /* Variable which holds all */
                                            /* data that is unique for  */
                                            /* each service instance.   */

#endif

static HRSServerInstance_t InstanceList[HRS_MAXIMUM_SUPPORTED_INSTANCES];
                                            /* Variable which holds the */
                                            /* service instance data.   */

static Boolean_t InstanceListInitialized;   /* Variable that flags that */
                                            /* is used to denote that   */
                                            /* this module has been     */
                                            /* successfully initialized.*/

   /* The following are the prototypes of local functions.              */
static Boolean_t InitializeModule(void);
static void CleanupModule(void);

static unsigned int CalculateMaximumSupportedIntervals(HRS_Heart_Rate_Measurement_Data_t *Heart_Rate_Measurement, Word_t MTU);
static int FormatHeartRateMeasurement(HRS_Heart_Rate_Measurement_Data_t *Heart_Rate_Measurement, unsigned int BufferLength, Byte_t *Buffer);
static int DecodeHeartRateMeasurement(unsigned int BufferLength, Byte_t *Value, HRS_Heart_Rate_Measurement_Data_t *Temperature_Measurement);
static int DecodeBodySensorLocation(unsigned int BufferLength, Byte_t *Value, Byte_t *BodySensorLocation);
static int DecodeClientConfigurationValue(unsigned int BufferLength, Byte_t *Buffer, Word_t *ClientConfiguration);

#if BTPS_CONFIGURATION_HRS_SUPPORT_ENERGY_EXPENDED_STATUS

static int DecodeHeartRateControlCommand(unsigned int BufferLength, Byte_t *Buffer, HRS_Heart_Rate_Control_Command_t *Command);
static Boolean_t HeartRateControlCommandSupported(HRSServerInstance_t *HRSServerInstance, HRS_Heart_Rate_Control_Command_t Command);

#endif

static HRS_Event_Data_t *FormatEventHeader(unsigned int BufferLength, Byte_t *Buffer, HRS_Event_Type_t EventType, unsigned int InstanceID, unsigned int ConnectionID, unsigned int *TransactionID, GATT_Connection_Type_t ConnectionType, BD_ADDR_t *BD_ADDR);

static Boolean_t InstanceRegisteredByStackID(unsigned int BluetoothStackID);
static HRSServerInstance_t *AcquireServiceInstance(unsigned int BluetoothStackID, unsigned int *InstanceID);

static int HRSRegisterService(unsigned int BluetoothStackID, unsigned long Supported_Commands, HRS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange);

   /* Bluetooth Event Callbacks.                                        */
static void BTPSAPI GATT_ServerEventCallback(unsigned int BluetoothStackID, GATT_Server_Event_Data_t *GATT_ServerEventData, unsigned long CallbackParameter);

   /* The following function is a utility function that is used to      */
   /* reduce the ifdef blocks that are needed to handle the difference  */
   /* between module initialization for Threaded and NonThreaded stacks.*/
static Boolean_t InitializeModule(void)
{
   /* All we need to do is flag that we are initialized.                */
   if(!InstanceListInitialized)
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

   /* The following function is used to calculate the maximum number of */
   /* RR Intervals that may be sent in a notification on a connection   */
   /* with a given MTU, based on the Flags of the Heart Rate            */
   /* Measurement.                                                      */
   /* * NOTE * This is an internal function where no check on the input */
   /*          parameters will be performed.                            */
static unsigned int CalculateMaximumSupportedIntervals(HRS_Heart_Rate_Measurement_Data_t *Heart_Rate_Measurement, Word_t MTU)
{
   Word_t       MaximumNotificationValue;
   Word_t       SpareBytes;
   unsigned int ret_val = 0;

   /* Only continue if the flags specify that we will be sending RR     */
   /* Intervals in this notification.                                   */
   if(Heart_Rate_Measurement->Flags & HRS_HEART_RATE_MEASUREMENT_FLAGS_RR_INTERVAL_PRESENT)
   {
      /* Calculate the maximum length of the characteristic value in a  */
      /* Handle-Value notification.                                     */
      MaximumNotificationValue = (Word_t)(MTU - ATT_HANDLE_VALUE_NOTIFICATION_PDU_SIZE(0));

      /* Calculate the spare bytes available in the notification for RR */
      /* Intervals after all the other space requirements are accounted */
      /* for.                                                           */
      SpareBytes               = (Word_t)(MaximumNotificationValue - HRS_HEART_RATE_MEASUREMENT_LENGTH(Heart_Rate_Measurement->Flags, 0));

      /* The maximum number of RR Intervals is just the spare number of */
      /* bytes divided by the size of an RR Interval.                   */
      ret_val                  = (unsigned int)(SpareBytes / sizeof(Heart_Rate_Measurement->RR_Intervals[0]));
   }

   return(ret_val);
}

   /* The following function is used to format a Heart Rate Measurement */
   /* into a specified buffer.                                          */
static int FormatHeartRateMeasurement(HRS_Heart_Rate_Measurement_Data_t *Heart_Rate_Measurement, unsigned int BufferLength, Byte_t *Buffer)
{
   int     ret_val;
   Byte_t *tempPtr;
   Word_t  Index;

   /* Verify that the input parameters appear semi-valid.               */
   if((Heart_Rate_Measurement) && (BufferLength) && (Buffer))
   {
      /* Verify that the buffer is big enough to hold the measurement.  */
      if(BufferLength >= HRS_HEART_RATE_MEASUREMENT_LENGTH(Heart_Rate_Measurement->Flags, Heart_Rate_Measurement->Number_Of_RR_Intervals))
      {
         /* Copy the flags into the Heart Rate Header.                  */
         ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(((HRS_Heart_Rate_Measurement_Header_t *)Buffer)->Flags), Heart_Rate_Measurement->Flags);

         /* Set a temporary pointer past the header of the Heart Rate   */
         /* measurement.                                                */
         tempPtr = ((HRS_Heart_Rate_Measurement_t *)Buffer)->Variable_Data;

         /* Format the Heart Rate field into the measurement.           */
         if(Heart_Rate_Measurement->Flags & HRS_HEART_RATE_MEASUREMENT_FLAGS_HEART_RATE_IS_WORD)
         {
            ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(tempPtr, Heart_Rate_Measurement->Heart_Rate);
            tempPtr += WORD_SIZE;
         }
         else
         {
            /* By default the Heart Rate is a 1 byte field (unless the  */
            /* flags indicated otherwise).                              */
            ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(tempPtr, Heart_Rate_Measurement->Heart_Rate);
            tempPtr += BYTE_SIZE;
         }

         /* If the Energy Expended Status is present then format this   */
         /* field into the measurement.                                 */
         if(Heart_Rate_Measurement->Flags & HRS_HEART_RATE_MEASUREMENT_FLAGS_ENERGY_EXPENDED_PRESENT)
         {
            ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(tempPtr, Heart_Rate_Measurement->Energy_Expended);
            tempPtr += WORD_SIZE;
         }

         /* Format all of the RR Intervals into the Heart Rate          */
         /* Measurement.                                                */
         Index = 0;
         while(Index < Heart_Rate_Measurement->Number_Of_RR_Intervals)
         {
            /* Assign the next RR Interval into the measurement.        */
            ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(tempPtr, Heart_Rate_Measurement->RR_Intervals[Index]);
            tempPtr += WORD_SIZE;

            /* Increment the index.                                     */
            Index++;
         }

         /* Return success to the caller.                               */
         ret_val = 0;
      }
      else
         ret_val = HRS_ERROR_MALFORMATTED_DATA;
   }
   else
      ret_val = HRS_ERROR_INVALID_PARAMETER;

   return(ret_val);
}

   /* The following function is used to decode a temperature measurement*/
   /* that has been received from a remote Client.                      */
static int DecodeHeartRateMeasurement(unsigned int BufferLength, Byte_t *Value, HRS_Heart_Rate_Measurement_Data_t *Heart_Rate_Measurement)
{
   int           ret_val;
   Byte_t       *tempPtr;
   Word_t        NumberIntervals;
   Word_t        Index;
   unsigned int  ExpectedLength;

   /* Verify that the input parameters appear semi-valid.               */
   if((BufferLength) && (Value) && (Heart_Rate_Measurement))
   {
      /* Verify that the Temperature Measurement is a valid length.     */
      if(BufferLength >= HRS_HEART_RATE_MEASUREMENT_HEADER_DATA_SIZE)
      {
         /* Read the Flags from the Temperature Measurement.            */
         Heart_Rate_Measurement->Flags = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(((HRS_Heart_Rate_Measurement_Header_t *)Value)->Flags));

         /* Calculated the expected length based on the flags.          */
         ExpectedLength                = HRS_HEART_RATE_MEASUREMENT_MINIMUM_LENGTH(Heart_Rate_Measurement->Flags);

         /* Verify that the received data holds at least the expected   */
         /* data size.                                                  */
         if(BufferLength >= ExpectedLength)
         {
            /* Calculate the number of RR Records in the measurement if */
            /* the flags indicate that this field is present.           */
            if(Heart_Rate_Measurement->Flags & HRS_HEART_RATE_MEASUREMENT_FLAGS_RR_INTERVAL_PRESENT)
            {
               /* Calculate the number of RR Intervals that are present */
               /* in the Heart Rate Measurement.                        */
               NumberIntervals = (Word_t)((BufferLength - HRS_HEART_RATE_MEASUREMENT_LENGTH(Heart_Rate_Measurement->Flags, 0))/sizeof(Heart_Rate_Measurement->RR_Intervals[0]));
            }
            else
               NumberIntervals = 0;

            /* Set a pointer into the variable part of the Heart Rate   */
            /* Measurement.                                             */
            tempPtr = ((HRS_Heart_Rate_Measurement_t *)Value)->Variable_Data;

            /* Parse out the Heart Rate value field in the message.     */
            if(Heart_Rate_Measurement->Flags & HRS_HEART_RATE_MEASUREMENT_FLAGS_HEART_RATE_IS_WORD)
            {
               Heart_Rate_Measurement->Heart_Rate = READ_UNALIGNED_WORD_LITTLE_ENDIAN(tempPtr);
               tempPtr += WORD_SIZE;
            }
            else
            {
               Heart_Rate_Measurement->Heart_Rate = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(tempPtr);
               tempPtr += BYTE_SIZE;
            }

            /* Parse out the Energy Expended field if it is present.    */
            if(Heart_Rate_Measurement->Flags & HRS_HEART_RATE_MEASUREMENT_FLAGS_ENERGY_EXPENDED_PRESENT)
            {
               Heart_Rate_Measurement->Energy_Expended = READ_UNALIGNED_WORD_LITTLE_ENDIAN(tempPtr);
               tempPtr += WORD_SIZE;
            }

            /* If there are RR Intervals in the measurement AND we have */
            /* entries in the user specified structure to store the     */
            /* intervals, then go ahead do that now.                    */
            if((NumberIntervals) && (Heart_Rate_Measurement->Number_Of_RR_Intervals))
            {
               Index = 0;
               while((Index < NumberIntervals) && (Index < Heart_Rate_Measurement->Number_Of_RR_Intervals))
               {
                  /* Parse out the RR Interval in the measurement into  */
                  /* an entry in the caller specified structure.        */
                  Heart_Rate_Measurement->RR_Intervals[Index] = READ_UNALIGNED_WORD_LITTLE_ENDIAN(tempPtr);

                  /* Increment the pointer to the next RR Interval in   */
                  /* the measurement.                                   */
                  tempPtr += WORD_SIZE;

                  /* Increment to the next RR Interval.                 */
                  Index++;
               }

               /* Return the total number of RR Interval values that    */
               /* were parsed into the Heart Rate Measurement structure */
               /* to the caller.                                        */
               Heart_Rate_Measurement->Number_Of_RR_Intervals = Index;
            }
            else
            {
               /* No entries were specified in the Heart Rate           */
               /* Measurement structure that the caller passed in.  In  */
               /* this case we will just return the number of RR        */
               /* Intervals that were contained in the measurement.     */
               if(!(Heart_Rate_Measurement->Number_Of_RR_Intervals))
                  Heart_Rate_Measurement->Number_Of_RR_Intervals = NumberIntervals;
            }

            /* Return success to the caller.                            */
            ret_val = 0;
         }
         else
            ret_val = HRS_ERROR_MALFORMATTED_DATA;
      }
      else
         ret_val = HRS_ERROR_MALFORMATTED_DATA;
   }
   else
      ret_val = HRS_ERROR_INVALID_PARAMETER;

   return(ret_val);
}

   /* The following function is a utility function that exists to aid in*/
   /* decoding a value read from a Server, and interpreting it as a Body*/
   /* Sensor Location value.  This function returns the zero if         */
   /* successful or a negative error code.                              */
static int DecodeBodySensorLocation(unsigned int BufferLength, Byte_t *Value, Byte_t *BodySensorLocation)
{
   int ret_val;

   /* Verify that the input parameters appear semi-valid.               */
   if((BufferLength) && (Value) && (BodySensorLocation))
   {
      /* Verify the length of the value is as expected.                 */
      if(BufferLength == HRS_BODY_SENSOR_LOCATION_VALUE_LENGTH)
      {
         /* Parse out the Body Sensor Location out of the value.        */
         *BodySensorLocation = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(Value);

         ret_val             = 0;
      }
      else
         ret_val = HRS_ERROR_MALFORMATTED_DATA;
   }
   else
      ret_val = HRS_ERROR_INVALID_PARAMETER;

   return(ret_val);
}

   /* The following function is a utility function that exists to decode*/
   /* an Client Configuration value into a user specified boolean value.*/
   /* This function returns the zero if successful or a negative error  */
   /* code.                                                             */
static int DecodeClientConfigurationValue(unsigned int BufferLength, Byte_t *Buffer, Word_t *ClientConfiguration)
{
   int ret_val = HRS_ERROR_MALFORMATTED_DATA;

   /* Verify that the input parameters are valid.                       */
   if(((BufferLength == NON_ALIGNED_BYTE_SIZE) || (BufferLength == GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_LENGTH)) && (Buffer) && (ClientConfiguration))
   {
      /* Read the requested Client Configuration.                       */
      if(BufferLength == NON_ALIGNED_BYTE_SIZE)
         *ClientConfiguration = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(Buffer);
      else
         *ClientConfiguration = READ_UNALIGNED_WORD_LITTLE_ENDIAN(Buffer);

      ret_val              = 0;
   }
   else
   {
      if(BufferLength == GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_LENGTH)
         ret_val = HRS_ERROR_INVALID_PARAMETER;
   }

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

#if BTPS_CONFIGURATION_HRS_SUPPORT_ENERGY_EXPENDED_STATUS

   /* The following function is a utility function that exists to decode*/
   /* an Measurement Interval into a user specified value.  This        */
   /* function returns the zero if successful or a negative error code. */
static int DecodeHeartRateControlCommand(unsigned int BufferLength, Byte_t *Buffer, HRS_Heart_Rate_Control_Command_t *Command)
{
   int ret_val = HRS_ERROR_MALFORMATTED_DATA;

   /* Verify that the input parameters are valid.                       */
   if((BufferLength == HRS_HEART_RATE_CONTROL_POINT_VALUE_LENGTH) && (Buffer) && (Command))
   {
      /* Read the requested Client Configuration.                       */
      *Command = (HRS_Heart_Rate_Control_Command_t)READ_UNALIGNED_BYTE_LITTLE_ENDIAN(Buffer);

      ret_val  = 0;
   }
   else
      ret_val = HRS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is used to determine if a Heart Rate       */
   /* Control Command is supported by a specified HRS Server Instance.  */
   /* This function returns TRUE if the Heart Rate Control Command is   */
   /* supported or FALSE otherwise.                                     */
static Boolean_t HeartRateControlCommandSupported(HRSServerInstance_t *HRSServerInstance, HRS_Heart_Rate_Control_Command_t Command)
{
   Boolean_t ret_val = FALSE;

   /* Verify that the input parameters are valid.                       */
   if((HRSServerInstance) && (HRS_HEART_RATE_CONTROL_POINT_VALID_COMMAND(Command)))
   {
      switch(Command)
      {
         case ccResetEnergyExpended:
            if(HRSServerInstance->Supported_Commands & HRS_HEART_RATE_CONTROL_POINT_RESET_ENERGY_EXPENDED_SUPPORTED)
               ret_val = TRUE;
            break;
      }
   }

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

#endif

   /* The following function is a utility function that exists to format*/
   /* a HRS Event into the specified buffer.                            */
   /* * NOTE * TransactionID is optional and may be set to NULL.        */
   /* * NOTE * BD_ADDR is NOT optional and may NOT be set to NULL.      */
static HRS_Event_Data_t *FormatEventHeader(unsigned int BufferLength, Byte_t *Buffer, HRS_Event_Type_t EventType, unsigned int InstanceID, unsigned int ConnectionID, unsigned int *TransactionID, GATT_Connection_Type_t ConnectionType, BD_ADDR_t *BD_ADDR)
{
   HRS_Event_Data_t *EventData = NULL;

   if((BufferLength >= (HRS_EVENT_DATA_SIZE + HRS_EVENT_DATA_BUFFER_SIZE)) && (Buffer) && (BD_ADDR))
   {
      /* Format the header of the event, that is data that is common to */
      /* all events.                                                    */
      BTPS_MemInitialize(Buffer, 0, BufferLength);

      EventData                                                               = (HRS_Event_Data_t *)Buffer;
      EventData->Event_Data_Type                                              = EventType;
      EventData->Event_Data.HRS_Read_Client_Configuration_Data               = (HRS_Read_Client_Configuration_Data_t *)(((Byte_t *)EventData) + HRS_EVENT_DATA_SIZE);

      EventData->Event_Data.HRS_Read_Client_Configuration_Data->InstanceID   = InstanceID;
      EventData->Event_Data.HRS_Read_Client_Configuration_Data->ConnectionID = ConnectionID;

      if(TransactionID)
      {
         EventData->Event_Data.HRS_Read_Client_Configuration_Data->TransactionID  = *TransactionID;
         EventData->Event_Data.HRS_Read_Client_Configuration_Data->ConnectionType = ConnectionType;
         EventData->Event_Data.HRS_Read_Client_Configuration_Data->RemoteDevice   = *BD_ADDR;
      }
      else
      {
         EventData->Event_Data.HRS_Client_Configuration_Update_Data->ConnectionType = ConnectionType;
         EventData->Event_Data.HRS_Client_Configuration_Update_Data->RemoteDevice   = *BD_ADDR;
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

   for(Index=0;Index<HRS_MAXIMUM_SUPPORTED_INSTANCES;Index++)
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
static HRSServerInstance_t *AcquireServiceInstance(unsigned int BluetoothStackID, unsigned int *InstanceID)
{
   unsigned int         LocalInstanceID;
   unsigned int         Index;
   HRSServerInstance_t *ret_val = NULL;

   /* Lock the Bluetooth Stack to gain exclusive access to this         */
   /* Bluetooth Protocol Stack.                                         */
   if(!BSC_LockBluetoothStack(BluetoothStackID))
   {
      /* Acquire the BSC List Lock while we are searching the instance  */
      /* list.                                                          */
      if(BSC_AcquireListLock())
      {
         /* Store a copy of the passed in InstanceID locally.           */
         LocalInstanceID = *InstanceID;

         /* Verify that the Instance ID is valid.                       */
         if((LocalInstanceID) && (LocalInstanceID <= HRS_MAXIMUM_SUPPORTED_INSTANCES))
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
            if(!LocalInstanceID)
            {
               /* Try to find a free instance.                          */
               for(Index=0;Index<HRS_MAXIMUM_SUPPORTED_INSTANCES;Index++)
               {
                  /* Check to see if this instance is being used.       */
                  if(!(InstanceList[Index].ServiceID))
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
      if(!ret_val)
         BSC_UnLockBluetoothStack(BluetoothStackID);
   }

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is a utility function which is used to     */
   /* register an HRS Service.  This function returns the positive,     */
   /* non-zero, Instance ID on success or a negative error code.        */
static int HRSRegisterService(unsigned int BluetoothStackID, unsigned long Supported_Commands, HRS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange)
{
   int                  ret_val;
   unsigned int         InstanceID;
   HRSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (EventCallback) && (ServiceID))
   {
      /* Verify that no instance is registered to this Bluetooth Stack. */
      if(!InstanceRegisteredByStackID(BluetoothStackID))
      {
         /* Acquire a free HRS Instance.                                */
         InstanceID = 0;
         if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
         {
            /* Call GATT to register the HRS service.                   */
            ret_val = GATT_Register_Service(BluetoothStackID, HRS_SERVICE_FLAGS, HEART_RATE_SERVICE_ATTRIBUTE_COUNT, (GATT_Service_Attribute_Entry_t *)Heart_Rate_Service, ServiceHandleRange, GATT_ServerEventCallback, InstanceID);
            if(ret_val > 0)
            {
               /* Save the Instance information.                        */
               ServiceInstance->BluetoothStackID   = BluetoothStackID;
               ServiceInstance->ServiceID          = (unsigned int)ret_val;
               ServiceInstance->Supported_Commands = Supported_Commands;
               ServiceInstance->EventCallback      = EventCallback;
               ServiceInstance->CallbackParameter  = CallbackParameter;
               *ServiceID                          = (unsigned int)ret_val;

#if BTPS_CONFIGURATION_HRS_SUPPORT_BODY_SENSOR_LOCATION

               /* Intilize the Instance Data for this instance.         */
               BTPS_MemInitialize(&InstanceData[InstanceID-1], 0, HRS_INSTANCE_DATA_SIZE);

               ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(InstanceData[InstanceID-1].Body_Sensor_Location_Length), HRS_BODY_SENSOR_LOCATION_VALUE_LENGTH);

#endif

               /* Return the HRS Instance ID.                           */
               ret_val = (int)InstanceID;
            }

            /* UnLock the previously locked Bluetooth Stack.            */
            BSC_UnLockBluetoothStack(BluetoothStackID);
         }
         else
            ret_val = HRS_ERROR_INSUFFICIENT_RESOURCES;
      }
      else
         ret_val = HRS_ERROR_SERVICE_ALREADY_REGISTERED;
   }
   else
      ret_val = HRS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is the GATT Server Event Callback that     */
   /* handles all requests made to the HRS Service for all registered   */
   /* instances.                                                        */
static void BTPSAPI GATT_ServerEventCallback(unsigned int BluetoothStackID, GATT_Server_Event_Data_t *GATT_ServerEventData, unsigned long CallbackParameter)
{
   Word_t               AttributeOffset;

#if BTPS_CONFIGURATION_HRS_SUPPORT_BODY_SENSOR_LOCATION

   Word_t               InstanceTag;

#endif

   Word_t               ValueLength;
   Byte_t              *Value;
   Byte_t               Event_Buffer[HRS_EVENT_DATA_SIZE + HRS_EVENT_DATA_BUFFER_SIZE];
   unsigned int         TransactionID;
   unsigned int         InstanceID;
   HRS_Event_Data_t    *EventData;
   HRSServerInstance_t *ServiceInstance;

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
               /* Verify that the Event Data is valid.                  */
               if(GATT_ServerEventData->Event_Data.GATT_Read_Request_Data)
               {
                  AttributeOffset = GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->AttributeOffset;
                  TransactionID   = GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->TransactionID;

                  /* Verify that they are not trying to write with an   */
                  /* offset or using preprared writes.                  */
                  if(!(GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->AttributeValueOffset))
                  {
                     if(Heart_Rate_Service[AttributeOffset].Attribute_Entry_Type == aetCharacteristicDescriptor16)
                     {
                        EventData = FormatEventHeader(sizeof(Event_Buffer), Event_Buffer, etHRS_Server_Read_Client_Configuration_Request, InstanceID, GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->ConnectionID, &TransactionID, GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->ConnectionType, &(GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->RemoteDevice));
                        if(EventData)
                        {
                           /* Format the rest of the event.             */
                           EventData->Event_Data_Size                                                        = HRS_READ_CLIENT_CONFIGURATION_DATA_SIZE;
                           EventData->Event_Data.HRS_Read_Client_Configuration_Data->ClientConfigurationType = ctHeartReateMeasurement;

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
                           GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR);
                     }

#if BTPS_CONFIGURATION_HRS_SUPPORT_BODY_SENSOR_LOCATION

                     else
                     {
                        /* Get the instance tag for the characteristic. */
                        InstanceTag = (Word_t)(((GATT_Characteristic_Value_16_Entry_t *)Heart_Rate_Service[AttributeOffset].Attribute_Value)->Characteristic_Value_Length);
                        ValueLength = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(((Byte_t *)(&InstanceData[InstanceID-1]))[InstanceTag]));
                        Value       = (Byte_t *)(&(((Byte_t *)(&InstanceData[InstanceID-1]))[InstanceTag + WORD_SIZE]));

                        /* Respond with the data.                       */
                        GATT_Read_Response(BluetoothStackID, TransactionID, (unsigned int)ValueLength, Value);
                     }

#endif

                  }
                  else
                     GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, ATT_PROTOCOL_ERROR_CODE_ATTRIBUTE_NOT_LONG);
               }
               break;
            case etGATT_Server_Write_Request:
               /* Verify that the Event Data is valid.                  */
               if(GATT_ServerEventData->Event_Data.GATT_Write_Request_Data)
               {
                  AttributeOffset = GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->AttributeOffset;
                  TransactionID   = GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->TransactionID;
                  ValueLength     = GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->AttributeValueLength;
                  Value           = GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->AttributeValue;

                  if((!(GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->AttributeValueOffset)) && (!(GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->DelayWrite)))
                  {
                     if(Heart_Rate_Service[AttributeOffset].Attribute_Entry_Type == aetCharacteristicDescriptor16)
                     {
                        /* Begin formatting the Client Configuration    */
                        /* Update event.                                */
                        EventData = FormatEventHeader(sizeof(Event_Buffer), Event_Buffer, etHRS_Server_Client_Configuration_Update, InstanceID, GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->ConnectionID, NULL, GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->ConnectionType, &(GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->RemoteDevice));
                        if(EventData)
                        {
                           /* Format the rest of the event.             */
                           EventData->Event_Data_Size                                                          = HRS_CLIENT_CONFIGURATION_UPDATE_DATA_SIZE;
                           EventData->Event_Data.HRS_Client_Configuration_Update_Data->ClientConfigurationType = ctHeartReateMeasurement;

                           /* Attempt to decode the request Client      */
                           /* Configuration.                            */
                           if(!DecodeClientConfigurationValue(ValueLength, Value, &(EventData->Event_Data.HRS_Client_Configuration_Update_Data->ClientConfiguration)))
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
                              GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, ATT_PROTOCOL_ERROR_CODE_REQUEST_NOT_SUPPORTED);
                        }
                        else
                           GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR);
                     }

#if BTPS_CONFIGURATION_HRS_SUPPORT_ENERGY_EXPENDED_STATUS

                     else
                     {
                        /* Format and Dispatch the event.               */
                        EventData = FormatEventHeader(sizeof(Event_Buffer), Event_Buffer, etHRS_Server_Heart_Rate_Control_Point_Command, InstanceID, GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->ConnectionID, NULL, GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->ConnectionType, &(GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->RemoteDevice));
                        if(EventData)
                        {
                           /* Format the rest of the event.             */
                           EventData->Event_Data_Size = HRS_HEART_RATE_CONTROL_COMMAND_DATA_SIZE;

                           /* Parse out the command.                    */
                           if(!DecodeHeartRateControlCommand(ValueLength, Value, &(EventData->Event_Data.HRS_Heart_Rate_Control_Command_Data->Command)))
                           {
                              /* Verify that the Command is supported.  */
                              if(HeartRateControlCommandSupported(ServiceInstance, EventData->Event_Data.HRS_Heart_Rate_Control_Command_Data->Command))
                              {
                                 /* Since the requested Measurement     */
                                 /* Interval was formatted correctly and*/
                                 /* with the accepted range we will     */
                                 /* accept the new measurement interval.*/
                                 GATT_Write_Response(BluetoothStackID, TransactionID);

                                 /* Dispatch the event.                 */
                                 __BTPSTRY
                                 {
                                    (*ServiceInstance->EventCallback)(ServiceInstance->BluetoothStackID, EventData, ServiceInstance->CallbackParameter);
                                 }
                                 __BTPSEXCEPT(1)
                                 {
                                    /* Do Nothing.                      */
                                 }
                              }
                              else
                                 GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, HRS_ERROR_CODE_CONTROL_POINT_NOT_SUPPORTED);
                           }
                           else
                              GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, HRS_ERROR_CODE_CONTROL_POINT_NOT_SUPPORTED);
                        }
                        else
                           GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR);
                     }

#endif

                  }
                  else
                     GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, ATT_PROTOCOL_ERROR_CODE_REQUEST_NOT_SUPPORTED);
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
   /* Bluetooth Stack HRS Module is Initialized correctly.  This        */
   /* function *MUST* be called before ANY other Bluetooth Stack HRS    */
   /* function can be called.  This function returns non-zero if the    */
   /* Module was initialized correctly, or a zero value if there was an */
   /* error.                                                            */
   /* * NOTE * Internally, this module will make sure that this function*/
   /*          has been called at least once so that the module will    */
   /*          function.  Calling this function from an external        */
   /*          location is not necessary.                               */
int InitializeHRSModule(void)
{
   return((int)InitializeModule());
}

   /* The following function is responsible for instructing the         */
   /* Bluetooth Stack HRSC Module to clean up any resources that it has */
   /* allocated.  Once this function has completed, NO other Bluetooth  */
   /* Stack HRS Functions can be called until a successful call to the  */
   /* InitializeHRSModule() function is made.  The parameter to this    */
   /* function specifies the context in which this function is being    */
   /* called.  If the specified parameter is TRUE, then the module will */
   /* make sure that NO functions that would require waiting/blocking on*/
   /* Mutexes/Events are called.  This parameter would be set to TRUE if*/
   /* this function was called in a context where threads would not be  */
   /* allowed to run.  If this function is called in the context where  */
   /* threads are allowed to run then this parameter should be set to   */
   /* FALSE.                                                            */
void CleanupHRSModule(Boolean_t ForceCleanup)
{
   /* Check to make sure that this module has been initialized.         */
   if(InstanceListInitialized)
   {
      /* Wait for access to the HRS Context List.                       */
      if((ForceCleanup) || ((!ForceCleanup) && (BSC_AcquireListLock())))
      {
         /* Cleanup the Instance List.                                  */
         BTPS_MemInitialize(InstanceList, 0, sizeof(InstanceList));

         if(!ForceCleanup)
            BSC_ReleaseListLock();
      }

      /* Cleanup the module.                                            */
      CleanupModule();
   }
}

   /* The following function is responsible for opening a HRS Server.   */
   /* The first parameter is the Bluetooth Stack ID on which to open the*/
   /* server.  The second parameter is the mask of supported Heart Rate */
   /* Control Point commands.  The third parameter is the Callback      */
   /* function to call when an event occurs on this Server Port.  The   */
   /* fourth parameter is a user-defined callback parameter that will be*/
   /* passed to the callback function with each event.  The final       */
   /* parameter is a pointer to store the GATT Service ID of the        */
   /* registered HRS service.  This can be used to include the service  */
   /* registered by this call.  This function returns the positive,     */
   /* non-zero, Instance ID or a negative error code.                   */
   /* * NOTE * Only 1 HRS Server may be open at a time, per Bluetooth   */
   /*          Stack ID.                                                */
   /* * NOTE * The Supported_Commands parameter must be made up of bit  */
   /*          masks of the form:                                       */
   /*             HRS_HEART_RATE_CONTROL_POINT_XXX_SUPPORTED            */
   /* * NOTE * All Client Requests will be dispatch to the EventCallback*/
   /*          function that is specified by the second parameter to    */
   /*          this function.                                           */
int BTPSAPI HRS_Initialize_Service(unsigned int BluetoothStackID, unsigned long Supported_Commands, HRS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID)
{
   GATT_Attribute_Handle_Group_t ServiceHandleRange;

    /* Initialize the Service Handle Group to 0.                        */
   ServiceHandleRange.Starting_Handle = 0;
   ServiceHandleRange.Ending_Handle   = 0;

   return(HRSRegisterService(BluetoothStackID, Supported_Commands, EventCallback, CallbackParameter, ServiceID, &ServiceHandleRange));
}

   /* The following function is responsible for opening a HRS Server.   */
   /* The first parameter is the Bluetooth Stack ID on which to open the*/
   /* server.  The second parameter is the mask of supported Heart Rate */
   /* Control Point commands.  The third parameter is the Callback      */
   /* function to call when an event occurs on this Server Port.  The   */
   /* fourth parameter is a user-defined callback parameter that will be*/
   /* passed to the callback function with each event.  The next        */
   /* parameter is a pointer to store the GATT Service ID of the        */
   /* registered HRS service.  This can be used to include the service  */
   /* registered by this call.  The final parameter is a pointer, that  */
   /* on input can be used to control the location of the service in the*/
   /* GATT database, and on ouput to store the service handle range.    */
   /* This function returns the positive, non-zero, Instance ID or a    */
   /* negative error code.                                              */
   /* * NOTE * Only 1 HRS Server may be open at a time, per Bluetooth   */
   /*          Stack ID.                                                */
   /* * NOTE * The Supported_Commands parameter must be made up of bit  */
   /*          masks of the form:                                       */
   /*          HRS_HEART_RATE_CONTROL_POINT_XXX_SUPPORTED               */
   /* * NOTE * All Client Requests will be dispatch to the EventCallback*/
   /*          function that is specified by the second parameter to    */
   /*          this function.                                           */
int BTPSAPI HRS_Initialize_Service_Handle_Range(unsigned int BluetoothStackID, unsigned long Supported_Commands, HRS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange)
{
   return(HRSRegisterService(BluetoothStackID, Supported_Commands, EventCallback, CallbackParameter, ServiceID, ServiceHandleRange));
}

   /* The following function is responsible for closing a previously    */
   /* opened HRS Server.  The first parameter is the Bluetooth Stack ID */
   /* on which to close the server.  The second parameter is the        */
   /* InstanceID that was returned from a successful call to            */
   /* HRS_Initialize_Service().  This function returns a zero if        */
   /* successful or a negative return error code if an error occurs.    */
int BTPSAPI HRS_Cleanup_Service(unsigned int BluetoothStackID, unsigned int InstanceID)
{
   int                   ret_val;
   HRSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID))
   {
      /* Acquire the specified HRS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Verify that the service is actually registered.             */
         if(ServiceInstance->ServiceID)
         {
            /* Call GATT to un-register the service.                    */
            GATT_Un_Register_Service(BluetoothStackID, ServiceInstance->ServiceID);

            /* mark the instance entry as being free.                   */
            BTPS_MemInitialize(ServiceInstance, 0, HRS_SERVER_INSTANCE_DATA_SIZE);

            /* return success to the caller.                            */
            ret_val = 0;
         }
         else
            ret_val = HRS_ERROR_INVALID_PARAMETER;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(BluetoothStackID);
      }
      else
         ret_val = HRS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = HRS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for querying the number of  */
   /* attributes that are contained in the HRS Service that is          */
   /* registered with a call to HRS_Initialize_Service().  This function*/
   /* returns the non-zero number of attributes that are contained in a */
   /* HRS Server or zero on failure.                                    */
unsigned int BTPSAPI HRS_Query_Number_Attributes(void)
{
   /* Simply return the number of attributes that are contained in a HRS*/
   /* service.                                                          */
   return(HEART_RATE_SERVICE_ATTRIBUTE_COUNT);
}

   /* The following function is responsible for setting the Body Sensor */
   /* Location on the specified HRS Instance.  The first parameter is   */
   /* the Bluetooth Stack ID of the Bluetooth Device.  The second       */
   /* parameter is the InstanceID returned from a successful call to    */
   /* HRS_Initialize_Server().  The final parameter is the Body Sensor  */
   /* Location to set for the specified HRS Instance.  This function    */
   /* returns a zero if successful or a negative return error code if an*/
   /* error occurs.                                                     */
   /* * NOTE * The Body_Sensor_Location parameter should be an          */
   /*          enumerated value of the form                             */
   /*          HRS_BODY_SENSOR_LOCATION_XXX.                            */
int BTPSAPI HRS_Set_Body_Sensor_Location(unsigned int BluetoothStackID, unsigned int InstanceID, Byte_t Body_Sensor_Location)
{
#if BTPS_CONFIGURATION_HRS_SUPPORT_BODY_SENSOR_LOCATION

   int                  ret_val;
   HRSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID))
   {
      /* Acquire the specified HRS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Assign the Temperature Type for the specified instance.     */
         ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(InstanceData[InstanceID-1].Body_Sensor_Location), Body_Sensor_Location);

         /* Return success to the caller.                               */
         ret_val = 0;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = HRS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = HRS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);

#else

   return(BTPS_ERROR_FEATURE_NOT_AVAILABLE);

#endif
}

   /* The following function is responsible for querying the current    */
   /* Temperature Type on the specified HRS Instance.  The first        */
   /* parameter is the Bluetooth Stack ID of the Bluetooth Device.  The */
   /* second parameter is the InstanceID returned from a successful call*/
   /* to HRS_Initialize_Server().  The final parameter is a pointer to  */
   /* return the current Body Sensor Location for the specified HRS     */
   /* Instance.  This function returns a zero if successful or a        */
   /* negative return error code if an error occurs.                    */
int BTPSAPI HRS_Query_Body_Sensor_Location(unsigned int BluetoothStackID, unsigned int InstanceID, Byte_t *Body_Sensor_Location)
{
#if BTPS_CONFIGURATION_HRS_SUPPORT_BODY_SENSOR_LOCATION

   int                  ret_val;
   HRSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (Body_Sensor_Location))
   {
      /* Acquire the specified HRS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Query the curent Temperature Type.                          */
         *Body_Sensor_Location = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(InstanceData[InstanceID-1].Body_Sensor_Location));

         /* Return success to the caller.                               */
         ret_val           = 0;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = HRS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = HRS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);

#else

   return(BTPS_ERROR_FEATURE_NOT_AVAILABLE);

#endif
}

   /* The following function is responsible for responding to a HRS Read*/
   /* Client Configuration Request.  The first parameter is the         */
   /* Bluetooth Stack ID of the Bluetooth Device.  The second parameter */
   /* is the InstanceID returned from a successful call to              */
   /* HRS_Initialize_Server().  The third is the Transaction ID of the  */
   /* request.  The final parameter contains the Client Configuration to*/
   /* send to the remote device.  This function returns a zero if       */
   /* successful or a negative return error code if an error occurs.    */
int BTPSAPI HRS_Read_Client_Configuration_Response(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int TransactionID, Word_t Client_Configuration)
{
   int                  ret_val;
   Word_t               ValueLength;
   NonAlignedWord_t     ClientConfiguration;
   HRSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (TransactionID))
   {
      /* Acquire the specified HRS Instance.                            */
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
         ret_val = HRS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = HRS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for sending a Heart Rate    */
   /* Measurement notification to a specified remote device.  The first */
   /* parameter is the Bluetooth Stack ID of the Bluetooth Device.  The */
   /* second parameter is the InstanceID returned from a successful call*/
   /* to HRS_Initialize_Server().  The third parameter is the           */
   /* ConnectionID of the remote device to send the notification to.    */
   /* The final parameter is the Heart Rate Measurement data to notify. */
   /* This function returns a zero if successful or a negative return   */
   /* error code if an error occurs.                                    */
int BTPSAPI HRS_Notify_Heart_Rate_Measurement(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int ConnectionID, HRS_Heart_Rate_Measurement_Data_t *Heart_Rate_Measurement)
{
   int                  ret_val;
   Byte_t              *NotificationData;
   Word_t               MTU;
   Word_t               MaximumRRIntervals;
   unsigned int         NotificationDataLength;
   HRSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (ConnectionID) && (Heart_Rate_Measurement) && ((!(Heart_Rate_Measurement->Flags & HRS_HEART_RATE_MEASUREMENT_FLAGS_RR_INTERVAL_PRESENT)) || ((Heart_Rate_Measurement->Flags & HRS_HEART_RATE_MEASUREMENT_FLAGS_RR_INTERVAL_PRESENT) && (Heart_Rate_Measurement->Number_Of_RR_Intervals))))
   {
      /* Acquire the specified HRS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Do not allow the user to notify the Energy Expended Status  */
         /* field if this support is not included in the configuration. */
#if !BTPS_CONFIGURATION_HRS_SUPPORT_ENERGY_EXPENDED_STATUS

         /* Clear the Energy Expended present flag since support for    */
         /* this field is not compiled into the module.                 */
         Heart_Rate_Measurement->Flags &= ~HRS_HEART_RATE_MEASUREMENT_FLAGS_ENERGY_EXPENDED_PRESENT;

#endif

         /* Call GATT to determine the Connection MTU, this is needed to*/
         /* determine the total number of RR Intervals that can be fit  */
         /* into the message (if necessary).                            */
         if(!(ret_val = GATT_Query_Connection_MTU(BluetoothStackID, ConnectionID, &MTU)))
         {
            /* If RR Intervals were specified then we need to check to  */
            /* see what the maximum number of RR Intervals that will fit*/
            /* into a notification for this connection.                 */
            if(Heart_Rate_Measurement->Flags & HRS_HEART_RATE_MEASUREMENT_FLAGS_RR_INTERVAL_PRESENT)
            {
               /* Calculate the number of RR Intervals that can fit into*/
               /* a notification for this connection.                   */
               MaximumRRIntervals = (Word_t)CalculateMaximumSupportedIntervals(Heart_Rate_Measurement, MTU);

               /* If the number of RR Intervals specified in the Heart  */
               /* Rate Measurement is more than will fit into a         */
               /* notification then truncate the number of measurements */
               /* that we will format into the notification.            */
               if(Heart_Rate_Measurement->Number_Of_RR_Intervals > MaximumRRIntervals)
                  Heart_Rate_Measurement->Number_Of_RR_Intervals = MaximumRRIntervals;
            }
            else
               Heart_Rate_Measurement->Number_Of_RR_Intervals = 0;

            /* Allocate a buffer that is big enough to hold the         */
            /* notification.                                            */
            NotificationDataLength = HRS_HEART_RATE_MEASUREMENT_LENGTH(Heart_Rate_Measurement->Flags, Heart_Rate_Measurement->Number_Of_RR_Intervals);
            if((NotificationData = BTPS_AllocateMemory(NotificationDataLength)) != NULL)
            {
               /* Initialize the Notification Data memory.              */
               BTPS_MemInitialize(NotificationData, 0, NotificationDataLength);

               /* Format the Heart Rate Measurement into the            */
               /* Notification.                                         */
               if(!(ret_val = FormatHeartRateMeasurement(Heart_Rate_Measurement, NotificationDataLength, NotificationData)))
               {
                  /* Attempt to send the notification.                  */
                  ret_val = GATT_Handle_Value_Notification(ServiceInstance->BluetoothStackID, ServiceInstance->ServiceID, ConnectionID, HRS_HEART_RATE_MEASUREMENT_ATTRIBUTE_OFFSET, (Word_t)NotificationDataLength, (Byte_t *)NotificationData);
                  if(ret_val > 0)
                     ret_val = 0;
               }

               /* Free the previously allocated memory.                 */
               BTPS_FreeMemory(NotificationData);
            }
            else
               ret_val = HRS_ERROR_INSUFFICIENT_RESOURCES;
         }

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = HRS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = HRS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* HRS Client API.                                                   */

   /* The following function is responsible for parsing a value received*/
   /* from a remote HRS Server interpreting it as a Heart Rate          */
   /* Measurement characteristic.  The first parameter is the length of */
   /* the value returned by the remote HRS Server.  The second parameter*/
   /* is a pointer to the data returned by the remote HRS Server.  The  */
   /* final parameter is a pointer to store the parsed Temperature      */
   /* Measurement value.  This function returns a zero if successful or */
   /* a negative return error code if an error occurs.                  */
   /* * NOTE * On INPUT the Number_Of_RR_Intervals member of the        */
   /*          HeartRateMeasurement parameter must contain the number of*/
   /*          entries in the RR_Intervals array.  On RETURN this       */
   /*          parameter will contain the actual number of RR Interval  */
   /*          values that were parsed from the Heart Rate Measurement  */
   /*          value (which will always be less than or equal to the    */
   /*          number entries allocated in the structure).              */
   /* * NOTE * It is possible to query the total number of RR Interval  */
   /*          values in the Heart Rate Measurement value by passing 0  */
   /*          for the Number_Of_RR_Intervals member of the             */
   /*          HeartRateMeasurement parameter.  In this case on return  */
   /*          the Number_Of_RR_Intervals member will contain the total */
   /*          number of RR Interval values in the Heart Rate           */
   /*          Measurement value BUT no RR Intervals will be parsed into*/
   /*          the HeartRateMeasurement structure.                      */
int BTPSAPI HRS_Decode_Heart_Rate_Measurement(unsigned int ValueLength, Byte_t *Value, HRS_Heart_Rate_Measurement_Data_t *HeartRateMeasurement)
{
   /* Call the internal function to decode the Heart Rate Measurement.  */
   return(DecodeHeartRateMeasurement(ValueLength, Value, HeartRateMeasurement));
}

   /* The following function is responsible for parsing a value received*/
   /* from a remote HRS Server interpreting it as a Body Sensor Location*/
   /* value.  The first parameter is the length of the value returned by*/
   /* the remote HRS Server.  The second parameter is a pointer to the  */
   /* data returned by the remote HRS Server.  The final parameter is a */
   /* pointer to store the parsed Body Sensor Location value.  This     */
   /* function returns a zero if successful or a negative return error  */
   /* code if an error occurs.                                          */
int BTPSAPI HRS_Decode_Body_Sensor_Location(unsigned int ValueLength, Byte_t *Value, Byte_t *BodySensorLocation)
{
   /* Call the internal function to decode the Body Sensor Location.    */
   return(DecodeBodySensorLocation(ValueLength, Value, BodySensorLocation));
}

   /* The following function is responsible for formatting a Heart Rate */
   /* Control Command into a user specified buffer.  The first parameter*/
   /* is the command to format.  The final two parameters contain the   */
   /* length of the buffer, and the buffer, to format the command into. */
   /* This function returns a zero if successful or a negative return   */
   /* error code if an error occurs.                                    */
   /* * NOTE * The BufferLength and Buffer parameter must point to a    */
   /*          buffer of at least                                       */
   /*          HRS_HEART_RATE_CONTROL_POINT_VALUE_LENGTH in size.       */
int BTPSAPI HRS_Format_Heart_Rate_Control_Command(HRS_Heart_Rate_Control_Command_t Command, unsigned int BufferLength, Byte_t *Buffer)
{
   int ret_val;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((HRS_HEART_RATE_CONTROL_POINT_VALID_COMMAND(Command)) && (BufferLength >= HRS_HEART_RATE_CONTROL_POINT_VALUE_LENGTH) && (Buffer))
   {
      /* Assign the command into the user specified buffer.             */
      ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(Buffer, Command);

      /* Return success to the caller.                                  */
      ret_val = 0;
   }
   else
      ret_val = HRS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}
