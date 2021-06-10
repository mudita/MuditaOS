# from: https://www.mattkeeter.com/blog/2018-01-06-versioning/ (modified)

execute_process(COMMAND git rev-parse --short HEAD
                OUTPUT_VARIABLE GIT_REV
                OUTPUT_STRIP_TRAILING_WHITESPACE
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
    set(GIT_DAILY_TAG "N/A")
else()
    execute_process(
        COMMAND bash -c "git diff --quiet --exit-code || echo +"
        OUTPUT_VARIABLE GIT_DIFF
        OUTPUT_STRIP_TRAILING_WHITESPACE
        WORKING_DIRECTORY ${SRC_DIR}
        )
    execute_process(
        COMMAND git describe --tags --match "release*" HEAD
        RESULT_VARIABLE ret
        OUTPUT_VARIABLE GIT_TAG
        OUTPUT_STRIP_TRAILING_WHITESPACE
        WORKING_DIRECTORY ${SRC_DIR}
        ERROR_QUIET
        )
        if(NOT ret EQUAL "0")
            set(GIT_TAG "none")
        endif()
    execute_process(
        COMMAND git tag --points-at HEAD 
        RESULT_VARIABLE ret
        OUTPUT_VARIABLE GIT_DAILY_TAG
        OUTPUT_STRIP_TRAILING_WHITESPACE
        WORKING_DIRECTORY ${SRC_DIR}
        ERROR_QUIET
        )
    execute_process(
        COMMAND git rev-parse --abbrev-ref HEAD
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    execute_process(
        COMMAND uname -r
        OUTPUT_VARIABLE BUILD_HOST
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    execute_process(
        COMMAND git config user.name
        OUTPUT_VARIABLE BUILD_USER
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    execute_process(
        COMMAND date +%F-%T
        OUTPUT_VARIABLE BUILD_DATE
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    execute_process(
        COMMAND grep tskKERNEL_VERSION_NUMBER ${SRC_DIR}/module-os/FreeRTOS/include/task.h
        COMMAND awk "{print $3}"
        COMMAND tr -d "\""
        OUTPUT_VARIABLE KERNEL_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
endif()

if (GIT_DAILY_TAG)
    string(REPLACE "\n" ";" GIT_DAILY_TAG_LIST ${GIT_DAILY_TAG})
    list(LENGTH GIT_DAILY_TAG_LIST DAILY_LEN)
    foreach(TAG_ITEM IN LISTS GIT_DAILY_TAG_LIST)
        if(${TAG_ITEM} MATCHES "release-([0-9]+).([0-9]+).([0-9]+)")
            break()
        elseif(${TAG_ITEM} MATCHES "daily-([0-9]+).([0-9]+).([0-9]+)")
            continue()
        endif()
    endforeach()
else()
    string(REGEX MATCH "release-([0-9]+).([0-9]+).([0-9]+)" VERSION_RAW ${GIT_TAG})
endif()

set(CMAKE_PROJECT_VERSION_MAJOR "${CMAKE_MATCH_1}")
set(CMAKE_PROJECT_VERSION_MINOR "${CMAKE_MATCH_2}")
set(CMAKE_PROJECT_VERSION_PATCH "${CMAKE_MATCH_3}")
set(CMAKE_PROJECT_VERSION "${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}.${CMAKE_PROJECT_VERSION_PATCH}")
