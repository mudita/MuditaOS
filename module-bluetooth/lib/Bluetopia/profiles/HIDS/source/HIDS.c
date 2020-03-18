/*****< hids.c >***************************************************************/
/*      Copyright 2012 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  HIDS - Bluetooth Stack Human Interface Device Service (GATT Based) for    */
/*         Stonestreet One Bluetooth Protocol Stack.                          */
/*                                                                            */
/*  Author:  Tim Cook                                                         */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   10/24/12  T. Cook        Initial Creation.                               */
/******************************************************************************/
#include "SS1BTPS.h"        /* Bluetooth Stack API Prototypes/Constants.      */
#include "SS1BTGAT.h"       /* Bluetooth Stack GATT API Prototypes/Constants. */
#include "SS1BTHIDS.h"      /* Bluetooth HIDS API Prototypes/Constants.       */

#include "BTPSKRNL.h"       /* BTPS Kernel Prototypes/Constants.              */
#include "HIDS.h"           /* Bluetooth HIDS Prototypes/Constants.           */

   /* The following define the characteristic types that we will use    */
   /* internally.                                                       */
typedef enum
{
   ictReport                   = rtReport,
   ictBootKeyboardInputReport  = rtBootKeyboardInputReport,
   ictBootKeyboardOutputReport = rtBootKeyboardOutputReport,
   ictBootMouseInputReport     = rtBootMouseInputReport,
   ictProtocolMode,
   ictReportMap,
   ictControlPoint
} HIDS_Internal_Characteristic_Type_t;

   /*********************************************************************/
   /**               Human Interface Device Service Table              **/
   /*********************************************************************/

   /* The Human Interface Device Service Declaration UUID.              */
