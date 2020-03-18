/*****< dis.c >****************************************************************/
/*      Copyright 2011 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  DIS - Bluetooth Stack Device Information Service (GATT Based) for         */
/*        Stonestreet One Bluetooth Protocol Stack.                           */
/*                                                                            */
/*  Author:  Tim Cook                                                         */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   10/04/11  T. Cook        Initial creation.                               */
/*   12/5/12   T. Cook        Update to DIS v1.1.                             */
/******************************************************************************/
#include "SS1BTPS.h"        /* Bluetooth Stack API Prototypes/Constants.      */
#include "SS1BTGAT.h"       /* Bluetooth Stack GATT API Prototypes/Constants. */
#include "SS1BTDIS.h"       /* Bluetooth DIS API Prototypes/Constants.        */

#include "BTPSKRNL.h"       /* BTPS Kernel Prototypes/Constants.              */
#include "DIS.h"            /* Bluetooth DIS Prototypes/Constants.            */

   /* The following controls the number of supported DIS instances.     */
#define DIS_MAXIMUM_SUPPORTED_INSTANCES                  (BTPS_CONFIGURATION_DIS_MAXIMUM_SUPPORTED_INSTANCES)

   /* There must be at least one characteristic that is supported for   */
   /* DIS to operate effectively.                                       */
#if ((!BTPS_CONFIGURATION_DIS_SUPPORT_MANUFACTURER_NAME) && (!BTPS_CONFIGURATION_DIS_SUPPORT_MODEL_NUMBER) && (!BTPS_CONFIGURATION_DIS_SUPPORT_SERIAL_NUMBER) && (!BTPS_CONFIGURATION_DIS_SUPPORT_HARDWARE_REVISION) && (!BTPS_CONFIGURATION_DIS_SUPPORT_FIRMWARE_REVISION) && (!BTPS_CONFIGURATION_DIS_SUPPORT_SOFTWARE_REVISION) && (!BTPS_CONFIGURATION_DIS_SUPPORT_SYSTEM_ID) && (!BTPS_CONFIGURATION_DIS_SUPPORT_IEEE_CERTIFICATION_DATA) && (!BTPS_CONFIGURATION_DIS_SUPPORT_PNP_ID))

#error "At least 1 characteristic must be supported for DIS in the configs."

#endif

   /* The following structure defines the structure of a generic DIS    */
   /* Characteristic Value.                                             */
   /* * NOTE * The Data member is a pointer to a binary blob that must  */
   /*          be formatted such that if the characteristic value is    */
   /*          read, we can internally respond with the data directly   */
   /*          using this pointer.                                      */
typedef struct _tagDISCharacteristicValue_t
{
   Word_t  Characteristic_Length;
   Byte_t *Data;
} DISCharacteristicValue_t;

#define DIS_CHARACTERISTIC_VALUE_DATA_SIZE               (sizeof(DISCharacteristicValue_t))

   /* The following structure defines the DIS Instance data this unique */
   /* per DIS service instance.                                         */
typedef struct _tagDIS_Instance_Data_t
{

#if BTPS_CONFIGURATION_DIS_SUPPORT_MANUFACTURER_NAME

   DISCharacteristicValue_t Manufacturer_Name;

#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_MODEL_NUMBER

   DISCharacteristicValue_t Model_Number;

#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_SERIAL_NUMBER

   DISCharacteristicValue_t Serial_Number;

#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_HARDWARE_REVISION

   DISCharacteristicValue_t Hardware_Revision;

#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_FIRMWARE_REVISION

   DISCharacteristicValue_t Firmware_Revision;

#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_SOFTWARE_REVISION

   DISCharacteristicValue_t Software_Revision;

#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_SYSTEM_ID


   DISCharacteristicValue_t System_ID;
   Byte_t                   System_ID_Buffer[DIS_SYSTEM_ID_VALUE_DATA_SIZE];

#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_IEEE_CERTIFICATION_DATA

   DISCharacteristicValue_t IEEE_Certification_Data;

#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_PNP_ID

   DISCharacteristicValue_t PNP_ID;
   DIS_PNP_ID_Value_t       PNP_ID_Buffer;

#endif

} DIS_Instance_Data_t;

#define DIS_INSTANCE_DATA_SIZE                           (sizeof(DIS_Instance_Data_t))

   /* The following define the various instance data tags.              */
#if BTPS_CONFIGURATION_DIS_SUPPORT_MANUFACTURER_NAME

   #define DIS_MANUFACTURE_NAME_INSTANCE_TAG             (BTPS_STRUCTURE_OFFSET(DIS_Instance_Data_t, Manufacturer_Name))

#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_MODEL_NUMBER

   #define DIS_MODEL_NUMBER_INSTANCE_TAG                 (BTPS_STRUCTURE_OFFSET(DIS_Instance_Data_t, Model_Number))

#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_SERIAL_NUMBER

   #define DIS_SERIAL_NUMBER_INSTANCE_TAG                (BTPS_STRUCTURE_OFFSET(DIS_Instance_Data_t, Serial_Number))

#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_HARDWARE_REVISION

   #define DIS_HARDWARE_REVISION_INSTANCE_TAG            (BTPS_STRUCTURE_OFFSET(DIS_Instance_Data_t, Hardware_Revision))

#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_FIRMWARE_REVISION

   #define DIS_FIRMWARE_REVISION_INSTANCE_TAG            (BTPS_STRUCTURE_OFFSET(DIS_Instance_Data_t, Firmware_Revision))

#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_SOFTWARE_REVISION

   #define DIS_SOFTWARE_REVISION_INSTANCE_TAG            (BTPS_STRUCTURE_OFFSET(DIS_Instance_Data_t, Software_Revision))

#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_SYSTEM_ID

   #define DIS_SYSTEM_ID_INSTANCE_TAG                    (BTPS_STRUCTURE_OFFSET(DIS_Instance_Data_t, System_ID))

#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_IEEE_CERTIFICATION_DATA

   #define DIS_IEEE_CERTIFICATION_DATA_INSTANCE_TAG      (BTPS_STRUCTURE_OFFSET(DIS_Instance_Data_t, IEEE_Certification_Data))

#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_PNP_ID

   #define DIS_PNP_ID_INSTANCE_TAG                       (BTPS_STRUCTURE_OFFSET(DIS_Instance_Data_t, PNP_ID))

#endif

   /*********************************************************************/
   /**               Device Information Service Table                  **/
   /*********************************************************************/

   /* The Device Information Service Declaration UUID.                  */
static BTPSCONST GATT_Primary_Service_16_Entry_t DIS_Service_UUID =
{
   DIS_SERVICE_BLUETOOTH_UUID_CONSTANT
};

