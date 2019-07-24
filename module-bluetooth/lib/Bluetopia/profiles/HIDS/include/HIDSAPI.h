/*
 * Copyright 2012 - 2014 Stonestreet One.
 * All Rights Reserved.
 * Author:  Tim Cook
 *** MODIFICATION HISTORY ****************************************************
 *
 *   mm/dd/yy  F. Lastname    Description of Modification
 *   --------  -----------    ------------------------------------------------
 *   10/24/12  T. Cook        Initial Creation (Based on HTS).
 ****************************************************************************
 */

/**
 *  @file HIDSAPI.h
 *
 *  @brief Stonestreet One Bluetooth Human Interface Device Service (GATT
 *      based) API Type Definitions, Constants, and Prototypes.
 *      To use any of the following API's,
 *      include the following declaration in your application.
 *
 *  @code
 *  #include "SS1BTHIDS.h"
 *  @endcode
 *  ============================================================================
 */
#ifndef __HIDSAPIH__
#define __HIDSAPIH__

#include "SS1BTPS.h"      /*! Bluetooth Stack API Prototypes/Constants. */
#include "SS1BTGAT.h"     /*! Bluetooth Stack GATT API Prototypes/Constants. */
#include "HIDSType.h"     /*! Human Information Device Service Types/Constants. */

   /*! Error Return Codes.
    * Error Codes that are smaller than these (less than -1000) are
    * related to the Bluetooth Protocol Stack itself (see BTERRORS.H).
    */
#define HIDS_ERROR_INVALID_PARAMETER                      (-1000)
#define HIDS_ERROR_INVALID_BLUETOOTH_STACK_ID             (-1001)
#define HIDS_ERROR_INSUFFICIENT_RESOURCES                 (-1002)
#define HIDS_ERROR_SERVICE_ALREADY_REGISTERED             (-1003)
#define HIDS_ERROR_INVALID_INSTANCE_ID                    (-1004)
#define HIDS_ERROR_MALFORMATTED_DATA                      (-1005)
#define HIDS_ERROR_INDICATION_OUTSTANDING                 (-1006)
#define HIDS_ERROR_UNKNOWN_ERROR                          (-1007)

   /* The follwoing defines the values of the Flags parameter that is
    * provided in the HIDS_Initialize_Service Function.
    */
#define HIDS_FLAGS_SUPPORT_MOUSE                          0x01
#define HIDS_FLAGS_SUPPORT_KEYBOARD                       0x02
#define HIDS_FLAGS_BOOT_MOUSE_INPUT_REPORT_WRITABLE       0x04
#define HIDS_FLAGS_BOOT_KEYBOARD_INPUT_REPORT_WRITABLE    0x08

   /*! The followng defines the format of a Report Reference.  The first
    * member specifies the Report ID.  The second member is the Report
    * Type.  @param PARAMFINAL The final parameter (which is only valid when the
    * ReportType is set to
    * HIDS_REPORT_REFERENCE_REPORT_TYPE_INPUT_REPORT) specifies if the
    * Set Report procedure is optionally supported by the Input Report.
    * \note The Report Type is an enumerated type that must be in the
    *          format HIDS_REPORT_REFERENCE_REPORT_TYPE_XXX.
    */
typedef struct _tagHIDS_Report_Reference_Data_t
{
   Byte_t ReportID;
   Byte_t ReportType;
} HIDS_Report_Reference_Data_t;

#define HIDS_REPORT_REFERENCE_DATA_SIZE                  (sizeof(HIDS_Report_Reference_Data_t))

   /*! The following define an additional bit mask that may be set in the
    * ReportType member of the HIDS_Report_Reference_Data_t structure.
    * This bit mask is only valid if the ReportType is set to
    * HIDS_REPORT_REFERENCE_REPORT_TYPE_INPUT_REPORT and if set
    * specifies that the Input Report is writable (an optional feature).
    * \note This may ONLY be set when this structure is passed to
    *          HIDS_Initialize_Service().
    */
#define HIDS_REPORT_REFERENCE_SUPPORT_INPUT_WRITE_FEATURE      0x80

   /* The followng defines the format of a HID Information.  The first
    * member specifies the HID Version of the HID Specification
    * implemented by HID Device.  The second member is the Country for
    * which the hardware is localized for.  The last parameter contins
    * bit flags that specify the capabilities of the device.
    * \note The Flags Type is an enumerated type that must be in the
    *          format HIDS_HID_INFORMATION_FLAGS_XXX.
    */
typedef struct _tagHIDS_HID_Information_Data_t
{
   Word_t Version;
   Byte_t CountryCode;
   Byte_t Flags;
} HIDS_HID_Information_Data_t;

#define HIDS_INFORMATION_DATA_SIZE                       (sizeof(HIDS_HID_Information_Data_t))

   /*! The following define the valid Protocol Modes that are allowed by
    * HID.
    */
typedef enum
{
   pmBoot,
   pmReport
} HIDS_Protocol_Mode_t;

   /*! The following define the valid Control Commands that may be sent
    * or received.
    */
