/*****< gaps.c >***************************************************************/
/*      Copyright 2011 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  GAPS - Bluetooth Stack Generic Access Profile Service (GATT Based) for    */
/*         Stonestreet One Bluetooth Protocol Stack.                          */
/*                                                                            */
/*  Author:  Tim Cook                                                         */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   09/30/11  T. Cook        Initial creation.                               */
/******************************************************************************/
#include "SS1BTPS.h"        /* Bluetooth Stack API Prototypes/Constants.      */
#include "SS1BTGAT.h"       /* Bluetooth Stack GATT API Prototypes/Constants. */
#include "SS1BTGAP.h"       /* Bluetooth GAPS API Prototypes/Constants.       */

#include "BTPSKRNL.h"       /* BTPS Kernel Prototypes/Constants.              */
#include "GAPS.h"           /* Bluetooth APS Prototypes/Constants.            */

   /* The following controls the number of supported GAPS instances.    */
#define GAPS_MAXIMUM_SUPPORTED_INSTANCES                          (BTPS_CONFIGURATION_GAPS_MAXIMUM_SUPPORTED_INSTANCES)

   /* The following MACRO is a utility MACRO that exists to aid in      */
   /* converting an unsigned long in milliseconds to 2 Baseband slots.  */
#define CONVERT_TO_TWO_BASEBAND_SLOTS(_x)                         ((unsigned long)((((4000L * (_x)) / 500L) + 5L)/10L))

   /* The following MACRO is a utility MACRO that exists to aid in      */
   /* converting 2 Baseband slots to an unsigned long in milliseconds.  */
#define CONVERT_FROM_TWO_BASEBAND_SLOTS(_x)                       ((unsigned long)((((5000L * (_x)) / 400L) + 5L)/10L))

   /* The following MACRO is a utility MACRO that exists to aid in      */
   /* converting a Millisecond value to a HCI LE Link Supervision       */
   /* Timeout which is specified in units of 10 ms.                     */
#define CONVERT_TO_LINK_SUPERVISION_TIMEOUT(_x)                   ((unsigned long)((_x) / 10))

   /* The following MACRO is a utility MACRO that exists to aid in      */
   /* converting a HCI LE Link Supervision Timeout which is specified in*/
   /* units of 10 ms to milliseconds.                                   */
#define CONVERT_FROM_LINK_SUPERVISION_TIMEOUT(_x)                 ((unsigned long)((_x) * 10))

   /* The following MACRO is a utility MACRO that exists to aid in      */
   /* verifying a specified Connection Parameter against an inclusive   */
   /* Min & Max, and a don't care value.                                */
#define VERIFY_CONNECTION_PARAMETER(_x, _Min, _Max)        ((((_x) >= (_Min)) && ((_x) <= (_Max))) || ((_x) == GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_NO_SPECIFIC_PREFERRED))

   /* The following structure defines the Instance Data that must be    */
   /* unique for each GAP service registered (Only 1 per Bluetooth      */
   /* Stack).                                                           */
typedef __PACKED_STRUCT_BEGIN__ struct _tagGAP_Instance_Data_t
{
   NonAlignedWord_t                                 DeviceAppearanceLength;
   NonAlignedWord_t                                 DeviceAppearanceInstance;
   NonAlignedWord_t                                 DeviceNameLength;
   NonAlignedByte_t                                 DeviceNameInstance[GAP_MAXIMUM_DEVICE_NAME_LENGTH];

#if BTPS_CONFIGURATION_GAPS_SUPPORT_PREFERRED_CONNECTION_PARAMETERS

   NonAlignedWord_t                                 PreferredConnectionParametersLength;
   GAP_Peripheral_Preferred_Connection_Parameters_t PreferredConnectionParameters;

#endif

} __PACKED_STRUCT_END__ GAP_Instance_Data_t;

#define GAP_INSTANCE_DATA_SIZE                           (sizeof(GAP_Instance_Data_t))

   /* The following define the instance tags for each GAP Service data  */
   /* that is unique per registered service.                            */
#define GAP_DEVICE_APPEARANCE_INSTANCE_TAG_VALUE         (BTPS_STRUCTURE_OFFSET(GAP_Instance_Data_t, DeviceAppearanceLength))
#define GAP_DEVICE_NAME_INSTANCE_TAG_VALUE               (BTPS_STRUCTURE_OFFSET(GAP_Instance_Data_t, DeviceNameLength))

#if BTPS_CONFIGURATION_GAPS_SUPPORT_PREFERRED_CONNECTION_PARAMETERS

   #define GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_TAG_VALUE  (BTPS_STRUCTURE_OFFSET(GAP_Instance_Data_t, PreferredConnectionParametersLength))

#endif

   /*********************************************************************/
   /**              Generic Access Profile Service Table               **/
   /*********************************************************************/

   /* The Generic Access Profile Service Declaration UUID.              */
