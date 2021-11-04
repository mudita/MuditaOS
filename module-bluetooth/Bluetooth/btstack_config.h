// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

//
// btstack_config.h for generic POSIX H4 port
//

#ifndef __BTSTACK_CONFIG
#define __BTSTACK_CONFIG

// Port related features
#define HAVE_MALLOC
#ifndef TARGET_RT1051
#define HAVE_POSIX_FILE_IO
#endif
#define HAVE_BTSTACK_STDIN
#define HAVE_POSIX_TIME
#define HAVE_EM9304_PATCH_CONTAINER
// BTstack features that can be enabled
// #define ENABLE_BLE
#define ENABLE_CLASSIC
#define ENABLE_HFP_WIDE_BAND_SPEECH
#define ENABLE_L2CAP_ENHANCED_RETRANSMISSION_MODE
// #define ENABLE_LE_CENTRAL
// #define ENABLE_LE_PERIPHERAL
// #define ENABLE_LE_SECURE_CONNECTIONS
// #define ENABLE_MICRO_ECC_FOR_LE_SECURE_CONNECTIONS
// #define ENABLE_LE_DATA_CHANNELS
// #define ENABLE_LE_DATA_LENGTH_EXTENSION

// #define ENABLE_LOG_ERROR
// #define ENABLE_LOG_INFO
// #define ENABLE_LOG_DEBUG

#define ENABLE_SCO_OVER_HCI
#define ENABLE_SDP_DES_DUMP
// #define ENABLE_EHCILL

// BTstack configuration. buffers, sizes, ...
#define HCI_INCOMING_PRE_BUFFER_SIZE 14 // sizeof benep heade, avoid memcpy
//#define HCI_ACL_PAYLOAD_SIZE         (1691 + 4)
#define HCI_ACL_PAYLOAD_SIZE         (1021+4)

#define ENABLE_GATT_CLIENT_PAIRING
#define ENABLE_L2CAP_ENHANCED_RETRANSMISSION_MODE
#define ENABLE_CC256X_BAUDRATE_CHANGE_FLOWCONTROL_BUG_WORKAROUND
//#define HAVE_EMBEDDED_TIME_MS

//#define MAX_NR_HCI_CONNECTIONS                    4
//#define MAX_NR_L2CAP_SERVICES                     5
//#define MAX_NR_L2CAP_CHANNELS                     6
//#define MAX_NR_RFCOMM_MULTIPLEXERS                2
//#define MAX_NR_RFCOMM_SERVICES                    3
//#define MAX_NR_RFCOMM_CHANNELS                    3
//#define MAX_NR_BTSTACK_LINK_KEY_DB_MEMORY_ENTRIES 10

// As an option - much slower (according to docs)
// HCI Controller to Host Flow Control
// #define ENABLE_HCI_CONTROLLER_TO_HOST_FLOW_CONTROL
//
// // Interal ring buffer: 21 kB
// #define HCI_HOST_ACL_PACKET_NUM 20
// #define HCI_HOST_ACL_PACKET_LEN 1024
// #define HCI_HOST_SCO_PACKET_NUM 10
// #define HCI_HOST_SCO_PACKET_LEN 60
#endif
