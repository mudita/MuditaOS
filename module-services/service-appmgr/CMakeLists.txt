include_directories(${CMAKE_PROJECT_NAME}
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}"
)

include_directories(${PROJECT_NAME}
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}"
		"${CMAKE_CURRENT_LIST_DIR}/.."
)

message("${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}")

target_sources(${PROJECT_NAME}
	PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationManifest.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/Controller.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/model/ApplicationManager.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/model/ApplicationHandle.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/model/ApplicationsRegistry.cpp"
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/Actions.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationManifest.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/Controller.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/model/ApplicationManager.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/model/ApplicationHandle.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/model/ApplicationsRegistry.hpp"
)

