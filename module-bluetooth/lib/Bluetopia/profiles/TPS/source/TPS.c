/*****< tps.c >****************************************************************/
/*      Copyright 2012 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*      Copyright 2015 Texas Instruments Incorporated.                        */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  TPS - Bluetooth Stack Tx Power Service (GATT Based) for                   */
/*        Stonestreet One Bluetooth Protocol Stack.                           */
/*                                                                            */
/*  Author:  Jay Wheeler                                                      */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   08/15/12  J. Wheeler     Initial creation.                               */
/*   01/18/15  D. Horowitz    Adding Validation check to Tx power             */
/*                            Changing TX_Power_Level from Byte_t to SByte_t  */
/******************************************************************************/
#include "SS1BTPS.h"        /* Bluetooth Stack API Prototypes/Constants.      */
#include "SS1BTGAT.h"       /* Bluetooth Stack GATT API Prototypes/Constants. */
#include "SS1BTTPS.h"       /* Bluetooth TPS API Prototypes/Constants.        */

#include "BTPSKRNL.h"       /* BTPS Kernel Prototypes/Constants.              */
#include "TPS.h"            /* Bluetooth TPS Prototypes/Constants.            */

   /* The following controls the number of supported TPS instances.     */
#define TPS_MAXIMUM_SUPPORTED_INSTANCES                 (BTPS_CONFIGURATION_TPS_MAXIMUM_SUPPORTED_INSTANCES)

   /* The following defines the TPS Instance Data, that contains data   */
   /* is unique for each TPS Service Instance.                          */
typedef __PACKED_STRUCT_BEGIN__ struct _tagTPS_Instance_Data_t
{
   NonAlignedWord_t   Tx_Power_Level_Length;
   NonAlignedSByte_t  Tx_Power_Level;
} __PACKED_STRUCT_END__ TPS_Instance_Data_t;

#define TPS_INSTANCE_DATA_SIZE                           (sizeof(TPS_Instance_Data_t))

#define TPS_TX_POWER_LEVEL_INSTANCE_TAG                  (BTPS_STRUCTURE_OFFSET(TPS_Instance_Data_t, Tx_Power_Level_Length))

   /*********************************************************************/
   /**                      Tx Power Service Table                     **/
   /*********************************************************************/

   /* The Tx Power Service Declaration UUID.                            */
static BTPSCONST GATT_Primary_Service_16_Entry_t TPS_Service_UUID =
{
   TPS_SERVICE_BLUETOOTH_UUID_CONSTANT
};

   /* The Tx Power Level Characteristic Declaration.                    */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t TPS_Tx_Power_Level_Declaration =
{
   GATT_CHARACTERISTIC_PROPERTIES_READ,
   TPS_TX_POWER_LEVEL_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The Tx Power Level Characteristic Value.                          */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t  TPS_Tx_Power_Level_Value =
{
   TPS_TX_POWER_LEVEL_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   TPS_TX_POWER_LEVEL_INSTANCE_TAG,
   NULL
};

   /* The following defines the Tx Power Service that is                */
   /* registered with the GATT_Register_Service function call.          */
   /* * NOTE * This array will be registered with GATT in the call to   */
   /*          GATT_Register_Service.                                   */
BTPSCONST GATT_Service_Attribute_Entry_t Tx_Power_Service[] =
{
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetPrimaryService16,            (Byte_t *)&TPS_Service_UUID},
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetCharacteristicDeclaration16, (Byte_t *)&TPS_Tx_Power_Level_Declaration},
   {GATT_ATTRIBUTE_FLAGS_READABLE,          aetCharacteristicValue16,       (Byte_t *)&TPS_Tx_Power_Level_Value},
};

#define TX_POWER_SERVICE_ATTRIBUTE_COUNT                                (sizeof(Tx_Power_Service)/sizeof(GATT_Service_Attribute_Entry_t))

#define TPS_TX_POWER_LEVEL_ATTRIBUTE_OFFSET                             2

   /*********************************************************************/
   /**                    END OF SERVICE TABLE                         **/
   /*********************************************************************/

   /* TPS Service Instance Block.  This structure contains All          */
   /* information associated with a specific Bluetooth Stack ID (member */
   /* is present in this structure).                                    */
