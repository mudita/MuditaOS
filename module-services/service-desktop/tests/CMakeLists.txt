file (COPY "${CMAKE_CURRENT_LIST_DIR}/muditaos-unittest.tar" DESTINATION "${CMAKE_BINARY_DIR}/sys/updates")
file (COPY "${CMAKE_CURRENT_LIST_DIR}/factory-test" DESTINATION "${CMAKE_BINARY_DIR}/sys")

add_catch2_executable(
    NAME
        service-desktop
    SRCS
        unittest.cpp
        tests-main.cpp
    LIBS
        service-desktop
        module-utils
        module-apps
        iosyscalls
    DEPS
        PurePhone-disk-img
)

add_catch2_executable(
    NAME
        service-desktop-endpoint-contacts
    SRCS
        tests-main.cpp
        test-contacts.cpp
    LIBS
        service-desktop
        module-utils
        module-apps
        module-vfs
        iosyscalls
)

add_catch2_executable(
    NAME
        service-desktop-endpoint-filesystem
    SRCS
        tests-main.cpp
        test-fs-ep-operations.cpp
    LIBS
        service-desktop
        module-utils
        module-apps
        module-vfs
        iosyscalls
)