static BTPSCONST GATT_Primary_Service_16_Entry_t GAP_Service_UUID =
{
   GAP_SERVICE_BLUETOOTH_UUID_CONSTANT
};

   /* The Device Name Characteristic Declaration.                       */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t GAP_Device_Name_Declaration =
{
   GATT_CHARACTERISTIC_PROPERTIES_READ,
   GAP_DEVICE_NAME_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};


   /* The Device Name Characteristic Value.                             */
static GATT_Characteristic_Value_16_Entry_t  GAP_Device_Name_Value =
{
   GAP_DEVICE_NAME_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   GAP_DEVICE_NAME_INSTANCE_TAG_VALUE,
   NULL
};

   /* The Device Appearence Characteristic Declaration.                 */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t GAP_Device_Appearence_Declaration =
{
   GATT_CHARACTERISTIC_PROPERTIES_READ,
   GAP_DEVICE_APPEARANCE_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The Device Appearence Characteristic Value.                       */
static GATT_Characteristic_Value_16_Entry_t  GAP_Device_Appearence_Value =
{
   GAP_DEVICE_APPEARANCE_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   GAP_DEVICE_APPEARANCE_INSTANCE_TAG_VALUE,
   NULL
};

#if BTPS_CONFIGURATION_GAPS_SUPPORT_PREFERRED_CONNECTION_PARAMETERS

   /* The Peripheral Preferred Connection Parameters Characteristic     */
   /* Declaration.                                                      */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t GAP_Preferred_Connection_Parameters_Declaration =
{
   GATT_CHARACTERISTIC_PROPERTIES_READ,
   GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The Peripheral Preferred Connection Parameters Characteristic     */
   /* Value.                                                            */
static GATT_Characteristic_Value_16_Entry_t  GAP_Preferred_Connection_Parameters_Value =
{
   GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_TAG_VALUE,
   NULL
};

#endif

   /* The following defines the Immediate Service that is registered    */
   /* with the GATT_Register_Service function call.                     */
   /* * NOTE * This array will be registered with GATT in the call to   */
   /*          GATT_Register_Service.                                   */
BTPSCONST GATT_Service_Attribute_Entry_t Generic_Access_Profile_Service[] =
{
   {GATT_ATTRIBUTE_FLAGS_READABLE, aetPrimaryService16,            (Byte_t *)&GAP_Service_UUID},                                /* GAP Primary Service Declaration.                                */
   {GATT_ATTRIBUTE_FLAGS_READABLE, aetCharacteristicDeclaration16, (Byte_t *)&GAP_Device_Name_Declaration},                     /* GAP Device Name Characteristic Declaration.                     */
   {GATT_ATTRIBUTE_FLAGS_READABLE, aetCharacteristicValue16,       (Byte_t *)&GAP_Device_Name_Value},                           /* GAP Device Name Value.                                          */
   {GATT_ATTRIBUTE_FLAGS_READABLE, aetCharacteristicDeclaration16, (Byte_t *)&GAP_Device_Appearence_Declaration},               /* GAP Device Appearence Characteristic Declaration.               */
   {GATT_ATTRIBUTE_FLAGS_READABLE, aetCharacteristicValue16,       (Byte_t *)&GAP_Device_Appearence_Value},                     /* GAP Device Appearence Value.                                    */

#if BTPS_CONFIGURATION_GAPS_SUPPORT_PREFERRED_CONNECTION_PARAMETERS

   {GATT_ATTRIBUTE_FLAGS_READABLE, aetCharacteristicDeclaration16, (Byte_t *)&GAP_Preferred_Connection_Parameters_Declaration}, /* GAP Preferred Connection Parameters Characteristic Declaration. */
   {GATT_ATTRIBUTE_FLAGS_READABLE, aetCharacteristicValue16,       (Byte_t *)&GAP_Preferred_Connection_Parameters_Value},       /* GAP Preferred Connection Parameters Value.                      */

#endif
};

#define GENERIC_ACCESS_PROFILE_SERVICE_ATTRIBUTE_COUNT    (sizeof(Generic_Access_Profile_Service)/sizeof(GATT_Service_Attribute_Entry_t))

   /* The following define the various Attribute Offsets in the GAP     */
   /* Service Table (declared in GAPSrv.h).                             */
#define GAP_SERVICE_DECLARATION_ATTRIBUTE_OFFSET            0x00
#define GAP_DEVICE_NAME_DECLARATION_ATTRIBUTE_OFFSET        0x01
#define GAP_DEVICE_NAME_VALUE_ATTRIBUTE_OFFSET              0x02
#define GAP_DEVICE_APPEARENCE_DECLARATION_ATTRIBUTE_OFFSET  0x03
#define GAP_DEVICE_APPEARENCE_VALUE_ATTRIBUTE_OFFSET        0x04

   /* The following defines the GAP GATT Service Flags MASK that should */
   /* be passed into GATT_Register_Service when the GAP Service is      */
   /* registered.                                                       */
#define GAP_SERVICE_FLAGS                                (GATT_SERVICE_FLAGS_LE_SERVICE | GATT_SERVICE_FLAGS_BR_EDR_SERVICE)

   /*********************************************************************/
   /**                    END OF SERVICE TABLE                         **/
   /*********************************************************************/

   /* GAPS Service Instance Block.  This structure contains All         */
   /* information associated with a specific Bluetooth Stack ID (member */
   /* is present in this structure).                                    */
typedef struct _tagGAPSServerInstance_t
{
   unsigned int          BluetoothStackID;
   unsigned int          ServiceID;
} GAPSServerInstance_t;

#define GAPS_SERVER_INSTANCE_DATA_SIZE                    (sizeof(GAPSServerInstance_t))

   /* Internal Variables to this Module (Remember that all variables    */
   /* declared static are initialized to 0 automatically by the         */
   /* compiler as part of standard C/C++).                              */

static GAP_Instance_Data_t InstanceData[GAPS_MAXIMUM_SUPPORTED_INSTANCES];
                                            /* Variable which holds all */
                                            /* data that is unique for  */
                                            /* each service instance.   */

static GAPSServerInstance_t InstanceList[GAPS_MAXIMUM_SUPPORTED_INSTANCES];
                                            /* Variable which holds the */
                                            /* service instance data.   */

static Boolean_t InstanceListInitialized;   /* Variable that flags that */
                                            /* is used to denote that   */
                                            /* this module has been     */
                                            /* successfully initialized.*/

   /* The following are the prototypes of local functions.              */
static Boolean_t InitializeModule(void);
static void CleanupModule(void);

static int DecodePreferredConnectionParameters(unsigned int BufferLength, Byte_t *Value, GAP_Preferred_Connection_Parameters_t *PreferredConnectionParameters);

#if BTPS_CONFIGURATION_GAPS_SUPPORT_PREFERRED_CONNECTION_PARAMETERS

static int FormatPreferredConnectionParameters(GAP_Preferred_Connection_Parameters_t *PreferredConnectionParameters, GAP_Peripheral_Preferred_Connection_Parameters_t *PackedPreferredConnectionParameters);

#endif

static Boolean_t InstanceRegisteredByStackID(unsigned int BluetoothStackID);
static GAPSServerInstance_t *AcquireServiceInstance(unsigned int BluetoothStackID, unsigned int *InstanceID);

static int GAPSRegisterService(unsigned int BluetoothStackID, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange);

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

   /* The following function is a utility function that is provided to  */
   /* allow a mechanism of parsing a packed Preferred Connection        */
   /* Parameter structure into the GAP_Preferred_Connection_Parameters_t*/
   /* structure.  This function returns ZERO on success or a negative   */
   /* error code.                                                       */
static int DecodePreferredConnectionParameters(unsigned int BufferLength, Byte_t *Value, GAP_Preferred_Connection_Parameters_t *PreferredConnectionParameters)
{
   int    ret_val;
   Word_t Temp;

   /* Verify the input parameters.                                      */
   if((BufferLength == GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_DATA_SIZE) && (Value) && (PreferredConnectionParameters))
   {
      /* The input parameters are correct so go ahead and decode the    */
      /* value.                                                         */

      /* Decode the Minimum Connection Interval.                        */
      Temp = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(((GAP_Peripheral_Preferred_Connection_Parameters_t *)Value)->Minimum_Connection_Interval));
      if(Temp != GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_NO_SPECIFIC_PREFERRED)
         Temp = (Word_t)CONVERT_FROM_TWO_BASEBAND_SLOTS(Temp);

      PreferredConnectionParameters->Minimum_Connection_Interval = Temp;

      /* Decode the Maximum Connection Interval.                        */
      Temp = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(((GAP_Peripheral_Preferred_Connection_Parameters_t *)Value)->Maximum_Connection_Interval));
      if(Temp != GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_NO_SPECIFIC_PREFERRED)
         Temp = (Word_t)CONVERT_FROM_TWO_BASEBAND_SLOTS(Temp);

      PreferredConnectionParameters->Maximum_Connection_Interval = Temp;

      /* Decode the Slave Latency.                                      */
      PreferredConnectionParameters->Slave_Latency = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(((GAP_Peripheral_Preferred_Connection_Parameters_t *)Value)->Slave_Latency));

      /* Decode the Supervision Timeout.                                */
      Temp = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(((GAP_Peripheral_Preferred_Connection_Parameters_t *)Value)->Supervision_Timeout_Multiplier));
      if(Temp != GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_NO_SPECIFIC_PREFERRED)
         Temp = (Word_t)CONVERT_FROM_LINK_SUPERVISION_TIMEOUT(Temp);

      PreferredConnectionParameters->Supervision_Timeout = Temp;

      /* Return success to the caller.                                  */
      ret_val = 0;
   }
   else
   {
      if(BufferLength != GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_DATA_SIZE)
         ret_val = GAPS_ERROR_MALFORMATTED_DATA;
      else
         ret_val = GAPS_ERROR_INVALID_PARAMETER;
   }

   return(ret_val);
}

