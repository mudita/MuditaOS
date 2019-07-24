/*****< l2captyp.h >***********************************************************/
/*      Copyright 2000 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  L2CAPTYP - Bluetooth L2CAP Type Definitions/Constants.                    */
/*                                                                            */
/*  Author:  Tim Thomas                                                       */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   09/11/00  T. Thomas      Initial creation.                               */
/******************************************************************************/
#ifndef __L2CAPTYPH__
#define __L2CAPTYPH__

#include "BTTypes.h"            /* Bluetooth Type Definitions.                */

   /* The following Constants represent the defined Bluetooth L2CAP     */
   /* Channel Identifier Values.                                        */
#define L2CAP_CHANNEL_IDENTIFIER_NULL_IDENTIFIER                        0x0000
#define L2CAP_CHANNEL_IDENTIFIER_SIGNALLING_CHANNEL                     0x0001
#define L2CAP_CHANNEL_IDENTIFIER_CONNECTIONLESS_CHANNEL                 0x0002
#define L2CAP_CHANNEL_IDENTIFIER_AMP_MANAGER                            0x0003
#define L2CAP_CHANNEL_IDENTIFIER_ATT_PROTOCOL_CHANNEL                   0x0004
#define L2CAP_CHANNEL_IDENTIFIER_LE_SIGNALLING_CHANNEL                  0x0005
#define L2CAP_CHANNEL_IDENTIFIER_SECURITY_MANAGER_CHANNEL               0x0006

#define L2CAP_CHANNEL_IDENTIFIER_MINIMUM_CHANNEL_IDENTIFIER             0x0040
#define L2CAP_CHANNEL_IDENTIFIER_MAXIMUM_CHANNEL_IDENTIFIER             0xFFFF

#define L2CAP_CHANNEL_IDENTIFIER_MINIMUM_LE_CHANNEL_IDENTIFIER          0x0040
#define L2CAP_CHANNEL_IDENTIFIER_MAXIMUM_LE_CHANNEL_IDENTIFIER          0x007F

   /* The following Constants represent the defined Bluetooth L2CAP PSM */
   /* Minimum and Maximum Values.                                       */
#define L2CAP_PSM_MINIMUM_PSM                                           0x0001
#define L2CAP_PSM_MAXIMUM_PSM                                           0xFEFF

   /* The following Constants represent the defined Bluetooth L2CAP LE  */
   /* PSM Minimum and Maximum Values.                                   */
#define L2CAP_LE_PSM_MINIMUM_PSM                                        0x0001
#define L2CAP_LE_PSM_MAXIMUM_PSM                                        0x00FF

   /* The following Constant represents an Invalid PSM value that can   */
   /* be used by the programmer to initialize an invalid PSM value.     */
#define L2CAP_PSM_INVALID_PSM                                           0x0000

   /* The following constant represents the maximum credit count for a  */
   /* connection orieneted channel using LE Credit Based Flow Control.  */
#define L2CAP_MAXIMUM_LE_CREDIT_COUNT                                   65535

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability of testing whether or not a specified PSM value is    */
   /* valid.  The first parameter to this MACRO is the PSM Value to     */
   /* verify.  This MACRO returns a boolean value based upon whether    */
   /* or not the specified PSM value is valid.  This MACRO returns a    */
   /* boolean TRUE if the specified PSM is valid, or a boolean FALSE    */
   /* if the specified PSM value is invalid.                            */
#define L2CAP_PSM_VALID_PSM(_x)                         (((_x) & 0x0001) && (!((_x) & 0x0100)) && ((_x) >= L2CAP_PSM_MINIMUM_PSM) && ((_x) <= L2CAP_PSM_MAXIMUM_PSM))

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability of testing whether or not a specified LE PSM value is */
   /* valid.  The first parameter to this MACRO is the LE PSM Value to  */
   /* verify.  This MACRO returns a boolean value based upon whether or */
   /* not the specified LE PSM value is valid.  This MACRO returns a    */
   /* boolean TRUE if the specified LE PSM is valid, or a boolean FALSE */
   /* if the specified LE PSM value is invalid.                         */
#define L2CAP_LE_PSM_VALID_PSM(_x)                      (((_x) >= L2CAP_LE_PSM_MINIMUM_PSM) && ((_x) <= L2CAP_LE_PSM_MAXIMUM_PSM))

   /* The following Constants represent the defined Bluetooth L2CAP     */
   /* Link Timeout Minimum and Maximum Values (in Milliseconds).        */
#define L2CAP_LINK_TIMEOUT_MINIMUM_VALUE                                0x0000
#define L2CAP_LINK_TIMEOUT_MAXIMUM_VALUE                                0x9FC4
#define L2CAP_LINK_TIMEOUT_DEFAULT_VALUE                                0x4E20

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability of testing whether or not a specified LinkTO value is */
   /* valid.  The first parameter to this MACRO is the LinkTO Value to  */
   /* verify.  This MACRO returns a boolean value based upon whether or */
   /* not the specified LinkTO value is valid.  This MACRO returns a    */
   /* boolean TRUE if the specified LinkTO is valid, or a boolean       */
   /* FALSE if the specified LinkTo value is invalid.                   */
#define L2CAP_LINK_TIMEOUT_VALID_LINK_TIMEOUT(_x)       (((_x) >= L2CAP_LINK_TIMEOUT_MINIMUM_VALUE) && ((_x) <= L2CAP_LINK_TIMEOUT_MAXIMUM_VALUE))

   /* The following Constants represent the define Bluetooth L2CAP      */
   /* Minimum and Maximum Values for Signal Identifiers.                */
#define L2CAP_SIGNAL_IDENTIFIER_MINIMUM_SIGNAL_IDENTIFIER               0x0001
#define L2CAP_SIGNAL_IDENTIFIER_MAXIMUM_SIGNAL_IDENTIFIER               0x00FF

   /* The following Constants represent the internally defined L2CAP    */
   /* Minimum and Maximum Values for Data Packet Identifiers.           */
#define L2CAP_DATA_IDENTIFIER_MINIMUM_DATA_IDENTIFIER                   0x0100
#define L2CAP_DATA_IDENTIFIER_MAXIMUM_DATA_IDENTIFIER                   0xFFFF

   /* The following MACROs are utility MACROs that exists to aid code   */
   /* readability of testing whether or not a specified Identifier value*/
   /* is valid.  The first parameter to this MACRO is the Identifier    */
   /* Value to verify.  This MACRO returns a boolean value based upon   */
   /* whether or not the specified Identifier value is valid.  These    */
   /* MACRO's return a boolean TRUE if the specified Identifier is      */
   /* valid, or a boolean FALSE if the specified Identifier value is    */
   /* invalid.                                                          */
#define L2CAP_IDENTIFIER_VALID_SIGNAL_IDENTIFIER(_x)    (((_x) >= L2CAP_SIGNAL_IDENTIFIER_MINIMUM_SIGNAL_IDENTIFIER) && ((_x) <= L2CAP_SIGNAL_IDENTIFIER_MAXIMUM_SIGNAL_IDENTIFIER))
#define L2CAP_IDENTIFIER_VALID_DATA_IDENTIFIER(_x)      ((_x) >= L2CAP_DATA_IDENTIFIER_MINIMUM_DATA_IDENTIFIER)

   /* The following Constants represent the defined Bluetooth L2CAP MTU */
   /* Minimum and Maximum Values.                                       */
#define L2CAP_MINIMUM_CONNECTION_MTU_SIZE                               48
#define L2CAP_MAXIMUM_CONNECTION_MTU_SIZE                               (65535 - ((sizeof(NonAlignedWord_t)*2)))

   /* The following Constants represent the defined Bluetooth L2CAP     */
   /* Flush Timeout Minimum and Maximum Values.                         */
#define L2CAP_FLUSH_TIMEOUT_MINIMUM_VALUE                               0x0000
#define L2CAP_FLUSH_TIMEOUT_MAXIMUM_VALUE                               0x04FF

   /* The following Constant represents the defined Bluetooth L2CAP     */
   /* Flush Timeout Inifinite Value.                                    */
