/*****< hcidrv.h >*************************************************************/
/*      Copyright 2000 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  HCIDRV - HCI Packet Driver Implementation for Generic Bluetooth Stack.    */
/*                                                                            */
/*  Author:  Damon Lange                                                      */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   07/26/00  D. Lange       Initial creation.                               */
/******************************************************************************/
#ifndef __HCIDRVH__
#define __HCIDRVH__

#include "BTAPITyp.h"           /* Bluetooth API Type Definitions.            */
#include "HCITypes.h"           /* Generic Bluetooth HCI Types/Constants.     */

   /* Error Return Codes.                                               */
#define HCI_DRIVER_ERROR_INVALID_DRIVER_INFORMATION (-1)/* Error that denotes */
                                                        /* that the specified */
                                                        /* Driver Information */
                                                        /* Structure is       */
                                                        /* invalid.           */

#define HCI_DRIVER_ERROR_UNSUPPORTED_DRIVER_TYPE (-2)   /* Error that denotes */
                                                        /* that the specified */
                                                        /* Driver Information */
                                                        /* Structure is       */
                                                        /* invalid (Driver    */
                                                        /* Type is NOT        */
                                                        /* supported by this  */
                                                        /* HCI Driver Library.*/

#define HCI_DRIVER_ERROR_INVALID_CALLBACK_INFORMATION (-3)/* Error that       */
                                                        /* denotes that the   */
                                                        /* specified Callback */
                                                        /* function pointer is*/
                                                        /* invalid.           */

#define HCI_DRIVER_ERROR_INITIALIZING_DRIVER    (-4)    /* Error that denotes */
                                                        /* that an error      */
                                                        /* occurred during    */
                                                        /* Driver             */
                                                        /* Configuration.     */

#define HCI_DRIVER_ERROR_INVALID_DRIVER_ID      (-5)    /* Error that denotes */
                                                        /* Driver ID supplied */
                                                        /* as function        */
                                                        /* argument is NOT    */
                                                        /* registered with    */
                                                        /* the Library.       */

#define HCI_DRIVER_ERROR_INVALID_CALLBACK_ID    (-6)    /* Error that denotes */
                                                        /* Callback ID        */
                                                        /* supplied as        */
                                                        /* function argument  */
                                                        /* is NOT valid.      */

#define HCI_DRIVER_ERROR_INVALID_DATA           (-7)    /* Error that denotes */
                                                        /* the specified Data */
                                                        /* to be sent was     */
                                                        /* invalid.           */

#define HCI_DRIVER_ERROR_INVALID_PACKET_ID      (-8)    /* Error that denotes */
                                                        /* HCI Packet ID      */
                                                        /* supplied as        */
                                                        /* function argument  */
                                                        /* is NOT valid.      */

#define HCI_DRIVER_ERROR_INVALID_SCO_CONFIGURATION (-9) /* Error that denotes */
                                                        /* HCI Driver SCO     */
                                                        /* Configuration      */
                                                        /* supplied as        */
                                                        /* function argument  */
                                                        /* is NOT valid.      */

#define HCI_DRIVER_ERROR_UNKNOWN                (-10)   /* Error that is not  */
                                                        /* covered by any     */
                                                        /* other Error Code.  */

#define HCI_DRIVER_ERROR_UNIMPLEMENTED          (-11)   /* Error that denotes */
                                                        /* the specified      */
                                                        /* function or        */
                                                        /* operation is not   */
                                                        /* implemented.       */

   /* The following declared type represents the Prototype Function for */
   /* an HCI Packet Driver Receive Data Callback.  This function will be*/
   /* called whenever a complete HCI Packet has been received by the HCI*/
   /* Driver that was opened with the specified Driver ID.  This        */
   /* function passes to the caller the HCI Driver ID, the Packet that  */
   /* was received and the HCI Driver Callback Parameter that was       */
   /* specified when this Callback was installed.  The caller is free to*/
   /* use the contents of the HCI Packet ONLY in the context of this    */
   /* callback.  If the caller requires the Data for a longer period of */
   /* time, then the callback function MUST copy the data into another  */
   /* Data Buffer.  This function is guaranteed NOT to be invoked more  */
   /* than once simultaneously for the specified installed callback     */
   /* (i.e.  this function DOES NOT have be reentrant).  It Needs to be */
   /* noted however, that if the same Callback function is installed for*/
   /* different Callback Types, then the function IS NOT guaranteed to  */
   /* be NOT be called simultaneously.  The function is only guaranteed */
   /* to be called serially for the context in which the Callback has   */
   /* been installed.  This means that if the same Callback function is */
   /* installed for both ACL Data and HCI Event Packets, then the       */
   /* function COULD be called simultaneously for both Packet Types, but*/
   /* NOT simultaneously for HCI Event Packets OR ACL Data Packets.  It */
   /* should also be noted that this function is called in the Thread   */
   /* Context of a Thread that the User does NOT own.  Therefore,       */
   /* processing in this function should be as efficient as possible    */
   /* (this argument holds anyway because another Packet will not be    */
   /* received while this function call is outstanding).                */
   /* ** NOTE ** This function MUST NOT Block and wait for events that  */
   /*            can only be satisfied by Receiving HCI Data Packets.  A*/
   /*            Deadlock WILL occur because NO Other Receive HCI       */
   /*            Callbacks will be issued while this function is        */
   /*            currently outstanding.                                 */
