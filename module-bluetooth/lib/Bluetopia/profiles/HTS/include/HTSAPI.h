/*
 * Copyright 2011 - 2014 Stonestreet One.
 * All Rights Reserved.
 * Author:  Tim Cook
 *** MODIFICATION HISTORY ****************************************************
 *
 *   mm/dd/yy  F. Lastname    Description of Modification
 *   --------  -----------    ------------------------------------------------
 *   10/04/11  T. Cook        Initial creation.
 ****************************************************************************
 */

/**
 *  @file HTSAPI.h
 *
 *  @brief Stonestreet One Bluetooth Health Thermometer Service (GATT
 *      based) API Type Definitions, Constants, and Prototypes.
 *      To use any of the following API's,
 *      include the following declaration in your application.
 *
 *  @code
 *  #include "SS1BTHTS.h"
 *  @endcode
 *  ============================================================================
 */
#ifndef __HTSAPIH__
#define __HTSAPIH__

#include "SS1BTPS.h"        /*! Bluetooth Stack API Prototypes/Constants. */
#include "SS1BTGAT.h"       /*! Bluetooth Stack GATT API Prototypes/Constants. */
#include "HTSTypes.h"       /*! Health Thermometer Service Types/Constants. */
   /*! Error Return Codes.
    * Error Codes that are smaller than these (less than -1000) are
    * related to the Bluetooth Protocol Stack itself (see BTERRORS.H).
    */
#define HTS_ERROR_INVALID_PARAMETER                      (-1000)
#define HTS_ERROR_INVALID_BLUETOOTH_STACK_ID             (-1001)
#define HTS_ERROR_INSUFFICIENT_RESOURCES                 (-1002)
#define HTS_ERROR_SERVICE_ALREADY_REGISTERED             (-1003)
#define HTS_ERROR_INVALID_INSTANCE_ID                    (-1004)
#define HTS_ERROR_MALFORMATTED_DATA                      (-1005)
#define HTS_ERROR_INDICATION_OUTSTANDING                 (-1006)
#define HTS_ERROR_UNKNOWN_ERROR                          (-1007)

   /*! The following structure defines the format of a HTS Time Stamp
    * that may be included in Temperature Measurements.
    */
typedef struct _tagHTS_Time_Stamp_Data_t
{
   Word_t Year;
   Byte_t Month;
   Byte_t Day;
   Byte_t Hours;
   Byte_t Minutes;
   Byte_t Seconds;
} HTS_Time_Stamp_Data_t;

#define HTS_TIME_STAMP_DATA_SIZE                         (sizeof(HTS_Time_Stamp_Data_t))

   /*! The following MACRO is a utility MACRO that exists to valid that a
    * specified Time Stamp is valid.  The only parameter to this
    * function is the HTS_Time_Stamp_t structure to valid.  This MACRO
    * returns TRUE if the Time Stamp is valid or FALSE otherwise.
    */
#define HTS_TIME_STAMP_VALID_TIME_STAMP(_x)              ((GATT_DATE_TIME_VALID_YEAR(((_x)).Year)) && (GATT_DATE_TIME_VALID_MONTH(((_x)).Month)) && (GATT_DATE_TIME_VALID_DAY(((_x)).Day)) && (GATT_DATE_TIME_VALID_HOURS(((_x)).Hours)) && (GATT_DATE_TIME_VALID_MINUTES(((_x)).Minutes)) && (GATT_DATE_TIME_VALID_SECONDS(((_x)).Seconds)))

   /*! The following defines the format of the Valid Range value that may
    * be set.
    */
typedef struct _tagHTS_Valid_Range_Data_t
{
   Word_t Lower_Bounds;
   Word_t Upper_Bounds;
} HTS_Valid_Range_Data_t;

