/*****< hts.c >****************************************************************/
/*      Copyright 2011 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  HTS - Bluetooth Stack Health Thermometer Service (GATT Based) for         */
/*        Stonestreet One Bluetooth Protocol Stack.                           */
/*                                                                            */
/*  Author:  Tim Cook                                                         */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   10/04/11  T. Cook        Initial creation.                               */
/******************************************************************************/
#include "SS1BTPS.h"        /* Bluetooth Stack API Prototypes/Constants.      */
#include "SS1BTGAT.h"       /* Bluetooth Stack GATT API Prototypes/Constants. */
#include "SS1BTHTS.h"       /* Bluetooth HTS API Prototypes/Constants.        */

#include "BTPSKRNL.h"       /* BTPS Kernel Prototypes/Constants.              */
#include "HTS.h"            /* Bluetooth HTS Prototypes/Constants.            */

   /* The following controls the number of supported HTS instances.     */
#define HTS_MAXIMUM_SUPPORTED_INSTANCES                 (BTPS_CONFIGURATION_HTS_MAXIMUM_SUPPORTED_INSTANCES)

   /* The following structure defines the structure of a generic HTS    */
   /* Characteristic Value.                                             */
   /* * NOTE * The Data member is a pointer to a binary blob that must  */
   /*          be formatted such that if the characteristic value is    */
   /*          read, we can internally respond with the data directly   */
   /*          using this pointer.                                      */
typedef struct _tagHTSCharacteristicValue_t
{
   Word_t  Characteristic_Length;
   Byte_t *Data;
} HTSCharacteristicValue_t;

#define HTS_CHARACTERISTIC_VALUE_DATA_SIZE               (sizeof(HTSCharacteristicValue_t))

#if ((BTPS_CONFIGURATION_HTS_SUPPORT_TEMPERATURE_TYPE) || (BTPS_CONFIGURATION_HTS_SUPPORT_MEASUREMENT_INTERVAL))

   /* The following defines the HTS Instance Data, that contains data   */
   /* is unique for each HTS Service Instance.                          */
typedef __PACKED_STRUCT_BEGIN__ struct _tagHTS_Instance_Data_t
{
#if BTPS_CONFIGURATION_HTS_SUPPORT_TEMPERATURE_TYPE

   NonAlignedWord_t  Temperature_Type_Length;
   NonAlignedByte_t  Temperature_Type;

#endif

#if BTPS_CONFIGURATION_HTS_SUPPORT_MEASUREMENT_INTERVAL

   NonAlignedWord_t  Measurement_Interval_Length;
   NonAlignedWord_t  Measurement_Interval;
   NonAlignedWord_t  Valid_Range_Length;
   HTS_Valid_Range_t Valid_Range;

#endif

} __PACKED_STRUCT_END__ HTS_Instance_Data_t;

#define HTS_INSTANCE_DATA_SIZE                           (sizeof(HTS_Instance_Data_t))

#if BTPS_CONFIGURATION_HTS_SUPPORT_TEMPERATURE_TYPE

#define HTS_TEMPERATURE_TYPE_INSTANCE_TAG                (BTPS_STRUCTURE_OFFSET(HTS_Instance_Data_t, Temperature_Type_Length))

#endif

#if BTPS_CONFIGURATION_HTS_SUPPORT_MEASUREMENT_INTERVAL

#define HTS_MEASUREMENT_INTERVAL_INSTANCE_TAG            (BTPS_STRUCTURE_OFFSET(HTS_Instance_Data_t, Measurement_Interval_Length))
#define HTS_VALID_RANGE_INSTANCE_TAG                     (BTPS_STRUCTURE_OFFSET(HTS_Instance_Data_t, Valid_Range_Length))

#endif

#endif

   /*********************************************************************/
   /**               Health Thermometer Service Table                  **/
   /*********************************************************************/

   /* The Health Thermometer Service Declaration UUID.                  */
static BTPSCONST GATT_Primary_Service_16_Entry_t HTS_Service_UUID =
{
   HTS_SERVICE_BLUETOOTH_UUID_CONSTANT
};

   /* The Temperature Measurement Characteristic Declaration.           */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t HTS_Temperature_Measurement_Declaration =
{
   GATT_CHARACTERISTIC_PROPERTIES_INDICATE,
   HTS_TEMPERATURE_MEASUREMENT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The Temperature Measurement Characteristic Value.                 */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t  HTS_Temperature_Measurement_Value =
{
   HTS_TEMPERATURE_MEASUREMENT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   0,
   NULL
};

#if BTPS_CONFIGURATION_HTS_SUPPORT_TEMPERATURE_TYPE

   /* The Temperature Type Characteristic Declaration.                  */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t HTS_Temperature_Type_Declaration =
{
   GATT_CHARACTERISTIC_PROPERTIES_READ,
   HTS_TEMPERATURE_TYPE_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The Temperature Type Characteristic Value.                        */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t  HTS_Temperature_Type_Value =
{
   HTS_TEMPERATURE_TYPE_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   HTS_TEMPERATURE_TYPE_INSTANCE_TAG,
   NULL
};

#endif

#if BTPS_CONFIGURATION_HTS_SUPPORT_INTERMEDIATE_TEMPERATURE

   /* The Intermediate Temperature Characteristic Declaration.          */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t HTS_Intermediate_Temperature_Declaration =
{
   GATT_CHARACTERISTIC_PROPERTIES_NOTIFY,
   HTS_INTERMEDIATE_TEMPERATURE_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The Intermediate Temperature Characteristic Value.                */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t  HTS_Intermediate_Temperature_Value =
{
   HTS_INTERMEDIATE_TEMPERATURE_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   0,
   NULL
};

#endif

#if BTPS_CONFIGURATION_HTS_SUPPORT_MEASUREMENT_INTERVAL

   /* The following define the Measurement Interval Characteristic and  */
   /* Service Table Flags.                                              */


   /* The Measurement Interval Characteristic Declaration.              */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t HTS_Measurement_Interval_Declaration =
{
   (GATT_CHARACTERISTIC_PROPERTIES_READ|GATT_CHARACTERISTIC_PROPERTIES_INDICATE|GATT_CHARACTERISTIC_PROPERTIES_WRITE),
   HTS_MEASUREMENT_INTERVAL_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The Measurement Interval Characteristic Value.                    */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t  HTS_Measurement_Interval_Value =
{
   HTS_MEASUREMENT_INTERVAL_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   HTS_MEASUREMENT_INTERVAL_INSTANCE_TAG,
   NULL
};

   /* The Valid Range Descriptor.                                       */
static GATT_Characteristic_Descriptor_16_Entry_t Valid_Range_Descriptor =
{
   HTS_VALID_RANGE_DESCRIPTOR_BLUETOOTH_UUID_CONSTANT,
   HTS_VALID_RANGE_INSTANCE_TAG,
   NULL
};

#endif

   /* Client Characteristic Configuration Descriptor.                   */
static GATT_Characteristic_Descriptor_16_Entry_t Client_Characteristic_Configuration =
{
   GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_BLUETOOTH_UUID_CONSTANT,
   GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_LENGTH,
   NULL
};

   /* The following base offsets are used to calculate the correct      */
   /* attribute even if certain features are compiled out of the module.*/
#define ATTRIBUTE_OFFSET_BASE_0                          0
#define ATTRIBUTE_OFFSET_BASE_1                          0

   /* The following defines the Health Thermometer service that is      */
   /* registered with the GATT_Register_Service function call.          */
   /* * NOTE * This array will be registered with GATT in the call to   */
   /*          GATT_Register_Service.                                   */
BTPSCONST GATT_Service_Attribute_Entry_t Health_Thermometer_Service[] =
{
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetPrimaryService16,            (Byte_t *)&HTS_Service_UUID},
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetCharacteristicDeclaration16, (Byte_t *)&HTS_Temperature_Measurement_Declaration},
   {0,                                      aetCharacteristicValue16,       (Byte_t *)&HTS_Temperature_Measurement_Value},
   {GATT_ATTRIBUTE_FLAGS_READABLE_WRITABLE, aetCharacteristicDescriptor16,  (Byte_t *)&Client_Characteristic_Configuration},

#if BTPS_CONFIGURATION_HTS_SUPPORT_TEMPERATURE_TYPE

   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetCharacteristicDeclaration16, (Byte_t *)&HTS_Temperature_Type_Declaration},
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetCharacteristicValue16,       (Byte_t *)&HTS_Temperature_Type_Value},

   /* If we are including support for the temperature type              */
   /* characteristic we must define the Attribute Offset Base 0 to be 2 */
   /* so that the attribute offset defines are not thrown off.          */
#undef ATTRIBUTE_OFFSET_BASE_0
#define ATTRIBUTE_OFFSET_BASE_0             2

#endif

#if BTPS_CONFIGURATION_HTS_SUPPORT_INTERMEDIATE_TEMPERATURE

   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetCharacteristicDeclaration16, (Byte_t *)&HTS_Intermediate_Temperature_Declaration},
   {0,                                      aetCharacteristicValue16,       (Byte_t *)&HTS_Intermediate_Temperature_Value},
   {GATT_ATTRIBUTE_FLAGS_READABLE_WRITABLE, aetCharacteristicDescriptor16,  (Byte_t *)&Client_Characteristic_Configuration},

   /* If we are including support for the intermediate temperature      */
   /* characteristic we must define the Attribute Offset Base 1 to be 3 */
   /* so that the attribute offset defines are not thrown off.          */
