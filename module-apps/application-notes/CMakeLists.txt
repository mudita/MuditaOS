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
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationNotes.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/model/NotesListModel.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/presenter/NotesMainWindowPresenter.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/NotesItem.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/NotesMainWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/NotesEditWindow.cpp"
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationNotes.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/model/NotesListModel.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/presenter/NotesMainWindowPresenter.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/NotesItem.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/NotesMainWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/NotesEditWindow.hpp"
)
target_include_directories(${PROJECT_NAME}
    PRIVATE
        service-db
)

