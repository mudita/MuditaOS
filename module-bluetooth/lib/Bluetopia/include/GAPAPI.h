/*
 * Copyright 2000 - 2014 Stonestreet One.
 * All Rights Reserved.
 * Author:  Damon Lange
 *** MODIFICATION HISTORY ****************************************************
 *
 *   mm/dd/yy  F. Lastname    Description of Modification
 *   --------  -----------    ------------------------------------------------
 *   10/27/00  D. Lange       Initial creation.
 *   12/07/07  D. Mason       Changes for BT 2.1
 *   09/18/08  J. Toole       Updates for BT 2.1
 *   06/30/11  T. Cook        Updates for BT 4.0
 *   01/02/14  T. Cook        Changes for BT 4.1
 *   12/07/15  L. Gersi	     Updates for BT 4.2 - LE SC
 ****************************************************************************
 */

/**
 *  @file GAPAPI.h
 *
 *  @brief Stonestreet One Bluetooth Stack Generic Access Profile API
 *      Type Definitions, Constants, and Prototypes. To use any of the following API's,
 *      include the following declaration in your application.
 *
 *  @code
 *  #include "SS1BTPS.h"
 *  @endcode
 *  ============================================================================
 */
#ifndef __GAPAPIH__
#define __GAPAPIH__

#include "HCITypes.h"           /*! HCI Related Type Definitions. */
#include "BTAPITyp.h"           /*! Bluetooth API Type Definitions. */
#include "BTTypes.h"            /*! Bluetooth Type Definitions/Constants. */

#include "SMTypes.h"
#include "BTPSCFG.h"            /*! BTPS Configuration Constants. */

   /*! GAP Basic Rate/Enhanced Data Rate (BR/EDR) type definitions and
    * constants.
    * The following values represent the Time Limits (in seconds) that
    * are allowable for the various Discoverability Modes.  These
    * values are used with the GAP_Set_Discoverability_Mode() function.
    * \note There is a special value defined for a Time Limit of
    *          Infinite for the General Discoverability Mode.
    */
#define MINIMUM_DISCOVERABLE_MODE_TIME_LIMITED_DISCOVERABLE_MODE      (31L)
#define MAXIMUM_DISCOVERABLE_MODE_TIME_LIMITED_DISCOVERABLE_MODE      (60L)

#define MINIMUM_DISCOVERABLE_MODE_TIME_GENERAL_DISCOVERABLE_MODE      (31L)
#define MAXIMUM_DISCOVERABLE_MODE_TIME_GENERAL_DISCOVERABLE_MODE (60L*60L*24L)

#define INFINITE_DISCOVERABLE_MODE_TIME_GENERAL_DISCOVERABLE_MODE         0

   /*! The following values represent the Time Limits (in seconds) that
    * are allowable for the Input Time Limits for the Inquiry Process.
    * These values are used with the GAP_Perform_Inquiry() function.
    * \note If the Minimum and Maximum Inquiry Periods are used then
    *          the following equation *MUST* be satisfied:
    *          Maximum Period Length >
    *          Minimum Period Length >
    *          Inquiry Length
    */
#define MINIMUM_INQUIRY_LENGTH                                            2
#define MAXIMUM_INQUIRY_LENGTH                                           61

#define MINIMUM_MINIMUM_INQUIRY_PERIOD_LENGTH                          (3L)
#define MAXIMUM_MINIMUM_INQUIRY_PERIOD_LENGTH                      (83883L)

#define MINIMUM_MAXIMUM_INQUIRY_PERIOD_LENGTH                          (4L)
#define MAXIMUM_MAXIMUM_INQUIRY_PERIOD_LENGTH                      (83883L)

   /*! The following values represent the Minimum and Maximum Number of
    * actual Bluetooth Devices that are to be waited for during the
    * Inquiry Process.  These values are used with the
    * GAP_Perform_Inquiry() function.
    * \note There is a special value defined for an unlimited number
    *          of Inquiry Responses.
    */
#define MINIMUM_NUMBER_INQUIRY_RESPONSES                                  1
#define MAXIMUM_NUMBER_INQUIRY_RESPONSES                                255

#define INFINITE_NUMBER_INQUIRY_RESPONSES                                 0

   /*! The following enumerated type represents the supported Discovery
    * Modes that a Bluetooth Device can be set to.  These types are
    * used with the GAP_Set_Discoverability_Mode() and the
    * GAP_Query_Discoverability_Mode() functions.
    */
typedef enum
{
   dmNonDiscoverableMode,
   dmLimitedDiscoverableMode,
   dmGeneralDiscoverableMode
} GAP_Discoverability_Mode_t;

   /*! The following enumerated type represents the supported
    * Connectability Modes that a Bluetooth Device can be set to.  These
    * types are used with the GAP_Set_Connectability_Mode() and the
    * GAP_Query_Connectability_Mode() functions.
    */
typedef enum
{
   cmNonConnectableMode,
   cmConnectableMode
} GAP_Connectability_Mode_t;

   /*! The following enumerated type represents the supported
    * Pairability Modes that a Bluetooth Device can be set to.  These
    * types are used with the GAP_Set_Pairability_Mode() and the
    * GAP_Query_Pairability_Mode() functions.
    * \note If the local Bluetooth device is a 4.1 or greater chipset
    *          enabling Secure Simple Pairing (SSP) will also enable
    *          Secure Connections support.
    */
typedef enum
{
  pmNonPairableMode,
  pmPairableMode,
  pmPairableMode_EnableSecureSimplePairing
} GAP_Pairability_Mode_t;

   /*! The following enumerated type represents the supported
    * Authentication Modes that a Bluetooth Device can be set to.  These
    * types are used with the GAP_Set_Authentication_Mode() and the
    * GAP_Query_Authentication_Mode() functions.
    */
typedef enum
{
   amDisabled,
   amEnabled
} GAP_Authentication_Mode_t;

   /*! The following enumerated type represents the supported Encryption
    * Modes that a Bluetooth Device can be set to.  These types are used
    * with the GAP_Set_Encryption_Mode(), GAP_Query_Encryption_Mode()
    * and the GAP_LE_Query_Encryption_Mode() functions.
    * \note For the GAP_Set_Encryption_Mode() API the emEnabled and
    *          emEnabled_AES enumerated values have the same meaning.
    * \note For the return from GAP_Query_Encryption_Mode() (classic
    *          connections only) emEnabled_AES means that the specified
    *          link is encrypted with AES-CCM encryption (4.1 or greater
    *          chipsets only).  emEnabled means that the specified link
    *          is encrypted with E0 encryption.
    * \note For the return from GAP_LE_Query_Encryption_Mode() (low
    *          energy connections only) emEnabled means that the
    *          specified link is encrypted with AES-CCM encryption (4.0
    *          or greater chipsets only).  emEnabled_AES will never be
    *          returned for an LE connection.
    */
typedef enum
{
   emDisabled,
   emEnabled,
   emEnabled_AES
} GAP_Encryption_Mode_t;

   /*! The following enumerated type represents the supported Bonding
    * Types that the Bluetooth Device can be instructed to perform.
    * These types are used with the GAP_Initiate_Bonding() function.
    */
typedef enum
{
   btGeneral,
   btDedicated,
   btDedicated_ManualDisconnect
} GAP_Bonding_Type_t;

   /*! The following enumerated type represents the supported Inquiry
    * Types that can be used when performing an Inquiry Process of
    * Bluetooth Device(s).  These types are used with the
    * GAP_Perform_Inquiry() function.
    */
typedef enum
{
   itGeneralInquiry,
   itLimitedInquiry
} GAP_Inquiry_Type_t;

   /*! The following enumerated type represents the supported Inquiry
    * Modes that a Bluetooth Device can be set to.  These types are used
    * with the GAP_Set_Inquiry_Mode() and the GAP_Query_Inquiry_Mode()
    * functions.
    */
typedef enum
{
   imStandard,
   imRSSI,
   imExtended
} GAP_Inquiry_Mode_t;

   /*! The following enumerated type represents the GAP Event Reason
    * (and valid Data) and is used with the GAP Event Callback.
    */
typedef enum
{
   etInquiry_Result, /*!< Notify the host of the result of a completed inquiry (including all found devices). */
   etEncryption_Change_Result, /*!< Notify the host of a device link encryption change. */
   etAuthentication, /*!< Notify the host of a GAP authentication event. */
   etRemote_Name_Result, /*!< Notify the host of the result of a completed remote name request. */
   etInquiry_Entry_Result, /*!< Notify the host of an individual inquiry result. */
   etInquiry_With_RSSI_Entry_Result, /*!< Notify the host of an individual inquiry result with RSSI information. */
   etExtended_Inquiry_Entry_Result, /*!< Notify the host of an individual inquiry result that contains Extended Inquiry Result information. */
   etEncryption_Refresh_Complete, /*!< Notify the host of the result of a completed encryption refresh request.*/
   etRemote_Features_Result, /*!< Notify the host of the result of a completed remote features request. */
   etRemote_Version_Information_Result, /*!< Notify the host of the result of a completed remote version information request. */
   etAuthenticated_Payload_Timeout /*!< Notify the host that the authenticated payload timeout has expired for the specified connection to a remote device. */
} GAP_Event_Type_t;

   /*! The following type declaration defines an Individual Inquiry
    * Result Entry.  This information forms the Data Portion of the
    * GAP_Inquiry_Event_Data_t structure.
    */
typedef struct _tagGAP_Inquiry_Data_t
{
   BD_ADDR_t         BD_ADDR;
   Byte_t            Page_Scan_Repetition_Mode;
   Byte_t            Page_Scan_Period_Mode;
   Byte_t            Page_Scan_Mode;
   Class_of_Device_t Class_of_Device;
   Word_t            Clock_Offset;
} GAP_Inquiry_Data_t;

#define GAP_INQUIRY_DATA_SIZE                                     (sizeof(GAP_Inquiry_Data_t))

   /*! The following type declaration defines the result of an Inquiry
    * Process that was started via the GAP_Perform_Inquiry() function.
    * The Number of Devices Entry defines the number of Inquiry Data
    * Entries that the GAP_Inquiry_Data member points to (if non-zero).
    */
typedef struct _tagGAP_Inquiry_Event_Data_t
{
   Word_t              Number_Devices;
   GAP_Inquiry_Data_t *GAP_Inquiry_Data;
} GAP_Inquiry_Event_Data_t;

#define GAP_INQUIRY_EVENT_DATA_SIZE                               (sizeof(GAP_Inquiry_Event_Data_t))

   /*! The following type declaration defines an individual result of an
    * Inquiry Process that was started via the GAP_Perform_Inquiry()
    * function.  This event data is generated for each Inquiry Result as
    * it is received.
    */
typedef struct _tagGAP_Inquiry_Entry_Event_Data_t
{
   BD_ADDR_t         BD_ADDR;
   Byte_t            Page_Scan_Repetition_Mode;
   Byte_t            Page_Scan_Period_Mode;
   Byte_t            Page_Scan_Mode;
   Class_of_Device_t Class_of_Device;
   Word_t            Clock_Offset;
} GAP_Inquiry_Entry_Event_Data_t;

#define GAP_INQUIRY_ENTRY_EVENT_DATA_SIZE                         (sizeof(GAP_Inquiry_Entry_Event_Data_t))

   /*! The following type declaration defines an Individual Inquiry
    * Result with RSSI Entry.  This information forms the Data Portion
    * of the GAP_Inquiry_Event_Data_t structure.
    */
typedef struct _tagGAP_Inquiry_With_RSSI_Entry_Event_Data_t
{
   BD_ADDR_t         BD_ADDR;
   Byte_t            Page_Scan_Repetition_Mode;
   Byte_t            Page_Scan_Period_Mode;
   Class_of_Device_t Class_of_Device;
   Word_t            Clock_Offset;
   SByte_t           RSSI;
} GAP_Inquiry_With_RSSI_Entry_Event_Data_t;

#define GAP_INQUIRY_WITH_RSSI_ENTRY_EVENT_DATA_SIZE               (sizeof(GAP_Inquiry_With_RSSI_Entry_Event_Data_t))

   /*! The following structure defines an individual Extended Inquiry
    * Response Structure Entry that is present in an Extended Inquiry
    * Response Data field.  This structure is used with the
    * GAP_Extended_Inquiry_Response_Data_t container structure so that
    * individual entries can be accessed in a convienant, array-like,
    * form.  The first member specifies the Extended Inquiry Response
    * Data type (These types are of the form
    * HCI_EXTENDED_INQUIRY_RESPONSE_DATA_TYPE_xxx, where 'xxx'
    * is the individual Data type).  The second member specifies the
    * length of data that is pointed to by the third member.  The
    * third member points to the actual data for the individual entry
    * (length is specified by the second member).
    * \note The Data_Type member is defined in the specification to
    *          be variable length.  The current specification does not
    *          utilize this member in this way (they are all defined to
    *          be a single octet, currently).
    */
typedef struct _tagGAP_Extended_Inquiry_Response_Data_Entry_t
{
   DWord_t  Data_Type;
   Byte_t   Data_Length;
   Byte_t  *Data_Buffer;
} GAP_Extended_Inquiry_Response_Data_Entry_t;

#define GAP_EXTENDED_INQUIRY_RESPONSE_DATA_ENTRY_SIZE             (sizeof(GAP_Extended_Inquiry_Response_Data_Entry_t))

   /*! The following structure is a container structure that is used to
    * represent all the entries in an Extended Inquiry Response Data
    * Field.  This structure is used so that all fields are easy to
    * parse and access (i.e. there are no MACRO's required to access
    * variable length records).  The first member of this structure
    * specifies how many individual entries are contained in the
    * Extended Inquiry Response Data structure.  The second member is a
    * pointer to an array that contains each individual entry of the
    * Extended Inquiry Response Data structure (note that the number of
    * individual entries pointed to by this array will be specified by
    * the Number_Entries member (first member).
    */
typedef struct _tagGAP_Extended_Inquiry_Response_Data_t
{
   DWord_t                                     Number_Data_Entries;
   GAP_Extended_Inquiry_Response_Data_Entry_t *Data_Entries;
} GAP_Extended_Inquiry_Response_Data_t;

#define GAP_EXTENDED_INQUIRY_RESPONSE_DATA_SIZE                   (sizeof(GAP_Extended_Inquiry_Response_Data_t))

   /*! The following type declaration defines an Extended Inquiry Result
    * This information forms the Data Portion of the
    * GAP_Extended_Inquiry_Entry_Event_Data_t structure.
    */
typedef struct _tagGAP_Extended_Inquiry_Entry_Event_Data_t
{
   BD_ADDR_t                             BD_ADDR;
   Byte_t                                Page_Scan_Repetition_Mode;
   Byte_t                                Reserved;
   Class_of_Device_t                     Class_of_Device;
   Word_t                                Clock_Offset;
   SByte_t                               RSSI;
   GAP_Extended_Inquiry_Response_Data_t  Extended_Inquiry_Response_Data;
   Extended_Inquiry_Response_Data_t     *Raw_Extended_Inquiry_Response_Data;
} GAP_Extended_Inquiry_Entry_Event_Data_t;

#define GAP_EXTENDED_INQUIRY_ENTRY_EVENT_DATA_SIZE                (sizeof(GAP_Extended_Inquiry_Entry_Event_Data_t))

   /*! The following type declaration defines GAP Encryption Status
    * Information that is used with the GAP Encryption Change Result
    * Event.
    */
typedef struct _tagGAP_Encryption_Mode_Event_Data_t
{
   BD_ADDR_t             Remote_Device;
   Byte_t                Encryption_Change_Status;
   GAP_Encryption_Mode_t Encryption_Mode;
} GAP_Encryption_Mode_Event_Data_t;

#define GAP_ENCRYPTION_MODE_EVENT_DATA_SIZE                       (sizeof(GAP_Encryption_Mode_Event_Data_t))

   /*! The following enumerated type represents currently defined
    * Keypress actions that can be specified with the authentication
    * events.
    */
typedef enum
{
   kpEntryStarted,
   kpDigitEntered,
   kpDigitErased,
   kpCleared,
   kpEntryCompleted
} GAP_Keypress_t;

   /*! The following enumerated type is used with the
    * GAP_IO_Capabilities_t structure and Bonding functions to specify
    * the Bonding requirements of the Authentication Requirements of the
    * I/O Capability Request Reply Event.
    */
typedef enum
{
   ibNoBonding,
   ibDedicatedBonding,
   ibGeneralBonding
} GAP_IO_Capability_Bonding_Type_t;

   /*! The following represent the defined I/O Capabilities that can be
    * specified/utilized by this module.
    * Display Only         - The device only has a display with no
    *                        input capability.
    * Display with Yes/No  - The device has both a display and the
    *                        ability for a user to enter yes/no, either
    *                        through a single keypress or via a Keypad
    * Keyboard Only        - The device has no display capability, but
    *                        does have a single key or keypad.
    * No Input nor Output  - The device has no input and no output. A
    *                        device such as this may use Out of Band or
    *                        Just Works associations.
    */
typedef enum
{
   icDisplayOnly,
   icDisplayYesNo,
   icKeyboardOnly,
   icNoInputNoOutput
} GAP_IO_Capability_t;

   /*! The following constant represents the maximum number of digits
    * that can can be specified for a Pass Key.
    */
#define GAP_PASSKEY_MAXIMUM_NUMBER_OF_DIGITS                      (HCI_PASSKEY_MAXIMUM_NUMBER_OF_DIGITS)

   /*! The following constants represent the minimum and maximum values
    * that are valid for Pass Keys.
    */
#define GAP_PASSKEY_MINIMUM_VALUE                                 (HCI_PASSKEY_NUMERIC_VALUE_MINIMUM)
#define GAP_PASSKEY_MAXIMUM_VALUE                                 (HCI_PASSKEY_NUMERIC_VALUE_MAXIMUM)

   /*! The following type declaration defines GAP Encryption Refresh
    * Information that is used with the GAP Encryption Refresh Result
    * Event.
    */
typedef struct _tagGAP_Encryption_Refresh_Complete_Event_Data_t
{
   BD_ADDR_t Remote_Device;
   Byte_t    Status;
} GAP_Encryption_Refresh_Complete_Event_Data_t;

#define GAP_ENCRYPTION_REFRESH_COMPLETE_EVENT_DATA_SIZE           (sizeof(GAP_Encryption_Refresh_Complete_Event_Data_t))

   /*! The following structure defines the Out of Band (OOB) Data that is
    * exchanged during the Out of Band Authentication process.
    * \note The Simple_Pairing_Hash and Simple_Pairing_Randomizer
    *          members are derived from the P-192 public key.
    */
typedef struct _tagGAP_Out_Of_Band_Data_t
{
   Simple_Pairing_Hash_t       Simple_Pairing_Hash;
   Simple_Pairing_Randomizer_t Simple_Pairing_Randomizer;
} GAP_Out_Of_Band_Data_t;

#define GAP_OUT_OF_BAND_DATA_SIZE                                 (sizeof(GAP_Out_Of_Band_Data_t))

   /*! The following structure defines the extended Out of Band (OOB)
    * Data that is exchanged during the Out of Band Authentication
    * process.
    * \note The Simple_Pairing_Hash_192 and
    *          Simple_Pairing_Randomizer_192 members are derived from
    *          the P-192 public key.
    * \note The Simple_Pairing_Hash_256 and
    *          Simple_Pairing_Randomizer_256 members are derived from
    *          the P-256 public key.
    */
typedef struct _tagGAP_Extended_Out_Of_Band_Data_t
{
   Simple_Pairing_Hash_t       Simple_Pairing_Hash_192;
   Simple_Pairing_Randomizer_t Simple_Pairing_Randomizer_192;
   Simple_Pairing_Hash_t       Simple_Pairing_Hash_256;
   Simple_Pairing_Randomizer_t Simple_Pairing_Randomizer_256;
} GAP_Extended_Out_Of_Band_Data_t;

#define GAP_EXTENDED_OUT_OF_BAND_DATA_SIZE                        (sizeof(GAP_Extended_Out_Of_Band_Data_t))

   /*! The following structure defines the I/O Capabilities supported
    * during Capablities exchange during the Authentication Process
    * (required during Secure Simple Pairing).
    */
typedef struct _tagGAP_IO_Capabilities_t
{
   GAP_IO_Capability_t              IO_Capability;
   Boolean_t                        OOB_Data_Present;
   Boolean_t                        MITM_Protection_Required;
   GAP_IO_Capability_Bonding_Type_t Bonding_Type;
   Boolean_t                        OOB_256_Data_Present;
} GAP_IO_Capabilities_t;

#define GAP_IO_CAPABILITIES_SIZE                                  (sizeof(GAP_IO_Capabilities_t))

   /*! The following define is used to determine if the structure passed
    * to this layer is the legacy GAP_IO_Capabilities_t structure before
    * the addition of the OOB_256_Data_Present member.
    */
#define GAP_LEGACY_IO_CAPABILITIES_SIZE                           (BTPS_STRUCTURE_OFFSET(GAP_IO_Capabilities_t, OOB_256_Data_Present))

   /*! The following enumerated type is used with the Authentication
    * Event Data Structure and defines the reason that the
    * Authentication Callback was issued, this defines what data in the
    * structure is pertinant.
    */
typedef enum
{
   atLinkKeyRequest,
   atPINCodeRequest,
   atAuthenticationStatus,
   atLinkKeyCreation,
   atIOCapabilityRequest,
   atUserConfirmationRequest,
   atPasskeyRequest,
   atPasskeyNotification,
   atKeypressNotification,
   atRemoteOutOfBandDataRequest,
   atIOCapabilityResponse,
   atSecureSimplePairingComplete
} GAP_Authentication_Event_Type_t;

   /*! The following enumerated type represents the different
    * Authentication Methods that can be used.
    */
