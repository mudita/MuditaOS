/*
 * Copyright 2010 - 2014 Stonestreet One.
 * All Rights Reserved.
 * Author:  Damon Lange
 *** MODIFICATION HISTORY ****************************************************
 *
 *   mm/dd/yy  F. Lastname    Description of Modification
 *   --------  -----------    ------------------------------------------------
 *   12/12/10  D. Lange       Initial creation.
 ****************************************************************************
 */

/**
 *  @file GATTAPI.h
 *
 *  @brief Stonestreet One Bluetooth Stack Generic Attribute Profile
 *      (GATT) using the Attribute Protocol (ATT) Type Definitions,
 *      Constants, and Prototypes.
 *
 *  @code
 *  #include "SS1BTGAT.h"
 *  @endcode
 *  ============================================================================
 */
#ifndef __GATTAPIH__
#define __GATTAPIH__

#include "SS1BTPS.h"            /*! Bluetooth Stack API Prototypes/Constants. */

#include "GATTType.h"           /*! Bluetooth GATT Type Definitions/Constants. */
#include "ATTTypes.h"           /*! Bluetooth ATT Type Definitions/Constants. */
   /*! Error Return Codes.
    * Error Codes that are smaller than these (less than -1000) are
    * related to the Bluetooth Protocol Stack itself (see BTERRORS.H).
    */
#define BTGATT_ERROR_INVALID_PARAMETER                             (-1000)
#define BTGATT_ERROR_NOT_INITIALIZED                               (-1001)
#define BTGATT_ERROR_CONTEXT_ALREADY_EXISTS                        (-1002)
#define BTGATT_ERROR_INVALID_BLUETOOTH_STACK_ID                    (-1004)
#define BTGATT_ERROR_INSUFFICIENT_RESOURCES                        (-1005)
#define BTGATT_ERROR_INVALID_RESPONSE                              (-1006)
#define BTGATT_ERROR_INVALID_OPERATION                             (-1007)
#define BTGATT_ERROR_INVALID_HANDLE_VALUE                          (-1008)
#define BTGATT_ERROR_INVALID_CONNECTION_ID                         (-1009)
#define BTGATT_ERROR_OUTSTANDING_REQUEST_EXISTS                    (-1010)
#define BTGATT_ERROR_INVALID_SERVICE_TABLE_FORMAT                  (-1011)
#define BTGATT_ERROR_INVALID_TRANSACTION_ID                        (-1012)
#define BTGATT_ERROR_INSUFFICIENT_HANDLES                          (-1013)

   /*! The following constants represent the defined flag bit-mask values
    * that can be passed to the GATT_Initialize() function.
    */
#define GATT_INITIALIZATION_FLAGS_SUPPORT_LE                       0x00000001L
#define GATT_INITIALIZATION_FLAGS_SUPPORT_BR_EDR                   0x00000002L

   /*! The following bit masks define that allowable flags that may be
    * specified in the Attribute_Flags member of the
    * GATT_Service_Attribute_Entry_t structure.
    */
#define GATT_ATTRIBUTE_FLAGS_READABLE                              0x01
#define GATT_ATTRIBUTE_FLAGS_WRITABLE                              0x02
#define GATT_ATTRIBUTE_FLAGS_HIDDEN                                0x04
#define GATT_ATTRIBUTE_FLAGS_READABLE_WRITABLE                     (GATT_ATTRIBUTE_FLAGS_READABLE|GATT_ATTRIBUTE_FLAGS_WRITABLE)

   /*! The following bit-masks define the allowable flags that may be
    * specified in the Service_Flags parameter of the
    * GATT_Register_Service() function.
    */
#define GATT_SERVICE_FLAGS_LE_SERVICE                              0x01
#define GATT_SERVICE_FLAGS_BR_EDR_SERVICE                          0x02

   /*! The following constants represent the execute write confirmation
    * values that are possible in the GATT execute write confirmation
    * event data information.
    */
#define GATT_EXECUTE_WRITE_CONFIRMATION_STATUS_NO_ERROR            0x00
#define GATT_EXECUTE_WRITE_CONFIRMATION_STATUS_ERROR               0x01

   /*! The following constants represent the server confirmation status
    * values that are possible in the GATT server confirmation event
    * data information.
    */
#define GATT_CONFIRMATION_STATUS_SUCCESS                           0x00
#define GATT_CONFIRMATION_STATUS_TIMEOUT                           0x01

   /*! The following constants represent the connection confirmation
    * status values that are possible in the GATT connect confirmation
    * event data information event (BR/EDR only).
    */
#define GATT_CONNECTION_CONFIRMATION_STATUS_SUCCESS                0x00
#define GATT_CONNECTION_CONFIRMATION_STATUS_CONNECTION_TIMEOUT     0x01
#define GATT_CONNECTION_CONFIRMATION_STATUS_CONNECTION_REFUSED     0x02
#define GATT_CONNECTION_CONFIRMATION_STATUS_UNKNOWN_ERROR          0x03

   /*! The following defines the maximum supported GATT MTU.
    */
#define GATT_MAXIMUM_SUPPORTED_STACK_MTU                          (BTPS_CONFIGURATION_GATT_MAXIMUM_SUPPORTED_MTU_SIZE)

   /*! The following enumerated type represents the supported BR/EDR
    * incoming connection modes supported by GATT.
    * \note These values are only applicable to BR/EDR connections.
    */
typedef enum
{
   gimAutomaticAccept,
   gimAutomaticReject,
   gimManualAccept
} GATT_Incoming_Connection_Mode_t;

   /*! The following enumerated type represents the different connection
    * types that are supported by GATT.
    */
typedef enum
{
   gctLE,
   gctBR_EDR
} GATT_Connection_Type_t;

   /*! The following enumerated type represents the different types of
    * service definitions that GATT supports.
    */
typedef enum
{
   stSecondary,
   stPrimary
} GATT_Service_Type_t;

   /*! The following enumerated type represents the different types of
    * request errors.
    */
typedef enum
{
   retErrorResponse,
   retProtocolTimeout,
   retPrepareWriteDataMismatch
} GATT_Request_Error_Type_t;

   /*! The following enumerated type represents the different UUID types
    * that are allowable as attribute types and attribute values.
    */
typedef enum
{
   guUUID_16,
   guUUID_128,
   guUUID_32
} GATT_UUID_Type_t;

   /*! The following data type represents a container that can hold both
    * 16 and 128 bit GATT UUIDs.
    */
typedef struct _tagGATT_UUID_t
{
   GATT_UUID_Type_t UUID_Type;
   union
   {
      UUID_16_t  UUID_16;
      UUID_32_t  UUID_32;
      UUID_128_t UUID_128;
   } UUID;
} GATT_UUID_t;

#define GATT_UUID_DATA_SIZE                              (sizeof(GATT_UUID_t))

   /*! The following enumerated type represents the different types of
    * attributes that may be placed in a GATT_Service_Attribute_Entry_t
    * structure.
    */
typedef enum
{
   aetPrimaryService16,
   aetPrimaryService128,
   aetSecondaryService16,
   aetSecondaryService128,
   aetIncludeDefinition,
   aetCharacteristicDeclaration16,
   aetCharacteristicDeclaration128,
   aetCharacteristicValue16,
   aetCharacteristicValue128,
   aetCharacteristicDescriptor16,
   aetCharacteristicDescriptor128,
   aetPrimaryService32,
   aetSecondaryService32,
   aetCharacteristicDeclaration32,
   aetCharacteristicValue32,
   aetCharacteristicDescriptor32
} GATT_Service_Attribute_Entry_Type_t;

   /*! The following structure represents the structure of the data that
    * must be specified for a GATT_Service_Attribute_Entry_t of type
    * aetPrimaryService16.
    */
typedef struct _tagGATT_Primary_Service_16_Entry_t
{
   UUID_16_t Service_UUID;
} GATT_Primary_Service_16_Entry_t;

#define GATT_PRIMARY_SERVICE_16_ENTRY_DATA_SIZE          (sizeof(GATT_Primary_Service_16_Entry_t))

   /*! The following structure represents the structure of the data that
    * must be specified for a GATT_Service_Attribute_Entry_t of type
    * aetPrimaryService32.
    */
typedef struct _tagGATT_Primary_Service_32_Entry_t
{
   UUID_32_t Service_UUID;
} GATT_Primary_Service_32_Entry_t;

#define GATT_PRIMARY_SERVICE_32_ENTRY_DATA_SIZE          (sizeof(GATT_Primary_Service_32_Entry_t))

   /*! The following structure represents the structure of the data that
    * must be specified for a GATT_Service_Attribute_Entry_t of type
    * aetPrimaryService128.
    */
typedef struct _tagGATT_Primary_Service_128_Entry_t
{
   UUID_128_t Service_UUID;
} GATT_Primary_Service_128_Entry_t;

#define GATT_PRIMARY_SERVICE_128_ENTRY_DATA_SIZE         (sizeof(GATT_Primary_Service_128_Entry_t))

   /*! The following structure represents the structure of the data that
    * must be specified for a GATT_Service_Attribute_Entry_t of type
    * aetSecondaryService16.
    */
typedef struct _tagGATT_Secondary_Service_16_Entry_t
{
   UUID_16_t Service_UUID;
} GATT_Secondary_Service_16_Entry_t;

#define GATT_SECONDARY_SERVICE_16_ENTRY_DATA_SIZE        (sizeof(GATT_Secondary_Service_16_Entry_t))

   /*! The following structure represents the structure of the data that
    * must be specified for a GATT_Service_Attribute_Entry_t of type
    * aetSecondaryService32.
    */
typedef struct _tagGATT_Secondary_Service_32_Entry_t
{
   UUID_32_t Service_UUID;
} GATT_Secondary_Service_32_Entry_t;

#define GATT_SECONDARY_SERVICE_32_ENTRY_DATA_SIZE        (sizeof(GATT_Secondary_Service_32_Entry_t))

   /*! The following structure represents the structure of the data that
    * must be specified for a GATT_Service_Attribute_Entry_t of type
    * aetSecondaryService128.
    */
typedef struct _tagGATT_Secondary_Service_128_Entry_t
{
   UUID_128_t Service_UUID;
} GATT_Secondary_Service_128_Entry_t;

#define GATT_SECONDARY_SERVICE_128_ENTRY_DATA_SIZE       (sizeof(GATT_Secondary_Service_128_Entry_t))

   /*! The following structure represents the structure of the data that
    * must be specified for a GATT_Service_Attribute_Entry_t of type
    * aetIncludeDefinition.  The ServiceID must be the return value from
    * the GATT_Register_Service() function (used to register the
    * included service).
    */
typedef struct _tagGATT_Include_Definition_Entry_t
{
   unsigned int ServiceID;
} GATT_Include_Definition_Entry_t;

#define GATT_INCLUDE_DEFINITION_ENTRY_DATA_SIZE          (sizeof(GATT_Include_Definition_Entry_t))

   /*! The following structure represents the structure of the data that
    * must be specified for a GATT_Service_Attribute_Entry_t of type
    * aetCharacteristicDeclaration16.
    */
typedef struct _tagGATT_Characteristic_Declaration_16_Entry_t
{
   Byte_t    Properties;
   UUID_16_t Characteristic_Value_UUID;
} GATT_Characteristic_Declaration_16_Entry_t;

#define GATT_CHARACTERISTIC_DECLARATION_16_ENTRY_DATA_SIZE (sizeof(GATT_Characteristic_Declaration_16_Entry_t))

   /*! The following structure represents the structure of the data that
    * must be specified for a GATT_Service_Attribute_Entry_t of type
    * aetCharacteristicDeclaration32.
    */
typedef struct _tagGATT_Characteristic_Declaration_32_Entry_t
{
   Byte_t    Properties;
   UUID_32_t Characteristic_Value_UUID;
} GATT_Characteristic_Declaration_32_Entry_t;

#define GATT_CHARACTERISTIC_DECLARATION_32_ENTRY_DATA_SIZE (sizeof(GATT_Characteristic_Declaration_32_Entry_t))

   /*! The following structure represents the structure of the data that
    * must be specified for a GATT_Service_Attribute_Entry_t of type
    * aetCharacteristicDeclaration128.
    */
typedef struct _tagGATT_Characteristic_Declaration_128_Entry_t
{
   Byte_t     Properties;
   UUID_128_t Characteristic_Value_UUID;
} GATT_Characteristic_Declaration_128_Entry_t;

#define GATT_CHARACTERISTIC_DECLARATION_128_ENTRY_DATA_SIZE (sizeof(GATT_Characteristic_Declaration_128_Entry_t))

   /*! The following structure represents the structure of the data that
    * must be specified for a GATT_Service_Attribute_Entry_t of type
    * aetCharacteristicValue16.
    */
typedef struct _tagGATT_Characteristic_Value_16_Entry_t
{
   UUID_16_t     Characteristic_Value_UUID;
   unsigned int  Characteristic_Value_Length;
   Byte_t       *Characteristic_Value;
} GATT_Characteristic_Value_16_Entry_t;

#define GATT_CHARACTERISTIC_VALUE_16_ENTRY_DATA_SIZE     (sizeof(GATT_Characteristic_Value_16_Entry_t))

   /*! The following structure represents the structure of the data that
    * must be specified for a GATT_Service_Attribute_Entry_t of type
    * aetCharacteristicValue32.
    */
typedef struct _tagGATT_Characteristic_Value_32_Entry_t
{
   UUID_32_t     Characteristic_Value_UUID;
   unsigned int  Characteristic_Value_Length;
   Byte_t       *Characteristic_Value;
} GATT_Characteristic_Value_32_Entry_t;

#define GATT_CHARACTERISTIC_VALUE_32_ENTRY_DATA_SIZE     (sizeof(GATT_Characteristic_Value_32_Entry_t))

   /*! The following structure represents the structure of the data that
    * must be specified for a GATT_Service_Attribute_Entry_t of type
    * aetCharacteristicValue128.
    */
typedef struct _tagGATT_Characteristic_Value_128_Entry_t
{
   UUID_128_t    Characteristic_Value_UUID;
   unsigned int  Characteristic_Value_Length;
   Byte_t       *Characteristic_Value;
} GATT_Characteristic_Value_128_Entry_t;

#define GATT_CHARACTERISTIC_VALUE_128_ENTRY_DATA_SIZE    (sizeof(GATT_Characteristic_Value_128_Entry_t))

   /*! The following structure represents the structure of the data that
    * must be specified for a GATT_Service_Attribute_Entry_t of type
    * aetCharacteristicDescriptor16.
    */
typedef struct _tagGATT_Characteristic_Descriptor_16_Entry_t
{
   UUID_16_t     Characteristic_Descriptor_UUID;
   unsigned int  Characteristic_Descriptor_Length;
   Byte_t       *Characteristic_Descriptor;
} GATT_Characteristic_Descriptor_16_Entry_t;

#define GATT_CHARACTERISTIC_DESCRIPTOR_16_ENTRY_DATA_SIZE (sizeof(GATT_Characteristic_Descriptor_16_Entry_t))

   /*! The following structure represents the structure of the data that
    * must be specified for a GATT_Service_Attribute_Entry_t of type
    * aetCharacteristicDescriptor32.
    */
typedef struct _tagGATT_Characteristic_Descriptor_32_Entry_t
{
   UUID_32_t     Characteristic_Descriptor_UUID;
   unsigned int  Characteristic_Descriptor_Length;
   Byte_t       *Characteristic_Descriptor;
} GATT_Characteristic_Descriptor_32_Entry_t;

#define GATT_CHARACTERISTIC_DESCRIPTOR_32_ENTRY_DATA_SIZE (sizeof(GATT_Characteristic_Descriptor_32_Entry_t))

   /*! The following structure represents the structure of the data that
    * must be specified for a GATT_Service_Attribute_Entry_t of type
    * aetCharacteristicDescriptor128.
    */
typedef struct _tagGATT_Characteristic_Descriptor_128_Entry_t
{
   UUID_128_t    Characteristic_Descriptor_UUID;
   unsigned int  Characteristic_Descriptor_Length;
   Byte_t       *Characteristic_Descriptor;
} GATT_Characteristic_Descriptor_128_Entry_t;

