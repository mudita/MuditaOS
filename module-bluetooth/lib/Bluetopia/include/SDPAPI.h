/*
 * Copyright 2000 - 2014 Stonestreet One.
 * All Rights Reserved.
 * Author:  Damon Lange
 *** MODIFICATION HISTORY ****************************************************
 *
 *   mm/dd/yy  F. Lastname    Description of Modification
 *   --------  -----------    ------------------------------------------------
 *   09/11/00  D. Lange       Initial creation.
 ****************************************************************************
 */

/**
 *  @file SDPAPI.h
 *
 *  @brief Stonestreet One Bluetooth Stack SDP API Type Definitions,
 *      Constants, and Prototypes. To use any of the following API's,
 *      include the following declaration in your application.
 *
 *  @code
 *  #include "SS1BTPS.h"
 *  @endcode
 *  ============================================================================
 */
#ifndef __SDPAPIH__
#define __SDPAPIH__

#include "BTPSKRNL.h"           /*! BTPS Kernel Prototypes/Constants. */

#include "BTAPITyp.h"           /*! Bluetooth API Type Definitions. */
#include "SDPTypes.h"           /*! Bluetooth SDP Type Definitions/Constants. */

#include "BTPSCFG.h"            /*! BTPS Configuration Constants. */

   /*! The following enumerated type represents the different Connection
    * Modes that are supported by SDP. These constants are used with
    * the SDP_Set_Disconnect_Mode() function.
    */
typedef enum
{
   dmAutomatic,
   dmManual
} SDP_Disconnect_Mode_t;

   /*! The following constant specifies the default SDP Disconnect Mode
    * that is used by SDP by default.
    */
#define SDP_DEFAULT_SERVER_DISCONNECT_MODE                           (BTPS_CONFIGURATION_SDP_DEFAULT_DISCONNECT_MODE)

   /*! The following enumerated type represents the supported server
    * connection modes supported by the local SDP Server.
    */
typedef enum
{
   sdAutomaticAccept,
   sdManualAccept
} SDP_Server_Connection_Mode_t;

   /*! The following enumerated type represents all the allowable SDP
    * Data Element Types that can be used with the SDP API.
    */
typedef enum
{
   deNIL,
   deNULL,
   deUnsignedInteger1Byte,
   deUnsignedInteger2Bytes,
   deUnsignedInteger4Bytes,
   deUnsignedInteger8Bytes,
   deUnsignedInteger16Bytes,
   deSignedInteger1Byte,
   deSignedInteger2Bytes,
   deSignedInteger4Bytes,
   deSignedInteger8Bytes,
   deSignedInteger16Bytes,
   deTextString,
   deBoolean,
   deURL,
   deUUID_16,
   deUUID_32,
   deUUID_128,
   deSequence,
   deAlternative
} SDP_Data_Element_Type_t;

   /*! The following structure holds all SDP Information that is required
    * for an SDP Attribute when stored in an array.  This is the
    * structure that is passed to SDP_Add_Raw_Attribute().
    * \note The NumberOfUUIDOffsets and UUIDOffsets must contain the
    *          number of UUID offsets and a pointer to the UUID offset
    *          that contain the index in the AttributeData of the SDP
    *          Data Element type that prefixes each UUID in the
    *          Attribute Data.
    */
typedef struct _tagSDP_ConstantServiceAttributeEntry_t
{
   Byte_t   Flags;
   Word_t   AttributeID;
   DWord_t  AttributeLength;
   DWord_t  NumberOfUUIDOffsets;
   Word_t  *UUIDOffsets;
   Byte_t  *AttributeData;
} SDP_ConstantServiceAttributeEntry_t;

   /*! The following flag is used to indicate if the data pointed to by
    * UUIDOffsets and AttributeData is constant.  If this flag is set
    * then SDP will not make a local copy of this information and
    * instead will used the pointers that are passed in.
    */
#define SDP_CONSTANT_SERVICE_ATTRIBUTE_FLAGS_CONSTANT_DATA         0x01

   /*! The following data type represents a special SDP Data Type.  This
    * structure ONLY holds the UUID Data Types.  This structure is
    * provided so that API calls that only deal with UUID's can use
    * this data type instead of the more generic SDP_Data_Element_Type_t
    * Data Type.  This will aid in code readability and also aid in
    * making the code that only processes UUID's more simple.
    */
typedef struct _tagSDP_UUID_Entry_t
{
   SDP_Data_Element_Type_t SDP_Data_Element_Type;
   union
   {
      UUID_16_t  UUID_16;
      UUID_32_t  UUID_32;
      UUID_128_t UUID_128;
   } UUID_Value;
} SDP_UUID_Entry_t;

#define SDP_UUID_ENTRY_SIZE                             (sizeof(SDP_UUID_Entry_t))

   /*! The following data type represents a special SDP Data Type.  This
    * structure ONLY holds the Attribute ID Information.  This
    * structure is provided so that API calls that only deal with
    * Attribute ID Lists can use this data type instead of the more
    * generic SDP_Data_Element_Type_t Data Type.  This will aid in code
    * readability and also aid in making the code that only processes
    * Attribute ID Lists more simple.
    * The BOOLEAN Flag specifies whether or not the Attribute ID List
    * specifies a Range or a single Attribute ID.  If this flag is
    * TRUE, then the End_Attribute_ID is valid and is used.  The
    * End_Attribute_ID value MUST be greater than the Start_Attribute_ID
    * value of the entry is considered invalid (if the Attribute_Range
    * flag is TRUE).  If the Attribute_Range member is FALSE, then the
    * Start_Attribute_ID member is the only member that is used.
    */
typedef struct _tagSDP_Attribute_ID_List_Entry_t
{
   Boolean_t Attribute_Range;
   Word_t    Start_Attribute_ID;
   Word_t    End_Attribute_ID;
} SDP_Attribute_ID_List_Entry_t;

#define SDP_ATTRIBUTE_ID_LIST_ENTRY_SIZE                (sizeof(SDP_Attribute_ID_List_Entry_t))

   /*! The following Data Structure represents a structure that will hold
    * an individual SDP Data Element.  The SDP_Data_Element_Type field
    * holds the SDP Data Element Type, the Length field holds the number
    * of Bytes that the Actual Data Element Value occupies (this value
    * represents the buffer size that the pointer member of the union
    * points to), and finally, the union defines the actual Data Type
    * value.  It should be noted that the Text Field and the URL
    * members are pointers to data because the data itself is variable
    * in length.
    * \note The following structure also supports the SDP Data Types
    *          of Sequences and Alternatives.  This is treated as any
    *          of the other Data Element Types, the
    *          SDP_Data_Element_Length field denotes the Number of the
    *          Data Elements that the SDP_Data_Element_Sequence OR the
    *          SDP_Data_Element_Alternative Member points to.
    */
