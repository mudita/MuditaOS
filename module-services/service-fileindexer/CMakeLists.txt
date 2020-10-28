message("${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )

target_include_directories(${PROJECT_NAME} PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}"
		${CMAKE_CURRENT_BINARY_DIR}/../../module-audio/Audio/decoder/taglib
		${CMAKE_CURRENT_SOURCE_DIR}/../../module-audio/Audio/decoder/taglib/taglib
		${CMAKE_CURRENT_SOURCE_DIR}/../../module-audio/Audio/decoder/taglib/taglib/toolkit
)

target_sources( ${PROJECT_NAME}
	PRIVATE
	"ServiceFileIndexer.cpp"
	"messages/FileChangeMessage.cpp"
	"startupIndexer.cpp"
	"notesIndexer.cpp"
)
