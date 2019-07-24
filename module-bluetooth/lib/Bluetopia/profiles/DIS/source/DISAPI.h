/*
 * Copyright 2011 - 2014 Stonestreet One.
 * All Rights Reserved.
 * Author:  Tim Cook
 *** MODIFICATION HISTORY ****************************************************
 *
 *   mm/dd/yy  F. Lastname    Description of Modification
 *   --------  -----------    ------------------------------------------------
 *   10/04/11  T. Cook        Initial creation.
 *   12/5/12   T. Cook        Update to DIS v1.1.
 ****************************************************************************
 */

/**
 *  @file DISAPI.h
 *
 *  @brief Stonestreet One Bluetooth Device Information Service (GATT
 *      based) API Type Definitions, Constants, and Prototypes.
 *      To use any of the following API's,
 *      include the following declaration in your application.
 *  @code
 *  #include "SS1BTDIS.h"
 *  @endcode
 *
 *  The Device Information Service, DIS, programming interface defines the protocols
 *  and procedures to be used to implement DIS capabilities for both Server and Client services.
 *  ============================================================================
 */
#ifndef __DISAPIH__
#define __DISAPIH__

#include "SS1BTPS.h"        /*! Bluetooth Stack API Prototypes/Constants. */
#include "SS1BTGAT.h"       /*! Bluetooth Stack GATT API Prototypes/Constants. */
#include "DISTypes.h"       /*! Device Information Service Types/Constants. */
   /*! Error Return Codes.
    * Error Codes that are smaller than these (less than -1000) are
    * related to the Bluetooth Protocol Stack itself (see BTERRORS.H).
    */
#define DIS_ERROR_INVALID_PARAMETER                      (-1000)
#define DIS_ERROR_INSUFFICIENT_RESOURCES                 (-1001)
#define DIS_ERROR_SERVICE_ALREADY_REGISTERED             (-1003)
#define DIS_ERROR_INVALID_INSTANCE_ID                    (-1004)
#define DIS_ERROR_MALFORMATTED_DATA                      (-1005)
#define DIS_ERROR_UNKNOWN_ERROR                          (-1006)

   /*! The following controls the maximum length of a static string.
    */
#define DIS_MAXIMUM_SUPPORTED_STRING                     (BTPS_CONFIGURATION_DIS_MAXIMUM_SUPPORTED_STRING_LENGTH)

   /*! The following structure represents all of the data contained in
    * the DIS PNP ID value.  The VendorID Source is a value that
    * describes who assigned the following VendorID.  The VendorID
    * identifies the vendor from the source described in the VendorID
    * Source.  The ProductID is a vendor maintained product ID.  The
    * ProductVersion is a vendor mainted product version.
    * \note The VendorID Source must be an enumerated value of the
    *          form DIS_PNP_ID_VENDOR_SOURCE_XXX.
    */
typedef struct _tagDIS_PNP_ID_Data_t
{
   Byte_t VendorID_Source;
   Word_t VendorID;
   Word_t ProductID;
   Word_t ProductVersion;
} DIS_PNP_ID_Data_t;

#define DIS_PNP_ID_DATA_SIZE                             (sizeof(DIS_PNP_ID_Data_t))

   /*! @brief The following function is responsible for opening a DIS Server.
    * @param BluetoothStackID The first parameter is the Bluetooth Stack ID on which to open the
    * server.  @param ServiceID The final parameter is a pointer to store the GATT
    * Service ID of the registered DIS service.  This can be used to
    * include the service registered by this call.
    * @return This function returns the positive, non-zero, Instance ID or a negative error
    * code.
    * \note Only 1 DIS Server may be open at a time, per Bluetooth
    *          Stack ID.
    */
