# from: https://www.mattkeeter.com/blog/2018-01-06-versioning/

set(VERSION_HEADER "${CMAKE_BINARY_DIR}/source/version.hpp")

execute_process(COMMAND git log --pretty=format:'%h' -n 1
                OUTPUT_VARIABLE GIT_REV
                ERROR_QUIET)

if ( NOT SRC_DIR )
    set(SRC_DIR ${CMAKE_SOURCE_DIR})
endif()
# Check whether we got any revision (which isn't
# always the case, e.g. when someone downloaded a zip
# file from Github instead of a checkout)
if ("${GIT_REV}" STREQUAL "")
    set(GIT_REV "N/A")
    set(GIT_DIFF "")
    set(GIT_TAG "N/A")
    set(GIT_BRANCH "N/A")
else()
    execute_process(
        COMMAND bash -c "git diff --quiet --exit-code || echo +"
        OUTPUT_VARIABLE GIT_DIFF
        WORKING_DIRECTORY ${SRC_DIR}
        )
    execute_process(
        COMMAND git describe --tags
        OUTPUT_VARIABLE GIT_TAG ERROR_QUIET RESULT_VARIABLE ret
        WORKING_DIRECTORY ${SRC_DIR}
        )
        if(NOT ret EQUAL "0")
            set(GIT_TAG "none")
        endif()
    execute_process(
        COMMAND git rev-parse --abbrev-ref HEAD
        OUTPUT_VARIABLE GIT_BRANCH)

    string(STRIP "${GIT_REV}" GIT_REV)
    string(SUBSTRING "${GIT_REV}" 1 7 GIT_REV)
    string(STRIP "${GIT_DIFF}" GIT_DIFF)
    string(STRIP "${GIT_TAG}" GIT_TAG)
    string(STRIP "${GIT_BRANCH}" GIT_BRANCH)
endif()
message("GIT_REV: ${GIT_REV}")
message("GIT_TAG: ${GIT_TAG}")

string(REGEX MATCH "release-([0-9]*).([0-9]*).([0-9]*)" VERSION_RAW ${GIT_TAG})

set(CMAKE_PROJECT_VERSION_MAJOR "${CMAKE_MATCH_1}")
set(CMAKE_PROJECT_VERSION_MINOR "${CMAKE_MATCH_2}")
set(CMAKE_PROJECT_VERSION_PATCH "${CMAKE_MATCH_3}")

message("Vession: ${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}.${CMAKE_PROJECT_VERSION_PATCH}")

configure_file(
    ${SRC_DIR}/source/version.hpp.template
    ${CMAKE_BINARY_DIR}/source/version.hpp
    )
