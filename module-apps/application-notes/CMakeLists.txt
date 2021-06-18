# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

add_library(application-notes STATIC)

target_include_directories(application-notes
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(application-notes
	PRIVATE
		ApplicationNotes.cpp
		data/NotesFoundData.cpp
		data/NoteSwitchData.cpp
		model/NotesListModel.cpp
		model/NotesRepository.cpp
		model/NotesSearchListModel.cpp
		presenter/NoteEditWindowPresenter.cpp
		presenter/NotePreviewWindowPresenter.cpp
		presenter/NotesMainWindowPresenter.cpp
		presenter/NotesSearchResultPresenter.cpp
		presenter/SearchEngineWindowPresenter.cpp
		widgets/NotesItem.cpp
		windows/NoteEditWindow.cpp
		windows/NoteMainWindow.cpp
		windows/NotePreviewWindow.cpp
		windows/NotesOptions.cpp
		windows/SearchEngineWindow.cpp
		windows/SearchResultsWindow.cpp
	PRIVATE
		data/NotesFoundData.hpp
		data/NoteSwitchData.hpp
		model/NotesListModel.hpp
		model/NotesRepository.hpp
		model/NotesSearchListModel.hpp
		presenter/NoteEditWindowPresenter.hpp
		presenter/NotePreviewWindowPresenter.hpp
		presenter/NotesMainWindowPresenter.hpp
		presenter/NotesSearchResultPresenter.hpp
		presenter/SearchEngineWindowPresenter.hpp
		widgets/NotesItem.hpp
		windows/NoteEditWindow.hpp
		windows/NoteMainWindow.hpp
		windows/NotePreviewWindow.hpp
		windows/NotesOptions.hpp
		windows/SearchEngineWindow.hpp
		windows/SearchResultsWindow.hpp
	PUBLIC
		include/application-notes/ApplicationNotes.hpp
)


option(ENABLE_APP_NOTES "Enable application notes" ON)

target_compile_definitions(application-notes
    INTERFACE
        $<$<BOOL:${ENABLE_APP_NOTES}>:ENABLE_APP_NOTES>
)

target_link_libraries(application-notes
	PRIVATE
		apps-common
		clipboard
		module-gui
		module-db
		service-db
		log
		time
		i18n		
)