#define GATT_CHARACTERISTIC_DESCRIPTOR_128_ENTRY_DATA_SIZE (sizeof(GATT_Characteristic_Descriptor_128_Entry_t))

   /*! The following structure defines the format of an attribute entry
    * for a GATT Service.  The Attribute_Entry_Type describes the type
    * of attribute.  The Attribute_Value contains the attribute value,
    * which must point to the correct entry structure corresponding to
    * the specified Attribute_Entry_Type type.
    * \note By specifying GATT_ATTRIBUTE_FLAGS_HIDDEN in the
    *          Attribute_Flags member, GATT will allocate a handle for
    *          the attribute, but the attribute will not appear on any
    *          discovery requests, AND all attempts to read or write to
    *          the attribute will fail.
    * \note Since the GATT_Register_Service() function takes a
    *          pointer to an array of these structures, the
    *          Attribute_Flags member may be modified after a sucessful
    *          call to the GATT_Register_Service() function, however
    *          locking should be performed to prevent modifying it while
    *          GATT is reading it.  Refer to BSCAPI.h and the
    *          BSC_LockBluetoothStack() and BSC_UnLockBluetoothStack()
    *          functions for the API to perform this locking.
    * \note The Attribute_Value member *MUST* point to the correct
    *          datatype based on the correct entry type.  The
    *          corresponding attribute value for each supported
    *          attribute entry type is as follows:
    *
    *             - aetPrimaryService16
    *                  GATT_Primary_Service_16_Entry_t
    *             - aetPrimaryService32
    *                  GATT_Primary_Service_32_Entry_t
    *             - aetPrimaryService128
    *                  GATT_Primary_Service_128_Entry_t
    *             - aetSecondaryService16
    *                  GATT_Secondary_Service_16_Entry_t
    *             - aetSecondaryService32
    *                  GATT_Secondary_Service_32_Entry_t
    *             - aetSecondaryService128
    *                  GATT_Secondary_Service_128_Entry_t
    *             - aetIncludeDefinition
    *                  GATT_Include_Definition_Entry_t
    *             - aetCharacteristicDeclaration16
    *                  GATT_Characteristic_Declaration_16_Entry_t
    *             - aetCharacteristicDeclaration32
    *                  GATT_Characteristic_Declaration_32_Entry_t
    *             - aetCharacteristicDeclaration128
    *                  GATT_Characteristic_Declaration_128_Entry_t
    *             - aetCharacteristicValue16
    *                  GATT_Characteristic_Value_16_Entry_t
    *             - aetCharacteristicValue32
    *                  GATT_Characteristic_Value_32_Entry_t
    *             - aetCharacteristicValue128
    *                  GATT_Characteristic_Value_128_Entry_t
    *             - aetCharacteristicDescriptor16
    *                  GATT_Characteristic_Descriptor_16_Entry_t
    *             - aetCharacteristicDescriptor32
    *                  GATT_Characteristic_Descriptor_32_Entry_t
    *             - aetCharacteristicDescriptor128
    *                  GATT_Characteristic_Descriptor_128_Entry_t
    */
typedef struct _tagGATT_Service_Attribute_Entry_t
{
   Byte_t                               Attribute_Flags;
   GATT_Service_Attribute_Entry_Type_t  Attribute_Entry_Type;
   void                                *Attribute_Value;
} GATT_Service_Attribute_Entry_t;

#define GATT_SERVICE_ATTRIBUTE_ENTRY_DATA_SIZE           (sizeof(GATT_Service_Attribute_Entry_t))

   /*! The following data type represents the structure of a attribute
    * Handle/End group handle list.
    */
typedef struct _tagGATT_Attribute_Handle_Group_t
{
   Word_t Starting_Handle;
   Word_t Ending_Handle;
} GATT_Attribute_Handle_Group_t;

#define GATT_ATTRIBUTE_HANDLE_GROUP_DATA_SIZE            (sizeof(GATT_Attribute_Handle_Group_t))

   /*! The following data type represents the structure of information
    * that is returned about a GATT Service from a GATT service
    * discovery procedure.
    */
typedef struct _tagGATT_Service_Information_t
{
   Word_t      Service_Handle;
   Word_t      End_Group_Handle;
   GATT_UUID_t UUID;
} GATT_Service_Information_t;

#define GATT_SERVICE_INFORMATION_DATA_SIZE               (sizeof(GATT_Service_Information_t))

   /*! The following data type represents the structure of information
    * that is returned about a GATT service from a GATT service
    * discovery procedure.
    */
typedef struct _tagGATT_Service_Information_By_UUID_t
{
   Word_t Service_Handle;
   Word_t End_Group_Handle;
} GATT_Service_Information_By_UUID_t;

#define GATT_SERVICE_INFORMATION_BY_UUID_DATA_SIZE       (sizeof(GATT_Service_Information_By_UUID_t))

   /* The following data type represents the structure of information
    * that is returned about a GATT included service declaration from
    * a GATT find included services procedure.
    * \note The UUID member is only valid for a included service if
    *          the UUID_Valid member is TRUE. Otherwise it is invalid
    *          and should be ignored.
    */
typedef struct _tagGATT_Include_Information_t
{
   Word_t      Include_Attribute_Handle;
   Word_t      Included_Service_Handle;
   Word_t      Included_Service_End_Group_Handle;
   Boolean_t   UUID_Valid;
   GATT_UUID_t UUID;
} GATT_Include_Information_t;

#define GATT_INCLUDE_INFORMATION_DATA_SIZE               (sizeof(GATT_Include_Information_t))

   /*! For the LE Only GATT build we will define this data type to be a
    * void pointer since SDPAPI.h is not included for a Single Mode
    * Stack.
    */
#ifndef __SDPAPIH__

typedef void * SDP_UUID_Entry_t;

#endif

   /*! The following structure is used with the
    * GATT_Register_SDP_Record() function.  This structure (when
    * specified) contains additional SDP Service Information that will
    * be added to the SDP GATT Service Record Entry.  The first member
    * of this strucuture specifies the Number of Service Class UUID's
    * that are present in the SDPUUIDEntries Array.  This member must be
    * at least one, and the SDPUUIDEntries member must point to an array
    * of SDP UUID Entries that contains (at least) as many entries
    * specified by the NumberServiceClassUUID member.
    */
typedef struct _tagGATT_SDP_Service_Record_t
{
   unsigned int        NumberServiceClassUUID;
   SDP_UUID_Entry_t   *SDPUUIDEntries;
} GATT_SDP_Service_Record_t;

#define GATT_SDP_SERVICE_RECORD_SIZE                    (sizeof(GATT_SDP_Service_Record_t))

   /*! The following structure represents the structure of the Service
    * Changed characteristic value.  The Affected_Start_Handle contains
    * the first attribute in the GATT Database that has been affected by
    * the addition and/or the removal of a GATT Service.  The
    * Affected_End_Handle contains the last attribute in the GATT
    * Database that has been affected by the addition and/or the removal
    * of a GATT Service.
    */
typedef struct _tagGATT_Service_Changed_Data_t
{
   Word_t Affected_Start_Handle;
   Word_t Affected_End_Handle;
} GATT_Service_Changed_Data_t;

#define GATT_SERVICE_CHANGED_DATA_SIZE                   (sizeof(GATT_Service_Changed_Data_t))

   /*! GATT Service Discovery types.
    * The following structure defines the information that is returned
    * about a discovered GATT Characteristic Descriptor.
    */
typedef struct _tagGATT_Characteristic_Descriptor_Information_t
{
   Word_t      Characteristic_Descriptor_Handle;
   GATT_UUID_t Characteristic_Descriptor_UUID;
} GATT_Characteristic_Descriptor_Information_t;

#define GATT_CHARACTERISTIC_DESCRIPTOR_INFORMATION_DATA_SIZE (sizeof(GATT_Characteristic_Descriptor_Information_t))

   /*! The following structure defines the information that is returned
    * about a discovered GATT Characteristic.
    */
typedef struct _tagGATT_Characteristic_Information_t
{
   GATT_UUID_t                                   Characteristic_UUID;
   Word_t                                        Characteristic_Handle;
   Byte_t                                        Characteristic_Properties;
   unsigned int                                  NumberOfDescriptors;
   GATT_Characteristic_Descriptor_Information_t *DescriptorList;
} GATT_Characteristic_Information_t;

#define GATT_CHARACTERISTIC_INFORMATION_DATA_SIZE            (sizeof(GATT_Characteristic_Information_t))

   /*! GATT Event API Types.
    * These events are issued to the application via callbacks
    * registered to receive connection events.
    */
typedef enum
{
   etGATT_Connection_Device_Connection_Request, /*!< Dispatched when a remote BR/EDR device is attempting a GATT connection to the local GATT server. */
   etGATT_Connection_Device_Connection, /*!< Dispatched when a remote Bluetooth device is connected to the GATT profile. */
   etGATT_Connection_Device_Connection_Confirmation, /*!< Dispatched when an out-going (client) BR/EDR connection is complete. */
   etGATT_Connection_Device_Disconnection, /*!< Dispatched when a remote Bluetooth device is disconnected from the GATT profile. */
   etGATT_Connection_Device_Connection_MTU_Update, /*!< Dispatched when the MTU for a remote Bluetooth LE device is changed. */
   etGATT_Connection_Server_Indication, /*!< Dispatched when a remote Bluetooth device sends a GATT server indication to the local GATT profile. */
   etGATT_Connection_Server_Notification, /*!< Dispatched when a remote Bluetooth device sends a GATT server notification to the local GATT profile. */
   etGATT_Connection_Service_Database_Update, /*!< Dispatched when the GATT database has been changed due to the addition/removal of a GATT Service. */
   etGATT_Connection_Service_Changed_Read_Request, /*!< Dispatched when a remote Bluetooth device attempts to read it’s Service Changed value. */
   etGATT_Connection_Service_Changed_Confirmation, /*!< Dispatched when a confirmation is received from a GATT Service Changed indication that was sent by the local Bluetooth Device. */
   etGATT_Connection_Device_Buffer_Empty, /*!< Dispatched when the buffer for the specified device connection becomes empty. */
   etGATT_Connection_Service_Changed_CCCD_Read_Request, /*!< Dispatched when a Client attempts to read its unique Client Characteristic Configuration Descriptor (CCCD) value for the Service Changed characteristic. */
   etGATT_Connection_Service_Changed_CCCD_Update /*!< Dispatched when a Client updates its unique CCCD value for the Service Changed characteristic. */
} GATT_Connection_Event_Type_t;

   /*! These events are issued to the application via the callback
    * registered when the application registers a local GATT service on
    * the local device.
    */
typedef enum
{
   etGATT_Server_Device_Connection, /*!< Dispatched when a remote Bluetooth device is connected to the GATT profile. */
   etGATT_Server_Device_Disconnection, /*!< Dispatched when a remote Bluetooth device is disconnected from the GATT profile. */
   etGATT_Server_Read_Request, /*!< Dispatched when a read value request is received by the GATT server from a connected Bluetooth device. */
   etGATT_Server_Write_Request, /*!< Dispatched when a write value request is received by the GATT server from a connected Bluetooth device. */
   etGATT_Server_Signed_Write_Request, /*!< Dispatched when a signed write value request is received by the GATT server from a connected Bluetooth device. */
   etGATT_Server_Execute_Write_Request, /*!< Dispatched when an execute write request is received by the GATT server from a connected Bluetooth device. */
   etGATT_Server_Execute_Write_Confirmation, /*!< Dispatched to inform services when it is safe to commit prepared writes that have been processed. */
   etGATT_Server_Confirmation_Response, /*!< Dispatched when an indication acknowledgment is received from a connected Bluetooth device. */
   etGATT_Server_Device_Connection_MTU_Update, /*!< Dispatched when the MTU for a remote Bluetooth LE device is changed. */
   etGATT_Server_Device_Buffer_Empty /*!< Dispatched when the buffer for the specified device connection becomes empty. */
} GATT_Server_Event_Type_t;

   /*! The following enumerated type represents all the allowable GATT
    * Client Event Data Types that will be returned in the GATT Client
    * Callback Function.
    */
typedef enum
{
   etGATT_Client_Error_Response, /*!< Dispatched when an error response is received, in response to a request, from a connected Bluetooth device. */
   etGATT_Client_Service_Discovery_Response, /*!< Dispatched when a service discovery response is received from a connected Bluetooth device. */
   etGATT_Client_Service_Discovery_By_UUID_Response, /*!< Dispatched when a service discovery by UUID response is received from a connected Bluetooth device. */
   etGATT_Client_Included_Services_Discovery_Response, /*!< Dispatched when an included services discovery response is received from a connected Bluetooth device. */
   etGATT_Client_Characteristic_Discovery_Response, /*!< Dispatched when a characteristic discovery response is received from a connected Bluetooth device. */
   etGATT_Client_Characteristic_Descriptor_Discovery_Response, /*!< Dispatched when a characteristic descriptor discovery response is received from a connected Bluetooth device. */
   etGATT_Client_Read_Response, /*!< Dispatched when a read response is received from a connected Bluetooth device. */
   etGATT_Client_Read_Long_Response, /*!< Dispatched when a read long response is received from a connected Bluetooth device. */
   etGATT_Client_Read_By_UUID_Response, /*!< Dispatched when a read by UUID response is received from a connected Bluetooth device. */
   etGATT_Client_Read_Multiple_Response, /*!< Dispatched when a read multiple response is received from a connected Bluetooth device. */
   etGATT_Client_Write_Response, /*!< Dispatched when a write response is received from a connected Bluetooth device. */
   etGATT_Client_Prepare_Write_Response, /*!< Dispatched when an prepare write response is received from a connected Bluetooth device. */
   etGATT_Client_Execute_Write_Response, /*!< Dispatched when an execute write response is received from a connected Bluetooth device. */
   etGATT_Client_Exchange_MTU_Response /*!< Dispatched when an exchange MTU response is received from a connected Bluetooth LE device. */
} GATT_Client_Event_Type_t;

   /*! The following enumerated type represents all the allowable GATT
    * Service Discovery Event Data Types that will be returned in the
    * GATT Service Discovery Callback Function.
    */
typedef enum
{
   etGATT_Service_Discovery_Indication, /*!< Dispatched when a service on the remote device has been discovered successfully. */
   etGATT_Service_Discovery_Complete /*!< Dispatched when a service discovery operation has completed. */
} GATT_Service_Discovery_Event_Type_t;

   /* GATT Connection Events.
    * The following event is dispatched when a remote device is
    * attempting to connect with the local GATT instance and the
    * connection mode is set to manual (BR/EDR ONLY).
    */
typedef struct _tagGATT_Device_Connection_Request_Data_t
{
   GATT_Connection_Type_t ConnectionType;
   BD_ADDR_t              RemoteDevice;
} GATT_Device_Connection_Request_Data_t;

#define GATT_DEVICE_CONNECTION_REQUEST_DATA_SIZE         (sizeof(GATT_Device_Connection_Request_Data_t))

   /*! The following event is dispatched when a remote device is
    * connected to the local GATT instance.
    * \note This event contains the Connection ID that is used to
    *          uniquely identify the connection.
    */
typedef struct _tagGATT_Device_Connection_Data_t
{
   unsigned int           ConnectionID;
   GATT_Connection_Type_t ConnectionType;
   BD_ADDR_t              RemoteDevice;
   Word_t                 MTU;
} GATT_Device_Connection_Data_t;

#define GATT_DEVICE_CONNECTION_DATA_SIZE                 (sizeof(GATT_Device_Connection_Data_t))

   /*! The following event is dispatched to signify the result of a
    * remote device connection status (that was originated via the the
    * GATT_Connect_Device() function).
    */
typedef struct _tagGATT_Device_Connection_Confirmation_Data_t
{
   unsigned int           ConnectionID;
   unsigned int           ConnectionStatus;
   GATT_Connection_Type_t ConnectionType;
   BD_ADDR_t              RemoteDevice;
   Word_t                 MTU;
} GATT_Device_Connection_Confirmation_Data_t;

#define GATT_DEVICE_CONNECTION_CONFIRMATION_DATA_SIZE    (sizeof(GATT_Device_Connection_Confirmation_Data_t))

   /*! The following event is dispatched when a remote device disconnects
    * from the local GATT instance.
    */
typedef struct _tagGATT_Device_Disconnection_Data_t
{
   unsigned int           ConnectionID;
   GATT_Connection_Type_t ConnectionType;
   BD_ADDR_t              RemoteDevice;
} GATT_Device_Disconnection_Data_t;

#define GATT_DEVICE_DISCONNECTION_DATA_SIZE              (sizeof(GATT_Device_Disconnection_Data_t))

   /*! The following event is dispatched when a the buffer for the
    * specified Device becames empty.
    */
typedef struct _tagGATT_Device_Buffer_Empty_Data_t
{
   unsigned int           ConnectionID;
   GATT_Connection_Type_t ConnectionType;
   BD_ADDR_t              RemoteDevice;
} GATT_Device_Buffer_Empty_Data_t;

#define GATT_DEVICE_BUFFER_EMPTY_DATA_SIZE               (sizeof(GATT_Device_Buffer_Empty_Data_t))

   /*! The following data structure represents the data that is returned
    * in a Handle/Value notification event.
    */
typedef struct _tagGATT_Server_Notification_Data_t
{
   unsigned int            ConnectionID;
   GATT_Connection_Type_t  ConnectionType;
   BD_ADDR_t               RemoteDevice;
   Word_t                  AttributeHandle;
   Word_t                  AttributeValueLength;
   Byte_t                 *AttributeValue;
} GATT_Server_Notification_Data_t;

   /*! The following MACRO is a utility MACRO that exists to aid in
    * determining the amount of memory that is required to store
    * a GATT notification event. The first parameter to this MACRO is
    * the number of bytes in the AttributeValue field.
    */
#define GATT_SERVER_NOTIFICATION_DATA_SIZE               (sizeof(GATT_Server_Notification_Data_t))

   /*! The following data structure represents the data that is returned
    * in a Handle/Value indication event.
    */
typedef struct _tagGATT_Server_Indication_Data_t
{
   unsigned int            ConnectionID;
   unsigned int            TransactionID;
   GATT_Connection_Type_t  ConnectionType;
   BD_ADDR_t               RemoteDevice;
   Word_t                  AttributeHandle;
   Word_t                  AttributeValueLength;
   Byte_t                 *AttributeValue;
} GATT_Server_Indication_Data_t;

   /*! The following MACRO is a utility MACRO that exists to aid in
    * determining the amount of memory that is required to store
    * a GATT indication event. The first parameter to this MACRO is
    * the number of bytes in the AttributeValue field.
    */
#define GATT_SERVER_INDICATION_DATA_SIZE                 (sizeof(GATT_Server_Indication_Data_t))

   /*! The following event is dispatched when the MTU for a remote LE
    * device is changed.
    * \note This event contains the Connection ID that is used to
    *          uniquely identify the connection.
    */