typedef struct _tagSDP_Data_Element_t
{
   SDP_Data_Element_Type_t           SDP_Data_Element_Type;
   DWord_t                           SDP_Data_Element_Length;
   union
   {
      Byte_t                         UnsignedInteger1Byte;
      Word_t                         UnsignedInteger2Bytes;
      DWord_t                        UnsignedInteger4Bytes;
      Byte_t                         UnsignedInteger8Bytes[8];
      Byte_t                         UnsignedInteger16Bytes[16];
      SByte_t                        SignedInteger1Byte;
      SWord_t                        SignedInteger2Bytes;
      SDWord_t                       SignedInteger4Bytes;
      Byte_t                         SignedInteger8Bytes[8];
      Byte_t                         SignedInteger16Bytes[16];
      Byte_t                         Boolean;
      UUID_16_t                      UUID_16;
      UUID_32_t                      UUID_32;
      UUID_128_t                     UUID_128;
      Byte_t                        *TextString;
      Byte_t                        *URL;
      struct _tagSDP_Data_Element_t *SDP_Data_Element_Sequence;
      struct _tagSDP_Data_Element_t *SDP_Data_Element_Alternative;
   } SDP_Data_Element;
} SDP_Data_Element_t;

#define SDP_DATA_ELEMENT_SIZE                           (sizeof(SDP_Data_Element_t))

   /*! SDP Event API Types.
    * The following enumerated type represents all the allowable SDP
    * Request Response Data Types that will be returned in the SDP
    * Response Callback Function.
    */
typedef enum
{
   rdTimeout,
   rdConnectionError,
   rdErrorResponse,
   rdServiceSearchResponse,
   rdServiceAttributeResponse,
   rdServiceSearchAttributeResponse,
   rdServiceAttributeResponse_Raw,
   rdServiceSearchAttributeResponse_Raw
} SDP_Response_Data_Type_t;

   /*! The following enumerated type represents all the allowable SDP
    * Connection Event Types that will be returned in the SDP Connection
    * Event Callback function that was registered via the
    * SDP_Set_Server_Connection_Mode()
    */
typedef enum
{
   etSDP_Connection_Request, /*!< Event that signals a connection request has been received. */
   etSDP_Connect_Indication,  /*!< Event that signals a connection indication has been received. */
   etSDP_Disconnect_Indication  /*!< Event that signals a disconnection request has been received. */
} SDP_Connection_Event_Type_t;

   /*! SDP Response Events.
    * The following Data Structure represents the SDP Error Response
    * Information that is returned by a Remote SDP Server when an
    * invalid request has been received.  The Error_Info field is an
    * optional field that may or may be present depending upon the
    * Error Code value.  If there is NO Error Information, then the
    * Error_Info_Length member will be set to zero and the Error_Info
    * member pointer will be NULL.  If there is Error Information, then
    * the Error_Info member will be a non-NULL pointer to the Error
    * Information, and the Error_Info_Length member will contain the
    * Length of the Data (in Bytes) that the Error_Info Pointer points
    * to.
    */
typedef struct _tagSDP_Error_Response_Data_t
{
   Word_t   Error_Code;
   Word_t   Error_Info_Length;
   Byte_t  *Error_Info;
} SDP_Error_Response_Data_t;

#define SDP_ERROR_RESPONSE_DATA_SIZE                    (sizeof(SDP_Error_Response_Data_t))

   /*! The following Data Structure represents the SDP Service Search
    * Response Data that is returned by the Remote SDP Server when a
    * SDP Service Search Request is submitted.  If there are Service
    * Records then the Total_Service_Record_Count member will contain
    * the Number Service Record Handles that the Service_Record_List
    * member points to.  If there are NO Service Record Handles, then
    * the Total_Service_Record_Count member will be zero, and the
    * Service_Record_List member will set to NULL.
    */
typedef struct _tagSDP_Service_Search_Response_Data_t
{
   Word_t   Total_Service_Record_Count;
   DWord_t *Service_Record_List;
} SDP_Service_Search_Response_Data_t;

#define SDP_SERVICE_SEARCH_RESPONSE_DATA_SIZE           (sizeof(SDP_Service_Search_Response_Data_t))

   /*! The following Data Structure represents a Single SDP Service
    * Attribute.  This Attribute consists of an Attribute ID and the
    * Attribute Data which is a SDP Data Element (Note the SDP Data
    * Element could contain a list of SDP Data Elements).
    */
typedef struct _tagSDP_Attribute_Value_Data_t
{
   Word_t              Attribute_ID;
   SDP_Data_Element_t *SDP_Data_Element;
} SDP_Service_Attribute_Value_Data_t;

#define SDP_SERVICE_ATTRIBUTE_VALUE_DATA_SIZE           (sizeof(SDP_Service_Attribute_Value_Data_t))

   /*! The following Data Structure represents an Attribute List that
    * the Remote SDP Server returns when a SDP Service Attribute Request
    * is processed.  If there are Attributes, then the
    * Number_Attribute_Values member will contain a non-zero value, and
    * the SDP_Service_Attribute_Value_Data will point to an array of
    * Service Attributes (Attribute ID/Attribute Data).  If the
    * Number_Attribute_Values member is zero, then there are NO
    * Attributes present in the list, and the
    * SDP_Service_Attribute_Value_Data member will be NULL.
    */
typedef struct _tagSDP_Attribute_Response_Data_t
{
   Word_t                              Number_Attribute_Values;
   SDP_Service_Attribute_Value_Data_t *SDP_Service_Attribute_Value_Data;
} SDP_Service_Attribute_Response_Data_t;

