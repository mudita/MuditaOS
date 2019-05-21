target_sources( ${PROJECT_NAME}

	PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}/Color.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/DrawCommand.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/Font.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/BoundingBox.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/Context.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/Renderer.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/PixMap.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/PixMapManager.cpp"
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/Color.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/DrawCommand.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/Font.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/BoundingBox.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/Context.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/Renderer.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/PixMap.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/PixMapManager.hpp"
)
