/*****< pass.c >***************************************************************/
/*      Copyright 2011 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  PASS - Bluetooth Stack Phone Alert State Service (GATT Based) for         */
/*         Stonestreet One Bluetooth Protocol Stack.                          */
/*                                                                            */
/*  Author:  Tim Cook                                                         */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   09/22/11  T. Cook        Initial creation.                               */
/******************************************************************************/
#include "SS1BTPS.h"        /* Bluetooth Stack API Prototypes/Constants.      */
#include "SS1BTGAT.h"       /* Bluetooth Stack GATT API Prototypes/Constants. */
#include "SS1BTPAS.h"       /* Bluetooth PASS API Prototypes/Constants.       */

#include "BTPSKRNL.h"       /* BTPS Kernel Prototypes/Constants.              */
#include "PASS.h"           /* Bluetooth PASS Prototypes/Constants.           */

   /* The following controls the number of supported PASS instances.    */
#define PASS_MAXIMUM_SUPPORTED_INSTANCES                 (BTPS_CONFIGURATION_PASS_MAXIMUM_SUPPORTED_INSTANCES)

   /* The following structure defines the Instance Data that must be    */
   /* unique for each PASS service registered (Only 1 per Bluetooth     */
   /* Stack).                                                           */
typedef __PACKED_STRUCT_BEGIN__ struct _tagPASS_Instance_Data_t
{
   NonAlignedWord_t AlertStatusLength;
   NonAlignedByte_t AlertStatusInstance;
   NonAlignedWord_t RingerSettingLength;
   NonAlignedByte_t RingerSettingInstance;
} __PACKED_STRUCT_END__ PASS_Instance_Data_t;

#define PASS_INSTANCE_DATA_SIZE                          (sizeof(PASS_Instance_Data_t))

   /* The following define the instance tags for each PASS service data */
   /* that is unique per registered service.                            */
#define PASS_ALERT_STATUS_INSTANCE_TAG_VALUE             (BTPS_STRUCTURE_OFFSET(PASS_Instance_Data_t, AlertStatusLength))
#define PASS_RINGER_SETTING_INSTANCE_TAG_VALUE           (BTPS_STRUCTURE_OFFSET(PASS_Instance_Data_t, RingerSettingLength))

   /*********************************************************************/
   /**               Phone Alert Status Service Table                  **/
   /*********************************************************************/

   /* The Phone Alert Status Service Declaration UUID.                  */
static BTPSCONST GATT_Primary_Service_16_Entry_t PASS_Service_UUID =
{
   PASS_SERVICE_BLUETOOTH_UUID_CONSTANT
};

   /* The Alert Status Characteristic Declaration.                      */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t PASS_Alert_Status_Declaration =
{
   (GATT_CHARACTERISTIC_PROPERTIES_READ|GATT_CHARACTERISTIC_PROPERTIES_NOTIFY),
   PASS_ALERT_STATUS_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The Alert Status Characteristic Value.                            */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t  PASS_Alert_Status_Value =
{
   PASS_ALERT_STATUS_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   PASS_ALERT_STATUS_INSTANCE_TAG_VALUE,
   NULL
};

   /* Client Characteristic Configuration.                              */
static GATT_Characteristic_Descriptor_16_Entry_t Client_Characteristic_Configuration =
{
   GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_BLUETOOTH_UUID_CONSTANT,
   GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_LENGTH,
   NULL
};

   /* The Ringer Setting Characteristic Declaration.                    */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t PASS_Ringer_Setting_Declaration =
{
   (GATT_CHARACTERISTIC_PROPERTIES_READ|GATT_CHARACTERISTIC_PROPERTIES_NOTIFY),
   PASS_RINGER_SETTING_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The Ringer Setting Characteristic Value.                          */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t  PASS_Ringer_Setting_Value =
{
   PASS_RINGER_SETTING_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   PASS_RINGER_SETTING_INSTANCE_TAG_VALUE,
   NULL
};

   /* The Ringer Control Point Characteristic Declaration.              */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t PASS_Ringer_Control_Point_Declaration =
{
   GATT_CHARACTERISTIC_PROPERTIES_WRITE_WITHOUT_RESPONSE,
   PASS_RINGER_CONTROL_POINT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The Ringer Control Point Characteristic Value.                    */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t PASS_Ringer_Control_Point_Value =
{
   PASS_RINGER_CONTROL_POINT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   PASS_RINGER_CONTROL_POINT_VALUE_LENGTH,
   NULL
};

   /* The following defines the Phone Alert Status service that is      */
   /* registered with the GATT_Register_Service function call.          */
   /* * NOTE * This array will be registered with GATT in the call to   */
   /*          GATT_Register_Service.                                   */
BTPSCONST GATT_Service_Attribute_Entry_t Phone_Alert_Status_Service[] =
{
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetPrimaryService16,            (Byte_t *)&PASS_Service_UUID},                     /* PASS Primary Service Declaration.                     */
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetCharacteristicDeclaration16, (Byte_t *)&PASS_Alert_Status_Declaration},         /* PASS Alert Status Characteristic Declaration.         */
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetCharacteristicValue16,       (Byte_t *)&PASS_Alert_Status_Value},               /* PASS Alert Status Value.                              */
   {GATT_ATTRIBUTE_FLAGS_READABLE_WRITABLE, aetCharacteristicDescriptor16,  (Byte_t *)&Client_Characteristic_Configuration},   /* PASS Alert Status Client Configuration.               */
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetCharacteristicDeclaration16, (Byte_t *)&PASS_Ringer_Setting_Declaration},       /* PASS Ringer Setting Characteristic Declaration.       */
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetCharacteristicValue16,       (Byte_t *)&PASS_Ringer_Setting_Value},             /* PASS Ringer Setting Value.                            */
   {GATT_ATTRIBUTE_FLAGS_READABLE_WRITABLE, aetCharacteristicDescriptor16,  (Byte_t *)&Client_Characteristic_Configuration},   /* PASS Ringer Setting Client Configuration.             */
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetCharacteristicDeclaration16, (Byte_t *)&PASS_Ringer_Control_Point_Declaration}, /* PASS Ringer Control Point Characteristic Declaration. */
   {GATT_ATTRIBUTE_FLAGS_WRITABLE,          aetCharacteristicValue16,       (Byte_t *)&PASS_Ringer_Control_Point_Value},       /* PASS Ringer Control Point Value.                      */
};

