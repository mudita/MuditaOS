/*
 * Copyright 2011 - 2014 Stonestreet One.
 * All Rights Reserved.
 * Author: Tim Cook
 *** MODIFICATION HISTORY ****************************************************
 *
 *   mm/dd/yy  F. Lastname    Description of Modification
 *   --------  -----------    ------------------------------------------------
 *   11/28/11  T. Cook        Initial creation.
 ****************************************************************************
 */

/**
 *  @file HRSAPI.h
 *
 *  @brief Stonestreet One Bluetooth Heart Rate Service (GATT based) API
 *      Type Definitions, Constants, and Prototypes.
 *      To use any of the following API's,
 *      include the following declaration in your application.
 *
 *  @code
 *  #include "SS1BTHRS.h"
 *  @endcode
 *
 *
 * The Heart Rate Service, HRS, programming interface defines the protocols and procedures to be used to implement HRS capabilities for both Server and Client services.
 *  ============================================================================
 */
#ifndef __HRSAPIH__
#define __HRSAPIH__

#include "SS1BTPS.h"        /*! Bluetooth Stack API Prototypes/Constants. */
#include "SS1BTGAT.h"       /*! Bluetooth Stack GATT API Prototypes/Constants. */
#include "HRSTypes.h"       /*! Heart Rate Service Types/Constants. */

   /*! Error Return Codes.
    * Error Codes that are smaller than these (less than -1000) are
    * related to the Bluetooth Protocol Stack itself (see BTERRORS.H).
    */
#define HRS_ERROR_INVALID_PARAMETER                      (-1000)
#define HRS_ERROR_INVALID_BLUETOOTH_STACK_ID             (-1001)
#define HRS_ERROR_INSUFFICIENT_RESOURCES                 (-1002)
#define HRS_ERROR_SERVICE_ALREADY_REGISTERED             (-1003)
#define HRS_ERROR_INVALID_INSTANCE_ID                    (-1004)
#define HRS_ERROR_MALFORMATTED_DATA                      (-1005)
#define HRS_ERROR_INDICATION_OUTSTANDING                 (-1006)
#define HRS_ERROR_UNKNOWN_ERROR                          (-1007)

   /*! The following flags specifiy that Heart Rate Control Point
    * commands that are to be supported by a Heart Rate instance.  These
    * will be passed into HRS_Initialize_Service().
    */
#define HRS_HEART_RATE_CONTROL_POINT_RESET_ENERGY_EXPENDED_SUPPORTED    0x01

   /*! The followng defines the format of a Heart Rate Measurement.  The
    * first member specifies information on the optional fields that are
    * included in this structure.  The second member is the Heart Rate.
    * The third member is an optional Energy Expended Field.  The fourth
    * member contains the number of entries in the that is specified by
    * the final member.  The final member is the optional RR Interval
    * array.
    * \note The Flags member is made up of bitmasks of the form
    *          HRS_HEART_RATE_MEASUREMENT_FLAGS_XXX.
    * \note The Heart_Rate member, that is a word in the structure,
    *          is truncated to a byte value UNLESS the following flag is
    *          specified in the Flags member:
    *             HRS_HEART_RATE_MEASUREMENT_FLAGS_HEART_RATE_IS_WORD
    * \note The Energy_Expended member of this structure is ignored
    *          UNLESS the following flag is specified in the Flags
    *          member:
    *          HRS_HEART_RATE_MEASUREMENT_FLAGS_ENERGY_EXPENDED_PRESENT
    * \note The Number_Of_RR_Intervals and RR_Intervals members are
    *          ignored UNLESS the following flag is specified in the
    *          Flags member:
    *             HRS_HEART_RATE_MEASUREMENT_FLAGS_RR_INTERVAL_PRESENT
    *          If this flag is specified in the Flags member than there
    *          MUST be at least 1 entry in the RR_Intervals array when
    *          notifying the Heart Rate Measurement value.  When
    *          decoding a received Heart Rate Measurement, this field
    *          may bet set to 0.
    */
