# service-db tests
add_catch2_executable(
        NAME
            service-db
        SRCS
            test-service-db-api.cpp
            test-service-db-settings-messages.cpp
            test-service-db-quotes.cpp
            test-factory-settings.cpp
        LIBS
            module-db::test::helpers
            module-audio
            module-cellular
            module-sys
            module-utils
            module-vfs
            service-audio
            service-cellular
)

add_catch2_executable(
        NAME
            entry-path
        SRCS
            test-entry-path.cpp
            ${CMAKE_SOURCE_DIR}/module-services/service-db/EntryPath.cpp
        LIBS
            module-sys
        INCLUDE
            ${CMAKE_SOURCE_DIR}/module-utils/
            ${CMAKE_SOURCE_DIR}/module-services/service-db/
)

add_subdirectory(test-settings-Settings)