typedef enum
{
   pcSuspend,
   pcExitSuspend
} HIDS_Control_Point_Command_t;

   /*! The following define the valid report types that may be dispatched
    * in a Read or Write request to denote the report value that the
    * remote Host is trying to Read/Write.
    */
typedef enum
{
   rtReport,
   rtBootKeyboardInputReport,
   rtBootKeyboardOutputReport,
   rtBootMouseInputReport
} HIDS_Report_Type_t;

   /*! The following enumeration covers all the events generated by the
    * HIDS Profile.  These are used to determine the type of each event
    * generated, and to ensure the proper union element is accessed for
    * the HIDS_Event_Data_t structure.
    */
typedef enum
{
   etHIDS_Server_Read_Client_Configuration_Request, /*!< Dispatched to a HIDS Server when a HIDS Client is attempting to read a descriptor. */
   etHIDS_Server_Client_Configuration_Update_Request, /*!< Dispathed to a HIDS Server when a HIDS Client is writing a Client Configuration descriptor. */
   etHIDS_Server_Get_Protocol_Mode_Request, /*!< Dispathced to a HIDS Server when a HIDS Client is attempting to get the current Protocol Mode. */
   etHIDS_Server_Set_Protocol_Mode_Request, /*!< Dispathced to a HIDS Server when a HIDS Client is attempting to set the current Protocol Mode. */
   etHIDS_Server_Get_Report_Map_Request, /*!< Dispathced to a HIDS Server when a HIDS Client is attempting to get the Report Map value. */
   etHIDS_Server_Get_Report_Request, /*!< Dispathced to a HIDS Server when a HIDS Client is attempting to get the specified Report value. */
   etHIDS_Server_Set_Report_Request, /*!< Dispathced to a HIDS Server when a HIDS Client is attempting to set the Report value. */
   etHIDS_Server_Control_Point_Command_Indication /*!< Dispathced to a HIDS Server in response to the reception of a request from a Client to write the Control Point Command. */
} HIDS_Event_Type_t;

   /*! The following HIDS Profile Event is dispatched to a HIDS Server
    * when a HIDS Client is attempting to read a descriptor.  The
    * ConnectionID, ConnectionType, and RemoteDevice specifiy the Client
    * that is making the request.  The TransactionID specifies the
    * TransactionID of the request, this can be used when responding to
    * the request using the HIDS_Client_Configuration_Read_Response()
    * API function.  The ReportType specifies the Descriptor that the
    * Client is attempting to read.  The final member is a report
    * reference structure (Only valid if ReportType is set to rtReport)
    * that contains the Report ID and Report Type of the characteristic
    * value whose CCCD is being read.
    * \note Only the following characteristic types may be returned
    *          in this event:
    *                   rtReport (Input Report Type Only)
    *                   rtBootKeyboardInputReport
    *                   rtBootMouseInputReport
    * \note The ReportReferenceData member is only valid if
    *          ReportType is set to rtReport.
    */
typedef struct _tagHIDS_Read_Client_Configuration_Data_t
{
   unsigned int                 InstanceID;
   unsigned int                 ConnectionID;
   unsigned int                 TransactionID;
   GATT_Connection_Type_t       ConnectionType;
   BD_ADDR_t                    RemoteDevice;
   HIDS_Report_Type_t           ReportType;
   HIDS_Report_Reference_Data_t ReportReferenceData;
} HIDS_Read_Client_Configuration_Data_t;

#define HIDS_READ_CLIENT_CONFIGURATION_DATA_SIZE         (sizeof(HIDS_Read_Client_Configuration_Data_t))

   /*! The following HIDS Profile Event is dispatched to a HIDS Server
    * when a HIDS Client has written a Client Configuration descriptor.
    * The ConnectionID, ConnectionType, and RemoteDevice specifiy the
    * Client that is making the update.  The ReportType specifies the
    * Descriptor that the Client is writing.  The ReportReferenceData
    * member is a report reference structure (Only valid if ReportType
    * is set to rtReport) that contains the Report ID and Report Type of
    * the characteristic value whose CCCD is being read.  The final
    * member is the new Client Configuration for the specified
    * characteristic.
    * \note Only the following characteristic types may be returned
    *          in this event:
    *                   rtReport (Input Report Type Only)
    *                   rtBootKeyboardInputReport
    *                   rtBootMouseInputReport
    * \note The ReportReferenceData member is only valid if
    *          ReportType is set to rtReport.
    */
typedef struct _tagHIDS_Client_Configuration_Update_Data_t
{
   unsigned int                 InstanceID;
   unsigned int                 ConnectionID;
   GATT_Connection_Type_t       ConnectionType;
   BD_ADDR_t                    RemoteDevice;
   HIDS_Report_Type_t           ReportType;
   HIDS_Report_Reference_Data_t ReportReferenceData;
   Word_t                       ClientConfiguration;
} HIDS_Client_Configuration_Update_Data_t;

