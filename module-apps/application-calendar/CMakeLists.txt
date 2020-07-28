target_sources( ${PROJECT_NAME}

	PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationCalendar.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/CalendarMainWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/DayEventsWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/CalendarEventsOptionsWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/AllEventsWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/models/DayEventsModel.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/models/AllEventsModel.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/models/DayModel.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/models/MonthModel.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/DayEventsItem.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/AllEventsItem.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/CalendarListView.cpp"
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationCalendar.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/models/DayEventsModel.hpp"

)

