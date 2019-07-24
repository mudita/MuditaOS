/*
 * Copyright 2011 - 2014 Stonestreet One.
 * All Rights Reserved.
 * Author:  Tim Cook
 *** MODIFICATION HISTORY ****************************************************
 *
 *   mm/dd/yy  F. Lastname    Description of Modification
 *   --------  -----------    ------------------------------------------------
 *   09/26/11  T. Cook        Initial creation.
 ****************************************************************************
 */

/**
 *  @file ANSAPI.h
 *
 *  @brief Stonestreet One Bluetooth Alert Notification Service (GATT
 *      based) API Type Definitions, Constants, and Prototypes.
 *      To use any of the following API's,
 *      include the following declaration in your application.
 *
 *  @code
 *  #include "SS1BTANS.h"
 *  @endcode
 *
 *  The Alert Notification Service programming interface defines the protocols and procedures to be used to implement Alert Notification Service capabilities.
 *  ============================================================================
 */
#ifndef __ANSAPIH__
#define __ANSAPIH__

#include "SS1BTPS.h"       /*! Bluetooth Stack API Prototypes/Constants. */
#include "SS1BTGAT.h"      /*! Bluetooth Stack GATT API Prototypes/Constants. */
#include "ANSTypes.h"      /*! Alert Notification Service Types/Constants. */

   /*! Error Return Codes.
    * Error Codes that are smaller than these (less than -1000) are
    * related to the Bluetooth Protocol Stack itself (see BTERRORS.H).
    */
#define ANS_ERROR_INVALID_PARAMETER                      (-1000)
#define ANS_ERROR_INSUFFICIENT_RESOURCES                 (-1001)
#define ANS_ERROR_SERVICE_ALREADY_REGISTERED             (-1002)
#define ANS_ERROR_INVALID_INSTANCE_ID                    (-1003)
#define ANS_ERROR_MALFORMATTED_DATA                      (-1004)
#define ANS_ERROR_UNKNOWN_ERROR                          (-1005)

   /*! The following structure defines the valid Support Categories bits
    * that may be set to specify the supported New Alert and Unread
    * Alert Status categories.
    */
#define ANS_SUPPORTED_CATEGORIES_SIMPLE_ALERT            0x0001
#define ANS_SUPPORTED_CATEGORIES_EMAIL                   0x0002
#define ANS_SUPPORTED_CATEGORIES_NEWS                    0x0004
#define ANS_SUPPORTED_CATEGORIES_CALL                    0x0008
#define ANS_SUPPORTED_CATEGORIES_MISSED_CALL             0x0010
#define ANS_SUPPORTED_CATEGORIES_SMS_MMS                 0x0020
#define ANS_SUPPORTED_CATEGORIES_VOICE_MAIL              0x0040
#define ANS_SUPPORTED_CATEGORIES_SCHEDULE                0x0080
#define ANS_SUPPORTED_CATEGORIES_HIGH_PRIORITY_ALERT     0x0100
#define ANS_SUPPORTED_CATEGORIES_INSTANT_MESSAGE         0x0200

   /*! The following enumerated type represents all of the valid commands
    * that may be received in an
    * etANS_Server_Control_Point_Command_Indication Server event OR that
    * may be written to a remote ANS Server.
    */
typedef enum
{
   pcEnable_New_Alert_Notifications        = ANS_COMMAND_ID_ENABLE_NEW_INCOMING_ALERT_NOTIFICATION,
   pcEnable_Unread_Category_Notifications  = ANS_COMMAND_ID_ENABLE_UNREAD_CATEGORY_STATUS_NOTIFICATION,
   pcDisable_New_Alert_Notifications       = ANS_COMMAND_ID_DISABLE_NEW_INCOMING_ALERT_NOTIFICATION,
   pcDisable_Unread_Category_Notifications = ANS_COMMAND_ID_DISABLE_UNREAD_CATEGORY_STATUS_NOTIFICATION,
   pcNotify_New_Alert_Immediately          = ANS_COMMAND_ID_NOTIFY_NEW_INCOMING_ALERT_IMMEDIATELY,
   pcNotify_Unread_Category_Immediately    = ANS_COMMAND_ID_NOTIFY_UNREAD_CATEGORY_STATUS_IMMEDIATELY
} ANS_Control_Point_Command_t;

   /*! The following enumerated type represents all of the Category
    * Identifications that may be received in an
    * etANS_Server_Control_Point_Command_Indication Server event OR that
    * may be written to a remote ANS Server.
    */
