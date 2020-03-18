/*****< lls.c >****************************************************************/
/*      Copyright 2012 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  LLS - Bluetooth Stack Link Loss Service (GATT Based) for                  */
/*        Stonestreet One Bluetooth Protocol Stack.                           */
/*                                                                            */
/*  Author:  Ajay Parashar                                                    */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   06/28/12  A. Parashar       Initial creation.                            */
/******************************************************************************/
#include "SS1BTPS.h"    /* Bluetooth Stack API Prototypes/Constants.          */
#include "SS1BTGAT.h"   /* Bluetooth Stack GATT APIPrototypes/Constants.      */
#include "SS1BTLLS.h"   /* Bluetooth LLS API Prototypes/Constants.            */
#include "BTPSKRNL.h"   /* BTPS Kernel Prototypes/Constants.                  */
#include "LLS.h"        /* Bluetooth LLS Prototypes/Constants.                */

   /* The following controls the number of supported LLS instances.     */
#define LLS_MAXIMUM_SUPPORTED_INSTANCES                 (BTPS_CONFIGURATION_LLS_MAXIMUM_SUPPORTED_INSTANCES)

   /* The following structure defines the Instance Data that must be    */
   /* unique for each LLS service registered (Only 1 per Bluetooth      */
   /* Stack).                                                           */
typedef __PACKED_STRUCT_BEGIN__ struct _tagLLS_Instance_Data_t
{
  NonAlignedWord_t Alert_Level_Length;
  NonAlignedByte_t Alert_Level;

} __PACKED_STRUCT_END__ LLS_Instance_Data_t;

#define LLS_INSTANCE_DATA_SIZE                           (sizeof(LLS_Instance_Data_t))

   /* The following define the instance tags for each LLS service data  */
   /* that is unique per registered service.                            */
#define LLS_ALERT_LEVEL_INSTANCE_TAG                   (BTPS_STRUCTURE_OFFSET(LLS_Instance_Data_t,Alert_Level_Length ))

   /*********************************************************************/
   /**               Link Lost Service Table                           **/
   /*********************************************************************/

   /* The Link Lost  Service Declaration UUID.                          */
static BTPSCONST GATT_Primary_Service_16_Entry_t LLS_Service_UUID =
{
   LLS_SERVICE_BLUETOOTH_UUID_CONSTANT
};

   /* The Alert Level Characteristic Declaration.                       */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t LLS_Alert_Level_Declaration =
{
   (GATT_CHARACTERISTIC_PROPERTIES_READ|GATT_CHARACTERISTIC_PROPERTIES_WRITE),
   LLS_ALERT_LEVEL_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The Alert Level Characteristic Value.                             */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t LLS_Alert_Level_Value =
{
   LLS_ALERT_LEVEL_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   LLS_ALERT_LEVEL_INSTANCE_TAG,
   NULL
};

   /* The following defines the Link Loss service that is registered    */
   /* with the GATT_Register_Service function call.                     */
   /* * NOTE * This array will be registered with GATT in the call to   */
   /*          GATT_Register_Service.                                   */
BTPSCONST GATT_Service_Attribute_Entry_t Link_Loss_Service[] =
{
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetPrimaryService16,            (Byte_t *)&LLS_Service_UUID},
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetCharacteristicDeclaration16, (Byte_t *)&LLS_Alert_Level_Declaration},
   {GATT_ATTRIBUTE_FLAGS_READABLE_WRITABLE, aetCharacteristicValue16,       (Byte_t *)&LLS_Alert_Level_Value},
};

#define LINK_LOSS_SERVICE_ATTRIBUTE_COUNT                      (sizeof(Link_Loss_Service)/sizeof(GATT_Service_Attribute_Entry_t))

   /*********************************************************************/
   /**                    END OF SERVICE TABLE                         **/
   /*********************************************************************/

   /* The following type defines a union large enough to hold all events*/
   /* dispatched by this module.                                        */