#define HIDS_CLIENT_CONFIGURATION_UPDATE_DATA_SIZE       (sizeof(HIDS_Client_Configuration_Update_Data_t))

   /*! The following HIDS Profile Event is dispatched to a HIDS Server
    * when a HIDS Client is attempting to get the current Protocol Mode.
    * The ConnectionID, ConnectionType, and RemoteDevice specifiy the
    * Client that is making the request.  The TransactionID specifies
    * the TransactionID of the request, this can be used when responding
    * to the request using the HIDS_Get_Protocol_Mode_Response() API
    * function.
    */
typedef struct _tagHIDS_Get_Protocol_Mode_Request_Data_t
{
   unsigned int           InstanceID;
   unsigned int           ConnectionID;
   unsigned int           TransactionID;
   GATT_Connection_Type_t ConnectionType;
   BD_ADDR_t              RemoteDevice;
} HIDS_Get_Protocol_Mode_Request_Data_t;

#define HIDS_GET_PROTOCOL_MODE_REQUEST_DATA_SIZE         (sizeof(HIDS_Get_Protocol_Mode_Request_Data_t))

   /*! The following HIDS Profile Event is dispatched to a HIDS Server
    * when a HIDS Client is attempting to set the current Protocol Mode.
    * The ConnectionID, ConnectionType, and RemoteDevice specifiy the
    * Client that is making the request.  The TransactionID specifies
    * the TransactionID of the request, this can be used when responding
    * to the request using the HIDS_Set_Protocol_Mode_Response() API
    * function.  The final member is the ProtocolMode that the HIDS
    * client is attempting to set.
    */
typedef struct _tagHIDS_Set_Protocol_Mode_Request_Data_t
{
   unsigned int           InstanceID;
   unsigned int           ConnectionID;
   unsigned int           TransactionID;
   GATT_Connection_Type_t ConnectionType;
   BD_ADDR_t              RemoteDevice;
   HIDS_Protocol_Mode_t   ProtocolMode;
} HIDS_Set_Protocol_Mode_Request_Data_t;

#define HIDS_SET_PROTOCOL_MODE_REQUEST_DATA_SIZE         (sizeof(HIDS_Set_Protocol_Mode_Request_Data_t))

   /*! The following HIDS Profile Event is dispatched to a HIDS Server
    * when a HIDS Client is attempting to get the Report Map value.  The
    * ConnectionID, ConnectionType, and RemoteDevice specifiy the Client
    * that is making the request.  The TransactionID specifies the
    * TransactionID of the request, this can be used when responding to
    * the request using the HIDS_Get_Report_Map_Response() API function.
    * The final parameter is the offset into the Report Map that the
    * HIDS Client is attempting to read.
    */
typedef struct _tagHIDS_Get_Report_Map_Request_Data_t
{
   unsigned int           InstanceID;
   unsigned int           ConnectionID;
   unsigned int           TransactionID;
   GATT_Connection_Type_t ConnectionType;
   BD_ADDR_t              RemoteDevice;
   Word_t                 ReportMapOffset;
} HIDS_Get_Report_Map_Request_Data_t;

#define HIDS_GET_REPORT_MAP_REQUEST_DATA_SIZE            (sizeof(HIDS_Get_Report_Map_Request_Data_t))

   /*! The following HIDS Profile Event is dispatched to a HIDS Server
    * when a HIDS Client is attempting to get the a Report value.  The
    * ConnectionID, ConnectionType, and RemoteDevice specifiy the Client
    * that is making the request.  The TransactionID specifies the
    * TransactionID of the request, this can be used when responding to
    * the request using the HIDS_Get_Report_Response() API function.
    * The ReportOffset parameter is the offset into the Report that the
    * HIDS Client is attempting to read.  The ReportType specifies the
    * report that the HIDS Client is attempting to get.  The
    * ReportReferenceData member is a report reference structure (Only
    * valid if ReportType is set to rtReport) that contains the Report
    * ID and Report Type of the Report that is being read.
    * \note The ReportReferenceData member is only valid if
    *          ReportType is set to rtReport.
    */
typedef struct _tagHIDS_Get_Report_Request_Data_t
{
   unsigned int                 InstanceID;
   unsigned int                 ConnectionID;
   unsigned int                 TransactionID;
   GATT_Connection_Type_t       ConnectionType;
   BD_ADDR_t                    RemoteDevice;
   Word_t                       ReportOffset;
   HIDS_Report_Type_t           ReportType;
   HIDS_Report_Reference_Data_t ReportReferenceData;
} HIDS_Get_Report_Request_Data_t;