typedef enum
{
  ciSimpleAlert       = ANS_ALERT_CATEGORY_ID_SIMPLE_ALERT,
  ciEmail             = ANS_ALERT_CATEGORY_ID_EMAIL,
  ciNews              = ANS_ALERT_CATEGORY_ID_NEWS,
  ciCall              = ANS_ALERT_CATEGORY_ID_CALL,
  ciMissedCall        = ANS_ALERT_CATEGORY_ID_MISSED_CALL,
  ciSMS_MMS           = ANS_ALERT_CATEGORY_ID_SMS_MMS,
  ciVoiceMail         = ANS_ALERT_CATEGORY_ID_VOICE_MAIL,
  ciSchedule          = ANS_ALERT_CATEGORY_ID_SCHEDULE,
  ciHighPriorityAlert = ANS_ALERT_CATEGORY_ID_HIGH_PRIORITY_ALERT,
  ciInstantMessage    = ANS_ALERT_CATEGORY_ID_INSTANT_MESSAGE,
  ciAllCategories     = ANS_ALERT_CATEGORY_ID_ALL_CATEGORIES
} ANS_Category_Identification_t;

   /*! The following enumerated type defines the valid Read Request types
    * that a server may receive in a
    * etANS_Server_Read_Client_Configuration_Request or
    * etANS_Server_Client_Configuration_Update event.
    * \note For each event it is up to the application to return (or
    *          write) the correct Client Configuration descriptor based
    *          on this value.
    */
typedef enum
{
   ctNewAlert,
   ctUnreadAlertStatus
} ANS_Characteristic_Type_t;

   /*! The following enumerated type defines the valid Supported
    * Categories type that may be set in ANS_Set_Supported_Categories
    * and ANS_Query_Supported_Categories API function.
    */
typedef enum
{
   scNewAlert,
   scUnreadAlertStatus
} ANS_Supported_Categories_Type_t;

   /*! The following structure contains the Handles that will need to be
    * cached by a ANS client in order to only do service discovery once.
    */
typedef struct _tagANS_Client_Information_t
{
   Word_t Supported_New_Alert_Category;
   Word_t New_Alert;
   Word_t New_Alert_Client_Configuration;
   Word_t Supported_Unread_Alert_Category;
   Word_t Unread_Alert_Status;
   Word_t Unread_Alert_Status_Client_Configuration;
   Word_t Control_Point;
} ANS_Client_Information_t;

#define ANS_CLIENT_INFORMATION_DATA_SIZE                (sizeof(ANS_Client_Information_t))

   /*! The following structure contains all of the per Client data that
    * will need to be stored by a ANS Server.
    */
typedef struct _tagANS_Server_Information_t
{
   Boolean_t New_Alert_Client_Configuration;
   Boolean_t Unread_Alert_Status_Client_Configuration;
   Word_t    Enabled_New_Alert_Categories;
   Word_t    Enabled_Unread_Alert_Categories;
} ANS_Server_Information_t;

#define ANS_SERVER_INFORMATION_DATA_SIZE                (sizeof(ANS_Server_Information_t))

   /*! The following MACRO is a utility MACRO that exists to aid in
    * enabling a specific ANS Category.  The first parameter to this
    * MACRO should be a Word_t pointer to enable a category in. The second parameter should be the category to enable.
    * \note the second parameter MUST be of the form:
    *                   ANS_ALERT_CATEGORY_ID_XXX
    */
#define ANS_SERVER_INFORMATION_ENABLE_CATEGORY(_x, _y)   \
   (((Word_t *)(_x))[0]) |= (((_y) != ANS_ALERT_CATEGORY_ID_ALL_CATEGORIES)?((Word_t)((0x0001) << ((_y) % (WORD_SIZE*8)))):((Word_t)0xFFFF))

   /*! The following MACRO is a utility MACRO that exists to aid in
    * disabling a specific ANS Category. The first parameter to this
    * MACRO should be a Word_t pointer to disable a category in. The second parameter should be the category to enable.
    * \note The second parameter MUST be of the form:
    *                   ANS_ALERT_CATEGORY_ID_XXX
    */
