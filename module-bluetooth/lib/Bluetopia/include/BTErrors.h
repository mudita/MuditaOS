/*****< bterrors.h >***********************************************************/
/*      Copyright 2000 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*      Copyright 2015 Texas Instruments Incorporated.                        */
/*      All Rights Reserved.                                                  */
/*									      									  */	
/*  BTERRORS - Stonestreet One Bluetooth Protocol Stack Error Codes.          */
/*                                                                            */
/*  Author:  Tim Thomas                                                       */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   12/21/00  T. Thomas      Initial creation.                               */
/*   12/07/15  L. Gersi	         Updates for BT 4.2 - LE SC		      		  */
/******************************************************************************/
#ifndef __BTERRORSH__
#define __BTERRORSH__

#define BTPS_ERROR_INVALID_PARAMETER                             (-1)
#define BTPS_ERROR_INVALID_BLUETOOTH_STACK_ID                    (-2)
#define BTPS_ERROR_STACK_INITIALIZATION_ERROR                    (-3)
#define BTPS_ERROR_HCI_INITIALIZATION_ERROR                      (-4)
#define BTPS_ERROR_GAP_INITIALIZATION_ERROR                      (-5)
#define BTPS_ERROR_SCO_INITIALIZATION_ERROR                      (-6)
#define BTPS_ERROR_L2CAP_INITIALIZATION_ERROR                    (-7)
#define BTPS_ERROR_RFCOMM_INITIALIZATION_ERROR                   (-8)
#define BTPS_ERROR_SDP_INITIALIZATION_ERROR                      (-9)
#define BTPS_ERROR_SPP_INITIALIZATION_ERROR                     (-10)
#define BTPS_ERROR_GOEP_INITIALIZATION_ERROR                    (-11)
#define BTPS_ERROR_OTP_INITIALIZATION_ERROR                     (-12)
#define BTPS_ERROR_DEBUG_CALLBACK_ALREADY_INSTALLED             (-13)
#define BTPS_ERROR_HCI_DRIVER_ERROR                             (-14)
#define BTPS_ERROR_DEVICE_RESET_ERROR                           (-15)
#define BTPS_ERROR_HCI_RESPONSE_ERROR                           (-16)
#define BTPS_ERROR_HCI_TIMEOUT_ERROR                            (-17)
#define BTPS_ERROR_UNSUPPORTED_HCI_VERSION                      (-18)
#define BTPS_ERROR_UNKNOWN_SUPPORTED_FEATURES                   (-19)
#define BTPS_ERROR_UNKNOWN_HCI_BUFFER_SIZE                      (-20)
#define BTPS_ERROR_UNABLE_TO_REGISTER_EVENT_CALLBACK            (-21)
#define BTPS_ERROR_UNABLE_TO_REGISTER_ACL_CALLBACK              (-22)
#define BTPS_ERROR_UNABLE_TO_REGISTER_SCO_CALLBACK              (-23)
#define BTPS_ERROR_SIGNALLING_MTU_EXCEEDED                      (-24)
#define BTPS_ERROR_UNABLE_TO_REGISTER_PSM                       (-25)
#define BTPS_ERROR_L2CAP_NOT_INITIALIZED                        (-26)
#define BTPS_ERROR_UNABLE_TO_UNREGISTER_PSM                     (-27)
#define BTPS_ERROR_PSM_NOT_REGISTERED                           (-28)
#define BTPS_ERROR_ATTEMPTING_CONNECTION_TO_DEVICE              (-29)
#define BTPS_ERROR_ACCEPTING_CONNECTION_FROM_DEVICE             (-30)
#define BTPS_ERROR_INVALID_FLUSH_TIMEOUT_VALUE                  (-31)
#define BTPS_ERROR_INVALID_STATE_FOR_CONFIG                     (-32)
#define BTPS_ERROR_ADDING_CID_INFORMATION                       (-33)
#define BTPS_ERROR_ADDING_CONNECTION_INFORMATION                (-34)
#define BTPS_ERROR_ADDING_IDENTIFIER_INFORMATION                (-35)
#define BTPS_ERROR_INVALID_CONNECTION_STATE                     (-36)
#define BTPS_ERROR_CHANNEL_NOT_IN_OPEN_STATE                    (-37)
#define BTPS_ERROR_INVALID_CID                                  (-38)
#define BTPS_ERROR_WRITING_DATA_TO_DEVICE                       (-39)
#define BTPS_ERROR_MEMORY_ALLOCATION_ERROR                      (-40)
#define BTPS_ERROR_NEGOTIATED_MTU_EXCEEDED                      (-41)
#define BTPS_ERROR_CONECTIONLESS_MTU_EXCEEDED                   (-42)
#define BTPS_ERROR_CID_NOT_GROUP_CID                            (-43)
#define BTPS_ERROR_GROUP_MEMBER_ALREADY_EXISTS                  (-44)
#define BTPS_ERROR_GROUP_MEMBER_NOT_FOUND                       (-45)
#define BTPS_ERROR_CONNECTION_TO_DEVICE_LOST                    (-46)
#define BTPS_ERROR_INVALID_CID_TYPE                             (-47)
#define BTPS_ERROR_SDP_DATA_ELEMENT_EXPECTED                    (-48)
#define BTPS_ERROR_SDP_INVALID_DATA_ELEMENT_LENGTH              (-49)
#define BTPS_ERROR_SDP_NOT_INITIALIZED                          (-50)
#define BTPS_ERROR_SDP_INVALID_DATA_ELEMENT                     (-51)
#define BTPS_ERROR_ADDING_SERVICE_ATTRIBUTE                     (-52)
#define BTPS_ERROR_DELETING_SERVICE_RECORD                      (-53)
#define BTPS_ERROR_EXPECTED_UUID_ENTRY                          (-54)
#define BTPS_ERROR_SDP_INVALID_DATA_TYPE                        (-55)
#define BTPS_ERROR_GAP_NOT_INITIALIZED                          (-56)
#define BTPS_ERROR_DEVICE_HCI_ERROR                             (-57)
#define BTPS_ERROR_INVALID_MODE                                 (-58)
#define BTPS_ERROR_ADDING_CALLBACK_INFORMATION                  (-59)
#define BTPS_ERROR_DELETING_CALLBACK_INFORMATION                (-60)
#define BTPS_ERROR_NO_CALLBACK_REGISTERED                       (-61)
#define BTPS_ERROR_SCO_NOT_INITIALIZED                          (-62)
#define BTPS_ERROR_MAX_SCO_CONNECTIONS                          (-63)
#define BTPS_ERROR_INTERNAL_ERROR                               (-64)
#define BTPS_ERROR_INSUFFICIENT_BUFFER_SPACE                    (-65)
#define BTPS_ERROR_INSUFFICIENT_RESOURCES                       (-66)
#define BTPS_ERROR_RFCOMM_NOT_INITIALIZED                       (-67)
#define BTPS_ERROR_RFCOMM_ADDING_SERVER_INFORMATION             (-68)
#define BTPS_ERROR_RFCOMM_REMOVING_SERVER_INFORMATION           (-69)
#define BTPS_ERROR_RFCOMM_UNABLE_TO_ADD_CONNECTION_INFORMATION  (-70)
#define BTPS_ERROR_RFCOMM_UNABLE_TO_ADD_CHANNEL_INFORMATION     (-71)
#define BTPS_ERROR_RFCOMM_UNABLE_TO_CONNECT_TO_REMOTE_DEVICE    (-72)
#define BTPS_ERROR_RFCOMM_UNABLE_TO_COMMUNICATE_WITH_REMOTE_DEVICE (-73)
#define BTPS_ERROR_RFCOMM_INVALID_TEI                           (-74)
#define BTPS_ERROR_RFCOMM_INVALID_DLCI                          (-75)
#define BTPS_ERROR_RFCOMM_DISC_ALREADY_PENDING                  (-76)
#define BTPS_ERROR_RFCOMM_TEI_IS_DISCONNECTING                  (-77)
#define BTPS_ERROR_RFCOMM_CONTROL_MESSAGE_CURRENTLY_PENDING     (-78)
#define BTPS_ERROR_RFCOMM_FLOW_IS_DISABLED                      (-79)
#define BTPS_ERROR_RFCOMM_INVALID_MAX_FRAME_SIZE                (-80)
#define BTPS_ERROR_RFCOMM_COMMAND_NOT_ALLOWED                   (-81)
#define BTPS_ERROR_RFCOMM_ADDING_MESSAGE_INFORMATION            (-82)
#define BTPS_ERROR_RFCOMM_INVALID_FLOW_STATE                    (-83)
#define BTPS_ERROR_RFCOMM_MAX_FRAME_SIZE_EXCEEDED               (-84)
#define BTPS_ERROR_SPP_NOT_INITIALIZED                          (-85)
#define BTPS_ERROR_SPP_PORT_NOT_OPENED                          (-86)
#define BTPS_ERROR_SPP_BUFFER_FULL                              (-87)
#define BTPS_ERROR_OUTSTANDING_TRANSACTION                      (-88)
#define BTPS_ERROR_TIMER_VALUE_OUT_OF_RANGE                     (-89)
#define BTPS_ERROR_GOEP_NOT_INITIALIZED                         (-90)
#define BTPS_ERROR_GOEP_COMMAND_NOT_ALLOWED                     (-91)
#define BTPS_ERROR_OTP_NOT_INITIALIZED                          (-92)
#define BTPS_ERROR_OTP_REQUEST_OUTSTANDING                      (-93)
#define BTPS_ERROR_OTP_ERROR_PARSING_DATA                       (-94)
#define BTPS_ERROR_OTP_ALREADY_CONNECTED                        (-95)
#define BTPS_ERROR_OTP_NO_CONNECTION                            (-96)
#define BTPS_ERROR_OTP_ACTION_NOT_ALLOWED                       (-97)
#define BTPS_ERROR_DEVICE_NOT_CONNECTED                         (-98)
#define BTPS_ERROR_ACTION_NOT_ALLOWED                           (-99)
#define BTPS_ERROR_SPP_BUFFER_EMPTY                             (-100)
#define BTPS_ERROR_VS_HCI_ERROR                                 (-101)
#define BTPS_ERROR_ALREADY_OUTSTANDING                          (-102)
#define BTPS_ERROR_FEATURE_NOT_AVAILABLE                        (-103)
#define BTPS_ERROR_LOCAL_CONTROLLER_DOES_NOT_SUPPORT_LE         (-104)
#define BTPS_ERROR_SCAN_ACTIVE                                  (-105)
#define BTPS_ERROR_SLAVE_CONNECTION_PRESENT                     (-106)
#define BTPS_ERROR_INVALID_DEVICE_ROLE_MODE                     (-107)
#define BTPS_ERROR_DEVICE_IS_SLAVE                              (-108)
#define BTPS_ERROR_INVALID_CONNECTION_HANDLE                    (-109)
#define BTPS_ERROR_READ_REMOTE_FEATURES_OUTSTANDING             (-110)
#define BTPS_ERROR_CREATE_CONNECTION_OUTSTANDING                (-111)
#define BTPS_ERROR_INVALID_CONNECTION_PARAMETERS                (-112)
#define BTPS_ERROR_WHITE_LIST_SIZE_EXCEEDED                     (-113)
#define BTPS_ERROR_WHITE_LIST_IN_USE                            (-114)
#define BTPS_ERROR_INVALID_RANDOM_ADDRESS                       (-115)
#define BTPS_ERROR_RANDOM_ADDRESS_IN_USE                        (-116)
#define BTPS_ERROR_PAIRING_ACTIVE                               (-117)
#define BTPS_ERROR_PAIRING_NOT_ACTIVE                           (-118)
#define BTPS_ERROR_INVALID_STATE                                (-119)
#define BTPS_ERROR_FEATURE_NOT_CURRENTLY_ACTIVE                 (-120)
#define BTPS_ERROR_CONNECTION_UPDATE_PROCEDURE_OUTSTANDING      (-121)
#define BTPS_ERROR_NUMERIC_COMPARISON_FAILED                	(-122)

   /* The following defines the start of the HCI Status Code Error Base.*/
   /* Errors between (-500) -> (-755) are errors returned due to a Error*/
   /* Code between returned by the Bluetooth Controller.                */
#define BTPS_ERROR_CODE_HCI_STATUS_BASE                         (-500)

   /* The following MACRO is a utility MACRO that provides a mechanism  */
   /* for the return value in the range -500->-755 to be converted to an*/
   /* HCI Controller Status Code.  This MACRO accepts as it's only      */
   /* parameter the HCI Error Code to convert and returns the HCI Status*/
   /* Code.                                                             */
#define CONVERT_ERROR_CODE_TO_HCI_STATUS_CODE(_x)               ((Byte_t)(-((_x) - BTPS_ERROR_CODE_HCI_STATUS_BASE)))

#endif
