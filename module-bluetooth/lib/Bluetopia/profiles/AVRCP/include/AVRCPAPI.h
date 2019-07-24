/*
 * Copyright 2006 - 2014 Stonestreet One.
 * All Rights Reserved.
 * Author:  Nikhil Deo
 *** MODIFICATION HISTORY ****************************************************
 *
 *   mm/dd/yy  F. Lastname    Description of Modification
 *   --------  -----------    ------------------------------------------------
 *   01/09/06  N. Deo            Initial creation.
 ****************************************************************************
 */

/**
 *  @file AVRCPAPI.h
 *
 *  @brief  Stonestreet One Bluetooth Stack Audio/Video Remote Control
 *      Profile Type Definitions and Constants.
 *
 *  @code
 *  #include "SS1BTAVR.h"
 *  @endcode
 *  ============================================================================
 */
#ifndef __AVRCPAPIH__
#define __AVRCPAPIH__

#include "SS1BTPS.h"       /*! Bluetooth Stack API Prototypes/Constants. */

#include "AVRTypes.h"      /*! Audio/Video Remote Control Type Definitions. */

   /*! Error Return Codes.
    * Error Codes that are smaller than these (less than -1000) are
    * related to the Bluetooth Protocol Stack itself (see BTERRORS.H).
    */
#define BTAVRCP_ERROR_INVALID_PARAMETER                                  (-2000)
#define BTAVRCP_ERROR_NOT_INITIALIZED                                    (-2001)
#define BTAVRCP_ERROR_INVALID_BLUETOOTH_STACK_ID                         (-2002)
#define BTAVRCP_ERROR_LIBRARY_INITIALIZATION_ERROR                       (-2003)
#define BTAVRCP_ERROR_INSUFFICIENT_RESOURCES                             (-2004)
#define BTAVRCP_ERROR_MESSAGE_TOO_LONG                                   (-2005)
#define BTAVRCP_ERROR_BUFFER_TOO_SMALL                                   (-2006)
#define BTAVRCP_ERROR_UNABLE_TO_DECODE_MESSAGE                           (-2007)
#define BTAVRCP_ERROR_INVALID_MESSAGE_FRAGMENT_DATA                      (-2008)
#define BTAVRCP_ERROR_BROWSING_CHANNEL_MTU_SIZE_TO_SMALL                 (-2009)
#define BTAVRCP_ERROR_UNABLE_TO_DECODE_VENDOR_DEPENDENT                  (-2010)

  /*! The underlying Transport for AVRCP is the Audio/Video Control
    * Transport Protocol (AVCTP).  Rather than duplicate ALL profile
    * connection management functions, AVRCP will need to call the
    * functions provided in the AVCTP library to perform AVRCP actions.
    * This library will merely provide utility functions that can be used
    * to build AVRCP messages that can be directly passed to AVCTP
    * directly.
    * The following enumerated type is used with the
    * AVRCP_Register_SDP_Record_Version() function to specify a specific
    * AVRCP Version to be supported.  This allows a device to only
    * support a certain version and not some of the more advanced
    * features introduced in later profile versions.
    * \note The original SDP Registration function,
    *          AVRCP_Register_SDP_Record(), merely registers an SDP
    *          record claiming support for the latest profile version.
    */
typedef enum
{
   apvVersion1_0,
   apvVersion1_3,
   apvVersion1_4
} AVRCP_Version_t;

   /*! The following enumerated type represents all of the packet types
    * that can be decoded in a AVRCP_Message_Information_t structure.
    */
typedef enum
{
   amtUnknown,
   amtUnitInfo,
   amtSubunitInfo,
   amtPassThrough,
   amtVendorDependent_Generic,
   amtBrowsingChannel_Generic,
   amtFragmentedMessage,
   amtGroupNavigation,
   amtGetCapabilities,
   amtListPlayerApplicationSettingAttributes,
   amtListPlayerApplicationSettingValues,
   amtGetCurrentPlayerApplicationSettingValue,
   amtSetPlayerApplicationSettingValue,
   amtGetPlayerApplicationSettingAttributeText,
   amtGetPlayerApplicationSettingValueText,
   amtInformDisplayableCharacterSet,
   amtInformBatteryStatusOfCT,
   amtGetElementAttributes,
   amtGetPlayStatus,
   amtRegisterNotification,
   amtRequestContinuingResponse,
   amtAbortContinuingResponse,
   amtSetAbsoluteVolume,
   amtCommandRejectResponse,
   amtSetAddressedPlayer,
   amtPlayItem,
   amtAddToNowPlaying,
   amtSetBrowsedPlayer,
   amtChangePath,
   amtGetItemAttributes,
   amtSearch,
   amtGetFolderItems,
   amtGeneralReject
} AVRCP_Message_Type_t;

  /*! The following structure represents an AV/C Message or Browsing
    * Message that was decoded from the AVRCP_Decode_Message() function
    * and it is of unknown type (amtUnknown).
    * \note This is only returned for AV/C formatted Messages and
    *          Browsing messages.
    */
typedef struct _tagAVRCP_Unknown_Command_Data_t
{
   Byte_t  PDU_ID;
   Word_t  ParameterLength;
   Byte_t *ParameterData;
} AVRCP_Unknown_Command_Data_t;

#define AVRCP_UNKNOWN_COMMAND_DATA_SIZE                              (sizeof(AVRCP_Unknown_Command_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Unit_Info_Command() function to build a UNIT INFO
    * Command based on the specified parameters.
    */
typedef struct _tagAVRCP_Unit_Info_Command_Data_t
{
   Byte_t CommandType;
   Byte_t SubunitType;
   Byte_t SubunitID;
   Byte_t UnitType;
   Byte_t Unit;
} AVRCP_Unit_Info_Command_Data_t;

#define AVRCP_UNIT_INFO_COMMAND_DATA_SIZE                            (sizeof(AVRCP_Unit_Info_Command_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Unit_Info_Response() function to build a UNIT INFO
    * Response message based on the specified parameters.
    */
typedef struct _tagAVRCP_Unit_Info_Response_Data_t
{
   Byte_t             ResponseCode;
   Byte_t             SubunitType;
   Byte_t             SubunitID;
   Byte_t             UnitType;
   Byte_t             Unit;
   AVRCP_Company_ID_t CompanyID;
} AVRCP_Unit_Info_Response_Data_t;

#define AVRCP_UNIT_INFO_RESPONSE_DATA_SIZE                           (sizeof(AVRCP_Unit_Info_Response_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Subunit_Info_Command() function to build a SUBUNIT
    * INFO Command message based on the specified parameters.
    */
typedef struct _tagAVRCP_Subunit_Info_Command_Data_t
{
   Byte_t CommandType;
   Byte_t SubunitType;
   Byte_t SubunitID;
   Byte_t Page;
} AVRCP_Subunit_Info_Command_Data_t;

#define AVRCP_SUBUNIT_INFO_COMMAND_DATA_SIZE                         (sizeof(AVRCP_Subunit_Info_Command_Data_t))

   /*! The following structure is a container structure used to hold an
    * individual SUBUNIT Table Entry in the SUBUNIT INFO Response.
    */
typedef struct _tagAVRCP_Subunit_Table_List_Entry_t
{
   Byte_t SubunitType;
   Byte_t MaxSubunitID;
} AVRCP_Subunit_Table_List_Entry_t;

#define AVRCP_SUBUNIT_TABLE_LIST_ENTRY_SIZE                          (sizeof(AVRCP_Subunit_Table_List_Entry_t))

   /*! The following structure is used with the
    * AVRCP_Format_Subunit_Info_Response() function to build a SUBUNIT
    * INFO Response message based on the specified parameters.
    */
typedef struct _tagAVRCP_Subunit_Info_Response_Data_t
{
   Byte_t                            ResponseCode;
   Byte_t                            SubunitType;
   Byte_t                            SubunitID;
   Byte_t                            Page;
   Byte_t                            NumberSubunitEntries;
   AVRCP_Subunit_Table_List_Entry_t *SubunitTable;
} AVRCP_Subunit_Info_Response_Data_t;

#define AVRCP_SUBUNIT_INFO_RESPONSE_DATA_SIZE                        (sizeof(AVRCP_Subunit_Info_Response_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Pass_Through_Command() function to build a Pass
    * Through Command message based on the specified parameters.
    * \note The StateFlag member specifies button down (FALSE) or
    *          button up (TRUE).
    */
typedef struct _tagAVRCP_Pass_Through_Command_Data_t
{
   Byte_t     CommandType;
   Byte_t     SubunitType;
   Byte_t     SubunitID;
   Byte_t     OperationID;
   Boolean_t  StateFlag;
   Byte_t     OperationDataLength;
   Byte_t    *OperationData;
} AVRCP_Pass_Through_Command_Data_t;

#define AVRCP_PASS_THROUGH_COMMAND_DATA_SIZE                         (sizeof(AVRCP_Pass_Through_Command_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Pass_Through_Response() function to build a Pass
    * Through Response message based on the specified parameters.
    * \note The StateFlag member specifies button down (FALSE) or
    *          button up (TRUE).
    */
typedef struct _tagAVRCP_Pass_Through_Response_Data_t
{
   Byte_t     ResponseCode;
   Byte_t     SubunitType;
   Byte_t     SubunitID;
   Byte_t     OperationID;
   Boolean_t  StateFlag;
   Byte_t     OperationDataLength;
   Byte_t    *OperationData;
} AVRCP_Pass_Through_Response_Data_t;

#define AVRCP_PASS_THROUGH_RESPONSE_DATA_SIZE                        (sizeof(AVRCP_Pass_Through_Response_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Vendor_Dependent_Generic_Command() function to build
    * a Generic Vendor Dependent Command message based on the specified
    * parameters.
    */
typedef struct _tagAVRCP_Vendor_Dependent_Generic_Command_Data_t
{
   Byte_t              CommandType;
   Byte_t              SubunitType;
   Byte_t              SubunitID;
   AVRCP_Company_ID_t  CompanyID;
   unsigned int        DataLength;
   Byte_t             *DataBuffer;
} AVRCP_Vendor_Dependent_Generic_Command_Data_t;

#define AVRCP_VENDOR_DEPENDENT_GENERIC_COMMAND_DATA_SIZE             (sizeof(AVRCP_Vendor_Dependent_Generic_Command_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Vendor_Dependent_Generic_Response() function to build
    * a Generic Vendor Dependent Response message based on the specified
    * parameters.
    */
typedef struct _tagAVRCP_Vendor_Dependent_Generic_Response_Data_t
{
   Byte_t              ResponseCode;
   Byte_t              SubunitType;
   Byte_t              SubunitID;
   AVRCP_Company_ID_t  CompanyID;
   unsigned int        DataLength;
   Byte_t             *DataBuffer;
} AVRCP_Vendor_Dependent_Generic_Response_Data_t;

#define AVRCP_VENDOR_DEPENDENT_GENERIC_RESPONSE_DATA_SIZE            (sizeof(AVRCP_Vendor_Dependent_Generic_Response_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Browsing_Channel_Generic_Message() function to build
    * a Generic Browsing Channel message based on the specified
    * parameters.
    */
typedef struct _tagAVRCP_Browsing_Channel_Generic_Message_Data_t
{
   Byte_t  PDU_ID;
   Word_t  ParameterLength;
   Byte_t *ParameterData;
} AVRCP_Browsing_Channel_Generic_Message_Data_t;

#define AVRCP_BROWSING_CHANNEL_GENERIC_MESSAGE_DATA_SIZE             (sizeof(AVRCP_Browsing_Channel_Generic_Message_Data_t))

   /*! The following structure is used with AVRCP Specific AV/C Response
    * Formatting functions to build a Response.  This state information
    * is required because it is possible that the messages require
    * fragmentation.
    */
typedef struct _tagAVRCP_Response_Message_State_Info_t
{
   Boolean_t    Complete;
   unsigned int Offset;
} AVRCP_Response_Message_State_Info_t;

#define AVRCP_RESPONSE_MESSAGE_STATE_INFO_SIZE                       (sizeof(AVRCP_Response_Message_State_Info_t))

   /*! The following structure is used to encapsulate an AVRCP Specific
    * AV/C Response Fragment.  Once all Fragments have been retrieved
    * the entire message can be decoded with the
    * AVRCP_Decode_Fragmented_Message() function.
    * \note Once all message fragments are retrieved, they can be
    *          decoded via a call to the
    *          AVRCP_Decode_Fragmented_Message_List() function.
    * \note Each individual fragment *MUST* be freed by either
    *          calling AVRCP_Free_Decoded_Message() on each individual
    *          fragment OR calling AVRCP_Free_Fragmented_Message_List()
    *          with the fragment list.
    */
typedef struct _tagAVRCP_Message_Fragment_Data_t
{
   Byte_t         PDU_ID;
   unsigned int   FragmentLength;
   unsigned char *FragmentData;
} AVRCP_Message_Fragment_Data_t;

#define AVRCP_MESSAGE_FRAGMENT_DATA_SIZE                             (sizeof(AVRCP_Message_Fragment_Data_t))

   /*! The following type is used to specify the various button states
    * (used with Vendor Pass-through formatted messages).
    */
typedef enum
{
   bsPressed,
   bsReleased
} AVRCP_Button_State_t;

   /*! The following structure is used with the
    * AVRCP_Format_Group_Navigation_Command() function to build a Group
    * Navigation Command message based on the specified parameters.
    * \note The NavigationType member maps to the Vendor Unique ID
    *          field of the Pass-through command.
    */
typedef struct _tagAVRCP_Group_Navigation_Command_Data_t
{
   AVRCP_Button_State_t ButtonState;
   Word_t               NavigationType;
} AVRCP_Group_Navigation_Command_Data_t;

#define AVRCP_GROUP_NAVIGATION_COMMAND_DATA_SIZE                     (sizeof(AVRCP_Group_Navigation_Command_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Group_Navigation_Response() function to build a Group
    * Navigation Response message based on the specified parameters.
    * \note The NavigationType member maps to the Vendor Unique ID
    *          field of the Pass-through command.
    */