#define ANS_SERVER_INFORMATION_DISABLE_CATEGORY(_x, _y)  \
   (((Word_t *)(_x))[0]) &= (((_y) != ANS_ALERT_CATEGORY_ID_ALL_CATEGORIES)?((Word_t)~((0x0001) << ((_y) % (WORD_SIZE*8)))):((Word_t)0x0000))

   /*! The following MACRO is a utility MACRO that exists to aid in
    * determining if a specific ANS Category is enabled. The first parameter to this MACRO should be a Word_t pointer to test for the
    * category. The second parameter should be the category to check.
    * This MACRO returns a Boolean_t TRUE if the category is enabled or
    * FALSE otherwise.
    * \note The second parameter MUST be of the form:
    *                   ANS_ALERT_CATEGORY_ID_XXX
    */
#define ANS_SERVER_INFORMATION_CATEGORY_ENABLED(_x, _y)  \
   ((((Word_t *)(_x))[0]) & ((0x0001) << ((_y) % (WORD_SIZE*8))))

   /*! The following represents to the structure of a New Alert.  This is
    * used to notify a remote ANS Client of a New Alert in a specified
    * category.
    * \note The LastAlertString member is optional and may be set to
    *          NULL.
    * \note CategoryID MAY NOT be set to ciAllCategories in this
    *          structure.
    */
typedef struct _tagANS_New_Alert_Data_t
{
   ANS_Category_Identification_t  CategoryID;
   Byte_t                         NumberOfNewAlerts;
   char                          *LastAlertString;
} ANS_New_Alert_Data_t;

#define ANS_NEW_ALERT_DATA_SIZE                          (sizeof(ANS_New_Alert_Data_t))

   /*! The following represents to the structure of an Unread Alert.
    * This is used to notify a remote ANS Client of a Unread Alert in a
    * specified category.
    * \note CategoryID MAY NOT be set to ciAllCategories in this
    *          structure.
    */
typedef struct _tagANS_Un_Read_Alert_Data_t
{
   ANS_Category_Identification_t CategoryID;
   Byte_t                        NumberOfUnreadAlerts;
} ANS_Un_Read_Alert_Data_t;

#define ANS_UN_READ_ALERT_DATA_SIZE                      (sizeof(ANS_Un_Read_Alert_Data_t))

   /*! The following enumeration covers all the events generated by the
    * ANS Profile.  These are used to determine the type of each event
    * generated, and to ensure the proper union element is accessed for
    * the ANS_Event_Data_t structure.
    */
typedef enum
{
   etANS_Server_Read_Client_Configuration_Request, /*!< Dispatched when an ANS Client requests to read Client Configuration Descriptor from a registered ANS Server. */
   etANS_Server_Client_Configuration_Update, /*!< Dispatched when an ANS Client requests to update Client Configuration Descriptor on to a registered ANS Server. */
   etANS_Server_Control_Point_Command_Indication /*!< Dispatched to a ANS Server in response to the reception of request from  ANS Client to write to the Control Point command. */
} ANS_Event_Type_t;

   /*! The following ANS Profile Event is dispatched to a ANS Server when
    * a ANS Client is attempting to read a descriptor.  The
    * ConnectionID, ConnectionType, and RemoteDevice specifiy the Client
    * that is making the request.  The DescriptorType specifies the
    * Descriptor that the Client is attempting to read.  The
    * TransactionID specifies the TransactionID of the request, this can
    * be used when responding to the request using the
    * ANS_Client_Configuration_Read_Response() API function.
    */
typedef struct _tagANS_Read_Client_Configuration_Data_t
{
   unsigned int              InstanceID;
   unsigned int              ConnectionID;
   unsigned int              TransactionID;
   GATT_Connection_Type_t    ConnectionType;
   BD_ADDR_t                 RemoteDevice;
   ANS_Characteristic_Type_t ClientConfigurationType;
} ANS_Read_Client_Configuration_Data_t;