typedef struct _tagTPSServerInstance_t
{
   unsigned int         BluetoothStackID;
   unsigned int         ServiceID;
} TPSServerInstance_t;

#define TPS_SERVER_INSTANCE_DATA_SIZE                    (sizeof(TPSServerInstance_t))

   /* Internal Variables to this Module (Remember that all variables    */
   /* declared static are initialized to 0 automatically by the         */
   /* compiler as part of standard C/C++).                              */

static TPS_Instance_Data_t InstanceData[TPS_MAXIMUM_SUPPORTED_INSTANCES];
                                            /* Variable which holds all */
                                            /* data that is unique for  */
                                            /* each service instance.   */

static TPSServerInstance_t InstanceList[TPS_MAXIMUM_SUPPORTED_INSTANCES];
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
static TPSServerInstance_t *AcquireServiceInstance(unsigned int BluetoothStackID, unsigned int *InstanceID);

static int TPSRegisterService(unsigned int BluetoothStackID, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange);

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

   for(Index=0;Index<TPS_MAXIMUM_SUPPORTED_INSTANCES;Index++)
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
static TPSServerInstance_t *AcquireServiceInstance(unsigned int BluetoothStackID, unsigned int *InstanceID)
{
   unsigned int         LocalInstanceID;
   unsigned int         Index;
   TPSServerInstance_t *ret_val = NULL;

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
         if((LocalInstanceID) && (LocalInstanceID <= TPS_MAXIMUM_SUPPORTED_INSTANCES))
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
               for(Index=0;Index<TPS_MAXIMUM_SUPPORTED_INSTANCES;Index++)
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
   /* register an TPS Service.  This function returns the positive,     */
   /* non-zero, Instance ID on success or a negative error code.        */
static int TPSRegisterService(unsigned int BluetoothStackID, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange)
{
   int                  ret_val;
   unsigned int         InstanceID;
   TPSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (ServiceID))
   {
      /* Verify that no instance is registered to this Bluetooth Stack. */
      if(!InstanceRegisteredByStackID(BluetoothStackID))
      {
         /* Acquire a free TPS Instance.                                */
         InstanceID = 0;
         if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
         {
            /* Call GATT to register the TPS service.                   */
            ret_val = GATT_Register_Service(BluetoothStackID, TPS_SERVICE_FLAGS, TX_POWER_SERVICE_ATTRIBUTE_COUNT, (GATT_Service_Attribute_Entry_t *)Tx_Power_Service, ServiceHandleRange, GATT_ServerEventCallback, InstanceID);
            if(ret_val > 0)
            {
               /* Save the Instance information.                        */
               ServiceInstance->BluetoothStackID  = BluetoothStackID;
               ServiceInstance->ServiceID         = (unsigned int)ret_val;
               *ServiceID                         = (unsigned int)ret_val;

               /* Initialize the Instance Data for this instance.       */
               BTPS_MemInitialize(&InstanceData[InstanceID-1], 0, TPS_INSTANCE_DATA_SIZE);

               ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(InstanceData[InstanceID-1].Tx_Power_Level_Length), TPS_TX_POWER_LEVEL_LENGTH);

               /* Return the TPS Instance ID.                           */
               ret_val = (int)InstanceID;
            }

            /* UnLock the previously locked Bluetooth Stack.            */
            BSC_UnLockBluetoothStack(BluetoothStackID);
         }
         else
            ret_val = TPS_ERROR_INSUFFICIENT_RESOURCES;
      }
      else
         ret_val = TPS_ERROR_SERVICE_ALREADY_REGISTERED;
   }
   else
      ret_val = TPS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is the GATT Server Event Callback that     */
   /* handles all requests made to the TPS Service for all registered   */
   /* instances.                                                        */
