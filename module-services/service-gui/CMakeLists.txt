project(service-gui)
message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )

set(SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ServiceGUI.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/WorkerGUI.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ContextPool.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/RenderCache.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/DrawCommandsQueue.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SynchronizationMechanism.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/messages/DrawMessage.cpp"
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    service-eink
    module-gui
    module-apps
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
        "${CMAKE_CURRENT_LIST_DIR}/messages"
)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif()