#define HIDS_GET_REPORT_REQUEST_DATA_SIZE                (sizeof(HIDS_Get_Report_Request_Data_t))

   /*! The following HIDS Profile Event is dispatched to a HIDS Server
    * when a HIDS Client is attempting to set the a Report value.  The
    * ConnectionID, ConnectionType, and RemoteDevice specifiy the Client
    * that is making the request.  The TransactionID specifies the
    * TransactionID of the request, this can be used when responding to
    * the request using the HIDS_Set_Report_Response() API function.
    * The ReportType specifies the report that the HIDS Client is
    * attempting to set.  The ReportReferenceData member is a report
    * reference structure (Only valid if ReportType is set to rtReport)
    * that contains the Report ID and Report Type of the Report that is
    * being written.  The final two members specify the length of the
    * data and a pointer to the data that the HIDS Client is attempting
    * to write.
    * \note The ReportReferenceData member is only valid if
    *          ReportType is set to rtReport.
    */
typedef struct _tagHIDS_Set_Report_Request_Data_t
{
   unsigned int                  InstanceID;
   unsigned int                  ConnectionID;
   unsigned int                  TransactionID;
   GATT_Connection_Type_t        ConnectionType;
   BD_ADDR_t                     RemoteDevice;
   HIDS_Report_Type_t            ReportType;
   HIDS_Report_Reference_Data_t  ReportReferenceData;
   unsigned int                  ReportLength;
   Byte_t                       *Report;
} HIDS_Set_Report_Request_Data_t;

#define HIDS_SET_REPORT_REQUEST_DATA_SIZE                (sizeof(HIDS_Set_Report_Request_Data_t))

   /*! The following is dispatched to a HIDS Server in response to the
    * reception of request from a Client to write to the Control Point
    * Command.
    */
typedef struct _tagHIDS_Control_Point_Command_Data_t
{
   unsigned int                 InstanceID;
   unsigned int                 ConnectionID;
   GATT_Connection_Type_t       ConnectionType;
   BD_ADDR_t                    RemoteDevice;
   HIDS_Control_Point_Command_t ControlPointCommand;
} HIDS_Control_Point_Command_Data_t;

#define HIDS_CONTROL_POINT_COMMAND_DATA_SIZE             (sizeof(HIDS_Control_Point_Command_Data_t))

   /*! The following structure represents the container structure for
    * holding all HIDS Profile Event Data.  This structure is received
    * for each event generated.  The Event_Data_Type member is used to
    * determine the appropriate union member element to access the
    * contained data.  The Event_Data_Size member contains the total
    * size of the data contained in this event.
    */
typedef struct _tagHIDS_Event_Data_t
{
   HIDS_Event_Type_t Event_Data_Type;
   Word_t            Event_Data_Size;
   union
   {
      HIDS_Read_Client_Configuration_Data_t   *HIDS_Read_Client_Configuration_Data;
      HIDS_Client_Configuration_Update_Data_t *HIDS_Client_Configuration_Update_Data;
      HIDS_Get_Protocol_Mode_Request_Data_t   *HIDS_Get_Protocol_Mode_Request_Data;
      HIDS_Set_Protocol_Mode_Request_Data_t   *HIDS_Set_Protocol_Mode_Request_Data;
      HIDS_Get_Report_Map_Request_Data_t      *HIDS_Get_Report_Map_Data;
      HIDS_Get_Report_Request_Data_t          *HIDS_Get_Report_Request_Data;
      HIDS_Set_Report_Request_Data_t          *HIDS_Set_Report_Request_Data;
      HIDS_Control_Point_Command_Data_t       *HIDS_Control_Point_Command_Data;
   } Event_Data;
} HIDS_Event_Data_t;

#define HIDS_EVENT_DATA_SIZE                             (sizeof(HIDS_Event_Data_t))

   /*! The following declared type represents the Prototype Function for
    * a HIDS Event Data Callback.  This function will be called whenever
    * an HIDS Event occurs that is associated with the specified
    * Bluetooth Stack ID.  This function passes to the caller the
    * Bluetooth Stack ID, the HIDS Event Data that occurred and the HIDS
    * Event Callback Parameter that was specified when this Callback was
    * installed.  The caller is free to use the contents of the HIDS
    * Event Data ONLY in the context of this callback.  If the caller
    * requires the Data for a longer period of time, then the callback
    * function MUST copy the data into another Data Buffer This function
    * is guaranteed NOT to be invoked more than once simultaneously for
    * the specified installed callback (i.e.  this function DOES NOT
    * have be re-entrant).  It needs to be noted however, that if the
    * same Callback is installed more than once, then the callbacks will
    * be called serially.  Because of this, the processing in this
    * function should be as efficient as possible.  It should also be
    * noted that this function is called in the Thread Context of a
    * Thread that the User does NOT own.  Therefore, processing in this
    * function should be as efficient as possible (this argument holds
    * anyway because another HIDS Profile Event will not be processed
    * while this function call is outstanding).
    * \note This function MUST NOT Block and wait for events that
    *            can only be satisfied by receiving HIDS Event Packets.
    *            A Deadlock WILL occur because NO HIDS Event Callbacks
    *            will be issued while this function is currently
    *            outstanding.
    */
