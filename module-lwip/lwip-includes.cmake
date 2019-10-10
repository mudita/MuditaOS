set (LWIP_DIR ${CMAKE_SOURCE_DIR}/module-lwip/lib/lwip)
set (LWIP_CONTRIB_DIR ${CMAKE_SOURCE_DIR}/module-lwip/lib/lwip/contrib)

set (LWIP_INCLUDE_DIRS
    "${LWIP_DIR}/src/include"
    "${LWIP_DIR}/contrib"
    "${LWIP_DIR}/contrib/ports/freertos/include"
    "${CMAKE_SOURCE_DIR}/module-lwip/includes/"
)

set(LWIP_LIBRARIES lwipcontribapps lwipallapps lwipcore)
