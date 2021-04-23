target_sources( ${PROJECT_NAME}
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/ApplicationCalculator.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/CalculatorMainWindow.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/widgets/MathOperationsBox.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/data/CalculatorUtility.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/data/CalculatorInputProcessor.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/data/CalculatorInputProcessorText.cpp"

    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/ApplicationCalculator.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/widgets/CalculatorStyle.hpp"
        )

if (${ENABLE_TESTS})
    add_subdirectory( tests )
endif()

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        service-evtmgr)