typedef struct _tagHRS_Heart_Rate_Measurement_Data_t
{
   Byte_t Flags;
   Word_t Heart_Rate;
   Word_t Energy_Expended;
   Word_t Number_Of_RR_Intervals;
   Word_t RR_Intervals[1];
} HRS_Heart_Rate_Measurement_Data_t;

   /*! The following MACRO is a utility MACRO that exists to aid in
    * determine the amount of memory that is needed to hold a Heart Rate
    * Measurement with a certain number of RR Interval values present.
    * The only parameter to this MACRO is the number of RR Intervals
    * that will be present in the Heart Rate Measurement.
    */
#define HRS_HEART_RATE_MEASUREMENT_DATA_SIZE(_x)         (BTPS_STRUCTURE_OFFSET(HRS_Heart_Rate_Measurement_Data_t, RR_Intervals) + ((_x)*WORD_SIZE))

   /*! The following define the valid Read Request types that a server
    * may receive in a etHRS_Server_Read_Client_Configuration_Request or
    * etHRS_Server_Client_Configuration_Update event.
    * \note For each event it is up to the application to return (or
    *          write) the correct Client Configuration descriptor based
    *          on this value.
    */
typedef enum
{
   ctHeartReateMeasurement
} HRS_Characteristic_Type_t;

   /*! The following defines the valid Heart Rate Control Point commands
    * that may be sent or received.
    */
typedef enum
{
   ccResetEnergyExpended = HRS_HEART_RATE_CONTROL_POINT_RESET_ENERGY_EXPENDED
} HRS_Heart_Rate_Control_Command_t;

   /*! The following enumeration covers all the events generated by the
    * HRS Profile.  These are used to determine the type of each event
    * generated, and to ensure the proper union element is accessed for
    * the HRS_Event_Data_t structure.
    */
typedef enum
{
   etHRS_Server_Read_Client_Configuration_Request, /*!< Dispatched to a HRS Server when a HRS Client is attempting to read a descriptor. */
   etHRS_Server_Client_Configuration_Update, /*!< Dispatched to a HRS Server when a HRS Client has written a Client Configuration descriptor. */
   etHRS_Server_Heart_Rate_Control_Point_Command /*!< Dispatched to a HRS Server when a HRS client sends a request to read Heart Rate data. */
} HRS_Event_Type_t;

   /*! The following structure contains the Handles that will need to be
    * cached by a HRS client in order to only do service discovery once.
    */
typedef struct _tagHRS_Client_Information_t
{
   Word_t Heart_Rate_Measurement;
   Word_t Heart_Rate_Measurement_Client_Configuration;
   Word_t Body_Sensor_Location;
   Word_t Heart_Rate_Control_Point;
} HRS_Client_Information_t;

#define HRS_CLIENT_INFORMATION_DATA_SIZE                (sizeof(HRS_Client_Information_t))

   /*! The following structure contains all of the per Client data that
    * will need to be stored by a HRS Server.
    */
typedef struct _tagHRS_Server_Information_t
{
   Word_t Heart_Rate_Measurement_Client_Configuration;
} HRS_Server_Information_t;

#define HRS_SERVER_INFORMATION_DATA_SIZE                (sizeof(HRS_Server_Information_t))

   /*! The following HRS Profile Event is dispatched to a HRS Server when
    * a HRS Client is attempting to read a descriptor.  The
    * ConnectionID, ConnectionType, and RemoteDevice specifiy the Client
    * that is making the request.  The DescriptorType specifies the
    * Descriptor that the Client is attempting to read.  The
    * TransactionID specifies the TransactionID of the request, this can
    * be used when responding to the request using the
    * HRS_Client_Configuration_Read_Response() API function.
    */
typedef struct _tagHRS_Read_Client_Configuration_Data_t
{
   unsigned int              InstanceID;
   unsigned int              ConnectionID;
   unsigned int              TransactionID;
   GATT_Connection_Type_t    ConnectionType;
   BD_ADDR_t                 RemoteDevice;
   HRS_Characteristic_Type_t ClientConfigurationType;
} HRS_Read_Client_Configuration_Data_t;

