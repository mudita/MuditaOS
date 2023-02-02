project(module-cellular VERSION 1.0
        DESCRIPTION "Cellular module library")

include(SerialPort)

module_is_test_entity()

set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/modem/ATParser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/modem/ATStream.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/modem/ATURCStream.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/modem/ATCommon.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/modem/mux/DLCChannel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/modem/mux/CellularMux.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/modem/mux/CellularMuxData.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/Urc.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/UrcQind.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/UrcCusd.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/UrcCtze.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/UrcCreg.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/UrcCmti.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/UrcClip.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/UrcCpin.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/UrcQiurc.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/UrcPoweredDown.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/UrcRing.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/UrcQSimstat.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/UrcResponse.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/UrcFactory.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/Commands.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/Cmd.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/ATFactory.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/response.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/cmd/src/CSCA.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/cmd/src/QECCNUM.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/cmd/src/CLCC.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/cmd/src/CFUN.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/cmd/src/CPBR.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/cmd/src/QNWINFO.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/cmd/src/QSIMSTAT.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/cmd/src/QCFGUsbnet.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/cmd/src/CSQ.cpp
        )

add_library(${PROJECT_NAME} STATIC ${SOURCES})

# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
define_serial(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/at/
        ${CMAKE_SOURCE_DIR}/module-services

)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        date::date
        eventstore
        module-bsp
        module-db
        module-services
        module-sys
        module-utils
        module-vfs
        service-appmgr
        service-cellular
        service-eink

    PRIVATE
        Microsoft.GSL::GSL
	re2::re2
        time-constants
)

if (${ENABLE_TESTS})
    add_subdirectory( test )
endif()

set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/modem/mux/DLCChannel.cpp ${CMAKE_CURRENT_SOURCE_DIR}/modem/mux/CellularMuxData.cpp PROPERTIES COMPILE_OPTIONS "-O0;-fno-omit-frame-pointer;-ggdb3")
