cmake_minimum_required(VERSION 3.12)

project(unittest_module-bsp VERSION 1.0
        DESCRIPTION "BSP module library")

add_executable( ${PROJECT_NAME} EXCLUDE_FORM_ALL
        "bsp_audio_tests.cpp"
        "tests-main.cpp"
        "tests-bluetooth.cpp"
        )


target_link_libraries( ${PROJECT_NAME} PRIVATE module-bsp portaudio )

add_test(NAME ${PROJECT_NAME}
         COMMAND ${PROJECT_NAME}
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
         )

add_dependencies(check ${PROJECT_NAME})
