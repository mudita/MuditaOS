project(service-fileindexer VERSION 1.0
	DESCRIPTION "File indexer service ")

add_library(${PROJECT_NAME} STATIC ${SOURCES})

# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})

target_sources( ${PROJECT_NAME}
	PRIVATE
	"ServiceFileIndexer.cpp"
	"StartupIndexer.cpp"
)


target_link_libraries(${PROJECT_NAME} 
	PUBLIC
		module-bsp 
		module-os 
		module-utils 
		module-vfs 
		module-sys 
		tag
	PRIVATE
		utf8
)
