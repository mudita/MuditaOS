/*
 * Copyright 2012 - 2013 Stonestreet One.
 * All Rights Reserved.
 * Author:  Samuel Hishmeh
 *** MODIFICATION HISTORY ****************************************************
 *
 *   mm/dd/yy  F. Lastname    Description of Modification
 *   --------  -----------    ------------------------------------------------
 *   10/11/12  S. Hishmeh     Initial creation.
 ****************************************************************************
 */

/**
 *  @file DIDAPI.h
 *
 *  @brief Stonestreet One Bluetooth Stack Device Identification Profile
 *      API Type Definitions, Constants, and Prototypes.
 *
 *  @code
 *  #include "SS1BTDID.h"
 *  @endcode
 *  ============================================================================
 */
#ifndef __DIDAPIH__
#define __DIDAPIH__

#include "SS1BTPS.h"      /*! Bluetooth Stack API Prototypes/Constants. */

   /*! Error Codes that are smaller than these (less than -1000) are
    * related to the Bluetooth Protocol Stack itself (see BTERRORS.H).
    */
#define BTDID_RESULT_INVALID_DID_INFORMATION       (-1000)

   /*! SDP Service Classes for the Device Identification Profile.
    * The following MACRO is a utility MACRO that assigns the Device
    * Identification Service Class Bluetooth Universally Unique
    * Identifier (DID_PROFILE_UUID_16) to the specified UUID_16_t
    * variable.  This MACRO accepts one parameter which is the UUID_16_t
    * variable that is to receive the DID_PROFILE_UUID_16 Constant
    * value.
    */
#define SDP_ASSIGN_DID_PROFILE_UUID_16(_x)         ASSIGN_SDP_UUID_16((_x), 0x12, 0x00)

   /*! The following MACRO is a utility MACRO that assigns the Device
    * Identification Service Class Bluetooth Universally Unique
    * Identifier (DID_PROFILE_UUID_32) to the specified UUID_32_t
    * variable.  This MACRO accepts one parameter which is the UUID_32_t
    * variable that is to receive the DID_PROFILE_UUID_16 Constant
    * value.
    */
#define SDP_ASSIGN_DID_PROFILE_UUID_32(_x)         ASSIGN_SDP_UUID_32((_x), 0x00, 0x00, 0x12, 0x00)

   /*! The following MACRO is a utility MACRO that assigns the Device
    * Identification Service Class Bluetooth Universally Unique
    * Identifier (DID_PROFILE_UUID_16) to the specified UUID_128_t
    * variable.  This MACRO accepts one parameter which is the
    * UUID_128_t variable that is to receive the DID_PROFILE_UUID_16
    * Constant value.
    */
#define SDP_ASSIGN_DID_PROFILE_UUID_128(_x)        ASSIGN_SDP_UUID_128((_x), 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /* Defines the Profile Version Number used within the SDP Record for
    * the Device Identification Profile.
    */
#define DID_PROFILE_VERSION                        (0x0103)

   /*! The following MACROs are utility MACROs that take as input the
    * return value of a successful call to
    * DID_Parse_Device_Information() and return the Major or Minor
    * fields of the DID Profile Version.
    */
#define DID_GET_PROFILE_VERSION_MAJOR(_x)          (((_x) >> 8) & 0xFF)
#define DID_GET_PROFILE_VERSION_MINOR(_x)          ((_x) & 0xFF)

   /*! The following MACROs are utility MACROs that take as input the
    * DeviceVersion field of a DID_Info_t structure and return the
    * Major, Minor, and Sub-Minor fields of the Device Version.
    */
#define DID_GET_DEVICE_VERSION_MAJOR(_x)           (((_x) >> 8) & 0xFF)
#define DID_GET_DEVICE_VERSION_MINOR(_x)           (((_x) >> 4) & 0x0F)
#define DID_GET_DEVICE_VERSION_SUBMINOR(_x)        ((_x) & 0x0F)

   /*! The following MACROs are utility MACROs that take as input the
    * Major, Minor, or Sub-Minor fields of the Device Version and assign
    * the appropriate bits in the first parameter.
    */
#define DID_SET_DEVICE_VERSION_MAJOR(_x, _y)       ((_x) = (((_x) & 0x00FF) | ((_y & 0xFF) << 8)))
#define DID_SET_DEVICE_VERSION_MINOR(_x, _y)       ((_x) = (((_x) & 0xFF0F) | ((_y & 0x0F) << 4)))
#define DID_SET_DEVICE_VERSION_SUBMINOR(_x, _y)    ((_x) = (((_x) & 0xFFF0) | ((_y & 0x0F))))

   /*! The following MACRO is a utility MACRO that takes as input the
    * Major, Minor, and Sub-Minor Device Version fields, and packs the
    * fields into 2 bytes.
    */