static BTPSCONST GATT_Primary_Service_16_Entry_t HIDS_Service_UUID =
{
   HIDS_SERVICE_BLUETOOTH_UUID_CONSTANT
};

   /* The HID Information Characteristic Declaration.                   */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t HIDS_HID_Information_Declaration =
{
   GATT_CHARACTERISTIC_PROPERTIES_READ,
   HIDS_HID_INFORMATION_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The Temperature Type Characteristic Declaration.                  */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t HIDS_HID_Control_Point_Declaration =
{
   GATT_CHARACTERISTIC_PROPERTIES_WRITE_WITHOUT_RESPONSE,
   HIDS_HID_CONTROL_POINT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The Temperature Type Characteristic Value.                        */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t  HIDS_HID_Control_Point_Value =
{
   HIDS_HID_CONTROL_POINT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   ictControlPoint,
   NULL
};

   /* The Report Map Characteristic Declaration.                        */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t HIDS_Report_Map_Declaration =
{
   GATT_CHARACTERISTIC_PROPERTIES_READ,
   HIDS_REPORT_MAP_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The Report Map Value.                                             */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t HIDS_Report_Map_Value =
{
   HIDS_REPORT_MAP_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   ictReportMap,
   NULL
};

   /* The Input Report Characteristic Declaration.                      */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t HIDS_Input_Report_Declaration_Readable =
{
   (GATT_CHARACTERISTIC_PROPERTIES_READ|GATT_CHARACTERISTIC_PROPERTIES_NOTIFY),
   HIDS_REPORT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t HIDS_Input_Report_Declaration_ReadWrite =
{
   (GATT_CHARACTERISTIC_PROPERTIES_READ|GATT_CHARACTERISTIC_PROPERTIES_WRITE|GATT_CHARACTERISTIC_PROPERTIES_NOTIFY),
   HIDS_REPORT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};


   /* The Output Report Characteristic Declaration.                     */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t HIDS_Output_Report_Declaration =
{
   (GATT_CHARACTERISTIC_PROPERTIES_READ|GATT_CHARACTERISTIC_PROPERTIES_WRITE_WITHOUT_RESPONSE|GATT_CHARACTERISTIC_PROPERTIES_WRITE),
   HIDS_REPORT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The Feature Report Characteristic Declaration.                    */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t HIDS_Feature_Report_Declaration =
{
   (GATT_CHARACTERISTIC_PROPERTIES_READ|GATT_CHARACTERISTIC_PROPERTIES_WRITE),
   HIDS_REPORT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The HID Report Value.                                             */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t  HIDS_Report_Value =
{
   HIDS_REPORT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   ictReport,
   NULL
};


   /* The Protocol Mode Characteristic Declaration.                     */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t HIDS_Protocol_Mode_Declaration =
{
   (GATT_CHARACTERISTIC_PROPERTIES_READ|GATT_CHARACTERISTIC_PROPERTIES_WRITE_WITHOUT_RESPONSE),
   HIDS_PROTOCOL_MODE_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The Protocol Mode Characteristic Value.                           */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t  HIDS_Protocol_Mode_Value =
{
   HIDS_PROTOCOL_MODE_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   ictProtocolMode,
   NULL
};

   /* Client Characteristic Configuration Descriptor.                   */
static GATT_Characteristic_Descriptor_16_Entry_t Client_Characteristic_Configuration =
{
   GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_BLUETOOTH_UUID_CONSTANT,
   GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_LENGTH,
   NULL
};

   /* The HID Boot Mouse Input Report Declaration.                      */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t HIDS_Boot_Mouse_Input_Report_Declaration_Read =
{
   (GATT_CHARACTERISTIC_PROPERTIES_READ | GATT_CHARACTERISTIC_PROPERTIES_NOTIFY),
   HIDS_BOOT_MOUSE_INPUT_REPORT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The HID Boot Mouse Input Report Declaration.                      */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t HIDS_Boot_Mouse_Input_Report_Declaration_ReadWrite =
{
   (GATT_CHARACTERISTIC_PROPERTIES_READ | GATT_CHARACTERISTIC_PROPERTIES_WRITE | GATT_CHARACTERISTIC_PROPERTIES_NOTIFY),
   HIDS_BOOT_MOUSE_INPUT_REPORT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The HID Boot Mouse Input Report Value.                            */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t  HIDS_Boot_Mouse_Input_Report_Value =
{
   HIDS_BOOT_MOUSE_INPUT_REPORT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   ictBootMouseInputReport,
   NULL
};

   /* The HID Boot Keyboard Input Report Declaration.                   */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t HIDS_Boot_Keyboard_Input_Report_Declaration_Read =
{
   (GATT_CHARACTERISTIC_PROPERTIES_READ | GATT_CHARACTERISTIC_PROPERTIES_NOTIFY),
   HIDS_BOOT_KEYBOARD_INPUT_REPORT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t HIDS_Boot_Keyboard_Input_Report_Declaration_ReadWrite =
{
   (GATT_CHARACTERISTIC_PROPERTIES_READ | GATT_CHARACTERISTIC_PROPERTIES_WRITE | GATT_CHARACTERISTIC_PROPERTIES_NOTIFY),
   HIDS_BOOT_KEYBOARD_INPUT_REPORT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The HID Boot Keyboard Input Report Value.                         */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t  HIDS_Boot_Keyboard_Input_Report_Value =
{
   HIDS_BOOT_KEYBOARD_INPUT_REPORT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   ictBootKeyboardInputReport,
   NULL
};

   /* The HID Boot Keyboard Output Report Declaration.                  */
static BTPSCONST GATT_Characteristic_Declaration_16_Entry_t HIDS_Boot_Keyboard_Output_Report_Declaration =
{
   (GATT_CHARACTERISTIC_PROPERTIES_READ|GATT_CHARACTERISTIC_PROPERTIES_WRITE_WITHOUT_RESPONSE|GATT_CHARACTERISTIC_PROPERTIES_WRITE),
   HIDS_BOOT_KEYBOARD_OUTPUT_REPORT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT
};

   /* The HID Boot Keyboard Output Report Value.                        */
static BTPSCONST GATT_Characteristic_Value_16_Entry_t  HIDS_Boot_Keyboard_Output_Report_Value =
{
   HIDS_BOOT_KEYBOARD_OUTPUT_REPORT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT,
   ictBootKeyboardOutputReport,
   NULL
};

   /* The following defines the number of Mandatory Attribute Entries.  */
   /* The Minimum entries include the following:                        */
   /*          Primary Service                 1 Entry                  */
   /*          HID Information                 2 Entries                */
   /*          Control Point                   2 Entries                */
   /*          Report Map                      2 Entries                */
#define MINIMUM_NUMBER_ATTRIBUTE_ENTRIES                 7
#define NUMBER_ATTRIBUTE_ENTRIES_PER_INCLUDED_SERVICE    1
#define NUMBER_ATTRIBUTE_ENTRIES_PER_EXTERNAL_REFERENCE  1
#define NUMBER_ATTRIBUTE_ENTRIES_PER_REPORT_REFERENCE    1
#define NUMBER_ATTRIBUTE_ENTRIES_FOR_PROTOCOL_MODE       2
#define NUMBER_ATTRIBUTE_ENTRIES_FOR_MOUSE_SUPPORT       3
#define NUMBER_ATTRIBUTE_ENTRIES_FOR_KEYBOARD_SUPPORT    5
#define NUMBER_ATTRIBUTE_ENTRIES_PER_INPUT_REPORT        4
#define NUMBER_ATTRIBUTE_ENTRIES_PER_OUTPUT_REPORT       3
#define NUMBER_ATTRIBUTE_ENTRIES_PER_FEATURE_REPORT      3

   /*********************************************************************/
   /**                    END OF SERVICE TABLE                         **/
   /*********************************************************************/

   /* The following type defines a union large enough to hold all events*/
   /* dispatched by this module.                                        */
typedef union
{
   HIDS_Read_Client_Configuration_Data_t   HIDS_Read_Client_Configuration_Data;
   HIDS_Client_Configuration_Update_Data_t HIDS_Client_Configuration_Update_Data;
   HIDS_Get_Protocol_Mode_Request_Data_t   HIDS_Get_Protocol_Mode_Request_Data;
   HIDS_Set_Protocol_Mode_Request_Data_t   HIDS_Set_Protocol_Mode_Request_Data;
   HIDS_Get_Report_Map_Request_Data_t      HIDS_Get_Report_Map_Data;
   HIDS_Get_Report_Request_Data_t          HIDS_Get_Report_Request_Data;
   HIDS_Set_Report_Request_Data_t          HIDS_Set_Report_Request_Data;
   HIDS_Control_Point_Command_Data_t       HIDS_Control_Point_Command_Data;
} HIDS_Event_Data_Buffer_t;

#define HIDS_EVENT_DATA_BUFFER_SIZE                      (sizeof(HIDS_Event_Data_Buffer_t))

   /* The following structure defines all of the information that is    */
   /* stored about an External Report Reference Characteristic          */
   /* Descriptor.                                                       */
typedef struct _tagHIDS_External_Report_Reference_t
{
   GATT_Characteristic_Descriptor_16_Entry_t DescriptorAttributeEntry;
   GATT_UUID_t                               UUID;
} HIDS_HID_External_Report_Reference_t;

#define HIDS_HID_EXTERNAL_REPORT_REFERENCE_DATA_SIZE     (sizeof(HIDS_HID_External_Report_Reference_t))

   /* The following structure defines all of the information that is    */
   /* stored about a Report Reference Characteristic Descriptor.        */
typedef struct _tagHIDS_HID_Report_Reference_t
{
   GATT_Characteristic_Descriptor_16_Entry_t DescriptorAttributeEntry;
   HIDS_Report_Reference_t                   ReportReferenceInfo;
} HIDS_HID_Report_Reference_t;

#define HIDS_HID_REPORT_REFERENCE_DATA_SIZE              (sizeof(HIDS_HID_Report_Reference_t))

   /* The following structure defines all of the information that is    */
   /* stored about a registered HID Service.                            */
typedef struct _tagHIDS_Service_Info_t
{
   unsigned int                          InstanceID;
   unsigned int                          BluetoothStackID;
   unsigned int                          ServiceID;
   Word_t                                ReportMapOffset;
   Word_t                                ProtocolModeOffset;
   Word_t                                BootMouseInputReportOffset;
   Word_t                                BootKeyboardInputReportOffset;
   Word_t                                BootKeyboardOutputReportOffset;
   HIDS_HID_Information_t                HIDServiceInformation;
   GATT_Characteristic_Value_16_Entry_t  HIDServerInformationValue;
   unsigned int                          NumberIncludedServices;
   GATT_Include_Definition_Entry_t      *IncludedServiceList;
   unsigned int                          NumExternalReferences;
   HIDS_HID_External_Report_Reference_t *ExternalReportReferenceList;
   unsigned int                          NumReports;
   HIDS_HID_Report_Reference_t          *ReportReferenceInfoList;
   unsigned int                          NumberServiceAttributeEntries;
   GATT_Service_Attribute_Entry_t       *ServiceAttributeTable;
   HIDS_Event_Callback_t                 EventCallback;
   unsigned long                         CallbackParameter;
   struct _tagHIDS_Service_Info_t       *NextHIDServiceInfoPtr;
} HIDS_Service_Info_t;

#define HID_SERVICE_INFO_DATA_SIZE                       (sizeof(HIDS_Service_Info_t))

   /* Internal Variables to this Module (Remember that all variables    */
   /* declared static are initialized to 0 automatically by the         */
   /* compiler as part of standard C/C++).                              */

static HIDS_Service_Info_t *HIDServiceInfoList;  /* Variable which holds*/
                                                 /* the First Entry     */
                                                 /* (Head of List) of   */
                                                 /* All currently opened*/
                                                 /* GATT instances.     */

static Boolean_t HIDServiceInfoListInitialized; /* Variable which flags*/
                                                 /* whether or not the  */
                                                 /* GATT module has     */
                                                 /* initialized the GATT*/
                                                 /* Context information */
                                                 /* list and also denote*/
                                                 /* that this module has*/
                                                 /* been succesfully    */
                                                 /* initialized.        */

   /* The following are the prototypes of local functions.              */
static Boolean_t InitializeModule(void);
static void CleanupModule(void);

static unsigned int GetNextInstanceID(void);
static HIDS_Service_Info_t *CreateHIDServiceInfoEntry(HIDS_Service_Info_t **ListHead, unsigned int BluetoothStackID);
static HIDS_Service_Info_t *SearchHIDServiceInfoEntry(HIDS_Service_Info_t **ListHead, unsigned int InstanceID);
static HIDS_Service_Info_t *DeleteHIDServiceInfoEntry(HIDS_Service_Info_t **ListHead, unsigned int InstanceID);
static void FreeHIDServiceInfoEntryMemory(HIDS_Service_Info_t *EntryToFree);
static void FreeHIDServiceInfoEntryList(HIDS_Service_Info_t **ListHead);

static Boolean_t ValueToReportReference(HIDS_Service_Info_t *ServiceInfo, unsigned int AttributeOffset, HIDS_Report_Reference_Data_t *ReportReference);
static Boolean_t ClientConfigurationToType(HIDS_Service_Info_t *ServiceInfo, unsigned int AttributeOffset, HIDS_Internal_Characteristic_Type_t *CharacteristicType);
static Boolean_t ClientConfigurationToReportReference(HIDS_Service_Info_t *ServiceInfo, unsigned int AttributeOffset, HIDS_Report_Reference_Data_t *ReportReference);
static int DecodeClientConfigurationValue(unsigned int BufferLength, Byte_t *Buffer, Word_t *ClientConfiguration);

static unsigned int ReportReferenceToOffset(HIDS_Service_Info_t *ServiceInfo, HIDS_Report_Reference_Data_t *ReportReference);

static int ConfigureHIDServiceInfo(HIDS_Service_Info_t *ServiceInfo, unsigned int NumberIncludedServices, unsigned int NumberExternalReference, unsigned int NumberReportReferenceInfos, unsigned int NumberAttributeEntries);

static HIDS_Event_Data_t *FormatEventHeader(unsigned int BufferLength, Byte_t *Buffer, HIDS_Event_Type_t EventType, unsigned int InstanceID, unsigned int ConnectionID, unsigned int *TransactionID, GATT_Connection_Type_t ConnectionType, BD_ADDR_t *BD_ADDR);

static HIDS_Service_Info_t *AcquireServiceInstance(unsigned int BluetoothStackID, unsigned int InstanceID);

static int HIDSRegisterService(unsigned int BluetoothStackID, Byte_t Flags, HIDS_HID_Information_Data_t *HIDInformation, unsigned int NumIncludedServices, unsigned int *ServiceIDList, unsigned int NumExternalReportReferences, GATT_UUID_t *ReferenceUUID, unsigned int NumReports, HIDS_Report_Reference_Data_t *ReportReference, HIDS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t  *ServiceHandleRange);

   /* Bluetooth Event Callbacks.                                        */
static void BTPSAPI GATT_ServerEventCallback(unsigned int BluetoothStackID, GATT_Server_Event_Data_t *GATT_ServerEventData, unsigned long CallbackParameter);

   /* The following function is a utility function that is used to      */
   /* reduce the ifdef blocks that are needed to handle the difference  */
   /* between module initialization for Threaded and NonThreaded stacks.*/
static Boolean_t InitializeModule(void)
{
   /* All we need to do is flag that we are initialized.                */
   if(!HIDServiceInfoListInitialized)
   {
      HIDServiceInfoListInitialized = TRUE;

     HIDServiceInfoList             = NULL;
   }

   return(TRUE);
}

   /* The following function is a utility function that exists to       */
   /* perform stack specific (threaded versus nonthreaded) cleanup.     */
static void CleanupModule(void)
{
   /* Flag that we are no longer initialized.                           */
   HIDServiceInfoListInitialized = FALSE;
}

   /* The following function generates the Next Instance ID for this    */
   /* Implementation.                                                   */
static unsigned int GetNextInstanceID(void)
{
   static unsigned int NextInstanceID = 0;

   /* Increment the Counter to the next number.  Check the new number to*/
   /* see if it has gone negative.  If so, return to the first valid    */
   /* Number (one).                                                     */
   NextInstanceID++;

   if(((int)NextInstanceID) < 0)
      NextInstanceID = 1;

   /* Simply return the Record Handle Number to the caller.             */
   return(NextInstanceID);
}

   /* The following function adds the specified Entry to the specified  */
   /* List.  This function allocates and adds an entry to the list that */
   /* has the same attributes as the Entry passed into this function.   */
   /* This function will return NULL if NO Entry was added.  This can   */
   /* occur if the element passed in was deemed invalid or the actual   */
   /* List Head was invalid.                                            */
   /* * NOTE * Since this is an internal function no check is performed */
   /*          on the input parameters.                                 */
static HIDS_Service_Info_t *CreateHIDServiceInfoEntry(HIDS_Service_Info_t **ListHead, unsigned int BluetoothStackID)
{
   Boolean_t            Result;
   HIDS_Service_Info_t *EntryToAdd;

   /* OK, data seems semi-valid, let's allocate a new data structure to */
   /* add to the list.                                                  */
   if((EntryToAdd = (HIDS_Service_Info_t *)BTPS_AllocateMemory(HID_SERVICE_INFO_DATA_SIZE)) != NULL)
   {
      /* Initialize the GATT Context.                                   */
      BTPS_MemInitialize(EntryToAdd, 0, HID_SERVICE_INFO_DATA_SIZE);

      EntryToAdd->InstanceID       = GetNextInstanceID();
      EntryToAdd->BluetoothStackID = BluetoothStackID;

      /* Add the GATT Context to the Context List.                      */
      Result = BSC_AddGenericListEntry_Actual(ekUnsignedInteger, BTPS_STRUCTURE_OFFSET(HIDS_Service_Info_t, InstanceID), BTPS_STRUCTURE_OFFSET(HIDS_Service_Info_t, NextHIDServiceInfoPtr), (void **)(ListHead), (void *)(EntryToAdd));
      if(!Result)
      {
         /* An error occurred so free the memory allocated for the      */
         /* context.                                                    */
         BTPS_FreeMemory(EntryToAdd);

         EntryToAdd = NULL;
      }
   }

   /* Return the result to the caller.                                  */
   return(EntryToAdd);
}

   /* The following function searches the specified List for the        */
   /* specified Instance ID.  This function returns NULL if either the  */
   /* List Head is invalid, the Instance ID is invalid, or the specified*/
   /* Instance ID was NOT found.                                        */
static HIDS_Service_Info_t *SearchHIDServiceInfoEntry(HIDS_Service_Info_t **ListHead, unsigned int InstanceID)
{
   return((HIDS_Service_Info_t *)BSC_SearchGenericListEntry(ekUnsignedInteger, (void *)(&InstanceID), BTPS_STRUCTURE_OFFSET(HIDS_Service_Info_t, InstanceID), BTPS_STRUCTURE_OFFSET(HIDS_Service_Info_t, NextHIDServiceInfoPtr), (void **)(ListHead)));
}

   /* The following function searches the specified GATT Context List   */
   /* for the specified Instance ID and removes it from the List.  This */
   /* function returns NULL if either the List Head is invalid, the     */
   /* Instance ID is invalid, or the specified GATT Entry was NOT       */
   /* present in the list.  The entry returned will have the Next Entry */
   /* field set to NULL, and the caller is responsible for deleting the */
   /* memory associated with this entry by calling                      */
   /* FreeHIDServiceInfoEntryMemory().                                  */
static HIDS_Service_Info_t *DeleteHIDServiceInfoEntry(HIDS_Service_Info_t **ListHead, unsigned int InstanceID)
{
   return((HIDS_Service_Info_t *)BSC_DeleteGenericListEntry(ekUnsignedInteger, (void *)(&InstanceID), BTPS_STRUCTURE_OFFSET(HIDS_Service_Info_t, InstanceID), BTPS_STRUCTURE_OFFSET(HIDS_Service_Info_t, NextHIDServiceInfoPtr), (void **)(ListHead)));
}

   /* This function frees the memory associated with the specified HID  */
   /* Service Info member.  No check is done on this entry other than   */
   /* making sure it is NOT NULL.                                       */
   /* * NOTE * Since this is an internal function no check is performed */
   /*          on the input parameters.                                 */
static void FreeHIDServiceInfoEntryMemory(HIDS_Service_Info_t *EntryToFree)
{
   /* Free all memory allocated for this entry.                         */
   if(EntryToFree->IncludedServiceList)
      BTPS_FreeMemory(EntryToFree->IncludedServiceList);

   if(EntryToFree->ReportReferenceInfoList)
      BTPS_FreeMemory(EntryToFree->ReportReferenceInfoList);

   if(EntryToFree->ExternalReportReferenceList)
      BTPS_FreeMemory(EntryToFree->ExternalReportReferenceList);

   if(EntryToFree->ServiceAttributeTable)
      BTPS_FreeMemory(EntryToFree->ServiceAttributeTable);

   BTPS_FreeMemory(EntryToFree);
}

   /* The following function deletes (and free's all memory) every      */
   /* element of the specified List.  Upon return of this function, the */
   /* Head Pointer is set to NULL.                                      */
static void FreeHIDServiceInfoEntryList(HIDS_Service_Info_t **ListHead)
{
   HIDS_Service_Info_t *EntryToFree;
   HIDS_Service_Info_t *tmpEntry;

   if(ListHead)
   {
      /* Simply traverse the list and free every element present.       */
      EntryToFree = *ListHead;

      while(EntryToFree)
      {
         tmpEntry    = EntryToFree;
         EntryToFree = EntryToFree->NextHIDServiceInfoPtr;

         FreeHIDServiceInfoEntryMemory(tmpEntry);
      }

      /* Make sure the List appears to be empty.                        */
      *ListHead = NULL;
   }
}

   /* The following function is a utility function that is used to      */
   /* convert a CCCD Descriptor at the specified offset into a          */
   /* Characteristic Type.                                              */
   /* * NOTE * Since this is an internal function no check is performed */
   /*          on the input parameters.                                 */
static Boolean_t ClientConfigurationToType(HIDS_Service_Info_t *ServiceInfo, unsigned int AttributeOffset, HIDS_Internal_Characteristic_Type_t *CharacteristicType)
{
   Boolean_t                             ret_val;
   unsigned int                          Index;
   HIDS_Internal_Characteristic_Type_t   Temp;
   GATT_Characteristic_Value_16_Entry_t *Characteristic;

   for(Index=AttributeOffset,ret_val=FALSE;(Index>1)&&(!ret_val);Index--)
   {
      switch(ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type)
      {
         case aetCharacteristicDescriptor128:
         case aetCharacteristicDescriptor16:
            /* Nothing to do continue.                                  */
            break;
         case aetCharacteristicValue16:
            /* Check to see if the characteristic type is specified.    */
            Characteristic = (GATT_Characteristic_Value_16_Entry_t *)ServiceInfo->ServiceAttributeTable[Index].Attribute_Value;
            if(Characteristic)
            {
               /* Verify that the structure is valid for holding a      */
               /* characteristic type.                                  */
               if(Characteristic->Characteristic_Value == NULL)
               {
                  /* Verify that the characteristic type is valid to    */
                  /* have a CCCD.                                       */
                  Temp = (HIDS_Internal_Characteristic_Type_t)Characteristic->Characteristic_Value_Length;
                  if((Temp == ictReport) || (Temp == ictBootKeyboardInputReport) || (Temp == ictBootMouseInputReport))
                  {
                     /* Return the characteristic type to the caller.   */
                     *CharacteristicType = Temp;

                     ret_val             = TRUE;
                  }
                  else
                  {
                     /* Force us to exit loop next iteration.           */
                     Index = 1;
                  }
               }
               else
               {
                  /* Force us to exit loop next iteration.              */
                  Index = 1;
               }
            }
            else
            {
               /* Force us to exit loop next iteration.                 */
               Index = 1;
            }
            break;
         default:
            /* Force us to exit loop next iteration.                    */
            Index = 1;
            break;
       }
   }

   return(ret_val);
}

   /* The following function is a utility function that is used to      */
   /* convert a Characteristic Value at the specified offset into a     */
   /* Report Reference Structure.                                       */
   /* * NOTE * Since this is an internal function no check is performed */
   /*          on the input parameters.                                 */
static Boolean_t ValueToReportReference(HIDS_Service_Info_t *ServiceInfo, unsigned int AttributeOffset, HIDS_Report_Reference_Data_t *ReportReference)
{
   Boolean_t    ret_val;
   unsigned int Index;

   for(Index=AttributeOffset+1,ret_val=FALSE;Index<ServiceInfo->NumberServiceAttributeEntries;Index++)
   {
      /* Verify that we have not reached a characteristic or include    */
      /* attribute (we must be on the descriptors).                     */
      if((ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type == aetCharacteristicDescriptor16) || (ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type == aetCharacteristicDescriptor128))
      {
         /* Verify that this is a 16 bit UUID Descriptor (as Report     */
         /* Reference has a 16 bit UUID).                               */
         if(ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type == aetCharacteristicDescriptor16)
         {
            /* Verify that this is a Report Reference Descriptor.       */
            if(HIDS_COMPARE_REPORT_REFERENCE_DESCRIPTOR_UUID_TO_UUID_16(((GATT_Characteristic_Descriptor_16_Entry_t *)ServiceInfo->ServiceAttributeTable[Index].Attribute_Value)->Characteristic_Descriptor_UUID))
            {
               /* Get the Report ID and Report Type.                    */
               ReportReference->ReportID   = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(((HIDS_Report_Reference_t *)(((GATT_Characteristic_Descriptor_16_Entry_t *)ServiceInfo->ServiceAttributeTable[Index].Attribute_Value)->Characteristic_Descriptor))->Report_ID));
               ReportReference->ReportType = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(((HIDS_Report_Reference_t *)(((GATT_Characteristic_Descriptor_16_Entry_t *)ServiceInfo->ServiceAttributeTable[Index].Attribute_Value)->Characteristic_Descriptor))->Report_Type));

               /* Return success to the caller.                         */
               ret_val                     = TRUE;
               break;
            }
         }
      }
      else
         break;
   }

   return(ret_val);
}

   /* The following function is a utility function that is used to      */
   /* convert a CCCD Descriptor at the specified offset into a Report   */
   /* Reference Structure.                                              */
   /* * NOTE * Since this is an internal function no check is performed */
   /*          on the input parameters.                                 */
static Boolean_t ClientConfigurationToReportReference(HIDS_Service_Info_t *ServiceInfo, unsigned int AttributeOffset, HIDS_Report_Reference_Data_t *ReportReference)
{
   Boolean_t    ret_val;
   unsigned int Index;

   for(Index=AttributeOffset,ret_val=FALSE;Index>1;Index--)
   {
      /* Verify that we have not reached a characteristic or include    */
      /* attribute (we must be on the descriptors).                     */
      if((ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type == aetCharacteristicDescriptor16) || (ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type == aetCharacteristicDescriptor128))
      {
         /* Verify that this is a 16 bit UUID Descriptor (as Report     */
         /* Reference has a 16 bit UUID).                               */
         if(ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type == aetCharacteristicDescriptor16)
         {
            /* Verify that this is a Report Reference Descriptor.       */
            if(HIDS_COMPARE_REPORT_REFERENCE_DESCRIPTOR_UUID_TO_UUID_16(((GATT_Characteristic_Descriptor_16_Entry_t *)ServiceInfo->ServiceAttributeTable[Index].Attribute_Value)->Characteristic_Descriptor_UUID))
            {
               /* Get the Report ID and Report Type.                    */
               ReportReference->ReportID   = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(((HIDS_Report_Reference_t *)(((GATT_Characteristic_Descriptor_16_Entry_t *)ServiceInfo->ServiceAttributeTable[Index].Attribute_Value)->Characteristic_Descriptor))->Report_ID));
               ReportReference->ReportType = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(((HIDS_Report_Reference_t *)(((GATT_Characteristic_Descriptor_16_Entry_t *)ServiceInfo->ServiceAttributeTable[Index].Attribute_Value)->Characteristic_Descriptor))->Report_Type));

               /* Return success to the caller.                         */
               ret_val                     = TRUE;
               break;
            }
         }
      }
      else
         break;
   }

   return(ret_val);
}

   /* The following function is a utility function that exists to decode*/
   /* an Client Configuration value into a user specified boolean value.*/
   /* This function returns the zero if successful or a negative error  */
   /* code.                                                             */
static int DecodeClientConfigurationValue(unsigned int BufferLength, Byte_t *Buffer, Word_t *ClientConfiguration)
{
   int ret_val = HIDS_ERROR_MALFORMATTED_DATA;

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
         ret_val = HIDS_ERROR_INVALID_PARAMETER;
   }

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is a utility function which is used to     */
   /* convert a ReportReference structure (with Report ID, Report Type) */
   /* into a AttributeOffset for the specified HID Service Instance.    */
   /* This function returns a positive non-zero value on success or ZERO*/
   /* on failure.                                                       */
   /* * NOTE * Since this is an internal function no check is performed */
   /*          on the input parameters.                                 */
static unsigned int ReportReferenceToOffset(HIDS_Service_Info_t *ServiceInfo, HIDS_Report_Reference_Data_t *ReportReference)
{
   Byte_t                                     ReportID;
   Byte_t                                     ReportType;
   unsigned int                               Index;
   unsigned int                               Offset = 0;
   GATT_Characteristic_Value_16_Entry_t      *CharacteristicValue;
   GATT_Characteristic_Descriptor_16_Entry_t *DescriptorValue;

   for(Index=2;Index<ServiceInfo->NumberServiceAttributeEntries;)
   {
      /* Verify that this is a characteristic value.                    */
      if(ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type == aetCharacteristicValue16)
      {
         /* Get a pointer to the characteristic value structure.        */
         CharacteristicValue = (GATT_Characteristic_Value_16_Entry_t *)ServiceInfo->ServiceAttributeTable[Index].Attribute_Value;

         /* Check to see if this is a HID Report characteristic.        */
         if(HIDS_COMPARE_HIDS_REPORT_UUID_TO_UUID_16(CharacteristicValue->Characteristic_Value_UUID))
         {
            /* Save the offset in the table of this Report              */
            /* Characteristic.                                          */
            Offset = Index++;

            /* The next attribute must be a descriptor or else an error */
            /* has occurred.                                            */
            if((Index<ServiceInfo->NumberServiceAttributeEntries) && (ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type == aetCharacteristicDescriptor16))
            {
               /* Get a pointer to the descriptor.                      */
               DescriptorValue = (GATT_Characteristic_Descriptor_16_Entry_t *)ServiceInfo->ServiceAttributeTable[Index].Attribute_Value;

               /* Verify that this is a report reference characteristic.*/
               if(HIDS_COMPARE_REPORT_REFERENCE_DESCRIPTOR_UUID_TO_UUID_16(DescriptorValue->Characteristic_Descriptor_UUID))
               {
                  /* Get a pointer to the descriptor value.             */
                  if(DescriptorValue->Characteristic_Descriptor)
                  {
                     /* Get the Report ID and Report Type of this Report*/
                     /* Reference Descriptor.                           */
                     ReportID   = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(((HIDS_Report_Reference_t *)DescriptorValue->Characteristic_Descriptor)->Report_ID));
                     ReportType = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(((HIDS_Report_Reference_t *)DescriptorValue->Characteristic_Descriptor)->Report_Type));

                     /* Check to see if the Report ID and Report Type   */
                     /* match the passed in values.                     */
                     if((ReportID == ReportReference->ReportID) && (ReportType == ReportReference->ReportType))
                     {
                        /* We have found the correct characteristic     */
                        /* value so just exit the loop.                 */
                        break;
                     }
                     else
                     {
                        /* Not a match so continue searching.           */
                        Offset = 0;
                        continue;
                     }
                  }
                  else
                  {
                     Offset = 0;
                     continue;
                  }
               }
               else
               {
                  Offset = 0;
                  continue;
               }
            }
            else
            {
               Offset = 0;
               continue;
            }
         }
      }

      /* Increment to the next entry in the table.                      */
      ++Index;
   }

   return(Offset);
}

   /* The following function is a utility function that is used to      */
   /* allocate and configure the HID Service Info entry that is         */
   /* specified by the first parameter.  This function returns ZERO on  */
   /* success or a negative error code.                                 */
   /* * NOTE * Since this is an internal function no check is performed */
   /*          on the input parameters.                                 */
   /* * NOTE * If this function returns an error it will delete the     */
   /*          specified Service Info from the HID Service Info List.   */
static int ConfigureHIDServiceInfo(HIDS_Service_Info_t *ServiceInfo, unsigned int NumberIncludedServices, unsigned int NumberExternalReference, unsigned int NumberReportReference, unsigned int NumberAttributeEntries)
{
   int ret_val = 0;

   /* Allocate the required Number of Included Services.                */
   if(NumberIncludedServices)
   {
      ServiceInfo->NumberIncludedServices = NumberIncludedServices;
      ServiceInfo->IncludedServiceList    = (GATT_Include_Definition_Entry_t *)BTPS_AllocateMemory(NumberIncludedServices*GATT_INCLUDE_DEFINITION_ENTRY_DATA_SIZE);
      if(ServiceInfo->IncludedServiceList == NULL)
         ret_val = HIDS_ERROR_INSUFFICIENT_RESOURCES;
   }
   else
      ServiceInfo->IncludedServiceList = NULL;

   /* Allocate memory for any External Report Reference Descriptors.    */
   if((!ret_val) && (NumberExternalReference))
   {
      ServiceInfo->NumExternalReferences       = NumberExternalReference;
      ServiceInfo->ExternalReportReferenceList = (HIDS_HID_External_Report_Reference_t *)BTPS_AllocateMemory(NumberExternalReference*HIDS_HID_EXTERNAL_REPORT_REFERENCE_DATA_SIZE);
      if(ServiceInfo->ExternalReportReferenceList == NULL)
         ret_val = HIDS_ERROR_INSUFFICIENT_RESOURCES;
   }
   else
      ServiceInfo->ExternalReportReferenceList = NULL;

   /* Allocate memory for any Report Reference Descriptors.             */
   if((!ret_val) && (NumberReportReference))
   {
      ServiceInfo->NumReports              = NumberReportReference;
      ServiceInfo->ReportReferenceInfoList = (HIDS_HID_Report_Reference_t *)BTPS_AllocateMemory(NumberReportReference*HIDS_HID_REPORT_REFERENCE_DATA_SIZE);
      if(ServiceInfo->ReportReferenceInfoList == NULL)
         ret_val = HIDS_ERROR_INSUFFICIENT_RESOURCES;
   }
   else
      ServiceInfo->ReportReferenceInfoList = NULL;

   /* Now allocate memory for the attribute table.                      */
   if(!ret_val)
   {
      ServiceInfo->NumberServiceAttributeEntries = NumberAttributeEntries;
      ServiceInfo->ServiceAttributeTable         = (GATT_Service_Attribute_Entry_t *)BTPS_AllocateMemory(ServiceInfo->NumberServiceAttributeEntries*GATT_SERVICE_ATTRIBUTE_ENTRY_DATA_SIZE);
      if(ServiceInfo->ServiceAttributeTable == NULL)
         ret_val = HIDS_ERROR_INSUFFICIENT_RESOURCES;
   }

   /* If an error occurred delete the entry from the list and free the  */
   /* allocated memory.                                                 */
   if(ret_val < 0)
   {
      /* Delete the HID Service Info entry and free the allocated       */
      /* memory.                                                        */
      if((ServiceInfo = DeleteHIDServiceInfoEntry(&HIDServiceInfoList, ServiceInfo->InstanceID)) != NULL)
         FreeHIDServiceInfoEntryMemory(ServiceInfo);
   }

   return(ret_val);
}

   /* The following function is a utility function that exists to format*/
   /* a HIDS Event into the specified buffer.                           */
   /* * NOTE * TransactionID is optional and may be set to NULL.        */
   /* * NOTE * BD_ADDR is NOT optional and may NOT be set to NULL.      */
static HIDS_Event_Data_t *FormatEventHeader(unsigned int BufferLength, Byte_t *Buffer, HIDS_Event_Type_t EventType, unsigned int InstanceID, unsigned int ConnectionID, unsigned int *TransactionID, GATT_Connection_Type_t ConnectionType, BD_ADDR_t *BD_ADDR)
{
   HIDS_Event_Data_t *EventData = NULL;

   if((BufferLength >= (HIDS_EVENT_DATA_SIZE + HIDS_EVENT_DATA_BUFFER_SIZE)) && (Buffer) && (BD_ADDR))
   {
      /* Format the header of the event, that is data that is common to */
      /* all events.                                                    */
      BTPS_MemInitialize(Buffer, 0, BufferLength);

      EventData                                                               = (HIDS_Event_Data_t *)Buffer;
      EventData->Event_Data_Type                                              = EventType;
      EventData->Event_Data.HIDS_Read_Client_Configuration_Data               = (HIDS_Read_Client_Configuration_Data_t *)(((Byte_t *)EventData) + HIDS_EVENT_DATA_SIZE);

      EventData->Event_Data.HIDS_Read_Client_Configuration_Data->InstanceID   = InstanceID;
      EventData->Event_Data.HIDS_Read_Client_Configuration_Data->ConnectionID = ConnectionID;

      if(TransactionID)
      {
         EventData->Event_Data.HIDS_Read_Client_Configuration_Data->TransactionID  = *TransactionID;
         EventData->Event_Data.HIDS_Read_Client_Configuration_Data->ConnectionType = ConnectionType;
         EventData->Event_Data.HIDS_Read_Client_Configuration_Data->RemoteDevice   = *BD_ADDR;
      }
      else
      {
         EventData->Event_Data.HIDS_Client_Configuration_Update_Data->ConnectionType = ConnectionType;
         EventData->Event_Data.HIDS_Client_Configuration_Update_Data->RemoteDevice   = *BD_ADDR;
      }
   }

   /* Finally return the result to the caller.                          */
   return(EventData);
}

   /* The following function is a utility function that exists to       */
   /* acquire a specified service instance.                             */
   /* * NOTE * Since this is an internal function no check is done on   */
   /*          the input parameters.                                    */
static HIDS_Service_Info_t *AcquireServiceInstance(unsigned int BluetoothStackID, unsigned int InstanceID)
{
   HIDS_Service_Info_t *ret_val = NULL;

   /* Lock the Bluetooth Stack to gain exclusive access to this         */
   /* Bluetooth Protocol Stack.                                         */
   if(!BSC_LockBluetoothStack(BluetoothStackID))
   {
      /* Acquire the BSC List Lock while we are searching the instance  */
      /* list.                                                          */
      if(BSC_AcquireListLock())
      {
         /* Search the list for the specified HID Service Instance.     */
         ret_val = SearchHIDServiceInfoEntry(&HIDServiceInfoList, InstanceID);

         /* Release the previously acquired list lock.                  */
         BSC_ReleaseListLock();
      }

      /* If we failed to acquire the instance then we should un-lock the*/
      /* previously acquired Bluetooth Stack.                           */
      if(ret_val == NULL)
         BSC_UnLockBluetoothStack(BluetoothStackID);
   }

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is a utility function which is used to     */
   /* register an HIDS Service.  This function returns the positive,    */
   /* non-zero, Instance ID on success or a negative error code.        */
static int HIDSRegisterService(unsigned int BluetoothStackID, Byte_t Flags, HIDS_HID_Information_Data_t *HIDInformation, unsigned int NumIncludedServices, unsigned int *ServiceIDList, unsigned int NumExternalReportReferences, GATT_UUID_t *ReferenceUUID, unsigned int NumReports, HIDS_Report_Reference_Data_t *ReportReference, HIDS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t  *ServiceHandleRange)
{
   int                  ret_val;
   Byte_t               ReportType;
   Boolean_t            InputReportWritable;
   unsigned int         i;
   unsigned int         Index;
   unsigned int         NumberAttributeEntries;
   HIDS_Service_Info_t *ServiceInfo;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (HIDInformation) && ((NumIncludedServices == 0) || ((NumIncludedServices) && (ServiceIDList))) && ((NumExternalReportReferences == 0) || ((NumExternalReportReferences) && (ReferenceUUID))) && ((NumReports == 0) || ((NumReports) && (ReportReference))) && (EventCallback) && (ServiceID))
   {
      /* Lock the Bluetooth Stack to gain exclusive access to this      */
      /* Bluetooth Protocol Stack.                                      */
      if(!BSC_LockBluetoothStack(BluetoothStackID))
      {
         /* Determine the number of Attribute Entries that will be      */
         /* required.                                                   */
         NumberAttributeEntries  = MINIMUM_NUMBER_ATTRIBUTE_ENTRIES;
         NumberAttributeEntries += (NUMBER_ATTRIBUTE_ENTRIES_PER_INCLUDED_SERVICE * NumIncludedServices);
         NumberAttributeEntries += (NUMBER_ATTRIBUTE_ENTRIES_PER_EXTERNAL_REFERENCE * NumExternalReportReferences);
         for(i=0,ret_val=0;(i<NumReports)&&(!ret_val);i++)
         {
            if(ReportReference[i].ReportType == HIDS_REPORT_REFERENCE_REPORT_TYPE_INPUT_REPORT)
               NumberAttributeEntries += NUMBER_ATTRIBUTE_ENTRIES_PER_INPUT_REPORT;
            else
            {
               if(ReportReference[i].ReportType == HIDS_REPORT_REFERENCE_REPORT_TYPE_OUTPUT_REPORT)
                  NumberAttributeEntries += NUMBER_ATTRIBUTE_ENTRIES_PER_OUTPUT_REPORT;
               else
               {
                  if(ReportReference[i].ReportType == HIDS_REPORT_REFERENCE_REPORT_TYPE_FEATURE_REPORT)
                     NumberAttributeEntries += NUMBER_ATTRIBUTE_ENTRIES_PER_FEATURE_REPORT;
                  else
                     ret_val = HIDS_ERROR_INVALID_PARAMETER;
               }
            }
         }

         /* Continue only if no error has occurred.                     */
         if(!ret_val)
         {
            /* Check to see if this is a Keyboard or Mouse device.      */
            if(Flags & (HIDS_FLAGS_SUPPORT_MOUSE | HIDS_FLAGS_SUPPORT_KEYBOARD))
            {
               /* Protocol Mode Entry is required.                      */
               NumberAttributeEntries += NUMBER_ATTRIBUTE_ENTRIES_FOR_PROTOCOL_MODE;

               /* Account for Keyboard and Mouse entries.               */
               if(Flags & HIDS_FLAGS_SUPPORT_MOUSE)
                 NumberAttributeEntries += NUMBER_ATTRIBUTE_ENTRIES_FOR_MOUSE_SUPPORT;

               if(Flags & HIDS_FLAGS_SUPPORT_KEYBOARD)
                 NumberAttributeEntries += NUMBER_ATTRIBUTE_ENTRIES_FOR_KEYBOARD_SUPPORT;
            }

            /* Wait for Access to the HID Service Info List.            */
            if(BSC_AcquireListLock())
            {
               /* Create the HID Service Info entry for this HID        */
               /* Service.                                              */
               ServiceInfo = CreateHIDServiceInfoEntry(&HIDServiceInfoList, BluetoothStackID);
               if(ServiceInfo)
               {
                  /* Allocate all of the required memory for the list   */
                  /* entries and do some configuration.                 */
                  if(!(ret_val = ConfigureHIDServiceInfo(ServiceInfo, NumIncludedServices, NumExternalReportReferences, NumReports, NumberAttributeEntries)))
                  {
                     /* Add the Specified HID Information into the HID  */
                     /* Information Structure.                          */
                     ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&(ServiceInfo->HIDServiceInformation.HID_Version), HIDInformation->Version);
                     ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(ServiceInfo->HIDServiceInformation.CountryCode), HIDInformation->CountryCode);
                     ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(ServiceInfo->HIDServiceInformation.Flags), HIDInformation->Flags);

                     /* Start building the attribute table.             */
                     Index = 0;

                     /* Add the Primary Service Declaration.            */
                     ServiceInfo->ServiceAttributeTable[Index].Attribute_Flags      = GATT_ATTRIBUTE_FLAGS_READABLE;
                     ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type = aetPrimaryService16;
                     ServiceInfo->ServiceAttributeTable[Index].Attribute_Value      = (Byte_t *)&HIDS_Service_UUID;
                     ++Index;

                     /* Add all of the Included Services to the list.   */
                     for(i=0;i<NumIncludedServices;i++,Index++)
                     {
                        /* Format the Service ID of the Included        */
                        /* Service.                                     */
                        ServiceInfo->IncludedServiceList[i].ServiceID                  = ServiceIDList[i];

                        /* Format Service Table entry for the Included  */
                        /* Service in the Service Table.                */
                        ServiceInfo->ServiceAttributeTable[Index].Attribute_Flags      = GATT_ATTRIBUTE_FLAGS_READABLE;
                        ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type = aetIncludeDefinition;
                        ServiceInfo->ServiceAttributeTable[Index].Attribute_Value      = (Byte_t *)&(ServiceInfo->IncludedServiceList[i]);
                        Index++;
                     }

                     /* Add the HID Information Characteristic.         */
                     ServiceInfo->ServiceAttributeTable[Index].Attribute_Flags        = GATT_ATTRIBUTE_FLAGS_READABLE;
                     ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type   = aetCharacteristicDeclaration16;
                     ServiceInfo->ServiceAttributeTable[Index].Attribute_Value        = (Byte_t *)&HIDS_HID_Information_Declaration;
                     ++Index;

                     ServiceInfo->ServiceAttributeTable[Index].Attribute_Flags        = GATT_ATTRIBUTE_FLAGS_READABLE;
                     ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type   = aetCharacteristicValue16;
                     ServiceInfo->ServiceAttributeTable[Index].Attribute_Value        = (Byte_t *)&(ServiceInfo->HIDServerInformationValue);
                     ++Index;

                     HIDS_ASSIGN_HID_INFORMATION_UUID_16(ServiceInfo->HIDServerInformationValue.Characteristic_Value_UUID);

                     ServiceInfo->HIDServerInformationValue.Characteristic_Value_Length = HIDS_HID_INFORMATION_SIZE;
                     ServiceInfo->HIDServerInformationValue.Characteristic_Value        = (Byte_t *)&(ServiceInfo->HIDServiceInformation);

                     /* Add the HID Control Point Characteristic.       */
                     ServiceInfo->ServiceAttributeTable[Index].Attribute_Flags      = GATT_ATTRIBUTE_FLAGS_READABLE;
                     ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type = aetCharacteristicDeclaration16;
                     ServiceInfo->ServiceAttributeTable[Index].Attribute_Value      = (Byte_t *)&HIDS_HID_Control_Point_Declaration;
                     ++Index;

                     ServiceInfo->ServiceAttributeTable[Index].Attribute_Flags      = GATT_ATTRIBUTE_FLAGS_WRITABLE;
                     ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type = aetCharacteristicValue16;
                     ServiceInfo->ServiceAttributeTable[Index].Attribute_Value      = (Byte_t *)&HIDS_HID_Control_Point_Value;
                     ++Index;

                     /* Add the HID Report Map Characteristic.          */
                     ServiceInfo->ServiceAttributeTable[Index].Attribute_Flags      = GATT_ATTRIBUTE_FLAGS_READABLE;
                     ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type = aetCharacteristicDeclaration16;
                     ServiceInfo->ServiceAttributeTable[Index].Attribute_Value      = (Byte_t *)&HIDS_Report_Map_Declaration;
                     ++Index;

                     ServiceInfo->ServiceAttributeTable[Index].Attribute_Flags      = GATT_ATTRIBUTE_FLAGS_READABLE;
                     ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type = aetCharacteristicValue16;
                     ServiceInfo->ServiceAttributeTable[Index].Attribute_Value      = (Byte_t *)&HIDS_Report_Map_Value;

                     /* Save the offset of the Report Map Characteristic*/
                     /* Value in the Service Table.                     */
                     ServiceInfo->ReportMapOffset                                   = (Word_t)Index;

                     /* Increment the index past the Report Map         */
                     /* Characteristic Value.                           */
                     ++Index;

                     /* Add the HID External Report Reference           */
                     /* Characteristic Descriptors to the HID Report Map*/
                     /* Characteristic.                                 */
                     for(i=0;i<NumExternalReportReferences;i++,Index++)
                     {
                        /* Format an entry in the Service Table for this*/
                        /* External Report Reference Descriptor.        */
                        ServiceInfo->ServiceAttributeTable[Index].Attribute_Flags                                             = GATT_ATTRIBUTE_FLAGS_READABLE;
                        ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type                                        = aetCharacteristicDescriptor16;
                        ServiceInfo->ServiceAttributeTable[Index].Attribute_Value                                             = (Byte_t *)&(ServiceInfo->ExternalReportReferenceList[i].DescriptorAttributeEntry);

                        /* Format the UUID of this External Report      */
                        /* Reference UUID.                              */
                        ServiceInfo->ExternalReportReferenceList[i].UUID                                                      = ReferenceUUID[i];

                        /* Format the attribute table entry for the     */
                        /* External Report Reference Descriptor.        */
                        HIDS_ASSIGN_EXTERNAL_REPORT_REFERENCE_DESCRIPTOR_UUID(ServiceInfo->ExternalReportReferenceList[i].DescriptorAttributeEntry.Characteristic_Descriptor_UUID);
                        ServiceInfo->ExternalReportReferenceList[i].DescriptorAttributeEntry.Characteristic_Descriptor_Length = (ReferenceUUID[i].UUID_Type == guUUID_16)?UUID_16_SIZE:UUID_128_SIZE;
                        ServiceInfo->ExternalReportReferenceList[i].DescriptorAttributeEntry.Characteristic_Descriptor        = (Byte_t *)&(ServiceInfo->ExternalReportReferenceList[i].UUID.UUID.UUID_16);
                     }

                     /* Add the requested Report Characteristics.       */
                     for(i=0;i<NumReports;i++)
                     {
                        /* Cache the report type.                       */
                        ReportType            = ReportReference[i].ReportType;

                        /* Check to see if this report (if an Input     */
                        /* Report) may be writable.                     */
                        InputReportWritable   = (Boolean_t)((ReportType & HIDS_REPORT_REFERENCE_SUPPORT_INPUT_WRITE_FEATURE)?TRUE:FALSE);

                        /* Clear any Invalid Bits set in the report     */
                        /* type.                                        */
                        ReportType           &= (Byte_t)~HIDS_REPORT_REFERENCE_SUPPORT_INPUT_WRITE_FEATURE;

                        /* Format the Report Characteristic Declaration */
                        /* based on the report type.                    */
                        if(ReportType == HIDS_REPORT_REFERENCE_REPORT_TYPE_INPUT_REPORT)
                        {
                           ServiceInfo->ServiceAttributeTable[Index].Attribute_Flags      = GATT_ATTRIBUTE_FLAGS_READABLE;
                           ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type = aetCharacteristicDeclaration16;
                           ServiceInfo->ServiceAttributeTable[Index].Attribute_Value      = (InputReportWritable)?(Byte_t *)&HIDS_Input_Report_Declaration_ReadWrite:(Byte_t *)&HIDS_Input_Report_Declaration_Readable;
                        }
                        else
                        {
                           if(ReportType == HIDS_REPORT_REFERENCE_REPORT_TYPE_OUTPUT_REPORT)
                           {
                              ServiceInfo->ServiceAttributeTable[Index].Attribute_Flags      = GATT_ATTRIBUTE_FLAGS_READABLE;
                              ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type = aetCharacteristicDeclaration16;
                              ServiceInfo->ServiceAttributeTable[Index].Attribute_Value      = (Byte_t *)&HIDS_Output_Report_Declaration;
                           }
                           else
                           {
                              if(ReportType == HIDS_REPORT_REFERENCE_REPORT_TYPE_FEATURE_REPORT)
                              {
                                 ServiceInfo->ServiceAttributeTable[Index].Attribute_Flags      = GATT_ATTRIBUTE_FLAGS_READABLE;
                                 ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type = aetCharacteristicDeclaration16;
                                 ServiceInfo->ServiceAttributeTable[Index].Attribute_Value      = (Byte_t *)&HIDS_Feature_Report_Declaration;
                              }
                              else
                              {
                                 ret_val = HIDS_ERROR_INVALID_PARAMETER;
                                 break;
                              }
                           }
                        }

                        /* Increment the index past the Report          */
                        /* Characteristic Declaration.                  */
                        ++Index;

                        /* Format the entry in the Service Table for the*/
                        /* Report Characteristic Value.                 */
                        ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type = aetCharacteristicValue16;
                        ServiceInfo->ServiceAttributeTable[Index].Attribute_Value      = (Byte_t *)&HIDS_Report_Value;

                        /* Ensure that the attribute properties for the */
                        /* Report Characteristic Value are set          */
                        /* correctly.  All reports are readable and     */
                        /* writable with the exception of the Input     */
                        /* Report Type (and this type can be optionally */
                        /* writable).                                   */
                        if((ReportType == HIDS_REPORT_REFERENCE_REPORT_TYPE_INPUT_REPORT) && (InputReportWritable == FALSE))
                           ServiceInfo->ServiceAttributeTable[Index].Attribute_Flags = GATT_ATTRIBUTE_FLAGS_READABLE;
                        else
                           ServiceInfo->ServiceAttributeTable[Index].Attribute_Flags = GATT_ATTRIBUTE_FLAGS_READABLE_WRITABLE;

                        /* Increment the index past the Report          */
                        /* Characteristic Value.                        */
                        ++Index;

                        /* Format the Report Reference Characteristic   */
                        /* Descriptor that we will add to the Service   */
                        /* Table.                                       */
                        /* * NOTE * WE WILL ALWAYS format the report    */
                        /*          refence structure directly after the*/
                        /*          characteristic value.  Not doing so */
                        /*          will break other functions that     */
                        /*          expect this.                        */
                        ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(ServiceInfo->ReportReferenceInfoList[i].ReportReferenceInfo.Report_ID), ReportReference[i].ReportID);
                        ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(ServiceInfo->ReportReferenceInfoList[i].ReportReferenceInfo.Report_Type), ReportType);

                        HIDS_ASSIGN_REPORT_REFERENCE_DESCRIPTOR_UUID_16(ServiceInfo->ReportReferenceInfoList[i].DescriptorAttributeEntry.Characteristic_Descriptor_UUID);
                        ServiceInfo->ReportReferenceInfoList[i].DescriptorAttributeEntry.Characteristic_Descriptor_Length = HIDS_REPORT_REFERENCE_SIZE;
                        ServiceInfo->ReportReferenceInfoList[i].DescriptorAttributeEntry.Characteristic_Descriptor        = (Byte_t *)&(ServiceInfo->ReportReferenceInfoList[i].ReportReferenceInfo);

                        /* Add the Report Reference Characteristic      */
                        /* Descriptor to the Service Table.             */
                        ServiceInfo->ServiceAttributeTable[Index].Attribute_Flags      = GATT_ATTRIBUTE_FLAGS_READABLE;
                        ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type = aetCharacteristicDescriptor16;
                        ServiceInfo->ServiceAttributeTable[Index].Attribute_Value      = (Byte_t *)&(ServiceInfo->ReportReferenceInfoList[i].DescriptorAttributeEntry);

                        ++Index;

                        /* If the report type is a Input Report then it */
                        /* will need a Client Configuration Value.      */
                        if(ReportType == HIDS_REPORT_REFERENCE_REPORT_TYPE_INPUT_REPORT)
                        {
                           ServiceInfo->ServiceAttributeTable[Index].Attribute_Flags      = GATT_ATTRIBUTE_FLAGS_READABLE_WRITABLE;
                           ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type = aetCharacteristicDescriptor16;
                           ServiceInfo->ServiceAttributeTable[Index].Attribute_Value      = (Byte_t *)&Client_Characteristic_Configuration;

                           /* Increment the index past the CCCD for the */
                           /* Input Report Characteristic.              */
                           ++Index;
                        }
                     }

                     /* Continue only if no error has occurred.         */
                     if(!ret_val)
                     {
                        /* Assign any Boot Mode Declarations            */
                        if(Flags & (HIDS_FLAGS_SUPPORT_MOUSE | HIDS_FLAGS_SUPPORT_KEYBOARD))
                        {
                           /* Add the Protocol Mode Characteristic      */
                           /* Declaration to the Service Table.         */
                           ServiceInfo->ServiceAttributeTable[Index].Attribute_Flags      = GATT_ATTRIBUTE_FLAGS_READABLE;
                           ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type = aetCharacteristicDeclaration16;
                           ServiceInfo->ServiceAttributeTable[Index].Attribute_Value      = (Byte_t *)&HIDS_Protocol_Mode_Declaration;
                           ++Index;

                           /* Add the Protocol Mode Characteristic Value*/
                           /* to the Service Table.                     */
                           ServiceInfo->ServiceAttributeTable[Index].Attribute_Flags      = GATT_ATTRIBUTE_FLAGS_READABLE_WRITABLE;
                           ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type = aetCharacteristicValue16;
                           ServiceInfo->ServiceAttributeTable[Index].Attribute_Value      = (Byte_t *)&HIDS_Protocol_Mode_Value;

                           /* Save the offset in the Service Table of   */
                           /* the Protocol Mode Characteristic Value.   */
                           ServiceInfo->ProtocolModeOffset                                = (Word_t)Index;

                           /* Increment the index past the Protocol Mode*/
                           /* Characteristic Value.                     */
                           ++Index;

                           /* Check to see if the Boot Mouse support is */
                           /* to be added.  If so we need to add the    */
                           /* Boot Mouse Input Report Characteristic to */
                           /* the Service Table, along with a CCCD.     */
                           if(Flags & HIDS_FLAGS_SUPPORT_MOUSE)
                           {
                              /* Add the Boot Mouse Input Report        */
                              /* Characteristic Declaration to the      */
                              /* Service Table.                         */
                              ServiceInfo->ServiceAttributeTable[Index].Attribute_Flags      = GATT_ATTRIBUTE_FLAGS_READABLE;
                              ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type = aetCharacteristicDeclaration16;
                              ServiceInfo->ServiceAttributeTable[Index].Attribute_Value      = (Flags & HIDS_FLAGS_BOOT_MOUSE_INPUT_REPORT_WRITABLE)?((Byte_t *)&HIDS_Boot_Mouse_Input_Report_Declaration_ReadWrite):((Byte_t *)&HIDS_Boot_Mouse_Input_Report_Declaration_Read);
                              ++Index;

                              /* Add the Boot Mouse Input Report        */
                              /* Characteristic Value to the Service    */
                              /* Table.                                 */
                              ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type = aetCharacteristicValue16;
                              ServiceInfo->ServiceAttributeTable[Index].Attribute_Value      = (Byte_t *)&HIDS_Boot_Mouse_Input_Report_Value;

                              /* If the caller wants to allow the Boot  */
                              /* Mouse Input Report to be writable we   */
                              /* need to make sure that the             */
                              /* characteristic value is writable,      */
                              /* otherwise it will only be readable.    */
                              if(Flags & HIDS_FLAGS_BOOT_MOUSE_INPUT_REPORT_WRITABLE)
                                 ServiceInfo->ServiceAttributeTable[Index].Attribute_Flags = GATT_ATTRIBUTE_FLAGS_READABLE_WRITABLE;
                              else
                                 ServiceInfo->ServiceAttributeTable[Index].Attribute_Flags = GATT_ATTRIBUTE_FLAGS_READABLE;

                              /* Save the Attribute Offset of the Boot  */
                              /* Mouse Input Report to the Service      */
                              /* Information structure.                 */
                              ServiceInfo->BootMouseInputReportOffset = (Word_t)Index;

                              /* Increment the index past the Boot Mouse*/
                              /* Input Report Characteristic Value.     */
                              ++Index;

                              /* Add the CCCD for the Mouse Input Report*/
                              /* characteristic to the Service Table.   */
                              ServiceInfo->ServiceAttributeTable[Index].Attribute_Flags      = GATT_ATTRIBUTE_FLAGS_READABLE_WRITABLE;
                              ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type = aetCharacteristicDescriptor16;
                              ServiceInfo->ServiceAttributeTable[Index].Attribute_Value      = (Byte_t *)&Client_Characteristic_Configuration;
                              ++Index;
                           }

                           /* Check to see if Boot Keyboard support is  */
                           /* to be included.  If so we will add all of */
                           /* the required characteristics and          */
                           /* associated descriptors.                   */
                           if(Flags & HIDS_FLAGS_SUPPORT_KEYBOARD)
                           {
                              /* Add the Boot Keyboard Input Report     */
                              /* Characteristic Declaration to the      */
                              /* Service Table.                         */
                              ServiceInfo->ServiceAttributeTable[Index].Attribute_Flags      = GATT_ATTRIBUTE_FLAGS_READABLE;
                              ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type = aetCharacteristicDeclaration16;
                              ServiceInfo->ServiceAttributeTable[Index].Attribute_Value      = (Flags & HIDS_FLAGS_BOOT_KEYBOARD_INPUT_REPORT_WRITABLE)?((Byte_t *)&HIDS_Boot_Keyboard_Input_Report_Declaration_ReadWrite):((Byte_t *)&HIDS_Boot_Keyboard_Input_Report_Declaration_Read);
                              ++Index;

                              /* Add the Boot Keyboard Input Report     */
                              /* Characteristic Value to the Service    */
                              /* Table.                                 */
                              ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type = aetCharacteristicValue16;
                              ServiceInfo->ServiceAttributeTable[Index].Attribute_Value      = (Byte_t *)&HIDS_Boot_Keyboard_Input_Report_Value;

                              /* If the caller wants to allow the Boot  */
                              /* Keyboard Input Report to be writable we*/
                              /* need to make sure that the             */
                              /* characteristic value is writable,      */
                              /* otherwise it will only be readable.    */
                              if(Flags & HIDS_FLAGS_BOOT_KEYBOARD_INPUT_REPORT_WRITABLE)
                                 ServiceInfo->ServiceAttributeTable[Index].Attribute_Flags = GATT_ATTRIBUTE_FLAGS_READABLE_WRITABLE;
                              else
                                 ServiceInfo->ServiceAttributeTable[Index].Attribute_Flags = GATT_ATTRIBUTE_FLAGS_READABLE;

                              /* Save the Attribute Offset of the Boot  */
                              /* Keyboard Input Report to the Service   */
                              /* Information structure.                 */
                              ServiceInfo->BootKeyboardInputReportOffset = (Word_t)Index;

                              /* Increment the index past the Boot      */
                              /* Keyboard Input Report Characteristic   */
                              /* Value.                                 */
                              ++Index;

                              /* Add the CCCD for the Boot Keyboard     */
                              /* Input Report characteristic to the     */
                              /* Service Table.                         */
                              ServiceInfo->ServiceAttributeTable[Index].Attribute_Flags      = GATT_ATTRIBUTE_FLAGS_READABLE_WRITABLE;
                              ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type = aetCharacteristicDescriptor16;
                              ServiceInfo->ServiceAttributeTable[Index].Attribute_Value      = (Byte_t *)&Client_Characteristic_Configuration;
                              ++Index;

                              /* Add the Boot Keyboard Output Report    */
                              /* Characteristic Declaration to the      */
                              /* Service Table.                         */
                              ServiceInfo->ServiceAttributeTable[Index].Attribute_Flags      = GATT_ATTRIBUTE_FLAGS_READABLE;
                              ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type = aetCharacteristicDeclaration16;
                              ServiceInfo->ServiceAttributeTable[Index].Attribute_Value      = (Byte_t *)&HIDS_Boot_Keyboard_Output_Report_Declaration;
                              ++Index;

                              /* Add the Boot Keyboard Output Report    */
                              /* Characteristic Value to the Service    */
                              /* Table.                                 */
                              ServiceInfo->ServiceAttributeTable[Index].Attribute_Flags      = GATT_ATTRIBUTE_FLAGS_READABLE_WRITABLE;
                              ServiceInfo->ServiceAttributeTable[Index].Attribute_Entry_Type = aetCharacteristicValue16;
                              ServiceInfo->ServiceAttributeTable[Index].Attribute_Value      = (Byte_t *)&HIDS_Boot_Keyboard_Output_Report_Value;

                              /* Save the Attribute Offset of the Boot  */
                              /* Keyboard Output Report to the Service  */
                              /* Information structure.                 */
                              ServiceInfo->BootKeyboardOutputReportOffset = (Word_t)Index;

                              /* Increment the index past the Boot      */
                              /* Keyboard Output Report Characteristic  */
                              /* Value.                                 */
                              ++Index;
                           }
                        }

                        /* Call GATT to register the HIDS service.      */
                        ret_val = GATT_Register_Service(BluetoothStackID, HIDS_SERVICE_FLAGS, ServiceInfo->NumberServiceAttributeEntries, ServiceInfo->ServiceAttributeTable, ServiceHandleRange, GATT_ServerEventCallback, ServiceInfo->InstanceID);
                        if(ret_val > 0)
                        {
                           /* Save the Instance information.            */
                           ServiceInfo->BluetoothStackID  = BluetoothStackID;
                           ServiceInfo->ServiceID         = (unsigned int)ret_val;
                           ServiceInfo->EventCallback     = EventCallback;
                           ServiceInfo->CallbackParameter = CallbackParameter;
                           *ServiceID                     = (unsigned int)ret_val;

                           /* Return the HIDS Instance ID.              */
                           ret_val                        = (int)ServiceInfo->InstanceID;
                        }
                     }

                     /* If an error occurred make sure we clean up the  */
                     /* allocated resources.                            */
                     if(ret_val < 0)
                     {
                        /* Delete the HID Service Info entry and free   */
                        /* the allocated memory.                        */
                        if((ServiceInfo = DeleteHIDServiceInfoEntry(&HIDServiceInfoList, ServiceInfo->InstanceID)) != NULL)
                           FreeHIDServiceInfoEntryMemory(ServiceInfo);
                     }
                  }
               }
               else
                  ret_val = HIDS_ERROR_INSUFFICIENT_RESOURCES;

               /* Release the Mutex that we acquired earlier.           */
               BSC_ReleaseListLock();
            }
            else
               ret_val = HIDS_ERROR_INVALID_PARAMETER;
         }

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(BluetoothStackID);
      }
      else
         ret_val = HIDS_ERROR_INVALID_BLUETOOTH_STACK_ID;
   }
   else
      ret_val = HIDS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is the GATT Server Event Callback that     */
   /* handles all requests made to the HIDS Service for all registered  */
   /* instances.                                                        */
static void BTPSAPI GATT_ServerEventCallback(unsigned int BluetoothStackID, GATT_Server_Event_Data_t *GATT_ServerEventData, unsigned long CallbackParameter)
{
   Byte_t                               ErrorCode;
   Byte_t                               ProtocolMode;
   Word_t                               AttributeOffset;
   Word_t                               ValueLength;
   Byte_t                               Opcode;
   Byte_t                              *Value;
   Byte_t                               Event_Buffer[HIDS_EVENT_DATA_SIZE + HIDS_EVENT_DATA_BUFFER_SIZE];
   unsigned int                         TransactionID;
   unsigned int                         InstanceID;
   unsigned int                         ConnectionID;
   HIDS_Event_Data_t                   *EventData;
   HIDS_Service_Info_t                 *ServiceInfo;
   HIDS_Report_Reference_Data_t         ReportReferenceData;
   HIDS_Internal_Characteristic_Type_t  Characteristic_Type;

   /* Verify that all parameters to this callback are Semi-Valid.       */
   if((BluetoothStackID) && (GATT_ServerEventData) && (CallbackParameter))
   {
      /* The Instance ID is always registered as the callback parameter.*/
      InstanceID = (unsigned int)CallbackParameter;

      /* Acquire the Service Instance for the specified service.        */
      if((ServiceInfo = AcquireServiceInstance(BluetoothStackID, InstanceID)) != NULL)
      {
         switch(GATT_ServerEventData->Event_Data_Type)
         {
            case etGATT_Server_Read_Request:
               /* Verify that the Event Data is valid.                  */
               if(GATT_ServerEventData->Event_Data.GATT_Read_Request_Data)
               {
                  AttributeOffset = GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->AttributeOffset;
                  TransactionID   = GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->TransactionID;
                  ConnectionID    = GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->ConnectionID;
                  ErrorCode       = 0;

                  /* Check to see if this is a read of a Characteristic */
                  /* Value or Characteristic Descriptor.                */
                  if(ServiceInfo->ServiceAttributeTable[AttributeOffset].Attribute_Entry_Type == aetCharacteristicValue16)
                  {
                     /* Get a pointer to the characteristic structure.  */
                     Value = ServiceInfo->ServiceAttributeTable[AttributeOffset].Attribute_Value;
                     if(Value)
                     {
                        /* Check to see if static data is registered.   */
                        if((((GATT_Characteristic_Value_16_Entry_t *)Value)->Characteristic_Value_Length) && (((GATT_Characteristic_Value_16_Entry_t *)Value)->Characteristic_Value))
                        {
                           /* Static data so just respond with the read.*/
                           ValueLength = (Word_t)((GATT_Characteristic_Value_16_Entry_t *)Value)->Characteristic_Value_Length;
                           Value       = ((GATT_Characteristic_Value_16_Entry_t *)Value)->Characteristic_Value;

                           GATT_Read_Response(BluetoothStackID, TransactionID, ValueLength, Value);
                        }
                        else
                        {
                           /* Initialize the Report Reference Structure */
                           /* to all 0s.                                */
                           ReportReferenceData.ReportID   = 0;
                           ReportReferenceData.ReportType = 0;

                           /* Initialize the event data to null.        */
                           EventData                      = NULL;

                           /* Since this isn't static data we must have */
                           /* formatted the type of value into the      */
                           /* length.                                   */
                           Characteristic_Type = (HIDS_Internal_Characteristic_Type_t)((GATT_Characteristic_Value_16_Entry_t *)Value)->Characteristic_Value_Length;
                           switch(Characteristic_Type)
                           {
                              case ictReport:
                                 if(!ValueToReportReference(ServiceInfo, AttributeOffset, &ReportReferenceData))
                                 {
                                    /* Failed to get the Report         */
                                    /* Reference structure so return an */
                                    /* error.                           */
                                    ErrorCode = ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR;
                                    break;
                                 }

                                 /* * NOTE * Intentional Fall Through.  */
                              case ictBootKeyboardInputReport:
                              case ictBootKeyboardOutputReport:
                              case ictBootMouseInputReport:
                                 /* Format the Get Report Request Event.*/
                                 if((EventData = FormatEventHeader(sizeof(Event_Buffer), Event_Buffer, etHIDS_Server_Get_Report_Request, InstanceID, ConnectionID, &TransactionID, GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->ConnectionType, &(GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->RemoteDevice))) != NULL)
                                 {
                                    /* Format the rest of the event.    */
                                    EventData->Event_Data.HIDS_Get_Report_Request_Data->ReportType          = (HIDS_Report_Type_t)Characteristic_Type;
                                    EventData->Event_Data.HIDS_Get_Report_Request_Data->ReportReferenceData = ReportReferenceData;
                                    EventData->Event_Data.HIDS_Get_Report_Request_Data->ReportOffset        = GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->AttributeValueOffset;
                                 }
                                 break;
                              case ictProtocolMode:
                                 /* Format the Get Protocol Mode Request*/
                                 /* Event.                              */
                                 EventData = FormatEventHeader(sizeof(Event_Buffer), Event_Buffer, etHIDS_Server_Get_Protocol_Mode_Request, InstanceID, ConnectionID, &TransactionID, GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->ConnectionType, &(GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->RemoteDevice));
                                 break;
                              case ictReportMap:
                                 /* Format the Get Report Map Request   */
                                 /* Event.                              */
                                 EventData = FormatEventHeader(sizeof(Event_Buffer), Event_Buffer, etHIDS_Server_Get_Report_Map_Request, InstanceID, ConnectionID, &TransactionID, GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->ConnectionType, &(GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->RemoteDevice));
                                 if(EventData)
                                 {
                                    /* Format the rest of the event.    */
                                    EventData->Event_Data.HIDS_Get_Report_Map_Data->ReportMapOffset = GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->AttributeValueOffset;
                                 }
                                 break;
                              case ictControlPoint:
                                 ErrorCode = ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR;
                                 break;
                           }

                           /* Dispatch the event if necessary.          */
                           if((!ErrorCode) && (EventData))
                           {
                              /* Go ahead and dispatch the event to the */
                              /* caller.                                */
                              __BTPSTRY
                              {
                                 (*ServiceInfo->EventCallback)(ServiceInfo->BluetoothStackID, EventData, ServiceInfo->CallbackParameter);
                              }
                              __BTPSEXCEPT(1)
                              {
                                 /* Do Nothing.                         */
                              }
                           }
                           else
                           {
                              if(!ErrorCode)
                                 ErrorCode = ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR;
                           }
                        }
                     }
                     else
                        ErrorCode = ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR;
                  }
                  else
                  {
                     /* Attempt to read a descriptor.  Now check to see */
                     /* if is a request to read a static value.         */
                     if(ServiceInfo->ServiceAttributeTable[AttributeOffset].Attribute_Entry_Type == aetCharacteristicDescriptor16)
                     {
                        /* Get a pointer to the descriptor structure.   */
                        Value = ServiceInfo->ServiceAttributeTable[AttributeOffset].Attribute_Value;
                        if(Value)
                        {
                           /* Check to see if static data is registered.*/
                           if((((GATT_Characteristic_Descriptor_16_Entry_t *)Value)->Characteristic_Descriptor_Length) && (((GATT_Characteristic_Descriptor_16_Entry_t *)Value)->Characteristic_Descriptor))
                           {
                              /* Static data so just respond with the   */
                              /* read.                                  */
                              ValueLength = (Word_t)((GATT_Characteristic_Descriptor_16_Entry_t *)Value)->Characteristic_Descriptor_Length;
                              Value       = ((GATT_Characteristic_Descriptor_16_Entry_t *)Value)->Characteristic_Descriptor;

                              GATT_Read_Response(BluetoothStackID, TransactionID, ValueLength, Value);
                           }
                           else
                           {
                              /* If it isn't static data then it must be*/
                              /* a Read of a CCCD.  If so map the CCCD  */
                              /* to the characteristic value it belongs */
                              /* to                                     */
                              if(GATT_COMPARE_CLIENT_CHARACTERISTIC_CONFIGURATION_ATTRIBUTE_TYPE_TO_BLUETOOTH_UUID_16(((GATT_Characteristic_Descriptor_16_Entry_t *)Value)->Characteristic_Descriptor_UUID))
                              {
                                 /* Map the CCCD to the Characteristic  */
                                 /* Type.                               */
                                 Characteristic_Type = ictControlPoint;
                                 if(ClientConfigurationToType(ServiceInfo, AttributeOffset, &Characteristic_Type))
                                 {
                                    /* Initialize the Report Reference  */
                                    /* Structure to all 0s.             */
                                    ReportReferenceData.ReportID   = 0;
                                    ReportReferenceData.ReportType = 0;

                                    /* If the CCCD belongs to a Report  */
                                    /* (i.e.  not a Boot Report) then we*/
                                    /* need to get the Report Reference */
                                    /* Structure for the Report.        */
                                    if((Characteristic_Type != ictReport) || (ClientConfigurationToReportReference(ServiceInfo, AttributeOffset, &ReportReferenceData)))
                                    {
                                       /* Format the CCCD Read Event.   */
                                       if((EventData = FormatEventHeader(sizeof(Event_Buffer), Event_Buffer, etHIDS_Server_Read_Client_Configuration_Request, InstanceID, ConnectionID, &TransactionID, GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->ConnectionType, &(GATT_ServerEventData->Event_Data.GATT_Read_Request_Data->RemoteDevice))) != NULL)
                                       {
                                          /* Format the rest of the     */
                                          /* event.                     */
                                          EventData->Event_Data.HIDS_Read_Client_Configuration_Data->ReportType          = (HIDS_Report_Type_t)Characteristic_Type;
                                          EventData->Event_Data.HIDS_Read_Client_Configuration_Data->ReportReferenceData = ReportReferenceData;

                                          /* Go ahead and dispatch the  */
                                          /* event to the caller.       */
                                          __BTPSTRY
                                          {
                                             (*ServiceInfo->EventCallback)(ServiceInfo->BluetoothStackID, EventData, ServiceInfo->CallbackParameter);
                                          }
                                          __BTPSEXCEPT(1)
                                          {
                                             /* Do Nothing.             */
                                          }
                                       }
                                       else
                                          ErrorCode = ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR;
                                    }
                                    else
                                       ErrorCode = ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR;
                                 }
                                 else
                                    ErrorCode = ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR;
                              }
                              else
                                 ErrorCode = ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR;
                           }
                        }
                        else
                           ErrorCode = ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR;
                     }
                     else
                        ErrorCode = ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR;
                  }

                  /* Send an error response to the request if necessary.*/
                  if(ErrorCode)
                     GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, ErrorCode);
               }
               break;
            case etGATT_Server_Write_Request:
               /* Verify that the Event Data is valid.                  */
               if(GATT_ServerEventData->Event_Data.GATT_Write_Request_Data)
               {
                  AttributeOffset = GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->AttributeOffset;
                  TransactionID   = GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->TransactionID;
                  ConnectionID    = GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->ConnectionID;
                  ValueLength     = GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->AttributeValueLength;
                  ErrorCode       = 0;

                  if((ValueLength) && (!(GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->AttributeValueOffset)) && (!(GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->DelayWrite)))
                  {
                     /* Check to see if this is a read of a             */
                     /* Characteristic Value or Characteristic          */
                     /* Descriptor.                                     */
                     if(ServiceInfo->ServiceAttributeTable[AttributeOffset].Attribute_Entry_Type == aetCharacteristicValue16)
                     {
                        /* Get a pointer to the characteristic          */
                        /* structure.                                   */
                        Value = ServiceInfo->ServiceAttributeTable[AttributeOffset].Attribute_Value;
                        if(Value)
                        {
                           /* Initialize the Report Reference Structure */
                           /* to all 0s.                                */
                           ReportReferenceData.ReportID   = 0;
                           ReportReferenceData.ReportType = 0;

                           /* Initialize the event data to null.        */
                           EventData                      = NULL;

                           /* Since this isn't static data we must have */
                           /* formatted the type of value into the      */
                           /* length.                                   */
                           Characteristic_Type = (HIDS_Internal_Characteristic_Type_t)((GATT_Characteristic_Value_16_Entry_t *)Value)->Characteristic_Value_Length;
                           switch(Characteristic_Type)
                           {
                              case ictReport:
                                 if(!ValueToReportReference(ServiceInfo, AttributeOffset, &ReportReferenceData))
                                 {
                                    /* Failed to get the Report         */
                                    /* Reference structure so return an */
                                    /* error.                           */
                                    ErrorCode = ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR;
                                    break;
                                 }

                                 /* * NOTE * Intentional Fall Through.  */
                              case ictBootKeyboardInputReport:
                              case ictBootKeyboardOutputReport:
                              case ictBootMouseInputReport:
                                 /* Format the Get Report Request Event.*/
                                 if((EventData = FormatEventHeader(sizeof(Event_Buffer), Event_Buffer, etHIDS_Server_Set_Report_Request, InstanceID, ConnectionID, &TransactionID, GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->ConnectionType, &(GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->RemoteDevice))) != NULL)
                                 {
                                    /* Format the rest of the event.    */
                                    EventData->Event_Data.HIDS_Set_Report_Request_Data->ReportType          = (HIDS_Report_Type_t)Characteristic_Type;
                                    EventData->Event_Data.HIDS_Set_Report_Request_Data->ReportReferenceData = ReportReferenceData;
                                    EventData->Event_Data.HIDS_Set_Report_Request_Data->ReportLength        = ValueLength;
                                    EventData->Event_Data.HIDS_Set_Report_Request_Data->Report              = GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->AttributeValue;
                                 }
                                 break;
                              case ictProtocolMode:
                                 /* Get the Protocol Mode.              */
                                 ProtocolMode = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->AttributeValue);
                                 if((ProtocolMode == HIDS_PROTOCOL_MODE_BOOT) || (ProtocolMode == HIDS_PROTOCOL_MODE_REPORT))
                                 {
                                    /* Note, as this is defined to only */
                                    /* allow it to be written using the */
                                    /* Write Without Response procedure,*/
                                    /* we will go ahead and verify that */
                                    /* the write being used is a Write  */
                                    /* Without Response.  If not we will*/
                                    /* fail the response.               */
                                    if((!GATT_Query_Transaction_Opcode(BluetoothStackID, TransactionID, &Opcode)) && (Opcode == ATT_PROTOCOL_CODE_WRITE_COMMAND))
                                    {
                                       /* Go ahead and call the write   */
                                       /* response to cleanup the       */
                                       /* resources in GATT.            */
                                       GATT_Write_Response(BluetoothStackID, TransactionID);

                                       /* Format the Get Protocol Mode  */
                                       /* Request Event.                */
                                       EventData = FormatEventHeader(sizeof(Event_Buffer), Event_Buffer, etHIDS_Server_Set_Protocol_Mode_Request, InstanceID, ConnectionID, &TransactionID, GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->ConnectionType, &(GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->RemoteDevice));
                                       if(EventData)
                                       {
                                          /* Format the rest of the     */
                                          /* event.                     */
                                          EventData->Event_Data.HIDS_Set_Protocol_Mode_Request_Data->ProtocolMode = (ProtocolMode == HIDS_PROTOCOL_MODE_BOOT)?pmBoot:pmReport;
                                       }
                                    }
                                    else
                                       ErrorCode = ATT_PROTOCOL_ERROR_CODE_REQUEST_NOT_SUPPORTED;
                                 }
                                 else
                                    ErrorCode = ATT_PROTOCOL_ERROR_CODE_INVALID_ATTRIBUTE_VALUE_LENGTH;
                                 break;
                              case ictControlPoint:
                                 /* First ensure that the HIDS Client   */
                                 /* uses the Write Without Response PDU.*/
                                 if((!GATT_Query_Transaction_Opcode(BluetoothStackID, TransactionID, &Opcode)) && (Opcode == ATT_PROTOCOL_CODE_WRITE_COMMAND))
                                 {
                                    /* Get the Control Point Command.   */
                                    ProtocolMode = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->AttributeValue);
                                    if((ProtocolMode == HIDS_CONTROL_POINT_COMMAND_SUSPEND) || (ProtocolMode == HIDS_CONTROL_POINT_COMMAND_EXIT_SUSPEND))
                                    {
                                       /* Go ahead and accept the write */
                                       /* request (since it is a command*/
                                       /* we just do that here to allow */
                                       /* GATT to cleanup).             */
                                       GATT_Write_Response(BluetoothStackID, TransactionID);

                                       /* Format the Get Report Map     */
                                       /* Request Event.                */
                                       if((EventData = FormatEventHeader(sizeof(Event_Buffer), Event_Buffer, etHIDS_Server_Control_Point_Command_Indication, InstanceID, ConnectionID, NULL, GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->ConnectionType, &(GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->RemoteDevice))) != NULL)
                                       {
                                          /* Go ahead and format the    */
                                          /* rest of the event.         */
                                          EventData->Event_Data.HIDS_Control_Point_Command_Data->ControlPointCommand = (ProtocolMode == HIDS_CONTROL_POINT_COMMAND_SUSPEND)?pcSuspend:pcExitSuspend;
                                       }
                                    }
                                    else
                                       ErrorCode = ATT_PROTOCOL_ERROR_CODE_REQUEST_NOT_SUPPORTED;
                                 }
                                 else
                                    ErrorCode = ATT_PROTOCOL_ERROR_CODE_REQUEST_NOT_SUPPORTED;
                                 break;
                              case ictReportMap:
                                 ErrorCode = ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR;
                                 break;
                           }

                           /* Dispatch the event if necessary.          */
                           if((!ErrorCode) && (EventData))
                           {
                              /* Go ahead and dispatch the event to the */
                              /* caller.                                */
                              __BTPSTRY
                              {
                                 (*ServiceInfo->EventCallback)(ServiceInfo->BluetoothStackID, EventData, ServiceInfo->CallbackParameter);
                              }
                              __BTPSEXCEPT(1)
                              {
                                 /* Do Nothing.                         */
                              }
                           }
                           else
                           {
                              if(!ErrorCode)
                                 ErrorCode = ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR;
                           }
                        }
                        else
                           ErrorCode = ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR;
                     }
                     else
                     {
                        /* Attempt to read a descriptor.  Now check to  */
                        /* see if is a request to read a static value.  */
                        if(ServiceInfo->ServiceAttributeTable[AttributeOffset].Attribute_Entry_Type == aetCharacteristicDescriptor16)
                        {
                           /* Get a pointer to the descriptor structure.*/
                           Value = ServiceInfo->ServiceAttributeTable[AttributeOffset].Attribute_Value;
                           if(Value)
                           {
                              /* If it isn't static data then it must be*/
                              /* a Read of a CCCD.  If so map the CCCD  */
                              /* to the characteristic value it belongs */
                              /* to                                     */
                              if(GATT_COMPARE_CLIENT_CHARACTERISTIC_CONFIGURATION_ATTRIBUTE_TYPE_TO_BLUETOOTH_UUID_16(((GATT_Characteristic_Descriptor_16_Entry_t *)Value)->Characteristic_Descriptor_UUID))
                              {
                                 /* Map the CCCD to the Characteristic  */
                                 /* Type.                               */
                                 Characteristic_Type = ictControlPoint;
                                 if(ClientConfigurationToType(ServiceInfo, AttributeOffset, &Characteristic_Type))
                                 {
                                    /* Initialize the Report Reference  */
                                    /* Structure to all 0s.             */
                                    ReportReferenceData.ReportID   = 0;
                                    ReportReferenceData.ReportType = 0;

                                    /* If the CCCD belongs to a Report  */
                                    /* (i.e.  not a Boot Report) then we*/
                                    /* need to get the Report Reference */
                                    /* Structure for the Report.        */
                                    if((Characteristic_Type != ictReport) || (ClientConfigurationToReportReference(ServiceInfo, AttributeOffset, &ReportReferenceData)))
                                    {
                                       /* Format the CCCD Read Event.   */
                                       if((EventData = FormatEventHeader(sizeof(Event_Buffer), Event_Buffer, etHIDS_Server_Client_Configuration_Update_Request, InstanceID, ConnectionID, NULL, GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->ConnectionType, &(GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->RemoteDevice))) != NULL)
                                       {
                                          /* Format the rest of the     */
                                          /* event.                     */
                                          EventData->Event_Data.HIDS_Client_Configuration_Update_Data->ReportType          = (HIDS_Report_Type_t)Characteristic_Type;
                                          EventData->Event_Data.HIDS_Client_Configuration_Update_Data->ReportReferenceData = ReportReferenceData;

                                          /* Attempt to decode the      */
                                          /* Client Configuration Value.*/
                                          if(!DecodeClientConfigurationValue(ValueLength, GATT_ServerEventData->Event_Data.GATT_Write_Request_Data->AttributeValue, &(EventData->Event_Data.HIDS_Client_Configuration_Update_Data->ClientConfiguration)))
                                          {
                                             /* Accept the Write Request*/
                                             /* to the CCCD.            */
                                             GATT_Write_Response(BluetoothStackID, TransactionID);

                                             /* Go ahead and dispatch   */
                                             /* the event to the caller.*/
                                             __BTPSTRY
                                             {
                                                (*ServiceInfo->EventCallback)(ServiceInfo->BluetoothStackID, EventData, ServiceInfo->CallbackParameter);
                                             }
                                             __BTPSEXCEPT(1)
                                             {
                                                /* Do Nothing.          */
                                             }
                                          }
                                          else
                                             ErrorCode = ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR;
                                       }
                                       else
                                          ErrorCode = ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR;
                                    }
                                    else
                                       ErrorCode = ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR;
                                 }
                                 else
                                    ErrorCode = ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR;
                              }
                              else
                                 ErrorCode = ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR;
                           }
                           else
                              ErrorCode = ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR;
                        }
                        else
                           ErrorCode = ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR;
                     }

                     /* Send an error response to the request if        */
                     /* necessary.                                      */
                     if(ErrorCode)
                        GATT_Error_Response(BluetoothStackID, TransactionID, AttributeOffset, ErrorCode);
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
         BSC_UnLockBluetoothStack(ServiceInfo->BluetoothStackID);
      }
   }
}

   /* The following function is responsible for making sure that the    */
   /* Bluetooth Stack HIDS Module is Initialized correctly.  This       */
   /* function *MUST* be called before ANY other Bluetooth Stack HIDS   */
   /* function can be called.  This function returns non-zero if the    */
   /* Module was initialized correctly, or a zero value if there was an */
   /* error.                                                            */
   /* * NOTE * Internally, this module will make sure that this function*/
   /*          has been called at least once so that the module will    */
   /*          function.  Calling this function from an external        */
   /*          location is not necessary.                               */
int InitializeHIDSModule(void)
{
   return((int)InitializeModule());
}

   /* The following function is responsible for instructing the         */
   /* Bluetooth Stack HIDSC Module to clean up any resources that it has*/
   /* allocated.  Once this function has completed, NO other Bluetooth  */
   /* Stack HIDS Functions can be called until a successful call to the */
   /* InitializeHIDSModule() function is made.  The parameter to this   */
   /* function specifies the context in which this function is being    */
   /* called.  If the specified parameter is TRUE, then the module will */
   /* make sure that NO functions that would require waiting/blocking on*/
   /* Mutexes/Events are called.  This parameter would be set to TRUE if*/
   /* this function was called in a context where threads would not be  */
   /* allowed to run.  If this function is called in the context where  */
   /* threads are allowed to run then this parameter should be set to   */
   /* FALSE.                                                            */
void CleanupHIDSModule(Boolean_t ForceCleanup)
{
   /* Check to make sure that this module has been initialized.         */
   if(HIDServiceInfoListInitialized)
   {
      /* Wait for access to the HIDS Context List.                      */
      if((ForceCleanup) || ((!ForceCleanup) && (BSC_AcquireListLock())))
      {
         /* Free the List and ALL Its Resources.                        */
         FreeHIDServiceInfoEntryList(&HIDServiceInfoList);

         if(!ForceCleanup)
            BSC_ReleaseListLock();
      }

      /* Cleanup the module.                                            */
      CleanupModule();
   }

   /* Clear All variables to NULL just to be safe.                      */
   HIDServiceInfoList            = NULL;

   /* Flag Module as NOT being initialized.                             */
   HIDServiceInfoListInitialized = FALSE;
}

   /* The following function is responsible for opening a HID over GATT */
   /* Service.  The first parameter is the Bluetooth Stack ID on which  */
   /* to open the server.  The second parameter is a bit mask of flags  */
   /* which is used to control what the HID Service supports.  The third*/
   /* parameter is a pointer to a HID Information structure containing  */
   /* information about the HID Service.  The next two parameters       */
   /* specify a list of ServiceIDs that contain the ServiceIDs of the   */
   /* services to be included by this HID Instance.  The next two       */
   /* parameters contain a list of GATT UUIDs that contain a list of    */
   /* UUIDs of characteristics referenced by this HID Instance.  The    */
   /* next two parameters contain a list of reports that will be        */
   /* contained in this HID Service.  The next two parameters contain a */
   /* callback function and callback parameter that will be called when */
   /* an event occurs on this HID Instance.  The final parameter is a   */
   /* pointer to store the GATT Service ID of the registered HIDS       */
   /* service.  This can be used to include the service registered by   */
   /* this call.  This function returns the positive, non-zero, Instance*/
   /* ID or a negative error code.                                      */
   /* * NOTE * The Flags parameter must be a bit mask made of bits of   */
   /*          the form HIDS_FLAGS_XXX.                                 */
   /* * NOTE * The ServiceIDList parameter must contain valid ServiceIDs*/
   /*          of services that have already been registered with GATT. */
int BTPSAPI HIDS_Initialize_Service(unsigned int BluetoothStackID, Byte_t Flags, HIDS_HID_Information_Data_t *HIDInformation, unsigned int NumIncludedServices, unsigned int *ServiceIDList, unsigned int NumExternalReportReferences, GATT_UUID_t *ReferenceUUID, unsigned int NumReports, HIDS_Report_Reference_Data_t *ReportReference, HIDS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID)
{
    GATT_Attribute_Handle_Group_t ServiceHandleRange;

    /* Initialize the Service Handle Group to 0.                        */
   ServiceHandleRange.Starting_Handle = 0;
   ServiceHandleRange.Ending_Handle   = 0;

   return(HIDSRegisterService(BluetoothStackID, Flags, HIDInformation, NumIncludedServices, ServiceIDList, NumExternalReportReferences, ReferenceUUID, NumReports, ReportReference, EventCallback, CallbackParameter, ServiceID, &ServiceHandleRange));
}

   /* The following function is responsible for opening a HID over GATT */
   /* Service.  The first parameter is the Bluetooth Stack ID on which  */
   /* to open the server.  The second parameter is a bit mask of flags  */
   /* which is used to control what the HID Service supports.  The third*/
   /* parameter is a pointer to a HID Information structure containing  */
   /* information about the HID Service.  The next two parameters       */
   /* specify a list of ServiceIDs that contain the ServiceIDs of the   */
   /* services to be included by this HID Instance.  The next two       */
   /* parameters contain a list of GATT UUIDs that contain a list of    */
   /* UUIDs of characteristics referenced by this HID Instance.  The    */
   /* next two parameters contain a list of reports that will be        */
   /* contained in this HID Service.  The next two parameters contain a */
   /* callback function and callback parameter that will be called when */
   /* an event occurs on this HID Instance.  The next parameter is a    */
   /* pointer to store the GATT Service ID of the registered HIDS       */
   /* service.  This can be used to include the service registered by   */
   /* this call.  The final parameter is a pointer, that on input can be*/
   /* used to control the location of the service in the GATT database, */
   /* and on ouput to store the service handle range.  This function    */
   /* returns the positive, non-zero, Instance ID or a negative error   */
   /* code.                                                             */
   /* * NOTE * The Flags parameter must be a bit mask made of bits of   */
   /*          the form HIDS_FLAGS_XXX.                                 */
   /* * NOTE * The ServiceIDList parameter must contain valid ServiceIDs*/
   /*          of services that have already been registered with GATT. */
int BTPSAPI HIDS_Initialize_Service_Handle_Range(unsigned int BluetoothStackID, Byte_t Flags, HIDS_HID_Information_Data_t *HIDInformation, unsigned int NumIncludedServices, unsigned int *ServiceIDList, unsigned int NumExternalReportReferences, GATT_UUID_t *ReferenceUUID, unsigned int NumReports, HIDS_Report_Reference_Data_t *ReportReference, HIDS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange)
{
   return(HIDSRegisterService(BluetoothStackID, Flags, HIDInformation, NumIncludedServices, ServiceIDList, NumExternalReportReferences, ReferenceUUID, NumReports, ReportReference, EventCallback, CallbackParameter, ServiceID, ServiceHandleRange));
}

   /* The following function is responsible for closing a previously    */
   /* opened HIDS Server.  The first parameter is the Bluetooth Stack ID*/
   /* on which to close the server.  The second parameter is the        */
   /* InstanceID that was returned from a successful call to            */
   /* HIDS_Initialize_Service().  This function returns a zero if       */
   /* successful or a negative return error code if an error occurs.    */
int BTPSAPI HIDS_Cleanup_Service(unsigned int BluetoothStackID, unsigned int InstanceID)
{
   int                  ret_val;
   HIDS_Service_Info_t *ServiceInfo;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID))
   {
      /* Lock the Bluetooth Stack to gain exclusive access to this      */
      /* Bluetooth Protocol Stack.                                      */
      if(!BSC_LockBluetoothStack(BluetoothStackID))
      {
         /* Acquire the BSC List Lock while we are searching the        */
         /* instance list.                                              */
         if(BSC_AcquireListLock())
         {
            /* Delete the HID Service Info entry and free the allocated */
            /* memory.                                                  */
            if((ServiceInfo = DeleteHIDServiceInfoEntry(&HIDServiceInfoList, InstanceID)) != NULL)
            {
               /* Verify that the service is actually registered.       */
               if(ServiceInfo->ServiceID)
               {
                  /* Call GATT to un-register the service.              */
                  GATT_Un_Register_Service(ServiceInfo->BluetoothStackID, ServiceInfo->ServiceID);

                  /* return success to the caller.                      */
                  ret_val = 0;
               }
               else
                  ret_val = HIDS_ERROR_INVALID_PARAMETER;

               FreeHIDServiceInfoEntryMemory(ServiceInfo);
            }
            else
               ret_val = HIDS_ERROR_INVALID_INSTANCE_ID;

            /* Release the previously acquired list lock.               */
            BSC_ReleaseListLock();
         }
         else
            ret_val = HIDS_ERROR_INVALID_INSTANCE_ID;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(BluetoothStackID);
      }
      else
         ret_val = HIDS_ERROR_INVALID_PARAMETER;
   }
   else
      ret_val = HIDS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for querying the number of  */
   /* attributes that are contained in the HIDS Service that is         */
   /* registered with a call to HIDS_Initialize_Service().  The first   */
   /* parameter is a bit mask of flags which is used to control what the*/
   /* HID Service supports.  The second parameter specifies the number  */
   /* of services that will be included by this HID instance.  The third*/
   /* parameter specifies the number of external characteristics that   */
   /* are referenced by reports to be contained in this HID instance.   */
   /* The next two parameters contain a list of reports that will be    */
   /* contained in this HID Service.  This function returns the non-zero*/
   /* number of attributes that are contained in a HIDS Server or zero  */
   /* on failure.                                                       */