typedef void (BTPSAPI *HCI_DriverCallback_t)(unsigned int HCIDriverID, HCI_Packet_t *HCIPacket, unsigned long CallbackParameter);

   /* The following declared type represents the Prototype Function for */
   /* an HCI Packet Driver Debug Data Callback.  This function will be  */
   /* called whenever a complete HCI Packet has been sent or received by*/
   /* the HCI Driver that was opened with the specified Driver ID.  This*/
   /* function passes to the caller the HCI Driver ID, the Packet that  */
   /* was received and the HCI Driver Callback Parameter that was       */
   /* specified when this Callback was installed.  The caller is free to*/
   /* use the contents of the HCI Packet ONLY in the context of this    */
   /* callback.  If the caller requires the Data for a longer period of */
   /* time, then the callback function MUST copy the data into another  */
   /* Data Buffer.  This function is guaranteed NOT to be invoked more  */
   /* than once simultaneously for the specified installed callback     */
   /* (i.e.  this function DOES NOT have be reentrant).  It Needs to be */
   /* noted however, that if the same Callback function is installed for*/
   /* different Callback Types, then the function IS NOT guaranteed to  */
   /* be NOT be called simultaneously.  The function is only guaranteed */
   /* to be called serially for the context in which the Callback has   */
   /* been installed.  It should also be noted that this function is    */
   /* called in the Thread Context of a Thread that the User does NOT   */
   /* own.  Therefore, processing in this function should be as         */
   /* efficient as possible (this argument holds anyway because Packet  */
   /* Processing (Sending/Receiving) will be suspended while function   */
   /* call is outstanding).                                             */
   /* ** NOTE ** THIS FUNCTION MUST NOT CALL ANY HCI DRIVER FUNCTIONS!! */
   /*            FAILURE TO FOLLOW THIS GUIDELINE WILL RESULT IN        */
   /*            POTENTIAL DEADLOCKS AND/OR ERRATIC BEHAVIOR !!!!!!!!!! */
   /*            The Debug Packet Callback is a VERY LOW LEVEL Callback */
   /*            and as such, does NOT allow the HCI Driver go be       */
   /*            re-entrant !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
typedef void (BTPSAPI *HCI_DebugPacketCallback_t)(unsigned int HCIDriverID, Boolean_t PacketSent, HCI_Packet_t *HCIPacket, unsigned long CallbackParameter);

   /* The following function is responsible for Opening an HCI Packet   */
   /* Driver of the specified Type.  The parameter to this function     */
   /* *MUST* be specified.  If this function is successful, the caller  */
   /* will receive a non-zero, non-negative return value which serves as*/
   /* the HCIDriverID parameter for all other functions in the HCI      */
   /* Packet Driver.  Since all HCI Packet Driver functions require a   */
   /* valid HCI Driver ID, this function must be called successfully    */
   /* before any other function can be called.  If this function fails, */
   /* then the return value is a negative error code (see error codes   */
   /* above).                                                           */
BTPSAPI_DECLARATION int BTPSAPI HCI_OpenDriver(HCI_DriverInformation_t *DriverInformation);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_OpenDriver_t)(HCI_DriverInformation_t *HCI_DriverInformation);
#endif

   /* The following function is responsible for reconfiguring the       */
   /* currently active HCI Driver.  The Input parameter to this function*/
   /* MUST have been acquired by a successful call to HCI_OpenDriver(). */
   /* This function accepts the HCI Driver ID of the driver to          */
   /* reconfigured, followed by a BOOLEAN which specifies whether or not*/
   /* any internal HCI Driver state machines should be reset (for       */
   /* example BCSP and/or Packet building state machines).              */