#define ANS_READ_CLIENT_CONFIGURATION_DATA_SIZE          (sizeof(ANS_Read_Client_Configuration_Data_t))

   /*! The following ANS Profile Event is dispatched to a ANS Server when
    * a ANS Client has written a Client Configuration descriptor.  The
    * ConnectionID, ConnectionType, and RemoteDevice specifiy the Client
    * that is making the update.  The DescriptorType specifies the
    * Descriptor that the Client is writing.
    */
typedef struct _tagANS_Client_Configuration_Update_Data_t
{
   unsigned int              InstanceID;
   unsigned int              ConnectionID;
   GATT_Connection_Type_t    ConnectionType;
   BD_ADDR_t                 RemoteDevice;
   ANS_Characteristic_Type_t ClientConfigurationType;
   Boolean_t                 NotificationsEnabled;
} ANS_Client_Configuration_Update_Data_t;

#define ANS_CLIENT_CONFIGURATION_UPDATE_DATA_SIZE        (sizeof(ANS_Client_Configuration_Update_Data_t))

   /*! The following ANS Profile Event is dispatched to a ANS Server when
    * a ANS Client has written a command to the ANS Control Point.  The
    * ConnectionID, ConnectionType, and RemoteDevice specifiy the Client
    * that is making the update.  The Command specifies the command that
    * the Client is sending and the Category specifies the category that
    * the command applies to.
    */
typedef struct _tagANS_Control_Point_Command_Data_t
{
   unsigned int                  InstanceID;
   unsigned int                  ConnectionID;
   GATT_Connection_Type_t        ConnectionType;
   BD_ADDR_t                     RemoteDevice;
   ANS_Control_Point_Command_t   Command;
   ANS_Category_Identification_t Category;
} ANS_Control_Point_Command_Data_t;

#define ANS_CONTROL_POINT_COMMAND_DATA_SIZE              (sizeof(ANS_Control_Point_Command_Data_t))

   /*! The following structure represents the container structure for
    * holding all ANS Profile Event Data.  This structure is received
    * for each event generated.  The Event_Data_Type member is used to
    * determine the appropriate union member element to access the
    * contained data.  The Event_Data_Size member contains the total
    * size of the data contained in this event.
    */
typedef struct _tagANS_Event_Data_t
{
   ANS_Event_Type_t Event_Data_Type;
   Word_t           Event_Data_Size;
   union
   {
      ANS_Read_Client_Configuration_Data_t   *ANS_Read_Client_Configuration_Data;
      ANS_Client_Configuration_Update_Data_t *ANS_Client_Configuration_Update_Data;
      ANS_Control_Point_Command_Data_t       *ANS_Control_Point_Command_Data;
   } Event_Data;
} ANS_Event_Data_t;

#define ANS_EVENT_DATA_SIZE                             (sizeof(ANS_Event_Data_t))

   /*! The following declared type represents the Prototype Function for
    * a ANS Profile Event Receive Data Callback.  This function will be
    * called whenever an ANS Profile Event occurs that is associated
    * with the specified Bluetooth Stack ID. @param BluetoothStackID This function passes to
    * the caller the Bluetooth Stack ID. @param ANS_Event_Data The ANS Event Data that
    * occurred. @param CallbackParameter The ANS Profile Event Callback Parameter that was
    * specified when this Callback was installed.  The caller is free to
    * use the contents of the ANS Profile Event Data ONLY in the context
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
    * possible (this argument holds anyway because another ANS Profile
    * Event will not be processed while this function call is
    * outstanding).
    * \note This function MUST NOT Block and wait for events that
    *            can only be satisfied by Receiving ANS Profile Event
    *            Packets.  A Deadlock WILL occur because NO ANS Event
    *            Callbacks will be issued while this function is
    *            currently outstanding.
    */