#define SDP_SERVICE_ATTRIBUTE_RESPONSE_DATA_SIZE        (sizeof(SDP_Service_Attribute_Response_Data_t))

   /*! The following Data Structure represents a Service Record Attribute
    * List that the Remote SDP Server returns when a SDP Service Search
    * Attribute Request is processed.  If there were Service Records
    * that matched the requested Search Pattern, then the
    * Number_Service_Records member will contain a non-zero value, and
    * the SDP_Service_Attribute_Response member will point to an array
    * of SDP_Service_Attribute_Response_t data structures.  Each element
    * of this array will specify the Attribute(s) for an individual
    * Service Record .  If the Number_Service_Records member is zero,
    * then there are NO Service Records (and thus NO Attributes)
    * present in the list, and the SDP_Service_Attribute_Response_Data
    * member will be NULL.
    */
typedef struct _tagSDP_Search_Attribute_Response_Data_t
{
   Word_t                                 Number_Service_Records;
   SDP_Service_Attribute_Response_Data_t *SDP_Service_Attribute_Response_Data;
} SDP_Service_Search_Attribute_Response_Data_t;

#define SDP_SERVICE_SEARCH_ATTRIBUTE_RESPONSE_DATA_SIZE (sizeof(SDP_Service_Search_Attribute_Response_Data_t))

   /*! The following Data Structure represents the raw Service Record
    * data the Remote SDP Server returns when either a SDP Service
    * Attribute Request or SDP Service Search Attribute Request is
    * processed.  There is not processing of the data (other than it
    * will not contain any continuation information).
    * \note This event is only valid for the following event types:
    *             - rdServiceAttributeResponse_Raw
    *             - rdServiceSearchAttributeResponse_Raw
    *          and the data format will be the raw representation of
    *          the correct response data for the corresponding SDP
    *          search result.
    */
typedef struct _tagSDP_Raw_Attribute_Response_Data_t
{
   SDP_Response_Data_Type_t  SDP_Response_Data_Type;
   DWord_t                   Raw_Attribute_Data_Length;
   Byte_t                   *Raw_Attribute_Data;
} SDP_Raw_Attribute_Response_Data_t;

#define SDP_RAW_ATTRIBUTE_RESPONSE_DATA_SIZE            (sizeof(SDP_Raw_Attribute_Response_Data_t))

   /*! The following Data Structure represents the Data that is
    * returned in the SDP Response Callback.  This Data Structure
    * is a container for ALL possible Data that could be returned in
    * a SDP Response Callback.  Note that the type of Data returned
    * depends on the SDP Request that the callback was installed for.
    * Also note that for the rdTimeout and rdConnectionError
    * SDP_Response_Data_Types there is NO further Data available for
    * these Errors.
    */
typedef struct _tagSDP_Response_Data_t
{
   SDP_Response_Data_Type_t SDP_Response_Data_Type;
   union
   {
      SDP_Error_Response_Data_t                    SDP_Error_Response_Data;
      SDP_Service_Search_Response_Data_t           SDP_Service_Search_Response_Data;
      SDP_Service_Attribute_Response_Data_t        SDP_Service_Attribute_Response_Data;
      SDP_Service_Search_Attribute_Response_Data_t SDP_Service_Search_Attribute_Response_Data;
      SDP_Raw_Attribute_Response_Data_t            SDP_Raw_Attribute_Response_Data;
   } SDP_Response_Data;
} SDP_Response_Data_t;

#define SDP_RESPONSE_DATA_SIZE                          (sizeof(SDP_Response_Data_t))

   /*! SDP Connection Events. */
   /*! The following event is dispatched when a remote device is
    * requesting a SDP connection to the local device and the connection
    * mode has been explicitly set to sdManualAccept (via the
    * SDP_Set_Server_Connection_Mode() function).  The BD_ADDR specifies
    * the Bluetooth Address of the Remote Device that is connecting.
    * \note This event is only dispatched to servers that are in
    *          Manual Accept Mode.
    * \note This event must be responded to with the
    *          SDP_Connect_Request_Response() function in order to
    *          accept the outstanding Connect Request.
    */
typedef struct _tagSDP_Connect_Request_Indication_Data_t
{
   BD_ADDR_t BD_ADDR;
} SDP_Connect_Request_Indication_Data_t;

#define SDP_CONNECT_REQUEST_INDICATION_DATA_SIZE        (sizeof(SDP_Connect_Request_Indication_Data_t))

   /*! This event is dispatched by the local SDP server when a remote SDP
    * client has connected.
    * \note This event is only dispatched to the event callback that
    *          was registered via the SDP_Set_Server_Connection_Mode()
    *          function.
    */
typedef struct _tagSDP_Connect_Indication_Data_t
{
   BD_ADDR_t BD_ADDR;
} SDP_Connect_Indication_Data_t;

#define SDP_CONNECT_INDICATION_DATA_SIZE                (sizeof(SDP_Connect_Indication_Data_t))

   /*! This event is dispatched by the local SDP server when a remote SDP
    * client has disconnected.
    * \note This event is only dispatched to the event callback that
    *          was registered via the SDP_Set_Server_Connection_Mode()
    *          function.
    */
typedef struct _tagSDP_Disconnect_Indication_Data_t
{
   BD_ADDR_t BD_ADDR;
} SDP_Disconnect_Indication_Data_t;

#define SDP_DISCONNECT_INDICATION_DATA_SIZE             (sizeof(SDP_Disconnect_Indication_Data_t))

   /*! The following structure represents the container structure for
    * holding all SDP connection event data.
    * \note This event data is only applicable to the event callback
    *          that was registered via the
    *          SDP_Set_Server_Connection_Mode() function.
    */
typedef struct _tagSDP_Connection_Event_Data_t
{
   SDP_Connection_Event_Type_t Event_Data_Type;
   Word_t                      Event_Data_Size;
   union
   {
      SDP_Connect_Request_Indication_Data_t *SDP_Connect_Request_Indication_Data;
      SDP_Connect_Indication_Data_t         *SDP_Connect_Indication_Data;
      SDP_Disconnect_Indication_Data_t      *SDP_Disconnect_Indication_Data;
   } Event_Data;
} SDP_Connection_Event_Data_t;

#define SDP_CONNECTION_EVENT_DATA_SIZE                  (sizeof(SDP_Connection_Event_Data_t))

   /*! The following structure is used with the
    * SDP_Parse_Raw_Attribute_Response_Data() function to hold parsed
    * SDP Attribute Response Data.  Please see @brief The following functions
    * for more information:
    *   - SDP_Service_Attribute_Request_Raw()
    *   - SDP_Service_Search_Attribute_Request_Raw()
    *   - SDP_Parse_Raw_Attribute_Response_Data()
    */
