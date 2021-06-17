set (LWIP_DIR ${CMAKE_SOURCE_DIR}/third-party/lwip/src)
set (LWIP_CONTRIB_DIR ${CMAKE_SOURCE_DIR}/third-party/lwip/src/contrib)

set (LWIP_INCLUDE_DIRS
    "${LWIP_DIR}/src/include"
    "${LWIP_DIR}/contrib"
    "${LWIP_DIR}/contrib/ports/freertos/include"
    "${CMAKE_SOURCE_DIR}/module-lwip/includes/"
    "${CMAKE_SOURCE_DIR}/module-lwip/lib/"
)

set(LWIP_LIBRARIES lwipcontribapps lwipallapps lwipcore)
