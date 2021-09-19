project(module-gui VERSION 1.0
        DESCRIPTION "GUI library dedicated for pure phone.")

module_is_test_entity()

add_library(${PROJECT_NAME} STATIC ${SOURCES})
include(gui/CMakeLists.txt)
include(gui/core/CMakeLists.txt)
include(gui/widgets/CMakeLists.txt)
include(gui/input/CMakeLists.txt)
include(gui/dom/CMakeLists.txt)

include_directories( ../module-utils )

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        ${TARGET_LIBRARIES}
        module-bsp
        eventstore
        utils-math
        module-utils
        sys-phonemodes
        sys-service
        sys-watchdog
        service-bluetooth
        utf8
    PRIVATE
        i18n
        json::json
        Microsoft.GSL::GSL
        pugixml::pugixml
        utils-time
)

# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/core
)


target_compile_definitions(${PROJECT_NAME} PUBLIC )

if (${ENABLE_TESTS})
    add_subdirectory(test/test-catch)
    add_subdirectory(test/test-catch-text)
    add_subdirectory(test/test-google)
endif ()
