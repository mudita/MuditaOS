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
		"${CMAKE_CURRENT_LIST_DIR}/model/NotesRepository.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/presenter/NotesMainWindowPresenter.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/presenter/NotePreviewWindowPresenter.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/presenter/NoteEditWindowPresenter.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/NotesItem.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/NoteMainWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/NotePreviewWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/NoteEditWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/NotesOptions.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/data/NoteSwitchData.cpp"
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationNotes.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/model/NotesListModel.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/model/NotesRepository.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/presenter/NotesMainWindowPresenter.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/presenter/NotePreviewWindowPresenter.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/presenter/NoteEditWindowPresenter.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/NotesItem.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/NoteMainWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/NotePreviewWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/NoteEditWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/NotesOptions.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/data/NoteSwitchData.hpp"
)
target_include_directories(${PROJECT_NAME}
    PRIVATE
        service-db
)

