cmake_minimum_required(VERSION 3.12)
include(thirdparty)

project(module-vfs VERSION 1.0
        DESCRIPTION "VFS module library")

set(FREERTOS_FAT_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/board/freeRTOS_FAT/ff_crc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/freeRTOS_FAT/ff_dir.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/freeRTOS_FAT/ff_error.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/freeRTOS_FAT/ff_fat.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/freeRTOS_FAT/ff_file.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/freeRTOS_FAT/ff_format.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/freeRTOS_FAT/ff_ioman.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/freeRTOS_FAT/ff_locking.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/freeRTOS_FAT/ff_memory.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/freeRTOS_FAT/ff_stdio.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/freeRTOS_FAT/ff_string.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/freeRTOS_FAT/ff_sys.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/freeRTOS_FAT/ff_time.c
        )

third_party_source_optimization(${FREERTOS_FAT_SOURCES})

# Disable some warnings due to ugly embedded hacks
# in the FREERTOS FAT implementation
if(${PROJECT_TARGET}  STREQUAL "TARGET_Linux")
set_property(SOURCE ${FREERTOS_FAT_SOURCES} APPEND_STRING PROPERTY COMPILE_FLAGS
" -Wno-overflow -Wno-type-limits -Wno-format -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast")
endif()

set(PROJECT_INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/board/freeRTOS_FAT/include
        ${CMAKE_CURRENT_SOURCE_DIR}/board/free_rtos_custom/include
        )

set(SOURCES ""
        ${FREERTOS_FAT_SOURCES}
        vfs-utils.cpp
        vfs.cpp
)

if(NOT ${PROJECT_TARGET}  STREQUAL "TARGET_Linux")
    include(targets/Target_Cross.cmake)
endif()

if(${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
    include(targets/Target_RT1051.cmake)
elseif(${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    include(targets/Target_Linux.cmake)
else()
    message(FATAL_ERROR "Invalid target!")
endif()

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${BOARD_SOURCES})


# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PUBLIC ${BOARD_DIR_INCLUDES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_BINARY_DIR})
target_compile_features(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_FEATURES})
target_compile_options(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_OPTIONS})
target_link_options(${PROJECT_NAME} PUBLIC ${TARGET_LINK_OPTIONS})

target_compile_definitions(${PROJECT_NAME}

        PUBLIC
)

target_include_directories(${PROJECT_NAME}

        PUBLIC

        ${CMAKE_CURRENT_SOURCE_DIR}


)

target_link_libraries(${PROJECT_NAME} ${TARGET_LIBRARIES} module-os module-bsp module-utils)

if(${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    target_link_libraries(${PROJECT_NAME} -lstdc++fs)
endif()

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif ()
