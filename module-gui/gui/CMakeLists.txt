include_directories( ${PROJECT_NAME}

    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
        "${CMAKE_CURRENT_LIST_DIR}/core"
        "${CMAKE_CURRENT_LIST_DIR}/widgets"
        "${CMAKE_CURRENT_LIST_DIR}/widgets/text"
        "${CMAKE_CURRENT_LIST_DIR}/input"
        "${CMAKE_CURRENT_LIST_DIR}/dom"
)

target_include_directories( ${PROJECT_NAME}

    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
        "${CMAKE_CURRENT_LIST_DIR}/core"
        "${CMAKE_CURRENT_LIST_DIR}/widgets"
        "${CMAKE_CURRENT_LIST_DIR}/widgets/text"
        "${CMAKE_CURRENT_LIST_DIR}/input"
        "${CMAKE_CURRENT_LIST_DIR}/dom"
)

target_sources(${PROJECT_NAME}

    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/Common.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/SwitchData.hpp"
)

