/*****< ans.c >****************************************************************/
/*      Copyright 2011 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  ANS - Bluetooth Stack Alert Notification Service (GATT Based) for         */
/*        Stonestreet One Bluetooth Protocol Stack.                           */
/*                                                                            */
/*  Author:  Tim Cook                                                         */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   09/26/11  T. Cook        Initial creation.                               */
/******************************************************************************/
#include "SS1BTPS.h"        /* Bluetooth Stack API Prototypes/Constants.      */
#include "SS1BTGAT.h"       /* Bluetooth Stack GATT API Prototypes/Constants. */
#include "SS1BTANS.h"       /* Bluetooth ANS API Prototypes/Constants.        */

#include "BTPSKRNL.h"       /* BTPS Kernel Prototypes/Constants.              */
#include "ANS.h"            /* Bluetooth ANS Prototypes/Constants.            */

   /* The following controls the number of supported ANS instances.     */
#define ANS_MAXIMUM_SUPPORTED_INSTANCES                 (BTPS_CONFIGURATION_ANS_MAXIMUM_SUPPORTED_INSTANCES)

   /* The following structure defines the Instance Data that must be    */
   /* unique for each ANS service registered (Only 1 per Bluetooth      */
   /* Stack).                                                           */
typedef __PACKED_STRUCT_BEGIN__ struct _tagANS_Instance_Data_t
{
   NonAlignedWord_t SupportedNewAlertCategoryLength;
   NonAlignedByte_t SupportedNewAlertCategoryInstance[2];
   NonAlignedWord_t SupportedUnreadAlertCategoryLength;
   NonAlignedByte_t SupportedUnreadAlertCategoryInstance[2];
} __PACKED_STRUCT_END__ ANS_Instance_Data_t;

#define ANS_INSTANCE_DATA_SIZE                           (sizeof(ANS_Instance_Data_t))

   /* The following define the instance tags for each ANS service data  */
   /* that is unique per registered service.                            */
#define ANS_SUPPORTED_NEW_ALERT_CATEGORY_INSTANCE_TAG_VALUE          (BTPS_STRUCTURE_OFFSET(ANS_Instance_Data_t, SupportedNewAlertCategoryLength))
#define ANS_SUPPORTED_UNREAD_ALERT_CATEGORY_INSTANCE_TAG_VALUE       (BTPS_STRUCTURE_OFFSET(ANS_Instance_Data_t, SupportedUnreadAlertCategoryLength))

   /*********************************************************************/
   /**               Alert Notification Service Table                  **/
   /*********************************************************************/

   /* The Alert Notification Service Declaration UUID.                  */
static BTPSCONST GATT_Primary_Service_16_Entry_t ANS_Service_UUID =
{
   ANS_SERVICE_BLUETOOTH_UUID_CONSTANT
};

   /* The Supported New Alert Category Characteristic Declaration.      */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t ANS_Supported_New_Alert_Category_Declaration =
{
   GATT_CHARACTERISTIC_PROPERTIES_READ,
   ANS_SUPORTED_NEW_ALERT_CATEGORY_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The Supported New Alert Category Characteristic Value.            */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t  ANS_Supported_New_Alert_Category_Value =
{
   ANS_SUPORTED_NEW_ALERT_CATEGORY_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   ANS_SUPPORTED_NEW_ALERT_CATEGORY_INSTANCE_TAG_VALUE,
   NULL
};

   /* The New Alert Characteristic Declaration.                         */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t ANS_New_Alert_Declaration =
{
   GATT_CHARACTERISTIC_PROPERTIES_NOTIFY,
   ANS_NEW_ALERT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The New Alert Characteristic Value.                               */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t  ANS_New_Alert_Value =
{
   ANS_NEW_ALERT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   0,
   NULL
};

   /* The Supported Unread Alert Category Characteristic Declaration.   */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t ANS_Supported_Unread_Alert_Category_Declaration =
{
   GATT_CHARACTERISTIC_PROPERTIES_READ,
   ANS_SUPPORTED_UNREAD_ALERT_CATEGORY_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The Supported Unread Alert Category Characteristic Value.         */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t  ANS_Supported_Unread_Alert_Category_Value =
{
   ANS_SUPPORTED_UNREAD_ALERT_CATEGORY_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   ANS_SUPPORTED_UNREAD_ALERT_CATEGORY_INSTANCE_TAG_VALUE,
   NULL
};

   /* The Unread Alert Status Characteristic Declaration.               */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t ANS_Unread_Alert_Status_Declaration =
{
   GATT_CHARACTERISTIC_PROPERTIES_NOTIFY,
   ANS_UNREAD_ALERT_STATUS_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The Unread Alert Status Characteristic Value.                     */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t  ANS_Unread_Alert_Status_Value =
{
   ANS_UNREAD_ALERT_STATUS_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   0,
   NULL
};

   /* The Control Point Characteristic Declaration.                     */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t ANS_Control_Point_Declaration =
{
   GATT_CHARACTERISTIC_PROPERTIES_WRITE,
   ANS_CONTROL_POINT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The Control Point Characteristic Value.                           */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t  ANS_Control_Point_Value =
{
   ANS_CONTROL_POINT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   0,
   NULL
};

   /* Client Characteristic Configuration.                              */
static BTPSCONST GATT_Characteristic_Descriptor_16_Entry_t Client_Characteristic_Configuration =
{
   GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_BLUETOOTH_UUID_CONSTANT,
   GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_LENGTH,
   NULL
};

   /* The following defines the Alert Notification service that is      */
   /* registered with the GATT_Register_Service function call.          */
   /* * NOTE * This array will be registered with GATT in the call to   */
   /*          GATT_Register_Service.                                   */
static BTPSCONST GATT_Service_Attribute_Entry_t Alert_Notification_Service[] =
{
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetPrimaryService16,            (Byte_t *)&ANS_Service_UUID},
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetCharacteristicDeclaration16, (Byte_t *)&ANS_Supported_New_Alert_Category_Declaration},
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetCharacteristicValue16,       (Byte_t *)&ANS_Supported_New_Alert_Category_Value},
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetCharacteristicDeclaration16, (Byte_t *)&ANS_New_Alert_Declaration},
   {0,                                      aetCharacteristicValue16,       (Byte_t *)&ANS_New_Alert_Value},
   {GATT_ATTRIBUTE_FLAGS_READABLE_WRITABLE, aetCharacteristicDescriptor16,  (Byte_t *)&Client_Characteristic_Configuration},
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetCharacteristicDeclaration16, (Byte_t *)&ANS_Supported_Unread_Alert_Category_Declaration},
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetCharacteristicValue16,       (Byte_t *)&ANS_Supported_Unread_Alert_Category_Value},
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetCharacteristicDeclaration16, (Byte_t *)&ANS_Unread_Alert_Status_Declaration},
   {0,                                      aetCharacteristicValue16,       (Byte_t *)&ANS_Unread_Alert_Status_Value},
   {GATT_ATTRIBUTE_FLAGS_READABLE_WRITABLE, aetCharacteristicDescriptor16,  (Byte_t *)&Client_Characteristic_Configuration},
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetCharacteristicDeclaration16, (Byte_t *)&ANS_Control_Point_Declaration},
   {GATT_ATTRIBUTE_FLAGS_WRITABLE,          aetCharacteristicValue16,       (Byte_t *)&ANS_Control_Point_Value},
};