typedef void (BTPSAPI *ANS_Event_Callback_t)(unsigned int BluetoothStackID, ANS_Event_Data_t *ANS_Event_Data, unsigned long CallbackParameter);

   /*! ANS Server API. */

   /*! @brief The following function is responsible for opening a ANS Server.
    * @param BluetoothStackID The first parameter is the Bluetooth Stack ID on which to open the
    * server.  @param EventCallback The second parameter is the Callback function to call
    * when an event occurs on this Server Port.  @param CallbackParameter The third parameter is
    * a user-defined callback parameter that will be passed to the
    * callback function with each event.  @param ServiceID The final parameter is a
    * pointer to store the GATT Service ID of the registered ANS
    * service. This can be used to include the service registered by
    * this call.  @return This function returns the positive, non-zero, Instance
    * ID or a negative error code.
    * \note Only 1 ANS Server may be open at a time, per Bluetooth
    *          Stack ID.
    * \note All Client Requests will be dispatch to the EventCallback
    *          function that is specified by the second parameter to
    *          this function.
    */
BTPSAPI_DECLARATION int BTPSAPI ANS_Initialize_Service(unsigned int BluetoothStackID, ANS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_ANS_Initialize_Service_t)(unsigned int BluetoothStackID, ANS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID);
#endif

   /*! @brief The following function is responsible for opening a ANS Server.
    * @param BluetoothStackID The first parameter is the Bluetooth Stack ID on which to open the
    * server.  @param EventCallback The second parameter is the Callback function to call
    * when an event occurs on this Server Port.  @param CallbackParameter The third parameter is
    * a user-defined callback parameter that will be passed to the
    * callback function with each event.  @param ServiceID The fourth parameter is a
    * pointer to store the GATT Service ID of the registered ANS
    * service.  This can be used to include the service registered by
    * this call.  @param ServiceHandleRange The final parameter is a pointer, that on input can be
    * used to control the location of the service in the GATT database,
    * and on ouput to store the service handle range.
    * @return This function returns the positive, non-zero, Instance ID or a negative error
    * code.
    * \note Only 1 ANS Server may be open at a time, per Bluetooth
    *          Stack ID.
    * \note All Client Requests will be dispatch to the EventCallback
    *          function that is specified by the second parameter to
    *          this function.
    */
BTPSAPI_DECLARATION int BTPSAPI ANS_Initialize_Service_Handle_Range(unsigned int BluetoothStackID, ANS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_ANS_Initialize_Service_Handle_Range_t)(unsigned int BluetoothStackID, ANS_Event_Callback_t EventCallback, unsigned long CallbackParameter, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange);
#endif

   /*! @brief The following function is responsible for closing a previously ANS
    * Server.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID on which to
    * close the server.  @param InstanceID The second parameter is the InstanceID that was
    * returned from a successful call to ANS_Initialize_Service().
    * @return This function returns a zero if successful or a negative return
    * error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI ANS_Cleanup_Service(unsigned int BluetoothStackID, unsigned int InstanceID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_ANS_Cleanup_Service_t)(unsigned int BluetoothStackID, unsigned int InstanceID);
#endif

   /*! @brief The following function is responsible for querying the number of
    * attributes that are contained in the ANS Service that is
    * registered with a call to ANS_Initialize_Service().
    * @return This function returns the non-zero number of attributes that are contained in an
    * ANS Server or zero on failure.
    */
BTPSAPI_DECLARATION unsigned int BTPSAPI ANS_Query_Number_Attributes(void);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef unsigned int (BTPSAPI *PFN_ANS_Query_Number_Attributes_t)(void);
#endif

   /*! @brief The following function is responsible for setting the Alert
    * Notification Supported Categories for the specified Category Type
    * on the specified ANS Instance. @param BluetoothStackID The first parameter is the
    * Bluetooth Stack ID of the Bluetooth Device. @param InstanceID The second parameter
    * is the InstanceID returned from a successful call to
    * ANS_Initialize_Server(). @param SupportedCategoryType The third parameter specifies the
    * Category to set the Supported Categories for.  @param SupportedCategoriesMask The final parameter
    * is the Supported Categories bit mask to set as the supported
    * categories for the specified ANS Instance. @return This function returns
    * a zero if successful or a negative return error code if an error
    * occurs.
    * \note The SupportedCategoriesMask is a bit mask that is made up
    *          of bit masks of the form ANS_SUPPORTED_CATEGORIES_XXX.
    */