#if BTPS_CONFIGURATION_GAPS_SUPPORT_PREFERRED_CONNECTION_PARAMETERS

   /* The following function is a utility function that is provided to  */
   /* allow a formatting a GAP_Preferred_Connection_Parameters_t        */
   /* structure into a packed                                           */
   /* GAP_Peripheral_Preferred_Connection_Parameters_t structure.  This */
   /* function returns ZERO on success or a negative error code.        */
static int FormatPreferredConnectionParameters(GAP_Preferred_Connection_Parameters_t *PreferredConnectionParameters, GAP_Peripheral_Preferred_Connection_Parameters_t *PackedPreferredConnectionParameters)
{
   int    ret_val;
   Word_t Temp;

   /* Verify the input parameters.                                      */
   if((PreferredConnectionParameters) && (PackedPreferredConnectionParameters))
   {
      /* Verify the Minimum and Maximum Connection Interval.            */
      if((VERIFY_CONNECTION_PARAMETER(PreferredConnectionParameters->Minimum_Connection_Interval, MINIMUM_MINIMUM_CONNECTION_INTERVAL, MAXIMUM_MINIMUM_CONNECTION_INTERVAL)) && (VERIFY_CONNECTION_PARAMETER(PreferredConnectionParameters->Maximum_Connection_Interval, MINIMUM_MAXIMUM_CONNECTION_INTERVAL, MAXIMUM_MAXIMUM_CONNECTION_INTERVAL)))
      {
         /* Verify the Supervision Timeout and the Slave Latency.       */
         if(((PreferredConnectionParameters->Slave_Latency >= MINIMUM_SLAVE_LATENCY) && (PreferredConnectionParameters->Slave_Latency <= MAXIMUM_SLAVE_LATENCY)) && (VERIFY_CONNECTION_PARAMETER(PreferredConnectionParameters->Supervision_Timeout, MINIMUM_LINK_SUPERVISION_TIMEOUT, MAXIMUM_LINK_SUPERVISION_TIMEOUT)))
         {
            /* Format the packed structure.                             */
            Temp = PreferredConnectionParameters->Minimum_Connection_Interval;
            if(Temp != GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_NO_SPECIFIC_PREFERRED)
               Temp = (Word_t)CONVERT_TO_TWO_BASEBAND_SLOTS(Temp);

            ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(PackedPreferredConnectionParameters->Minimum_Connection_Interval), Temp);

            Temp = PreferredConnectionParameters->Maximum_Connection_Interval;
            if(Temp != GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_NO_SPECIFIC_PREFERRED)
               Temp = (Word_t)CONVERT_TO_TWO_BASEBAND_SLOTS(Temp);

            ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(PackedPreferredConnectionParameters->Maximum_Connection_Interval), Temp);

            Temp = PreferredConnectionParameters->Supervision_Timeout;
            if(Temp != GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_NO_SPECIFIC_PREFERRED)
               Temp = (Word_t)CONVERT_TO_LINK_SUPERVISION_TIMEOUT(Temp);

            ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(PackedPreferredConnectionParameters->Supervision_Timeout_Multiplier), Temp);
            ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(PackedPreferredConnectionParameters->Slave_Latency), PreferredConnectionParameters->Slave_Latency);

            /* Return success to the caller.                            */
            ret_val = 0;
         }
         else
            ret_val = GAPS_ERROR_INVALID_PARAMETER;
      }
      else
         ret_val = GAPS_ERROR_INVALID_PARAMETER;
   }
   else
      ret_val = GAPS_ERROR_INVALID_PARAMETER;

   return(ret_val);
}

