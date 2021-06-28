add_catch2_executable(
    NAME
        calculator
    SRCS
        tests-main.cpp
        CalculatorInput_tests.cpp
        CalculatorUtility_tests.cpp
    LIBS
        application-calculator
        module-sys
    USE_FS
)

target_include_directories(catch2-calculator PRIVATE $<TARGET_PROPERTY:application-calculator,INCLUDE_DIRECTORIES>)