#define ALERT_NOTIFICATION_SERVICE_ATTRIBUTE_COUNT                         (sizeof(Alert_Notification_Service)/sizeof(GATT_Service_Attribute_Entry_t))

#define ANS_NEW_ALERT_CHARACTERISTIC_OFFSET                                4
#define ANS_NEW_ALERT_CLIENT_CHARACTERISTIC_CONFIGURATION_OFFSET           5
#define ANS_UNREAD_ALERT_STATUS_CHARACTERISTIC_OFFSET                      9
#define ANS_UNREAD_ALERT_STATUS_CLIENT_CHARACTERISTIC_CONFIGURATION_OFFSET 10

   /* The following defines the ANS GATT Service Flags MASK that should */
   /* be ANSed into GATT_Register_Service when the ANS Service is       */
   /* registered.                                                       */
#define ANS_SERVICE_FLAGS                                            (GATT_SERVICE_FLAGS_LE_SERVICE)

   /*********************************************************************/
   /**                    END OF SERVICE TABLE                         **/
   /*********************************************************************/

   /* The following MACRO is a utility MACRO that exists to determine   */
   /* if the Command ID that is the only parameter to this MACRO is     */
   /* valid.                                                            */
#define ANS_COMMAND_ID_VALID(_x)                         ((((Byte_t)(_x)) >= ANS_COMMAND_ID_ENABLE_NEW_INCOMING_ALERT_NOTIFICATION) && (((Byte_t)(_x)) <= ANS_COMMAND_ID_NOTIFY_UNREAD_CATEGORY_STATUS_IMMEDIATELY))

   /* The following MACRO is a utility MACRO that exists to determine if*/
   /* the Category ID that is the only parameter to this MACRO is valid */
   /* (and allow it to be the All Categories special category).         */
#define ANS_CATEGORY_ID_VALID_INCL_ALL_CATEGORIES(_x)    (((((Byte_t)(_x)) >= ANS_ALERT_CATEGORY_ID_SIMPLE_ALERT) && (((Byte_t)(_x)) <= ANS_ALERT_CATEGORY_ID_INSTANT_MESSAGE)) || (((Byte_t)(_x)) == ANS_ALERT_CATEGORY_ID_ALL_CATEGORIES))

   /* The following MACRO is a utility MACRO that exists to determine if*/
   /* the Category ID that is the only parameter to this MACRO is valid */
   /* (and do not allow it to be the All Categories special category).  */
#define ANS_CATEGORY_ID_VALID_EXCL_ALL_CATEGORIES(_x)    (((((Byte_t)(_x)) >= ANS_ALERT_CATEGORY_ID_SIMPLE_ALERT) && (((Byte_t)(_x)) <= ANS_ALERT_CATEGORY_ID_INSTANT_MESSAGE)))

   /* The following type defines a union large enough to hold all events*/
   /* dispatched by this module.                                        */
typedef union
{
   ANS_Read_Client_Configuration_Data_t   ANS_Read_Client_Configuration_Data;
   ANS_Client_Configuration_Update_Data_t ANS_Client_Configuration_Update_Data;
   ANS_Control_Point_Command_Data_t       ANS_Control_Point_Command_Data;
} ANS_Event_Data_Buffer_t;

#define ANS_EVENT_DATA_BUFFER_SIZE                      (sizeof(ANS_Event_Data_Buffer_t))

   /* ANS Service Instance Block.  This structure contains All          */
   /* information associated with a specific Bluetooth Stack ID (member */
   /* is present in this structure).                                    */
typedef struct _tagANSServerInstance_t
{
   unsigned int         BluetoothStackID;
   unsigned int         ServiceID;
   ANS_Event_Callback_t EventCallback;
   unsigned long        CallbackParameter;
} ANSServerInstance_t;

#define ANS_SERVER_INSTANCE_DATA_SIZE                    (sizeof(ANSServerInstance_t))

   /* Internal Variables to this Module (Remember that all variables    */
   /* declared static are initialized to 0 automatically by the         */
   /* compiler as part of standard C/C++).                              */

static ANS_Instance_Data_t InstanceData[ANS_MAXIMUM_SUPPORTED_INSTANCES];
                                            /* Variable which holds all */
                                            /* data that is unique for  */
                                            /* each service instance.   */

static ANSServerInstance_t InstanceList[ANS_MAXIMUM_SUPPORTED_INSTANCES];
                                            /* Variable which holds the */
                                            /* service instance data.   */

static Boolean_t InstanceListInitialized;   /* Variable that flags that */
                                            /* is used to denote that   */
                                            /* this module has been     */
                                            /* successfully initialized.*/

   /* The following are the prototypes of local functions.              */
static Boolean_t InitializeModule(void);
static void CleanupModule(void);

static int DecodeSupportedCategories(unsigned int BufferLength, Byte_t *Value, Word_t *SupportedCategoriesMask);
static int DecodeControlPointCommand(Word_t ValueLength, Byte_t *Value, ANS_Control_Point_Command_t *Command, ANS_Category_Identification_t *CommandCategory);
static Boolean_t VerifyControlPointCommand(unsigned int InstanceID, ANS_Control_Point_Command_t Command, ANS_Category_Identification_t Category);
static int DecodeClientConfigurationValue(unsigned int BufferLength, Byte_t *Buffer, Boolean_t *NotificationsEnabled);

static ANS_Event_Data_t *FormatEventHeader(unsigned int BufferLength, Byte_t *Buffer, ANS_Event_Type_t EventType, unsigned int InstanceID, unsigned int ConnectionID, unsigned int *TransactionID, GATT_Connection_Type_t ConnectionType, BD_ADDR_t *BD_ADDR);

static Boolean_t InstanceRegisteredByStackID(unsigned int BluetoothStackID);
static ANSServerInstance_t *AcquireServiceInstance(unsigned int BluetoothStackID, unsigned int *InstanceID);

static int ANSRegisterService(unsigned int BluetoothStackID, ANS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange);

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

   /* The following function is used to decode a Supported Categories   */
   /* value.                                                            */
