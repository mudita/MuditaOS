cmake_minimum_required(VERSION 3.12)

project(service-fileindexer VERSION 1.0
	DESCRIPTION "File indexer service ")

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${BOARD_SOURCES})

# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PUBLIC ${BOARD_DIR_INCLUDES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})
target_compile_features(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_FEATURES})
target_link_options(${PROJECT_NAME} PUBLIC ${TARGET_LINK_OPTIONS})


target_sources( ${PROJECT_NAME}
	PRIVATE
	"ServiceFileIndexer.cpp"
	"StartupIndexer.cpp"
	"notesIndexer.cpp"
)


target_link_libraries(${PROJECT_NAME} module-bsp module-os module-utils module-vfs module-sys tag )
