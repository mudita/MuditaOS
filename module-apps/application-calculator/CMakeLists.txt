# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

add_library(application-calculator STATIC)

target_include_directories(application-calculator
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(application-calculator
    PRIVATE
        ApplicationCalculator.cpp
        data/CalculatorInputProcessor.cpp
        data/CalculatorInputProcessorText.cpp
        data/CalculatorUtility.cpp
        widgets/MathOperationsBox.cpp
        windows/CalculatorMainWindow.cpp
   PUBLIC
        data/CalculatorInputProcessor.hpp
        data/CalculatorInputProcessorText.hpp
        data/CalculatorUtility.hpp
        include/application-calculator/ApplicationCalculator.hpp
        widgets/CalculatorStyle.hpp
        widgets/MathOperationsBox.hpp
        windows/CalculatorMainWindow.hpp
        
       
)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif()

target_link_libraries(application-calculator
    PUBLIC
        apps-common
    PRIVATE
        app
        i18n
        Microsoft.GSL::GSL
        module-gui
        module-utils
        tinyexpr::tinyexpr
        utf8
)
