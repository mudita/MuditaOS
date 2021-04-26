# service-db tests
add_catch2_executable(
        NAME
            settings-Settings
        SRCS
            main.cpp
            test-settings-Interface.cpp
            # test-settings-Settings.cpp
            #../../agents/settings/Settings.cpp
        INCLUDE
            ${CMAKE_SOURCE_DIR}/module-sys
            ${CMAKE_SOURCE_DIR}/source
            ${CMAKE_SOURCE_DIR}/module-services/service-db
        DEPS
)