#if BTPS_CONFIGURATION_DIS_SUPPORT_MANUFACTURER_NAME

   /* The Manufacturer Name Characteristic Declaration.                 */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t DIS_Manufacturer_Name_Declaration =
{
   GATT_CHARACTERISTIC_PROPERTIES_READ,
   DIS_MANUFACTURER_NAME_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The Manufacturer Name Characteristic Value.                       */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t DIS_Manufacturer_Name_Value =
{
   DIS_MANUFACTURER_NAME_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   DIS_MANUFACTURE_NAME_INSTANCE_TAG,
   NULL
};

#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_MODEL_NUMBER

   /* The Model Number Characteristic Declaration.                      */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t DIS_Model_Number_Declaration =
{
   GATT_CHARACTERISTIC_PROPERTIES_READ,
   DIS_MODEL_NUMBER_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The Model Number Characteristic Value.                            */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t DIS_Model_Number_Value =
{
   DIS_MODEL_NUMBER_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   DIS_MODEL_NUMBER_INSTANCE_TAG,
   NULL
};

#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_SERIAL_NUMBER

   /* The Serial Number Characteristic Declaration.                     */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t DIS_Serial_Number_Declaration =
{
   GATT_CHARACTERISTIC_PROPERTIES_READ,
   DIS_SERIAL_NUMBER_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The Serial Number Characteristic Value.                           */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t DIS_Serial_Number_Value =
{
   DIS_SERIAL_NUMBER_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   DIS_SERIAL_NUMBER_INSTANCE_TAG,
   NULL
};

#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_HARDWARE_REVISION

   /* The Hardware Revision Characteristic Declaration.                 */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t DIS_Hardware_Revision_Declaration =
{
   GATT_CHARACTERISTIC_PROPERTIES_READ,
   DIS_HARDWARE_REVISION_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The Hardware Revision Characteristic Value.                       */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t DIS_Hardware_Revision_Value =
{
   DIS_HARDWARE_REVISION_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   DIS_HARDWARE_REVISION_INSTANCE_TAG,
   NULL
};

#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_FIRMWARE_REVISION

   /* The Firmware Revision Characteristic Declaration.                 */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t DIS_Firmware_Revision_Declaration =
{
   GATT_CHARACTERISTIC_PROPERTIES_READ,
   DIS_FIRMWARE_REVISION_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The Firmware Revision Characteristic Value.                       */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t DIS_Firmware_Revision_Value =
{
   DIS_FIRMWARE_REVISION_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   DIS_FIRMWARE_REVISION_INSTANCE_TAG,
   NULL
};

#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_SOFTWARE_REVISION

   /* The Software Revision Characteristic Declaration.                 */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t DIS_Software_Revision_Declaration =
{
   GATT_CHARACTERISTIC_PROPERTIES_READ,
   DIS_SOFTWARE_REVISION_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The Software Revision Characteristic Value.                       */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t DIS_Software_Revision_Value =
{
   DIS_SOFTWARE_REVISION_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   DIS_SOFTWARE_REVISION_INSTANCE_TAG,
   NULL
};

#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_SYSTEM_ID

   /* The System ID Characteristic Declaration.                         */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t DIS_System_ID_Declaration =
{
   GATT_CHARACTERISTIC_PROPERTIES_READ,
   DIS_SYSTEM_ID_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The System ID Characteristic Value.                               */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t DIS_System_ID_Value =
{
   DIS_SYSTEM_ID_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   DIS_SYSTEM_ID_INSTANCE_TAG,
   NULL
};

#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_IEEE_CERTIFICATION_DATA

   /* The IEEE Certification Data Characteristic Declaration.           */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t DIS_IEEE_Certification_Data_Declaration =
{
   GATT_CHARACTERISTIC_PROPERTIES_READ,
   DIS_IEEE_CERTIFICATION_DATA_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The IEEE Certification Data Characteristic Value.                 */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t DIS_IEEE_Certification_Data_Value =
{
   DIS_IEEE_CERTIFICATION_DATA_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   DIS_IEEE_CERTIFICATION_DATA_INSTANCE_TAG,
   NULL
};

#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_PNP_ID

   /* The PnP ID Characteristic Declaration.                            */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t DIS_PNP_ID_Declaration =
{
   GATT_CHARACTERISTIC_PROPERTIES_READ,
   DIS_PNP_ID_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The PnP ID Characteristic Value.                                  */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t DIS_PNP_ID_Value =
{
   DIS_PNP_ID_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   DIS_PNP_ID_INSTANCE_TAG,
   NULL
};

#endif

   /* The following defines the Device Information service that is      */
   /* registered with the GATT_Register_Service function call.          */
   /* * NOTE * This array will be registered with GATT in the call to   */
   /*          GATT_Register_Service.                                   */
BTPSCONST GATT_Service_Attribute_Entry_t Device_Information_Service[] =
{
   {GATT_ATTRIBUTE_FLAGS_READABLE, aetPrimaryService16,            (Byte_t *)&DIS_Service_UUID},

#if BTPS_CONFIGURATION_DIS_SUPPORT_MANUFACTURER_NAME
   {GATT_ATTRIBUTE_FLAGS_READABLE, aetCharacteristicDeclaration16, (Byte_t *)&DIS_Manufacturer_Name_Declaration},
   {GATT_ATTRIBUTE_FLAGS_READABLE, aetCharacteristicValue16,       (Byte_t *)&DIS_Manufacturer_Name_Value},
#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_MODEL_NUMBER
   {GATT_ATTRIBUTE_FLAGS_READABLE, aetCharacteristicDeclaration16, (Byte_t *)&DIS_Model_Number_Declaration},
   {GATT_ATTRIBUTE_FLAGS_READABLE, aetCharacteristicValue16,       (Byte_t *)&DIS_Model_Number_Value},
#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_SERIAL_NUMBER
   {GATT_ATTRIBUTE_FLAGS_READABLE, aetCharacteristicDeclaration16, (Byte_t *)&DIS_Serial_Number_Declaration},
   {GATT_ATTRIBUTE_FLAGS_READABLE, aetCharacteristicValue16,       (Byte_t *)&DIS_Serial_Number_Value},
#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_HARDWARE_REVISION
   {GATT_ATTRIBUTE_FLAGS_READABLE, aetCharacteristicDeclaration16, (Byte_t *)&DIS_Hardware_Revision_Declaration},
   {GATT_ATTRIBUTE_FLAGS_READABLE, aetCharacteristicValue16,       (Byte_t *)&DIS_Hardware_Revision_Value},
#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_FIRMWARE_REVISION
   {GATT_ATTRIBUTE_FLAGS_READABLE, aetCharacteristicDeclaration16, (Byte_t *)&DIS_Firmware_Revision_Declaration},
   {GATT_ATTRIBUTE_FLAGS_READABLE, aetCharacteristicValue16,       (Byte_t *)&DIS_Firmware_Revision_Value},
#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_SOFTWARE_REVISION
   {GATT_ATTRIBUTE_FLAGS_READABLE, aetCharacteristicDeclaration16, (Byte_t *)&DIS_Software_Revision_Declaration},
   {GATT_ATTRIBUTE_FLAGS_READABLE, aetCharacteristicValue16,       (Byte_t *)&DIS_Software_Revision_Value},
#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_SYSTEM_ID
   {GATT_ATTRIBUTE_FLAGS_READABLE, aetCharacteristicDeclaration16, (Byte_t *)&DIS_System_ID_Declaration},
   {GATT_ATTRIBUTE_FLAGS_READABLE, aetCharacteristicValue16,       (Byte_t *)&DIS_System_ID_Value},
#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_IEEE_CERTIFICATION_DATA
   {GATT_ATTRIBUTE_FLAGS_READABLE, aetCharacteristicDeclaration16, (Byte_t *)&DIS_IEEE_Certification_Data_Declaration},
   {GATT_ATTRIBUTE_FLAGS_READABLE, aetCharacteristicValue16,       (Byte_t *)&DIS_IEEE_Certification_Data_Value},
#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_PNP_ID
   {GATT_ATTRIBUTE_FLAGS_READABLE, aetCharacteristicDeclaration16, (Byte_t *)&DIS_PNP_ID_Declaration},
   {GATT_ATTRIBUTE_FLAGS_READABLE, aetCharacteristicValue16,       (Byte_t *)&DIS_PNP_ID_Value},
#endif
};

#define DEVICE_INFORMATION_SERVICE_ATTRIBUTE_COUNT                      (sizeof(Device_Information_Service)/sizeof(GATT_Service_Attribute_Entry_t))

   /* The following defines the DIS GATT Service Flags MASK that should */
   /* be passed into GATT_Register_Service when the DIS Service is      */
   /* registered.                                                       */
#define DIS_SERVICE_FLAGS                               (GATT_SERVICE_FLAGS_LE_SERVICE|GATT_SERVICE_FLAGS_BR_EDR_SERVICE)

   /*********************************************************************/
   /**                    END OF SERVICE TABLE                         **/
   /*********************************************************************/

   /* DIS Service Instance Block.  This structure contains All          */
   /* information associated with a specific Bluetooth Stack ID (member */
   /* is present in this structure).                                    */
typedef struct _tagDISServerInstance_t
{
   unsigned int BluetoothStackID;
   unsigned int ServiceID;
} DISServerInstance_t;

#define DIS_SERVER_INSTANCE_DATA_SIZE                    (sizeof(DISServerInstance_t))

   /* Internal Variables to this Module (Remember that all variables    */
   /* declared static are initialized to 0 automatically by the         */
   /* compiler as part of standard C/C++).                              */

static DIS_Instance_Data_t InstanceData[DIS_MAXIMUM_SUPPORTED_INSTANCES];
                                            /* Variable which holds all */
                                            /* data that is unique for  */
                                            /* each service instance.   */

static DISServerInstance_t InstanceList[DIS_MAXIMUM_SUPPORTED_INSTANCES];
                                            /* Variable which holds the */
                                            /* service instance data.   */

static Boolean_t InstanceListInitialized;   /* Variable that flags that */
                                            /* is used to denote that   */
                                            /* this module has been     */
                                            /* successfully initialized.*/

   /* The following are the prototypes of local functions.              */
static Boolean_t InitializeModule(void);
static void CleanupModule(void);

static Boolean_t InstanceRegisteredByStackID(unsigned int BluetoothStackID);
static DISServerInstance_t *AcquireServiceInstance(unsigned int BluetoothStackID, unsigned int *InstanceID);

static int DISRegisterService(unsigned int BluetoothStackID, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange);

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

   /* The following function is a utility function that exists to check */
   /* to see if an instance has already been registered for a specified */
   /* Bluetooth Stack ID.                                               */
   /* * NOTE * Since this is an internal function no check is done on   */
   /*          the input parameters.                                    */
static Boolean_t InstanceRegisteredByStackID(unsigned int BluetoothStackID)
{
   Boolean_t    ret_val = FALSE;
   unsigned int Index;

   for(Index=0;Index<DIS_MAXIMUM_SUPPORTED_INSTANCES;Index++)
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
static DISServerInstance_t *AcquireServiceInstance(unsigned int BluetoothStackID, unsigned int *InstanceID)
{
   unsigned int          LocalInstanceID;
   unsigned int          Index;
   DISServerInstance_t *ret_val = NULL;

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
         if((LocalInstanceID) && (LocalInstanceID <= DIS_MAXIMUM_SUPPORTED_INSTANCES))
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
               for(Index=0;Index<DIS_MAXIMUM_SUPPORTED_INSTANCES;Index++)
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
   /* register an DIS Service.  This function returns the positive,     */
   /* non-zero, Instance ID on success or a negative error code.        */
static int DISRegisterService(unsigned int BluetoothStackID, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange)
{
   int                  ret_val;
   unsigned int         InstanceID;
   DISServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (ServiceID))
   {
      /* Verify that no instance is registered to this Bluetooth Stack. */
      if(!InstanceRegisteredByStackID(BluetoothStackID))
      {
         /* Acquire a free DIS Instance.                                */
         InstanceID = 0;
         if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
         {
            /* Call GATT to register the DIS service.                   */
            ret_val = GATT_Register_Service(BluetoothStackID, DIS_SERVICE_FLAGS, DEVICE_INFORMATION_SERVICE_ATTRIBUTE_COUNT, (GATT_Service_Attribute_Entry_t *)Device_Information_Service, ServiceHandleRange, GATT_ServerEventCallback, InstanceID);
            if(ret_val > 0)
            {
               /* Save the Instance information.                        */
               ServiceInstance->BluetoothStackID = BluetoothStackID;
               ServiceInstance->ServiceID        = (unsigned int)ret_val;
               *ServiceID                        = (unsigned int)ret_val;

               /* Intilize the Instance Data for this instance.         */
               BTPS_MemInitialize(&InstanceData[InstanceID-1], 0, DIS_INSTANCE_DATA_SIZE);

#if BTPS_CONFIGURATION_DIS_SUPPORT_SYSTEM_ID

               InstanceData[InstanceID-1].System_ID.Characteristic_Length = DIS_SYSTEM_ID_VALUE_DATA_SIZE;
               InstanceData[InstanceID-1].System_ID.Data                  = InstanceData[InstanceID-1].System_ID_Buffer;

#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_PNP_ID

               InstanceData[InstanceID-1].PNP_ID.Characteristic_Length  = DIS_PNP_ID_VALUE_DATA_SIZE;
               InstanceData[InstanceID-1].PNP_ID.Data                   = (Byte_t *)&(InstanceData[InstanceID-1].PNP_ID_Buffer);

#endif

               /* Return the DIS Instance ID.                           */
               ret_val = (int)InstanceID;
            }

            /* UnLock the previously locked Bluetooth Stack.            */
            BSC_UnLockBluetoothStack(BluetoothStackID);
         }
         else
            ret_val = DIS_ERROR_INSUFFICIENT_RESOURCES;
      }
      else
         ret_val = DIS_ERROR_SERVICE_ALREADY_REGISTERED;
   }
   else
      ret_val = DIS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is the GATT Server Event Callback that     */
   /* handles all requests made to the DIS Service for all registered   */
   /* instances.                                                        */
static void BTPSAPI GATT_ServerEventCallback(unsigned int BluetoothStackID, GATT_Server_Event_Data_t *GATT_ServerEventData, unsigned long CallbackParameter)
{
   Word_t                    AttributeOffset;
   Word_t                    InstanceTag;
   Word_t                    ValueLength;
   Byte_t                   *Value;
   unsigned int              TransactionID;
   unsigned int              InstanceID;
   DISServerInstance_t      *ServiceInstance;
   DISCharacteristicValue_t *CharacteristicValue;

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

                  /* Get the instance tag and data for the              */
                  /* characteristic.                                    */
                  InstanceTag         = (Word_t)(((GATT_Characteristic_Value_16_Entry_t *)Device_Information_Service[AttributeOffset].Attribute_Value)->Characteristic_Value_Length);
                  CharacteristicValue = (DISCharacteristicValue_t *)&(((Byte_t *)(&InstanceData[InstanceID-1]))[InstanceTag]);
                  if(GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->AttributeValueOffset <= CharacteristicValue->Characteristic_Length)
                  {
                     /* Calculate the length of the data that we are    */
                     /* going to return.                                */
                     ValueLength = (Word_t)(CharacteristicValue->Characteristic_Length - GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->AttributeValueOffset);

                     if(CharacteristicValue->Data)
                        Value = &(CharacteristicValue->Data[GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->AttributeValueOffset]);
                     else
                     {
                        Value       = NULL;
                        ValueLength = 0;
                     }

                     /* Respond with the data.                          */
                     GATT_Read_Response(BluetoothStackID, TransactionID, (unsigned int)ValueLength, Value);
                  }
                  else
                     GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, ATT_PROTOCOL_ERROR_CODE_INVALID_OFFSET);
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
   /* Bluetooth Stack DIS Module is Initialized correctly.  This        */
   /* function *MUST* be called before ANY other Bluetooth Stack DIS    */
   /* function can be called.  This function returns non-zero if the    */
   /* Module was initialized correctly, or a zero value if there was an */
   /* error.                                                            */
   /* * NOTE * Internally, this module will make sure that this function*/
   /*          has been called at least once so that the module will    */
   /*          function.  Calling this function from an external        */
   /*          location is not necessary.                               */
int InitializeDISModule(void)
{
   return((int)InitializeModule());
}

   /* The following function is responsible for instructing the         */
   /* Bluetooth Stack DISC Module to clean up any resources that it has */
   /* allocated.  Once this function has completed, NO other Bluetooth  */
   /* Stack DIS Functions can be called until a successful call to the  */
   /* InitializeDISModule() function is made.  The parameter to this    */
   /* function specifies the context in which this function is being    */
   /* called.  If the specified parameter is TRUE, then the module will */
   /* make sure that NO functions that would require waiting/blocking on*/
   /* Mutexes/Events are called.  This parameter would be set to TRUE if*/
   /* this function was called in a context where threads would not be  */
   /* allowed to run.  If this function is called in the context where  */
   /* threads are allowed to run then this parameter should be set to   */
   /* FALSE.                                                            */
void CleanupDISModule(Boolean_t ForceCleanup)
{
   /* Check to make sure that this module has been initialized.         */
   if(InstanceListInitialized)
   {
      /* Wait for access to the DIS Context List.                       */
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

   /* DIS Server API.                                                   */

   /* The following function is responsible for opening a DIS Server.   */
   /* The first parameter is the Bluetooth Stack ID on which to open the*/
   /* server.  The final parameter is a pointer to store the GATT       */
   /* Service ID of the registered DIS service.  This can be used to    */
   /* include the service registered by this call.  This function       */
   /* returns the positive, non-zero, Instance ID or a negative error   */
   /* code.                                                             */
   /* * NOTE * Only 1 DIS Server may be open at a time, per Bluetooth   */
   /*          Stack ID.                                                */
int BTPSAPI DIS_Initialize_Service(unsigned int BluetoothStackID, unsigned int *ServiceID)
{
  GATT_Attribute_Handle_Group_t ServiceHandleRange;

    /* Initialize the Service Handle Group to 0.                        */
   ServiceHandleRange.Starting_Handle = 0;
   ServiceHandleRange.Ending_Handle   = 0;

   return(DISRegisterService(BluetoothStackID, ServiceID, &ServiceHandleRange));
}

   /* The following function is responsible for opening a DIS Server.   */
   /* The first parameter is the Bluetooth Stack ID on which to open the*/
   /* server.  The second parameter is a pointer to store the GATT      */
   /* Service ID of the registered DIS service.  This can be used to    */
   /* include the service registered by this call.  The final parameter */
   /* is a pointer, that on input can be used to control the location of*/
   /* the service in the GATT database, and on ouput to store the       */
   /* service handle range.  This function returns the positive,        */
   /* non-zero, Instance ID or a negative error code.                   */
   /* * NOTE * Only 1 DIS Server may be open at a time, per Bluetooth   */
   /*          Stack ID.                                                */
int BTPSAPI DIS_Initialize_Service_Handle_Range(unsigned int BluetoothStackID, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange)
{
   return(DISRegisterService(BluetoothStackID, ServiceID, ServiceHandleRange));
}

   /* The following function is responsible for closing a previously DIS*/
   /* Server.  The first parameter is the Bluetooth Stack ID on which to*/
   /* close the server.  The second parameter is the InstanceID that was*/
   /* returned from a successfull call to DIS_Initialize_Service().     */
   /* This function returns a zero if successful or a negative return   */
   /* error code if an error occurs.                                    */
int BTPSAPI DIS_Cleanup_Service(unsigned int BluetoothStackID, unsigned int InstanceID)
{
   int                   ret_val;
   DISServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID))
   {
      /* Acquire the specified DIS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Verify that the service is actually registered.             */
         if(ServiceInstance->ServiceID)
         {
            /* Call GATT to un-register the service.                    */
            GATT_Un_Register_Service(BluetoothStackID, ServiceInstance->ServiceID);

            /* Free any of the dynamically allocated values.            */

#if BTPS_CONFIGURATION_DIS_SUPPORT_MANUFACTURER_NAME

            if(InstanceData[InstanceID-1].Manufacturer_Name.Data)
               BTPS_FreeMemory(InstanceData[InstanceID-1].Manufacturer_Name.Data);

#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_MODEL_NUMBER

            if(InstanceData[InstanceID-1].Model_Number.Data)
               BTPS_FreeMemory(InstanceData[InstanceID-1].Model_Number.Data);

#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_SERIAL_NUMBER

            if(InstanceData[InstanceID-1].Serial_Number.Data)
               BTPS_FreeMemory(InstanceData[InstanceID-1].Serial_Number.Data);

#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_HARDWARE_REVISION

            if(InstanceData[InstanceID-1].Hardware_Revision.Data)
               BTPS_FreeMemory(InstanceData[InstanceID-1].Hardware_Revision.Data);

#endif

#if BTPS_CONFIGURATION_DIS_SUPPORT_FIRMWARE_REVISION

            if(InstanceData[InstanceID-1].Firmware_Revision.Data)
               BTPS_FreeMemory(InstanceData[InstanceID-1].Firmware_Revision.Data);

#endif


#if BTPS_CONFIGURATION_DIS_SUPPORT_SOFTWARE_REVISION

            if(InstanceData[InstanceID-1].Software_Revision.Data)
               BTPS_FreeMemory(InstanceData[InstanceID-1].Software_Revision.Data);

#endif


#if BTPS_CONFIGURATION_DIS_SUPPORT_IEEE_CERTIFICATION_DATA

            if(InstanceData[InstanceID-1].IEEE_Certification_Data.Data)
               BTPS_FreeMemory(InstanceData[InstanceID-1].IEEE_Certification_Data.Data);

#endif

            /* Mark the instance entry as being free.                   */
            BTPS_MemInitialize(ServiceInstance, 0, DIS_SERVER_INSTANCE_DATA_SIZE);

            /* Clear the instance data.                                 */
            BTPS_MemInitialize(InstanceData, 0, DIS_INSTANCE_DATA_SIZE);

            /* return success to the caller.                            */
            ret_val = 0;
         }
         else
            ret_val = DIS_ERROR_INVALID_PARAMETER;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(BluetoothStackID);
      }
      else
         ret_val = DIS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = DIS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for querying the number of  */
   /* attributes that are contained in the DIS Service that is          */
   /* registered with a call to DIS_Initialize_Service().  This function*/
   /* returns the non-zero number of attributes that are contained in a */
   /* DIS Server or zero on failure.                                    */
unsigned int BTPSAPI DIS_Query_Number_Attributes(void)
{
   /* Simply return the number of attributes that are contained in a DIS*/
   /* service.                                                          */
   return(DEVICE_INFORMATION_SERVICE_ATTRIBUTE_COUNT);
}

   /* The following function is responsible for setting the Manufacturer*/
   /* Name characteristic on the specified Device Information Service   */
   /* instance.  The first parameter is the Bluetooth Stack ID of the   */
   /* Bluetooth Device.  The second parameter is the InstanceID returned*/
   /* from a successful call to DIS_Initialize_Server().  The final     */
   /* parameter is the Manufacturer Name to set as the current          */
   /* Manufacturer Name for the specified Device Information Service    */
   /* Instance.  The Manufacturer Name parameter must be a pointer to a */
   /* NULL terminated ASCII String of at most                           */
   /* DIS_MAXIMUM_SUPPORTED_STRING (not counting the trailing NULL      */
   /* terminator).  This function returns a zero if successful or a     */
   /* negative return error code if an error occurs.                    */
int BTPSAPI DIS_Set_Manufacturer_Name(unsigned int BluetoothStackID, unsigned int InstanceID, char *ManufacturerName)
{
#if BTPS_CONFIGURATION_DIS_SUPPORT_MANUFACTURER_NAME

   int                  ret_val;
   unsigned int         StringLength;
   DISServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (ManufacturerName) && ((StringLength = BTPS_StringLength(ManufacturerName)) <= DIS_MAXIMUM_SUPPORTED_STRING))
   {
      /* Acquire the specified DIS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Free any previous buffer allocated.                         */
         if(InstanceData[InstanceID-1].Manufacturer_Name.Data)
         {
            BTPS_FreeMemory(InstanceData[InstanceID-1].Manufacturer_Name.Data);
            InstanceData[InstanceID-1].Manufacturer_Name.Data = NULL;
         }

         /* Attempt to allocate memory for the Manufacturer Name string.*/
         if((InstanceData[InstanceID-1].Manufacturer_Name.Data = BTPS_AllocateMemory(StringLength)) != NULL)
         {
            /* Set the length and copy the data into the Manufacturer   */
            /* Name.                                                    */
            InstanceData[InstanceID-1].Manufacturer_Name.Characteristic_Length = (Word_t)StringLength;
            BTPS_MemCopy(InstanceData[InstanceID-1].Manufacturer_Name.Data, ManufacturerName, StringLength);

            /* Return success to the caller.                            */
            ret_val = 0;
         }
         else
            ret_val = DIS_ERROR_INSUFFICIENT_RESOURCES;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = DIS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = DIS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);

#else

   return(BTPS_ERROR_FEATURE_NOT_AVAILABLE);

#endif
}

   /* The following function is responsible for querying the current    */
   /* Manufacturer Name characteristic value on the specified DIS       */
   /* instance.  The first parameter is the Bluetooth Stack ID of the   */
   /* Bluetooth Device.  The second parameter is the InstanceID returned*/
   /* from a successful call to DIS_Initialize_Server().  The final     */
   /* parameter is a pointer to a structure to return the current       */
   /* Manufacturer Name for the specified DIS Service Instance.  The    */
   /* Manufacturer Name Length should be at least                       */
   /* (DIS_MAXIMUM_SUPPORTED_STRING+1) to hold the Maximum allowable    */
   /* string (plus a single character to hold the NULL terminator) This */
   /* function returns a zero if successful or a negative return error  */
   /* code if an error occurs.                                          */
int BTPSAPI DIS_Query_Manufacturer_Name(unsigned int BluetoothStackID, unsigned int InstanceID, char *ManufacturerName)
{
#if BTPS_CONFIGURATION_DIS_SUPPORT_MANUFACTURER_NAME

   int                  ret_val;
   Word_t               Length;
   DISServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (ManufacturerName))
   {
      /* Acquire the specified DIS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Get the current Manufacturer Name.                          */
         Length = InstanceData[InstanceID-1].Manufacturer_Name.Characteristic_Length;
         if(InstanceData[InstanceID-1].Manufacturer_Name.Data)
            BTPS_MemCopy(ManufacturerName, InstanceData[InstanceID-1].Manufacturer_Name.Data, Length);

         ManufacturerName[Length] = 0;

         /* Return success to the caller.                               */
         ret_val = 0;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = DIS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = DIS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);

#else

   return(BTPS_ERROR_FEATURE_NOT_AVAILABLE);

#endif
}

   /* The following function is responsible for setting the Model Number*/
   /* characteristic on the specified Device Information Service        */
   /* instance.  The first parameter is the Bluetooth Stack ID of the   */
   /* Bluetooth Device.  The second parameter is the InstanceID returned*/
   /* from a successful call to DIS_Initialize_Server().  The final     */
   /* parameter is the Model Number to set as the current Model Number  */
   /* for the specified Device Information Service Instance.  The Model */
   /* Number parameter must be a pointer to a NULL terminated ASCII     */
   /* String of at most DIS_MAXIMUM_SUPPORTED_STRING (not counting the  */
   /* trailing NULL terminator).  This function returns a zero if       */
   /* successful or a negative return error code if an error occurs.    */
int BTPSAPI DIS_Set_Model_Number(unsigned int BluetoothStackID, unsigned int InstanceID, char *ModelNumber)
{
#if BTPS_CONFIGURATION_DIS_SUPPORT_MODEL_NUMBER

   int                  ret_val;
   unsigned int         StringLength;
   DISServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (ModelNumber) && ((StringLength = BTPS_StringLength(ModelNumber)) <= DIS_MAXIMUM_SUPPORTED_STRING))
   {
      /* Acquire the specified DIS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Free any previous buffer allocated.                         */
         if(InstanceData[InstanceID-1].Model_Number.Data)
         {
            BTPS_FreeMemory(InstanceData[InstanceID-1].Model_Number.Data);
            InstanceData[InstanceID-1].Model_Number.Data = NULL;
         }

         /* Attempt to allocate memory for the Model Number string.     */
         if((InstanceData[InstanceID-1].Model_Number.Data = BTPS_AllocateMemory(StringLength)) != NULL)
         {
            /* Set the length and copy the data into the Model Number.  */
            InstanceData[InstanceID-1].Model_Number.Characteristic_Length = (Word_t)StringLength;
            BTPS_MemCopy(InstanceData[InstanceID-1].Model_Number.Data, ModelNumber, StringLength);

            /* Return success to the caller.                            */
            ret_val = 0;
         }
         else
            ret_val = DIS_ERROR_INSUFFICIENT_RESOURCES;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = DIS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = DIS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);

#else

   return(BTPS_ERROR_FEATURE_NOT_AVAILABLE);

#endif
}

   /* The following function is responsible for querying the current    */
   /* Model Number characteristic value on the specified DIS instance.  */
   /* The first parameter is the Bluetooth Stack ID of the Bluetooth    */
   /* Device.  The second parameter is the InstanceID returned from a   */
   /* successful call to DIS_Initialize_Server().  The final parameter  */
   /* is a pointer to a structure to return the current Model Number for*/
   /* the specified DIS Service Instance.  The Model Number Length      */
   /* should be at least (DIS_MAXIMUM_SUPPORTED_STRING+1) to hold the   */
   /* Maximum allowable string (plus a single character to hold the NULL*/
   /* terminator) This function returns a zero if successful or a       */
   /* negative return error code if an error occurs.                    */
int BTPSAPI DIS_Query_Model_Number(unsigned int BluetoothStackID, unsigned int InstanceID, char *ModelNumber)
{
#if BTPS_CONFIGURATION_DIS_SUPPORT_MODEL_NUMBER

   int                  ret_val;
   Word_t               Length;
   DISServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (ModelNumber))
   {
      /* Acquire the specified DIS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Get the current Model Number.                               */
         Length = InstanceData[InstanceID-1].Model_Number.Characteristic_Length;
         if(InstanceData[InstanceID-1].Model_Number.Data)
            BTPS_MemCopy(ModelNumber, InstanceData[InstanceID-1].Model_Number.Data, Length);

         ModelNumber[Length] = 0;

         /* Return success to the caller.                               */
         ret_val = 0;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = DIS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = DIS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);

#else

   return(BTPS_ERROR_FEATURE_NOT_AVAILABLE);

#endif
}

   /* The following function is responsible for setting the Serial      */
   /* Number characteristic on the specified Device Information Service */
   /* instance.  The first parameter is the Bluetooth Stack ID of the   */
   /* Bluetooth Device.  The second parameter is the InstanceID returned*/
   /* from a successful call to DIS_Initialize_Server().  The final     */
   /* parameter is the Serial Number to set as the current Serial Number*/
   /* for the specified Device Information Service Instance.  The Serial*/
   /* Number parameter must be a pointer to a NULL terminated ASCII     */
   /* String of at most DIS_MAXIMUM_SUPPORTED_STRING (not counting the  */
   /* trailing NULL terminator).  This function returns a zero if       */
   /* successful or a negative return error code if an error occurs.    */
int BTPSAPI DIS_Set_Serial_Number(unsigned int BluetoothStackID, unsigned int InstanceID, char *SerialNumber)
{
#if BTPS_CONFIGURATION_DIS_SUPPORT_SERIAL_NUMBER

   int                  ret_val;
   unsigned int         StringLength;
   DISServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (SerialNumber) && ((StringLength = BTPS_StringLength(SerialNumber)) <= DIS_MAXIMUM_SUPPORTED_STRING))
   {
      /* Acquire the specified DIS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Free any previous buffer allocated.                         */
         if(InstanceData[InstanceID-1].Serial_Number.Data)
         {
            BTPS_FreeMemory(InstanceData[InstanceID-1].Serial_Number.Data);
            InstanceData[InstanceID-1].Serial_Number.Data = NULL;
         }

         /* Attempt to allocate memory for the Serial Number string.    */
         if((InstanceData[InstanceID-1].Serial_Number.Data = BTPS_AllocateMemory(StringLength)) != NULL)
         {
            /* Set the length and copy the data into the Serial Number. */
            InstanceData[InstanceID-1].Serial_Number.Characteristic_Length = (Word_t)StringLength;
            BTPS_MemCopy(InstanceData[InstanceID-1].Serial_Number.Data, SerialNumber, StringLength);

            /* Return success to the caller.                            */
            ret_val = 0;
         }
         else
            ret_val = DIS_ERROR_INSUFFICIENT_RESOURCES;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = DIS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = DIS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);

#else

   return(BTPS_ERROR_FEATURE_NOT_AVAILABLE);

#endif
}

   /* The following function is responsible for querying the current    */
   /* Serial Number characteristic value on the specified DIS instance. */
   /* The first parameter is the Bluetooth Stack ID of the Bluetooth    */
   /* Device.  The second parameter is the InstanceID returned from a   */
   /* successful call to DIS_Initialize_Server().  The final parameter  */
   /* is a pointer to a structure to return the current Serial Number   */
   /* for the specified DIS Service Instance.  The Serial Number Length */
   /* should be at least (DIS_MAXIMUM_SUPPORTED_STRING+1) to hold the   */
   /* Maximum allowable string (plus a single character to hold the NULL*/
   /* terminator) This function returns a zero if successful or a       */
   /* negative return error code if an error occurs.                    */
int BTPSAPI DIS_Query_Serial_Number(unsigned int BluetoothStackID, unsigned int InstanceID, char *SerialNumber)
{
#if BTPS_CONFIGURATION_DIS_SUPPORT_SERIAL_NUMBER

   int                  ret_val;
   Word_t               Length;
   DISServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (SerialNumber))
   {
      /* Acquire the specified DIS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Get the current Serial Number.                              */
         Length = InstanceData[InstanceID-1].Serial_Number.Characteristic_Length;
         if(InstanceData[InstanceID-1].Serial_Number.Data)
            BTPS_MemCopy(SerialNumber, InstanceData[InstanceID-1].Serial_Number.Data, Length);

         SerialNumber[Length] = 0;

         /* Return success to the caller.                               */
         ret_val = 0;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = DIS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = DIS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);

#else

   return(BTPS_ERROR_FEATURE_NOT_AVAILABLE);

#endif
}

   /* The following function is responsible for setting the Hardware    */
   /* Revision characteristic on the specified Device Information       */
   /* Service instance.  The first parameter is the Bluetooth Stack ID  */
   /* of the Bluetooth Device.  The second parameter is the InstanceID  */
   /* returned from a successful call to DIS_Initialize_Server().  The  */
   /* final parameter is the Hardware Revision to set as the current    */
   /* Hardware Revision for the specified Device Information Service    */
   /* Instance.  The Hardware Revision parameter must be a pointer to a */
   /* NULL terminated ASCII String of at most                           */
   /* DIS_MAXIMUM_SUPPORTED_STRING (not counting the trailing NULL      */
   /* terminator).  This function returns a zero if successful or a     */
   /* negative return error code if an error occurs.                    */
int BTPSAPI DIS_Set_Hardware_Revision(unsigned int BluetoothStackID, unsigned int InstanceID, char *Hardware_Revision)
{
#if BTPS_CONFIGURATION_DIS_SUPPORT_HARDWARE_REVISION

   int                  ret_val;
   unsigned int         StringLength;
   DISServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (Hardware_Revision) && ((StringLength = BTPS_StringLength(Hardware_Revision)) <= DIS_MAXIMUM_SUPPORTED_STRING))
   {
      /* Acquire the specified DIS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Free any previous buffer allocated.                         */
         if(InstanceData[InstanceID-1].Hardware_Revision.Data)
         {
            BTPS_FreeMemory(InstanceData[InstanceID-1].Hardware_Revision.Data);
            InstanceData[InstanceID-1].Hardware_Revision.Data = NULL;
         }

         /* Attempt to allocate memory for the Hardware Revision string.*/
         if((InstanceData[InstanceID-1].Hardware_Revision.Data = BTPS_AllocateMemory(StringLength)) != NULL)
         {
            /* Set the length and copy the data into the Hardware       */
            /* Revision.                                                */
            InstanceData[InstanceID-1].Hardware_Revision.Characteristic_Length = (Word_t)StringLength;
            BTPS_MemCopy(InstanceData[InstanceID-1].Hardware_Revision.Data, Hardware_Revision, StringLength);

            /* Return success to the caller.                            */
            ret_val = 0;
         }
         else
            ret_val = DIS_ERROR_INSUFFICIENT_RESOURCES;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = DIS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = DIS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);

#else

   return(BTPS_ERROR_FEATURE_NOT_AVAILABLE);

#endif
}

   /* The following function is responsible for querying the current    */
   /* Hardware Revision characteristic value on the specified DIS       */
   /* instance.  The first parameter is the Bluetooth Stack ID of the   */
   /* Bluetooth Device.  The second parameter is the InstanceID returned*/
   /* from a successful call to DIS_Initialize_Server().  The final     */
   /* parameter is a pointer to a structure to return the current       */
   /* Hardware Revision for the specified DIS Service Instance.  The    */
   /* Hardware Revision Length should be at least                       */
   /* (DIS_MAXIMUM_SUPPORTED_STRING+1) to hold the Maximum allowable    */
   /* string (plus a single character to hold the NULL terminator) This */
   /* function returns a zero if successful or a negative return error  */
   /* code if an error occurs.                                          */
int BTPSAPI DIS_Query_Hardware_Revision(unsigned int BluetoothStackID, unsigned int InstanceID, char *Hardware_Revision)
{
#if BTPS_CONFIGURATION_DIS_SUPPORT_HARDWARE_REVISION

   int                  ret_val;
   Word_t               Length;
   DISServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (Hardware_Revision))
   {
      /* Acquire the specified DIS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Get the current Hardware Revision.                          */
         Length = InstanceData[InstanceID-1].Hardware_Revision.Characteristic_Length;
         if(InstanceData[InstanceID-1].Hardware_Revision.Data)
            BTPS_MemCopy(Hardware_Revision, InstanceData[InstanceID-1].Hardware_Revision.Data, Length);

         Hardware_Revision[Length] = 0;

         /* Return success to the caller.                               */
         ret_val = 0;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = DIS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = DIS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);

#else

   return(BTPS_ERROR_FEATURE_NOT_AVAILABLE);

#endif
}

   /* The following function is responsible for setting the Firmware    */
   /* Revision characteristic on the specified Device Information       */
   /* Service instance.  The first parameter is the Bluetooth Stack ID  */
   /* of the Bluetooth Device.  The second parameter is the InstanceID  */
   /* returned from a successful call to DIS_Initialize_Server().  The  */
   /* final parameter is the Firmware Revision to set as the current    */
   /* Firmware Revision for the specified Device Information Service    */
   /* Instance.  The Firmware Revision parameter must be a pointer to a */
   /* NULL terminated ASCII String of at most                           */
   /* DIS_MAXIMUM_SUPPORTED_STRING (not counting the trailing NULL      */
   /* terminator).  This function returns a zero if successful or a     */
   /* negative return error code if an error occurs.                    */
int BTPSAPI DIS_Set_Firmware_Revision(unsigned int BluetoothStackID, unsigned int InstanceID, char *FirmwareRevision)
{
#if BTPS_CONFIGURATION_DIS_SUPPORT_FIRMWARE_REVISION

   int                  ret_val;
   unsigned int         StringLength;
   DISServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (FirmwareRevision) && ((StringLength = BTPS_StringLength(FirmwareRevision)) <= DIS_MAXIMUM_SUPPORTED_STRING))
   {
      /* Acquire the specified DIS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Free any previous buffer allocated.                         */
         if(InstanceData[InstanceID-1].Firmware_Revision.Data)
         {
            BTPS_FreeMemory(InstanceData[InstanceID-1].Firmware_Revision.Data);
            InstanceData[InstanceID-1].Firmware_Revision.Data = NULL;
         }

         /* Attempt to allocate memory for the Firmware Revision string.*/
         if((InstanceData[InstanceID-1].Firmware_Revision.Data = BTPS_AllocateMemory(StringLength)) != NULL)
         {
            /* Set the length and copy the data into the Firmware       */
            /* Revision.                                                */
            InstanceData[InstanceID-1].Firmware_Revision.Characteristic_Length = (Word_t)StringLength;
            BTPS_MemCopy(InstanceData[InstanceID-1].Firmware_Revision.Data, FirmwareRevision, StringLength);

            /* Return success to the caller.                            */
            ret_val = 0;
         }
         else
            ret_val = DIS_ERROR_INSUFFICIENT_RESOURCES;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = DIS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = DIS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);

#else

   return(BTPS_ERROR_FEATURE_NOT_AVAILABLE);

#endif
}

   /* The following function is responsible for querying the current    */
   /* Firmware Revision characteristic value on the specified DIS       */
   /* instance.  The first parameter is the Bluetooth Stack ID of the   */
   /* Bluetooth Device.  The second parameter is the InstanceID returned*/
   /* from a successful call to DIS_Initialize_Server().  The final     */
   /* parameter is a pointer to a structure to return the current       */
   /* Firmware Revision for the specified DIS Service Instance.  The    */
   /* Firmware Revision Length should be at least                       */
   /* (DIS_MAXIMUM_SUPPORTED_STRING+1) to hold the Maximum allowable    */
   /* string (plus a single character to hold the NULL terminator) This */
   /* function returns a zero if successful or a negative return error  */
   /* code if an error occurs.                                          */
int BTPSAPI DIS_Query_Firmware_Revision(unsigned int BluetoothStackID, unsigned int InstanceID, char *FirmwareRevision)
{
#if BTPS_CONFIGURATION_DIS_SUPPORT_FIRMWARE_REVISION

   int                  ret_val;
   Word_t               Length;
   DISServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (FirmwareRevision))
   {
      /* Acquire the specified DIS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Get the current Firmware Revision.                          */
         Length = InstanceData[InstanceID-1].Firmware_Revision.Characteristic_Length;
         if(InstanceData[InstanceID-1].Firmware_Revision.Data)
            BTPS_MemCopy(FirmwareRevision, InstanceData[InstanceID-1].Firmware_Revision.Data, Length);

         FirmwareRevision[Length] = 0;

         /* Return success to the caller.                               */
         ret_val = 0;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = DIS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = DIS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);

#else

   return(BTPS_ERROR_FEATURE_NOT_AVAILABLE);

#endif
}

   /* The following function is responsible for setting the Software    */
   /* Revision characteristic on the specified Device Information       */
   /* Service instance.  The first parameter is the Bluetooth Stack ID  */
   /* of the Bluetooth Device.  The second parameter is the InstanceID  */
   /* returned from a successful call to DIS_Initialize_Server().  The  */
   /* final parameter is the Software Revision to set as the current    */
   /* Software Revision for the specified Device Information Service    */
   /* Instance.  The Software Revision parameter must be a pointer to a */
   /* NULL terminated ASCII String of at most                           */
   /* DIS_MAXIMUM_SUPPORTED_STRING (not counting the trailing NULL      */
   /* terminator).  This function returns a zero if successful or a     */
   /* negative return error code if an error occurs.                    */
int BTPSAPI DIS_Set_Software_Revision(unsigned int BluetoothStackID, unsigned int InstanceID, char *SoftwareRevision)
{
#if BTPS_CONFIGURATION_DIS_SUPPORT_SOFTWARE_REVISION

   int                  ret_val;
   unsigned int         StringLength;
   DISServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (SoftwareRevision) && ((StringLength = BTPS_StringLength(SoftwareRevision)) <= DIS_MAXIMUM_SUPPORTED_STRING))
   {
      /* Acquire the specified DIS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Free any previous buffer allocated.                         */
         if(InstanceData[InstanceID-1].Software_Revision.Data)
         {
            BTPS_FreeMemory(InstanceData[InstanceID-1].Software_Revision.Data);
            InstanceData[InstanceID-1].Software_Revision.Data = NULL;
         }

         /* Attempt to allocate memory for the Software Revision string.*/
         if((InstanceData[InstanceID-1].Software_Revision.Data = BTPS_AllocateMemory(StringLength)) != NULL)
         {
            /* Set the length and copy the data into the Software       */
            /* Revision.                                                */
            InstanceData[InstanceID-1].Software_Revision.Characteristic_Length = (Word_t)StringLength;
            BTPS_MemCopy(InstanceData[InstanceID-1].Software_Revision.Data, SoftwareRevision, StringLength);

            /* Return success to the caller.                            */
            ret_val = 0;
         }
         else
            ret_val = DIS_ERROR_INSUFFICIENT_RESOURCES;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = DIS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = DIS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);

#else

   return(BTPS_ERROR_FEATURE_NOT_AVAILABLE);

#endif
}

   /* The following function is responsible for querying the current    */
   /* Software Revision characteristic value on the specified DIS       */
   /* instance.  The first parameter is the Bluetooth Stack ID of the   */
   /* Bluetooth Device.  The second parameter is the InstanceID returned*/
   /* from a successful call to DIS_Initialize_Server().  The final     */
   /* parameter is a pointer to a structure to return the current       */
   /* Software Revision for the specified DIS Service Instance.  The    */
   /* Software Revision Length should be at least                       */
   /* (DIS_MAXIMUM_SUPPORTED_STRING+1) to hold the Maximum allowable    */
   /* string (plus a single character to hold the NULL terminator) This */
   /* function returns a zero if successful or a negative return error  */
   /* code if an error occurs.                                          */
int BTPSAPI DIS_Query_Software_Revision(unsigned int BluetoothStackID, unsigned int InstanceID, char *SoftwareRevision)
{
#if BTPS_CONFIGURATION_DIS_SUPPORT_SOFTWARE_REVISION

   int                  ret_val;
   Word_t               Length;
   DISServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (SoftwareRevision))
   {
      /* Acquire the specified DIS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Get the current Software Revision.                          */
         Length = InstanceData[InstanceID-1].Software_Revision.Characteristic_Length;
         if(InstanceData[InstanceID-1].Software_Revision.Data)
            BTPS_MemCopy(SoftwareRevision, InstanceData[InstanceID-1].Software_Revision.Data, Length);

         SoftwareRevision[Length] = 0;

         /* Return success to the caller.                               */
         ret_val = 0;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = DIS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = DIS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);

#else

   return(BTPS_ERROR_FEATURE_NOT_AVAILABLE);

#endif
}

   /* The following function is responsible for setting the System ID   */
   /* characteristic on the specified Device Information Service        */
   /* instance.  The first parameter is the Bluetooth Stack ID of the   */
   /* Bluetooth Device.  The second parameter is the InstanceID returned*/
   /* from a successful call to DIS_Initialize_Server().  The final     */
   /* parameter is the System ID to set as the current System ID for the*/
   /* specified Device Information Service Instance.  This function     */
   /* returns a zero if successful or a negative return error code if an*/
   /* error occurs.                                                     */
