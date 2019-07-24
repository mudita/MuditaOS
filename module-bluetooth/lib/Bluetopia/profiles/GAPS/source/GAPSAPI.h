/*
 * Copyright 2011 - 2014 Stonestreet One.
 * All Rights Reserved.
 * Author:  Tim Cook
 *** MODIFICATION HISTORY ****************************************************
 *
 *   mm/dd/yy  F. Lastname    Description of Modification
 *   --------  -----------    ------------------------------------------------
 *   09/29/11  T. Cook        Initial creation.
 ****************************************************************************
 */

/**
 *  @file GAPSAPI.h
 *
 *  @brief Stonestreet One Bluetooth Generic Access Profile Service (GATT
 *      based) API Type Definitions, Constants, and Prototypes.
 *      To use any of the following API's,
 *      include the following declaration in your application.
 *
 *  @code
 *  #include "SS1BTGAP.h"
 *  @endcode
 *
 *  The Generic Access Profile Service programming interface defines the protocols and procedures to be used to implement Generic Access Profile Service capabilities.
 *  ============================================================================
 */
#ifndef __GAPSAPIH__
#define __GAPSAPIH__

#include "SS1BTPS.h"        /*! Bluetooth Stack API Prototypes/Constants. */
#include "SS1BTGAT.h"       /*! Bluetooth Stack GATT API Prototypes/Constants. */
#include "GAPSType.h"       /*! GAP Service Types/Constants. */

   /*! Error Return Codes.
    * Error Codes that are smaller than these (less than -1000) are
    * related to the Bluetooth Protocol Stack itself (see BTERRORS.H).
    */
#define GAPS_ERROR_INVALID_PARAMETER                     (-1000)
#define GAPS_ERROR_INVALID_BLUETOOTH_STACK_ID            (-1001)
#define GAPS_ERROR_INSUFFICIENT_RESOURCES                (-1002)
#define GAPS_ERROR_SERVICE_ALREADY_REGISTERED            (-1003)
#define GAPS_ERROR_INVALID_INSTANCE_ID                   (-1004)
#define GAPS_ERROR_MALFORMATTED_DATA                     (-1005)
#define GAPS_ERROR_UNKNOWN_ERROR                         (-1006)

   /*! The following represents to the structure of the Peripheral
    * Preferred Connection Parameters.  This is used to inform a remote
    * Master device of the local device's preferred connection
    * parameters.
    * \note With the exception of the Slave_Latency member, which is
    *          specified in Connection Events, all other members of this
    *          structure are specified in milliseconds.
    * \note With the exception of the Slave_Latency member, all other
    *          members may be set to
    *       GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_NO_SPECIFIC_
    *       PREFERRED
    *          to indicate that the local device does not have any
    *          preference for the specified parameter.
    * \note The Minimum_Connection_Interval must satisfy the
    *          following equation (with defines from GAPAPI.h):
    *                Connection_Interval_Min >=
    *                      MINIMUM_MINIMUM_CONNECTION_INTERVAL
    *            AND
    *                Connection_Interval_Min <=
    *                      MAXIMUM_MINIMUM_CONNECTION_INTERVAL
    *            OR
    *                Connection_Interval_Min ==
    *       GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_NO_SPECIFIC_
    *       PREFERRED
    * \note The Maximum_Connection_Interval must satisfy the
    *          following equation (with defines from GAPAPI.h):
    *                Connection_Interval_Max >=
    *                      MINIMUM_MAXIMUM_CONNECTION_INTERVAL
    *            AND
    *                Connection_Interval_Max <=
    *                      MAXIMUM_MAXIMUM_CONNECTION_INTERVAL
    *            OR
    *                Connection_Interval_Max ==
    *       GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_NO_SPECIFIC_
    *       PREFERRED
    * \note The Slave_Latency must satisfy the following equation
    *          (with defines from GAPAPI.h):
    *                Slave_Latency >=
    *                      MINIMUM_SLAVE_LATENCY
    *           AND
    *                Slave_Latency <=
    *                      MAXIMUM_SLAVE_LATENCY
    * \note The Supervision_Timeout must satisfy the following
    *          equation (with defines from GAPAPI.h):
    *                Supervision_Timeout >=
    *                     MINIMUM_LINK_SUPERVISION_TIMEOUT
    *          AND
    *                Supervision_Timeout <=
    *                     MAXIMUM_LINK_SUPERVISION_TIMEOUT
    *          OR
    *                Supervision_Timeout ==
    *       GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_NO_SPECIFIC_
    *       PREFERRED
    */
typedef struct _tagGAP_Preferred_Connection_Parameters_t
{
   Word_t Minimum_Connection_Interval;
   Word_t Maximum_Connection_Interval;
   Word_t Slave_Latency;
   Word_t Supervision_Timeout;
} GAP_Preferred_Connection_Parameters_t;