typedef struct _tagAVRCP_Group_Navigation_Response_Data_t
{
   Byte_t               ResponseCode;
   AVRCP_Button_State_t ButtonState;
   Word_t               NavigationType;
} AVRCP_Group_Navigation_Response_Data_t;

#define AVRCP_GROUP_NAVIGATION_RESPONSE_DATA_SIZE                    (sizeof(AVRCP_Group_Navigation_Response_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Get_Capabilities_Command() function to build a Get
    * Capabilities Command message based on the specified parameters.
    */
typedef struct _tagAVRCP_Get_Capabilities_Command_Data_t
{
   Byte_t CapabilityID;
} AVRCP_Get_Capabilities_Command_Data_t;

#define AVRCP_GET_CAPABILITIES_COMMAND_DATA_SIZE                     (sizeof(AVRCP_Get_Capabilities_Command_Data_t))

   /*! The following structure is container structure that is used with
    * the CapabilityInfoList member of the
    * AVRCP_Get_Capabilities_Response_Data_t structure to denote an
    * individual Capability.
    */
typedef struct _tagAVRCP_Capability_Info_t
{
   union
   {
      AVRCP_Company_ID_t CompanyID;
      Byte_t             EventID;
   } CapabilityInfo;
} AVRCP_Capability_Info_t;

#define AVRCP_CAPABILITY_INFO_SIZE                                   (sizeof(AVRCP_Capability_Info_t))

   /*! The following structure is used with the
    * AVRCP_Format_Get_Capabilities_Response() function to build a Get
    * Capabilities Response message based on the specified parameters.
    */
typedef struct _tagAVRCP_Get_Capabilities_Response_Data_t
{
   Byte_t                   ResponseCode;
   Byte_t                   CapabilityID;
   Byte_t                   NumberCapabilities;
   AVRCP_Capability_Info_t *CapabilityInfoList;
} AVRCP_Get_Capabilities_Response_Data_t;

#define AVRCP_GET_CAPABILITIES_RESPONSE_DATA_SIZE                    (sizeof(AVRCP_Get_Capabilities_Response_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_List_Player_Application_Setting_Attributes_Response()
    * function to build a List Player Application Setting Attributes
    * Response message based on the specified parameters.
    */
typedef struct _tagAVRCP_List_Player_Application_Setting_Attributes_Response_Data_t
{
   Byte_t  ResponseCode;
   Byte_t  NumberAttributes;
   Byte_t *AttributeIDList;
} AVRCP_List_Player_Application_Setting_Attributes_Response_Data_t;

#define AVRCP_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES_RESPONSE_DATA_SIZE   (sizeof(AVRCP_List_Player_Application_Setting_Attributes_Response_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_List_Player_Application_Setting_Values_Command()
    * function to build a List Player Application Setting Values Command
    * message based on the specified parameters.
    */
typedef struct _tagAVRCP_List_Player_Application_Setting_Values_Command_Data_t
{
   Byte_t AttributeID;
} AVRCP_List_Player_Application_Setting_Values_Command_Data_t;

#define AVRCP_LIST_PLAYER_APPLICATION_SETTING_VALUES_COMMAND_DATA_SIZE     (sizeof(AVRCP_List_Player_Application_Setting_Values_Command_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_List_Player_Application_Setting_Values_Response()
    * function to build a List Player Application Setting Values
    * Response message based on the specified parameters.
    */
typedef struct _tagAVRCP_List_Player_Application_Setting_Values_Response_Data_t
{
   Byte_t  ResponseCode;
   Byte_t  NumberValueIDs;
   Byte_t *ValueIDList;
} AVRCP_List_Player_Application_Setting_Values_Response_Data_t;

#define AVRCP_LIST_PLAYER_APPLICATION_SETTING_VALUES_RESPONSE_DATA_SIZE    (sizeof(AVRCP_List_Player_Application_Setting_Values_Response_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Get_Current_Player_Application_Setting_Value_Command
    * function to build a Get Current Player Application Setting Value
    * Command message based on the specified parameters.
    */
typedef struct _tagAVRCP_Get_Current_Player_Application_Setting_Value_Command_Data_t
{
   Byte_t  NumberAttributeIDs;
   Byte_t *AttributeIDList;
} AVRCP_Get_Current_Player_Application_Setting_Value_Command_Data_t;

#define AVRCP_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE_COMMAND_DATA_SIZE  (sizeof(AVRCP_Get_Current_Player_Application_Setting_Value_Command_Data_t))

   /*! The following structure is container structure that is used with
    * the AttributeValueIDList member of the
    * AVRCP_Get_Current_Player_Application_Setting_Value_Response_Data_t
    * and AVRCP_Set_Player_Application_Setting_Value_Command_Data_t
    * structures to denote an individual AttributeID/Value ID pair.
    */
typedef struct _tagAVRCP_Attribute_Value_ID_List_Entry_t
{
   Byte_t AttributeID;
   Byte_t ValueID;
} AVRCP_Attribute_Value_ID_List_Entry_t;

#define AVRCP_ATTRIBUTE_VALUE_ID_LIST_ENTRY_SIZE                     (sizeof(AVRCP_Attribute_Value_ID_List_Entry_t))

   /*! The following structure is used with the
    * AVRCP_Format_Get_Current_Player_Application_Setting_Value_Response
    * function to build a Get Current Player Application Setting Value
    * Response message based on the specified parameters.
    */
typedef struct _tagAVRCP_Get_Current_Player_Application_Setting_Value_Response_Data_t
{
   Byte_t                                 ResponseCode;
   Byte_t                                 NumberAttributeValueIDs;
   AVRCP_Attribute_Value_ID_List_Entry_t *AttributeValueIDList;
} AVRCP_Get_Current_Player_Application_Setting_Value_Response_Data_t;

#define AVRCP_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE_RESPONSE_DATA_SIZE (sizeof(AVRCP_Get_Current_Player_Application_Setting_Value_Response_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Set_Player_Application_Setting_Value_Command()
    * function to build a Set Player Application Setting Value Command
    * message based on the specified parameters.
    */
typedef struct _tagAVRCP_Set_Player_Application_Setting_Value_Command_Data_t
{
   Byte_t                                 NumberAttributeValueIDs;
   AVRCP_Attribute_Value_ID_List_Entry_t *AttributeValueIDList;
} AVRCP_Set_Player_Application_Setting_Value_Command_Data_t;

#define AVRCP_SET_PLAYER_APPLICATION_SETTING_VALUE_COMMAND_DATA_SIZE    (sizeof(AVRCP_Set_Player_Application_Setting_Value_Command_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Set_Player_Application_Setting_Value_Response()
    * function to build a Set Player Application Setting Value Response
    * message based on the specified parameters.
    */
typedef struct _tagAVRCP_Set_Player_Application_Setting_Value_Response_Data_t
{
   Byte_t ResponseCode;
} AVRCP_Set_Player_Application_Setting_Value_Response_Data_t;

#define AVRCP_SET_PLAYER_APPLICATION_SETTING_VALUE_RESPONSE_DATA_SIZE   (sizeof(AVRCP_Set_Player_Application_Setting_Value_Response_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Get_Player_Application_Setting_Attribute_Text_Command
    * function to build a Get Player Application Setting Attribute Text
    * Command message based on the specified parameters.
    */
typedef struct _tagAVRCP_Get_Player_Application_Setting_Attribute_Text_Command_Data_t
{
   Byte_t  NumberAttributeIDs;
   Byte_t *AttributeIDList;
} AVRCP_Get_Player_Application_Setting_Attribute_Text_Command_Data_t;

#define AVRCP_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT_COMMAND_DATA_SIZE (sizeof(AVRCP_Get_Player_Application_Setting_Attribute_Text_Command_Data_t))

   /*! The following structure is container structure that is used with
    * the AttributeTextEntryList member of the
    * AVRCP_Get_Player_Application_Setting_Attribute_...
    *    ...Text_Response_Data_t structure to denote an individual
    * Attribute Text Entry.
    */
typedef struct _tagAVRCP_Attribute_Text_List_Entry_t
{
   Byte_t  AttributeID;
   Word_t  CharacterSet;
   Byte_t  AttributeStringLength;
   Byte_t *AttributeStringData;
} AVRCP_Attribute_Text_List_Entry_t;

#define AVRCP_ATTRIBUTE_TEXT_LIST_ENTRY_SIZE                         (sizeof(AVRCP_Attribute_Text_List_Entry_t))

   /*! The following structure is used with the
    * AVRCP_Format_Get_Player_Application_Setting_...
    *    ...Attribute_Text_Response() function to build a Get Player
    * Application Setting Attribute Text Response message based on the
    * specified parameters.
    */
typedef struct _tagAVRCP_Get_Player_Application_Setting_Attribute_Text_Response_Data_t
{
   Byte_t                             ResponseCode;
   Byte_t                             NumberAttributeTextEntries;
   AVRCP_Attribute_Text_List_Entry_t *AttributeTextEntryList;
} AVRCP_Get_Player_Application_Setting_Attribute_Text_Response_Data_t;

#define AVRCP_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT_RESPONSE_DATA_SIZE   (sizeof(AVRCP_Get_Player_Application_Setting_Attribute_Text_Response_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Get_Player_Application_Setting_Value_Text_Command
    * function to build a Get Player Application Setting Value Text
    * Command message based on the specified parameters.
    */
typedef struct _tagAVRCP_Get_Player_Application_Setting_Value_Text_Command_Data_t
{
   Byte_t  AttributeID;
   Byte_t  NumberValueIDs;
   Byte_t *ValueIDList;
} AVRCP_Get_Player_Application_Setting_Value_Text_Command_Data_t;

#define AVRCP_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT_COMMAND_DATA_SIZE  (sizeof(AVRCP_Get_Player_Application_Setting_Value_Text_Command_Data_t))

   /*! The following structure is container structure that is used with
    * the ValueTextEntryList member of the
    * AVRCP_Get_Player_Application_Setting_Value_Text_Response_Data_t
    * structure to denote an individual Value Text Entry.
    */
typedef struct _tagAVRCP_Value_Text_List_Entry_t
{
   Byte_t  ValueID;
   Word_t  CharacterSet;
   Byte_t  ValueStringLength;
   Byte_t *ValueStringData;
} AVRCP_Value_Text_List_Entry_t;

#define AVRCP_VALUE_TEXT_LIST_ENTRY_SIZE                             (sizeof(AVRCP_Value_Text_List_Entry_t))

   /*! The following structure is used with the
    * AVRCP_Format_Get_Player_Application_Setting_Value_Text_Response()
    * function to build a Get Player Application Setting Value Text
    * Response message based on the specified parameters.
    */
typedef struct _tagAVRCP_Get_Player_Application_Setting_Value_Text_Response_Data_t
{
   Byte_t                         ResponseCode;
   Byte_t                         NumberValueTextEntries;
   AVRCP_Value_Text_List_Entry_t *ValueTextEntryList;
} AVRCP_Get_Player_Application_Setting_Value_Text_Response_Data_t;

#define AVRCP_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT_RESPONSE_DATA_SIZE    (sizeof(AVRCP_Get_Player_Application_Setting_Value_Text_Response_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Inform_Displayable_Character_Set_Command() function
    * to build an Inform Displayable Character Set Command message based
    * on the specified parameters.
    */
typedef struct _tagAVRCP_Inform_Displayable_Character_Set_Command_Data_t
{
   Byte_t  NumberCharacterSets;
   Word_t *CharacterSetList;
} AVRCP_Inform_Displayable_Character_Set_Command_Data_t;

#define AVRCP_INFORM_DISPLAYABLE_CHARACTER_SET_COMMAND_DATA_SIZE     (sizeof(AVRCP_Inform_Displayable_Character_Set_Command_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Inform_Displayable_Character_Set_Response() function
    * to build an Inform Displayable Character Set Response message
    * based on the specified parameters.
    */
typedef struct _tagAVRCP_Inform_Displayable_Character_Set_Response_Data_t
{
   Byte_t ResponseCode;
} AVRCP_Inform_Displayable_Character_Set_Response_Data_t;

#define AVRCP_INFORM_DISPLAYABLE_CHARACTER_SET_RESPONSE_DATA_SIZE    (sizeof(AVRCP_Inform_Displayable_Character_Set_Response_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Inform_Battery_Status_Of_CT_Command() function to
    * build an Inform Battery Status Of CT Command message based on the
    * specified parameters.
    */
typedef struct _tagAVRCP_Inform_Battery_Status_Of_CT_Command_Data_t
{
   Byte_t BatteryStatus;
} AVRCP_Inform_Battery_Status_Of_CT_Command_Data_t;

#define AVRCP_INFORM_BATTERY_STATUS_OF_CT_COMMAND_DATA_SIZE          (sizeof(AVRCP_Inform_Battery_Status_Of_CT_Command_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Inform_Battery_Status_Of_CT_Response() function to
    * build an Inform Battery Status Of CT Response message based on the
    * specified parameters.
    */
typedef struct _tagAVRCP_Inform_Battery_Status_Of_CT_Response_Data_t
{
   Byte_t ResponseCode;
} AVRCP_Inform_Battery_Status_Of_CT_Response_Data_t;

#define AVRCP_INFORM_BATTERY_STATUS_OF_CT_RESPONSE_DATA_SIZE         (sizeof(AVRCP_Inform_Battery_Status_Of_CT_Response_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Get_Element_Attributes_Command() function to build a
    * Get Element Attributes Command message based on the specified
    * parameters.
    */
typedef struct _tagAVRCP_Get_Element_Attributes_Command_Data_t
{
   QWord_t  Identifier;
   Byte_t   NumberAttributes;
   DWord_t *AttributeIDList;
} AVRCP_Get_Element_Attributes_Command_Data_t;