typedef struct _tagSDP_Parsed_Attribute_Response_Data_t
{
   SDP_Response_Data_t  SDP_Response_Data;
   void                *RESERVED;
} SDP_Parsed_Attribute_Response_Data_t;

   /*! The following declared type represents the prototype Function for
    * a SDP Request Callback.  This function will be called whenever
    * a SDP Request Response returns withith the Bluetooth Protocol
    * Stack that is specified with the specified Bluetooth Stack ID.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param SDPRequestID Unique identifier associated with an outstanding Request.
    * @param SDP_Response_Data Pointer to aSDP_Response_Data_t structure that
    * contains the results from an SDP request.  This structure is defined below.
    * @param CallbackParameter User defined value received in the DSP request
    * and dispatched with the SDP response. This can be used to uniquely identify
    * a response when multiple requests are outstanding.
    * The caller is free to use the
    * contents of the SDP Request Response Data ONLY in the context of
    * this callback.  If the caller requires the Data for a longer
    * period of time, then the callback function MUST copy the data
    * into another Data Buffer(s).  This function is guaranteed NOT to
    * be invoked more than once simultaneously for the specified
    * installed callback (i.e. this function DOES NOT have to be
    * reentrant).  The processing in this function should be as
    * efficient as possible.  It should be noted that this function is
    * called in the Thread Context of a Thread that the User does NOT
    * own.  Therefore, processing in this function should be as
    * possible.
    * \note This function MUST NOT Block and wait for events that
    *            can only be satisfied by Receiving other Bluetooth
    *            Stack Events.  A Deadlock WILL occur because other
    *            Callbacks might not be issued while this function is
    *            currently outstanding.
    */
typedef void (BTPSAPI *SDP_Response_Callback_t)(unsigned int BluetoothStackID, unsigned int SDPRequestID, SDP_Response_Data_t *SDP_Response_Data, unsigned long CallbackParameter);

   /*! The following declared type represents the prototype function for
    * a SDP connection event Callback.  This function will be called
    * whenever a SDP connection Event occurs that is associated with the
    * specified Bluetooth stack ID.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param SDP_Connection_Event_Data Pointer to the passed event data.
    * See definitions in the next section.
    * @param CallbackParameter User-defined parameter (e.g., tag value)
    * that was defined in the callback registration.
    * If the caller requires the data for a longer period of time, then the callback
    * function MUST copy the data into another data buffer.  This
    * function is guaranteed NOT to be invoked more than once
    * simultaneously for the specified installed callback (i.e.  this
    * function DOES NOT have be reentrant).  It needs to be noted,
    * however, that if the same callback is installed more than once,
    * then the callbacks will be called serially.  Because of this, the
    * processing in this function should be as efficient as possible.
    * It should also be noted that this function is called in the thread
    * context of a thread that the user does NOT own.  Therefore,
    * processing in this function should be as efficient as possible
    * (this argument holds anyway because another SDP Event will not be
    * processed while this function call is outstanding).
    * \note This function MUST NOT block and wait for events that
    *            can only be satisfied by receiving SDP events.  A
    *            deadlock WILL occur because NO SDP event callbacks will
    *            be issued while this function is currently outstanding.
    */
typedef void (BTPSAPI *SDP_Connection_Event_Callback_t)(unsigned int BluetoothStackID, SDP_Connection_Event_Data_t *SDP_Connection_Event_Data, unsigned long CallbackParameter);

   /*! @brief The following function is responsible for adding an SDP Service
    * Record to the SDP Database.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param NumberServiceClassUUID Number of UUIDs that are present in the array of UUIDs
    * @param SDP_UUID_Entry Array of UUIDs that represent the ServiceClassIDList
    * attributes of the Service Record.
    * @return This function returns a negative return value if the specified
    * Service Record was unable to be added to the SDP Database.
    * \note The UUID values that are specified in this Service
    *          Record call represent the ServiceClassIDList Attributes
    *          of the Service Record.  This parameter is required
    *          because EVERY SDP Service Record MUST have this
    *          Attribute.
    */
BTPSAPI_DECLARATION long BTPSAPI SDP_Create_Service_Record(unsigned int BluetoothStackID, unsigned int NumberServiceClassUUID, SDP_UUID_Entry_t SDP_UUID_Entry[]);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef long (BTPSAPI *PFN_SDP_Create_Service_Record_t)(unsigned int BluetoothStackID, unsigned int NumberServiceClassUUID, SDP_UUID_Entry_t SDP_UUID_Entry[]);
#endif

   /*! @brief The following function is responsible for Updating the Service
    * Class Attribute for the specified SDP Service Record.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param Service_Record_Handle Handle to the service record to be deleted.
    * This value is obtained from a successful call to SDP_Create_Service_Record.
    * @param NumberServiceClassUUID Number of UUIDs that are present in the array of UUIDs
    * @param SDP_UUID_Entry[] Array of UUIDs that represent the
    * ServiceClassIDList attributes of the Service Record.
    * @return This function returns zero if the Service class was updated
    * successfully, or a negative value if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI SDP_Update_Service_Record_Service_Class(unsigned int BluetoothStackID, DWord_t Service_Record_Handle, unsigned int NumberServiceClassUUID, SDP_UUID_Entry_t SDP_UUID_Entry[]);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SDP_Update_Service_Record_Service_Class_t)(unsigned int BluetoothStackID, DWord_t Service_Record_Handle, unsigned int NumberServiceClassUUID, SDP_UUID_Entry_t SDP_UUID_Entry[]);
#endif

   /*! @brief The following function is responsible for Deleting a SDP Service
    * Record that was added with the SDP_Create_Service_Record()
    * function.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param Service_Record_Handle Handle to the service record to be deleted.
    * This value is obtained from a successful call to SDP_Create_Service_Record.
    * @return This function returns zero if the
    * specified Service Record was able to be deleted successfully, or
    * a negative return error code if the Service Record was NOT able to
    * be deleted successfully.  If this function completes successfully
    * the Service Record is NO longer valid on the specified SDP
    * Server.
    */
