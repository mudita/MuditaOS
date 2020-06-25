cmake_minimum_required(VERSION 3.12)

project(unittest_service-desktop VERSION 1.0
        DESCRIPTION "Service Desktop")

set (ENABLE_TEST_LOGS, OFF)

file (COPY "${CMAKE_CURRENT_LIST_DIR}/pureos-unittest.tar" DESTINATION "${CMAKE_BINARY_DIR}/sys/updates")

add_catch2_executable(
	NAME
		service-desktop
	SRCS
	    "${CMAKE_CURRENT_LIST_DIR}/unittest.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/tests-main.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/../update/UpdatePureOS.cpp"
	LIBS
		module-services
)

#target_link_libraries (${PROJECT_NAME} PRIVATE module-vfs module-utils module-services)

#add_test(NAME ${PROJECT_NAME}
#         COMMAND ${PROJECT_NAME}
#         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#         )
#add_dependencies(check ${PROJECT_NAME})

#set(ASSETS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test_dir")
#set(ASSETS_TARGET_DIR "${CMAKE_BINARY_DIR}/module-services/service-desktop/test_dir")