#define AVRCP_GET_ELEMENT_ATTRIBUTES_COMMAND_DATA_SIZE               (sizeof(AVRCP_Get_Element_Attributes_Command_Data_t))

   /*! The following structure is container structure that is used with
    * the ElementAttributeList member of the
    * AVRCP_Get_Element_Attributes_Response_Data_t and
    * AVRCP_Media_Element_Item_Data_t structures to denote an individual
    * Element Attribute Entry.
    */
typedef struct _tagAVRCP_Element_Attribute_List_Entry_t
{
   DWord_t  AttributeID;
   Word_t   CharacterSet;
   Word_t   AttributeValueLength;
   Byte_t  *AttributeValueData;
} AVRCP_Element_Attribute_List_Entry_t;

#define AVRCP_ELEMENT_ATTRIBUTE_LIST_ENTRY_SIZE                      (sizeof(AVRCP_Element_Attribute_List_Entry_t))

   /*! The following structure is used with the
    * AVRCP_Format_Get_Element_Attributes_Response() function to build a
    * Get Element Attributes Response message based on the specified
    * parameters.
    */
typedef struct _tagAVRCP_Get_Element_Attributes_Response_Data_t
{
   Byte_t                                ResponseCode;
   Byte_t                                NumberAttributes;
   AVRCP_Element_Attribute_List_Entry_t *AttributeList;
} AVRCP_Get_Element_Attributes_Response_Data_t;

#define AVRCP_GET_ELEMENT_ATTRIBUTES_RESPONSE_DATA_SIZE              (sizeof(AVRCP_Get_Element_Attributes_Response_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Get_Play_Status_Response() function to build a Get
    * Play Status Response message based on the specified parameters.
    */
typedef struct _tagAVRCP_Get_Play_Status_Response_Data_t
{
   Byte_t  ResponseCode;
   DWord_t SongLength;
   DWord_t SongPosition;
   Byte_t  PlayStatus;
} AVRCP_Get_Play_Status_Response_Data_t;

#define AVRCP_GET_PLAY_STATUS_RESPONSE_DATA_SIZE                     (sizeof(AVRCP_Get_Play_Status_Response_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Register_Notification_Command() function to build a
    * Register Notification Command message based on the specified
    * parameters.
    */
typedef struct _tagAVRCP_Register_Notification_Command_Data_t
{
   Byte_t  EventID;
   DWord_t PlaybackInterval;
} AVRCP_Register_Notification_Command_Data_t;

#define AVRCP_REGISTER_NOTIFICATION_COMMAND_DATA_SIZE                (sizeof(AVRCP_Register_Notification_Command_Data_t))

   /*! The following structure is a container structure for the Playback
    * Status Changed Event Registration Notification.  This is used with
    * the AVRCP_Register_Notification_Response_Data_t structure for
    * Event Registration/Notifications.
    */
typedef struct _tagAVRCP_Event_Playback_Status_Changed_Data_t
{
   Byte_t PlayStatus;
} AVRCP_Event_Playback_Status_Changed_Data_t;

#define AVRCP_EVENT_PLAYBACK_STATUS_CHANGED_DATA_SIZE                (sizeof(AVRCP_Event_Playback_Status_Changed_Data_t))

   /*! The following structure is a container structure for the Track
    * Changed Event Registration Notification.  This is used with the
    * AVRCP_Register_Notification_Response_Data_t structure for Event
    * Registration/Notifications.
    */
typedef struct _tagAVRCP_Event_Track_Changed_Data_t
{
   QWord_t Identifier;
} AVRCP_Event_Track_Changed_Data_t;

#define AVRCP_EVENT_TRACK_CHANGED_DATA_SIZE                          (sizeof(AVRCP_Event_Track_Changed_Data_t))

   /*! The following structure is a container structure for the Playback
    * Position Changed Event Registration Notification.  This is used
    * with the AVRCP_Register_Notification_Response_Data_t structure for
    * Event Registration/Notifications.
    */
typedef struct _tagAVRCP_Event_Playback_Pos_Changed_Data_t
{
   DWord_t PlaybackPosition;
} AVRCP_Event_Playback_Pos_Changed_Data_t;

#define AVRCP_EVENT_PLAYBACK_POS_CHANGED_DATA_SIZE                   (sizeof(AVRCP_Event_Playback_Pos_Changed_Data_t))

   /*! The following structure is a container structure for the Battery
    * Status Changed Event Registration Notification.  This is used with
    * the AVRCP_Register_Notification_Response_Data_t structure for
    * Event Registration/Notifications.
    */
typedef struct _tagAVRCP_Event_Batt_Status_Changed_Data_t
{
   Byte_t BatteryStatus;
} AVRCP_Event_Batt_Status_Changed_Data_t;

#define AVRCP_EVENT_BATT_STATUS_CHANGED_DATA_SIZE                    (sizeof(AVRCP_Event_Batt_Status_Changed_Data_t))

   /*! The following structure is a container structure for the System
    * Status Changed Event Registration Notification.  This is used with
    * the AVRCP_Register_Notification_Response_Data_t structure for
    * Event Registration/Notifications.
    */
typedef struct _tagAVRCP_Event_System_Status_Changed_Data_t
{
   Byte_t SystemStatus;
} AVRCP_Event_System_Status_Changed_Data_t;

#define AVRCP_EVENT_SYSTEM_STATUS_CHANGED_DATA_SIZE                  (sizeof(AVRCP_Event_System_Status_Changed_Data_t))

   /*! The following structure is a container structure for the Player
    * Application Setting Changed Event Registration Notification.  This
    * is used with the AVRCP_Register_Notification_Response_Data_t
    * structure for Event Registration/Notifications.
    */
typedef struct _tagAVRCP_Event_Player_Application_Setting_Changed_Data_t
{
   Byte_t                                 NumberAttributeValueIDs;
   AVRCP_Attribute_Value_ID_List_Entry_t *AttributeValueIDList;
} AVRCP_Event_Player_Application_Setting_Changed_Data_t;

#define AVRCP_EVENT_PLAYER_APPLICATION_SETTING_CHANGED_DATA_SIZE     (sizeof(AVRCP_Event_Player_Application_Setting_Changed_Data_t))

   /*! The following structure is a container structure for the Event
    * Volume Changed Event Registration Notification.  This is used with
    * the AVRCP_Register_Notification_Response_Data_t structure for
    * Event Registration/Notifications.
    */
typedef struct _tagAVRCP_Event_Volume_Changed_Data_t
{
   Byte_t AbsoluteVolume;
} AVRCP_Event_Volume_Changed_Data_t;

#define AVRCP_EVENT_VOLUME_CHANGED_DATA_SIZE                         (sizeof(AVRCP_Event_Volume_Changed_Data_t))

   /*! The following structure is a container structure for the Addressed
    * Player Changed Event Registration Notification.  This is used with
    * the AVRCP_Register_Notification_Response_Data_t structure for
    * Event Registration/Notifications.
    */
typedef struct _tagAVRCP_Event_Addressed_Player_Changed_Data_t
{
   Word_t MediaPlayerID;
   Word_t UIDCounter;
} AVRCP_Event_Addressed_Player_Changed_Data_t;

#define AVRCP_EVENT_ADDRESSED_PLAYER_CHANGED_DATA_SIZE               (sizeof(AVRCP_Event_Addressed_Player_Changed_Data_t))

   /*! The following structure is a container structure for the UIDs
    * Changed Event Registration Notification.  This is used with the
    * AVRCP_Register_Notification_Response_Data_t structure for Event
    * Registration/Notifications.
    */
typedef struct _tagAVRCP_Event_UIDs_Changed_Data_t
{
   Word_t UIDCounter;
} AVRCP_Event_UIDs_Changed_Data_t;

#define AVRCP_EVENT_UIDS_CHANGED_DATA_SIZE                           (sizeof(AVRCP_Event_UIDs_Changed_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Register_Notification_Response() to build a Register
    * Notification Response message based on the specified parameters.
    */
typedef struct _tagAVRCP_Register_Notification_Response_Data_t
{
   Byte_t ResponseCode;
   Byte_t EventID;
   union
   {
      AVRCP_Event_Playback_Status_Changed_Data_t            PlaybackStatusChangedData;
      AVRCP_Event_Track_Changed_Data_t                      TrackChangedData;
      AVRCP_Event_Playback_Pos_Changed_Data_t               PlaybackPosChangedData;
      AVRCP_Event_Batt_Status_Changed_Data_t                BattStatusChangedData;
      AVRCP_Event_System_Status_Changed_Data_t              SystemStatusChangedData;
      AVRCP_Event_Player_Application_Setting_Changed_Data_t PlayerApplicationSettingChangedData;
      AVRCP_Event_Volume_Changed_Data_t                     VolumeChangedData;
      AVRCP_Event_Addressed_Player_Changed_Data_t           AddressedPlayerChangedData;
      AVRCP_Event_UIDs_Changed_Data_t                       UIDsChangedData;
   } NotificationData;
} AVRCP_Register_Notification_Response_Data_t;

#define AVRCP_REGISTER_NOTIFICATION_RESPONSE_DATA_SIZE               (sizeof(AVRCP_Register_Notification_Response_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Request_Continuing_Response_Command() function to
    * build a Request Continuing Response Command message based on the
    * specified parameters.
    */
typedef struct _tagAVRCP_Request_Continuing_Response_Command_Data_t
{
   AVRCP_Message_Type_t MessageType;
} AVRCP_Request_Continuing_Response_Command_Data_t ;

#define AVRCP_REQUEST_CONTINUING_RESPONSE_COMMAND_DATA_SIZE          (sizeof(AVRCP_Request_Continuing_Response_Command_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Abort_Continuing_Response_Command() function to build
    * an Abort Continuing Response Command message based on the
    * specified parameters.
    */
typedef struct _tagAVRCP_Abort_Continuing_Response_Command_Data_t
{
   AVRCP_Message_Type_t MessageType;
} AVRCP_Abort_Continuing_Response_Command_Data_t ;

#define AVRCP_ABORT_CONTINUING_RESPONSE_COMMAND_DATA_SIZE            (sizeof(AVRCP_Abort_Continuing_Response_Command_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Abort_Continuing_Response_Response() function to
    * build an Abort Continuing Response Response message based on the
    * specified parameters.
    */
typedef struct _tagAVRCP_Abort_Continuing_Response_Response_Data_t
{
   Byte_t ResponseCode;
} AVRCP_Abort_Continuing_Response_Response_Data_t ;

#define AVRCP_ABORT_CONTINUING_RESPONSE_RESPONSE_DATA_SIZE           (sizeof(AVRCP_Abort_Continuing_Response_Response_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Set_Absolute_Volume_Command() function to build a Set
    * Absolute Volume Command message based on the specified parameters.
    */
typedef struct _tagAVRCP_Set_Absolute_Volume_Command_Data_t
{
   Byte_t AbsoluteVolume;
} AVRCP_Set_Absolute_Volume_Command_Data_t ;

#define AVRCP_SET_ABSOLUTE_VOLUME_COMMAND_DATA_SIZE                  (sizeof(AVRCP_Set_Absolute_Volume_Command_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Set_Absolute_Volume_Response() function to build a
    * Set Absolute Volume Response message based on the specified
    * parameters.
    */
typedef struct _tagAVRCP_Set_Absolute_Volume_Response_Data_t
{
   Byte_t ResponseCode;
   Byte_t AbsoluteVolume;
} AVRCP_Set_Absolute_Volume_Response_Data_t ;

#define AVRCP_SET_ABSOLUTE_VOLUME_RESPONSE_DATA_SIZE                 (sizeof(AVRCP_Set_Absolute_Volume_Response_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Command_Reject_Response() function to build a Generic
    * Command Reject Response message based on the specified parameters.
    * \note If the MessageType member specifies amtUnknown then the
    *          PDU_ID value is used directly in the Reject Response.
    */
typedef struct _tagAVRCP_Command_Reject_Response_Data_t
{
  Byte_t               ResponseCode;
  AVRCP_Message_Type_t MessageType;
  Byte_t               PDU_ID;
  Byte_t               ErrorCode;
} AVRCP_Command_Reject_Response_Data_t;

#define AVRCP_COMMAND_REJECT_RESPONSE_DATA_SIZE                      (sizeof(AVRCP_Command_Reject_Response_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Set_Addressed_Player_Command() function to build a
    * Set Addressed Player Command message based on the specified
    * parameters.
    */
typedef struct _tagAVRCP_Set_Addressed_Player_Command_Data_t
{
   Word_t MediaPlayerID;
} AVRCP_Set_Addressed_Player_Command_Data_t;

#define AVRCP_SET_ADDRESSED_PLAYER_COMMAND_DATA_SIZE                 (sizeof(AVRCP_Set_Addressed_Player_Command_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Set_Addressed_Player_Response() function to build a
    * Set Addressed Player Response message based on the specified
    * parameters.
    */
typedef struct _tagAVRCP_Set_Addressed_Player_Response_Data_t
{
   Byte_t ResponseCode;
   Byte_t Status;
} AVRCP_Set_Addressed_Player_Response_Data_t ;

#define AVRCP_SET_ADDRESSED_PLAYER_RESPONSE_DATA_SIZE                (sizeof(AVRCP_Set_Addressed_Player_Response_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Play_Item_Command() function to build a Play Item
    * Command message based on the specified parameters.
    */
typedef struct _tagAVRCP_Play_Item_Command_Data_t
{
   Byte_t  Scope;
   QWord_t UID;
   Word_t  UIDCounter;
} AVRCP_Play_Item_Command_Data_t;

#define AVRCP_PLAY_ITEM_COMMAND_DATA_SIZE                            (sizeof(AVRCP_Play_Item_Command_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Play_Item_Response() function to build a Play Item
    * Response message based on the specified parameters.
    */
typedef struct _tagAVRCP_Play_Item_Response_Data_t
{
   Byte_t ResponseCode;
   Byte_t Status;
} AVRCP_Play_Item_Response_Data_t ;

#define AVRCP_PLAY_ITEM_RESPONSE_DATA_SIZE                           (sizeof(AVRCP_Play_Item_Response_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Add_To_Now_Playing_Command() function to build an Add
    * To Now Playing Command message based on the specified parameters.
    */