#define HTS_VALID_RANGE_DATA_SIZE                        (sizeof(HTS_Valid_Range_Data_t))

   /*! The followng defines the format of a Temperature Measurement.  The
    * first member specifies information on the optional fields that are
    * included in this structure.  The second member is the temperature.
    * The third member is an optional time stamp.  The final member is
    * the optional temperature type.
    * \note The Flags member is made up of bitmasks of the form
    *          HTS_TEMPERATURE_MEASUREMENT_FLAGS_XXX.
    * \note The Temperature_Type member is an enumerated type that
    *          must be in the format HTS_TEMPERATURE_TYPE_XXX.
    * \note If the Temperature is in Fahrenheit then the
    *          HTS_TEMPERATURE_MEASUREMENT_FLAGS_FAHRENHEIT flag must be
    *          specified in the Flags member.  Otherwise Celsius is
    *          assumed.
    * \note It the optional Time Stamp is included in the data then
    *          the HTS_TEMPERATURE_MEASUREMENT_FLAGS_TIME_STAMP flag
    *          must be specified in the Flags member.
    * \note It the optional Temperature Type is included in the data
    *          then the
    *          HTS_TEMPERATURE_MEASUREMENT_FLAGS_TEMPERATURE_TYPE flag
    *          must be specified in the Flags member.
    */
typedef struct _tagHTS_Temperature_Measurement_Data_t
{
   Byte_t                 Flags;
   HTS_Temperature_Data_t Temperature;
   HTS_Time_Stamp_Data_t  Time_Stamp;
   Byte_t                 Temperature_Type;
} HTS_Temperature_Measurement_Data_t;

#define HTS_TEMPERATURE_MEASUREMENT_DATA_SIZE            (sizeof(HTS_Temperature_Measurement_Data_t))

   /*! The following define the valid Read Request types that a server
    * may receive in a etHTS_Server_Read_Client_Configuration_Request or
    * etHTS_Server_Client_Configuration_Update event.  This is also used
    * by the HTS_Send_Notification to denote the characteristic value to
    * notify.
    * \note For each event it is up to the application to return (or
    *          write) the correct Client Configuration descriptor based
    *          on this value.
    */
typedef enum
{
   ctTemperatureMeasurement,
   ctIntermediateTemperature,
   ctMeasurementInterval
} HTS_Characteristic_Type_t;

   /*! The following enumeration covers all the events generated by the
    * HTS Profile.  These are used to determine the type of each event
    * generated, and to ensure the proper union element is accessed for
    * the HTS_Event_Data_t structure.
    */
typedef enum
{
   etHTS_Server_Read_Client_Configuration_Request, /*!< Dispatched to a HTS Server when a HTS Client is attempting to read a descriptor. */
   etHTS_Server_Client_Configuration_Update, /*!< Dispatched to a HTS Server when a HTS Client has written a Client Configuration descriptor. */
   etHTS_Measurement_Interval_Update, /*!< Dispatched to a HTS Server when a HTS Client is attempting to write the Measurement Interval Characteristic. */
   etHTS_Confirmation_Response /*!< Dispatched to a HTS Server when a HTS client has sent a confirmation response to a previously sent confirmation request. */
} HTS_Event_Type_t;

   /*! The following structure contains the Handles that will need to be
    * cached by a HTS client in order to only do service discovery once.
    */
typedef struct _tagHTS_Client_Information_t
{
   Word_t Temperature_Measurement;
   Word_t Temperature_Measurement_Client_Configuration;
   Word_t Temperature_Type;
   Word_t Intermediate_Temperature;
   Word_t Intermediate_Temperature_Client_Configuration;
   Word_t Measurement_Interval;
   Word_t Measurement_Interval_Client_Configuration;
   Word_t Measurement_Interval_Valid_Range_Descriptor;
} HTS_Client_Information_t;

#define HTS_CLIENT_INFORMATION_DATA_SIZE                (sizeof(HTS_Client_Information_t))

   /*! The following structure contains all of the per Client data that
    * will need to be stored by a HTS Server.
    */
typedef struct _tagHTS_Server_Information_t
{
   Word_t Temperature_Measurement_Client_Configuration;
   Word_t Intermediate_Temperature_Client_Configuration;
   Word_t Measurement_Interval_Client_Configuration;
} HTS_Server_Information_t;

