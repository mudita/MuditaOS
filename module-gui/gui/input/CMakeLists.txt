target_sources( ${PROJECT_NAME}

	PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}/InputEvent.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/Profile.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/Translator.cpp"
		
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/InputEvent.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/Profile.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/Translator.hpp"
)