#define L2CAP_FLUSH_TIMEOUT_INFINITE_VALUE                              0xFFFF

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability of testing whether or not a specified Flush Timeout   */
   /* value is valid.  The first parameter to this MACRO is the Flush   */
   /* Timeout Value to verify.  This MACRO returns a boolean value based*/
   /* upon whether or not the specified Flush Timeout value is valid.   */
   /* This MACRO returns a boolean TRUE if the specified Flush Timeout  */
   /* is valid, or a boolean FALSE if the specified Flush Timeout value */
   /* is invalid.                                                       */
#define L2CAP_FLUSH_TIMEOUT_VALID_FLUSH_TIMEOUT(_x)    (((_x) == L2CAP_FLUSH_TIMEOUT_INFINITE_VALUE)  || (((_x) >= L2CAP_FLUSH_TIMEOUT_MINIMUM_VALUE) && ((_x) <= L2CAP_FLUSH_TIMEOUT_MAXIMUM_VALUE)))

   /* The following Constants represent the defined Bluetooth L2CAP     */
   /* Command Reject Reason Values.                                     */
#define L2CAP_COMMAND_REJECT_REASON_NOT_UNDERSTOOD                      0x0000
#define L2CAP_COMMAND_REJECT_REASON_SIGNALING_MTU_EXCEEDED              0x0001
#define L2CAP_COMMAND_REJECT_REASON_INVALID_CID                         0x0002

   /* The following Constants represent the defined Bluetooth L2CAP     */
   /* Connection Result Values.                                         */
#define L2CAP_CONNECT_RESULT_CONNECTION_SUCCESSFUL                      0x0000
#define L2CAP_CONNECT_RESULT_CONNECTION_PENDING                         0x0001
#define L2CAP_CONNECT_RESULT_CONNECTION_REFUSED_PSM_NOT_REGISTERED      0x0002
#define L2CAP_CONNECT_RESULT_CONNECTION_REFUSED_SECURITY_RELATED        0x0003

   /* The following Constants represent error codes that may be returned*/
   /* in an Connect Confirmation event that are Bluetopia Specific.     */
   /* * NOTE * The Bluetopia Specific result codes are used to represent*/
   /*          those failures that have occurred that prevented the     */
   /*          establishment of a usable ACL connection to the remote   */
   /*          device required to support the L2CAP Protocol.           */
   /* * NOTE * When the following values are used to indicate local     */
   /*          errors, the Status value in the confirmation event will  */
   /*          indicate the error code that was returned by the local   */
   /*          Bluetooth device.                                        */
#define L2CAP_CONNECT_RESULT_CONNECTION_TIMEOUT                         0xEEEE
#define L2CAP_CONNECT_RESULT_CONNECTION_ACL_CONNECTION_FAILURE          0xEEEF
#define L2CAP_CONNECT_RESULT_CONNECTION_AUTHENTICATION_FAILURE          0xEEF0

   /* The following Constants represent the defined Bluetooth L2CAP LE  */
   /* Credit Based Connection Result Values.                            */
#define L2CAP_LE_CONNECT_RESULT_CONNECTION_SUCCESSFUL                               0x0000
#define L2CAP_LE_CONNECT_RESULT_CONNECTION_REFUSED_LE_PSM_NOT_REGISTERED            0x0002
#define L2CAP_LE_CONNECT_RESULT_CONNECTION_REFUSED_NO_RESOURCES                     0x0004
#define L2CAP_LE_CONNECT_RESULT_CONNECTION_REFUSED_INSUFFICIENT_AUTHENTICATION      0x0005
#define L2CAP_LE_CONNECT_RESULT_CONNECTION_REFUSED_INSUFFICIENT_AUTHORIZATION       0x0006
#define L2CAP_LE_CONNECT_RESULT_CONNECTION_REFUSED_INSUFFICIENT_ENCRYPTION_KEY_SIZE 0x0007
#define L2CAP_LE_CONNECT_RESULT_CONNECTION_REFUSED_INSUFFICIENT_ENCRYPTION          0x0008

   /* The following Constants represent the defined Bluetooth L2CAP     */
   /* Connection Status Values.                                         */
#define L2CAP_CONNECT_STATUS_NO_FURTHER_INFORMATION                     0x0000
#define L2CAP_CONNECT_STATUS_AUTHENTICATION_PENDING                     0x0001
#define L2CAP_CONNECT_STATUS_AUTHORIZATION_PENDING                      0x0002

   /* The following Constants represent the defined Bluetooth L2CAP     */
   /* Connection Response Response Values.                              */
#define L2CAP_CONNECT_RESPONSE_RESPONSE_SUCCESSFUL                      0x0000
#define L2CAP_CONNECT_RESPONSE_RESPONSE_PENDING                         0x0001
#define L2CAP_CONNECT_RESPONSE_RESPONSE_REFUSED_PSM_NOT_REGISTERED      0x0002
#define L2CAP_CONNECT_RESPONSE_RESPONSE_REFUSED_SECURITY_BLOCK          0x0003
#define L2CAP_CONNECT_RESPONSE_RESPONSE_REFUSED_NO_RESOURCES            0x0004

   /* The following Constants represent the defined Bluetooth L2CAP     */
   /* Connection Response Status Values.                                */
#define L2CAP_CONNECT_RESPONSE_STATUS_NO_FURTHER_INFORMATION            0x0000
#define L2CAP_CONNECT_RESPONSE_STATUS_AUTHENTICATION_PENDING            0x0001
#define L2CAP_CONNECT_RESPONSE_STATUS_AUTHORIZATION_PENDING             0x0002

   /* The following Constants represent the defined Bluetooth L2CAP     */
   /* Configuarion Flags Values.                                        */
#define L2CAP_CONFIG_FLAGS_CONTINUATION_FLAG                            0x0001

   /* The following Constants represent the defined Bluetooth L2CAP     */
   /* Config Option Type Designator Values.                             */
#define L2CAP_CONFIG_OPTION_TYPE_MTU                                    0x01
#define L2CAP_CONFIG_OPTION_TYPE_FLUSH_TIMEOUT                          0x02
#define L2CAP_CONFIG_OPTION_TYPE_QOS                                    0x03
#define L2CAP_CONFIG_OPTION_TYPE_MODE                                   0x04
#define L2CAP_CONFIG_OPTION_TYPE_FCS                                    0x05
#define L2CAP_CONFIG_OPTION_TYPE_EXTENDED_FLOW_SPEC                     0x06
#define L2CAP_CONFIG_OPTION_TYPE_EXTENDED_WINDOW_SIZE                   0x07
#define L2CAP_CONFIG_OPTION_TYPE_VALUE_MASK                             0x7F
#define L2CAP_CONFIG_OPTION_TYPE_HINT_MASK                              0x80

   /* The following Constants represent the defined Bluetooth L2CAP     */
   /* Config Response Result Values.                                    */
#define L2CAP_CONFIGURE_RESPONSE_RESULT_SUCCESS                         0x0000
#define L2CAP_CONFIGURE_RESPONSE_RESULT_FAILURE_UNACCEPTABLE_PARAMETERS 0x0001
#define L2CAP_CONFIGURE_RESPONSE_RESULT_FAILURE_REJECTED_NO_REASON      0x0002
#define L2CAP_CONFIGURE_RESPONSE_RESULT_FAILURE_UNKNOWN_OPTIONS         0x0003
#define L2CAP_CONFIGURE_RESPONSE_RESULT_PENDING                         0x0004
#define L2CAP_CONFIGURE_RESPONSE_RESULT_FAILURE_FLOW_SPEC_REJECTED      0x0005
#define L2CAP_CONFIGURE_RESPONSE_RESULT_TIMEOUT                         0xEEEE

   /* The following Constants represent the defined Bluetooth L2CAP     */
   /* Disconnect Response Result Values.                                */
#define L2CAP_DISCONNECT_RESPONSE_RESULT_SUCCESS                        0x0000
#define L2CAP_DISCONNECT_RESPONSE_RESULT_TIMEOUT                        0xEEEE

   /* The following Constants represent the defined Bluetooth L2CAP     */
   /* Data Read Result Values.                                          */
