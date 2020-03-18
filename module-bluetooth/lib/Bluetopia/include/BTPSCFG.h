/*****< btpscfg.h >************************************************************/
/*      Copyright 2009 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*      Copyright 2015 Texas Instruments Incorporated.                        */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  BTPSCFG - Stonestreet One Bluetooth Protocol Stack configuration          */
/*            directives/constants.  The information contained in this file   */
/*            controls various compile time parameters that are needed to     */
/*            build Bluetopia for a specific platform.                        */
/*                                                                            */
/*  Author:  Damon Lange                                                      */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   06/09/09  D. Lange       Initial creation.                               */
/*   09/12/16  D. Horowitz    Update Features.                                */
/******************************************************************************/
#ifndef __BTPSCFGH__
#define __BTPSCFGH__

#include "BTPSKRNL.h"           /* BTPS Kernel Prototypes/Constants.          */

   /* Internal Timer Module Configuration.                              */
#define BTPS_CONFIGURATION_TIMER_MAXIMUM_NUMBER_CONCURRENT_TIMERS            16
#define BTPS_CONFIGURATION_TIMER_TIMER_THREAD_STACK_SIZE                    512
#define BTPS_CONFIGURATION_TIMER_MINIMUM_TIMER_RESOLUTION_MS                 50

   /* Generic HCI Driver Interface Configuration.                       */
   /* * NOTE * The Receive Packet Buffer Size is the size (in bytes) of */
   /*          the largest HCI Packet (including header) that can be    */
   /*          received.                                                */
#define BTPS_CONFIGURATION_HCI_DRIVER_RECEIVE_PACKET_BUFFER_SIZE             (HCI_CALCULATE_ACL_DATA_SIZE(HCI_PACKET_TYPE_3_DH5_MAXIMUM_PAYLOAD_SIZE))
#define BTPS_CONFIGURATION_HCI_DRIVER_3WIRE_DEFAULT_SLIDING_WINDOW_SIZE          4
#define BTPS_CONFIGURATION_HCI_DRIVER_3WIRE_DEFAULT_SUPPORT_CRC                  1
#define BTPS_CONFIGURATION_HCI_DRIVER_3WIRE_DEFAULT_SUPPORT_OOF_FLOW_CONTROL     0
#define BTPS_CONFIGURATION_HCI_DRIVER_3WIRE_DEFAULT_IDLE_TIME_MS               350
#define BTPS_CONFIGURATION_HCI_DRIVER_3WIRE_DEFAULT_RETRANSMIT_TIME_MS         250
#define BTPS_CONFIGURATION_HCI_DRIVER_3WIRE_DEFAULT_ACKNOWLEDGE_DELAY_MS     (BTPS_CONFIGURATION_TIMER_MINIMUM_TIMER_RESOLUTION_MS)
#define BTPS_CONFIGURATION_HCI_DRIVER_BCSP_UART_BUFFER_LENGTH                16384
#define BTPS_CONFIGURATION_HCI_DRIVER_BCSP_MAXIMUM_MESSAGE_LENGTH             4096
#define BTPS_CONFIGURATION_HCI_DRIVER_H4DS_DEFAULT_IDLE_TIME_MS                350
#define BTPS_CONFIGURATION_HCI_DRIVER_H4DS_DEFAULT_TRANSMIT_FLUSH_TIME_MS     1000
#define BTPS_CONFIGURATION_HCI_DRIVER_H4DS_DEFAULT_SYNC_TIME_MS                250
#define BTPS_CONFIGURATION_HCI_DRIVER_H4DS_DEFAULT_CONF_TIME_MS                250
#define BTPS_CONFIGURATION_HCI_DRIVER_H4DS_DEFAULT_WAKE_UP_MESSAGE_TIME_MS     250
#define BTPS_CONFIGURATION_HCI_DRIVER_H4DS_DEFAULT_TICKS_PER_WAKE_UP            25
#define BTPS_CONFIGURATION_HCI_DRIVER_H4DS_DEFAULT_WAKE_UP_MESSAGE_COUNT        50
#define BTPS_CONFIGURATION_HCI_DRIVER_SIBS_DEFAULT_TX_IDLE_TIME_MS             350
#define BTPS_CONFIGURATION_HCI_DRIVER_SIBS_DEFAULT_RETRANSMIT_TIME_MS          100
#define BTPS_CONFIGURATION_HCI_DRIVER_SIBS_DEFAULT_AGRESSIVE_SLEEP               0

   /* Host Controller Interface Configuration.                          */
   /* * NOTE * The maximum supported HCI packet size is the largest     */
   /*          size (in bytes) of an HCI Packet (including header) that */
   /*          can be sent or received.                                 */