#undef ATTRIBUTE_OFFSET_BASE_1
#define ATTRIBUTE_OFFSET_BASE_1             3

#endif


#if BTPS_CONFIGURATION_HTS_SUPPORT_MEASUREMENT_INTERVAL

   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetCharacteristicDeclaration16, (Byte_t *)&HTS_Measurement_Interval_Declaration},
   {GATT_ATTRIBUTE_FLAGS_READABLE_WRITABLE, aetCharacteristicValue16,       (Byte_t *)&HTS_Measurement_Interval_Value},
   {GATT_ATTRIBUTE_FLAGS_READABLE_WRITABLE, aetCharacteristicDescriptor16,  (Byte_t *)&Client_Characteristic_Configuration},
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetCharacteristicDescriptor16,  (Byte_t *)&Valid_Range_Descriptor},

#endif
};

#define HEALTH_THERMOMETER_SERVICE_ATTRIBUTE_COUNT                      (sizeof(Health_Thermometer_Service)/sizeof(GATT_Service_Attribute_Entry_t))

#define HTS_TEMPERATURE_MEASUREMENT_ATTRIBUTE_OFFSET                    2
#define HTS_TEMPERATURE_MEASUREMENT_CCD_ATTRIBUTE_OFFSET                3
#define HTS_INTERMEDIATE_TEMPERATURE_ATTRIBUTE_OFFSET                   (5 + ATTRIBUTE_OFFSET_BASE_0)
#define HTS_INTERMEDIATE_TEMPERATURE_CCD_ATTRIBUTE_OFFSET               (6 + ATTRIBUTE_OFFSET_BASE_0)
#define HTS_MEASUREMENT_INTERVAL_ATTRIBUTE_OFFSET                       (5 + ATTRIBUTE_OFFSET_BASE_0 + ATTRIBUTE_OFFSET_BASE_1)
#define HTS_MEASUREMENT_INTERVAL_CCD_ATTRIBUTE_OFFSET                   (6 + ATTRIBUTE_OFFSET_BASE_0 + ATTRIBUTE_OFFSET_BASE_1)
#define HTS_MEASUREMENT_INTERVAL_VALID_RANGE_DESCRIPTOR                 (7 + ATTRIBUTE_OFFSET_BASE_0 + ATTRIBUTE_OFFSET_BASE_1)

   /*********************************************************************/
   /**                    END OF SERVICE TABLE                         **/
   /*********************************************************************/

   /* The following type defines a union large enough to hold all events*/
   /* dispatched by this module.                                        */
typedef union
{
   HTS_Read_Client_Configuration_Data_t   Read_Client_Data;
   HTS_Client_Configuration_Update_Data_t Client_Configuration_Update_Data;
   HTS_Measurement_Interval_Update_Data_t Measurement_Interval_Update_Data;
   HTS_Confirmation_Data_t                HTS_Confirmation_Data;
} HTS_Event_Data_Buffer_t;

#define HTS_EVENT_DATA_BUFFER_SIZE                      (sizeof(HTS_Event_Data_Buffer_t))

   /* The following structure represents the information that will be   */
   /* stored during an outstanding GATT indication.                     */
typedef struct _tagIndicatonInfo_t_t
{
   unsigned int ConnectionID;
   int          TransactionID;
} IndicatonInfo_t;

   /* HTS Service Instance Block.  This structure contains All          */
   /* information associated with a specific Bluetooth Stack ID (member */
   /* is present in this structure).                                    */
typedef struct _tagHTSServerInstance_t
{
   unsigned int         BluetoothStackID;
   unsigned int         ServiceID;
   HTS_Event_Callback_t EventCallback;
   unsigned long        CallbackParameter;
   IndicatonInfo_t      TemperatureMeasurementIndicactionInfo;
   IndicatonInfo_t      MeasurementIntervalIndicationInfo;
} HTSServerInstance_t;

#define HTS_SERVER_INSTANCE_DATA_SIZE                    (sizeof(HTSServerInstance_t))

   /* Internal Variables to this Module (Remember that all variables    */
   /* declared static are initialized to 0 automatically by the         */
   /* compiler as part of standard C/C++).                              */

#if ((BTPS_CONFIGURATION_HTS_SUPPORT_TEMPERATURE_TYPE) || (BTPS_CONFIGURATION_HTS_SUPPORT_MEASUREMENT_INTERVAL))

static HTS_Instance_Data_t InstanceData[HTS_MAXIMUM_SUPPORTED_INSTANCES];
                                            /* Variable which holds all */
                                            /* data that is unique for  */
                                            /* each service instance.   */

#endif

static HTSServerInstance_t InstanceList[HTS_MAXIMUM_SUPPORTED_INSTANCES];
                                            /* Variable which holds the */
                                            /* service instance data.   */

static Boolean_t InstanceListInitialized;   /* Variable that flags that */
                                            /* is used to denote that   */
                                            /* this module has been     */
                                            /* successfully initialized.*/

   /* The following are the prototypes of local functions.              */
static Boolean_t InitializeModule(void);
static void CleanupModule(void);

static int FormatTemperatureMeasurement(HTS_Temperature_Measurement_Data_t *Temperature_Measurement, unsigned int BufferLength, Byte_t *Buffer);
static int DecodeTemperatureMeasurement(unsigned int BufferLength, Byte_t *Value, HTS_Temperature_Measurement_Data_t *Temperature_Measurement);
static int DecodeClientConfigurationValue(unsigned int BufferLength, Byte_t *Buffer, Word_t *ClientConfiguration);

#if BTPS_CONFIGURATION_HTS_SUPPORT_MEASUREMENT_INTERVAL

static Boolean_t VerifyMeasurementInterval(unsigned int InstanceID, Word_t MeasurementInterval);
static int DecodeMeasurementInterval(unsigned int InstanceID, unsigned int BufferLength, Byte_t *Buffer, Word_t *MeasurementInterval);

#endif

static int DecodeValidRange(unsigned int BufferLength, Byte_t *Buffer, HTS_Valid_Range_Data_t *ValidRange);

static HTS_Event_Data_t *FormatEventHeader(unsigned int BufferLength, Byte_t *Buffer, HTS_Event_Type_t EventType, unsigned int InstanceID, unsigned int ConnectionID, unsigned int *TransactionID, GATT_Connection_Type_t ConnectionType, BD_ADDR_t *BD_ADDR);

static Boolean_t InstanceRegisteredByStackID(unsigned int BluetoothStackID);
static HTSServerInstance_t *AcquireServiceInstance(unsigned int BluetoothStackID, unsigned int *InstanceID);

static int HTSRegisterService(unsigned int BluetoothStackID, HTS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange);

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

   /* The following function is a utility function that exists to format*/
   /* a Temperature Measurement Command into a user specified buffer.   */