typedef struct _tagGATT_Device_Connection_MTU_Update_Data_t
{
   unsigned int           ConnectionID;
   GATT_Connection_Type_t ConnectionType;
   BD_ADDR_t              RemoteDevice;
   Word_t                 MTU;
} GATT_Device_Connection_MTU_Update_Data_t;

#define GATT_DEVICE_CONNECTION_MTU_UPDATE_DATA_SIZE      (sizeof(GATT_Device_Connection_MTU_Update_Data_t))

   /*! The following event is dispatched when the GATT Database has
    * changed due to the addition/removal of a GATT Service.
    * \note The ServiceAdded member is TRUE if the GATT Database has
    *          changed due to the addition of a GATT Service.  It will
    *          be FALSE otherwise.
    * \note The ServiceChangedData contains the handle range of the
    *          region of the GATT Database that has been affected.
    */
typedef struct _tagGATT_Connection_Service_Database_Update_Data_t
{
   Boolean_t                   ServiceAdded;
   GATT_Service_Changed_Data_t ServiceChangedData;
} GATT_Connection_Service_Database_Update_Data_t;

#define GATT_CONNECTION_SERVICE_DATABASE_UPDATE_DATA_SIZE (sizeof(GATT_Connection_Service_Database_Update_Data_t))

   /*! The following event is dispatched when a remote GATT client is
    * attempting to read the GATT Service Changed characteristic value.
    * \note It is the responsibility of the application that receives
    *          this event to respond with the correct data for each
    *          client.
    */
typedef struct _tagGATT_Connection_Service_Changed_Read_Data_t
{
   unsigned int           ConnectionID;
   unsigned int           TransactionID;
   GATT_Connection_Type_t ConnectionType;
   BD_ADDR_t              RemoteDevice;
} GATT_Connection_Service_Changed_Read_Data_t;

#define GATT_CONNECTION_SERVICE_CHANGED_READ_DATA_SIZE   (sizeof(GATT_Connection_Service_Changed_Read_Data_t))

   /*! The following event is dispatched when a Confirmation is received
    * from a Service Changed Indication that was previously sent out.
    * \note If the Status is not equal to
    *          GATT_CONFIRMATION_STATUS_SUCCESS then the Service Changed
    *          Indication should be considered to have failed.
    */
typedef struct _tagGATT_Connection_Service_Changed_Confirmation_Data_t
{
   unsigned int           ConnectionID;
   unsigned int           TransactionID;
   GATT_Connection_Type_t ConnectionType;
   BD_ADDR_t              RemoteDevice;
   Byte_t                 Status;
} GATT_Connection_Service_Changed_Confirmation_Data_t;

#define GATT_CONNECTION_SERVICE_CHANGED_CONFIRMATION_DATA_SIZE (sizeof(GATT_Connection_Service_Changed_Confirmation_Data_t))

   /*! The following event is dispatched when a remote GATT client is
    * attempting to read it's GATT Service Changed Client Characteristic
    * Configuration Descriptor value.
    * \note It is the responsibility of the application that receives
    *          this event to respond with the correct data for each
    *          client.
    */
typedef struct _tagGATT_Connection_Service_Changed_CCCD_Read_Data_t
{
   unsigned int           ConnectionID;
   unsigned int           TransactionID;
   GATT_Connection_Type_t ConnectionType;
   BD_ADDR_t              RemoteDevice;
} GATT_Connection_Service_Changed_CCCD_Read_Data_t;

#define GATT_CONNECTION_SERVICE_CHANGED_CCCD_READ_DATA_SIZE    (sizeof(GATT_Connection_Service_Changed_CCCD_Read_Data_t))

   /*! The following event is dispatched when a remote GATT client is
    * attempting to update it's GATT Service Changed Client
    * Characteristic Configuration Descriptor value.
    * \note It is the responsibility of the application that receives
    *          this event to store this value persistently for bonded
    *          devices.
    */
typedef struct _tagGATT_Connection_Service_Changed_CCCD_Update_Data_t
{
   unsigned int           ConnectionID;
   GATT_Connection_Type_t ConnectionType;
   BD_ADDR_t              RemoteDevice;
   Word_t                 ClientConfigurationValue;
} GATT_Connection_Service_Changed_CCCD_Update_Data_t;

#define GATT_CONNECTION_SERVICE_CHANGED_CCCD_UPDATE_DATA_SIZE  (sizeof(GATT_Connection_Service_Changed_CCCD_Update_Data_t))

   /*! The following structure represents the container structure for
    * holding all GATT connection event data.
    */
typedef struct _tagGATT_Connection_Event_Data_t
{
   GATT_Connection_Event_Type_t Event_Data_Type;
   Word_t                       Event_Data_Size;
   union
   {
      GATT_Device_Connection_Request_Data_t               *GATT_Device_Connection_Request_Data;
      GATT_Device_Connection_Data_t                       *GATT_Device_Connection_Data;
      GATT_Device_Connection_Confirmation_Data_t          *GATT_Device_Connection_Confirmation_Data;
      GATT_Device_Disconnection_Data_t                    *GATT_Device_Disconnection_Data;
      GATT_Device_Buffer_Empty_Data_t                     *GATT_Device_Buffer_Empty_Data;
      GATT_Server_Notification_Data_t                     *GATT_Server_Notification_Data;
      GATT_Server_Indication_Data_t                       *GATT_Server_Indication_Data;
      GATT_Device_Connection_MTU_Update_Data_t            *GATT_Device_Connection_MTU_Update_Data;
      GATT_Connection_Service_Database_Update_Data_t      *GATT_Connection_Service_Database_Update_Data;
      GATT_Connection_Service_Changed_Read_Data_t         *GATT_Connection_Service_Changed_Read_Data;
      GATT_Connection_Service_Changed_Confirmation_Data_t *GATT_Connection_Service_Changed_Confirmation_Data;
      GATT_Connection_Service_Changed_CCCD_Read_Data_t    *GATT_Connection_Service_Changed_CCCD_Read_Data;
      GATT_Connection_Service_Changed_CCCD_Update_Data_t  *GATT_Connection_Service_Changed_CCCD_Update_Data;
   } Event_Data;
} GATT_Connection_Event_Data_t;

#define GATT_CONNECTION_EVENT_DATA_SIZE                  (sizeof(GATT_Connection_Event_Data_t))

   /*! GATT Server Events.
    * The following data structure represents the data that is returned
    * in a read request event.
    */
typedef struct _tagGATT_Read_Request_Data_t
{
   unsigned int           ConnectionID;
   unsigned int           TransactionID;
   GATT_Connection_Type_t ConnectionType;
   BD_ADDR_t              RemoteDevice;
   unsigned int           ServiceID;
   Word_t                 AttributeOffset;
   Word_t                 AttributeValueOffset;
} GATT_Read_Request_Data_t;

#define GATT_READ_REQUEST_DATA_SIZE                      (sizeof(GATT_Read_Request_Data_t))

   /*! The following data structure represents the data that is returned
    * in a write request event.
    */
typedef struct _tagGATT_Write_Request_Data_t
{
   unsigned int            ConnectionID;
   unsigned int            TransactionID;
   GATT_Connection_Type_t  ConnectionType;
   BD_ADDR_t               RemoteDevice;
   unsigned int            ServiceID;
   Word_t                  AttributeOffset;
   Word_t                  AttributeValueLength;
   Word_t                  AttributeValueOffset;
   Byte_t                 *AttributeValue;
   Boolean_t               DelayWrite;
} GATT_Write_Request_Data_t;

#define GATT_WRITE_REQUEST_DATA_SIZE                     (sizeof(GATT_Write_Request_Data_t))

   /*! The following data structure represents the data that is returned
    * in a signed write request Event.
    */
typedef struct _tagGATT_Signed_Write_Request_Data_t
{
   unsigned int                    ConnectionID;
   unsigned int                    TransactionID;
   GATT_Connection_Type_t          ConnectionType;
   BD_ADDR_t                       RemoteDevice;
   unsigned int                    ServiceID;
   Word_t                          AttributeOffset;
   Word_t                          AttributeValueLength;
   Byte_t                         *AttributeValue;
   ATT_Authentication_Signature_t  AuthenticationSignature;
} GATT_Signed_Write_Request_Data_t;

   /*! The following MACRO is a utility MACRO that exists to aid in
    * determining the amount of memory that is required to store a GATT
    * signed write request event. The first parameter to this MACRO is
    * the number of bytes in the AttributeValue member.
    */
#define GATT_SIGNED_WRITE_REQUEST_DATA_SIZE              (sizeof(GATT_Signed_Write_Request_Data_t))

   /*! The following data structure represents the data that is returned
    * in an execute write request event.  No queued writes for the
    * specified GATT client should be committed into the database UNTIL
    * the GATT execture write confirmation event is received with the
    * status set to GATT_EXECUTE_WRITE_CONFIRMATION_STATUS_NO_ERROR.
    * \note If CancelWrite is TRUE, then all pending writes received
    *          from the specified GATT client should be discarded.
    */
typedef struct _tagGATT_Execute_Write_Request_Data_t
{
   unsigned int           ConnectionID;
   unsigned int           TransactionID;
   GATT_Connection_Type_t ConnectionType;
   BD_ADDR_t              RemoteDevice;
   unsigned int           ServiceID;
   Boolean_t              CancelWrite;
} GATT_Execute_Write_Request_Data_t;

#define GATT_EXECUTE_WRITE_REQUEST_DATA_SIZE             (sizeof(GATT_Execute_Write_Request_Data_t))

   /*! The following data structure represents the data that is returned
    * in an execute write confirmation event.
    */
typedef struct _tagGATT_Execute_Write_Confirmation_Data_t
{
   unsigned int           ConnectionID;
   unsigned int           TransactionID;
   GATT_Connection_Type_t ConnectionType;
   BD_ADDR_t              RemoteDevice;
   unsigned int           ServiceID;
   Byte_t                 Status;
} GATT_Execute_Write_Confirmation_Data_t;

#define GATT_EXECUTE_WRITE_CONFIRMATION_DATA_SIZE        (sizeof(GATT_Execute_Write_Confirmation_Data_t))

   /*! The following data structure represents the data that is returned
    * in a Handle/Value confirmation event (that is sent in response to
    * a Handle/Value indication sent by the GATT Server).
    * \note If the Status is not equal to
    *          GATT_CONFIRMATION_STATUS_SUCCESS then the Handle/Value
    *          indication should be considered to have failed.
    * \note If the Status is equal to
    *          GATT_CONFIRMATION_STATUS_SUCCESS then the BytesWritten
    *          member will contain the number of data bytes that were
    *          actually indicated.
    */
typedef struct _tagGATT_Confirmation_Data_t
{
   unsigned int           ConnectionID;
   unsigned int           TransactionID;
   GATT_Connection_Type_t ConnectionType;
   BD_ADDR_t              RemoteDevice;
   Byte_t                 Status;
   Word_t                 BytesWritten;
} GATT_Confirmation_Data_t;

#define GATT_CONFIRMATION_DATA_SIZE                      (sizeof(GATT_Confirmation_Data_t))

   /*! The following structure represents the container structure for
    * holding all GATT server event data.
    */
typedef struct _tagGATT_Server_Event_Data_t
{
   GATT_Server_Event_Type_t Event_Data_Type;
   Word_t                   Event_Data_Size;
   union
   {
      GATT_Device_Connection_Data_t            *GATT_Device_Connection_Data;
      GATT_Device_Disconnection_Data_t         *GATT_Device_Disconnection_Data;
      GATT_Device_Buffer_Empty_Data_t          *GATT_Device_Buffer_Empty_Data;
      GATT_Device_Connection_MTU_Update_Data_t *GATT_Device_Connection_MTU_Update_Data;
      GATT_Read_Request_Data_t                 *GATT_Read_Request_Data;
      GATT_Write_Request_Data_t                *GATT_Write_Request_Data;
      GATT_Signed_Write_Request_Data_t         *GATT_Signed_Write_Request_Data;
      GATT_Execute_Write_Request_Data_t        *GATT_Execute_Write_Request_Data;
      GATT_Execute_Write_Confirmation_Data_t   *GATT_Execute_Write_Confirmation_Data;
      GATT_Confirmation_Data_t                 *GATT_Confirmation_Data;
   } Event_Data;
} GATT_Server_Event_Data_t;

#define GATT_SERVER_EVENT_DATA_SIZE                      (sizeof(GATT_Server_Event_Data_t))

   /*! GATT Client Event types.
    * The following data structure represents the data that is returned
    * for an error response event.
    * \note The RequestHandle and ErrorCode members are valid ONLY
    *          if the Error_Type member is retErrorResponse.
    */
typedef struct _tagGATT_Request_Error_Data_t
{
   unsigned int              ConnectionID;
   unsigned int              TransactionID;
   GATT_Connection_Type_t    ConnectionType;
   BD_ADDR_t                 RemoteDevice;
   GATT_Request_Error_Type_t ErrorType;
   Byte_t                    RequestOpCode;
   Word_t                    RequestHandle;
   Byte_t                    ErrorCode;
} GATT_Request_Error_Data_t;

#define GATT_REQUEST_ERROR_DATA_SIZE                     (sizeof(GATT_Request_Error_Data_t))

   /*! The following data type represents the type of data returned in a
    * service discovery event started by a call to the
    * GATT_Discover_Services() function.
    */
typedef struct _tagGATT_Service_Discovery_Response_Data_t
{
   unsigned int                ConnectionID;
   unsigned int                TransactionID;
   GATT_Connection_Type_t      ConnectionType;
   BD_ADDR_t                   RemoteDevice;
   Word_t                      NumberOfServices;
   GATT_Service_Information_t *ServiceInformationList;
} GATT_Service_Discovery_Response_Data_t;

   /*! The following MACRO is a utility MACRO that exists to calculate
    * the data size needed to hold the GATT_Service_Discovery_Response_t
    * structure along with the GATT_Service_Information_List list.  The
    * parameter to this MACRO specifies the number of services that are
    * referenced by this structure.
    */
#define GATT_SERVICE_DISCOVERY_RESPONSE_DATA_SIZE        (sizeof(GATT_Service_Discovery_Response_Data_t))

   /*! The following data type represents the type of data returned in a
    * service discovery event started by a call to the
    * GATT_Discover_Services() function.
    */
typedef struct _tagGATT_Service_Discovery_By_UUID_Response_Data_t
{
   unsigned int                        ConnectionID;
   unsigned int                        TransactionID;
   GATT_Connection_Type_t              ConnectionType;
   BD_ADDR_t                           RemoteDevice;
   Word_t                              NumberOfServices;
   GATT_Service_Information_By_UUID_t *ServiceInformationList;
} GATT_Service_Discovery_By_UUID_Response_Data_t;

   /*! The following MACRO is a utility MACRO that exists to calculate
    * the data size needed to hold the GATT_Service_Discovery_Response_t
    * structure along with the GATT_Service_Information_List list.  The
    * parameter to this MACRO specifies the number of services that are
    * referenced by this structure.
    */
#define GATT_SERVICE_DISCOVERY_BY_UUID_RESPONSE_DATA_SIZE (sizeof(GATT_Service_Discovery_By_UUID_Response_Data_t))

   /*! The following structure represents the data that is returned in an
    * include services discovery response event.
    */
typedef struct _tagGATT_Included_Services_Discovery_Response_Data_t
{
   unsigned int                ConnectionID;
   unsigned int                TransactionID;
   GATT_Connection_Type_t      ConnectionType;
   BD_ADDR_t                   RemoteDevice;
   Word_t                      NumberOfIncludes;
   GATT_Include_Information_t *IncludeInformationList;
} GATT_Included_Services_Discovery_Response_Data_t;

   /*! The following MACRO is a utility MACRO that exists to calculate
    * the memory size needed to hold an include services discovery
    * event. The first parameter to this MACRO specifies the number of
    * include services that are included in this event.
    */
#define GATT_INCLUDED_SERVICES_DISCOVERY_RESPONSE_DATA_SIZE (sizeof(GATT_Included_Services_Discovery_Response_Data_t))

   /*! The following structure represents the data that is in a
    * characteristic declaration value entry.
    */
typedef struct _tagGATT_Characteristic_Value_t
{
   Byte_t      CharacteristicProperties;
   Word_t      CharacteristicValueHandle;
   GATT_UUID_t CharacteristicUUID;
} GATT_Characteristic_Value_t;

#define GATT_CHARACTERISTIC_VALUE_SIZE                   (sizeof(GATT_Characteristic_Value_t))

   /*! The following structure represents the data that is contained in a
    * characteristic entry.
    */
typedef struct _tagGATT_Characteristic_Entry_t
{
   Word_t                      DeclarationHandle;
   GATT_Characteristic_Value_t CharacteristicValue;
} GATT_Characteristic_Entry_t;

#define GATT_CHARACTERISTIC_ENTRY_SIZE                   (sizeof(GATT_Characteristic_Entry_t))

   /*! The following data structure represents the data that is returned
    * in a characteristic discovery response event.
    */
typedef struct _tagGATT_Characteristic_Discovery_Response_Data_t
{
   unsigned int                 ConnectionID;
   unsigned int                 TransactionID;
   GATT_Connection_Type_t       ConnectionType;
   BD_ADDR_t                    RemoteDevice;
   Word_t                       NumberOfCharacteristics;
   GATT_Characteristic_Entry_t *CharacteristicEntryList;
} GATT_Characteristic_Discovery_Response_Data_t;

   /*! The following MACRO is a utility MACRO that exists to aid in
    * determining the amount of memory that is required in storing a
    * characteristic discovery event for the specified number of
    * characteristics.  The parameter to this MACRO is the number of
    * characteristics that are present this event.
    */
