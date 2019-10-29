
include_directories( ${CMAKE_PROJECT_NAME}

	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}"
)

include_directories( ${PROJECT_NAME}

	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}"
)

target_sources( ${PROJECT_NAME}

	PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationCallLog.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/CalllogModel.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/CalllogItem.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/CallLogMainWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/CallLogDetailsWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/CallLogOptionsWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/CallLogCallDeleteWindow.cpp"
		
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationCallLog.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/CalllogModel.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/CallLogMainWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/CallLogDetailsWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/CallLogOptionsWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/CallLogCallDeleteWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/data/CallLogSwitchData.hpp"
)