typedef void (BTPSAPI *HIDS_Event_Callback_t)(unsigned int BluetoothStackID, HIDS_Event_Data_t *HIDS_Event_Data, unsigned long CallbackParameter);

   /* HIDS Server API.
    * @brief The following function is responsible for opening a HID over GATT
    * Service.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID on which
    * to open the server.  @param Flags The second parameter is a bit mask of flags
    * which is used to control what the HID Service supports.
    * @param HIDInformation The third parameter is a pointer to a HID Information structure containing
    * information about the HID Service.
    * @param NumIncludedServices The fourth parameter specifies the number of included services.
    * @param ServiceIDList The fifth parameter is a list of ServiceIDs that contain the
    * ServiceIDs of the services to be included by this HID instance.
    * @param NumExternalReportReferences The sixth parameter contains the number  of
    * of report references.
    * @param ReferenceUUID The seventh parameter contains a list of GATT UUIDs that contain a list of
    * UUIDs of characteristics referenced by this HID Instance.
    * @param NumReports The eighth parameter contains the number of reports.
    * @param ReportReference The ninth parameter contains a list of reports that will be
    * contained in this HID Service.
    * @param EventCallback The tenth parameter is a callback function that will be called when
    * an event occurs on this HID instance.
    * @param CallbackParameter The eleventh parameter is the callback parameter.
    * @param ServiceID The final parameter is a
    * pointer to store the GATT Service ID of the registered HIDS
    * service.  This can be used to include the service registered by
    * this call.  @return This function returns the positive, non-zero, Instance
    * ID or a negative error code.
    * \note The Flags parameter must be a bit mask made of bits of
    *          the form HIDS_FLAGS_XXX.
    * \note The ServiceIDList parameter must contain valid ServiceIDs
    *          of services that have already been registered with GATT.
    */
BTPSAPI_DECLARATION int BTPSAPI HIDS_Initialize_Service(unsigned int BluetoothStackID, Byte_t Flags, HIDS_HID_Information_Data_t *HIDInformation, unsigned int NumIncludedServices, unsigned int *ServiceIDList, unsigned int NumExternalReportReferences, GATT_UUID_t *ReferenceUUID, unsigned int NumReports, HIDS_Report_Reference_Data_t *ReportReference, HIDS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HIDS_Initialize_Service_t)(unsigned int BluetoothStackID, Byte_t Flags, HIDS_HID_Information_Data_t *HIDInformation, unsigned int NumIncludedServices, unsigned int *ServiceIDList, unsigned int NumExternalReportReferences, GATT_UUID_t *ReferenceUUID, unsigned int NumReports, HIDS_Report_Reference_Data_t *ReportReference, HIDS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID);
#endif

   /*! @brief The following function is responsible for opening a HID over GATT
    * Service.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID on which
    * to open the server.  @param Flags The second parameter is a bit mask of flags
    * which is used to control what the HID Service supports.
    * @param HIDInformation The third parameter is a pointer to a HID Information structure containing
    * information about the HID Service.
    * @param NumIncludedServices The fourth parameter specifies the number of included services.
    * @param ServiceIDList The fifth parameter is a list of ServiceIDs that contain the
    * ServiceIDs of the services to be included by this HID instance.
    * @param NumExternalReportReferences The sixth parameter contains the number  of
    * of report references.
    * @param ReferenceUUID The seventh parameter contains a list of GATT UUIDs that contain a list of
    * UUIDs of characteristics referenced by this HID Instance.
    * @param NumReports The eighth parameter contains the number of reports.
    * @param ReportReference The ninth parameter contains a list of reports that will be
    * contained in this HID Service.
    * @param EventCallback The tenth parameter is a callback function that will be called when
    * an event occurs on this HID instance.
    * @param CallbackParameter The eleventh parameter is the callback parameter.
    * @param ServiceID The final parameter is a
    * pointer to store the GATT Service ID of the registered HIDS
    * service.  This can be used to include the service registered by
    * this call.
    * @param ServiceHandleRange The final parameter is a pointer, that on input can be
    * used to control the location of the service in the GATT database,
    * and on ouput to store the service handle range.
    * @return This function returns the positive, non-zero, Instance ID or a negative error
    * code.
    * \note The Flags parameter must be a bit mask made of bits of
    *          the form HIDS_FLAGS_XXX.
    * \note The ServiceIDList parameter must contain valid ServiceIDs
    *          of services that have already been registered with GATT.
    */