typedef struct _tagAVRCP_Add_To_Now_Playing_Command_Data_t
{
   Byte_t  Scope;
   QWord_t UID;
   Word_t  UIDCounter;
} AVRCP_Add_To_Now_Playing_Command_Data_t;

#define AVRCP_ADD_TO_NOW_PLAYING_COMMAND_DATA_SIZE                   (sizeof(AVRCP_Add_To_Now_Playing_Command_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Add_To_Now_Playing_Response() function to build an
    * Add To Now Playing Response message based on the specified
    * parameters.
    */
typedef struct _tagAVRCP_Add_To_Now_Playing_Response_Data_t
{
   Byte_t ResponseCode;
   Byte_t Status;
} AVRCP_Add_To_Now_Playing_Response_Data_t ;

#define AVRCP_ADD_TO_NOW_PLAYING_RESPONSE_DATA_SIZE                  (sizeof(AVRCP_Add_To_Now_Playing_Response_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Set_Browsed_Player_Command() function to build a Set
    * Browsed Player Command message based on the specified parameters
    * (this message is only applicable for the Browsing Channel).
    */
typedef struct _tagAVRCP_Set_Browsed_Player_Command_Data_t
{
   Word_t PlayerID;
} AVRCP_Set_Browsed_Player_Command_Data_t;

#define AVRCP_SET_BROWSED_PLAYER_COMMAND_DATA_SIZE                   (sizeof(AVRCP_Set_Browsed_Player_Command_Data_t))

   /*! The following structure is container structure that is used with
    * the FolderNameList member of the
    * AVRCP_Set_Browsed_Player_Response_Data_t structure to denote an
    * individual Folder Name (in the overall path).
    */
typedef struct _tagAVRCP_Folder_Name_List_Entry_t
{
   Word_t  FolderNameLength;
   Byte_t *FolderName;
} AVRCP_Folder_Name_List_Entry_t;

#define AVRCP_FOLDER_NAME_LIST_ENTRY_SIZE                            (sizeof(AVRCP_Folder_Name_List_Entry_t))

   /*! The following structure is used with the
    * AVRCP_Format_Set_Browsed_Player_Response() function to build a Set
    * Browsed Player Response message based on the specified parameters
    * (this message is only applicable for the Browsing Channel).
    */
typedef struct _tagAVRCP_Set_Browsed_Player_Response_Data_t
{
   Byte_t                          Status;
   Word_t                          UIDCounter;
   DWord_t                         NumberItems;
   Word_t                          CharacterSet;
   Byte_t                          FolderDepth;
   AVRCP_Folder_Name_List_Entry_t *FolderNameList;
} AVRCP_Set_Browsed_Player_Response_Data_t;

#define AVRCP_SET_BROWSED_PLAYER_RESPONSE_DATA_SIZE                  (sizeof(AVRCP_Set_Browsed_Player_Response_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Change_Path_Command() function to build a Change Path
    * Command message based on the specified parameters (this message is
    * only applicable for the Browsing Channel).
    */
typedef struct _tagAVRCP_Change_Path_Command_Data_t
{
   Word_t  UIDCounter;
   Byte_t  Direction;
   QWord_t FolderUID;
} AVRCP_Change_Path_Command_Data_t;

#define AVRCP_CHANGE_PATH_COMMAND_DATA_SIZE                          (sizeof(AVRCP_Change_Path_Command_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Change_Path_Response() function to build a Change
    * Path Response message based on the specified parameters (this
    * message is only applicable for the Browsing Channel).
    */
typedef struct _tagAVRCP_Change_Path_Response_Data_t
{
   Byte_t  Status;
   DWord_t NumberItems;
} AVRCP_Change_Path_Response_Data_t;

#define AVRCP_CHANGE_PATH_RESPONSE_DATA_SIZE                         (sizeof(AVRCP_Change_Path_Response_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Get_Item_Attributes_Command() function to build a Get
    * Item Attributes Command message based on the specified parameters
    * (this message is only applicable for the Browsing Channel).
    */
typedef struct _tagAVRCP_Get_Item_Attributes_Command_Data_t
{
   Byte_t   Scope;
   QWord_t  UID;
   Word_t   UIDCounter;
   Byte_t   NumberAttributeIDs;
   DWord_t *AttributeIDList;
} AVRCP_Get_Item_Attributes_Command_Data_t;

#define AVRCP_GET_ITEM_ATTRIBUTES_COMMAND_DATA_SIZE                  (sizeof(AVRCP_Get_Item_Attributes_Command_Data_t))

   /*! The following structure is container structure that is used with
    * the AttributeValueList member of the
    * AVRCP_Get_Item_Attributes_Response_Data_t structure to denote an
    * individual Attribute Value Entry.
    */
typedef struct _tagAVRCP_Attribute_Value_List_Entry_t
{
   DWord_t AttributeID;
   Word_t  CharacterSet;
   Word_t  AttributeValueLength;
   Byte_t *AttributeValueData;
} AVRCP_Attribute_Value_List_Entry_t;

#define AVRCP_ATTRIBUTE_VALUE_LIST_ENTRY_SIZE                        (sizeof(AVRCP_Attribute_Value_List_Entry_t))

   /*! The following structure is used with the
    * AVRCP_Format_Get_Item_Attributes_Response() function to build a
    * Get Item Attributes Response message based on the specified
    * parameters (this message is only applicable for the Browsing
    * Channel).
    */
typedef struct _tagAVRCP_Get_Item_Attributes_Response_Data_t
{
   Byte_t                              Status;
   Byte_t                              NumberAttributes;
   AVRCP_Attribute_Value_List_Entry_t *AttributeValueList;
} AVRCP_Get_Item_Attributes_Response_Data_t;

#define AVRCP_GET_ITEM_ATTRIBUTES_RESPONSE_DATA_SIZE                 (sizeof(AVRCP_Get_Item_Attributes_Response_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Search_Command() function to build a Search Command
    * message based on the specified parameters (this message is only
    * applicable for the Browsing Channel).
    */
typedef struct _tagAVRCP_Search_Command_Data_t
{
   Word_t  CharacterSet;
   Word_t  Length;
   Byte_t *SearchString;
} AVRCP_Search_Command_Data_t;

#define AVRCP_SEARCH_COMMAND_DATA_SIZE                               (sizeof(AVRCP_Search_Command_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Search_Response() function to build a Search Response
    * message based on the specified parameters (this message is only
    * applicable for the Browsing Channel).
    */
typedef struct _tagAVRCP_Search_Response_Data_t
{
   Byte_t  Status;
   Word_t  UIDCounter;
   DWord_t NumberItems;
} AVRCP_Search_Response_Data_t;

#define AVRCP_SEARCH_RESPONSE_DATA_SIZE                              (sizeof(AVRCP_Search_Response_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_Get_Folder_Items_Command() function to build a Get
    * Folder Items Command message based on the specified parameters
    * (this message is only applicable for the Browsing Channel).
    */
typedef struct _tagAVRCP_Get_Folder_Items_Command_Data_t
{
   Byte_t   Scope;
   DWord_t  StartItem;
   DWord_t  EndItem;
   Byte_t   NumberAttributeIDs;
   DWord_t *AttributeIDList;
} AVRCP_Get_Folder_Items_Command_Data_t;

#define AVRCP_GET_FOLDER_ITEMS_COMMAND_DATA_SIZE                     (sizeof(AVRCP_Get_Folder_Items_Command_Data_t))

   /*! The following structure defines all the members that are present
    * in a Media Player Item which is part of the ItemList member of the
    * AVRCP_Get_Folder_Items_Response_Data_t structure.
    */
typedef struct _tagAVRCP_Media_Player_Item_Data_t
{
   Word_t                                   PlayerID;
   Byte_t                                   MajorPlayerType;
   DWord_t                                  PlayerSubType;
   Byte_t                                   PlayStatus;
   AVRCP_Media_Player_Item_Features_Mask_t  FeaturesMask;
   Word_t                                   CharacterSet;
   Word_t                                   DisplayableNameLength;
   Byte_t                                  *DisplayableName;
} AVRCP_Media_Player_Item_Data_t;

#define AVRCP_MEDIA_PLAYER_ITEM_DATA_SIZE                            (sizeof(AVRCP_Media_Player_Item_Data_t))

   /*! The following structure defines all the members that are present
    * in a Folder Item which is part of the ItemList member of the
    * AVRCP_Get_Folder_Items_Response_Data_t structure.
    */
typedef struct _tagAVRCP_Folder_Item_Data_t
{
   QWord_t  FolderUID;
   Byte_t   FolderType;
   Byte_t   IsPlayable;
   Word_t   CharacterSet;
   Word_t   DisplayableNameLength;
   Byte_t  *DisplayableName;
} AVRCP_Folder_Item_Data_t;

#define AVRCP_FOLDER_ITEM_DATA_SIZE                                  (sizeof(AVRCP_Folder_Item_Data_t))

   /*! The following structure defines all the members that are present
    * in a Media Element Item which is part of the ItemList member of
    * the AVRCP_Get_Folder_Items_Response_Data_t structure.
    */
typedef struct _tagAVRCP_Media_Element_Item_Data_t
{
   QWord_t                               UID;
   Byte_t                                MediaType;
   Word_t                                CharacterSet;
   Word_t                                DisplayableNameLength;
   Byte_t                               *DisplayableName;
   Byte_t                                NumberAttributes;
   AVRCP_Element_Attribute_List_Entry_t *ElementAttributeList;
} AVRCP_Media_Element_Item_Data_t;

#define AVRCP_MEDIA_ELEMENT_ITEM_DATA_SIZE                           (sizeof(AVRCP_Media_Element_Item_Data_t))

   /*! The following structure is container structure that is used with
    * the ItemList member of the AVRCP_Get_Folder_Items_Response_Data_t
    * structure to denote an individual Folder Item.
    */
typedef struct _tagAVRCP_Folder_Item_List_Entry_t
{
   Byte_t ItemType;
   union
   {
      AVRCP_Media_Player_Item_Data_t  MediaPlayerItemData;
      AVRCP_Folder_Item_Data_t        FolderItemData;
      AVRCP_Media_Element_Item_Data_t MediaElementItemData;
   } ItemData;
} AVRCP_Folder_Item_List_Entry_t;

#define AVRCP_FOLDER_ITEM_LIST_ENTRY_SIZE                            (sizeof(AVRCP_Folder_Item_List_Entry_t))

   /*! The following structure is used with the
    * AVRCP_Format_Get_Folder_Items_Response() function to build a Get
    * Folder Items Response message based on the specified parameters
    * (this message is only applicable for the Browsing Channel).
    */
typedef struct _tagAVRCP_Get_Folder_Items_Response_Data_t
{
   Byte_t                          Status;
   Word_t                          UIDCounter;
   Word_t                          NumberItems;
   AVRCP_Folder_Item_List_Entry_t *ItemList;
} AVRCP_Get_Folder_Items_Response_Data_t;

#define AVRCP_GET_FOLDER_ITEMS_RESPONSE_DATA_SIZE                    (sizeof(AVRCP_Get_Folder_Items_Response_Data_t))

   /*! The following structure is used with the
    * AVRCP_Format_General_Reject_Response() function to build a General
    * Reject Response message based on the specified parameters (this
    * message is only applicable for the Browsing Channel).
    */
typedef struct _tagAVRCP_General_Reject_Response_Data_t
{
   Byte_t RejectReason;
} AVRCP_General_Reject_Response_Data_t;

#define AVRCP_GENERAL_REJECT_RESPONSE_DATA_SIZE                      (sizeof(AVRCP_General_Reject_Response_Data_t))

   /*! The following structure is used with the AVRCP_Decode_Message()
    * function to decode a received AVRCP Message.
    */
