cmake_minimum_required(VERSION 3.12)


project(unittests_module-bsp VERSION 1.0
        DESCRIPTION "BSP module library")




add_executable( ${PROJECT_NAME}
        "${CMAKE_CURRENT_LIST_DIR}/unittest.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/tests-main.cpp"
        )


target_link_libraries( ${PROJECT_NAME} PRIVATE module-bsp )