#define L2CAP_DATA_READ_RESULT_SUCCESS                                  0x0000
#define L2CAP_DATA_READ_RESULT_ERROR                                    0x0001

   /* The following Constants represent the defined Bluetooth L2CAP     */
   /* Data Read Result Values.                                          */
#define L2CAP_DATA_READ_STATUS_MTU_EXCEEDED                             0x0000
#define L2CAP_DATA_READ_STATUS_RECEIVE_TIMEOUT                          0x0001
#define L2CAP_DATA_READ_STATUS_SIZE_ERROR                               0x0002
#define L2CAP_DATA_READ_STATUS_LOST_PACKET_ERROR                        0x0003

   /* The following Constants represent the defined Bluetooth L2CAP     */
   /* Echo Response (Ping) Result Values.                               */
#define L2CAP_ECHO_RESPONSE_RESULT_RESPONSE_RECEIVED                    0x0000
   /* The following Constants represent error codes that may be returned*/
   /* in an Echo Confirmation event that are Bluetopia Specific.        */
   /* * NOTE * The Bluetopia Specific result codes are used to represent*/
   /*          those failures that have occurred that prevented the     */
   /*          establishment of a usable ACL connection to the remote   */
   /*          device required to support the L2CAP Protocol.           */
#define L2CAP_ECHO_RESPONSE_RESULT_RESPONSE_TIMEOUT                     0xEEEE
#define L2CAP_ECHO_RESPONSE_RESULT_RESPONSE_ACL_CONNECTION_FAILURE      0xEEEF
#define L2CAP_ECHO_RESPONSE_RESULT_RESPONSE_AUTHENTICATION_FAILURE      0xEEF0

   /* The following Constants represent the defined Bluetooth L2CAP     */
   /* Information Request InfoType Values.                              */
#define L2CAP_INFORMATION_REQUEST_INFOTYPE_CONNECTIONLESS_MTU           0x0001
#define L2CAP_INFORMATION_REQUEST_INFOTYPE_EXTENDED_FEATURE_MASK        0x0002
#define L2CAP_INFORMATION_REQUEST_INFOTYPE_FIXED_CHANNELS_SUPPORTED     0x0003
#define L2CAP_INFORMATION_REQUEST_INFOTYPE_VALUE_MASK                   0x0003

#define L2CAP_DEFAULT_CONNECTIONLESS_MTU_SIZE                           670
#define L2CAP_DEFAULT_CONNECTION_MTU_SIZE                               672
#define L2CAP_DEFAULT_CONNECTION_FLUSH_TIMEOUT                          0xFFFF
#define L2CAP_DEFAULT_QOS_FLAGS                                         0
#define L2CAP_DEFAULT_QOS_SERVICE_TYPE                                  1
#define L2CAP_DEFAULT_QOS_TOKEN_RATE                                    0
#define L2CAP_DEFAULT_QOS_TOKEN_BUCKET_SIZE                             0
#define L2CAP_DEFAULT_QOS_PEAK_BANDWIDTH                                0
#define L2CAP_DEFAULT_QOS_LATENCY                                       0xFFFFFFFFL
#define L2CAP_DEFAULT_QOS_DELAY_VARIATION                               0xFFFFFFFFL
#define L2CAP_DEFAULT_RETRANSMISSION_MODE                               0
#define L2CAP_DEFAULT_FCS_OPTION                                        1
#define L2CAP_DEFAULT_ENHANCED_FLOW_SPEC_IDENTIFIER                     1
#define L2CAP_DEFAULT_ENHANCED_FLOW_SPEC_SERVICE_TYPE                   1
#define L2CAP_DEFAULT_ENHANCED_FLOW_SPEC_MAX_SDU_SIZE                   0xFFFF
#define L2CAP_DEFAULT_ENHANCED_FLOW_SPEC_SDU_INTER_ARRIVAL_TIME         0xFFFFFFFFL
#define L2CAP_DEFAULT_ENHANCED_FLOW_SPEC_ACCESS_LATENCY                 0xFFFFFFFFL
#define L2CAP_DEFAULT_ENHANCED_FLOW_SPEC_FLUSH_TIMEOUT                  0xFFFFFFFFL

   /* The following Constants represent the defines Modes of operation  */
#define L2CAP_MODE_INFO_MODE_BASIC                                      0
#define L2CAP_MODE_INFO_MODE_RETRANSMISSION                             1
#define L2CAP_MODE_INFO_MODE_FLOW_CONTROL                               2
#define L2CAP_MODE_INFO_MODE_ENHANCED_RETRANSMISSION                    3
#define L2CAP_MODE_INFO_MODE_STREAMING                                  4
#define L2CAP_MODE_INFO_MODE_WITH_EXTENDED_WINDOW_SIZE                  128

   /* The following Constants represent the defined Bluetooth L2CAP Tx  */
   /* Window Minimum and Maximum Values.                                */
#define L2CAP_MODE_INFO_TX_WINDOW_MINIMUM_VALUE                         1
#define L2CAP_MODE_INFO_TX_WINDOW_MAXIMUM_VALUE                         32
#define L2CAP_MODE_INFO_TX_WINDOW_MAXIMUM_VALUE_ENHANCED                63

   /* The following Constants represent the defined Bluetooth L2CAP FCS */
   /* Option Values.                                                    */
#define L2CAP_FCS_OPTION_NO_FCS                                         0
#define L2CAP_FCS_OPTION_16_BIT_FCS                                     1
#define L2CAP_FCS_OPTION_DEFAULT                                        1

   /* The following Constants represent the defined Bluetooth L2CAP     */
   /* Extended Flow Spec Service Type Option values.                    */
#define L2CAP_EXTENDED_FLOW_SPEC_SERVICE_TYPE_NO_TRAFFIC                0
#define L2CAP_EXTENDED_FLOW_SPEC_SERVICE_TYPE_BEST_EFFORT               1
#define L2CAP_EXTENDED_FLOW_SPEC_SERVICE_TYPE_GUARANTEED                2

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability of testing whether or not a specified Tx Window value */
   /* is valid.  The first parameter to this MACRO is the Tx Window     */
   /* Value to verify.  This MACRO returns a boolean value based upon   */
   /* whether or not the specified Tx Window value is within the desired*/
   /* range.  This MACRO returns a boolean TRUE if the value specified  */
   /* is valid, or a boolean FALSE if the specified value value is      */
   /* invalid.                                                          */
#define L2CAP_MODE_INFO_VALID_TX_WINDOW(_x)    (((_x) >= L2CAP_MODE_INFO_TX_WINDOW_MINIMUM_VALUE) && ((_x) <= L2CAP_MODE_INFO_TX_WINDOW_MAXIMUM_VALUE))

   /* The following MACRO us used to determine the value of the         */
   /* Retransmission timeout based on the current Flush Timeout.  The   */
   /* parameter of the MACRO is the Flush Timeout that is being used.   */
   /* The Flush Timeout value is specified in miliseconds and the result*/
   /* of the macro is on miliseconds.                                   */
#define L2CAP_ERTM_RETRANSMISION_TIMEOUT_VALUE(_x) (((_x) == L2CAP_FLUSH_TIMEOUT_INFINITE_VALUE)?2000:(((3*(_x)) > 1000)?(3*(_x)):1000))

   /* The following MACRO us used to determine the value of the Monitor */
   /* timeout based on the current Flush Timeout.  The parameter of the */
   /* MACRO is the Flush Timeout that is being used.  The Flush Timeout */
   /* value is specified in miliseconds and the result of the macro is  */
   /* on miliseconds.                                                   */
#define L2CAP_ERTM_MONITOR_TIMEOUT_VALUE(_x)       (((_x) == L2CAP_FLUSH_TIMEOUT_INFINITE_VALUE)?12000:(((6*(_x)) > 12000)?(6*(_x)):12000))

   /* The following constants define the FCS option valued.             */
#define L2CAP_FCS_OPTION_NO_FCS                                         0
#define L2CAP_FCS_OPTION_16_BIT_FCS                                     1

   /* The following Constants represent the defined Bluetooth L2CAP     */
   /* Information Response Result Values.                               */
