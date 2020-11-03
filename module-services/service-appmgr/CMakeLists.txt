project(service-appmgr)
message("${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}")


set(SOURCES
    ApplicationManager.cpp
    Controller.cpp
    )

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    module-utils
    module-apps
    service-gui
    )

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${module-services_SOURCE_DIR}
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}"
)