#endif

   /* The following function is a utility function that exists to check */
   /* to see if an instance has already been registered for a specified */
   /* Bluetooth Stack ID.                                               */
   /* * NOTE * Since this is an internal function no check is done on   */
   /*          the input parameters.                                    */
static Boolean_t InstanceRegisteredByStackID(unsigned int BluetoothStackID)
{
   Boolean_t    ret_val = FALSE;
   unsigned int Index;

   for(Index=0;Index<GAPS_MAXIMUM_SUPPORTED_INSTANCES;Index++)
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
static GAPSServerInstance_t *AcquireServiceInstance(unsigned int BluetoothStackID, unsigned int *InstanceID)
{
   unsigned int          LocalInstanceID;
   unsigned int          Index;
   GAPSServerInstance_t *ret_val = NULL;

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
         if((LocalInstanceID) && (LocalInstanceID <= GAPS_MAXIMUM_SUPPORTED_INSTANCES))
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
               for(Index=0;Index<GAPS_MAXIMUM_SUPPORTED_INSTANCES;Index++)
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
   /* register an GAPS Service.  This function returns the positive,    */
   /* non-zero, Instance ID on success or a negative error code.        */
static int GAPSRegisterService(unsigned int BluetoothStackID, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange)
{
   int                   ret_val;
   unsigned int          InstanceID;
   GAPSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (ServiceID))
   {
      /* Verify that no instance is registered to this Bluetooth Stack. */
      if(!InstanceRegisteredByStackID(BluetoothStackID))
      {
         /* Acquire a free GAPS Instance.                               */
         InstanceID = 0;
         if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
         {
            /* Call GATT to register the PASS service.                  */
            ret_val = GATT_Register_Service(BluetoothStackID, GAP_SERVICE_FLAGS, GENERIC_ACCESS_PROFILE_SERVICE_ATTRIBUTE_COUNT, (GATT_Service_Attribute_Entry_t *)Generic_Access_Profile_Service, ServiceHandleRange, GATT_ServerEventCallback, InstanceID);
            if(ret_val > 0)
            {
               /* Save the Instance information.                        */
               ServiceInstance->BluetoothStackID = BluetoothStackID;
               ServiceInstance->ServiceID        = (unsigned int)ret_val;
               *ServiceID                        = (unsigned int)ret_val;

               /* Intilize the Instance Data for this instance.         */
               BTPS_MemInitialize(&InstanceData[InstanceID-1], 0, GAP_INSTANCE_DATA_SIZE);
               ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(InstanceData[InstanceID-1].DeviceAppearanceLength), GAP_DEVICE_APPEARENCE_VALUE_LENGTH);
               ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(InstanceData[InstanceID-1].DeviceNameLength), 0);

#if BTPS_CONFIGURATION_GAPS_SUPPORT_PREFERRED_CONNECTION_PARAMETERS

               ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(InstanceData[InstanceID-1].PreferredConnectionParametersLength), GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_DATA_SIZE);
               ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(InstanceData[InstanceID-1].PreferredConnectionParameters.Minimum_Connection_Interval), GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_NO_SPECIFIC_PREFERRED);
               ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(InstanceData[InstanceID-1].PreferredConnectionParameters.Maximum_Connection_Interval), GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_NO_SPECIFIC_PREFERRED);
               ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(InstanceData[InstanceID-1].PreferredConnectionParameters.Supervision_Timeout_Multiplier), GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_NO_SPECIFIC_PREFERRED);

