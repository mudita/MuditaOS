set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

if(NOT DEFINED PROJECT_LIB_DIRECTORY )
    set(PROJECT_LIB_DIRECTORY "${CMAKE_BINARY_DIR}/lib/" CACHE STRING "Output path for static libraries")
    message("Setting PROJECT_LIB_DIRECTORY to ${PROJECT_LIB_DIRECTORY}")
endif()
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_LIB_DIRECTORY})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# add linux target cmake if exists for module
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/targets/Target_Linux.cmake)
    if(${PROJECT_TARGET} STREQUAL "TARGET_Linux")
        include(${CMAKE_CURRENT_SOURCE_DIR}/targets/Target_Linux.cmake)
    endif()
endif()
