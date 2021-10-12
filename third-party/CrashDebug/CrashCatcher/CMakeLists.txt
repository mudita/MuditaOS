add_library(CrashCatcher OBJECT)
add_library(CrashCatcher::CrashCatcher ALIAS CrashCatcher)

# These files were copied from 'third-party/CrashDebug/src/CrashCatcher' and modified.
#
# Reasons for modification:
# * CrashCatcher_armv7m.S: need to enter MCU user mode in order to create a file dump.
#
# * CrashCatcherPriv.h
#   CrashCatcher.c
#   CrashCatcher.h    : need to expose MCU registers' state for console dump.

target_sources(
        CrashCatcher
    PRIVATE
        CrashCatcherPriv.h
        CrashCatcher.c
        CrashCatcher_armv7m.S
    PUBLIC
        include/CrashCatcher/CrashCatcher.h
)

set_property(
	SOURCE CrashCatcher_armv7m.S
	APPEND PROPERTY COMPILE_OPTIONS "-x" "assembler-with-cpp"
)

target_include_directories(
        CrashCatcher
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