static int DecodeSupportedCategories(unsigned int BufferLength, Byte_t *Value, Word_t *SupportedCategoriesMask)
{
   int ret_val;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BufferLength) && (Value) && (SupportedCategoriesMask))
   {
      /* The first byte is a direct pass-through into the Supported     */
      /* Categories Mask.                                               */
      *SupportedCategoriesMask = (Word_t)Value[0];

      /* Check to see if the optional second byte has been specified.   */
      if(BufferLength >= 2)
      {
         /* The second byte has only two valid bits that are stored in  */
         /* the upper byte of the mask.                                 */
         *SupportedCategoriesMask |= (Word_t)((Value[1] << 8) & 0x0300);
      }

      /* Return success to the caller.                                  */
      ret_val                   = 0;
   }
   else
      ret_val = ANS_ERROR_INVALID_PARAMETER;

   /* Finally, return the result to the caller.                         */
   return(ret_val);
}

   /* The following function is a utility function that exists to decode*/
   /* a Control Point Command.  This function returns 0 if successful or*/
   /* a negative error code.                                            */
static int DecodeControlPointCommand(Word_t ValueLength, Byte_t *Value, ANS_Control_Point_Command_t *Command, ANS_Category_Identification_t *CommandCategory)
{
   int    ret_val = ANS_ERROR_MALFORMATTED_DATA;
   Byte_t TempByte;

   /* Verify that the input parameters are valid.                       */
   if((ValueLength == ANS_CONTROL_POINT_COMMAND_VALUE_DATA_SIZE) && (Value) && (Command) && (CommandCategory))
   {
      /* Read the CommandID and verify that we understand it.           */
      TempByte = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(((ANS_Control_Point_Command_Value_t *)Value)->CommandID));

      if(ANS_COMMAND_ID_VALID(TempByte))
      {
         /* Save the command.                                           */
         *Command = (ANS_Control_Point_Command_t)TempByte;

         /* Read the CategoryID and verify that we understand it.       */
         TempByte = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(((ANS_Control_Point_Command_Value_t *)Value)->CategoryID));

         if(ANS_CATEGORY_ID_VALID_INCL_ALL_CATEGORIES(TempByte))
         {
            /* Save the Category ID.                                    */
            *CommandCategory = (ANS_Category_Identification_t)TempByte;

            /* Return success to the caller.                            */
            ret_val          = 0;
         }
      }
   }
   else
   {
      if(ValueLength == ANS_CONTROL_POINT_COMMAND_VALUE_DATA_SIZE)
         ret_val = ANS_ERROR_INVALID_PARAMETER;
   }

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is used to verify that a command that was  */
   /* received is valid for a given category.                           */
static Boolean_t VerifyControlPointCommand(unsigned int InstanceID, ANS_Control_Point_Command_t Command, ANS_Category_Identification_t Category)
{
   Word_t    SupportedFeatures;
   Boolean_t ret_val = FALSE;

   /* Verify that the input parameters are valid.                       */
   if(InstanceID)
   {
      /* If the category is set to ciAllCategories no more check is     */
      /* needed.                                                        */
      if(Category != ciAllCategories)
      {
         /* Read the corret supported features mask based on the        */
         /* command.                                                    */
         switch(Command)
         {
            case pcEnable_New_Alert_Notifications:
            case pcDisable_New_Alert_Notifications:
            case pcNotify_New_Alert_Immediately:
               SupportedFeatures = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(InstanceData[InstanceID-1].SupportedNewAlertCategoryInstance));
               break;
            case pcEnable_Unread_Category_Notifications:
            case pcDisable_Unread_Category_Notifications:
            case pcNotify_Unread_Category_Immediately:
               SupportedFeatures = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(InstanceData[InstanceID-1].SupportedUnreadAlertCategoryInstance));
               break;
            default:
               /* To get rid of warnings flagged by some compilers we   */
               /* have this default case that sets the SupportedFeatures*/
               /* to 0.                                                 */
               SupportedFeatures = 0;
               break;
         }

         /* Verify that the category is enabled.                        */
         if(ANS_SERVER_INFORMATION_CATEGORY_ENABLED(&SupportedFeatures, Category))
         {
            /* Category is supported by the server so return TRUE.      */
            ret_val = TRUE;
         }
      }
      else
         ret_val = TRUE;
   }

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is a utility function that exists to decode*/
   /* an Client Configuration value into a user specified boolean value.*/
   /* This function returns the zero if successful or a negative error  */
   /* code.                                                             */
