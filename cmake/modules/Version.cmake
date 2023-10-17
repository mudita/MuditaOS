# Version cmake sets whole build version
# it has to be:
# 1. a parameter to the build, cmake shoudln't guess version - we should provide it explicitly
# 2. conformant with: https://appnroll.atlassian.net/wiki/spaces/MFP/pages/1168015551/Release+version+and+branch+management

execute_process(COMMAND git rev-parse --short HEAD
                OUTPUT_VARIABLE GIT_REV
                OUTPUT_STRIP_TRAILING_WHITESPACE
                )
execute_process(
                COMMAND git rev-parse --abbrev-ref HEAD
                OUTPUT_VARIABLE GIT_BRANCH
                OUTPUT_STRIP_TRAILING_WHITESPACE
                )

if ( (NOT OS_VERSION_MAJOR MATCHES [0-9]+) OR (NOT OS_VERSION_MINOR MATCHES [0-9]+) OR (NOT OS_VERSION_PATCH MATCHES [0-9]+) )
    message( WARNING "Version not set properly! Setting version to 0.0.0! Requires MAJOR.MINOR.PATCH as parameters was: ${OS_VERSION_MAJOR}.${OS_VERSION_MINOR}.${OS_VERSION_PATCH}")
    set(OS_VERSION_MAJOR "0")
    set(OS_VERSION_MINOR "0")
    set(OS_VERSION_PATCH "0")
else()
    message(STATUS "Version major: ${OS_VERSION_MAJOR} minor: ${OS_VERSION_MINOR} path: ${OS_VERSION_PATCH} label: ${OS_VERSION_LABEL}")
endif()

if ( (NOT DEFINED OS_VERSION_LABEL) OR (OS_VERSION_LABEL STREQUAL "") )
    set(OS_VERSION_LABEL "")
    set(PROJECT_VERSION ${OS_VERSION_MAJOR}.${OS_VERSION_MINOR}.${OS_VERSION_PATCH})
else()
    set(PROJECT_VERSION ${OS_VERSION_MAJOR}.${OS_VERSION_MINOR}.${OS_VERSION_PATCH}-${OS_VERSION_LABEL})
endif()

