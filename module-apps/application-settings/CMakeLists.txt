
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
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationSettings.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/SettingsMainWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/LanguageWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/BtWindow.cpp"

		"${CMAKE_CURRENT_LIST_DIR}/windows/UITestWindow.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/Info.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/SimSelectWindow.cpp"
"${CMAKE_CURRENT_LIST_DIR}/windows/DateTimeWindow.cpp"

	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationSettings.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/SettingsMainWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/LanguageWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/DateTimeWindow.hpp"
		#"${CMAKE_CURRENT_LIST_DIR}/widgets/DateTimeItem.hpp"
)