static int DecodeClientConfigurationValue(unsigned int BufferLength, Byte_t *Buffer, Boolean_t *NotificationsEnabled)
{
   int    ret_val = ANS_ERROR_MALFORMATTED_DATA;
   Word_t ClientConfiguration;

   /* Verify that the input parameters are valid.                       */
   if(((BufferLength == NON_ALIGNED_BYTE_SIZE) || (BufferLength == GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_LENGTH)) && (Buffer) && (NotificationsEnabled))
   {
      /* Read the requested Client Configuration.                       */
      if(BufferLength == NON_ALIGNED_BYTE_SIZE)
         ClientConfiguration = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(Buffer);
      else
         ClientConfiguration = READ_UNALIGNED_WORD_LITTLE_ENDIAN(Buffer);

      if(ClientConfiguration & GATT_CLIENT_CONFIGURATION_CHARACTERISTIC_NOTIFY_ENABLE)
      {
         *NotificationsEnabled = TRUE;
         ret_val               = 0;
      }
      else
      {
         if(!ClientConfiguration)
         {
            *NotificationsEnabled = FALSE;
            ret_val               = 0;
         }
      }
   }
   else
   {
      if(BufferLength == GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_LENGTH)
         ret_val = ANS_ERROR_INVALID_PARAMETER;
   }

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is a utility function that exists to format*/
   /* a ANS Event into the specified buffer.                            */
   /* * NOTE * TransactionID is optional and may be set to NULL.        */
   /* * NOTE * BD_ADDR is NOT optional and may NOT be set to NULL.      */
static ANS_Event_Data_t *FormatEventHeader(unsigned int BufferLength, Byte_t *Buffer, ANS_Event_Type_t EventType, unsigned int InstanceID, unsigned int ConnectionID, unsigned int *TransactionID, GATT_Connection_Type_t ConnectionType, BD_ADDR_t *BD_ADDR)
{
   ANS_Event_Data_t *EventData = NULL;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BufferLength >= (ANS_EVENT_DATA_SIZE + ANS_EVENT_DATA_BUFFER_SIZE)) && (Buffer) && (BD_ADDR))
   {
      /* Format the header of the event, that is data that is common to */
      /* all events.                                                    */
      BTPS_MemInitialize(Buffer, 0, BufferLength);

      EventData                                                              = (ANS_Event_Data_t *)Buffer;
      EventData->Event_Data_Type                                             = EventType;
      EventData->Event_Data.ANS_Read_Client_Configuration_Data               = (ANS_Read_Client_Configuration_Data_t *)(((Byte_t *)EventData) + ANS_EVENT_DATA_SIZE);

      EventData->Event_Data.ANS_Read_Client_Configuration_Data->InstanceID   = InstanceID;
      EventData->Event_Data.ANS_Read_Client_Configuration_Data->ConnectionID = ConnectionID;

      if(TransactionID)
      {
         EventData->Event_Data.ANS_Read_Client_Configuration_Data->TransactionID  = *TransactionID;
         EventData->Event_Data.ANS_Read_Client_Configuration_Data->ConnectionType = ConnectionType;
         EventData->Event_Data.ANS_Read_Client_Configuration_Data->RemoteDevice   = *BD_ADDR;
      }
      else
      {
         EventData->Event_Data.ANS_Client_Configuration_Update_Data->ConnectionType = ConnectionType;
         EventData->Event_Data.ANS_Client_Configuration_Update_Data->RemoteDevice   = *BD_ADDR;
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

   for(Index=0;Index<ANS_MAXIMUM_SUPPORTED_INSTANCES;Index++)
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
static ANSServerInstance_t *AcquireServiceInstance(unsigned int BluetoothStackID, unsigned int *InstanceID)
{
   unsigned int          LocalInstanceID;
   unsigned int          Index;
   ANSServerInstance_t *ret_val = NULL;

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
         if((LocalInstanceID) && (LocalInstanceID <= ANS_MAXIMUM_SUPPORTED_INSTANCES))
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
               for(Index=0;Index<ANS_MAXIMUM_SUPPORTED_INSTANCES;Index++)
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
   /* register an ANS Service.  This function returns the positive,     */
   /* non-zero, Instance ID on success or a negative error code.        */
static int ANSRegisterService(unsigned int BluetoothStackID, ANS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange)
{
   int                  ret_val;
   unsigned int         InstanceID;
   ANSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (EventCallback) && (ServiceID) && (ServiceHandleRange))
   {
      /* Verify that no instance is registered to this Bluetooth Stack. */
      if(!InstanceRegisteredByStackID(BluetoothStackID))
      {
         /* Acquire a free ANS Instance.                                */
         InstanceID = 0;
         if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
         {
            /* Call GATT to register the ANS service.                   */
            ret_val = GATT_Register_Service(BluetoothStackID, ANS_SERVICE_FLAGS, ALERT_NOTIFICATION_SERVICE_ATTRIBUTE_COUNT, (GATT_Service_Attribute_Entry_t *)Alert_Notification_Service, ServiceHandleRange, GATT_ServerEventCallback, InstanceID);
            if(ret_val > 0)
            {
               /* Save the Instance information.                        */
               ServiceInstance->BluetoothStackID   = BluetoothStackID;
               ServiceInstance->ServiceID          = (unsigned int)ret_val;
               ServiceInstance->EventCallback      = EventCallback;
               ServiceInstance->CallbackParameter  = CallbackParameter;
               *ServiceID                          = (unsigned int)ret_val;

               /* Initialize the Instance Data for this instance.       */
               BTPS_MemInitialize(&InstanceData[InstanceID-1], 0, ANS_INSTANCE_DATA_SIZE);
               ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(InstanceData[InstanceID-1].SupportedNewAlertCategoryLength), ANS_SUPPORTED_NEW_ALERT_CATEGORY_VALUE_LENGTH);
               ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(InstanceData[InstanceID-1].SupportedUnreadAlertCategoryLength), ANS_SUPPORTED_UNREAD_ALERT_CATEGORY_VALUE_LENGTH);

               /* Return the ANS Instance ID.                           */
               ret_val                             = (int)InstanceID;
            }

            /* UnLock the previously locked Bluetooth Stack.            */
            BSC_UnLockBluetoothStack(BluetoothStackID);
         }
         else
            ret_val = ANS_ERROR_INSUFFICIENT_RESOURCES;
      }
      else
         ret_val = ANS_ERROR_SERVICE_ALREADY_REGISTERED;
   }
   else
      ret_val = ANS_ERROR_INVALID_PARAMETER;

   /* Finally just return the result to the caller.                     */
   return(ret_val);
}

   /* The following function is the GATT Server Event Callback that     */
   /* handles all requests made to the PASS Service for all registered  */
   /* instances.                                                        */