typedef union
{
   LLS_Alert_Level_Update_Data_t          Alert_Level_Update_Data;
 } LLS_Event_Data_Buffer_t;

#define LLS_EVENT_DATA_BUFFER_SIZE                      (sizeof(LLS_Event_Data_Buffer_t))

   /* LLS Service Instance Block.  This structure contains All          */
   /* information associated with a specific Bluetooth Stack ID (member */
   /* is present in this structure).                                    */
typedef struct _tagLLSServerInstance_t
{
   unsigned int         BluetoothStackID;
   unsigned int         ServiceID;
   LLS_Event_Callback_t EventCallback;
   unsigned long        CallbackParameter;
} LLSServerInstance_t;

#define LLS_SERVER_INSTANCE_DATA_SIZE                    (sizeof(LLSServerInstance_t))

   /* Internal Variables to this Module (Remember that all variables    */
   /* declared static are initialized to 0 automatically by the         */
   /* compiler as part of standard C/C++).                              */
static LLS_Instance_Data_t  InstanceData[LLS_MAXIMUM_SUPPORTED_INSTANCES];
                                            /* Variable which holds all */
                                            /* data that is unique for  */
                                            /* each service instance.   */

static LLSServerInstance_t InstanceList[LLS_MAXIMUM_SUPPORTED_INSTANCES];
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
static LLSServerInstance_t *AcquireServiceInstance(unsigned int BluetoothStackID, unsigned int *InstanceID);

static int DecodeAlertLevel(unsigned int InstanceID, unsigned int BufferLength, Byte_t *Buffer, Byte_t *AlertLevel);
static LLS_Event_Data_t *FormatEventHeader(unsigned int BufferLength, Byte_t *Buffer, LLS_Event_Type_t EventType, unsigned int InstanceID, unsigned int ConnectionID, GATT_Connection_Type_t ConnectionType, BD_ADDR_t *BD_ADDR);

static int LLSRegisterService(unsigned int BluetoothStackID, LLS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange);

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

   /* The following function is a utility function that exists to decode*/
   /* an Alert Level into a user specified value.  This                 */
   /* function returns the zero if successful or a negative error code. */
static int DecodeAlertLevel(unsigned int InstanceID, unsigned int BufferLength, Byte_t *Buffer, Byte_t *AlertLevel)
{
   int ret_val = LLS_ERROR_MALFORMATTED_DATA;

   /* Verify that the input parameters are valid.                       */
   if((BufferLength == LLS_ALERT_LEVEL_LENGTH) && (Buffer) && (AlertLevel))
   {
      /* Read the requested Alert Level.                                */
      *AlertLevel = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(Buffer);

      /* Assign the Alert Level into the Instance Data.                 */
      ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(InstanceData[InstanceID-1].Alert_Level), *AlertLevel);
      ret_val = 0;
   }
   else
   {
      if(BufferLength == LLS_ALERT_LEVEL_LENGTH)
         ret_val = LLS_ERROR_INVALID_PARAMETER;
   }

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is a utility function that exists to format*/
   /* a LLS Event into the specified buffer.                            */
   /* * NOTE * TransactionID is optional and may be set to NULL.        */
   /* * NOTE * BD_ADDR is NOT optional and may NOT be set to NULL.      */