typedef enum
{
   atLinkKey,
   atPINCode,
   atUserConfirmation,
   atPassKey,
   atKeypress,
   atOutOfBandData,
   atIOCapabilities
} GAP_Authentication_Type_t;

   /*! The following type declaration defines GAP Authentication
    * Information that can be set and/or returned.  The first member
    * of this structure specifies which Data Member should be used.
    * \note For GAP Authentication Types that are rejections, the
    *          Authentication_Data_Length member is set to zero and
    *          All Data Members can be ignored (since none are valid).
    * \note Currently the Bonding_Type member of the IO_Capabilities
    *          member is ignored.  The correct value is calculated and
    *          inserted automatically.
    */
typedef struct _tagGAP_Authentication_Information_t
{
   GAP_Authentication_Type_t          GAP_Authentication_Type;
   Byte_t                             Authentication_Data_Length;
   union
   {
      PIN_Code_t                      PIN_Code;
      Link_Key_t                      Link_Key;
      Boolean_t                       Confirmation;
      DWord_t                         Passkey;
      GAP_Keypress_t                  Keypress;
      GAP_Out_Of_Band_Data_t          Out_Of_Band_Data;
      GAP_Extended_Out_Of_Band_Data_t Extended_Out_Of_Band_Data;
      GAP_IO_Capabilities_t           IO_Capabilities;
   } Authentication_Data;
} GAP_Authentication_Information_t;

#define GAP_AUTHENTICATION_INFORMATION_SIZE                       (sizeof(GAP_Authentication_Information_t))

   /*! The following type declaration specifies the Link Key information
    * included in a GAP_Authentication_Event_Data structure when the
    * GAP_Authentication_Event_Type = atLinkKeyCreation.
    */
typedef struct _tagGAP_Authentication_Event_Link_Key_Info_t
{
   Link_Key_t Link_Key;
   Byte_t     Key_Type;
} GAP_Authentication_Event_Link_Key_Info_t;

#define GAP_AUTHENTICATION_EVENT_LINK_KEY_INFO_SIZE               (sizeof(GAP_Authentication_Event_Link_Key_Info_t))

   /*! The following type declaration specifies the information that can
    * be returned in a GAP_Authentication_Callback.  This information
    * is passed to the Callback when a GAP_Authentication Callback is
    * issued.  The first member of this structure specifies which Data
    * Member is valid.  Currently the following members are valid for
    * the following values of the GAP_Authentication_Event_Type member:
    *
    *    atLinkKeyRequest              - No Further Data.
    *    atPINCodeRequest              - No Further Data.
    *    atAuthenticationStatus        - Authentication_Status is valid.
    *    atLinkKeyCreation             - Link_Key_Info is valid.
    *    atKeypressNotification        - Keypress_Type is valid.
    *    atUserConfirmationRequest     - Numeric_Value is valid.
    *    atPasskeyNotification         - Numeric_Value is valid.
    *    atPasskeyRequest              - No Further Data.
    *    atRemoteOutOfBandDataRequest  - No Further Data.
    *    atIOCapabilityRequest         - Remote_IO_Capabilities_Known is
    *                                    valid.
    *    atIOCapabilityResponse        - IO_Capabilities is valid.
    *    atSecureSimplePairingComplete - Secure_Simple_Pairing_Status is
    *                                    valid.
    *
    */
typedef struct _tagGAP_Authentication_Event_Data_t
{
   GAP_Authentication_Event_Type_t GAP_Authentication_Event_Type;
   BD_ADDR_t                       Remote_Device;
   union
   {
      Byte_t                                   Authentication_Status;
      Byte_t                                   Secure_Simple_Pairing_Status;
      Boolean_t                                Remote_IO_Capabilities_Known;
      GAP_Authentication_Event_Link_Key_Info_t Link_Key_Info;
      DWord_t                                  Numeric_Value;
      GAP_Keypress_t                           Keypress_Type;
      GAP_IO_Capabilities_t                    IO_Capabilities;
   } Authentication_Event_Data;
} GAP_Authentication_Event_Data_t;

#define GAP_AUTHENTICATION_EVENT_DATA_SIZE                        (sizeof(GAP_Authentication_Event_Data_t))

   /*! The following structure represents the GAP Remote Name Response
    * Event Data that is returned from the
    * GAP_Query_Remote_Device_Name() function.  The Remote_Name
    * member will point to a NULL terminated string that represents
    * the User Friendly Bluetooth Name of the Remote Device associated
    * with the specified BD_ADDR.
    */
typedef struct _tagGAP_Remote_Name_Event_Data_t
{
   Byte_t     Remote_Name_Status;
   BD_ADDR_t  Remote_Device;
   char      *Remote_Name;
} GAP_Remote_Name_Event_Data_t;

#define GAP_REMOTE_NAME_EVENT_DATA_SIZE                           (sizeof(GAP_Remote_Name_Event_Data_t))

   /*! The following structure represents the GAP Remote Features
    * Response Event Data that is returned from the
    * GAP_Query_Remote_Features() function.
    */
typedef struct _tagGAP_Remote_Features_Event_Data_t
{
   Byte_t         Status;
   BD_ADDR_t      BD_ADDR;
   LMP_Features_t Features;
   Byte_t         Page_Number;
   Byte_t         Maximum_Page_Number;
} GAP_Remote_Features_Event_Data_t;

#define GAP_REMOTE_FEATURES_EVENT_DATA_SIZE                       (sizeof(GAP_Remote_Features_Event_Data_t))

   /*! The following structure represents the GAP Remote Features
    * Response Event Data that is returned from the
    * GAP_Query_Remote_Features() function.
    */
typedef struct _tagGAP_Remote_Version_Information_Event_Data_t
{
   Byte_t         Status;
   BD_ADDR_t      BD_ADDR;
   Byte_t         LMP_Version;
   Word_t         Manufacturer_ID;
   Word_t         LMP_Subversion;
} GAP_Remote_Version_Information_Event_Data_t;

#define GAP_REMOTE_VERSION_INFORMATION_EVENT_DATA_SIZE            (sizeof(GAP_Remote_Version_Information_Event_Data_t))

   /*! The following structure represents the GAP Authentication Payload
    * Timeout Event Data that is returned when the a packet with a valid
    * MIC is not received from the remote device in the time specified
    * by the GAP_Set_Authenticated_Payload_Timeout() function.
    */
typedef struct _tagGAP_Authenticated_Payload_Timeout_Event_Data_t
{
   BD_ADDR_t BD_ADDR;
} GAP_Authenticated_Payload_Timeout_Event_Data_t;

#define GAP_AUTHENTICATED_PAYLOAD_TIMEOUT_EVENT_DATA_SIZE         (sizeof(GAP_Authenticated_Payload_Timeout_Event_Data_t))

   /*! The following structure represents the container structure that
    * holds all GAP Event Data Data.
    */
typedef struct _tagGAP_Event_Data_t
{
   GAP_Event_Type_t Event_Data_Type;
   Word_t           Event_Data_Size;
   union
   {
      GAP_Inquiry_Event_Data_t                       *GAP_Inquiry_Event_Data;
      GAP_Encryption_Mode_Event_Data_t               *GAP_Encryption_Mode_Event_Data;
      GAP_Authentication_Event_Data_t                *GAP_Authentication_Event_Data;
      GAP_Remote_Name_Event_Data_t                   *GAP_Remote_Name_Event_Data;
      GAP_Inquiry_Entry_Event_Data_t                 *GAP_Inquiry_Entry_Event_Data;
      GAP_Inquiry_With_RSSI_Entry_Event_Data_t       *GAP_Inquiry_With_RSSI_Entry_Event_Data;
      GAP_Extended_Inquiry_Entry_Event_Data_t        *GAP_Extended_Inquiry_Entry_Event_Data;
      GAP_Encryption_Refresh_Complete_Event_Data_t   *GAP_Encryption_Refresh_Complete_Event_Data;
      GAP_Remote_Features_Event_Data_t               *GAP_Remote_Features_Event_Data;
      GAP_Remote_Version_Information_Event_Data_t    *GAP_Remote_Version_Information_Event_Data;
      GAP_Authenticated_Payload_Timeout_Event_Data_t *GAP_Authenticated_Payload_Timeout_Event_Data;
   } Event_Data;
} GAP_Event_Data_t;

#define GAP_EVENT_DATA_SIZE                                       (sizeof(GAP_Event_Data_t))

   /*! The following declared type represents the Prototype Function for
    * the GAP Event Receive Data Callback. This function will be called
    * whenever a Callback has been registered for the specified GAP
    * Action that is associated with the specified Bluetooth Stack ID.
    * @param BluetoothStackID  Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize
    * @param GAP_Event_Data  Pointer to the passed event data.
    * @param CallbackParameter User-defined parameter (e.g., tag value)
    * that was defined in the callback registration.
    * The caller is free to use the contents of the GAP Event Data ONLY
    * in the context of this callback.  If the caller requires the Data
    * for a longer period of time, then the callback function MUST copy
    * the data into another Data Buffer.  This function is guaranteed
    * NOT to be invoked more than once simultaneously for the specified
    * installed callback (i.e. this  function DOES NOT have be
    * reentrant).  It Needs to be noted however, that if the same
    * Callback is installed more than once, then the callbacks will be
    * called serially.  Because of this, the processing in this function
    * should be as efficient as possible.  It should also be noted that
    * this function is called in the Thread Context of a Thread that
    * the User does NOT own.  Therefore, processing in this function
    * should be as efficient as possible (this argument holds anyway
    * because other GAP Events will not be processed while this
    * function call is outstanding).
    * \note This function MUST NOT Block and wait for events that
    *            can only be satisfied by Receiving other GAP Events.
    *            A Deadlock WILL occur because NO GAP Event
    *            Callbacks will be issued while this function is
    *            currently outstanding.
    */
typedef void (BTPSAPI *GAP_Event_Callback_t)(unsigned int BluetoothStackID, GAP_Event_Data_t *GAP_Event_Data, unsigned long CallbackParameter);

   /*! GAP Low Energy (LE) type definitions and constants.
    * The following values represent the minimum and maximum LE Scan
    * Interval and Window followings that are allowing to be used when
    * calling GAP_LE_Perform_Scan.
    * \note The Scan Window should always be less than Scan Interval
    *          unless continous scanning is required (potentially using
    *          more power) in which case they may be equal.
    * \note These values are specified in milli-seconds.
    */
#define MINIMUM_LE_SCAN_INTERVAL                                          3
#define MAXIMUM_LE_SCAN_INTERVAL                                      10240

#define MINIMUM_LE_SCAN_WINDOW                                            3
#define MAXIMUM_LE_SCAN_WINDOW                                        10240

   /*! The following values represent the Minimum Scan Length (in
    * seconds) that are allowable for the Scan Length for the LE Scan
    * Process.  These values are used with the GAP_LE_Perform_Scan()
    * function and are in seconds.
    * \note These values are specified in seconds.
    */
#define MINIMUM_LIMITED_DISCOVERABLE_SCAN_LENGTH                         11
#define MINIMUM_GENERAL_DISCOVERABLE_SCAN_LENGTH                         11

   /*! The following values represent the Minimum and Maximum Advertising
    * Interval Min and Max values.
    * \note These values are specified in milli-seconds.
    */
#define MINIMUM_ADVERTISING_INTERVAL                                     20
#define MAXIMUM_ADVERTISING_INTERVAL                                  10240
#define MINIMUM_NONCONNECTABLE_ADVERTISING_INTERVAL                     100

   /*! The following values represent the minimum and maximum connection
    * and default intervals that may be passed into the
    * GAP_LE_Create_Connection() function with the ConnectionParameters
    * parameter.
    * \note These are specified in milli-seconds.
    */
#define MINIMUM_MINIMUM_CONNECTION_INTERVAL                               8
#define MAXIMUM_MINIMUM_CONNECTION_INTERVAL                            4000

#define DEFAULT_MINIMUM_CONNECTION_INTERVAL                             500

#define MINIMUM_MAXIMUM_CONNECTION_INTERVAL                               8
#define MAXIMUM_MAXIMUM_CONNECTION_INTERVAL                            4000

#define DEFAULT_MAXIMUM_CONNECTION_INTERVAL                            1280

   /*! The following bit mask constants represent the bit mask values
    * that can be specified for the Advertising_Channel_Map member of
    * the GAP_LE_Advertising_Parameters_t structure to specify the
    * advertising channels that should be used.
    */
#define GAP_LE_ADVERTISING_CHANNEL_MAP_USE_CHANNEL_37                   (HCI_LE_ADVERTISING_CHANNEL_MAP_ENABLE_CHANNEL_37)
#define GAP_LE_ADVERTISING_CHANNEL_MAP_USE_CHANNEL_38                   (HCI_LE_ADVERTISING_CHANNEL_MAP_ENABLE_CHANNEL_38)
#define GAP_LE_ADVERTISING_CHANNEL_MAP_USE_CHANNEL_39                   (HCI_LE_ADVERTISING_CHANNEL_MAP_ENABLE_CHANNEL_39)
#define GAP_LE_ADVERTISING_CHANNEL_MAP_USE_ALL_CHANNELS                 (HCI_LE_ADVERTISING_CHANNEL_MAP_ENABLE_ALL_CHANNELS)

   /*! The following values represent the minimum, maximum and default
    * slave latency parameters that may be used in the
    * GAP_LE_Create_Connection() function with the ConnectionParameters
    * parameter.
    */
#define MINIMUM_SLAVE_LATENCY                                             0
#define MAXIMUM_SLAVE_LATENCY                                           500
#define DEFAULT_SLAVE_LATENCY                                             0

   /*! The following values represent the minimum, maximum and default
    * link supervision timeout that may be into the
    * GAP_LE_Create_Connection() function with the ConnectionParameters
    * parameter.
    * \note These are specified in milli-seconds.
    */
#define MINIMUM_LINK_SUPERVISION_TIMEOUT                                100
#define MAXIMUM_LINK_SUPERVISION_TIMEOUT                              32000
#define DEFAULT_LINK_SUPERVISION_TIMEOUT                              28000

   /*! The following values represent the minimum and maximum values for
    * the connection event parameters that are used with the
    * ConnectionParameters structure that is passed to the
    * GAP_LE_Create_Connection() function.
    * \note These are specified in milli-seconds.
    */
#define MINIMUM_CONNECTION_EVENT_LENGTH                                   0
#define MAXIMUM_CONNECTION_EVENT_LENGTH                               40959

   /*! The following values represent the minimum and maximum Encryption
    * Key size in the parameters to the
    * GAP_LE_Pair_Remote_Device() and GAP_LE_Authentication_Response()
    * functions.
    */
#define GAP_LE_MINIMUM_ENCRYPTION_KEY_SIZE                                7
#define GAP_LE_MAXIMUM_ENCRYPTION_KEY_SIZE                               16

   /*! The following MACRO is a utility MACRO that exists to aid in
    * determining if a specified Encryption Key Size is valid.
    */
#define GAP_LE_ENCRYPTION_KEY_SIZE_VALID(_x)   (((_x) >= GAP_LE_MINIMUM_ENCRYPTION_KEY_SIZE) && ((_x) <= GAP_LE_MAXIMUM_ENCRYPTION_KEY_SIZE))

   /*! The following MACRO is a utility MACRO that exists to aid in
    * determining if a BD_ADDR_t structure contains a BD_ADDR that is an
    * LE Static Private Address.  The MACRO returns Non-Zero if the
    * BD_ADDR is an LE Static Private Address. @param _x The first parameter to
    * this MACRO is a pointer to a BD_ADDR_t structure.
    */
#define GAP_LE_TEST_STATIC_ADDRESS_BITS(_x)    \
   ((((_x).BD_ADDR5) & (1 << 6)) && (((_x).BD_ADDR5) & (1 << 7)))

   /*! The following MACRO is a utility MACRO that exists to aid in
    * determining if a BD_ADDR_t structure contains a BD_ADDR that is an
    * LE Non-Resolvable Private Address.  The MACRO returns Non-Zero if
    * the BD_ADDR is an LE Non-Resolvable Private Address. @param _x The first parameter to this MACRO is a pointer to a BD_ADDR_t structure.
    */
#define GAP_LE_TEST_NONRESOLVABLE_ADDRESS_BITS(_x)    \
   ((!(((_x).BD_ADDR5) & (1 << 6))) && (!(((_x).BD_ADDR5) & (1 << 7))))

   /*! The following MACRO is a utility MACRO that exists to aid in
    * determining if a BD_ADDR_t structure contains a BD_ADDR that is
    * an LE Resolvable Private Address.  The MACRO returns Non-Zero if
    * the BD_ADDR is an LE Resolvable Private Address. @param _x The first parameter to this MACRO is a pointer to a BD_ADDR_t structure.
    */
#define GAP_LE_TEST_RESOLVABLE_ADDRESS_BITS(_x)    \
   ((((_x).BD_ADDR5) & (1 << 6)) && (!(((_x).BD_ADDR5) & (1 << 7))))

   /*! The following MACRO defines the maximum number of digits that may
    * be entered as an LE Passkey.
    */
#define GAP_LE_PASSKEY_MAXIMUM_NUMBER_OF_DIGITS                           6

   /*! The following enumerated type represents the types of scans that
    * may be started via a call to the GAP_LE_Perform_Scan() function.
    */
typedef enum
{
   stPassive,
   stActive
} GAP_LE_Scan_Type_t;

   /*! The following enumerated type represents the type of filter
    * policy that may be used for functions that accept a filter policy
    * parameter.
    */
typedef enum
{
   fpNoFilter,
   fpWhiteList
} GAP_LE_Filter_Policy_t;

   /*! The following enumerated type represents the possible Advertising
    * Report types that can be returned via the GAP
    * etLE_Advertising_Report for each device in the mentioned events.
    */
typedef enum
{
   rtConnectableUndirected,
   rtConnectableDirected,
   rtScannableUndirected,
   rtNonConnectableUndirected,
   rtScanResponse
} GAP_LE_Advertising_Report_Type_t;

   /*! The following enumerated type represents the GAP LE connectability
    * mode that may be used.
    */
typedef enum
{
   lcmNonConnectable,
   lcmConnectable,
   lcmDirectConnectable,
   lcmLowDutyCycleDirectConnectable
} GAP_LE_Connectability_Mode_t;

   /*! The following enumerated type represents the supported
    * Pairability Modes that a Bluetooth device can be set to.  These
    * types are used with the GAP_LE_Set_Pairability_Mode() and the
    * GAP_LE_Query_Pairability_Mode() functions.
    */
typedef enum
{
   lpmNonPairableMode,
   lpmPairableMode
} GAP_LE_Pairability_Mode_t;

   /*! The following enumerated type represents the different IO
    * Capabilities an LE device may have (used during pairing
    * procedure).
    */
typedef enum
{
   licDisplayOnly,
   licDisplayYesNo,
   licKeyboardOnly,
   licNoInputNoOutput,
   licKeyboardDisplay
} GAP_LE_IO_Capability_t;

   /*! The following enumerated type represents the different Bonding
    * types that an LE device may set.
    */
typedef enum
{
   lbtNoBonding,
   lbtBonding
} GAP_LE_Bonding_Type_t;

   /*! The following enumerated type represents the different LE Address
    * Types.
    */
typedef enum
{
   latPublic,
   latRandom
} GAP_LE_Address_Type_t;

   /*! The following structure represents the type of connectability
    * parameters that may be passed into GAP_LE_Set_Advertising_Enable()
    * function.  The first member specifies the type of connectability
    * Mode to use.  The next member specifies the type of address for
    * the Local Bluetooth device to use.  The next two members are only
    * used if the ConnectabilityMode member is set to
    * lcmDirectConnectable and represent the Direct Address Type and
    * the Direct Address to attempt to connect to.
    */
typedef struct _tagGAP_LE_Connectability_Parameters_t
{
   GAP_LE_Connectability_Mode_t Connectability_Mode;
   GAP_LE_Address_Type_t        Own_Address_Type;
   GAP_LE_Address_Type_t        Direct_Address_Type;
   BD_ADDR_t                    Direct_Address;
} GAP_LE_Connectability_Parameters_t;

#define GAP_LE_CONNECTABILITY_PARAMETERS_SIZE                     (sizeof(GAP_LE_Connectability_Parameters_t))

   /*! The following structure represents the type of Advertising
    * parameters that may be passed into the
    * GAP_LE_Set_Advertising_Enable() function.
    * \note The Advertising_Interval_Min and Advertising_Interval_Max
    *          members are specified in milli-seconds.
    */
typedef struct _tagGAP_LE_Advertising_Parameters_t
{
   Word_t                 Advertising_Interval_Min;
   Word_t                 Advertising_Interval_Max;
   Byte_t                 Advertising_Channel_Map;
   GAP_LE_Filter_Policy_t Scan_Request_Filter;
   GAP_LE_Filter_Policy_t Connect_Request_Filter;
} GAP_LE_Advertising_Parameters_t;

#define GAP_LE_ADVERTISING_PARAMETERS_SIZE                        (sizeof(GAP_LE_Advertising_Parameters_t))

   /*! The following structure is a container structure for all the
    * connection parameters passed into the GAP_LE_Create_Connection()
    * function.
    * \note All members of this structure are specified in
    *          milli-seconds EXCEPT Slave_Latency which is specified in
    *          number of connection events.
    */
typedef struct _tagGAP_LE_Connection_Parameters_t
{
   Word_t Connection_Interval_Min;
   Word_t Connection_Interval_Max;
   Word_t Slave_Latency;
   Word_t Supervision_Timeout;
   Word_t Minimum_Connection_Length;
   Word_t Maximum_Connection_Length;
} GAP_LE_Connection_Parameters_t;

#define GAP_LE_CONNECTION_PARAMETERS_SIZE                         (sizeof(GAP_LE_Connection_Parameters_t))

   /*! The following structure is a container structure for all the
    * connection parameters that are present for all LE connections.
    * \note The Connection_Interval and Supervision_Timeout members
    *          are specified in milliseconds.
    * \note The Slave_Latency member is specified in number of
    *          connection intervals.
    */