typedef struct _tagAVRCP_Message_Information_t
{
   AVRCP_Message_Type_t MessageType;
   Boolean_t            Response;
   Boolean_t            FinalMessage;
   union
   {
      AVRCP_Unit_Info_Command_Data_t                                      UnitInfoCommandData;
      AVRCP_Unit_Info_Response_Data_t                                     UnitInfoResponseData;
      AVRCP_Subunit_Info_Command_Data_t                                   SubunitInfoCommandData;
      AVRCP_Subunit_Info_Response_Data_t                                  SubunitInfoResponseData;
      AVRCP_Pass_Through_Command_Data_t                                   PassThroughCommandData;
      AVRCP_Pass_Through_Response_Data_t                                  PassThroughResponseData;
      AVRCP_Vendor_Dependent_Generic_Command_Data_t                       VendorDependentGenericCommandData;
      AVRCP_Vendor_Dependent_Generic_Response_Data_t                      VendorDependentGenericResponseData;
      AVRCP_Browsing_Channel_Generic_Message_Data_t                       BrowsingChannelGenericMessageData;
      AVRCP_Message_Fragment_Data_t                                       MessageFragmentData;
      AVRCP_Group_Navigation_Command_Data_t                               GroupNavigationCommandData;
      AVRCP_Group_Navigation_Response_Data_t                              GroupNavigationResponseData;
      AVRCP_Get_Capabilities_Command_Data_t                               GetCapabilitiesCommandData;
      AVRCP_Get_Capabilities_Response_Data_t                              GetCapabilitiesResponseData;
      AVRCP_List_Player_Application_Setting_Attributes_Response_Data_t    ListPlayerApplicationSettingAttributesResponseData;
      AVRCP_List_Player_Application_Setting_Values_Command_Data_t         ListPlayerApplicationSettingValuesCommandData;
      AVRCP_List_Player_Application_Setting_Values_Response_Data_t        ListPlayerApplicationSettingValuesResponseData;
      AVRCP_Get_Current_Player_Application_Setting_Value_Command_Data_t   GetCurrentPlayerApplicationSettingValueCommandData;
      AVRCP_Get_Current_Player_Application_Setting_Value_Response_Data_t  GetCurrentPlayerApplicationSettingValueResponseData;
      AVRCP_Set_Player_Application_Setting_Value_Command_Data_t           SetPlayerApplicationSettingValueCommandData;
      AVRCP_Set_Player_Application_Setting_Value_Response_Data_t          SetPlayerApplicationSettingValueResponseData;
      AVRCP_Get_Player_Application_Setting_Attribute_Text_Command_Data_t  GetPlayerApplicationSettingAttributeTextCommandData;
      AVRCP_Get_Player_Application_Setting_Attribute_Text_Response_Data_t GetPlayerApplicationSettingAttributeTextResponseData;
      AVRCP_Get_Player_Application_Setting_Value_Text_Command_Data_t      GetPlayerApplicationSettingValueTextCommandData;
      AVRCP_Get_Player_Application_Setting_Value_Text_Response_Data_t     GetPlayerApplicationSettingValueTextResponseData;
      AVRCP_Inform_Displayable_Character_Set_Command_Data_t               InformDisplayableCharacterSetCommandData;
      AVRCP_Inform_Displayable_Character_Set_Response_Data_t              InformDisplayableCharacterSetResponseData;
      AVRCP_Inform_Battery_Status_Of_CT_Command_Data_t                    InformBatteryStatusOfCTCommandData;
      AVRCP_Inform_Battery_Status_Of_CT_Response_Data_t                   InformBatteryStatusOfCTResponseData;
      AVRCP_Get_Element_Attributes_Command_Data_t                         GetElementAttributesCommandData;
      AVRCP_Get_Element_Attributes_Response_Data_t                        GetElementAttributesResponseData;
      AVRCP_Get_Play_Status_Response_Data_t                               GetPlayStatusResponseData;
      AVRCP_Register_Notification_Command_Data_t                          RegisterNotificationCommandData;
      AVRCP_Register_Notification_Response_Data_t                         RegisterNotificationResponseData;
      AVRCP_Request_Continuing_Response_Command_Data_t                    RequestContinuingResponseCommandData;
      AVRCP_Abort_Continuing_Response_Command_Data_t                      AbortContinuingResponseCommandData;
      AVRCP_Abort_Continuing_Response_Response_Data_t                     AbortContinuingResponseResponseData;
      AVRCP_Set_Absolute_Volume_Command_Data_t                            SetAbsoluteVolumeCommandData;
      AVRCP_Set_Absolute_Volume_Response_Data_t                           SetAbsoluteVolumeResponseData;
      AVRCP_Command_Reject_Response_Data_t                                CommandRejectResponseData;
      AVRCP_Set_Addressed_Player_Command_Data_t                           SetAddressedPlayerCommandData;
      AVRCP_Set_Addressed_Player_Response_Data_t                          SetAddressedPlayerResponseData;
      AVRCP_Play_Item_Command_Data_t                                      PlayItemCommandData;
      AVRCP_Play_Item_Response_Data_t                                     PlayItemResponseData;
      AVRCP_Add_To_Now_Playing_Command_Data_t                             AddToNowPlayingCommandData;
      AVRCP_Add_To_Now_Playing_Response_Data_t                            AddToNowPlayingResponseData;
      AVRCP_Unknown_Command_Data_t                                        UnknownCommandData;
      AVRCP_Set_Browsed_Player_Command_Data_t                             SetBrowsedPlayerCommandData;
      AVRCP_Set_Browsed_Player_Response_Data_t                            SetBrowsedPlayerResponseData;
      AVRCP_Change_Path_Command_Data_t                                    ChangePathCommandData;
      AVRCP_Change_Path_Response_Data_t                                   ChangePathResponseData;
      AVRCP_Get_Item_Attributes_Command_Data_t                            GetItemAttributesCommandData;
      AVRCP_Get_Item_Attributes_Response_Data_t                           GetItemAttributesResponseData;
      AVRCP_Search_Command_Data_t                                         SearchCommandData;
      AVRCP_Search_Response_Data_t                                        SearchResponseData;
      AVRCP_Get_Folder_Items_Command_Data_t                               GetFolderItemsCommandData;
      AVRCP_Get_Folder_Items_Response_Data_t                              GetFolderItemsResponseData;
      AVRCP_General_Reject_Response_Data_t                                GeneralRejectResponseData;
   } MessageInformation;
} AVRCP_Message_Information_t;

    /*! AVRCP SDP Registration functions. */

    /*! @brief The following function is responsible for registering an AVRCP
    * Controller or Target service record to the SDP database.
    * @param BluetoothStackID The first parameter to this function is the Bluetooth Stack ID of the
    * Bluetooth Stack associated with the AVRCP Controller or Target
    * Server for which this service record is being registered.
    * @param Controller The second parameter to this function specifies whether this record is
    * to specify a Controller (TRUE) or a Target (FALSE) SDP Record.
    * @param ServiceName The third parameter to this function is the Service Name to be
    * associated with this service record.  @param ProviderName The fourth parameter to this
    * function is the Provider name to be associated with this service
    * record.  @param SupportedFeaturesMask The fifth parameter specifies a bit mask that identifies
    * the features supported by the local AVRCP server.
    * @param SDPServiceRecordHandle The first parameter to this function is a pointer to a DWord_t which
    * receives the SDP Service Record Handle if this function
    * successfully creates a service record.  If @return This function returns
    * zero, then the SDPServiceRecordHandle entry will contain the
    * Service Record Handle of the added SDP Service Record.  If this
    * function fails, a negative return error code will be returned (see
    * BTERRORS.H) and the SDPServiceRecordHandle value will be
    * undefined.
    * \note This function registers an SDP Record with the latest
    *          AVRCP version.  If a prior version of AVRCP is required,
    *          then the AVRCP_Register_SDP_Record_Version() function
    *          should be called to specify a specific version.
    * \note The Service Record Handle that is returned from this
    *          function will remain in the SDP Record Database until it
    *          is deleted by calling the SDP_Delete_Service_Record()
    *          function.
    * \note A MACRO is provided to Delete the Service Record from the
    *          SDP Data Base.  This MACRO maps the
    *          AVRCP_Un_Register_SDP_Record() to the
    *          SDP_Delete_Service_Record() function.
    * \note The Service Name is always added at Attribute ID 0x0100.
    *          A Language Base Attribute ID List is created that
    *          specifies that 0x0100 is UTF-8 Encoded, English Language.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Register_SDP_Record(unsigned int BluetoothStackID, Boolean_t Controller, char *ServiceName, char *ProviderName, Word_t SupportedFeaturesMask, DWord_t *SDPServiceRecordHandle);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Register_SDP_Record_t)(unsigned int BluetoothStackID, Boolean_t Controller, char *ServiceName, char *ProviderName, Word_t SupportedFeaturesMask, DWord_t *SDPServiceRecordHandle);
#endif

   /*! @brief The following function is responsible for registering an AVRCP
    * Controller or Target service record to the SDP database
    * (specifying a specific AVRCP version).  @param BluetoothStackID The first parameter to
    * this function is the Bluetooth Stack ID of the Bluetooth Stack
    * associated with the AVRCP Controller or Target Server for which
    * this service record is being registered.  @param Controller The second parameter to
    * this function specifies whether this record is to specify a
    * Controller (TRUE) or a Target (FALSE) SDP Record.  @param ServiceName The third parameter to this function is the Service Name to be associated
    * with this service record.  @param ProviderName The fourth parameter to this function
    * is the Provider name to be associated with this service record.
    * @param SupportedFeaturesMask The fifth parameter specifies a bit mask that identifies the
    * features supported by the local AVRCP server. @param AVRCPVersion The sixth parameter
    * specifies the actual AVRCP version that is to be published.
    * @param SDPServiceRecordHandle The first parameter to this function is a pointer to a DWord_t which
    * receives the SDP Service Record Handle if this function
    * successfully creates a service record.  If @return This function returns
    * zero, then the SDPServiceRecordHandle entry will contain the
    * Service Record Handle of the added SDP Service Record.  If this
    * function fails, a negative return error code will be returned (see
    * BTERRORS.H) and the SDPServiceRecordHandle value will be
    * undefined.
    * \note The Service Record Handle that is returned from this
    *          function will remain in the SDP Record Database until it
    *          is deleted by calling the SDP_Delete_Service_Record()
    *          function.
    * \note A MACRO is provided to Delete the Service Record from the
    *          SDP Data Base.  This MACRO maps the
    *          AVRCP_Un_Register_SDP_Record() to the
    *          SDP_Delete_Service_Record() function.
    * \note The Service Name is always added at Attribute ID 0x0100.
    *          A Language Base Attribute ID List is created that
    *          specifies that 0x0100 is UTF-8 Encoded, English Language.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Register_SDP_Record_Version(unsigned int BluetoothStackID, Boolean_t Controller, char *ServiceName, char *ProviderName, Word_t SupportedFeaturesMask, AVRCP_Version_t AVRCPVersion, DWord_t *SDPServiceRecordHandle);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Register_SDP_Record_Version_t)(unsigned int BluetoothStackID, Boolean_t Controller, char *ServiceName, char *ProviderName, Word_t SupportedFeaturesMask, AVRCP_Version_t AVRCPVersion, DWord_t *SDPServiceRecordHandle);
#endif

   /*! The following MACRO is a utility MACRO that simply deletes the
    * AVRCP SDP Service Record (specified by the second parameter) from
    * the SDP Database.  This MACRO simply maps to the
    * SDP_Delete_Service_Record() function.  This MACRO is only provided
    * so that the caller doesn't have to sift through the SDP API for
    * very simplistic applications.  This function accepts as input the
    * Bluetooth Stack ID of the Bluetooth Protocol Stack that the
    * Service Record exists on and the SDP Service Record Handle.  The
    * SDP Service Record Handle was returned via a successful call to
    * the AVRCP_Register_SDP_Record() function.  This MACRO returns the
    * result of the SDP_Delete_Service_Record() function, which is zero
    * for success or a negative return error code (see BTERRORS.H).
    */
