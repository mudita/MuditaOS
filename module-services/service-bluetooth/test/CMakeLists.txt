add_catch2_executable(
    NAME
        bt-storage
    SRCS
        tests-main.cpp
        test-SettingsSerializer.cpp
        test-SettingsVisitors.cpp
        test-SettingsHolder.cpp

        # this lib crap, without dependency to the whole world
        ${PROJECT_SOURCE_DIR}/service-bluetooth/SettingsSerializer.cpp
        ${PROJECT_SOURCE_DIR}/service-bluetooth/SettingsHolder.cpp

        # external crap
        ${CMAKE_SOURCE_DIR}/module-utils/json/json11.cpp
        ${CMAKE_SOURCE_DIR}/module-bluetooth/lib/btstack/src/btstack_util.c
        ${CMAKE_SOURCE_DIR}/module-services/service-db/agents/settings/Settings.cpp
        ${CMAKE_SOURCE_DIR}/module-services/service-db/EntryPath.cpp
    INCLUDE
        ${PROJECT_SOURCE_DIR}/service-bluetooth/
        ${CMAKE_SOURCE_DIR}/module-utils
        ${CMAKE_SOURCE_DIR}/module-bluetooth/Bluetooth/
        ${CMAKE_SOURCE_DIR}/module-bluetooth/lib/btstack/src/
        ${CMAKE_SOURCE_DIR}/module-services/service-db/ # for: service-db/
        ${CMAKE_SOURCE_DIR}/module-sys/ # for: Service/ServiceProxy.hpp
        ${CMAKE_SOURCE_DIR}/third-party/magic_enum/include/ #magic_enum.hpp
        ${CMAKE_SOURCE_DIR}/source/ # MessageType.hpp
    LIBS
)