unsigned int BTPSAPI HIDS_Query_Number_Attributes(Byte_t Flags, unsigned int NumIncludedServices, unsigned int NumExternalReportReferences, unsigned int NumReports, HIDS_Report_Reference_Data_t *ReportReference)
{
   unsigned int Index;
   unsigned int NumberAttributeEntries;

   /* Make sure the parameters passed to us are semi-valid.             */
   if(((NumReports == 0) ||((NumReports) && (ReportReference))))
   {
      /* Determine the initial number of attributes needed along with   */
      /* the number of attributes needed for the included services and  */
      /* external report reference descriptors.                         */
      NumberAttributeEntries  = MINIMUM_NUMBER_ATTRIBUTE_ENTRIES;
      NumberAttributeEntries += (NUMBER_ATTRIBUTE_ENTRIES_PER_INCLUDED_SERVICE * NumIncludedServices);
      NumberAttributeEntries += (NUMBER_ATTRIBUTE_ENTRIES_PER_EXTERNAL_REFERENCE * NumExternalReportReferences);

      /* Check to see if this is a Keyboard or Mouse device.            */
      if(Flags & (HIDS_FLAGS_SUPPORT_MOUSE | HIDS_FLAGS_SUPPORT_KEYBOARD))
      {
         /* Protocol Mode Entry is required.                            */
         NumberAttributeEntries += NUMBER_ATTRIBUTE_ENTRIES_FOR_PROTOCOL_MODE;

         /* Account for Keyboard and Mouse entries.                     */
         if(Flags & HIDS_FLAGS_SUPPORT_MOUSE)
           NumberAttributeEntries += NUMBER_ATTRIBUTE_ENTRIES_FOR_MOUSE_SUPPORT;

         if(Flags & HIDS_FLAGS_SUPPORT_KEYBOARD)
           NumberAttributeEntries += NUMBER_ATTRIBUTE_ENTRIES_FOR_KEYBOARD_SUPPORT;
      }

      /* Calculate the number of attributes needed for all of the       */
      /* reports.                                                       */
      for(Index=0;Index<NumReports;Index++)
      {
         /* Calculate the number of attributes needed for this report   */
         /* based on the report type.                                   */
         if(ReportReference[Index].ReportType == HIDS_REPORT_REFERENCE_REPORT_TYPE_INPUT_REPORT)
            NumberAttributeEntries += NUMBER_ATTRIBUTE_ENTRIES_PER_INPUT_REPORT;
         else
         {
            if(ReportReference[Index].ReportType == HIDS_REPORT_REFERENCE_REPORT_TYPE_OUTPUT_REPORT)
               NumberAttributeEntries += NUMBER_ATTRIBUTE_ENTRIES_PER_OUTPUT_REPORT;
            else
            {
               if(ReportReference[Index].ReportType == HIDS_REPORT_REFERENCE_REPORT_TYPE_FEATURE_REPORT)
                  NumberAttributeEntries += NUMBER_ATTRIBUTE_ENTRIES_PER_FEATURE_REPORT;
               else
               {
                  /* Unknown report type so just return 0 and exit the  */
                  /* loop.                                              */
                  NumberAttributeEntries = 0;
                  break;
               }
            }
         }
      }
   }
   else
      NumberAttributeEntries = 0;

   /* Finally return the result to the caller.                          */
   return(NumberAttributeEntries);
}

   /* The following function is responsible for responding to a HIDS    */
   /* Read Client Configuration Request.  The first parameter is the    */
   /* Bluetooth Stack ID of the Bluetooth Device.  The second parameter */
   /* is the InstanceID returned from a successful call to              */
   /* HIDS_Initialize_Server().  The third is the Transaction ID of the */
   /* request.  The final parameter contains the Client Configuration to*/
   /* send to the remote device.  This function returns a zero if       */
   /* successful or a negative return error code if an error occurs.    */