int BTPSAPI DIS_Set_System_ID(unsigned int BluetoothStackID, unsigned int InstanceID, DIS_System_ID_Value_t *SystemID)
{
#if BTPS_CONFIGURATION_DIS_SUPPORT_SYSTEM_ID

   int                  ret_val;
   DISServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (SystemID))
   {
      /* Acquire the specified DIS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Set the System ID.                                          */
         BTPS_MemCopy(InstanceData[InstanceID-1].System_ID.Data, SystemID, DIS_SYSTEM_ID_VALUE_DATA_SIZE);

         /* Return success to the caller.                               */
         ret_val = 0;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = DIS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = DIS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);

#else

   return(BTPS_ERROR_FEATURE_NOT_AVAILABLE);

#endif
}

   /* The following function is responsible for querying the current    */
   /* System ID characteristic value on the specified DIS instance.  The*/
   /* first parameter is the Bluetooth Stack ID of the Bluetooth Device.*/
   /* The second parameter is the InstanceID returned from a successful */
   /* call to DIS_Initialize_Server().  The final parameter is a pointer*/
   /* to a structure to return the current System ID for the specified  */
   /* DIS Service Instance.  This function returns a zero if successful */
   /* or a negative return error code if an error occurs.               */
int BTPSAPI DIS_Query_System_ID(unsigned int BluetoothStackID, unsigned int InstanceID, DIS_System_ID_Value_t *SystemID)
{
#if BTPS_CONFIGURATION_DIS_SUPPORT_SYSTEM_ID

   int                  ret_val;
   DISServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (SystemID))
   {
      /* Acquire the specified DIS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Get the current System ID.                                  */
         BTPS_MemCopy(SystemID, InstanceData[InstanceID-1].System_ID.Data, DIS_SYSTEM_ID_VALUE_DATA_SIZE);

         /* Return success to the caller.                               */
         ret_val = 0;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = DIS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = DIS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);