BTPSAPI_DECLARATION int BTPSAPI SDP_Delete_Service_Record(unsigned int BluetoothStackID, DWord_t Service_Record_Handle);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SDP_Delete_Service_Record_t)(unsigned int BluetoothStackID, DWord_t Service_Record_Handle);
#endif

   /*! @brief The following function is responsible for Adding SDP Service
    * Attributes to the specified SDP Service Record.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param Service_Record_Handle Handle to the service record of the service to
    * add the attribute to.  This value is obtained from a successful call
    * to SDP_Create_Service_Record.
    * @param Attribute_ID Unique identifier that distinguishes this attribute
    * from other service attributes.
    * @param SDP_Data_Element Pointer to an SDP_Data_Element_t structure.
    * This structure contains the Attribute information to be associated with the Attribute_ID.
    * @return This function returns zero if the specified Attribute was added
    * successfully, or a negative return value if the specified
    * attribute was unable to be added to the specified SDP Service
    * Record.
    * \note The SDP_ATTRIBUTE_ID_SERVICE_RECORD_HANDLE and the
    *          SDP_ATTRIBUTE_ID_SERVICE_CLASS_ID_LIST Attributes cannot
    *          be added since they were already added when the Service
    *          Record was created with the SDP_Create_Service_Record()
    *          function.
    * \note Empty Lists can be added as an Attribute by specifying
    *          that there are NO Data Elements present in the Data
    *          Element Sequence or Data Element Alternative (Data
    *          Element Length will be zero which signifies that the
    *          list contains NO Data Elements).
    */
BTPSAPI_DECLARATION int BTPSAPI SDP_Add_Attribute(unsigned int BluetoothStackID, DWord_t Service_Record_Handle, Word_t Attribute_ID, SDP_Data_Element_t *SDP_Data_Element);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SDP_Add_Attribute_t)(unsigned int BluetoothStackID, DWord_t Service_Record_Handle, Word_t Attribute_ID, SDP_Data_Element_t *SDP_Data_Element);
#endif

   /*! @brief The following function is responsible for Adding SDP Service Raw
    * Attributes (i.e. prebuilt) to the specified SDP Service Record.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param Service_Record_Handle Handle to the service record of the
    * service to add the attribute to.  This value is obtained from a
    * successful call to SDP_Create_Service_Record.
    * @param AttributeEntry Pointer to a structure containing information
    * on the attribute that is to be added to the specified service.
    * @return This function returns zero if the specified
    * Attribute was added successfully, or a negative return value if
    * the specified attribute was unable to be added to the specified
    * SDP Service Record.
    * \note The SDP_ATTRIBUTE_ID_SERVICE_RECORD_HANDLE and the
    *          SDP_ATTRIBUTE_ID_SERVICE_CLASS_ID_LIST Attributes cannot
    *          be added since they were already added when the Service
    *          Record was created with the SDP_Create_Service_Record()
    *          function.
    * \note Empty Lists can be added as an Attribute by specifying
    *          that there are NO Data Elements present in the Data
    *          Element Sequence or Data Element Alternative (Data
    *          Element Length will be zero which signifies that the list
    *          contains NO Data Elements).
    */
BTPSAPI_DECLARATION int BTPSAPI SDP_Add_Raw_Attribute(unsigned int BluetoothStackID, DWord_t Service_Record_Handle, SDP_ConstantServiceAttributeEntry_t *AttributeEntry);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SDP_Add_Raw_Attribute_t)(unsigned int BluetoothStackID, DWord_t Service_Record_Handle, SDP_ConstantServiceAttributeEntry_t *AttributeEntry);
#endif

   /*! @brief The following function is responsible for Deleting a SDP Attribute
    * that was previously added via a successful call to the
    * SDP_Add_Attribute() function.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param Service_Record_Handle Handle to the service record to be deleted.
    * This value is obtained from a successful call to SDP_Create_Service_Record.
    * @param Attribute_ID Unique identifier that distinguishes this attribute to be removed.
    * @return This function returns
    * zero if the Attribute was successfully deleted, or a negative
    * return error code if the Attribute was NOT able to be deleted
    * successfully.  If this function has completes successfully then
    * the specifed Attribute no longer exists in the specified SDP
    * Service Handle.
    * \note The SDP_ATTRIBUTE_ID_SERVICE_RECORD_HANDLE and the
    *          SDP_ATTRIBUTE_ID_SERVICE_CLASS_ID_LIST Attributes cannot
    *          be deleted since they are required to exist for EVERY
    *          SDP Service Record.
    */
BTPSAPI_DECLARATION int BTPSAPI SDP_Delete_Attribute(unsigned int BluetoothStackID, DWord_t Service_Record_Handle, Word_t Attribute_ID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SDP_Delete_Attribute_t)(unsigned int BluetoothStackID, DWord_t Service_Record_Handle, Word_t Attribute_ID);
#endif

   /*! @brief The following function is responsible for issuing an SDP Service
    * Search Request to the specified BD_ADDR.  This function will
    * return the result of the Search Request in the SDP Response
    * Callback that is specified in the calling of this function.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param BD_ADDR Address of the Bluetooth device where the SDP Server resides.
    * @param MaximumServiceRecordCount Specifies the Maximum number of service
    * records to be returned for this request.
    * @param NumberServiceUUID Number of Service UUIDs that are contained in
    * the array of Service UUIDs.
    * @param SDP_UUID_Entry Pointer to an array of Service UUIDs that will
    * serve as the Service Search Pattern.  This parameter must point to an
    * array that contains the number of entries specified by the NumberServiceUUID parameter.
    * @param SDP_Response_Callback Callback function pointer of type
    * SDP_Response_Callback_t to be used to dispatch the result of the Service Search.
    * @param CallbackParameter User-defined value to be dispatched with
    * the result of the request.  This can be used to uniquely identify a
    * response when multiple requests are outstanding.
    * @return This function will return a positive, non-zero,
    * return code if successful, or a negative return error code if
    * there was an error.  If this function is successful, the user can
    * call the SDP_Cancel_Service_Request() function to cancel a the
    * SDP Service Search Request prematurely. It should be noted that
    * the Number of UUID Parameter must be at least one, and the
    * Service UUID Parameter must point to a List of at least the
    * Number of UUID's that have been specified.  Finally, the BD_ADDR
    * Parameter and the SDP_Reponse_Callback Parameter MUST be valid or
    * the call to this function will be unsuccessful.
    */