#endif

               /* Return the GAPS Instance ID.                          */
               ret_val                           = (int)InstanceID;
            }

            /* UnLock the previously locked Bluetooth Stack.            */
            BSC_UnLockBluetoothStack(BluetoothStackID);
         }
         else
            ret_val = GAPS_ERROR_INSUFFICIENT_RESOURCES;
      }
      else
         ret_val = GAPS_ERROR_SERVICE_ALREADY_REGISTERED;
   }
   else
      ret_val = GAPS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}


   /* The following function is the GATT Server Event Callback that     */
   /* handles all requests made to the PASS Service for all registered  */
   /* instances.                                                        */
static void BTPSAPI GATT_ServerEventCallback(unsigned int BluetoothStackID, GATT_Server_Event_Data_t *GATT_ServerEventData, unsigned long CallbackParameter)
{
   Word_t                AttributeOffset;
   Word_t                InstanceTag;
   Word_t                ValueLength;
   Byte_t               *Value;
   unsigned int          TransactionID;
   unsigned int          InstanceID;
   GAPSServerInstance_t *ServiceInstance;

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

                  /* Get the Instance Tag and the length of the value   */
                  /* that is being read.                                */
                  InstanceTag = (Word_t)(((GATT_Characteristic_Value_16_Entry_t *)Generic_Access_Profile_Service[AttributeOffset].Attribute_Value)->Characteristic_Value_Length);
                  ValueLength = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(((Byte_t *)(&InstanceData[InstanceID-1]))[InstanceTag]));

                  /* Verify that they are not trying to write with an   */
                  /* offset or using preprared writes.                  */
                  if(GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->AttributeValueOffset <= ValueLength)
                  {
                     /* Calculate the length of the value from the      */
                     /* specified offset.                               */
                     ValueLength -= GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->AttributeValueOffset;

                     /* Get a pointer to the value at the specified     */
                     /* offset.                                         */
                     Value        = (Byte_t *)(&(((Byte_t *)(&InstanceData[InstanceID-1]))[InstanceTag + WORD_SIZE + GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->AttributeValueOffset]));

                     /* Respond with the data.                          */
                     GATT_Read_Response(BluetoothStackID, TransactionID, (unsigned int)ValueLength, Value);
                  }
                  else
                     GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, ATT_PROTOCOL_ERROR_CODE_INVALID_OFFSET);
               }
               break;
            case etGATT_Server_Write_Request:
               /* For now we are not going to allow writable            */
               /* characteristics in the GAP Service.  In the future we */
               /* may add the Reconnection Address which is writable or */
               /* allow the Device Name to be written.                  */
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
   /* Bluetooth Stack GAPS Module is Initialized correctly.  This       */
   /* function *MUST* be called before ANY other Bluetooth Stack GAPS   */
   /* function can be called.  This function returns non-zero if the    */
   /* Module was initialized correctly, or a zero value if there was an */
   /* error.                                                            */
   /* * NOTE * Internally, this module will make sure that this function*/
   /*          has been called at least once so that the module will    */
   /*          function.  Calling this function from an external        */
   /*          location is not necessary.                               */