int BTPSAPI HIDS_Read_Client_Configuration_Response(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int TransactionID, Word_t Client_Configuration)
{
   int                  ret_val;
   Word_t               ValueLength;
   NonAlignedWord_t     ClientConfiguration;
   HIDS_Service_Info_t *ServiceInfo;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (TransactionID))
   {
      /* Acquire the specified HIDS Instance.                           */
      ServiceInfo = AcquireServiceInstance(BluetoothStackID, InstanceID);
      if(ServiceInfo)
      {
         /* Format the Read Response.                                   */
         ValueLength = GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_LENGTH;
         ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&ClientConfiguration, Client_Configuration);

         /* Send the response.                                          */
         ret_val = GATT_Read_Response(ServiceInfo->BluetoothStackID, TransactionID, (unsigned int)ValueLength, (Byte_t *)&ClientConfiguration);

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInfo->BluetoothStackID);
      }
      else
         ret_val = HIDS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = HIDS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for responding to a HIDS Get*/
   /* Protocol Mode Request.  The first parameter is the Bluetooth Stack*/
   /* ID of the Bluetooth Device.  The second parameter is the          */
   /* InstanceID returned from a successful call to                     */
   /* HIDS_Initialize_Server().  The third is the Transaction ID of the */
   /* request.  The fourth parameter is an ErrorCode parameter that can */
   /* be used to respond with an error response to the request.  The    */
   /* final parameter contains the Protocol Mode to respond with.  This */
   /* function returns a zero if successful or a negative return error  */
   /* code if an error occurs.                                          */
   /* * NOTE * If ErrorCode is 0 then this function will respond to the */
   /*          Get Protocol Mode Request successfully.  If ErrorCode is */
   /*          NON-ZERO then this function will respond with an error   */
   /*          with the error code set to the parameter.                */