BTPSAPI_DECLARATION int BTPSAPI SDP_Service_Search_Request(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t MaximumServiceRecordCount, unsigned int NumberServiceUUID, SDP_UUID_Entry_t SDP_UUID_Entry[], SDP_Response_Callback_t SDP_Response_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SDP_Service_Search_Request_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t MaximumServiceRecordCount, unsigned int NumberServiceUUID, SDP_UUID_Entry_t SDP_UUID_Entry[], SDP_Response_Callback_t SDP_Response_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is responsible for issuing an SDP Service
    * Attribute Request to the specified BD_ADDR.  This function will
    * return the result of the Attribute Request in the SDP Response
    * Callback that is specified in the calling of this function.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR  Address of the Bluetooth device where the SDP Server
    * resides.
    * @param ServiceRecordHandle Handle of the remote service record to
    * be searched.  This value is either known in advance or is determined
    * by looking at the SDP_Service_Search_Response data.
    * @param NumberAttributeListElements Number of Attribute Elements that
    * are contained in the array of Attribute Elements.
    * @param AttributeIDList Array of Attribute Elements on which to search.
    * @param SDP_Response_Callback Callback function pointer of type
    * SDP_Response_Callback_t to be used to dispatch the result of the Service Search.
    * @param CallbackParameter User-defined value to be dispatched with
    * the result of the request.  This can be used to uniquely identify a
    * response when multiple requests are outstanding.
    * @return This function will return a positive, non-zero, return
    * code if successful, or a
    * negative return error code if there was an error.  If this
    * function is successful, the user can call the
    * SDP_Cancel_Service_Request() function to cancel a the SDP Service
    * Search Request prematurely.  It should be noted that the Number
    * of Attributes that are contained in the Attribute List must be at
    * least one, and the Attribute ID List Parameter must point to a
    * List of Attribute ID's that contains at least the Number of
    * Attribute List Entries that have been specified.  Finally, the
    * BD_ADDR Parameter and the SDP_Reponse_Callback Parameter MUST
    * be valid or the call to this function will be unsuccessful.
    */
BTPSAPI_DECLARATION int BTPSAPI SDP_Service_Attribute_Request(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, DWord_t ServiceRecordHandle, unsigned int NumberAttributeListElements, SDP_Attribute_ID_List_Entry_t AttributeIDList[], SDP_Response_Callback_t SDP_Response_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SDP_Service_Attribute_Request_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, DWord_t ServiceRecordHandle, unsigned int NumberAttributeListElements, SDP_Attribute_ID_List_Entry_t AttributeIDList[], SDP_Response_Callback_t SDP_Response_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is responsible for issuing an SDP Service
    * Attribute Request to the specified BD_ADDR.  This function will
    * return the result of the Attribute Request in the SDP Response
    * Callback that is specified in the calling of this function.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR  Address of the Bluetooth device where the SDP Server
    * resides.
    * @param ServiceRecordHandle Handle of the remote service record to
    * be searched.  This value is either known in advance or is determined
    * by looking at the SDP_Service_Search_Response data.
    * @param NumberAttributeListElements Number of Attribute Elements that
    * are contained in the array of Attribute Elements.
    * @param AttributeIDList Array of Attribute Elements on which to search.
    * @param SDP_Response_Callback Callback function pointer of type
    * SDP_Response_Callback_t to be used to dispatch the result of the Service Search.
    * @param CallbackParameter User-defined value to be dispatched
    * with the result of the request.  This can be used to uniquely
    * identify a response when multiple requests are outstanding.
    * @return This function will return a positive, non-zero, return code if successful, or a
    * negative return error code if there was an error.  If this
    * function is successful, the user can call the
    * SDP_Cancel_Service_Request() function to cancel a the SDP Service
    * Search Request prematurely.  It should be noted that the Number
    * of Attributes that are contained in the Attribute List must be at
    * least one, and the Attribute ID List Parameter must point to a
    * List of Attribute ID's that contains at least the Number of
    * Attribute List Entries that have been specified.  Finally, the
    * BD_ADDR Parameter and the SDP_Reponse_Callback Parameter MUST
    * be valid or the call to this function will be unsuccessful.
    * \note This function is identical to the
    *
    *             SDP_Service_Attribute_Request() function, except
    *
    *          that a successful response will be the following event:
    *
    *             rdServiceAttributeResponse_Raw
    *
    *          (and the SDP Result data will be un-parsed).
    * \note If valid un-parsed SDP Response data is received via the
    *          event above, then the caller is free to parse the data
    *          manually or the caller can call the:
    *
    *             SDP_Parse_Raw_Attribute_Response_Data()
    *
    *          function to parse the data into the Bluetopia parsed
    *          SDP data format.
    */
BTPSAPI_DECLARATION int BTPSAPI SDP_Service_Attribute_Request_Raw(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, DWord_t ServiceRecordHandle, unsigned int NumberAttributeListElements, SDP_Attribute_ID_List_Entry_t AttributeIDList[], SDP_Response_Callback_t SDP_Response_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SDP_Service_Attribute_Request_Raw_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, DWord_t ServiceRecordHandle, unsigned int NumberAttributeListElements, SDP_Attribute_ID_List_Entry_t AttributeIDList[], SDP_Response_Callback_t SDP_Response_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is responsible for issuing an SDP Service
    * Search Attribute Request to the specified BD_ADDR.  This function
    * will return the result of the Service Search Attribute Request in
    * the SDP Response Callback that is specified in the calling of this
    * function.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR  Address of the Bluetooth device where the SDP Server
    * resides.
    * @param NumberServiceUUID Number of Service UUIDs that area contained
    * in an array of Service UUIDs.
    * @param SDP_UUID_Entry Pointer to an array of Service UUIDs that will
    * serve as the Service Search Pattern.  This parameter must point
    * to an array that contains the number of entries specified by the
    * NumberServiceUUID parameter.
    * @param NumberAttributeListElements Number of Attribute Elements that
    * are contained in the array of Attribute Elements.
    * @param AttributeIDList Array of Attribute Elements on which to search.
    * @param SDP_Response_Callback Callback function pointer of type
    * SDP_Response_Callback_t to be used to dispatch the result of the Service Search.
    * @param CallbackParameter User-defined value to be dispatched with
    * the result of the request.  This can be used to uniquely identify
    * a response when multiple requests are outstanding.
    * @return This function will return a
    * positive, non-zero, return code if successful, or a negative
    * return error code if there was an error.  If this function is
    * successful, the user can call the SDP_Cancel_Service_Request()
    * function to cancel a the SDP Service Search Request prematurely.
    * It should be noted that the Number of UUID Parameter must be at
    * least one, and the Service UUID Parameter must point to a List of
    * at least the Number of UUID's that have been specified.  Finally,
    * the BD_ADDR Parameter and the SDP_Reponse_Callback Parameter MUST
    * be valid or the call to this function will be unsuccessful.
    * It should also be noted that the Number of Attributes that are
    * in the Attribute List must be at least one, and the Attribute ID
    * List Parameter must point to a List of Attribute ID's that
    * contains at least the Number of Attribute List Entries that have
    * been specified.  Finally, the BD_ADDR Parameter and the
    * SDP_Reponse_Callback Parameter MUST be valid or the call to this
    * function will be unsuccessful.
    */
BTPSAPI_DECLARATION int BTPSAPI SDP_Service_Search_Attribute_Request(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, unsigned int NumberServiceUUID, SDP_UUID_Entry_t SDP_UUID_Entry[], unsigned int NumberAttributeListElements, SDP_Attribute_ID_List_Entry_t AttributeIDList[], SDP_Response_Callback_t SDP_Response_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SDP_Service_Search_Attribute_Request_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, unsigned int NumberServiceUUID, SDP_UUID_Entry_t SDP_UUID_Entry[], unsigned int NumberAttributeListElements, SDP_Attribute_ID_List_Entry_t AttributeIDList[], SDP_Response_Callback_t SDP_Response_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is responsible for issuing an SDP Service
    * Search Attribute Request to the specified BD_ADDR.  This function
    * will return the result of the Service Search Attribute Request in
    * the SDP Response Callback that is specified in the calling of this
    * function.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR  Address of the Bluetooth device where the SDP Server
    * resides.
    * @param NumberServiceUUID Number of Service UUIDs that area contained
    * in an array of Service UUIDs.
    * @param SDP_UUID_Entry Pointer to an array of Service UUIDs that will
    * serve as the Service Search Pattern.  This parameter must point to
    * an array that contains the number of entries specified by the
    * NumberServiceUUID parameter.
    * @param NumberAttributeListElements Number of Attribute Elements that
    * are contained in the array of Attribute Elements.
    * @param AttributeIDList Array of Attribute Elements on which to search.
    * @param SDP_Response_Callback Callback function pointer of type
    * SDP_Response_Callback_t to be used to dispatch the result of the Service Search.
    * @param CallbackParameter User-defined value to be dispatched with
    * the result of the request.  This can be used to uniquely identify
    * a response when multiple requests are outstanding.
    * @return This function will return a
    * positive, non-zero, return code if successful, or a negative
    * return error code if there was an error.  If this function is
    * successful, the user can call the SDP_Cancel_Service_Request()
    * function to cancel a the SDP Service Search Request prematurely.
    * It should be noted that the Number of UUID Parameter must be at
    * least one, and the Service UUID Parameter must point to a List of
    * at least the Number of UUID's that have been specified.  Finally,
    * the BD_ADDR Parameter and the SDP_Reponse_Callback Parameter MUST
    * be valid or the call to this function will be unsuccessful.
    * It should also be noted that the Number of Attributes that are
    * in the Attribute List must be at least one, and the Attribute ID
    * List Parameter must point to a List of Attribute ID's that
    * contains at least the Number of Attribute List Entries that have
    * been specified.  Finally, the BD_ADDR Parameter and the
    * SDP_Reponse_Callback Parameter MUST be valid or the call to this
    * function will be unsuccessful.
    * \note This function is identical to the
    *
    *             SDP_Service_Search_Attribute_Request() function,
    *
    *          except that a successful response will be the following
    *          event:
    *
    *             rdServiceSearchAttributeResponse_Raw
    *
    *          (and the SDP Result Data will be un-parsed).
    * \note If valid un-parsed SDP Response data is received via the
    *          event above, then the caller is free to parse the data
    *          manually or the caller can call the:
    *
    *             SDP_Parse_Raw_Attribute_Response_Data()
    *
    *          function to parse the data into the Bluetopia parsed
    *          SDP data format.
    */
BTPSAPI_DECLARATION int BTPSAPI SDP_Service_Search_Attribute_Request_Raw(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, unsigned int NumberServiceUUID, SDP_UUID_Entry_t SDP_UUID_Entry[], unsigned int NumberAttributeListElements, SDP_Attribute_ID_List_Entry_t AttributeIDList[], SDP_Response_Callback_t SDP_Response_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SDP_Service_Search_Attribute_Request_Raw_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, unsigned int NumberServiceUUID, SDP_UUID_Entry_t SDP_UUID_Entry[], unsigned int NumberAttributeListElements, SDP_Attribute_ID_List_Entry_t AttributeIDList[], SDP_Response_Callback_t SDP_Response_Callback, unsigned long CallbackParameter);
#endif

   /* This function is responsible for terminating a currently
    * executing SDP Service Request.  This function accepts as input the
    * Bluetooth Protocol Stack ID of the Bluetooth Protocol Stack the
    * SDP Service Request was issued on, and the SDP Service Request
    * ID of the SDP Service Request that was issued.  The SDP Service
    * Request ID is obtained via a successful call to one of the
    * following functions:
    *    - SDP_Service_Search_Request()
    *    - SDP_Service_Attribute_Request()
    *    - SDP_Service_Search_Attribute_Request()
    * After this function is called, the callback that was installed
    * for the specified SDP Service Request will NOT be called and the
    * caller will NOT have access to the SDP Service Response
    * Information for the SDP Service Request.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param ServiceRequestID Unique identifier associated with an outstanding Request.
    */
BTPSAPI_DECLARATION void BTPSAPI SDP_Cancel_Service_Request(unsigned int BluetoothStackID, unsigned int ServiceRequestID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (BTPSAPI *PFN_SDP_Cancel_Service_Request_t)(unsigned int BluetoothStackID, unsigned int ServiceRequestID);
#endif

   /*! @brief The following function is a utility function that exists to parse
    * the specified Raw SDP Attribute Response Data into the Bluetopia
    * SDP API (Parsed) format.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param RawAttributeResponseData The raw SDP Attribute Response data to parse.
    * @param ParsedAttributeResponseData Must contain a pointer to a
    * structure that is to receive the parsed SDP Attribute Response
    * information upon a successful return.
    * @return This function returns zero if successful or a negative value if an
    * error occurred.
    * \note If this function is successful the final parameter *MUST*
    *          be passed to the
    *          SDP_Free_Parsed_Attribute_Response_Data() to free any
    *          allocated resources that were allocated to track the
    *          Parsed SDP Data.  Failure to do this WILL RESULT IN A
    *          RESOURCE/MEMORY LEAK!
    * \note The Raw SDP Attribute Response Data (second parameter)
    *          *MUST* remain active while the data is processed as well
    *          as even during the call to the
    *          SDP_Free_Parsed_Attribute_Response_Data() function.
    *          This buffer and the data that this buffer points to can
    *          ONLY be freed after the call to the
    *          SDP_Free_Parsed_Attribute_Response_Data() function.
    */
BTPSAPI_DECLARATION int BTPSAPI SDP_Parse_Raw_Attribute_Response_Data(unsigned int BluetoothStackID, SDP_Raw_Attribute_Response_Data_t *RawAttributeResponseData, SDP_Parsed_Attribute_Response_Data_t *ParsedAttributeResponseData);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SDP_Parse_Raw_Attribute_Response_Data_t)(unsigned int BluetoothStackID, SDP_Raw_Attribute_Response_Data_t *RawAttributeResponseData, SDP_Parsed_Attribute_Response_Data_t *ParsedAttributeResponseData);
#endif

   /*! @brief The following function is provided to allow a mechanism to free
    * all resources that were allocated to parse Raw SDP Response Data
    * into Bluetopia Parsed SDP Data. See the
    * SDP_Parse_Raw_Attribute_Response_Data() function for more
    * information.
    * @param ParsedAttributeResponseData Must contain a pointer to a structure
    * that was passed to the successful call to SDP_Parse_Raw_Attribute_Response_Data().
    */
BTPSAPI_DECLARATION void BTPSAPI SDP_Free_Parsed_Attribute_Response_Data(SDP_Parsed_Attribute_Response_Data_t *ParsedAttributeResponseData);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (BTPSAPI *PFN_SDP_Free_Parsed_Attribute_Response_Data_t)(SDP_Parsed_Attribute_Response_Data_t *ParsedAttributeResponseData);
#endif

   /*! @brief This function is responsible for informing the SDP Module how it
    * is to execute SDP Service Requests regarding the Connection
    * Disconnection.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param SDPDisconnectMode What type of mode should be set.
    * The possible values are:
    * dmAutomatic {default mode}
    * dmManual
    * @return This function will return zero if the Connection Mode was
    * successfully set, or a negative return error code if there was an
    * error.
    * \note If the caller specifies SDP Disconnect Mode dmManual
    *          then the caller is responsible for disconnecting the
    *          the SDP Connection (to the remote server) by calling
    *          the SDP_Disconnect_Server() function.  If the SDP
    *          Disconnect Mode dmAutomatic is chosen (default) then the
    *          Connection to the server is automatically terminated
    *          when the SDP Transaction completes.
    * \note The SDP Connection Mode can ONLY be changed when there
    *          are NO Client SDP Transactions outstanding.
    */
BTPSAPI_DECLARATION int BTPSAPI SDP_Set_Disconnect_Mode(unsigned int BluetoothStackID, SDP_Disconnect_Mode_t SDPDisconnectMode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SDP_Set_Disconnect_Mode_t)(unsigned int BluetoothStackID, SDP_Disconnect_Mode_t SDPDisconnectMode);
#endif

   /*! @brief The following function is responsible for Disconnecting a Remote
    * SDP Server Connection that is still open.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param BD_ADDR Bluetooth device address of the Remote SDP
    * Server for which the local device is currently connected
    * @return This function returns zero if successful, or a
    * negative return error code if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI SDP_Disconnect_Server(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SDP_Disconnect_Server_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR);
#endif

   /*! @brief The following function is responsible for allowing a mechanism to
    * query the SDP Server Connection Mode.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param SDPServerConnectionMode Pointer to return the current SDP server
    * connection mode into.  If this function returns success this will point
    * to one of the following values:
    * sdAutomaticAccept
    * sdManualAccept
    * @return This function returns zero if successful, or a negative return
    * value if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI SDP_Get_Server_Connection_Mode(unsigned int BluetoothStackID, SDP_Server_Connection_Mode_t *SDPServerConnectionMode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SDP_Get_Server_Connection_Mode_t)(unsigned int BluetoothStackID, SDP_Server_Connection_Mode_t *SDPServerConnectionMode);
#endif

   /*! @brief The following function is responsible for allowing a mechanism to
    * change the current SDP Server Connection Mode.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param SDPServerConnectionMode The server connection mode to set.
    * This may be one of the following values:
    * sdAutomaticAccept
    * sdManualAccept
    * @param SDPConnectionEventCallback A connection event callback
    * function that can be specified to watch SDP connection events.
    * @param CallbackParameter Callback parameter that will be passed
    * to the connection event callback specified in the SDPConnectionEventCallback
    * parameter whenever a SDP connection event occurs.
    * @return This function returns zero if
    * successful, or a negative return error code if an error occurred.
    * \note The Default Server Connection Mode is sdAutomaticAccept.
    * \note If the Server Mode is set to sdManualAccept then the
    *          SDPConnectionEventCallback parameter must be valid.
    * \note If the Server Mode is set to sdAutomaticAccept AND a
    *          callback is specified then the caller can watch the
    *          connection state.
    */
BTPSAPI_DECLARATION int BTPSAPI SDP_Set_Server_Connection_Mode(unsigned int BluetoothStackID, SDP_Server_Connection_Mode_t SDPServerConnectionMode, SDP_Connection_Event_Callback_t SDPConnectionEventCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SDP_Set_Server_Connection_Mode_t)(unsigned int BluetoothStackID, SDP_Server_Connection_Mode_t SDPServerConnectionMode, SDP_Connection_Event_Callback_t SDPConnectionEventCallback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is responsible for accepting an individual
    * request to connect to the local SDP Server.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param BD_ADDR The BD_ADDR of the remote device whose SDP connection request
    * is being accepted.
    * @return This function returns zero if successful, or a negative
    * return error code if an error occurred.
    * \note The connection to the server is not established until a
    *          Connect Indication event has actually occurred.
    * \note This event will only be dispatched if the server mode was
    *          explicitly set to sdManualAccept via the
    *          SDP_Set_Server_Connection_Mode() function.
    */
BTPSAPI_DECLARATION int BTPSAPI SDP_Connect_Request_Response(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SDP_Connect_Request_Response_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR);
#endif

#endif
