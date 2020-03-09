target_include_directories(${PROJECT_NAME}
        PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/SEGGER/"
        "${CMAKE_CURRENT_LIST_DIR}/Config/"
        "${CMAKE_CURRENT_LIST_DIR}/FreeRTOSV10/"
        "${CMAKE_CURRENT_LIST_DIR}/FreeRTOSV10/Config/"
        "${CMAKE_CURRENT_LIST_DIR}/FreeRTOSV10/Patch"


        )
target_compile_options(${PROJECT_NAME} PUBLIC "-w")
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
target_sources(${PROJECT_NAME}
        PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/FreeRTOSV10/Config/SEGGER_SYSVIEW_Config_FreeRTOS.c"
        "${CMAKE_CURRENT_LIST_DIR}/FreeRTOSV10/SEGGER_SYSVIEW_FreeRTOS.c"
        "${CMAKE_CURRENT_LIST_DIR}/SEGGER/SEGGER_SYSVIEW.c"
        )
