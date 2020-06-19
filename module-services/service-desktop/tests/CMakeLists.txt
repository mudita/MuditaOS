cmake_minimum_required(VERSION 3.12)

project(unittest_service-desktop VERSION 1.0
        DESCRIPTION "Service Desktop")

add_executable( ${PROJECT_NAME} EXCLUDE_FROM_ALL
        "${CMAKE_CURRENT_LIST_DIR}/unittest.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/tests-main.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/../update/UpdatePureOS.cpp"
        )

target_link_libraries (${PROJECT_NAME} PRIVATE module-vfs module-utils module-services)

add_test(NAME ${PROJECT_NAME}
         COMMAND ${PROJECT_NAME}
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
         )
add_dependencies(check ${PROJECT_NAME})

set(ASSETS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test_dir")
set(ASSETS_TARGET_DIR "${CMAKE_BINARY_DIR}/module-services/service-desktop/test_dir")