#define AVRCP_Un_Register_SDP_Record(__BluetoothStackID, __SDPRecordHandle) \
        (SDP_Delete_Service_Record(__BluetoothStackID, __SDPRecordHandle))

    /*! Generic AVRCP IEEE 1394 A/V Commands/Responses. */
    /*! @brief The following function is a utility function that exists to format
    * an AVRCP Unit Info Command. @param BluetoothStackID Unique identifier
    * assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param BufferLength Length of the provided buffer to which the command data will be written.
    * @param Buffer Pointer to a buffer to which the command data will be written.
    * @return This function returns the amount of the data that was copied into the specified
    * buffer to build the command (if successful, positive) or function returns a negative return code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified command.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Unit_Info_Command(unsigned int BluetoothStackID, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Unit_Info_Command_t)(unsigned int BluetoothStackID, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Unit Info Response. @param BluetoothStackID Unique identifier
    * assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param UnitInfoResponseData Pointer to a structure containing the data to
    * write into the provided buffer. @param BufferLength Length of the provided buffer
    * to which the response data will be written. @param Buffer Pointer to a buffer
    * to which the response data will be written.
    * @return This function returns the amount of the data that was copied into the specified
    * buffer to build the response (if successful, positive) or a negative return code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified response.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Unit_Info_Response(unsigned int BluetoothStackID, AVRCP_Unit_Info_Response_Data_t *UnitInfoResponseData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Unit_Info_Response_t)(unsigned int BluetoothStackID, AVRCP_Unit_Info_Response_Data_t *UnitInfoResponseData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Subunit Info Command. @param BluetoothStackID Unique identifier
    * assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param SubunitInfoCommandData Pointer to a structure containing the data
    * to write into the provided buffer. @param BufferLength Length of the provided
    * buffer to which the command data will be written. @param Buffer Pointer to a
    * buffer to which the command data will be written.
    * @return This function returns the amount of the data that was copied into the specified
    * buffer to build the command (if successful, positive) or a negative return code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified command.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Subunit_Info_Command(unsigned int BluetoothStackID, AVRCP_Subunit_Info_Command_Data_t *SubunitInfoCommandData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Subunit_Info_Command_t)(unsigned int BluetoothStackID, AVRCP_Subunit_Info_Command_Data_t *SubunitInfoCommandData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Subunit Info Response. @param BluetoothStackID Unique identifier
    * assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param SubunitInfoResponseData Pointer to a structure containing the
    * data to write into the provided buffer.@param BufferLength  Length of the
    * provided buffer to which the response data will be written.
    * @param Buffer Pointer to a buffer to which the response data will be written.
    * @return This function returns the amount of the data that was copied into the specified
    * buffer to build the response (if successful, positive) or a negative return code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified response.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Subunit_Info_Response(unsigned int BluetoothStackID, AVRCP_Subunit_Info_Response_Data_t *SubunitInfoResponseData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Subunit_Info_Response_t)(unsigned int BluetoothStackID, AVRCP_Subunit_Info_Response_Data_t *SubunitInfoResponseData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Pass Through Command.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param PassThroughCommandData Pointer to a structure containing the
    * data to write into the provided buffer. @param BufferLength Length of the
    * provided buffer to which the command data will be written.
    * @param Buffer Pointer to a buffer to which the command data will be written.
    * @return This function returns the amount of the data that was copied into the specified
    * buffer to build the command (if successful, positive) or a negative return code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified command.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Pass_Through_Command(unsigned int BluetoothStackID, AVRCP_Pass_Through_Command_Data_t *PassThroughCommandData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Pass_Through_Command_t)(unsigned int BluetoothStackID, AVRCP_Pass_Through_Command_Data_t *PassThroughCommandData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Pass Through Response.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param PassThroughResponseData Pointer to a structure containing the data to write into the provided buffer.
    * @param BufferLength  Length of the provided buffer to which the response data will be written.
    * @param Buffer  Pointer to a buffer to which the response data will be written.
    * @return This function returns the amount of the data that was copied into the specified
    * buffer to build the response (if successful, positive) ora negative return code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified response.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Pass_Through_Response(unsigned int BluetoothStackID, AVRCP_Pass_Through_Response_Data_t *PassThroughResponseData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Pass_Through_Response_t)(unsigned int BluetoothStackID, AVRCP_Pass_Through_Response_Data_t *PassThroughResponseData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /* AVRCP Vendor Dependent AV/C Commands/Responses.
    * @brief The following function is a utility function that exists to format
    * a Generic AVRCP Vendor Dependent Command.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param VendorDependentCommandData  Pointer to a structure containing the data to write into the provided buffer.
    * @param BufferLength  Length of the provided buffer to which the command data will be written.
    * @param Buffer  Pointer to a buffer to which the command data will be written.
    * @return This function returns the amount of the data that was copied into
    * the specified buffer to build the command (if successful,
    * positive) or a negative return code if there
    * was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified command.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Vendor_Dependent_Generic_Command(unsigned int BluetoothStackID, AVRCP_Vendor_Dependent_Generic_Command_Data_t *VendorDependentCommandData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Vendor_Dependent_Generic_Command_t)(unsigned int BluetoothStackID, AVRCP_Vendor_Dependent_Generic_Command_Data_t *VendorDependentCommandData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * a Generic AVRCP Vendor Dependent Response.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param VendorDependentResponseData Pointer to a structure containing the data to write into the provided buffer.
    * @param BufferLength  Length of the provided buffer to which the response data will be written.
    * @param Buffer  Pointer to a buffer to which the response data will be written.
    * @return This function returns the amount of the data that was copied into
    * the specified buffer to build the response (if successful,
    * positive) or a negative return code if there
    * was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified response.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Vendor_Dependent_Generic_Response(unsigned int BluetoothStackID, AVRCP_Vendor_Dependent_Generic_Response_Data_t *VendorDependentResponseData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Vendor_Dependent_Generic_Response_t)(unsigned int BluetoothStackID, AVRCP_Vendor_Dependent_Generic_Response_Data_t *VendorDependentResponseData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * a Generic AVRCP Browsing Channel Message (command or response).
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param BrowsingChannelGenericMessageData Pointer to a structure containing the data to write into the provided buffer.
    * @param BufferLength  Length of the provided buffer to which the response data will be written.
    * @param Buffer  Pointer to a buffer to which the response data will be written.
    * @return This function returns the amount of the
    * data that was copied into the specified buffer to build the
    * message (if successful, positive) or a
    * negative return code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified message.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Browsing_Channel_Generic_Message(unsigned int BluetoothStackID, AVRCP_Browsing_Channel_Generic_Message_Data_t *BrowsingChannelGenericMessageData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Browsing_Channel_Generic_Message_t)(unsigned int BluetoothStackID, AVRCP_Browsing_Channel_Generic_Message_Data_t *BrowsingChannelGenericMessageData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! AVRCP Extensions to IEEE 1394 A/V Commands/Responses. */
   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Group Navigation Pass Through Command.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param GroupNavigationCommandData The data to populate into the command (required)
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the command into.
    * @return This function returns the amount of the data that was
    * copied into the specified buffer to build the command (if
    * successful, positive) or a negative return
    * code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified command.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Group_Navigation_Command(unsigned int BluetoothStackID, AVRCP_Group_Navigation_Command_Data_t *GroupNavigationCommandData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Group_Navigation_Command_t)(unsigned int BluetoothStackID, AVRCP_Group_Navigation_Command_Data_t *GroupNavigationCommandData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Group Navigation Pass Through Response.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param GroupNavigationResponseData The data to populate into the response (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the response into.
    * @return This function returns the amount of the data that
    * was copied into the specified buffer to build the response (if
    * successful, positive) or a negative return
    * code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified response.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Group_Navigation_Response(unsigned int BluetoothStackID, AVRCP_Group_Navigation_Response_Data_t *GroupNavigationResponseData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Group_Navigation_Response_t)(unsigned int BluetoothStackID, AVRCP_Group_Navigation_Response_Data_t *GroupNavigationResponseData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Get Capabilities Command.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param GetCapabilitiesCommandData The data to populate into the command (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the response into.
    * @return This function returns the amount of the data that was copied into the
    * specified buffer to build the command (if successful, positive).
    * or a negative return code if there was an
    * error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified command.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Get_Capabilities_Command(unsigned int BluetoothStackID, AVRCP_Get_Capabilities_Command_Data_t *GetCapabilitiesCommandData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Get_Capabilities_Command_t)(unsigned int BluetoothStackID, AVRCP_Get_Capabilities_Command_Data_t *GetCapabilitiesCommandData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Get Capabilities Response.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param MessageStateInfo The Response State Information (required).
    * @param GetCapabilitiesResponseData The data to populate into the response (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the response into.
    * @return This function returns the
    * amount of the data that was copied into the specified buffer to
    * build the response (if successful, positive) or a negative return code if there was an error.
    * \note The Response Message State Information *MUST* be
    *          initialized with the correct values:
    *              - Complete - FALSE
    *              - Offset   - 0
    *          if, after the first successful invocation of this
    *          function, the Complete parameter is still FALSE, then
    *          this means the message was fragmented.  Subsequent
    *          calls to this function require this member to passed
    *          with the values that are present in the returned
    *          structure.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified response.  Note that in this case,
    *          this function will STILL update the Response Message
    *          State Information.  In this case, the caller *MUST*
    *          reinitialize the structure to the correct value before
    *          calling this function to build the correct message.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Get_Capabilities_Response(unsigned int BluetoothStackID, AVRCP_Response_Message_State_Info_t *MessageStateInfo, AVRCP_Get_Capabilities_Response_Data_t *GetCapabilitiesResponseData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Get_Capabilities_Response_t)(unsigned int BluetoothStackID, AVRCP_Response_Message_State_Info_t *MessageStateInfo, AVRCP_Get_Capabilities_Response_Data_t *GetCapabilitiesResponseData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP List Player Application Setting Attributes Command.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the command into.
    * @return This function returns the amount of the data
    * that was copied into the specified buffer to build the command (if
    * successful, positive) or a negative return
    * code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified command.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_List_Player_Application_Setting_Attributes_Command(unsigned int BluetoothStackID, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_List_Player_Application_Setting_Attributes_Command_t)(unsigned int BluetoothStackID, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP List Player Application Setting Attributes Response.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param ListPlayerApplicationSettingAttributesResponseData The data to populate into the response (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the response into.
    * @return This function returns the amount of the
    * data that was copied into the specified buffer to build the
    * response (if successful, positive) or a
    * negative return code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified response.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_List_Player_Application_Setting_Attributes_Response(unsigned int BluetoothStackID, AVRCP_List_Player_Application_Setting_Attributes_Response_Data_t *ListPlayerApplicationSettingAttributesResponseData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_List_Player_Application_Setting_Attributes_Response_t)(unsigned int BluetoothStackID, AVRCP_List_Player_Application_Setting_Attributes_Response_Data_t *ListPlayerApplicationSettingAttributesResponseData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP List Player Application Setting Values Command.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param ListPlayerApplicationSettingValuesCommandData The data to populate into the command (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the command into.
    * @return This function returns the amount of the data that
    * was copied into the specified buffer to build the command (if
    * successful, positive) or a negative return
    * code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified command.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_List_Player_Application_Setting_Values_Command(unsigned int BluetoothStackID, AVRCP_List_Player_Application_Setting_Values_Command_Data_t *ListPlayerApplicationSettingValuesCommandData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_List_Player_Application_Setting_Values_Command_t)(unsigned int BluetoothStackID, AVRCP_List_Player_Application_Setting_Values_Command_Data_t *ListPlayerApplicationSettingValuesCommandData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP List Player Application Setting Values Response.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param ListPlayerApplicationSettingValuesResponseData The data to populate into the response (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the response into
    * @return This function returns the amount of the
    * data that was copied into the specified buffer to build the
    * response (if successful, positive) or a
    * negative return code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified response.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_List_Player_Application_Setting_Values_Response(unsigned int BluetoothStackID, AVRCP_List_Player_Application_Setting_Values_Response_Data_t *ListPlayerApplicationSettingValuesResponseData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_List_Player_Application_Setting_Values_Response_t)(unsigned int BluetoothStackID, AVRCP_List_Player_Application_Setting_Values_Response_Data_t *ListPlayerApplicationSettingValuesResponseData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Get Current Player Application Setting Value Command.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param GetCurrentPlayerApplicationSettingValueCommandData The data to populate into the command (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the command into.
    * @return This function returns the amount of the
    * data that was copied into the specified buffer to build the
    * command (if successful, positive) or a
    * negative return code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified command.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Get_Current_Player_Application_Setting_Value_Command(unsigned int BluetoothStackID, AVRCP_Get_Current_Player_Application_Setting_Value_Command_Data_t *GetCurrentPlayerApplicationSettingValueCommandData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Get_Current_Player_Application_Setting_Value_Command_t)(unsigned int BluetoothStackID, AVRCP_Get_Current_Player_Application_Setting_Value_Command_Data_t *GetCurrentPlayerApplicationSettingValueCommandData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Get Current Player Application Setting Value Response.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param MessageStateInfo This function accepts the Response State Information (required).
    * @param GetCurrentPlayerApplicationSettingValueResponseData The data to populate into the response (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the response into.
    * @return This function returns the amount of the data that was copied into
    * the specified buffer to build the response (if successful,
    * positive) or a negative return code if there
    * was an error.
    * \note The Response Message State Information *MUST* be
    *          initialized with the correct values:
    *              - Complete - FALSE
    *              - Offset   - 0
    *          if, after the first successful invocation of this
    *          function, the Complete parameter is still FALSE, then
    *          this means the message was fragmented.  Subsequent
    *          calls to this function require this member to passed
    *          with the values that are present in the returned
    *          structure.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified response.  Note that in this case,
    *          this function will STILL update the Reponse Message
    *          State Information.  In this case, the caller *MUST*
    *          reinitialize the structure to the correct value before
    *          calling this function to build the correct message.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Get_Current_Player_Application_Setting_Value_Response(unsigned int BluetoothStackID, AVRCP_Response_Message_State_Info_t *MessageStateInfo, AVRCP_Get_Current_Player_Application_Setting_Value_Response_Data_t *GetCurrentPlayerApplicationSettingValueResponseData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Get_Current_Player_Application_Setting_Value_Response_t)(unsigned int BluetoothStackID, AVRCP_Response_Message_State_Info_t *MessageStateInfo, AVRCP_Get_Current_Player_Application_Setting_Value_Response_Data_t *GetCurrentPlayerApplicationSettingValueResponseData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Set Player Application Setting Value Command.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param SetPlayerApplicationSettingValueCommandData The data to populate into the command (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the command into.
    * @return This function returns the amount of the data that
    * was copied into the specified buffer to build the command (if
    * successful, positive) or a negative return
    * code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified command.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Set_Player_Application_Setting_Value_Command(unsigned int BluetoothStackID, AVRCP_Set_Player_Application_Setting_Value_Command_Data_t *SetPlayerApplicationSettingValueCommandData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Set_Player_Application_Setting_Value_Command_t)(unsigned int BluetoothStackID, AVRCP_Set_Player_Application_Setting_Value_Command_Data_t *SetPlayerApplicationSettingValueCommandData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Set Player Application Setting Value Response.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param SetPlayerApplicationSettingValueResponseData The data to populate into the response (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the response into.
    * @return This function returns the amount of the data that
    * was copied into the specified buffer to build the response (if
    * successful, positive) or a negative return
    * code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified response.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Set_Player_Application_Setting_Value_Response(unsigned int BluetoothStackID, AVRCP_Set_Player_Application_Setting_Value_Response_Data_t *SetPlayerApplicationSettingValueResponseData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Set_Player_Application_Setting_Value_Response_t)(unsigned int BluetoothStackID, AVRCP_Set_Player_Application_Setting_Value_Response_Data_t *SetPlayerApplicationSettingValueResponseData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Get Player Application Setting Attribute Text Command.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param GetPlayerApplicationSettingAttributeTextCommandData The data to populate into the command (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the command into.
    * @return This function returns the amount
    * of the data that was copied into the specified buffer to build
    * the command (if successful, positive) or a
    * negative return code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified command.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Get_Player_Application_Setting_Attribute_Text_Command(unsigned int BluetoothStackID, AVRCP_Get_Player_Application_Setting_Attribute_Text_Command_Data_t *GetPlayerApplicationSettingAttributeTextCommandData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Get_Player_Application_Setting_Attribute_Text_Command_t)(unsigned int BluetoothStackID, AVRCP_Get_Player_Application_Setting_Attribute_Text_Command_Data_t *GetPlayerApplicationSettingAttributeTextCommandData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Get Player Application Setting Attribute Text Response.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param MessageStateInfo This function accepts the Response State Information (required).
    * @param GetPlayerApplicationSettingAttributeTextResponseData The data to populate into the response (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the response into.
    * @return This function returns the amount of the data that was copied into
    * the specified buffer to build the response (if successful,
    * positive) or a negative return code if there
    * was an error.
    * \note The Response Message State Information *MUST* be
    *          initialized with the correct values:
    *              - Complete - FALSE
    *              - Offset   - 0
    *          if, after the first successful invocation of this
    *          function, the Complete parameter is still FALSE, then
    *          this means the message was fragmented.  Subsequent
    *          calls to this function require this member to passed
    *          with the values that are present in the returned
    *          structure.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified response.  Note that in this case,
    *          this function will STILL update the Reponse Message
    *          State Information.  In this case, the caller *MUST*
    *          reinitialize the structure to the correct value before
    *          calling this function to build the correct message.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Get_Player_Application_Setting_Attribute_Text_Response(unsigned int BluetoothStackID, AVRCP_Response_Message_State_Info_t *MessageStateInfo, AVRCP_Get_Player_Application_Setting_Attribute_Text_Response_Data_t *GetPlayerApplicationSettingAttributeTextResponseData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Get_Player_Application_Setting_Attribute_Text_Response_t)(unsigned int BluetoothStackID, AVRCP_Response_Message_State_Info_t *MessageStateInfo, AVRCP_Get_Player_Application_Setting_Attribute_Text_Response_Data_t *GetPlayerApplicationSettingAttributeTextResponseData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Get Player Application Setting Value Text Command.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param GetPlayerApplicationSettingValueTextCommandData The data to populate into the command (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the command into.
    * @return This function returns the amount of the data that
    * was copied into the specified buffer to build the command (if
    * successful, positive) or a negative return
    * code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified command.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Get_Player_Application_Setting_Value_Text_Command(unsigned int BluetoothStackID, AVRCP_Get_Player_Application_Setting_Value_Text_Command_Data_t *GetPlayerApplicationSettingValueTextCommandData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Get_Player_Application_Setting_Value_Text_Command_t)(unsigned int BluetoothStackID, AVRCP_Get_Player_Application_Setting_Value_Text_Command_Data_t *GetPlayerApplicationSettingValueTextCommandData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Get Player Application Setting Value Text Response.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param MessageStateInfo This function accepts the Response State Information (required).
    * @param GetPlayerApplicationSettingValueTextResponseData The data to populate into the response (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the response into.
    * @return This function returns the amount of the data that was copied into
    * the specified buffer to build the response (if successful,
    * positive) or a negative return code if there
    * was an error.
    * \note The Response Message State Information *MUST* be
    *          initialized with the correct values:
    *             - Complete - FALSE
    *             - Offset   - 0
    *          if, after the first successful invocation of this
    *          function, the Complete parameter is still FALSE, then
    *          this means the message was fragmented.  Subsequent calls
    *          to this function require this member to passed with the
    *          values that are present in the returned structure.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified response.  Note that in this case,
    *          this function will STILL update the Reponse Message State
    *          Information.  In this case, the caller *MUST*
    *          reinitialize the structure to the correct value before
    *          calling this function to build the correct message.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Get_Player_Application_Setting_Value_Text_Response(unsigned int BluetoothStackID, AVRCP_Response_Message_State_Info_t *MessageStateInfo, AVRCP_Get_Player_Application_Setting_Value_Text_Response_Data_t *GetPlayerApplicationSettingValueTextResponseData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Get_Player_Application_Setting_Value_Text_Response_t)(unsigned int BluetoothStackID, AVRCP_Response_Message_State_Info_t *MessageStateInfo, AVRCP_Get_Player_Application_Setting_Value_Text_Response_Data_t *GetPlayerApplicationSettingValueTextResponseData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Inform Displayable Character Set Command.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param InformDisplayableCharacterSetCommandData The data to populate into the command (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the command into.
    * @return This function returns the amount of the data that was
    * copied into the specified buffer to build the command (if
    * successful, positive) or a negative return
    * code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified command.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Inform_Displayable_Character_Set_Command(unsigned int BluetoothStackID, AVRCP_Inform_Displayable_Character_Set_Command_Data_t *InformDisplayableCharacterSetCommandData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Inform_Displayable_Character_Set_Command_t)(unsigned int BluetoothStackID, AVRCP_Inform_Displayable_Character_Set_Command_Data_t *InformDisplayableCharacterSetCommandData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Inform Displayable Character Set Response.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param InformDisplayableCharacterSetResponseData The data to populate into the response (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the response into.
    * @return This function returns the amount of the data that
    * was copied into the specified buffer to build the response (if
    * successful, positive) or a negative return
    * code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified response.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Inform_Displayable_Character_Set_Response(unsigned int BluetoothStackID, AVRCP_Inform_Displayable_Character_Set_Response_Data_t *InformDisplayableCharacterSetResponseData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Inform_Displayable_Character_Set_Response_t)(unsigned int BluetoothStackID, AVRCP_Inform_Displayable_Character_Set_Response_Data_t *InformDisplayableCharacterSetResponseData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Inform Battery Status of CT Command.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param InformBatteryStatusOfCTCommandData The data to populate into the command (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the command into.
    * @return This function returns the amount of the data that was
    * copied into the specified buffer to build the command (if
    * successful, positive) or a negative return
    * code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified command.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Inform_Battery_Status_Of_CT_Command(unsigned int BluetoothStackID, AVRCP_Inform_Battery_Status_Of_CT_Command_Data_t *InformBatteryStatusOfCTCommandData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Inform_Battery_Status_Of_CT_Command_t)(unsigned int BluetoothStackID, AVRCP_Inform_Battery_Status_Of_CT_Command_Data_t *InformBatteryStatusOfCTCommandData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Inform Battery Status of CT Response.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param InformBatteryStatusOfCTResponseData The data to populate into the response (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the response into.
    * @return This function returns the amount of the data that
    * was copied into the specified buffer to build the response (if
    * successful, positive) or a negative return
    * code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified response.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Inform_Battery_Status_Of_CT_Response(unsigned int BluetoothStackID, AVRCP_Inform_Battery_Status_Of_CT_Response_Data_t *InformBatteryStatusOfCTResponseData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Inform_Battery_Status_Of_CT_Response_t)(unsigned int BluetoothStackID, AVRCP_Inform_Battery_Status_Of_CT_Response_Data_t *InformBatteryStatusOfCTResponseData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Get Element Attributes Command.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param GetElementAttributesCommandData The data to populate into the command (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the command into.
    * @return This function returns the amount of the data that was copied into the
    * specified buffer to build the command (if successful, positive)
    * or a negative return code if there was an
    * error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified command.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Get_Element_Attributes_Command(unsigned int BluetoothStackID, AVRCP_Get_Element_Attributes_Command_Data_t *GetElementAttributesCommandData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Get_Element_Attributes_Command_t)(unsigned int BluetoothStackID, AVRCP_Get_Element_Attributes_Command_Data_t *GetElementAttributesCommandData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Get Element Attributes Response.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param MessageStateInfo This function accepts the Response State Information (required).
    * @param GetElementAttributesResponseData The data to populate into the response (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the response into.
    * @return This function returns
    * the amount of the data that was copied into the specified buffer
    * to build the response (if successful, positive) or a negative return code if there was an error.
    * \note The Response Message State Information *MUST* be
    *          initialized with the correct values:
    *             - Complete - FALSE
    *             - Offset   - 0
    *          if, after the first successful invocation of this
    *          function, the Complete parameter is still FALSE, then
    *          this means the message was fragmented.  Subsequent calls
    *          to this function require this member to passed with the
    *          values that are present in the returned structure.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified response.  Note that in this case,
    *          this function will STILL update the Reponse Message State
    *          Information.  In this case, the caller *MUST*
    *          reinitialize the structure to the correct value before
    *          calling this function to build the correct message.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Get_Element_Attributes_Response(unsigned int BluetoothStackID, AVRCP_Response_Message_State_Info_t *MessageStateInfo, AVRCP_Get_Element_Attributes_Response_Data_t *GetElementAttributesResponseData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Get_Element_Attributes_Response_t)(unsigned int BluetoothStackID, AVRCP_Response_Message_State_Info_t *MessageStateInfo, AVRCP_Get_Element_Attributes_Response_Data_t *GetElementAttributesResponseData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Get Play Status Command.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the command into.
    * @return This function returns the amount of the data that was copied into the
    * specified buffer to build the command (if successful, positive)
    * or a negative return code if there was an
    * error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified command.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Get_Play_Status_Command(unsigned int BluetoothStackID, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Get_Play_Status_Command_t)(unsigned int BluetoothStackID, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Get Play Status Response.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param GetPlayStatusResponseData The data to populate into the response (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the response into.
    * @return This function returns the amount of the data that was copied into the
    * specified buffer to build the response (if successful, positive)
    * or a negative return code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified response.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Get_Play_Status_Response(unsigned int BluetoothStackID, AVRCP_Get_Play_Status_Response_Data_t *GetPlayStatusResponseData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Get_Play_Status_Response_t)(unsigned int BluetoothStackID, AVRCP_Get_Play_Status_Response_Data_t *GetPlayStatusResponseData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Register Notification Command.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param RegisterNotificationCommandData The data to populate into the command (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the command into.
    * @return This function returns the amount of the data that was copied into the
    * specified buffer to build the command (if successful, positive)
    * or a negative return code if there was an
    * error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified command.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Register_Notification_Command(unsigned int BluetoothStackID, AVRCP_Register_Notification_Command_Data_t *RegisterNotificationCommandData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Register_Notification_Command_t)(unsigned int BluetoothStackID, AVRCP_Register_Notification_Command_Data_t *RegisterNotificationCommandData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Get Play Status Response.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param RegisterNotificationResponseData The data to populate into the response (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the response into.
    * @return This function returns the amount of the data that was copied into the
    * specified buffer to build the response (if successful, positive)
    * or a negative return code if there was an
    * error.
    * \note This function is used to build both the Notification
    *          Response AND the actual Event Notification itself.  The
    *          specification defines the response as having a
    *          Response Code of either:
    *             - AVRCP_RESPONSE_INTERIM (success)
    *             - AVRCP_RESPONSE_REJECTED (failure)
    *             - AVRCP_RESPONSE_NOT_IMPLEMENTED (failure)
    *          Notification Events have a Response Code of:
    *             - AVRCP_RESPONSE_CHANGED
    *          See the AVRCP specification for more information.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified response.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Register_Notification_Response(unsigned int BluetoothStackID, AVRCP_Register_Notification_Response_Data_t *RegisterNotificationResponseData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Register_Notification_Response_t)(unsigned int BluetoothStackID, AVRCP_Register_Notification_Response_Data_t *RegisterNotificationResponseData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Request Continuing Response Command.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param RequestContinuingResponseCommandData The data to populate into the command (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the command into.
    * @return This function returns the amount of the data that was
    * copied into the specified buffer to build the command (if
    * successful, positive) or a negative return
    * code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified command.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Request_Continuing_Response_Command(unsigned int BluetoothStackID, AVRCP_Request_Continuing_Response_Command_Data_t *RequestContinuingResponseCommandData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Request_Continuing_Response_Command_t)(unsigned int BluetoothStackID, AVRCP_Request_Continuing_Response_Command_Data_t *RequestContinuingResponseCommandData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Abort Continuing Response Command.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param AbortContinuingResponseCommandData The data to populate into the command (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the command into.
    * @return This function returns the amount of the data that was copied into the
    * specified buffer to build the command (if successful, positive)
    * or a negative return code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified command.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Abort_Continuing_Response_Command(unsigned int BluetoothStackID, AVRCP_Abort_Continuing_Response_Command_Data_t *AbortContinuingResponseCommandData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Abort_Continuing_Response_Command_t)(unsigned int BluetoothStackID, AVRCP_Abort_Continuing_Response_Command_Data_t *AbortContinuingResponseCommandData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Abort Continuing Response Response.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param AbortContinuingResponseResponseData The data to populate into the response (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the response into.
    * @return This function returns the amount of the data that
    * was copied into the specified buffer to build the response (if
    * successful, positive) or a negative return
    * code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified response.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Abort_Continuing_Response_Response(unsigned int BluetoothStackID, AVRCP_Abort_Continuing_Response_Response_Data_t *AbortContinuingResponseResponseData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Abort_Continuing_Response_Response_t)(unsigned int BluetoothStackID, AVRCP_Abort_Continuing_Response_Response_Data_t *AbortContinuingResponseResponseData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Set Absolute Volume Command.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param SetAbsoluteVolumeCommandData The data to populate into the command (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the command into.
    * @return This function returns the amount of the data that was copied into the
    * specified buffer to build the command (if successful, positive).
    * or a negative return code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified command.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Set_Absolute_Volume_Command(unsigned int BluetoothStackID, AVRCP_Set_Absolute_Volume_Command_Data_t *SetAbsoluteVolumeCommandData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Set_Absolute_Volume_Command_t)(unsigned int BluetoothStackID, AVRCP_Set_Absolute_Volume_Command_Data_t *SetAbsoluteVolumeCommandData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Set Absolute Volume Response.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param SetAbsoluteVolumeResponseData The data to populate into the response (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the response into.
    * @return This function returns the amount of the data that was copied into
    * the specified buffer to build the response (if successful,
    * positive) or a negative return code if there
    * was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified response.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Set_Absolute_Volume_Response(unsigned int BluetoothStackID, AVRCP_Set_Absolute_Volume_Response_Data_t *SetAbsoluteVolumeResponseData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Set_Absolute_Volume_Response_t)(unsigned int BluetoothStackID, AVRCP_Set_Absolute_Volume_Response_Data_t *SetAbsoluteVolumeResponseData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Set Addressed Player Command.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param SetAddressedPlayerCommandData The data to populate into the command (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the command into.
    * @return This function returns the amount of the data that was copied into the
    * specified buffer to build the command (if successful, positive)
    * or a negative return code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified command.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Set_Addressed_Player_Command(unsigned int BluetoothStackID, AVRCP_Set_Addressed_Player_Command_Data_t *SetAddressedPlayerCommandData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Set_Addressed_Player_Command_t)(unsigned int BluetoothStackID, AVRCP_Set_Addressed_Player_Command_Data_t *SetAddressedPlayerCommandData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Set Addressed Player Response.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param SetAddressedPlayerResponseData The data to populate into the response (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the response into.
    * @return This function returns the amount of the data that was copied into
    * the specified buffer to build the response (if successful,
    * positive) or a negative return code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified response.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Set_Addressed_Player_Response(unsigned int BluetoothStackID, AVRCP_Set_Addressed_Player_Response_Data_t *SetAddressedPlayerResponseData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Set_Addressed_Player_Response_t)(unsigned int BluetoothStackID, AVRCP_Set_Addressed_Player_Response_Data_t *SetAddressedPlayerResponseData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Play Item Command.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param PlayItemCommandData The data to populate into the command (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the command into.
    * @return This function returns the amount of the data that was copied into the specified
    * buffer to build the command (if successful, positive) or a negative return code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified command.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Play_Item_Command(unsigned int BluetoothStackID, AVRCP_Play_Item_Command_Data_t *PlayItemCommandData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Play_Item_Command_t)(unsigned int BluetoothStackID, AVRCP_Play_Item_Command_Data_t *PlayItemCommandData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Play Item Response.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param PlayItemResponseData The data to populate into the response (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the response into.
    * @return This function returns the amount of the data that was copied into the specified
    * buffer to build the response (if successful, positive) or a negative return code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified response.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Play_Item_Response(unsigned int BluetoothStackID, AVRCP_Play_Item_Response_Data_t *PlayItemResponseData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Play_Item_Response_t)(unsigned int BluetoothStackID, AVRCP_Play_Item_Response_Data_t *PlayItemResponseData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Add To Now Playing Command.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param AddToNowPlayingCommandData The data to populate into the command (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the command into.
    * @return This function returns the amount of the data that was copied into the
    * specified buffer to build the command (if successful, positive)
    * or a negative return code if there was an
    * error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified command.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Add_To_Now_Playing_Command(unsigned int BluetoothStackID, AVRCP_Add_To_Now_Playing_Command_Data_t *AddToNowPlayingCommandData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Add_To_Now_Playing_Command_t)(unsigned int BluetoothStackID, AVRCP_Add_To_Now_Playing_Command_Data_t *AddToNowPlayingCommandData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Add To Now Playing Response.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param AddToNowPlayingResponseData The data to populate into the response (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the response into.
    * @return This function returns the amount of the data that was copied into
    * the specified buffer to build the response (if successful,
    * positive) or a negative return code if there
    * was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified response.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Add_To_Now_Playing_Response(unsigned int BluetoothStackID, AVRCP_Add_To_Now_Playing_Response_Data_t *AddToNowPlayingResponseData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Add_To_Now_Playing_Response_t)(unsigned int BluetoothStackID, AVRCP_Add_To_Now_Playing_Response_Data_t *AddToNowPlayingResponseData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Command Reject Response (only applicable for AV/C
    * Specific Commands - Control Channel).
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param CommandRejectResponseData The data to populate into the response (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the response into.
    * @return This function returns the amount of the data that was copied into
    * the specified buffer to build the response (if successful,
    * positive) or a negative return code if there
    * was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified response.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Command_Reject_Response(unsigned int BluetoothStackID, AVRCP_Command_Reject_Response_Data_t *CommandRejectResponseData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Command_Reject_Response_t)(unsigned int BluetoothStackID, AVRCP_Command_Reject_Response_Data_t *CommandRejectResponseData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /* AVRCP Browsing Channel AV/C Commands/Responses.
    * @brief The following function is a utility function that exists to format
    * an AVRCP Set Browsed Player Command (only applicable for the
    * Browsing channel).
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param AddToNowPlayingCommandData The data to populate into the command (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the command into.
    * @return This function returns
    * the amount of the data that was copied into the specified buffer
    * to build the command (if successful, positive) or a negative return code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified command.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Set_Browsed_Player_Command(unsigned int BluetoothStackID, AVRCP_Set_Browsed_Player_Command_Data_t *SetBrowsedPlayerCommandData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Set_Browsed_Player_Command_t)(unsigned int BluetoothStackID, AVRCP_Set_Browsed_Player_Command_Data_t *SetBrowsedPlayerCommandData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Set Browsed Player Response (only applicable for the
    * Browsing Channel).
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param MTUSize The maximum allowable PDU Size (MTU) of the Browsing Channel
    * @param SetBrowsedPlayerResponseData The data to populate into the response (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the response into.
    * @return This function returns the
    * amount of the data that was copied into the specified buffer to
    * build the response (if successful, positive) or a negative return code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified response.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Set_Browsed_Player_Response(unsigned int BluetoothStackID, Word_t MTUSize, AVRCP_Set_Browsed_Player_Response_Data_t *SetBrowsedPlayerResponseData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Set_Browsed_Player_Response_t)(unsigned int BluetoothStackID, Word_t MTUSize, AVRCP_Set_Browsed_Player_Response_Data_t *SetBrowsedPlayerResponseData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Change Path Command (only applicable for the Browsing
    * channel).
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param ChangePathCommandData The data to populate into the command (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the command into.
    * @return This function returns the
    * amount of the data that was copied into the specified buffer to
    * build the command (if successful, positive) or a negative return code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified command.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Change_Path_Command(unsigned int BluetoothStackID, AVRCP_Change_Path_Command_Data_t *ChangePathCommandData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Change_Path_Command_t)(unsigned int BluetoothStackID, AVRCP_Change_Path_Command_Data_t *ChangePathCommandData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Change Path Response (only applicable for the Browsing
    * Channel).
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param ChangePathResponseData The data to populate into the response (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the response into.
    * @return This function returns the
    * amount of the data that was copied into the specified buffer to
    * build the response (if successful, positive) or a negative return code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified response.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Change_Path_Response(unsigned int BluetoothStackID, AVRCP_Change_Path_Response_Data_t *ChangePathResponseData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Change_Path_Response_t)(unsigned int BluetoothStackID, AVRCP_Change_Path_Response_Data_t *ChangePathResponseData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Get Item Attributes Command (only applicable for the
    * Browsing channel).
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param MTUSize The maximum allowable PDU Size (MTU) of the Browsing Channel
    * @param GetItemAttributesCommandData The data to populate into the command (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the command into.
    * @return This function returns the
    * amount of the data that was copied into the specified buffer to
    * build the command (if successful, positive) or a negative return code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified command.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Get_Item_Attributes_Command(unsigned int BluetoothStackID, Word_t MTUSize, AVRCP_Get_Item_Attributes_Command_Data_t *GetItemAttributesCommandData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Get_Item_Attributes_Command_t)(unsigned int BluetoothStackID, Word_t MTUSize, AVRCP_Get_Item_Attributes_Command_Data_t *GetItemAttributesCommandData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Change Path Response (only applicable for the Browsing
    * channel).
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param MTUSize The maximum allowable PDU Size (MTU) of the Browsing Channel
    * @param GetItemAttributesResponseData The data to populate into the response (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the response into.
    * @return This function returns the
    * amount of the data that was copied into the specified buffer to
    * build the response (if successful, positive) or a negative return code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified response.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Get_Item_Attributes_Response(unsigned int BluetoothStackID, Word_t MTUSize, AVRCP_Get_Item_Attributes_Response_Data_t *GetItemAttributesResponseData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Get_Item_Attributes_Response_t)(unsigned int BluetoothStackID, Word_t MTUSize, AVRCP_Get_Item_Attributes_Response_Data_t *GetItemAttributesResponseData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Search Command (only applicable for the Browsing
    * channel).
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param MTUSize The maximum allowable PDU Size (MTU) of the Browsing Channel
    * @param SearchCommandData The data to populate into the command (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the command into.
    * @return This function returns the
    * amount of the data that was copied into the specified buffer to
    * build the command (if successful, positive) or a negative return code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified command.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Search_Command(unsigned int BluetoothStackID, Word_t MTUSize, AVRCP_Search_Command_Data_t *SearchCommandData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Search_Command_t)(unsigned int BluetoothStackID, Word_t MTUSize, AVRCP_Search_Command_Data_t *SearchCommandData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Search Response (only applicable for the Browsing
    * Channel).
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param SearchResponseData The data to populate into the response (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the response into.
    * @return This function returns the
    * amount of the data that was copied into the specified buffer to
    * build the response (if successful, positive) or a negative return code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified response.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Search_Response(unsigned int BluetoothStackID, AVRCP_Search_Response_Data_t *SearchResponseData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Search_Response_t)(unsigned int BluetoothStackID, AVRCP_Search_Response_Data_t *SearchResponseData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Get Folder Items Command (only applicable for the
    * Browsing channel).
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param MTUSize The maximum allowable PDU Size (MTU) of the Browsing Channel
    * @param GetFolderItemsCommandData The data to populate into the command (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the command into.
    * @return This function returns the
    * amount of the data that was copied into the specified buffer to
    * build the command (if successful, positive) or a negative return code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified command.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Get_Folder_Items_Command(unsigned int BluetoothStackID, Word_t MTUSize, AVRCP_Get_Folder_Items_Command_Data_t *GetFolderItemsCommandData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Get_Folder_Items_Command_t)(unsigned int BluetoothStackID, Word_t MTUSize, AVRCP_Get_Folder_Items_Command_Data_t *GetFolderItemsCommandData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP Get Folder Items Response (only applicable for the
    * Browsing channel).
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param MTUSize The maximum allowable PDU Size (MTU) of the Browsing Channel
    * @param GetFolderItemsResponseData The data to populate into the response (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the response into.
    * @return This function returns the
    * amount of the data that was copied into the specified buffer to
    * build the response (if successful, positive) or a negative return code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified response.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_Get_Folder_Items_Response(unsigned int BluetoothStackID, Word_t MTUSize, AVRCP_Get_Folder_Items_Response_Data_t *GetFolderItemsResponseData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_Get_Folder_Items_Response_t)(unsigned int BluetoothStackID, Word_t MTUSize, AVRCP_Get_Folder_Items_Response_Data_t *GetFolderItemsResponseData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is a utility function that exists to format
    * an AVRCP General Reject Response (only applicable for the Browsing
    * Channel).
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param GeneralRejectResponseData The data to populate into the response (required).
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the response into.
    * @return This function returns the
    * amount of the data that was copied into the specified buffer to
    * build the response (if successful, positive) or a negative return code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the specified response.
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Format_General_Reject_Response(unsigned int BluetoothStackID, AVRCP_General_Reject_Response_Data_t *GeneralRejectResponseData, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Format_General_Reject_Response_t)(unsigned int BluetoothStackID, AVRCP_General_Reject_Response_Data_t *GeneralRejectResponseData, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is provided to allow a mechanism for
    * parsing/decoding the specified AVRCP Response Message.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param BrowsingChannelMessage  Set if the message to decode is a Browse Channel message.
    * @param Response  Set if the message to decode is a Response message.
    * @param MessageLength Length of the message in bytes.
    * @param MessageData Pointer to a buffer that contains the message to decode.
    * @param MessageInformation Pointer to a structure that will contain the parsed message data.
    *  @return This function returns zero if successful, or a negative return code if
    * there was an error.
    * \note If @return This function returns success (zero) then the caller
    *          *MUST* call the AVRCP_Free_Decoded_Message() function
    *          with the decoded Message buffer that was passed to this
    *          function.  This guarantees that all resources for the
    *          Message will be freed.
    * \note The decoded Message will have pointers in it that point
    *          to memory that is located within the Message Buffer
    *          passed to this function (third parameter).  The Message
    *          data cannot be freed or altered until after the
    *          AVRCP_Free_Decoded_Message() function is called (i.e.
    *          the caller is finished with the message contents).
    */
BTPSAPI_DECLARATION int BTPSAPI AVRCP_Decode_Message(unsigned int BluetoothStackID, Boolean_t BrowsingChannelMessage, Boolean_t Response, unsigned int MessageLength, Byte_t *MessageData, AVRCP_Message_Information_t *MessageInformation);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_AVRCP_Decode_Message_t)(unsigned int BluetoothStackID, Boolean_t BrowsingChannelMessage, Boolean_t Response, unsigned int MessageLength, Byte_t *MessageData, AVRCP_Message_Information_t *MessageInformation);
#endif

   /*! @brief The following function is provided to free all the resources
    * associated with a AVRCP Decoded Message.  This is the data that
    * was populated into the AVRCP_Message_Information_t buffer that was
    * passed to the AVRCP_Decode_Message() function.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param MessageInformation  Pointer to an AVRCP_Message_Information_t structure to free.
    */
BTPSAPI_DECLARATION void BTPSAPI AVRCP_Free_Decoded_Message(unsigned int BluetoothStackID, AVRCP_Message_Information_t *MessageInformation);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (BTPSAPI *PFN_AVRCP_Free_Decoded_Message_t)(unsigned int BluetoothStackID, AVRCP_Message_Information_t *MessageInformation);
#endif

   /*! @brief The following function is provided to allow a mechanism for
    * rebuilding the specified AVRCP Fragmented Message (based on the
    * specified Fragment List).
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param NumberMessageFragments The number of message fragments.
    * @param MessageFragmentList The Fragmented Message List.
    * @param BufferLength The size of the buffer.
    * @param Buffer A pointer to the buffer to build the response message into.
    * @return This function returns the
    * amount of the data that was copied into the specified buffer to
    * build the response (if successful, positive) or a negative return code if there was an error.
    * \note Passing zero and NULL for the BufferLength and Buffer
    *          parameters (respectively) instructs this function to
    *          simply calculate the number of bytes that are required to
    *          hold the rebuilt response message.
    */
BTPSAPI_DECLARATION long BTPSAPI AVRCP_Rebuild_Fragmented_Message(unsigned int BluetoothStackID, unsigned int NumberMessageFragments, AVRCP_Message_Information_t *MessageFragmentList, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef long (BTPSAPI *PFN_AVRCP_Rebuild_Fragmented_Message_t)(unsigned int BluetoothStackID, unsigned int NumberMessageFragments, AVRCP_Message_Information_t *MessageFragmentList, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is provided to free all the resources
    * associated with the specified AVRCP Message Fragment List. This
    * is the data that was populated into the
    * AVRCP_Message_Information_t buffer that was passed to the
    * AVRCP_Decode_Message() function. This function frees all the decoded Message
    * Information buffers and frees all resources (except the
    * AVRCP_Message_Information_t structures themselves (the array)
    * which the caller owns).
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param NumberMessageFragments This function accepts the number of Fragment Entries
    * that are to be freed.
    * @param MessageFragmentList The Fragmented Message List.
    */
BTPSAPI_DECLARATION void BTPSAPI AVRCP_Free_Fragmented_Message_List(unsigned int BluetoothStackID, unsigned int NumberMessageFragments, AVRCP_Message_Information_t *MessageFragmentList);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (BTPSAPI *PFN_AVRCP_Free_Fragmented_Message_List_t)(unsigned int BluetoothStackID, unsigned int NumberMessageFragments, AVRCP_Message_Information_t *MessageFragmentList);
#endif

#endif