BTPSAPI_DECLARATION int BTPSAPI DIS_Initialize_Service(unsigned int BluetoothStackID, unsigned int *ServiceID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_DIS_Initialize_Service_t)(unsigned int BluetoothStackID, unsigned int *ServiceID);
#endif

   /*! @brief The following function is responsible for opening a DIS Server.
    * @param BluetoothStackID The first parameter is the Bluetooth Stack ID on which to open the
    * server.  @param ServiceID The second parameter is a pointer to store the GATT
    * Service ID of the registered DIS service.  This can be used to
    * include the service registered by this call.
    * @param ServiceHandleRange The final parameter
    * is a pointer, that on input can be used to control the location of
    * the service in the GATT database, and on ouput to store the
    * service handle range. @return This function returns the positive,
    * non-zero, Instance ID or a negative error code.
    * \note Only 1 DIS Server may be open at a time, per Bluetooth
    *          Stack ID.
    */
BTPSAPI_DECLARATION int BTPSAPI DIS_Initialize_Service_Handle_Range(unsigned int BluetoothStackID, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_DIS_Initialize_Service_Handle_Range_t)(unsigned int BluetoothStackID, unsigned int *ServiceID, GATT_Attribute_Handle_Group_t *ServiceHandleRange);
#endif

   /*! @brief The following function is responsible for closing a previously DIS
    * Server. @param BluetoothStackID The first parameter is the Bluetooth Stack ID on which to
    * close the server. @param InstanceID The second parameter is the InstanceID that was
    * returned from a successful call to DIS_Initialize_Service().
    * @return This function returns a zero if successful or a negative return
    * error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI DIS_Cleanup_Service(unsigned int BluetoothStackID, unsigned int InstanceID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_DIS_Cleanup_Service_t)(unsigned int BluetoothStackID, unsigned int InstanceID);
#endif

   /*! @brief The following function is responsible for querying the number of
    * attributes that are contained in the DIS Service that is
    * registered with a call to DIS_Initialize_Service().
    * @return This function returns the non-zero number of attributes that are contained in a
    * DIS Server or zero on failure.
    */
