add_catch2_executable(
    NAME
        apps-common
    SRCS
        test-CallbackStorage.cpp
        test-PhoneModesPolicies.cpp
        tests-BluetoothSettingsModel.cpp
        test-Model.cpp
    LIBS
        module-apps
)

add_subdirectory(windows)