#else

   return(BTPS_ERROR_FEATURE_NOT_AVAILABLE);

#endif
}

   /* The following function is responsible for setting the IEEE        */
   /* Certification Data characteristic on the specified Device         */
   /* Information Service instance.  The first parameter is the         */
   /* Bluetooth Stack ID of the Bluetooth Device.  The second parameter */
   /* is the InstanceID returned from a successful call to              */
   /* DIS_Initialize_Server().  The third parameter is the length of the*/
   /* IEEE Certification Data.  The final parameter is the IEEE         */
   /* Certification Data to set as the current IEEE Certification Data  */
   /* for the specified Device Information Service Instance.  The IEEE  */
   /* Certification Data parameter must be a pointer to a array of at   */
   /* most DIS_MAXIMUM_SUPPORTED_STRING characters.  This function      */
   /* returns a zero if successful or a negative return error code if an*/
   /* error occurs.                                                     */
int BTPSAPI DIS_Set_IEEE_Certification_Data(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int Length, Byte_t *IEEE_Certification_Data)
{
#if BTPS_CONFIGURATION_DIS_SUPPORT_IEEE_CERTIFICATION_DATA

   int                  ret_val;
   DISServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (Length) && (Length <= DIS_MAXIMUM_SUPPORTED_STRING) && (IEEE_Certification_Data))
   {
      /* Acquire the specified DIS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Free any previous buffer allocated.                         */
         if(InstanceData[InstanceID-1].IEEE_Certification_Data.Data)
         {
            BTPS_FreeMemory(InstanceData[InstanceID-1].IEEE_Certification_Data.Data);
            InstanceData[InstanceID-1].IEEE_Certification_Data.Data = NULL;
         }

         /* Attempt to allocate memory for the IEEE Certification Data  */
         /* string.                                                     */
         if((InstanceData[InstanceID-1].IEEE_Certification_Data.Data = BTPS_AllocateMemory(Length)) != NULL)
         {
            /* Set the length and copy the data into the IEEE           */
            /* Certification Data.                                      */
            InstanceData[InstanceID-1].IEEE_Certification_Data.Characteristic_Length = (Word_t)Length;
            BTPS_MemCopy(InstanceData[InstanceID-1].IEEE_Certification_Data.Data, IEEE_Certification_Data, Length);

            /* Return success to the caller.                            */
            ret_val = 0;
         }
         else
            ret_val = DIS_ERROR_INSUFFICIENT_RESOURCES;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = DIS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = DIS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);