#define GAP_PREFERRED_CONNECTION_PARAMETERS_DATA_SIZE    (sizeof(GAP_Preferred_Connection_Parameters_t))

   /*! GAPS Server API. */

   /*! @brief The following function is responsible for opening a GAPS Server.
    * @param BluetoothStackID The first parameter is the Bluetooth Stack ID on which to open the
    * server.  @param ServiceID The final parameter is a pointer to store the GATT
    * Service ID of the registered GAPS service.  This can be used to
    * include the service registered by this call.
    * @return This function returns the positive, non-zero, Instance ID or a negative error
    * code.
    * \note Only 1 GAPS Server may be open at a time, per Bluetooth
    *          Stack ID.
    */
BTPSAPI_DECLARATION int BTPSAPI GAPS_Initialize_Service(unsigned int BluetoothStackID, unsigned int *ServiceID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAPS_Initialize_Service_t)(unsigned int BluetoothStackID, unsigned int *ServiceID);
#endif

   /*! @brief The following function is responsible for opening a GAPS Server.
    * @param BluetoothStackID The first parameter is the Bluetooth Stack ID on which to open the
    * server.  @param ServiceID The second parameter is a pointer to store the GATT
    * Service ID of the registered GAPS service.  This can be used to
    * include the service registered by this call.
    * @param ServiceHandleRange The final parameter
    * is a pointer, that on input can be used to control the location of
    * the service in the GATT database, and on ouput to store the
    * service handle range.  @return This function returns the positive,
    * non-zero, Instance ID or a negative error code.
    * \note Only 1 GAPS Server may be open at a time, per Bluetooth
    *          Stack ID.
    */
BTPSAPI_DECLARATION int BTPSAPI GAPS_Initialize_Service_Handle_Range(unsigned int BluetoothStackID, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAPS_Initialize_Service_Handle_Range_t)(unsigned int BluetoothStackID, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange);
#endif

   /*! @brief The following function is responsible for closing a previously
    * GAPS Server. @param BluetoothStackID The first parameter is the Bluetooth Stack ID on
    * which to close the server. @param InstanceID The second parameter is the InstanceID
    * that was returned from a successful call to
    * GAPS_Initialize_Service(). @return This function returns a zero if
    * successful or a negative return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI GAPS_Cleanup_Service(unsigned int BluetoothStackID, unsigned int InstanceID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAPS_Cleanup_Service_t)(unsigned int BluetoothStackID, unsigned int InstanceID);
#endif

   /*! @brief The following function is responsible for querying the number of
    * attributes that are contained in the GAPS Service that is
    * registered with a call to GAPS_Initialize_Service().
    * @return This function returns the non-zero number of attributes that are
    * contained in a GAPS Server or zero on failure.
    */
BTPSAPI_DECLARATION unsigned int BTPSAPI GAPS_Query_Number_Attributes(void);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef unsigned int (BTPSAPI *PFN_GAPS_Query_Number_Attributes_t)(void);
#endif

   /*! @brief The following function is responsible for setting the Device Name
    * characteristic on the specified GAP Service instance.
    * @param BluetoothStackID The first parameter
    * is the Bluetooth Stack ID of the Bluetooth Device.
    * @param InstanceID The second parameter is the InstanceID returned from a successful call
    * to GAPS_Initialize_Server().
    * @param DeviceName The final parameter is the Device
    * Name to set as the current Device Name for the specified GAP
    * Service Instance.  The Name parameter must be a pointer to a NULL
    * terminated ASCII String of at most GAP_MAXIMUM_DEVICE_NAME_LENGTH
    * (not counting the trailing NULL terminator).
    * @return This function returns a zero if successful or a negative return error code if an
    * error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI GAPS_Set_Device_Name(unsigned int BluetoothStackID, unsigned int InstanceID, char *DeviceName);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAPS_Set_Device_Name_t)(unsigned int BluetoothStackID, unsigned int InstanceID, char *DeviceName);
#endif

   /*! @brief The following function is responsible for querying the current
    * Device Name characteristic value on the specified GAPS instance.
    * @param BluetoothStackID The first parameter is the Bluetooth Stack ID of the Bluetooth
    * Device.  @param InstanceID The second parameter is the InstanceID returned from a
    * successful call to GAPS_Initialize_Server().  @param NameBuffer The final parameter
    * is a pointer to a structure to return the current Device Name for
    * the specified GAPS Service Instance.  The NameBuffer Length should
    * be at least (GAP_MAXIMUM_DEVICE_NAME_LENGTH+1) to hold the Maximum
    * allowable Name (plus a single character to hold the NULL
    * terminator). @return This function returns a zero if successful or a
    * negative return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI GAPS_Query_Device_Name(unsigned int BluetoothStackID, unsigned int InstanceID, char *NameBuffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAPS_Query_Device_Name_t)(unsigned int BluetoothStackID, unsigned int InstanceID, char *NameBuffer);
#endif

   /*! @brief The following function is responsible for setting the Device
    * Appearance characteristic on the specified GAP Service instance.
    * @param BluetoothStackID The first parameter is the Bluetooth Stack ID of the Bluetooth
    * Device.  @param InstanceID The second parameter is the InstanceID returned from a
    * successful call to GAPS_Initialize_Server().  @param DeviceAppearance The final parameter
    * is the Device Appearance to set as the current Device Appearance
    * for the specified GAP Service Instance. @return This function returns a
    * zero if successful or a negative return error code if an error
    * occurs.
    * \note The DeviceAppearance is an enumeration, which should be
    *          of the form GAP_DEVICE_APPEARENCE_VALUE_XXX.
    */