#define HTS_SERVER_INFORMATION_DATA_SIZE                (sizeof(HTS_Server_Information_t))

   /*! The following HTS Profile Event is dispatched to a HTS Server when
    * a HTS Client is attempting to read a descriptor.  The
    * ConnectionID, ConnectionType, and RemoteDevice specifiy the Client
    * that is making the request.  The DescriptorType specifies the
    * Descriptor that the Client is attempting to read.  The
    * TransactionID specifies the TransactionID of the request, this can
    * be used when responding to the request using the
    * HTS_Client_Configuration_Read_Response() API function.
    */
typedef struct _tagHTS_Read_Client_Configuration_Data_t
{
   unsigned int              InstanceID;
   unsigned int              ConnectionID;
   unsigned int              TransactionID;
   GATT_Connection_Type_t    ConnectionType;
   BD_ADDR_t                 RemoteDevice;
   HTS_Characteristic_Type_t ClientConfigurationType;
} HTS_Read_Client_Configuration_Data_t;

#define HTS_READ_CLIENT_CONFIGURATION_DATA_SIZE         (sizeof(HTS_Read_Client_Configuration_Data_t))

   /*! The following HTS Profile Event is dispatched to a HTS Server when
    * a HTS Client has written a Client Configuration descriptor.  The
    * ConnectionID, ConnectionType, and RemoteDevice specifiy the Client
    * that is making the update.  The ClientConfigurationType specifies
    * the Descriptor that the Client is writing.  The final member is
    * the new Client Configuration for the specified characteristic.
    */
typedef struct _tagHTS_Client_Configuration_Update_Data_t
{
   unsigned int              InstanceID;
   unsigned int              ConnectionID;
   GATT_Connection_Type_t    ConnectionType;
   BD_ADDR_t                 RemoteDevice;
   HTS_Characteristic_Type_t ClientConfigurationType;
   Word_t                    ClientConfiguration;
} HTS_Client_Configuration_Update_Data_t;

#define HTS_CLIENT_CONFIGURATION_UPDATE_DATA_SIZE       (sizeof(HTS_Client_Configuration_Update_Data_t))

   /*! The following HTS Profile Event is dispatched to a HTS Server when
    * a HTS Client is attempting to write the Measurement Interval
    * characteristic.  The ConnectionID, ConnectionType, and
    * RemoteDevice specifiy the Client that is making the update.  The
    * final member is the new Measurement Interval.
    */
typedef struct _tagHTS_Measurement_Interval_Update_Data_t
{
   unsigned int           InstanceID;
   unsigned int           ConnectionID;
   GATT_Connection_Type_t ConnectionType;
   BD_ADDR_t              RemoteDevice;
   Word_t                 NewMeasurementInterval;
} HTS_Measurement_Interval_Update_Data_t;

#define HTS_MEASUREMENT_INTERVAL_UPDATE_DATA_SIZE (sizeof(HTS_Measurement_Interval_Update_Data_t))

   /*! The following HTS Profile Event is dispatched to a HTS Server when
    * a HTS Client has sent a confirmation to a previously sent
    * confirmation.  The ConnectionID, ConnectionType, and RemoteDevice
    * specifiy the Client that is making the update.  The
    * Characteristic_Type specifies which Indication the Client has sent
    * a confirmation for.  @param PARAMFINAL The final parameter specifies the status of
    * the Indication
    * \note The Characteristic_Type parameter will NEVER be set to
    *          ctIntermediateTemperature for this event.
    * \note The Status member is set to one of the following values:
    *          GATT_CONFIRMATION_STATUS_SUCCESS
    *          GATT_CONFIRMATION_STATUS_TIMEOUT
    */
typedef struct _tagHTS_Confirmation_Data_t
{
   unsigned int              InstanceID;
   unsigned int              ConnectionID;
   GATT_Connection_Type_t    ConnectionType;
   BD_ADDR_t                 RemoteDevice;
   HTS_Characteristic_Type_t Characteristic_Type;
   Byte_t                    Status;
} HTS_Confirmation_Data_t;