#define BTPS_CONFIGURATION_HCI_MAXIMUM_SUPPORTED_HCI_PACKET_SIZE           (BTPS_CONFIGURATION_HCI_DRIVER_RECEIVE_PACKET_BUFFER_SIZE)
#define BTPS_CONFIGURATION_HCI_SYNCHRONOUS_WAIT_TIMEOUT_MS                 5000
#define BTPS_CONFIGURATION_HCI_DISPATCH_THREAD_STACK_SIZE                  3584
#define BTPS_CONFIGURATION_HCI_NUMBER_DISPATCH_MAILBOX_SLOTS               48
#define BTPS_CONFIGURATION_HCI_DISPATCH_SCHEDULER_TIME_MS                  (BTPS_MINIMUM_SCHEDULER_RESOLUTION)
#define BTPS_CONFIGURATION_HCI_NUMBER_OF_ACL_DATA_PACKETS                  0
#define BTPS_CONFIGURATION_HCI_NUMBER_OF_SCO_DATA_PACKETS                  0

   /* L2CAP Configuration.                                              */
#define BTPS_CONFIGURATION_L2CAP_MAXIMUM_SUPPORTED_STACK_MTU               (HCI_PACKET_TYPE_3_DH5_MAXIMUM_PAYLOAD_SIZE - L2CAP_DATA_PACKET_HEADER_SIZE)
#define BTPS_CONFIGURATION_L2CAP_DEFAULT_RTX_TIMER_TIMEOUT_S                 15
#define BTPS_CONFIGURATION_L2CAP_DEFAULT_ERTX_TIMER_TIMEOUT_S               300
#define BTPS_CONFIGURATION_L2CAP_DEFAULT_IDLE_TIMER_TIMEOUT_S                 2
#define BTPS_CONFIGURATION_L2CAP_DEFAULT_CONFIG_TIMER_TIMEOUT_S              60
#define BTPS_CONFIGURATION_L2CAP_DEFAULT_RECEIVE_TIMER_TIMEOUT_S             60
#define BTPS_CONFIGURATION_L2CAP_DEFAULT_LINK_CONNECT_REQUEST_CONFIG       (cqNoRoleSwitch)
#define BTPS_CONFIGURATION_L2CAP_DEFAULT_LINK_CONNECT_RESPONSE_CONFIG      (csMaintainCurrentRole)
#define BTPS_CONFIGURATION_L2CAP_ACL_CONNECTION_DELAY_TIMEOUT_MS              0

   /* RFCOMM Configuration.                                             */
#define BTPS_CONFIGURATION_RFCOMM_DEFAULT_LINK_TIMEOUT_MS                  (L2CAP_LINK_TIMEOUT_DEFAULT_VALUE)
#define BTPS_CONFIGURATION_RFCOMM_DEFAULT_ACKNOWLEDGEMENT_TIMER_S            20
#define BTPS_CONFIGURATION_RFCOMM_DEFAULT_RESPONSE_TIMER_S                   20
#define BTPS_CONFIGURATION_RFCOMM_EXTENDED_ACKNOWLEDGEMENT_TIMER_S           60
#define BTPS_CONFIGURATION_RFCOMM_CONNECTION_SUPERVISOR_TIMER_S              30
#define BTPS_CONFIGURATION_RFCOMM_DISCONNECT_SUPERVISOR_TIMER_S               5
#define BTPS_CONFIGURATION_RFCOMM_MAXIMUM_SUPPORTED_STACK_FRAME_SIZE      ((RFCOMM_FRAME_SIZE_MAXIMUM_VALUE > L2CAP_MAXIMUM_SUPPORTED_STACK_MTU)?L2CAP_MAXIMUM_SUPPORTED_STACK_MTU:RFCOMM_FRAME_SIZE_MAXIMUM_VALUE)
#define BTPS_CONFIGURATION_RFCOMM_DEFAULT_MAXIMUM_NUMBER_QUEUED_DATA_PACKETS  6
#define BTPS_CONFIGURATION_RFCOMM_DEFAULT_QUEUED_DATA_PACKETS_THRESHOLD       2

   /* SCO Configuration.                                                */
#define BTPS_CONFIGURATION_SCO_DEFAULT_BUFFER_SIZE                            0
#define BTPS_CONFIGURATION_SCO_DEFAULT_CONNECTION_MODE                    (scmEnableConnections)
#define BTPS_CONFIGURATION_SCO_DEFAULT_PHYSICAL_TRANSPORT                 (sptCodec)

   /* SDP Configuration.                                                */