#define GATT_CHARACTERISTIC_DISCOVERY_RESPONSE_DATA_SIZE (sizeof(GATT_Characteristic_Discovery_Response_Data_t))

   /*! The following structure represents the data that is in a
    * characteristic descriptor discovery event entry.
    */
typedef struct _tagGATT_Characteristic_Descriptor_Entry_t
{
   Word_t      Handle;
   GATT_UUID_t UUID;
} GATT_Characteristic_Descriptor_Entry_t;

#define GATT_CHARACTERISTIC_DESCRIPTOR_ENTRY_SIZE        (sizeof(GATT_Characteristic_Descriptor_Entry_t))

   /*! The following data structure represents the data that is returned
    * in a characteristic descriptor discovery response event.
    */
typedef struct _tagGATT_Characteristic_Descriptor_Discovery_Response_Data_t
{
   unsigned int                            ConnectionID;
   unsigned int                            TransactionID;
   GATT_Connection_Type_t                  ConnectionType;
   BD_ADDR_t                               RemoteDevice;
   Word_t                                  NumberOfCharacteristicDescriptors;
   GATT_Characteristic_Descriptor_Entry_t *CharacteristicDescriptorEntryList;
} GATT_Characteristic_Descriptor_Discovery_Response_Data_t;

   /*! The following MACRO is a utility MACRO that exists to aid in
    * determining the amount of memory that is required to store a
    * characteristic descriptor discovery event given the specified
    * number of characteristics. The first parameter to this MACRO is
    * the number of characteristics that will be stored in this event.
    */
#define GATT_CHARACTERISTIC_DESCRIPTOR_DISCOVERY_RESPONSE_DATA_SIZE (sizeof(GATT_Characteristic_Descriptor_Discovery_Response_Data_t))

   /*! The following data structure represents the data that is returned
    * in a read response event.
    */
typedef struct _tagGATT_Read_Response_Data_t
{
   unsigned int            ConnectionID;
   unsigned int            TransactionID;
   GATT_Connection_Type_t  ConnectionType;
   BD_ADDR_t               RemoteDevice;
   Word_t                  AttributeValueLength;
   Byte_t                 *AttributeValue;
} GATT_Read_Response_Data_t;

#define GATT_READ_RESPONSE_DATA_SIZE                     (sizeof(GATT_Read_Response_Data_t))

   /*! The following data structure represents the data that is returned
    * in a read long response event.
    */
typedef struct _tagGATT_Read_Long_Response_Data_t
{
   unsigned int            ConnectionID;
   unsigned int            TransactionID;
   GATT_Connection_Type_t  ConnectionType;
   BD_ADDR_t               RemoteDevice;
   Word_t                  AttributeValueLength;
   Byte_t                 *AttributeValue;
} GATT_Read_Long_Response_Data_t;

   /*! The following MACRO is a utility MACRO that exists to aid in
    * determining the amount of memory that is required to store a GATT
    * read long response event.  The parameter to this MACRO is the
    * number of bytes contained in the AttributeValue member.
    */
#define GATT_READ_LONG_RESPONSE_DATA_SIZE                (sizeof(GATT_Read_Long_Response_Data_t))

   /*! The following represents the structure of a GATT read response
    * event entry.
    */
typedef struct _tagGATT_Read_Event_Entry_t
{
   Word_t  AttributeHandle;
   Word_t  AttributeValueLength;
   Byte_t *AttributeValue;
} GATT_Read_Event_Entry_t;

   /*! The following MACRO is a utility MACRO that exists to aid in
    * determining the amount of memory that is required to store a GATT
    * read event entry.  The parameter to this MACRO is the number of
    * bytes in the AttributeValue member.
    */
#define GATT_READ_EVENT_ENTRY_SIZE                       (sizeof(GATT_Read_Event_Entry_t))

   /*! The following data structure represents the data that is returned
    * in a read by UUID event.
    */
typedef struct _tagGATT_Read_By_UUID_Response_Data_t
{
   unsigned int             ConnectionID;
   unsigned int             TransactionID;
   GATT_Connection_Type_t   ConnectionType;
   BD_ADDR_t                RemoteDevice;
   Word_t                   NumberOfAttributes;
   GATT_Read_Event_Entry_t *AttributeList;
} GATT_Read_By_UUID_Response_Data_t;

   /*! The following MACRO is a utility MACRO that exists to aid in
    * determining the amount of memory that is required to store a GATT
    * read by UUID event entry. The first parameter to this MACRO is
    * the number of attributes that are being returned in the
    * AttributeList member.  The next is the number of bytes that are in
    * the AttributeValue for each attribute that is returned in the
    * AttributeList.
    */
#define GATT_READ_BY_UUID_RESPONSE_DATA_SIZE             (sizeof(GATT_Read_By_UUID_Response_Data_t))

   /*! The following data structure represents the data that is returned
    * in a read multiple response event.
    * \note The AttributeValuesLength is the TOTAL length (in bytes)
    *          of ALL attribute values, NOT the length of an individual
    *          attribute value.
    */
typedef struct _tagGATT_Read_Multiple_Response_Data_t
{
   unsigned int            ConnectionID;
   unsigned int            TransactionID;
   GATT_Connection_Type_t  ConnectionType;
   BD_ADDR_t               RemoteDevice;
   Word_t                  AttributeValuesLength;
   Byte_t                 *AttributeValues;
} GATT_Read_Multiple_Response_Data_t;

   /*! The following MACRO is a utility MACRO that exists to aid in
    * determining the amount of memory that is required to store a GATT
    * read multiple response event.  The parameter to this MACRO is the
    * number of bytes contained in the AttributeValues member.
    */
#define GATT_READ_MULTIPLE_RESPONSE_DATA_SIZE            (sizeof(GATT_Read_Multiple_Response_Data_t))

   /*! The following data structure represents the data that is returned
    * in a write response event.
    */
typedef struct _tagGATT_Write_Response_Data_t
{
   unsigned int           ConnectionID;
   unsigned int           TransactionID;
   GATT_Connection_Type_t ConnectionType;
   BD_ADDR_t              RemoteDevice;
   unsigned int           BytesWritten;
} GATT_Write_Response_Data_t;

#define GATT_WRITE_RESPONSE_DATA_SIZE                    (sizeof(GATT_Write_Response_Data_t))

   /*! The following data structure represents the data that is returned
    * in a prepare write response event.
    */
typedef struct _tagGATT_Prepare_Write_Response_Data_t
{
   unsigned int            ConnectionID;
   unsigned int            TransactionID;
   GATT_Connection_Type_t  ConnectionType;
   BD_ADDR_t               RemoteDevice;
   unsigned int            BytesWritten;
   Word_t                  AttributeHandle;
   Word_t                  AttributeValueOffset;
   Word_t                  AttributeValueLength;
   Byte_t                 *AttributeValue;
} GATT_Prepare_Write_Response_Data_t;

   /*! The following MACRO is a utility MACRO that exists to aid in
    * determining the amount of memory that is required to store a GATT
    * prepare write response event.  The parameter to this MACRO is the
    * number of bytes contained in the AttributeValue member.
    */
#define GATT_PREPARE_WRITE_RESPONSE_DATA_SIZE            (sizeof(GATT_Prepare_Write_Response_Data_t))

   /*! The following data structure represents the data that is returned
    * in a execute write response event.
    */
typedef struct _tagGATT_Execute_Write_Response_Data_t
{
   unsigned int           ConnectionID;
   unsigned int           TransactionID;
   GATT_Connection_Type_t ConnectionType;
   BD_ADDR_t              RemoteDevice;
} GATT_Execute_Write_Response_Data_t;

#define GATT_EXECUTE_WRITE_RESPONSE_DATA_SIZE            (sizeof(GATT_Execute_Write_Response_Data_t))

   /*! The following data structure represents the data that is returned
    * in a Exchange MTU Response event.
    */
typedef struct _tagGATT_Exchange_MTU_Response_Data_t
{
   unsigned int           ConnectionID;
   unsigned int           TransactionID;
   GATT_Connection_Type_t ConnectionType;
   BD_ADDR_t              RemoteDevice;
   Word_t                 ServerMTU;
} GATT_Exchange_MTU_Response_Data_t;

#define GATT_EXCHANGE_MTU_RESPONSE_DATA_SIZE      (sizeof(GATT_Exchange_MTU_Response_Data_t))

   /*! The following structure represents the container structure for
    * holding all GATT client event data.
    */
typedef struct _tagGATT_Client_Event_Data_t
{
   GATT_Client_Event_Type_t Event_Data_Type;
   Word_t                   Event_Data_Size;
   union
   {
      GATT_Request_Error_Data_t                                *GATT_Request_Error_Data;
      GATT_Service_Discovery_Response_Data_t                   *GATT_Service_Discovery_Response_Data;
      GATT_Service_Discovery_By_UUID_Response_Data_t           *GATT_Service_Discovery_By_UUID_Response_Data;
      GATT_Included_Services_Discovery_Response_Data_t         *GATT_Included_Services_Discovery_Response_Data;
      GATT_Characteristic_Discovery_Response_Data_t            *GATT_Characteristic_Discovery_Response_Data;
      GATT_Characteristic_Descriptor_Discovery_Response_Data_t *GATT_Characteristic_Descriptor_Discovery_Response_Data;
      GATT_Read_Response_Data_t                                *GATT_Read_Response_Data;
      GATT_Read_By_UUID_Response_Data_t                        *GATT_Read_By_UUID_Response_Data;
      GATT_Read_Long_Response_Data_t                           *GATT_Read_Long_Response_Data;
      GATT_Read_Multiple_Response_Data_t                       *GATT_Read_Multiple_Response_Data;
      GATT_Write_Response_Data_t                               *GATT_Write_Response_Data;
      GATT_Prepare_Write_Response_Data_t                       *GATT_Prepare_Write_Response_Data;
      GATT_Execute_Write_Response_Data_t                       *GATT_Execute_Write_Response_Data;
      GATT_Exchange_MTU_Response_Data_t                        *GATT_Exchange_MTU_Response_Data;
   } Event_Data;
} GATT_Client_Event_Data_t;

#define GATT_CLIENT_EVENT_DATA_SIZE                      (sizeof(GATT_Client_Event_Data_t))

   /*! GATT Service Discovery Event types.
    * The following structure represents the data that is returned in a
    * Service Discovery Indication event.
    */
typedef struct _tagGATT_Service_Discovery_Indication_Data_t
{
   unsigned int                       ConnectionID;
   GATT_Service_Information_t         ServiceInformation;
   unsigned int                       NumberOfIncludedService;
   GATT_Service_Information_t        *IncludedServiceList;
   unsigned int                       NumberOfCharacteristics;
   GATT_Characteristic_Information_t *CharacteristicInformationList;
} GATT_Service_Discovery_Indication_Data_t;

#define GATT_SERVICE_DISCOVERY_INDICATION_DATA_SIZE      (sizeof(GATT_Service_Discovery_Indication_Data_t))

   /*! The following structure represents the data that is returned in a
    * GATT Service Discovery Complete event.
    */
typedef struct _tagGATT_Service_Discovery_Complete_Data_t
{
   unsigned int ConnectionID;
   Byte_t       Status;
} GATT_Service_Discovery_Complete_Data_t;

#define GATT_SERVICE_DISCOVERY_COMPLETE_DATA_SIZE        (sizeof(GATT_Service_Discovery_Complete_Data_t))

   /*! The following define the valid values that the Status member of
    * the GATT_Service_Discovery_Complete_Data_t may be set to.
    */
#define GATT_SERVICE_DISCOVERY_STATUS_SUCCESS             0x00
#define GATT_SERVICE_DISCOVERY_STATUS_RESPONSE_ERROR      0x01
#define GATT_SERVICE_DISCOVERY_STATUS_RESPONSE_TIMEOUT    0x02
#define GATT_SERVICE_DISCOVERY_STATUS_UNKNOWN_ERROR       0x03

   /*! The following structure represents the container structure for
    * Holding all GATT Service Discovery Event Data.
    */
typedef struct _tagGATT_Service_Discovery_Event_Data_t
{
   GATT_Service_Discovery_Event_Type_t Event_Data_Type;
   Word_t                              Event_Data_Size;
   union
   {
      GATT_Service_Discovery_Indication_Data_t *GATT_Service_Discovery_Indication_Data;
      GATT_Service_Discovery_Complete_Data_t   *GATT_Service_Discovery_Complete_Data;
   } Event_Data;
} GATT_Service_Discovery_Event_Data_t;

#define GATT_SERVICE_DISCOVERY_EVENT_DATA_SIZE           (sizeof(GATT_Service_Discovery_Event_Data_t))

   /*! The following declared type represents the prototype function for
    * a GATT connection event Callback.  This function will be called
    * whenever a GATT connection Event occurs that is associated with
    * the specified Bluetooth stack ID.  This function passes to the
    * caller the Bluetooth stack ID, the GATT event data that occurred
    * and the GATT event callback parameter that was specified when this
    * Callback was installed.  The caller is free to use the contents
    * of the GATT event data ONLY in the context of this callback.  If
    * the caller requires the data for a longer period of time, then
    * the callback function MUST copy the data into another data
    * buffer.  This function is guaranteed NOT to be invoked more than
    * once simultaneously for the specified installed callback (i.e.
    * this function DOES NOT have be reentrant).  It needs to be noted,
    * however, that if the same callback is installed more than once,
    * then the callbacks will be called serially.  Because of this, the
    * processing in this function should be as efficient as possible.
    * It should also be noted that this function is called in the
    * thread context of a thread that the user does NOT own.  Therefore,
    * processing in this function should be as efficient as possible
    * (this argument holds anyway because another GATT Event will not
    * be processed while this function call is outstanding).
    * \note This function MUST NOT block and wait for events that
    *            can only be satisfied by receiving GATT events.  A
    *            deadlock WILL occur because NO GATT event callbacks
    *            will be issued while this function is currently
    *            outstanding.
    */
typedef void (BTPSAPI *GATT_Connection_Event_Callback_t)(unsigned int BluetoothStackID, GATT_Connection_Event_Data_t *GATT_Connection_Event_Data, unsigned long CallbackParameter);

   /*! The following declared type represents the prototype function for
    * a GATT server event callback.  This function will be called
    * whenever a GATT server event occurs that is associated with the
    * specified Bluetooth stack ID.  This function passes to the caller
    * the Bluetooth stack ID, the GATT event data that occurred and
    * the GATT event callback parameter that was specified when this
    * callback was installed.  The caller is free to use the contents
    * of the GATT event data ONLY in the context of this callback.  If
    * the caller requires the Data for a longer period of time, then
    * the callback function MUST copy the data into another data
    * buffer.  This function is guaranteed NOT to be invoked more than
    * once simultaneously for the specified installed callback (i.e.
    * this function DOES NOT have be reentrant).  It needs to be noted,
    * however, that if the same callback is installed more than once,
    * then the callbacks will be called serially.  Because of this, the
    * processing in this function should be as efficient as possible.
    * It should also be noted that this function is called in the
    * thread context of a thread that the user does NOT own.  Therefore,
    * processing in this function should be as efficient as possible
    * (this argument holds anyway because another GATT event will not
    * be processed while this function call is outstanding).
    * \note This function MUST NOT block and wait for events that
    *            can only be satisfied by receiving GATT events.  A
    *            deadlock WILL occur because NO GATT event callbacks
    *            will be issued while this function is currently
    *            outstanding.
    */
typedef void (BTPSAPI *GATT_Server_Event_Callback_t)(unsigned int BluetoothStackID, GATT_Server_Event_Data_t *GATT_Server_Event_Data, unsigned long CallbackParameter);

   /*! The following declared type represents the prototype function for
    * a GATT client event callback.  This function will be called
    * whenever a GATT client event occurs that is associated with the
    * specified Bluetooth stack ID.  This function passes to the caller
    * the Bluetooth stack ID, the GATT event data that occurred and the
    * GATT event callback parameter that was specified when this
    * callback was installed.  The caller is free to use the contents of
    * the GATT event data ONLY in the context of this callback.  If the
    * caller requires the Data for a longer period of time, then the
    * callback function MUST copy the data into another data buffer.
    * This function is guaranteed NOT to be invoked more than once
    * simultaneously for the specified installed callback (i.e. this
    * function DOES NOT have be reentrant).  It needs to be noted,
    * however, that if the same callback is installed more than once,
    * then the callbacks will be called serially.  Because of this, the
    * processing in this function should be as efficient as possible.
    * It should also be noted that this function is called in the thread
    * context of a thread that the user does NOT own.  Therefore,
    * processing in this function should be as efficient as possible
    * (this argument holds anyway because another GATT event will not be
    * processed while this function call is outstanding).
    * \note This function MUST NOT block and wait for events that
    *            can only be satisfied by receiving GATT events.  A
    *            deadlock WILL occur because NO GATT event callbacks
    *            will be issued while this function is currently
    *            outstanding.
    */