#define HTS_CONFIRMATION_DATA_SIZE                       (sizeof(HTS_Confirmation_Data_t))

   /*! The following structure represents the container structure for
    * holding all HTS Profile Event Data.  This structure is received
    * for each event generated.  The Event_Data_Type member is used to
    * determine the appropriate union member element to access the
    * contained data.  The Event_Data_Size member contains the total
    * size of the data contained in this event.
    */
typedef struct _tagHTS_Event_Data_t
{
   HTS_Event_Type_t Event_Data_Type;
   Word_t           Event_Data_Size;
   union
   {
      HTS_Read_Client_Configuration_Data_t   *HTS_Read_Client_Configuration_Data;
      HTS_Client_Configuration_Update_Data_t *HTS_Client_Configuration_Update_Data;
      HTS_Measurement_Interval_Update_Data_t *HTS_Measurement_Interval_Update_Data;
      HTS_Confirmation_Data_t                *HTS_Confirmation_Data;
   } Event_Data;
} HTS_Event_Data_t;

#define HTS_EVENT_DATA_SIZE                             (sizeof(HTS_Event_Data_t))

   /*! The following declared type represents the Prototype Function for
    * a HTS Profile Event Receive Data Callback.  This function will be
    * called whenever an HTS Profile Event occurs that is associated
    * with the specified Bluetooth Stack ID. @param BluetoothStackID This function passes to
    * the caller the Bluetooth Stack ID. @param HTS_Event_Data The HTS Event Data that
    * occurred. @param CallbackParameter The HTS Profile Event Callback Parameter that was
    * specified when this Callback was installed.  The caller is free to
    * use the contents of the HTS Profile Event Data ONLY in the context
    * of this callback.  If the caller requires the Data for a longer
    * period of time, then the callback function MUST copy the data into
    * another Data Buffer This function is guaranteed NOT to be invoked
    * more than once simultaneously for the specified installed callback
    * (i.e.  this function DOES NOT have be re-entrant).  It needs to be
    * noted however, that if the same Callback is installed more than
    * once, then the callbacks will be called serially.  Because of
    * this, the processing in this function should be as efficient as
    * possible.  It should also be noted that this function is called in
    * the Thread Context of a Thread that the User does NOT own.
    * Therefore, processing in this function should be as efficient as
    * possible (this argument holds anyway because another HTS Profile
    * Event will not be processed while this function call is
    * outstanding).
    * \note This function MUST NOT Block and wait for events that
    *            can only be satisfied by Receiving HTS Profile Event
    *            Packets.  A Deadlock WILL occur because NO HTS Event
    *            Callbacks will be issued while this function is
    *            currently outstanding.
    */
typedef void (BTPSAPI *HTS_Event_Callback_t)(unsigned int BluetoothStackID, HTS_Event_Data_t *HTS_Event_Data, unsigned long CallbackParameter);

   /*! HTS Server API.
    * @brief The following function is responsible for opening a HTS Server.
    * @param BluetoothStackID The first parameter is the Bluetooth Stack ID on which to open the
    * server.  @param EventCallback The second parameter is the Callback function to call
    * when an event occurs on this Server Port.  @param CallbackParameter The third parameter is
    * a user-defined callback parameter that will be passed to the
    * callback function with each event. @param ServiceID The final parameter is a
    * pointer to store the GATT Service ID of the registered HTS
    * service.  This can be used to include the service registered by
    * this call.  @return This function returns the positive, non-zero, Instance
    * ID or a negative error code.
    * \note Only 1 HTS Server may be open at a time, per Bluetooth
    *          Stack ID.
    * \note All Client Requests will be dispatch to the EventCallback
    *          function that is specified by the second parameter to
    *          this function.
    */
