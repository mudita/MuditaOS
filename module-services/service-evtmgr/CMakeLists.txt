include_directories( ${CMAKE_PROJECT_NAME}

	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}"
)

include_directories( ${PROJECT_NAME}

	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}"
)

message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )

target_sources( ${PROJECT_NAME}

	PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}/EventManager.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/alarm/EventManagerAlarm.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/WorkerEvent.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/harness/Parser.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/harness/events/KeyPress.cpp"
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/EventManager.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/WorkerEvent.hpp"
)

