include_directories( ${PROJECT_NAME}

	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}"
		"${CMAKE_CURRENT_LIST_DIR}/core"
		"${CMAKE_CURRENT_LIST_DIR}/widgets"
		"${CMAKE_CURRENT_LIST_DIR}/input"
)

target_include_directories( ${PROJECT_NAME}

	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}"
		"${CMAKE_CURRENT_LIST_DIR}/core"
		"${CMAKE_CURRENT_LIST_DIR}/widgets"
		"${CMAKE_CURRENT_LIST_DIR}/input"
)

target_sources(${PROJECT_NAME}

	PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}/AudioCommon.cpp"
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/Common.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/SwitchData.hpp"
)