typedef struct _tagGAP_LE_Current_Connection_Parameters_t
{
   Word_t Connection_Interval;
   Word_t Slave_Latency;
   Word_t Supervision_Timeout;
} GAP_LE_Current_Connection_Parameters_t;

   /*! The following structure represents the data structure that is the
    * structure that represents the data for an individual White List
    * entry that is used with the White List Management functions in
    * this module.
    */
typedef struct _tagGAP_LE_White_List_Entry_t
{
   GAP_LE_Address_Type_t Address_Type;
   BD_ADDR_t             Address;
} GAP_LE_White_List_Entry_t;

#define GAP_LE_WHITE_LIST_ENTRY_SIZE                              (sizeof(GAP_LE_White_List_Entry_t))

   /*! The following structure represents the GAP LE Remote Features
    * Response Event Data that is returned from the
    * GAP_LE_Read_Remote_Features() function in the
    * etLE_Remote_Features GAP LE Event.
    */
typedef struct _tagGAP_LE_Remote_Features_Event_Data_t
{
   Byte_t        Status;
   BD_ADDR_t     BD_ADDR;
   LE_Features_t LE_Features;
} GAP_LE_Remote_Features_Event_Data_t;

#define GAP_LE_REMOTE_FEATURES_EVENT_DATA_SIZE                    (sizeof(GAP_LE_Remote_Features_Event_Data_t))

   /*! The following data type represents the structure of the Initiator
    * and Responder Key Distribution fields in the
    * GAP_LE_Pairing_Capabilities_t information.
    */
typedef struct _tagGAP_LE_Key_Distribution_t
{
   Boolean_t Encryption_Key;
   Boolean_t Identification_Key;
   Boolean_t Signing_Key;
} GAP_LE_Key_Distribution_t;

#define GAP_LE_KEY_DISTRIBUTION_SIZE                              (sizeof(GAP_LE_Key_Distribution_t))

   /*! The following type declaration represents the information that
    * is requested in a latPairingRequest of the etLE_Authentication
    * event.
    * \note The RemoteKeys structure represents the keys that the
    *          LOCAL device is requesting from the REMOTE device.
    *          The LocalKeys structure represents the keys that the
    *          LOCAL device is requesting to send to the REMOTE device.
    */
typedef struct _tagGAP_LE_Pairing_Capabilities_t
{
   GAP_LE_IO_Capability_t    IO_Capability;
   Boolean_t                 OOB_Present;
   GAP_LE_Bonding_Type_t     Bonding_Type;
   Boolean_t                 MITM;
   Boolean_t                 SC;
   Boolean_t                 Keypress;
   Boolean_t                 P256DebugMode;
   Byte_t                    Maximum_Encryption_Key_Size;
   GAP_LE_Key_Distribution_t Receiving_Keys;
   GAP_LE_Key_Distribution_t Sending_Keys;
} GAP_LE_Pairing_Capabilities_t;

#define GAP_LE_PAIRING_CAPABILITIES_SIZE                          (sizeof(GAP_LE_Pairing_Capabilities_t))

   /*! The following structure represents the structure of data that is
    * used to specify the Out Of Band Information when pairing.
    */
typedef struct _tagGAP_LE_OOB_Data_t
{
   Encryption_Key_t OOB_Key;
} GAP_LE_OOB_Data_t;

#define GAP_LE_OOB_DATA_SIZE                                      (sizeof(GAP_LE_OOB_Data_t))

   /*! The following structure represents the structure of data that is
    * used to specify the Out Of Band Information when using Secure Connections pairing.
    */
typedef struct _tagGAP_LE_SC_OOB_Data_t
{
   SM_Random_Value_t RemoteRand;
   SM_Confirm_Value_t RemoteConfirmation;
} GAP_LE_SC_OOB_Data_t;

#define GAP_LE_SC_OOB_DATA_SIZE                                      (sizeof(GAP_LE_SC_OOB_Data_t))

   /*! The following structure represents the structure of data that is
    * used to specify the current encryption information.
    */
typedef struct _tagGAP_LE_Encryption_Information_t
{
   Byte_t          Encryption_Key_Size;
   Long_Term_Key_t LTK;
   Word_t          EDIV;
   Random_Number_t Rand;
} GAP_LE_Encryption_Information_t;

#define GAP_LE_ENCRYPTION_INFORMATION_DATA_SIZE                   (sizeof(GAP_LE_Encryption_Information_t))

   /*! The following structure represents the structure of data that is
    * used to specify the current identity information.
    */
typedef struct _tagGAP_LE_Identity_Information_t
{
   Encryption_Key_t      IRK;
   GAP_LE_Address_Type_t Address_Type;
   BD_ADDR_t             Address;
} GAP_LE_Identity_Information_t;

#define GAP_LE_IDENTITY_INFORMATION_DATA_SIZE                     (sizeof(GAP_LE_Identity_Information_t))

   /*! The following structure represents the structure of data that is
    * used to specify the current signing information.
    */
typedef struct _tagGAP_LE_Signing_Information_t
{
   Encryption_Key_t CSRK;
} GAP_LE_Signing_Information_t;

#define GAP_LE_SIGNING_INFORMATION_DATA_SIZE                      (sizeof(GAP_LE_Signing_Information_t))

   /*! The following structure represents the structure of data that
    * represents a Long Term Key and the key size.
    */
typedef struct _tagGAP_LE_Long_Term_Key_Information_t
{
   Byte_t          Encryption_Key_Size;
   Long_Term_Key_t Long_Term_Key;
} GAP_LE_Long_Term_Key_Information_t;

#define GAP_LE_LONG_TERM_KEY_INFORMATION_DATA_SIZE               (sizeof(GAP_LE_Long_Term_Key_Information_t))

   /*! The following enumerated type represents the different
    * Authentication Methods that can be used.
    * \note These values are used when responding with an
    *          Authentication Response.
    */
typedef enum
{
   larLongTermKey,
   larOutOfBandData,
   larPairingCapabilities,
   larPasskey,
   larUserConfirmation, 
   larConfirmation,
   larError,
   larEncryptionInformation,
   larIdentityInformation,
   larSigningInformation
} GAP_LE_Authentication_Response_Type_t;

   /*! The following type declaration defines GAP Authentication Response
    * Information that can be set.  The first member of this structure
    * specifies which Data Member should be used.
    * \note For GAP Authentication Response Types that are
    *          rejections, the Authentication_Data_Length member is set
    *          to zero and All Data Members can be ignored (since none
    *          are valid).
    */
typedef struct _tagGAP_LE_Authentication_Response_Information_t
{
   GAP_LE_Authentication_Response_Type_t GAP_LE_Authentication_Type;
   Byte_t                                Authentication_Data_Length;
   union
   {
      GAP_LE_Long_Term_Key_Information_t Long_Term_Key_Information;
      GAP_LE_Pairing_Capabilities_t      Pairing_Capabilities;
      GAP_LE_OOB_Data_t                  Out_Of_Band_Data;
      DWord_t                            Passkey;
      Boolean_t							 AcceptedNumericValue;
      GAP_LE_SC_OOB_Data_t				 SC_Out_Of_Band_Data;
      Byte_t                             Error_Code;
      GAP_LE_Encryption_Information_t    Encryption_Information;
      GAP_LE_Identity_Information_t      Identity_Information;
      GAP_LE_Signing_Information_t       Signing_Information;
   } Authentication_Data;
} GAP_LE_Authentication_Response_Information_t;

#define GAP_LE_AUTHENTICATION_RESPONSE_INFORMATION_SIZE           (sizeof(GAP_LE_Authentication_Response_Information_t))

   /*! The following type declaration represents the structure of
    * information stored in the GAP_LE_Security_Information_t structure
    * which is passed to the GAP_LE_Re_establish_Security() function
    * when the local device is the slave of the specified connection.
    */
typedef struct _tagGAP_LE_Slave_Security_Information_t
{
   GAP_LE_Bonding_Type_t Bonding_Type;
   Boolean_t             MITM;
   Boolean_t             SC;
   Boolean_t             Keypress;
   Boolean_t		   	 P256DebugMode;
} GAP_LE_Slave_Security_Information_t;

#define GAP_LE_SLAVE_SECURITY_INFORMATION_SIZE                    (sizeof(GAP_LE_Slave_Security_Information_t))

   /*! The following type declaration represents the structure of
    * information stored in the GAP_LE_Security_Information_t structure
    * which is passed to the GAP_LE_Reestablish_Security() function when
    * we the local device is the master of the specified connection.
    */
typedef struct _tagGAP_LE_Master_Security_Information_t
{
   Byte_t          Encryption_Key_Size;
   Long_Term_Key_t LTK;
   Word_t          EDIV;
   Random_Number_t Rand;
} GAP_LE_Master_Security_Information_t;

#define GAP_LE_MASTER_SECURITY_INFORMATION_SIZE                   (sizeof(GAP_LE_Master_Security_Information_t))

   /*! The following type declaration is a container structure that
    * contains the data passed to the GAP_LE_Reestablish_Security()
    * function, which takes different data parameters based on the local
    * device role.
    */
typedef struct _tagGAP_LE_Security_Information_t
{
   Boolean_t Local_Device_Is_Master;
   union
   {
      GAP_LE_Slave_Security_Information_t  Slave_Information;
      GAP_LE_Master_Security_Information_t Master_Information;
   } Security_Information;
} GAP_LE_Security_Information_t;

#define GAP_LE_SECURITY_INFORMATION_DATA_SIZE                     (sizeof(GAP_LE_Security_Information_t))

   /*! The following enumerated type represents the GAP LE Event Reason
    * (and valid Data) and is used with the GAP Event Callback.
    */
typedef enum
{
   etLE_Remote_Features_Result, /*!< Notify the host of the result of a completed LE remote features request. */
   etLE_Advertising_Report, /*!< Notify the host of an individual advertising report that was received during a scanning procedure. */
   etLE_Connection_Complete, /*!< Notify the host that a device is now connected. */
   etLE_Disconnection_Complete, /*!< Notify the host that a device is no longer connected. */
   etLE_Encryption_Change, /*!< Notify the host of a LE device link encryption change. */
   etLE_Encryption_Refresh_Complete, /*!< Notify the host of the result of a completed LE encryption refresh request. */
   etLE_Authentication, /*!< Notify the host of a GAP LE authentication event. */
   etLE_Connection_Parameter_Update_Request, /*!< Notify the host of a connection parameter update request (received by master from a connected slave). */
   etLE_Connection_Parameter_Update_Response, /*!< Notify the host of the status of a connection parameter update request (received by slave from a connected master). */
   etLE_Connection_Parameter_Updated, /*!< Notify the host of a change in the connection parameters for a specified connection. */
   etLE_Authenticated_Payload_Timeout_Expired  /*!< Notify the host that the authenticated payload timeout has expired for the specified LE connection to a remote device. */
} GAP_LE_Event_Type_t;

   /*! The following structure defines an individual Advertising Report
    * Structure Entry that is present in an LE Advertising Report.
    * This structure is used with the GAP_LE_Advertising_Report_Data_t
    * container structure so that individual entries can be accessed in
    * a convenient, array-like, form.  The first member specifies the
    * Advertising Data type (These types are of the form
    * HCI_LE_ADVERTISING_REPORT_DATA_TYPE_xxx where 'xxx' is the
    * individual Data type).  The second member specifies the length of
    * data that is pointed to by the third member.  The third member
    * points to the actual data for the individual entry (length is
    * specified by the second member).
    * \note The AD_Type member is defined in the specification to
    *          be variable length.  The current specification does not
    *          utilize this member in this way (they are all defined to
    *          be a single octet, currently).
    */
typedef struct _tagGAP_LE_Advertising_Data_Entry_t
{
   DWord_t  AD_Type;
   Byte_t   AD_Data_Length;
   Byte_t  *AD_Data_Buffer;
} GAP_LE_Advertising_Data_Entry_t;

#define GAP_LE_ADVERTISING_DATA_ENTRY_SIZE                        (sizeof(GAP_LE_Advertising_Data_Entry_t))

   /*! The following structure is a container structure that is used to
    * represent all the entries in an Advertising Data Structure.
    * This structure is used so that all fields are easy to
    * parse and access (i.e. there are no MACRO's required to access
    * variable length records).  The first member of this structure
    * specifies how many individual entries are contained in the
    * Extended Inquiry Response Data structure.  The second member is a
    * pointer to an array that contains each individual entry of the
    * Extended Inquiry Response Data structure (note that the number of
    * individual entries pointed to by this array will be specified by
    * the Number_Entries member (first member).
    */
typedef struct _tagGAP_LE_Advertising_Data_t
{
   unsigned int                     Number_Data_Entries;
   GAP_LE_Advertising_Data_Entry_t *Data_Entries;
} GAP_LE_Advertising_Data_t;

#define GAP_LE_ADVERTISING_DATA_SIZE                              (sizeof(GAP_LE_Advertising_Data_t))

   /*! The following structure is a container structure that is used to
    * represent all the entries in an LE Advertising Report Data Field
    * This structure is used so that all fields are easy to parse and
    * access (i.e. there are no MACRO's required to access variable
    * length records).  The Advertising_Data member contains the the
    * actual parsed data for the report (either a Scan Response or
    * Advertising report - specified by the
    * GAP_LE_Advertising_Report_Type member).
    * \note The Raw_Report_Length and Raw_Report_Data members contain
    *          the length (in bytes) of the actual report received, as
    *          well as a pointer to the actual bytes present in the
    *          report, respectively.
    */
typedef struct _tagGAP_LE_Advertising_Report_Data_t
{
   GAP_LE_Advertising_Report_Type_t  Advertising_Report_Type;
   GAP_LE_Address_Type_t             Address_Type;
   BD_ADDR_t                         BD_ADDR;
   SByte_t                           RSSI;
   GAP_LE_Advertising_Data_t         Advertising_Data;
   Byte_t                            Raw_Report_Length;
   Byte_t                           *Raw_Report_Data;
} GAP_LE_Advertising_Report_Data_t;

#define GAP_LE_ADVERTISING_REPORT_DATA_SIZE                       (sizeof(GAP_LE_Advertising_Report_Data_t))

   /*! The following type declaration defines the structure of the data
    * returned in a GAP etLE_Advertising_Report event. The structure
    * is used to return Advertising Reports returned from multiple
    * devices.
    */
typedef struct _tagGAP_LE_Advertising_Report_Event_Data_t
{
   unsigned int                       Number_Device_Entries;
   GAP_LE_Advertising_Report_Data_t  *Advertising_Data;
} GAP_LE_Advertising_Report_Event_Data_t;

#define GAP_LE_ADVERTISING_REPORT_EVENT_DATA_SIZE                 (sizeof(GAP_LE_Advertising_Report_Event_Data_t))

   /*! The following type declaration defines the structure of the data
    * returned in a GAP etLE_Connection_Complete event.
    */
typedef struct _tagGAP_LE_Connection_Complete_Event_Data_t
{
   Byte_t                                 Status;
   Boolean_t                              Master;
   GAP_LE_Address_Type_t                  Peer_Address_Type;
   BD_ADDR_t                              Peer_Address;
   GAP_LE_Current_Connection_Parameters_t Current_Connection_Parameters;
} GAP_LE_Connection_Complete_Event_Data_t;

#define GAP_LE_CONNECTION_COMPLETE_EVENT_DATA_SIZE                (sizeof(GAP_LE_Connection_Complete_Event_Data_t))

   /*! The following type declaration defines the structure of the data
    * returned in a GAP etLE_Disconnection_Complete event.
    */
typedef struct _tagGAP_LE_Disconnection_Complete_Event_Data_t
{
   Byte_t                Status;
   Byte_t                Reason;
   GAP_LE_Address_Type_t Peer_Address_Type;
   BD_ADDR_t             Peer_Address;
} GAP_LE_Disconnection_Complete_Event_Data_t;

#define GAP_LE_DISCONNECTION_COMPLETE_EVENT_DATA_SIZE             (sizeof(GAP_LE_Disconnection_Complete_Event_Data_t))

   /*! The following type declaration defines the structure of the data
    * returned in a GAP etLE_Encryption_Change event.
    */
typedef struct _tagGAP_LE_Encryption_Change_Event_Data_t
{
   BD_ADDR_t             BD_ADDR;
   Byte_t                Encryption_Change_Status;
   GAP_Encryption_Mode_t Encryption_Mode;
   Boolean_t             SC;  	 /*!< This flag is used in order to let the upper layer know if we are in SC or not */
   Long_Term_Key_t 		 LTK;    /*!< In case of LE SC the upper layer should save the LTK at this stage */
   Boolean_t             Reestablish; /*!< This flag is set to TRUE, if the encryption was re-established */
   Byte_t				 Encryption_Key_Size; /*!< The LTK size. */
} GAP_LE_Encryption_Change_Event_Data_t;

#define GAP_LE_ENCRYPTION_CHANGE_EVENT_DATA_SIZE                  (sizeof(GAP_LE_Encryption_Change_Event_Data_t))

   /*! The following type declaration defines GAP Encryption Refresh
    * Information that is used with the GAP LE Encryption Refresh Result
    * Event.
    */
typedef struct _tagGAP_LE_Encryption_Refresh_Complete_Event_Data_t
{
   BD_ADDR_t BD_ADDR;
   Byte_t    Status;
} GAP_LE_Encryption_Refresh_Complete_Event_Data_t;

#define GAP_LE_ENCRYPTION_REFRESH_COMPLETE_EVENT_DATA_SIZE        (sizeof(GAP_LE_Encryption_Refresh_Complete_Event_Data_t))

   /*! The following enumerated type represents the different
    * Authentication Requests that can be specified.
    */
typedef enum
{
   latLongTermKeyRequest,
   latSecurityRequest,
   latPairingRequest,
   latKeypressNotification,
   latConfirmationRequest,
   latPairingStatus,
   latEncryptionInformationRequest,
   latIdentityInformationRequest,
   latSigningInformationRequest,
   latEncryptionInformation,
   latIdentityInformation,
   latSigningInformation,
   latSecurityEstablishmentComplete
} GAP_LE_Authentication_Event_Type_t;

   /*! The following type declaration specifies the Link Key information
    * included in a GAP_LE_Authentication_Event_Data structure when the
    * GAP_LE_Authentication_Event_Type = atKeyRequest.
    */
typedef struct _tagGAP_LE_Key_Request_Info_t
{
   Random_Number_t Rand;
   Word_t          EDIV;
} GAP_LE_Key_Request_Info_t;

#define GAP_LE_KEY_REQUEST_INFO_DATA_SIZE                         (sizeof(GAP_LE_Link_Key_Request_Info_t))

   /*! The following structure represents the data returned in a
    * latSecurityRequest etLE_Authentication event.
    */
typedef struct _tagGAP_LE_Security_Request_t
{
   GAP_LE_Bonding_Type_t Bonding_Type;
   Boolean_t             MITM;
   Boolean_t             SC;
   Boolean_t             Keypress;
} GAP_LE_Security_Request_t;

#define GAP_LE_SECURITY_REQUEST_DATA_SIZE                         (sizeof(GAP_LE_Security_Request_t))

   /*! The following define the different types of confirmation requests
    * that may be generated.
    * \note The crtNone type means that "Just Works" pairing
    *          method is being used.
    */
typedef enum
{
   crtNone,
   crtNumeric,	
   crtPasskey,
   crtDisplay,
   crtOOB
} GAP_LE_Confirmation_Request_Type_t;

   /*! The following represents the data returned in a
    * latConfirmationRequest etLE_Authentication event.
    * \note The DisplayPasskey member is only valid when RequestType
    *          is crtDisplay, and this member should be display to the
    *          user to allow the remote device to enter it.
    * \note The DisplayPasskey member is between 000,000 and 999,999
    *          and all digits MUST BE DISPLAYED, including leading
    *          ZEROS.
    */
typedef struct _tagGAP_LE_Confirmation_Request_t
{
   GAP_LE_Confirmation_Request_Type_t Request_Type;
   DWord_t                            Display_Passkey;
   DWord_t							  Display_Numeric;
   Boolean_t			  			  SC;
   GAP_LE_Bonding_Type_t     		  Remote_Bonding_Type;
   Byte_t                             Negotiated_Encryption_Key_Size;
} GAP_LE_Confirmation_Request_t;

   /*! The following type declaration represents the information that is
    * returned in a latPairingStatus etLE_Authentication event.
    */
typedef struct _tagGAP_LE_Pairing_Status_t
{
   Boolean_t Authenticated_Encryption_Key;
   Byte_t    Status;
   Byte_t    Negotiated_Encryption_Key_Size;
} GAP_LE_Pairing_Status_t;

#define GAP_LE_PAIRING_STATUS_DATA_SIZE                           (sizeof(GAP_LE_Pairing_Status_t))

   /*! The following define the status codes that may be returned in a
    * Pairing Status Authentication Event.
    */
#define GAP_LE_PAIRING_STATUS_NO_ERROR                                  0x00
#define GAP_LE_PAIRING_STATUS_DISCONNECTED                              0x01
#define GAP_LE_PAIRING_STATUS_LOCAL_RESOURCES                           0x02
#define GAP_LE_PAIRING_STATUS_PROTOCOL_TIMEOUT                          0x03
#define GAP_LE_PAIRING_STATUS_PASSKEY_ENTRY_FAILED                      0x04
#define GAP_LE_PAIRING_STATUS_OOB_NOT_AVAILABLE                         0x05
#define GAP_LE_PAIRING_STATUS_AUTHENTICATION_REQUIREMENTS               0x06
#define GAP_LE_PAIRING_STATUS_CONFIRM_VALUE_FAILED                      0x07
#define GAP_LE_PAIRING_STATUS_PAIRING_NOT_SUPPORTED                     0x08
#define GAP_LE_PAIRING_STATUS_ENCRYPTION_KEY_SIZE                       0x09
#define GAP_LE_PAIRING_STATUS_COMMAND_NOT_SUPPORTED                     0x0A
#define GAP_LE_PAIRING_STATUS_UNSPECIFIED_REASON                        0x0B
#define GAP_LE_PAIRING_STATUS_REPEATED_ATTEMPTS                         0x0C
#define GAP_LE_PAIRING_STATUS_INVALID_PARAMETERS                        0x0D
#define GAP_LE_PAIRING_STATUS_DHKEY_CHECK_FAILED                    	0x0E
#define GAP_LE_PAIRING_STATUS_NUMERIC_COMPARISON_FAILED      			0x0F

   /*! The following structure represents the structure of data that is
    * passed into a GAP_LE_Authentication_Event_Data_t structure with a
    * Response_Type of latEncryptionInformation.
    */
