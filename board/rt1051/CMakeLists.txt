add_library(CrashCatcherARM STATIC)

target_sources(CrashCatcherARM
    PRIVATE
        crashdump/CrashCatcher_armv7m.S
)

set_source_files_properties(crashdump/CrashCatcher_armv7m.S PROPERTIES LANGUAGE C)

target_link_libraries(CrashCatcherARM
    PRIVATE
        CrashCatcher::CrashCatcher
)