BTPSAPI_DECLARATION int BTPSAPI GAPS_Set_Device_Appearance(unsigned int BluetoothStackID, unsigned int InstanceID, Word_t DeviceAppearance);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAPS_Set_Device_Appearance_t)(unsigned int BluetoothStackID, unsigned int InstanceID, Word_t DeviceAppearance);
#endif

   /*! @brief The following function is responsible for querying the Device
    * Appearance characteristic on the specified GAP Service instance.
    * @param BluetoothStackID The first parameter is the Bluetooth Stack ID of the Bluetooth
    * Device.  @param InstanceID The second parameter is the InstanceID returned from a
    * successful call to GAPS_Initialize_Server().  @param DeviceAppearance The final parameter
    * is a pointer to store the current Device Appearance for the
    * specified GAP Service Instance. @return This function returns a zero if
    * successful or a negative return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI GAPS_Query_Device_Appearance(unsigned int BluetoothStackID, unsigned int InstanceID, Word_t *DeviceAppearance);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAPS_Query_Device_Appearance_t)(unsigned int BluetoothStackID, unsigned int InstanceID, Word_t *DeviceAppearance);
#endif

   /*! @brief The following function is responsible for setting the Peripheral
    * Preferred Connection Parameter characteristic on the specified GAP
    * Service instance.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID
    * of the Bluetooth Device.  @param InstanceID The second parameter is the InstanceID
    * returned from a successful call to GAPS_Initialize_Server().
    * @param PreferredConnectionParameters The final parameter is the Preferred Connection Parameters to set as
    * the current Peripheral Preferred Connection Parameters for the
    * specified GAP Service Instance. @return This function returns a zero if
    * successful or a negative return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI GAPS_Set_Preferred_Connection_Parameters(unsigned int BluetoothStackID, unsigned int InstanceID, GAP_Preferred_Connection_Parameters_t *PreferredConnectionParameters);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAPS_Set_Preferred_Connection_Parameters_t)(unsigned int BluetoothStackID, unsigned int InstanceID, GAP_Preferred_Connection_Parameters_t *PreferredConnectionParameters);
#endif

   /*! @brief The following function is responsible for querying the Peripheral
    * Preferred Connection Parameter characteristic on the specified GAP
    * Service instance.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID
    * of the Bluetooth Device.  @param InstanceID The second parameter is the InstanceID
    * returned from a successful call to GAPS_Initialize_Server().
    * @param PreferredConnectionParameters The final parameter is a pointer to a structure to store the current
    * Peripheral Preferred Connection Parameters for the specified GAP
    * Service Instance. @return This function returns a zero if successful or a
    * negative return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI GAPS_Query_Preferred_Connection_Parameters(unsigned int BluetoothStackID, unsigned int InstanceID, GAP_Preferred_Connection_Parameters_t *PreferredConnectionParameters);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAPS_Query_Preferred_Connection_Parameters_t)(unsigned int BluetoothStackID, unsigned int InstanceID, GAP_Preferred_Connection_Parameters_t *PreferredConnectionParameters);
#endif

   /*! GAPS Client API. */

   /*! @brief The following function is responsible for decoding a Peripheral
    * Preferred Connection Parameter characteristic value that was
    * received from a remote device. @param ValueLength The first parameter is the length
    * of the value received from the remote device. @param Value The second is a
    * pointer to the value that was received from the remote device.
    * @param PreferredConnectionParameters The final parameter is a pointer to a structure to store the
    * decoded Peripheral Preferred Connection Parameters value that was
    * received from the remote device. @return This function returns a zero if
    * successful or a negative return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI GAPS_Decode_Preferred_Connection_Parameters(unsigned int ValueLength, Byte_t *Value, GAP_Preferred_Connection_Parameters_t *PreferredConnectionParameters);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAPS_Decode_Preferred_Connection_Parameters_t)(unsigned int ValueLength, Byte_t *Value, GAP_Preferred_Connection_Parameters_t *PreferredConnectionParameters);
#endif

#endif