BTPSAPI_DECLARATION int BTPSAPI HTS_Initialize_Service(unsigned int BluetoothStackID, HTS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HTS_Initialize_Service_t)(unsigned int BluetoothStackID, HTS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID);
#endif

   /*! @brief The following function is responsible for opening a HTS Server.
    * @param BluetoothStackID The first parameter is the Bluetooth Stack ID on which to open the
    * server.  @param EventCallback The second parameter is the Callback function to call
    * when an event occurs on this Server Port.  @param CallbackParameter The third parameter is
    * a user-defined callback parameter that will be passed to the
    * callback function with each event.  @param ServiceID The fourth parameter is a
    * pointer to store the GATT Service ID of the registered HTS
    * service.  This can be used to include the service registered by
    * this call.  @param ServiceHandleRange The final parameter is a pointer, that on input can be
    * used to control the location of the service in the GATT database,
    * and on ouput to store the service handle range.
    * @return This function returns the positive, non-zero, Instance ID or a negative error
    * code.
    * \note Only 1 HTS Server may be open at a time, per Bluetooth
    *          Stack ID.
    * \note All Client Requests will be dispatch to the EventCallback
    *          function that is specified by the second parameter to
    *          this function.
    */
BTPSAPI_DECLARATION int BTPSAPI HTS_Initialize_Service_Handle_Range(unsigned int BluetoothStackID, HTS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HTS_Initialize_Service_Handle_Range_t)(unsigned int BluetoothStackID, HTS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange);
#endif

   /*! @brief The following function is responsible for closing a previously
    * opened HTS Server.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID
    * on which to close the server. @param InstanceID The second parameter is the
    * InstanceID that was returned from a successful call to
    * HTS_Initialize_Service(). @return This function returns a zero if
    * successful or a negative return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI HTS_Cleanup_Service(unsigned int BluetoothStackID, unsigned int InstanceID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HTS_Cleanup_Service_t)(unsigned int BluetoothStackID, unsigned int InstanceID);
#endif

   /*! @brief The following function is responsible for querying the number of
    * attributes that are contained in the HTS Service that is
    * registered with a call to HTS_Initialize_Service().
    * @return This function returns the non-zero number of attributes that are contained in a
    * HTS Server or zero on failure.
    */
BTPSAPI_DECLARATION unsigned int BTPSAPI HTS_Query_Number_Attributes(void);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef unsigned int (BTPSAPI *PFN_HTS_Query_Number_Attributes_t)(void);
#endif

   /*! @brief The following function is responsible for setting the Temperature
    * Type on the specified HTS Instance.  @param BluetoothStackID The first parameter is the
    * Bluetooth Stack ID of the Bluetooth Device.  @param InstanceID The second parameter
    * is the InstanceID returned from a successful call to
    * HTS_Initialize_Server().  @param Temperature_Type The final parameter is the Temperature
    * Type to set for the specified HTS Instance. @return This function returns
    * a zero if successful or a negative return error code if an error
    * occurs.
    * \note The Temperature_Type parameter should be an enumerated
    *          value of the form HTS_TEMPERATURE_TYPE_XXX.
    */
