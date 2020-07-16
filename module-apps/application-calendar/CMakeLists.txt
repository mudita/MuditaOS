target_sources( ${PROJECT_NAME}

	PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationCalendar.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/CalendarMainWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/DayWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/models/CalendarModel.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/CalendarItem.cpp"
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationCalendar.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/models/CalendarModel.hpp"
)

