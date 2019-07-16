cmake_minimum_required(VERSION 3.12)

project(module-gui VERSION 1.0
        DESCRIPTION "GUI library dedicated for pure phone.")

set(CMAKE_CXX_STANDARD 14)

if(NOT DEFINED PROJECT_LIB_DIRECTORY )
	set(PROJECT_LIB_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/" CACHE STRING "Output path for static libraries")
	message("Setting PROJECT_LIB_DIRECTORY to ${PROJECT_LIB_DIRECTORY}")
endif()

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_LIB_DIRECTORY})

if(NOT DEFINED PROJECT_BUILD_DIRECTORY )
	set(PROJECT_BUILD_DIRECTORY "${CMAKE_SOURCE_DIR}/build" CACHE STRING "Output directory for building" )
	message("Setting PROJECT_BUILD_DIRECTORY to ${PROJECT_BUILD_DIRECTORY}")
endif()

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BUILD_DIRECTORY})

message("PROJECT TARGET: ${PROJECT_TARGET}")
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

# Host target configuration(mainly used for unit testing)

if(DEFINED BUILD_UNIT_TESTS)

	project("unittest_gui" VERSION 1.0
        DESCRIPTION "GUI tests.")

	add_executable( "unittest_gui" "${CMAKE_CURRENT_LIST_DIR}/test/unittest_gui.cpp")
	target_include_directories( "unittest_gui" 
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/" 
		"${CMAKE_CURRENT_LIST_DIR}/../module-utils"
		"${CMAKE_CURRENT_LIST_DIR}/../module-vfs"
	)
	message(" GUI TEST LIB DIR: ${PROJECT_LIB_DIRECTORY}")
	target_link_directories("unittest_gui"  PUBLIC "${PROJECT_LIB_DIRECTORY}" )
	target_link_libraries("unittest_gui" PUBLIC module-vfs module-utils module-gui )

endif()
