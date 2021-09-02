# service-db tests
add_catch2_executable(
        NAME
            service-db-settings
        SRCS
            main.cpp
            test-service-db-settings-api.cpp
        LIBS
            db
            evtmgr
            module-audio
            module-cellular
            module-vfs
            service-audio
            service-cellular
        DEPS
            module-sys
)