static int FormatTemperatureMeasurement(HTS_Temperature_Measurement_Data_t *Temperature_Measurement, unsigned int BufferLength, Byte_t *Buffer)
{
   int ret_val = 0;

   /* Verify that the input parameters are valid.                       */
   if((Temperature_Measurement) && (BufferLength >= HTS_TEMPERATURE_MEASUREMENT_LENGTH(Temperature_Measurement->Flags)) && (Buffer))
   {
      /* Format the Header of the Temperature Measurement.              */
      ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(((HTS_Temperature_Measurement_Header_t *)Buffer)->Flags), Temperature_Measurement->Flags);

      /* Format the temperature.                                        */
      ASSIGN_UNALIGNED_GENERIC_TYPE(&(((HTS_Temperature_Measurement_Header_t *)Buffer)->Temperature), Temperature_Measurement->Temperature);

      /* Format the rest of the measurement based on the optional       */
      /* parameters.                                                    */
      if(Temperature_Measurement->Flags & HTS_TEMPERATURE_MEASUREMENT_FLAGS_TEMPERATURE_TYPE)
      {
         if(Temperature_Measurement->Flags & HTS_TEMPERATURE_MEASUREMENT_FLAGS_TIME_STAMP)
         {
            ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(((HTS_Temperature_Measurement_With_Time_Stamp_t *)Buffer)->Temperature_Type), Temperature_Measurement->Temperature_Type);
         }
         else
            ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(((HTS_Temperature_Measurement_Without_Time_Stamp_t *)Buffer)->Temperature_Type), Temperature_Measurement->Temperature_Type);

         if(!HTS_TEMPERATURE_TYPE_VALID_TEMPERATURE_TYPE(Temperature_Measurement->Temperature_Type))
            ret_val = HTS_ERROR_INVALID_PARAMETER;
      }

      /* If the Time Stamp is specified then format it into the         */
      /* specified buffer.                                              */
      if((!ret_val) && (Temperature_Measurement->Flags & HTS_TEMPERATURE_MEASUREMENT_FLAGS_TIME_STAMP))
      {
         if(HTS_TIME_STAMP_VALID_TIME_STAMP(Temperature_Measurement->Time_Stamp))
         {
            GATT_DATE_TIME_ASSIGN_YEAR(&(((HTS_Temperature_Measurement_With_Time_Stamp_t *)Buffer)->Time_Stamp), Temperature_Measurement->Time_Stamp.Year);
            GATT_DATE_TIME_ASSIGN_MONTH(&(((HTS_Temperature_Measurement_With_Time_Stamp_t *)Buffer)->Time_Stamp), Temperature_Measurement->Time_Stamp.Month);
            GATT_DATE_TIME_ASSIGN_DAY(&(((HTS_Temperature_Measurement_With_Time_Stamp_t *)Buffer)->Time_Stamp), Temperature_Measurement->Time_Stamp.Day);
            GATT_DATE_TIME_ASSIGN_HOURS(&(((HTS_Temperature_Measurement_With_Time_Stamp_t *)Buffer)->Time_Stamp), Temperature_Measurement->Time_Stamp.Hours);
            GATT_DATE_TIME_ASSIGN_MINUTES(&(((HTS_Temperature_Measurement_With_Time_Stamp_t *)Buffer)->Time_Stamp), Temperature_Measurement->Time_Stamp.Minutes);
            GATT_DATE_TIME_ASSIGN_SECONDS(&(((HTS_Temperature_Measurement_With_Time_Stamp_t *)Buffer)->Time_Stamp), Temperature_Measurement->Time_Stamp.Seconds);
         }
         else
            ret_val = HTS_ERROR_INVALID_PARAMETER;
      }
   }
   else
      ret_val = HTS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is used to decode a temperature measurement*/
   /* that has been received from a remote Client.                      */
static int DecodeTemperatureMeasurement(unsigned int BufferLength, Byte_t *Value, HTS_Temperature_Measurement_Data_t *Temperature_Measurement)
{
   int          ret_val;
   Byte_t       Flags;
   unsigned int ExpectedLength;

   /* Verify that the input parameters appear semi-valid.               */
   if((BufferLength) && (Value) && (Temperature_Measurement))
   {
      /* Verify that the Temperature Measurement is a valid length.     */
      if(BufferLength >= HTS_TEMPERATURE_MEASUREMENT_HEADER_DATA_SIZE)
      {
         /* Read the Flags from the Temperature Measurement.            */
         Flags = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(((HTS_Temperature_Measurement_Header_t *)Value)->Flags));

         /* Calculated the expected length based on the flags.          */
         ExpectedLength = HTS_TEMPERATURE_MEASUREMENT_LENGTH(Flags);

         /* Verify that the received data holds at least the expected   */
         /* data size.                                                  */
         if(BufferLength >= ExpectedLength)
         {
            /* Initialize the return value to success.                  */
            ret_val = 0;

            /* Initialize the Temperature Measurement.                  */
            BTPS_MemInitialize(Temperature_Measurement, 0, HTS_TEMPERATURE_MEASUREMENT_DATA_SIZE);

            /* Parse out the Temperature Measurement header.            */
            Temperature_Measurement->Flags       = Flags;
            Temperature_Measurement->Temperature = READ_UNALIGNED_GENERIC_TYPE(&(((HTS_Temperature_Measurement_Header_t *)Value)->Temperature));

            /* Parse out the Temperature Type (if one was specified).   */
            if(Flags & HTS_TEMPERATURE_MEASUREMENT_FLAGS_TEMPERATURE_TYPE)
            {
               if(Flags & HTS_TEMPERATURE_MEASUREMENT_FLAGS_TIME_STAMP)
                  Temperature_Measurement->Temperature_Type = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(((HTS_Temperature_Measurement_With_Time_Stamp_t *)Value)->Temperature_Type));
               else
                  Temperature_Measurement->Temperature_Type = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(((HTS_Temperature_Measurement_Without_Time_Stamp_t *)Value)->Temperature_Type));

               if(!HTS_TEMPERATURE_TYPE_VALID_TEMPERATURE_TYPE(Temperature_Measurement->Temperature_Type))
                  ret_val = HTS_ERROR_MALFORMATTED_DATA;
            }

            /* Parse out the Time Stamp (if one was specified AND no    */
            /* other error has occurred).                               */
            if((!ret_val) && (Flags & HTS_TEMPERATURE_MEASUREMENT_FLAGS_TIME_STAMP))
            {
               Temperature_Measurement->Time_Stamp.Year    = GATT_DATE_TIME_READ_YEAR(&(((HTS_Temperature_Measurement_With_Time_Stamp_t *)Value)->Time_Stamp));
               Temperature_Measurement->Time_Stamp.Month   = GATT_DATE_TIME_READ_MONTH(&(((HTS_Temperature_Measurement_With_Time_Stamp_t *)Value)->Time_Stamp));
               Temperature_Measurement->Time_Stamp.Day     = GATT_DATE_TIME_READ_DAY(&(((HTS_Temperature_Measurement_With_Time_Stamp_t *)Value)->Time_Stamp));
               Temperature_Measurement->Time_Stamp.Hours   = GATT_DATE_TIME_READ_HOURS(&(((HTS_Temperature_Measurement_With_Time_Stamp_t *)Value)->Time_Stamp));
               Temperature_Measurement->Time_Stamp.Minutes = GATT_DATE_TIME_READ_MINUTES(&(((HTS_Temperature_Measurement_With_Time_Stamp_t *)Value)->Time_Stamp));
               Temperature_Measurement->Time_Stamp.Seconds = GATT_DATE_TIME_READ_SECONDS(&(((HTS_Temperature_Measurement_With_Time_Stamp_t *)Value)->Time_Stamp));

               /* Verify that the Time Stamp is valid.                  */
               if(!HTS_TIME_STAMP_VALID_TIME_STAMP(Temperature_Measurement->Time_Stamp))
                  ret_val = HTS_ERROR_MALFORMATTED_DATA;
            }
         }
         else
            ret_val = HTS_ERROR_MALFORMATTED_DATA;
      }
      else
         ret_val = HTS_ERROR_MALFORMATTED_DATA;
   }
   else
      ret_val = HTS_ERROR_INVALID_PARAMETER;

   return(ret_val);
}

   /* The following function is a utility function that exists to decode*/
   /* an Client Configuration value into a user specified boolean value.*/
   /* This function returns the zero if successful or a negative error  */
   /* code.                                                             */
static int DecodeClientConfigurationValue(unsigned int BufferLength, Byte_t *Buffer, Word_t *ClientConfiguration)
{
   int ret_val = HTS_ERROR_MALFORMATTED_DATA;

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
         ret_val = HTS_ERROR_INVALID_PARAMETER;
   }

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