BTPSAPI_DECLARATION int BTPSAPI HIDS_Initialize_Service_Handle_Range(unsigned int BluetoothStackID, Byte_t Flags, HIDS_HID_Information_Data_t *HIDInformation, unsigned int NumIncludedServices, unsigned int *ServiceIDList, unsigned int NumExternalReportReferences, GATT_UUID_t *ReferenceUUID, unsigned int NumReports, HIDS_Report_Reference_Data_t *ReportReference, HIDS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HIDS_Initialize_Service_Handle_Range_t)(unsigned int BluetoothStackID, Byte_t Flags, HIDS_HID_Information_Data_t *HIDInformation, unsigned int NumIncludedServices, unsigned int *ServiceIDList, unsigned int NumExternalReportReferences, GATT_UUID_t *ReferenceUUID, unsigned int NumReports, HIDS_Report_Reference_Data_t *ReportReference, HIDS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange);
#endif

   /*! @brief The following function is responsible for closing a previously
    * opened HIDS Server.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID
    * on which to close the server.  @param InstanceID The second parameter is the
    * InstanceID that was returned from a successfull call to
    * HIDS_Initialize_Service(). @return This function returns a zero if
    * successful or a negative return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI HIDS_Cleanup_Service(unsigned int BluetoothStackID, unsigned int InstanceID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HIDS_Cleanup_Service_t)(unsigned int BluetoothStackID, unsigned int InstanceID);
#endif

   /*! @brief The following function is responsible for querying the number of
    * attributes that are contained in the HIDS Service that is
    * registered with a call to HIDS_Initialize_Service().
    * @param Flags The first parameter is a bit mask of flags which is used to control what the
    * HID Service supports.  @param NumIncludedServices The second parameter specifies the number
    * of services that will be included by this HID instance.
    * @param NumExternalReportReferences The third parameter specifies the number of external characteristics that
    * are referenced by reports to be contained in this HID instance.
    * @param NumReports The eighth parameter contains the number of reports.
    * @param ReportReference The ninth parameter contains a list of reports that will be
    * contained in this HID Service.
    * @return This function returns the non-zero
    * number of attributes that are contained in a HIDS Server or zero
    * on failure.
    */
BTPSAPI_DECLARATION unsigned int BTPSAPI HIDS_Query_Number_Attributes(Byte_t Flags, unsigned int NumIncludedServices, unsigned int NumExternalReportReferences, unsigned int NumReports, HIDS_Report_Reference_Data_t *ReportReference);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef unsigned int (BTPSAPI *PFN_HIDS_Query_Number_Attributes_t)(Byte_t Flags, unsigned int NumIncludedServices, unsigned int NumExternalReportReferences, unsigned int NumReports, HIDS_Report_Reference_Data_t *ReportReference);
#endif

   /*! @brief The following function is responsible for responding to a HIDS
    * Read Client Configuration Request.  @param BluetoothStackID The first parameter is the
    * Bluetooth Stack ID of the Bluetooth Device.  @param InstanceID The second parameter
    * is the InstanceID returned from a successful call to
    * HIDS_Initialize_Server(). @param TransactionID The third parameter is the Transaction ID of the
    * request. @param Client_Configuration The final parameter contains the Client Configuration to
    * send to the remote device.  @return This function returns a zero if
    * successful or a negative return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI HIDS_Read_Client_Configuration_Response(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int TransactionID, Word_t Client_Configuration);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HIDS_Read_Client_Configuration_Response_t)(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int TransactionID, Word_t Client_Configuration);
#endif

   /*! @brief The following function is responsible for responding to a HIDS Get
    * Protocol Mode Request.  @param BluetoothStackID The first parameter is the Bluetooth Stack
    * ID of the Bluetooth Device.  @param InstanceID The second parameter is the
    * InstanceID returned from a successful call to
    * HIDS_Initialize_Server(). @param TransactionID The third parameter is the Transaction ID of the
    * request.  @param ErrorCode The fourth parameter is an ErrorCode parameter that can
    * be used to respond with an error response to the request.
    * @param CurrentProtocolMode The final parameter contains the Protocol Mode to respond with.
    * @return This function returns a zero if successful or a negative return error
    * code if an error occurs.
    * \note If ErrorCode is 0 then this function will respond to the
    *          Get Protocol Mode Request successfully.  If ErrorCode is
    *          NON-ZERO then this function will respond with an error
    *          with the error code set to the parameter.
    */
BTPSAPI_DECLARATION int BTPSAPI HIDS_Get_Protocol_Mode_Response(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int TransactionID, Byte_t ErrorCode, HIDS_Protocol_Mode_t CurrentProtocolMode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HIDS_Get_Protocol_Mode_Response_t)(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int TransactionID, Byte_t ErrorCode, HIDS_Protocol_Mode_t CurrentProtocolMode);
#endif

   /*! @brief The following function is responsible for responding to a HIDS Get
    * Report Map Request.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID
    * of the Bluetooth Device.  @param InstanceID The second parameter is the InstanceID
    * returned from a successful call to HIDS_Initialize_Server(). @param TransactionID The
    * third parameter is the Transaction ID of the request.  @param ErrorCode The fourth parameter
    * is an ErrorCode parameter that can be used to respond with an
    * error response to the request. @param ReportMapLength The fifth parameter specifies
    * the report ma length.
    * @param ReportMap The final parameter specifies the
    * data to respond with (if ErrorCode is 0).
    * @return This function returns a zero if successful or a negative return
    * error code if an error occurs.
    * \note If ErrorCode is 0 then this function will respond to the
    *          Get Report Request successfully.  If ErrorCode is
    *          NON-ZERO then this function will respond with an error
    *          with the error code set to the parameter.
    */
