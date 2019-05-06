cmake_minimum_required(VERSION 3.14)
project(PurePhone)

set(CMAKE_CXX_STANDARD 14)


if(NOT DEFINED PROJECT_LIB_DIRECTORY )
    set(PROJECT_LIB_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/" CACHE STRING "Output path for static libraries")
    message("Setting PROJECT_LIB_DIRECTORY to ${PROJECT_LIB_DIRECTORY}")
endif()

if(NOT DEFINED PROJECT_BUILD_DIRECTORY )
    set(PROJECT_BUILD_DIRECTORY "${CMAKE_SOURCE_DIR}/build" CACHE STRING "Output directory for building" )
    message("Setting PROJECT_BUILD_DIRECTORY to ${PROJECT_BUILD_DIRECTORY}")
endif()


if (NOT TARGET module-core)
    message("add_subdirectory module-core")
    add_subdirectory(module-core)
    include_directories(${CMAKE_PROJECT_NAME} PUBLIC
            "${CMAKE_CURRENT_LIST_DIR}/module-core" )
endif ()

if (NOT TARGET module-gui)
    message("add_subdirectory module-gui")
    add_subdirectory(module-gui)
    include_directories(${CMAKE_PROJECT_NAME} PUBLIC
            "${CMAKE_CURRENT_LIST_DIR}/module-gui" )
endif ()

add_executable(PurePhone main.cpp)

target_link_libraries(${PROJECT_NAME} pthread libmodule-core ${LIBRARY_GUI})