#if BTPS_CONFIGURATION_HTS_SUPPORT_MEASUREMENT_INTERVAL

   /* The following function is used to verify that the Measurement     */
   /* Interval is valid.                                                */
static Boolean_t VerifyMeasurementInterval(unsigned int InstanceID, Word_t MeasurementInterval)
{
   Word_t    Lower_Bounds;
   Word_t    Upper_Bounds;
   Boolean_t ret_val = FALSE;

   /* Read the valid bounds for the measurement interval.               */
   Lower_Bounds = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(InstanceData[InstanceID-1].Valid_Range.Lower_Bounds));
   Upper_Bounds = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(InstanceData[InstanceID-1].Valid_Range.Upper_Bounds));

   /* Verify that the requested interval is valid.                      */
   if((!MeasurementInterval) || ((MeasurementInterval >= Lower_Bounds) && (MeasurementInterval <= Upper_Bounds)))
   {
      /* Requested Measurement Interval is valid so return success.     */
      ret_val = TRUE;
   }

   return(ret_val);
}

   /* The following function is a utility function that exists to decode*/
   /* an Measurement Interval into a user specified value.  This        */
   /* function returns the zero if successful or a negative error code. */
static int DecodeMeasurementInterval(unsigned int InstanceID, unsigned int BufferLength, Byte_t *Buffer, Word_t *MeasurementInterval)
{
   int ret_val = HTS_ERROR_MALFORMATTED_DATA;

   /* Verify that the input parameters are valid.                       */
   if((BufferLength == HTS_MEASUREMENT_INTERVAL_VALUE_LENGTH) && (Buffer) && (MeasurementInterval))
   {
      /* Read the requested Client Configuration.                       */
      *MeasurementInterval = READ_UNALIGNED_WORD_LITTLE_ENDIAN(Buffer);

      /* Verify that the requested interval is valid.                   */
      if(VerifyMeasurementInterval(InstanceID, *MeasurementInterval))
      {
         /* Assign the Measurement Interval into the Instance Data.     */
         ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(InstanceData[InstanceID-1].Measurement_Interval), *MeasurementInterval);

         /* Requested Measurement Interval is valid so return success.  */
         ret_val = 0;
      }
   }
   else
   {
      if(BufferLength == GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_LENGTH)
         ret_val = HTS_ERROR_INVALID_PARAMETER;
   }

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

#endif

   /* The following function is a utility function that exists to decode*/
   /* an Valid Range into a user specified value.  This function returns*/
   /* the zero if successful or a negative error code.                  */
static int DecodeValidRange(unsigned int BufferLength, Byte_t *Buffer, HTS_Valid_Range_Data_t *ValidRange)
{
   int ret_val = HTS_ERROR_MALFORMATTED_DATA;

   /* Verify that the input parameters are valid.                       */
   if((BufferLength == HTS_VALID_RANGE_DATA_SIZE) && (Buffer) && (ValidRange))
   {
      /* Read the packed Valid Range Data.                              */
      ValidRange->Lower_Bounds = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(((HTS_Valid_Range_t *)Buffer)->Lower_Bounds));
      ValidRange->Upper_Bounds = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(((HTS_Valid_Range_t *)Buffer)->Upper_Bounds));

      ret_val                  = 0;
   }
   else
   {
      if(BufferLength == GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_LENGTH)
         ret_val = HTS_ERROR_INVALID_PARAMETER;
   }

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is a utility function that exists to format*/
   /* a HTS Event into the specified buffer.                            */
   /* * NOTE * TransactionID is optional and may be set to NULL.        */
   /* * NOTE * BD_ADDR is NOT optional and may NOT be set to NULL.      */