#define L2CAP_INFORMATION_RESPONSE_RESULT_SUCCESS                       0x0000
#define L2CAP_INFORMATION_RESPONSE_RESULT_NOT_SUPPORTED                 0x0001
#define L2CAP_INFORMATION_RESPONSE_RESULT_PDU_REJECTED                  0x0002

   /* The following Constants represent error codes that may be returned*/
   /* in an Information Confirmation event that are Bluetopia Specific. */
   /* * NOTE * The Bluetopia Specific result codes are used to represent*/
   /*          those failures that have occurred that prevented the     */
   /*          establishment of a usable ACL connection to the remote   */
   /*          device required to support the L2CAP Protocol.           */
#define L2CAP_INFORMATION_RESPONSE_RESULT_TIMEOUT                       0xEEEE
#define L2CAP_INFORMATION_RESPONSE_RESULT_ACL_CONNECTION_FAILURE        0xEEEF
#define L2CAP_INFORMATION_RESPONSE_RESULT_AUTHENTICATION_FAILURE        0xEEF0

   /* The following values represent the minimum and maximum values of  */
   /* the Interval value of the Connection Parameters (respectively).   */
   /* These values specify 1.25 milli-second increments.                */
#define L2CAP_CONNECTION_PARAMETERS_INTERVAL_MINIMUM_VALUE              6
#define L2CAP_CONNECTION_PARAMETERS_INTERVAL_MAXIMUM_VALUE              3200

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability of testing whether or not a specified Interval Value  */
   /* specified in an Update Connection Parameters request is valid.    */
   /* This MACRO returns a boolean TRUE if the specified value is       */
   /* within, the expected range or a boolean FALSE if the specified    */
   /* value is not.                                                     */
#define L2CAP_CONNECTION_PARAMETERS_INTERVAL_VALUE_VALID(_x)            (((_x) >= L2CAP_CONNECTION_PARAMETERS_INTERVAL_MINIMUM_VALUE) && ((_x) <= L2CAP_CONNECTION_PARAMETERS_INTERVAL_MAXIMUM_VALUE))

   /* The following values represent the minimum and maximum values of  */
   /* the Interval value of the Connection Parameters (respectively).   */
   /* These values specify 1.25 second increments.                      */
#define L2CAP_CONNECTION_PARAMETERS_TIMEOUT_MULITIPLIER_MINIMUM_VALUE   10
#define L2CAP_CONNECTION_PARAMETERS_TIMEOUT_MULITIPLIER_MAXIMUM_VALUE   3200

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability of testing whether or not a specified Timeout         */
   /* Multiplier value specified in an Update Connection Parameters     */
   /* request is valid.  This MACRO returns a boolean TRUE if the       */
   /* specified value is within, the expected range or a boolean FALSE  */
   /* if the specified value is not.                                    */
#define L2CAP_CONNECTION_PARAMETERS_TIMEOUT_MULTIPLIER_VALUE_VALID(_x)  (((_x) >= L2CAP_CONNECTION_PARAMETERS_TIMEOUT_MULITIPLIER_MINIMUM_VALUE) && ((_x) <= L2CAP_CONNECTION_PARAMETERS_TIMEOUT_MULITIPLIER_MAXIMUM_VALUE))

   /* The following defines the maximum value that can be specified for */
   /* the Slave Latency in an Update Connection Parameters request.     */
#define L2CAP_CONNECTION_PARAMETERS_MAX_SLAVE_LATENCY_MINUMUM_VALUE     0
#define L2CAP_CONNECTION_PARAMETERS_MAX_SLAVE_LATENCY_MAXIMUM_VALUE     499

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability of testing whether or not a specified maximum slave   */
   /* latency value specified in an Update Connection Parameters request*/
   /* is valid.  This MACRO returns a boolean TRUE if the specified     */
   /* value is within, the expected range or a boolean FALSE if the     */
   /* specified value is not.                                           */
#define L2CAP_CONNECTION_PARAMETERS_MAX_SLAVE_LATENCY_VALUE_VALID(_x)   (((_x) >= L2CAP_CONNECTION_PARAMETERS_MAX_SLAVE_LATENCY_MINUMUM_VALUE) && ((_x) <= L2CAP_CONNECTION_PARAMETERS_MAX_SLAVE_LATENCY_MAXIMUM_VALUE))

   /* The following are the bit number defines that should be used when */
   /* accessing bit data in the L2CAP_Extended_Feature_Mask_t structure.*/
#define L2CAP_EXTENDED_FEATURE_FLOW_CONTROL_BIT_NUMBER                  0
#define L2CAP_EXTENDED_FEATURE_RETRANSMIT_MODE_BIT_NUMBER               1
#define L2CAP_EXTENDED_FEATURE_BI_DIRECTIONAL_QOS_BIT_NUMBER            2
#define L2CAP_EXTENDED_FEATURE_ENHANCED_RETRANSMISSION_MODE_BIT_NUMBER  3
#define L2CAP_EXTENDED_FEATURE_STREAMING_MODE_BIT_NUMBER                4
#define L2CAP_EXTENDED_FEATURE_FCS_OPTION_BIT_NUMBER                    5
#define L2CAP_EXTENDED_FEATURE_ENHANCED_FLOW_SPEC_BIT_NUMBER            6
#define L2CAP_EXTENDED_FEATURE_FIXED_CHANNELS_BIT_NUMBER                7
#define L2CAP_EXTENDED_FEATURE_EXTENDED_WINDOW_SIZE_BIT_NUMBER          8
#define L2CAP_EXTENDED_FEATURE_UNICAST_DATA_RECEPTION_BIT_NUMBER        9
#define L2CAP_EXTENDED_FEATURE_MASK_EXTENSION_BIT_NUMBER                31

   /* The following represent the Bit number of the Supported Fixed     */
   /* Channels.                                                         */
#define L2CAP_FIXED_CHANNELS_SUPPORTED_L2CAP_NULL_IDENTIFIER_BIT        0
#define L2CAP_FIXED_CHANNELS_SUPPORTED_L2CAP_SIGNALLING_CHANNEL_BIT     1
#define L2CAP_FIXED_CHANNELS_SUPPORTED_CONNECTIONLESS_CHANNEL_BIT       2
#define L2CAP_FIXED_CHANNELS_SUPPORTED_AMP_MANAGER_CHANNEL_BIT          3
#define L2CAP_FIXED_CHANNELS_SUPPORTED_AMP_MANAGER_TEST_CHANNEL_BIT     63

   /* The following Constants represent the defined Bluetooth L2CAP     */
   /* Group Add Member Response Result Values.                          */
#define L2CAP_GROUP_ADD_MEMBER_RESPONSE_RESULT_SUCCESS                  0x0000
#define L2CAP_GROUP_ADD_MEMBER_RESPONSE_RESULT_FAILURE                  0x0001

   /* The following Constants represent the defined Bluetooth L2CAP     */
   /* Group Membership Result Values.                                   */
#define L2CAP_GROUP_MEMBERSHIP_RESPONSE_RESULT_SUCCESS                  0x0000
#define L2CAP_GROUP_MEMBERSHIP_RESPONSE_RESULT_FAILURE                  0x0001

   /* The following Constants represent the defined Bluetooth L2CAP     */
   /* Connection Parameter Update Response Result Values.               */
#define L2CAP_CONNECTION_PARAMETER_UPDATE_RESPONSE_RESULT_ACCEPTED      0x0000
#define L2CAP_CONNECTION_PARAMETER_UPDATE_RESPONSE_RESULT_REJECTED      0x0001

   /* According to the Bluetooth Specification 1.0B, L2CAP              */
   /* Implementations Signalling packets should not exceed an MTU size  */
   /* of 48 bytes without first testing to see if a larger packet can   */
   /* be supported.                                                     */
#define L2CAP_SIGNALING_MTU_SIZE                                        48

   /* L2CAP Signaling Command Codes.                                    */