#define BTPS_CONFIGURATION_SDP_PDU_RESPONSE_TIMEOUT_MS                    10000
#define BTPS_CONFIGURATION_SDP_DEFAULT_LINK_TIMEOUT_MS                    (L2CAP_LINK_TIMEOUT_DEFAULT_VALUE)
#define BTPS_CONFIGURATION_SDP_DEFAULT_DISCONNECT_MODE                    (dmAutomatic)

   /* SPP Configuration.                                                */
#define BTPS_CONFIGURATION_SPP_DEFAULT_SERVER_CONNECTION_MODE             (smAutomaticAccept)
#define BTPS_CONFIGURATION_SPP_MINIMUM_SUPPORTED_STACK_BUFFER_SIZE        (RFCOMM_FRAME_SIZE_MINIMUM_VALUE)
#define BTPS_CONFIGURATION_SPP_MAXIMUM_SUPPORTED_STACK_BUFFER_SIZE        65535
#define BTPS_CONFIGURATION_SPP_DEFAULT_TRANSMIT_BUFFER_SIZE               (BTPS_CONFIGURATION_SPP_DEFAULT_FRAME_SIZE * 1)
#define BTPS_CONFIGURATION_SPP_DEFAULT_RECEIVE_BUFFER_SIZE                (BTPS_CONFIGURATION_SPP_DEFAULT_FRAME_SIZE * 3)
#define BTPS_CONFIGURATION_SPP_DEFAULT_FRAME_SIZE                         (329)

   /* OTP Configuration.                                                */
#define BTPS_CONFIGURATION_OTP_OBJECT_INFO_MAXIMUM_NAME_LENGTH              288
#define BTPS_CONFIGURATION_OTP_OBJECT_INFO_MAXIMUM_TYPE_LENGTH               64
#define BTPS_CONFIGURATION_OTP_OBJECT_INFO_MAXIMUM_OWNER_LENGTH              64
#define BTPS_CONFIGURATION_OTP_OBJECT_INFO_MAXIMUM_GROUP_LENGTH              64

   /* ANT Configuration.                                                */
#define BTPS_CONFIGURATION_ANT_SUPPORT_MESSAGE_PARSING                        1

   /* AVCTP Configuration.                                              */
#define BTPS_CONFIGURATION_AVCTP_DEFAULT_LINK_TIMEOUT                     (L2CAP_LINK_TIMEOUT_DEFAULT_VALUE)
#define BTPS_CONFIGURATION_AVCTP_MAXIMUM_SUPPORTED_MTU                    (L2CAP_MAXIMUM_SUPPORTED_STACK_MTU)
#define BTPS_CONFIGURATION_AVCTP_BROWSING_CHANNEL_SUPPORTED_DEFAULT       (FALSE)

#define BTPS_CONFIGURATION_AVCTP_MAXIMUM_NUMBER_PROFILE_CONNECTIONS                   8
#define BTPS_CONFIGURATION_AVCTP_SUPPORT_SDP_LANGUAGE_BASE_ID                         1
#define BTPS_CONFIGURATION_AVCTP_SUPPORT_BROWSING_CHANNEL                             1

#define BTPS_CONFIGURATION_AVCTP_DEFAULT_BROWSING_CHANNEL_PDU_SIZE                 1024
#define BTPS_CONFIGURATION_AVCTP_DEFAULT_BROWSING_CHANNEL_TX_WINDOW                  10
#define BTPS_CONFIGURATION_AVCTP_DEFAULT_BROWSING_CHANNEL_MAX_TX_ATTEMPTS           255
#define BTPS_CONFIGURATION_AVCTP_DEFAULT_BROWSING_CHANNEL_MONITOR_TIMEOUT_MS       2000
#define BTPS_CONFIGURATION_AVCTP_DEFAULT_BROWSING_CHANNEL_RETRANSMISSION_TIMEOUT_MS 300

   /* AVRCP Configuration.                                              */
#define BTPS_CONFIGURATION_AVRCP_SUPPORT_BROWSING_CHANNEL                     (BTPS_CONFIGURATION_AVCTP_SUPPORT_BROWSING_CHANNEL)
#define BTPS_CONFIGURATION_AVRCP_MESSAGE_LIBRARY_ONLY                         0
#define BTPS_CONFIGURATION_AVRCP_SUPPORT_VENDOR_DEPENDENT                     1