BTPSAPI_DECLARATION int BTPSAPI HTS_Set_Temperature_Type(unsigned int BluetoothStackID, unsigned int InstanceID, Byte_t Temperature_Type);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HTS_Set_Temperature_Type_t)(unsigned int BluetoothStackID, unsigned int InstanceID, Byte_t Temperature_Type);
#endif

   /*! @brief The following function is responsible for querying the current
    * Temperature Type on the specified HTS Instance.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID of the Bluetooth Device.
    * @param InstanceID The second parameter is the InstanceID returned from a successful call
    * to HTS_Initialize_Server().  @param Temperature_Type The final parameter is a pointer to
    * return the current Temperature Type for the specified HTS
    * Instance. @return This function returns a zero if successful or a
    * negative return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI HTS_Query_Temperature_Type(unsigned int BluetoothStackID, unsigned int InstanceID, Byte_t *Temperature_Type);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HTS_Query_Temperature_Type_t)(unsigned int BluetoothStackID, unsigned int InstanceID, Byte_t *Temperature_Type);
#endif

   /*! @brief The following function is responsible for setting the Measurement
    * Interval on the specified HTS Instance.  @param BluetoothStackID The first parameter is
    * the Bluetooth Stack ID of the Bluetooth Device.  @param InstanceID The second parameter is the InstanceID returned from a successful call to
    * HTS_Initialize_Server(). @param Measurement_Interval The final parameter is the Measurement
    * Interval to set for the specified HTS Instance. @return This function returns a zero if successful or a negative return error code if an
    * error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI HTS_Set_Measurement_Interval(unsigned int BluetoothStackID, unsigned int InstanceID, Word_t Measurement_Interval);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HTS_Set_Measurement_Interval_t)(unsigned int BluetoothStackID, unsigned int InstanceID, Word_t Measurement_Interval);
#endif

   /*! @brief The following function is responsible for querying the current
    * Measurement Interval on the specified HTS Instance.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID of the Bluetooth Device.
    * @param InstanceID The second parameter is the InstanceID returned from a successful call
    * to HTS_Initialize_Server(). @param Measurement_Interval The final parameter is a pointer to
    * return the current Measurement Interval for the specified HTS
    * Instance. @return This function returns a zero if successful or a
    * negative return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI HTS_Query_Measurement_Interval(unsigned int BluetoothStackID, unsigned int InstanceID, Word_t *Measurement_Interval);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HTS_Query_Measurement_Interval_t)(unsigned int BluetoothStackID, unsigned int InstanceID, Word_t *Measurement_Interval);
#endif

   /*! @brief The following function is responsible for setting the Valid Range
    * descriptor value on the specified HTS Instance.
    * @param BluetoothStackID The first parameter is the Bluetooth Stack ID of the Bluetooth Device.
    * @param InstanceID The second parameter is the InstanceID returned from a successful call
    * to HTS_Initialize_Server().  @param ValidRange The final parameter is the Valid
    * Range to set for the specified HTS Instance.
    * @return This function returns a zero if successful or a negative return error code if an
    * error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI HTS_Set_Valid_Range(unsigned int BluetoothStackID, unsigned int InstanceID, HTS_Valid_Range_Data_t *ValidRange);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HTS_Set_Valid_Range_t)(unsigned int BluetoothStackID, unsigned int InstanceID, HTS_Valid_Range_Data_t *ValidRange);
#endif

   /*! @brief The following function is responsible for querying the Valid Range
    * descriptor value on the specified HTS Instance.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID of the Bluetooth Device.
    * @param InstanceID The second parameter is the InstanceID returned from a successful call
    * to HTS_Initialize_Server(). @param ValidRange The final parameter is a pointer to
    * store the Valid Range structure for the specified HTS Instance.
    * @return This function returns a zero if successful or a negative return
    * error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI HTS_Query_Valid_Range(unsigned int BluetoothStackID, unsigned int InstanceID, HTS_Valid_Range_Data_t *ValidRange);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HTS_Query_Valid_Range_t)(unsigned int BluetoothStackID, unsigned int InstanceID, HTS_Valid_Range_Data_t *ValidRange);
#endif

   /*! @brief The following function is responsible for responding to a HTS Read
    * Client Configuration Request.  @param BluetoothStackID The first parameter is the
    * Bluetooth Stack ID of the Bluetooth Device.  @param InstanceID The second parameter
    * is the InstanceID returned from a successful call to
    * HTS_Initialize_Server(). @param TransactionID The third parameter is the Transaction ID of the
    * request. @param Client_Configuration The final parameter contains the Client Configuration to
    * send to the remote device.  @return This function returns a zero if
    * successful or a negative return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI HTS_Read_Client_Configuration_Response(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int TransactionID, Word_t Client_Configuration);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HTS_Read_Client_Configuration_Response_t)(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int TransactionID, Word_t Client_Configuration);
#endif

   /*! @brief The following function is responsible for sending an Intermediate
    * Temperature notification to a specified remote device.
    * @param BluetoothStackID The first parameter is the Bluetooth Stack ID of the Bluetooth Device.
    * @param InstanceID The second parameter is the InstanceID returned from a successful call
    * to HTS_Initialize_Server().  @param ConnectionID The third parameter is the
    * ConnectionID of the remote device to send the notification to.
    * @param Temperature_Measurement The final parameter is the Intermediate Temperature data to
    * notify.  @return This function returns a zero if successful or a negative
    * return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI HTS_Notify_Intermediate_Temperature(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int ConnectionID, HTS_Temperature_Measurement_Data_t *Temperature_Measurement);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HTS_Notify_Intermediate_Temperature_t)(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int ConnectionID, HTS_Temperature_Measurement_Data_t *Temperature_Measurement);
#endif

   /*! @brief The following function is responsible for sending a Measurement
    * Interval indication to a specified remote device.
    * @param BluetoothStackID The first parameter is the Bluetooth Stack ID of the Bluetooth Device.
    * @param InstanceID The second parameter is the InstanceID returned from a successful call
    * to HTS_Initialize_Server().  @param ConnectionID The third parameter is the
    * ConnectionID of the remote device to send the notification to.
    * @return This function returns a zero if successful or a negative return
    * error code if an error occurs.
    * \note Only 1 Measurement Interval indication may be outstanding
    *          per HTS Instance.
    */