#else

   return(BTPS_ERROR_FEATURE_NOT_AVAILABLE);

#endif
}

   /* The following function is responsible for querying the current    */
   /* IEEE Certification Data characteristic value on the specified DIS */
   /* instance.  The first parameter is the Bluetooth Stack ID of the   */
   /* Bluetooth Device.  The second parameter is the InstanceID returned*/
   /* from a successful call to DIS_Initialize_Server().  The final     */
   /* parameter is a pointer to a structure to return the current IEEE  */
   /* Certification Data for the specified DIS Service Instance.  The   */
   /* IEEE Certification Data Length should be at least                 */
   /* (DIS_MAXIMUM_SUPPORTED_STRING) to hold the Maximum allowable IEEE */
   /* Certification Data.  This function the length of the IEEE         */
   /* Certification Data if successful or a negative return error code  */
   /* if an error occurs.                                               */
int BTPSAPI DIS_Query_IEEE_Certification_Data(unsigned int BluetoothStackID, unsigned int InstanceID, Byte_t *IEEE_Certification_Data)
{
#if BTPS_CONFIGURATION_DIS_SUPPORT_IEEE_CERTIFICATION_DATA

   int                  ret_val;
   Word_t               Length;
   DISServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (IEEE_Certification_Data))
   {
      /* Acquire the specified DIS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Get the current IEEE Certification Data.                    */
         Length = InstanceData[InstanceID-1].IEEE_Certification_Data.Characteristic_Length;
         if(InstanceData[InstanceID-1].IEEE_Certification_Data.Data)
            BTPS_MemCopy(IEEE_Certification_Data, InstanceData[InstanceID-1].IEEE_Certification_Data.Data, Length);

         /* Return the number of bytes in the IEEE Certification Data to*/
         /* the caller.                                                 */
         ret_val = (int)Length;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = DIS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = DIS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);

