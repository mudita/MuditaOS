include_directories( ${PROJECT_NAME}
    PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}"
)

message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )

target_sources( ${PROJECT_NAME}

    PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/ServiceBT.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/WorkerBT.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/BtLogger.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/BtFile.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/BtInject.cpp"
)

