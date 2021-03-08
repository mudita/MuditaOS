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

add_subdirectory(test-settings)
