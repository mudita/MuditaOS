if (NOT IS_SYMLINK "${CMAKE_BINARY_DIR}/sys/user")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/sys/user")
endif()

add_catch2_executable(
    NAME
        calculator
    SRCS
        tests-main.cpp
        CalculatorInput_tests.cpp
        CalculatorUtility_tests.cpp
    LIBS
        application-calculator
        iosyscalls
        module-gui
        module-sys
    DEPS
        assets
)

target_include_directories(catch2-calculator PRIVATE $<TARGET_PROPERTY:application-calculator,INCLUDE_DIRECTORIES>)