static HTS_Event_Data_t *FormatEventHeader(unsigned int BufferLength, Byte_t *Buffer, HTS_Event_Type_t EventType, unsigned int InstanceID, unsigned int ConnectionID, unsigned int *TransactionID, GATT_Connection_Type_t ConnectionType, BD_ADDR_t *BD_ADDR)
{
   HTS_Event_Data_t *EventData = NULL;

   if((BufferLength >= (HTS_EVENT_DATA_SIZE + HTS_EVENT_DATA_BUFFER_SIZE)) && (Buffer) && (BD_ADDR))
   {
      /* Format the header of the event, that is data that is common to */
      /* all events.                                                    */
      BTPS_MemInitialize(Buffer, 0, BufferLength);

      EventData                                                               = (HTS_Event_Data_t *)Buffer;
      EventData->Event_Data_Type                                              = EventType;
      EventData->Event_Data.HTS_Read_Client_Configuration_Data               = (HTS_Read_Client_Configuration_Data_t *)(((Byte_t *)EventData) + HTS_EVENT_DATA_SIZE);

      EventData->Event_Data.HTS_Read_Client_Configuration_Data->InstanceID   = InstanceID;
      EventData->Event_Data.HTS_Read_Client_Configuration_Data->ConnectionID = ConnectionID;

      if(TransactionID)
      {
         EventData->Event_Data.HTS_Read_Client_Configuration_Data->TransactionID  = *TransactionID;
         EventData->Event_Data.HTS_Read_Client_Configuration_Data->ConnectionType = ConnectionType;
         EventData->Event_Data.HTS_Read_Client_Configuration_Data->RemoteDevice   = *BD_ADDR;
      }
      else
      {
         EventData->Event_Data.HTS_Client_Configuration_Update_Data->ConnectionType = ConnectionType;
         EventData->Event_Data.HTS_Client_Configuration_Update_Data->RemoteDevice   = *BD_ADDR;
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

   for(Index=0;Index<HTS_MAXIMUM_SUPPORTED_INSTANCES;Index++)
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
static HTSServerInstance_t *AcquireServiceInstance(unsigned int BluetoothStackID, unsigned int *InstanceID)
{
   unsigned int          LocalInstanceID;
   unsigned int          Index;
   HTSServerInstance_t *ret_val = NULL;

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
         if((LocalInstanceID) && (LocalInstanceID <= HTS_MAXIMUM_SUPPORTED_INSTANCES))
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
               for(Index=0;Index<HTS_MAXIMUM_SUPPORTED_INSTANCES;Index++)
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
   /* register an HTS Service.  This function returns the positive,     */
   /* non-zero, Instance ID on success or a negative error code.        */
static int HTSRegisterService(unsigned int BluetoothStackID, HTS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange)
{
   int                  ret_val;
   unsigned int         InstanceID;
   HTSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (EventCallback) && (ServiceID))
   {
      /* Verify that no instance is registered to this Bluetooth Stack. */
      if(!InstanceRegisteredByStackID(BluetoothStackID))
      {
         /* Acquire a free HTS Instance.                                */
         InstanceID = 0;
         if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
         {
            /* Call GATT to register the HTS service.                   */
            ret_val = GATT_Register_Service(BluetoothStackID, HTS_SERVICE_FLAGS, HEALTH_THERMOMETER_SERVICE_ATTRIBUTE_COUNT, (GATT_Service_Attribute_Entry_t *)Health_Thermometer_Service, ServiceHandleRange, GATT_ServerEventCallback, InstanceID);
            if(ret_val > 0)
            {
               /* Save the Instance information.                        */
               ServiceInstance->BluetoothStackID  = BluetoothStackID;
               ServiceInstance->ServiceID         = (unsigned int)ret_val;
               ServiceInstance->EventCallback     = EventCallback;
               ServiceInstance->CallbackParameter = CallbackParameter;
               *ServiceID                         = (unsigned int)ret_val;

#if ((BTPS_CONFIGURATION_HTS_SUPPORT_TEMPERATURE_TYPE) || (BTPS_CONFIGURATION_HTS_SUPPORT_MEASUREMENT_INTERVAL))

               /* Intilize the Instance Data for this instance.         */
               BTPS_MemInitialize(&InstanceData[InstanceID-1], 0, HTS_INSTANCE_DATA_SIZE);

#endif

#if BTPS_CONFIGURATION_HTS_SUPPORT_TEMPERATURE_TYPE

               ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(InstanceData[InstanceID-1].Temperature_Type_Length), HTS_TEMPERATURE_TYPE_VALUE_LENGTH);

#endif

#if BTPS_CONFIGURATION_HTS_SUPPORT_MEASUREMENT_INTERVAL

               ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(InstanceData[InstanceID-1].Measurement_Interval_Length), HTS_MEASUREMENT_INTERVAL_VALUE_LENGTH);
               ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(InstanceData[InstanceID-1].Valid_Range_Length), HTS_VALID_RANGE_SIZE);

#endif

               /* Return the HTS Instance ID.                           */
               ret_val                                        = (int)InstanceID;
            }

            /* UnLock the previously locked Bluetooth Stack.            */
            BSC_UnLockBluetoothStack(BluetoothStackID);
         }
         else
            ret_val = HTS_ERROR_INSUFFICIENT_RESOURCES;
      }
      else
         ret_val = HTS_ERROR_SERVICE_ALREADY_REGISTERED;
   }
   else
      ret_val = HTS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is the GATT Server Event Callback that     */
   /* handles all requests made to the HTS Service for all registered   */
   /* instances.                                                        */
static void BTPSAPI GATT_ServerEventCallback(unsigned int BluetoothStackID, GATT_Server_Event_Data_t *GATT_ServerEventData, unsigned long CallbackParameter)
{
   Word_t                     AttributeOffset;

#if ((BTPS_CONFIGURATION_HTS_SUPPORT_TEMPERATURE_TYPE) || (BTPS_CONFIGURATION_HTS_SUPPORT_MEASUREMENT_INTERVAL))

   Word_t                     InstanceTag;

#endif

   Word_t                     ValueLength;
   Byte_t                    *Value;
   Byte_t                     Event_Buffer[HTS_EVENT_DATA_SIZE + HTS_EVENT_DATA_BUFFER_SIZE];
   unsigned int               TransactionID;
   unsigned int               InstanceID;
   HTS_Event_Data_t          *EventData;
   HTSServerInstance_t       *ServiceInstance;
   HTS_Characteristic_Type_t  Characteristic_Type;

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
                     if((Health_Thermometer_Service[AttributeOffset].Attribute_Entry_Type == aetCharacteristicDescriptor16) && (AttributeOffset != HTS_MEASUREMENT_INTERVAL_VALID_RANGE_DESCRIPTOR))
                     {
                        EventData = FormatEventHeader(sizeof(Event_Buffer), Event_Buffer, etHTS_Server_Read_Client_Configuration_Request, InstanceID, GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->ConnectionID, &TransactionID, GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->ConnectionType, &(GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->RemoteDevice));
                        if(EventData)
                        {
                           /* Format the rest of the event.             */
                           EventData->Event_Data_Size = HTS_READ_CLIENT_CONFIGURATION_DATA_SIZE;

                           /* Determine the Correct Client Configuration*/
                           /* Type.                                     */
                           if(AttributeOffset == HTS_TEMPERATURE_MEASUREMENT_CCD_ATTRIBUTE_OFFSET)
                              EventData->Event_Data.HTS_Read_Client_Configuration_Data->ClientConfigurationType = ctTemperatureMeasurement;
                           else
                           {
                              if(AttributeOffset == HTS_INTERMEDIATE_TEMPERATURE_CCD_ATTRIBUTE_OFFSET)
                                 EventData->Event_Data.HTS_Read_Client_Configuration_Data->ClientConfigurationType = ctIntermediateTemperature;
                              else
                                 EventData->Event_Data.HTS_Read_Client_Configuration_Data->ClientConfigurationType = ctMeasurementInterval;
                           }

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
#if ((BTPS_CONFIGURATION_HTS_SUPPORT_TEMPERATURE_TYPE) || (BTPS_CONFIGURATION_HTS_SUPPORT_MEASUREMENT_INTERVAL))

                     else
                     {
                        /* Get the instance tag for the characteristic. */
                        InstanceTag = (Word_t)(((GATT_Characteristic_Value_16_Entry_t *)Health_Thermometer_Service[AttributeOffset].Attribute_Value)->Characteristic_Value_Length);
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
                     if(Health_Thermometer_Service[AttributeOffset].Attribute_Entry_Type == aetCharacteristicDescriptor16)
                     {
                        /* Begin formatting the Client Configuration    */
                        /* Update event.                                */
                        EventData = FormatEventHeader(sizeof(Event_Buffer), Event_Buffer, etHTS_Server_Client_Configuration_Update, InstanceID, GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->ConnectionID, NULL, GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->ConnectionType, &(GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->RemoteDevice));
                        if(EventData)
                        {
                           /* Format the rest of the event.             */
                           EventData->Event_Data_Size = HTS_CLIENT_CONFIGURATION_UPDATE_DATA_SIZE;

                           /* Determine the Correct Client Configuration*/
                           /* Type.                                     */
                           if(AttributeOffset == HTS_TEMPERATURE_MEASUREMENT_CCD_ATTRIBUTE_OFFSET)
                              EventData->Event_Data.HTS_Client_Configuration_Update_Data->ClientConfigurationType = ctTemperatureMeasurement;
                           else
                           {
                              if(AttributeOffset == HTS_INTERMEDIATE_TEMPERATURE_CCD_ATTRIBUTE_OFFSET)
                                 EventData->Event_Data.HTS_Client_Configuration_Update_Data->ClientConfigurationType = ctIntermediateTemperature;
                              else
                                 EventData->Event_Data.HTS_Client_Configuration_Update_Data->ClientConfigurationType = ctMeasurementInterval;
                           }

                           /* Attempt to decode the request Client      */
                           /* Configuration.                            */
                           if(!DecodeClientConfigurationValue(ValueLength, Value, &(EventData->Event_Data.HTS_Client_Configuration_Update_Data->ClientConfiguration)))
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

#if BTPS_CONFIGURATION_HTS_SUPPORT_MEASUREMENT_INTERVAL

                     else
                     {
                        /* Format and Dispatch the event.               */
                        EventData = FormatEventHeader(sizeof(Event_Buffer), Event_Buffer, etHTS_Measurement_Interval_Update, InstanceID, GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->ConnectionID, NULL, GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->ConnectionType, &(GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->RemoteDevice));
                        if(EventData)
                        {
                           /* Format the rest of the event.             */
                           EventData->Event_Data_Size = HTS_MEASUREMENT_INTERVAL_UPDATE_DATA_SIZE;

                           /* Parse out the command.                    */
                           if(!DecodeMeasurementInterval(InstanceID, ValueLength, Value, &(EventData->Event_Data.HTS_Measurement_Interval_Update_Data->NewMeasurementInterval)))
                           {
                              /* Since the requested Measurement        */
                              /* Interval was formatted correctly and   */
                              /* with the accepted range we will accept */
                              /* the new measurement interval.          */
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
                              GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, HTS_ERROR_CODE_OUT_OF_RANGE);
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
            case etGATT_Server_Confirmation_Response:
               /* Verify that the Event Data is valid.                  */
               if(GATT_ServerEventData->Event_Data.GATT_Confirmation_Data)
               {
                  /* Determine the type of the indication that has been */
                  /* confirmed.                                         */
                  if((ServiceInstance->TemperatureMeasurementIndicactionInfo.ConnectionID == GATT_ServerEventData->Event_Data.GATT_Confirmation_Data->ConnectionID) && ((unsigned int)ServiceInstance->TemperatureMeasurementIndicactionInfo.TransactionID == GATT_ServerEventData->Event_Data.GATT_Confirmation_Data->TransactionID))
                  {
                     ValueLength                                                         = 1;
                     Characteristic_Type                                                 = ctTemperatureMeasurement;
                     ServiceInstance->TemperatureMeasurementIndicactionInfo.ConnectionID = 0;
                  }
                  else
                  {
                     if((ServiceInstance->MeasurementIntervalIndicationInfo.ConnectionID == GATT_ServerEventData->Event_Data.GATT_Confirmation_Data->ConnectionID) && ((unsigned int)ServiceInstance->MeasurementIntervalIndicationInfo.TransactionID == GATT_ServerEventData->Event_Data.GATT_Confirmation_Data->TransactionID))
                     {
                        ValueLength                                                         = 1;
                        Characteristic_Type                                             = ctMeasurementInterval;
                        ServiceInstance->MeasurementIntervalIndicationInfo.ConnectionID = 0;
                     }
                     else
                     {
                        ValueLength = 0;
                     }
                  }

                  if(ValueLength)
                  {
                     /* Format the event.                               */
                     EventData = FormatEventHeader(sizeof(Event_Buffer), Event_Buffer, etHTS_Confirmation_Response, InstanceID, GATT_ServerEventData->Event_Data.GATT_Confirmation_Data->ConnectionID, NULL, GATT_ServerEventData->Event_Data.GATT_Confirmation_Data->ConnectionType, &(GATT_ServerEventData->Event_Data.GATT_Confirmation_Data->RemoteDevice));
                     if(EventData)
                     {
                        /* Format the rest of the event.                */
                        EventData->Event_Data_Size                                       = HTS_CONFIRMATION_DATA_SIZE;
                        EventData->Event_Data.HTS_Confirmation_Data->Characteristic_Type = Characteristic_Type;
                        EventData->Event_Data.HTS_Confirmation_Data->Status              = GATT_ServerEventData->Event_Data.GATT_Confirmation_Data->Status;

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
            case etGATT_Server_Device_Disconnection:
               /* Verify that the Event Data is valid.                  */
               if(GATT_ServerEventData->Event_Data.GATT_Device_Disconnection_Data)
               {
                  /* Check for an outstanding Temperature Measurement   */
                  /* indication.                                        */
                  if(ServiceInstance->TemperatureMeasurementIndicactionInfo.ConnectionID == GATT_ServerEventData->Event_Data.GATT_Device_Disconnection_Data->ConnectionID)
                  {
                     ServiceInstance->TemperatureMeasurementIndicactionInfo.ConnectionID = 0;
                  }

                  /* Check for an outstanding Measurement Interval      */
                  /* indication.                                        */
                  if(ServiceInstance->MeasurementIntervalIndicationInfo.ConnectionID == GATT_ServerEventData->Event_Data.GATT_Device_Disconnection_Data->ConnectionID)
                  {
                     ServiceInstance->MeasurementIntervalIndicationInfo.ConnectionID = 0;
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
   /* Bluetooth Stack HTS Module is Initialized correctly.  This        */
   /* function *MUST* be called before ANY other Bluetooth Stack HTS    */
   /* function can be called.  This function returns non-zero if the    */
   /* Module was initialized correctly, or a zero value if there was an */
   /* error.                                                            */
   /* * NOTE * Internally, this module will make sure that this function*/
   /*          has been called at least once so that the module will    */
   /*          function.  Calling this function from an external        */
   /*          location is not necessary.                               */
int InitializeHTSModule(void)
{
   return((int)InitializeModule());
}

   /* The following function is responsible for instructing the         */
   /* Bluetooth Stack HTSC Module to clean up any resources that it has */
   /* allocated.  Once this function has completed, NO other Bluetooth  */
   /* Stack HTS Functions can be called until a successful call to the  */
   /* InitializeHTSModule() function is made.  The parameter to this    */
   /* function specifies the context in which this function is being    */
   /* called.  If the specified parameter is TRUE, then the module will */
   /* make sure that NO functions that would require waiting/blocking on*/
   /* Mutexes/Events are called.  This parameter would be set to TRUE if*/
   /* this function was called in a context where threads would not be  */
   /* allowed to run.  If this function is called in the context where  */
   /* threads are allowed to run then this parameter should be set to   */
   /* FALSE.                                                            */
void CleanupHTSModule(Boolean_t ForceCleanup)
{
   /* Check to make sure that this module has been initialized.         */
   if(InstanceListInitialized)
   {
      /* Wait for access to the HTS Context List.                       */
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

   /* The following function is responsible for opening a HTS Server.   */
   /* The first parameter is the Bluetooth Stack ID on which to open the*/
   /* server.  The second parameter is the Callback function to call    */
   /* when an event occurs on this Server Port.  The third parameter is */
   /* a user-defined callback parameter that will be passed to the      */
   /* callback function with each event.  The final parameter is a      */
   /* pointer to store the GATT Service ID of the registered HTS        */
   /* service.  This can be used to include the service registered by   */
   /* this call.  This function returns the positive, non-zero, Instance*/
   /* ID or a negative error code.                                      */
   /* * NOTE * Only 1 HTS Server may be open at a time, per Bluetooth   */
   /*          Stack ID.                                                */
   /* * NOTE * All Client Requests will be dispatch to the EventCallback*/
   /*          function that is specified by the second parameter to    */
   /*          this function.                                           */
int BTPSAPI HTS_Initialize_Service(unsigned int BluetoothStackID, HTS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID)
{
   GATT_Attribute_Handle_Group_t ServiceHandleRange;

    /* Initialize the Service Handle Group to 0.                        */
   ServiceHandleRange.Starting_Handle = 0;
   ServiceHandleRange.Ending_Handle   = 0;

   return(HTSRegisterService(BluetoothStackID, EventCallback, CallbackParameter, ServiceID, &ServiceHandleRange));
}

   /* The following function is responsible for opening a HTS Server.   */
   /* The first parameter is the Bluetooth Stack ID on which to open the*/
   /* server.  The second parameter is the Callback function to call    */
   /* when an event occurs on this Server Port.  The third parameter is */
   /* a user-defined callback parameter that will be passed to the      */
   /* callback function with each event.  The fourth parameter is a     */
   /* pointer to store the GATT Service ID of the registered HTS        */
   /* service.  This can be used to include the service registered by   */
   /* this call.  The final parameter is a pointer, that on input can be*/
   /* used to control the location of the service in the GATT database, */
   /* and on ouput to store the service handle range.  This function    */
   /* returns the positive, non-zero, Instance ID or a negative error   */
   /* code.                                                             */
   /* * NOTE * Only 1 HTS Server may be open at a time, per Bluetooth   */
   /*          Stack ID.                                                */
   /* * NOTE * All Client Requests will be dispatch to the EventCallback*/
   /*          function that is specified by the second parameter to    */
   /*          this function.                                           */
int BTPSAPI HTS_Initialize_Service_Handle_Range(unsigned int BluetoothStackID, HTS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange)
{
   return(HTSRegisterService(BluetoothStackID, EventCallback, CallbackParameter, ServiceID, ServiceHandleRange));
}

   /* The following function is responsible for closing a previously    */
   /* opened HTS Server.  The first parameter is the Bluetooth Stack ID */
   /* on which to close the server.  The second parameter is the        */
   /* InstanceID that was returned from a successful call to            */
   /* HTS_Initialize_Service().  This function returns a zero if        */
   /* successful or a negative return error code if an error occurs.    */
int BTPSAPI HTS_Cleanup_Service(unsigned int BluetoothStackID, unsigned int InstanceID)
{
   int                   ret_val;
   HTSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID))
   {
      /* Acquire the specified HTS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Verify that the service is actually registered.             */
         if(ServiceInstance->ServiceID)
         {
            /* Call GATT to un-register the service.                    */
            GATT_Un_Register_Service(BluetoothStackID, ServiceInstance->ServiceID);

            /* mark the instance entry as being free.                   */
            BTPS_MemInitialize(ServiceInstance, 0, HTS_SERVER_INSTANCE_DATA_SIZE);

            /* return success to the caller.                            */
            ret_val = 0;
         }
         else
            ret_val = HTS_ERROR_INVALID_PARAMETER;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(BluetoothStackID);
      }
      else
         ret_val = HTS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = HTS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for querying the number of  */
   /* attributes that are contained in the HTS Service that is          */
   /* registered with a call to HTS_Initialize_Service().  This function*/
   /* returns the non-zero number of attributes that are contained in a */
   /* HTS Server or zero on failure.                                    */
unsigned int BTPSAPI HTS_Query_Number_Attributes(void)
{
   /* Simply return the number of attributes that are contained in a HTS*/
   /* service.                                                          */
   return(HEALTH_THERMOMETER_SERVICE_ATTRIBUTE_COUNT);
}

   /* The following function is responsible for setting the Temperature */
   /* Type on the specified HTS Instance.  The first parameter is the   */
   /* Bluetooth Stack ID of the Bluetooth Device.  The second parameter */
   /* is the InstanceID returned from a successful call to              */
   /* HTS_Initialize_Server().  The final parameter is the Temperature  */
   /* Type to set for the specified HTS Instance.  This function returns*/
   /* a zero if successful or a negative return error code if an error  */
   /* occurs.                                                           */
   /* * NOTE * The Temperature_Type parameter should be an enumerated   */
   /*          value of the form HTS_TEMPERATURE_TYPE_XXX.              */
int BTPSAPI HTS_Set_Temperature_Type(unsigned int BluetoothStackID, unsigned int InstanceID, Byte_t Temperature_Type)
{
#if BTPS_CONFIGURATION_HTS_SUPPORT_TEMPERATURE_TYPE

   int                  ret_val;
   HTSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID))
   {
      /* Acquire the specified HTS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Assign the Temperature Type for the specified instance.     */
         ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(InstanceData[InstanceID-1].Temperature_Type), Temperature_Type);

         /* Return success to the caller.                               */
         ret_val = 0;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = HTS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = HTS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);

#else

   return(BTPS_ERROR_FEATURE_NOT_AVAILABLE);

#endif
}

   /* The following function is responsible for querying the current    */
   /* Temperature Type on the specified HTS Instance.  The first        */
   /* parameter is the Bluetooth Stack ID of the Bluetooth Device.  The */
   /* second parameter is the InstanceID returned from a successful call*/
   /* to HTS_Initialize_Server().  The final parameter is a pointer to  */
   /* return the current Temperature Type for the specified HTS         */
   /* Instance.  This function returns a zero if successful or a        */
   /* negative return error code if an error occurs.                    */
int BTPSAPI HTS_Query_Temperature_Type(unsigned int BluetoothStackID, unsigned int InstanceID, Byte_t *Temperature_Type)
{
#if BTPS_CONFIGURATION_HTS_SUPPORT_TEMPERATURE_TYPE

   int                  ret_val;
   HTSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (Temperature_Type))
   {
      /* Acquire the specified HTS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Query the curent Temperature Type.                          */
         *Temperature_Type = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(InstanceData[InstanceID-1].Temperature_Type));

         /* Return success to the caller.                               */
         ret_val           = 0;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = HTS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = HTS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);

#else

   return(BTPS_ERROR_FEATURE_NOT_AVAILABLE);

#endif
}

   /* The following function is responsible for setting the Measurement */
   /* Interval on the specified HTS Instance.  The first parameter is   */
   /* the Bluetooth Stack ID of the Bluetooth Device.  The second       */
   /* parameter is the InstanceID returned from a successful call to    */
   /* HTS_Initialize_Server().  The final parameter is the Measurement  */
   /* Interval to set for the specified HTS Instance.  This function    */
   /* returns a zero if successful or a negative return error code if an*/
   /* error occurs.                                                     */
int BTPSAPI HTS_Set_Measurement_Interval(unsigned int BluetoothStackID, unsigned int InstanceID, Word_t Measurement_Interval)
{
#if BTPS_CONFIGURATION_HTS_SUPPORT_MEASUREMENT_INTERVAL

   int                  ret_val;
   HTSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID))
   {
      /* Acquire the specified HTS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Assign the measurement interval for the specified instance. */
         ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(InstanceData[InstanceID-1].Measurement_Interval), Measurement_Interval);

         /* Return success to the caller.                               */
         ret_val = 0;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = HTS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = HTS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);

#else

   return(BTPS_ERROR_FEATURE_NOT_AVAILABLE);

#endif
}

   /* The following function is responsible for querying the current    */
   /* Measurement Interval on the specified HTS Instance.  The first    */
   /* parameter is the Bluetooth Stack ID of the Bluetooth Device.  The */
   /* second parameter is the InstanceID returned from a successful call*/
   /* to HTS_Initialize_Server().  The final parameter is a pointer to  */
   /* return the current Measurement Interval for the specified HTS     */
   /* Instance.  This function returns a zero if successful or a        */
   /* negative return error code if an error occurs.                    */
int BTPSAPI HTS_Query_Measurement_Interval(unsigned int BluetoothStackID, unsigned int InstanceID, Word_t *Measurement_Interval)
{
#if BTPS_CONFIGURATION_HTS_SUPPORT_MEASUREMENT_INTERVAL

   int                  ret_val;
   HTSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (Measurement_Interval))
   {
      /* Acquire the specified HTS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Assign the measurement interval for the specified instance. */
         *Measurement_Interval = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(InstanceData[InstanceID-1].Measurement_Interval));

         /* Return success to the caller.                               */
         ret_val               = 0;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = HTS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = HTS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);

#else

   return(BTPS_ERROR_FEATURE_NOT_AVAILABLE);

#endif
}

   /* The following function is responsible for setting the Valid Range */
   /* descriptor value on the specified HTS Instance.  The first        */
   /* parameter is the Bluetooth Stack ID of the Bluetooth Device.  The */
   /* second parameter is the InstanceID returned from a successful call*/
   /* to HTS_Initialize_Server().  The final parameter is the Valid     */
   /* Range to set for the specified HTS Instance.  This function       */
   /* returns a zero if successful or a negative return error code if an*/
   /* error occurs.                                                     */
int BTPSAPI HTS_Set_Valid_Range(unsigned int BluetoothStackID, unsigned int InstanceID, HTS_Valid_Range_Data_t *ValidRange)
{
#if BTPS_CONFIGURATION_HTS_SUPPORT_MEASUREMENT_INTERVAL

   int                  ret_val;
   HTSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (ValidRange))
   {
      /* Acquire the specified HTS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Assign the measurement interval for the specified instance. */
         ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(InstanceData[InstanceID-1].Valid_Range.Lower_Bounds), ValidRange->Lower_Bounds);
         ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(InstanceData[InstanceID-1].Valid_Range.Upper_Bounds), ValidRange->Upper_Bounds);

         /* Return success to the caller.                               */
         ret_val = 0;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = HTS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = HTS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);

#else

   return(BTPS_ERROR_FEATURE_NOT_AVAILABLE);

#endif
}

   /* The following function is responsible for querying the Valid Range*/
   /* descriptor value on the specified HTS Instance.  The first        */
   /* parameter is the Bluetooth Stack ID of the Bluetooth Device.  The */
   /* second parameter is the InstanceID returned from a successful call*/
   /* to HTS_Initialize_Server().  The final parameter is a pointer to  */
   /* store the Valid Range structure for the specified HTS Instance.   */
   /* This function returns a zero if successful or a negative return   */
   /* error code if an error occurs.                                    */
int BTPSAPI HTS_Query_Valid_Range(unsigned int BluetoothStackID, unsigned int InstanceID, HTS_Valid_Range_Data_t *ValidRange)
{
#if BTPS_CONFIGURATION_HTS_SUPPORT_MEASUREMENT_INTERVAL

   int                  ret_val;
   HTSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (ValidRange))
   {
      /* Acquire the specified HTS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Assign the measurement interval for the specified instance. */
         ValidRange->Lower_Bounds = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(InstanceData[InstanceID-1].Valid_Range.Lower_Bounds));
         ValidRange->Upper_Bounds = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(InstanceData[InstanceID-1].Valid_Range.Upper_Bounds));

         /* Return success to the caller.                               */
         ret_val                  = 0;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = HTS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = HTS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);