#else

   return(BTPS_ERROR_FEATURE_NOT_AVAILABLE);

#endif
}

   /* The following function is responsible for setting the Plug and    */
   /* Play (PnP) ID characteristic on the specified Device Information  */
   /* Service instance.  The first parameter is the Bluetooth Stack ID  */
   /* of the Bluetooth Device.  The second parameter is the InstanceID  */
   /* returned from a successful call to DIS_Initialize_Server().  The  */
   /* final parameter is the PnP ID Data to set as the current PnP ID   */
   /* Data for the specified Device Information Service Instance.  This */
   /* function returns a zero if successful or a negative return error  */
   /* code if an error occurs.                                          */
int BTPSAPI DIS_Set_PNP_ID(unsigned int BluetoothStackID, unsigned int InstanceID, DIS_PNP_ID_Data_t *PNP_ID_Data)
{
#if BTPS_CONFIGURATION_DIS_SUPPORT_PNP_ID

   int                  ret_val;
   DISServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (PNP_ID_Data))
   {
      /* Acquire the specified DIS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Format the PnP ID value that will be sent over the air from */
         /* the user specified data.                                    */
         ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(InstanceData[InstanceID-1].PNP_ID_Buffer.VendorID_Source), PNP_ID_Data->VendorID_Source);
         ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(InstanceData[InstanceID-1].PNP_ID_Buffer.VendorID), PNP_ID_Data->VendorID);
         ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(InstanceData[InstanceID-1].PNP_ID_Buffer.ProductID), PNP_ID_Data->ProductID);
         ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(InstanceData[InstanceID-1].PNP_ID_Buffer.ProductVersion), PNP_ID_Data->ProductVersion);

         /* Return success to the caller.                               */
         ret_val = 0;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = DIS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = DIS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);

