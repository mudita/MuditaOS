cmake_minimum_required(VERSION 3.12)
set (ENABLE_TEST_LOGS ON)

project(unittest_service-desktop VERSION 1.0
        DESCRIPTION "Service Desktop")

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