BTPSAPI_DECLARATION int BTPSAPI ANS_Set_Supported_Categories(unsigned int BluetoothStackID, unsigned int InstanceID, ANS_Supported_Categories_Type_t SupportedCategoryType, Word_t SupportedCategoriesMask);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_ANS_Set_Supported_Categories_t)(unsigned int BluetoothStackID, unsigned int InstanceID, ANS_Supported_Categories_Type_t SupportedCategoryType, Word_t SupportedCategoriesMask);
#endif

   /*! @brief The following function is responsible for setting the Alert
    * Notification Supported Categories for the specified Category Type
    * on the specified ANS Instance.  @param BluetoothStackID The first parameter is the
    * Bluetooth Stack ID of the Bluetooth Device. @param InstanceID The second parameter
    * is the InstanceID returned from a successful call to
    * ANS_Initialize_Server(). @param SupportedCategoryType The third parameter specifies the
    * Category to query the Supported Categories for.
    * @param SupportedCategoriesMask The final parameter is a pointer to store the Supported Categories bit mask
    * for the specified ANS Instance. @return This function returns a zero if
    * successful or a negative return error code if an error occurs.
    * \note The SupportedCategoriesMask is a pointer to a bit mask
    *          that will be made up of bit masks of the form
    *          ANS_SUPPORTED_CATEGORIES_XXX, if this function returns
    *          success.
    */
BTPSAPI_DECLARATION int BTPSAPI ANS_Query_Supported_Categories(unsigned int BluetoothStackID, unsigned int InstanceID, ANS_Supported_Categories_Type_t SupportedCategoryType, Word_t *SupportedCategoriesMask);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_ANS_Query_Supported_Categories_t)(unsigned int BluetoothStackID, unsigned int InstanceID, ANS_Supported_Categories_Type_t SupportedCategoryType, Word_t *SupportedCategoriesMask);
#endif

   /*! @brief The following function is responsible for responding to a ANS Read
    * Client Configuration Request.  @param BluetoothStackID The first parameter is the
    * Bluetooth Stack ID of the Bluetooth Device.  @param InstanceID The second parameter
    * is the InstanceID returned from a successful call to
    * ANS_Initialize_Server(). @param TransactionID The third is the Transaction ID of the
    * request. @param NotificationsEnabled The final parameter contains the Client Configuration to
    * send to the remote device.  @return This function returns a zero if
    * successful or a negative return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI ANS_Read_Client_Configuration_Response(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int TransactionID, Boolean_t NotificationsEnabled);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_ANS_Read_Client_Configuration_Response_t)(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int TransactionID, Boolean_t NotificationsEnabled);
#endif

   /*! @brief The following function is responsible for sending a New Alert
    * notification to a specified remote device.  @param BluetoothStackID The first parameter is
    * the Bluetooth Stack ID of the Bluetooth Device.
    * @param InstanceID The second parameter is the InstanceID returned from a successful call to
    * ANS_Initialize_Server().  @param ConnectionID The third parameter is the ConnectionID
    * of the remote device to send the notification to.
    * @param NewAlert The final parameter is the New Alert data to notify.
    * @return This function returns
    * a zero if successful or a negative return error code if an error
    * occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI ANS_New_Alert_Notification(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int ConnectionID, ANS_New_Alert_Data_t *NewAlert);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_ANS_New_Alert_Notification_t)(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int ConnectionID, ANS_New_Alert_Data_t *NewAlert);
#endif

   /*! @brief The following function is responsible for sending a UnRead Alert
    * Status notification to a specified remote device.
    * @param BluetoothStackID The first parameter is the Bluetooth Stack ID of the Bluetooth Device.
    * @param InstanceID The second parameter is the InstanceID returned from a successful call
    * to ANS_Initialize_Server().  @param ConnectionID The third parameter is the
    * ConnectionID of the remote device to send the notification to.
    * @param UnReadAlert The final parameter is the UnRead Alert data to notify.
    * @return This function returns a zero if successful or a negative return error
    * code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI ANS_Un_Read_Alert_Status_Notification(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int ConnectionID, ANS_Un_Read_Alert_Data_t *UnReadAlert);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_ANS_Un_Read_Alert_Status_Notification_t)(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int ConnectionID, ANS_Un_Read_Alert_Data_t *UnReadAlert);
#endif

   /*! ANS Client API. */
   /*! @brief The following function is responsible for parsing a New Alert
    * notification received from a remote ANS Server.
    * @param ValueLength The first parameter is the length of the value returned by the remote ANS
    * Server. @param Value The second parameter is a pointer to the data returned by
    * the remote ANS Server. @return This function returns a pointer to the
    * decode New Alert data or NULL if an error occurred.
    * \note The return value from this function MUST be freed by
    *          calling ANS_Free_New_Alert_Data() when the decoded New
    *          Alert Notification is no longer needed.
    */
