set (LWIP_DIR ${CMAKE_SOURCE_DIR}/third-party/lwip/lwip)
set (LWIP_CONTRIB_DIR ${CMAKE_SOURCE_DIR}/third-party/lwip/lwip/contrib)

set (LWIP_INCLUDE_DIRS
    "${LWIP_DIR}/src/include"
    "${LWIP_DIR}/contrib"
    "${LWIP_DIR}/contrib/ports/freertos/include"
    "${CMAKE_SOURCE_DIR}/third-party/lwip/includes/"
    "${CMAKE_SOURCE_DIR}/third-party/lwip/"
)

#set(LWIP_LIBRARIES lwipcontribapps lwipallapps lwipcore)