BTPSAPI_DECLARATION int BTPSAPI HCI_ReconfigureDriver(unsigned int HCIDriverID, Boolean_t ResetStateMachines, HCI_Driver_Reconfigure_Data_t *DriverReconfigureData);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_ReconfigureDriver_t)(unsigned int HCIDriverID, Boolean_t ResetStateMachines, HCI_Driver_Reconfigure_Data_t *DriverReconfigureData);
#endif

   /* The following function is responsible for Closing the HCI Packet  */
   /* Driver that was opened via a successful call to the               */
   /* HCI_OpenDriver() function.  The Input parameter to this function  */
   /* MUST have been acquired by a successful call to HCI_OpenDriver(). */
   /* Once this function completes, the HCI Packet Driver that was      */
   /* closed cannot be accessed again (sending/receiving HCI data) by   */
   /* this module until the Driver is Re-Opened by calling the          */
   /* HCI_OpenDriver() function.                                        */
BTPSAPI_DECLARATION void BTPSAPI HCI_CloseDriver(unsigned int HCIDriverID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_CloseDriver_t)(unsigned int HCIDriverID);
#endif

   /* The following function is provided to allow a mechanism for       */
   /* modules to force the processing of incoming HCI Data.             */
   /* * NOTE * This function is only applicable in device stacks that   */
   /*          are non-threaded.  This function has no effect for device*/
   /*          stacks that are operating in threaded environments.      */
