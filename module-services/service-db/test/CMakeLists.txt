# service-db tests
add_catch2_executable(
        NAME
            service-db
        SRCS
            main.cpp
            test-service-db-api.cpp
            test-service-db-settings-messages.cpp
            #test-service-db-file_indexer.cpp
            test-service-db-quotes.cpp
            test-factory-settings.cpp
        LIBS
            service-audio
            module-vfs
            module-audio
            service-cellular
            module-cellular
            iosyscalls
        DEPS
            disk_image
)

add_catch2_executable(
        NAME
            entry-path
        SRCS
            main.cpp
            test-entry-path.cpp
        LIBS
            service-db
            module-db
)

add_subdirectory(test-settings)
