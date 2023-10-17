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
		windows/NoteCreateWindow.cpp
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
		windows/NoteCreateWindow.hpp
		windows/NoteMainWindow.hpp
		windows/NotePreviewWindow.hpp
		windows/NotesOptions.hpp
		windows/SearchEngineWindow.hpp
		windows/SearchResultsWindow.hpp
	PUBLIC
		include/application-notes/ApplicationNotes.hpp
)

target_link_libraries(application-notes
	PRIVATE
		app
		apps-common
		clipboard
		module-gui
		module-db
		service-db
		log
		utils-time
		i18n		
)
