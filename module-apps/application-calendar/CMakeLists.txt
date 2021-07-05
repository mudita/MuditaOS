target_sources( ${PROJECT_NAME}

	PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationCalendar.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/data/CalendarData.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/models/MonthModel.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/CalendarMainWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/DayLabel.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/MonthBox.cpp"
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationCalendar.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/models/MonthModel.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/data/OptionParser.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/data/CalendarData.hpp"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        service-db
    )