BTPSAPI_DECLARATION int BTPSAPI HIDS_Get_Report_Map_Response(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int TransactionID, Byte_t ErrorCode, unsigned int ReportMapLength, Byte_t *ReportMap);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HIDS_Get_Report_Map_Response_t)(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int TransactionID, Byte_t ErrorCode, unsigned int ReportMapLength, Byte_t *ReportMap);
#endif

   /*! @brief The following function is responsible for responding to a HIDS Get
    * Report Map Request.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID
    * of the Bluetooth Device.  @param InstanceID The second parameter is the InstanceID
    * returned from a successful call to HIDS_Initialize_Server(). @param TransactionID The
    * third parameter is the Transaction ID of the request.  @param ReportType The fourth parameter
    * is the ReportType that the client is attempting to get. @param ReportReferenceData The fifth
    * parameter, which is only valid if ReportType is rtReport, contains
    * the report reference data of the Report that the client is
    * attempting to get. @param ErrorCode The sixth parameter is an ErrorCode parameter
    * that can be used to respond with an error response to the request.
    * @param ReportLength ThReport Map length
    * @param Report The data to respond with (if ErrorCode is 0).
    * @return This function returns a zero if
    * successful or a negative return error code if an error occurs.
    * \note If ErrorCode is 0 then this function will respond to the
    *          Get Report Request successfully.  If ErrorCode is
    *          NON-ZERO then this function will respond with an error
    *          with the error code set to the parameter.
    * \note The ReportReferenceData member is only valid if
    *          ReportType is set to rtReport.
    */
BTPSAPI_DECLARATION int BTPSAPI HIDS_Get_Report_Response(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int TransactionID, HIDS_Report_Type_t ReportType, HIDS_Report_Reference_Data_t *ReportReferenceData, Byte_t ErrorCode, unsigned int ReportLength, Byte_t *Report);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HIDS_Get_Report_Response_t)(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int TransactionID, HIDS_Report_Type_t ReportType, HIDS_Report_Reference_Data_t *ReportReferenceData, Byte_t ErrorCode, unsigned int ReportLength, Byte_t *Report);
#endif

   /*! @brief The following function is responsible for responding to a HIDS Set
    * Report Map Request.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID
    * of the Bluetooth Device.  @param InstanceID The second parameter is the InstanceID
    * returned from a successful call to HIDS_Initialize_Server(). @param TransactionID The
    * third parameter is the Transaction ID of the request.  @param ReportType The fourth parameter
    * is the ReportType that the client is attempting to set. @param ReportReferenceData The fifth
    * parameter, which is only valid if ReportType is rtReport, contains
    * the report reference data of the Report that the client is
    * attempting to set. @param ErrorCode The final parameter is an ErrorCode parameter
    * that can be used to respond with an error response to the request.
    * @return This function returns a zero if successful or a negative return
    * error code if an error occurs.
    * \note If ErrorCode is 0 then this function will respond to the
    *          Get Report Request successfully.  If ErrorCode is
    *          NON-ZERO then this function will respond with an error
    *          with the error code set to the parameter.
    * \note The ReportReferenceData member is only valid if
    *          ReportType is set to rtReport.
    */
BTPSAPI_DECLARATION int BTPSAPI HIDS_Set_Report_Response(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int TransactionID, HIDS_Report_Type_t ReportType, HIDS_Report_Reference_Data_t *ReportReferenceData, Byte_t ErrorCode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HIDS_Set_Report_Response_t)(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int TransactionID, HIDS_Report_Type_t ReportType, HIDS_Report_Reference_Data_t *ReportReferenceData, Byte_t ErrorCode);
#endif

   /*! @brief The following function is responsible for sending an Input Report
    * notification to a specified remote device.
    * @param BluetoothStackID The first parameter is
    * the Bluetooth Stack ID of the Bluetooth Device.
    * @param InstanceID The second parameter is the InstanceID returned from a successful call to
    * HIDS_Initialize_Server().
    * @param ConnectionID The third parameter is the ConnectionID
    * of the remote device to send the notification to.
    * @param ReportType The fourth parameter specifies the ReportType of the report that is to be
    * notified. @param ReportReferenceData The fifth parameter is a pointer to a Report Reference
    * structure that is only used (and must be specified only if) the
    * ReportType is rtInputReport.
    * @param InputReportLength The length of the Input Report.
    * @param InputReportData The A pointer to the Input Report that
    * is to be notified.  @return This function returns the number of bytes that
    * were successfully notified on success or a negative return error
    * code if an error occurs.
    * \note The only valid values that ReportType may be set to are
    *          as follows:
    *                rtReport (Input Report Only)
    *                rtBootKeyboardInputReport
    *                rtBootMouseInputReport
    * \note If the ReportType is rtReport, then the
    *          ReportReferenceData must be valid and point to a Report
    *          Reference structure of the Input Report to be notified.
    *          Otherwise the ReportReferenceData parameter can be NULL.
    */