#define BTPS_CONFIGURATION_AVRCP_SUPPORT_CONTROLLER_ROLE                      1
#define BTPS_CONFIGURATION_AVRCP_SUPPORT_TARGET_ROLE                          1

   /* BIP Configuration.                                                */
#define BTPS_CONFIGURATION_BIP_MAXIMUM_OBEX_PACKET_LENGTH                  8000

   /* BPP Configuration.                                                */
#define BTPS_CONFIGURATION_BPP_MAXIMUM_OBEX_PACKET_LENGTH                  8000

   /* BTCOMM Configuration.                                             */
#define BTPS_CONFIGURATION_BTCOMM_COM_VCOM_BUFFER_SIZE                    (SPP_FRAME_SIZE_DEFAULT)
#define BTPS_CONFIGURATION_BTCOMM_SPP_TRANSMIT_BUFFER_SIZE                (SPP_BUFFER_SIZE_DEFAULT_TRANSMIT)
#define BTPS_CONFIGURATION_BTCOMM_SPP_RECEIVE_BUFFER_SIZE                 (SPP_BUFFER_SIZE_DEFAULT_RECEIVE)
#define BTPS_CONFIGURATION_BTCOMM_DISPATCH_THREAD_STACK_SIZE              24576
#define BTPS_CONFIGURATION_BTCOMM_NUMBER_DISPATCH_MAILBOX_SLOTS             128

   /* BTSER Configuration.                                              */
#define BTPS_CONFIGURATION_BTSER_SER_VSER_BUFFER_SIZE                     (SPP_FRAME_SIZE_DEFAULT)
#define BTPS_CONFIGURATION_BTSER_SPP_TRANSMIT_BUFFER_SIZE                 (SPP_BUFFER_SIZE_DEFAULT_TRANSMIT)
#define BTPS_CONFIGURATION_BTSER_SPP_RECEIVE_BUFFER_SIZE                  (SPP_BUFFER_SIZE_DEFAULT_RECEIVE)
#define BTPS_CONFIGURATION_BTSER_DISPATCH_THREAD_STACK_SIZE               24576
#define BTPS_CONFIGURATION_BTSER_NUMBER_DISPATCH_MAILBOX_SLOTS              128

   /* DUN Configuration.                                                */
#define BTPS_CONFIGURATION_DUN_SERIAL_BUFFER_SIZE                         (SPP_FRAME_SIZE_DEFAULT)
#define BTPS_CONFIGURATION_DUN_SPP_TRANSMIT_BUFFER_SIZE                   (SPP_BUFFER_SIZE_DEFAULT_TRANSMIT)
#define BTPS_CONFIGURATION_DUN_SPP_RECEIVE_BUFFER_SIZE                    (SPP_BUFFER_SIZE_DEFAULT_RECEIVE)
#define BTPS_CONFIGURATION_DUN_DISPATCH_THREAD_STACK_SIZE                 24576
#define BTPS_CONFIGURATION_DUN_NUMBER_DISPATCH_MAILBOX_SLOTS                128

   /* FAX Configuration.                                                */
#define BTPS_CONFIGURATION_FAX_SERIAL_BUFFER_SIZE                         (SPP_FRAME_SIZE_DEFAULT)
#define BTPS_CONFIGURATION_FAX_SPP_TRANSMIT_BUFFER_SIZE                   (SPP_BUFFER_SIZE_DEFAULT_TRANSMIT)
#define BTPS_CONFIGURATION_FAX_SPP_RECEIVE_BUFFER_SIZE                    (SPP_BUFFER_SIZE_DEFAULT_RECEIVE)
#define BTPS_CONFIGURATION_FAX_DISPATCH_THREAD_STACK_SIZE                 24576
#define BTPS_CONFIGURATION_FAX_NUMBER_DISPATCH_MAILBOX_SLOTS                128

   /* FTP Configuration.                                                */
#define BTPS_CONFIGURATION_FTP_MAXIMUM_OBEX_PACKET_LENGTH                  8000

   /* GAVD Configuration.                                               */
#define BTPS_CONFIGURATION_GAVD_DEFAULT_LINK_TIMEOUT_MS                   (L2CAP_LINK_TIMEOUT_DEFAULT_VALUE)
#define BTPS_CONFIGURATION_GAVD_SIGNALLING_RESPONSE_TIMEOUT_MS             6000
#define BTPS_CONFIGURATION_GAVD_SIGNALLING_DISCONNECT_TIMEOUT_MS           3000
#define BTPS_CONFIGURATION_GAVD_CHANNEL_DELAY_DISCONNECT_TIMEOUT_MS        3000
#define BTPS_CONFIGURATION_GAVD_DATA_PACKET_QUEUEING_FLAGS                (L2CA_QUEUEING_FLAG_LIMIT_BY_PACKETS | L2CA_QUEUEING_FLAG_DISCARD_OLDEST)
#define BTPS_CONFIGURATION_GAVD_MAXIMUM_NUMBER_QUEUED_DATA_PACKETS            0
#define BTPS_CONFIGURATION_GAVD_QUEUED_DATA_PACKETS_THRESHOLD                 0