#define L2CAP_CODE_REJECT_COMMAND                                       0x01
#define L2CAP_CODE_CONNECTION_REQUEST                                   0x02
#define L2CAP_CODE_CONNECTION_RESPONSE                                  0x03
#define L2CAP_CODE_CONFIG_REQUEST                                       0x04
#define L2CAP_CODE_CONFIG_RESPONSE                                      0x05
#define L2CAP_CODE_DISCONNECTION_REQUEST                                0x06
#define L2CAP_CODE_DISCONNECTION_RESPONSE                               0x07
#define L2CAP_CODE_ECHO_REQUEST                                         0x08
#define L2CAP_CODE_ECHO_RESPONSE                                        0x09
#define L2CAP_CODE_INFORMATION_REQUEST                                  0x0A
#define L2CAP_CODE_INFORMATION_RESPONSE                                 0x0B
#define L2CAP_CODE_CREATE_CHANNEL_REQUEST                               0x0C
#define L2CAP_CODE_CREATE_CHANNEL_RESPONSE                              0x0D
#define L2CAP_CODE_MOVE_CHANNEL_REQUEST                                 0x0E
#define L2CAP_CODE_MOVE_CHANNEL_RESPONSE                                0x0F
#define L2CAP_CODE_MOVE_CHANNEL_CONFIRMATION                            0x10
#define L2CAP_CODE_MOVE_CHANNEL_CONFIRMATION_RESPONSE                   0x11
#define L2CAP_CODE_CONNECTION_PARAMETER_UPDATE_REQUEST                  0x12
#define L2CAP_CODE_CONNECTION_PARAMETER_UPDATE_RESPONSE                 0x13
#define L2CAP_CODE_LE_CREDIT_BASED_CONNECTION_REQUEST                   0x14
#define L2CAP_CODE_LE_CREDIT_BASED_CONNECTION_RESPONSE                  0x15
#define L2CAP_CODE_LE_FLOW_CONTROL_CREDIT                               0x16

   /* L2CAP SDP Protocol UUID's.                                        */

   /* The following MACRO is a utility MACRO that assigns the L2CAP     */
   /* Bluetooth Universally Unique Identifier (L2CAP_UUID_16) to the    */
   /* specified UUID_16_t variable.  This MACRO accepts one parameter   */
   /* which is the UUID_16_t variable that is receive the L2CAP_UUID_16 */
   /* Constant value.                                                   */
#define SDP_ASSIGN_L2CAP_UUID_16(_x)            ASSIGN_SDP_UUID_16((_x), 0x01, 0x00)

   /* The following MACRO is a utility MACRO that assigns the L2CAP     */
   /* Bluetooth Universally Unique Identifier (L2CAP_UUID_32) to the    */
   /* specified UUID_32_t variable.  This MACRO accepts one parameter   */
   /* which is the UUID_32_t variable that is receive the L2CAP_UUID_32 */
   /* Constant value.                                                   */
#define SDP_ASSIGN_L2CAP_UUID_32(_x)            ASSIGN_SDP_UUID_32((_x), 0x00, 0x00, 0x01, 0x00)

   /* The following MACRO is a utility MACRO that assigns the L2CAP     */
   /* Bluetooth Universally Unique Identifier (L2CAP_UID_128) to the    */
   /* specified UUID_128_t variable.  This MACRO accepts one parameter  */
   /* which is the UUID_128_t variable that is receive the              */
   /* L2CAP_UUID_128 Constant value.                                    */
#define SDP_ASSIGN_L2CAP_UUID_128(_x)           ASSIGN_SDP_UUID_128((_x), 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /* The following structure represents the Header data that is        */
   /* present in all L2CAP Signaling Commands.  The 'Code' parameter    */
   /* identifies the command code, the 'identifier' field holds a unique*/
   /* number that is used to identify a response to the command.  The   */
   /* 'Data Length' field holds the size of the command data.           */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_Signal_Command_Header_t
{
  NonAlignedByte_t Code;
  NonAlignedByte_t Identifier;
  NonAlignedWord_t DataLength;
} __PACKED_STRUCT_END__ L2CAP_Signal_Command_Header_t;

#define L2CAP_SIGNAL_COMMAND_HEADER_SIZE                (sizeof(L2CAP_Signal_Command_Header_t))

   /* The following structure represents the structure of the L2CAP     */
   /* Reject Command Packet.                                            */
typedef __PACKED_STRUCT_BEGIN__ struct _L2CAP_Reject_Command_t
{
  L2CAP_Signal_Command_Header_t CommandHeader;
  NonAlignedWord_t              Reason;
  Byte_t                        Variable_Data[1];
} __PACKED_STRUCT_END__ L2CAP_Reject_Command_t;

   /* The following MACRO is provided to allow the programmer a very    */
   /* simple means of quickly determining the number of Data Bytes that */
   /* will be required to hold a L2CAP Signal Reject Command Packet.    */
   /* The first parameter to this MACRO is the Number of Bytes of       */
   /* Variable Data that are contained/required for the L2CAP Reject    */
   /* Command.                                                          */
#define L2CAP_REJECT_COMMAND_SIZE(_x)                   ((sizeof(L2CAP_Reject_Command_t) - sizeof(Byte_t)) + (unsigned int)(_x))

   /* The following structure represents the structure of the L2CAP     */
   /* Connect Command Packet.                                           */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_Connect_Request_t
{
  L2CAP_Signal_Command_Header_t CommandHeader;
  NonAlignedWord_t              PSM;
  NonAlignedWord_t              SCID;
} __PACKED_STRUCT_END__ L2CAP_Connect_Request_t;

#define L2CAP_CONNECT_REQUEST_SIZE                      (sizeof(L2CAP_Connect_Request_t))

   /* The following structure represents the structure of the L2CAP     */
   /* Connect Response Packet.                                          */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_Connect_Response_t
{
  L2CAP_Signal_Command_Header_t CommandHeader;
  NonAlignedWord_t              DCID;
  NonAlignedWord_t              SCID;
  NonAlignedWord_t              Result;
  NonAlignedWord_t              Status;
} __PACKED_STRUCT_END__ L2CAP_Connect_Response_t;

#define L2CAP_CONNECT_RESPONSE_SIZE                     (sizeof(L2CAP_Connect_Response_t))

   /* The following structure represents the structure of the L2CAP     */
   /* Config Request Packet.                                            */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_Config_Request_t
{
  L2CAP_Signal_Command_Header_t CommandHeader;
  NonAlignedWord_t              DCID;
  NonAlignedWord_t              Flags;
  Byte_t                        Variable_Data[1];
} __PACKED_STRUCT_END__ L2CAP_Config_Request_t;

   /* A Config Request can have up to as many bytes of optional data    */
   /* that is allowed by the supported MTU size, thus the               */
   /* MAX_PAYLOAD_SIZE.                                                 */
#define L2CAP_CONFIG_REQUEST_SIZE(_x)                   (sizeof(L2CAP_Config_Request_t) - sizeof(Byte_t) + (unsigned int)(_x))

   /* The following structure represents the structure of the L2CAP     */
   /* Config Response Packet.                                           */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_Config_Response_t
{
  L2CAP_Signal_Command_Header_t CommandHeader;
  NonAlignedWord_t              SCID;
  NonAlignedWord_t              Flags;
  NonAlignedWord_t              Result;
  Byte_t                        Variable_Data[1];
} __PACKED_STRUCT_END__ L2CAP_Config_Response_t;

   /* A Config Response can have up to as many bytes of optional data   */
   /* that is allowed by the supported MTU size, thus the               */
   /* MAX_PAYLOAD_SIZE.                                                 */
#define L2CAP_CONFIG_RESPONSE_SIZE(_x)                  (sizeof(L2CAP_Config_Response_t) - sizeof(Byte_t) + (unsigned int)(_x))

   /* The following structure represents the Header information that    */
   /* will prefix each Config Parameter.                                */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_Config_Param_Header_t
{
   Byte_t Type;
   Byte_t Length;
} __PACKED_STRUCT_END__ L2CAP_Config_Param_Header_t;

#define L2CAP_CONFIG_PARAM_HEADER_SIZE                  (sizeof(L2CAP_Config_Param_Header_t))

   /* The following structure represents the L2CAP QoS Flow Spec        */
   /* parameters.                                                       */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_Flow_Spec_t
{
   Byte_t             Flags;
   Byte_t             ServiceType;
   NonAlignedDWord_t  TokenRate;
   NonAlignedDWord_t  TokenBucketSize;
   NonAlignedDWord_t  PeakBandwidth;
   NonAlignedDWord_t  Latency;
   NonAlignedDWord_t  DelayVariation;
} __PACKED_STRUCT_END__ L2CAP_Flow_Spec_t;