#else

   return(BTPS_ERROR_FEATURE_NOT_AVAILABLE);

#endif
}

   /* The following function is responsible for responding to a HTS Read*/
   /* Client Configuration Request.  The first parameter is the         */
   /* Bluetooth Stack ID of the Bluetooth Device.  The second parameter */
   /* is the InstanceID returned from a successful call to              */
   /* HTS_Initialize_Server().  The third is the Transaction ID of the  */
   /* request.  The final parameter contains the Client Configuration to*/
   /* send to the remote device.  This function returns a zero if       */
   /* successful or a negative return error code if an error occurs.    */
int BTPSAPI HTS_Read_Client_Configuration_Response(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int TransactionID, Word_t Client_Configuration)
{
   int                  ret_val;
   Word_t               ValueLength;
   NonAlignedWord_t     ClientConfiguration;
   HTSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (TransactionID))
   {
      /* Acquire the specified HTS Instance.                            */
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
         ret_val = HTS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = HTS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for sending an Intermediate */
   /* Temperature notification to a specified remote device.  The first */
   /* parameter is the Bluetooth Stack ID of the Bluetooth Device.  The */
   /* second parameter is the InstanceID returned from a successful call*/
   /* to HTS_Initialize_Server().  The third parameter is the           */
   /* ConnectionID of the remote device to send the notification to.    */
   /* The final parameter is the Intermediate Temperature data to       */
   /* notify.  This function returns a zero if successful or a negative */
   /* return error code if an error occurs.                             */
int BTPSAPI HTS_Notify_Intermediate_Temperature(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int ConnectionID, HTS_Temperature_Measurement_Data_t *Temperature_Measurement)
{
#if BTPS_CONFIGURATION_HTS_SUPPORT_INTERMEDIATE_TEMPERATURE

   int                  ret_val;
   Byte_t              *NotificationData;
   unsigned int         NotificationDataLength;
   HTSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (ConnectionID) && (Temperature_Measurement))
   {
      /* Acquire the specified HTS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Calculate the required length for the temperature           */
         /* measurement.                                                */
         NotificationDataLength = HTS_TEMPERATURE_MEASUREMENT_LENGTH(Temperature_Measurement->Flags);

         /* Allocate a buffer for the notification.                     */
         if((NotificationData = BTPS_AllocateMemory(NotificationDataLength)) != NULL)
         {
            /* Format the notification.                                 */
            if(!(ret_val = FormatTemperatureMeasurement(Temperature_Measurement, NotificationDataLength, NotificationData)))
            {
               /* Attempt to send the notification.                     */
               ret_val = GATT_Handle_Value_Notification(ServiceInstance->BluetoothStackID, ServiceInstance->ServiceID, ConnectionID, HTS_INTERMEDIATE_TEMPERATURE_ATTRIBUTE_OFFSET, (Word_t)NotificationDataLength, (Byte_t *)NotificationData);
               if(ret_val > 0)
                  ret_val = 0;
            }

            /* Free the memory allocated for the Notification Data.     */
            BTPS_FreeMemory(NotificationData);
         }
         else
            ret_val = HTS_ERROR_INSUFFICIENT_RESOURCES;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = HTS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = HTS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);

#else

   return(BTPS_ERROR_FEATURE_NOT_AVAILABLE);

#endif
}

   /* The following function is responsible for sending a Measurement   */
   /* Interval indication to a specified remote device.  The first      */
   /* parameter is the Bluetooth Stack ID of the Bluetooth Device.  The */
   /* second parameter is the InstanceID returned from a successful call*/
   /* to HTS_Initialize_Server().  The third parameter is the           */
   /* ConnectionID of the remote device to send the notification to.    */
   /* This function returns a zero if successful or a negative return   */
   /* error code if an error occurs.                                    */
   /* * NOTE * Only 1 Measurement Interval indication may be outstanding*/
   /*          per HTS Instance.                                        */