#define BTPS_CONFIGURATION_GAVD_SUPPORT_SDP_LANGUAGE_BASE_ID                  0
#define BTPS_CONFIGURATION_GAVD_SUPPORT_CONTENT_PROTECTION                    1
#define BTPS_CONFIGURATION_GAVD_SUPPORT_MULTIPLEXING                          1
#define BTPS_CONFIGURATION_GAVD_SUPPORT_REPORTING_CHANNEL                     1
#define BTPS_CONFIGURATION_GAVD_SUPPORT_RECOVERY_CHANNEL                      1
#define BTPS_CONFIGURATION_GAVD_SUPPORT_SECURITY_CONTROL                      1
#define BTPS_CONFIGURATION_GAVD_SUPPORT_HEADER_COMPRESSION                    1
#define BTPS_CONFIGURATION_GAVD_SUPPORT_GET_CHANNEL_INFORMATION               1

   /* HCRP Configuration.                                               */
#define BTPS_CONFIGURATION_HCRP_DEFAULT_LINK_TIMEOUT_MS                   10000

   /* Headset Configuration.                                            */
#define BTPS_CONFIGURATION_HDSET_SUPPORT_HEADSET_ROLE                         1
#define BTPS_CONFIGURATION_HDSET_SUPPORT_GATEWAY_ROLE                         1
#define BTPS_CONFIGURATION_HDSET_SUPPORT_AUDIO_DATA                           1

   /* HFRE Configuration.                                               */
#define BTPS_CONFIGURATION_HFRE_SUPPORT_HANDSFREE_ROLE                        1
#define BTPS_CONFIGURATION_HFRE_SUPPORT_AUDIO_GATEWAY_ROLE                    1
#define BTPS_CONFIGURATION_HFRE_SUPPORT_AUDIO_DATA                            1
#define BTPS_CONFIGURATION_HFRE_SUPPORT_LONG_SPRINTF_MODIFIER                 1
#define BTPS_CONFIGURATION_HFRE_MAXIMUM_INPUT_BUFFER_SIZE                   256

   /* HID Configuration.                                                */
#define BTPS_CONFIGURATION_HID_DEFAULT_LINK_TIMEOUT_MS                    10000
#define BTPS_CONFIGURATION_HID_DATA_PACKET_QUEUEING_FLAGS                (L2CA_QUEUEING_FLAG_LIMIT_BY_PACKETS)
#define BTPS_CONFIGURATION_HID_MAXIMUM_NUMBER_QUEUED_DATA_PACKETS             0
#define BTPS_CONFIGURATION_HID_QUEUED_DATA_PACKETS_THRESHOLD                  0

#define BTPS_CONFIGURATION_HID_SUPPORT_HOST_ROLE                              1
#define BTPS_CONFIGURATION_HID_SUPPORT_DEVICE_ROLE                            1

   /* LAP Configuration.                                                */
#define BTPS_CONFIGURATION_LAP_SERIAL_BUFFER_SIZE                         (SPP_FRAME_SIZE_DEFAULT)
#define BTPS_CONFIGURATION_LAP_SPP_TRANSMIT_BUFFER_SIZE                   (SPP_BUFFER_SIZE_DEFAULT_TRANSMIT)
#define BTPS_CONFIGURATION_LAP_SPP_RECEIVE_BUFFER_SIZE                    (SPP_BUFFER_SIZE_DEFAULT_RECEIVE)
#define BTPS_CONFIGURATION_LAP_DISPATCH_THREAD_STACK_SIZE                 24576
#define BTPS_CONFIGURATION_LAP_NUMBER_DISPATCH_MAILBOX_SLOTS                128

   /* Object Push Configuration (Legacy).                               */
#define BTPS_CONFIGURATION_OBJP_MAXIMUM_OBEX_PACKET_LENGTH                 8000

   /* OPP Configuration.                                                */
#define BTPS_CONFIGURATION_OPP_MAXIMUM_OBEX_PACKET_LENGTH                  8000

   /* PAN Configuration.                                                */