typedef void (BTPSAPI *GATT_Client_Event_Callback_t)(unsigned int BluetoothStackID, GATT_Client_Event_Data_t *GATT_Client_Event_Data, unsigned long CallbackParameter);

   /*! The following declared type represents the Prototype Function for
    * a GATT Service Discovery Event Data Callback.  This function will
    * be called whenever a GATT Service Discovery Event occurs that is
    * associated with the specified Bluetooth Stack ID.  This function
    * passes to the caller the Bluetooth Stack ID, the GATT Service
    * Discovery Event Data that occurred and the GATT Service Discovery
    * Event Callback Parameter that was specified when this Callback was
    * installed.  The caller is free to use the contents of the GATT
    * Service Discovery Event Data ONLY in the context of this callback.
    * If the caller requires the Data for a longer period of time, then
    * the callback function MUST copy the data into another Data Buffer.
    * This function is guaranteed NOT to be invoked more than once
    * simultaneously for the specified installed callback (i.e.  this
    * function DOES NOT have be reentrant).  It needs to be noted
    * however, that if the same Callback is installed more than once,
    * then the callbacks will be called serially.  Because of this, the
    * processing in this function should be as efficient as possible.
    * It should also be noted that this function is called in the Thread
    * Context of a Thread that the User does NOT own.  Therefore,
    * processing in this function should be as efficient as possible
    * (this argument holds anyway because another GATT Service Discovery
    * Event will not be processed while this function call is
    * outstanding).
    * \note This function MUST NOT Block and wait for events that
    *            can only be satisfied by Receiving GATT Service
    *            Discovery Event Packets.  A Deadlock WILL occur because
    *            NO GATT Service Discovery Event Callbacks will be
    *            issued while this function is currently outstanding.
    */
typedef void (BTPSAPI *GATT_Service_Discovery_Event_Callback_t)(unsigned int BluetoothStackID, GATT_Service_Discovery_Event_Data_t *GATT_Service_Discovery_Event_Data, unsigned long CallbackParameter);

   /*! GATT General API. */
   /*! @brief The following function is responsible for initializing the GATT.
    * This function MUST be called before any other GATT function.
    * @param BluetoothStackID    Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param Flags Initialization flags bit-mask.  This value must be
    * one (or more) of the following bit-mask constant flags:
    * GATT_INITIALIZATION_FLAGS_SUPPORT_LE, GATT_INITIALIZATION_FLAGS_SUPPORT_BR_EDR
    * @param ConnectionEventCallback Callback function that is registered
    * to receive connection events.
    * @param CallbackParameter   A user-defined parameter (e.g., a tag value)
    * that will be passed back to the user in the callback function with each
    * connection event.
    * @return This function returns zero if
    * successful, or a negative return value if there was an error.
    * \note This function must be called once for a given Bluetooth
    *          stack ID.
    * \note The Flags parameter must be made up of bitmasks of the
    *          form GATT_INITIALIZATION_FLAGS_SUPPORT_XXX AND cannot be
    *          ZERO.
    * \note The specified callback will receive ALL the of the
    *          defined connection events.  This is in contrast to the
    *          callback registered with the
    *          GATT_Register_Connection_Events() function which will
    *          receive ALL the connection events EXCEPT:
    *             - etGATT_Connection_Device_Connection_Request
    *          It should be noted that the connection request event
    *          is ONLY applicable to BR/EDR devices (i.e. not LE
    *          devices).
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Initialize(unsigned int BluetoothStackID, unsigned long Flags, GATT_Connection_Event_Callback_t ConnectionEventCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Initialize_t)(unsigned int BluetoothStackID, unsigned long Flags, GATT_Connection_Event_Callback_t ConnectionEventCallback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is responsible for cleaning up a previously
    * initialized GATT instance.  After calling this function,
    * GATT_Initialize() must be called for the GATT using the specified
    * Bluetooth stack again before any other GATT function can be
    * called.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @return This function returns zero if successful, or a negative return
    * value if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Cleanup(unsigned int BluetoothStackID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Cleanup_t)(unsigned int BluetoothStackID);
#endif

   /*! @brief The following function is provided to allow a mechanism to
    * register an event callback to monitor device connection and
    * server indication events.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param ConnectionEventCallback Callback function that is registered to
    * receive connection events.
    * @param CallbackParameter A user-defined parameter (e.g., a tag value)
    * that will be passed back to the user in the callback function with each
    * connection event.
    * @return This function returns a
    * positive value if successful, or a negative return error code if
    * there was an error.
    * \note A successful return value from this function represents
    *          the Event Callback ID.  This value can be passed to the
    *          GATT_Un_Register_Connection_Events() function to
    *          un-register this callback.
    * \note The specified callback will receive ALL the of the
    *          defined connection events EXCEPT:
    *             - etGATT_Connection_Device_Connection_Request
    *             - etGATT_Connection_Service_Changed_Read_Request
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Register_Connection_Events(unsigned int BluetoothStackID, GATT_Connection_Event_Callback_t ConnectionEventCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Register_Connection_Events_t)(unsigned int BluetoothStackID, GATT_Connection_Event_Callback_t ConnectionEventCallback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to allow a mechanism to
    * un-register a previously registered connect event callback
    * (registered via a successful call to the
    * GATT_Register_Connection_Events() function.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param EventCallbackID Event callback ID of the event callback to un-register.  This value was obtained via the successful return value from calling the GATT_Register_Connection_Events() function.
    * @return This function returns zero if
    * successful or a negative return error code if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Un_Register_Connection_Events(unsigned int BluetoothStackID, unsigned int EventCallbackID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Un_Register_Connection_Events_t)(unsigned int BluetoothStackID, unsigned int EventCallbackID);
#endif

   /*! @brief The following function is provided to allow a mechanism to accept
    * or reject an incoming BR/EDR connection when operating in
    * gimManualAccept mode (incoming connection mode).
    * @param BluetoothStackID    Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Bluetooth device address of the remote BR/EDR Bluetooth
    * device that is attempting to connect with the local device.
    * @param AcceptConnection    Specifies whether to accept the incoming
    * BR/EDR connection (TRUE) or reject the incoming BR/EDR connection (FALSE).
    * @return This function returns zero if successful, or a negative return error
    * code if there was an error.
    * \note The device is not connected until the device connection
    *          event is received.
    * \note This function is only valid for BR/EDR connections.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Connection_Request_Response(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Boolean_t AcceptConnection);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Connection_Request_Response_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Boolean_t AcceptConnection);
#endif

   /*! @brief The following function is a BR/EDR ONLY function that exists to
    * make a connection to a remote GATT instance.  This function is
    * only applicable to BR/EDR connections.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param BD_ADDR Bluetooth device address of the remote BR/EDR Bluetooth GATT
    * server to connect.
    * @param ConnectionEventCallback Callback function that is registered to
    * receive the connection confirmation event (which contains the connection status).
    * @param CallbackParameter A user-defined parameter (e.g., a tag value)
    * that will be passed back to the user in the callback function.
    * @return This function returns a positive value if successful, or a
    * negative return error code if there was an error.
    * \note A successful return value from this function represents
    *          the Connection ID of the connection.
    * \note The ONLY event that will be dispatched to the event
    *          callback specified in this function is the:
    *             - etGATT_Connection_Device_Connection_Confirmation
    *          event.
    * \note This function is only valid for BR/EDR connections.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Connect(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GATT_Connection_Event_Callback_t ConnectionEventCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Connect_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GATT_Connection_Event_Callback_t ConnectionEventCallback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to allow a mechanism to
    * disconnect a currently connected BR/EDR GATT connection.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param ConnectionID Connection ID that identifies the currently
    * connected BR/EDR GATT connection that is to be disconnected.
    * @return This function returns zero if
    * succesful or a negative return error code if there was an error.
    * \note This function is only valid for BR/EDR connections.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Disconnect(unsigned int BluetoothStackID, unsigned int ConnectionID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Disconnect_t)(unsigned int BluetoothStackID, unsigned int ConnectionID);
#endif

   /*! @brief The following function is responsible for retrieving the current
    * BR/EDR GATT incoming connection mode.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param IncomingConnectionMode  Pointer to a buffer that is to receive
    * the currently configured BR/EDR GATT incoming connection mode.
    * This value will be one of the following:
    * gimAutomaticAccept or gimAutomaticReject or gimManualAccept
    * @return This function returns zero if successful, or a negative return error
    * code if an error occurred.
    * \note The default incoming connection mode is
    *          gimAutomaticAccept.
    * \note This function is used for GATT implementations which
    *          implement Bluetooth security mode 2.
    * \note This function is only applicable to BR/EDR connections.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Get_Incoming_Connection_Mode(unsigned int BluetoothStackID, GATT_Incoming_Connection_Mode_t *IncomingConnectionMode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Get_Incoming_Connection_Mode_t)(unsigned int BluetoothStackID, GATT_Incoming_Connection_Mode_t *IncomingConnectionMode);
#endif

   /*! @brief The following function is responsible for setting the BR/EDR GATT
    * incoming connection mode.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param IncomingConnectionMode New BR/EDR GATT incoming connection mode.
    * This value must be one of the following:
    * gimAutomaticAccept or gimAutomaticReject or gimManualAccept.
    * @return This function returns zero if successful, or a negative return
    * error code if an error occurred.
    * \note The default incoming connection mode is
    *          gimAutomaticAccept.
    * \note This function is used for GATT implementations which
    *          implement Bluetooth security mode 2.
    * \note This function is only applicable to BR/EDR connections.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Set_Incoming_Connection_Mode(unsigned int BluetoothStackID, GATT_Incoming_Connection_Mode_t IncomingConnectionMode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Set_Incoming_Connection_Mode_t)(unsigned int BluetoothStackID, GATT_Incoming_Connection_Mode_t IncomingConnectionMode);
#endif

   /*! @brief The following function is provided to allow a means to add a
    * Generic SDP Service Record to the SDP Database.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize
    * @param SDPServiceRecord Any additional Service Discovery Protocol information
    * to be added to the record for the GATT SDP record.
    * @param SDPServiceRecordHandle Returned handle to the SDP Database entry which
    * may be used to remove the entry at a later time.
    * @return If this function returns zero, then the
    * SDPServiceRecordHandle entry will contain the Service Record
    * Handle of the added SDP Service Record.  If this function fails, a
    * negative return error code will be returned (see BTERRORS.H) and
    * the SDPServiceRecordHandle value will be undefined.
    * \note The Service Record Handle that is returned from this
    *          function will remain in the SDP Record Database until it
    *          is deleted by calling the SDP_Delete_Service_Record()
    *          function.
    * \note A MACRO is provided to Delete the Service Record from the
    *          SDP Data Base.  This MACRO maps the
    *          GATT_Un_Register_SDP_Record() to
    *          SDP_Delete_Service_Record().
    * \note If NO UUID Information is specified in the
    *          SDPServiceRecord Parameter, then the default GATT Service
    *          Class's are added.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Register_SDP_Record(unsigned int BluetoothStackID, GATT_SDP_Service_Record_t *SDPServiceRecord, DWord_t *SDPServiceRecordHandle);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Register_SDP_Record_t)(unsigned int BluetoothStackID, GATT_SDP_Service_Record_t *SDPServiceRecord, DWord_t *SDPServiceRecordHandle);
#endif

   /*! The following MACRO is a utility MACRO that simply deletes
    * the GATT SDP Service Record (specified by the third parameter)
    * from SDP Database.  This MACRO simply maps to the
    * SDP_Delete_Service_Record() function.  This MACRO is only
    * provided so that the caller doesn't have to sift through the SDP
    * API for very simplistic applications.  This function accepts as
    * input the Bluetooth Stack ID of the Bluetooth Protocol Stack
    * that the Service Record exists on, and the SDP Service Record
    * Handle.  The SDP Service Record Handle was returned via a
    * succesful call to the GATT_Register_SDP_Record() function.  See
    * the GATT_Register_SDP_Record() function for more information.
    * This MACRO returns the result of the SDP_Delete_Service_Record()
    * function, which is zero for success or a negative return error
    * code (see BTERRORS.H).
    */
#define GATT_Un_Register_SDP_Record(__BluetoothStackID, __SDPRecordHandle) \
        (SDP_Delete_Service_Record(__BluetoothStackID, __SDPRecordHandle))

   /*! GATT Server API. */
   /*! @brief The following function is provided to allow a means to add a GATT
    * Service to the local GATT Database.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param ServiceFlags Specifies the current service flags to apply to the
    * registered service.  The value of this parameter is a bit-mask of zero or more of the following values:
    * GATT_SERVICE_FLAGS_LE_SERVICE or GATT_SERVICE_FLAGS_BR_EDR_SERVICE.
    * @param NumberOfServiceAttributeEntries Specifies the total number of
    * service attribute entries that are contained in the ServiceTable parameter.
    * @param ServiceTable Pointer to an array of service attribute entries
    * that specify all of the service attributes for the registered service.
    * @param ServiceHandleGroupResult This parameter is both an input and
    * output parameter.  On input this parameter can be used to request that
    * GATT place the service at a specific handle range in the GATT database.
    * This is accomplished by setting the Starting_Handle and Ending_Handle members of this
    * structure to the requested handle range in the GATT database.
    * If these members are zero, or otherwise invalid, on input then the GATT
    * layer will place the service in the first available region in the GATT database.
    * On output GATT will return the handle range of the registered service if this
    * function returns success.
    * @param ServerEventCallback Callback function that is registered to
    * receive events that are associated with the specified service.
    * @param CallbackParameter A user-defined parameter (e.g., a tag value)
    * that will be passed back to the user in the callback function.
    * @return  This function will return a positive
    * non-zero service ID if successful, or a negative return
    * error code if there was an error.  If this function returns
    * success then the ServiceHandleGroupResult buffer will contain the
    * service's attribute handle range.
    * \note The first entry in the array pointed to by service will
    *          be allocated with the starting handle specified in the
    *          ServiceHandleGroupResult structure.  The second entry
    *          will be allocated at the starting Handle + 1 and so on.
    * \note The ServiceFlags is a bit mask that is made up of bit
    *          masks of the form GATT_SERVICE_FLAGS_XXX.  The
    *          ServicesFlags MAY NOT be ZERO, i.e.  at least one
    *          GATT_SERVICE_FLAGS_XXX MUST be set for this call to
    *          succeed.
    * \note If the GATT_SERVICE_FLAGS_BR_EDR_SERVICE bit is set in
    *          the ServiceFlags, it is the responsibility of the app to
    *          call GATT_Register_Service_SDP_Record() passing in the
    *          handle range that is returned from a successful call to
    *          this function.
    * \note The ServiceHandleGroupResult parameter is both an input
    *          and an output parameter.  If the following test is true
    *          (and Starting_Handle and Ending_Handle members are
    *          greater than zero) then the GATT layer will attempt to
    *          assign the service into the specified location in the
    *          service table:
    *
    *             ServiceHandleGroupResult->Ending_Handle ==
    *               (ServiceHandleGroupResult->Starting_Handle +
    *               (NumberOfServiceAttributeEntries - 1))
    *
    *          If the specified location is not available then* @return This function will return the error code
    *          BTGATT_ERROR_INSUFFICIENT_HANDLES.
    * \note The Service Table passed to this function is stored as a
    *          reference and as such its associated memory MUST be
    *          maintained by the application until the service has been
    *          removed via a call to GATT_Un_Register_Service().
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Register_Service(unsigned int BluetoothStackID, Byte_t ServiceFlags, unsigned int NumberOfServiceAttributeEntries, GATT_Service_Attribute_Entry_t *ServiceTable, GATT_Attribute_Handle_Group_t *ServiceHandleGroupResult, GATT_Server_Event_Callback_t ServerEventCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Register_Service_t)(unsigned int BluetoothStackID, Byte_t ServiceFlags, unsigned int NumberOfServiceAttributeEntries, GATT_Service_Attribute_Entry_t *ServiceTable, GATT_Attribute_Handle_Group_t *ServiceHandleGroupResult, GATT_Server_Event_Callback_t ServerEventCallback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to un-register a previously
    * registered GATT service that was registered via a successful call
    * to GATT_Register_Service() function.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param ServiceID Specifies the service ID of the service that is to be removed.  This value is the successful return value from the call to GATT_Register_Service.
    */