#define HRS_READ_CLIENT_CONFIGURATION_DATA_SIZE         (sizeof(HRS_Read_Client_Configuration_Data_t))

   /*! The following HRS Profile Event is dispatched to a HRS Server when
    * a HRS Client has written a Client Configuration descriptor.  The
    * ConnectionID, ConnectionType, and RemoteDevice specifiy the Client
    * that is making the update.  The ClientConfigurationType specifies
    * the Descriptor that the Client is writing.  The final member is
    * the new Client Configuration for the specified characteristic.
    */
typedef struct _tagHRS_Client_Configuration_Update_Data_t
{
   unsigned int              InstanceID;
   unsigned int              ConnectionID;
   GATT_Connection_Type_t    ConnectionType;
   BD_ADDR_t                 RemoteDevice;
   HRS_Characteristic_Type_t ClientConfigurationType;
   Word_t                    ClientConfiguration;
} HRS_Client_Configuration_Update_Data_t;

#define HRS_CLIENT_CONFIGURATION_UPDATE_DATA_SIZE       (sizeof(HRS_Client_Configuration_Update_Data_t))

   /*! The following HRS Profile Event is dispatched to a HRS Server when
    * a HRS Client has sent a Heart Rate Control Point Command.  The
    * ConnectionID, ConnectionType, and RemoteDevice specifiy the Client
    * that is making the update.  @param PARAMFINAL The final parameter specifies the
    * Heart Rate Control Point command that the client sent.
    */
typedef struct _tagHRS_Heart_Rate_Control_Command_Data_t
{
   unsigned int                     InstanceID;
   unsigned int                     ConnectionID;
   GATT_Connection_Type_t           ConnectionType;
   BD_ADDR_t                        RemoteDevice;
   HRS_Heart_Rate_Control_Command_t Command;
} HRS_Heart_Rate_Control_Command_Data_t;

#define HRS_HEART_RATE_CONTROL_COMMAND_DATA_SIZE         (sizeof(HRS_Heart_Rate_Control_Command_Data_t))

   /*! The following structure represents the container structure for
    * holding all HRS Profile Event Data.  This structure is received
    * for each event generated.  The Event_Data_Type member is used to
    * determine the appropriate union member element to access the
    * contained data.  The Event_Data_Size member contains the total
    * size of the data contained in this event.
    */
typedef struct _tagHRS_Event_Data_t
{
   HRS_Event_Type_t Event_Data_Type;
   Word_t           Event_Data_Size;
   union
   {
      HRS_Read_Client_Configuration_Data_t   *HRS_Read_Client_Configuration_Data;
      HRS_Client_Configuration_Update_Data_t *HRS_Client_Configuration_Update_Data;
      HRS_Heart_Rate_Control_Command_Data_t  *HRS_Heart_Rate_Control_Command_Data;
   } Event_Data;
} HRS_Event_Data_t;

#define HRS_EVENT_DATA_SIZE                             (sizeof(HRS_Event_Data_t))

   /*! The following declared type represents the Prototype Function for
    * a HRS Profile Event Receive Data Callback.  This function will be
    * called whenever an HRS Profile Event occurs that is associated
    * with the specified Bluetooth Stack ID. @param BluetoothStackID This function passes to
    * the caller the Bluetooth Stack ID. @param HRS_Event_Data the HRS Event Data that
    * occurred. @param CallbackParameter The HRS Profile Event Callback Parameter that was
    * specified when this Callback was installed.  The caller is free to
    * use the contents of the HRS Profile Event Data ONLY in the context
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
    * possible (this argument holds anyway because another HRS Profile
    * Event will not be processed while this function call is
    * outstanding).
    * \note This function MUST NOT Block and wait for events that
    *            can only be satisfied by Receiving HRS Profile Event
    *            Packets.  A Deadlock WILL occur because NO HRS Event
    *            Callbacks will be issued while this function is
    *            currently outstanding.
    */