int BTPSAPI HIDS_Get_Protocol_Mode_Response(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int TransactionID, Byte_t ErrorCode, HIDS_Protocol_Mode_t CurrentProtocolMode)
{
   int                  ret_val;
   Byte_t               ProtocolMode;
   HIDS_Service_Info_t *ServiceInfo;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (TransactionID) && ((ErrorCode) || ((CurrentProtocolMode == pmBoot) || (CurrentProtocolMode == pmReport))))
   {
      /* Acquire the specified HIDS Instance.                           */
      ServiceInfo = AcquireServiceInstance(BluetoothStackID, InstanceID);
      if(ServiceInfo)
      {
         /* Verify that this HID Instance has a protocol mode           */
         /* characteristic.                                             */
         if(ServiceInfo->ProtocolModeOffset)
         {
            /* Check to see if we are responding with an error or with  */
            /* success.                                                 */
            if(!ErrorCode)
            {
               /* Respond with the protocol mode.                       */
               ProtocolMode = (Byte_t)((CurrentProtocolMode == pmBoot)?HIDS_PROTOCOL_MODE_BOOT:HIDS_PROTOCOL_MODE_REPORT);

               /* Respond successfully with the current protocol mode.  */
               ret_val      = GATT_Read_Response(ServiceInfo->BluetoothStackID, TransactionID, 1, &ProtocolMode);
            }
            else
            {
               /* Respond with an error to the request.                 */
               ret_val = GATT_Error_Response(ServiceInfo->BluetoothStackID, TransactionID, ServiceInfo->ProtocolModeOffset, ErrorCode);
            }
         }
         else
            ret_val = HIDS_ERROR_INVALID_PARAMETER;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInfo->BluetoothStackID);
      }
      else
         ret_val = HIDS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = HIDS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for responding to a HIDS Get*/
   /* Report Map Request.  The first parameter is the Bluetooth Stack ID*/
   /* of the Bluetooth Device.  The second parameter is the InstanceID  */
   /* returned from a successful call to HIDS_Initialize_Server().  The */
   /* third is the Transaction ID of the request.  The fourth parameter */
   /* is an ErrorCode parameter that can be used to respond with an     */
   /* error response to the request.  The final parameters specify the  */
   /* Report Map length and data to respond with (if ErrorCode is 0).   */
   /* This function returns a zero if successful or a negative return   */
   /* error code if an error occurs.                                    */
   /* * NOTE * If ErrorCode is 0 then this function will respond to the */
   /*          Get Report Request successfully.  If ErrorCode is        */
   /*          NON-ZERO then this function will respond with an error   */
   /*          with the error code set to the parameter.                */
