# service-db tests
add_catch2_executable(
        NAME
                service-db
        SRCS
                main.cpp
                test-service-db-api.cpp
                test-service-db-settings-messages.cpp
        LIBS
                module-services
)