int InitializeGAPSModule(void)
{
   return((int)InitializeModule());
}

   /* The following function is responsible for instructing the         */
   /* Bluetooth Stack GAPS Module to clean up any resources that it has */
   /* allocated.  Once this function has completed, NO other Bluetooth  */
   /* Stack GAPS Functions can be called until a successful call to the */
   /* InitializeGAPSModule() function is made.  The parameter to this   */
   /* function specifies the context in which this function is being    */
   /* called.  If the specified parameter is TRUE, then the module will */
   /* make sure that NO functions that would require waiting/blocking on*/
   /* Mutexes/Events are called.  This parameter would be set to TRUE if*/
   /* this function was called in a context where threads would not be  */
   /* allowed to run.  If this function is called in the context where  */
   /* threads are allowed to run then this parameter should be set to   */
   /* FALSE.                                                            */
void CleanupGAPSModule(Boolean_t ForceCleanup)
{
   /* Check to make sure that this module has been initialized.         */
   if(InstanceListInitialized)
   {
      /* Wait for access to the GAPS Context List.                      */
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

   /* The following function is responsible for opening a GAPS Server.  */
   /* The first parameter is the Bluetooth Stack ID on which to open the*/
   /* server.  The final parameter is a pointer to store the GATT       */
   /* Service ID of the registered GAPS service.  This can be used to   */
   /* include the service registered by this call.  This function       */
   /* returns the positive, non-zero, Instance ID or a negative error   */
   /* code.                                                             */
   /* * NOTE * Only 1 GAPS Server may be open at a time, per Bluetooth  */
   /*          Stack ID.                                                */
int BTPSAPI GAPS_Initialize_Service(unsigned int BluetoothStackID, unsigned int *ServiceID)
{
   GATT_Attribute_Handle_Group_t ServiceHandleRange;

    /* Initialize the Service Handle Group to 0.                        */
   ServiceHandleRange.Starting_Handle = 0;
   ServiceHandleRange.Ending_Handle   = 0;

   return(GAPSRegisterService(BluetoothStackID, ServiceID, &ServiceHandleRange));
}

   /* The following function is responsible for opening a GAPS Server.  */
   /* The first parameter is the Bluetooth Stack ID on which to open the*/
   /* server.  The second parameter is a pointer to store the GATT      */
   /* Service ID of the registered GAPS service.  This can be used to   */
   /* include the service registered by this call.  The final parameter */
   /* is a pointer, that on input can be used to control the location of*/
   /* the service in the GATT database, and on ouput to store the       */
   /* service handle range.  This function returns the positive,        */
   /* non-zero, Instance ID or a negative error code.                   */
   /* * NOTE * Only 1 GAPS Server may be open at a time, per Bluetooth  */
   /*          Stack ID.                                                */
int BTPSAPI GAPS_Initialize_Service_Handle_Range(unsigned int BluetoothStackID, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange)
{
   return(GAPSRegisterService(BluetoothStackID, ServiceID, ServiceHandleRange));
}

   /* The following function is responsible for closing a previously    */
   /* GAPS Server.  The first parameter is the Bluetooth Stack ID on    */
   /* which to close the server.  The second parameter is the InstanceID*/
   /* that was returned from a successful call to                       */
   /* GAPS_Initialize_Service().  This function returns a zero if       */
   /* successful or a negative return error code if an error occurs.    */
int BTPSAPI GAPS_Cleanup_Service(unsigned int BluetoothStackID, unsigned int InstanceID)
{
   int                   ret_val;
   GAPSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID))
   {
      /* Acquire the specified GAPS Instance.                           */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Verify that the service is actually registered.             */
         if(ServiceInstance->ServiceID)
         {
            /* Call GATT to un-register the service.                    */
            GATT_Un_Register_Service(BluetoothStackID, ServiceInstance->ServiceID);

            /* mark the instance entry as being free.                   */
            BTPS_MemInitialize(ServiceInstance, 0, GAPS_SERVER_INSTANCE_DATA_SIZE);

            /* return success to the caller.                            */
            ret_val = 0;
         }
         else
            ret_val = GAPS_ERROR_INVALID_PARAMETER;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(BluetoothStackID);
      }
      else
         ret_val = GAPS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = GAPS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for querying the number of  */
   /* attributes that are contained in the GAPS Service that is         */
   /* registered with a call to GAPS_Initialize_Service().  This        */
   /* function returns the non-zero number of attributes that are       */
   /* contained in a GAPS Server or zero on failure.                    */
unsigned int BTPSAPI GAPS_Query_Number_Attributes(void)
{
   /* Simply return the number of attributes that are contained in a    */
   /* GAPS service.                                                     */
   return(GENERIC_ACCESS_PROFILE_SERVICE_ATTRIBUTE_COUNT);
}

   /* The following function is responsible for setting the Device Name */
   /* characteristic on the specified GAP Service instance.  The first  */
   /* parameter is the Bluetooth Stack ID of the Bluetooth Device.  The */
   /* second parameter is the InstanceID returned from a successful call*/
   /* to GAPS_Initialize_Server().  The final parameter is the Device   */
   /* Name to set as the current Device Name for the specified GAP      */
   /* Service Instance.  The Name parameter must be a pointer to a NULL */
   /* terminated ASCII String of at most GAP_MAXIMUM_DEVICE_NAME_LENGTH */
   /* (not counting the trailing NULL terminator).  This function       */
   /* returns a zero if successful or a negative return error code if an*/
   /* error occurs.                                                     */
int BTPSAPI GAPS_Set_Device_Name(unsigned int BluetoothStackID, unsigned int InstanceID, char *DeviceName)
{
   int                   ret_val;
   Word_t                Length;
   GAPSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (DeviceName) && (BTPS_StringLength(DeviceName) <= GAP_MAXIMUM_DEVICE_NAME_LENGTH))
   {
      /* Acquire the specified GAPS Instance.                           */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Get the length of the Instance Data.                        */
         Length = (Word_t)BTPS_StringLength(DeviceName);

         /* Set the length of the Device Name.                          */
         ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(InstanceData[InstanceID-1].DeviceNameLength), Length);

         /* Copy in the Device Name.                                    */
         if(Length)
            BTPS_MemCopy(InstanceData[InstanceID-1].DeviceNameInstance, DeviceName, Length);

         /* Return success to the caller.                               */
         ret_val = 0;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = GAPS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = GAPS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for querying the current    */
   /* Alert Status characteristic value on the specified GAPS instance. */
   /* The first parameter is the Bluetooth Stack ID of the Bluetooth    */
   /* Device.  The second parameter is the InstanceID returned from a   */
   /* successful call to GAPS_Initialize_Server().  The final parameter */
   /* is a pointer to a structure to return the current Device for the  */
   /* specified GAPS Service Instance.  The NameBuffer Length should be */
   /* at least (GAP_MAXIMUM_DEVICE_NAME_LENGTH+1) to hold the Maximum   */
   /* allowable Name (plus a single character to hold the NULL          */
   /* terminator) This function returns a zero if successful or a       */
   /* negative return error code if an error occurs.                    */
int BTPSAPI GAPS_Query_Device_Name(unsigned int BluetoothStackID, unsigned int InstanceID, char *NameBuffer)
{
   int                   ret_val;
   Word_t                Length;
   GAPSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (NameBuffer))
   {
      /* Acquire the specified GAPS Instance.                           */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Get the length of the Instance Data.                        */
         Length = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(InstanceData[InstanceID-1].DeviceNameLength));

         /* Verify that the length is valid.                            */
         if(Length <= GAP_MAXIMUM_DEVICE_NAME_LENGTH)
         {
            /* Copy in the device name.                                 */
            BTPS_MemCopy(NameBuffer, InstanceData[InstanceID-1].DeviceNameInstance, Length);
            NameBuffer[Length] = 0;

            /* Return success to the caller.                            */
            ret_val            = 0;
         }
         else
            ret_val = GAPS_ERROR_INVALID_PARAMETER;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = GAPS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = GAPS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for setting the Device      */
   /* Appearance characteristic on the specified GAP Service instance.  */
   /* The first parameter is the Bluetooth Stack ID of the Bluetooth    */
   /* Device.  The second parameter is the InstanceID returned from a   */
   /* successful call to GAPS_Initialize_Server().  The final parameter */
   /* is the Device Appearance to set as the current Device Appearance  */
   /* for the specified GAP Service Instance.  This function returns a  */
   /* zero if successful or a negative return error code if an error    */
   /* occurs.                                                           */
   /* * NOTE * The DeviceAppearance is an enumeration, which should be  */
   /*          of the form GAP_DEVICE_APPEARENCE_VALUE_XXX.             */
int BTPSAPI GAPS_Set_Device_Appearance(unsigned int BluetoothStackID, unsigned int InstanceID, Word_t DeviceAppearance)
{
   int                   ret_val;
   GAPSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID))
   {
      /* Acquire the specified GAPS Instance.                           */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Set the Device Appearance.                                  */
         ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(InstanceData[InstanceID-1].DeviceAppearanceInstance), DeviceAppearance);

         /* Return success to the caller.                               */
         ret_val = 0;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = GAPS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = GAPS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for querying the Device     */
   /* Appearance characteristic on the specified GAP Service instance.  */
   /* The first parameter is the Bluetooth Stack ID of the Bluetooth    */
   /* Device.  The second parameter is the InstanceID returned from a   */
   /* successful call to GAPS_Initialize_Server().  The final parameter */
   /* is a pointer to store the current Device Appearance for the       */
   /* specified GAP Service Instance.  This function returns a zero if  */
   /* successful or a negative return error code if an error occurs.    */