#define L2CAP_FLOW_SPEC_SIZE                            (sizeof(L2CAP_Flow_Spec_t))

   /* The following structure represents the L2CAP Mode Info parameters.*/
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_Mode_Info_t
{
   Byte_t           Mode;
   Byte_t           TxWindowSize;
   Byte_t           MaxTransmitAttempts;
   NonAlignedWord_t RetransmissionTimeout;
   NonAlignedWord_t MonitorTimeout;
   NonAlignedWord_t MaxPDUSize;
} __PACKED_STRUCT_END__ L2CAP_Mode_Info_t;

#define L2CAP_MODE_INFO_SIZE                            (sizeof(L2CAP_Mode_Info_t))

   /* The following structure represents the MTU option data contained  */
   /* in a Configuration signaling packet.                              */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_Config_Param_MTU_t
{
   L2CAP_Config_Param_Header_t ParamHeader;
   NonAlignedWord_t            MTU;
} __PACKED_STRUCT_END__ L2CAP_Config_Param_MTU_t;

#define L2CAP_CONFIG_PARAM_MTU_SIZE                     (sizeof(L2CAP_Config_Param_MTU_t))

   /* The following structure represents an unknown option info from    */
   /* in a Configuration signaling packet.                              */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_Config_Param_Unknown_t
{
   L2CAP_Config_Param_Header_t ParamHeader;
} __PACKED_STRUCT_END__ L2CAP_Config_Param_Unknown_t;

#define L2CAP_CONFIG_PARAM_UNKNOWN_SIZE                 (sizeof(L2CAP_Config_Param_Unknown_t))

   /* The following structure represents the Flush Timeout option data  */
   /* contained in a Configuration signaling packet.                    */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_Config_Param_FlushTO_t
{
   L2CAP_Config_Param_Header_t ParamHeader;
   NonAlignedWord_t            FlushTO;
} __PACKED_STRUCT_END__ L2CAP_Config_Param_FlushTO_t;

#define L2CAP_CONFIG_PARAM_FLUSH_TIMEOUT_SIZE           (sizeof(L2CAP_Config_Param_FlushTO_t))

   /* The following structure represents the QoS option data contained  */
   /* in a Configuration signaling packet.                              */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_Config_Param_QoS_t
{
   L2CAP_Config_Param_Header_t ParamHeader;
   L2CAP_Flow_Spec_t           FlowSpec;
} __PACKED_STRUCT_END__ L2CAP_Config_Param_QoS_t;

#define L2CAP_CONFIG_PARAM_QOS_SIZE                     (sizeof(L2CAP_Config_Param_QoS_t))

   /* The following structure represents the QoS option data contained  */
   /* in a Configuration signaling packet.                              */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_Config_Param_Mode_Info_t
{
   L2CAP_Config_Param_Header_t ParamHeader;
   L2CAP_Mode_Info_t           ModeInfo;
} __PACKED_STRUCT_END__ L2CAP_Config_Param_Mode_Info_t;

#define L2CAP_CONFIG_PARAM_MODE_INFO_SIZE               (sizeof(L2CAP_Config_Param_Mode_Info_t))

   /* The following structure represents the FCS option data contained  */
   /* in a Configuration signaling packet.                              */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_Config_Param_FCS_Info_t
{
   L2CAP_Config_Param_Header_t ParamHeader;
   Byte_t                      FCSOption;
} __PACKED_STRUCT_END__ L2CAP_Config_Param_FCS_Info_t;

#define L2CAP_CONFIG_PARAM_FCS_INFO_SIZE                (sizeof(L2CAP_Config_Param_FCS_Info_t))

   /* The following structure represents the QoS option data contained  */
   /* in a Configuration signaling packet.                              */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_Config_Param_Extended_Flow_Spec_t
{
   L2CAP_Config_Param_Header_t ParamHeader;
   Byte_t                      Identifier;
   Byte_t                      Service_Type;
   NonAlignedWord_t            Maximum_SDU_Size;
   NonAlignedDWord_t           SDU_Inter_Arrival_Time;
   NonAlignedDWord_t           Access_Latency;
   NonAlignedDWord_t           Flush_Timeout;
} __PACKED_STRUCT_END__ L2CAP_Config_Param_Extended_Flow_Spec_t;

#define L2CAP_CONFIG_PARAM_EXTENDED_FLOW_SPEC_SIZE     (sizeof(L2CAP_Config_Param_Extended_Flow_Spec_t))

   /* The following structure represents the QoS option data contained  */
   /* in a Configuration signaling packet.                              */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_Config_Param_Extended_Window_Size_t
{
   L2CAP_Config_Param_Header_t ParamHeader;
   NonAlignedWord_t            MaxWindowSize;
} __PACKED_STRUCT_END__ L2CAP_Config_Param_Extended_Window_Size_t;

#define L2CAP_CONFIG_PARAM_EXTENDED_WINDOW_SIZE_SIZE     (sizeof(L2CAP_Config_Param_Extended_Window_Size_t))

   /* The following structure represents the structure of the L2CAP     */
   /* Disconnect Request Packet.                                        */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_Disconnect_Request_t
{
  L2CAP_Signal_Command_Header_t CommandHeader;
  NonAlignedWord_t              DCID;
  NonAlignedWord_t              SCID;
} __PACKED_STRUCT_END__ L2CAP_Disconnect_Request_t;

#define L2CAP_DISCONNECT_REQUEST_SIZE                   (sizeof(L2CAP_Disconnect_Request_t))

   /* The following structure represents the structure of the L2CAP     */
   /* Disconnect Response Packet.                                       */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_Disconnect_Response_t
{
  L2CAP_Signal_Command_Header_t CommandHeader;
  NonAlignedWord_t              DCID;
  NonAlignedWord_t              SCID;
} __PACKED_STRUCT_END__ L2CAP_Disconnect_Response_t;

#define L2CAP_DISCONNECT_RESPONSE_SIZE                  (sizeof(L2CAP_Disconnect_Response_t))

   /* The following structure represents the structure of the L2CAP     */
   /* Echo Request Packet.                                              */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_Echo_Request_t
{
  L2CAP_Signal_Command_Header_t CommandHeader;
  Byte_t                        Variable_Data[1];
} __PACKED_STRUCT_END__ L2CAP_Echo_Request_t;

   /* An Echo Request can have up to as many bytes of optional data that*/
   /* is allowed by the supported MTU size, thus the MAX_PAYLOAD_SIZE.  */
#define L2CAP_ECHO_REQUEST_SIZE(_x)                     (sizeof(L2CAP_Echo_Request_t) - sizeof(Byte_t) + (unsigned int)(_x))

#define L2CAP_MAX_ECHO_REQUEST_VARIABLE_DATA_SIZE       (L2CAP_SIGNALING_MTU_SIZE - L2CAP_ECHO_REQUEST_SIZE(0))

   /* The following structure represents the structure of the L2CAP     */
   /* Echo Response Packet.                                             */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_Echo_Response_t
{
  L2CAP_Signal_Command_Header_t CommandHeader;
  Byte_t                        Variable_Data[1];
} __PACKED_STRUCT_END__ L2CAP_Echo_Response_t;

   /* An Echo Response can have up to as many bytes of optional data    */
   /* that is allowed by the supported MTU size, thus the               */
   /* MAX_PAYLOAD_SIZE.                                                 */
#define L2CAP_ECHO_RESPONSE_SIZE(_x)                    (sizeof(L2CAP_Echo_Response_t) - sizeof(Byte_t) + (unsigned int)(_x))

#define L2CAP_MAX_ECHO_RESPONSE_VARIABLE_DATA_SIZE      (L2CAP_SIGNALING_MTU_SIZE - L2CAP_ECHO_RESPONSE_SIZE(0))

   /* The following structure represents the structure of the L2CAP     */
   /* Information Request Packet.                                       */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_Information_Request_t
{
  L2CAP_Signal_Command_Header_t CommandHeader;
  NonAlignedWord_t              InfoType;
} __PACKED_STRUCT_END__ L2CAP_Information_Request_t;