BTPSAPI_DECLARATION void BTPSAPI HCI_ProcessHCIDriverInformation(unsigned int HCIDriverID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (BTPSAPI *PFN_HCI_ProcessHCIDriverInformation_t)(unsigned int HCIDriverID);
#endif

   /* The following function is responsible for Registering an HCI      */
   /* Driver Callback function that will be called when an HCI Event    */
   /* Packet is received.  The first parameter is obtained by a         */
   /* successful call to the HCI_OpenDriver() function.  The second     */
   /* parameter is a pointer to a function that is to be called when an */
   /* HCI Event Packet is received on the specified HCI Packet Driver   */
   /* (specified by the first parameter).  The CallbackParameter        */
   /* Parameter is a User defined value that will passed to the Callback*/
   /* Parameter when it is called.  The return value of this function   */
   /* will be a non-zero, non-negative value on success or a negative   */
   /* return code if there was a failure.  On success the value         */
   /* represents an HCI Callback ID that can be used in a call to the   */
   /* HCI_UnRegisterCallback() function to remove the Callback from the */
   /* HCI Packet Driver System.                                         */
BTPSAPI_DECLARATION int BTPSAPI HCI_RegisterEventCallback(unsigned int HCIDriverID, HCI_DriverCallback_t EventCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_RegisterEventCallback_t)(unsigned int HCIDriverID, HCI_DriverCallback_t EventCallback, unsigned long CallbackParameter);
#endif

   /* The following function is responsible for Registering an HCI      */
   /* Driver Callback function that will be called when an HCI ACL Data */
   /* Packet is received.  The first parameter is obtained by a         */
   /* successful call to the HCI_OpenDriver() function.  The second     */
   /* parameter is a pointer to a function that is to be called when an */
   /* HCI Event Packet is received on the specified HCI Packet Driver   */
   /* (specified by the first parameter).  The CallbackParameter        */
   /* Parameter is a User defined value that will passed to the Callback*/
   /* Parameter when it is called.  The return value of this function   */
   /* will be a non-zero, non-negative value on success or a negative   */
   /* return code if there was a failure.  On success the value         */
   /* represents an HCI Callback ID that can be used in a call to the   */
   /* HCI_UnRegisterCallback() function to remove the Callback from the */
   /* HCI Packet Driver System.                                         */
BTPSAPI_DECLARATION int BTPSAPI HCI_RegisterACLDataCallback(unsigned int HCIDriverID, HCI_DriverCallback_t ACLDataCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_RegisterACLDataCallback_t)(unsigned int HCIDriverID, HCI_DriverCallback_t ACLDataCallback, unsigned long CallbackParameter);
#endif

   /* The following function is responsible for Registering an HCI      */
   /* Driver Callback function that will be called when an HCI SCO Data */
   /* Packet is received.  The first parameter is obtained by a         */
   /* successful call to the HCI_OpenDriver() function.  The second     */
   /* parameter is a pointer to a function that is to be called when an */
   /* HCI Event Packet is received on the specified HCI Packet Driver   */
   /* (specified by the first parameter).  The CallbackParameter        */
   /* Parameter is a User defined value that will passed to the Callback*/
   /* Parameter when it is called.  The return value of this function   */
   /* will be a non-zero, non-negative value on success or a negative   */
   /* return code if there was a failure.  On success the value         */
   /* represents an HCI Callback ID that can be used in a call to the   */
   /* HCI_UnRegisterCallback() function to remove the Callback from the */
   /* HCI Packet Driver System.                                         */
BTPSAPI_DECLARATION int BTPSAPI HCI_RegisterSCODataCallback(unsigned int HCIDriverID, HCI_DriverCallback_t SCODataCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_RegisterSCODataCallback_t)(unsigned int HCIDriverID, HCI_DriverCallback_t SCODataCallback, unsigned long CallbackParameter);
#endif

   /* The following function is provided to allow an application a means*/
   /* of removing a previously installed HCI Packet Driver Callback     */
   /* function from the system.  The first parameter of this function   */
   /* specifies the HCI Driver to remove the Callback Function from.    */
   /* The first parameter is obtained via a successful call to the      */
   /* HCI_OpenDriver() function.  The second parameter specifies the    */
   /* Callback Function to delete.  This value is a Callback ID that was*/
   /* returned when a specified Callback was Registered.  Callbacks can */
   /* only be registered with the following functions: - The            */
   /* HCI_RegisterEventCallback() function.  - The                      */
   /* HCI_RegisterACLDataCallback() function.  - The                    */
   /* HCI_RegisterSCODataCallback() function.  - The                    */
   /* HCI_RegisterDebugCallback() function.  After this function is     */
   /* called, the previously installed Callback function will NOT be    */
   /* called when HCI Packet Data is received (unless the function has  */
   /* been registered more than once, in which case the function will   */
   /* have another Callback ID).                                        */
BTPSAPI_DECLARATION void BTPSAPI HCI_UnRegisterCallback(unsigned int HCIDriverID, unsigned int CallbackID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (BTPSAPI *PFN_HCI_UnRegisterCallback_t)(unsigned int HCIDriverID, unsigned int CallbackID);
#endif

   /* The following function is responsible for actually sending an HCI */
   /* Packet through the Opened HCI Driver.  This function accepts as   */
   /* input the HCI Driver to send the specified HCI Packet through and */
   /* the HCI Packet to send.  The last two parameters are valid ONLY   */
   /* for HCI Command Packets and are ignored for the other types of HCI*/
   /* Packets that are sent through this function.  These parameters    */
   /* provide a means for the caller to specify a Callback function that*/
   /* will be called when an HCI Event Response (only for the specified */
   /* HCI Command Type) is received.  This will allow the caller a means*/
   /* of providing the synchronous sending of HCI Command Data          */
   /* (possibly).  This mechanism is not required, and if the Callback  */
   /* Function specified is NULL, then this mechanism is not utilized.  */
   /* This function returns a non-zero, non-negative value on success,  */
   /* or a negative return code if there was an error (see error codes  */
   /* above).  A successful call to this function will yield an HCI     */
   /* Packet ID that can be passed to the HCI_CancelPacket() function to*/
   /* possibly Cancel the Packet from being sent (at some future time). */
   /* * NOTE * This function does NOT immediately send the Data to the  */
   /*          specified HCI Driver Device.  The HCI Driver can possibly*/
   /*          Queue the Packet to send at a future time (based upon the*/
   /*          state of the Bluetooth Device and the type of HCI Packet */
   /*          Data to be sent).  An example where the HCI Packet Driver*/
   /*          would Queue an HCI Packet would be an HCI Command Packet */
   /*          if the Bluetooth Device informed the HCI Packet Driver   */
   /*          that it can no longer accept HCI Command Packets.  The   */
   /*          HCI Command would then be De-Queued when the Bluetooth   */
   /*          Device informed the HCI Packet Driver that it can accept */
   /*          HCI Command Data.                                        */
BTPSAPI_DECLARATION int BTPSAPI HCI_SendPacket(unsigned int HCIDriverID, HCI_Packet_t *HCIPacket, HCI_DriverCallback_t SendPacketCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_SendPacket_t)(unsigned int HCIDriverID, HCI_Packet_t *HCIPacket, HCI_DriverCallback_t SendPacketCallback, unsigned long CallbackParameter);
#endif

   /* The following function is provided to allow a means to possibly   */
   /* cancel a Queued HCI Packet.  The first parameter to this function */
   /* is the HCI Driver ID which represents the Bluetooth Device to     */
   /* which the specified HCI Packet (Parameter two, obtained via a     */
   /* successful call to HCI_SendPacket().  This function returns a     */
   /* non-zero, non-negative value if the specified HCI Packet was      */
   /* cancelled (this means it was NEVER sent), or a negative return    */
   /* error code.  It should be noted that one of the error codes that  */
   /* this function returns specifies that the HCI Packet could NOT be  */
   /* Cancelled.  This means that either there was NO Packet with the   */
   /* specified HCI Packet ID, or that the specified HCI Packet was     */
   /* already Cancelled or Sent.                                        */
BTPSAPI_DECLARATION int BTPSAPI HCI_CancelPacket(unsigned int HCIDriverID, unsigned int HCIPacketID);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_CancelPacket_t)(unsigned int HCIDriverID, unsigned int HCIPacketID);
#endif

   /* The following function is responsible for configuring a specific  */
   /* HCI Driver with a specific SCO Configuration.  Only certain types */
   /* of HCI Interfaces support this type of configuration (USB is among*/
   /* them at the present time).  This function accepts as input the HCI*/
   /* Driver ID to change the SCO Configuration of, and the SCO         */
   /* Configuration to change to.  This function returns a zero return  */
   /* value if successful, or a negative return value if there is an    */
   /* error (see Error codes above).                                    */
BTPSAPI_DECLARATION int BTPSAPI HCI_ChangeSCOConfiguration(unsigned int HCIDriverID, HCI_SCOConfiguration_t HCISCOConfiguration);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_ChangeSCOConfiguration_t)(unsigned int HCIDriverID, HCI_SCOConfiguration_t HCISCOConfiguration);
#endif

   /* The following function is responsible for configuring a specific  */
   /* HCI Driver with a specific set of SCO connection handles.  This   */
   /* function accepts as input the HCI Driver ID to change the SCO     */
   /* configuration of, and the number of current SCO handles and a     */
   /* pointer to a list of SCO handles.  This function returns a zero   */
   /* return value if successful, or a negative return value if there is*/
   /* an error (see Error codes above).                                 */