int BTPSAPI GAPS_Query_Device_Appearance(unsigned int BluetoothStackID, unsigned int InstanceID, Word_t *DeviceAppearance)
{
   int                   ret_val;
   GAPSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (DeviceAppearance))
   {
      /* Acquire the specified GAPS Instance.                           */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Get the current Device Appearance.                          */
         *DeviceAppearance = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(InstanceData[InstanceID-1].DeviceAppearanceInstance));

         /* Return success to the caller.                               */
         ret_val = 0;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = GAPS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = GAPS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for setting the Peripheral  */
   /* Preferred Connection Parameter characteristic on the specified GAP*/
   /* Service instance.  The first parameter is the Bluetooth Stack ID  */
   /* of the Bluetooth Device.  The second parameter is the InstanceID  */
   /* returned from a successful call to GAPS_Initialize_Server().  The */
   /* final parameter is the Preferred Connection Parameters to set as  */
   /* the current Peripheral Preferred Connection Parameters for the    */
   /* specified GAP Service Instance.  This function returns a zero if  */
   /* successful or a negative return error code if an error occurs.    */
int BTPSAPI GAPS_Set_Preferred_Connection_Parameters(unsigned int BluetoothStackID, unsigned int InstanceID, GAP_Preferred_Connection_Parameters_t *PreferredConnectionParameters)
{
#if BTPS_CONFIGURATION_GAPS_SUPPORT_PREFERRED_CONNECTION_PARAMETERS

   int                   ret_val;
   GAPSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (PreferredConnectionParameters))
   {
      /* Acquire the specified GAPS Instance.                           */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Format the Preferred Connection Parameters into the instance*/
         /* data.                                                       */
         ret_val = FormatPreferredConnectionParameters(PreferredConnectionParameters, &(InstanceData[InstanceID-1].PreferredConnectionParameters));

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = GAPS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = GAPS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);