#define PHONE_ALERT_STATUS_SERVICE_ATTRIBUTE_COUNT                      (sizeof(Phone_Alert_Status_Service)/sizeof(GATT_Service_Attribute_Entry_t))

#define PASS_ALERT_STATUS_CHARACTERISTIC_VALUE_OFFSET                   2
#define PASS_ALERT_STATUS_CLIENT_CHARACTERISTIC_CONFIGURATION_OFFSET    3
#define PASS_RINGER_SETTING_CHARACTERISTIC_VALUE_OFFSET                 5
#define PASS_RINGER_SETTING_CLIENT_CHARACTERISTIC_CONFIGURATION_OFFSET  6

   /* The following defines the PASS GATT Service Flags MASK that should*/
   /* be passed into GATT_Register_Service when the PASS Service is     */
   /* registered.                                                       */
#define PASS_SERVICE_FLAGS                               (GATT_SERVICE_FLAGS_LE_SERVICE)

   /*********************************************************************/
   /**                    END OF SERVICE TABLE                         **/
   /*********************************************************************/

   /* The following type defines a union large enough to hold all events*/
   /* dispatched by this module.                                        */
typedef union
{
   PASS_Read_Client_Configuration_Data_t   PASS_Read_Client_Configuration_Data;
   PASS_Client_Configuration_Update_Data_t PASS_Client_Configuration_Update_Data;
   PASS_Ringer_Control_Command_Data_t      PASS_Ringer_Control_Command_Data;
} PASS_Event_Data_Buffer_t;

#define PASS_EVENT_DATA_BUFFER_SIZE                      (sizeof(PASS_Event_Data_Buffer_t))

   /* PASS Service Instance Block.  This structure contains All         */
   /* information associated with a specific Bluetooth Stack ID (member */
   /* is present in this structure).                                    */
typedef struct _tagPASSServerInstance_t
{
   unsigned int          BluetoothStackID;
   unsigned int          ServiceID;
   PASS_Event_Callback_t EventCallback;
   unsigned long         CallbackParameter;
} PASSServerInstance_t;

#define PASS_SERVER_INSTANCE_DATA_SIZE                    (sizeof(PASSServerInstance_t))

   /* Internal Variables to this Module (Remember that all variables    */
   /* declared static are initialized to 0 automatically by the         */
   /* compiler as part of standard C/C++).                              */

static PASS_Instance_Data_t InstanceData[PASS_MAXIMUM_SUPPORTED_INSTANCES];
                                            /* Variable which holds all */
                                            /* data that is unique for  */
                                            /* each service instance.   */

static PASSServerInstance_t InstanceList[PASS_MAXIMUM_SUPPORTED_INSTANCES];
                                            /* Variable which holds the */
                                            /* service instance data.   */

static Boolean_t InstanceListInitialized;   /* Variable that flags that */
                                            /* is used to denote that   */
                                            /* this module has been     */
                                            /* successfully initialized.*/

   /* The following are the prototypes of local functions.              */
static Boolean_t InitializeModule(void);
static void CleanupModule(void);

static int FormatAlertStatus(PASS_Alert_Status_t AlertStatus, unsigned int BufferLength, Byte_t *Buffer);
static int DecodeAlertStatus(unsigned int BufferLength, Byte_t *Buffer, PASS_Alert_Status_t *AlertStatus);

static int FormatRingerSetting(PASS_Ringer_Setting_t RingerSetting, unsigned int BufferLength, Byte_t *Buffer);
static int DecodeRingerSetting(unsigned int BufferLength, Byte_t *Buffer, PASS_Ringer_Setting_t *RingerSetting);

static int FormatRingerControlCommand(PASS_Ringer_Control_Command_t RingerControlCommand, unsigned int BufferLength, Byte_t *Buffer);
static int DecodeRingerControlCommand(unsigned int BufferLength, Byte_t *Buffer, PASS_Ringer_Control_Command_t *RingerControlCommand);

static int DecodeClientConfigurationValue(unsigned int BufferLength, Byte_t *Buffer, Boolean_t *NotificationsEnabled);

static PASS_Event_Data_t *FormatEventHeader(unsigned int BufferLength, Byte_t *Buffer, PASS_Event_Type_t EventType, unsigned int InstanceID, unsigned int ConnectionID, unsigned int *TransactionID, GATT_Connection_Type_t ConnectionType, BD_ADDR_t *BD_ADDR);

static Boolean_t InstanceRegisteredByStackID(unsigned int BluetoothStackID);
static PASSServerInstance_t *AcquireServiceInstance(unsigned int BluetoothStackID, unsigned int *InstanceID);

