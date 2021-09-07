# service-db tests
add_catch2_executable(
        NAME
            service-db
        SRCS
            main.cpp
            test-service-db-api.cpp
            test-service-db-settings-messages.cpp
            test-service-db-quotes.cpp
            test-factory-settings.cpp
        LIBS
            iosyscalls
            module-audio
            module-cellular
            module-sys
            module-utils
            module-vfs
            service-audio
            service-cellular
        USE_FS
)

add_catch2_executable(
        NAME
            entry-path
        SRCS
            main.cpp
            test-entry-path.cpp
            ${CMAKE_SOURCE_DIR}/module-services/service-db/EntryPath.cpp
        LIBS
            module-sys
        INCLUDE
            ${CMAKE_SOURCE_DIR}/module-utils/
            ${CMAKE_SOURCE_DIR}/module-services/service-db/
)

add_subdirectory(test-settings-Settings)