BTPSAPI_DECLARATION ANS_New_Alert_Data_t *BTPSAPI ANS_Decode_New_Alert_Notification(unsigned int ValueLength, Byte_t *Value);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef ANS_New_Alert_Data_t *(BTPSAPI *PFN_ANS_Decode_New_Alert_Notification_t)(unsigned int ValueLength, Byte_t *Value);
#endif

   /*! @brief The following function is responsible for free the decoded New
    * Alert Data that was returned by a successful call to
    * ANS_Decode_New_Alert_Notification. @param NewAlertData The only parameter to this
    * function is the New Alert Data returned by the call to
    * ANS_Decode_New_Alert_Notification.
    */
BTPSAPI_DECLARATION void BTPSAPI ANS_Free_New_Alert_Data(ANS_New_Alert_Data_t *NewAlertData);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (BTPSAPI *PFN_ANS_Free_New_Alert_Data_t)(ANS_New_Alert_Data_t *NewAlertData);
#endif

   /*! @brief The following function is responsible for parsing an UnRead Alert
    * Status notification received from a remote ANS Server.
    * @param ValueLength The first parameter is the length of the value returned by the remote ANS
    * Server.  @param Value The second parameter is a pointer to the data returned by
    * the remote ANS Server. @param UnReadAlert The final parameter is a pointer to store
    * the parsed UnReadAlert value. @return This function returns a zero if
    * successful or a negative return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI ANS_Decode_Un_Read_Alert_Status_Notification(unsigned int ValueLength, Byte_t *Value, ANS_Un_Read_Alert_Data_t *UnReadAlert);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_ANS_Decode_Un_Read_Alert_Status_Notification_t)(unsigned int ValueLength, Byte_t *Value, ANS_Un_Read_Alert_Data_t *UnReadAlert);
#endif

   /*! @brief The following function is responsible for parsing a Supported
    * Categories value received from a remote ANS Server.
    * @param ValueLength The first parameter is the length of the value returned by the remote ANS
    * Server. @param Value The second parameter is a pointer to the data returned by
    * the remote ANS Server.  @param SupportedCategoriesMask The final parameter is a pointer to store
    * the Supported Categories bit mask. @return This function returns a zero
    * if successful or a negative return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI ANS_Decode_Supported_Categories(unsigned int ValueLength, Byte_t *Value, Word_t *SupportedCategoriesMask);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_ANS_Decode_Supported_Categories_t)(unsigned int ValueLength, Byte_t *Value, Word_t *SupportedCategoriesMask);
#endif

   /*! @brief The following function is responsible for formatting an Alert
    * Notification Control Point Command into a user specified buffer.
    * @param CommandBuffer The first parameter to this function is the Command Buffer to
    * format the command into. @param Command The second parameter specifies the Command
    * to format into the buffer. @param CommandCategory The final parameter specifies the
    * category that the command applies to. @return This function returns a
    * zero if successful or a negative return error code if an error
    * occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI ANS_Format_Control_Point_Command(ANS_Control_Point_Command_Value_t *CommandBuffer, ANS_Control_Point_Command_t Command, ANS_Category_Identification_t CommandCategory);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_ANS_Format_Control_Point_Command_t)(ANS_Control_Point_Command_Value_t *CommandBuffer, ANS_Control_Point_Command_t Command, ANS_Category_Identification_t CommandCategory);
#endif

#endif