BTPSAPI_DECLARATION int BTPSAPI HCI_ChangeSCOConfigurationHandles(unsigned int HCIDriverID, unsigned int NumberHandles, Word_t *ConnectionHandleList);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_ChangeSCOConfigurationHandles_t)(unsigned int HCIDriverID, unsigned int NumberHandles, Word_t *ConnectionHandleList);
#endif

   /* The following function is a Debugging function that allows the    */
   /* caller to register a Debugging Callback that will be called EACH  */
   /* Time a Packet is Sent or Received.  Because this Function will be */
   /* called every time a Packet is Sent or Received, this function     */
   /* should only be used when debugging is required because of the     */
   /* Performance Penalty that is present when using this mechanism.    */
   /* The first parameter is obtained by a successful call to the       */
   /* HCI_OpenDriver() function.  The second parameter is a pointer to a*/
   /* function that is to be called when an HCI Packet (of any type) is */
   /* Sent OR Received on the specified HCI Packet Driver (specified by */
   /* the first parameter).  The CallbackParameter Parameter is a User  */
   /* defined value that will passed to the Callback Parameter when it  */
   /* is called.  The return value of this function will be a non-zero, */
   /* non-negative value on success or a negative return code if there  */
   /* was a failure.  On success the value represents an HCI Callback ID*/
   /* that can be used in a call to the HCI_UnRegisterCallback()        */
   /* function to remove the Debugging Callback from the from the HCI   */
   /* Packet Driver System.                                             */
BTPSAPI_DECLARATION int BTPSAPI HCI_RegisterDebugPacketCallback(unsigned int HCIDriverID, HCI_DebugPacketCallback_t HCI_DebugPacketCallback, unsigned long CallbackParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_HCI_RegisterDebugPacketCallback_t)(unsigned int HCIDriverID, HCI_DebugPacketCallback_t HCI_DebugPacketCallback, unsigned long CallbackParameter);
#endif

#endif
