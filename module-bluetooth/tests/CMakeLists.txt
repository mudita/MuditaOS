add_catch2_executable(
    NAME
        Bluetooth-tests
    SRCS
        tests-StatefulController.cpp
        tests-BluetoothDevicesModel.cpp
        tests-Devicei.cpp
        tests-BTKeysStorage.cpp
    LIBS
        module-sys
        module-bluetooth
        service-bluetooth
        test::fakeit
)