#define BTPS_CONFIGURATION_PAN_DEFAULT_LINK_TIMEOUT_MS                    (L2CAP_LINK_TIMEOUT_DEFAULT_VALUE)
#define BTPS_CONFIGURATION_PAN_DEFAULT_CONTROL_PACKET_RESPONSE_TIMEOUT_MS 10000
#define BTPS_CONFIGURATION_PAN_DISPATCH_THREAD_STACK_SIZE                 32768
#define BTPS_CONFIGURATION_PAN_NUMBER_DISPATCH_MAILBOX_SLOTS                128
#define BTPS_CONFIGURATION_PAN_L2CAP_QUEUE_DEPTH                            256
#define BTPS_CONFIGURATION_PAN_L2CAP_QUEUE_THRESHOLD                         16
#define BTPS_CONFIGURATION_PAN_BROADCAST_QUEUE_DEPTH                       2048
#define BTPS_CONFIGURATION_PAN_BROADCAST_QUEUE_THRESHOLD                      0
#define BTPS_CONFIGURATION_PAN_PACKET_QUEUE_DEPTH                             0
#define BTPS_CONFIGURATION_PAN_PACKET_QUEUE_THRESHOLD                         0

   /* PBAP Configuration.                                               */
#define BTPS_CONFIGURATION_PBAP_MAXIMUM_OBEX_PACKET_LENGTH                  256

#define BTPS_CONFIGURATION_PBAP_SUPPORT_SERVER_ROLE                           1
#define BTPS_CONFIGURATION_PBAP_SUPPORT_CLIENT_ROLE                           1

   /* SYNC Configuration.                                               */
#define BTPS_CONFIGURATION_SYNC_MAXIMUM_OBEX_PACKET_LENGTH                 8000

   /* MAP Configuration.                                                */
#define BTPS_CONFIGURATION_MAP_MAXIMUM_MESSAGE_ACCESS_OBEX_PACKET_LENGTH    256
#define BTPS_CONFIGURATION_MAP_MAXIMUM_NOTIFICATION_OBEX_PACKET_LENGTH      256

#define BTPS_CONFIGURATION_MAP_SUPPORT_SERVER_ROLE                            1
#define BTPS_CONFIGURATION_MAP_SUPPORT_CLIENT_ROLE                            1

   /* ISPP Configuration.                                               */
#define BTPS_CONFIGURATION_ISPP_SUPPORT_SLEEP_COMMAND                         0
#define BTPS_CONFIGURATION_ISPP_SUPPORT_IAP2                                  1
#define BTPS_CONFIGURATION_ISPP_SUPPORT_QUEUE_SESSION_PACKET_POINTER          0

   /* GATT Configuration.                                               */
#define BTPS_CONFIGURATION_GATT_MAXIMUM_SUPPORTED_MTU_SIZE                  (131)
#define BTPS_CONFIGURATION_GATT_DEFAULT_LINK_TIMEOUT                        (L2CAP_LINK_TIMEOUT_DEFAULT_VALUE)
#define BTPS_CONFIGURATION_GATT_DEFAULT_MAXIMUM_NUMBER_QUEUED_DATA_PACKETS  0
#define BTPS_CONFIGURATION_GATT_DEFAULT_QUEUED_DATA_PACKETS_THRESHOLD       0
#define BTPS_CONFIGURATION_GATT_SUPPORT_BR_EDR                              0
#define BTPS_CONFIGURATION_GATT_SUPPORT_LE                                  1
#define BTPS_CONFIGURATION_GATT_SUPPORT_EXCHANGE_MTU_REQUEST                1
#define BTPS_CONFIGURATION_GATT_SUPPORT_SERVER_ROLE                         1
#define BTPS_CONFIGURATION_GATT_SUPPORT_CLIENT_ROLE                         1
#define BTPS_CONFIGURATION_GATT_SUPPORT_REGISTER_CONNECTION_EVENTS          1
#define BTPS_CONFIGURATION_GATT_SUPPORT_UN_REGISTER_CONNECTION_EVENTS       1
#define BTPS_CONFIGURATION_GATT_SUPPORT_READ_BLOB_REQUEST                   1
#define BTPS_CONFIGURATION_GATT_SUPPORT_READ_MULTIPLE_REQUEST               1
#define BTPS_CONFIGURATION_GATT_SUPPORT_WRITE_COMMAND                       1
#define BTPS_CONFIGURATION_GATT_SUPPORT_SIGNED_WRITE_COMMAND                1
#define BTPS_CONFIGURATION_GATT_SUPPORT_WRITE_REQUEST                       1
#define BTPS_CONFIGURATION_GATT_SUPPORT_PREPARE_WRITE_REQUEST               1
#define BTPS_CONFIGURATION_GATT_SUPPORT_HANDLE_VALUE_INDICATION             1
#define BTPS_CONFIGURATION_GATT_SUPPORT_HANDLE_VALUE_NOTIFICATION           1
#define BTPS_CONFIGURATION_GATT_SUPPORT_32_BIT_UUIDS                        1
#define BTPS_CONFIGURATION_GATT_SUPPORT_DYNAMIC_SERVICE_REGISTRATION        0

   /* Audio configuration.                                              */
