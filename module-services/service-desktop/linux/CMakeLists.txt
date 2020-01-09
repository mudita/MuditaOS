target_include_directories(${PROJECT_NAME} 
	PUBLIC 
		"${CMAKE_CURRENT_LIST_DIR}"
)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
target_sources(${PROJECT_NAME}
	PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}/Worker.cpp"
)