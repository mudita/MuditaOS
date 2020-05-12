cmake_minimum_required(VERSION 3.12)

project(module-gui VERSION 1.0
        DESCRIPTION "GUI library dedicated for pure phone.")

if( NOT ${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    include(targets/Target_Cross.cmake)
else()
	set(CMAKE_CXX_STANDARD 17)
    include(targets/Target_Linux.cmake)
endif()


add_library(${PROJECT_NAME} STATIC ${SOURCES} ${BOARD_SOURCES})
include(gui/CMakeLists.txt)
include(gui/core/CMakeLists.txt)
include(gui/widgets/CMakeLists.txt)
include(gui/input/CMakeLists.txt)

include_directories( ../module-vfs )
include_directories( ../module-utils )

target_link_libraries(${PROJECT_NAME} PUBLIC module-utils module-vfs ${TARGET_LIBRARIES} )

# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PUBLIC ${BOARD_DIR_INCLUDES} )
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})
target_compile_features(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_FEATURES})
target_compile_options(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_OPTIONS})
target_link_options(${PROJECT_NAME} PUBLIC ${TARGET_LINK_OPTIONS})


target_include_directories(${PROJECT_NAME}

        PUBLIC

        ${CMAKE_CURRENT_SOURCE_DIR}
)


target_compile_definitions(${PROJECT_NAME} PUBLIC )
add_subdirectory(test)