static void BTPSAPI GATT_ServerEventCallback(unsigned int BluetoothStackID, GATT_Server_Event_Data_t *GATT_ServerEventData, unsigned long CallbackParameter)
{
   Word_t               AttributeOffset;
   Word_t               InstanceTag;
   Word_t               ValueLength;
   Byte_t              *Value;
   Byte_t               Event_Buffer[ANS_EVENT_DATA_SIZE + ANS_EVENT_DATA_BUFFER_SIZE];
   unsigned int         TransactionID;
   unsigned int         InstanceID;
   ANS_Event_Data_t    *EventData;
   ANSServerInstance_t *ServiceInstance;

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
                     if(Alert_Notification_Service[AttributeOffset].Attribute_Entry_Type == aetCharacteristicDescriptor16)
                     {
                        EventData = FormatEventHeader(sizeof(Event_Buffer), Event_Buffer, etANS_Server_Read_Client_Configuration_Request, InstanceID, GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->ConnectionID, &TransactionID, GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->ConnectionType, &(GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->RemoteDevice));
                        if(EventData)
                        {
                           if((AttributeOffset == ANS_NEW_ALERT_CLIENT_CHARACTERISTIC_CONFIGURATION_OFFSET) || (AttributeOffset == ANS_UNREAD_ALERT_STATUS_CLIENT_CHARACTERISTIC_CONFIGURATION_OFFSET))
                           {
                              /* Format the rest of the event.          */
                              EventData->Event_Data_Size                                                        = ANS_READ_CLIENT_CONFIGURATION_DATA_SIZE;
                              EventData->Event_Data.ANS_Read_Client_Configuration_Data->ClientConfigurationType = (AttributeOffset == ANS_NEW_ALERT_CLIENT_CHARACTERISTIC_CONFIGURATION_OFFSET)?ctNewAlert:ctUnreadAlertStatus;

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
                              GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR);
                        }
                        else
                           GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR);
                     }
                     else
                     {
                        /* Get the instance tag for the characteristic. */
                        InstanceTag = (Word_t)(((GATT_Characteristic_Value_16_Entry_t *)Alert_Notification_Service[AttributeOffset].Attribute_Value)->Characteristic_Value_Length);
                        ValueLength = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(((Byte_t *)(&InstanceData[InstanceID-1]))[InstanceTag]));
                        Value       = (Byte_t *)(&(((Byte_t *)(&InstanceData[InstanceID-1]))[InstanceTag + WORD_SIZE]));

                        /* Respond with the data.                       */
                        GATT_Read_Response(BluetoothStackID, TransactionID, (unsigned int)ValueLength, Value);
                     }
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
                     if(Alert_Notification_Service[AttributeOffset].Attribute_Entry_Type == aetCharacteristicDescriptor16)
                     {
                        /* Begin formatting the Client Configuration    */
                        /* Update event.                                */
                        EventData = FormatEventHeader(sizeof(Event_Buffer), Event_Buffer, etANS_Server_Client_Configuration_Update, InstanceID, GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->ConnectionID, NULL, GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->ConnectionType, &(GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->RemoteDevice));
                        if(EventData)
                        {
                           /* Format the rest of the event.             */
                           EventData->Event_Data_Size                                                          = ANS_CLIENT_CONFIGURATION_UPDATE_DATA_SIZE;
                           EventData->Event_Data.ANS_Client_Configuration_Update_Data->ClientConfigurationType = (AttributeOffset == ANS_NEW_ALERT_CLIENT_CHARACTERISTIC_CONFIGURATION_OFFSET)?ctNewAlert:ctUnreadAlertStatus;

                           /* Attempt to decode the request Client      */
                           /* Configuration.                            */
                           if(!DecodeClientConfigurationValue(ValueLength, Value, &(EventData->Event_Data.ANS_Client_Configuration_Update_Data->NotificationsEnabled)))
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
                     else
                     {
                        /* Format and Dispatch the event.               */
                        EventData = FormatEventHeader(sizeof(Event_Buffer), Event_Buffer, etANS_Server_Control_Point_Command_Indication, InstanceID, GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->ConnectionID, NULL, GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->ConnectionType, &(GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->RemoteDevice));
                        if(EventData)
                        {
                           /* Format the rest of the event.             */
                           EventData->Event_Data_Size = ANS_CONTROL_POINT_COMMAND_DATA_SIZE;

                           /* Parse out the command.                    */
                           if(!DecodeControlPointCommand(ValueLength, Value, &(EventData->Event_Data.ANS_Control_Point_Command_Data->Command), &(EventData->Event_Data.ANS_Control_Point_Command_Data->Category)))
                           {
                              /* Verify that the command is valid (i.e. */
                              /* not trying to enable a category that is*/
                              /* not supported).                        */
                              if(VerifyControlPointCommand(InstanceID, EventData->Event_Data.ANS_Control_Point_Command_Data->Command, EventData->Event_Data.ANS_Control_Point_Command_Data->Category))
                              {
                                 /* Accept the Write Request since we   */
                                 /* have decoded the command            */
                                 /* successfully.                       */
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
                                 GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, ANS_ERROR_CODE_COMMAND_NOT_SUPPORTED);
                           }
                           else
                              GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, ANS_ERROR_CODE_COMMAND_NOT_SUPPORTED);
                        }
                        else
                           GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR);
                     }
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
   /* Bluetooth Stack ANS Module is Initialized correctly.  This        */
   /* function *MUST* be called before ANY other Bluetooth Stack ANS    */
   /* function can be called.  This function returns non-zero if the    */
   /* Module was initialized correctly, or a zero value if there was an */
   /* error.                                                            */
   /* * NOTE * Internally, this module will make sure that this function*/
   /*          has been called at least once so that the module will    */
   /*          function.  Calling this function from an external        */
   /*          location is not necessary.                               */
int InitializeANSModule(void)
{
   return((int)InitializeModule());
}

   /* The following function is responsible for instructing the         */
   /* Bluetooth Stack ANS Module to clean up any resources that it has  */
   /* allocated.  Once this function has completed, NO other Bluetooth  */
   /* Stack ANS Functions can be called until a successful call to the  */
   /* InitializeANSModule() function is made.  The parameter to this    */
   /* function specifies the context in which this function is being    */
   /* called.  If the specified parameter is TRUE, then the module will */
   /* make sure that NO functions that would require waiting/blocking on*/
   /* Mutexes/Events are called.  This parameter would be set to TRUE if*/
   /* this function was called in a context where threads would not be  */
   /* allowed to run.  If this function is called in the context where  */
   /* threads are allowed to run then this parameter should be set to   */
   /* FALSE.                                                            */