int BTPSAPI HIDS_Get_Report_Map_Response(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int TransactionID, Byte_t ErrorCode, unsigned int ReportMapLength, Byte_t *ReportMap)
{
   int                  ret_val;
   HIDS_Service_Info_t *ServiceInfo;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (TransactionID) && ((ErrorCode) || ((ReportMapLength) && (ReportMap))))
   {
      /* Acquire the specified HIDS Instance.                           */
      ServiceInfo = AcquireServiceInstance(BluetoothStackID, InstanceID);
      if(ServiceInfo)
      {
         /* Verify that this HID Instance has a protocol mode           */
         /* characteristic.                                             */
         if(ServiceInfo->ReportMapOffset)
         {
            /* Check to see if we are responding with an error or with  */
            /* success.                                                 */
            if(!ErrorCode)
            {
               /* Respond successfully to the Get Report Map Request.   */
               ret_val = GATT_Read_Response(ServiceInfo->BluetoothStackID, TransactionID, ReportMapLength, ReportMap);
            }
            else
            {
               /* Respond with an error to the request.                 */
               ret_val = GATT_Error_Response(ServiceInfo->BluetoothStackID, TransactionID, ServiceInfo->ReportMapOffset, ErrorCode);
            }
         }
         else
            ret_val = HIDS_ERROR_INVALID_PARAMETER;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInfo->BluetoothStackID);
      }
      else
         ret_val = HIDS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = HIDS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for responding to a HIDS Get*/
   /* Report Map Request.  The first parameter is the Bluetooth Stack ID*/
   /* of the Bluetooth Device.  The second parameter is the InstanceID  */
   /* returned from a successful call to HIDS_Initialize_Server().  The */
   /* third is the Transaction ID of the request.  The fourth parameter */
   /* is the ReportType that the client is attempting to get.  The fifth*/
   /* parameter, which is only valid if ReportType is rtReport, contains*/
   /* the report reference data of the Report that the client is        */
   /* attempting to get.  The sixth parameter is an ErrorCode parameter */
   /* that can be used to respond with an error response to the request.*/
   /* The final parameters specify the Report Map length and data to    */
   /* respond with (if ErrorCode is 0).  This function returns a zero if*/
   /* successful or a negative return error code if an error occurs.    */
   /* * NOTE * If ErrorCode is 0 then this function will respond to the */
   /*          Get Report Request successfully.  If ErrorCode is        */
   /*          NON-ZERO then this function will respond with an error   */
   /*          with the error code set to the parameter.                */
   /* * NOTE * The ReportReferenceData member is only valid if          */
   /*          ReportType is set to rtReport.                           */
