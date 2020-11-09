project(service-gui)
message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )

set(SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ServiceGUI.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/WorkerGUI.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/messages/DrawMessage.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/messages/GUIDisplayReady.cpp"
)


add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    module-gui
    service-eink
    module-apps
    )

target_include_directories(${PROJECT_NAME}
    PRIVATE
        module-gui
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
        "${CMAKE_CURRENT_LIST_DIR}/messages"
)

if (${ENABLE_TESTS})
    add_subdirectory(source/tests)
endif ()