static LLS_Event_Data_t *FormatEventHeader(unsigned int BufferLength, Byte_t *Buffer, LLS_Event_Type_t EventType, unsigned int InstanceID, unsigned int ConnectionID, GATT_Connection_Type_t ConnectionType, BD_ADDR_t *BD_ADDR)
{
   LLS_Event_Data_t *EventData = NULL;

   if((BufferLength >= (LLS_EVENT_DATA_SIZE + LLS_EVENT_DATA_BUFFER_SIZE)) && (Buffer) && (BD_ADDR))
   {
      /* Format the header of the event, that is data that is common to */
      /* all events.                                                    */
      BTPS_MemInitialize(Buffer, 0, BufferLength);

      EventData                                                               = (LLS_Event_Data_t *)Buffer;
      EventData->Event_Data_Type                                              = EventType;
      EventData->Event_Data.LLS_Alert_Level_Update_Data                       = (LLS_Alert_Level_Update_Data_t *)(((Byte_t *)EventData) + LLS_EVENT_DATA_SIZE);

      EventData->Event_Data.LLS_Alert_Level_Update_Data->InstanceID   = InstanceID;
      EventData->Event_Data.LLS_Alert_Level_Update_Data->ConnectionID = ConnectionID;

      EventData->Event_Data.LLS_Alert_Level_Update_Data->ConnectionType = ConnectionType;
      EventData->Event_Data.LLS_Alert_Level_Update_Data->RemoteDevice   = *BD_ADDR;

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

   for(Index=0;Index<LLS_MAXIMUM_SUPPORTED_INSTANCES;Index++)
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
static LLSServerInstance_t *AcquireServiceInstance(unsigned int BluetoothStackID, unsigned int *InstanceID)
{
   unsigned int         LocalInstanceID;
   unsigned int         Index;
   LLSServerInstance_t *ret_val = NULL;

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
         if((LocalInstanceID) && (LocalInstanceID <= LLS_MAXIMUM_SUPPORTED_INSTANCES))
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
               for(Index=0;Index<LLS_MAXIMUM_SUPPORTED_INSTANCES;Index++)
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
      {
         BSC_UnLockBluetoothStack(BluetoothStackID);
      }
   }

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is a utility function which is used to     */
   /* register an LLS Service.  This function returns the positive,     */
   /* non-zero, Instance ID on success or a negative error code.        */
static int LLSRegisterService(unsigned int BluetoothStackID, LLS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange)
{
   int                  ret_val;
   unsigned int         InstanceID;
   LLSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (ServiceID))
   {
      /* Verify that no instance is registered to this Bluetooth Stack. */
      if(!InstanceRegisteredByStackID(BluetoothStackID))
      {
         /* Acquire a free LLS Instance.                                */
         InstanceID = 0;
         if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
         {
            /* Call GATT to register the LLS service.                   */
            ret_val = GATT_Register_Service(BluetoothStackID, LLS_SERVICE_FLAGS, LINK_LOSS_SERVICE_ATTRIBUTE_COUNT, (GATT_Service_Attribute_Entry_t *)Link_Loss_Service, ServiceHandleRange, GATT_ServerEventCallback, InstanceID);
            if(ret_val > 0)
            {
                /* Save the Instance information.                       */
               ServiceInstance->BluetoothStackID  = BluetoothStackID;
               ServiceInstance->ServiceID         = (unsigned int)ret_val;
               ServiceInstance->EventCallback     = EventCallback;
               ServiceInstance->CallbackParameter = CallbackParameter;
               *ServiceID                         = (unsigned int)ret_val;

               /* Intilize the Instance Data for this instance.         */
               BTPS_MemInitialize(&InstanceData[InstanceID-1], 0,LLS_INSTANCE_DATA_SIZE);

               ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(InstanceData[InstanceID-1].Alert_Level_Length), LLS_ALERT_LEVEL_LENGTH);

               /* Return the LLS Instance ID.                           */
               ret_val = (int)InstanceID;
            }

            /* UnLock the previously locked Bluetooth Stack.            */
            BSC_UnLockBluetoothStack(BluetoothStackID);
         }
         else
            ret_val = LLS_ERROR_INSUFFICIENT_RESOURCES;
      }
      else
         ret_val = LLS_ERROR_SERVICE_ALREADY_REGISTERED;
   }
   else
      ret_val = LLS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is the GATT Server Event Callback that     */
   /* handles all requests made to the LLS Service for all registered   */
   /* instances.                                                        */
static void BTPSAPI GATT_ServerEventCallback(unsigned int BluetoothStackID, GATT_Server_Event_Data_t *GATT_ServerEventData, unsigned long CallbackParameter)
{
   Word_t               AttributeOffset;
   Word_t               InstanceTag;
   Word_t               ValueLength;
   Byte_t              *Value;
   unsigned int         TransactionID;
   unsigned int         InstanceID;
   LLSServerInstance_t *ServiceInstance;
   LLS_Event_Data_t    *EventData;
   Byte_t               Event_Buffer[LLS_EVENT_DATA_SIZE + LLS_EVENT_DATA_BUFFER_SIZE];

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
            /* Verify that the Event Data is valid.                     */
            AttributeOffset = GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->AttributeOffset;
            TransactionID   = GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->TransactionID;

            if(GATT_ServerEventData->Event_Data.GATT_Read_Request_Data)
            {
               /* Verify that they are not trying to write with an      */
               /* offset or using preprared writes.                     */
               if(!(GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->AttributeValueOffset))
               {
                  /* Get the instance tag for the characteristic.       */
                  InstanceTag = (Word_t)(((GATT_Characteristic_Value_16_Entry_t *)Link_Loss_Service[AttributeOffset].Attribute_Value)->Characteristic_Value_Length);
                  ValueLength = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(((Byte_t *)(&InstanceData[InstanceID-1]))[InstanceTag]));
                  Value       = (Byte_t *)(&(((Byte_t *)(&InstanceData[InstanceID-1]))[InstanceTag + WORD_SIZE]));

                  /* Respond with the data.                             */
                  GATT_Read_Response(BluetoothStackID, TransactionID, (unsigned int)ValueLength, Value);
               }
            }
            else
               GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, ATT_PROTOCOL_ERROR_CODE_ATTRIBUTE_NOT_LONG);
            break;
         case etGATT_Server_Write_Request:
            AttributeOffset = GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->AttributeOffset;
            TransactionID   = GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->TransactionID;
            ValueLength     = GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->AttributeValueLength;
            Value           = GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->AttributeValue;
            /* Verify the Event data is valid                           */
            if(GATT_ServerEventData->Event_Data.GATT_Write_Request_Data)
            {
               /* Verify that the attribute value is valid.             */
               if((Value) && (ValueLength == LLS_ALERT_LEVEL_LENGTH))
               {
                  /* Format and Dispatch the event.                     */
                  EventData = FormatEventHeader(sizeof(Event_Buffer), Event_Buffer, etLLS_Alert_Level_Update, InstanceID, GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->ConnectionID, GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->ConnectionType, &(GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->RemoteDevice));
                  if(EventData)
                  {
                     /* Format the rest of the event.                   */
                     EventData->Event_Data_Size = LLS_ALERT_LEVEL_UPDATE_DATA_SIZE;

                     /* Parse out the command.                          */
                     if(!DecodeAlertLevel(InstanceID, ValueLength, Value, &(EventData->Event_Data.LLS_Alert_Level_Update_Data->AlertLevel)))
                     {
                        /* Since the requested Alert Level was formatted*/
                        /* correctly and with the accepted range we will*/
                        /* accept the new Alert Level.                  */
                        GATT_Write_Response(BluetoothStackID, TransactionID);

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
                     else
                        GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR);
                  }
                  else
                     GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR);
               }
               else
                  GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, ATT_PROTOCOL_ERROR_CODE_INVALID_ATTRIBUTE_VALUE_LENGTH);
            }
            break;
         default:
            /* Do nothing, as this is just here to get rid of           */
            /* warnings that some compilers flag when not all cases     */
            /* are handled in a switch off of a enumerated value.       */
            break;
         }

         BSC_UnLockBluetoothStack(BluetoothStackID);
      }
   }
}

   /* The following function is responsible for making sure that the    */
   /* Bluetooth Stack LLS Module is Initialized correctly.  This        */
   /* function *MUST* be called before ANY other Bluetooth Stack LLS    */
   /* function can be called.  This function returns non-zero if the    */
   /* Module was initialized correctly, or a zero value if there was an */
   /* error.                                                            */
   /* * NOTE * Internally, this module will make sure that this function*/
   /*          has been called at least once so that the module will    */
   /*          function.  Calling this function from an external        */
   /*          location is not necessary.                               */
