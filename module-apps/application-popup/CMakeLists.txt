include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}")

target_sources( ${PROJECT_NAME}

	PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationPopup.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/EventReminderWindow.cpp"
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationPopup.hpp"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        service-db
    )