#else

   return(BTPS_ERROR_FEATURE_NOT_AVAILABLE);

#endif
}

   /* The following function is responsible for querying the current PnP*/
   /* ID Data characteristic value on the specified DIS instance.  The  */
   /* first parameter is the Bluetooth Stack ID of the Bluetooth Device.*/
   /* The second parameter is the InstanceID returned from a successful */
   /* call to DIS_Initialize_Server().  The final parameter is a pointer*/
   /* to a structure to return the current PnP ID Data for the specified*/
   /* DIS Service Instance.  This function returns a zero if successful */
   /* or a negative return error code if an error occurs.               */
int BTPSAPI DIS_Query_PNP_ID(unsigned int BluetoothStackID, unsigned int InstanceID, DIS_PNP_ID_Data_t *PNP_ID_Data)
{
#if BTPS_CONFIGURATION_DIS_SUPPORT_PNP_ID

   int                  ret_val;
   DISServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (PNP_ID_Data))
   {
      /* Acquire the specified DIS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Parse the current PnP data and return to the caller.        */
         PNP_ID_Data->VendorID_Source = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(InstanceData[InstanceID-1].PNP_ID_Buffer.VendorID_Source));
         PNP_ID_Data->VendorID        = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(InstanceData[InstanceID-1].PNP_ID_Buffer.VendorID));
         PNP_ID_Data->ProductID       = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(InstanceData[InstanceID-1].PNP_ID_Buffer.ProductID));
         PNP_ID_Data->ProductVersion  = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(InstanceData[InstanceID-1].PNP_ID_Buffer.ProductVersion));

         /* Return success to the caller.                               */
         ret_val                      = 0;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = DIS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = DIS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);