#define BTPS_CONFIGURATION_AUD_MAXIMUM_CONCURRENT_STREAMS                      3
#define BTPS_CONFIGURATION_AUD_SBC_MINIMUM_BIT_POOL_VALUE                   0x0A
#define BTPS_CONFIGURATION_AUD_SBC_MAXIMUM_BIT_POOL_VALUE                   0x50
#define BTPS_CONFIGURATION_AUD_ENDPOINT_MEDIA_CHANNEL_MTU_SIZE              672
#define BTPS_CONFIGURATION_AUD_ENDPOINT_REPORTING_CHANNEL_MTU_SIZE          1000
#define BTPS_CONFIGURATION_AUD_ENDPOINT_RECOVERY_CHANNEL_MTU_SIZE           1000
#define BTPS_CONFIGURATION_AUD_ENDPOINT_SRC_CONNECT_TIMEOUT_MS              3000
#define BTPS_CONFIGURATION_AUD_ENDPOINT_SNK_CONNECT_TIMEOUT_MS              3000
#define BTPS_CONFIGURATION_AUD_SIGNALLING_DISCONNECT_TIMEOUT_MS                0
#define BTPS_CONFIGURATION_AUD_REMOTE_CONTROL_CONNECT_TIMEOUT_INITIATOR_MS  1000
#define BTPS_CONFIGURATION_AUD_REMOTE_CONTROL_CONNECT_TIMEOUT_ACCEPTOR_MS   1250

#define BTPS_CONFIGURATION_AUD_SUPPORT_REMOTE_CONTROL                          1
#define BTPS_CONFIGURATION_AUD_REMOTE_CONTROL_MAXIMUM_OUTSTANDING_COMMANDS (AVCTP_TRANSACTION_ID_MASK + 1)
#define BTPS_CONFIGURATION_AUD_REMOTE_CONTROL_DEFAULT_FRAGMENTS                4
#define BTPS_CONFIGURATION_AUD_REMOTE_CONTROL_MAXIMUM_FRAGMENTS                16

#define BTPS_CONFIGURATION_AUD_SUPPORT_AAC                                     1
#define BTPS_CONFIGURATION_AUD_AAC_MAXIMUM_PEAK_BIT_RATE_VALUE            264630

#define BTPS_CONFIGURATION_AUD_SUPPORT_MP3                                     1
#define BTPS_CONFIGURATION_AUD_MP3_SUPPORTED_A2DP_BIT_RATE_INDICES        0x7FFE

   /* ANS Configuration.                                                */
#define BTPS_CONFIGURATION_ANS_MAXIMUM_SUPPORTED_INSTANCES                1

   /* BAS Configuration.                                                */
#define BTPS_CONFIGURATION_BAS_MAXIMUM_SUPPORTED_INSTANCES                1

   /* BLS Configuration.                                                */
#define BTPS_CONFIGURATION_BLS_MAXIMUM_SUPPORTED_INSTANCES                1
#define BTPS_CONFIGURATION_BLS_SUPPORT_INTERMEDIATE_CUFF_PRESSURE         1

   /* CSCS Configuration.                                               */
#define BTPS_CONFIGURATION_CSCS_MAXIMUM_SUPPORTED_INSTANCES               1
#define BTPS_CONFIGURATION_CSCS_SUPPORTED_WHEEL_REVOLUTION_DATA           1
#define BTPS_CONFIGURATION_CSCS_SUPPORTED_CRANK_REVOLUTION_DATA           1
#define BTPS_CONFIGURATION_CSCS_SUPPORTED_MULTIPLE_SENSOR_LOCATION        1
#define BTPS_CONFIGURATION_CSCS_SUPPORTED_SC_CONTROL_POINT                BTPS_CONFIGURATION_CSCS_SUPPORTED_MULTIPLE_SENSOR_LOCATION || BTPS_CONFIGURATION_CSCS_SUPPORTED_WHEEL_REVOLUTION_DATA

   /* CTS Configuration.                                                */