typedef void (BTPSAPI *HRS_Event_Callback_t)(unsigned int BluetoothStackID, HRS_Event_Data_t *HRS_Event_Data, unsigned long CallbackParameter);

   /*! HRS Server API. */

   /*! @brief The following function is responsible for opening a HRS Server.
    * @param BluetoothStackID The first parameter is the Bluetooth Stack ID on which to open the
    * server.  @param Supported_Commands The second parameter is the mask of supported Heart Rate
    * Control Point commands.  @param EventCallback The third parameter is the Callback
    * function to call when an event occurs on this Server Port.
    * @param CallbackParameter The fourth parameter is a user-defined callback parameter that will be
    * passed to the callback function with each event.
    * @param ServiceID The final parameter is a pointer to store the GATT Service ID of the
    * registered HRS service.  This can be used to include the service
    * registered by this call. @return This function returns the positive,
    * non-zero, Instance ID or a negative error code.
    * \note Only 1 HRS Server may be open at a time, per Bluetooth
    *          Stack ID.
    * \note The Supported_Commands parameter must be made up of bit
    *          masks of the form:
    *             HRS_HEART_RATE_CONTROL_POINT_XXX_SUPPORTED
    * \note All Client Requests will be dispatch to the EventCallback
    *          function that is specified by the second parameter to
    *          this function.
    */
BTPSAPI_DECLARATION int BTPSAPI HRS_Initialize_Service(unsigned int BluetoothStackID, unsigned long Supported_Commands, HRS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HRS_Initialize_Service_t)(unsigned int BluetoothStackID, unsigned long Supported_Commands, HRS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID);
#endif

   /*! @brief The following function is responsible for opening a HRS Server.
    * @param BluetoothStackID The first parameter is the Bluetooth Stack ID on which to open the
    * server.  @param Supported_Commands The second parameter is the mask of supported Heart Rate
    * Control Point commands.  @param EventCallback The third parameter is the Callback
    * function to call when an event occurs on this Server Port.
    * @param CallbackParameter The fourth parameter is a user-defined callback parameter that will be
    * passed to the callback function with each event. @param ServiceID The fifth
    * parameter is a pointer to store the GATT Service ID of the
    * registered HRS service.  This can be used to include the service
    * registered by this call.  @param ServiceHandleRange The final parameter is a pointer, that
    * on input can be used to control the location of the service in the
    * GATT database, and on ouput to store the service handle range.
    * @return This function returns the positive, non-zero, Instance ID or a
    * negative error code.
    * \note Only 1 HRS Server may be open at a time, per Bluetooth
    *          Stack ID.
    * \note The Supported_Commands parameter must be made up of bit
    *          masks of the form:
    *          HRS_HEART_RATE_CONTROL_POINT_XXX_SUPPORTED
    * \note All Client Requests will be dispatch to the EventCallback
    *          function that is specified by the second parameter to
    *          this function.
    */
BTPSAPI_DECLARATION int BTPSAPI HRS_Initialize_Service_Handle_Range(unsigned int BluetoothStackID, unsigned long Supported_Commands, HRS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HRS_Initialize_Service_Handle_Range_t)(unsigned int BluetoothStackID, unsigned long Supported_Commands, HRS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange);
#endif

   /*! @brief The following function is responsible for closing a previously
    * opened HRS Server.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID
    * on which to close the server. @param InstanceID The second parameter is the
    * InstanceID that was returned from a successful call to
    * HRS_Initialize_Service(). @return This function returns a zero if
    * successful or a negative return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI HRS_Cleanup_Service(unsigned int BluetoothStackID, unsigned int InstanceID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HRS_Cleanup_Service_t)(unsigned int BluetoothStackID, unsigned int InstanceID);
#endif

   /*! @brief The following function is responsible for querying the number of
    * attributes that are contained in the HRS Service that is
    * registered with a call to HRS_Initialize_Service().
    * @return This function returns the non-zero number of attributes that are contained in a
    * HRS Server or zero on failure.
    */