#else

   return(BTPS_ERROR_FEATURE_NOT_AVAILABLE);

#endif
}

   /* DIS Client API.                                                   */

   /* The following function is responsible for parsing a PnP ID value  */
   /* received from a remote DIS Server.  The first parameter is the    */
   /* length of the value returned by the remote DIS Server.  The second*/
   /* parameter is a pointer to the data returned by the remote DIS     */
   /* Server.  The final parameter is a pointer to store the parsed PnP */
   /* ID data.  This function returns a zero if successful or a negative*/
   /* return error code if an error occurs.                             */
int BTPSAPI DIS_Decode_PNP_ID(unsigned int ValueLength, Byte_t *Value, DIS_PNP_ID_Data_t *PNP_ID_Data)
{
   int ret_val;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((ValueLength) && (Value) && (PNP_ID_Data))
   {
      /* Verify that the value length is the right size.                */
      if(ValueLength == DIS_PNP_ID_VALUE_DATA_SIZE)
      {
         /* Parse the current PnP data and return to the caller.        */
         PNP_ID_Data->VendorID_Source = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(((DIS_PNP_ID_Value_t *)Value)->VendorID_Source));
         PNP_ID_Data->VendorID        = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(((DIS_PNP_ID_Value_t *)Value)->VendorID));
         PNP_ID_Data->ProductID       = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(((DIS_PNP_ID_Value_t *)Value)->ProductID));
         PNP_ID_Data->ProductVersion  = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(((DIS_PNP_ID_Value_t *)Value)->ProductVersion));

         /* Return success to the caller.                               */
         ret_val                      = 0;
      }
      else
         ret_val = DIS_ERROR_MALFORMATTED_DATA;
   }
   else
      ret_val = DIS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}