BTPSAPI_DECLARATION void BTPSAPI GATT_Un_Register_Service(unsigned int BluetoothStackID, unsigned int ServiceID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (BTPSAPI *PFN_GATT_Un_Register_Service_t)(unsigned int BluetoothStackID, unsigned int ServiceID);
#endif

   /*! @brief The following function is provided to allow a means to add a
    * Generic SDP Service Record to the SDP Database.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param SDPServiceRecord Service Discovery Protocol information to
    * be added to the record for the GATT SDP record.
    * @param ServiceHandleRange Service Handle Range that is returned
    * from a successful call to GATT_Register_Service().
    * @param SDPServiceRecordHandle Returned handle to the SDP Database
    * entry which may be used to remove the entry at a later time.
    * @return If this function returns zero, then the SDPServiceRecordHandle entry will
    * contain the Service Record Handle of the added SDP Service Record.
    * If this function fails, a negative return error code will be
    * returned (see BTERRORS.H) and the SDPServiceRecordHandle value
    * will be undefined.
    * \note The Service Record Handle that is returned from this
    *          function will remain in the SDP Record Database until it
    *          is deleted by calling the SDP_Delete_Service_Record()
    *          function.
    * \note A MACRO is provided to Delete the Service Record from the
    *          SDP Data Base.  This MACRO maps the
    *          GATT_Un_Register_Service_SDP_Record() to
    *          SDP_Delete_Service_Record().
    * \note It is the responsibility of the application to call this
    *          API for ever registered service that supports BR/EDR
    *          access.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Register_Service_SDP_Record(unsigned int BluetoothStackID, GATT_SDP_Service_Record_t *SDPServiceRecord, GATT_Attribute_Handle_Group_t *ServiceHandleRange, DWord_t *SDPServiceRecordHandle);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Register_Service_SDP_Record_t)(unsigned int BluetoothStackID, GATT_SDP_Service_Record_t *SDPServiceRecord, GATT_Attribute_Handle_Group_t *ServiceHandleRange, DWord_t *SDPServiceRecordHandle);
#endif

   /*! The following MACRO is a utility MACRO that simply deletes
    * the GATT SDP Service Record (specified by the third parameter) from SDP Database.  This MACRO simply maps to the
    * SDP_Delete_Service_Record() function.  This MACRO is only
    * provided so that the caller doesn't have to sift through the SDP
    * API for very simplistic applications.  This function accepts as
    * input the Bluetooth Stack ID of the Bluetooth Protocol Stack
    * that the Service Record exists on, and the SDP Service Record
    * Handle.  The SDP Service Record Handle was returned via a
    * succesful call to the GATT_Register_SDP_Record() function.  See
    * the GATT_Register_SDP_Record() function for more information.
    * This MACRO returns the result of the SDP_Delete_Service_Record()
    * function, which is zero for success or a negative return error
    * code (see BTERRORS.H).
    */
#define GATT_Un_Register_Service_SDP_Record(__BluetoothStackID, __SDPRecordHandle) \
        (SDP_Delete_Service_Record(__BluetoothStackID, __SDPRecordHandle))

   /*! @brief The following function is provided to allow a means of responding
    * with a successful response to a received read request event.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param TransactionID Transaction ID of the original read request.
    * This value was received in the etGATT_Server_Read_Request event.
    * @param DataLength Specifies the amount of data to return.
    * This is the amount of data (in bytes) pointed to by the Data parameter.
    * @param Data Specifies the buffer that contains the data to
    * return in the read response.
    * This buffer must point to a buffer that contains (at least) as many
    * bytes as specified by the DataLength parameter.
    * @return This function will return a ZERO
    * return code if successful, or a negative return error code if
    * there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Read_Response(unsigned int BluetoothStackID, unsigned int TransactionID, unsigned int DataLength, Byte_t *Data);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Read_Response_t)(unsigned int BluetoothStackID, unsigned int TransactionID, unsigned int DataLength, Byte_t *Data);
#endif

   /*! @brief The following function is provided to allow a means of responding
    * with a successful response to a received write request event.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param TransactionID Transaction ID of the original write request.
    * This value was received in the etGATT_Server_Write_Request event.
    * @return This function will return a ZERO return code if successful,
    * or a negative return error code if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Write_Response(unsigned int BluetoothStackID, unsigned int TransactionID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Write_Response_t)(unsigned int BluetoothStackID, unsigned int TransactionID);
#endif

   /*! @brief The following function is provided to allow a means of responding
    * witha successful response to a received execute write request
    * event.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param TransactionID Transaction ID of the original execute write request.
    * This value was received in the etGATT_Server_Execute_Write_Request event.
    * @return This function will return a ZERO return code if
    * successful, or a negative return error code if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Execute_Write_Response(unsigned int BluetoothStackID, unsigned int TransactionID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Execute_Write_Response_t)(unsigned int BluetoothStackID, unsigned int TransactionID);
#endif

   /*! @brief The following function is provided to allow a means of responding
    * with a generic error response to a specified received client
    * request.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param TransactionID Transaction ID of the original request.  This value was
    * received in the GATT request event.
    * @param AttributeOffset Attribute offset of the first attribute that causes
    * the error.  This value will be greater than or equal to zero (specifies the very first attribute in the service) and less than the maximum number of attributes contained in the service.
    * @param ErrorCode Error code to return as a response to the request.
    * @return This function will return
    * a ZERO return code if successful, or a negative return error code
    * if there was an error.
    * \note The AttributeOffset parameter must be less than or equal
    *          to the last index in the service table that was
    *          registered in the call to the GATT_Register_Service()
    *          function.
    * \note The ErrorCode variable should be one of the
    *          ATT_PROTOCOL_ERROR_CODE_xxx values.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Error_Response(unsigned int BluetoothStackID, unsigned int TransactionID, Word_t AttributeOffset, Byte_t ErrorCode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Error_Response_t)(unsigned int BluetoothStackID, unsigned int TransactionID, Word_t AttributeOffset, Byte_t ErrorCode);
#endif

   /*! @brief The following function is provided to allow a means of sending a
    * Handle/Value indication to a remote GATT client.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param ServiceID Service ID of the service that is sending the indication.
    * @param ConnectionID Connection ID of the currently connected remote
    * client device to send the handle/value indication.
    * @param AttributeOffset Attribute offset of the attribute that is being
    * indicated.  This value will be greater than or equal to zero (specifies the very first attribute in the service) and less than the maximum number of attributes contained in the service.
    * @param AttributeValueLength Length (in bytes) of the attribute value
    * data that is to be indicated.
    * @param AttributeValue Buffer that contains the value data to be indicated.
    * This buffer must contain (at least) the number of bytes specified by the AttributeValueLength parameter.
    * @return This function will return the positive, non-zero,
    * Transaction ID of the Handle
    * Value Indication or a negative error code.
    * \note The ServiceID parameter must have been returned from a
    *          successful call to the GATT_Register_Server() function.
    * \note If successful the return value will contain the
    *          Transaction ID that can be used to cancel the Indication.
    * \note A etGATT_Server_Confirmation_Response event will be
    *          dispatched to the registered service event callback
    *          when the indication is acknowledged by the remote device.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Handle_Value_Indication(unsigned int BluetoothStackID, unsigned int ServiceID, unsigned int ConnectionID, Word_t AttributeOffset, Word_t AttributeValueLength, Byte_t *AttributeValue);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Handle_Value_Indication_t)(unsigned int BluetoothStackID, unsigned int ServiceID, unsigned int ConnectionID, Word_t AttributeOffset, Word_t AttributeValueLength, Byte_t *AttributeValue);
#endif

   /*! @brief The following function is provided to allow a means of sending a
    * Handle/Value notification to a remote GATT client.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param ServiceID Service ID of the service that is sending the notification.
    * @param ConnectionID Connection ID of the currently connected remote client
    * device to send the handle/value notification.
    * @param AttributeOffset Attribute offset of the attribute that is being notified.  This value will be greater than or equal to zero (specifies the very first attribute in the service) and less than the maximum number of attributes contained in the service.
    * @param AttributeValueLength Length (in bytes) of the attribute value data
    * that is to be notified.
    * @param AttributeValue Buffer that contains the value data to be notified.
    *  This buffer must contain (at least) the number of bytes specified by the AttributeValueLength parameter.
    * @return This function will return a non-negative value that represents the
    * actual length
    * of the attribute value that was notified, or a negative return
    * error code if there was an error.
    * \note The ServiceID parameter must have been returned from a
    *          successful call to the GATT_Register_Server() function.
    * \note If a successful return value is returned, it will be
    *          greater than ZERO and less than or equal to
    *          AttributeValueLength parameter.
    * \note If @return This function returns the Error Code:
    *
    *             BTPS_ERROR_INSUFFICIENT_BUFFER_SPACE
    *
    *          then this is a signal that the queueing limits have been
    *          exceeded for the specified connection.  The caller must
    *          then wait for the
    *
    *             etGATT_Connection_Device_Buffer_Empty
    *
    *          event before sending any un-acknowledged packets to the
    *          specified device.  See the GATT_Set_Queuing_Parameters()
    *          function for more information.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Handle_Value_Notification(unsigned int BluetoothStackID, unsigned int ServiceID, unsigned int ConnectionID, Word_t AttributeOffset, Word_t AttributeValueLength, Byte_t *AttributeValue);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Handle_Value_Notification_t)(unsigned int BluetoothStackID, unsigned int ServiceID, unsigned int ConnectionID, Word_t AttributeOffset, Word_t AttributeValueLength, Byte_t *AttributeValue);
#endif

   /*! @brief The following function is provided to allow a means of verifying a
    * received signed write data event that was received from a remote
    * GATT Client.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param ServiceID Service ID of the service that is sending the notification.
    * @param AttributeOffset Attribute offset of the attribute that is being written.  This value will be greater than or equal to zero (specifies the very first attribute in the service) and less than the maximum number of attributes contained in the service.
    * @param AttributeValueLength Length (in bytes) of the attribute
    * value data that is to be verified/written.
    * @param AttributeValue Buffer that contains the value data to be
    * verified/written.  This buffer must contain (at least) the number of bytes specified by the AttributeValueLength parameter.
    * @param ReceivedSignature Pointer to the GATT/ATT signature that
    * was received in the write request event.
    * @param CSRK Pointer to the connection signature resolving
    * key (CSRK) that is to be used to verify the received signature.
    * @return This function returns TRUE if the received signature
    * matches the signature generated internally with the specified CSRK
    * or FALSE if the signatures DO NOT match.
    */
BTPSAPI_DECLARATION Boolean_t BTPSAPI GATT_Verify_Signature(unsigned int BluetoothStackID, unsigned int ServiceID, Word_t AttributeOffset, unsigned int AttributeValueLength, Byte_t *AttributeValue, ATT_Authentication_Signature_t *ReceivedSignature, Encryption_Key_t *CSRK);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef Boolean_t (BTPSAPI *PFN_GATT_Verify_Signature_t)(unsigned int BluetoothStackID, unsigned int ServiceID, Word_t AttributeOffset, unsigned int AttributeValueLength, Byte_t *AttributeValue, ATT_Authentication_Signature_t *ReceivedSignature, Encryption_Key_t *CSRK);
#endif

   /* GATT Service Changed API.
    * @brief The following function is provided to allow a mechanism of
    * responding to a GATT Client's request to read the Service Changed
    * characteristic value.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param TransactionID Transaction ID of the original Service Changed read
    * request.  This value was received in the
    * etGATT_Connection_Service_Changed_Read_Request event.
    * @param Service_Changed_Data Specifies a pointer to the data to
    * respond to the Service Changed Read Request with.
    * @return This function will return a ZERO return code if successful, or a
    * negative return error code if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Service_Changed_Read_Response(unsigned int BluetoothStackID, unsigned int TransactionID, GATT_Service_Changed_Data_t *Service_Changed_Data);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Service_Changed_Read_Response_t)(unsigned int BluetoothStackID, unsigned int TransactionID, GATT_Service_Changed_Data_t *Service_Changed_Data);
#endif

   /*! @brief The following function is provided to allow a mechanism of
    * responding to a GATT Client's request to read it's Service Changed
    * Client Characteristic Configuration Descriptor (CCCD) value.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param TransactionID Transaction ID of the original Service Changed
    * read request.  This value was received in the
    * etGATT_Connection_Service_Changed_CCCD_Read_Request event.
    * @param CCCD Value of the Client’s CCCD for the Service Changed
    * characteristic to respond to the request with.
    * @return This function will return a ZERO
    * return code if successful, or a negative return error code if
    * there was an error.
    * \note Each client shall have a unique value for this
    *          descriptor.
    * \note It is the application's responsibility to store this
    *          persistently for each bonded device.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Service_Changed_CCCD_Read_Response(unsigned int BluetoothStackID, unsigned int TransactionID, Word_t CCCD);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Service_Changed_CCCD_Read_Response_t)(unsigned int BluetoothStackID, unsigned int TransactionID, Word_t CCCD);
#endif

   /*! @brief The following function is provided to allow a mechanism of
    * indicating to a specified GATT Client the Service Changed
    * characteristic value.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param ConnectionID Connection ID of the currently connected remote
    * client device to send the Service Changed indication.
    * @param Service_Changed_Data Specifies a pointer to the Service Changed
    * data to indicate.
    * @return This function will return the positive, non-zero, Transaction ID of the Handle
    * Value Indication or a negative error code.
    * \note If successful the return value will contain the
    *          Transaction ID that can be used to cancel the Indication.
    * \note A etGATT_Connection_Service_Changed_Confirmation event
    *          will be dispatched to the registered GATT Connection
    *          Callbacks when the indication is acknowledged by the
    *          remote device.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Service_Changed_Indication(unsigned int BluetoothStackID, unsigned int ConnectionID, GATT_Service_Changed_Data_t *Service_Changed_Data);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Service_Changed_Indication_t)(unsigned int BluetoothStackID, unsigned int ConnectionID, GATT_Service_Changed_Data_t *Service_Changed_Data);
#endif

   /* GATT Client API.
    * @brief The following function is provided to allow a means of requesting
    * a change of the Connection MTU used with a remote GATT server.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param ConnectionID Connection ID of the currently connected remote GATT
    * server device.
    * @param RequestedMTU The MTU to request from the remote, connected, LE device.
    * This value must be between the following values:
    * ATT_PROTOCOL_MTU_MINIMUM_LE + GATT_MAXIMUM_SUPPORTED_STACK_MTU
    * @param ClientEventCallback Callback function that is registered to receive
    * the exchange MTU response event.
    * @param CallbackParameter   A user-defined parameter (e.g., a tag value)
    * that will be passed back to the user in the callback function.
    * @return This function will return the positive, non-zero, Transaction ID of the request or a
    * negative error code.
    * \note The etGATT_Client_Exchange_MTU_Response event will signal
    *          when a response is received from the remote GATT server.
    *          The new MTU will be the smaller of the RequestedMTU
    *          specified in this function, and the MTU received in the
    *          etGATT_Client_Exchange_MTU_Response event.
    * \note If successful the return value will contain the
    *          Transaction ID that can be used to cancel the request.
    * \note This function is only valid for LE connections.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Exchange_MTU_Request(unsigned int BluetoothStackID, unsigned int ConnectionID, Word_t RequestedMTU, GATT_Client_Event_Callback_t ClientEventCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Exchange_MTU_Request_t)(unsigned int BluetoothStackID, unsigned int ConnectionID, Word_t RequestedMTU, GATT_Client_Event_Callback_t ClientEventCallback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to allow a means of discovering
    * primary services on a remote GATT database server.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param ConnectionID Connection ID of the currently connected remote GATT
    * server device.
    * @param StartingHandle Starting attribute handle to use to begin the search range.  This value must be between:
    * ATT_PROTOCOL_HANDLE_MINIMUM_VALUE and ATT_PROTOCOL_HANDLE_MAXIMUM_VALUE
    * @param EndingHandle Ending attribute handle to use to end the search range.
    *  This value must be between (and at least one value larger than the starting attribute handle):
    * ATT_PROTOCOL_HANDLE_MINIMUM_VALUE and ATT_PROTOCOL_HANDLE_MAXIMUM_VALUE
    * @param ClientEventCallback Callback function that is registered to receive
    * the discover services response event.
    * @param CallbackParameter A user-defined parameter (e.g., a tag value) that
    * will be passed back to the user in the callback function.
    * @return This function will return the positive,
    * non-zero, Transaction ID of the request or a negative error code.
    * \note To discover all services on a remote GATT database the
    *          first call to this function should have the starting
    *          handle set to:
    *             ATT_PROTOCOL_HANDLE_MINIMUM_VALUE
    *          and the ending handle set to:
    *             ATT_PROTOCOL_HANDLE_MAXIMUM_VALUE
    * \note The etGATT_Client_Service_Discovery_Response event will
    *          signal when the service discovery event started by the
    *          call to this function has completed.  To discover all
    *          services this function should be called again with the
    *          starting handle set to one greater than the last end
    *          group handle returned in the
    *          GATT_Service_Information_List of the
    *          etGATT_Client_Service_Discovery_Response Event.
    * \note If successful the return value will contain the
    *          Transaction ID that can be used to cancel the request.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Discover_Services(unsigned int BluetoothStackID, unsigned int ConnectionID, Word_t StartingHandle, Word_t EndingHandle, GATT_Client_Event_Callback_t ClientEventCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Discover_Services_t)(unsigned int BluetoothStackID, unsigned int ConnectionID, Word_t StartingHandle, Word_t EndingHandle, GATT_Client_Event_Callback_t ClientEventCallback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to allow a means of discovering
    * primary services with a specified UUID on a Remote GATT database
    * server.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param ConnectionID Connection ID of the currently connected remote GATT
    * server device.
    * @param StartingHandle Starting attribute handle to use to begin the search range.  This value must be between:
    * ATT_PROTOCOL_HANDLE_MINIMUM_VALUE and ATT_PROTOCOL_HANDLE_MAXIMUM_VALUE
    * @param EndingHandle Ending attribute handle to use to end the search range.
    * This value must be between (and at least one value larger than the starting
    * attribute handle):
    * ATT_PROTOCOL_HANDLE_MINIMUM_VALUE and ATT_PROTOCOL_HANDLE_MAXIMUM_VALUE
    * @param UUID    Contains the service UUID to search for.  This structure has
    * the following format:
    * typedef struct
    * {
    *    GATT_UUID_Type_t UUID_Type;
    *    union
    *    {
    *       UUID_6_t          UUID_6;
    *       UUID_28_t         UUID_28;
    *    } UUID;
    * } GATT_UUID_t;
    * where, UUID_Type is defined to be one of the following: guUUID_6 or guUUID_28.
    * @param ClientEventCallback Callback function that is registered to receive the discover services by UUID response event.
    * @param CallbackParameter   A user-defined parameter (e.g., a tag value) that will be passed back to the user in the callback function.
    * @return This function will return the positive, non-zero, Transaction ID of the request
    * or a negative error code.
    * \note To discover all services on a remote GATT database with
    *          the specified UUID the first call to this function should
    *          have the starting handle set to:
    *             ATT_PROTOCOL_HANDLE_MINIMUM_VALUE
    *          and the ending handle set to:
    *             ATT_PROTOCOL_HANDLE_MAXIMUM_VALUE
    * \note The etGATT_Client_Service_Discovery_By_UUID_Response
    *          event will signal when the service discovery event
    *          started by the call to this function has completed.  To
    *          discover all services of the specified UUID this function
    *          should be called again with the starting handle set to
    *          one greater than the last end group handle returned in
    *          the GATT_Service_Information_List of the
    *          etGATT_Client_Service_Discovery_By_UUID_Response Event.
    * \note If successful the return value will contain the
    *          Transaction ID that can be used to cancel the request.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Discover_Services_By_UUID(unsigned int BluetoothStackID, unsigned int ConnectionID, Word_t StartingHandle, Word_t EndingHandle, GATT_UUID_t *UUID, GATT_Client_Event_Callback_t ClientEventCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Discover_Services_By_UUID_t)(unsigned int BluetoothStackID, unsigned int ConnectionID, Word_t StartingHandle, Word_t EndingHandle, GATT_UUID_t *UUID, GATT_Client_Event_Callback_t ClientEventCallback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to allow a means of discovering
    * the included services on a remote GATT database server for a
    * specified service.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param ConnectionID Connection ID of the currently connected remote
    * GATT server device.
    * @param ServiceStartingHandle Starting service attribute handle to use to
    * begin the search range.  This value must be between:
    * ATT_PROTOCOL_HANDLE_MINIMUM_VALUE and ATT_PROTOCOL_HANDLE_MAXIMUM_VALUE
    * @param ServiceEndingHandle Ending service attribute handle to use to
    * end the search range.  This value must be between (and at least one value larger
    * than the starting attribute handle):
    * ATT_PROTOCOL_HANDLE_MINIMUM_VALUE and ATT_PROTOCOL_HANDLE_MAXIMUM_VALUE
    * @param ClientEventCallback Callback function that is registered
    * to receive the included services response event.
    * @param CallbackParameter A user-defined parameter (e.g., a tag value)
    * that will be passed back to the user in the callback function.
    * @return This function will return the positive, non-zero, Transaction ID of the request or a
    * negative error code.
    * \note To discover all included services on a remote GATT
    *          database with the specified UUID the first call to this
    *          function should have the starting handle set to:
    *             ATT_PROTOCOL_HANDLE_MINIMUM_VALUE
    *          and the ending handle set to:
    *             ATT_PROTOCOL_HANDLE_MAXIMUM_VALUE
    * \note The etGATT_Client_Included_Services_Discovery_Response
    *          event will signal when the service discovery event
    *          started by the call to this function has completed.  To
    *          discover all services of the specified UUID this function
    *          should be called again with the starting handle set to
    *          one greater than the last end group handle returned in
    *          the GATT_Service_Information_List of the
    *          etGATT_Client_Included_Services_Discovery_Response Event.
    * \note If successful the return value will contain the
    *          Transaction ID that can be used to cancel the request.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Discover_Included_Services(unsigned int BluetoothStackID, unsigned int ConnectionID, Word_t ServiceStartingHandle, Word_t ServiceEndingHandle, GATT_Client_Event_Callback_t ClientEventCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Discover_Included_Services_t)(unsigned int BluetoothStackID, unsigned int ConnectionID, Word_t ServiceStartingHandle, Word_t ServiceEndingHandle, GATT_Client_Event_Callback_t ClientEventCallback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to allow a means of discovering
    * characteristics on a remote GATT database server for a specified
    * service.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param ConnectionID Connection ID of the currently connected remote
    * GATT server device.
    * @param ServiceStartingHandle Starting service attribute handle
    * to use to begin the search range. This value must be between:
    * ATT_PROTOCOL_HANDLE_MINIMUM_VALUE and ATT_PROTOCOL_HANDLE_MAXIMUM_VALUE
    * @param ServiceEndingHandle Ending service attribute handle to use to end
    * the search range.  This value must be between (and at least one value
    * larger than the starting attribute handle):
    * ATT_PROTOCOL_HANDLE_MINIMUM_VALUE and ATT_PROTOCOL_HANDLE_MAXIMUM_VALUE
    * @param ClientEventCallback Callback function that is registered to
    * receive the included characteristics response event.
    * @param CallbackParameter A user-defined parameter (e.g., a tag value)
    * that will be passed back to the user in the callback function.
    * @return This function will return the positive, non-zero, Transaction
    * ID of the request or a negative error code.
    * \note To discover all characteristics on a remote GATT database
    *          server for a specified service, the first call to this
    *          function should have the starting handle and the ending
    *          handles set to the starting and ending handles of the
    *          service to be searched.
    * \note The etGATT_Client_Characteristic_Discovery_Response event
    *          will signal that this function may be called again with
    *          the starting handle set to one greater than the last
    *          characteristic declaration handle specified in the
    *          GATT_Characteristic_Discovery_Entry_List list.
    * \note If successful the return value will contain the
    *          Transaction ID that can be used to cancel the request.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Discover_Characteristics(unsigned int BluetoothStackID, unsigned int ConnectionID, Word_t ServiceStartingHandle, Word_t ServiceEndingHandle, GATT_Client_Event_Callback_t ClientEventCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Discover_Characteristics_t)(unsigned int BluetoothStackID, unsigned int ConnectionID, Word_t ServiceStartingHandle, Word_t ServiceEndingHandle, GATT_Client_Event_Callback_t ClientEventCallback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to allow a means of discovering
    * characteristic descriptors on a remote GATT database server for a
    * specified Characteristic.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param ConnectionID Connection ID of the currently connected remote
    * GATT server device.
    * @param CharacteristicStartingHandle Starting characteristic
    * attribute handle to use to begin the search range.
    * This value must be between:
    * ATT_PROTOCOL_HANDLE_MINIMUM_VALUE and ATT_PROTOCOL_HANDLE_MAXIMUM_VALUE.
    * @param CharacteristicEndingHandle  Ending characteristic attribute
    * handle to use to end the search range.  This value must be between
    * (and at least one value larger than the starting attribute handle):
    * @param ClientEventCallback Callback function that is registered to receive the included characteristic descriptor response event.
    * @param CallbackParameter A user-defined parameter (e.g., a tag value)
    * that will be passed back to the user in the callback function.
    * @return This function will return the positive, non-zero, Transaction ID
    * of the request or a negative error code.
    * \note To discover all characteristic descriptors on a remote
    *          GATT database server, the first call to this function
    *          should have the starting and ending handles set to the
    *          starting and ending handles of the service that is being
    *          searched.
    * \note The
    *          etGATT_Client_Characteristic_Descriptor_...
    *             ...Discovery_Response
    *          event will signal that this function may be called again
    *          with the characteristic starting handle set to one
    *          greater than the ending handle in the
    *          GATT_Characteristic_Descriptor_Discovery_Entry_List list.
    * \note If successful the return value will contain the
    *          Transaction ID that can be used to cancel the request.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Discover_Characteristic_Descriptors(unsigned int BluetoothStackID, unsigned int ConnectionID, Word_t CharacteristicStartingHandle, Word_t CharacteristicEndingHandle, GATT_Client_Event_Callback_t ClientEventCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Discover_Characteristic_Descriptors_t)(unsigned int BluetoothStackID, unsigned int ConnectionID, Word_t CharacteristicStartingHandle, Word_t CharacteristicEndingHandle, GATT_Client_Event_Callback_t ClientEventCallback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to allow a means of performing
    * a read request on a remote device for a specific attribute value.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param ConnectionID Connection ID of the currently connected remote GATT server
    * device.
    * @param AttributeHandle Attribute handle of the attribute that is to be read.
    * This value must be between:
    * ATT_PROTOCOL_HANDLE_MINIMUM_VALUE and ATT_PROTOCOL_HANDLE_MAXIMUM_VALUE
    * @param ClientEventCallback Callback function that is registered to receive
    * the included read value response event.
    * @param CallbackParameter A user-defined parameter (e.g., a tag value)
    * that will be passed back to the user in the callback function.
    * @return This function will return the positive, non-zero, Transaction ID
    * of the request or a negative error code.
    * \note If successful the return value will contain the
    *          Transaction ID that can be used to cancel the request.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Read_Value_Request(unsigned int BluetoothStackID, unsigned int ConnectionID, Word_t AttributeHandle, GATT_Client_Event_Callback_t ClientEventCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Read_Value_Request_t)(unsigned int BluetoothStackID, unsigned int ConnectionID, Word_t AttributeHandle, GATT_Client_Event_Callback_t ClientEventCallback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to allow a means of performing
    * a read long request on a remote device for a specific attribute
    * value.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param ConnectionID Connection ID of the currently connected remote
    * GATT server device.
    * @param AttributeHandle Attribute handle of the attribute that is to be read.
    * This value must be between:ATT_PROTOCOL_HANDLE_MINIMUM_VALUE and ATT_PROTOCOL_HANDLE_MAXIMUM_VALUE
    * @param AttributeOffset Starting offset (in bytes) of the attribute
    * value data to read.
    * @param ClientEventCallback Callback function that is registered to
    * receive the included read value response event.
    * @param CallbackParameter A user-defined parameter (e.g., a tag value)
    * that will be passed back to the user in the callback function.
    * @return This function will return the positive, non-zero, Transaction ID of the request
    * or a negative error code.
    * \note The etGATT_Client_Read_Long_Response event indicates that
    *          a read long request issued by this function has
    *          completed.  If the event is successful then the attribute
    *          offset should be increased by attribute value length that
    *          is returned in the response.  The next call to the
    *          GATT_Read_Long_Value_Request() function should
    *          specifythis new offset.  This can be done until the
    *          entire attribute value has been read.
    * \note If successful the return value will contain the
    *          Transaction ID that can be used to cancel the request.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Read_Long_Value_Request(unsigned int BluetoothStackID, unsigned int ConnectionID, Word_t AttributeHandle, Word_t AttributeOffset, GATT_Client_Event_Callback_t ClientEventCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Read_Long_Value_Request_t)(unsigned int BluetoothStackID, unsigned int ConnectionID, Word_t AttributeHandle, Word_t AttributeOffset, GATT_Client_Event_Callback_t ClientEventCallback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to allow a means of performing
    * a read by UUID request on a remote device for a specific attribute
    * value based on the specified UUID.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param ConnectionID Connection ID of the currently connected
    * remote GATT server device.
    * @param AttributeUUID Contains the attribute UUID to search for.
    * where, UUID_Type is defined to be one of the following: guUUID_6 or guUUID_28.
    * @param ServiceStartHandle Starting service attribute handle
    * to use to begin the search range. This value must be between:
    * ATT_PROTOCOL_HANDLE_MINIMUM_VALUE and ATT_PROTOCOL_HANDLE_MAXIMUM_VALUE
    * @param ServiceEndHandle Ending service attribute handle to use to
    * end the search range.
    * This value must be between (and at least one value larger than the starting
    * attribute handle):
    * ATT_PROTOCOL_HANDLE_MINIMUM_VALUE and ATT_PROTOCOL_HANDLE_MAXIMUM_VALUE
    * @param ClientEventCallback Callback function that is registered to
    * receive the included read attribute value by UUID response event.
    * @param CallbackParameter A user-defined parameter (e.g., a tag value)
    * that will be passed back to the user in the callback function.
    * @return This function will return the positive,
    * non-zero, Transaction ID of the request or a negative error code.
    * \note If successful the return value will contain the
    *          Transaction ID that can be used to cancel the request.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Read_Value_By_UUID_Request(unsigned int BluetoothStackID, unsigned int ConnectionID, GATT_UUID_t *AttributeUUID, Word_t ServiceStartHandle, Word_t ServiceEndHandle, GATT_Client_Event_Callback_t ClientEventCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Read_Value_By_UUID_Request_t)(unsigned int BluetoothStackID, unsigned int ConnectionID, GATT_UUID_t *AttributeUUID, Word_t ServiceStartHandle, Word_t ServiceEndHandle, GATT_Client_Event_Callback_t ClientEventCallback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to allow a means of performing
    * a read multiple request on a remote device for multiple
    * attributes.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param ConnectionID Connection ID of the currently connected
    * remote GATT server device.
    * @param NumberOfHandles Specifies the total number of attribute
    * handle entries that are contained in the AttributeHandleList parameter.
    * @param AttributeHandleList Pointer to an array of attribute
    * handle entries that specify all of the attributes that should be read.
    * Each member in this array must be between:
    * ATT_PROTOCOL_HANDLE_MINIMUM_VALUE and ATT_PROTOCOL_HANDLE_MAXIMUM_VALUE.
    * @param ClientEventCallback Callback function that is registered
    * to receive the included read multiple values response event.
    * @param CallbackParameter   A user-defined parameter (e.g., a tag value)
    * that will be passed back to the user in the callback function.
    * @return This function will return the positive, non-zero, Transaction ID of the request
    * or a negative error code.
    * \note If successful the return value will contain the
    *          Transaction ID that can be used to cancel the request.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Read_Multiple_Values_Request(unsigned int BluetoothStackID, unsigned int ConnectionID, Word_t NumberOfHandles, Word_t *AttributeHandleList, GATT_Client_Event_Callback_t ClientEventCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Read_Multiple_Values_Request_t)(unsigned int BluetoothStackID, unsigned int ConnectionID, Word_t NumberOfHandles, Word_t *AttributeHandleList, GATT_Client_Event_Callback_t ClientEventCallback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to allow a means of performing
    * a write request to a remote device for a specified attribute.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param ConnectionID Connection ID of the currently connected
    * remote GATT server device.
    * @param AttributeHandle Attribute handle of the attribute to write.
    * This value must be between:
    * ATT_PROTOCOL_HANDLE_MINIMUM_VALUE and ATT_PROTOCOL_HANDLE_MAXIMUM_VALUE.
    * @param AttributeLength Length (in bytes) of the actual
    * attribute value data to write to the specified attribute.
    * @param AttributeValue  Buffer that contains (at least)
    * as many bytes a specified by the AttributeLength parameter.
    * @param ClientEventCallback Callback function that
    * is registered to receive the included the write value response event.
    * @param CallbackParameter A user-defined parameter
    * (e.g., a tag value) that will be passed back to the user in the callback function.
    * @return This function will return the positive, non-zero, Transaction ID of the request or a
    * negative error code.
    * \note This function will not write an attribute value with a
    *          length greater than the current MTU - 3.  To write a
    *          longer attribute value use the
    *          GATT_Prepare_Write_Request() function instead.
    * \note This function may write less than the number of requested
    *          bytes.  This can happen if the number of bytes to write
    *          is less than what can fit in the MTU for the specified
    *          connection.  The data in the
    *          etGATT_Client_Write_Response, that is dispatched if the
    *          remote device accepts the request, indicates the number
    *          of bytes that were written to the remote device.
    * \note If successful the return value will contain the
    *          Transaction ID that can be used to cancel the request.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Write_Request(unsigned int BluetoothStackID, unsigned int ConnectionID, Word_t AttributeHandle, Word_t AttributeLength, void *AttributeValue, GATT_Client_Event_Callback_t ClientEventCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Write_Request_t)(unsigned int BluetoothStackID, unsigned int ConnectionID, Word_t AttributeHandle, Word_t AttributeLength, void *AttributeValue, GATT_Client_Event_Callback_t ClientEventCallback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to allow a means of performing
    * a write without response request to remote device for a specified
    * attribute.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param ConnectionID Connection ID of the currently connected remote
    * GATT server device.
    * @param AttributeHandle Attribute handle of the attribute to write.
    * This value must be between:
    * ATT_PROTOCOL_HANDLE_MINIMUM_VALUE and ATT_PROTOCOL_HANDLE_MAXIMUM_VALUE.
    * @param AttributeLength Length (in bytes) of the actual attribute
    * value data to write to the specified attribute.
    * @param AttributeValue Buffer that contains (at least)
    * as many bytes a specified by the AttributeLength parameter.
    * @return This function will return the number of bytes written on success or a
    * negative error code.
    * \note No response is issued by the remote GATT server to this
    *          command.  Therefore there is no guarantee of whether the
    *          remote GATT server actually performed the write or
    *          rejected it due to some error condition.
    * \note If @return This function returns the Error Code:
    *
    *        BTPS_ERROR_INSUFFICIENT_BUFFER_SPACE
    *        then this is a signal that the queueing limits have been
    *        exceeded for the specified connection.  The caller must
    *        then wait for the
    *        etGATT_Connection_Device_Buffer_Empty
    *        event before sending any un-acknowledged packets to the
    *        specified device.  See the GATT_Set_Queuing_Parameters()
    *        function for more information.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Write_Without_Response_Request(unsigned int BluetoothStackID, unsigned int ConnectionID, Word_t AttributeHandle, Word_t AttributeLength, void *AttributeValue);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Write_Without_Response_Request_t)(unsigned int BluetoothStackID, unsigned int ConnectionID, Word_t AttributeHandle, Word_t AttributeLength, void *AttributeValue);
#endif

   /*! @brief The following function is provided to allow a means of performing
    * a signed write without response request to remote device for a
    * specified attribute.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param ConnectionID Connection ID of the currently connected remote
    * GATT server device.
    * @param AttributeHandle Attribute handle of the attribute to write.
    * This value must be between:
    * ATT_PROTOCOL_HANDLE_MINIMUM_VALUE and ATT_PROTOCOL_HANDLE_MAXIMUM_VALUE.
    * @param SignCounter The Sign Counter.
    * @param AttributeLength Length (in bytes) of the actual
    * attribute value data to write to the specified attribute.
    * @param AttributeValue Buffer that contains (at least) as many
    * bytes a specified by the AttributeLength parameter.
    * @param CSRK Pointer to the connection signature resolving
    * key (CSRK) that will be used to sign the data that is to be sent.
    * @return This function will return the number of bytes written on success or a
    * negative error code.
    * \note No response is issued by the remote GATT server to this
    *          command.  Therefore there is no guarantee of whether the
    *          remote GATT server actually performed the write or
    *          rejected it due to some error condition.
    * \note If @return This function returns the Error Code:
    *
    *             BTPS_ERROR_INSUFFICIENT_BUFFER_SPACE
    *
    *          then this is a signal that the queueing limits have been
    *          exceeded for the specified connection.  The caller must
    *          then wait for the
    *
    *             etGATT_Connection_Device_Buffer_Empty
    *
    *          event before sending any un-acknowledged packets to the
    *          specified device.  See the GATT_Set_Queuing_Parameters()
    *          function for more information.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Signed_Write_Without_Response(unsigned int BluetoothStackID, unsigned int ConnectionID, Word_t AttributeHandle, DWord_t SignCounter, Word_t AttributeLength, void *AttributeValue, Encryption_Key_t *CSRK);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Signed_Write_Without_Response_t)(unsigned int BluetoothStackID, unsigned int ConnectionID, Word_t AttributeHandle, DWord_t SignCounter, Word_t AttributeLength, void *AttributeValue, Encryption_Key_t *CSRK);
#endif

   /*! @brief The following function is provided to allow a means of performing
    * a prepare write request to remote device for a specific attribute.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param ConnectionID Connection ID of the currently connected remote
    * GATT server device.
    * @param AttributeHandle Attribute handle of the attribute to write.
    * This value must be between:
    * ATT_PROTOCOL_HANDLE_MINIMUM_VALUE
    * ATT_PROTOCOL_HANDLE_MAXIMUM_VALUE
    * @param AttributeLength Total length (in bytes) of the actual
    * attribute value data to write to the specified attribute.
    * @param AttributeValueOffset Offset (in bytes) of the attribute value to write.
    * This value must be smaller than the AttributeLength parameter.
    * @param AttributeValue Buffer that contains (at least) as many
    * bytes a specified by  the AttributeLength parameter minus the
    * AttributeValueOffset parameter.
    * @param ClientEventCallback Callback function that is registered
    * to receive the included the prepare write value response event.
    * @param CallbackParameter A user-defined parameter (e.g., a tag value)
    * that will be passed back to the user in the callback function.
    * @return This function will return the positive, non-zero, Transaction ID of
    * the request or a
    * negative error code.
    * \note This function can be used to write a long attribute value
    *          as well as to perform a reliable write to a regular
    *          attribute.
    * \note The etGATT_Client_Prepare_Write_Response event will
    *          signal to the installed client event callback that this
    *          function may be called again with the offset increased by
    *          the amount returned in the event.
    * \note Upon receiving the etGATT_Client_Prepare_Write_Response
    *          event, once the entire attribute has been sent to the
    *          remote device through calling the
    *          GATT_Prepare_Write_Request() with the offset increased,
    *          the write may actually be executed via a call to the
    *          GATT_Execute_Write_Request() function.
    * \note This function may write less than the number of requested
    *          bytes.  This can happen if the number of bytes to write
    *          is less than what can fit in the MTU for the specified
    *          connection.  The data in the
    *          etGATT_Client_Prepare_Write_Response, that is dispatched
    *          if the remote device accepts the request, indicates the
    *          number of bytes that were written to the remote device.
    * \note If successful the return value will contain the
    *          Transaction ID that can be used to cancel the request.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Prepare_Write_Request(unsigned int BluetoothStackID, unsigned int ConnectionID, Word_t AttributeHandle, Word_t AttributeLength, Word_t AttributeValueOffset, void *AttributeValue, GATT_Client_Event_Callback_t ClientEventCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Prepare_Write_Request_t)(unsigned int BluetoothStackID, unsigned int ConnectionID, Word_t AttributeHandle, Word_t AttributeLength, Word_t AttributeValueOffset, void *AttributeValue, GATT_Client_Event_Callback_t ClientEventCallback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to allow a means of performing
    * an execute write request to remote device for a specified
    * attribute.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param ConnectionID Connection ID of the currently connected remote GATT
    * server device.
    * @param CancelWrite Boolean flag that specifies whether or not to
    * cancel (TRUE) the prepared write requests, or to commit/execute the
    * prepared write requests (FALSE).
    * @param ClientEventCallback Callback function that is registered to
    * receive the included the prepare write value response event.
    * @param CallbackParameter A user-defined parameter (e.g., a tag value)
    * that will be passed back to the user in the callback function.
    * @return This function will return the positive, non-zero, Transaction ID of the request or a
    * negative error code.
    * \note If the Write parameter is FALSE then all prepared writes
    *          set up through prior calls to
    *          GATT_Prepare_Write_Request() function which have not been
    *          executed with calls to the GATT_Execute_Write_Request()
    *          function will be canceled.
    * \note If successful the return value will contain the
    *          Transaction ID that can be used to cancel the request.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Execute_Write_Request(unsigned int BluetoothStackID, unsigned int ConnectionID, Boolean_t CancelWrite, GATT_Client_Event_Callback_t ClientEventCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Execute_Write_Request_t)(unsigned int BluetoothStackID, unsigned int ConnectionID, Boolean_t CancelWrite, GATT_Client_Event_Callback_t ClientEventCallback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to allow a means to send a
    * Handle/Value confirmation to a remote GATT database server that
    * has sent a Handle/Value indication.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param ConnectionID Connection ID of the currently connected remote GATT
    * server device.
    * @param TransactionID Transaction ID of the received handle/value
    * indication that is being acknowledged.
    * @return This function will return zero if successful, or a negative return error code if
    * there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Handle_Value_Confirmation(unsigned int BluetoothStackID, unsigned int ConnectionID, unsigned int TransactionID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Handle_Value_Confirmation_t)(unsigned int BluetoothStackID, unsigned int ConnectionID, unsigned int TransactionID);
#endif

    /*! @brief The following function is used to initiate a GATT Service
    * Discovery process to the specified GATT connection.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param ConnectionID Connection ID of the currently connected
    * remote GATT server device to perform service discovery on.
    * @param NumberOfUUID Option parameter that, if specified,
    * contains the number of UUIDs that are contained in the UUIDList parameter.
    * @param UUIDList Optional list of Service UUIDs to attempt to
    * discover on the specified remote device.
    * @param ServiceDiscoveryCallback Callback function that will
    * be called with the result of the service discovery operation.
    * @param CallbackParameter Callback parameter that will be passed
    * to the specified ServiceDiscoveryCallback when called with the
    * result of the service discovery operation.
    * @return The function returns zero on success and a negative return value if
    * there was an error.
    * \note The NumberOfUUID and UUIDList are optional.  If they are
    *          specified then they specify the list of services that
    *          should be searched for.  Only services with UUIDs that
    *          are in this list will be discovered.
    * \note Only 1 service discovery operation can be outstanding at
    *          a time.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Start_Service_Discovery(unsigned int BluetoothStackID, unsigned int ConnectionID, unsigned int NumberOfUUID, GATT_UUID_t *UUIDList, GATT_Service_Discovery_Event_Callback_t ServiceDiscoveryCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_DEBUG_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Start_Service_Discovery_t)(unsigned int BluetoothStackID, unsigned int ConnectionID, unsigned int NumberOfUUID, GATT_UUID_t *UUIDList, GATT_Service_Discovery_Event_Callback_t ServiceDiscoveryCallback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is used to initiate a GATT Service
    * Discovery process to the specified GATT connection.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param ConnectionID Connection ID of the currently connected
    * remote GATT server device to perform service discovery on.
    * @param DiscoveryHandleRange Handle range of the GATT database
    * on the remote device to perform the discovery procedure on.
    * @param NumberOfUUID Option parameter that, if specified,
    * contains the number of UUIDs that are contained in the UUIDList parameter.
    * @param UUIDList Optional list of Service UUIDs to attempt
    * to discover on the specified remote device.
    * @param ServiceDiscoveryCallback Callback function that
    * will be called with the result of the service discovery operation.
    * @param CallbackParameter Callback parameter that will
    * be passed to the specified ServiceDiscoveryCallback when called
    * with the result of the service discovery operation.
    * @return The function returns zero on success and a
    * negative return value if there was an error.
    * \note The DiscoveryHandleRange range parameter is optional.
    *          However if it specified the handle range must be valid
    *          (i.e.  Start Handle is less than or equal to End Handle).
    * \note The NumberOfUUID and UUIDList are optional.  If they are
    *          specified then they specify the list of services that
    *          should be searched for.  Only services with UUIDs that
    *          are in this list will be discovered.
    * \note Only 1 service discovery operation can be outstanding at
    *          a time.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Start_Service_Discovery_Handle_Range(unsigned int BluetoothStackID, unsigned int ConnectionID, GATT_Attribute_Handle_Group_t *DiscoveryHandleRange, unsigned int NumberOfUUID, GATT_UUID_t *UUIDList, GATT_Service_Discovery_Event_Callback_t ServiceDiscoveryCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_DEBUG_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Start_Service_Discovery_Handle_Range_t)(unsigned int BluetoothStackID, unsigned int ConnectionID, GATT_Attribute_Handle_Group_t *DiscoveryHandleRange, unsigned int NumberOfUUID, GATT_UUID_t *UUIDList, GATT_Service_Discovery_Event_Callback_t ServiceDiscoveryCallback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is used to terminate the GATT Service
    * Discovery process.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param ConnectionID Connection ID of the currently connected
    * remote GATT server device that has a service discovery operation outstanding.
    * @return The function returns a negative return value if
    * there was an error and zero in success.
    * \note This function will cancel any service discovery
    *          operations that are currently in progress and release all
    *          request information in the queue that are waiting to be
    *          executed.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Stop_Service_Discovery(unsigned int BluetoothStackID, unsigned int ConnectionID);

#ifdef INCLUDE_DEBUG_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Stop_Service_Discovery_t)(unsigned int BluetoothStackID, unsigned int ConnectionID);
#endif

   /* GATT shared Server/Client API.
    * @brief The following function is responsible for canceling an outstanding
    * transaction.  This is used to cancel packets that are sent from
    * the local device to a remote device.  This will not cancel a
    * packet that was received from a remote device.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param TransactionID Transaction ID of the transaction that is to be cancelled.
    * @return This function will return
    * zero if successful, or a negative return error code if there was
    * an error.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Cancel_Transaction(unsigned int BluetoothStackID, unsigned int TransactionID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Cancel_Transaction_t)(unsigned int BluetoothStackID, unsigned int TransactionID);
#endif

   /*! @brief The following function is responsible for querying the current
    * maximum support GATT MTU (Maximum Transmission Unit) of the local
    * GATT layer.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param MTU Pointer to return the maximum supported MTU.
    * @return This function will return zero if successful, or a negative
    * return error code if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Query_Maximum_Supported_MTU(unsigned int BluetoothStackID, Word_t *MTU);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Query_Maximum_Supported_MTU_t)(unsigned int BluetoothStackID, Word_t *MTU);
#endif

   /*! @brief The following function is responsible for changing the current
    * maximum support GATT MTU (Maximum Transmission Unit) of the local
    * device.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param MTU MTU to configure as the maximum supported for the GATT layer.
    * @return This function will return zero if successful, or a
    * negative return error code if there was an error.
    * \note The MTU value passed to this function must follow the
    *          following formula:
    *
    *    ATT_PROTOCOL_MTU_MINIMUM_LE <= MTU <= ATT_PROTOCOL_MTU_MAXIMUM
    *
    * \note This function can only be used when there are no active
    *          GATT connections.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Change_Maximum_Supported_MTU(unsigned int BluetoothStackID, Word_t MTU);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Change_Maximum_Supported_MTU_t)(unsigned int BluetoothStackID, Word_t MTU);
#endif

   /*! @brief The following function is responsible for querying the current MTU
    * of the specified connection.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param ConnectionID Connection ID of the connection to query the MTU for.
    * @param MTU Pointer to return the MTU for the connection.
    * @return This function will return zero if
    * successful, or a negative return error code if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Query_Connection_MTU(unsigned int BluetoothStackID, unsigned int ConnectionID, Word_t *MTU);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Query_Connection_MTU_t)(unsigned int BluetoothStackID, unsigned int ConnectionID, Word_t *MTU);
#endif

   /*! @brief The following function is responsible for querying the Connection
    * of the specified connection.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param ConnectionType Identifies the type of connection to query
    * the Connection ID.  This value must be one of the following:  gctLE or gctBR_EDR
    * @param BD_ADDR Specifies the address of the remote Bluetooth device to query
    * the Connection ID for.
    * @param ConnectionID    Pointer to return the Connection ID for the
    * connection if successful.
    * @return This function will return zero if successful, or a
    * negative return error code if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Query_Connection_ID(unsigned int BluetoothStackID, GATT_Connection_Type_t ConnectionType, BD_ADDR_t BD_ADDR, unsigned int *ConnectionID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Query_Connection_ID_t)(unsigned int BluetoothStackID, GATT_Connection_Type_t ConnectionType, BD_ADDR_t BD_ADDR, unsigned int *ConnectionID);
#endif

   /*! @brief The following function is responsible for querying the Attribute
    * Protocol Opcode of the specified Transaction.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param TransactionID Transaction ID of the transaction to query the
    * Attribute Protocol Opcode for.
    * @param Opcode Pointer to return the Opcode for the specified transaction.
    * @return This function will return zero
    * if successful, or a negative return error code if there was an
    * error.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Query_Transaction_Opcode(unsigned int BluetoothStackID, unsigned int TransactionID, Byte_t *Opcode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Query_Transaction_Opcode_t)(unsigned int BluetoothStackID, unsigned int TransactionID, Byte_t *Opcode);
#endif

   /*! @brief The following function is responsible for setting the lower level
    * queuing parameters.  These parameters are used to control aspects
    * of the amount of data packets that can be queued into the lower
    * level (per individual device connection).  This mechanism allows
    * for the flexibility to limit the amount of RAM that is used for
    * streaming type applications that are using un-acknowledged
    * transactions.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param MaximumNumberDataPackets Maximum number of un-acknowledged
    * packets that may be queued internally.
    * @param QueuedDataPacketsThreshold The lower threshold limit that the
    * lower layer should call back to signify that it can queue more data packets for transmission.
    * @param DiscardOldest Boolean that specifies if the oldest packets
    * should be discarded when a buffer full condition occurs (if TRUE).
    * If FALSE no packets will be discarded when the buffer is full.
    * This can be useful to isochronous-like applications.
    * @return This function returns zero
    * if successful or a negative return error code if there is an
    * error.
    * \note Setting both the MaximumNumberDataPackets and
    *          QueuedDataPacketsThreshold parameters to zero will
    *          disable the queuing mechanism.  This means that the
    *          amount of queued packets will only be limited by the
    *          amount of available RAM.
    * \note Only un-acknowledged transactions are affected by the
    *          queueing.  Acknowledged GATT transactions are never
    *          affected by this queueing mechanism.  The APIs that this
    *          will affect are the following:
    *             - GATT_Write_Without_Response_Request()
    *             - GATT_Signed_Write_Without_Response()
    *             - GATT_Handle_Value_Notification()
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Set_Queuing_Parameters(unsigned int BluetoothStackID, unsigned int MaximumNumberDataPackets, unsigned int QueuedDataPacketsThreshold, Boolean_t DiscardOldest);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Set_Queuing_Parameters_t)(unsigned int BluetoothStackID, unsigned int MaximumNumberDataPackets, unsigned int QueuedDataPacketsThreshold, Boolean_t DiscardOldest);
#endif

   /*! @brief The following function is responsible for setting the lower level
    * queuing parameters.  These parameters are used to control aspects
    * of the amount of data packets that can be queued into the lower
    * level (per individual connection).  This mechanism allows for the
    * flexibility to limit the amount of RAM that is used for streaming
    * type applications.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param MaximumNumberDataPackets Pointer to return the maximum number
    * of un-acknowledged packets that may be queued internally.
    * @param QueuedDataPacketsThreshold Pointer to return the the lower
    * threshold limit that the lower layer should call back to signify that it can queue more data packets for transmission.
    * @param DiscardOldest Pointer to return the boolean that specifies
    * if the oldest packets should be discarded when a buffer full condition occurs (if TRUE).  If FALSE no packets will be discarded when the buffer is full.  This can be useful to isochronous-like applications.
    * @return This function returns zero if successful
    * or a negative return error code if there is an error.
    * \note See the GATT_Set_Queuing_Parameters() function for more
    *          information.
    */
BTPSAPI_DECLARATION int BTPSAPI GATT_Get_Queuing_Parameters(unsigned int BluetoothStackID, unsigned int *MaximumNumberDataPackets, unsigned int *QueuedDataPacketsThreshold, Boolean_t *DiscardOldest);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GATT_Get_Queuing_Parameters_t)(unsigned int BluetoothStackID, unsigned int *MaximumNumberDataPackets, unsigned int *QueuedDataPacketsThreshold, Boolean_t *DiscardOldest);
#endif

   /*! @brief The following function is responsible for determining if a
    * specified service range is availble for a service to be registered
    * in.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param ServiceHandleGroup Pointer to a structure containing the start and
    * end handle of a region in the GATT database to determine the availability of.
    * @return This function returns TRUE if the specified handle range is
    * available for use or FALSE otherwise.
    */
BTPSAPI_DECLARATION Boolean_t BTPSAPI GATT_Query_Service_Range_Availability(unsigned int BluetoothStackID, GATT_Attribute_Handle_Group_t *ServiceHandleGroup);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef Boolean_t (BTPSAPI *PFN_GATT_Query_Service_Range_Availability_t)(unsigned int BluetoothStackID, GATT_Attribute_Handle_Group_t *ServiceHandleGroup);
#endif

#endif