BTPSAPI_DECLARATION unsigned int BTPSAPI HRS_Query_Number_Attributes(void);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef unsigned int (BTPSAPI *PFN_HRS_Query_Number_Attributes_t)(void);
#endif

   /*! @brief The following function is responsible for setting the Body Sensor
    * Location on the specified HRS Instance.  @param BluetoothStackID The first parameter is
    * the Bluetooth Stack ID of the Bluetooth Device.
    * @param InstanceID The second parameter is the InstanceID returned from a successful call to
    * HRS_Initialize_Server().  @param Body_Sensor_Location The final parameter is the Body Sensor
    * Location to set for the specified HRS Instance.
    * @return This function returns a zero if successful or a negative return error code if an
    * error occurs.
    * \note The Body_Sensor_Location parameter should be an
    *          enumerated value of the form
    *          HRS_BODY_SENSOR_LOCATION_XXX.
    */
BTPSAPI_DECLARATION int BTPSAPI HRS_Set_Body_Sensor_Location(unsigned int BluetoothStackID, unsigned int InstanceID, Byte_t Body_Sensor_Location);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HRS_Set_Body_Sensor_Location_t)(unsigned int BluetoothStackID, unsigned int InstanceID, Byte_t Body_Sensor_Location);
#endif

   /*! @brief The following function is responsible for querying the current
    * Temperature Type on the specified HRS Instance.
    * @param BluetoothStackID The first parameter is the Bluetooth Stack ID of the Bluetooth Device.
    * @param InstanceID The second parameter is the InstanceID returned from a successful call
    * to HRS_Initialize_Server().
    * @param Body_Sensor_Location The final parameter is a pointer to
    * return the current Body Sensor Location for the specified HRS
    * Instance.  @return This function returns a zero if successful or a
    * negative return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI HRS_Query_Body_Sensor_Location(unsigned int BluetoothStackID, unsigned int InstanceID, Byte_t *Body_Sensor_Location);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HRS_Query_Body_Sensor_Location_t)(unsigned int BluetoothStackID, unsigned int InstanceID, Byte_t *Body_Sensor_Location);
#endif

   /*! @brief The following function is responsible for responding to a HRS Read
    * Client Configuration Request.  @param BluetoothStackID The first parameter is the
    * Bluetooth Stack ID of the Bluetooth Device.  @param InstanceID The second parameter
    * is the InstanceID returned from a successful call to
    * HRS_Initialize_Server(). @param TransactionID The third parameter is the Transaction ID of the
    * request.  @param Client_Configuration The final parameter contains the Client Configuration to
    * send to the remote device.  @return This function returns a zero if
    * successful or a negative return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI HRS_Read_Client_Configuration_Response(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int TransactionID, Word_t Client_Configuration);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HRS_Read_Client_Configuration_Response_t)(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int TransactionID, Word_t Client_Configuration);
#endif

   /*! @brief The following function is responsible for sending a Heart Rate
    * Measurement notification to a specified remote device.
    * @param BluetoothStackID The first parameter is the Bluetooth Stack ID of the Bluetooth Device.
    * @param InstanceID The second parameter is the InstanceID returned from a successful call
    * to HRS_Initialize_Server().  @param ConnectionID The third parameter is the
    * ConnectionID of the remote device to send the notification to.
    * @param Heart_Rate_Measurement The final parameter is the Heart Rate Measurement data to notify.
    * @return This function returns a zero if successful or a negative return
    * error code if an error occurs.
    * \note IF the following flag:
    *             HRS_HEART_RATE_MEASUREMENT_FLAGS_RR_INTERVAL_PRESENT
    *          is set in the Flags member of the Heart_Rate_Measurement
    *          parameter, them the Number_Of_RR_Intervals member of the
    *          Heart_Rate_Measurement parameter on INPUT must contain
    *          the number of RR Intervals values that are contained in
    *          the RR_Intervals member of the Heart_Rate_Measurement
    *          parameter.  If @return This function returns success then on
    *          OUTPUT the Number_Of_RR_Intervals member will contain the
    *          total number of RR Interval values that were formatted
    *          into the notification (this is guaranteed to be less than
    *          or equal to the value of Number_Of_RR_Intervals on
    *          INPUT).
    */