static void BTPSAPI GATT_ServerEventCallback(unsigned int BluetoothStackID, GATT_Server_Event_Data_t *GATT_ServerEventData, unsigned long CallbackParameter)
{
   Word_t                     AttributeOffset;
   Word_t                     InstanceTag;
   Word_t                     ValueLength;
   Byte_t                    *Value;
   unsigned int               TransactionID;
   unsigned int               InstanceID;
   TPSServerInstance_t       *ServiceInstance;

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
                  /* offset or using prepared writes.                   */
                  if(!(GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->AttributeValueOffset))
                  {
                     /* Verify that the read request is on the Tx Power */
                     /* Level characteristic.                           */
                     if((Tx_Power_Service[AttributeOffset].Attribute_Entry_Type == aetCharacteristicValue16) && (AttributeOffset == TPS_TX_POWER_LEVEL_ATTRIBUTE_OFFSET))
                     {
                        /* Get the instance tag for the characteristic. */
                        InstanceTag = (Word_t)(((GATT_Characteristic_Value_16_Entry_t *)Tx_Power_Service[AttributeOffset].Attribute_Value)->Characteristic_Value_Length);
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
   /* Bluetooth Stack TPS Module is Initialized correctly.  This        */
   /* function *MUST* be called before ANY other Bluetooth Stack TPS    */
   /* function can be called.  This function returns non-zero if the    */
   /* Module was initialized correctly, or a zero value if there was an */
   /* error.                                                            */
   /* * NOTE * Internally, this module will make sure that this function*/
   /*          has been called at least once so that the module will    */
   /*          function.  Calling this function from an external        */
   /*          location is not necessary.                               */
int InitializeTPSModule(void)
{
   return((int)InitializeModule());
}

   /* The following function is responsible for instructing the         */
   /* Bluetooth Stack TPS Module to clean up any resources that it has  */
   /* allocated.  Once this function has completed, NO other Bluetooth  */
   /* Stack TPS Functions can be called until a successful call to the  */
   /* InitializeTPSModule() function is made.  The parameter to this    */
   /* function specifies the context in which this function is being    */
   /* called.  If the specified parameter is TRUE, then the module will */
   /* make sure that NO functions that would require waiting/blocking on*/
   /* Mutexes/Events are called.  This parameter would be set to TRUE if*/
   /* this function was called in a context where threads would not be  */
   /* allowed to run.  If this function is called in the context where  */
   /* threads are allowed to run then this parameter should be set to   */
   /* FALSE.                                                            */
void CleanupTPSModule(Boolean_t ForceCleanup)
{
   /* Check to make sure that this module has been initialized.         */
   if(InstanceListInitialized)
   {
      /* Wait for access to the TPS Context List.                       */
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

   /* The following function is responsible for opening a TPS Server.   */
   /* The first parameter is the Bluetooth Stack ID on which to open the*/
   /* server.  The second parameter is a pointer to store the GATT      */
   /* Service ID of the registered TPS service.  This can be used to    */
   /* include the service registered by this call.  This function       */
   /* returns the positive, non-zero, Instance ID or a negative error   */
   /* code.                                                             */
   /* * NOTE * Only 1 TPS Server may be open at a time, per Bluetooth   */
   /*          Stack ID.                                                */
int BTPSAPI TPS_Initialize_Service(unsigned int BluetoothStackID, unsigned int *ServiceID)
{
   GATT_Attribute_Handle_Group_t ServiceHandleRange;

    /* Initialize the Service Handle Group to 0.                        */
   ServiceHandleRange.Starting_Handle = 0;
   ServiceHandleRange.Ending_Handle   = 0;

   return(TPSRegisterService(BluetoothStackID, ServiceID, &ServiceHandleRange));
}

   /* The following function is responsible for opening a TPS Server.   */
   /* The first parameter is the Bluetooth Stack ID on which to open the*/
   /* server.  The second parameter is a pointer to store the GATT      */
   /* Service ID of the registered TPS service.  This can be used to    */
   /* include the service registered by this call.  The final parameter */
   /* is a pointer, that on input can be used to control the location of*/
   /* the service in the GATT database, and on ouput to store the       */
   /* service handle range.  This function returns the positive,        */
   /* non-zero, Instance ID or a negative error code.                   */
   /* * NOTE * Only 1 TPS Server may be open at a time, per Bluetooth   */
   /*          Stack ID.                                                */
int BTPSAPI TPS_Initialize_Service_Handle_Range(unsigned int BluetoothStackID, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange)
{
   return(TPSRegisterService(BluetoothStackID, ServiceID, ServiceHandleRange));
}

   /* The following function is responsible for closing a previously    */
   /* opened TPS Server.  The first parameter is the Bluetooth Stack ID */
   /* on which to close the server.  The second parameter is the        */
   /* InstanceID that was returned from a successful call to            */
   /* TPS_Initialize_Service().  This function returns a zero if        */
   /* successful or a negative return error code if an error occurs.    */
int BTPSAPI TPS_Cleanup_Service(unsigned int BluetoothStackID, unsigned int InstanceID)
{
   int                  ret_val;
   TPSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID))
   {
      /* Acquire the specified TPS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Verify that the service is actually registered.             */
         if(ServiceInstance->ServiceID)
         {
            /* Call GATT to un-register the service.                    */
            GATT_Un_Register_Service(BluetoothStackID, ServiceInstance->ServiceID);

            /* Mark the instance entry as being free.                   */
            BTPS_MemInitialize(ServiceInstance, 0, TPS_SERVER_INSTANCE_DATA_SIZE);

            /* Return success to the caller.                            */
            ret_val = 0;
         }
         else
            ret_val = TPS_ERROR_INVALID_PARAMETER;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(BluetoothStackID);
      }
      else
         ret_val = TPS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = TPS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for querying the number of  */
   /* attributes that are contained in the TPS Service that is          */
   /* registered with a call to TPS_Initialize_Service().  This function*/
   /* returns the non-zero number of attributes that are contained in a */
   /* TPS Server or zero on failure.                                    */
unsigned int BTPSAPI TPS_Query_Number_Attributes(void)
{
   /* Simply return the number of attributes that are contained in a TPS*/
   /* service.                                                          */
   return(TX_POWER_SERVICE_ATTRIBUTE_COUNT);
}

   /* The following function is responsible for setting the Tx Power    */
   /* Level on the specified TPS Instance.  The first parameter is the  */
   /* Bluetooth Stack ID of the Bluetooth Device.  The second parameter */
   /* is the InstanceID returned from a successful call to              */
   /* TPS_Initialize_Server().  The final parameter is the Tx Power     */
   /* Level to set for the specified TPS Instance.  This function       */
   /* returns a zero if successful or a negative return error code if   */
   /* an error occurs.                                                  */
int BTPSAPI TPS_Set_Tx_Power_Level(unsigned int BluetoothStackID, unsigned int InstanceID, SByte_t Tx_Power_Level)
{
   int                  ret_val;
   TPSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (TPS_TX_POWER_LEVEL_VALID(Tx_Power_Level)))
   {
      /* Acquire the specified TPS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Assign the Tx Power Level for the specified instance.       */
         ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(InstanceData[InstanceID-1].Tx_Power_Level), Tx_Power_Level);

         /* Return success to the caller.                               */
         ret_val = 0;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = TPS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = TPS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for querying the current    */
   /* Tx Power Level on the specified TPS Instance.  The first          */
   /* parameter is the Bluetooth Stack ID of the Bluetooth Device.  The */
   /* second parameter is the InstanceID returned from a successful call*/
   /* to TPS_Initialize_Server().  The final parameter is a pointer to  */
   /* return the current Tx Power Level for the specified TPS           */
   /* Instance.  This function returns a zero if successful or a        */
   /* negative return error code if an error occurs.                    */
int BTPSAPI TPS_Query_Tx_Power_Level(unsigned int BluetoothStackID, unsigned int InstanceID, SByte_t *Tx_Power_Level)
{

   int                  ret_val;
   TPSServerInstance_t *ServiceInstance;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (Tx_Power_Level))
   {
      /* Acquire the specified TPS Instance.                            */
      if((ServiceInstance = AcquireServiceInstance(BluetoothStackID, &InstanceID)) != NULL)
      {
         /* Query the current Tx Power Level.                           */
         *Tx_Power_Level = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(InstanceData[InstanceID-1].Tx_Power_Level));

         /* Return success to the caller.                               */
         ret_val           = 0;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInstance->BluetoothStackID);
      }
      else
         ret_val = TPS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = TPS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}