BTPSAPI_DECLARATION int BTPSAPI HIDS_Notify_Input_Report(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int ConnectionID, HIDS_Report_Type_t ReportType, HIDS_Report_Reference_Data_t *ReportReferenceData, Word_t InputReportLength, Byte_t *InputReportData);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HIDS_Notify_Input_Report_t)(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int ConnectionID, HIDS_Report_Type_t ReportType, HIDS_Report_Reference_Data_t *ReportReferenceData, Word_t InputReportLength, Byte_t *InputReportData);
#endif

   /* HIDS Client API.
    * @brief The following function is responsible for parsing a value received
    * from a remote HIDS Server and interpreting it as a HID Information
    * value.  @param ValueLength The first parameter is the length of the value returned by
    * the remote HIDS Server.  @param Value The second parameter is a pointer to the
    * data returned by the remote HIDS Server.  @param HIDSHIDInformation The final parameter is a
    * pointer to store the parsed HID Information value (if successful).
    * @return This function returns a zero if successful or a negative return
    * error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI HIDS_Decode_HID_Information(unsigned int ValueLength, Byte_t *Value, HIDS_HID_Information_Data_t *HIDSHIDInformation);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HIDS_Decode_HID_Information_t)(unsigned int ValueLength, Byte_t *Value, HIDS_HID_Information_Data_t *HIDSHIDInformation);
#endif

   /*! @brief The following function is responsible for parsing a value received
    * from a remote HIDS Server and interpreting it as a Report
    * Reference value.  @param ValueLength The first parameter is the length of the value
    * returned by the remote HIDS Server.  @param Value The second parameter is a
    * pointer to the data returned by the remote HIDS Server.
    * @param ReportReferenceData The final parameter is a pointer to store the parsed Report Reference data
    * (if successful).  @return This function returns a zero if successful or a
    * negative return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI HIDS_Decode_Report_Reference(unsigned int ValueLength, Byte_t *Value, HIDS_Report_Reference_Data_t *ReportReferenceData);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HIDS_Decode_Report_Reference_t)(unsigned int ValueLength, Byte_t *Value, HIDS_Report_Reference_Data_t *ReportReferenceData);
#endif

   /*! @brief The following function is responsible for parsing a value received
    * from a remote HIDS Server and interpreting it as a External Report
    * Reference value.  @param ValueLength The first parameter is the length of the value
    * returned by the remote HIDS Server.  @param Value The second parameter is a
    * pointer to the data returned by the remote HIDS Server.
    * @param ExternalReportReferenceUUID The final parameter is a pointer to store the parsed External Report
    * Reference data (if successful).  @return This function returns a zero if
    * successful or a negative return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI HIDS_Decode_External_Report_Reference(unsigned int ValueLength, Byte_t *Value, GATT_UUID_t *ExternalReportReferenceUUID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HIDS_Decode_External_Report_Reference_t)(unsigned int ValueLength, Byte_t *Value, GATT_UUID_t *ExternalReportReferenceUUID);
#endif

   /*! @brief The following function is responsible for formatting a HIDS
    * Protocol Mode into a user specified buffer.  @param ProtocolMode The first parameter
    * is the command to format.
    * @param BufferLength The second parameter contains the length of the buffer.
    * @param Buffer The final parameter contains a pointer to the buffer
    * to format the Protocol Mode into.
    * @return This function returns a zero if successful or a negative
    * return error code if an error occurs.
    * \note The BufferLength and Buffer parameter must point to a
    *          buffer of at least HIDS_PROTOCOL_MODE_VALUE_LENGTH in
    *          size.
    */
BTPSAPI_DECLARATION int BTPSAPI HIDS_Format_Protocol_Mode(HIDS_Protocol_Mode_t ProtocolMode, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HIDS_Format_Protocol_Mode_t)(HIDS_Protocol_Mode_t ProtocolMode, unsigned int BufferLength, Byte_t *Buffer);
#endif

   /*! @brief The following function is responsible for formatting a HIDS
    * Control Point Command into a user specified buffer.
    * @param Command The first parameter is the command to format.
    * @param BufferLength The second parameter contains the length of the buffer.
    * @param Buffer The final parameter contains a pointer to the buffer
    * to format the Protocol Mode into.
    * @return This function returns a zero if successful or a
    * negative return error code if an error occurs.
    * \note The BufferLength and Buffer parameter must point to a
    *          buffer of at least HIDS_CONTROL_POINT_VALUE_LENGTH in
    *          size.
    */
BTPSAPI_DECLARATION int BTPSAPI HIDS_Format_Control_Point_Command(HIDS_Control_Point_Command_t Command, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HIDS_Format_Control_Point_Command_t)(HIDS_Control_Point_Command_t Command, unsigned int BufferLength, Byte_t *Buffer);
#endif

#endif