int BTPSAPI HTS_Indicate_Measurement_Interval(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int ConnectionID)
{
#if BTPS_CONFIGURATION_HTS_SUPPORT_MEASUREMENT_INTERVAL

   int                  ret_val;
   unsigned int         DataLength;
   HTSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (ConnectionID))
   {
      /* Acquire the specified HTS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Verify that no Measurement Interval Indication is           */
         /* outstanding.                                                */
         if(!(ServiceInstance->MeasurementIntervalIndicationInfo.ConnectionID))
         {
            /* Get the length of the interval.                          */
            DataLength = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(InstanceData[InstanceID-1].Measurement_Interval_Length));

            /* Indicate the data.                                       */
            ret_val = GATT_Handle_Value_Indication(ServiceInstance->BluetoothStackID, ServiceInstance->ServiceID, ConnectionID, HTS_MEASUREMENT_INTERVAL_ATTRIBUTE_OFFSET, (Word_t)DataLength, (Byte_t *)&(InstanceData[InstanceID-1].Measurement_Interval));
            if(ret_val > 0)
            {
               /* Save the Transaction ID of the Indication.            */
               ServiceInstance->MeasurementIntervalIndicationInfo.ConnectionID  = ConnectionID;
               ServiceInstance->MeasurementIntervalIndicationInfo.TransactionID = (unsigned int)ret_val;
               ret_val                                                          = 0;
            }
         }
         else
            ret_val = HTS_ERROR_INDICATION_OUTSTANDING;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = HTS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = HTS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);

