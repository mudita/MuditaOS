
include_directories( ${CMAKE_PROJECT_NAME}

    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
)

target_sources( ${PROJECT_NAME}

    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/ApplicationMusicPlayer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/MusicPlayerAllSongsWindow.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/MusicPlayerEmptyWindow.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/widgets/SongItem.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/widgets/Action.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/models/SongsModel.cpp"
        
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/ApplicationMusicPlayer.hpp"
)