int BTPSAPI HIDS_Get_Report_Response(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int TransactionID, HIDS_Report_Type_t ReportType, HIDS_Report_Reference_Data_t *ReportReferenceData, Byte_t ErrorCode, unsigned int ReportLength, Byte_t *Report)
{
   int                  ret_val;
   unsigned int         AttributeOffset;
   HIDS_Service_Info_t *ServiceInfo;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (TransactionID) && ((ReportType != rtReport) || (ReportReferenceData)) && ((ErrorCode) || ((ReportLength) && (Report))))
   {
      /* Acquire the specified HIDS Instance.                           */
      ServiceInfo = AcquireServiceInstance(BluetoothStackID, InstanceID);
      if(ServiceInfo)
      {
         /* Determine what type of report is being read and determine   */
         /* the Attribute Offset of the actual characteristic being     */
         /* read.                                                       */
         switch(ReportType)
         {
            case rtBootMouseInputReport:
               AttributeOffset = ServiceInfo->BootMouseInputReportOffset;
               break;
            case rtBootKeyboardInputReport:
               AttributeOffset = ServiceInfo->BootKeyboardInputReportOffset;
               break;
            case rtBootKeyboardOutputReport:
               AttributeOffset = ServiceInfo->BootKeyboardOutputReportOffset;
               break;
            case rtReport:
               AttributeOffset = ReportReferenceToOffset(ServiceInfo, ReportReferenceData);
               break;
            default:
               AttributeOffset = 0;
               break;
         }

         /* Verify that we correctly determined the attribute offset of */
         /* the characteristic that is being notified.                  */
         if(AttributeOffset)
         {
            /* Check to see if we are responding with an error or with  */
            /* success.                                                 */
            if(!ErrorCode)
            {
               /* Respond successfully to the Get Report Request.       */
               ret_val = GATT_Read_Response(ServiceInfo->BluetoothStackID, TransactionID, ReportLength, Report);
            }
            else
            {
               /* Respond with an error to the request.                 */
               ret_val = GATT_Error_Response(ServiceInfo->BluetoothStackID, TransactionID, (Word_t)AttributeOffset, ErrorCode);
            }
         }
         else
            ret_val = HIDS_ERROR_INVALID_PARAMETER;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInfo->BluetoothStackID);
      }
      else
         ret_val = HIDS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = HIDS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for responding to a HIDS Set*/
   /* Report Map Request.  The first parameter is the Bluetooth Stack ID*/
   /* of the Bluetooth Device.  The second parameter is the InstanceID  */
   /* returned from a successful call to HIDS_Initialize_Server().  The */
   /* third is the Transaction ID of the request.  The fourth parameter */
   /* is the ReportType that the client is attempting to set.  The fifth*/
   /* parameter, which is only valid if ReportType is rtReport, contains*/
   /* the report reference data of the Report that the client is        */
   /* attempting to set.  The sixth parameter is an ErrorCode parameter */
   /* that can be used to respond with an error response to the request.*/
   /* This function returns a zero if successful or a negative return   */
   /* error code if an error occurs.                                    */
   /* * NOTE * If ErrorCode is 0 then this function will respond to the */
   /*          Get Report Request successfully.  If ErrorCode is        */
   /*          NON-ZERO then this function will respond with an error   */
   /*          with the error code set to the parameter.                */
   /* * NOTE * The ReportReferenceData member is only valid if          */
   /*          ReportType is set to rtReport.                           */