#else

   return(BTPS_ERROR_FEATURE_NOT_AVAILABLE);

#endif
}

   /* The following function is responsible for sending a Temperature   */
   /* Measurement indication to a specified remote device.  The first   */
   /* parameter is the Bluetooth Stack ID of the Bluetooth Device.  The */
   /* second parameter is the InstanceID returned from a successful call*/
   /* to HTS_Initialize_Server().  The third parameter is the           */
   /* ConnectionID of the remote device to send the indication to.  The */
   /* final parameter is the Temperature Measurement data to indicate.  */
   /* This function returns a zero if successful or a negative return   */
   /* error code if an error occurs.                                    */
   /* * NOTE * Only 1 Temperature Measurement indication may be         */
   /*          outstanding per HTS Instance.                            */
int BTPSAPI HTS_Indicate_Temperature_Measurement(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int ConnectionID, HTS_Temperature_Measurement_Data_t *Temperature_Measurement)
{
   int                  ret_val;
   Byte_t              *IndicationData;
   unsigned int         IndicationDataLength;
   HTSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (ConnectionID) && (Temperature_Measurement))
   {
      /* Acquire the specified HTS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Verify that no Temperature Measurement Indication is        */
         /* outstanding.                                                */
         if(!(ServiceInstance->TemperatureMeasurementIndicactionInfo.ConnectionID))
         {
            /* Calculate the required length for the temperature        */
            /* measurement.                                             */
            IndicationDataLength = HTS_TEMPERATURE_MEASUREMENT_LENGTH(Temperature_Measurement->Flags);

            /* Allocate a buffer for the notification.                  */
            if((IndicationData = BTPS_AllocateMemory(IndicationDataLength)) != NULL)
            {
               /* Format the notification.                              */
               if(!(ret_val = FormatTemperatureMeasurement(Temperature_Measurement, IndicationDataLength, IndicationData)))
               {
                  /* Attempt to send the notification.                  */
                  ret_val = GATT_Handle_Value_Indication(ServiceInstance->BluetoothStackID, ServiceInstance->ServiceID, ConnectionID, HTS_TEMPERATURE_MEASUREMENT_ATTRIBUTE_OFFSET, (Word_t)IndicationDataLength, (Byte_t *)IndicationData);
                  if(ret_val > 0)
                  {
                     /* Save the Transaction ID of the Indication.      */
                     ServiceInstance->TemperatureMeasurementIndicactionInfo.ConnectionID  = ConnectionID;
                     ServiceInstance->TemperatureMeasurementIndicactionInfo.TransactionID = (unsigned int)ret_val;
                     ret_val                                                              = 0;
                  }
               }

               /* Free the memory allocated for the Indication Data.    */
               BTPS_FreeMemory(IndicationData);
            }
            else
               ret_val = HTS_ERROR_INSUFFICIENT_RESOURCES;
         }
         else
            ret_val = HTS_ERROR_INDICATION_OUTSTANDING;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = HTS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = HTS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}


   /* HTS Client API.                                                   */

   /* The following function is responsible for parsing a value received*/
   /* from a remote HTS Server interpreting it as a Temperature         */
   /* Measurement characteristic.  The first parameter is the length of */
   /* the value returned by the remote HTS Server.  The second parameter*/
   /* is a pointer to the data returned by the remote HTS Server.  The  */
   /* final parameter is a pointer to store the parsed Temperature      */
   /* Measurement value.  This function returns a zero if successful or */
   /* a negative return error code if an error occurs.                  */
int BTPSAPI HTS_Decode_Temperature_Measurement(unsigned int ValueLength, Byte_t *Value, HTS_Temperature_Measurement_Data_t *TemperatureMeasurement)
{
   /* Call the internal function to decode the Temperature Measurement. */
   return(DecodeTemperatureMeasurement(ValueLength, Value, TemperatureMeasurement));
}

   /* The following function is responsible for parsing a value received*/
   /* from a remote HTS Server interpreting it as a Valid Range         */
   /* descriptor.  The first parameter is the length of the value       */
   /* returned by the remote HTS Server.  The second parameter is a     */
   /* pointer to the data returned by the remote HTS Server.  The final */
   /* parameter is a pointer to store the parsed Valid Range value.     */
   /* This function returns a zero if successful or a negative return   */
   /* error code if an error occurs.                                    */
int BTPSAPI HTS_Decode_Valid_Range(unsigned int ValueLength, Byte_t *Value, HTS_Valid_Range_Data_t *ValidRange)
{
   /* Call the internal function to decode the Temperature Measurement. */
   return(DecodeValidRange(ValueLength, Value, ValidRange));
}