BTPSAPI_DECLARATION int BTPSAPI HRS_Notify_Heart_Rate_Measurement(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int ConnectionID, HRS_Heart_Rate_Measurement_Data_t *Heart_Rate_Measurement);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HRS_Notify_Heart_Rate_Measurement_t)(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int ConnectionID, HRS_Heart_Rate_Measurement_Data_t *Heart_Rate_Measurement);
#endif

   /*! HRS Client API. */

   /*! @brief The following function is responsible for parsing a value received
    * from a remote HRS Server interpreting it as a Heart Rate
    * Measurement characteristic.  @param ValueLength The first parameter is the length of
    * the value returned by the remote HRS Server.  @param Value The second parameter
    * is a pointer to the data returned by the remote HRS Server.
    * @param HeartRateMeasurement The final parameter is a pointer to store the parsed Temperature
    * Measurement value.  @return This function returns a zero if successful or
    * a negative return error code if an error occurs.
    * \note On INPUT the Number_Of_RR_Intervals member of the
    *          HeartRateMeasurement parameter must contain the number of
    *          entries in the RR_Intervals array.  On RETURN this
    *          parameter will contain the actual number of RR Interval
    *          values that were parsed from the Heart Rate Measurement
    *          value (which will always be less than or equal to the
    *          number entries allocated in the structure).
    * \note It is possible to query the total number of RR Interval
    *          values in the Heart Rate Measurement value by passing 0
    *          for the Number_Of_RR_Intervals member of the
    *          HeartRateMeasurement parameter.  In this case on return
    *          the Number_Of_RR_Intervals member will contain the total
    *          number of RR Interval values in the Heart Rate
    *          Measurement value BUT no RR Intervals will be parsed into
    *          the HeartRateMeasurement structure.
    */
BTPSAPI_DECLARATION int BTPSAPI HRS_Decode_Heart_Rate_Measurement(unsigned int ValueLength, Byte_t *Value, HRS_Heart_Rate_Measurement_Data_t *HeartRateMeasurement);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HRS_Decode_Heart_Rate_Measurement_t)(unsigned int ValueLength, Byte_t *Value, HRS_Heart_Rate_Measurement_Data_t *HeartRateMeasurement);
#endif

   /*! @brief The following function is responsible for parsing a value received
    * from a remote HRS Server interpreting it as a Body Sensor Location
    * value.  @param ValueLength The first parameter is the length of the value returned by
    * the remote HRS Server.  @param Value The second parameter is a pointer to the
    * data returned by the remote HRS Server.
    * @param BodySensorLocation The final parameter is a
    * pointer to store the parsed Body Sensor Location value.
    * @return This function returns a zero if successful or a negative return error
    * code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI HRS_Decode_Body_Sensor_Location(unsigned int ValueLength, Byte_t *Value, Byte_t *BodySensorLocation);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HRS_Decode_Body_Sensor_Location_t)(unsigned int ValueLength, Byte_t *Value, Byte_t *BodySensorLocation);
#endif

   /*! @brief The following function is responsible for formatting a Heart Rate
    * Control Command into a user specified buffer.  @param Command The first parameter
    * is the command to format. @param BufferLength The second parameter contains the length of the buffer.
    * @param Buffer The final parameter is a pointer to the buffer to format the command into.
    * @return This function returns a zero if successful or a negative return
    * error code if an error occurs.
    * \note The BufferLength and Buffer parameter must point to a
    *          buffer of at least
    *          HRS_HEART_RATE_CONTROL_POINT_VALUE_LENGTH in size.
    */
BTPSAPI_DECLARATION int BTPSAPI HRS_Format_Heart_Rate_Control_Command(HRS_Heart_Rate_Control_Command_t Command, unsigned int BufferLength, Byte_t *Buffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HRS_Format_Heart_Rate_Control_Command_t)(HRS_Heart_Rate_Control_Command_t Command, unsigned int BufferLength, Byte_t *Buffer);
#endif

#endif