void CleanupANSModule(Boolean_t ForceCleanup)
{
   /* Check to make sure that this module has been initialized.         */
   if(InstanceListInitialized)
   {
      /* Wait for access to the ANS Context List.                       */
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

   /* The following function is responsible for opening a ANS Server.   */
   /* The first parameter is the Bluetooth Stack ID on which to open the*/
   /* server.  The second parameter is the Callback function to call    */
   /* when an event occurs on this Server Port.  The third parameter is */
   /* a user-defined callback parameter that will be passed to the      */
   /* callback function with each event.  The final parameter is a      */
   /* pointer to store the GATT Service ID of the registered ANS        */
   /* service.  This can be used to include the service registered by   */
   /* this call.  This function returns the positive, non-zero, Instance*/
   /* ID or a negative error code.                                      */
   /* * NOTE * Only 1 ANS Server may be open at a time, per Bluetooth   */
   /*          Stack ID.                                                */
   /* * NOTE * All Client Requests will be dispatch to the EventCallback*/
   /*          function that is specified by the second parameter to    */
   /*          this function.                                           */
int BTPSAPI ANS_Initialize_Service(unsigned int BluetoothStackID, ANS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID)
{
   GATT_Attribute_Handle_Group_t ServiceHandleRange;

    /* Initialize the Service Handle Group to 0.                        */
   ServiceHandleRange.Starting_Handle = 0;
   ServiceHandleRange.Ending_Handle   = 0;

   return(ANSRegisterService(BluetoothStackID, EventCallback, CallbackParameter, ServiceID, &ServiceHandleRange));
}

   /* The following function is responsible for opening a ANS Server.   */
   /* The first parameter is the Bluetooth Stack ID on which to open the*/
   /* server.  The second parameter is the Callback function to call    */
   /* when an event occurs on this Server Port.  The third parameter is */
   /* a user-defined callback parameter that will be passed to the      */
   /* callback function with each event.  The fourth parameter is a     */
   /* pointer to store the GATT Service ID of the registered ANS        */
   /* service.  This can be used to include the service registered by   */
   /* this call.  The final parameter is a pointer, that on input can be*/
   /* used to control the location of the service in the GATT database, */
   /* and on ouput to store the service handle range.  This function    */
   /* returns the positive, non-zero, Instance ID or a negative error   */
   /* code.                                                             */
   /* * NOTE * Only 1 ANS Server may be open at a time, per Bluetooth   */
   /*          Stack ID.                                                */
   /* * NOTE * All Client Requests will be dispatch to the EventCallback*/
   /*          function that is specified by the second parameter to    */
   /*          this function.                                           */
int BTPSAPI ANS_Initialize_Service_Handle_Range(unsigned int BluetoothStackID, ANS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange)
{
   return(ANSRegisterService(BluetoothStackID, EventCallback, CallbackParameter, ServiceID, ServiceHandleRange));
}

   /* The following function is responsible for closing a previously ANS*/
   /* Server.  The first parameter is the Bluetooth Stack ID on which to*/
   /* close the server.  The second parameter is the InstanceID that was*/
   /* returned from a successful call to ANS_Initialize_Service().  This*/
   /* function returns a zero if successful or a negative return error  */
   /* code if an error occurs.                                          */
int BTPSAPI ANS_Cleanup_Service(unsigned int BluetoothStackID, unsigned int InstanceID)
{
   int                  ret_val;
   ANSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID))
   {
      /* Acquire the specified ANS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Verify that the service is actually registered.             */
         if(ServiceInstance->ServiceID)
         {
            /* Call GATT to un-register the service.                    */
            GATT_Un_Register_Service(BluetoothStackID, ServiceInstance->ServiceID);

            /* mark the instance entry as being free.                   */
            BTPS_MemInitialize(ServiceInstance, 0, ANS_SERVER_INSTANCE_DATA_SIZE);

            /* return success to the caller.                            */
            ret_val = 0;
         }
         else
            ret_val = ANS_ERROR_INVALID_PARAMETER;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(BluetoothStackID);
      }
      else
         ret_val = ANS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = ANS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for querying the number of  */
   /* attributes that are contained in the ANS Service that is          */
   /* registered with a call to ANS_Initialize_Service().  This function*/
   /* returns the non-zero number of attributes that are contained in an*/
   /* ANS Server or zero on failure.                                    */
unsigned int BTPSAPI ANS_Query_Number_Attributes(void)
{
   /* Simply return the number of attributes that are contained in an   */
   /* ANS service.                                                      */
   return(ALERT_NOTIFICATION_SERVICE_ATTRIBUTE_COUNT);
}


   /* The following function is responsible for setting the Alert       */
   /* Notification Supported Categories for the specified Category Type */
   /* on the specified ANS Instance.  The first parameter is the        */
   /* Bluetooth Stack ID of the Bluetooth Device.  The second parameter */
   /* is the InstanceID returned from a successful call to              */
   /* ANS_Initialize_Server().  The next parameter specifies the        */
   /* Category to set the Supported Categories for.  The final parameter*/
   /* is the Supported Categories bit mask to set as the supported      */
   /* categories for the specified ANS Instance.  This function returns */
   /* a zero if successful or a negative return error code if an error  */
   /* occurs.                                                           */
   /* * NOTE * The SupportedCategoriesMask is a bit mask that is made up*/
   /*          of bit masks of the form ANS_SUPPORTED_CATEGORIES_XXX.   */
int BTPSAPI ANS_Set_Supported_Categories(unsigned int BluetoothStackID, unsigned int InstanceID, ANS_Supported_Categories_Type_t SupportedCategoryType, Word_t SupportedCategoriesMask)
{
   int                  ret_val;
   Byte_t              *SupportedCategoriesPtr;
   ANSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && ((SupportedCategoryType == scNewAlert) || (SupportedCategoryType == scUnreadAlertStatus)) && (SupportedCategoriesMask))
   {
      /* Acquire the specified ANS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Determine the correct category to configure.                */
         if(SupportedCategoryType == scNewAlert)
            SupportedCategoriesPtr = (Byte_t *)&(InstanceData[InstanceID-1].SupportedNewAlertCategoryInstance);
         else
            SupportedCategoriesPtr = (Byte_t *)&(InstanceData[InstanceID-1].SupportedUnreadAlertCategoryInstance);

         /* The first byte of the Supported Categories Mask is a direct */
         /* pass through of the LSB of the SupportedCategoriesMask      */
         /* parameter.                                                  */
         ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&SupportedCategoriesPtr[0], SupportedCategoriesMask);

         /* The second byte of the supported categories is the lower 2  */
         /* bits of the MSB of the SupportedCategoriesMask parameter.   */
         ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&SupportedCategoriesPtr[1], (0x03 & (SupportedCategoriesMask >> 8)));

         /* Return success to the caller.                               */
         ret_val = 0;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = ANS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = ANS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for setting the Alert       */
   /* Notification Supported Categories for the specified Category Type */
   /* on the specified ANS Instance.  The first parameter is the        */
   /* Bluetooth Stack ID of the Bluetooth Device.  The second parameter */
   /* is the InstanceID returned from a successful call to              */
   /* ANS_Initialize_Server().  The next parameter specifies the        */
   /* Category to query the Supported Categories for.  The final        */
   /* parameter is a pointer to store the Supported Categories bit mask */
   /* for the specified ANS Instance.  This function returns a zero if  */
   /* successful or a negative return error code if an error occurs.    */
   /* * NOTE * The SupportedCategoriesMask is a pointer to a bit mask   */
   /*          that will be made up of bit masks of the form            */
   /*          ANS_SUPPORTED_CATEGORIES_XXX, if this function returns   */
   /*          success.                                                 */
int BTPSAPI ANS_Query_Supported_Categories(unsigned int BluetoothStackID, unsigned int InstanceID, ANS_Supported_Categories_Type_t SupportedCategoryType, Word_t *SupportedCategoriesMask)
{
   int                  ret_val;
   Byte_t              *SupportedCategoriesPtr;
   unsigned int         Length;
   ANSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && ((SupportedCategoryType == scNewAlert) || (SupportedCategoryType == scUnreadAlertStatus)) && (SupportedCategoriesMask))
   {
      /* Acquire the specified ANS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Determine the correct category to configure.                */
         if(SupportedCategoryType == scNewAlert)
         {
            SupportedCategoriesPtr = (Byte_t *)&(InstanceData[InstanceID-1].SupportedNewAlertCategoryInstance);
            Length                 = (unsigned int)READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(InstanceData[InstanceID-1].SupportedNewAlertCategoryLength));
         }
         else
         {
            SupportedCategoriesPtr = (Byte_t *)&(InstanceData[InstanceID-1].SupportedUnreadAlertCategoryInstance);
            Length                 = (unsigned int)READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(InstanceData[InstanceID-1].SupportedUnreadAlertCategoryLength));
         }

         /* Decode the Supported Categories value.                      */
         ret_val = DecodeSupportedCategories(Length, SupportedCategoriesPtr, SupportedCategoriesMask);

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = ANS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = ANS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for responding to a ANS Read*/
   /* Client Configuration Request.  The first parameter is the         */
   /* Bluetooth Stack ID of the Bluetooth Device.  The second parameter */
   /* is the InstanceID returned from a successful call to              */
   /* ANS_Initialize_Server().  The third is the Transaction ID of the  */
   /* request.  The final parameter contains the Client Configuration to*/
   /* send to the remote device.  This function returns a zero if       */
   /* successful or a negative return error code if an error occurs.    */