int InitializeLLSModule(void)
{
   return((int)InitializeModule());
}

   /* The following function is responsible for instructing the         */
   /* Bluetooth Stack LLS Module to clean up any resources that it has  */
   /* allocated.  Once this function has completed, NO other Bluetooth  */
   /* Stack LLS Functions can be called until a successful call to the  */
   /* InitializeLLSModule() function is made.  The parameter to this    */
   /* function specifies the context in which this function is being    */
   /* called.  If the specified parameter is TRUE, then the module will */
   /* make sure that NO functions that would require waiting/blocking on*/
   /* Mutexes/Events are called.  This parameter would be set to TRUE if*/
   /* this function was called in a context where threads would not be  */
   /* allowed to run.  If this function is called in the context where  */
   /* threads are allowed to run then this parameter should be set to   */
   /* FALSE.                                                            */
void CleanupLLSModule(Boolean_t ForceCleanup)
{
   /* Check to make sure that this module has been initialized.         */
   if(InstanceListInitialized)
   {
      /* Wait for access to the LLS Context List.                       */
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

   /* The following function is responsible for opening a LLS Server.   */
   /* The first parameter is the Bluetooth Stack ID on which to open the*/
   /* server.  The second parameter is the Callback function to call    */
   /* when an event occurs on this Server Port.  The third parameter is */
   /* a user-defined callback parameter that will be passed to the      */
   /* callback function with each event.  The final parameter is a      */
   /* pointer to store the GATT Service ID of the registered LLS        */
   /* service.  This can be used to include the service registered by   */
   /* this call.  This function returns the positive, non-zero, Instance*/
   /* ID or a negative error code.                                      */
   /* * NOTE * Only 1 LLS Server may be open at a time, per Bluetooth   */
   /*          Stack ID.                                                */
   /* * NOTE * All Client Requests will be dispatch to the EventCallback*/
   /*          function that is specified by the second parameter to    */
   /*          this function.                                           */
int BTPSAPI LLS_Initialize_Service(unsigned int BluetoothStackID, LLS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID)
{
    GATT_Attribute_Handle_Group_t ServiceHandleRange;

    /* Initialize the Service Handle Group to 0.                        */
   ServiceHandleRange.Starting_Handle = 0;
   ServiceHandleRange.Ending_Handle   = 0;

   return(LLSRegisterService(BluetoothStackID, EventCallback, CallbackParameter, ServiceID, &ServiceHandleRange));
}

   /* The following function is responsible for opening a LLS Server.   */
   /* The first parameter is the Bluetooth Stack ID on which to open the*/
   /* server.  The second parameter is the Callback function to call    */
   /* when an event occurs on this Server Port.  The third parameter is */
   /* a user-defined callback parameter that will be passed to the      */
   /* callback function with each event.  The fourth parameter is a     */
   /* pointer to store the GATT Service ID of the registered LLS        */
   /* service.  This can be used to include the service registered by   */
   /* this call.  The final parameter is a pointer, that on input can be*/
   /* used to control the location of the service in the GATT database, */
   /* and on ouput to store the service handle range.  This function    */
   /* returns the positive, non-zero, Instance ID or a negative error   */
   /* code.                                                             */
   /* * NOTE * Only 1 LLS Server may be open at a time, per Bluetooth   */
   /*          Stack ID.                                                */
   /* * NOTE * All Client Requests will be dispatch to the EventCallback*/
   /*          function that is specified by the second parameter to    */
   /*          this function.                                           */
int BTPSAPI LLS_Initialize_Service_Handle_Range(unsigned int BluetoothStackID, LLS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange)
{
   return(LLSRegisterService(BluetoothStackID, EventCallback, CallbackParameter, ServiceID, ServiceHandleRange));
}

   /* The following function is responsible for closing a previously    */
   /* opened LLS Server.  The first parameter is the Bluetooth Stack ID */
   /* on which to close the server.  The second parameter is the        */
   /* InstanceID that was returned from a successful call to            */
   /* LLS_Initialize_Service().  This function returns a zero if        */
   /* successful or a negative return error code if an error occurs.    */
int BTPSAPI LLS_Cleanup_Service(unsigned int BluetoothStackID, unsigned int InstanceID)
{
   int                  ret_val;
   LLSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID))
   {
      /* Acquire the specified LLS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Verify that the service is actually registered.             */
         if(ServiceInstance->ServiceID)
         {
            /* Call GATT to un-register the service.                    */
            GATT_Un_Register_Service(BluetoothStackID, ServiceInstance->ServiceID);

            /* mark the instance entry as being free.                   */
            BTPS_MemInitialize(ServiceInstance, 0, LLS_SERVER_INSTANCE_DATA_SIZE);

            /* return success to the caller.                            */
            ret_val = 0;
         }
         else
            ret_val = LLS_ERROR_INVALID_PARAMETER;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(BluetoothStackID);
      }
      else
         ret_val = LLS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = LLS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for querying the number of  */
   /* attributes that are contained in the LLS Service that is          */
   /* registered with a call to LLS_Initialize_Service().  This function*/
   /* returns the non-zero number of attributes that are contained in a */
   /* LLS Server or zero on failure.                                    */
unsigned int BTPSAPI LLS_Query_Number_Attributes(void)
{
   /* Simply return the number of attributes that are contained in a LLS*/
   /* service.                                                          */
   return(LINK_LOSS_SERVICE_ATTRIBUTE_COUNT);
}

   /* The following function is responsible for setting the Alert Level */
   /* on the specified LLS Instance.  The first parameter is Bluetooth  */
   /* Stack ID of the Bluetooth Device.  The second parameter is the    */
   /* InstanceID returned from a successful call to                     */
   /* LLS_Initialize_Server().  The final parameter is the Alert Level  */
   /* to set for the specified LLS Instance.  This function returns a   */
   /* zero if successful or a negative return error code if an error    */
   /* occurs.                                                           */
int BTPSAPI LLS_Set_Alert_Level(unsigned int BluetoothStackID, unsigned int InstanceID, Byte_t Alert_Level)
{
   int                  ret_val;
   LLSServerInstance_t *ServiceInstance;

   /* Make sure the Parameters passed to us are semi-valid              */
   if((BluetoothStackID) && (InstanceID) && (Alert_Level >= LLS_ALERT_LEVEL_NO_ALERT) && (Alert_Level <= LLS_ALERT_LEVEL_HIGH_ALERT))
   {
      /*Acquire the Specified LLS Instance                              */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID,&InstanceID)) != NULL)
      {
         /* Assign the Alert Level for the specified instance.          */
         ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(InstanceData[InstanceID-1].Alert_Level), Alert_Level);

         /* Return success to the caller.                               */
         ret_val = 0;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = LLS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = LLS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for querying the Alert Level*/
   /* on the specified LLS Instance.The first parameter is the Bluetooth*/
   /* Stack ID of the Bluetooth Device.  The second parameter is the    */
   /* InstanceID returned from a successful call LLS_Initialize_Server  */
   /* The final parameter is a pointer to return Alert Level for the    */
   /* specified LLS Instance.  This function returns a zero if          */
   /* successful or a negative return error code if an error occurs.    */
int BTPSAPI LLS_Query_Alert_Level(unsigned int BluetoothStackID, unsigned int InstanceID, Byte_t *Alert_Level)
{
   int                  ret_val;
   LLSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (Alert_Level))
   {
      /* Acquire the specified LLS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Retrieve the LLS from the specified instance.               */
         *Alert_Level             = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(InstanceData[InstanceID-1].Alert_Level));
         /* Return success to the caller.                               */
         ret_val                 = 0;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = LLS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = LLS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}
