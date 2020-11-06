include_directories( ${CMAKE_PROJECT_NAME}

	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}"
)

include_directories( ${PROJECT_NAME}

	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}"
)

target_include_directories(${PROJECT_NAME}
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/../service-audio/api"
)

message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )

target_sources( ${PROJECT_NAME}

	PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}/EventManager.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/messages/Message.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/alarm/EventManagerAlarm.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/WorkerEvent.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/api/EventManagerServiceAPI.cpp"
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/EventManager.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/WorkerEvent.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/api/EventManagerServiceAPI.hpp"
)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        service-audio
)

