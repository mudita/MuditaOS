#cmake_minimum_required(VERSION 3.14)

#if(${CMAKE_VERSION} VERSION_LESS 3.12)
#    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
#endif()
#
#set (CMAKE_CXX_STANDARD 17)
#
#if(NOT DEFINED PROJECT_LIB_DIRECTORY )
#	set(PROJECT_LIB_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/" CACHE STRING "Output path for static libraries")
#	message("Setting PROJECT_LIB_DIRECTORY to ${PROJECT_LIB_DIRECTORY}")
#endif()
#
#set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_LIB_DIRECTORY})
#
#if(NOT DEFINED PROJECT_BUILD_DIRECTORY )
#	set(PROJECT_BUILD_DIRECTORY "${CMAKE_SOURCE_DIR}/build" CACHE STRING "Output directory for building" )
#	message("Setting PROJECT_BUILD_DIRECTORY to ${PROJECT_BUILD_DIRECTORY}")
#endif()
#
#set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BUILD_DIRECTORY})

set (CMAKE_CXX_STANDARD 17)

project( gui_renderer VERSION 1.0
	DESCRIPTION "GTK application for showing draw buffer."
	LANGUAGES CXX )

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

find_package(PkgConfig REQUIRED)               
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)
                   
add_executable( ${PROJECT_NAME} 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui_renderer.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RArea.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RArea.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RWindow.hpp )

target_link_libraries( ${PROJECT_NAME}  ${GTKMM_LIBRARIES}  )
target_include_directories(${PROJECT_NAME} PUBLIC ${GTKMM_LIBRARY_DIRS} )
target_include_directories( ${PROJECT_NAME}  PUBLIC ${GTKMM_INCLUDE_DIRS}  )
target_link_libraries( ${PROJECT_NAME} ${LIBRT} rt pthread )
    
##find_library(LIBRT NAMES rt librt NO_DEFAULT_PATH)
#message( "LIBRT: ${LIBRT}" ) 
#if(LIBRT)
#	target_link_libraries( ${PROJECT_NAME} ${LIBRT} rt pthread )
#endif()
                   


#add_subdirectory( src )