BTPSAPI_DECLARATION int BTPSAPI HTS_Indicate_Measurement_Interval(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int ConnectionID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HTS_Indicate_Measurement_Interval_t)(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int ConnectionID);
#endif

   /*! @brief The following function is responsible for sending a Temperature
    * Measurement indication to a specified remote device.
    * @param BluetoothStackID The first parameter is the Bluetooth Stack ID of the Bluetooth Device.
    * @param InstanceID The second parameter is the InstanceID returned from a successful call
    * to HTS_Initialize_Server().  @param ConnectionID The third parameter is the
    * ConnectionID of the remote device to send the indication to.
    * @param Temperature_Measurement The final parameter is the Temperature Measurement data to indicate.
    * @return This function returns a zero if successful or a negative return
    * error code if an error occurs.
    * \note Only 1 Temperature Measurement indication may be
    *          outstanding per HTS Instance.
    */
BTPSAPI_DECLARATION int BTPSAPI HTS_Indicate_Temperature_Measurement(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int ConnectionID, HTS_Temperature_Measurement_Data_t *Temperature_Measurement);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HTS_Indicate_Temperature_Measurement_t)(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int ConnectionID, HTS_Temperature_Measurement_Data_t *Temperature_Measurement);
#endif

   /*! HTS Client API. */

   /*! @brief The following function is responsible for parsing a value received
    * from a remote HTS Server interpreting it as a Temperature
    * Measurement characteristic.  @param ValueLength The first parameter is the length of
    * the value returned by the remote HTS Server.
    * @param Value The second parameter is a pointer to the value.
    * @param TemperatureMeasurement The third parameter
    * is a pointer to the data returned by the remote HTS Server. @param TemperatureMeasurement The final parameter is a pointer to store the parsed Temperature
    * Measurement value. @return This function returns a zero if successful or
    * a negative return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI HTS_Decode_Temperature_Measurement(unsigned int ValueLength, Byte_t *Value, HTS_Temperature_Measurement_Data_t *TemperatureMeasurement);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HTS_Decode_Temperature_Measurement_t)(unsigned int ValueLength, Byte_t *Value, HTS_Temperature_Measurement_Data_t *TemperatureMeasurement);
#endif

   /*! @brief The following function is responsible for parsing a value received
    * from a remote HTS Server interpreting it as a Valid Range
    * descriptor.  @param ValueLength The first parameter is the length of the value
    * returned by the remote HTS Server.  @param Value The second parameter is a
    * pointer to the data returned by the remote HTS Server.
    * @param ValidRange The final parameter is a pointer to store the parsed Valid Range value.
    * @return This function returns a zero if successful or a negative return
    * error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI HTS_Decode_Valid_Range(unsigned int ValueLength, Byte_t *Value, HTS_Valid_Range_Data_t *ValidRange);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HTS_Decode_Valid_Range_t)(unsigned int ValueLength, Byte_t *Value, HTS_Valid_Range_Data_t *ValidRange);
#endif

#endif
