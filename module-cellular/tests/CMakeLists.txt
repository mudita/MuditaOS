cmake_minimum_required(VERSION 3.12)


project(unittest_module-cellular VERSION 1.0
        DESCRIPTION "Cellular module library unit tests")




add_executable( ${PROJECT_NAME}

        "${CMAKE_CURRENT_SOURCE_DIR}/main_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests-main.cpp"
        )

target_link_libraries( ${PROJECT_NAME} PRIVATE module-cellular )