#else

   return(BTPS_ERROR_FEATURE_NOT_AVAILABLE);

#endif
}

   /* The following function is responsible for querying the Peripheral */
   /* Preferred Connection Parameter characteristic on the specified GAP*/
   /* Service instance.  The first parameter is the Bluetooth Stack ID  */
   /* of the Bluetooth Device.  The second parameter is the InstanceID  */
   /* returned from a successful call to GAPS_Initialize_Server().  The */
   /* final parameter is a pointer to a structure to store the current  */
   /* Peripheral Preferred Connection Parameters for the specified GAP  */
   /* Service Instance.  This function returns a zero if successful or a*/
   /* negative return error code if an error occurs.                    */
int BTPSAPI GAPS_Query_Preferred_Connection_Parameters(unsigned int BluetoothStackID, unsigned int InstanceID, GAP_Preferred_Connection_Parameters_t *PreferredConnectionParameters)
{
#if BTPS_CONFIGURATION_GAPS_SUPPORT_PREFERRED_CONNECTION_PARAMETERS

   int                   ret_val;
   GAPSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (PreferredConnectionParameters))
   {
      /* Acquire the specified GAPS Instance.                           */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Decode the Preferred Connection Parameters from the instance*/
         /* data.                                                       */
         ret_val = DecodePreferredConnectionParameters(GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_DATA_SIZE, (Byte_t *)&(InstanceData[InstanceID-1].PreferredConnectionParameters), PreferredConnectionParameters);

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = GAPS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = GAPS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);

#else

   return(BTPS_ERROR_FEATURE_NOT_AVAILABLE);

#endif
}

   /* GAPS Client API.                                                  */

   /* The following function is responsible for decoding a Peripheral   */
   /* Preferred Connection Parameter characteristic value that was      */
   /* received from a remote device.  The first parameter is the length */
   /* of the value received from the remote device.  The second is a    */
   /* pointer to the value that was received from the remote device.    */
   /* The final parameter is a pointer to a structure to store the      */
   /* decoded Peripheral Preferred Connection Parameters value that was */
   /* received from the remote device.  This function returns a zero if */
   /* successful or a negative return error code if an error occurs.    */
int BTPSAPI GAPS_Decode_Preferred_Connection_Parameters(unsigned int ValueLength, Byte_t *Value, GAP_Preferred_Connection_Parameters_t *PreferredConnectionParameters)
{
   /* Just call the internal function to do all of the work.            */
   return(DecodePreferredConnectionParameters(ValueLength, Value, PreferredConnectionParameters));
}