int BTPSAPI HIDS_Set_Report_Response(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int TransactionID, HIDS_Report_Type_t ReportType, HIDS_Report_Reference_Data_t *ReportReferenceData, Byte_t ErrorCode)
{
   int                  ret_val;
   unsigned int         AttributeOffset;
   HIDS_Service_Info_t *ServiceInfo;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (TransactionID) && ((ReportType != rtReport) || (ReportReferenceData)))
   {
      /* Acquire the specified HIDS Instance.                           */
      ServiceInfo = AcquireServiceInstance(BluetoothStackID, InstanceID);
      if(ServiceInfo)
      {
         /* Determine what type of report is being written and determine*/
         /* the Attribute Offset of the actual characteristic being     */
         /* written.                                                    */
         switch(ReportType)
         {
            case rtBootMouseInputReport:
               AttributeOffset = ServiceInfo->BootMouseInputReportOffset;
               break;
            case rtBootKeyboardInputReport:
               AttributeOffset = ServiceInfo->BootKeyboardInputReportOffset;
               break;
            case rtBootKeyboardOutputReport:
               AttributeOffset = ServiceInfo->BootKeyboardOutputReportOffset;
               break;
            case rtReport:
               AttributeOffset = ReportReferenceToOffset(ServiceInfo, ReportReferenceData);
               break;
            default:
               AttributeOffset = 0;
               break;
         }

         /* Verify that we correctly determined the attribute offset of */
         /* the characteristic that is being notified.                  */
         if(AttributeOffset)
         {
            /* Check to see if we are responding with an error or with  */
            /* success.                                                 */
            if(!ErrorCode)
            {
               /* Respond successfully to the Set Report Request.       */
               ret_val = GATT_Write_Response(ServiceInfo->BluetoothStackID, TransactionID);
            }
            else
            {
               /* Respond with an error to the request.                 */
               ret_val = GATT_Error_Response(ServiceInfo->BluetoothStackID, TransactionID, (Word_t)AttributeOffset, ErrorCode);
            }
         }
         else
            ret_val = HIDS_ERROR_INVALID_PARAMETER;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInfo->BluetoothStackID);
      }
      else
         ret_val = HIDS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = HIDS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for sending an Input Report */
   /* notification to a specified remote device.  The first parameter is*/
   /* the Bluetooth Stack ID of the Bluetooth Device.  The second       */
   /* parameter is the InstanceID returned from a successful call to    */
   /* HIDS_Initialize_Server().  The third parameter is the ConnectionID*/
   /* of the remote device to send the notification to.  The fourth     */
   /* parameter specifies the ReportType of the report that is to be    */
   /* notified.  The fifth parameter is a pointer to a Report Reference */
   /* structure that is only used (and must be specified only if) the   */
   /* ReportType is rtInputReport.  The final parameters contain the    */
   /* length of the Input Report and a pointer to the Input Report that */
   /* is to be notified.  This function returns the number of bytes that*/
   /* were successfully notified on success or a negative return error  */
   /* code if an error occurs.                                          */
   /* * NOTE * If the ReportType is rtReport, then the                  */
   /*          ReportReferenceData must be valid and point to a Report  */
   /*          Reference structure of the Input Report to be notified.  */
   /*          Otherwise the ReportReferenceData parameter can be NULL. */
int BTPSAPI HIDS_Notify_Input_Report(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int ConnectionID, HIDS_Report_Type_t ReportType, HIDS_Report_Reference_Data_t *ReportReferenceData, Word_t InputReportLength, Byte_t *InputReportData)
{
   int                  ret_val;
   unsigned int         AttributeOffset;
   HIDS_Service_Info_t *ServiceInfo;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((BluetoothStackID) && (InstanceID) && (ConnectionID) && ((ReportType != rtReport) || ((ReportReferenceData) && (ReportReferenceData->ReportType == HIDS_REPORT_REFERENCE_REPORT_TYPE_INPUT_REPORT))) && (InputReportLength) && (InputReportData))
   {
      /* Acquire the specified HIDS Instance.                           */
      ServiceInfo = AcquireServiceInstance(BluetoothStackID, InstanceID);
      if(ServiceInfo)
      {
         /* Determine what type of input report is being notified and   */
         /* determine the Attribute Offset of the actual characteristic */
         /* being notified.                                             */
         switch(ReportType)
         {
            case rtBootMouseInputReport:
               AttributeOffset = ServiceInfo->BootMouseInputReportOffset;
               break;
            case rtBootKeyboardInputReport:
               AttributeOffset = ServiceInfo->BootKeyboardInputReportOffset;
               break;
            case rtReport:
               AttributeOffset = ReportReferenceToOffset(ServiceInfo, ReportReferenceData);
               break;
            default:
               AttributeOffset = 0;
               break;
         }

         /* Verify that we correctly determined the attribute offset of */
         /* the characteristic that is being notified.                  */
         if(AttributeOffset)
         {
            /* Now go ahead and send the input report notification.     */
            ret_val = GATT_Handle_Value_Notification(BluetoothStackID, ServiceInfo->ServiceID, ConnectionID, (Word_t)AttributeOffset, InputReportLength, InputReportData);
         }
         else
            ret_val = HIDS_ERROR_INVALID_PARAMETER;

         /* UnLock the previously locked Bluetooth Stack.               */
         BSC_UnLockBluetoothStack(ServiceInfo->BluetoothStackID);
      }
      else
         ret_val = HIDS_ERROR_INVALID_INSTANCE_ID;
   }
   else
      ret_val = HIDS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* HIDS Client API.                                                  */

   /* The following function is responsible for parsing a value received*/
   /* from a remote HIDS Server and interpreting it as a HID Information*/
   /* value.  The first parameter is the length of the value returned by*/
   /* the remote HIDS Server.  The second parameter is a pointer to the */
   /* data returned by the remote HIDS Server.  The final parameter is a*/
   /* pointer to store the parsed HID Information value (if successful).*/
   /* This function returns a zero if successful or a negative return   */
   /* error code if an error occurs.                                    */
int BTPSAPI HIDS_Decode_HID_Information(unsigned int ValueLength, Byte_t *Value, HIDS_HID_Information_Data_t *HIDSHIDInformation)
{
   int ret_val;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((ValueLength == HIDS_HID_INFORMATION_SIZE) && (Value) && (HIDSHIDInformation))
   {
      /* Data appears to be valid.  Attempt to parse the HID            */
      /* Information.                                                   */
      HIDSHIDInformation->Version     = READ_UNALIGNED_WORD_LITTLE_ENDIAN(&(((HIDS_HID_Information_t *)Value)->HID_Version));
      HIDSHIDInformation->CountryCode = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(((HIDS_HID_Information_t *)Value)->CountryCode));
      HIDSHIDInformation->Flags       = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(((HIDS_HID_Information_t *)Value)->Flags));

      /* Return success to the caller.                                  */
      ret_val                         = 0;
   }
   else
   {
      if(ValueLength != HIDS_HID_INFORMATION_SIZE)
         ret_val = HIDS_ERROR_MALFORMATTED_DATA;
      else
         ret_val = HIDS_ERROR_INVALID_PARAMETER;
   }

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for parsing a value received*/
   /* from a remote HIDS Server and interpreting it as a Report         */
   /* Reference value.  The first parameter is the length of the value  */
   /* returned by the remote HIDS Server.  The second parameter is a    */
   /* pointer to the data returned by the remote HIDS Server.  The final*/
   /* parameter is a pointer to store the parsed Report Reference data  */
   /* (if successful).  This function returns a zero if successful or a */
   /* negative return error code if an error occurs.                    */
int BTPSAPI HIDS_Decode_Report_Reference(unsigned int ValueLength, Byte_t *Value, HIDS_Report_Reference_Data_t *ReportReferenceData)
{
   int ret_val;

   /* Make sure the parameters passed to us are semi-valid.             */
   if((ValueLength == HIDS_REPORT_REFERENCE_SIZE) && (Value) && (ReportReferenceData))
   {
      /* Data appears to be valid.  Attempt to parse the HID Report     */
      /* Reference.                                                     */
      ReportReferenceData->ReportID   = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(((HIDS_Report_Reference_t *)Value)->Report_ID));
      ReportReferenceData->ReportType = READ_UNALIGNED_BYTE_LITTLE_ENDIAN(&(((HIDS_Report_Reference_t *)Value)->Report_Type));

      /* Return success to the caller.                                  */
      ret_val                         = 0;
   }
   else
   {
      if(ValueLength != HIDS_REPORT_REFERENCE_SIZE)
         ret_val = HIDS_ERROR_MALFORMATTED_DATA;
      else
         ret_val = HIDS_ERROR_INVALID_PARAMETER;
   }

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for parsing a value received*/
   /* from a remote HIDS Server and interpreting it as a External Report*/
   /* Reference value.  The first parameter is the length of the value  */
   /* returned by the remote HIDS Server.  The second parameter is a    */
   /* pointer to the data returned by the remote HIDS Server.  The final*/
   /* parameter is a pointer to store the parsed External Report        */
   /* Reference data (if successful).  This function returns a zero if  */
   /* successful or a negative return error code if an error occurs.    */
int BTPSAPI HIDS_Decode_External_Report_Reference(unsigned int ValueLength, Byte_t *Value, GATT_UUID_t *ExternalReportReferenceUUID)
{
   int ret_val;

   /* Make sure the parameters passed to us are semi-valid.             */
   if(((ValueLength == UUID_16_SIZE) || (ValueLength == UUID_128_SIZE)) && (Value) && (ExternalReportReferenceUUID))
   {
      /* Data appears to be valid.  Attempt to parse the HID Exernal    */
      /* Report Reference.                                              */
      if(ValueLength == UUID_16_SIZE)
      {
         ExternalReportReferenceUUID->UUID_Type = guUUID_16;
         BTPS_MemCopy(&(ExternalReportReferenceUUID->UUID.UUID_16), Value, UUID_16_SIZE);
      }
      else
      {
         ExternalReportReferenceUUID->UUID_Type = guUUID_128;
         BTPS_MemCopy(&(ExternalReportReferenceUUID->UUID.UUID_128), Value, UUID_128_SIZE);
      }

      /* Return success to the caller.                                  */
      ret_val = 0;
   }
   else
   {
      if((ValueLength != UUID_16_SIZE) && (ValueLength != UUID_128_SIZE))
         ret_val = HIDS_ERROR_MALFORMATTED_DATA;
      else
         ret_val = HIDS_ERROR_INVALID_PARAMETER;
   }

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for formatting a HIDS       */
   /* Protocol Mode into a user specified buffer.  The first parameter  */
   /* is the command to format.  The final two parameters contain the   */
   /* length of the buffer, and the buffer, to format the Protocol Mode */
   /* into.  This function returns a zero if successful or a negative   */
   /* return error code if an error occurs.                             */
   /* * NOTE * The BufferLength and Buffer parameter must point to a    */
   /*          buffer of at least HIDS_PROTOCOL_MODE_VALUE_LENGTH in    */
   /*          size.                                                    */
int BTPSAPI HIDS_Format_Protocol_Mode(HIDS_Protocol_Mode_t ProtocolMode, unsigned int BufferLength, Byte_t *Buffer)
{
   int ret_val;

   /* Make sure the parameters passed to us are semi-valid.             */
   if(((ProtocolMode == pmBoot) || (ProtocolMode == pmReport)) && (BufferLength >= HIDS_PROTOCOL_MODE_VALUE_LENGTH) && (Buffer))
   {
      /* Format the value.                                              */
      if(ProtocolMode == pmBoot)
      {
         ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(Buffer, HIDS_PROTOCOL_MODE_BOOT);
      }
      else
      {
         ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(Buffer, HIDS_PROTOCOL_MODE_REPORT);
      }

      /* Return success to the caller.                                  */
      ret_val = 0;
   }
   else
      ret_val = HIDS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}

   /* The following function is responsible for formatting a HIDS       */
   /* Control Point Command into a user specified buffer.  The first    */
   /* parameter is the command to format.  The final two parameters     */
   /* contain the length of the buffer, and the buffer, to format the   */
   /* command into.  This function returns a zero if successful or a    */
   /* negative return error code if an error occurs.                    */
   /* * NOTE * The BufferLength and Buffer parameter must point to a    */
   /*          buffer of at least HIDS_CONTROL_POINT_VALUE_LENGTH in    */
   /*          size.                                                    */
int BTPSAPI HIDS_Format_Control_Point_Command(HIDS_Control_Point_Command_t Command, unsigned int BufferLength, Byte_t *Buffer)
{
   int ret_val;

   /* Make sure the parameters passed to us are semi-valid.             */
   if(((Command == pcSuspend) || (Command == pcExitSuspend)) && (BufferLength >= HIDS_CONTROL_POINT_VALUE_LENGTH) && (Buffer))
   {
      /* Format the value.                                              */
      if(Command == pcSuspend)
      {
         ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(Buffer, HIDS_CONTROL_POINT_COMMAND_SUSPEND);
      }
      else
      {
         ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(Buffer, HIDS_CONTROL_POINT_COMMAND_EXIT_SUSPEND);
      }

      /* Return success to the caller.                                  */
      ret_val = 0;
   }
   else
      ret_val = HIDS_ERROR_INVALID_PARAMETER;

   /* Finally return the result to the caller.                          */
   return(ret_val);
}