typedef struct _tagGAP_LE_Encryption_Request_Information_t
{
   Byte_t Encryption_Key_Size;
} GAP_LE_Encryption_Request_Information_t;

#define GAP_LE_ENCRYPTION_REQUEST_INFORMATION_DATA_SIZE           (sizeof(GAP_LE_Encryption_Request_Information_t))

   /*! The following structure represents the structure of data that is
    * returned into GAP_LE_Authentication_Event_Data_t structure with a
    * type of latSecurityEstablishmentComplete.
    */
typedef struct _tagGAP_LE_Security_Establishment_Complete_t
{
   Byte_t Status;
} GAP_LE_Security_Establishment_Complete_t;

#define GAP_LE_SECURITY_ESTABLISHMENT_COMPLETE_DATA_SIZE          (sizeof(GAP_LE_Security_Establishment_Complete_t))

   /*! The following constants represent the defined security status code
    * that are used with the Status member of the
    * GAP_LE_Security_Establishment_Complete_t event information.
    */
#define GAP_LE_SECURITY_ESTABLISHMENT_STATUS_CODE_NO_ERROR                 0x00
#define GAP_LE_SECURITY_ESTABLISHMENT_STATUS_CODE_LONG_TERM_KEY_ERROR      0x01
#define GAP_LE_SECURITY_ESTABLISHMENT_STATUS_CODE_EDIV_RAND_INVALID        0x02
#define GAP_LE_SECURITY_ESTABLISHMENT_STATUS_CODE_DEVICE_TRIED_TO_REPAIR   0x03
#define GAP_LE_SECURITY_ESTABLISHMENT_STATUS_CODE_LINK_DISCONNECTED        0x04
#define GAP_LE_SECURITY_ESTABLISHMENT_STATUS_CODE_TIMEOUT                  0x05

   /*! The following type declaration specifies the information that can
    * be returned in a GAP_Authentication_Callback.  This information
    * is passed to the Callback when a GAP_Authentication Callback is
    * issued.  The first member of this structure specifies which Data
    * Member is valid.  Currently the following members are valid for
    * the following values of the GAP_Authentication_Event_Type member:
    *
    *  latLongTermKeyRequest            - Long_Term_Key_Request is
    *                                     valid.
    *  latSecurityRequest               - Security_Request is valid.
    *  latPairingRequest                - Pairing_Request is valid.
    *  latConfirmationRequest           - Confirmation_Request is valid.
    *  latPairingStatus                 - Pairing_Status is valid.
    *  latEncryptionInformationRequest  - Encryption_Request_Information
    *                                     is valid.
    *  latIdentityInformationRequest    - No further data.
    *  latSigningInformationRequest     - No further data.
    *  latEncryptionInformation         - Encryption_Information valid.
    *  latIdentityInformation           - Identity_Information is valid.
    *  latSigningInformation            - Signing_Information is valid.
    *  latSecurityEstablishmentComplete - Security_Establishment_...
    *                                        ...Complete is valid.
    */
typedef struct _tagGAP_LE_Authentication_Event_Data_t
{
   GAP_LE_Authentication_Event_Type_t GAP_LE_Authentication_Event_Type;
   BD_ADDR_t                          BD_ADDR;
   union
   {
      GAP_LE_Key_Request_Info_t                Long_Term_Key_Request;
      GAP_LE_Pairing_Capabilities_t            Pairing_Request;
      GAP_LE_Security_Request_t                Security_Request;
      GAP_LE_Confirmation_Request_t            Confirmation_Request;
      GAP_LE_Pairing_Status_t                  Pairing_Status;
      GAP_LE_Encryption_Request_Information_t  Encryption_Request_Information;
      GAP_LE_Encryption_Information_t          Encryption_Information;
      GAP_LE_Identity_Information_t            Identity_Information;
      GAP_LE_Signing_Information_t             Signing_Information;
      GAP_LE_Security_Establishment_Complete_t Security_Establishment_Complete;
      GAP_LE_Keypress_t                        Keypress_Type;
   } Authentication_Event_Data;
} GAP_LE_Authentication_Event_Data_t;

#define GAP_LE_AUTHENTICATION_EVENT_DATA_SIZE                     (sizeof(GAP_LE_Authentication_Event_Data_t))

   /*! The following type declaration specifies the information that is
    * returned in a connection parameter update indication event that is
    * sent by slave to indicate that it wants to have it's connection
    * parameters updated.
    * \note All members of this structure are specified in
    *          milli-seconds EXCEPT Slave_Latency which is specified in
    *          number of connection events.
    */
typedef struct _tagGAP_LE_Connection_Parameter_Update_Request_Event_Data_t
{
   BD_ADDR_t BD_ADDR;
   Word_t    Conn_Interval_Min;
   Word_t    Conn_Interval_Max;
   Word_t    Slave_Latency;
   Word_t    Conn_Supervision_Timeout;
} GAP_LE_Connection_Parameter_Update_Request_Event_Data_t;

#define GAP_LE_CONNECTION_PARAMETER_UPDATE_REQUEST_EVENT_DATA_SIZE   (sizeof(GAP_LE_Connection_Parameter_Update_Request_Event_Data_t))

   /*! The following type declaration specifies the information that is
    * returned in a connection parameter update confirmation event that
    * is sent by master to the slave to indicate that it has accepted
    * or rejected a connection parameter update request.
    */
typedef struct _tagGAP_LE_Connection_Parameter_Update_Response_Event_Data_t
{
   BD_ADDR_t BD_ADDR;
   Boolean_t Accepted;
} GAP_LE_Connection_Parameter_Update_Response_Event_Data_t;

#define GAP_LE_CONNECTION_PARAMETER_UPDATE_RESPONSE_EVENT_DATA_SIZE (sizeof(GAP_LE_Connection_Parameter_Update_Response_Event_Data_t))

   /*! The following type declaration specifies the information that is
    * returned in a connection parameter update event that is sent by
    * the controller whenever the connection parameters for a connection
    * change.
    */
typedef struct _tagGAP_LE_Connection_Parameter_Updated_Event_Data_t
{
   Byte_t                                 Status;
   BD_ADDR_t                              BD_ADDR;
   GAP_LE_Current_Connection_Parameters_t Current_Connection_Parameters;
} GAP_LE_Connection_Parameter_Updated_Event_Data_t;

#define GAP_LE_CONNECTION_PARAMETER_UPDATED_EVENT_DATA_SIZE (sizeof(GAP_LE_Connection_Parameter_Updated_Event_Data_t))

   /*! The following type declaration specifies the information that is
    * returned in a LE Authenticated Payload Timeout expired event that
    * is sent by the controller whenever a packet from the remote device
    * with a valid MIC (for encrypted connections) is not received
    * within the configurable timeout period.
    */
typedef struct _tagGAP_LE_Authenticated_Payload_Timeout_Expired_Event_Data_t
{
   BD_ADDR_t BD_ADDR;
} GAP_LE_Authenticated_Payload_Timeout_Expired_Event_Data_t;

#define GAP_LE_AUTHENTICATED_PAYLOAD_TIMEOUT_EXPIRED_EVENT_DATA_SIZE (sizeof(GAP_LE_Authenticated_Payload_Timeout_Expired_Event_Data_t))

   /*! The following structure represents the container structure that
    * holds all GAP LE Event Data Data.
    */
typedef struct _tagGAP_LE_Event_Data_t
{
   GAP_LE_Event_Type_t Event_Data_Type;
   Word_t              Event_Data_Size;
   union
   {
      GAP_LE_Remote_Features_Event_Data_t                       *GAP_LE_Remote_Features_Event_Data;
      GAP_LE_Advertising_Report_Event_Data_t                    *GAP_LE_Advertising_Report_Event_Data;
      GAP_LE_Connection_Complete_Event_Data_t                   *GAP_LE_Connection_Complete_Event_Data;
      GAP_LE_Disconnection_Complete_Event_Data_t                *GAP_LE_Disconnection_Complete_Event_Data;
      GAP_LE_Encryption_Change_Event_Data_t                     *GAP_LE_Encryption_Change_Event_Data;
      GAP_LE_Encryption_Refresh_Complete_Event_Data_t           *GAP_LE_Encryption_Refresh_Complete_Event_Data;
      GAP_LE_Authentication_Event_Data_t                        *GAP_LE_Authentication_Event_Data;
      GAP_LE_Connection_Parameter_Update_Request_Event_Data_t   *GAP_LE_Connection_Parameter_Update_Request_Event_Data;
      GAP_LE_Connection_Parameter_Update_Response_Event_Data_t  *GAP_LE_Connection_Parameter_Update_Response_Event_Data;
      GAP_LE_Connection_Parameter_Updated_Event_Data_t          *GAP_LE_Connection_Parameter_Updated_Event_Data;
      GAP_LE_Authenticated_Payload_Timeout_Expired_Event_Data_t *GAP_LE_Authenticated_Payload_Timeout_Expired_Event_Data;
   } Event_Data;
} GAP_LE_Event_Data_t;

#define GAP_LE_EVENT_DATA_SIZE                                    (sizeof(GAP_LE_Event_Data_t))

   /*! The following declared type represents the Prototype Function for
    * the GAP Event Receive Data Callback. This function will be called
    * whenever a Callback has been registered for the specified GAP
    * Action that is associated with the specified Bluetooth stack ID.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize
    * @param GAP_LE_Event_Data Pointer to the passed event data.
    * @param CallbackParameter User-defined parameter
    * (e.g., tag value) that was defined in the callback registration.
    * The caller is free to use the contents of the GAP Event Data ONLY
    * in the context of this callback.  If the caller requires the Data
    * for a longer period of time, then the callback function MUST copy
    * the data into another Data Buffer.  This function is guaranteed
    * NOT to be invoked more than once simultaneously for the specified
    * installed callback (i.e. this  function DOES NOT have be
    * reentrant).  It Needs to be noted however, that if the same
    * Callback is installed more than once, then the callbacks will be
    * called serially.  Because of this, the processing in this function
    * should be as efficient as possible.  It should also be noted that
    * this function is called in the Thread Context of a Thread that
    * the User does NOT own.  Therefore, processing in this function
    * should be as efficient as possible (this argument holds anyway
    * because other GAP Events will not be processed while this
    * function call is outstanding).
    * \note This function MUST NOT Block and wait for events that
    *            can only be satisfied by Receiving other GAP Events.
    *            A Deadlock WILL occur because NO GAP Event
    *            Callbacks will be issued while this function is
    *            currently outstanding.
    */