#define L2CAP_INFORMATION_REQUEST_SIZE                  (sizeof(L2CAP_Information_Request_t))

   /* The following structure represents the structure of the L2CAP     */
   /* Information Response Packet.                                      */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_Information_Response_t
{
  L2CAP_Signal_Command_Header_t CommandHeader;
  NonAlignedWord_t              InfoType;
  NonAlignedWord_t              Result;
  Byte_t                        Variable_Data[1];
} __PACKED_STRUCT_END__ L2CAP_Information_Response_t;

#define L2CAP_INFORMATION_RESPONSE_SIZE(_x)             (sizeof(L2CAP_Information_Response_t) - sizeof(Byte_t) + (unsigned int)(_x))

   /* The following structure represents the structure of the L2CAP     */
   /* Connection Parameter Update Packet.                               */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_Connection_Parameter_Update_Request_t
{
  L2CAP_Signal_Command_Header_t CommandHeader;
  NonAlignedWord_t              IntervalMin;
  NonAlignedWord_t              IntervalMax;
  NonAlignedWord_t              SlaveLatency;
  NonAlignedWord_t              TimeoutMultiplier;
} __PACKED_STRUCT_END__ L2CAP_Connection_Parameter_Update_Request_t;

#define L2CAP_CONNECTION_PARAMETER_UPDATE_REQUEST_SIZE  (sizeof(L2CAP_Connection_Parameter_Update_Request_t))

   /* The following structure represents the structure of the L2CAP     */
   /* Connection Parameter Update Response Packet.                      */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_Connection_Parameter_Update_Response_t
{
  L2CAP_Signal_Command_Header_t CommandHeader;
  NonAlignedWord_t              Result;
} __PACKED_STRUCT_END__ L2CAP_Connection_Parameter_Update_Response_t;

#define L2CAP_CONNECTION_PARAMETER_UPDATE_RESPONSE_SIZE (sizeof(L2CAP_Connection_Parameter_Update_Response_t))

   /* The following structure represents the structure of the L2CAP LE  */
   /* Credit Based Connection Request Packet.                           */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_LE_Credit_Based_Connection_Request_t
{
  L2CAP_Signal_Command_Header_t CommandHeader;
  NonAlignedWord_t              LE_PSM;
  NonAlignedWord_t              SCID;
  NonAlignedWord_t              MTU;
  NonAlignedWord_t              MPS;
  NonAlignedWord_t              InitialCredits;
} __PACKED_STRUCT_END__ L2CAP_LE_Credit_Based_Connection_Request_t;

#define L2CAP_LE_CREDIT_BASED_CONNECTION_REQUEST_SIZE   (sizeof(L2CAP_LE_Credit_Based_Connection_Request_t))

   /* The following structure represents the structure of the L2CAP LE  */
   /* Credit Based Connection Response Packet.                          */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_LE_Credit_Based_Connection_Response_t
{
  L2CAP_Signal_Command_Header_t CommandHeader;
  NonAlignedWord_t              DCID;
  NonAlignedWord_t              MTU;
  NonAlignedWord_t              MPS;
  NonAlignedWord_t              InitialCredits;
  NonAlignedWord_t              Result;
} __PACKED_STRUCT_END__ L2CAP_LE_Credit_Based_Connection_Response_t;

#define L2CAP_LE_CREDIT_BASED_CONNECTION_RESPONSE_SIZE  (sizeof(L2CAP_LE_Credit_Based_Connection_Response_t))

   /* The following structure represents the structure of the L2CAP LE  */
   /* Flow Control Credit Packet.                                       */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_LE_Flow_Control_Credit_t
{
  L2CAP_Signal_Command_Header_t CommandHeader;
  NonAlignedWord_t              DCID;
  NonAlignedWord_t              Credits;
} __PACKED_STRUCT_END__ L2CAP_LE_Flow_Control_Credit_t;

#define L2CAP_LE_FLOW_CONTROL_CREDIT_SIZE               (sizeof(L2CAP_LE_Flow_Control_Credit_t))

   /* The following type declaration represents the structure of the    */
   /* Header of an L2CAP Data Packet.  This Header Information is       */
   /* contained in Every Defined L2CAP Data Packet.  This structure     */
   /* forms the basis of additional defined L2CAP Data Packets.  Since  */
   /* this structure is present at the beginning of Every Defined L2CAP */
   /* Data Packet, this structure will be the first element of Every    */
   /* Defined L2CAP Data Packet in this file.                           */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_Data_Packet_Header_t
{
   NonAlignedWord_t Length;
   NonAlignedWord_t ChannelID;
} __PACKED_STRUCT_END__ L2CAP_Data_Packet_Header_t;

#define L2CAP_DATA_PACKET_HEADER_SIZE                   (sizeof(L2CAP_Data_Packet_Header_t))

   /* The following MACRO is provided to allow the programmer a very    */
   /* simple means of quickly determining the number of Data Bytes that */
   /* will be required to hold a L2CAP Signal Command Packet.  The value*/
   /* 'x' should be the Length of the Payload data.                     */
#define L2CAP_CALCULATE_DATA_PACKET_SIZE(_x)            (L2CAP_DATA_PACKET_HEADER_SIZE + (unsigned int)(_x))

   /* The following structure represents the structure of the L2CAP     */
   /* Connectionless Packet.                                            */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_Connectionless_Packet_t
{
   L2CAP_Data_Packet_Header_t L2CAP_Data_Packet_Header;
   NonAlignedWord_t           PSM;
   Byte_t                     Variable_Data[1];
} __PACKED_STRUCT_END__ L2CAP_Connectionless_Packet_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an L2CAP          */
   /* Connectionless Data Packet Structure based upon the size of the   */
   /* Connectionless Data associated with the Packet. The first         */
   /* parameter to this MACRO is the size (in Bytes) of the L2CAP Data  */
   /* that is to be part of the L2CAP Connectionless Data.              */
#define L2CAP_CONNECTIONLESS_PACKET_SIZE(_x)            ((sizeof(L2CAP_Connectionless_Packet_t) - sizeof(Byte_t)) + (unsigned int)(_x))

   /* The following structure represents the structure of the L2CAP     */
   /* Connection Oriented Packet.                                       */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_Connection_Oriented_Packet_t
{
   L2CAP_Data_Packet_Header_t L2CAP_Data_Packet_Header;
   Byte_t                     Variable_Data[1];
} __PACKED_STRUCT_END__ L2CAP_Connection_Oriented_Packet_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an L2CAP          */
   /* Connection Oriented Data Packet Structure based upon the size of  */
   /* the Connection Oriented Data associated with the Packet. The      */
   /* first parameter to this MACRO is the size (in Bytes) of the L2CAP */
   /* Data that is to be part of the L2CAP Connection Oriented Data.    */
#define L2CAP_CONNECTION_ORIENTED_PACKET_SIZE(_x)       ((sizeof(L2CAP_Connection_Oriented_Packet_t) - sizeof(Byte_t)) + (unsigned int)(_x))

   /* The following structure represents the structure of the L2CAP     */
   /* Signalling Command Packet.                                        */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_Signalling_Command_Packet_t
{
   L2CAP_Data_Packet_Header_t L2CAP_Data_Packet_Header;
   Byte_t                     Variable_Data[1];
} __PACKED_STRUCT_END__ L2CAP_Signaling_Command_Packet_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an L2CAP          */
   /* Signalling Command Packet Structure based upon the size of the    */
   /* Signalling Command Data associated with the Packet. The first     */
   /* parameter to this MACRO is the size (in Bytes) of the L2CAP Data  */
   /* that is to be part of the L2CAP Signalling Command Packet.        */
#define L2CAP_SIGNALLING_COMMAND_PACKET_SIZE(_x)        ((sizeof(L2CAP_Signalling_Command_Packet_t) - sizeof(Byte_t)) + (unsigned int)(_x))

   /* The following defines the size of the FCS field that may be added */
   /* to the following packets.  The following packet definitions may   */
   /* require the FCS to be added and the Size Macros do not            */
   /* automatically include this option.                                */