static int PASSRegisterService(unsigned int BluetoothStackID, PASS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange);

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
   /* an Alert Status value into a user specified buffer.  This function*/
   /* returns the number of bytes formatted into Buffer (positive,      */
   /* non-zero) or a negative error code.                               */
static int FormatAlertStatus(PASS_Alert_Status_t AlertStatus, unsigned int BufferLength, Byte_t *Buffer)
{
   int ret_val;

   /* Verify that the input parameters are valid.                       */
   if((BufferLength >= PASS_ALERT_STATUS_VALUE_LENGTH) && (Buffer))
   {
      /* Initialize the buffer.                                         */
      *Buffer = 0;

      /* Format the Alert Status.                                       */
      if(AlertStatus.RingerStateActive)
         *Buffer |= PASS_ALERT_STATUS_RINGER_STATE_ACTIVE;

      if(AlertStatus.VibrateStateActive)
         *Buffer |= PASS_ALERT_STATUS_VIBRATE_STATE_ACTIVE;

      if(AlertStatus.DisplayStateActive)
         *Buffer |= PASS_ALERT_STATUS_DISPLAY_ALERT_STATE_ACTIVE;

      /* return the number of bytes formatted into buffer.              */
      ret_val = PASS_ALERT_STATUS_VALUE_LENGTH;
   }
   else
      ret_val = PASS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is a utility function that exists to decode*/
   /* an Alert Status value into a user specified Alert Status          */
   /* structure.  This function returns the zero if successful or a     */
   /* negative error code.                                              */
static int DecodeAlertStatus(unsigned int BufferLength, Byte_t *Buffer, PASS_Alert_Status_t *AlertStatus)
{
   int ret_val = PASS_ERROR_MALFORMATTED_DATA;

   /* Verify that the input parameters are valid.                       */
   if((BufferLength == PASS_ALERT_STATUS_VALUE_LENGTH) && (Buffer) && (AlertStatus))
   {
      /* Initialize the Alert Status structure.                         */
      BTPS_MemInitialize(AlertStatus, 0, sizeof(PASS_Alert_Status_t));

      /* Format the Alert Status structure.                             */
      if(*Buffer & PASS_ALERT_STATUS_RINGER_STATE_ACTIVE)
         AlertStatus->RingerStateActive  = TRUE;

      if(*Buffer & PASS_ALERT_STATUS_VIBRATE_STATE_ACTIVE)
         AlertStatus->VibrateStateActive = TRUE;

      if(*Buffer & PASS_ALERT_STATUS_DISPLAY_ALERT_STATE_ACTIVE)
         AlertStatus->DisplayStateActive  = TRUE;

      /* Success to the caller.                                         */
      ret_val = 0;
   }
   else
   {
      if(BufferLength == PASS_ALERT_STATUS_VALUE_LENGTH)
         ret_val = PASS_ERROR_INVALID_PARAMETER;
   }

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is a utility function that exists to format*/
   /* an Ringer Setting value into a user specified buffer.  This       */
   /* function returns the number of bytes formatted into Buffer        */
   /* (positive, non-zero) or a negative error code.                    */
static int FormatRingerSetting(PASS_Ringer_Setting_t RingerSetting, unsigned int BufferLength, Byte_t *Buffer)
{
   int ret_val = PASS_RINGER_SETTING_VALUE_LENGTH;

   /* Verify that the input parameters are valid.                       */
   if(((RingerSetting == rsSilent) || (RingerSetting == rsNormal)) && (BufferLength >= PASS_RINGER_SETTING_VALUE_LENGTH) && (Buffer))
   {
      /* Format the Ringer Setting.                                     */
      ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(Buffer, RingerSetting);

      ret_val = PASS_RINGER_SETTING_VALUE_LENGTH;
   }
   else
      ret_val = PASS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is a utility function that exists to decode*/
   /* an Ringer Setting value into a user specified Ringer Setting      */
   /* enumerated value.  This function returns the zero if successful or*/
   /* a negative error code.                                            */
static int DecodeRingerSetting(unsigned int BufferLength, Byte_t *Buffer, PASS_Ringer_Setting_t *RingerSetting)
{
   int ret_val = PASS_ERROR_MALFORMATTED_DATA;

   /* Verify that the input parameters are valid.                       */
   if((BufferLength == PASS_RINGER_SETTING_VALUE_LENGTH) && (Buffer) && (RingerSetting))
   {
      if(*Buffer == PASS_RINGER_SETTING_RINGER_SILENT)
      {
         *RingerSetting = rsSilent;
         ret_val        = 0;
      }
      else
      {
         if(*Buffer == PASS_RINGER_SETTING_RINGER_NORMAL)
         {
            *RingerSetting = rsNormal;
            ret_val        = 0;
         }
      }
   }
   else
   {
      if(BufferLength == PASS_RINGER_SETTING_VALUE_LENGTH)
         ret_val = PASS_ERROR_INVALID_PARAMETER;
   }

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is a utility function that exists to format*/
   /* a Ringer Control Point Command into a user specified buffer.      */
static int FormatRingerControlCommand(PASS_Ringer_Control_Command_t RingerControlCommand, unsigned int BufferLength, Byte_t *Buffer)
{
   int ret_val;

   /* Verify that the input parameters are valid.                       */
   if(((RingerControlCommand == rcSilent) || (RingerControlCommand == rcMuteOnce) || (RingerControlCommand == rcCancelSilent)) && (BufferLength >= PASS_RINGER_CONTROL_POINT_VALUE_LENGTH) && (Buffer))
   {
      /* Format the Ringer Control Point command.                       */
      ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(Buffer, RingerControlCommand);

      ret_val = PASS_RINGER_CONTROL_POINT_VALUE_LENGTH;
   }
   else
      ret_val = PASS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is a utility function that exists to decode*/
   /* an Ringer Control Command value into a user specified Ringer      */
   /* Control Command enumerated value.  This function returns the zero */
   /* if successful or a negative error code.                           */
static int DecodeRingerControlCommand(unsigned int BufferLength, Byte_t *Buffer, PASS_Ringer_Control_Command_t *RingerControlCommand)
{
   int ret_val = PASS_ERROR_MALFORMATTED_DATA;

   /* Verify that the input parameters are valid.                       */
   if((BufferLength == PASS_RINGER_CONTROL_POINT_VALUE_LENGTH) && (Buffer) && (RingerControlCommand))
   {
      if(*Buffer == PASS_RINGER_CONTROL_COMMAND_SILENT_MODE)
      {
         *RingerControlCommand = rcSilent;
         ret_val               = 0;
      }
      else
      {
         if(*Buffer == PASS_RINGER_CONTROL_COMMAND_MUTE_ONCE)
         {
            *RingerControlCommand = rcMuteOnce;
            ret_val               = 0;
         }
         else
         {
            if(*Buffer == PASS_RINGER_CONTROL_COMMAND_CANCEL_SILENT_MODE)
            {
               *RingerControlCommand = rcCancelSilent;
               ret_val               = 0;
            }
         }
      }
   }
   else
   {
      if(BufferLength == PASS_RINGER_CONTROL_POINT_VALUE_LENGTH)
         ret_val = PASS_ERROR_INVALID_PARAMETER;
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
   int    ret_val = PASS_ERROR_MALFORMATTED_DATA;
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
         ret_val = PASS_ERROR_INVALID_PARAMETER;
   }

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is a utility function that exists to format*/
   /* a PASS Event into the specified buffer.                           */
   /* * NOTE * TransactionID is optional and may be set to NULL.        */
   /* * NOTE * BD_ADDR is NOT optional and may NOT be set to NULL.      */
static PASS_Event_Data_t *FormatEventHeader(unsigned int BufferLength, Byte_t *Buffer, PASS_Event_Type_t EventType, unsigned int InstanceID, unsigned int ConnectionID, unsigned int *TransactionID, GATT_Connection_Type_t ConnectionType, BD_ADDR_t *BD_ADDR)
{
   PASS_Event_Data_t *EventData = NULL;

   if((BufferLength >= (PASS_EVENT_DATA_SIZE + PASS_EVENT_DATA_BUFFER_SIZE)) && (Buffer) && (BD_ADDR))
   {
      /* Format the header of the event, that is data that is common to */
      /* all events.                                                    */
      BTPS_MemInitialize(Buffer, 0, BufferLength);

      EventData                                                               = (PASS_Event_Data_t *)Buffer;
      EventData->Event_Data_Type                                              = EventType;
      EventData->Event_Data.PASS_Read_Client_Configuration_Data               = (PASS_Read_Client_Configuration_Data_t *)(((Byte_t *)EventData) + PASS_EVENT_DATA_SIZE);

      EventData->Event_Data.PASS_Read_Client_Configuration_Data->InstanceID   = InstanceID;
      EventData->Event_Data.PASS_Read_Client_Configuration_Data->ConnectionID = ConnectionID;

      if(TransactionID)
      {
         EventData->Event_Data.PASS_Read_Client_Configuration_Data->TransactionID  = *TransactionID;
         EventData->Event_Data.PASS_Read_Client_Configuration_Data->ConnectionType = ConnectionType;
         EventData->Event_Data.PASS_Read_Client_Configuration_Data->RemoteDevice   = *BD_ADDR;
      }
      else
      {
         EventData->Event_Data.PASS_Ringer_Control_Command_Data->ConnectionType = ConnectionType;
         EventData->Event_Data.PASS_Ringer_Control_Command_Data->RemoteDevice   = *BD_ADDR;
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

   for(Index=0;Index<PASS_MAXIMUM_SUPPORTED_INSTANCES;Index++)
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
static PASSServerInstance_t *AcquireServiceInstance(unsigned int BluetoothStackID, unsigned int *InstanceID)
{
   unsigned int          LocalInstanceID;
   unsigned int          Index;
   PASSServerInstance_t *ret_val = NULL;

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
         if((LocalInstanceID) && (LocalInstanceID <= PASS_MAXIMUM_SUPPORTED_INSTANCES))
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
               for(Index=0;Index<PASS_MAXIMUM_SUPPORTED_INSTANCES;Index++)
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
   /* register an PASS Service.  This function returns the positive,    */
   /* non-zero, Instance ID on success or a negative error code.        */
static int PASSRegisterService(unsigned int BluetoothStackID, PASS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange)
{
   int                   ret_val;
   unsigned int          InstanceID;
   PASSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (EventCallback) && (ServiceID))
   {
      /* Verify that no instance is registered to this Bluetooth Stack. */
      if(!InstanceRegisteredByStackID(BluetoothStackID))
      {
         /* Acquire a free PASS Instance.                               */
         InstanceID = 0;
         if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
         {
            /* Call GATT to register the PASS service.                  */
            ret_val = GATT_Register_Service(BluetoothStackID, PASS_SERVICE_FLAGS, PHONE_ALERT_STATUS_SERVICE_ATTRIBUTE_COUNT, (GATT_Service_Attribute_Entry_t *)Phone_Alert_Status_Service, ServiceHandleRange, GATT_ServerEventCallback, InstanceID);
            if(ret_val > 0)
            {
               /* Save the Instance information.                        */
               ServiceInstance->BluetoothStackID  = BluetoothStackID;
               ServiceInstance->ServiceID         = (unsigned int)ret_val;
               ServiceInstance->EventCallback     = EventCallback;
               ServiceInstance->CallbackParameter = CallbackParameter;
               *ServiceID                         = (unsigned int)ret_val;

               /* Intilize the Instance Data for this instance.         */
               BTPS_MemInitialize(&InstanceData[InstanceID-1], 0, PASS_INSTANCE_DATA_SIZE);
               ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(InstanceData[InstanceID-1].AlertStatusLength), PASS_ALERT_STATUS_VALUE_LENGTH);
               ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(InstanceData[InstanceID-1].RingerSettingLength), PASS_RINGER_SETTING_VALUE_LENGTH);

               /* Return the PASS Instance ID.                          */
               ret_val                            = (int)InstanceID;
            }

            /* UnLock the previously locked Bluetooth Stack.            */
            BSC_UnLockBluetoothStack(BluetoothStackID);
         }
         else
            ret_val = PASS_ERROR_INSUFFICIENT_RESOURCES;
      }
      else
         ret_val = PASS_ERROR_SERVICE_ALREADY_REGISTERED;
   }
   else
      ret_val = PASS_ERROR_INVALID_PARAMETER;

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
   Byte_t                Event_Buffer[PASS_EVENT_DATA_SIZE + PASS_EVENT_DATA_BUFFER_SIZE];
   unsigned int          TransactionID;
   unsigned int          InstanceID;
   PASS_Event_Data_t    *EventData;
   PASSServerInstance_t *ServiceInstance;

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
                     if(Phone_Alert_Status_Service[AttributeOffset].Attribute_Entry_Type == aetCharacteristicDescriptor16)
                     {
                        EventData = FormatEventHeader(sizeof(Event_Buffer), Event_Buffer, etPASS_Server_Read_Client_Configuration_Request, InstanceID, GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->ConnectionID, &TransactionID, GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->ConnectionType, &(GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->RemoteDevice));
                        if(EventData)
                        {
                           if((AttributeOffset == PASS_ALERT_STATUS_CLIENT_CHARACTERISTIC_CONFIGURATION_OFFSET) || (AttributeOffset == PASS_RINGER_SETTING_CLIENT_CHARACTERISTIC_CONFIGURATION_OFFSET))
                           {
                              /* Format the rest of the event.          */
                              EventData->Event_Data_Size                                                         = PASS_READ_CLIENT_CONFIGURATION_DATA_SIZE;
                              EventData->Event_Data.PASS_Read_Client_Configuration_Data->ClientConfigurationType = (AttributeOffset == PASS_ALERT_STATUS_CLIENT_CHARACTERISTIC_CONFIGURATION_OFFSET)?rrAlertStatus:rrRingerSetting;

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
                        InstanceTag = (Word_t)(((GATT_Characteristic_Value_16_Entry_t *)Phone_Alert_Status_Service[AttributeOffset].Attribute_Value)->Characteristic_Value_Length);
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
                     if(Phone_Alert_Status_Service[AttributeOffset].Attribute_Entry_Type == aetCharacteristicDescriptor16)
                     {
                        /* Begin formatting the Client Configuration    */
                        /* Update event.                                */
                        EventData = FormatEventHeader(sizeof(Event_Buffer), Event_Buffer, etPASS_Server_Client_Configuration_Update, InstanceID, GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->ConnectionID, NULL, GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->ConnectionType, &(GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->RemoteDevice));
                        if(EventData)
                        {
                           /* Format the rest of the event.             */
                           EventData->Event_Data_Size                                                           = PASS_CLIENT_CONFIGURATION_UPDATE_DATA_SIZE;
                           EventData->Event_Data.PASS_Client_Configuration_Update_Data->ClientConfigurationType = (AttributeOffset == PASS_ALERT_STATUS_CLIENT_CHARACTERISTIC_CONFIGURATION_OFFSET)?rrAlertStatus:rrRingerSetting;

                           /* Attempt to decode the request Client      */
                           /* Configuration.                            */
                           if(!DecodeClientConfigurationValue(ValueLength, Value, &(EventData->Event_Data.PASS_Client_Configuration_Update_Data->NotificationsEnabled)))
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
                        /* Always except the write for the Ringer       */
                        /* Control Point, if data is malformatted we    */
                        /* will just throw it away.                     */
                        GATT_Write_Response(BluetoothStackID, TransactionID);

                        /* Format and Dispatch the event.               */
                        EventData = FormatEventHeader(sizeof(Event_Buffer), Event_Buffer, etPASS_Server_Ringer_Control_Command_Indication, InstanceID, GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->ConnectionID, NULL, GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->ConnectionType, &(GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->RemoteDevice));
                        if(EventData)
                        {
                           /* Format the rest of the event.             */
                           EventData->Event_Data_Size = PASS_RINGER_CONTROL_COMMAND_DATA_SIZE;

                           /* Parse out the command.                    */
                           if(!DecodeRingerControlCommand(ValueLength, Value, &(EventData->Event_Data.PASS_Ringer_Control_Command_Data->Command)))
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
                        }
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
   /* Bluetooth Stack PASS Module is Initialized correctly.  This       */
   /* function *MUST* be called before ANY other Bluetooth Stack PASS   */
   /* function can be called.  This function returns non-zero if the    */
   /* Module was initialized correctly, or a zero value if there was an */
   /* error.                                                            */
   /* * NOTE * Internally, this module will make sure that this function*/
   /*          has been called at least once so that the module will    */
   /*          function.  Calling this function from an external        */
   /*          location is not necessary.                               */
int InitializePASSModule(void)
{
   return((int)InitializeModule());
}

   /* The following function is responsible for instructing the         */
   /* Bluetooth Stack DISC Module to clean up any resources that it has */
   /* allocated.  Once this function has completed, NO other Bluetooth  */
   /* Stack DISC Functions can be called until a successful call to the */
   /* InitializePASSModule() function is made.  The parameter to this   */
   /* function specifies the context in which this function is being    */
   /* called.  If the specified parameter is TRUE, then the module will */
   /* make sure that NO functions that would require waiting/blocking on*/
   /* Mutexes/Events are called.  This parameter would be set to TRUE if*/
   /* this function was called in a context where threads would not be  */
   /* allowed to run.  If this function is called in the context where  */
   /* threads are allowed to run then this parameter should be set to   */
   /* FALSE.                                                            */
void CleanupPASSModule(Boolean_t ForceCleanup)
{
   /* Check to make sure that this module has been initialized.         */
   if(InstanceListInitialized)
   {
      /* Wait for access to the PASS Context List.                      */
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

   /* The following function is responsible for opening a PASS Server.  */
   /* The first parameter is the Bluetooth Stack ID on which to open the*/
   /* server.  The second parameter is the Callback function to call    */
   /* when an event occurs on this Server Port.  The third parameter is */
   /* a user-defined callback parameter that will be passed to the      */
   /* callback function with each event.  The final parameter is a      */
   /* pointer to store the GATT Service ID of the registered PASS       */
   /* service.  This can be used to include the service registered by   */
   /* this call.  This function returns the positive, non-zero, Instance*/
   /* ID or a negative error code.                                      */
   /* * NOTE * Only 1 PASS Server may be open at a time, per Bluetooth  */
   /*          Stack ID.                                                */
   /* * NOTE * All Client Requests will be dispatch to the EventCallback*/
   /*          function that is specified by the second parameter to    */
   /*          this function.                                           */
int BTPSAPI PASS_Initialize_Service(unsigned int BluetoothStackID, PASS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID)
{
   GATT_Attribute_Handle_Group_t ServiceHandleRange;

    /* Initialize the Service Handle Group to 0.                        */
   ServiceHandleRange.Starting_Handle = 0;
   ServiceHandleRange.Ending_Handle   = 0;

   return(PASSRegisterService(BluetoothStackID, EventCallback, CallbackParameter, ServiceID, &ServiceHandleRange));
}

   /* The following function is responsible for opening a PASS Server.  */
   /* The first parameter is the Bluetooth Stack ID on which to open the*/
   /* server.  The second parameter is the Callback function to call    */
   /* when an event occurs on this Server Port.  The third parameter is */
   /* a user-defined callback parameter that will be passed to the      */
   /* callback function with each event.  The fourth parameter is a     */
   /* pointer to store the GATT Service ID of the registered PASS       */
   /* service.  This can be used to include the service registered by   */
   /* this call.  The final parameter is a pointer, that on input can be*/
   /* used to control the location of the service in the GATT database, */
   /* and on ouput to store the service handle range.  This function    */
   /* returns the positive, non-zero, Instance ID or a negative error   */
   /* code.                                                             */
   /* * NOTE * Only 1 PASS Server may be open at a time, per Bluetooth  */
   /*          Stack ID.                                                */
   /* * NOTE * All Client Requests will be dispatch to the EventCallback*/
   /*          function that is specified by the second parameter to    */
   /*          this function.                                           */
int BTPSAPI PASS_Initialize_Service_Handle_Range(unsigned int BluetoothStackID, PASS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange)
{
   return(PASSRegisterService(BluetoothStackID, EventCallback, CallbackParameter, ServiceID, ServiceHandleRange));
}

   /* The following function is responsible for closing a previously    */
   /* PASS Server.  The first parameter is the Bluetooth Stack ID on    */
   /* which to close the server.  The second parameter is the InstanceID*/
   /* that was returned from a successful call to                       */
   /* PASS_Initialize_Service().  This function returns a zero if       */
   /* successful or a negative return error code if an error occurs.    */
int BTPSAPI PASS_Cleanup_Service(unsigned int BluetoothStackID, unsigned int InstanceID)
{
   int                   ret_val;
   PASSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID))
   {
      /* Acquire the specified PASS Instance.                           */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Verify that the service is actually registered.             */
         if(ServiceInstance->ServiceID)
         {
            /* Call GATT to un-register the service.                    */
            GATT_Un_Register_Service(BluetoothStackID, ServiceInstance->ServiceID);

            /* mark the instance entry as being free.                   */
            BTPS_MemInitialize(ServiceInstance, 0, PASS_SERVER_INSTANCE_DATA_SIZE);

            /* return success to the caller.                            */
            ret_val = 0;
         }
         else
            ret_val = PASS_ERROR_INVALID_PARAMETER;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(BluetoothStackID);
      }
      else
         ret_val = PASS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = PASS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for querying the number of  */
   /* attributes that are contained in the PASS Service that is         */
   /* registered with a call to PASS_Initialize_Service().  This        */
   /* function returns the non-zero number of attributes that are       */
   /* contained in a PASS Server or zero on failure.                    */
unsigned int BTPSAPI PASS_Query_Number_Attributes(void)
{
   /* Simply return the number of attributes that are contained in a    */
   /* PASS service.                                                     */
   return(PHONE_ALERT_STATUS_SERVICE_ATTRIBUTE_COUNT);
}

   /* The following function is responsible for setting the Alert Status*/
   /* characteristic on the specified PASS instance.  The first         */
   /* parameter is the Bluetooth Stack ID of the Bluetooth Device.  The */
   /* second parameter is the InstanceID returned from a successful call*/
   /* to PASS_Initialize_Server().  The final parameter is the Alert    */
   /* Status to set as the current Alert Status for the specified PASS  */
   /* Instance.  This function returns a zero if successful or a        */
   /* negative return error code if an error occurs.                    */
int BTPSAPI PASS_Set_Alert_Status(unsigned int BluetoothStackID, unsigned int InstanceID, PASS_Alert_Status_t AlertStatus)
{
   int                   ret_val;
   Word_t                Length;
   PASSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID))
   {
      /* Acquire the specified PASS Instance.                           */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Get the length of the Instance Data.                        */
         Length = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(InstanceData[InstanceID-1].AlertStatusLength));

         /* Format the Instance Data.                                   */
         FormatAlertStatus(AlertStatus, (unsigned int)Length, &(InstanceData[InstanceID-1].AlertStatusInstance));

         /* Return success to the caller.                               */
         ret_val = 0;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = PASS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = PASS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for querying the current    */
   /* Alert Status characteristic value on the specified PASS instance. */
   /* The first parameter is the Bluetooth Stack ID of the Bluetooth    */
   /* Device.  The second parameter is the InstanceID returned from a   */
   /* successful call to PASS_Initialize_Server().  The final parameter */
   /* is a pointer to a structure to return the current Alert Status for*/
   /* the specified PASS Instance.  This function returns a zero if     */
   /* successful or a negative return error code if an error occurs.    */
int BTPSAPI PASS_Query_Alert_Status(unsigned int BluetoothStackID, unsigned int InstanceID, PASS_Alert_Status_t *AlertStatus)
{
   int                   ret_val;
   Word_t                Length;
   PASSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (AlertStatus))
   {
      /* Acquire the specified PASS Instance.                           */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Get the length of the Instance Data.                        */
         Length = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(InstanceData[InstanceID-1].AlertStatusLength));

         /* Decode the Instance Data.                                   */
         ret_val = DecodeAlertStatus((unsigned int)Length, &(InstanceData[InstanceID-1].AlertStatusInstance), AlertStatus);

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = PASS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = PASS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for setting the Ringer      */
   /* Setting characteristic on the specified PASS instance.  The first */
   /* parameter is the Bluetooth Stack ID of the Bluetooth Device.  The */
   /* second parameter is the InstanceID returned from a successful call*/
   /* to PASS_Initialize_Server().  The final parameter is the Ringer   */
   /* Setting to set as the current Ringer Setting for the specified    */
   /* PASS Instance.  This function returns a zero if successful or a   */
   /* negative return error code if an error occurs.                    */
int BTPSAPI PASS_Set_Ringer_Setting(unsigned int BluetoothStackID, unsigned int InstanceID, PASS_Ringer_Setting_t RingerSetting)
{
   int                   ret_val;
   Word_t                Length;
   PASSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID))
   {
      /* Acquire the specified PASS Instance.                           */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Get the length of the Instance Data.                        */
         Length = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(InstanceData[InstanceID-1].RingerSettingLength));

         /* Format the Instance Data.                                   */
         ret_val = FormatRingerSetting(RingerSetting, (unsigned int)Length, &(InstanceData[InstanceID-1].RingerSettingInstance));
         if(ret_val > 0)
         {
            /* Return success to the caller.                            */
            ret_val = 0;
         }

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = PASS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = PASS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for querying the current    */
   /* Ringer Setting characteristic value on the specified PASS         */
   /* instance.  The first parameter is the Bluetooth Stack ID of the   */
   /* Bluetooth Device.  The second parameter is the InstanceID returned*/
   /* from a successful call to PASS_Initialize_Server().  The final    */
   /* parameter is a pointer to store the current Ringer Setting for the*/
   /* specified PASS Instance.  This function returns a zero if         */
   /* successful or a negative return error code if an error occurs.    */
int BTPSAPI PASS_Query_Ringer_Setting(unsigned int BluetoothStackID, unsigned int InstanceID, PASS_Ringer_Setting_t *RingerSetting)
{
   int                   ret_val;
   Word_t                Length;
   PASSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (RingerSetting))
   {
      /* Acquire the specified PASS Instance.                           */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Get the length of the Instance Data.                        */
         Length = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(InstanceData[InstanceID-1].RingerSettingLength));

         /* Decode the Instance Data.                                   */
         ret_val = DecodeRingerSetting((unsigned int)Length, &(InstanceData[InstanceID-1].RingerSettingInstance), RingerSetting);

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = PASS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = PASS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for responding to a Read    */
   /* Client Configuration Request.  The first parameter is the         */
   /* Bluetooth Stack ID of the Bluetooth Device.  The second parameter */
   /* is the InstanceID returned from a successful call to              */
   /* PASS_Initialize_Server().  The third is the Transaction ID of the */
   /* request.  The final parameter contains the Client Configuration to*/
   /* send to the remote device.  This function returns a zero if       */
   /* successful or a negative return error code if an error occurs.    */
int BTPSAPI PASS_Read_Client_Configuration_Response(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int TransactionID, Boolean_t NotificationsEnabled)
{
   int                   ret_val;
   Word_t                ValueLength;
   NonAlignedWord_t      ClientConfiguration;
   PASSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (TransactionID))
   {
      /* Acquire the specified PASS Instance.                           */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         ValueLength = GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_LENGTH;
         ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&ClientConfiguration, ((NotificationsEnabled)?GATT_CLIENT_CONFIGURATION_CHARACTERISTIC_NOTIFY_ENABLE:0));

         ret_val = GATT_Read_Response(BluetoothStackID, TransactionID, (unsigned int)ValueLength, (Byte_t *)&ClientConfiguration);

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = PASS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = PASS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for sending a notification  */
   /* of a specified characteristic to a specified remote device.  The  */
   /* first parameter is the Bluetooth Stack ID of the Bluetooth Device.*/
   /* The second parameter is the InstanceID returned from a successful */
   /* call to PASS_Initialize_Server().  The third parameter is the     */
   /* ConnectionID of the remote device to send the notification to.    */
   /* The final parameter specifies the characteristic to notify.  This */
   /* function returns a zero if successful or a negative return error  */
   /* code if an error occurs.                                          */
int BTPSAPI PASS_Send_Notification(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int ConnectionID, PASS_Characteristic_Type_t CharacteristicType)
{
   int                   ret_val;
   Byte_t               *Value;
   Word_t                InstanceTag;
   Word_t                ValueLength;
   Word_t                AttributeOffset;
   PASSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (ConnectionID) && ((CharacteristicType == rrAlertStatus) || (CharacteristicType == rrRingerSetting)))
   {
      /* Acquire the specified PASS Instance.                           */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Figure out the Attribute Offset and Instance Tag of the     */
         /* characteristic that we are notifing.                        */
         if(CharacteristicType == rrAlertStatus)
         {
            AttributeOffset = PASS_ALERT_STATUS_CHARACTERISTIC_VALUE_OFFSET;
            InstanceTag     = PASS_ALERT_STATUS_INSTANCE_TAG_VALUE;
         }
         else
         {
            AttributeOffset = PASS_RINGER_SETTING_CHARACTERISTIC_VALUE_OFFSET;
            InstanceTag     = PASS_RINGER_SETTING_INSTANCE_TAG_VALUE;
         }

         /* Get the value length and the data.                          */
         ValueLength = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(((Byte_t *)(&InstanceData[InstanceID-1]))[InstanceTag]));
         Value       = (Byte_t *)(&(((Byte_t *)(&InstanceData[InstanceID-1]))[InstanceTag + WORD_SIZE]));

         /* Send the notification.                                      */
         ret_val = GATT_Handle_Value_Notification(ServiceInstance->BluetoothStackID, ServiceInstance->ServiceID, ConnectionID, AttributeOffset, ValueLength, Value);
         if(ret_val == ValueLength)
            ret_val = 0;
         else
            ret_val = PASS_ERROR_UNKNOWN_ERROR;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = PASS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = PASS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* PASS Client API.                                                  */

   /* The following function is responsible for parsing a value received*/
   /* from a remote PASS Server interpreting it as the Alert Status     */
   /* characteristic.  The first parameter is the length of the value   */
   /* returned by the remote PASS Server.  The second parameter is a    */
   /* pointer to the data returned by the remote PASS Server.  The final*/
   /* parameter is a pointer to store the parsed Alert Status value.    */
   /* This function returns a zero if successful or a negative return   */
   /* error code if an error occurs.                                    */
int BTPSAPI PASS_Decode_Alert_Status(unsigned int ValueLength, Byte_t *Value, PASS_Alert_Status_t *AlertStatusResult)
{
   /* Call the internal function to decode the Alert Status.            */
   return(DecodeAlertStatus(ValueLength, Value, AlertStatusResult));
}

   /* The following function is responsible for parsing a value received*/
   /* from a remote PASS Server interpreting it as the Ringer Setting   */
   /* characteristic.  The first parameter is the length of the value   */
   /* returned by the remote PASS Server.  The second parameter is a    */
   /* pointer to the data returned by the remote PASS Server.  The final*/
   /* parameter is a pointer to store the parsed Ringer Setting value.  */
   /* This function returns a zero if successful or a negative return   */
   /* error code if an error occurs.                                    */
int BTPSAPI PASS_Decode_Ringer_Setting(unsigned int ValueLength, Byte_t *Value, PASS_Ringer_Setting_t *RingerSetting)
{
   /* Call the internal function to decode the Ringer Setting.          */
   return(DecodeRingerSetting(ValueLength, Value, RingerSetting));
}

   /* The following function is responsible for formatting a Ringer     */
   /* Control Point command.  This formatted value can be then written  */
   /* to a remote PASS Server.  The first parameter is the command to   */
   /* format.  The final two parameters are the buffer length and the   */
   /* buffer to format the command into.  The BufferLength must be large*/
   /* enough to hold the formatted command.  This function returns the  */
   /* number of bytes formatted into Buffer if successful or a negative */
   /* error code.                                                       */
int BTPSAPI PASS_Format_Ringer_Control_Command(PASS_Ringer_Control_Command_t RingerControlCommand, unsigned int BufferLength, Byte_t *Buffer)
{
   /* Call the internal function to format the Ringer Control Command.  */
   return(FormatRingerControlCommand(RingerControlCommand, BufferLength, Buffer));
}