int BTPSAPI ANS_Read_Client_Configuration_Response(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int TransactionID, Boolean_t NotificationsEnabled)
{
   int                  ret_val;
   Word_t               ValueLength;
   NonAlignedWord_t     ClientConfiguration;
   ANSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (TransactionID))
   {
      /* Acquire the specified ANS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Format the Read Response.                                   */
         ValueLength = GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_LENGTH;
         ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&ClientConfiguration, ((NotificationsEnabled)?GATT_CLIENT_CONFIGURATION_CHARACTERISTIC_NOTIFY_ENABLE:0));

         /* Send the response.                                          */
         ret_val = GATT_Read_Response(ServiceInstance->BluetoothStackID, TransactionID, (unsigned int)ValueLength, (Byte_t *)&ClientConfiguration);

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = ANS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = ANS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for sending a New Alert     */
   /* notification to a specified remote device.  The first parameter is*/
   /* the Bluetooth Stack ID of the Bluetooth Device.  The second       */
   /* parameter is the InstanceID returned from a successful call to    */
   /* ANS_Initialize_Server().  The third parameter is the ConnectionID */
   /* of the remote device to send the notification to.  The final      */
   /* parameter is the New Alert data to notify.  This function returns */
   /* a zero if successful or a negative return error code if an error  */
   /* occurs.                                                           */
int BTPSAPI ANS_New_Alert_Notification(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int ConnectionID, ANS_New_Alert_Data_t *NewAlert)
{
   int                           ret_val;
   unsigned int                  Length;
   ANSServerInstance_t          *ServiceInstance;
   ANS_New_Alert_Notification_t *NotificationData;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (ConnectionID) && (NewAlert) && (ANS_CATEGORY_ID_VALID_EXCL_ALL_CATEGORIES(NewAlert->CategoryID)))
   {
      /* Acquire the specified ANS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Calculate the length of the Last Alert String.              */
         if(NewAlert->LastAlertString)
            Length = BTPS_StringLength(NewAlert->LastAlertString);
         else
            Length = 0;

         /* Allocate a buffer for the notification.                     */
         if((NotificationData = BTPS_AllocateMemory(ANS_NEW_ALERT_NOTIFICATION_DATA_SIZE(Length))) != NULL)
         {
            /* Format the notification.                                 */
            ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(NotificationData->CategoryID), NewAlert->CategoryID);
            ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(NotificationData->NumberOfNewAlerts), NewAlert->NumberOfNewAlerts);

            /* Copy in the Last Alert String if one was specified.      */
            if(NewAlert->LastAlertString)
               BTPS_MemCopy(&(NotificationData->TextStringInformation[0]), NewAlert->LastAlertString, Length);

            /* Attempt to send the notification.                        */
            ret_val = GATT_Handle_Value_Notification(ServiceInstance->BluetoothStackID, ServiceInstance->ServiceID, ConnectionID, ANS_NEW_ALERT_CHARACTERISTIC_OFFSET, (Word_t)ANS_NEW_ALERT_NOTIFICATION_DATA_SIZE(Length), (Byte_t *)NotificationData);
            if(ret_val > 0)
               ret_val = 0;

            /* Free the memory allocated for the Notification Data.     */
            BTPS_FreeMemory(NotificationData);
         }
         else
            ret_val = ANS_ERROR_INSUFFICIENT_RESOURCES;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = ANS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = ANS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for sending a UnRead Alert  */
   /* Status notification to a specified remote device.  The first      */
   /* parameter is the Bluetooth Stack ID of the Bluetooth Device.  The */
   /* second parameter is the InstanceID returned from a successful call*/
   /* to ANS_Initialize_Server().  The third parameter is the           */
   /* ConnectionID of the remote device to send the notification to.    */
   /* The final parameter is the UnRead Alert data to notify.  This     */
   /* function returns a zero if successful or a negative return error  */
   /* code if an error occurs.                                          */
int BTPSAPI ANS_Un_Read_Alert_Status_Notification(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int ConnectionID, ANS_Un_Read_Alert_Data_t *UnReadAlert)
{
   int                                     ret_val;
   ANSServerInstance_t                    *ServiceInstance;
   ANS_Unread_Alert_Status_Notification_t *NotificationData;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (ConnectionID) && (UnReadAlert) && (ANS_CATEGORY_ID_VALID_EXCL_ALL_CATEGORIES(UnReadAlert->CategoryID)))
   {
      /* Acquire the specified ANS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Allocate a buffer for the notification.                     */
         if((NotificationData = BTPS_AllocateMemory(ANS_UNREAD_ALERT_STATUS_NOTIFICATION_DATA_SIZE)) != NULL)
         {
            /* Format the notification.                                 */
            ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(NotificationData->CategoryID), UnReadAlert->CategoryID);
            ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(NotificationData->UnreadCount), UnReadAlert->NumberOfUnreadAlerts);

            /* Attempt to send the notification.                        */
            ret_val = GATT_Handle_Value_Notification(ServiceInstance->BluetoothStackID, ServiceInstance->ServiceID, ConnectionID, ANS_UNREAD_ALERT_STATUS_CHARACTERISTIC_OFFSET, ANS_UNREAD_ALERT_STATUS_NOTIFICATION_DATA_SIZE, (Byte_t *)NotificationData);
            if(ret_val > 0)
               ret_val = 0;

            /* Free the memory allocated for the Notification Data.     */
            BTPS_FreeMemory(NotificationData);
         }
         else
            ret_val = ANS_ERROR_INSUFFICIENT_RESOURCES;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = ANS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = ANS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* ANS Client API.                                                   */

   /* The following function is responsible for parsing a New Alert     */
   /* notification received from a remote ANS Server.  The first        */
   /* parameter is the length of the value returned by the remote ANS   */
   /* Server.  The second parameter is a pointer to the data returned by*/
   /* the remote ANS Server.  This function returns a pointer to the    */
   /* decode New Alert data or NULL if an error occurred.               */
   /* * NOTE * The return value from this function MUST be freed by     */
   /*          calling ANS_Free_New_Alert_Data() when the decoded New   */
   /*          Alert Notification is no loner needed.                   */