typedef void (BTPSAPI *GAP_LE_Event_Callback_t)(unsigned int BluetoothStackID, GAP_LE_Event_Data_t *GAP_LE_Event_Data, unsigned long CallbackParameter);

   /*! GAP Basic Rate/Enhanced Data Rate (BR/EDR) API Prototypes. */
   /*! @brief The following function is provided to set the Discoverability Mode
    * of the Local Bluetooth Device specified by the Bluetooth Protocol
    * Stack that is specified by the Bluetooth Protocol Stack ID.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param GAP_Discoverability_Mode Value that defines the limits to being
    * discovered by other Bluetooth devices.
    * @param Max_Discoverable_Time Length of time in seconds that the unit
    * will be in the specified discoverable mode (not applicable for non discoverable mode).
    * @return This function returns zero if the Discoverability Mode was able to be
    * successfully changed, otherwise this function returns a negative
    * value which signifies an error condition.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Set_Discoverability_Mode(unsigned int BluetoothStackID, GAP_Discoverability_Mode_t GAP_Discoverability_Mode, unsigned long Max_Discoverable_Time);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Set_Discoverability_Mode_t)(unsigned int BluetoothStackID, GAP_Discoverability_Mode_t GAP_Discoverability_Mode, unsigned long Max_Discoverable_Time);
#endif

   /*! @brief The following function is provided to allow a means to query the
    * Current Discoverability Mode Parameters for the Bluetooth Device
    * that is specified by the Bluetooth Protocol Stack that is
    * associated with the specified Bluetooth Stack ID.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param GAP_Discoverability_Mode Pointer to a buffer to receive the
    * value that defines the current mode of discovery.
    * @param Max_Discoverable_Time Pointer to a buffer to receive the
    * length of time (in seconds) that the unit was specified to be the discoverable mode.
    * @return If this function returns success, then the GAP Discoverability
    * Mode will be valid and the Maximum Discoverable Time will be
    * valid as well.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Query_Discoverability_Mode(unsigned int BluetoothStackID, GAP_Discoverability_Mode_t *GAP_Discoverability_Mode, unsigned long *Max_Discoverable_Time);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Query_Discoverability_Mode_t)(unsigned int BluetoothStackID, GAP_Discoverability_Mode_t *GAP_Discoverability_Mode, unsigned long *Max_Discoverable_Time);
#endif

   /*! @brief The following function is provided to set the Connectability Mode
    * of the Local Bluetooth Device specified by the Bluetooth Protocol
    * Stack that is specified by the Bluetooth Protocol Stack ID.
    * BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack
    * via a call to BSC_Initialize.
    * GAP_Connectability_Mode Value that defines the connectability mode
    * (from other Bluetooth devices).
    * @return This function returns zero if the
    * Connectability Mode was able to be successfully changed, otherwise
    * This function returns a negative value which signifies an error
    * condition.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Set_Connectability_Mode(unsigned int BluetoothStackID, GAP_Connectability_Mode_t GAP_Connectability_Mode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Set_Connectability_Mode_t)(unsigned int BluetoothStackID, GAP_Connectability_Mode_t GAP_Connectability_Mode);
#endif

   /*! @brief The following function is provided to allow a means to query the
    * Current Connectability Mode Setting for the Bluetooth Device that
    * is specified by the Bluetooth Protocol Stack that is associated
    * with the specified Bluetooth Stack ID.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param GAP_Connectability_Mode Pointer to a buffer to receive the value
    * that defines the current mode of connectability.
    * @return This function will return zero on success, or a negative return error code if
    * there was an error. If this function returns success, then the
    * GAP Connectability Mode will contain the current Connectability
    * Mode.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Query_Connectability_Mode(unsigned int BluetoothStackID, GAP_Connectability_Mode_t *GAP_Connectability_Mode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Query_Connectability_Mode_t)(unsigned int BluetoothStackID, GAP_Connectability_Mode_t *GAP_Connectability_Mode);
#endif

   /*! @brief The following function is provided to set the Pairability Mode of
    * the Local Bluetooth Device specified by the Bluetooth Protocol
    * Stack that is specified by the Bluetooth Protocol Stack ID.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param GAP_Pairability_Mode Value that defines the pairability mode
    * (to other Bluetooth devices).
    * @return This function returns zero if the
    * Pairability Mode was able to be successfully changed, otherwise
    * This function returns a negative value which signifies an error
    * condition.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Set_Pairability_Mode(unsigned int BluetoothStackID, GAP_Pairability_Mode_t GAP_Pairability_Mode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Set_Pairability_Mode_t)(unsigned int BluetoothStackID, GAP_Pairability_Mode_t GAP_Pairability_Mode);
#endif

   /*! @brief The following function is provided to allow a means to query the
    * Current Pairability Mode Settings for the Bluetooth Device that
    * is specified by the Bluetooth Protocol Stack that is associated
    * with the specified Bluetooth Stack ID.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param GAP_Pairability_Mode Pointer to a buffer to receive the value
    * that defines the current mode of pairability.
    * @return  This function will return
    * zero on success, or a negative return error code if there was an
    * error. If this function returns success, then the GAP Pairability
    * Mode will contain the current Pairability Mode.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Query_Pairability_Mode(unsigned int BluetoothStackID, GAP_Pairability_Mode_t *GAP_Pairability_Mode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Query_Pairability_Mode_t)(unsigned int BluetoothStackID, GAP_Pairability_Mode_t *GAP_Pairability_Mode);
#endif

   /*! @brief The following function is provided to set the Authentication Mode
    * of the Local Bluetooth Device specified by the Bluetooth Protocol
    * Stack that is specified by the Bluetooth Protocol Stack ID.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param GAP_Authentication_Mode Value that defines the authentication
    * mode to set on the local device.
    * @return This function returns zero if the
    * Authentication Mode was able to be successfully changed, otherwise
    * This function returns a negative value which signifies an error
    * condition.
    * \note If Authentication is enabled for the Local Bluetooth
    *          Device, then this Means that EVERY Connection (both
    *          incoming and outgoing) will require Authentication at
    *          the Link Level.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Set_Authentication_Mode(unsigned int BluetoothStackID, GAP_Authentication_Mode_t GAP_Authentication_Mode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Set_Authentication_Mode_t)(unsigned int BluetoothStackID, GAP_Authentication_Mode_t GAP_Authentication_Mode);
#endif

   /*! @brief The following function is provided to allow a means to query the
    * Current Authentication Mode Settings for the Bluetooth Device
    * that is specified by the Bluetooth Protocol Stack that is
    * associated with the specified Bluetooth Stack ID.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param GAP_Authentication_Mode Pointer to an area to receive the value that
    * defines the current mode of Authentication.
    * @return This function will return zero on success, or a negative return error code if
    * there was an error. If this function returns success, then the
    * GAP Authentication Mode will be contain the current Authentication
    * Mode Settings.
    * \note If Authentication is enabled for the Local Bluetooth
    *          Device, then this Means that EVERY Connection (both
    *          incoming and outgoing) will require Authentication at
    *          the Link Level.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Query_Authentication_Mode(unsigned int BluetoothStackID, GAP_Authentication_Mode_t *GAP_Authentication_Mode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Query_Authentication_Mode_t)(unsigned int BluetoothStackID, GAP_Authentication_Mode_t *GAP_Authentication_Mode);
#endif

   /*! @brief The following function is provided to allow the Setting of
    * Encryption Modes for the Local Bluetooth Device or to
    * Enable/Disable Encryption for the specified Bluetooth Board
    * Address.
    * Stack of the Local Bluetooth Device.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR The second parameter is the Bluetooth address.
    * @param GAP_Encryption_Mode Value that defines the Encryption mode
    * of the Bluetooth device.
    * @param GAP_Event_Callback Callback function that will be used to
    * dispatch result information to the upper layers.
    * @param CallbackParameter User defined value to be used by the GAP
    * layer as an input parameter for the callbacks function.
    * @return This callback will contain the
    * actual status of the Encryption Change (success or failure).
    * This function returns zero if the Encryption Mode was changed
    * (see note below), or a negative return value which signifies an
    * error condition.
    * \note If the Local Board Address is specified for the second
    *          parameter, then this function will set the specified
    *          Encryption Mode for ALL further Link Level Connections.
    *          When the Local Board Address is specified, the Callback
    *          Function and Parameter are ignored, and the function
    *          return value indicates whether or not the Encryption
    *          Change was successful (for the Local Device).
    * \note If the second parameter is NOT the Local Board Address,
    *          then this function will set the Encryption Mode on the
    *          Link Level for the specified Bluetooth Link.  A Physical
    *          ACL Link MUST already exist for this to work.  The
    *          actual status of the Encryption Change for this link
    *          will be passed to the Callback Information that is
    *          required when using this function in this capacity.
    * \note Because this function is asynchronous in nature (when
    *          specifying a remote BD_ADDR), this function will notify
    *          the caller of the result via the installed Callback.  The
    *          caller is free to cancel the Encryption Mode Change at
    *          any time by issuing the GAP_Cancel_Set_Encryption_Mode()
    *          function and specifying the BD_ADDR of the Bluetooth
    *          Device that was specified in this call.  It should be
    *          noted that when the Callback is cancelled, the Callback
    *          is the ONLY thing that is cancelled (i.e. the GAP module
    *          still changes the Encryption for the Link, it's just that
    *          NO Callback is issued).
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Set_Encryption_Mode(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_Encryption_Mode_t GAP_Encryption_Mode, GAP_Event_Callback_t GAP_Event_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Set_Encryption_Mode_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_Encryption_Mode_t GAP_Encryption_Mode, GAP_Event_Callback_t GAP_Event_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to cancel the future calling
    * of an Encryption Mode Callback that was installed via a
    * successful call to the GAP_Set_Encryption_Mode() function.  This
    * function DOES NOT cancel the changing of the Encryption Mode for
    * the specified Bluetooth Device, it ONLY cancels the Callback
    * Notification.  This function accepts as input the Bluetooth
    * Protocol Stack ID of the Bluetooth Device used when the
    * GAP_Set_Encryption_Mode() function was previously issued, and the
    * Board Address of the Bluetooth Device that the previous call was
    * called with.  The BD_ADDR parameter MUST be valid, and cannot be
    * the BD_ADDR of the Local Bluetooth Device because the Local
    * Encryption Mode Change does not use the Callback mechanism.  @return This function returns zero if successful, or a negative return error
    * code if the function was unsuccessful.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Cancel_Set_Encryption_Mode(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Cancel_Set_Encryption_Mode_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR);
#endif

   /*! @brief The following function is provided to allow a means to query the
    * Current Encryption Mode Parameters for the Bluetooth Device that
    * is specified by the Bluetooth Protocol Stack that is associated
    * with the specified Bluetooth Stack ID.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Device address of the Bluetooth device to which the
    * encryption is to be retreived.
    * @param GAP_Encryption_Mode Pointer to a buffer to receive the current
    * encryption mode setting.
    * @return If this function returns success, then the GAP
    * Encryption Mode parameter will contain the current Encryption
    * Mode Setting for the specified Bluetooth Device/Bluetooth Link.
    * \note If the Local Board Address is specified for the second
    *          parameter, then this function will query the specified
    *          Encryption Mode for ALL future Link Level Connections.
    * \note If the second parameter is NOT the Local Board Address,
    *          then this function will query the Encryption Mode on the
    *          Link Level for the specified Bluetooth Link.  A physical
    *          ACL Link MUST already exist for this to work.
    * \note If the local Bluetooth Radio version is 2.1, then this
    *          function will return an error since this function has
    *          been deprecated for Bluetooth 2.1 and beyond. The HCI
    *          will actually return an error if you were to invoke this
    *          command, but that could add an unnecessary delay that
    *          be prevented by querying the current version in memory.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Query_Encryption_Mode(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_Encryption_Mode_t *GAP_Encryption_Mode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Query_Encryption_Mode_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_Encryption_Mode_t *GAP_Encryption_Mode);
#endif

   /*! @brief The following function is provided to allow a means to
    * Authenticate a Remote Device.  This function accepts as input the
    * Bluetooth Protocol Stack ID of the Local Bluetooth Device, the
    * Bluetooth Board Address of the Remote Device to Authenticate, and
    * the GAP Event Callback (and Callback Parameter) information that
    * is to be used during the Authentication Process to inform the
    * caller of Authentication Events and/or Requests.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Device address of the Bluetooth device to
    * authenticate.
    * @param GAP_Event_Callback Pointer to a callback function
    * to be used by the GAP layer to dispatch GAP event information for this request.
    * @param CallbackParameter User defined value to be used by
    * the GAP layer as an input parameter for the callback.
    * @return This function returns zero if successful or a negative return error code if
    * there was an error.  Note that even if this function returns
    * success, it does NOT mean that the specified Remote Device was
    * successfully Authenticated, it only means that the Authentication
    * Process has been started.
    * \note Because this function is asynchronous in nature, this
    *          function will notify the caller of the result via the
    *          installed Callback.  The caller is free to cancel the
    *          Authentication Process at any time by calling the
    *          GAP_Cancel_Authenticate_Remote_Device() function and
    *          specifying the BD_ADDR of the Bluetooth Device that was
    *          specified in this call.  It should be noted that when
    *          the Callback is cancelled, the Callback is the ONLY
    *          thing that is cancelled (i.e. the GAP module still
    *          processes the Authentication Events only NO Callback(s)
    *          are issued).
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Authenticate_Remote_Device(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_Event_Callback_t GAP_Event_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Authenticate_Remote_Device_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_Event_Callback_t GAP_Event_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to allow a mechanism for
    * Cancelling a Remote Device Authentication Process that was
    * successfully initiated via a successful call to the
    * GAP_Authenticate_Remote_Device() function.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Device address of the Bluetooth device on which to
    * cancel the authentication.
    * @return This function returns
    * zero if the Callback was successfully removed, or a negative
    * return error code if there was an error.
    * \note Calling this function does NOT terminate the Remote
    *          Authentication Process !!!!!  It only suspends further
    *          Authentication Events for the GAP Event Callback that
    *          was specified in the call of the original
    *          GAP_Authenticate_Remote_Device() function.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Cancel_Authenticate_Remote_Device(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Cancel_Authenticate_Remote_Device_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR);
#endif

   /*! @brief The following function is provided to allow a means to Register
    * a GAP Event Callback to accept Remote Authentication Requests.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param GAP_Event_Callback Pointer to a callback function to be used
    * by the GAP layer to dispatch GAP event information for this request.
    * @param CallbackParameter User defined value to be used by the GAP
    * layer as an input parameter for the specified callback.
    * @return This function returns zero if the Callback was
    * successfully installed, or a negative error return code if the
    * Callback was not successfully installed.  It should be noted that
    * ONLY ONE Remote Authentication Callback can be installed per
    * Bluetooth Device.  The Caller can Un-Register the Remote
    * Authentication Callback that was registered with this function
    * (if successful) by calling the
    * GAP_Un_Register_Remote_Authentication() function.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Register_Remote_Authentication(unsigned int BluetoothStackID, GAP_Event_Callback_t GAP_Event_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Register_Remote_Authentication_t)(unsigned int BluetoothStackID, GAP_Event_Callback_t GAP_Event_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to allow a mechanism to
    * Un-Register a previously registered GAP Event Callback for
    * Remote Authentication Events.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @return This function returns zero if successful, or a
    * negative return error code if unsuccessful.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Un_Register_Remote_Authentication(unsigned int BluetoothStackID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Un_Register_Remote_Authentication_t)(unsigned int BluetoothStackID);
#endif

   /*! @brief The following function is provided to allow a mechanism for the
    * Local Device to Respond to GAP Authentication Events.  This
    * function is used to set the Authentication Information for the
    * specified Bluetooth Device.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Device address of the Bluetooth device that is being
    * authenticated.
    * @param GAP_Authentication_Information  Pointer to a structure that
    * holds authentication information.
    * @return This function returns zero if successful, or a negative return
    * error code if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Authentication_Response(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_Authentication_Information_t *GAP_Authentication_Information);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Authentication_Response_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_Authentication_Information_t *GAP_Authentication_Information);
#endif

   /*! @brief The following function is provided to allow a mechanism for
    * Starting an Inquiry Scan Procedure.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param GAP_Inquiry_Type Type of inquiry scan to Perform.
    * @param MinimumPeriodLength Mininum length of time to perform the inquiry procedure (in seconds).
    * This parameter is only applicable if a periodic inquiry scan is required.
    * @param MaximumPeriodLength Maxinum length of time to perform the inquiry procedure (in seconds).
    * This parameter is only applicable if a periodic inquiry scan is required.
    * @param InquiryLength Length of time to perform the inquiry procedure (in seconds).
    * @param MaximumResponses Maximum number of responses to be received before the process is terminated.
    * @param GAP_Event_Callback Pointer to a callback function that is used by the GAP layer to dispatch
    * result information about the inquiry process.
    * @param CallbackParameter User defined value to be used by the GAP layer as an input parameter for
    * the specified callback.
    * @return This function returns zero if successful, or a negative return error code if
    * an Inquiry was unable to be performed.
    * \note Only ONE Inquiry can be performed at any given time.
    *          Calling this function while an outstanding Inquiry is
    *          in progress will fail.  The caller can call the
    *          GAP_Cancel_Inquiry() function to cancel a currently
    *          executing Inquiry procedure.
    * \note The Minimum and Maximum Inquiry Parameters are optional
    *          and, if specified, represent the Minimum and Maximum
    *          Periodic Inquiry Periods.  The caller should set BOTH
    *          of these values to zero if a simple Inquiry procedure
    *          is to be used (Non-Periodic).  If these two parameters
    *          are specified, then then these two parameters must
    *          satisfy the following formula:
    *
    *          MaximumPeriodLength > MinimumPeriodLength > InquiryLength
    *
    * \note All Inquiry Period Time parameters are specified in
    *          seconds.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Perform_Inquiry(unsigned int BluetoothStackID, GAP_Inquiry_Type_t GAP_Inquiry_Type, unsigned long MinimumPeriodLength, unsigned long MaximumPeriodLength, unsigned int InquiryLength, unsigned int MaximumResponses, GAP_Event_Callback_t GAP_Event_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Perform_Inquiry_t)(unsigned int BluetoothStackID, GAP_Inquiry_Type_t GAP_Inquiry_Type, unsigned long MinimumPeriodLength, unsigned long MaximumPeriodLength, unsigned int InquiryLength, unsigned int MaximumResponses, GAP_Event_Callback_t GAP_Event_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to allow a means of cancelling
    * an Inquiry Process that was started via a successful call to the
    * GAP_Perform_Inquiry() function.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @return This function returns zero if the Inquiry Process was
    * able to be cancelled, or a negative return error code if there
    * was an error. If this function returns success then the GAP
    * Callback that was installed with the GAP_Perform_Inquiry()
    * function will NEVER be called.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Cancel_Inquiry(unsigned int BluetoothStackID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Cancel_Inquiry_t)(unsigned int BluetoothStackID);
#endif

   /*! @brief The following function is provided to set the Inquiry Mode of the
    * Local Bluetooth Device specified by the Bluetooth Protocol Stack
    * that is specified by the Bluetooth Protocol Stack ID.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param GAP_Inquiry_Mode Specifies the inquiry mode to use.
    * @return This function returns zero if the Inquiry Mode was
    * able to be successfully changed, otherwise this function returns a
    * negative value which signifies an error condition.
    * \note The Inquiry Mode dictates how the local device will
    *          actually perform inquiries (and how the results will be
    *          returned).  The following table shows supported modes and
    *          the corresponding Inquiry Result Event for that mode.
    *
    *             Mode,       Inquiry Result Event\n
    *             imStandard, etInquiry_Entry_Result\n
    *             imRSSI, etInquiry_With_RSSI_Entry_Result\n
    *             imExtended, etExtended_Inquiry_Entry_Result\n
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Set_Inquiry_Mode(unsigned int BluetoothStackID, GAP_Inquiry_Mode_t GAP_Inquiry_Mode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Set_Inquiry_Mode_t)(unsigned int BluetoothStackID, GAP_Inquiry_Mode_t GAP_Inquiry_Mode);
#endif

   /*! @brief The following function is provided to allow a means to query the
    * Current Inquiry Mode Settings for the Bluetooth Device that is
    * specified by the Bluetooth Protocol Stack that is associated with
    * the specified Bluetooth Stack ID.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param GAP_Inquiry_Mode Pointer to a buffer to receive the current
    * inquiry mode setting.
    * @return If this function returns success, then the GAP Inquiry Mode will contain
    * the current Pairability Mode.
    * \note The Inquiry Mode dictates how the local device will
    *          actually perform inquiries (and how the results will be
    *          returned).  The following table shows supported modes and
    *          the corresponding Inquiry Result Event for that mode.
    *
    *             Mode,       Inquiry Result Event\n
    *             imStandard, etInquiry_Entry_Result\n
    *             imRSSI, etInquiry_With_RSSI_Entry_Result\n
    *             imExtended, etExtended_Inquiry_Entry_Result\n
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Query_Inquiry_Mode(unsigned int BluetoothStackID, GAP_Inquiry_Mode_t *GAP_Inquiry_Mode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Query_Inquiry_Mode_t)(unsigned int BluetoothStackID, GAP_Inquiry_Mode_t *GAP_Inquiry_Mode);
#endif

   /*! @brief The following function is provided to allow a mechanism to Query
    * the User Friendly Bluetooth Device Name of the specified Bluetooth
    * Device.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Address of the Bluetooth device of which the name is
    * to be retrieved.
    * @param GAP_Event_Callback Pointer to a callback function to be used
    * by the GAP layer to dispatch GAP event information for this request.
    * @param CallbackParameter User defined value to be used by the GAP
    * layer as an input parameter for the specified callback.
    * @return This function returns zero if successful, or a negative return
    * error code if the Remote Name Request was unable to be submitted.
    * If this function returns success, then the caller will be notified via the
    * specified Callback when the specified information has been
    * determined (or if there was an error).
    * \note This function cannot be used to determine the User
    *          Friendly name of the Local Bluetooth Device.  The
    *          GAP_Query_Local_Device_Name() function should be used for
    *          this purpose.  This function will fail if the Local
    *          Device's Bluetooth Address is specified.
    * \note Because this function is asynchronous in nature
    *          (specifying a remote BD_ADDR), this function will notify
    *          the caller of the result via the installed Callback.  The
    *          caller is free to cancel the Remote Name Request at any
    *          time by issuing the GAP_Cancel_Query_Remote_Name()
    *          function and specifying the BD_ADDR of the Bluetooth
    *          Device that was specified in this call.  It should be
    *          noted that when the Callback is cancelled, the Callback
    *          is the ONLY thing that is cancelled (i.e.  the GAP module
    *          still performs the Remote Name Inquiry, it's just that NO
    *          Callback is issued).
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Query_Remote_Device_Name(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_Event_Callback_t GAP_Event_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Query_Remote_Device_Name_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_Event_Callback_t GAP_Event_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to cancel the future calling
    * of a Remote Name Result Event Callback that was installed via a
    * successful call to the GAP_Query_Remote_Device_Name() function.
    * This function DOES NOT cancel the Querying of the Remote Device's
    * Name, it ONLY cancels the Callback Notification.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Address of the Bluetooth device of which the remote
    * name is being retrieved (which should be cancelled).
    * @return This function returns zero if
    * successful, or a negative return error code if the function was
    * unsuccessful.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Cancel_Query_Remote_Device_Name(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Cancel_Query_Remote_Device_Name_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR);
#endif

   /*! @brief The following function is provided to allow a mechanism to Query
    * the features of the specified Bluetooth Device.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Bluetooth device address of the remote device
    * @param GAP_Event_Callback Pointer to a callback function to be used
    * by the GAP layer to dispatch GAP event information for this request.
    * @param CallbackParameter User defined value to be used by the GAP
    * layer as an input parameter for the specified callback.
    * @return This function returns zero if successful, or a negative return error code if the
    * Remote Name Request was unable to be submitted. If this function returns success, then the caller will be notified via the
    * specified callback when the requested information has been
    * determined (or if there was an error).
    * \note Because this function is asynchronous in nature , this
    *          function will notify the caller of the result via the
    *          installed Callback.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Query_Remote_Features(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_Event_Callback_t GAP_Event_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Query_Remote_Features_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_Event_Callback_t GAP_Event_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to allow a mechanism to Query
    * the Version information of the specified Bluetooth Device.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param BD_ADDR Bluetooth device address of the remote device
    * @param GAP_Event_Callback Pointer to a callback function to be used by the
    * GAP layer to dispatch GAP event information for this request.
    * @param CallbackParameter User defined value to be used by the GAP layer as
    * an input parameter for the specified callback.
    * @return This function returns zero if successful, or a negative return error
    * code if the Remote Version Request was unable to be submitted.  If
    * This function returns success, then the caller will be notified
    * via the specified callback when the requested information has been
    * determined (or if there was an error).
    * \note Because this function is asynchronous in nature , this
    *          function will notify the caller of the result via the
    *          installed Callback.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Query_Remote_Version_Information(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_Event_Callback_t GAP_Event_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Query_Remote_Version_Information_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_Event_Callback_t GAP_Event_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to allow a means to Initiate
    * a Bonding Procedure.  This function can perform both General
    * and Dedicated Bonding based upon the type of Bonding requested.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Address of the Bluetooth device of which to Bond.
    * @param GAP_Bonding_Type Type of Bonbding to perform
    * @param GAP_Event_Callback  Pointer to a callback function to be used
    * by the GAP layer to dispatch GAP Event information for this request.
    * @param CallbackParameter User defined value to be used by the GAP
    * layer as an input parameter for all callbacks.
    * @return This function returns zero if
    * successful, or a negative return error code if unsuccessful.  If
    * this function is successful then all further information will
    * be returned through the Registered GAP Event Callback.  It should
    * be noted that if this function returns success that it does NOT
    * mean that the Remote Device has successfully Bonded with the
    * Local Device, it ONLY means that the Remote Device Bonding Process
    * has been started.  This function will only succeed if a Physical
    * Connection to the specified Remote Bluetooth Device does NOT
    * already exist.  This function will connect to the Bluetooth
    * Device and begin the Bonding Process.  If General Bonding is
    * specified then the Link is maintained, and will NOT be
    * terminated until the GAP_End_Bonding() function has been called.
    * This will allow for any higher level initialization that is needed
    * on the same physical link.  If Dedicated Bonding is performed,
    * then the Link is terminated automatically when the Authentication
    * Process has completed.
    * \note Due to the asynchronous nature of this process, the GAP
    *          Event Callback that is specified will inform the caller
    *          of any Events and/or Data that is part of the
    *          Authentication Process.  The GAP_Cancel_Bonding()
    *          function can be called at any time to end the Bonding
    *          Process and terminate the link (regardless of which
    *          Bonding method is being performed).
    * \note When using General Bonding, if an L2CAP Connection is
    *          established over the Bluetooth Link that was initiated
    *          with this function, the Bluetooth Protocol Stack MAY or
    *          MAY NOT terminate the Physical Link when (and if) an
    *          L2CAP Disconnect Request (or Response) is issued.  If
    *          this occurs, then calling GAP_End_Bonding() will have
    *          no effect (the GAP_End_Bonding() function will return
    *          an error code in this case).
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Initiate_Bonding(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_Bonding_Type_t GAP_Bonding_Type, GAP_Event_Callback_t GAP_Event_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Initiate_Bonding_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_Bonding_Type_t GAP_Bonding_Type, GAP_Event_Callback_t GAP_Event_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to allow a means to Cancel a
    * Bonding process that was started previously via a successful call
    * to the GAP_Initiate_Bonding() function.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param BD_ADDR Address of the Bluetooth device of which to cancel Bonding.
    * @return This function returns zero if the
    * Bonding Procedure was successfully terminated, or a negative
    * return value if there was an error.  This function terminates the
    * Connection and NO further GAP Event Callbacks will be issued
    * after this function has completed (if successful).
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Cancel_Bonding(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Cancel_Bonding_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR);
#endif

   /*! @brief The following function is provided to allow a means to terminate
    * a Link that was established via a call to the
    * GAP_Initiate_Bonding() function (that specified General Bonding
    * as the Bonding Type to perform).  This function has NO effect if
    * the Bonding Procedure was initiated using Dedicated Bonding.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Address of the Bluetooth device of which to end bonding.
    * @return This function returns zero if successful, or a negative return
    * error code if there was an error.  This function terminates the
    * Connection that was established and it guarantees that NO GAP
    * Event Callbacks will be issued to the GAP Event Callback that was
    * specified in the original GAP_Initiate_Bonding() function call
    * if this function returns success).
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_End_Bonding(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_End_Bonding_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR);
#endif

   /*! @brief The following function is responsible for Querying (and reporting)
    * the Board Address of the Local Bluetooth Device that is specified
    * by the Bluetooth Protocol Stack specified by the Bluetooth Stack
    * ID.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Pointer to memory in which to receive the local device
    * address.
    * @return If this function is successful, this function returns zero, and
    * the buffer that BD_ADDR points to will be filled with the Board
    * Address read from the Local Device. If this function returns a
    * negative value, then the BD_ADDR of the Local Device was NOT able
    * to be queried (error condition).
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Query_Local_BD_ADDR(unsigned int BluetoothStackID, BD_ADDR_t *BD_ADDR);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Query_Local_BD_ADDR_t)(unsigned int BluetoothStackID, BD_ADDR_t *BD_ADDR);
#endif

   /*! @brief The following function is provided to allow the changing of the
    * Class of Device of the Local Device specified by the Bluetooth
    * Protocol Stack that is specified by the Bluetooth Stack ID
    * parameter.  The Class of Device Parameter represents the Class of
    * Device value that is to be written to the Local Device.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param Class_of_Device Structure that holds the class of device information.
    * @return This function will return zero if the Class of Device was successfully
    * changed, or a negative return error code if there was an error
    * condition.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Set_Class_Of_Device(unsigned int BluetoothStackID, Class_of_Device_t Class_of_Device);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Set_Class_Of_Device_t)(unsigned int BluetoothStackID, Class_of_Device_t Class_of_Device);
#endif

   /*! @brief The following function is responsible for Querying (and reporting)
    * the Class of Device of the Local Bluetooth Device that is
    * specified by the Bluetooth Protocol Stack specified by the
    * Bluetooth Stack ID.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param Class_of_Device Pointer to a structure to receive the class
    * of device information.
    *@return If this function is successful, this function returns zero,
    * and the buffer that Class_Of_Device points to will
    * be filled with the Class of Device read from the Local Device.
    * If this function returns a negative value, then the Class of
    * Device of the Local Device was NOT able to be queried (error
    * condition).
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Query_Class_Of_Device(unsigned int BluetoothStackID, Class_of_Device_t *Class_of_Device);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Query_Class_Of_Device_t)(unsigned int BluetoothStackID, Class_of_Device_t *Class_of_Device);
#endif

   /*! @brief The following function is provided to allow the changing of the
    * Device Name of the Local Bluetooth Device specified by the
    * Bluetooth Protocol Stack that is specified by the Bluetooth Stack
    * ID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param Name Pointer to a buffer to containing the local device name.
    * \note The Name parameter must be a pointer to a NULL
    * terminated ASCII String of at most MAX_NAME_LENGTH (not counting
    * the trailing NULL terminator).
    * This function will return zero if
    * the Name was successfully changed, or a negative return error code
    * if there was an error condition.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Set_Local_Device_Name(unsigned int BluetoothStackID, char *Name);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Set_Local_Device_Name_t)(unsigned int BluetoothStackID, char *Name);
#endif

   /*! @brief The following function is responsible for Querying (and reporting)
    * the User Friendly Name of the Local Bluetooth Device that is
    * specified by the Bluetooth Protocol Stack specified by the
    * Bluetooth Stack ID.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param NameBufferLength Size of vuffer to receive local device name.
    * @param NameBuffer Pointer to a buffer to receive the local device name.
    * @return If this function is successful, this function returns zero and
    * the buffer that NameBuffer points to will be
    * filled with a NULL terminated ASCII representation of the Local
    * Device Name. If this function returns a negative value, then the
    * Local Device Name was NOT able to be queried (error condition).
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Query_Local_Device_Name(unsigned int BluetoothStackID, unsigned int NameBufferLength, char *NameBuffer);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Query_Local_Device_Name_t)(unsigned int BluetoothStackID, unsigned int NameBufferLength, char *NameBuffer);
#endif

   /*! @brief The following function is provided to allow a means to disconnect
    * an established Connection Link (ACL Connection) from the Protocol
    * Stack.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Address of the Bluetooth device of which to terminate
    * the link.
    * @return This function returns
    * zero if successful, or a negative return error code if there was
    * an error disconnecting the specified link.
    * \note This function should be used sparingly as it will NOT
    *          send protocol specific disconnections (i.e. it will NOT
    *          send an RFCOMM Disconnect or a L2CAP Disconnect Request).
    *          This function is a very low-level function in that it
    *          simply kills the ACL Link that has been established with
    *          the specified Bluetooth Device.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Disconnect_Link(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Disconnect_Link_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR);
#endif

   /*! @brief The following function is provided to allow a means to query the
    * ACL Connection Handle of a connection to a remote Bluetooth
    * Device.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Address of the Bluetooth device of which to query the
    * connection handle.
    * @param Connection_Handle Pointer to a variable that will receive the
    * connection handle associated with the specified Bluetooth device address.
    * @return  This function will return zero
    * on success, or a negative return error code if there was an error.
    * If this function returns success, then the Connection Handle
    * variable will contain the current ACL Connection Handle for the
    * connection to the specified Bluetooth Board Address.
    * \note If this function returns with an error, negative value,
    *          the value returned in the Connection_Handle variable
    *          should be considered invalid.
    * \note If this function returns BTPS_ERROR_DEVICE_NOT_CONNECTED
    *          a connection to the specified Bluetooth Board Address
    *          does not exist.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Query_Connection_Handle(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t *Connection_Handle);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Query_Connection_Handle_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t *Connection_Handle);
#endif

   /*! @brief The following function is provided to for Local devices that
    * support Out of Band, OOB, pairing using a technology such as NFC,
    * Near Field communications. It is used to obtain the Simple
    * Pairing Hash C and the Simple Pairing Randomizer R which are
    * intended to be transferred to a remote device using OOB.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param OutOfBandData Pointer to a buffer that is to receive the
    * Out Of Band Data that the local device has generated.
    * @return  This function will return zero on success, or a negative return error
    * code if there was an error. If this function returns success,
    * then the OutOfBandData variable will contain a new set of C and R
    * values that can be used in an OOB transfer.
    * \note A new value for C and R are created each time this call
    *          is made. Each OOB transfer will have unique C and R
    *          values so after each OOB transfer this function should be
    *          called to obtain a new set for the next OOB transfer.
    * \note These values are not kept on a reset or power off in
    *          which case a call to this should be invoked during time
    *          time of initialization.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Query_Local_Out_Of_Band_Data(unsigned int BluetoothStackID, GAP_Out_Of_Band_Data_t *OutOfBandData);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Query_Local_Out_Of_Band_Data_t)(unsigned int BluetoothStackID, GAP_Out_Of_Band_Data_t *OutOfBandData);
#endif

   /*! @brief The following function is provided to for Local devices that
    * support Out of Band, OOB, pairing using a technology such as NFC,
    * Near Field communications.  It is used to obtain the Simple
    * Pairing Hash C and the Simple Pairing Randomizer R, derived from
    * both the P-192 and P-256 public keys, which are intended to be
    * transferred to a remote device using OOB.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param ExtendedOutOfBandData Pointer to a buffer that is to receive the extended Out Of Band Data that the local device has generated.
    * @return This function will
    * return zero on success, or a negative return error code if there
    * was an error.  If this function returns success, then the
    * OutOfBandData variable will contain a new set of C and R values
    * that can be used in an OOB transfer.
    * \note A new value for C_192, R_192, C_256 and R_256 are created
    *          each time this call is made.  Each OOB transfer will have
    *          unique C_192, R_192, C_256 and R_256 values so after each
    *          OOB transfer this function should be called to obtain a
    *          new set for the next OOB transfer.
    * \note These values are not kept on a reset or power off in
    *          which case a call to this should be invoked during time
    *          time of initialization.
    * \note This function can only be called if the local Bluetooth
    *          device is a 4.1 or greater chipset.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Query_Local_Extended_Out_Of_Band_Data(unsigned int BluetoothStackID, GAP_Extended_Out_Of_Band_Data_t *ExtendedOutOfBandData);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Query_Local_Extended_Out_Of_Band_Data_t)(unsigned int BluetoothStackID, GAP_Extended_Out_Of_Band_Data_t *ExtendedOutOfBandData);
#endif

   /*! @brief The following function is provided to allow a means to set the
    * Authenticated Payload Timeout of a connection to a remote
    * Bluetooth Device.  This function accepts as it's input parameters
    * the Bluetooth Protocol Stack ID of the Bluetooth Protocol Stack
    * that the connection exists, the Bluetooth Board Address of the
    * Remote Bluetooth Device which is connected, and the Authenticated
    * Payload Timeout for the connection to the specified Bluetooth
    * Board Address.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Bluetooth address of the remote device to set the authenticated payload for.
    * @param AuthenticatedPayloadTimeout Value to set for the authenticated payload timeout for the specified connection.
    * @return This function will return zero on success, or a
    * negative return error code if there was an error.
    * \note If this function returns BTPS_ERROR_DEVICE_NOT_CONNECTED
    *          a connection to the specified Bluetooth Board Address
    *          does not exist.
    * \note This function can only be called if the local Bluetooth
    *          device is a 4.1 or greater chipset.
    * \note The AuthenticatedPayloadTimeout parameter is specified in
    *          milliseconds.
    * \note The AuthenticatedPayloadTimeout parameter must adhere to
    *          the following rules based on the link state for the
    *          connection to the specified remote device:
    *
    *             1)  AuthenticatedPayloadTimeout >=
    *                       Tsniff (if in sniff mode)
    *             2)  AuthenticatedPayloadTimeout >=
    *                       MaxSubrate * Tsniff (if in sniff subrating)
    *             3)  AuthenticatedPayloadTimeout >=
    *                       holdTO (if in hold mode)
    *
    *          This function may not be called for a connection in park
    *          mode.
    * \note If the Authenticated Payload Timeout is the time between
    *          valid packets received with MICs on an encrypted link.
    *          The etAuthenticated_Payload_Timeout event will be
    *          dispatched to the callback registered with
    *          GAP_Register_Remote_Authentication() if the timeout
    *          expires with no valid MIC received from the remote
    *          device.
    * \note This function will only set the Authenticated Payload
    *          Timeout for BR/EDR connections.  For LE connections the
    *          GAP_LE_Set_Authenticated_Payload_Timeout() function must
    *          be called.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Set_Authenticated_Payload_Timeout(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t AuthenticatedPayloadTimeout);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Set_Authenticated_Payload_Timeout_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t AuthenticatedPayloadTimeout);
#endif

   /*! @brief The following function is provided to allow a means to query the
    * Authenticated Payload Timeout of a connection to a remote
    * Bluetooth Device.  This function accepts as it's input parameters
    * the Bluetooth Protocol Stack ID of the Bluetooth Protocol Stack
    * that the connection exists, the Bluetooth Board Address of the
    * Remote Bluetooth Device which is connected, and a pointer to a
    * variable that will receive the Authenticated Payload Timeout for
    * the connection to the specified Bluetooth Board Address.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Bluetooth address of the remote device to query the authenticated payload for.
    * @param AuthenticatedPayloadTimeout Pointer to location to store the queried authenticated payload timeout.
    * @return This function will return zero on success, or a negative return error
    * code if there was an error.  If this function returns success,
    * then the Authenticated Payload Timeout variable will contain the
    * Authenticated Payload Timeout for the connection to the specified
    * Bluetooth Board Address.
    * \note If this function returns with an error, negative value,
    *          the value returned in the AuthenticatedPayloadTimeout
    *          variable should be considered invalid.
    * \note If this function returns BTPS_ERROR_DEVICE_NOT_CONNECTED
    *          a connection to the specified Bluetooth Board Address
    *          does not exist.
    * \note This function can only be called if the local Bluetooth
    *          device is a 4.1 or greater chipset.
    * \note The AuthenticatedPayloadTimeout parameter is returned in
    *          milliseconds.
    * \note This function will only query the Authenticated Payload
    *          Timeout for BR/EDR connections.  For LE connections the
    *          GAP_LE_Query_Authenticated_Payload_Timeout() function
    *          must be called.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Query_Authenticated_Payload_Timeout(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t *AuthenticatedPayloadTimeout);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Query_Authenticated_Payload_Timeout_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t *AuthenticatedPayloadTimeout);
#endif

   /*! @brief The following function is provided to allow the host to cause the
    * Controller to refresh the encryption by pausing then resuming.
    * A BD_ADDR type is passed and from that a 'valid' connection
    * handle is determined, otherwise an error shall be returned.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Bluetooth device address of the remote device
    * @param GAP_Event_Callback Pointer to a callback function to be used
    * by the GAP layer to dispatch GAP Event information for this request.
    * @param CallbackParameter User defined value to be used by the GAP
    * layer as an input parameter for all callbacks.
    * @return Zero (0) if the Refresh encryption process was successfully submitted.
    * An error code if negative; one of the following values:
    * BTPS_ERROR_INVALID_BLUETOOTH_STACK_ID
    * BTPS_ERROR_INVALID_PARAMETER
    * BTPS_ERROR_DEVICE_HCI_ERROR
    * \note Because this function is asynchronous in nature, this
    *          function will notify the caller of the completion of a
    *          refresh via the installed Callback. This operation
    *          cannot be cancelled at any time other than a disconnect.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Refresh_Encryption_Key(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_Event_Callback_t GAP_Event_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Refresh_Encryption_Key_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_Event_Callback_t GAP_Event_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to allow the local host to read
    * the Extended Inquiry Response Information currently stored in the
    * controller.  This is the data that the controller will return when
    * it returns an extended inquiry response to a remote device.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param FEC_Required Specifies whether FEC is required or not.
    * @param Extended_Inquiry_Response_Data  Buffer that is to receive the actual
    * Extended Inquiry  Response Data that the local Bluetooth device is currently using.
    * This buffer must be at least 240 bytes in length.
    * @return  This function will return zero if successful, or a negative return
    * error code if there was an error condition.  If this function returns success, then the Extended_Inquiry_Response_Data member
    * will be filled in with the correct data.
    * \note The GAP_Parse_Extended_Inquiry_Response_Data() function
    *          can be used to parse the Extended Inquiry Response Data
    *          for easy parsing (if required).
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Read_Extended_Inquiry_Information(unsigned int BluetoothStackID, Byte_t *FEC_Required, Extended_Inquiry_Response_Data_t *Extended_Inquiry_Response_Data);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Read_Extended_Inquiry_Information_t)(unsigned int BluetoothStackID, Byte_t *FEC_Required, Extended_Inquiry_Response_Data_t *Extended_Inquiry_Response_Data);
#endif

   /*! @brief The following function is provided to allow the local host to
    * write the extended inquiry information to be stored in the
    * controller.  This is the data that the controller will return when
    * it returns an extended inquiry response to a remote device.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param FEC_Required Specifies whether FEC is required or not.
    * @param Extended_Inquiry_Response_Data  Buffer that contains the actual
    * Extended Inquiry Response Data that the local Bluetooth device is to begin using.
    * This buffer must be at least 240 bytes in length.
    * @return This function will return zero if successful, or a negative return
    * error code if there was an error condition.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Write_Extended_Inquiry_Information(unsigned int BluetoothStackID, Byte_t FEC_Required, Extended_Inquiry_Response_Data_t *Extended_Inquiry_Response_Data);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Write_Extended_Inquiry_Information_t)(unsigned int BluetoothStackID, Byte_t FEC_Required, Extended_Inquiry_Response_Data_t *Extended_Inquiry_Response_Data);
#endif

   /*! @brief The following function is provided to allow a simple mechanism to
    * convert a GAP_Extended_Inquiry_Response_Data_t to the raw
    * Extended_Inquiry_Response_Data_t.
    * @param GAP_Extended_Inquiry_Response_Data  Pointer to the Parsed Extended
    * Inquiry data that is to be converted.
    * @param Extended_Inquiry_Response_Data  Buffer that is to receive the actual
    * Extended Inquiry Response Data from the parsed Extended Inquiry Data.  This buffer must be at least 240 bytes in length.
    * \note This second parameter *MUST*
    * point to the maximum sized Extended Inquiry Response Buffer size
    * (EXTENDED_INQUIRY_RESPONSE_DATA_SIZE).
    * @return This function will return
    * the number of successfully converted items (zero or more), or a
    * negative error code if there was an error.
    * \note This function will populate the entire
    *          Extended_Inquiry_Response_Data_t buffer (all
    *          EXTENDED_INQUIRY_RESPONSE_DATA_SIZE bytes).  If the
    *          specified information is smaller than the full
    *          Extended Inquiry Response Data size, the resulting buffer
    *          will be padded with zeros.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Convert_Extended_Inquiry_Response_Data(GAP_Extended_Inquiry_Response_Data_t *GAP_Extended_Inquiry_Response_Data, Extended_Inquiry_Response_Data_t *Extended_Inquiry_Response_Data);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Convert_Extended_Inquiry_Response_Data_t)(GAP_Extended_Inquiry_Response_Data_t *GAP_Extended_Inquiry_Response_Data, Extended_Inquiry_Response_Data_t *Extended_Inquiry_Response_Data);
#endif

   /*! @brief The following function is a utility function that exists to parse
    * the specified Extended_Inquiry_Response_Data_t information into
    * a GAP_Extended_Inquiry_Response_Data_t structure (for ease of
    * parsing).
    * @param Extended_Inquiry_Response_Data Buffer that contains the
    * actual Extended Inquiry Response Data that is to be parsed.
    * This buffer must be at least 240 bytes in length.
    * @param GAP_Extended_Inquiry_Response_Data Pointer to the Parsed
    * Extended Inquiry data that has been parsed.
    * @return Non-negative if successful.  This value represents the number
    * of valid Extended Inquiry Response data fields that were successfully parsed.
    * An error code if negative; one of the following values:
    * BTPS_ERROR_INVALID_BLUETOOTH_STACK_ID
    * BTPS_ERROR_INVALID_PARAMETER
    * BTPS_ERROR_INSUFFICIENT_BUFFER_SPACE
    * \note If this function is called with a NULL passed as the
    *          final parameter, then, this function will simply
    *          calculate the number of Extended Inquiry Data Information
    *          Entries that will be required to hold the parsed
    *          information.  If the final parameter is NOT NULL then it
    *          *MUST* contain the maximum number of entries that can
    *          be supported (specified via the Number_Data_Entries
    *          member) and the Data_Entries member must point to memory
    *          that contains (at least) that many members).
    * \note This function will return
    *        BTPS_ERROR_INSUFFICIENT_BUFFER_SPACE
    *        if there was not enough Data Entries specified (via the
    *        Number_Data_Entries member) to satisfy the parsing of
    *        the actual Extended Inquiry Response Data.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_Parse_Extended_Inquiry_Response_Data(Extended_Inquiry_Response_Data_t *Extended_Inquiry_Response_Data, GAP_Extended_Inquiry_Response_Data_t *GAP_Extended_Inquiry_Response_Data);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_Parse_Extended_Inquiry_Response_Data_t)(Extended_Inquiry_Response_Data_t *Extended_Inquiry_Response_Data, GAP_Extended_Inquiry_Response_Data_t *GAP_Extended_Inquiry_Response_Data);
#endif

   /*! GAP Low Energy (LE) API Prototypes. */

   /*! @brief The following function is provided to establish an LE master
    * connection to a remote device.  To establish an LE slave
    * connection to a remote device the GAP_LE_Set_Advertising_Enable()
    * function should be used to put the local Bluetooth device in a
    * connectable mode.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param ScanInterval Scan interval to use when scanning for the device(s)
    * to connect. This value is specified in milli-seconds
    * @param ScanWindow  Scan window to use when scanning for the device(s) to
    * connect. This value is specified in milli-seconds
    * @param InitatorFilterPolicy Filter policy to apply when scanning.
    * If the white-list filter is specified then the remote device address
    * (and address type) are ignored.
    * @param RemoteAddressType Specifies the type of the remote device address
    * to connect with (if not using white-list filter).
    * @param RemoteDevice Specifies the remote device address to connect with.
    * This value is required if no filter is specified as the filter policy.
    * @param LocalAddressType Specifies the type of the address the local device
    * is to use when connecting to the remote device
    * @param ConnectionParameters Specifies the parameters to use when actually
    * establishing the connection to the remote device
    * \note ALL parameters are specified in milli-seconds except the Slave_Latency
    * parameter which is specified in connection events.
    * @param GAP_LE_Event_Callback Pointer to a callback function to be used by
    * the GAP layer to dispatch GAP LE Event information for this request.
    * @param CallbackParameter User defined value to be used by the GAP layer as
    * an input parameter for the callback.
    * @return This function returns zero
    * if successful, or a negative return error code.
    * \note This function is asynchronous in nature and the caller
    *          is informed when the connection is established via the
    *          GAP_LE_Event_Callback function.
    * \note If the InitatorFilterPolicy parameter is set to
    *          fpWhiteList then the RemoteAddressType and RemoteDevice
    *          parameter are not used and NULL values may be passed in
    *          for them.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Create_Connection(unsigned int BluetoothStackID, unsigned int ScanInterval, unsigned int ScanWindow, GAP_LE_Filter_Policy_t InitatorFilterPolicy, GAP_LE_Address_Type_t RemoteAddressType, BD_ADDR_t *RemoteDevice, GAP_LE_Address_Type_t LocalAddressType, GAP_LE_Connection_Parameters_t *ConnectionParameters, GAP_LE_Event_Callback_t GAP_LE_Event_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Create_Connection_t)(unsigned int BluetoothStackID, unsigned int ScanInterval, unsigned int ScanWindow, GAP_LE_Filter_Policy_t InitatorFilterPolicy, GAP_LE_Address_Type_t RemoteAddressType, BD_ADDR_t *RemoteDevice, GAP_LE_Address_Type_t LocalAddressType, GAP_LE_Connection_Parameters_t *ConnectionParameters, GAP_LE_Event_Callback_t GAP_LE_Event_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to allow a means of canceling a
    * connection establishment proceduring that was started via a call
    * to the GAP_LE_Create_Connection() function.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @return This function returns zero if successful, or a negative return error
    * code.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Cancel_Create_Connection(unsigned int BluetoothStackID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Cancel_Create_Connection_t)(unsigned int BluetoothStackID);
#endif

   /*! @brief The following function is provided to allow a means of
    * disconnecting an LE connection.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Specifies the remote device address of the currently
    * connected device to disconnect.
    * @return This function returns zero if successful, or a negative return
    * error code.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Disconnect(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Disconnect_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR);
#endif

   /*! @brief The following function is provided to allow a means of Reading the
    * remote used features from a remote device over a previously
    * established LE connection.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Specifies the remote device address of the currently
    * connected device to query the remote LMP features.
    * @return This function returns zero if successful, or a negative return error
    * code.
    * \note This function is only valid to be called if the local
    *          Bluetooth is acting in the master role for the connection
    *          specified by the device address.
    * \note This function will not create an LE ACL connection to the
    *          specified device.  The LE ACL connection to the specified
    *          remote device must already exist before calling this
    *          function.
    * \note This function is asynchronous and the user will be
    *          notified via the Callback Function specified in the the
    *          call to GAP_LE_Create_Connection() that was used to
    *          establish this connection.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Read_Remote_Features(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Read_Remote_Features_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR);
#endif

   /*! @brief The following function is provided to allow a mechanism for
    * starting an LE scan procedure.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param ScanType Specifies the type of scan to perform.
    * @param ScanInterval Specifies interval to use while scanning.
    * @param ScanWindow Specifies window to use while scanning.
    * @param LocalAddressType Specifies the type of the address the local device is
    * to use when scanning.
    * @param FilterPolicy Filter policy to apply when scanning.
    * @param FilterDuplicates Specifies whether or not the host controller is to
    * filter duplicate scan responses.
    * @param GAP_LE_Event_Callback Pointer to a callback function to be used by
    * the GAP layer to dispatch GAP LE Event information for this request.
    * @param CallbackParameter User defined value to be used by the GAP layer as
    * an input parameter for the callback.
    * @return This function returns zero if successful, or a negative return error code if an
    * Scan was unable to be performed.
    * \note Only ONE scan can be performed at any given time.
    *          Calling this function while an outstanding scan is in
    *          progress will fail.  The caller can call the
    *          GAP_LE_Cancel_Scan() function to cancel a currently
    *          executing scanning procedure.
    * \note The ScanInterval and ScanWindow (specified in
    *          milli-seconds) must satisify the following equation:
    *          ScanWindow <= ScanInterval if ScanWindow == ScanInterval,
    *          LE Scanning will be performed continuously.
    * \note All scan period time parameters are specified in seconds.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Perform_Scan(unsigned int BluetoothStackID, GAP_LE_Scan_Type_t ScanType, unsigned int ScanInterval, unsigned int ScanWindow, GAP_LE_Address_Type_t LocalAddressType, GAP_LE_Filter_Policy_t FilterPolicy, Boolean_t FilterDuplicates, GAP_LE_Event_Callback_t GAP_LE_Event_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Perform_Scan_t)(unsigned int BluetoothStackID, GAP_LE_Scan_Type_t ScanType, unsigned int ScanInterval, unsigned int ScanWindow, GAP_LE_Address_Type_t LocalAddressType, GAP_LE_Filter_Policy_t FilterPolicy, Boolean_t FilterDuplicates, GAP_LE_Event_Callback_t GAP_LE_Event_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to allow a means of cancelling
    * a scan process that was started via a successful call to the
    * GAP_LE_Perform_Scan() function.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @return This function returns zero if the scan process was able to be
    * cancelled, or a negative return error code if there was an error.
    * If this function returns success then the GAP LE callback that was
    * installed with the GAP_LE_Perform_Scan() function will NEVER be
    * called.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Cancel_Scan(unsigned int BluetoothStackID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Cancel_Scan_t)(unsigned int BluetoothStackID);
#endif

   /*! The following structure provides a method of writing the
    * advertising data that will be advertised.  This function takes as
    * parameters the Bluetooth stack ID of the Bluetooth device, the
    * number of significant bytes in the advertising data and finally
    * the advertising data to write.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param Length Number of significant advertising data bytes contained in the
    * advertising data.
    * @param Advertising_Data Pointer to a buffer that contains the advertising data.
    * This buffer must be at least:
    * ADVERTISING_DATA_MAXIMUM_SIZE bytes long.
    * \note The length parameter specifies the actual number of bytes that are valid.
    * The remaining bytes should be padded with zero’s.
    * @return This function returns zero on
    * success or a negative error code.
    * \note The Length and Advertising_Data parameters are optional.
    *          If the intention is to advertising data whose length is
    *          zero then the Length and Advertising_Data parameters may
    *          be set to 0 and NULL respectively.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Set_Advertising_Data(unsigned int BluetoothStackID, unsigned int Length, Advertising_Data_t *Advertising_Data);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Set_Advertising_Data_t)(unsigned int BluetoothStackID, unsigned int Length, Advertising_Data_t *Advertising_Data);
#endif

   /*! @brief The following function is provided to allow a simple mechanism to
    * convert a GAP_LE_Advertising_Data_t to the raw Advertising_Data_t
    * representation. This second parameter *MUST* point to the maximum
    * sized Advertising Data Buffer size (ADVERTISING_DATA_SIZE).
    * @param GAP_LE_Advertising_Data Pointer to the parsed advertising data
    * that is to be converted.
    * @param Advertising_Data Buffer that is to receive the actual
    * advertising data from the parsed advertising data. This buffer must be at least:
    * ADVERTISING_DATA_SIZE bytes in length.
    * @return This function will return the number of successfully converted items
    * (zero or more), or a negative error code if there was an error.
    * \note This function will populate the entire Advertising_Data_t
    *          buffer (all ADVERTISING_DATA_SIZE bytes).  If the
    *          specified information is smaller than the full
    *          Advertising Data size, the resulting buffer will be
    *          padded with zeros.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Convert_Advertising_Data(GAP_LE_Advertising_Data_t *GAP_LE_Advertising_Data, Advertising_Data_t *Advertising_Data);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Convert_Advertising_Data_t)(GAP_LE_Advertising_Data_t *GAP_LE_Advertising_Data, Advertising_Data_t *Advertising_Data);
#endif

   /*! @brief The following function is a utility function that exists to parse
    * the specified Advertising_Data_t information into a
    * GAP_LE_Advertising_Data_t structure (for ease of parsing).
    * @param Advertising_Data Buffer that contains the actual advertising data
    * that is to be parsed.  This buffer must be at least:
    * ADVERTISING_DATA_SIZE
    * bytes long. \note If the advertising occupies less data bytes than the
    * data should be padded with zero bytes (0x00).
    * @param GAP_LE_Advertising_Data Pointer to the parsed advertising data
    * that has been parsed.  Note that if this parameter is not NULL then
    * the Number_Data_Entries member must contain the number of data entries
    * that the Data_Entries member points to (to receive the parsed data information.
    * @return Non-negative if successful. This value represents the number
    * of valid advertising data fields that were successfully parsed.
    * An error code if negative; one of the following values:
    * BTPS_ERROR_INVALID_PARAMETER or BTPS_ERROR_INSUFFICIENT_BUFFER_SPACE
    * \note If this function is called with a NULL passed as the
    *          final parameter, then, this function will simply
    *          calculate the number of Advertising Data Information
    *          Entries that will be required to hold the parsed
    *          information. If the final parameter is NOT NULL then it
    *          *MUST* contain the maximum number of entries that can be
    *          supported (specified via the Number_Data_Entries member)
    *          and the Data_Entries member must point to memory that
    *          contains (at least) that many members).
    * \note This function will return
    *
    *             BTPS_ERROR_INSUFFICIENT_BUFFER_SPACE
    *
    *          if there was not enough Data Entries specified (via the
    *          Number_Data_Entries member) to satisfy the parsing of
    *          the actual Advertising Data.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Parse_Advertising_Data(Advertising_Data_t *Advertising_Data, GAP_LE_Advertising_Data_t *GAP_LE_Advertising_Data);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Parse_Advertising_Data_t)(Advertising_Data_t *Advertising_Data, GAP_LE_Advertising_Data_t *GAP_LE_Advertising_Data);
#endif

   /*! The following structure provides a method of writing the
    * advertising data that will be returned when the local device
    * responds to a scan request.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param Length Number of significant advertising data bytes contained
    * in the advertising data.
    * @param Scan_Response_Data Pointer to a buffer that contains the response
    * data. This buffer must be at least:
    * SCAN_RESPONSE_DATA_MAXIMUM_SIZE
    * bytes long. \note The length parameter specifies the actual number of
    * bytes that are valid.  The remaining bytes should be padded with zero’s.
    * @return This function returns zero on success or
    * a negative error code.
    * \note The Length and Scan_Response_Data parameters are
    *          optional.  If the intention is to scan response data
    *          whose length is zero then the Length and
    *          Scan_Response_Data parameters may be set to 0 and NULL
    *          respectively.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Set_Scan_Response_Data(unsigned int BluetoothStackID, unsigned int Length, Scan_Response_Data_t *Scan_Response_Data);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Set_Scan_Response_Data_t)(unsigned int BluetoothStackID, unsigned int Length, Scan_Response_Data_t *Scan_Response_Data);
#endif

   /*! @brief The following function is provided to allow a simple mechanism to
    * convert a GAP_LE_Advertising_Data_t to the raw
    * Scan_Response_Data_t representation.  This second parameter *MUST*
    * point to the maximum sized Scan Response Buffer size
    * (SCAN_RESPONSE_DATA_SIZE).
    * @param GAP_LE_Advertising_Data Pointer to the parsed advertising data
    * that is to be converted.
    * @param Scan_Response_Data  Buffer that is to receive the actual scan
    * response data from the parsed advertising data. This buffer must be at least:
    * SCAN_RESPONSE_DATA_SIZE bytes in length.
    * @return This function will return the number
    * of successfully converted items (zero or more), or a negative
    * error code if there was an error.
    * \note This function will populate the entire
    *          Scan_Response_Data_t buffer (all SCAN_RESPONSE_DATA_SIZE
    *          bytes).  If the specified information is smaller than the
    *          full Scan Response size, the resulting buffer will be
    *          padded with zeros.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Convert_Scan_Response_Data(GAP_LE_Advertising_Data_t *GAP_LE_Advertising_Data, Scan_Response_Data_t *Scan_Response_Data);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Convert_Scan_Response_Data_t)(GAP_LE_Advertising_Data_t *GAP_LE_Advertising_Data, Scan_Response_Data_t *Scan_Response_Data);
#endif

   /*! @brief The following function is a utility function that exists to parse
    * the specified Scan_Response_Data_t information into a
    * GAP_LE_Advertising_Data_t structure (for ease of parsing).
    * @param Scan_Response_Data Buffer that contains the actual scan response
    * data that is to be parsed.  This buffer must be at least:
    * SCAN_RESPONSE_DATA_SIZE
    * bytes long. \note If the scan rsponse occupies less data bytes than the
    * data should be padded with zero bytes (0x00).
    * @param GAP_LE_Advertising_Data Pointer to the parsed scan response data
    * that has been parsed.  Note that if this parameter is not NULL then the
    * Number_Data_Entries member must contain the number of data entries that
    * the Data_Entries member points to (to receive the parsed data information.
    * @return Non-negative if successful. This value represents the number of
    * valid advertising data fields that were successfully parsed.
    * An error code if negative; one of the following values:
    * BTPS_ERROR_INVALID_PARAMETER or BTPS_ERROR_INSUFFICIENT_BUFFER_SPACE
    * \note If this function is called with a NULL passed as the
    *          final parameter, then, this function will simply
    *          calculate the number of Advertising Data Information
    *          Entries that will be required to hold the parsed
    *          information.  If the first parameter is NOT NULL then it
    *          *MUST* contain the maximum number of entries that can be
    *          supported (specified via the Number_Data_Entries member)
    *          and the Data_Entries member must point to memory that
    *          contains (at least) that many members).
    * \note This function will return
    *
    *             BTPS_ERROR_INSUFFICIENT_BUFFER_SPACE
    *
    *          if there was not enough Data Entries specified (via the
    *          Number_Data_Entries member) to satisfy the parsing of
    *          the actual Scan Response Data.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Parse_Scan_Response_Data(Scan_Response_Data_t *Scan_Response_Data, GAP_LE_Advertising_Data_t *GAP_LE_Advertising_Data);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Parse_Scan_Response_Data_t)(Scan_Response_Data_t *Scan_Response_Data, GAP_LE_Advertising_Data_t *GAP_LE_Advertising_Data);
#endif

   /*! The following structure provides a method of enabling advertising
    * using the data written using GAP_LE_Set_Advertising_Data.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param EnableScanResponse Flag which specifies whether or not the
    * device should send scan response data in response to a scan request.
    * @param GAP_LE_Advertising_Parameters Pointer to advertising parameters
    * that control how the advertising is performed.
    * @param GAP_LE_Connectability_Parameters Specifies the connectability
    * parameters to use while advertising.
    * @param GAP_LE_Event_Callback Pointer to a callback function to be
    * used by the GAP layer to dispatch GAP LE event information for this request.
    * @param CallbackParameter User defined value to be used by the GAP
    * layer as an input parameter for the callback.
    * @return This function will return zero on success, or a negative return error code if
    * there was an error.
    * \note The lcmLowDutyCycleDirectConnectable connectable mode is
    *          only valid to be passed to this device if the local
    *          Bluetooth device is a 4.1 or greater chipset.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Advertising_Enable(unsigned int BluetoothStackID, Boolean_t EnableScanResponse, GAP_LE_Advertising_Parameters_t *GAP_LE_Advertising_Parameters, GAP_LE_Connectability_Parameters_t *GAP_LE_Connectability_Parameters, GAP_LE_Event_Callback_t GAP_LE_Event_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Advertising_Enable_t)(unsigned int BluetoothStackID, Boolean_t EnableScanResponse, GAP_LE_Advertising_Parameters_t *GAP_LE_Advertising_Parameters, GAP_LE_Connectability_Parameters_t *GAP_LE_Connectability_Parameters, GAP_LE_Event_Callback_t GAP_LE_Event_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to allow a means of disabling
    * LE advertising that was enabled via a previously successful call
    * to to the GAP_LE_Advertising_Enable() function.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @return This function will return zero on success, or a negative return error
    * code if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Advertising_Disable(unsigned int BluetoothStackID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Advertising_Disable_t)(unsigned int BluetoothStackID);
#endif

   /*! @brief The following function is provided to allow a means to generate a
    * non-resolvable random device address.  This device address can
    * then be used as a random address by call
    * GAP_LE_Set_Random_Address().
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param NonResolvableAddress_Result Buffer that will receive the
    * generated non resolvable address upon successful execution of this function.
    * @return If this function returns sucess (zero).  This
    * function will return zero on success, or a negative return error
    * code if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Generate_Non_Resolvable_Address(unsigned int BluetoothStackID, BD_ADDR_t *NonResolvableAddress_Result);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Generate_Non_Resolvable_Address_t)(unsigned int BluetoothStackID, BD_ADDR_t *NonResolvableAddress_Result);
#endif

   /*! @brief The following function is provided to allow a means to generate a
    * static random device address.  This device address can then be
    * used as a random address by call GAP_LE_Set_Random_Address().
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param StaticAddress_Result Buffer that will receive the generated
    * static address upon successful execution of this function.
    * @return If this function returns
    * sucess (zero).  This function will return zero on success, or a
    * negative return error code if there was an error.
    * \note The Bluetooth Specification has defined that the Static
    *          Address should only change once per power cycle,
    *          therefore it is the requirement of the application that
    *          this function is only used to generate a new Static
    *          Address once per power cycle.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Generate_Static_Address(unsigned int BluetoothStackID, BD_ADDR_t *StaticAddress_Result);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Generate_Static_Address_t)(unsigned int BluetoothStackID, BD_ADDR_t *StaticAddress_Result);
#endif

   /*! @brief The following function is provided to allow a means to generate a
    * resolvable random device address.  This device address can then be
    * used as a random address by call to the
    * GAP_LE_Set_Random_Address() function.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param IRK Identity resolving key (IRK) that is used to generate the
    * resolvable address.
    * @param ResolvableAddress_Result Buffer that will receive a generated
    * resolvable address upon successful execution of this function.
    * @return If this function returns success.  This function will
    * return zero on success, or a negative return error code if there
    * was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Generate_Resolvable_Address(unsigned int BluetoothStackID, Encryption_Key_t *IRK, BD_ADDR_t *ResolvableAddress_Result);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Generate_Resolvable_Address_t)(unsigned int BluetoothStackID, Encryption_Key_t *IRK, BD_ADDR_t *ResolvableAddress_Result);
#endif

   /*! @brief The following function is provided to allow a means to attempt to
    * resolve a resolvable random device address.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param IRK Identity resolving key (IRK) that is used to resolve the
    * resolvable address.
    * @param ResolvableAddress Bluetooth address that represents the
    * resolvable address that is attempting to be resolved (using the specified IRK).
    * @return This function returns a
    * boolean result, (TRUE) if the resolvable address was resolved
    * using the passed in IRK, FALSE if it was not.
    */