#define L2CAP_FCS_SIZE                                  (sizeof(NonAlignedWord_t))
#define L2CAP_CONTROL_WORD_FIELD_SIZE                   (sizeof(NonAlignedWord_t))

   /* The following structure represents the structure of the L2CAP S   */
   /* Packet.                                                           */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_S_Packet_t
{
   NonAlignedWord_t Control;
   NonAlignedWord_t FCS;
} __PACKED_STRUCT_END__ L2CAP_S_Packet_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an L2CAP S Packet */
   /* Structure                                                         */
#define L2CAP_S_PACKET_SIZE                             (sizeof(L2CAP_S_Packet_t)- L2CAP_FCS_SIZE)

   /* The following structure represents the structure of the L2CAP I   */
   /* Packet.                                                           */
   /* * NOTE * The FCS Member in this structure should NOT be used to   */
   /*          access the FCS value.  It's place in this structure is   */
   /*          ONLY to show the programmer that the FCS Member follows  */
   /*          the Variable Data Member which is of varying length.     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_I_Packet_t
{
   NonAlignedWord_t Control;
   NonAlignedWord_t SDU_Length;
   Byte_t           Variable_Data[1];
   NonAlignedWord_t FCS;
} __PACKED_STRUCT_END__ L2CAP_I_Packet_t;

   /* The following defines the size of the SDU Length field.  When     */
   /* segmenting packets, the amount of payload that can be placed in   */
   /* the first packet will be reduced by this amount.  The SDU Length  */
   /* field is only present in the first segmented packet.              */
#define L2CAP_SDU_LENGTH_FIELD_SIZE                           (sizeof(NonAlignedWord_t))

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an L2CAP I Packet */
   /* Structure based upon the size of the Data associated with the     */
   /* Packet.  The first parameter to this MACRO is the size (in Bytes) */
   /* of the Payload that is to be part of the Packet.                  */
#define L2CAP_I_PACKET_SIZE(_x)                         ((sizeof(L2CAP_I_Packet_t) - sizeof(Byte_t) - L2CAP_FCS_SIZE) + (unsigned int)(_x))

   /* The following structure represents the structure of the L2CAP I   */
   /* Packet when Extended Window Size is used.                         */
   /* * NOTE * The FCS Member in this structure should NOT be used to   */
   /*          access the FCS value.  It's place in this structure is   */
   /*          ONLY to show the programmer that the FCS Member follows  */
   /*          the Variable Data Member which is of varying length.     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_E_Packet_t
{
   NonAlignedDWord_t Control;
   NonAlignedWord_t  SDU_Length;
   Byte_t            Variable_Data[1];
   NonAlignedWord_t  FCS;
} __PACKED_STRUCT_END__ L2CAP_E_Packet_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an L2CAP E Packet */
   /* Structure based upon the size of the Data associated with the     */
   /* Packet.  The first parameter to this MACRO is the size (in Bytes) */
   /* of the Payload that is to be part of the Packet.                  */
#define L2CAP_E_PACKET_SIZE(_x)                         ((sizeof(L2CAP_E_Packet_t) - sizeof(Byte_t) - L2CAP_FCS_SIZE) + (unsigned int)(_x))

   /* The following structure represents the structure of the L2CAP C   */
   /* Frame Packet.  This packet is a Continuation packet of an I Frame.*/
   /* * NOTE * The FCS Member in this structure should NOT be used to   */
   /*          access the FCS value.  It's place in this structure is   */
   /*          ONLY to show the programmer that the FCS Member follows  */
   /*          the Variable Data Member which is of varying length.     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_C_Packet_t
{
   NonAlignedWord_t Control;
   Byte_t           Variable_Data[1];
   NonAlignedWord_t FCS;
} __PACKED_STRUCT_END__ L2CAP_C_Packet_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an L2CAP C Packet */
   /* Structure based upon the size of the Data associated with the     */
   /* Packet.  The first parameter to this MACRO is the size (in Bytes) */
   /* of the Payload that is to be part of the Packet.                  */
#define L2CAP_C_PACKET_SIZE(_x)                         ((sizeof(L2CAP_C_Packet_t) - sizeof(Byte_t) - L2CAP_FCS_SIZE) + (unsigned int)(_x))

   /* The following structure represents the structure of the L2CAP S   */
   /* Frame Packet.                                                     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_S_Frame_Packet_t
{
   L2CAP_Data_Packet_Header_t L2CAP_Data_Packet_Header;
   L2CAP_S_Packet_t           S_Packet;
} __PACKED_STRUCT_END__ L2CAP_S_Frame_Packet_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an L2CAP S Frame  */
   /* Packet Structure                                                  */
#define L2CAP_S_FRAME_PACKET_SIZE                       (sizeof(L2CAP_Data_Packet_Header_t) + L2CAP_S_PACKET_SIZE)

   /* The following structure represents the structure of the L2CAP I   */
   /* Frame Packet.  This packet defines the structure of the first     */
   /* packet segment of a transmission.  The remaining packets are      */
   /* formatted to an L2CAP_C_Frame_Packet_t structure.                 */
   /* * NOTE * The FCS Member in this structure should NOT be used to   */
   /*          access the FCS value.  It's place in this structure is   */
   /*          ONLY to show the programmer that the FCS Member follows  */
   /*          the Variable Data Member which is of varying length.     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_I_Frame_Packet_t
{
   L2CAP_Data_Packet_Header_t L2CAP_Data_Packet_Header;
   L2CAP_I_Packet_t           I_Packet;
} __PACKED_STRUCT_END__ L2CAP_I_Frame_Packet_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an L2CAP I Frame  */
   /* Packet Structure based upon the size of the Data associated with  */
   /* the Packet.  The first parameter to this MACRO is the size (in    */
   /* Bytes) of the Payload that is to be part of the Packet.           */
#define L2CAP_I_FRAME_PACKET_SIZE(_x)                   (sizeof(L2CAP_Data_Packet_Header_t) + L2CAP_I_PACKET_SIZE(_x))

   /* The following structure represents the structure of the L2CAP C   */
   /* Frame Packet.  This packet is a Continuation packet of an I Frame.*/
   /* * NOTE * The FCS Member in this structure should NOT be used to   */
   /*          access the FCS value.  It's place in this structure is   */
   /*          ONLY to show the programmer that the FCS Member follows  */
   /*          the Variable Data Member which is of varying length.     */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_C_Frame_Packet_t
{
   L2CAP_Data_Packet_Header_t L2CAP_Data_Packet_Header;
   L2CAP_C_Packet_t           C_Packet;
} __PACKED_STRUCT_END__ L2CAP_C_Frame_Packet_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an L2CAP C Frame  */
   /* Packet Structure based upon the size of the Data associated with  */
   /* the Packet.  The first parameter to this MACRO is the size (in    */
   /* Bytes) of the Payload that is to be part of the Packet.           */
#define L2CAP_C_FRAME_PACKET_SIZE(_x)                   (sizeof(L2CAP_Data_Packet_Header_t) + L2CAP_C_PACKET_SIZE(_x))

   /* The following structure represents the structure of the L2CAP LE  */
   /* Frame for an L2CAP LE connection oriented channel.                */
typedef __PACKED_STRUCT_BEGIN__ struct _tagL2CAP_LE_Frame_Packet_t
{
   L2CAP_Data_Packet_Header_t L2CAP_Data_Packet_Header;
   NonAlignedWord_t           SDU_Length;
   Byte_t                     Variable_Data[1];
} __PACKED_STRUCT_END__ L2CAP_LE_Frame_Packet_t;

   /* The following MACRO is a utility MACRO that exists to aid code    */
   /* readability to Determine the size (in Bytes) of an L2CAP LE Frame */
   /* Packet Structure based upon the size of the Data associated with  */
   /* the Packet.  The first parameter to this MACRO is the size (in    */
   /* Bytes) of the Payload that is to be part of the Packet.           */
#define L2CAP_LE_FRAME_PACKET_SIZE(_x)                  ((sizeof(L2CAP_LE_Frame_Packet_t) - sizeof(Byte_t)) + (unsigned int)(_x))

#endif
