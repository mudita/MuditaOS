file (COPY "${CMAKE_CURRENT_LIST_DIR}/pureos-unittest.tar" DESTINATION "${CMAKE_BINARY_DIR}/sys/updates")
file (COPY "${CMAKE_CURRENT_LIST_DIR}/factory-test" DESTINATION "${CMAKE_BINARY_DIR}/sys")

add_catch2_executable(
	NAME
		service-desktop
	SRCS
		"${CMAKE_CURRENT_LIST_DIR}/unittest.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/tests-main.cpp"
		"../endpoints/update/UpdatePureOS.cpp"
		"../endpoints/factoryReset/FactoryReset.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/../parser/MessageHandler.cpp"


	LIBS
		module-services
		module-sys
)