BTPSAPI_DECLARATION unsigned int BTPSAPI DIS_Query_Number_Attributes(void);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef unsigned int (BTPSAPI *PFN_DIS_Query_Number_Attributes_t)(void);
#endif

   /*! @brief The following function is responsible for setting the Manufacturer
    * Name characteristic on the specified Device Information Service
    * instance.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID of the
    * Bluetooth Device.  @param InstanceID The second parameter is the InstanceID returned
    * from a successful call to DIS_Initialize_Server().
    * @param ManufacturerName The final parameter is the Manufacturer Name to set as the current
    * Manufacturer Name for the specified Device Information Service
    * Instance.  The Manufacturer Name parameter must be a pointer to a
    * NULL terminated ASCII String of at most
    * DIS_MAXIMUM_SUPPORTED_STRING (not counting the trailing NULL
    * terminator).  @return This function returns a zero if successful or a
    * negative return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI DIS_Set_Manufacturer_Name(unsigned int BluetoothStackID, unsigned int InstanceID, char *ManufacturerName);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_DIS_Set_Manufacturer_Name_t)(unsigned int BluetoothStackID, unsigned int InstanceID, char *ManufacturerName);
#endif

   /*! @brief The following function is responsible for querying the current
    * Manufacturer Name characteristic value on the specified DIS
    * instance.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID of the
    * Bluetooth Device.  @param InstanceID The second parameter is the InstanceID returned
    * from a successful call to DIS_Initialize_Server().
    * @param ManufacturerName The final parameter is a pointer to a structure to return the current
    * Manufacturer Name for the specified DIS Service Instance.  The
    * Manufacturer Name Length should be at least
    * (DIS_MAXIMUM_SUPPORTED_STRING+1) to hold the Maximum allowable
    * string (plus a single character to hold the NULL terminator).
    * @return This function returns a zero if successful or a negative return error
    * code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI DIS_Query_Manufacturer_Name(unsigned int BluetoothStackID, unsigned int InstanceID, char *ManufacturerName);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_DIS_Query_Manufacturer_Name_t)(unsigned int BluetoothStackID, unsigned int InstanceID, char *ManufacturerName);
#endif

   /*! @brief The following function is responsible for setting the Model Number
    * characteristic on the specified Device Information Service
    * instance. @param BluetoothStackID The first parameter is the Bluetooth Stack ID of the
    * Bluetooth Device. @param InstanceID The second parameter is the InstanceID returned
    * from a successful call to DIS_Initialize_Server().
    *b@param ModelNumber The final parameter is the Model Number to set as the current Model Number
    * for the specified Device Information Service Instance.  The Model
    * Number parameter must be a pointer to a NULL terminated ASCII
    * String of at most DIS_MAXIMUM_SUPPORTED_STRING (not counting the
    * trailing NULL terminator).  @return This function returns a zero if
    * successful or a negative return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI DIS_Set_Model_Number(unsigned int BluetoothStackID, unsigned int InstanceID, char *ModelNumber);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_DIS_Set_Model_Number_t)(unsigned int BluetoothStackID, unsigned int InstanceID, char *ModelNumber);
#endif

   /*! @brief The following function is responsible for querying the current
    * Model Number characteristic value on the specified DIS instance.
    * @param BluetoothStackID The first parameter is the Bluetooth Stack ID of the Bluetooth
    * Device.  @param InstanceID The second parameter is the InstanceID returned from a
    * successful call to DIS_Initialize_Server(). @param ModelNumber The final parameter
    * is a pointer to a structure to return the current Model Number for
    * the specified DIS Service Instance.  The Model Number Length
    * should be at least (DIS_MAXIMUM_SUPPORTED_STRING+1) to hold the
    * Maximum allowable string (plus a single character to hold the NULL
    * terminator) @return This function returns a zero if successful or a
    * negative return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI DIS_Query_Model_Number(unsigned int BluetoothStackID, unsigned int InstanceID, char *ModelNumber);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_DIS_Query_Model_Number_t)(unsigned int BluetoothStackID, unsigned int InstanceID, char *ModelNumber);
#endif

   /*! @brief The following function is responsible for setting the Serial
    * Number characteristic on the specified Device Information Service
    * instance.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID of the
    * Bluetooth Device.  @param InstanceID The second parameter is the InstanceID returned
    * from a successful call to DIS_Initialize_Server().
    * @param SerialNumber The final parameter is the Serial Number to set as the current Serial Number
    * for the specified Device Information Service Instance.  The Serial
    * Number parameter must be a pointer to a NULL terminated ASCII
    * String of at most DIS_MAXIMUM_SUPPORTED_STRING (not counting the
    * trailing NULL terminator).  @return This function returns a zero if
    * successful or a negative return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI DIS_Set_Serial_Number(unsigned int BluetoothStackID, unsigned int InstanceID, char *SerialNumber);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_DIS_Set_Serial_Number_t)(unsigned int BluetoothStackID, unsigned int InstanceID, char *SerialNumber);
#endif

   /*! @brief The following function is responsible for querying the current
    * Serial Number characteristic value on the specified DIS instance.
    * @param BluetoothStackID The first parameter is the Bluetooth Stack ID of the Bluetooth
    * Device. @param InstanceID The second parameter is the InstanceID returned from a
    * successful call to DIS_Initialize_Server(). @param SerialNumber The final parameter
    * is a pointer to a structure to return the current Serial Number
    * for the specified DIS Service Instance.  The Serial Number Length
    * should be at least (DIS_MAXIMUM_SUPPORTED_STRING+1) to hold the
    * Maximum allowable string (plus a single character to hold the NULL
    * terminator) @return This function returns a zero if successful or a
    * negative return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI DIS_Query_Serial_Number(unsigned int BluetoothStackID, unsigned int InstanceID, char *SerialNumber);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_DIS_Query_Serial_Number_t)(unsigned int BluetoothStackID, unsigned int InstanceID, char *SerialNumber);
#endif

   /*! @brief The following function is responsible for setting the Hardware
    * Revision characteristic on the specified Device Information
    * Service instance.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID
    * of the Bluetooth Device.  @param InstanceID The second parameter is the InstanceID
    * returned from a successful call to DIS_Initialize_Server().
    * @param Hardware_Revision The final parameter is the Hardware Revision to set as the current
    * Hardware Revision for the specified Device Information Service
    * Instance.  The Hardware Revision parameter must be a pointer to a
    * NULL terminated ASCII String of at most
    * DIS_MAXIMUM_SUPPORTED_STRING (not counting the trailing NULL
    * terminator). @return This function returns a zero if successful or a
    * negative return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI DIS_Set_Hardware_Revision(unsigned int BluetoothStackID, unsigned int InstanceID, char *Hardware_Revision);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_DIS_Set_Hardware_Revision_t)(unsigned int BluetoothStackID, unsigned int InstanceID, char *Hardware_Revision);
#endif

   /*! @brief The following function is responsible for querying the current
    * Hardware Revision characteristic value on the specified DIS
    * instance.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID of the
    * Bluetooth Device.  @param InstanceID The second parameter is the InstanceID returned
    * from a successful call to DIS_Initialize_Server().
    * @param Hardware_Revision The final parameter is a pointer to a structure to return the current
    * Hardware Revision for the specified DIS Service Instance.  The
    * Hardware Revision Length should be at least
    * (DIS_MAXIMUM_SUPPORTED_STRING+1) to hold the Maximum allowable
    * string (plus a single character to hold the NULL terminator).
    * @return This function returns a zero if successful or a negative return error
    * code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI DIS_Query_Hardware_Revision(unsigned int BluetoothStackID, unsigned int InstanceID, char *Hardware_Revision);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_DIS_Query_Hardware_Revision_t)(unsigned int BluetoothStackID, unsigned int InstanceID, char *Hardware_Revision);
#endif

   /*! @brief The following function is responsible for setting the Firmware
    * Revision characteristic on the specified Device Information
    * Service instance.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID
    * of the Bluetooth Device.  @param InstanceID The second parameter is the InstanceID
    * returned from a successful call to DIS_Initialize_Server().
    * @param FirmwareRevision The final parameter is the Firmware Revision to set as the current
    * Firmware Revision for the specified Device Information Service
    * Instance.  The Firmware Revision parameter must be a pointer to a
    * NULL terminated ASCII String of at most
    * DIS_MAXIMUM_SUPPORTED_STRING (not counting the trailing NULL
    * terminator). @return This function returns a zero if successful or a
    * negative return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI DIS_Set_Firmware_Revision(unsigned int BluetoothStackID, unsigned int InstanceID, char *FirmwareRevision);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_DIS_Set_Firmware_Revision_t)(unsigned int BluetoothStackID, unsigned int InstanceID, char *FirmwareRevision);
#endif

   /*! @brief The following function is responsible for querying the current
    * Firmware Revision characteristic value on the specified DIS
    * instance.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID of the
    * Bluetooth Device.  @param InstanceID The second parameter is the InstanceID returned
    * from a successful call to DIS_Initialize_Server().
    * @param FirmwareRevision The final parameter is a pointer to a structure to return the current
    * Firmware Revision for the specified DIS Service Instance.  The
    * Firmware Revision Length should be at least
    * (DIS_MAXIMUM_SUPPORTED_STRING+1) to hold the Maximum allowable
    * string (plus a single character to hold the NULL terminator).
    * @return This function returns a zero if successful or a negative return error
    * code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI DIS_Query_Firmware_Revision(unsigned int BluetoothStackID, unsigned int InstanceID, char *FirmwareRevision);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_DIS_Query_Firmware_Revision_t)(unsigned int BluetoothStackID, unsigned int InstanceID, char *FirmwareRevision);
#endif

   /*! @brief The following function is responsible for setting the Software
    * Revision characteristic on the specified Device Information
    * Service instance.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID
    * of the Bluetooth Device.  @param InstanceID The second parameter is the InstanceID
    * returned from a successful call to DIS_Initialize_Server().
    * @param SoftwareRevision The final parameter is the Software Revision to set as the current
    * Software Revision for the specified Device Information Service
    * Instance.  The Software Revision parameter must be a pointer to a
    * NULL terminated ASCII String of at most
    * DIS_MAXIMUM_SUPPORTED_STRING (not counting the trailing NULL
    * terminator). @return This function returns a zero if successful or a
    * negative return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI DIS_Set_Software_Revision(unsigned int BluetoothStackID, unsigned int InstanceID, char *SoftwareRevision);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_DIS_Set_Software_Revision_t)(unsigned int BluetoothStackID, unsigned int InstanceID, char *SoftwareRevision);
#endif

   /*! @brief The following function is responsible for querying the current
    * Software Revision characteristic value on the specified DIS
    * instance. @param BluetoothStackID The first parameter is the Bluetooth Stack ID of the
    * Bluetooth Device. @param InstanceID The second parameter is the InstanceID returned
    * from a successful call to DIS_Initialize_Server().
    * @param SoftwareRevision The final parameter is a pointer to a structure to return the current
    * Software Revision for the specified DIS Service Instance.  The
    * Software Revision Length should be at least
    * (DIS_MAXIMUM_SUPPORTED_STRING+1) to hold the Maximum allowable
    * string (plus a single character to hold the NULL terminator).
    * @return This function returns a zero if successful or a negative return error
    * code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI DIS_Query_Software_Revision(unsigned int BluetoothStackID, unsigned int InstanceID, char *SoftwareRevision);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_DIS_Query_Software_Revision_t)(unsigned int BluetoothStackID, unsigned int InstanceID, char *SoftwareRevision);
#endif

   /*! @brief The following function is responsible for setting the System ID
    * characteristic on the specified Device Information Service
    * instance.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID of the
    * Bluetooth Device.  @param InstanceID The second parameter is the InstanceID returned
    * from a successful call to DIS_Initialize_Server().
    * @param SystemID The final parameter is the System ID to set as the current System ID for the
    * specified Device Information Service Instance.
    * @return This function returns a zero if successful or a negative return error code if an
    * error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI DIS_Set_System_ID(unsigned int BluetoothStackID, unsigned int InstanceID, DIS_System_ID_Value_t *SystemID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_DIS_Set_System_ID_t)(unsigned int BluetoothStackID, unsigned int InstanceID, DIS_System_ID_Value_t *SystemID);
#endif

   /*! @brief The following function is responsible for querying the current
    * System ID characteristic value on the specified DIS instance.
    * @param BluetoothStackID The first parameter is the Bluetooth Stack ID of the Bluetooth Device.
    * @param InstanceID The second parameter is the InstanceID returned from a successful
    * call to DIS_Initialize_Server().  @param SystemID The final parameter is a pointer
    * to a structure to return the current System ID for the specified
    * DIS Service Instance. @return This function returns a zero if successful
    * or a negative return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI DIS_Query_System_ID(unsigned int BluetoothStackID, unsigned int InstanceID, DIS_System_ID_Value_t *SystemID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_DIS_Query_System_ID_t)(unsigned int BluetoothStackID, unsigned int InstanceID, DIS_System_ID_Value_t *SystemID);
#endif

   /*! @brief The following function is responsible for setting the IEEE
    * Certification Data characteristic on the specified Device
    * Information Service instance.  @param BluetoothStackID The first parameter is the
    * Bluetooth Stack ID of the Bluetooth Device.  @param InstanceID The second parameter
    * is the InstanceID returned from a successful call to
    * DIS_Initialize_Server(). @param Length The third parameter is the length of the
    * IEEE Certification Data. @param IEEE_Certification_Data The final parameter is the IEEE
    * Certification Data to set as the current IEEE Certification Data
    * for the specified Device Information Service Instance.  The IEEE
    * Certification Data parameter must be a pointer to a array of at
    * most DIS_MAXIMUM_SUPPORTED_STRING characters.
    * @return This function returns a zero if successful or a negative return error code if an
    * error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI DIS_Set_IEEE_Certification_Data(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int Length, Byte_t *IEEE_Certification_Data);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_DIS_Set_IEEE_Certification_Data_t)(unsigned int BluetoothStackID, unsigned int InstanceID, unsigned int Length, Byte_t *IEEE_Certification_Data);
#endif

   /*! @brief The following function is responsible for querying the current
    * IEEE Certification Data characteristic value on the specified DIS
    * instance.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID of the
    * Bluetooth Device.  @param InstanceID The second parameter is the InstanceID returned
    * from a successful call to DIS_Initialize_Server().
    * @param IEEE_Certification_Data The final parameter is a pointer to a structure to return the current IEEE
    * Certification Data for the specified DIS Service Instance.  The
    * IEEE Certification Data Length should be at least
    * (DIS_MAXIMUM_SUPPORTED_STRING) to hold the Maximum allowable IEEE
    * Certification Data.  This function the length of the IEEE
    * Certification Data if successful or a negative return error code
    * if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI DIS_Query_IEEE_Certification_Data(unsigned int BluetoothStackID, unsigned int InstanceID, Byte_t *IEEE_Certification_Data);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_DIS_Query_IEEE_Certification_Data_t)(unsigned int BluetoothStackID, unsigned int InstanceID, Byte_t *IEEE_Certification_Data);
#endif

   /*! @brief The following function is responsible for setting the Plug and
    * Play (PnP) ID characteristic on the specified Device Information
    * Service instance.  @param BluetoothStackID The first parameter is the Bluetooth Stack ID
    * of the Bluetooth Device.  @param InstanceID The second parameter is the InstanceID
    * returned from a successful call to DIS_Initialize_Server().
    * @param PNP_ID_Data The final parameter is the PnP ID Data to set as the current PnP ID
    * Data for the specified Device Information Service Instance.
    * @return This function returns a zero if successful or a negative return error
    * code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI DIS_Set_PNP_ID(unsigned int BluetoothStackID, unsigned int InstanceID, DIS_PNP_ID_Data_t *PNP_ID_Data);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_DIS_Set_PNP_ID_t)(unsigned int BluetoothStackID, unsigned int InstanceID, DIS_PNP_ID_Data_t *PNP_ID_Data);
#endif

   /*! @brief The following function is responsible for querying the current PnP
    * ID Data characteristic value on the specified DIS instance.
    * @param BluetoothStackID The first parameter is the Bluetooth Stack ID of the Bluetooth Device.
    * @param InstanceID The second parameter is the InstanceID returned from a successful
    * call to DIS_Initialize_Server().  @param PNP_ID_Data The final parameter is a pointer
    * to a structure to return the current PnP ID Data for the specified
    * DIS Service Instance. @return This function returns a zero if successful
    * or a negative return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI DIS_Query_PNP_ID(unsigned int BluetoothStackID, unsigned int InstanceID, DIS_PNP_ID_Data_t *PNP_ID_Data);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_DIS_Query_PNP_ID_t)(unsigned int BluetoothStackID, unsigned int InstanceID, DIS_PNP_ID_Data_t *PNP_ID_Data);
#endif

   /*! @brief The following function is responsible for parsing a PnP ID value
    * received from a remote DIS Server.  @param ValueLength The first parameter is the
    * length of the value returned by the remote DIS Server.
    * @param Value The second parameter is a pointer to the data returned by the remote DIS
    * Server. @param PNP_ID_Data The final parameter is a pointer to store the parsed PnP
    * ID data. @return This function returns a zero if successful or a negative
    * return error code if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI DIS_Decode_PNP_ID(unsigned int ValueLength, Byte_t *Value, DIS_PNP_ID_Data_t *PNP_ID_Data);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_DIS_Decode_PNP_ID_t)(unsigned int ValueLength, Byte_t *Value, DIS_PNP_ID_Data_t *PNP_ID_Data);
#endif

#endif