ANS_New_Alert_Data_t *BTPSAPI ANS_Decode_New_Alert_Notification(unsigned int ValueLength, Byte_t *Value)
{
   Byte_t                CategoryID;
   unsigned int          StringLength;
   ANS_New_Alert_Data_t *NewAlert = NULL;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((ValueLength >= ANS_NEW_ALERT_NOTIFICATION_DATA_SIZE(0)) && (Value))
   {
      /* Determine in a Last Alert text string was included in the      */
      /* notification.                                                  */
      StringLength = ValueLength - ANS_NEW_ALERT_NOTIFICATION_DATA_SIZE(0);

      /* Read the Category and make sure it is valid.                   */
      CategoryID   = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(((ANS_New_Alert_Notification_t *)Value)->CategoryID));

      if(ANS_CATEGORY_ID_VALID_EXCL_ALL_CATEGORIES(CategoryID))
      {
         /* Allocate memory for the decoded structure.                  */
         if((NewAlert = BTPS_AllocateMemory(ANS_NEW_ALERT_DATA_SIZE)) != NULL)
         {
            /* Decode the mandatory part of the structure.              */
            NewAlert->CategoryID        = (ANS_Category_Identification_t)CategoryID;
            NewAlert->NumberOfNewAlerts = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(((ANS_New_Alert_Notification_t *)Value)->NumberOfNewAlerts));
            NewAlert->LastAlertString   = NULL;

            /* Allocate memory for the Last Alert Text string if it was */
            /* specified.                                               */
            if(StringLength)
            {
               if((NewAlert->LastAlertString = BTPS_AllocateMemory(StringLength+1)) != NULL)
               {
                  /* Copy the Last Alert string into the allocated      */
                  /* buffer.                                            */
                  BTPS_MemCopy(NewAlert->LastAlertString, ((ANS_New_Alert_Notification_t *)Value)->TextStringInformation, StringLength);

                  /* Set the NULL terminator character.                 */
                  NewAlert->LastAlertString[StringLength] = (char)0;
               }
               else
               {
                  /* Failed to allocate memory for the Last Alert string*/
                  /* so free all of the allocated memory.               */
                  BTPS_FreeMemory(NewAlert);

                  NewAlert = NULL;
               }
            }
         }
      }
   }

   /* Finally, return the result to the caller.                         */
   return(NewAlert);
}

   /* The following function is responsible for free the decoded New    */
   /* Alert Data that was returned by a successful call to              */
   /* ANS_Decode_New_Alert_Notification.  The only parameter to this    */
   /* function is the New Alert Data returned by the call to            */
   /* ANS_Decode_New_Alert_Notification.                                */
void BTPSAPI ANS_Free_New_Alert_Data(ANS_New_Alert_Data_t *NewAlertData)
{
   /* Make sure the parameters passed to us are semi-valid.             */
   if(NewAlertData)
   {
      /* Free the Last Alert string if one was specified.               */
      if(NewAlertData->LastAlertString)
         BTPS_FreeMemory(NewAlertData->LastAlertString);

      /* Free the container structure that was allocated.               */
      BTPS_FreeMemory(NewAlertData);
   }
}

   /* The following function is responsible for parsing an UnRead Alert */
   /* Status notification received from a remote ANS Server.  The first */
   /* parameter is the length of the value returned by the remote ANS   */
   /* Server.  The second parameter is a pointer to the data returned by*/
   /* the remote ANS Server.  The final parameter is a pointer to store */
   /* the parsed UnReadAlert value.  This function returns a zero if    */
   /* successful or a negative return error code if an error occurs.    */
int BTPSAPI ANS_Decode_Un_Read_Alert_Status_Notification(unsigned int ValueLength, Byte_t *Value, ANS_Un_Read_Alert_Data_t *UnReadAlert)
{
   int    ret_val;
   Byte_t CategoryID;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((ValueLength >= ANS_UNREAD_ALERT_STATUS_NOTIFICATION_DATA_SIZE) && (Value) && (UnReadAlert))
   {
      /* Read the Category ID from the notification and make sure that  */
      /* it is valid.                                                   */
      CategoryID = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(((ANS_Unread_Alert_Status_Notification_t *)Value)->CategoryID));

      if(ANS_CATEGORY_ID_VALID_EXCL_ALL_CATEGORIES(CategoryID))
      {
         /* Format the decoded UnRead Alert data.                       */
         UnReadAlert->CategoryID           = (ANS_Category_Identification_t)CategoryID;
         UnReadAlert->NumberOfUnreadAlerts = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(((ANS_Unread_Alert_Status_Notification_t *)Value)->UnreadCount));

         /* Return success to the caller.                               */
         ret_val                           = 0;
      }
      else
         ret_val = ANS_ERROR_MALFORMATTED_DATA;
   }
   else
      ret_val = ANS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for parsing a Supported     */
   /* Categories value received from a remote ANS Server.  The first    */
   /* parameter is the length of the value returned by the remote ANS   */
   /* Server.  The second parameter is a pointer to the data returned by*/
   /* the remote ANS Server.  The final parameter is a pointer to store */
   /* the Supported Categories bit mask.  This function returns a zero  */
   /* if successful or a negative return error code if an error occurs. */
int BTPSAPI ANS_Decode_Supported_Categories(unsigned int ValueLength, Byte_t *Value, Word_t *SupportedCategoriesMask)
{
   /* Call the internal function to decode the value.                   */
   return(DecodeSupportedCategories(ValueLength, Value, SupportedCategoriesMask));
}

   /* The following function is responsible for formatting an Alert     */
   /* Notification Control Point Command into a user specified buffer.  */
   /* The first parameter to this function is the Command Buffer to     */
   /* format the command into.  The next parameter specifies the Command*/
   /* to format into the buffer.  The final parameter specifies the     */
   /* category that the command applies to.  This function returns a    */
   /* zero if successful or a negative return error code if an error    */
   /* occurs.                                                           */
int BTPSAPI ANS_Format_Control_Point_Command(ANS_Control_Point_Command_Value_t *CommandBuffer, ANS_Control_Point_Command_t Command, ANS_Category_Identification_t CommandCategory)
{
   int ret_val;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((CommandBuffer) && (ANS_COMMAND_ID_VALID(Command)) && (ANS_CATEGORY_ID_VALID_INCL_ALL_CATEGORIES(CommandCategory)))
   {
      /* Format the command.                                            */
      ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(CommandBuffer->CommandID), Command);
      ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(CommandBuffer->CategoryID), CommandCategory);

      /* Return success to the caller.                                  */
      ret_val = 0;
   }
   else
      ret_val = ANS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}