#define DID_MAKE_DEVICE_VERSION(_x, _y, _z)        ((((_x) & 0xFF) << 8) | ((_y & 0x0F) << 4) | (_z & 0x0F))

   /* Structure contains fields of a Device Identification SDP Record.
    * This type is used by DID_Register_SDP_Record() to add a new DID
    * SDP Record, and is passed as a parameter to
    * DID_Parse_Device_Information() to parse and store a DID SDP
    * Record.  The first 5 variables, VendorID, ProductID,
    * DeviceVersion, IsPrimaryRecord, and VendorIDSource, are mandatory
    * and will be included in every DID SDP Record.  The last 3
    * variables, ClientExecutableURL, ServiceDescription, and
    * DocumentationURL, are optional.
    */
typedef struct _tagDID_Info
{
   Word_t        VendorID;
   Word_t        ProductID;
   Word_t        DeviceVersion;
   Boolean_t     IsPrimaryRecord;
   Word_t        VendorIDSource;
   unsigned int  ClientExecutableURLLength;
   char         *ClientExecutableURL;
   unsigned int  ServiceDescriptionLength;
   char         *ServiceDescription;
   unsigned int  DocumentationURLLength;
   char         *DocumentationURL;
} DID_Info_t;

   /*! @brief The following function is responsible for adding a Device
    * Identification SDP Record to the SDP Database.  @param BluetoothStackID
    * The first parameter to this function is the Bluetooth Stack ID of the SDP
    * Server to create the SDP Record on.
    * @param DeviceIdentificationInfo The second parameter is a
    * pointer to a Device Info structure.  All mandatory attributes in
    * the Device Info structure will be added to a Device Identification
    * SDP Record, optional parameters will only be added if the optional
    * parameter's pointer points to a non-null memory address.  The
    * optional attribute pointers must be set to NULL to be excluded
    * from the SDP Record.  If this function is successful, this
    * function will return a positive, non-zero, value which represents
    * the SDP Server Record Handle that was created on the specified SDP
    * Server.  This value can be used as the SDP Record Handle for the
    * DID_Un_Register_SDP_Record() function.  @return This function returns a
    * negative return value if the Service Record was unable to be added
    * to the SDP Database.
    */
BTPSAPI_DECLARATION SDWord_t BTPSAPI DID_Register_SDP_Record(unsigned int BluetoothStackID, DID_Info_t *DeviceIdentificationInfo);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef SDWord_t (BTPSAPI *PFN_DID_Register_SDP_Record_t)(unsigned int BluetoothStackID, DID_Info_t *DeviceIdentificationInfo);
#endif

   /*! The following MACRO is a utility MACRO that deletes a DID SDP
    * Service Record (specified by the second parameter) from the SDP
    * Database.  This MACRO simply maps to the
    * SDP_Delete_Service_Record() function.  This MACRO is only provided
    * so that the caller doesn't have to sift through the SDP API for
    * very simplistic applications.  This function accepts as input the
    * Bluetooth Stack ID of the Bluetooth Protocol Stack that the
    * Service Record exists on and the SDP Service Record Handle that
    * was returned via a successful call to DID_Register_SDP_Record().
    * @return This function returns zero if the specified Service Record was
    * able to be deleted successfully, or a negative return error code
    * if the Service Record was NOT able to be deleted successfully.
    */
#define DID_Un_Register_SDP_Record(__BluetoothStackID, __SDPRecordHandle) \
        (SDP_Delete_Service_Record(__BluetoothStackID, __SDPRecordHandle))

   /*! @brief The following function parses an SDP response returned from
    * SDP_Service_Attribute_Request() or
    * SDP_Service_Search_Attribute_Request().
    * @param SDPServiceAttributeResponse The first parameter is a
    * pointer to the SDP Response Data received from the SDP Event
    * Callback function. @param DeviceInformation The second parameter is a pointer to a Device
    * Info structure where the attributes from the DID SDP Record will
    * be stored.  If an optional attribute was found its pointer will
    * point to a non-null memory address, and the
    * [OptionalAttribute]Length variable in the DID_Info_t structure
    * will be set appropriately.  If this function is called within an
    * SDP Response Callback function, the caller is free to use the
    * contents of the optional attributes ONLY in the context of the
    * callback.  If the caller requires the data for a longer period of
    * time the caller MUST copy the data into another buffer.  The
    * optional attribute pointers may or may not point to
    * null terminated strings.  To use an optional attribute as a string
    * the user MUST append a null terminator character if one does not
    * already exist.  If an optional attribute was not found its pointer
    * will be NULL and its [OptionalAttribute]Length variable will be
    * set to 0.  @return This function returns a positive, non-zero, value if
    * successful.  A successful positive return value can be used to
    * determine the Device Identification Profile Version of the remote
    * device.  A negative error code is returned if an error occurs.
    */
BTPSAPI_DECLARATION int BTPSAPI DID_Parse_Device_Information(SDP_Service_Attribute_Response_Data_t *SDPServiceAttributeResponse, DID_Info_t *DeviceInformation);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_DID_Parse_Device_Information_t)(SDP_Service_Attribute_Response_Data_t *SDPServiceAttributeResponse, DID_Info_t *DeviceInformation);
#endif

#endif

