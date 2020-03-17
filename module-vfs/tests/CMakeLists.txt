cmake_minimum_required(VERSION 3.12)


project(unittest_module-vfs VERSION 1.0
        DESCRIPTION "VFS module library")




add_executable( ${PROJECT_NAME} EXCLUDE_FROM_ALL
        "${CMAKE_CURRENT_LIST_DIR}/unittest.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/tests-main.cpp"
        )


target_link_libraries( ${PROJECT_NAME} PRIVATE module-vfs )

add_test(NAME ${PROJECT_NAME}
         COMMAND ${PROJECT_NAME}
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
         )
add_dependencies(check ${PROJECT_NAME})