#define BTPS_CONFIGURATION_CTS_MAXIMUM_SUPPORTED_INSTANCES                1
#define BTPS_CONFIGURATION_CTS_SUPPORT_REFERENCE_TIME_INFORMATION         1
#define BTPS_CONFIGURATION_CTS_SUPPORT_LOCAL_TIME_INFORMATION             1

   /* DIS Configuration.                                                */
#define BTPS_CONFIGURATION_DIS_MAXIMUM_SUPPORTED_INSTANCES                1
#define BTPS_CONFIGURATION_DIS_MAXIMUM_SUPPORTED_STRING_LENGTH            48
#define BTPS_CONFIGURATION_DIS_SUPPORT_MANUFACTURER_NAME                  1
#define BTPS_CONFIGURATION_DIS_SUPPORT_MODEL_NUMBER                       1
#define BTPS_CONFIGURATION_DIS_SUPPORT_SERIAL_NUMBER                      1
#define BTPS_CONFIGURATION_DIS_SUPPORT_HARDWARE_REVISION                  1
#define BTPS_CONFIGURATION_DIS_SUPPORT_FIRMWARE_REVISION                  1
#define BTPS_CONFIGURATION_DIS_SUPPORT_SOFTWARE_REVISION                  1
#define BTPS_CONFIGURATION_DIS_SUPPORT_SYSTEM_ID                          1
#define BTPS_CONFIGURATION_DIS_SUPPORT_IEEE_CERTIFICATION_DATA            1
#define BTPS_CONFIGURATION_DIS_SUPPORT_PNP_ID                             1

   /* GAP Service Configuration.                                        */
#define BTPS_CONFIGURATION_GAPS_MAXIMUM_SUPPORTED_INSTANCES               1
#define BTPS_CONFIGURATION_GAPS_MAXIMUM_SUPPORTED_DEVICE_NAME             48
#define BTPS_CONFIGURATION_GAPS_SUPPORT_PREFERRED_CONNECTION_PARAMETERS   1

   /* GLS Configuration.                                                */
#define BTPS_CONFIGURATION_GLS_MAXIMUM_SUPPORTED_INSTANCES                1
#define BTPS_CONFIGURATION_GLS_SUPPORT_MEASUREMENT_CONTEXT                1

   /* HRS Configuration.                                                */
#define BTPS_CONFIGURATION_HRS_MAXIMUM_SUPPORTED_INSTANCES                1
#define BTPS_CONFIGURATION_HRS_SUPPORT_BODY_SENSOR_LOCATION               1
#define BTPS_CONFIGURATION_HRS_SUPPORT_ENERGY_EXPENDED_STATUS             1

   /* HTS Configuration.                                                */
#define BTPS_CONFIGURATION_HTS_MAXIMUM_SUPPORTED_INSTANCES                1
#define BTPS_CONFIGURATION_HTS_SUPPORT_TEMPERATURE_TYPE                   1
#define BTPS_CONFIGURATION_HTS_SUPPORT_INTERMEDIATE_TEMPERATURE           1
#define BTPS_CONFIGURATION_HTS_SUPPORT_MEASUREMENT_INTERVAL               1
#define BTPS_CONFIGURATION_HTS_SUPPORT_MEASUREMENT_INTERVAL_WRITE         1

   /* IAS Configuration.                                                */
#define BTPS_CONFIGURATION_IAS_MAXIMUM_SUPPORTED_INSTANCES                1

   /* LLS Configuration.                                                */
#define BTPS_CONFIGURATION_LLS_MAXIMUM_SUPPORTED_INSTANCES                1

   /* NDCS Configuration.                                               */
#define BTPS_CONFIGURATION_NDCS_MAXIMUM_SUPPORTED_INSTANCES               1

   /* PASS Configuration.                                               */
#define BTPS_CONFIGURATION_PASS_MAXIMUM_SUPPORTED_INSTANCES               1

   /* RSCS Configuration.                                               */
#define BTPS_CONFIGURATION_RSCS_MAXIMUM_SUPPORTED_INSTANCES               1

   /* RTUS Configuration.                                               */
#define BTPS_CONFIGURATION_RTUS_MAXIMUM_SUPPORTED_INSTANCES               1

   /* SCPS Configuration.                                               */
#define BTPS_CONFIGURATION_SCPS_MAXIMUM_SUPPORTED_INSTANCES               1
#define BTPS_CONFIGURATION_SCPS_SUPPORT_SCAN_REFRESH                      1

   /* TPS Configuration.                                                */
#define BTPS_CONFIGURATION_TPS_MAXIMUM_SUPPORTED_INSTANCES                1


#endif
