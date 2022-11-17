# This modul checks if "SERIAL_PORT" for linux target variable is set
# if yes define_serial_port macro can be used to set proper definitions
#
#####
#   define_serial(TARGET_NAME)
#
#  this checks if SERIAL_PORT variable is defined and if so, add proper defines
#  for the specified target
#
#####
#   message_serial_status()
#
#  This macro can be used to message if and which port is used

include(Colours)

macro(define_serial)
    if(NOT ${ARGC} EQUAL 1 )
        message(WARNING "No target specified!")
    else()
        if(${PROJECT_TARGET} STREQUAL "TARGET_Linux")
            if (SERIAL_PORT)
                set(SERIAL_PORT_DEF "SERIAL_PORT=\"${SERIAL_PORT}\"")
                target_compile_definitions(
                    ${ARGV0}
                    PUBLIC
                    ${SERIAL_PORT_DEF}
                )
           endif()
       endif()
    endif()
endmacro()

macro(message_serial_status)
    if(${PROJECT_TARGET} STREQUAL "TARGET_Linux")
        if (SERIAL_PORT_DEF)
            message("Using serial port: ${SERIAL_PORT_DEF}")
        else()
            message ("${Orange}Serial Port not defined, modem support disabled! ${ColourReset}")
            message ("${Orange}add -DSERIAL_PORT=<path to serial> where modem is connected ${ColourReset}")
            message ("${Orange}ex.: \n ./configure.sh linux Debug -DSERIAL_PORT=/dev/ttyUSB0 ${ColourReset}")
        endif()
    endif()
endmacro()