BTPSAPI_DECLARATION Boolean_t BTPSAPI GAP_LE_Resolve_Address(unsigned int BluetoothStackID, Encryption_Key_t *IRK, BD_ADDR_t ResolvableAddress);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef Boolean_t (BTPSAPI *PFN_GAP_LE_Resolve_Address_t)(unsigned int BluetoothStackID, Encryption_Key_t *IRK, BD_ADDR_t ResolvableAddress);
#endif

   /*! @brief The following function is provided to allow a means to write a
    * random address to the Bluetooth device.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param RandomAddress Actual random address value to set in
    * the local device.
    * @return This function will
    * return zero on success, or a negative return error code if there
    * was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Set_Random_Address(unsigned int BluetoothStackID, BD_ADDR_t RandomAddress);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Set_Random_Address_t)(unsigned int BluetoothStackID, BD_ADDR_t RandomAddress);
#endif

   /*! @brief The following function is provided to allow a means to manage the
    * white list which is stored in the Bluetooth device which is
    * specified by the first parameter.  This function is responsible
    * for adding one (or more) devices to the white list.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param DeviceCount Total number of device list entries that are
    * pointed to by the WhiteListEntries buffer.
    * @param WhiteListEntries Buffer that contains one or more individual
    * white list device entries to write to the local device.
    * This buffer must point to (at least) DeviceCount entries.
    * @param AddedDeviceCount Upon successful execution of this
    * function contains the total number of white list entries that were
    * successfully written to the device white list.
    * @return This function will return
    * zero on success, or a negative return error code if there was an
    * error.
    * \note This function will return GAP_LE_ERROR_WHITE_LIST_IN_USE
    *          if a scanning, advertising or a connection establishment
    *          procedure is using the white list and the white list can
    *          not be modified.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Add_Device_To_White_List(unsigned int BluetoothStackID, unsigned int DeviceCount, GAP_LE_White_List_Entry_t *WhiteListEntries, unsigned int *AddedDeviceCount);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Add_Device_To_White_List_t)(unsigned int BluetoothStackID, unsigned int DeviceCount, GAP_LE_White_List_Entry_t *WhiteListEntries, unsigned int *AddedDeviceCount);
#endif

   /*! @brief The following function is provided to allow a means to manage the
    * white list which is stored in the Bluetooth device which is
    * specified by the first parameter.  This function is responsible
    * for removing devices to the white list.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param DeviceCount Total number of device list entries
    * that are pointed to by the WhiteListEntries buffer.
    * If this value is specified as zero then the next parameter
    * is ignored and all devices are removed from the white list.
    * @param WhiteListEntries Buffer that contains one or more
    * individual white list device entries to remove from the
    * local device.This buffer must point to (at least) DeviceCount entries.
    * @param RemovedDeviceCount Upon successful execution of this
    * function contains the total number of white list entries
    * that were successfully removed from the device white list.
    * @return This function will
    * return zero on success, or a negative return error code if there
    * was an error.
    * \note Specifying a DeviceCount of zero instructs the module to
    *          delete ALL members from the white list (i.e. clear the
    *          white list).
    * \note This function will return GAP_LE_ERROR_WHITE_LIST_IN_USE
    *          if a scanning, advertising or a connection establishment
    *          procedure is using the white list and the white list can
    *          not be modified.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Remove_Device_From_White_List(unsigned int BluetoothStackID, unsigned int DeviceCount, GAP_LE_White_List_Entry_t *WhiteListEntries, unsigned int *RemovedDeviceCount);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Remove_Device_From_White_List_t)(unsigned int BluetoothStackID, unsigned int DeviceCount, GAP_LE_White_List_Entry_t *WhiteListEntries, unsigned int *RemovedDeviceCount);
#endif

   /*! @brief The following function is provided to allow a means of reading
    * the white list size for the Bluetooth device specified by the
    * Bluetooth stack ID parameter.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param WhiteListSize Total number of device list entries are supported
    * by the local device.  This value is the number of entried NOT the number
    * of white list entry buffer size in bytes.
    * @return If this function returns success. This function will return zero on
    * success, or a negative return error code if there was an error.
    * \note The returned white list size is returned in terms of the
    *          number of device entries it may hold (not individual
    *          bytes).
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Read_White_List_Size(unsigned int BluetoothStackID, unsigned int *WhiteListSize);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Read_White_List_Size_t)(unsigned int BluetoothStackID, unsigned int *WhiteListSize);
#endif

   /*! @brief The following function is provided to allow a means of changing
    * the pairability mode of the Bluetooth device.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param PairableMode Pairability mode to set.
    * @return This function will return zero on
    * success, or a negative return error code if there was an error.
    * \note If the pairability mode is lpmNonPairableMode then the
    *          final two parameters are not used and may be ignored.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Set_Pairability_Mode(unsigned int BluetoothStackID, GAP_LE_Pairability_Mode_t PairableMode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Set_Pairability_Mode_t)(unsigned int BluetoothStackID, GAP_LE_Pairability_Mode_t PairableMode);
#endif

   /*! @brief The following function is provided to allow a means to register a
    * GAP LE event Callback to accept remote authentication requests.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param GAP_LE_Event_Callback Pointer to a callback function to be
    * used by the GAP layer to dispatch GAP LE event information for this request.
    * @param CallbackParameter User defined value to be used by the GAP
    * layer as an input parameter for the specified callback.
    * @return This function returns zero if the callback was
    * successfully installed, or a negative error return code if the
    * callback was not successfully installed.  It should be noted that
    * ONLY ONE remote authentication callback can be installed per
    * Bluetooth device.  The caller can un-register the remote
    * authentication callback that was registered with this function (if
    * successful) by calling the
    * GAP_LE_Un_Register_Remote_Authentication() function.
    * \note This function can be called by master or slave.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Register_Remote_Authentication(unsigned int BluetoothStackID, GAP_LE_Event_Callback_t GAP_LE_Event_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Register_Remote_Authentication_t)(unsigned int BluetoothStackID, GAP_LE_Event_Callback_t GAP_LE_Event_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to allow a mechanism to
    * un-register a previously registered GAP LE event callback for
    * remote authentication events.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @return This function returns zero if successful, or a
    * negative return error code if unsuccessful.
    * \note This function can be called by master or slave.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Un_Register_Remote_Authentication(unsigned int BluetoothStackID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Un_Register_Remote_Authentication_t)(unsigned int BluetoothStackID);
#endif

   /*! @brief The following function is provided to allow a means of pairing to
    * a remote device.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Bluetooth device address of the connected device
    * to pair with.
    * @param Capabilities Pointer to a buffer that holds the pairing
    * capabilities of the local host.
    * @param GAP_LE_Event_Callback Pointer to a callback function to
    * be used by the GAP layer to dispatch GAP LE event information for this request.
    * @param CallbackParameter User defined value to be used by the
    * GAP layer as an input parameter for the specified callback.
    * @return This function will return zero on success,
    * or a negative return error code if there was an error.
    * \note This function can only be issued by a master because the
    *          slave device is not allowed to perform security
    *          processes, the slave can only request them).
    * \note This function will not create an LE ACL connection to the
    *          specified device.  The LE ACL connection to the specified
    *          remote device must already exist before calling this
    *          function.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Pair_Remote_Device(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_LE_Pairing_Capabilities_t *Capabilities, GAP_LE_Event_Callback_t GAP_LE_Event_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Pair_Remote_Device_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_LE_Pairing_Capabilities_t *Capabilities, GAP_LE_Event_Callback_t GAP_LE_Event_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to allow a mechanism for the
    * local device to respond to GAP LE authentication events.  This
    * function is used to set the authentication information for the
    * specified Bluetooth device.
    * @param BluetoothStackID Unique identifier assigned to this
    * Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Device address of the Bluetooth device that
    * is being authenticated.
    * @param GAP_LE_Authentication_Information Pointer to a structure
    * that holds authentication information.
    * @return This function returns zero if successful,
    * or a negative return error code if there was an error.
    * \note This function can be issued by the master or the slave
    *          in response to an authentication/pairing event.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Authentication_Response(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_LE_Authentication_Response_Information_t *GAP_LE_Authentication_Information);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Authentication_Response_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_LE_Authentication_Response_Information_t *GAP_LE_Authentication_Information);
#endif

   /*! @brief The following function is provided to allow a means of requesting
    * a security re-establishment, using information transferred from a
    * previous pairing process (during a previous connection) to the
    * device specified by the BD_ADDR parameter.  The function accepts
    * as parameters the Bluetooth Stack ID of the Bluetooth Device, the
    * BD_ADDR of the device to re-establish security, the security
    * parameters, and the and a GAP LE event callback (and parameter).
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Bluetooth device address of the connected device to
    * re-establish security with.
    * @param SecurityInformation Pointer to a buffer that holds the
    * security information required to re-establish the security.
    * @param GAP_LE_Event_Callback Pointer to a callback function to
    * be used by the GAP layer to dispatch GAP LE event information for this request.
    * @param CallbackParameter User defined value to be used by the
    * GAP layer as an input parameter for the specified callback.
    * @return This function returns zero if successful or a negative error code.
    * \note This function can be called by either a master or slave
    *          (of the connected device).  The parameters that are used
    *          are taken from the SecurityInformation parameter (which
    *          contains parameters for both master and slave).
    * \note This function will not create an LE ACL connection to the
    *          specified device.  The LE ACL connection to the specified
    *          remote device must already exist before calling this
    *          function.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Reestablish_Security(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_LE_Security_Information_t *SecurityInformation, GAP_LE_Event_Callback_t GAP_LE_Event_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Reestablish_Security_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_LE_Security_Information_t *SecurityInformation, GAP_LE_Event_Callback_t GAP_LE_Event_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to allow a means of requesting
    * that the remote device begin a security process (either pairing or
    * re-establishing previously exstablished security).  This function
    * accepts the Bluetooth stack ID followed by the address of the
    * remote device, followed by the security information required to be
    * passed to the remote device.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Bluetooth device address of the connected device to request security from.
    * @param Bonding_Type The required bonding type for the security being requested.  This value is one of the following:
    * lbtNoBonding
    * lbtBonding
    * @param MITM Flag that specifies whether man in the middle (MITM) protection is required.
    * @param SC Flag that specifies whether Secure Connections (SC) pairing is required.
    * @param Keypress Flag that specifies whether Keypress notification is required.
    * @param P256Debug Flag that specifies whether P256 debug local keys is required.
    * @param GAP_LE_Event_Callback Pointer to a callback function to be used by the GAP layer to dispatch GAP LE event information for this request.
    * @param CallbackParameter User defined value to be used by the GAP layer as an input parameter for the specified callback.
    * @return This function returns zero if
    * successful or a negative return error code if there was an error.
    * \note This function can only be issued by a slave (because the
    *          master is responsible for performing the security
    *          procedure - the slave can only request it).
    * \note This function will not create an LE ACL connection to the
    *          specified device.  The LE ACL connection to the specified
    *          remote device must already exist before calling this
    *          function.
    * \note In case you are not using LE Secure Connections,
    *          SC, Keypress, P256Debug Boolean parameters
    *          should be set to FALSE
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Request_Security(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_LE_Bonding_Type_t Bonding_Type, Boolean_t MITM, Boolean_t SC, Boolean_t Keypress, Boolean_t P256Debug, GAP_LE_Event_Callback_t GAP_LE_Event_Callback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Request_Security_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_LE_Bonding_Type_t Bonding_Type, Boolean_t MITM, Boolean_t SC, Boolean_t Keypress, Boolean_t P256Debug, GAP_LE_Event_Callback_t GAP_LE_Event_Callback, unsigned long CallbackParameter);
#endif

   /*! @brief The following function is provided to configure a fixed Display
    * Passkey to be used during the pairing process.  A Fixed Display
    * Passkey will ONLY be used if the local device is chosen to display
    * the passkey based on the local I/O Capabilities and the remote I/O
    * Capabilities.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param Fixed_Display_Passkey Optional pointer to the fixed display
    * passkey to use.  If this parameter is NULL, then a fixed display
    * passkey that was previously set using this function is no longer used.
    * If this parameter is non-NULL then the passkey that it points to is used
    * for all future pairing operations where the local Bluetooth device displays the passkey.
    * @return This function returns zero if successful or a negative return
    * error code if there was an error.
    * \note If Fixed_Display_Passkey is not specified (NULL) then the
    *          Fixed Display Passkey that was specified in an earlier
    *          call to this function will no longer be used.
    * \note If successful this function will set the Passkey that
    *          will be used in all future pairing processes where the
    *          local device is chosen to display the passkey used
    *          during the pairing process.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Set_Fixed_Passkey(unsigned int BluetoothStackID, DWord_t *Fixed_Display_Passkey);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Set_Fixed_Passkey_t)(unsigned int BluetoothStackID, DWord_t *Fixed_Display_Passkey);
#endif

   /*! @brief The following function is provided to allow a means to query the
    * Current Encryption Mode Parameter for the Bluetooth Device that is
    * specified by the Bluetooth Protocol Stack that is associated with
    * the specified Bluetooth Stack ID.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Bluetooth device address of the connected device to query
    * the link encryption mode.
    * @param GAP_Encryption_Mode Pointer to store the link encryption mode.
    * This parameter is not optional, and can not be NULL.
    * @return If this function returns success, then the
    * GAP Encryption Mode parameter will contain the current Encryption
    * Mode Setting for the specified Bluetooth Link.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Query_Encryption_Mode(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_Encryption_Mode_t *GAP_Encryption_Mode);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Query_Encryption_Mode_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_Encryption_Mode_t *GAP_Encryption_Mode);
#endif

   /*! @brief The following function is provided to allow a means to query the
    * LE Connection Handle of a connection to a remote Bluetooth Low
    * Energy Device.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Address of the Bluetooth Low Energy device of which to
    * query the connection handle.
    * @param Connection_Handle Pointer to a variable that will receive the
    * connection handle associated with the specified Bluetooth device address.
    * @return If this function returns success,
    * then the Connection Handle variable will contain the current LE
    * Connection Handle for the connection to the specified Bluetooth
    * Board Address.
    * \note @return If this function returns with an error, negative value,
    *          the value returned in the Connection_Handle variable
    *          should be considered invalid.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Query_Connection_Handle(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t *Connection_Handle);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Query_Connection_Handle_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t *Connection_Handle);
#endif

   /*! @brief The following function is provided to allow a means to query the
    * LE Connection Parameters of a connection to a remote Bluetooth Low
    * Energy Device.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Address of the Bluetooth Low Energy device of which to
    * query the connection handle.
    * @param Current_Connection_Parameters Pointer to a structure that will
    * receive the connection parameters for the connection to the specified device.
    * @return If this function returns success,
    * then the Current_Connection_Parameters structure will contain the
    * current LE Connection Parameters for the connection to the
    * specified Bluetooth Board Address.
    * \note @return If this function returns with an error, negative value,
    *          the value returned in the Current_Connection_Parameters
    *          structure should be considered invalid.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Query_Connection_Parameters(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_LE_Current_Connection_Parameters_t *Current_Connection_Parameters);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Query_Connection_Parameters_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_LE_Current_Connection_Parameters_t *Current_Connection_Parameters);
#endif

   /*! @brief The following function is provided to allow a means of creating a
    * new Long Term Key (LTK).
    * @param BluetoothStackID    Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param DHK Diversifying hiding key used as input to generate the long
    * term key (LTK).
    * @param ER  Encryption root key (ERK) used with the DHK to generate the
    * long term key (LTK).
    * @param LTK_Result  Pointer to a buffer that will receive the generated
    * long term key (LTK).
    * @param DIV_Result  Pointer to a buffer that will receive the diversifier
    * (DIV) that was used to generate the long term key (LTK).
    * @param EDIV_Result Pointer to a buffer that will receive the encrypted
    * diversifier (EDIV) that was used to generate the long term key (LTK).
    * @param Rand_Result Pointer to a buffer that will receive the random number
    * that was used to generate the long term key (LTK).
    * @return This function returns zero if succesfull or a negative
    * error code.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Generate_Long_Term_Key(unsigned int BluetoothStackID, Encryption_Key_t *DHK, Encryption_Key_t *ER, Long_Term_Key_t *LTK_Result, Word_t *DIV_Result, Word_t *EDIV_Result, Random_Number_t *Rand_Result);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Generate_Long_Term_Key_t)(unsigned int BluetoothStackID, Encryption_Key_t *DHK, Encryption_Key_t *ER, Long_Term_Key_t *LTK_Result, Word_t *DIV_Result, Word_t *EDIV_Result, Random_Number_t *Rand_Result);
#endif

   /*! @brief The following function is provided to allow a means of dynamically
    * regenerating a Long Term Key (LTK) that was previously generated
    * with a call to the GAP_LE_Generate_Long_Term_Key() function.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param DHK Diversifying hiding key used as input to re-generate the
    * long term key (LTK).
    * @param ER  Encryption root key (ERK) that will be used to re-generate
    * the long term key (LTK).
    * @param EDIV Encrypted diversifier (EDIV) that will be used to re-generate
    * the long term key (LTK).
    * @param Rand Random number that will be used to during the re-generation
    * process.
    * @param LTK_Result  Pointer to a buffer that will receive the generated
    * long term key (LTK).
    * @return This function returns zero if
    * succesfull or a negative error code.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Regenerate_Long_Term_Key(unsigned int BluetoothStackID, Encryption_Key_t *DHK, Encryption_Key_t *ER, Word_t EDIV, Random_Number_t *Rand, Long_Term_Key_t *LTK_Result);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Regenerate_Long_Term_Key_t)(unsigned int BluetoothStackID, Encryption_Key_t *DHK, Encryption_Key_t *ER, Word_t EDIV, Random_Number_t *Rand, Long_Term_Key_t *LTK_Result);
#endif

   /*! @brief The following function is provided to allow a means of performing
    * the Diversify Function, d1, as specified in Volume 3, Part H,
    * Section 5.2.2.1 of the Core specification.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param Key Encryption key used as input to the diversify function.
    * @param DIn D value used as input to the diversify function.
    * @param RIn R value used as input to the diversify function.
    * @param Result Pointer to a buffer that will receive the
    * generated encryption key.
    * @return This function returns zero if
    * succesfull or a negative error code.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Diversify_Function(unsigned int BluetoothStackID, Encryption_Key_t *Key, Word_t DIn, Word_t RIn, Encryption_Key_t *Result);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Diversify_Function_t)(unsigned int BluetoothStackID, Encryption_Key_t *Key, Word_t Din, Word_t Rin, Encryption_Key_t *Result);
#endif

   /*! @brief The following function is provided to allow a mechanism of
    * requesting a connection parameter update for an LE connection.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth
    * Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Device address of the connected device that the slave
    * is requesting the connection parameter update of.
    * @param Connection_Interval_Min Minimum requested connection interval.
    * @param Connection_Interval_Max Maximum requested connection interval.
    * @param Slave_Latency Requested slave latency.
    * @param Supervision_Timeout Requested supervision timeout.
    * @return This function returns zero if
    * successful or a negative error code.
    * \note Connection_Interval_Min, Connection_Interval_Max and
    *          Connection_Supervision_Timeout must be specified in
    *          milli-seconds.
    * \note Connection_Interval_Min must satisfy the following
    *          equation:
    *                Connection_Interval_Min >=
    *                      MINIMUM_MINIMUM_CONNECTION_INTERVAL
    *            AND
    *                Connection_Interval_Min <=
    *                      MAXIMUM_MINIMUM_CONNECTION_INTERVAL
    * \note Connection_Interval_Max must satisfy the following
    *          equation:
    *                Connection_Interval_Max >=
    *                      MINIMUM_MAXIMUM_CONNECTION_INTERVAL
    *            AND
    *                Connection_Interval_Max <=
    *                      MAXIMUM_MAXIMUM_CONNECTION_INTERVAL
    * \note Slave_Latency must satisfy the following equation:
    *                Slave_Latency >=
    *                      MINIMUM_SLAVE_LATENCY
    *           AND
    *                Slave_Latency <=
    *                      MAXIMUM_SLAVE_LATENCY
    * \note Connection_Supervision_Timeout must satisfy the
    *          following equation:
    *                Connection_Supervision_Timeout >=
    *                     MINIMUM_LINK_SUPERVISION_TIMEOUT
    *          AND
    *                Connection_Supervision_Timeout <=
    *                     MAXIMUM_LINK_SUPERVISION_TIMEOUT
    * \note This function is asynchronous in nature, the caller will
    *          receive an etLE_Connection_Parameter_Update_Response
    *          event through the GAP LE event callback installed for
    *          this connection when the process has completed.
    * \note This function can only be issued by the slave of the
    *          connection (to request the master to change the
    *          parameters).
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Connection_Parameter_Update_Request(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t Connection_Interval_Min, Word_t Connection_Interval_Max, Word_t Slave_Latency, Word_t Supervision_Timeout);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Connection_Parameter_Update_Request_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t Connection_Interval_Min, Word_t Connection_Interval_Max, Word_t Slave_Latency, Word_t Supervision_Timeout);
#endif

   /*! @brief The following function is provided to allow a means of either
    * accepting or rejecting a connection parameter update request that
    * was received from a remote device.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param BD_ADDR Device address of the connected device that the master is
    * responding to the connection parameter update of.
    * @param Accept Flag that specifies whether the slave requested parameters
    * were accepted.  If this value is FALSE then the next parameter is ignored.
    * If TRUE, the next parameter specifies the new connection parameters.
    * @param ConnectionParameters Specifies the new, accepted, connection parameters
    * of the connection to the remote device.
    * @return This function returns zero if successful or a negative error code.
    * \note If Accept is FALSE, then the first parameter to this
    *          function is not used and is ignored.
    * \note If Accept is TRUE, then the first parameter specifies
    *          the connection parameters that are to be:
    *             - Sent to the slave
    *             - Applied to the current connection
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Connection_Parameter_Update_Response(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Boolean_t Accept, GAP_LE_Connection_Parameters_t *ConnectionParameters);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Connection_Parameter_Update_Response_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Boolean_t Accept, GAP_LE_Connection_Parameters_t *ConnectionParameters);
#endif

   /*! @brief The following function is provided to allow a mechanism of
    * attempting to update the connection parameters for an LE
    * connection.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol
    * Stack via a call to BSC_Initialize.
    * @param BD_ADDR Device address of the connected device that the master is
    * responding to the connection parameter update of.
    * @param ConnectionParameters Specifies the new connection parameters to
    * attempt to apply to the connection to the remote device.
    * @return This function returns zero if successful or a negative error code.
    * \note This function can only be issued by the Master of the
    *          connection.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Update_Connection_Parameters(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_LE_Connection_Parameters_t *ConnectionParameters);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Update_Connection_Parameters_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_LE_Connection_Parameters_t *ConnectionParameters);
#endif

   /*! @brief The following function is provided to allow a means to set the
    * Authenticated Payload Timeout of a LE connection to a remote
    * Bluetooth Device.  This function accepts as it's input parameters
    * the Bluetooth Protocol Stack ID of the Bluetooth Protocol Stack
    * that the connection exists, the Bluetooth Board Address of the
    * Remote LE Bluetooth Device which is connected, and the
    * Authenticated Payload Timeout for the LE connection to the
    * specified Bluetooth Board Address.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Bluetooth address of the remote LE device to set the authenticated payload for.
    * @param AuthenticatedPayloadTimeout Value to set for the authenticated payload timeout for the specified connection.
    * @return This function will return zero
    * on success, or a negative return error code if there was an error.
    * \note If this function returns BTPS_ERROR_DEVICE_NOT_CONNECTED
    *          a connection to the specified Bluetooth Board Address
    *          does not exist.
    * \note This function can only be called if the local Bluetooth
    *          device is a 4.1 or greater chipset.
    * \note The AuthenticatedPayloadTimeout parameter is specified in
    *          milliseconds.
    * \note The AuthenticatedPayloadTimeout parameter must adhere to
    *          the following rule:
    *
    *             AuthenticatedPayloadTimeout >=
    *                       ConnectionInterval * (1 + SlaveLatency)
    *
    * \note If the Authenticated Payload Timeout is the time between
    *          valid packets received with MICs on an encrypted link.
    *          The etLE_Authenticated_Payload_Timeout_Expired event will
    *          be dispatched to the GAP LE event callback installed for
    *          this connection if the timeout expires with no valid MIC
    *          received from the remote device.
    * \note This function will only set the Authenticated Payload
    *          Timeout for LE connections.  For BR/EDR connections the
    *          GAP_Set_Authenticated_Payload_Timeout() function must be
    *          called.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Set_Authenticated_Payload_Timeout(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t AuthenticatedPayloadTimeout);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Set_Authenticated_Payload_Timeout_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t AuthenticatedPayloadTimeout);
#endif

   /*! @brief The following function is provided to allow a means to query the
    * Authenticated Payload Timeout of a LE connection to a remote
    * Bluetooth Device.  This function accepts as it's input parameters
    * the Bluetooth Protocol Stack ID of the Bluetooth Protocol Stack
    * that the connection exists, the Bluetooth Board Address of the
    * Remote LE Bluetooth Device which is connected, and a pointer to a
    * variable that will receive the Authenticated Payload Timeout for
    * the connection to the specified Bluetooth Board Address.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Bluetooth address of the remote LE device to query the authenticated payload for.
    * @param AuthenticatedPayloadTimeout Pointer to location to store the queried authenticated payload timeout.
    * @return This function will return zero on success, or a negative return error
    * code if there was an error.  If this function returns success,
    * then the Authenticated Payload Timeout variable will contain the
    * Authenticated Payload Timeout for the LE connection to the
    * specified Bluetooth Board Address.
    * \note If @return This function returns with an error, negative value,
    *          the value returned in the AuthenticatedPayloadTimeout
    *          variable should be considered invalid.
    * \note If @return This function returns BTPS_ERROR_DEVICE_NOT_CONNECTED
    *          a connection to the specified Bluetooth Board Address
    *          does not exist.
    * \note This function can only be called if the local Bluetooth
    *          device is a 4.1 or greater chipset.
    * \note The AuthenticatedPayloadTimeout parameter is returned in
    *          milliseconds.
    * \note This function will only query the Authenticated Payload
    *          Timeout for LE connections.  For BR/EDR connections the
    *          GAP_Query_Authenticated_Payload_Timeout() function must
    *          be called.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_Query_Authenticated_Payload_Timeout(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t *AuthenticatedPayloadTimeout);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Query_Authenticated_Payload_Timeout_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, Word_t *AuthenticatedPayloadTimeout);
#endif

   /*! @brief The following function is provided to allow the use of LE Secure
    * Connections (SC) pairing in Out Of Band (OOB) association method.
    * The upper layer will use this function to generate the the local
    * OOB random value, and OOB confirmation value (ra/rb and Ca/Cb) as
    * defined in the Bluetooth specification.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param OOB_Local_Rand_Result Pointer to a buffer that will receive the generated local OOB random value.
    * @param OOB_Local_Confirm_Result Pointer to a buffer that will receive the generated local OOB confirmation value.
    * @return This function returns zero if successful or a negative error code.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_SC_OOB_Generate_Parameters(unsigned int BluetoothStackID, SM_Random_Value_t *OOB_Local_Rand_Result, SM_Confirm_Value_t *OOB_Local_Confirm_Result); 

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_SC_OOB_Generate_Parameters_t)(unsigned int BluetoothStackID, SM_Random_Value_t *OOB_Local_Rand_Result, SM_Confirm_Value_t *OOB_Local_Confirm_Result);
#endif

   /*! @brief The following function is provided to allow a mechanism of sending
    * keypress notification to a remote device. The upper layer will use
    * this function during LE SC pairing, when using the passkey method.
    * Only a device with IO capabilities of keyboard-only may use this
    * command.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param BD_ADDR Device address of the connected device that the keypress notification will be send to.
    * @param Keypress_Notification_Type	Specifies the key press type. This value will be one of the following:
    * LEkpEntryStarted
    * LEkpDigitEntered
    * LEkpDigitErased
    * LEkpCleared
    * LEkpEntryCompleted
    * @return This function returns zero if successful or a negative error code.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_SC_Send_Keypress_Notification(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_LE_Keypress_t *Keypress_Notification_Type);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_Send_Keypress_Notification_t)(unsigned int BluetoothStackID, BD_ADDR_t BD_ADDR, GAP_LE_Keypress_t *Keypress_Notification_Type);
#endif

   /*! @brief The following function is provided to allow a configuration
    * of LE Secure Connecions only mode. The upper layer will use this
    * function before the begining of LE SC pairing, in case it asks to
    * reject a device that supports only legacy pairing.
    * This mode should be used when it is more important for a device
    * to have high security than it is for it to maintain backwards
    * compatibility with devices that do not support SC.
    * This function accepts as parameters the Bluetooth stack ID of the
    * Bluetooth device,and a boolean EnableSCOnly that enable or disable
    * the SC only mode. This function should be used ones,
    * before the first pairing process.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @param EnableSCOnly Flag that specifies whether the SC Only mode will be enabled, or disabled. If this value is TRUE then SC Only mode is enabled.  If FALSE, the SC Only mode is disabled.
    * @return This function returns zero if successful or a negative error code.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_SC_Only_Mode(unsigned int BluetoothStackID, Boolean_t EnableSCOnly);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_SC_Only_Mode)(unsigned int BluetoothStackID, Boolean_t EnableSCOnly);
#endif

   /*! @brief The following function is provided to allow a regeneration
    * of the P-256 private and local public keys.
    * This function is relevant only in case of LE Secure Connections.
    * It shall NOT be used in the middle of a pairing process.
    * @param BluetoothStackID Unique identifier assigned to this Bluetooth Protocol Stack via a call to BSC_Initialize.
    * @return This function returns zero if successful or a negative error code.
    */
BTPSAPI_DECLARATION int BTPSAPI GAP_LE_SC_Regenerate_P256_Local_Keys(unsigned int BluetoothStackID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_GAP_LE_SC_Regenerate_P256_Local_Keys)(unsigned int BluetoothStackID);
#endif

#endif
