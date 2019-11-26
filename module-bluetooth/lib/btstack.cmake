set(BT_GLU "${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/glucode/")
set(BT_INT "${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/interface/bluekitchen/")
set(BT_STACK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/lib/btstack")


set(BT_CORE
    ${BT_STACK_ROOT}/src/btstack_memory.c
    ${BT_STACK_ROOT}/src/btstack_linked_list.c
    ${BT_STACK_ROOT}/src/btstack_memory_pool.c
    ${BT_STACK_ROOT}/src/btstack_run_loop.c
    ${BT_STACK_ROOT}/src/btstack_util.c
    )

set(BT_COMMON
    ${BT_STACK_ROOT}/src/ad_parser.c
    ${BT_STACK_ROOT}/src/hci.c
    ${BT_STACK_ROOT}/src/hci_cmd.c
    ${BT_STACK_ROOT}/src/hci_dump.c
    ${BT_STACK_ROOT}/src/l2cap.c
    ${BT_STACK_ROOT}/src/l2cap_signaling.c
    ${BT_STACK_ROOT}/src/btstack_audio.c
    ${BT_STACK_ROOT}/src/btstack_tlv.c
    ${BT_STACK_ROOT}/src/btstack_crypto.c
    #${BT_STACK_ROOT}/src/3rd-party/micro-ecc/uECC.c
    #${BT_STACK_ROOT}/src/sm.c
    )

set(BT_CLASSIC
    ${BT_STACK_ROOT}/src/classic/btstack_link_key_db_memory.c
    ${BT_STACK_ROOT}/src/classic/sdp_util.c
    ${BT_STACK_ROOT}/src/classic/gatt_sdp.c
    ${BT_STACK_ROOT}/src/classic/spp_server.c
    ${BT_STACK_ROOT}/src/classic/rfcomm.c
    ${BT_STACK_ROOT}/src/classic/bnep.c
    ${BT_STACK_ROOT}/src/classic/btstack_link_key_db_memory.c
    ${BT_STACK_ROOT}/src/classic/pan.c
    ${BT_STACK_ROOT}/src/classic/sdp_server.c
    ${BT_STACK_ROOT}/src/classic/device_id_server.c
    )

set(BNEP_LWIP
    ${BT_STACK_ROOT}/platform/lwip/bnep_lwip.c
    )

# ATT
# 	${BT_STACK_ROOT}/att_dispatch.c
#
# GATT_SERVER
# 	${BT_STACK_ROOT}/att_db.c
# 	${BT_STACK_ROOT}/att_server.c
#
# GATT_CLIENT
# 	${BT_STACK_ROOT}/gatt_client.c

    # PAN
    # 	${BT_STACK_ROOT}/pan.c

    # MBEDTLS = 					\
    # 	${BT_STACK_ROOT}/bignum.c 				\
    # 	${BT_STACK_ROOT}/ecp.c 					\
    # 	${BT_STACK_ROOT}/ecp_curves.c 			\
    # 	${BT_STACK_ROOT}/sm_mbedtls_allocator.c  \
    # 	${BT_STACK_ROOT}/memory_buffer_alloc.c   \
    # 	${BT_STACK_ROOT}/platform.c 				\
    #
    # LWIP_CORE_SRC  = init.c mem.c memp.c netif.c udp.c ip.c pbuf.c inet_chksum.c def.c tcp.c tcp_in.c tcp_out.c timeouts.c sys_arch.c
    # LWIP_IPV4_SRC  = acd.c dhcp.c etharp.c icmp.c ip4.c ip4_frag.c ip4_addr.c
    # LWIP_NETIF_SRC = ethernet.c
    # LWIP_HTTPD = altcp_proxyconnect.c fs.c httpd.c
    # LWIP_SRC = ${LWIP_CORE_SRC} ${LWIP_IPV4_SRC} ${LWIP_NETIF_SRC} ${LWIP_HTTPD} dhserver.c
    # # List of files for Bluedroid SBC codec
    # include ${BTSTACK_ROOT}/3rd-party/bluedroid/decoder/Makefile.inc
    # include ${BTSTACK_ROOT}/3rd-party/bluedroid/encoder/Makefile.inc

    # SBC_DECODER += \
    # 	btstack_sbc_plc.c \
    # 	btstack_sbc_decoder_bluedroid.c \
    #
    # SBC_ENCODER += \
    # 	btstack_sbc_encoder_bluedroid.c \
    # 	hfp_msbc.c \
    #
    # CVSD_PLC = \
    # 	btstack_cvsd_plc.c \
    #
    # AVDTP += \
    # 	avdtp_util.c           \
    # 	avdtp.c                \
    # 	avdtp_initiator.c      \
    # 	avdtp_acceptor.c       \
    # 	avdtp_source.c 	       \
    # 	avdtp_sink.c           \
    # 	a2dp_source.c          \
    # 	a2dp_sink.c            \
    # 	btstack_ring_buffer.c \

set(TARGET_LIBRARIES_INCLUDES
    "${BT_INT}"
    ${BT_STACK_ROOT}/platform/freertos/
    ${BT_STACK_ROOT}/platform/embedded/
    ${BT_STACK_ROOT}/chipset/cc256x

    ${BT_STACK_ROOT}/src/ble
    ${BT_STACK_ROOT}/src/classic
    ${BT_STACK_ROOT}/src
    ${BT_STACK_ROOT}/3rd-party/bluedroid/decoder/include
    ${BT_STACK_ROOT}/3rd-party/bluedroid/encoder/include
    ${BT_STACK_ROOT}/3rd-party/hxcmod-player
    # ${BT_STACK_ROOT}/3rd-party/lwip/core/src/include/
    ${BT_STACK_ROOT}/3rd-party/lwip/dhcp-server
    ${BT_STACK_ROOT}/3rd-party/md5
    ${BT_STACK_ROOT}/3rd-party/micro-ecc
    ${BT_STACK_ROOT}/3rd-party/yxml
    # ${BT_STACK_ROOT}/platform/lwip
    )

# pseudocode
#if(PLATFORM LINUX) {
#btstack_stdin_posix.c
#&& include from posix too
#}

if(${PROJECT_TARGET} STREQUAL "TARGET_Linux")
message("Linux specyfic includes")
message("-----------------------")
    list(APPEND TARGET_LIBRARIES_INCLUDES
        ${BT_STACK_ROOT}/platform/posix/
        )
else ()
endif()

include(${CMAKE_SOURCE_DIR}/module-lwip/lwip-includes.cmake)
list(APPEND TARGET_LIBRARIES_INCLUDES ${LWIP_INCLUDE_DIRS})
list(APPEND TARGET_LIBRARIES_INCLUDES
    ${BT_STACK_ROOT}/platform/lwip
    )

set(BOARD_DIR_SOURCES
            ${BT_INT}/Worker.cpp
            ${BT_INT}/GAP.cpp
            ${BT_INT}/PAN.cpp

            ${BT_GLU}/bluetooth_init_cc2564C_1.0.c
            ${BT_GLU}/btstack_uart_block_rt1051.cpp
            ${BT_GLU}/btstack_uart_block_rt1051.h
            ${BT_GLU}/hal_time_ms.c
            ${BT_STACK_ROOT}/chipset/cc256x/btstack_chipset_cc256x.c
            ${BT_STACK_ROOT}/platform/freertos/btstack_run_loop_freertos.c
            ${BT_STACK_ROOT}/src/hci_transport_h4.c
            ${BT_CORE}
            ${BT_COMMON}
            ${BT_CLASSIC}
            ${BNEP_LWIP}
    )

if(${PROJECT_TARGET} STREQUAL "TARGET_Linux")
message("Linux specyfic sources")
message("----------------------")
# include(${CMAKE_CURRENT_SOURCE_DIR}/targets/Target_Linux.cmake)
list(APPEND BOARD_DIR_SOURCES
    # ${BOARD_SOURCES}
    ${BT_STACK_ROOT}/platform/posix/btstack_stdin_posix.c
    ${BT_STACK_ROOT}/platform/posix/btstack_uart_block_posix.c
    ${BT_STACK_ROOT}/platform/posix/btstack_run_loop_posix.c
    ${BT_STACK_ROOT}/platform/posix/btstack_tlv_posix.c
    )
else()
endif()
